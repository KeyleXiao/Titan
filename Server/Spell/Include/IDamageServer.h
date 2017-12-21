/*******************************************************************
** 文件名:	IDamageServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/26/2015
** 版  本:	1.0
** 描  述:	伤害服务
********************************************************************/

#ifndef __IDAMAGESERVER_H__
#define __IDAMAGESERVER_H__

#include "IEntity.h"
struct DamageCalcContext;
struct CureCalcContext;
struct CureContext;

// ================================================================================//
/** 
   实体部件接口，继承该接口可以挂接到实体上

   注意：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：
   1.每个玩家都是一个独立协程,所以Part的实现中如果需要访问全局数据，一定要注意线程安全
   2.一个玩家的多个Part直接互相调用是线程安全的，可以放心使用
*/
// ================================================================================//
struct __IDamagePart : public __IEntityPart
{
	/** 产生一次伤害
	*/
	virtual void onDamage(DamageCalcContext *pContext) = 0;

	/** 产生一次治疗
	*/
	virtual void onCure(CureContext *pContext) = 0;
};


// 伤害服务器
struct IDamageServer
{
	/** 创建伤害部件
	*/
	virtual __IDamagePart *crateDamagePart() = 0;

	/** 执行Service的on_stop
	@return  
	*/
    virtual void onStop(void) = 0;

	/** 释放资源
	*/
	virtual void release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(SPELL_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(Spell))
extern "C" IDamageServer * CreateDamageServer();
#	define	CreateDamageServerProc	CreateDamageServer
#else													/// 动态库版本
typedef IDamageServer * (RKT_CDECL * procCreateDamageServer)();
#	define	CreateDamageServerProc	DllApi<procCreateDamageServer>::load(MAKE_DLL_NAME(Spell), "CreateDamageServer")
#endif

#endif // __IDAMAGESERVER_H__