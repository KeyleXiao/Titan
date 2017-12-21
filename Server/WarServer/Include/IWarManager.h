/*******************************************************************
** 文件名:	IWarManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	

			场景管理器

**************************** 修改记录 ******************************
    ** 修改人: ljw
    ** 日  期: 
** 描  述:  简化代码
********************************************************************/

#pragma once
#include "IServiceContainer.h"

// 房间创建战场上下文
struct Room_Create_War_Context
{
    int     nWarID;             // 场景服上战场对象ID
    BYTE    byRoomType;         // 房间类型
    DWORD   dwMatchTypeID;
    int     nRoomIndex;         // 房间ID
    int     nMapID;
    int     nSchemeConfigWarID;
    BYTE    nMode;
    BYTE    byRoleCount;
    DWORD   dwAvgRankScore;
    BYTE    byCampCount;

    Room_Create_War_Context(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

/**
	负责场景的创建，销毁，场景对象的查询，甚至动态负载等
*/
struct IWarManager
{
    // 创建战场
    virtual int CreateWar(Room_Create_War_Context& context) = 0;

    // 销毁战场
    virtual void DestroyWar(int nWarID) = 0;

    // 获取战场service
    virtual SERVICE_PTR GetWar(int nWarID) = 0;

    virtual void onStop() = 0;

    virtual void release() = 0;

    // 战场service退出回调
    virtual void OnWarDestroied(int nWarID) = 0;

    // 设置战场ID对应的ContainerID
    virtual void onWarCreate(int nWarID, SERVICE_PTR pContainer) = 0;


};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(WARSERVER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(WARSERVER))
extern "C" IWarManager * CreateWarManager();
#	define	CreateWarManagerProc	CreateWarManager
#else													/// 动态库版本
typedef IWarManager * (RKT_CDECL * procCreateWarManager)();
#	define	CreateWarManagerProc	DllApi<procCreateWarManager>::load(MAKE_DLL_NAME(WarServer), "CreateWarManager")
#endif

