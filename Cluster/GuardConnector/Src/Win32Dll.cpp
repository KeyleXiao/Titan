﻿// Win32Dll.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

_WIN32_DLL_API void Win32FunTest()
{
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

