#include "dll_injection.h"
#include <iostream>


LPVOID write_into_process(HANDLE hProcess, LPBYTE buffer, SIZE_T buffer_size, DWORD protect)
{
    LPVOID remoteAddress = VirtualAllocEx(hProcess, NULL, buffer_size, MEM_COMMIT | MEM_RESERVE, protect);
    if (remoteAddress == NULL) {
        std::cerr << "Could not allocate memory in the remote process\n";
        return NULL;
    }
    if (!WriteProcessMemory(hProcess, remoteAddress, buffer, buffer_size, NULL)) {
        VirtualFreeEx(hProcess, remoteAddress, buffer_size, MEM_FREE);
        return NULL;
    }
    return remoteAddress;
}

bool inject_with_loadlibrary(HANDLE hProcess, const wchar_t *inject_path)
{
    if (!inject_path) {
        return false;
    }
    HMODULE hModule = GetModuleHandleW(L"kernel32.dll");
    if (!hModule) return false;

    FARPROC hLoadLib = GetProcAddress(hModule, "LoadLibraryW");
    if (!hLoadLib) return false;

    //calculate size along with the terminating '\0'
    SIZE_T inject_path_size = (wcslen(inject_path)  + 1) * sizeof(inject_path[0]);

    // write the full path of the DLL into the remote process:
    PVOID remote_ptr = write_into_process(hProcess, (BYTE*)inject_path, inject_path_size, PAGE_READWRITE);
    if (!remote_ptr) {
        std::cerr << "Writing to process failed: " << std::hex << GetLastError() << "\n";
        return false;
    }
#ifdef _DEBUG
    std::cout << "[" << GetProcessId(hProcess) << "] Path writen to: " << remote_ptr << "\n";
#endif
    // Inject to the remote process:
    DWORD ret = WAIT_FAILED;
    HANDLE hndl = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)hLoadLib, remote_ptr, NULL, NULL);
    if (hndl) {
        ret = WaitForSingleObject(hndl, INJ_TIMEOUT);
        CloseHandle(hndl); hndl = NULL;
    }
    else {
        std::cout << "Creating thread failed!\n";
    }
    // cleanup:
    VirtualFreeEx(hProcess, remote_ptr, 0, MEM_RELEASE);
    if (ret == WAIT_OBJECT_0) {
        return true;
    }
    return false;
}

bool unload_remote_module(HANDLE hProcess, HANDLE toUnload)
{
    if (!toUnload) {
        return false;
    }
    HMODULE hModule = GetModuleHandleW(L"kernel32.dll");
    if (!hModule) return false;

    FARPROC toExec = GetProcAddress(hModule, "FreeLibrary");
    if (!toExec) return false;

    // Inject to the remote process:
    DWORD ret = WAIT_FAILED;
    HANDLE hndl = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)toExec, toUnload, NULL, NULL);
    if (hndl) {
        ret = WaitForSingleObject(hndl, INFINITE);
        CloseHandle(hndl); hndl = NULL;
    }
    else {
        std::cout << "Creating thread failed!\n";
    }
    // cleanup:
    if (ret == WAIT_OBJECT_0) {
        return true;
    }
    return false;
}

HANDLE open_process(DWORD pid)
{
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
        FALSE,
        pid
    );
    if (!hProcess || hProcess == INVALID_HANDLE_VALUE) {
        const DWORD err = GetLastError();
        if (err == ERROR_INVALID_PARAMETER) {
            std::cerr << "[ERROR] [" << std::dec << pid << "] Opening the process failed. Is the process still running?"<< std::endl;
            return NULL;
        }
        std::cerr << "[ERROR] [" << std::dec << pid << "] Opening the process failed: " << std::hex << "0x" << err << std::endl;
        return NULL;
    }
    return hProcess;
}
