/*******************************************************************
** 文件名:	EntityClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体客户端模块: 处理实体相关的客户端逻辑
********************************************************************/


#ifndef __ENTITYCLIENT_ENTITYCLIENT_H__
#define __ENTITYCLIENT_ENTITYCLIENT_H__

#include "IMessageDispatch.h"
#include "IClientGlobal.h"
#include "buffer.h"
#include "IEntityClient.h"
#include "EntityFactory.h"

// 实体消息统计
#include "PerformanceStat.h"

class EntityClient : public IEntityClient, public IMessageHandler
{
public:
	/**
	@name   :构造函数
	*/
	EntityClient();

	/////////IEntityClient/////////////////////////////////////////////////////////////////
	/**
	@name   : 创建技能系统服务
	*/
	virtual bool Create();

	/**
	@name   :释放技能系统服务
	*/
	virtual bool Release();
	
	/**
	@name   :获取实体工厂接口
	*/
	virtual IEntityFactory* GetEntityFactory();
	
	/** 接受实体显示层发出的事件
	@param entityID   : 实体ID
	@param eventID    : 事件ID,具体定义见(EntityViewDef.h)
	@param arg1       : 时间参数1
	@param arg2       : 时间参数2
	@return           : 是否接收成功
	*/
	virtual bool onEntityEvent( ENTITY_ID entityID,int32 eventID,int nParam,const char * strParam,void * ptrParam );

	/** 接受显示层发出的事件
	@param entityID   : 实体ID
	@param eventID : 事件ID,具体定义见(GameViewDef.h)
	@param arg1    : 时间参数1
	@param arg2    : 时间参数2
	@return        : 是否接收成功
	*/
	virtual bool onGameViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

	/**
	@name   :订阅显示层事件处理器
	*/
	virtual bool registerGameViewEventHandler( int32 eventID, IGameViewEventHandler * pHandler);

	/**
	@name   :取消订阅显示层事件处理器
	*/
	virtual bool unregisterGameViewEventHandler( int32 eventID );

	/////////IMessageHandler/////////////////////////////////////////////////////////////////
	/** 消息处理
	@param head : 消息头，通常为一个消息包的前4个字节
    @param data : 消息的实际内容，除去了消息头的
	@param len  : 内容长度
	*/
	virtual void onMessage( SNetMsgHead* head, void* data, size_t len);

    // 判定id是否在m_EnemySet内
    virtual bool isHaveEnemyId( UID uId , bool bIsRemove = false);

private:
    void onGlobalEntity( BYTE msgId,UID uidMaster,ibuffer & in );

	// 创建实体
	IClientEntity * onCreateEntity(UID uidEntity, ibuffer & in);

	// 释放实体
	void onDestroyEntity(UID uidEntity, ibuffer & in);

    // 实体进入视野
    void OnEntityAppear(UID uidEntity, ibuffer & in);

    // 实体进入视野
    void OnEntityDisappear(UID uidEntity, ibuffer & in);

    // 实体进出草丛（以服务器为准，透明处理）
    void OnEntityForestChange(UID uidEntity, int nForestID);

    // 实体隐形、取消隐形（透明处理）
    void OnEntityStealthChange(UID uidEntity, bool bStealing);

private:
	// 显示层事件订阅器
	typedef std::map<int32,IGameViewEventHandler *>  GAMEEVENT_MAP;

	EntityFactory		m_EntityFactory;

	GAMEEVENT_MAP		m_GameViewEventMap;

    // 我我方视野内的敌方英雄Id
    std::set<UID>       m_EnemySet;
#ifdef PERFORMANCE_CLIENT_TEST
    CPerformanceStat  m_MsgStat;
    int               m_nMsgFlag;  
#endif
};

#endif	// __ENTITYCLIENT_ENTITYCLIENT_H__
