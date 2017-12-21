/*******************************************************************
** 文件名:	IBuffClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/15  21:17
** 版  本:	1.0
** 描  述:	BUFF 客户端
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IEntity.h"
#include "BuffDef.h"
class CBuffPart;
struct EFFECT_CONTEXT;

struct IBuff
{
	/** 
	@param   
	@param   
	@return  
	*/
	virtual bool			Create(CBuffPart *pBuffPart, DWORD dwIndex, DWORD dwBuffID, DWORD dwBuffLevel, DWORD dwTime, EFFECT_CONTEXT *pContext=NULL) = NULL;

	/** 释放所有资源，并且销毁此对像
	@param   
	@param   
	@return    	
	*/
	virtual void			Release(void) = NULL;

	/** 启动效果
	@param   
	@param   
	@return  
	*/
	virtual bool			Start(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual bool			Stop(void) = NULL;

	/** buff id
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetBuffID(void) = NULL;

	/** 等级
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetLevel(void) = NULL;

    /** 标志
    @param   
    @param   
    @return  
    */
    virtual DWORD            GetFlag(void) = NULL;

	/** 剩余时间，外面要自已处理时间倒数,如果返回0,表示永久状态
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetLeftTime(void) = NULL;

	/** 图标ID
	@param   
	@param   
	@return  
	*/
	virtual int				GetIconID(void) = NULL;

	/** buff名字
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetBuffName(void) = NULL;

	/** buff描述
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetDesc(void) = NULL;

		/** 取得效果数量
	@param   
	@param   
	@return  
	*/
	virtual int				GetEffectCount(void) = NULL;

	/** 取得效果ID
	@param   
	@param   
	@return  
	*/
	virtual int				GetEffectIDByIndex(int index) = NULL;

	/** 取得总时间
	@param   
	@param   
	@return  
	*/
	virtual int				GetTotalTime(void) = NULL;

	/** 判断某个效果是否存在
	@param   
	@param   
	@return  
	*/
	virtual bool			IsEffectIDExist(int nEffectID) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetIndex(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetFlashID(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual EFFECT_CONTEXT	*GetEffectContext(void) = NULL;
};


// 实体上的buff部件
struct IBuffPart : public IEntityPart
{
	/** 通过buff index取得buff
	@param   
	@param   
	@return  
	*/
	virtual IBuff *			GetBuff(DWORD dwBuffIndex) = NULL;

	/** 某个BUFF是否存在
	@param   
	@param  
	@return  
	*/
	virtual bool			IsExist(DWORD dwBuffID) = NULL;

	/** 获得所有BUFF的index
	@param   pBuffIndex ：buff index的数组
	@param   nCount ：IN = 数组大小，OUT = buff数
	@return  成功：true, 失败：false，表示数组过小
	*/
	virtual bool			GetAllBuff(DWORD * pBuffIndex, int &nCount) = NULL;

	/** 右键取消某个buff
	@param   
	@param   
	@return  
	*/
	virtual bool			RightClickRemoveBuff(int nBuffIndex) = NULL;

	/** 通过buff id取得buff
	@param   
	@param   
	@return  
	*/
	virtual IBuff *			GetBuffByID(DWORD dwBuffID) = NULL;

	/** 某个effectID是否存在
	@param   
	@param  
	@return  
	*/
	virtual bool			IsEffectIDExist(int nEffectID) = NULL;

    virtual void            restore(){};
};

// BUFF 客户端
struct IBuffClient
{
	/** 创建
	*/
	virtual bool			create() = NULL;

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
	virtual IBuffPart *		CreatePart(void) = NULL;

	/** 取得某个buff的描述
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetBuffDesc(int nBuffID, int nBuffLevel) = NULL;

	/** 取得buff名字
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetBuffName(DWORD dwBuffID) = NULL;

	/** 取得buff图标ID
	@param dwBuffID: BUFF的ID  
	@return  
	*/
	virtual DWORD			GetBuffIconID(DWORD dwBuffID) = NULL;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"
#if defined(_LIB) || defined(BUFFCLIENT_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(BuffClient))
	extern "C" IBuffClient * CreateBuffClient();
#	define	CreateBuffClientProc	CreateBuffClient
#else													/// 动态库版本
	typedef IBuffClient * (RKT_CDECL * procCreateBuffClient)();
#	define	CreateBuffClientProc	DllApi<procCreateBuffClient>::load(MAKE_DLL_NAME(BuffClient), "CreateBuffClient")
#endif