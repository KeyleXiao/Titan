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

//#include "Common.h"
#include "Api.h"
#include "IEventEngine.h"

#include "IRenderView.h"
#include "GameDef.h"
#include "IEntityClient.h"
#include "DSkillCommon.h"
#include "SkillDef.h"
#include "IClientGlobal.h"
#include "Entity_inl.h"

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

//#define MINI_GAME_CLIENT
