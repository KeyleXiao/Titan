/*******************************************************************
** 文件名:	IEntityClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/10/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/


#ifndef __ENTITYCLIENT_I_ENTITYCLIENT_H__
#define __ENTITYCLIENT_I_ENTITYCLIENT_H__


struct IEntityViewStruct;
struct IGameViewStruct;
struct IEntityFactory;
typedef  int ENTITY_ID ;
// 显示层事件处理器
struct IGameViewEventHandler
{
	virtual bool onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam ) = 0;
};

// 实体类服务
struct IEntityClient
{
	/**
	@name   : 创建技能系统服务
	*/
	virtual bool Create() = 0;

	/**
	@name   :释放技能系统服务
	*/
	virtual bool Release() = 0;
	
	/**
	@name   :获取实体工厂接口
	*/
	virtual IEntityFactory* GetEntityFactory() = 0;

	/** 接受实体显示层发出的事件
	@param entityID   : 实体ID
	@param eventID    : 事件ID,具体定义见(EntityViewDef.h)
	@param arg1       : 时间参数1
	@param arg2       : 时间参数2
	@return           : 是否接收成功
	*/
	virtual bool onEntityEvent( ENTITY_ID entityID,int32 eventID,int nParam,const char * strParam,void * ptrParam ) = 0;

	/** 接受显示层发出的事件
	@param entityID   : 实体ID
	@param eventID : 事件ID,具体定义见(GameViewDef.h)
	@param arg1    : 时间参数1
	@param arg2    : 时间参数2
	@return        : 是否接收成功
	*/
	virtual bool onGameViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam ) = 0;

	/**
	@name   :订阅显示层事件处理器
	*/
	virtual bool registerGameViewEventHandler( int32 eventID, IGameViewEventHandler * pHandler) = 0;

	/**
	@name   :取消订阅显示层事件处理器
	*/
	virtual bool unregisterGameViewEventHandler( int32 eventID ) = 0;


    // 判定id是否在m_EnemyVec内
    virtual bool isHaveEnemyId( UID uId , bool bIsRemove = false) = 0;

};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(EntityClient_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(EntityClient))
extern "C" IEntityClient * CreateEntityClient(void);
#	define	CreateEntityClientProc	CreateEntityClient
#else														/// 动态库版本
typedef IEntityClient * (RKT_CDECL *procCreateEntityClient)(void);
#	define	CreateEntityClientProc	DllApi<procCreateEntityClient>::load(MAKE_DLL_NAME(EntityClient), "CreateEntityClient")
#endif


#endif // __ENTITYCLIENT_I_ENTITYCLIENT_H__