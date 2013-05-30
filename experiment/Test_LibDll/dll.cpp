﻿// created by i-saint
// distributed under Creative Commons Attribution (CC BY) license.
// https://github.com/i-saint/DynamicObjLoader

#include <windows.h>
#define dpLinkDynamic
#include "DynamicPatcher.h"

dpPatch void OverriddenByDll()
{
    printf("OverriddenByDll(): overridden!\n");
    typedef void (*OrigT)();
    OrigT f = (OrigT)dpGetUnpatchedFunction(&OverriddenByDll);
    f();
}