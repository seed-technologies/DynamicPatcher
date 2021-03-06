﻿// created by i-saint
// distributed under Creative Commons Attribution (CC BY) license.
// https://github.com/i-saint/DynamicPatcher

#include <windows.h>
#include <intrin.h>
#include <stdint.h>
#include <cstdio>
#include <clocale>
#include <algorithm>

//#define dpDisable
//#define dpLinkStatic
#include "DynamicPatcher.h"

#ifdef _M_X64
#   define dpPlatform "x64"
#else
#   define dpPlatform "Win32"
#endif
#ifdef _DEBUG
#   define dpConfiguration "Debug"
#else
#   define dpConfiguration "Release"
#endif
#define dpObjDir "_tmp/Test_Simple_" dpPlatform dpConfiguration 

// __declspec(dllexport) がついてる関数や class には export 属性が .obj にも残る。
// これを利用し、export 属性が付いているものはロードされた時自動的に patch する。
// ( dpPatch == __declspec(dllexport) )

dpScope(
int puts_hook(const char *s)
{
    typedef int (*puts_t)(const char *s);
    puts_t orig_puts = (puts_t)dpGetUnpatched(&puts);
    orig_puts("puts_hook()");
    return orig_puts(s);
}
)

class dpPatch Test
{
public:
    Test() : m_end_flag(false) {}
    virtual ~Test() {}

    dpNoInline virtual void doSomething()
    {
        puts("Test::doSomething()");
        printf("Test::s_value: %d\n", s_value);
        //m_end_flag = true;
    }

    bool getEndFlag() const { return m_end_flag; }

private:
    static int s_value;
    static const int s_cvalue;
    bool m_end_flag;
};
int Test::s_value = 42;
const int Test::s_cvalue = 42;

dpNoInline void Test_ThisMaybeOverridden()
{
    printf("Test_ThisMaybeOverridden()\n");
}

int main(int argc, char *argv[])
{
    dpInitialize(dpE_LogAll);
    dpAddModulePath(dpObjDir"/*.obj");
    dpAddSourcePath("Test_Simple");
    dpAddMSBuildCommand("Test_Simple.vcxproj /target:ClCompile /m /p:Configuration="dpConfiguration";Platform="dpPlatform);
    dpStartAutoBuild();

    printf("DynamicPatcher Test_Simple\n");
    {
        Test test;
        while(!test.getEndFlag()) {
            test.doSomething();

            ::Sleep(1000);
            dpUpdate();
        }
    }
    dpFinalize();
}

#ifdef dpWithTDisasm
dpOnLoad(
    dpPatchAddressToAddress(&puts, &puts_hook);
)
#endif // dpWithTDisasm

dpOnUnload(
    printf("unloaded.\n");
)
