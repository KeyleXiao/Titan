/*******************************************************************
** 文件名:	IEntity.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体接口
********************************************************************/


#pragma once

#include "ISerializable.h"
#include "EntityDef.h"

#define   ENTITY_CLIENT

class Vector3;
struct IEntity;

enum EntityDestroyReason
{
    DESTROY_REASON_NORMAL,
    DESTROY_REASON_DIE,
};


// 实体部件接口，继承该接口可以挂接到实体上
struct IEntityPart : public ISerializable
{
    virtual bool onSerialize( rkt::ibuffer & in,int flag ) { return true; }

    virtual bool onSerialize( UID uid, rkt::ibuffer & in,int flag ) { return true; } 

	virtual short  getPartID() = 0;

	virtual bool onLoad( IEntity* pEntity) = 0;

	// 消息通讯
	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len) = 0;

	// 显示层的命令
	virtual bool onCommand( int cmdid,void * data,size_t len ) = 0;

	virtual IEntity * getHost() = 0;

	virtual void release() = 0;

    virtual void restore() = 0;
};

struct SNetMsgHead;
// 实体对象,生物，玩家等从该接口继承
struct IEntity : public ISerializable
{
	virtual bool onSerialize( rkt::ibuffer & in,int flag ) { return true; } 

	virtual bool onSerialize( UID uid, rkt::ibuffer & in,int flag ) = 0;

	// 创建实体
	virtual bool create(UID uid,SEntiyExportShadowContext &shadowData,void * context,int len) = 0;

	// 取得实体ID
	virtual UID getUID() const = 0;

	// 取得实体类型掩码,比如即是生物，又是玩家，说明该对象可转成ICreature，也可以转成IActor
	virtual ENTITY_MASK getTypeMask() = 0;

    virtual bool		isHero() const = 0;

	// 取得实体名称
	virtual const char * getName() const  = 0;

	// 取得当前所在地图ID
	virtual int getMapID() const = 0;

	// 取得实体位置
	virtual Vector3 getLocation() const =0;

	// 取得实体朝向
	virtual Vector3 getAngle() const = 0;

	// 取得实体部件
	virtual IEntityPart * getEntityPart(ENTITY_PART id) = 0;

	// 发送命令,命令与事件不同，命令是外界通知实体做某件事情，实体做了，再发出事件
	virtual void sendCommand( int cmdid,void * data,int len ) = 0;

	// 处理实体消息
	virtual void handMessage(SNetMsgHead* head, void * pEntityHead, BYTE byPartID, int msgId, void * data,int len ) = 0;

	// 触发实体事件
	virtual void fireEvent( int eventID,void *data,int len ) = 0;

	// 实体发出事件前的投票,用来支持其他模块对某个事件进行否决
	virtual bool fireVote( int eventID,void *data,int len ) = 0;

    // 销毁
    virtual void destroy(EntityDestroyReason eReason = DESTROY_REASON_NORMAL, void* pParam = NULL) = 0;

    // 更新数据到显示层
    virtual void updateView( bool bVisible ,bool bUseTranSport = true) = 0;

	// 发送命令到显示层实体,用于封装getRenderView()->sendEntityCommand
	virtual void sendCommandToEntityView(int cmdID, int nParam, const char* strParam, void* ptrParam, int len) = 0;

    // 是否在视野内
    virtual bool isOnView() = 0;
};