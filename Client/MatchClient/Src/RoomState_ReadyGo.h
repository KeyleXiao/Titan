/*******************************************************************
** 文件名:	E:\speednew\Client\Game\Src\RoomState_ReadyGo.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/29/2015	19:39
** 版  本:	1.0
** 描  述:	
            所有玩家确认英雄 准备进入战斗
********************************************************************/

#pragma once

#include "IState.h"
#include "BaseRoomState.h"
#include "CurrentMatchRoomInfo.h"
//////////////////////////////////////////////////////////////////////////



class RoomState_ReadyGo : public BaseRoomState, public IGameViewEventHandler
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
	RoomState_ReadyGo(CurrentMatchRoomInfo* pRoom);
	virtual ~RoomState_ReadyGo();

private:
	//场景服发下来消息
	void onMsgAllConfirmHero( SNetMsgHead* head, void* data, size_t len );
	//场景服发下来消息
	void onMsgHeroSkinChange( SNetMsgHead* head, void* data, size_t len );
    //响应同步XP技能消息
    void onMsgSynAllHeroXPSkill( SNetMsgHead* head, void* data, size_t len );
    // 英雄同步数据
    void onMsgHeroWinDataInfo( SNetMsgHead* head, void* data, size_t len );

	// 显示层发上来的命令
	void onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT( int nParam, const char * strParam, void * ptrParam );
	void onGVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN( int nParam, const char * strParam,void * ptrParam );
};


//////////////////////////////////////////////////////////////////////////
