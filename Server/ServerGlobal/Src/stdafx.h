// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>



// TODO: 在此处引用程序需要的其他头文件
#include "Base.h"
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))
using namespace rkt;

#include "GameDef.h"
#include "StubMacroDef.h"
#include "Event_ManagedDef.h"
#include "TestModuleDef.h"
#include "ControllerDef.h"

#ifndef SAFE_RELEASE
#	define SAFE_RELEASE(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif
#ifndef Safe_Release
#	define Safe_Release(ptr)		if ((ptr)) {(ptr)->release(); (ptr) = 0;}
#endif
#ifndef SAFE_DELETEARRAY
#   define SAFE_DELETEARRAY(data)   if(data) { delete[] (data); data=NULL;};
#endif