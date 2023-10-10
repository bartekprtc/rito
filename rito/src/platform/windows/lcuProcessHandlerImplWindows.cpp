#include "lcuProcessHandlerImpl.h"

#include "exception.h"

#include "Poco/UnicodeConverter.h"

#include <libloaderapi.h>
#include <psapi.h>

#include <iostream>
#include <format>
#include <functional>
#include <regex>

namespace rito {

    using namespace Poco;

namespace {
constexpr auto MAX_PROCESS_COUNT{65536};
}

std::string extractRemotingAuthToken(const std::string& lcuCommand);
std::string extractAppPort(const std::string& lcuCommand);
bool isInteger(std::string_view str);

LcuProcessHandlerImpl::LcuProcessHandlerImpl()
  : m_dllLib{nullptr}, m_ntQueryInformationProcessPtr{nullptr}
{
}

LcuProcessHandlerImpl::~LcuProcessHandlerImpl()
{
    unloadNtDll();
}

LcuParameters LcuProcessHandlerImpl::getLcuProcessParameters()
{
    std::wstring wideLcuCommand{getLcuProcessCommand()};
    std::string lcuCommand;
    UnicodeConverter::convert(wideLcuCommand, lcuCommand);

    std::string remotingAuthToken{extractRemotingAuthToken(lcuCommand)};
    std::string appPort{extractAppPort(lcuCommand)};

    return {remotingAuthToken, static_cast<uint16_t>(std::stoi(appPort))};
}

void LcuProcessHandlerImpl::loadNtDll()
{
    if (m_dllLib)
    {
        return;
    }

    m_dllLib = LoadLibrary(TEXT("Ntdll.dll"));

    if (!m_dllLib)
    {
        throw LcuParametersException{"Unable to dynamically load Ntdll.dll"};
    }
}

void LcuProcessHandlerImpl::retrieveNtQueryInformationProcessFunctionPointer()
{
    if (m_ntQueryInformationProcessPtr)
    {
        return;
    }
    m_ntQueryInformationProcessPtr = reinterpret_cast<NtQueryInformationProcessPtr>(GetProcAddress(m_dllLib, "NtQueryInformationProcess"));

    if (!m_ntQueryInformationProcessPtr)
    {
        throw LcuParametersException{
          "Unable to retrieve NtQueryInformationProcess function pointer from Ntdll.dll"};
    }
}

void LcuProcessHandlerImpl::unloadNtDll()
{
    if (!m_dllLib)
    {
        return;
    }

    FreeLibrary(m_dllLib);
    m_dllLib = nullptr;
    m_ntQueryInformationProcessPtr = nullptr;
}

std::wstring LcuProcessHandlerImpl::getLcuProcessCommand()
{
    std::vector<DWORD> processes(MAX_PROCESS_COUNT); // Vector handles memory cleanup
    DWORD bytesReturned{};

    if (!EnumProcesses(processes.data(), sizeof(DWORD) * MAX_PROCESS_COUNT, &bytesReturned))
    {
        throw LcuParametersException{
          "Unable to to enumerate processes with EnumProcesses"};
    }

    DWORD processCount{bytesReturned / sizeof(DWORD)};
    for (auto i{0}; i < processCount; i++)
    {
        if (processes[i])
        {
            HANDLE processHandle{
              OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i])};

            auto processName{extractProcessName(processHandle)};
            if (processName == L"LeagueClientUx.exe")
            {
                std::wstring processCommand;
                try
                {
                    processCommand = extractProcessCommand(processHandle);
                }
                catch (...)
                {
                    CloseHandle(processHandle);
                    throw;
                }

                CloseHandle(processHandle);
                return processCommand;
            }

            CloseHandle(processHandle);
        }
    }

     throw LcuParametersException{"Unable to find League Client process"};
    }

std::wstring LcuProcessHandlerImpl::extractProcessName(HANDLE processHandle)
{
    TCHAR processName[MAX_PATH]{};
    if (processHandle)
    {
        HMODULE processModuleHandle;
        DWORD bytesReturnedModule{};

        if (EnumProcessModules(processHandle,
                               &processModuleHandle,
                               sizeof(processModuleHandle),
                               &bytesReturnedModule))
        {
            GetModuleBaseName(
              processHandle, processModuleHandle, processName, sizeof(processName) / sizeof(TCHAR));
        }
    }

    return std::wstring{processName};
}

std::wstring LcuProcessHandlerImpl::extractProcessCommand(HANDLE processHandle)
{
    loadNtDll();
    retrieveNtQueryInformationProcessFunctionPointer();

    // HANDLE heap{GetProcessHeap()};
    // PVOID processInfo{HeapAlloc(heap, HEAP_ZERO_MEMORY, sizeof(PROCESS_BASIC_INFORMATION))};
    PVOID processInfo{operator new(sizeof(PROCESS_BASIC_INFORMATION))};

    LONG status{m_ntQueryInformationProcessPtr(processHandle,
                                               PROCESSINFOCLASS::ProcessBasicInformation,
                                               processInfo,
                                               sizeof(PROCESS_BASIC_INFORMATION),
                                               nullptr)};
    PPEB peb{reinterpret_cast<PPEB>((reinterpret_cast<PVOID*>(processInfo))[1])};
    PPEB pebBuffer{new PEB};
    BOOL result{ReadProcessMemory(processHandle, peb, pebBuffer, sizeof(PEB), nullptr)};

    PRTL_USER_PROCESS_PARAMETERS processParameters{pebBuffer->ProcessParameters};
    // PRTL_USER_PROCESS_PARAMETERS pRtlProcParamCopy =
    //    (PRTL_USER_PROCESS_PARAMETERS)malloc(sizeof(RTL_USER_PROCESS_PARAMETERS));
    PRTL_USER_PROCESS_PARAMETERS processParametersBuffer{new RTL_USER_PROCESS_PARAMETERS};
    result = ReadProcessMemory(processHandle,
                               processParameters,
                               processParametersBuffer,
                               sizeof(RTL_USER_PROCESS_PARAMETERS),
                               nullptr);
    PWSTR command{processParametersBuffer->CommandLine.Buffer};
    USHORT commandLength{processParametersBuffer->CommandLine.Length};
    PWSTR commandBuffer{new WCHAR[commandLength]};
    result = ReadProcessMemory(processHandle,
                               command,
                               commandBuffer, // command line goes here
                               commandLength,
                                nullptr);

    std::wstring ws(commandBuffer);

    delete[] commandBuffer;
    delete processParametersBuffer;
    delete pebBuffer;
    delete processInfo;

    return ws;
}


std::string extractRemotingAuthToken(const std::string& lcuCommand)
{
    std::regex tokenRegex{"--remoting-auth-token=([a-zA-Z0-9-_]+)"};
    std::smatch tokenMatch;
    if (std::regex_search(lcuCommand, tokenMatch, tokenRegex))
    {
        return tokenMatch[1];
    }

    throw LcuParametersException(
        "Unable to extract remoting auth token from League Client launch command");
}

std::string extractAppPort(const std::string& lcuCommand)
{
    std::regex portRegex{"--app-port=(\\d+)"};
    std::smatch portMatch;
    if (std::regex_search(lcuCommand, portMatch, portRegex))
    {
        return portMatch[1];
    }

    throw LcuParametersException(
      "Unable to extract app port from League Client launch command");
}

bool isInteger(std::string_view str)
{
    return !str.empty() && std::all_of(str.cbegin(), str.cend(), [](char ch) {
        return std::isdigit(ch);
    });
}

}
