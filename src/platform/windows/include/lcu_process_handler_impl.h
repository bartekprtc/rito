#pragma once

#include "lcu_process_handler.h"

#include <windows.h>
#include <winternl.h>

#include <string>

typedef NTSTATUS(NTAPI* NtQueryInformationProcessPtr)(
        IN HANDLE ProcessHandle,
        IN PROCESSINFOCLASS ProcessInformationClass,
        OUT PVOID ProcessInformation,
        IN ULONG ProcessInformationLength,
        OUT PULONG ReturnLength OPTIONAL);

namespace rito {


class LcuProcessHandlerImpl
{
public:
    LcuProcessHandlerImpl();
    ~LcuProcessHandlerImpl();

    LcuParameters getLcuProcessParameters();

private:
    void loadNtDll();
    void retrieveNtQueryInformationProcessFunctionPointer();
    void unloadNtDll();

    std::wstring getLcuProcessCommand();
    std::wstring extractProcessName(HANDLE processHandle);
    std::wstring extractProcessCommand(HANDLE processHandle);

private:
    HINSTANCE m_dllLib;
    NtQueryInformationProcessPtr m_ntQueryInformationProcessPtr;
};

} // namespace rito
