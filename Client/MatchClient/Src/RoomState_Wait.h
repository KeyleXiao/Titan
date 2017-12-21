/*******************************************************************
** 文件名:	E:\speednew\Client\Game\Src\StateRoom_Wait.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/24/2015	11:19
** 版  本:	1.0
** 描  述:	
            房间状态 等待中
********************************************************************/

#pragma once

#include "IState.h"
#include "BaseRoomState.h"
#include "CurrentMatchRoomInfo.h"
#include "Intl.h"

//////////////////////////////////////////////////////////////////////////



class RoomState_Wait : public BaseRoomState, public IGameViewEventHandler
{

public:
	///IState///////////////////////////////////////////////////////////////////////
	virtual void onEnter();
	virtual void onLeave();
    virtual void restoreState();
    virtual void release();

	///BaseRoomState///////////////////////////////////////////////////////////////////////
	// 场景服消息
	virtual void	onServerMsg(SNetMsgHead* head, void* data, size_t len);

	// 客户端显示层消息
	virtual bool	onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

public:
	RoomState_Wait(CurrentMatchRoomInfo* pRoom);
	virtual ~RoomState_Wait();

private:
    void onMsgRoomOBRole( SNetMsgHead* head, void* data, size_t len );
	void onMsgRoomRole( SNetMsgHead* head, void* data, size_t len );
	void onMsgRoomStartGameFail( SNetMsgHead* head, void* data, size_t len );
	void onMsgKickOutPlayer( SNetMsgHead* head, void* data, size_t len );
	void onMsgPosChange( SNetMsgHead* head, void* data, size_t len );
	void onMsgObPosChange( SNetMsgHead* head, void* data, size_t len );
    void onMsgComputerHeroChange( SNetMsgHead* head, void* data, size_t len );
    void onMsgComputerHeroCanSelect( SNetMsgHead* head, void* data, size_t len);
    void onMsgComputerGlobleBanHeroList( SNetMsgHead* head, void* data, size_t len);
    void onMsgPreHeroChange( SNetMsgHead* head, void* data, size_t len );
	void onMsgLegendCupWaitCountdown( SNetMsgHead* head, void* data, size_t len );
    void onMsgChangeAIIDSuccess( SNetMsgHead* head, void* data, size_t len );
    void onMsgCampHeroCanSelect( SNetMsgHead* head, void* data, size_t len );
    void onMsgPreSelectList( SNetMsgHead* head, void* data, size_t len );
    void onMsgBanHeroCountDown( SNetMsgHead* head, void* data, size_t len );
    void onMsgBanHeroFlag( SNetMsgHead* head, void* data, size_t len );
    void onMsgBroadSingleBanHero( SNetMsgHead* head, void* data, size_t len );
    void onMsgAlreadyBanHeroInfo( SNetMsgHead* head, void* data, size_t len );
    void onMsgLegendCupWinMsgInfo( SNetMsgHead* head, void* data, size_t len );

	void onGVIEWCMD_MATCH_START_GAME(int nParam,const char * strParam,void * ptrParam);
    void onGVIEWCMD_MATCH_MASTER_COVER_COMPUTER(int nParam,const char * strParam,void * ptrParam);
	void onGVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE( int nParam,const char * strParam,void * ptrParam );
	void onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT( int nParam, const char * strParam, void * ptrParam );
	void onGVIEWCMD_MATCH_ROOM_HERO_INFO( int nParam, const char * strParam, void * ptrParam );
	void onGVIEWCM_MATCH_MASTER_KICK_ROLE( int nParam, const char * strParam, void * ptrParam );
	void onGVIEWCMD_MATCH_FROM_GAME_TO_ROOM( int nParam, const char * strParam, void * ptrParam );
    void onGVIEWCMD_MATCH_FROM_GETHREO_DES( int nParam, const char * strParam, void * ptrParam );
    void onGVIEWCMD_MATCH_COMPUTERPLAYER_ADD(int nParam, const char * strParam, void * ptrParam);
    void onGVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE(int nParam, const char * strParam, void * ptrParam);
    void onGVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE(int nParam, const char * strParam, void * ptrParam);
    void onGVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE(int nParam, const char * strParam, void * ptrParam);
    void onGVIEWCMD_MATCH_ROOM_PRE_SELECT_HERO( int nParam, const char * strParam,void * ptrParam );
    void onGVIEWCMD_MATCH_ROOM_BAN_HERO( int nParam, const char * strParam,void * ptrParam );
    
};


//////////////////////////////////////////////////////////////////////////
