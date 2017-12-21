// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

// Windows 头文件:
#include <windows.h>


// TODO: 在此处引用程序需要的其他头文件
#include "Base.h"
#define new RKT_NEW
using namespace rkt;
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))

#include "GameDef.h"
#include "EventDef.h"
//#include <WinDef.h>
#include "IServerGlobal.h"
#include "WarDef.h"
#include "MobaGoodsDef.h"
#include "MobaGoodsPart.h"
#include "EntityDef.h"
#include "Api.h"

// 释放内存宏
#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(p)			{ if((p) != NULL) { (p)->Release(); (p)=NULL; } }
#endif

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(p)			{ if((p) != NULL) { delete (p);     (p)=NULL; } }
#endif

#if !defined(SAFE_DELETEARRAY)
#define SAFE_DELETEARRAY(p)		{ if((p) != NULL) { delete [](p);	(p) = NULL;} }
#endif