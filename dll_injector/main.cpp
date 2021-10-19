#include <windows.h>
#include <iostream>
#include <sstream>

#include "dll_injection.h"
#include "process_privilege.h"

#include "params.h"
#include "actions.h"

#define INVALID_PID (-1)



bool action_load(t_params_struct &iParams)
{
    std::cout << "Selected Action: LOAD\n";
    size_t injected = 0;

    if (inject_into_process(iParams.pid, iParams.dll_path.c_str())) {
        if (is_module_in_process(iParams.pid, iParams.dll_path.c_str())) {
            paramkit::print_in_color(MAKE_COLOR(BLACK, LIME), "Injection OK!");
            return true;
        }
    }
    std::cout << "Injection failed\n";
    return false;
}

bool action_check(t_params_struct &iParams)
{
    std::cout << "Selected Action: CHECK\n";
    if (is_module_in_process(iParams.pid, iParams.dll_path.c_str())) {
        paramkit::print_in_color(MAKE_COLOR(BLACK, WHITE), "Module found!");
        return true;
    }
    paramkit::print_in_color(MAKE_COLOR(BLACK, WHITE), "Module not present in the process!");
    return false;
}

bool action_unload(t_params_struct &iParams)
{
    std::cout << "Selected Action: UNLOAD\n";

    bool isFound = false;
    if (!is_module_in_process(iParams.pid, iParams.dll_path.c_str())) {
        std::cout << "Module not present in the process!\n";
        return false;
    }
    if (unload_module(iParams.pid, iParams.dll_path.c_str())) {
        isFound = true;
        paramkit::print_in_color(MAKE_COLOR(BLACK,YELLOW), "Module unloaded!");
    }
    return isFound;
}


HANDLE create_new_process(IN std::wstring exe_path, IN  std::wstring cmd, OUT PROCESS_INFORMATION &pi, DWORD flags)
{
    std::wstring full_cmd = std::wstring(exe_path) + L" " + std::wstring(cmd);

    const size_t buf_len = (full_cmd.length() + 1) * sizeof(wchar_t);
    wchar_t* cmd_str = new wchar_t[buf_len];
    if (cmd_str) {
        memset(cmd_str, 0, buf_len);
        memcpy(cmd_str, full_cmd.c_str(), buf_len);
    }

    STARTUPINFOW si = { 0 };
    si.cb = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;

    HANDLE pHndl = NULL;
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    if (CreateProcessW(
        exe_path.c_str(),
        cmd_str,
        NULL, //lpProcessAttributes
        NULL, //lpThreadAttributes
        FALSE, //bInheritHandles
        flags, //dwCreationFlags
        NULL, //lpEnvironment 
        NULL, //lpCurrentDirectory
        &si, //lpStartupInfo
        &pi //lpProcessInformation
    ))
    {
        pHndl  = pi.hProcess;
    }
#ifdef _DEBUG
    else {
        std::cerr << "[ERROR] CreateProcess failed, Error = " << GetLastError() << std::endl;
    }
#endif
    delete[]cmd_str;
    return pHndl;
}


int wmain(int argc, const wchar_t * argv[])
{
    t_params_struct iParams = { 0 };
    iParams.pid = INVALID_PID;
    iParams.action = t_actions::ACTION_LOAD;
    {
        InjParams params;
        if (argc < 2) {
            params.printBanner();
            params.printInfo();
            system("pause");
            return 0;
        }
        if (!params.parse(argc, argv)) {
            return 0;
        }
        params.fillStruct(iParams);
    }
    
    if (set_debug_privilege()) {
        std::cout << "[*] Debug privilege set!\n";
    }

    std::string str(iParams.target.begin(), iParams.target.end());
    if (paramkit::is_number(str.c_str())) {
        iParams.pid = paramkit::get_number(str.c_str());
    }

    bool isCreated = false;
    HANDLE hThread = NULL;
    if (iParams.pid == INVALID_PID) {

        PROCESS_INFORMATION pi = { 0 };
        HANDLE proc = create_new_process(iParams.target, iParams.cmd, pi, CREATE_SUSPENDED | CREATE_NEW_CONSOLE);
        if (!proc) {
            std::cerr << "Failed to create the process\n";
            return -1;
        }

        isCreated = true;
        iParams.pid = pi.dwProcessId;
        hThread = pi.hThread;
    }

    std::wcout << "DLL: " << iParams.dll_path << "\n";
    std::cout << "PID: " << iParams.pid << "\n";

    bool res = false;
    switch (iParams.action) {
    case t_actions::ACTION_LOAD:
        res = action_load(iParams); break;
    case t_actions::ACTION_UNLOAD:
        res = action_unload(iParams); break;
    case t_actions::ACTION_CHECK:
        res = action_check(iParams); break;
    }

    std::cout << std::endl;

    if (isCreated) {
        ResumeThread(hThread);
    }
    return res ? 0 : -1;
}
