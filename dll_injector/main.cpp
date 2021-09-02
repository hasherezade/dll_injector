#include <windows.h>
#include <iostream>
#include <sstream>

#include "dll_injection.h"
#include "process_privilege.h"

#include "params.h"
#include "actions.h"

void banner()
{
    char logo1[] = "\n\
 /\\_/\\\n\
((@v@))  Hookoo\n\
():::()  Injector for hooking libraries\n\
-\" - \"----\n";

    paramkit::print_in_color(MAKE_COLOR(BROWN, DARK_BLUE), logo1);
    std::cout << "\n" << std::endl;
#ifdef _WIN64
    std::cout << "64-bit version\n";
#else
    std::cout << "32-bit version\n";
#endif
    std::cout << "Built on: " << __DATE__ << "\n";
    std::cout << std::endl;
}

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

void print_names(std::vector<std::string> successNames)
{
    std::vector<std::string>::iterator itr;
    for (itr = successNames.begin(); itr != successNames.end(); ++itr) {
        std::cout << *itr << "\n";
    }
}

int wmain(int argc, const wchar_t * argv[])
{
    t_params_struct iParams;
    {
        InjParams params;
        if (argc < 2) {
            banner();
            params.info();
            system("pause");
            return 0;
        }
        if (!params.parse(argc, argv) || !params.hasRequiredFilled()) {
            return 0;
        }
        params.fillStruct(iParams);
    }

    if (set_debug_privilege()) {
        std::cout << "[*] Debug privilege set!\n";
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
    return res ? 0 : -1;
}
