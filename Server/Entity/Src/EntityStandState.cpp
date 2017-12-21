/*******************************************************************
** 文件名:	EntityStandState.cpp
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

#include "StdAfx.h"
#include "EntityStandState.h"

/** 构造函数
@return  
*/
CEntityStandState::CEntityStandState(void)
{
	// 主人
	m_pMaster = NULL;
}

/** 析构函数
@return  
*/
CEntityStandState::~CEntityStandState(void)
{
	// 主人
	m_pMaster = NULL;
}

/** 创建
@return  
*/ 
bool CEntityStandState::create(__IEntity * pEntity)
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
DWORD CEntityStandState::getStateID(void)
{
	return ENTITY_STATE_STAND;
}

/** 启动状态
@return  
*/	
void CEntityStandState::start(void)
{

}

/** 结束状态
@return  
*/	
void CEntityStandState::end(void)
{

}

/** 否决消息，用于过滤掉当前状态下实体不用处理的消息
@return  
*/ 
bool CEntityStandState::onMessage(int msgId, SNetMsgHead * pMsgHead, void * data, size_t len)
{
	return true;
}


