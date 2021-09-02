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
        IntParam *myDec = dynamic_cast<IntParam*>(this->getParam(PARAM_PID));
        if (myDec && myDec->isSet()) paramsStruct.pid = myDec->value;
        
        EnumParam *myEnum = dynamic_cast<EnumParam*>(this->getParam(PARAM_ACTION));
        if (myEnum && myEnum->isSet()) paramsStruct.action = (myEnum->value) > ACTIONS_COUNT ? ACTION_CHECK: t_actions(myEnum->value);

        WStringParam *myStr = dynamic_cast<WStringParam*>(this->getParam(PARAM_DLL));
        if (myStr) {
            paramsStruct.dll_path = myStr->value;
        }
        return true;
    }
};
