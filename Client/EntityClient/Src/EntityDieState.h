/*******************************************************************
** 文件名:	EntityDieState.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-13
** 版  本:	1.0
** 描  述:	死亡状态
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "EntityState.h"

class CEntityDieState : public CEntityState
{
public:
	/** 创建
	@return  
	*/ 
	virtual bool				create(IEntity * pMaster);

	/** 取得本状态
	*/
	virtual DWORD				getState(void);

	/** 消息过滤
	@return  
	*/ 
	virtual bool				onMessage(BYTE byPartID, int msgId,  void * data, size_t len);

	/** 启动状态
	@param   
	@param   
	@return  
	*/	
	virtual void				start(void);

	/** 结束状态
	@param   
	@param   
	@return  
	*/	
	virtual void				end(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CEntityDieState(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CEntityDieState(void);
};