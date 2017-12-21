// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
//#include "GameAPI.h"
#include "ClientGlobal.h"
#include "CrashReport\CrashReport.h"

BOOL CALLBACK DllEnumWindowsProc(HWND hwnd, LPARAM lParam) 
{ 
    DWORD dwCurProcessId = *((DWORD*)lParam); 
    DWORD dwProcessId = 0; 

    GetWindowThreadProcessId(hwnd, &dwProcessId); 
    if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
    { 
        *((HWND *)lParam) = hwnd;
        return FALSE; 
    } 
    return TRUE; 
} 


HWND DllGetMainWindow() 
{ 
    DWORD dwCurrentProcessId = GetCurrentProcessId();
    if(!EnumWindows(DllEnumWindowsProc, (LPARAM)&dwCurrentProcessId)) 
    {     
        return (HWND)dwCurrentProcessId; 
    } 
    return NULL; 
}

//// DLL入口处理 //////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HMODULE hModule,	DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			g_MainWnd = DllGetMainWindow();
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		{
			//ErrorLn("DLL_PROCESS_DETACH");
		}
		break;
	}
	return TRUE;
}


