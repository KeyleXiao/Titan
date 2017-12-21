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
#define new RKT_NEW
using namespace rkt;
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Framework))


#include "ICenterServerProtocol.h"
#include "ICenterServerConnector.h"
using namespace CenterServer;

// 填充
#define fillClanData(ob, destEP, msgId) \
    SNetMsgHead* ___msgHead = (SNetMsgHead*)ob.data();\
    ___msgHead->bySrcEndPoint = MSG_ENDPOINT_SOCIAL;\
    ___msgHead->byDestEndPoint = destEP;\
    ___msgHead->byKeyModule = MSG_MODULEID_CLAN;\
    ___msgHead->byKeyAction = (ushort)msgId;\
    ob.offset(sizeof(SNetMsgHead))

// 0显示调试信息
// !=0禁止显示调试信息
#define MatchServer_DisableShowDebug 0