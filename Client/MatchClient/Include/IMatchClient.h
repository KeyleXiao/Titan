/*******************************************************************
** 文件名:	IMatchClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	比赛系统客户端
** 应  用:  		
	
*******************************************************************/

#pragma once

#include "stdafx.h"
#include "MatchDef.h"
#include "MatchMsgDef.h"
#include "SchemeDef.h"
#include "MatchSchemeDef.h"
#include "EntityViewDef.h"

struct IMatchClient
{
	
	virtual void                release(void) = NULL;

    virtual void                enterHall(void * context,int len) = NULL;

    virtual SMsgCreateRoom_OC*     getRoomData(DWORD RoomID) = NULL;

    virtual uint32              getCampPlayerCountMax( DWORD dwMatchType) = NULL;

    virtual uint32              getPlayerCountMax(DWORD dwMatchType) =NULL;

	virtual uint32				getObCountMax(DWORD dwMatchType) = NULL;

    virtual void	            make(SMsgCreateRoom_OC* pRoom, cmd_match_hall_add_room& cmd) =NULL;

    virtual SMapSchemeInfo*		getSchemeMap(DWORD MapID) =NULL;

    virtual SSchemeMatchRoom*	getSchemeMatchRoomByMatchType(DWORD dwMatchType)=NULL;

    virtual char const*			getTextInfo( int nID )=NULL;

    virtual BYTE	            getCampCount(DWORD dwMatchType) = NULL;

    virtual void	            loadSenece(DWORD dwMapID, Vector3 vecBornPos) = NULL;

    virtual DWORD	            getCurRoomMatchType() = NULL;

    virtual int	                getCurRoomType() = NULL;

    virtual DWORD	            getCurRoomID() = NULL;

};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(MatchClient))
extern "C" IMatchClient * CreateMatchClient(void);
#	define	CreateMatchClientProc	CreateMatchClient
#	define	CloseMatchClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(MatchClient))
API_IMPORT IMatchClient * CreateMatchClient(void);
#	define	CreateMatchClientProc	CreateMatchClient
#	define	CloseMatchClientProc()
#else													// 动态加载
typedef IMatchClient * (RKT_CDECL *procCreateMatchClient)(void);
#	define	CreateMatchClientProc	DllApi<procCreateMatchClient>::load(MAKE_DLL_NAME(MatchClient), "CreateMatchClient")
#	define	CloseMatchClientProc	DllApi<procCreateMatchClient>::freelib
#endif