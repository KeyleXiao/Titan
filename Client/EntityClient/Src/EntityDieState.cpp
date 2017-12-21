/*******************************************************************
** 文件名:	EntityDieState.cpp 
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

#include "StdAfx.h"
#include "EntityDieState.h"


/** 
@param   
@param   
@return  
*/
CEntityDieState::CEntityDieState(void)
{
	// 主人
	m_pMaster = NULL;
}

/** 
@param   
@param   
@return  
*/
CEntityDieState::~CEntityDieState(void)
{
	// 主人
	m_pMaster = NULL;
}


/** 创建
@param   
@param   
@return  
*/ 
bool CEntityDieState::create(IEntity * pMaster)
{
	if(pMaster == NULL)
	{
		return false;
	}

	// 主人
	m_pMaster = pMaster;

	return true;
}

/** 取得本状态
@return  
*/
DWORD CEntityDieState::getState(void)
{
	return ENTITY_STATE_DIE;
}

/** 消息过滤
@return  
*/ 
bool CEntityDieState::onMessage(BYTE byPartID, int msgId,  void * data,size_t len)
{
	return true;
}

/** 启动状态
@param   
@param   
@return  
*/	
void CEntityDieState::start(void)
{

}

/** 结束状态
@param   
@param   
@return  
*/	
void CEntityDieState::end(void)
{

}
