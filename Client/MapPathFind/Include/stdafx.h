// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

#include "Base.h"

using namespace rkt;

// 按CPU位数加载LIB
//#ifdef _WIN64
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(DetourNavMesh))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(NmgenRcn))


#include "PathFindDef.h"