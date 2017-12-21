/*******************************************************************
** 文件名:	EntityStandState.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-13
** 版  本:	1.0
** 描  述:	生物状态接口
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
bool CEntityStandState::create(IEntity * pMaster)
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
DWORD CEntityStandState::getState(void)
{
	return ENTITY_STATE_STAND;
}

/** 消息过滤
@param   
@param   
@return  
*/ 
bool CEntityStandState::onMessage(BYTE byPartID, int msgId,  void * data, size_t len)
{
	return true;
}

/** 启动状态
@param   
@param   
@return  
*/	
void CEntityStandState::start(void)
{

}

/** 结束状态
@param   
@param   
@return  
*/	
void CEntityStandState::end(void)
{

}

