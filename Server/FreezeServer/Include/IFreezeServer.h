/*******************************************************************
** 文件名:	e:\Rocket\Server\FreezeServer\Include\IFreezeServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/27  15:37
** 版  本:	1.0
** 描  述:	冷却服务器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

/*! \addtogroup IFreezeServer
*  冷却服务器
*  @{
*/

struct __IFreezePart;
struct SMiscBuildContext;

// 冷却服务器
struct IFreezeServer
{    
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
	
	/** 给实体创建冷却部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual __IFreezePart *	CreatePart(void) = NULL;

	
	/** 
	@param   
	@param   
	@return  
	*/
	virtual int  GetSchemeFreezeTime(DWORD dwClassID, DWORD dwFreezeID) = NULL;
};


// 冷却部件
struct __IFreezePart : public __IEntityPart
{
	/** 某个操作工作时，验证能否工作
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Skill
	@param   dwFreezeID ：冷却ID
	@param   
	@return  可以工作，返回true,否则返回false
	*/
	virtual bool			CanWork(DWORD dwClassID, DWORD dwFreezeID) = NULL;

	/** 操作开始工作，内部会自动冷却
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Skill
	@param   dwFreezeID ：冷却ID
	@param   
	@param   成功返回true,否则返回false
	@return  
	*/
	virtual bool			Work(DWORD dwClassID, DWORD dwFreezeID, SMiscBuildContext* pMiscContext=NULL) = NULL;

	/** 取消某个冷却，但无法取消公共冷却
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Skill
	@param   dwFreezeID ：冷却ID
	@param   bSync      ：是否需要同步到客户端
	@param   成功返回true,否则返回false   
	@return  
	*/
	virtual bool			Cancel(DWORD dwClassID, DWORD dwFreezeID, bool bSync = false) = NULL;

	/** 清除所有冷却(只是清内存)
	@param   
	@param   
	@return  
	*/
	virtual bool			Clean(void) = NULL;

	/** 清除某一类CD
	@param   
	@param   
	@return  
	*/
	virtual bool			Clean(DWORD dwClassID) = NULL;

	/** 增加或者减少冷却时间
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Skill
	@param   dwFreezeID ：冷却ID  
	@param   nTime		：增加或者减少冷却时间(ms)
	@return  
	*/
	virtual bool			AddFreezeTime(DWORD dwClassID, DWORD dwFreezeID, int nTime) = NULL;

	/** 打断技能增加或者减少冷却时间
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Skill
	@param   dwFreezeID ：冷却ID  
	@param   nTime		：增加或者减少冷却时间(ms)
	@return  
	*/
	virtual bool			AddSpellBreakFreezeTime(DWORD dwClassID, DWORD dwFreezeID, int nTime) = NULL;

	/** 增加或者减少冷却时间按百分比
	@param	 bChangeClassID : 减少ClassID所有冷却
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Skill
	@param   dwFreezeID ：冷却ID  
	@param   nRate		：增加或者减少冷却时间比例
	@return  
	*/
	virtual bool			AddRateFreezeTime(bool bChangeClassID, DWORD dwClassID, DWORD dwFreezeID, float fRate) = NULL;

	/** 强制设置冷却时间
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却ID  
	@param   nTime		：设置冷却时间(ms)
    @param   nIsResetCurFreeze ：是否清除其余干扰冷却（比如上次剩余时间或者已过时间）从而正确只设置nTime的冷却时间
	@return  
	*/
	virtual bool			SetFreezeTime(DWORD dwClassID, DWORD dwFreezeID, int nTime,bool nIsCleanInterferedFreeze=false,bool bBreak=false) = NULL;

	/** 获取冷却剩余时间
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却ID  
	@return  
	*/
	virtual DWORD			GetFreezeLeftTime(DWORD dwClassID, DWORD dwFreezeID) = NULL;

	/** 重置所有冷却为0，清CD
	@param   
	@param   
	@return  
	*/
	virtual bool			CleanAllFreeze(void) = NULL;

    /** 获取冷却时间
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却ID  
	@return  
	*/
	virtual DWORD			GetFreezeTime(DWORD dwClassID, DWORD dwFreezeID) = NULL;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(FREEZESERVER_STATIC_LIB)	/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(FreezeServer))
	extern "C" IFreezeServer * CreateFreezeServer(void);
	#	define	CreateFreezeServerProc	CreateFreezeServer
#else													/// 动态库版本
	typedef IFreezeServer * (RKT_CDECL *procCreateFreezeServer)(void);
	#	define	CreateFreezeServerProc	DllApi<procCreateFreezeServer>::load(MAKE_DLL_NAME(FreezeServer), "CreateFreezeServer")
#endif
