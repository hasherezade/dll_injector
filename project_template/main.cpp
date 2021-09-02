#include <iostream>
#include <paramkit.h>

#define PARAM_MY_DEC "pdec"
#define PARAM_MY_HEX "phex"

#define PARAM_MY_ASTRING "pastr"
#define PARAM_MY_WSTRING "pwstr"

#define PARAM_MY_BOOL "pbool"
#define PARAM_MY_ENUM "penum"

#define MAX_BUF 50

typedef enum {
    FRUIT_APPLE = 0,
    FRUIT_ORANGE = 1,
    FRUIT_STRAWBERY,
    FRUIT_COUNT
} t_fruits;

//---

using namespace paramkit;

typedef struct {
    DWORD myDec;
    DWORD myHex;
    bool myBool;
    char myABuf[MAX_BUF];
    wchar_t myWBuf[MAX_BUF];
    t_fruits myEnum;
} t_params_struct;

void print_params(t_params_struct &p)
{
    std::cout  << "myDec :  [" << std::dec << p.myDec << "] = " << std::hex << "[0x" << p.myDec << "]\n";
    std::cout  << "myHex :  [" << std::hex << "0x" << p.myHex << "] = [" << std::dec << p.myHex << "]\n";
    std::cout  << "myBool:  [" << std::dec << p.myBool << "]\n";
    std::cout  << "myABuf:  [" << p.myABuf << "]\n";
    std::wcout << "myWBuf:  [" << p.myWBuf << "]\n";
    std::cout  << "myEnum:  [" << std::dec << p.myEnum << "]\n";
}

class DemoParams : public Params
{
public:
    DemoParams()
        : Params()
    {
        this->addParam(new IntParam(PARAM_MY_DEC, true));
        this->setInfo(PARAM_MY_DEC, "Sample decimal Integer param");

        this->addParam(new IntParam(PARAM_MY_HEX, true, true));
        this->setInfo(PARAM_MY_HEX, "Sample hexadecimal Integer param");

        this->addParam(new BoolParam(PARAM_MY_BOOL, false));
        this->setInfo(PARAM_MY_BOOL, "Sample boolean param");

        this->addParam(new StringParam(PARAM_MY_ASTRING, false));
        this->setInfo(PARAM_MY_ASTRING, "Sample string param");

        this->addParam(new WStringParam(PARAM_MY_WSTRING, false));
        this->setInfo(PARAM_MY_WSTRING, "Sample wide string param");

        EnumParam *myEnum = new EnumParam(PARAM_MY_ENUM, GETNAME(t_fruits), false);
        this->addParam(myEnum);
        this->setInfo(PARAM_MY_ENUM, "Sample enum param");
        myEnum->addEnumValue(t_fruits::FRUIT_APPLE, "A", "green apples");
        myEnum->addEnumValue(t_fruits::FRUIT_ORANGE, "O", "oranges");
        myEnum->addEnumValue(t_fruits::FRUIT_STRAWBERY, "S", "fresh strawberries");

        //optional: group parameters
        std::string str_group = "string params";
        this->addGroup(new ParamGroup(str_group));
        this->addParamToGroup(PARAM_MY_ASTRING, str_group);
        this->addParamToGroup(PARAM_MY_WSTRING, str_group);

        str_group = "enums";
        this->addGroup(new ParamGroup(str_group));
        this->addParamToGroup(PARAM_MY_ENUM, str_group);
    }

    bool fillStruct(t_params_struct &paramsStruct)
    {
        BoolParam *myBool = dynamic_cast<BoolParam*>(this->getParam(PARAM_MY_BOOL));
        if (myBool) paramsStruct.myBool = myBool->value;

        IntParam *myDec = dynamic_cast<IntParam*>(this->getParam(PARAM_MY_DEC));
        if (myDec) paramsStruct.myDec = myDec->value;

        IntParam *myHex = dynamic_cast<IntParam*>(this->getParam(PARAM_MY_HEX));
        if (myHex) paramsStruct.myHex = myHex->value;

        StringParam *myStr = dynamic_cast<StringParam*>(this->getParam(PARAM_MY_ASTRING));
        if (myStr) {
            myStr->copyToCStr(paramsStruct.myABuf, _countof(paramsStruct.myWBuf));
        }
        WStringParam *myWStr = dynamic_cast<WStringParam*>(this->getParam(PARAM_MY_WSTRING));
        if (myWStr) {
            myWStr->copyToCStr(paramsStruct.myWBuf, _countof(paramsStruct.myWBuf));
        }
        EnumParam *myEnum = dynamic_cast<EnumParam*>(this->getParam(PARAM_MY_ENUM));
        if (myEnum) {
            paramsStruct.myEnum = (t_fruits)myEnum->value;
        }
        return true;
    }
};

int main(int argc, char* argv[])
{
    DemoParams params;
    if (argc < 2) {
        paramkit::print_in_color(CYAN, "Welcome to ParamKit Demo!");
        std::cout << "\n\n";
        params.info(false);
        return 0;
    }
    if (!params.parse(argc, argv)) {
        return 0;
    }
    std::cout << "\nPrinting the filled params:\n";
    params.print();

    t_params_struct p;
    params.fillStruct(p);

    std::cout << "\nConverted to the structure:\n";
    print_params(p);
    std::cout << std::endl;

    if (!params.hasRequiredFilled()) {
        std::cout << "[!] Some of the required parameters are not filled!\n";
    }
    else {
        std::cout << "[+] All the required parameters filled!\n";
    }
    return 0;
}
