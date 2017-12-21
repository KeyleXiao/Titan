/*******************************************************************
** 文件名:	EntityDieState.cpp 
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
#include "StdAfx.h"
#include "EntityDieState.h"
#include "SpellMessageDef.h"

/** 构造函数
@return  
*/
CEntityDieState::CEntityDieState(void)
{
	// 主人
	m_pMaster = NULL;

    m_dwStartTick = 0;
}

/** 析构函数
@return  
*/
CEntityDieState::~CEntityDieState(void)
{
	// 主人
	m_pMaster = NULL;
}

/** 创建
@return  
*/ 
bool CEntityDieState::create(__IEntity * pEntity)
{
	if(pEntity == NULL)
	{
		return false;
	}

	// 主人
	m_pMaster = pEntity;

	return true;
}

/** 取得本状态
@return  
*/
DWORD CEntityDieState::getStateID(void)
{
	return ENTITY_STATE_DIE;
}

/** 启动状态
@return  
*/	
void CEntityDieState::start(void)
{
    m_dwStartTick = getTickCountEx();
}

/** 结束状态
@return  
*/	
void CEntityDieState::end(void)
{
}

/** 否决消息，用于过滤掉当前状态下实体不用处理的消息
@return
*/ 
bool CEntityDieState::onMessage(SNetMsgHead * pMsgHead, SMsgEntityHead* pEntityMsgHead, void * data, size_t len)
{
	switch(pEntityMsgHead->byPartID)
	{
	case ENTITY_PART_MAX:	// 非部件实体处理消息
		{

		}
		break;

	default:	// 部件处理消息
		{
			// 生物死亡时只能接收复活消息和下线消息，其他的部件消息过滤掉
			// 增加七龙珠部件的复活消息
            // 增加小地图消息
            // 死亡可以处理AOI消息
			return (
                        pEntityMsgHead->byPartID == PART_DIE
                        || (pEntityMsgHead->byPartID == PART_LOGOUT)
                        || (pEntityMsgHead->byPartID == PART_SPELL) || (pEntityMsgHead->byPartID == PART_MOBAGOODS)
				        || (pEntityMsgHead->byPartID == PART_DRAGONBALL && pMsgHead->byKeyAction == CS_MSG_ENTITY_TEST_RELIVE) || (pEntityMsgHead->byPartID == PART_CHAT)
                        ||(pEntityMsgHead->byPartID == PART_WAR)
                        || (pEntityMsgHead->byPartID == PART_AOI)
                        ||(pEntityMsgHead->byPartID == PART_PROPERTY && pMsgHead->bySrcEndPoint==MSG_ENDPOINT_SCENE)
                        || (pEntityMsgHead->byPartID == PART_MOVE && pMsgHead->byKeyAction == SS_MSG_FORBID_MOVE_INITIATIVELY)
                    );
		}
		break;
	}
	
	return true;
}
