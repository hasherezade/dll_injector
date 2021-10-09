#pragma once

#include <paramkit.h>

#define PARAM_PID "pid"
#define PARAM_DLL "dll"
#define PARAM_ACTION "action"

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
        this->addParam(new IntParam(PARAM_PID, true));
        this->setInfo(PARAM_PID, "Target PID (where to inject)");

        this->addParam(new WStringParam(PARAM_DLL, true));
        this->setInfo(PARAM_DLL, "DLL path");

        EnumParam *myEnum = new EnumParam(PARAM_ACTION, "action_id", true);
        this->addParam(myEnum);
        this->setInfo(PARAM_ACTION, "Action to be executed");
        myEnum->addEnumValue(t_actions::ACTION_CHECK, "C", "check if the DLL is loaded");
        myEnum->addEnumValue(t_actions::ACTION_LOAD, "L", "load the DLL ");
        myEnum->addEnumValue(t_actions::ACTION_UNLOAD, "U", "unload the DLL");
    }

    bool fillStruct(t_params_struct &paramsStruct)
    {
        copyVal<IntParam>(PARAM_PID, paramsStruct.pid);
        copyVal<EnumParam>(PARAM_ACTION, paramsStruct.action);
        copyVal<WStringParam>(PARAM_DLL, paramsStruct.dll_path);
        return true;
    }
};
