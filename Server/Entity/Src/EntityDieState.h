/*******************************************************************
** 文件名:	EntityDieState.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-12
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

// 死亡状态
class CEntityDieState : public CEntityState
{
public:
	/** 创建
	@return  
	*/ 
	virtual bool				create(__IEntity * pEntity);

	/** 取得本状态ID
	@return  
	*/
	virtual DWORD				getStateID(void);

	/** 否决消息，用于过滤掉当前状态下实体不用处理的消息
	@return  
	*/ 
	virtual bool				onMessage(SNetMsgHead * pMsgHead, SMsgEntityHead* pEntityMsgHead, void * data, size_t len);

	/** 启动状态
	@return  
	*/	
	virtual void				start(void);

	/** 结束状态
	@return  
	*/	
    virtual void				end(void);

    virtual DWORD               getStartTick(){ return m_dwStartTick; }

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

private:
	/** 
	@param   
	@param   
	@return  
	*/
	bool						OnVetoMessage(SNetMsgHead * pGameMsgHead, LPCSTR pszMsg, int nLen);

    DWORD   m_dwStartTick;
};