/*******************************************************************
** 文件名:	EntityState.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-12
** 版  本:	1.0
** 描  述:	生物状态接口
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IEntity.h"
#include "NetMessageDef.h"
#include "EntityDef.h"
#include "WarMessageDef.h"
// 生物状态接口
class CEntityState
{
public:
	/** 创建
	@return  
	*/ 
	virtual bool				create(__IEntity * pEntity)
	{
		return false;
	}

	/** 取得本状态ID
	*/
	virtual DWORD				getStateID(void)	
	{
		return ENTITY_STATE_UNKNOW;
	}

	/** 否决消息，用于过滤掉当前状态下实体不用处理的消息
	@return  
	*/ 
	virtual bool				onMessage(SNetMsgHead * pMsgHead, SMsgEntityHead* pEntityMsgHead, void * data, size_t len)
	{
		return true;
	}


	/** 启动状态
	@return  
	*/	
	virtual void				start(void)
	{

	}

	/** 结束状态
	@return  
	*/	
	virtual void				end(void)
	{

	}

    virtual DWORD               getStartTick(){ return 0; }

	/** 
	@param   
	@param   
	@return  
	*/
	CEntityState(void)
	{
		// 主人
		m_pMaster = NULL;
	}

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CEntityState(void)
	{
		// 主人
		m_pMaster = NULL;
	}	

protected:
	// 主人
	__IEntity *			m_pMaster;
};