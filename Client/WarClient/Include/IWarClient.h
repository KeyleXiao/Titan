/*******************************************************************
** 文件名:	IWarClient.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/3/23  21:15
** 版  本:	1.0
** 描  述:	战场客户端
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __COMMON_I_WARCLIENT_H__
#define __COMMON_I_WARCLIENT_H__


#include "IEntity.h"
#include "SlotDef.h"

// 战场部件
struct IWarPart : public IEntityPart
{
	// 获得战场id
    virtual int GetWarID() = NULL;

	// 获取召唤师技能
	virtual int GetSummerID() = NULL;
};

// 战场物品部件
struct IMobaGoodsPart : public IEntityPart
{
	//槽位物品使用
	virtual void GoodsUse(SSlotGoodsInfo SlotGoodsInfo) = NULL;

	//槽位物品交换位置
	virtual void ExchangeSlot(int nSrcSlotIndex, int nTargetSlotIndex) = NULL;
};

// 冷却客户端
struct IWarClient
{
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			release(void) = NULL;

	/** 给实体创建战场部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual IWarPart *	CreatePart(void) = NULL;

	/** 给实体创建战场物品部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual IMobaGoodsPart *	CreateMobaGoodsPart(void) = NULL;

};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// 静态链接
	#	pragma comment(lib, MAKE_LIB_NAME(WarClient))
	extern "C" IWarClient * CreateWarClient(void);
	#	define	CreateWarClientProc	CreateWarClient
	#	define	CloseWarClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(WarClient))
API_IMPORT IWarClient * CreateWarClient(void);
#	define	CreateWarClientProc	CreateWarClient
#	define	CloseWarClientProc()
#else													// 动态加载
	typedef IWarClient * (RKT_CDECL *procCreateWarClient)(void);
	#	define	CreateWarClientProc	DllApi<procCreateWarClient>::load(MAKE_DLL_NAME(WarClient), "CreateWarClient")
	#	define	CloseWarClientProc	DllApi<procCreateWarClient>::freelib
#endif


#endif	// __COMMON_I_WARCLIENT_H__