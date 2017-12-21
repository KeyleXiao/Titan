/*******************************************************************
** 文件名:	Stage_Game.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	6/7/2015
** 版  本:	1.0
** 描  述:	在静态地图下状态管理

********************************************************************/

#pragma once

#include "Stage_Base.h"
#include "IMessageDispatch.h"
#include "EntityViewDef.h"
#include "IPathFindService.h"
#include "MatchDef.h"
#include "OrderDef.h"
#include "MatchSchemeDef.h"
#include "SchemeDef.h"
#include "IEntityProperty.h"

class Stage_Game : public Stage_Base,public IGameViewEventHandler,public IMessageHandler
{
    

public:
    Stage_Game();
    ~Stage_Game();
	virtual void onEnter( void * context,int len );
	virtual void update();
	virtual void onLeave();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void OnConnected( IConnection * conn );

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	virtual bool onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

	////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void onMessage( SNetMsgHead* head, void* data, size_t len);

    virtual bool findPath(void* pCmd);

public:

    // 登陆服消息
    //void onUpdateSelectActor( void* data,size_t len );

    void checkDisConnect();

private:
   

    IPathFindService * m_pPathFind;
};
