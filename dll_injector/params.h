#pragma once

#include <paramkit.h>

#define PARAM_PID "pid"
#define PARAM_EXE "exe"
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
    std::wstring exe_path;
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
        this->addParam(new IntParam(PARAM_PID, false));
        this->setInfo(PARAM_PID, "Target PID (where to inject)");

        this->addParam(new WStringParam(PARAM_EXE, false));
        this->setInfo(PARAM_EXE, "Exe to be run with the DLL injected");

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
        copyVal<WStringParam>(PARAM_EXE, paramsStruct.exe_path);
        return true;
    }
    
    bool hasAlternativesFilled()
    {
        IntParam *pidParam = dynamic_cast<IntParam*>(this->getParam(PARAM_PID));
        if (pidParam && pidParam->isSet()) {
            return true;
        }
        WStringParam *exeParam = dynamic_cast<WStringParam*>(this->getParam(PARAM_EXE));
        if (exeParam && exeParam->isSet()) {
            return true;
        }
        return false;
    }

    void printAlternatives()
    {
        std::cout << "Supply target PID or executable\n";
        IntParam *pidParam = dynamic_cast<IntParam*>(this->getParam(PARAM_PID));
        if (pidParam) {
            pidParam->printInColor(paramColor);
            std::cout << "\n";
        }
        WStringParam *exeParam = dynamic_cast<WStringParam*>(this->getParam(PARAM_EXE));
        if (exeParam && !exeParam->isSet()) {
            exeParam->printInColor(paramColor);
            std::cout << "\n";
        }
    }
};
