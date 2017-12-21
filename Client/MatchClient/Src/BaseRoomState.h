/*******************************************************************
** 文件名:	E:\speednew\Client\Game\Src\BaseRoomState.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/24/2015	11:19
** 版  本:	1.0
** 描  述:	
            房间状态 基类
********************************************************************/

#pragma once


#include "IState.h"
#include "MatchDef.h"
#include "MessageDispatch.h"
#include "IEntityClient.h"
#include "Stage_ManagedDef.h"


class CurrentMatchRoomInfo;
struct SNetMsgHead;
//////////////////////////////////////////////////////////////////////////
class BaseRoomState : public IState
{
public:
	// 场景服消息
	virtual void	onServerMsg(SNetMsgHead* pHead, void* pData, size_t stLen);

	virtual bool	onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

	const char*		getStateTypeName(uint32 nType);
public:
	CurrentMatchRoomInfo* m_pRoom;

public:
	BaseRoomState(CurrentMatchRoomInfo* pRoom, uint32 u32ID);
	virtual ~BaseRoomState();

private:
	void onMsgChangeMaster( SNetMsgHead* head, void* data, size_t len );
	void onMsgExitRoomToHall( SNetMsgHead* head, void* data, size_t len );
	void onMsgKickOutRoomToHall( SNetMsgHead* head, void* data, size_t len );
	void onMsgLoadScene( SNetMsgHead* head, void* data, size_t len );
	void onMsgChangeRoomState( SNetMsgHead* head, void* data, size_t len );
	void onMsgReturnToRoom(SNetMsgHead* head, void* data, size_t len);
	void onMsgExitRoom( SNetMsgHead* head, void* data, size_t len );
    //void onMsgUpdateSummonerSkillInfo(SNetMsgHead* head, void* data, size_t len);
    void onMsgUpdateSummonerSkillInfoInRoom(SNetMsgHead* pHead, void* pData, size_t stLen);
	void onMsgExitHallFromLegendRoom( SNetMsgHead* head, void* data, size_t len );
    void onMsgExitHallFromMatchRoom( SNetMsgHead* head, void* data, size_t len );

private:
    // 在房间内将变动时召唤师技能显示出来
    void showSummonerSkillInfo(SMsgSummoner_Skill* pMsgInfo);
};
//////////////////////////////////////////////////////////////////////////
