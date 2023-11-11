#include "lcu_process_handler_impl.h"

#include "exception.h"
#include "lcu_process_handler_helpers.h"

#include "Poco/UnicodeConverter.h"

#include <libloaderapi.h>
#include <psapi.h>

#include <functional>
#include <iostream>
#include <regex>

namespace rito {

using namespace Poco;

namespace {
constexpr auto max_process_count{65536};
}

Lcu_process_handler_impl::Lcu_process_handler_impl(std::filesystem::path dir_path)
  : m_dll_lib{nullptr}, m_nt_query_information_process_ptr{nullptr}
{
}

Lcu_process_handler_impl::~Lcu_process_handler_impl()
{
    unload_nt_dll();
}

Lcu_parameters Lcu_process_handler_impl::get_lcu_process_parameters()
{
    std::string lcu_command{get_lcu_process_command()};

    std::string remoting_auth_token{extract_remoting_auth_token(lcu_command)};
    std::string app_port{extract_app_port(lcu_command)};

    return {remoting_auth_token, static_cast<uint16_t>(std::stoi(app_port))};
}

void Lcu_process_handler_impl::load_nt_dll()
{
    if (m_dll_lib)
    {
        return;
    }

    m_dll_lib = LoadLibrary(TEXT("Ntdll.dll"));

    if (!m_dll_lib)
    {
        throw Lcu_parameters_exception{"Unable to dynamically load Ntdll.dll"};
    }
}

void Lcu_process_handler_impl::retrieve_nt_query_information_process_function_pointer()
{
    if (m_nt_query_information_process_ptr)
    {
        return;
    }
    m_nt_query_information_process_ptr = reinterpret_cast<nt_query_information_process_ptr>(
      GetProcAddress(m_dll_lib, "NtQueryInformationProcess"));

    if (!m_nt_query_information_process_ptr)
    {
        throw Lcu_parameters_exception{
          "Unable to retrieve NtQueryInformationProcess function pointer from Ntdll.dll"};
    }
}

void Lcu_process_handler_impl::unload_nt_dll()
{
    if (!m_dll_lib)
    {
        return;
    }

    FreeLibrary(m_dll_lib);
    m_dll_lib = nullptr;
    m_nt_query_information_process_ptr = nullptr;
}

std::string Lcu_process_handler_impl::get_lcu_process_command()
{
    std::vector<DWORD> processes(max_process_count); // Vector handles memory cleanup
    DWORD bytes_returned{};

    if (!EnumProcesses(processes.data(), sizeof(DWORD) * max_process_count, &bytes_returned))
    {
        throw Lcu_parameters_exception{"Unable to to enumerate processes with EnumProcesses"};
    }

    DWORD process_count{bytes_returned / sizeof(DWORD)};
    for (auto i{0}; i < process_count; i++)
    {
        if (processes[i])
        {
            HANDLE process_handle{
              OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i])};

            auto process_name{extract_process_name(process_handle)};
            if (process_name == "LeagueClientUx.exe")
            {
                std::string process_command;
                try
                {
                    process_command = extract_process_command(process_handle);
                }
                catch (...)
                {
                    CloseHandle(process_handle);
                    throw;
                }

                CloseHandle(process_handle);
                return process_command;
            }

            CloseHandle(process_handle);
        }
    }

    throw Lcu_parameters_exception{"Unable to find League Client process"};
}

std::string Lcu_process_handler_impl::extract_process_name(HANDLE process_handle)
{
    TCHAR process_name[MAX_PATH]{};
    if (process_handle)
    {
        HMODULE process_module_handle;
        DWORD module_bytes_returned{};

        if (EnumProcessModules(process_handle,
                               &process_module_handle,
                               sizeof(process_module_handle),
                               &module_bytes_returned))
        {
            GetModuleBaseName(process_handle,
                              process_module_handle,
                              process_name,
                              sizeof(process_name) / sizeof(TCHAR));
        }
    }

    std::string process_name_converted;
    UnicodeConverter::convert(process_name, process_name_converted);

    return process_name_converted;
}

std::string Lcu_process_handler_impl::extract_process_command(HANDLE process_handle)
{
    load_nt_dll();
    retrieve_nt_query_information_process_function_pointer();

    PVOID process_info{operator new(sizeof(PROCESS_BASIC_INFORMATION))};

    LONG status{m_nt_query_information_process_ptr(process_handle,
                                                   PROCESSINFOCLASS::ProcessBasicInformation,
                                                   process_info,
                                                   sizeof(PROCESS_BASIC_INFORMATION),
                                                   nullptr)};
    PPEB peb{reinterpret_cast<PPEB>((reinterpret_cast<PVOID*>(process_info))[1])};
    PPEB peb_buffer{new PEB};
    BOOL result{ReadProcessMemory(process_handle, peb, peb_buffer, sizeof(PEB), nullptr)};

    PRTL_USER_PROCESS_PARAMETERS process_parameters{peb_buffer->ProcessParameters};
    PRTL_USER_PROCESS_PARAMETERS process_parameters_buffer{new RTL_USER_PROCESS_PARAMETERS};
    result = ReadProcessMemory(process_handle,
                               process_parameters,
                               process_parameters_buffer,
                               sizeof(RTL_USER_PROCESS_PARAMETERS),
                               nullptr);
    PWSTR command{process_parameters_buffer->CommandLine.Buffer};
    USHORT command_length{process_parameters_buffer->CommandLine.Length};
    PWSTR command_buffer{new WCHAR[command_length]};
    result = ReadProcessMemory(process_handle,
                               command,
                               command_buffer, // command line goes here
                               command_length,
                               nullptr);

    std::string to;
    UnicodeConverter::convert(command, to);

    delete[] command_buffer;
    delete process_parameters_buffer;
    delete peb_buffer;
    delete process_info;

    return to;
}

} // namespace rito
