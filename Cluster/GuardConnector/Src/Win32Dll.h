// Win32Dll.h : 模块接口头文件样本
#ifndef __WIN32_DLL_H_
#define __WIN32_DLL_H_








#ifndef _WIN32_DLL_API
#ifdef WIN32
#define _WIN32_DLL_API extern "C" _declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib, "Win32Dll_d")
#elif RELEASEDEBUG
#pragma comment(lib, "Win32Dll_rd")
#else
#pragma comment(lib, "Win32Dll")
#endif
#else
#define _WIN32_DLL_API
#endif
#endif

_WIN32_DLL_API void Win32FunTest();

#endif // ndef __WIN32_DLL_H_
