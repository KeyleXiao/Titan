/*******************************************************************
** 文件名: WarCheckBadBehavior.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/6/23
** 版  本: 1.0
** 描  述: 战场检测恶劣行为：挂机,消极游戏等
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#include "StdAfx.h"
#include "WarCheckBadBehavior.h"
#include "GameAPI.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "Vector3.h"
#include "WarMessageDef.h"
#include "Entity_inl.h"
#include "TimerAxis.h"

CWarCheckBadBehavior::CWarCheckBadBehavior(IEntity* pEntity)
	:m_pMaster(pEntity)
	,m_nBottingCount(0)
{
}

CWarCheckBadBehavior::~CWarCheckBadBehavior()
{
}

void CWarCheckBadBehavior::OnTimer(unsigned long dwTimerID)
{
	if (dwTimerID == ETimerEventID_CheckBotting)
	{
		// 记录玩家当前位置
		savePlayerLocation();

		// 每2分钟检测是否挂机
		if (m_LocationArray.size() == CHECK_BOTING_INTERNAL)
		{
			// 是否挂机(2次的位置是否在同一个地方)
			if (isBotting())
			{
				// 告诉服务器,玩家挂机
				SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_BOTTING_STATE, NULL, 0);
			}
			m_LocationArray.clear();
		}
	}
}

bool CWarCheckBadBehavior::Init(int nBottingCount)
{
	if (m_pMaster == NULL)
		return false;

	m_nBottingCount = nBottingCount;

	// 还没到挂机最大次数,继续检测是否挂机行为（这里主要是为断线重连考虑,刚进游戏时候,本局游戏的挂机次数一定为0）
	if (m_nBottingCount < BOTTING_COUNT_MAX)
	{
		// 设置定时器
		gClientGlobal->getTimerAxis()->SetTimer(ETimerEventID_CheckBotting, CHECK_BAD_BEHAVIOR_INTERNAL, this);
	}
	
	return true;
}

void CWarCheckBadBehavior::Release()
{
	// 删除定时器
	gClientGlobal->getTimerAxis()->KillTimer(ETimerEventID_CheckBotting, this);

	delete this;
}

bool CWarCheckBadBehavior::onCommand(int cmdid, void * data, size_t len)
{
	switch (cmdid)
	{
	case ENTITY_CMD_BOTTING_CANCEL:
		{
			// 从新开始记录位置信息
			m_LocationArray.clear();

			SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_BOTTING_CANCEL, NULL, 0);
			return true;
		}
	default:
		return false;
	}

	return false;
}

bool CWarCheckBadBehavior::onMessage(int msgID, void * data, size_t len)
{
	switch (msgID)
	{
		case SC_MSG_WAR_BOTTING_NOTIFY:
		{
			onMessageBottingNotify(data, len);
		}
		break;
	}
	return true;
}

void CWarCheckBadBehavior::savePlayerLocation()
{
	if (gClientGlobal == NULL)
		return;

	IClientEntity * pHero = gClientGlobal->getHero();
	if (pHero == NULL)
		return;

	Vector3 location = pHero->getLocation();

	m_LocationArray.push_back(location);
}

bool CWarCheckBadBehavior::isBotting()
{
	if (m_LocationArray.empty())
		return false;

	// 检测是否与原始位置相差在1米范围内
	Vector3 originLoc = m_LocationArray.front();
	m_LocationArray.pop_front();
	while (!m_LocationArray.empty())
	{
		Vector3 loc = m_LocationArray.front();
		m_LocationArray.pop_front();
		if (originLoc.getDistance(loc) > 1.0 )
			return false;
	}

	return true;
}

void CWarCheckBadBehavior::onMessageBottingNotify(void* data, size_t len)
{
	if (m_pMaster == NULL || data == NULL)
		return;

	if (len < sizeof(msg_war_botting_notify))
		return;

	msg_war_botting_notify* pMsg = (msg_war_botting_notify*)data;

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_BOTTING_NOTIFY, pMsg->byType, 0, NULL, NULL);
}
