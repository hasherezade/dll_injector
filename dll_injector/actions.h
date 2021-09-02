#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

#include "params.h"

enum unload_status {
    USTAT_NONE,
    USTAT_ACCESS_DENIED,
    USTAT_NOT_FOUND,
    USTAT_FOUND_FAILED,
    USTAT_SUCCESS
};


HMODULE search_module_by_name(IN HANDLE hProcess, IN const std::wstring &searchedName);

bool is_module_in_process(DWORD pid, const wchar_t *dll_path);

unload_status unload_module(DWORD pid, const wchar_t *dll_path);

bool inject_into_process(DWORD pid, const wchar_t *dll_path);

