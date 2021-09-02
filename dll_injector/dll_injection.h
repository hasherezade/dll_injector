#pragma once

#include <windows.h>

#define INJ_TIMEOUT 100000

bool inject_with_loadlibrary(HANDLE hProcess, const wchar_t *inject_path);

bool unload_remote_module(HANDLE hProcess, HANDLE toUnload);

bool inject_into_process(DWORD pid, const wchar_t *dll_path);

HANDLE open_process(DWORD pid);
