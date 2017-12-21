/*******************************************************************
** 文件名:	EntityFightState.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-04-20
** 版  本:	1.0
** 描  述:	战斗状态
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "StdAfx.h"
#include "EntityFightState.h"


/** 构造函数
@return  
*/
CEntityFightState::CEntityFightState(void)
{
	// 主人
	m_pMaster = NULL;
}

/** 析构函数
@return  
*/
CEntityFightState::~CEntityFightState(void)
{
	// 主人
	m_pMaster = NULL;
}

/** 创建
@return  
*/ 
bool CEntityFightState::create(IEntity * pMaster)
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
DWORD CEntityFightState::getState(void)
{
	return ENTITY_STATE_FIGHT;
}

/** 消息过滤
@param   
@param   
@return  
*/ 
bool CEntityFightState::onMessage(BYTE byPartID, int msgId,  void * data, size_t len)
{
	return true;
}

/** 启动状态
@param   
@param   
@return  
*/	
void CEntityFightState::start(void)
{

}

/** 结束状态
@param   
@param   
@return  
*/	
void CEntityFightState::end(void)
{

}

