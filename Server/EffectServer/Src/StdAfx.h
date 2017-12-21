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
#pragma comment(lib, MAKE_DLL_LIB_NAME(MapPathFind))

#include "GameDef.h"
#include "EventDef.h"
#include "EffectDef.h"
#include "AOI_Define.h"
#include "ISceneManager.h"

#include "IServerGlobal.h"
#include "IEntity.h"
#include "IActorService.h"
#include "IPlayerRole.h"
#include "ISchemeCenter.h"
#include "IServiceContainer.h"
#include "IEntityProperty.h"
#include "ITimerService.h"

#include "Language_inl.h"