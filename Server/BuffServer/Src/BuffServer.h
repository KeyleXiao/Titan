/*******************************************************************
** 文件名:	BuffServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/12  21:44
** 版  本:	1.0
** 描  述:	buff服务器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IBuffServer.h"
#include "SchemeBuff.h"
#include "ReplaceRule.h"
#include "BuffRecycle.h"

// 申明
class CBuff;

// 定义
class CBuffServer : public IBuffServer
{
public:
	///////////////////////IBuffServer/////////////////////////////////
	/** 创建
	*/
	virtual bool			create();

	/** 执行Service的on_stop
	@return  
	*/
    virtual void            onStop(void);

	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			release(void);

	/** 给实体创建buff部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual __IBuffPart*		CreatePart(void);

	/** 取得buff名字
	@param   
	@param   
	@return  
	*/
	virtual const char*			GetBuffName(DWORD dwBuffID);

	/** 取得buff回收池的信息
	@param   
	@param   
	@return  
	*/
	virtual void				GetBuffRecycle(int &nRecycleSize, int &nReleaseSize, int &nRestoreSize);

	/** 取得所有配置信息
	@param   
	@param   
	@return  
	*/
	virtual TMAP_BUFFSCHEMEINFO *	GetBuffSchemeInfo(void);

	/** 设置BUFF模块优化标志 (方便LUA刷)
	@param   
	@param   
	@return  
	*/
	virtual void					SetOptimizeBuffFlag(bool bOpen);

	virtual int					getAccQueryIndex(DWORD dwBuffID, DWORD dwBuffLevel);

	///////////////////////CBuffServer/////////////////////////////////
	/** 取得地图配置信息
	@param   
	@param   
	@return  
	*/
	SBuffSchemeInfo *		GetBuffSchemeInfo(DWORD dwBuffID);

	/** 此方法检测是否能够添加
    @param   pEntity ：实体指针
	@param   dwRule ：规则
	@param   dwBuffID ：buffID
	@param   dwBuffLevel ：buff等级
	@param   uidAdd ：添加者
	@param   uidEffect ：作用者
	@return  
	*/
	bool					CanAdd(__IEntity *pEntity, DWORD dwRule, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);

	/** 压入场景对像
	@param   
	@param   
	@return  
	*/
	void					PushToRecycle(CBuff * pBuff);

	/** 弹出场景对像
	@param   
	@param   
	@return  
	*/
	CBuff *					PopFromRecycle(void);

	/** 获取BUFF模块优化标志
	@param   
	@param   
	@return  
	*/
	bool					IsOptimizeBuffOpen();


	/** 
	@param   
	@param   
	@return  
	*/
	CBuffServer(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CBuffServer(void);

private:
	// 叠加规则
	CReplaceRule			m_ReplaceRule;

	// BUFF回收器
	CBuffRecycle			m_BuffRecycle;

	// BUFF优化标志
	bool					m_bOptimizeBuff;
};
extern CBuffServer *		g_pBuffServer;