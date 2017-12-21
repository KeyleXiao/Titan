// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <metahost.h>  
#include <mscoree.h>  
#pragma comment(lib, "mscoree.lib")  

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>



// TODO: 在此处引用程序需要的其他头文件
#include "Base.h"
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(MapPathFindClient))
using namespace rkt;

#include "Common.h"
#include "IEventEngine.h"

#include "GameDef.h"
#include "GameViewDef.h"
#include "EntityDef.h"
#include "IRenderView.h"
#include "GatewayServiceDef.h"

#include "platform.h"
#include "NetMessageDef.h"
//#include "IEntityView.h"
#include "IEntityClient.h"
#include "DSkillCommon.h"
#include "ISpell.h"
#include "Task.h"
#include "Language_inl.h"


extern const std::string& getBasePath();