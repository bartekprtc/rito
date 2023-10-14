#pragma once

#include "lcu_process_handler.h"

#include <windows.h>
#include <winternl.h>

#include <string>

typedef NTSTATUS(NTAPI* nt_query_information_process_ptr)(
        IN HANDLE process_handle,
        IN PROCESSINFOCLASS process_information_class,
        OUT PVOID process_information,
        IN ULONG process_information_length,
        OUT PULONG return_length OPTIONAL);

namespace rito {


class Lcu_process_handler_impl
{
public:
    Lcu_process_handler_impl();
    ~Lcu_process_handler_impl();

    Lcu_parameters get_lcu_process_parameters();

private:
    void load_nt_dll();
    void retrieve_nt_query_information_process_function_pointer();
    void unload_nt_dll();

    std::string get_lcu_process_command();
    std::string extract_process_name(HANDLE process_handle);
    std::string extract_process_command(HANDLE process_handle);

private:
    HINSTANCE m_dll_lib;
    nt_query_information_process_ptr m_nt_query_information_process_ptr;
};

} // namespace rito
