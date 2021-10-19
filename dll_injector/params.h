#pragma once

#include <paramkit.h>

#define PARAM_PID "pid"
#define PARAM_TARGET "target"
#define PARAM_DLL "dll"
#define PARAM_ACTION "action"
#define PARAM_CMD "cmd"

using namespace paramkit;

typedef enum  {
    ACTION_CHECK = 0,
    ACTION_LOAD = 1,
    ACTION_UNLOAD,
    ACTIONS_COUNT
} t_actions;

typedef struct {
    DWORD pid;
    std::wstring dll_path;
    std::wstring target;
    std::wstring cmd;
    t_actions action;
} t_params_struct;


class InjParams : public Params
{
public:

    static std::wstring action_to_string(t_actions action)
    {
        switch (action) {
        case t_actions::ACTION_LOAD:
            return L"Load";
        case t_actions::ACTION_UNLOAD:
            return L"Unload";
        case t_actions::ACTION_CHECK:
            return L"Check";
        }
        return L"";
    }

    InjParams()
        : Params()
    {
        this->addParam(new WStringParam(PARAM_TARGET, true));
        this->setInfo(PARAM_TARGET, "Exe to be run with the DLL injected, or target PID (where to inject)");

        this->addParam(new WStringParam(PARAM_DLL, true));
        this->setInfo(PARAM_DLL, "DLL path");

        this->addParam(new WStringParam(PARAM_CMD, false));
        this->setInfo(PARAM_CMD, "Commandline arguments (for a newly created process)");

        EnumParam *myEnum = new EnumParam(PARAM_ACTION, "action_id", false);
        this->addParam(myEnum);
        this->setInfo(PARAM_ACTION, "Action to be executed");
        myEnum->addEnumValue(t_actions::ACTION_CHECK, "C", "check if the DLL is loaded");
        myEnum->addEnumValue(t_actions::ACTION_LOAD, "L", "load the DLL [DEFAULT]");
        myEnum->addEnumValue(t_actions::ACTION_UNLOAD, "U", "unload the DLL");
    }

    bool fillStruct(t_params_struct &paramsStruct)
    {
        copyVal<EnumParam>(PARAM_ACTION, paramsStruct.action);
        copyVal<WStringParam>(PARAM_DLL, paramsStruct.dll_path);
        copyVal<WStringParam>(PARAM_TARGET, paramsStruct.target);
        copyVal<WStringParam>(PARAM_CMD, paramsStruct.cmd);
        return true;
    }

    virtual void printBanner()
    {
        char logo1[] = "\n\
 /\\_/\\                                            \n\
((@v@))     Hookoo                                \n\
():::()     Injector for hooking libraries        \n\
-\" - \"----                                        ";
        paramkit::print_in_color(MAKE_COLOR(BROWN, DARK_BLUE), logo1);
        std::cout << "\n" << std::endl;
#ifdef _WIN64
        std::cout << "64-bit version\n";
#else
        std::cout << "32-bit version\n";
#endif
        std::cout << "Built on: " << __DATE__ << "\n";
        std::cout << "URL: https://github.com/hasherezade/dll_injector \n";
        std::cout << std::endl;
    }
};
