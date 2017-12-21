/*******************************************************************
** 文件名:	IFreezeClient.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/28  21:15
** 版  本:	1.0
** 描  述:	冷却客户端
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __COMMON_I_FREEZECLIENT_H__
#define __COMMON_I_FREEZECLIENT_H__


#include "IEntity.h"


struct SMiscBuildContext;

// 冷却部件
struct IFreezePart : public IEntityPart
{
	/** 某个操作工作时，验证能否工作
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却ID
	@param   
	@return  可以工作，返回true,否则返回false
	*/
	virtual bool			CanWork(DWORD dwClassID, DWORD dwFreezeID) = NULL;

	/** 操作开始工作，内部会自动冷却
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却ID
	@param   
	@param   成功返回true,否则返回false
	@return  
	*/
	virtual bool			Work(DWORD dwClassID, DWORD dwFreezeID, SMiscBuildContext* pMisContext=NULL) = NULL;

	/** 清除所有冷却
	@param   
	@param   
	@return  
	*/
	virtual bool			Clean(void) = NULL;

	/** 取得冷却已过了多长时间了，如果没有冷却或者过了冷却，返回0
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却ID  
	@return 
	*/
	virtual int				GetFreezeInfo(DWORD dwClassID, DWORD dwFreezeID, int &nTotalTime) = NULL;


	/** 外部修改冷却时间
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却ID  
	@return 
	*/
	virtual int				GetChangeTime(DWORD dwClassID, DWORD dwFreezeID) = NULL;

	/** 清除某个冷却，注意：但不能清除本冷却所影响到的公共冷却
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却I  
	@param   成功返回true,否则返回false
	*/
	virtual bool			Clean(DWORD dwClassID, DWORD dwFreezeID, bool bClearGroup = false) = NULL;

	/** 获取冷却剩余时间
	@param   dwClassID  ：冷却类ID，诸如：EFreeze_ClassID_Spell
	@param   dwFreezeID ：冷却ID  
	@return  
	*/
	virtual DWORD			GetFreezeLeftTime(DWORD dwClassID, DWORD dwFreezeID) = NULL;
};

// 冷却客户端
struct IFreezeClient
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

	/** 给实体创建冷却部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual IFreezePart *	CreatePart(void) = NULL;

	/** 取得冷却时间
	@param   
	@param   
	@return  返回-1，表示不用冷却
	*/

	virtual int				GetFreezeTime(DWORD dwClassID, DWORD dwFreezeID) = NULL;

	/** 取得客户端显示用途冷却时间
	@param   
	@param   
	@return  返回0，表示不用冷却
	*/
	virtual int				GetFreezeShowTime(DWORD dwClassID, DWORD dwFreezeID) = NULL;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// 静态链接
	#	pragma comment(lib, MAKE_LIB_NAME(FreezeClient))
	extern "C" IFreezeClient * CreateFreezeClient(void);
	#	define	CreateFreezeClientProc	CreateFreezeClient
	#	define	CloseFreezeClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(FreezeClient))
API_IMPORT IFreezeClient * CreateFreezeClient(void);
#	define	CreateFreezeClientProc	CreateFreezeClient
#	define	CloseFreezeClientProc()
#else													// 动态加载
	typedef IFreezeClient * (RKT_CDECL *procCreateFreezeClient)(void);
	#	define	CreateFreezeClientProc	DllApi<procCreateFreezeClient>::load(MAKE_DLL_NAME(FreezeClient), "CreateFreezeClient")
	#	define	CloseFreezeClientProc	DllApi<procCreateFreezeClient>::freelib
#endif


#endif	// __COMMON_I_FREEZECLIENT_H__