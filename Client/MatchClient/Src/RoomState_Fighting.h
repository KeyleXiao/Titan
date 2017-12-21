/*******************************************************************
** 文件名:	E:\speednew\Client\Game\Src\StateRoom_Wait.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/24/2015	11:19
** 版  本:	1.0
** 描  述:	
            房间状态 战斗
********************************************************************/

#pragma once

#include "IState.h"
#include "BaseRoomState.h"
#include "CurrentMatchRoomInfo.h"
//////////////////////////////////////////////////////////////////////////

class RoomState_Fighting : public BaseRoomState, public IGameViewEventHandler
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
	RoomState_Fighting(CurrentMatchRoomInfo* pRoom);
	virtual ~RoomState_Fighting();

private:
	void onMsgRoomReadyToFight( SNetMsgHead* head, void* data, size_t len );

	void onGVIEWCMD_MATCH_READY_TO_FIGHT(int nParam, const char * strParam, void * ptrParam);
	void onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT(int nParam, const char * strParam, void * ptrParam);
    void onGVIEWCMD_MATCH_FROM_GETHREO_DES( int nParam, const char * strParam, void * ptrParam );
    // 处理需要在小地图上隐藏的地方英雄
    void onGVIEWCMD_ENEMY_ID_BARRIER( int nParam, const char * strParam, void * ptrParam );
};


//////////////////////////////////////////////////////////////////////////
