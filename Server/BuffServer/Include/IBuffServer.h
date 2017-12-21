/*******************************************************************
** 文件名:	IBuffServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/12  17:02
** 版  本:	1.0
** 描  述:	BUFF服务器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IEntity.h"
#include "BuffDef.h"
#include <map>
using namespace stdext;
#include "IEffect.h"

// 实体上的buff部件
struct __IBuffPart : public __IEntityPart
{
	/** 给实体添加buff前，需调用此方法检测
	@param   dwBuffID ：buffID
	@param   dwBuffLevel ：buff等级
	@param   uidAdd ：添加者
	@return  
	*/
	virtual bool			CanAdd(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd) = NULL;

	/** 给实体添加buff
	@param   dwBuffID ：buffID
	@param   dwBuffLevel ：buff等级
	@param   uidAdd ：添加者 
	@param   dwTime ：BUFF持续时间，填0表示取配置时间
	@param   pBuffContext ：BUFF数据
	@param   nContextLen ：BUFF数据长度
	@return  
	*/
	virtual bool			Add(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0) = NULL;

	/** 给实体移除buff
	@param   dwBuffID ：buffID
	@param   uidAdd ：添加者，不能把别人添加的移除掉;为INVALID_UID时，移除所有dwBuffID
	@param   uidRemove ：移除者
	@return  
	*/
	virtual bool			Remove(DWORD dwBuffID, UID uidAdd, UID uidRemove) = NULL;

	/** 给实体移除同类型的buff
	@param  dwBuffFlag ：buff类型 
	@param  bAllFlag ：是否移除所有的同类型的
	@param  uidRemove ：移除者
	@return  
	*/
	virtual bool			Remove(DWORD dwBuffType, bool bAllFlag, UID uidRemove) = NULL;

	/** 延迟移除buff
	@param   dwBuffIndex  Buff索引
	@param   dwTime		  延迟删除时间
	*/
	virtual void			DelayRemove(DWORD dwBuffIndex, DWORD dwTime) = NULL;

	/** 某个BUFF是否存在
	@param   
	@param   uidAdd ：如果不管添加者，则填INVALID_UID
	@return  
	*/
	virtual bool			IsExist(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd) = NULL;

	/** 某种BUFF是否存在
	@param   
	@param   
	@return  
	*/
	virtual bool			IsExist(DWORD dwBuffType) = NULL;

	/** 清除所有BUFF
	@param   
	@param   
	@return  
	*/
	virtual bool			Clean(void) = NULL;

	/** 右键取消某个buff
	@param   
	@param   
	@return  
	*/
	virtual bool			RightClickRemoveBuff(DWORD dwBuffIndex) = NULL;

	/** 创建初始化的BUFF，总是在创建部件时，创建BUFF，但此时很多部件没有创建，很容易非法
	@param   
	@param   
	@return  
	*/
	virtual bool			BuildInitBuff() = NULL;

	/** 移除对方添加的某种类型的Buff
	@param   dwBuffType ：Buff的类型，填0表示移除所有类型
	@param   uidAdd		：添加者，填0表示移除所有生物添加的
	@param   uidRemove	：移除者
	@return  
	*/
	virtual void			CleanBuff(DWORD dwBuffType, UID uidAdd, UID uidRemove) = NULL;

	/** 取玩家某个BUFF的等级(如果有多个相同ID的BUFF取等级最高的)
	@param   dwBuffID：buff的ID
	@param   uidAdd ：如果不管添加者，则填INVALID_UID   
	@return  
	*/
	virtual DWORD			GetBuffLevel(DWORD dwBuffID, UID uidAdd) = NULL;

	/** 取不能添加BUFF的原因（CanAdd后，采用这个函数可以取到具体原因）
	@param   
	@param   
	@return  
	*/
	virtual int				GetErrorID() = NULL;

	/** 剩余时间
	@param   dwBuffID：buff的ID
	@param   uidAdd ：如果不管添加者，则填INVALID_UID     
	@return －1：表示时间已越过，不用通知客户端创建。0：表示永久时间。正数表示剩余时间 
	*/
	virtual int				GetLeftTime(DWORD dwBuffID, UID uidAdd) = NULL;

	/** 某个effectID是否存在
	@param   dwEffectID:效果ID
	@param  lIgnoreUid:忽略的添加者
	@return  
	*/
	virtual bool			IsEffectIDExist(DWORD dwEffectID, UID lIgnoreUid) = NULL;

	/** 设置累积BUFF
	@param  dwBuffID:buffID
	@param  uidAdd ：如果不管添加者，则填INVALID_UID
	@param  bOverlay:true递增BUFF等级，直到buff等级上限，false递减buff等级，直到0
	*/
	virtual void			SetAccumulateBuff(DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0) = NULL;

	/** 索引移除buff
	@param   dwBuffIndex  Buff索引
	@param   dwTime		  延迟删除时间
	*/
	virtual bool            RemoveBuffByIndex(DWORD dwIndex)=NULL;

    /** 取得类型buff列表
    @param   dwBuffType ：buff类型
	@param   pBuffID ：输出buffID列表
    @param   pBuffLevel ：输出buffLevel列表
    @param   pBuffTime ：输出buffTime列表
	@param   nOutCount ：数量
	@return  
	*/
	virtual void			GetBuff(DWORD dwBuffType, DWORD *pBuffID, DWORD *pBuffLevel, int *pBuffTime, int &nOutCount) = NULL;

};

struct IBuffServer
{
	/** 创建
	*/
	virtual bool			create() = NULL;
    
	/** 执行Service的on_stop
	@return  
	*/
    virtual void            onStop(void) = NULL;

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

	/** 给实体创建buff部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual __IBuffPart		*CreatePart(void) = NULL;

	/** 取得buff名字
	@param   
	@param   
	@return  
	*/
	virtual const char     *GetBuffName(DWORD dwBuffID) = NULL;

	/** 取得buff回收池的信息
	@param   
	@param   
	@return  
	*/
	virtual void			GetBuffRecycle(int &nRecycleSize, int &nReleaseSize, int &nRestoreSize) = NULL;

	/** 取得所有配置信息
	@param   
	@param   
	@return  
	*/
	virtual TMAP_BUFFSCHEMEINFO *GetBuffSchemeInfo(void) = NULL;

	/** 取得buff配置信息
	@param   
	@param   
	@return  
	*/
	virtual SBuffSchemeInfo*		GetBuffSchemeInfo(DWORD dwBuffID) = NULL;

	/** 设置BUFF模块优化标志 (方便LUA刷)
	@param   
	@param   
	@return  
	*/
	virtual void					SetOptimizeBuffFlag(bool bOpen) = NULL;

	/** 获取加速查询索引
	@param
	@param
	@return
	*/
	virtual int				   getAccQueryIndex(DWORD dwBuffID, DWORD dwBuffLevel) = NULL;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(BUFFSERVER_STATIC_LIB)		/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(BuffServer))
	extern "C" IBuffServer * CreateBuffServer(void);
	#	define	CreateBuffServerProc	CreateBuffServer
#else													/// 动态库版本
	typedef IBuffServer * (RKT_CDECL *procCreateBuffServer)(void);
	#	define	CreateBuffServerProc	DllApi<procCreateBuffServer>::load(MAKE_DLL_NAME(BuffServer), "CreateBuffServer")
#endif