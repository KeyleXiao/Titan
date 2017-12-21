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
#include "GameDef.h"

#include "EntityDef.h"
#include "buffer.h"

class Vector3;
struct __IEntity;
struct __IEntityEvent;
struct IBelongEntityMgr;

/** 
   实体部件接口，继承该接口可以挂接到实体上

   注意：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：
   1.每个玩家都是一个独立协程,所以Part的实现中如果需要访问全局数据，一定要注意线程安全
   2.一个玩家的多个Part直接互相调用是线程安全的，可以放心使用
*/
struct __IEntityPart : public ISerializable
{

	virtual short getPartID() = 0;

	virtual bool onLoad( __IEntity* pEntity, int nLoginMode) = 0;

	virtual __IEntity * getHost() = 0;

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len) = 0;

	virtual void release() = 0;

    // 切换英雄
    virtual void onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) = 0;

    virtual void restore() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
    内部实体对象接口: 使用该接口的必须明白实体的线程机制
	注意：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：
	1.每个玩家都是一个独立协程,所以Part的实现中如果需要访问全局数据，一定要注意线程安全
	2.一个玩家的多个Part直接互相调用是线程安全的，可以放心使用
*/
struct __IEntity : public ISerializable
{
	// 创建实体
	virtual bool create(UID uid,void * context,int len,int flag) = 0;
	
	// 释放
	virtual void release() = 0;

	// 销毁 : 因为智能指针会延迟销毁，所以有时候需要主动调用销毁函数
	virtual void destroy() = 0;

	// 取得实体ID
	virtual UID getUID() const = 0;

	// 取得实体类型
	virtual ENTITY_TYPE getType() = 0;

	// 取得实体名称
	virtual char * getName()  = 0;

	// 取得当前所在场景ID
	virtual int getSceneID() const = 0;

	// 取得当前所在地图ID
	virtual int getMapID() const = 0;

	// 取得实体位置
	virtual Vector3 getLocation() const =0;

	// 设置实体位置
	virtual void setLocation(Vector3 vLoc) =0;

	// 取得实体角度
	virtual Vector3 getAngle() const = 0;
	
	// 取得实体部件
	virtual __IEntityPart * getEntityPart(ENTITY_PART id) = 0;

	// 取得实体事件引擎
	virtual __IEntityEvent * getEntityEvent() = 0;

	// 处理消息
	virtual void handleMessage( void * data,int32 len ) = 0;

	// 改变当前状态 
	virtual bool changeState(int nState) = 0;

	// 取得当前状态 
	virtual int getState(void) = 0;  

	// 取得数值属性
	virtual int getIntProperty(int nPropID) = 0;

    // 设置数值属性
    virtual bool setIntProperty(int nPropID, int nValue) = 0;

	// 取得字符串属性
	virtual std::string getStringProperty(int nPropID) = 0;

	// 取得来源游戏世界ID
	virtual	int	getFromGameWorldID() = 0;

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect(int nEffectID, void * pAddContext, size_t len) = 0;

    // 移除效果
    virtual	void mobaGoodsRemoveEffect(int nID) = 0;

    // 获取附属实体管理对象
    virtual IBelongEntityMgr* getBelongEntityMgr(void) = 0;
};

struct IEntityFactory
{
	// 创建实体
	// @param type   : 实体类型
	// @param context: 创建现场
	// @param len    : 长度
	// @param flag   : 现场序列化标识，比如从数据库现场创建，从服务器发给客户端的现场创建
	virtual __IEntity * createEntity( ENTITY_TYPE type,void * context,int len,int flag ) = 0;

	// 根据UID查找实体
	virtual __IEntity * getEntity( UID uid) = 0;

	// 将这个实体对应的UID归还
	virtual void revertUID( UID uid ) = 0;
};
