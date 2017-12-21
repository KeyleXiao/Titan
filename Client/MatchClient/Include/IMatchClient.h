/*******************************************************************
** �ļ���:	IMatchClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	2015/10/19  17:21
** ��  ��:	1.0
** ��  ��:	����ϵͳ�ͻ���
** Ӧ  ��:  		
	
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


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// ��̬����
#	pragma comment(lib, MAKE_LIB_NAME(MatchClient))
extern "C" IMatchClient * CreateMatchClient(void);
#	define	CreateMatchClientProc	CreateMatchClient
#	define	CloseMatchClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // ��̬����
#	pragma comment(lib, MAKE_DLL_LIB_NAME(MatchClient))
API_IMPORT IMatchClient * CreateMatchClient(void);
#	define	CreateMatchClientProc	CreateMatchClient
#	define	CloseMatchClientProc()
#else													// ��̬����
typedef IMatchClient * (RKT_CDECL *procCreateMatchClient)(void);
#	define	CreateMatchClientProc	DllApi<procCreateMatchClient>::load(MAKE_DLL_NAME(MatchClient), "CreateMatchClient")
#	define	CloseMatchClientProc	DllApi<procCreateMatchClient>::freelib
#endif