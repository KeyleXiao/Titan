/*******************************************************************
** 文件名:	Stage_Choose.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/24/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "stdafx.h"
#include "Stage_Base.h"
#include "IMessageDispatch.h"

/**
	游戏状态 - 选择状态，负责选角色，根据所选角色相关参数进入游戏
*/
class Stage_Choose : public Stage_Base,public IGameViewEventHandler,public IMessageHandler
{
public:
	Stage_Choose();

	/////////////////////////////Stage_Base///////////////////////////////////////////////////////////////
	// 进入状态
	virtual void onEnter( void * context,int len );

    // 离开状态
    virtual void onLeave();

	// 逻辑更新
	virtual void update();
    
    ///////////////////////////IGameViewEventHandler////////////////////////////////////////
    virtual bool onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

    ///////////////////////////IMessageHandler//////////////////////////////////////////////
	virtual void onMessage( SNetMsgHead* head, void* data, size_t len);

	void selectHero( int nParam );

private:

    void onMsgUpdatePlayers( void * data,size_t len );

    void onMsgRecvActorName(void * data,size_t len);

    // 登陆服通知客户端信息
    void onMsgLoginNotifyClient(void * data, size_t len);

    // 请求进入游戏
    void requestEnterGame( const char* szActorName );

    // 返回登录状态
    void onCmdReturnLogin(void);

    // 请求随机玩家名字
    void onReqSearchActorName(void * data);


};
