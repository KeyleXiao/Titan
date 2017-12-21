﻿/*******************************************************************
** 文件名:	EntityStandState.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-12
** 版  本:	1.0
** 描  述:	站立状态
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "EntityState.h"

class CEntityStandState : public CEntityState
{
public:
	/** 创建
	@param   
	@param   
	@return  
	*/ 
	virtual bool				create(__IEntity * pEntity);

	/** 取得本状态ID
	@param   
	@param   
	@return  
	*/
	virtual DWORD				getStateID(void);

	/** 消息过滤
	@param   
	@param   
	@return  
	*/ 
	virtual bool				onMessage(int msgId, SNetMsgHead * pMsgHead, void * data, size_t len);

	/** 启动状态
	@return  
	*/	
	virtual void				start(void);

	/** 结束状态
	@return  
	*/	
	virtual void				end(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CEntityStandState(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CEntityStandState(void);
};