#pragma once

#include "Stage_Base.h"
#include "IMessageDispatch.h"
#include "EntityViewDef.h"
#include "IPathFindService.h"

class Stage_MatchBattle : public Stage_Base,public IGameViewEventHandler
{
public:
    Stage_MatchBattle() : m_pPathFind(0)
    {
    }

	virtual void onEnter( void * context,int len );
	virtual void update();
	virtual void onLeave();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void OnConnected( IConnection * conn );

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	virtual bool onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

    virtual bool findPath(void* pCmd);

private:

	// 离开游戏
	void onViewSendExitGameMsg();
	// 离开战场
	void onViewSendExitWarMsg();
    // 战场结束返回静态地图
    void onViewSendBackGameMsg();


private:
    IPathFindService * m_pPathFind;
};
