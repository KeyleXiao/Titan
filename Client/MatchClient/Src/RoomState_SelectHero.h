/*******************************************************************
** 文件名:	E:\speednew\Client\Game\Src\StateRoom_Wait.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/24/2015	11:19
** 版  本:	1.0
** 描  述:	
            房间状态 选英雄
********************************************************************/

#pragma once

#include "IState.h"
#include "BaseRoomState.h"
#include "CurrentMatchRoomInfo.h"
//////////////////////////////////////////////////////////////////////////



class RoomState_SelectHero : public BaseRoomState, public IGameViewEventHandler
{

public:
	///IState///////////////////////////////////////////////////////////////////////
	virtual void onEnter();
	virtual void onLeave();
    virtual void restoreState();
    virtual void release();
	
	////////BaseRoomState////////////////////////////////////////////////////
	// 场景服消息
	virtual void	onServerMsg(SNetMsgHead* pHead, void* pData, size_t stLen);

	// 客户端显示层消息
	virtual bool	onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );
	

public:
	RoomState_SelectHero(CurrentMatchRoomInfo* pRoom);
	virtual ~RoomState_SelectHero();

private:
	// 服务器发下来的消息
	void onMsgRoomStartSelectHero( SNetMsgHead* head, void* data, size_t len);
	void onMsgStartRoomRule( SNetMsgHead* head, void* data, size_t len);
	void onMsgRoomRuleBanHero( SNetMsgHead* head, void* data, size_t len);
	void onMsgRoomRuleSelectHero( SNetMsgHead* head, void* data, size_t len);
	void onMsgBanHeroList( SNetMsgHead* head, void* data, size_t len);
	void onMsgHeroCanSelect( SNetMsgHead* head, void* data, size_t len);
	void onMsgGlobleBanHeroList( SNetMsgHead* head, void* data, size_t len);
    void onMsgAllBanHeroInfo( SNetMsgHead* head, void* data, size_t len );

	void onMsgHeroChange( SNetMsgHead* head, void* data, size_t len );
	void onMsgConfirmHero( SNetMsgHead* head, void* pData, size_t stLen );
    void onMsgComputerHeroChange( SNetMsgHead* head, void* data, size_t len );

	// 响应显示层选择英雄
	void onGVIEWCMD_MATCH_ROOM_HERO_CHANGE( int nParam, const char * strParam,void * ptrParam );

    // 响应显示层玩家退出
	void onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT( int nParam, const char * strParam, void * ptrParam );

    // 响应显示层确定选择英雄
	void onGVIEWCMD_MATCH_ROOM_CONFIRM_HERO( int nParam, const char * strParam, void * ptrParam );

    // 响应显示层英雄信息
	void onGVIEWCMD_MATCH_ROOM_HERO_INFO( int nParam, const char * strParam, void * ptrParam );

    // 响应显示层获取英雄信息
    void onGVIEWCMD_MATCH_FROM_GETHREO_DES( int nParam, const char * strParam, void * ptrParam );

    // 响应显示层召唤师技能
    void onGVIEWCMD_SUMMONER_SKILL_RETURN(int nParam, const char * strParam, void * ptrParam);
    
};


//////////////////////////////////////////////////////////////////////////
