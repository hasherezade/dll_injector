#include "actions.h"

#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <psapi.h>

#include "dll_injection.h"

bool is_compatibile(HANDLE hProcess)
{
    BOOL isTargetWow64 = FALSE;
    IsWow64Process(hProcess, &isTargetWow64);

    BOOL isInjectorWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(), &isInjectorWow64);

    if (isTargetWow64 == isInjectorWow64) {
        return true;
    }
    return false;
}

bool get_process_name(IN HANDLE hProcess, OUT LPWSTR nameBuf, IN SIZE_T nameMax)
{
    HMODULE hMod;
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
        GetModuleBaseNameW(hProcess, hMod, nameBuf, nameMax);
        return true;
    }
    return false;
}

size_t enum_modules(IN HANDLE hProcess, IN OUT HMODULE hMods[], IN const DWORD hModsMax, IN DWORD filters) //throws exceptions
{
    if (hProcess == nullptr) {
        return 0;
    }
    
    DWORD cbNeeded;
#ifdef _WIN64
    if (!EnumProcessModulesEx(hProcess, hMods, hModsMax, &cbNeeded, filters)) {
        return 0;
    }
#else
    /*
    Some old, 32-bit versions of Windows do not have EnumProcessModulesEx,
    but we can use EnumProcessModules for the 32-bit version: it will work the same and prevent the compatibility issues.
    */
    if (!EnumProcessModules(hProcess, hMods, hModsMax, &cbNeeded)) {
        return 0;
    }
#endif
    const size_t modules_count = cbNeeded / sizeof(HMODULE);
    return modules_count;
}


inline WCHAR to_lowercase(WCHAR c1)
{
    if (c1 <= L'Z' && c1 >= L'A') {
        c1 = (c1 - L'A') + L'a';
    }
    return c1;
}

bool is_wanted_module(wchar_t* curr_name, wchar_t* wanted_name)
{
    if (wanted_name == NULL || curr_name == NULL) return false;

    WCHAR *curr_end_ptr = curr_name;
    while (*curr_end_ptr != L'\0') {
        curr_end_ptr++;
    }
    if (curr_end_ptr == curr_name) return false;

    wchar_t *wanted_end_ptr = wanted_name;
    while (*wanted_end_ptr != L'\0') {
        wanted_end_ptr++;
    }
    if (wanted_end_ptr == wanted_name) return false;

    while ((curr_end_ptr != curr_name) && (wanted_end_ptr != wanted_name)) {

        if (to_lowercase(*wanted_end_ptr) != to_lowercase(*curr_end_ptr)) {
            return false;
        }
        wanted_end_ptr--;
        curr_end_ptr--;
    }
    return true;
}

HMODULE search_module_by_name(IN HANDLE hProcess, IN const std::wstring &searchedName)
{
    const DWORD hModsMax = 0x1000;
    HMODULE hMods[hModsMax] = { 0 };

    size_t modules_count = enum_modules(hProcess, hMods, hModsMax, LIST_MODULES_ALL);

    wchar_t nameBuf[MAX_PATH] = { 0 };

    size_t i = 0;
    for (i = 0; i < modules_count; i++) {
        HMODULE hMod = hMods[i];
        if (!hMod || hMod == INVALID_HANDLE_VALUE ) break;

        memset(nameBuf, 0, sizeof(nameBuf));
        if (GetModuleFileNameExW(hProcess, hMod, nameBuf, MAX_PATH)) {
#ifdef _DEBUG
            std::wcout << nameBuf << "\n";
#endif
            if (is_wanted_module(nameBuf, (wchar_t*) searchedName.c_str())) {

                //std::wcout << "Matched " << nameBuf << "\n";

                return hMod;
            }
        }
    }
    return NULL;
}

bool is_module_in_process(DWORD pid, const wchar_t *dll_path)
{
    HANDLE hProcess = open_process(pid);
    if (!hProcess) return false;

    HANDLE fMod = search_module_by_name(hProcess, dll_path);
    bool isFound = false;
    if (fMod) {
        isFound = true;
    }
    CloseHandle(hProcess);
    return isFound;
}

unload_status unload_module(DWORD pid, const wchar_t *dll_path)
{
    HANDLE hProcess = open_process(pid);
    if (!hProcess) return USTAT_ACCESS_DENIED;

    if (!is_compatibile(hProcess)) {
        CloseHandle(hProcess);
        std::cerr << "[" << std::dec << pid << "] Injector bitness mismatch, skipping" << std::endl;
        return USTAT_ACCESS_DENIED;
    }

    HANDLE fMod = search_module_by_name(hProcess, dll_path);
    if (!fMod) {
        return USTAT_NOT_FOUND;
    }
    unload_status isUnl = unload_status::USTAT_NONE;
    if (unload_remote_module(hProcess, fMod)) {
        isUnl = USTAT_SUCCESS;
    }
    Sleep(10);
    if (search_module_by_name(hProcess, dll_path)) {
        // still loaded
        isUnl = USTAT_FOUND_FAILED;
    }
    CloseHandle(hProcess);
    return isUnl;
}


bool inject_into_process(DWORD pid, const wchar_t *dll_path)
{
    HANDLE hProcess = open_process(pid);
    if (!hProcess) return false;

    if (!is_compatibile(hProcess)) {
        CloseHandle(hProcess);
        std::cerr << "[" << std::dec << pid << "] Injector bitness mismatch, skipping" << std::endl;
        return false;
    }

    bool isLoaded = false;
    bool isInjected = inject_with_loadlibrary(hProcess, dll_path);
    if (search_module_by_name(hProcess, dll_path)) {
        isLoaded = true;
    }
    CloseHandle(hProcess);
#ifdef _DEBUG
    if (isLoaded) {
        std::cout << "[" << pid << "] Injected\n";
    }
    else {
        std::cerr << "[ERROR][" << std::dec << pid << "] Injection failed!\n";
    }
#endif
    return isLoaded;
}
