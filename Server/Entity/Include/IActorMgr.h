/*******************************************************************
** 文件名:	IActorMgr.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	2/23/2016
** 版  本:	1.0
** 描  述:	角色管理

********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "ActorDef.h"

#define ZoneServerMaxPersonNum 6000

struct __IPlayerRole;
struct IActorMgr
{
    // 创建角色  nReason:ELoginMode
    virtual UID createActor( SActorCreateContext* pContext, int nReason) = 0;

    // 通过udbid获取角色
    virtual SERVICE_PTR getActorByUDBID(UDBID udbid) = 0;

    // 通过pdbid获取角色
    virtual SERVICE_PTR getActorByPDBID(PDBID pdbid) = 0;

    // 通过uid获取角色
    virtual SERVICE_PTR getActorByUID(UID uid) = 0;

    // 通过clientID获取角色
    virtual SERVICE_PTR getActorByClientID(Gateway::ClientID client) = 0;

    // 获取角色总数
    virtual size_t getActorCount() = 0;

    //////////id转换//////////////
    virtual Gateway::ClientID UID2ClientID(UID uid) = 0;

    virtual PDBID UID2PDBID(UID uid) = 0;

    virtual UDBID UID2UDBID(UID uid) = 0;

    virtual UID ClientID2UID(Gateway::ClientID client) = 0;

    virtual UDBID ClientID2UDBID(Gateway::ClientID client) = 0;

    virtual PDBID ClientID2PDBID(Gateway::ClientID client) = 0;

    virtual UID PDBID2UID(PDBID pdbid) = 0;

    virtual void onCreateActor(UID uid, UDBID dwUserID, DWORD dwPDBID, int nSceneID, SERVICE_ID serviceID, DWORD dwClientID, bool isComputer) = 0;

    virtual void setClientID(PDBID idActor, ClientID client) = 0;
};

#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" IActorMgr * CreateActorManager();
#	define	CreateActorManagerProc	CreateActorManager
#else													/// 动态库版本
typedef IActorMgr * (RKT_CDECL * procCreateActorManager)();
#	define	CreateActorManagerProc	DllApi<procCreateActorManager>::load(MAKE_DLL_NAME(ENTITY), "CreateActorManager")
#endif