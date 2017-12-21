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
bool CEntityFightState::create(__IEntity * pEntity)
{
	if(pEntity == NULL)
	{
		return false;
	}

	// 主人
	m_pMaster = pEntity;

	return true;
}

/** 取得本状态ID
@return  
*/
DWORD CEntityFightState::getStateID(void)
{
	return ENTITY_STATE_FIGHT;
}

/** 启动状态
@return  
*/	
void CEntityFightState::start(void)
{

}

/** 结束状态
@return  
*/	
void CEntityFightState::end(void)
{

}

/** 否决消息，用于过滤掉当前状态下实体不用处理的消息
@return  
*/ 
bool CEntityFightState::onMessage(int msgId, SNetMsgHead * pMsgHead, void * data, size_t len)
{
	return true;
}


