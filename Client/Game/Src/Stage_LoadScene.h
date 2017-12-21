/*******************************************************************
** 文件名:	Stage_LoadScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	
** 描  述:	因为游戏需要统一进战场，这个状态用于控制加载界面的显示隐藏
            显示层当进入这个状态时显示加载界面 离开时隐藏
********************************************************************/
#pragma once

#include "Stage_Base.h"
#include "IMessageDispatch.h"
#include "MigrateDef.h"
#include "IEntityClient.h"

class Stage_LoadScene : 
    public Stage_Base,
    public IGameViewEventHandler,
    public IMessageHandler
{
public :
    Stage_LoadScene();

    ///////////////////////////Stage_Base////////////////////////////////////////////////////
	// 进入状态
	virtual void onEnter( void * context,int len );

    // 离开状态
    virtual void onLeave();

	// 逻辑更新
	virtual void update();

    ///////////////////////////IGameViewEventHandler////////////////////////////////////////

    virtual bool onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam );

    ///////////////////////////IMessageHandler//////////////////////////////////////////////
    /** 消息处理
	@param head : 消息头，通常为一个消息包的前4个字节
    @param data : 消息的实际内容，除去了消息头的
	@param len  : 内容长度
	*/
	virtual void onMessage( SNetMsgHead* head,void* data,size_t len );

private:
    // 发送场景加载进度
    void sendSceneResouceLoadProgress(int nProgress);

    // 地图ID
    int m_nMapID;

    PDBID m_idActor;
};
