/*******************************************************************
** 文件名:	PlayerKillPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/04/2016
** 版  本:	1.0
** 描  述:	PK部件实现
********************************************************************/

#include "StdAfx.h"
#include "PlayerKillPart.h"
#include "PlayerKillMessageDef.h"
#include "EntityHelper.h"
#include "ICamp.h"
#include "ExternalHelper.h"
#include "Event_ManagedDef.h"
#include "EventDef.h"
#include "PathFindHelper.h"
#include "IPathFindService.h"
#include "IBuffServer.h"
#include "PlayerBankPart.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "IEntityAOI.h"
#include "HeightFieldMgr.h"

/////////////////////////////////////////ISerializable/////////////////////////////////////////
bool CPlayerKillPart::onSerialize(rkt::ibuffer &in, int flag)
{
	return true;
}


bool CPlayerKillPart::deSerialize(rkt::obuf &out, int flag)
{
	return true;
}


/////////////////////////////////////////__IEntityPart/////////////////////////////////////////
/** 取得部件ID
*/
short CPlayerKillPart::getPartID(void)
{
	return PART_PK;
}


/** 载入部件
*/
bool CPlayerKillPart::onLoad(__IEntity *pEntity, int nLoginMode)
{
	m_pEntity = pEntity;

	if (m_pEntity == NULL)
	{
		return false;
	}

	// 默认设置阵营PK
	m_nPKType = PK_CAMP;
	m_nPKState = PK_STATE_NOMAL;
	m_dwPKStartTime = 0;
	m_uidPKMonster = 0;

	// 注册自己死亡消息
	g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DIE, __FILE__);
	// 注册自己登出消息
	g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_PREPARE_LOGOUT, __FILE__);
	// 注册切换英雄投票消息
	g_EHelper.Subscibe(m_pEntity, static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CHANGE_HERO, __FILE__);
	// 注册准备施法施法技能
	g_EHelper.Subscibe(m_pEntity, static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PREPARE_CAST_SPELL, __FILE__);
	// 注册复活前事件
	g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_BEFORE_RELIVE, __FILE__);

	return true;
}


/** 取得部件实体
*/
__IEntity *CPlayerKillPart::getHost(void)
{
	return m_pEntity;
}


/** 部件消息
*/
bool CPlayerKillPart::onMessage(void *pEntityHead, int nMsgID, void *pData, size_t nLen)
{
	switch (nMsgID)
	{
	case PK_MSG_CHOOSE_TARGET:
		{
			onClientMessageChooseTarget(pData, nLen);
		}
		break;
	case PK_MSG_REQUEST_PK:
		{
			onMessageRequestPK(pData, nLen);
		}
		break;
	case PK_MSG_HANDLE_INFO:
		{
			onClientMessageHandleInfo(pData, nLen);
		}
		break;
	case PK_MSG_START_PK:
		{
			onMessageStartPK(pData, nLen);
		}
		break;
	case PK_MSG_END_PK:
		{
			onMessageEndPK(pData, nLen);
		}
		break;
	case PK_MSG_CANCEL_PK:
		{
			onMessageCancelPK(pData, nLen);
		}
		break;
	default:
		break;
	}
	return true;
}

void CPlayerKillPart::destroy()
{
	// 清除PK信息
	clearPKInfo();

	if (m_pEntity != NULL)
	{
		// 注销自己死亡消息
		g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DIE);
		// 注销自己登出消息
		g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_PREPARE_LOGOUT);
		// 注册切换英雄投票消息
		g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CHANGE_HERO);
		// 取消准备施法施法技能
		g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PREPARE_CAST_SPELL);
		// 注册复活前事件
		g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_BEFORE_RELIVE);
	}
	m_pEntity = NULL;
}

/** 部件释放
*/
void CPlayerKillPart::release(void)
{
	delete this;
}


/** 切换英雄
*/
void CPlayerKillPart::onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore)
{

}


/////////////////////////////////////////TimerHandler/////////////////////////////////////////
void CPlayerKillPart::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case PK_LOGIC_TIMER:
		{
			if (m_nPKState == PK_STATE_NOMAL || m_dwPKStartTime <= 0 || isInvalidUID(m_uidPKMonster))
			{
				g_EHelper.KillTimer(PK_LOGIC_TIMER, this);
				break;
			}

			// 检测状态改变
			checkStateChange();

			// 检测战斗范围
			checkFightDistance();

			// 检测战斗时间
			checkFightTime();
		}
		break;
	default:
		break;
	}
}


/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
/**
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return
@note
@warning
@retval buffer
*/
void CPlayerKillPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_DIE:
		{
			if (m_pEntity == NULL)
			{
				break;
			}

			if (pszContext == 0 || nLen != sizeof(event_entity_die))
			{
				break;
			}
			event_entity_die *pEntityDie = (event_entity_die *)pszContext;

			// 检测战斗死亡
			checkFightDie();
		}
		break;
	case EVENT_ACTOR_PREPARE_LOGOUT:
		{
			if (m_pEntity == NULL)
			{
				break;
			}

			// 检测战斗登出
			checkFightLogout();
		}
		break;
	case EVENT_ENTITY_BEFORE_RELIVE:
		{
			if (m_pEntity == NULL)
			{
				break;
			}

			if (pszContext == NULL || nLen != sizeof(SEntityReliveInfo))
			{
				break;
			}

			SEntityReliveInfo * pContext = (SEntityReliveInfo*)pszContext;

			if (pContext->nDiePKType != PK_FIGHT)
			{
				break;
			}

			// PK状态才设置复活点为死亡点
			Vector3 ptReborn = m_pEntity->getLocation();
			Vector3 vAngles = m_pEntity->getAngle();

			pContext->fAngleY = vAngles.y;
			pContext->x = ptReborn.x;
			pContext->y = ptReborn.y;
			pContext->z = ptReborn.z;
		}
		break;
	default:
		break;
	}
}

void CPlayerKillPart::forceEndPK()
{
	// 强制结束PK
	if (m_nPKType != PK_FIGHT)
	{
		return;
	}

	// 提示玩家决斗结束
	g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_LOGOUT);
	// 设置自己失败信息
	setPKResult(PK_RESULT_LOST, true);

	// 设置结束PK
	setEndPK(m_uidTarget, m_pEntity->getUID(), PK_RESULT_WIN, "玩家强制退出决斗,决斗结束");
	// 清除PK信息
	clearPKInfo();
}

/**
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  如果返回false : 则中断执行，否则继续向下执行
@note
@warning
@retval buffer
*/
bool CPlayerKillPart::OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_CHANGE_HERO:
		{
			// PK状态不允许切换英雄
			if (m_nPKType == PK_FIGHT)
			{
				return false;
			}
		}
		break;

	case EVENT_ENTITY_PREPARE_CAST_SPELL:   // 准备施法施法技能
		{
			if (pszContext == NULL || nLen != sizeof(event_prepare_cast_spell))
			{
				return false;
			}
			event_prepare_cast_spell *pPrepareCastSpell = (event_prepare_cast_spell *)pszContext;

			int nEffecType = pPrepareCastSpell->nEffecType;
			int nAttackType = pPrepareCastSpell->nAttackType;

			// 响应NPC类技能、瞬发单体、飞行单体、随机选择、弹射校验下目标对象是否为不可攻击NPC
			if ((nEffecType & SpellEffectType_TouchNPC) == SpellEffectType_TouchNPC || nAttackType == INSTANT_SINGLE || nAttackType == FLY_SINGLE || nAttackType == RANDOM_SELECTION || nAttackType == RANDOM_CATAPULT)
			{
				return checkCanCastSpellOnMonster(pPrepareCastSpell->uidTarget);
			}
		}
		break;

	default:
		break;
	}

	return true;
}


/////////////////////////////////////////__IWingPart/////////////////////////////////////////
// 选择目标
bool CPlayerKillPart::chooseTarget(int nTargetFilter, UID uidTarget)
{
	if (m_pEntity == NULL)
	{
		return false;
	}

    // 所有类型
    if ((nTargetFilter&TARGET_ALL) == TARGET_ALL)
    {
        return true;
    }

    // 不可对AI使用 
    if ((nTargetFilter&TARGET_EXCLUDE_AI))
    {
        if(getProperty_Integer(uidTarget, PROPERTY_AI_SCHEME_ID) > 0)
        {
            return false;
        }
    }

	// 取得目标PK模式
	int nPKType = getEntityPKType(uidTarget);
	if (nPKType != m_nPKType)
	{
		return false;
	}

	// 取得目标立足点类型
	int nLocateType = getProperty_Integer(uidTarget, PROPERTY_FORMTYPE);
	if ((((nTargetFilter&TARGET_EXCLUDE_GROUND) && (nLocateType == FORM_GROUND))
		|| ((nTargetFilter&TARGET_EXCLUDE_HIGH_FLOOR) && (nLocateType == FORM_HATHPACE))
		|| ((nTargetFilter&TARGET_EXCLUDE_SKY) && (nLocateType == FORM_SKY))))
	{
		return false;
	}

	// 如果目标是NPC返回false
	if (UID_2_TYPE(uidTarget) == TYPE_MONSTER
		&& getProperty_Integer(uidTarget, PROPERTY_VOCATION) == MONSTER_SUB_TYPE_NPC)
	{
		return false;
	}

	switch (m_nPKType)
	{
	case PK_CAMP:
		{
			if (!checkCanCastSpellOnMonster(uidTarget))
			{
				return false;
			}

			UID uidOperator = m_pEntity->getUID();
			int nOperatorCamp = m_pEntity->getIntProperty(PROPERTY_CAMP);
			// 目标阵营
			int nTargetCamp = getProperty_Integer(uidTarget, PROPERTY_CAMP);
			// 取得怪物类型
			BYTE nMonsterType = (BYTE)getProperty_Integer(uidTarget, PROPERTY_SEX);
			BYTE nMonsterVoc = (BYTE)getProperty_Integer(uidTarget, PROPERTY_VOCATION);

			// 检测阵营
			return Camp::TestTargetCamp(nTargetFilter, uidOperator, nOperatorCamp, uidTarget, nTargetCamp, nMonsterType, nMonsterVoc);
		}
		break;
	case PK_FIGHT:
		{
			// 不是战斗状态
			if (m_nPKState != PK_STATE_FIGHT)
			{
				return false;
			}

			UID uidSelf = m_pEntity->getUID();

			int nSelfCamp = 0;
			int nTargetCamp = 0;
			if (isActor(uidSelf))
			{
				// 自己阵营
				CAMP_MAP::iterator Iter = m_CampMap.find(uidSelf);
				if (Iter == m_CampMap.end())
				{
					return false;
				}
				nSelfCamp = Iter->second;

				// 目标阵营
				Iter = m_CampMap.find(uidTarget);
				if (Iter == m_CampMap.end())
				{
					return false;
				}
				nTargetCamp = Iter->second;
			}
			else if (isMonster(uidSelf))
			{
				// 获取主人
				UID uidMaster = m_pEntity->getIntProperty(PROPERTY_MASTER_UID);
				if (uidMaster == INVALID_UID)
				{
					return false;
				}

				if (!isActor(uidMaster))
				{
					return false;
				}
				// 主人的阵营为怪物的阵营
				nSelfCamp = ::getPKCamp(uidMaster);


				// 目标阵营
				if (isActor(uidTarget))
				{
					nTargetCamp = ::getPKCamp(uidTarget);
				}
				else if (isMonster(uidTarget))
				{
					// 获取主人
					UID uidMaster = getProperty_Integer(uidTarget, PROPERTY_MASTER_UID);
					if (uidMaster == INVALID_UID)
					{
						return false;
					}

					if (!isActor(uidMaster))
					{
						return false;
					}
					// 主人的阵营为怪物的阵营
					nTargetCamp = ::getPKCamp(uidMaster);
				}
			}

			// 取得怪物类型
			BYTE nMonsterType = (BYTE)getProperty_Integer(uidTarget, PROPERTY_SEX);
			BYTE nMonsterVoc = (BYTE)getProperty_Integer(uidTarget, PROPERTY_VOCATION);
			// 检测阵营
			bool bTargetCamp = Camp::TestTargetCamp(nTargetFilter, uidSelf, nSelfCamp, uidTarget, nTargetCamp, nMonsterType, nMonsterVoc);

			// 是自己或者PK对象
			if (uidTarget == uidSelf || uidTarget == m_uidTarget)
				return bTargetCamp;

			UID uidMaster = INVALID_UID;
			UID uidTargetMaster = INVALID_UID;

			if (isMonster(uidSelf))
			{
				uidMaster = m_pEntity->getIntProperty(PROPERTY_MASTER_UID);
			}

			if (isMonster(uidTarget))
			{
				uidTargetMaster = getProperty_Integer(uidTarget, PROPERTY_MASTER_UID);
			}

			// 是自己的主人
			if (uidMaster != INVALID_UID && uidTarget == uidMaster)
				return bTargetCamp;

			// 是友方的召唤物
			if (uidMaster != INVALID_UID && uidTargetMaster != INVALID_UID && uidMaster == uidTargetMaster)
				return bTargetCamp;

			// 是敌方的召唤物
			if (uidTargetMaster != INVALID_UID && m_uidTarget == uidTargetMaster)
				return bTargetCamp;
		}
		break;
	default:
		break;
	}
	return false;
}

// 设置PK类型
void CPlayerKillPart::setPKType(int nType)
{
	if (nType <= PK_INVALID || nType >= PK_MAX)
	{
		return;
	}

	m_nPKType = nType;
}

// 获取PK类型
int CPlayerKillPart::getPKType(void)
{
	return m_nPKType;
}

// 设置决斗目标
void CPlayerKillPart::setFightTarget(UID uidTarget)
{
	m_uidTarget = uidTarget;

	if (!m_pEntity)
	{
		return;
	}
	__IEntityEvent * pEntityEvent = m_pEntity->getEntityEvent();
	if (!pEntityEvent)
	{
		return;
	}

	event_actor_pk_target_change ev;
	ev.uidTarget = m_uidTarget;
	pEntityEvent->FireExecute(EVENT_ACTOR_PK_TARGET_CHANGE, (LPCSTR)&ev, sizeof(ev));
}

// 检测决斗
bool CPlayerKillPart::checkPK(UID uidTarget)
{
	if (m_pEntity == NULL)
	{
		return false;
	}

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}

	ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
	if (pSchemePlayerKill == NULL)
	{
		return false;
	}

	SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
	if (pPlayerKill == NULL)
	{
		return false;
	}

	UID uidSelf = m_pEntity->getUID();
	if (uidSelf == uidTarget)
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_NO_CHOOSE_SELF);
		return false;
	}

	int nType = UID_2_TYPE(uidTarget);
	// 目标不是英雄
	if (nType != TYPE_PLAYER_ROLE)
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_NO_CHOOSE_TARGET);
		return false;
	}

	if (isInvalidUID(uidTarget))
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_NO_CHOOSE_TARGET);
		return false;
	}

	if (getEntityPKState(uidTarget) != PK_STATE_NOMAL)
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_ALREADY_PK_STATE);
		return false;
	}

	ActorHelper helper(uidTarget);
	IActorService *pActorService = helper.m_ptr;
	if (pActorService == NULL)
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_TARGET_NO_EXIST);
		return false;
	}

	int nSelfSceneID = m_pEntity->getSceneID();
	int nTargetSceneID = pActorService->getSceneID();
	if (nSelfSceneID != nTargetSceneID)
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_NO_SAME_SCENE);
		return false;
	}

	// 撮合状态不可PK
	if (pActorService->getGameState() == GAME_STATE_MATCH)
	{
		WarningLn(__FUNCTION__"(): CHAT_TIP_ENTITY_PK_PK_STATE_ERROR ! GAME_STATE_MATCH");
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_PK_STATE_ERROR);
		return false;
	}

	// 取得自己位置
	Vector3 vSelfLoc = m_pEntity->getLocation();
	// 取得目标位置
	Vector3 vTargetLoc = pActorService->getLocation();
	if (!vSelfLoc.checkDistance(vTargetLoc, (float)pPlayerKill->nDistance))
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_DISTANCE_FAR);
		return false;
	}

	return true;
}

// 获取PK状态
int CPlayerKillPart::getPKState(void)
{
	return m_nPKState;
}

// 设置PK状态
void CPlayerKillPart::setPKState(int nPKState)
{
	m_nPKState = nPKState;
}

int CPlayerKillPart::getPKCamp()
{
	UID uidSelf = m_pEntity->getUID();

	// 自己阵营
	CAMP_MAP::iterator iter = m_CampMap.find(uidSelf);
	if (iter != m_CampMap.end())
	{
		return iter->second;
	}

	return 0;
}

extern "C" __declspec(dllexport) __IPlayerKillPart *CreatePlayerKillPart()
{
	return new CPlayerKillPart;
}


/////////////////////////////////////////CPlayerKillPart/////////////////////////////////////////
/** 构造函数
*/
CPlayerKillPart::CPlayerKillPart(void)
{
	// PK部件实体初始化
	m_pEntity = NULL;
	// PK类型初始化
	m_nPKType = PK_INVALID;
	// PK状态初始化
	m_nPKState = PK_STATE_NOMAL;
	// PK开始时间初始化
	m_dwPKStartTime = 0;
	// PK创建怪物
	m_uidPKMonster = 0;
	// 上次请求时间
	m_dwLastRequestTime = 0;
	// 请求PK容器
	m_RequestPKVector.clear();
	// PK阵营记录
	m_CampMap.clear();
}

/** 析构函数
*/
CPlayerKillPart::~CPlayerKillPart(void)
{
}

// 客户端消息选择目标
void CPlayerKillPart::onClientMessageChooseTarget(void *pData, size_t nLen)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	if (pData == NULL || nLen != sizeof(SChooseTarget_CS))
	{
		return;
	}
	SChooseTarget_CS *pChooseTarget = (SChooseTarget_CS *)pData;

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
	if (pSchemePlayerKill == NULL)
	{
		return;
	}

	SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
	if (pPlayerKill == NULL)
	{
		return;
	}

	ISchemeSceneMatron *pSchemeSceneMatron = pSchemeCenter->getSchemeSceneMatron();
	if (pSchemeSceneMatron == NULL)
	{
		return;
	}

	// 禁止PK
	if (pSchemeSceneMatron->IsForbitPK(m_pEntity->getMapID()))
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_FORBIT_PK);
		return;
	}

	// 取得当前时间
	DWORD dwTick = getTickCountEx();
	if (dwTick <= m_dwLastRequestTime + pPlayerKill->nCoolTime)
	{
		char szParam[256] = { 0 };
		ssprintf_s(szParam, sizeof(szParam), "%d", (m_dwLastRequestTime + pPlayerKill->nCoolTime - dwTick) / 1000);
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_REQUEST_OFTEN, szParam);
		return;
	}

	UID uidTarget = pChooseTarget->uidTarget;
	if (!checkPK(uidTarget))
	{
		return;
	}

	// 设置上次请求时间
	m_dwLastRequestTime = dwTick;

	SRequestPK_SS requestPK;
	requestPK.uidOperator = m_pEntity->getUID();
	g_EHelper.sendEntityMessage(uidTarget, PART_PK, PK_MSG_REQUEST_PK, (char *)&requestPK, sizeof(requestPK));
}


// 请求决斗
void CPlayerKillPart::onMessageRequestPK(void *pData, size_t nLen)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
	if (pSchemePlayerKill == NULL)
	{
		return;
	}

	SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
	if (pPlayerKill == NULL)
	{
		return;
	}

	ISchemeSceneMatron *pSchemeSceneMatron = pSchemeCenter->getSchemeSceneMatron();
	if (pSchemeSceneMatron == NULL)
	{
		return;
	}

	// 禁止PK
	if (pSchemeSceneMatron->IsForbitPK(m_pEntity->getMapID()))
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_FORBIT_PK);
		return;
	}

	if (pData == NULL || nLen != sizeof(SRequestPK_SS))
	{
		return;
	}
	SRequestPK_SS *pRequestPK = (SRequestPK_SS *)pData;

	UID uidOperator = pRequestPK->uidOperator;
	if (!checkPK(uidOperator))
	{
		return;
	}

	ActorHelper helper(uidOperator);
	IActorService *pActorService = helper.m_ptr;
	if (pActorService == NULL)
	{
		return;
	}

	// 取得当前时间
	DWORD dwTick = getTickCountEx();
	REQUEST_PK_VECTOR::iterator Iter = m_RequestPKVector.begin();
	while (Iter != m_RequestPKVector.end())
	{
		// 移除超过时效的决斗请求
		if (dwTick >= Iter->dwTime + pPlayerKill->nRequestValidTime)
		{
			Iter = m_RequestPKVector.erase(Iter);
		}
		else
		{
			++Iter;
		}
	}

	Iter = m_RequestPKVector.begin();
	while (Iter != m_RequestPKVector.end())
	{
		if (Iter->uidOperator == uidOperator)
		{
			g_EHelper.showSystemMessage(uidOperator, CHAT_TIP_ENTITY_PK_TARGET_REPEAT_REQUEST);
			return;
		}
		else
		{
			++Iter;
		}
	}

	// 达到上限
	if ((int)m_RequestPKVector.size() >= pPlayerKill->nRequestValidCount)
	{
		g_EHelper.showSystemMessage(uidOperator, CHAT_TIP_ENTITY_PK_TARGET_BUSY);
		return;
	}

	SRequestPKInfo Info;
	Info.uidOperator = uidOperator;
	Info.dwTime = dwTick;
	ssprintf_s(Info.szInfo, sizeof(Info.szInfo), a2utf8("[%s]向你发起决斗！"), pActorService->getName().c_str());
	m_RequestPKVector.push_back(Info);

	// 发送请求PK
	sendRequestPK(Info);

	// 给发起者提示
	char szParam[256] = { 0 };
	ssprintf_s(szParam, sizeof(szParam), "%s", m_pEntity->getName());
	g_EHelper.showSystemMessage(uidOperator, CHAT_TIP_ENTITY_PK_REQUEST_TIP, szParam);
}


// 客户端消息操作信息
void CPlayerKillPart::onClientMessageHandleInfo(void *pData, size_t nLen)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemeSceneMatron *pSchemeSceneMatron = pSchemeCenter->getSchemeSceneMatron();
	if (pSchemeSceneMatron == NULL)
	{
		return;
	}

	// 禁止PK
	if (pSchemeSceneMatron->IsForbitPK(m_pEntity->getMapID()))
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_FORBIT_PK);
		return;
	}

	if (pData == NULL || nLen != sizeof(SHandInfo_CS))
	{
		return;
	}
	SHandInfo_CS *pInfo = (SHandInfo_CS *)pData;

	REQUEST_PK_VECTOR::iterator Iter = m_RequestPKVector.begin();
	while (Iter != m_RequestPKVector.end())
	{
		// 移除决斗玩家请求
		if (Iter->uidOperator == pInfo->uidOperator)
		{
			Iter = m_RequestPKVector.erase(Iter);
		}
		else
		{
			++Iter;
		}
	}

	UID uidSelf = m_pEntity->getUID();
	if (uidSelf == pInfo->uidOperator)
	{
		return;
	}

	ActorHelper helper(pInfo->uidOperator);
	IActorService *pActorService = helper.m_ptr;
	if (pActorService == NULL)
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_TARGET_OFFLINE);
		return;
	}

	if (!pInfo->bAgree)
	{
		/*char szParam[256]={0};
		if (pActorService != NULL)
		{
		ssprintf_s(szParam, sizeof(szParam), "%s", pActorService->getName().c_str());
		}
		g_EHelper.showSystemMessage(pInfo->uidOperator, CHAT_TIP_ENTITY_PK_NO_AGREE, szParam);*/

		SShowMessageBox_SC box;
		box.nType = 8;
		ssprintf_s(box.szCaption, sizeof(box.szCaption), a2utf8("决斗消息"));
		ssprintf_s(box.szText, sizeof(box.szText), a2utf8("[%s]拒绝了你的决斗请求"), m_pEntity->getName());
		g_EHelper.playerSend2Client(pInfo->uidOperator, PART_PK, PK_MSG_SHOW_MESSAGE_BOX, (char *)&box, sizeof(box));

		return;
	}

	// 检测决斗
	if (!checkPK(pInfo->uidOperator))
	{
		return;
	}

	if (!pActorService->checkPK(uidSelf))
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_PK_STATE_ERROR);
		return;
	}

	// 取得自己位置
	Vector3 vSelfLoc = m_pEntity->getLocation();
	Vector3 vTargetLoc = getLocation(pInfo->uidOperator);
	Vector3 vNewLoc = Vector3(vSelfLoc.x + (vTargetLoc.x - vSelfLoc.x) / 2, vSelfLoc.y + (vTargetLoc.y - vSelfLoc.y) / 2, vSelfLoc.z + (vTargetLoc.z - vSelfLoc.z) / 2);
	// 创建PK怪物
	if (!createPKMonster(vNewLoc))
	{
		ErrorLn("onClientMessageHandleInfo createPKMonster error");
		return;
	}

	m_CampMap[uidSelf] = CAMP_BLUE;
	m_CampMap[pInfo->uidOperator] = CAMP_RED;

	// 设置PK信息
	setPKInfo(pInfo->uidOperator, getTickCountEx());

	// 清空请求PK信息容器
	m_RequestPKVector.clear();

	SStartPK_SS startPK;
	startPK.uidTarget = uidSelf;
	startPK.nPKStartTime = m_dwPKStartTime;
	startPK.vNewLoc = vNewLoc;
	g_EHelper.sendEntityMessage(pInfo->uidOperator, PART_PK, PK_MSG_START_PK, (char *)&startPK, sizeof(startPK));

	SStartPK_SC startPK_SC;
	startPK_SC.uidTarget = pInfo->uidOperator;
	startPK_SC.vLoc = vNewLoc;
	g_EHelper.playerSend2Client(m_pEntity, PART_PK, PK_MSG_START_PK, (char *)&startPK_SC, sizeof(startPK_SC));
}


// 创建PK怪物
bool CPlayerKillPart::createPKMonster(Vector3 &vNewLoc)
{
	if (m_pEntity == NULL)
	{
		return false;
	}

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}

	ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
	if (pSchemePlayerKill == NULL)
	{
		return false;
	}

	SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
	if (pPlayerKill == NULL)
	{
		return false;
	}
	if (pPlayerKill->nMonsterID <= 0)
	{
		return false;
	}

	UID uidSelf = m_pEntity->getUID();
	// 场景ID
	int nSceneID = UID_2_SCENE(uidSelf);
	PathFindHelper PathHelper(nSceneID);
	IPathFindService *pPathService = PathHelper.m_ptr;
	if (pPathService == NULL)
	{
		return false;
	}
	// 找到地面的点
	pPathService->getHeight(&vNewLoc, vNewLoc.y);

	MonsterHelper helper(uidSelf);
	INPCService *pNpcService = helper.m_ptr;
	if (pNpcService == NULL)
	{
		return false;
	}

	CreateNpcContext NpcContext;
	NpcContext.nCamp = m_pEntity->getIntProperty(PROPERTY_CAMP);
	NpcContext.nWarID = getWarID(uidSelf);
	NpcContext.uidCreator = uidSelf;
	NpcContext.nLevel = 1;
	NpcContext.nPKType = m_nPKType;

	// 数据长度
	int nLen = sizeof(NpcContext);
	m_uidPKMonster = pNpcService->createNPC(pPlayerKill->nMonsterID, vNewLoc, NpcContext);

	if (isInvalidUID(m_uidPKMonster))
	{
		return false;
	}

	return true;
}


// 开始PK
void CPlayerKillPart::onMessageStartPK(void *pData, size_t nLen)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	if (pData == NULL || nLen != sizeof(SStartPK_SS))
	{
		return;
	}
	SStartPK_SS *pStartPK = (SStartPK_SS *)pData;

	UID uidSelf = m_pEntity->getUID();
	if (uidSelf == pStartPK->uidTarget)
	{
		return;
	}

	Vector3 vNewLoc = pStartPK->vNewLoc;
	// 创建PK怪物
	if (!createPKMonster(vNewLoc))
	{
		ErrorLn("onMessageStartPK createPKMonster error");
		return;
	}

	// 设置自己阵营
	m_CampMap[uidSelf] = CAMP_RED;
	m_CampMap[pStartPK->uidTarget] = CAMP_BLUE;

	// 设置PK信息
	setPKInfo(pStartPK->uidTarget, pStartPK->nPKStartTime);

	SStartPK_SC startPK_SC;
	startPK_SC.uidTarget = pStartPK->uidTarget;
	startPK_SC.vLoc = vNewLoc;
	g_EHelper.playerSend2Client(m_pEntity, PART_PK, PK_MSG_START_PK, (char *)&startPK_SC, sizeof(startPK_SC));
}


// 结束PK
void CPlayerKillPart::onMessageEndPK(void *pData, size_t nLen)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	if (pData == NULL || nLen != sizeof(SEndPK_SS))
	{
		return;
	}
	SEndPK_SS *pEndPK = (SEndPK_SS *)pData;

	// 不是决斗状态，不处理
	if (m_nPKType != PK_FIGHT)
	{
		return;
	}

	char szParam[256] = { 0 };

	if (pEndPK->nResult == PK_RESULT_LOST)
	{
		ssprintf_s(szParam, sizeof(szParam), ("%s,你被击败！"), pEndPK->szReason);
	}
	else if (pEndPK->nResult == PK_RESULT_WIN)
	{
		ssprintf_s(szParam, sizeof(szParam), ("%s,你获得胜利！"), pEndPK->szReason);
	}
	else
	{
		ssprintf_s(szParam, sizeof(szParam), ("%s,平局结束！"), pEndPK->szReason);
	}
	if (strlen(szParam) > 0)
	{
		g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_END_PK_PROMPT, a2utf8(szParam));
	}

	// 设置自己PK信息
	setPKResult(pEndPK->nResult);
	// 清除PK信息
	clearPKInfo();

	// 增加PK结束BUFF
	addPKBuff(false);
}


// 取消PK
void CPlayerKillPart::onMessageCancelPK(void *pData, size_t nLen)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	if (pData == NULL || nLen != sizeof(SCancelPK_CS))
	{
		return;
	}
	SCancelPK_CS *pCancelPK = (SCancelPK_CS *)pData;

	if (!pCancelPK->bCancel)
	{
		return;
	}

	// 不是决斗状态，不处理
	if (m_nPKType != PK_FIGHT)
	{
		return;
	}

	// 提示玩家决斗结束
	g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_CANCEL_PK);
	// 设置自己失败信息
	setPKResult(PK_RESULT_LOST);

	// 设置结束PK
	setEndPK(m_uidTarget, m_pEntity->getUID(), PK_RESULT_WIN, "玩家主动取消决斗,决斗结束");
	// 清除PK信息
	clearPKInfo();
}


// 设置PK信息
void CPlayerKillPart::setPKInfo(UID uidTarget, DWORD dwPKStartTime)
{
	// 设置PK类型
	m_nPKType = PK_FIGHT;
	// 设置决斗目标
	setFightTarget(uidTarget);
	// 设置战斗状态
	m_nPKState = PK_STATE_PREPARE;
	// 设置决斗开始时间
	m_dwPKStartTime = dwPKStartTime;
	// 启动PK逻辑定时器
	g_EHelper.SetTimer(PK_LOGIC_TIMER, PK_LOGIC_TIME, this, INFINITY_CALL, __FILE__);
}


// 清除PK信息
void CPlayerKillPart::clearPKInfo(void)
{
	// 设置PK类型
	m_nPKType = PK_CAMP;

	UID uidTarget = m_uidTarget;

	// 设置决斗目标
	setFightTarget(INVALID_UID);
	// 设置战斗状态
	m_nPKState = PK_STATE_NOMAL;
	// 设置决斗开始时间
	m_dwPKStartTime = 0;
	// 杀掉定时器
	g_EHelper.KillTimer(PK_LOGIC_TIMER, this);
	// 清除阵营信息
	m_CampMap.clear();

	// 删除创建怪物
	if (!isInvalidUID(m_uidPKMonster))
	{
		MonsterHelper helper(m_uidPKMonster);
		INPCService *pNpcService = helper.m_ptr;
		if (pNpcService != NULL)
		{
			pNpcService->destroyNPC(m_uidPKMonster);
		}
	}
	m_uidPKMonster = 0;

	if (m_pEntity)
	{
		// 把附属实体清空处理下
		IBelongEntityMgr * pBelongEntityMgr = m_pEntity->getBelongEntityMgr();
		if (pBelongEntityMgr)
		{
			pBelongEntityMgr->restore();
		}
	}
}

// 检测状态改变
void CPlayerKillPart::checkStateChange(void)
{
	switch (m_nPKState)
	{
	case PK_STATE_PREPARE:
		{
			ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
			if (pSchemeCenter == NULL)
			{
				break;
			}

			ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
			if (pSchemePlayerKill == NULL)
			{
				break;
			}

			SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
			if (pPlayerKill == NULL)
			{
				break;
			}

			// 取得当前时间
			DWORD dwTick = getTickCountEx();
			if (dwTick <= m_dwPKStartTime + pPlayerKill->nPrepareTime)
			{
				break;
			}

			if (m_nPKState == PK_STATE_PREPARE)
			{
				// 增加PK开始BUFF
				addPKBuff(true);
			}

			// 设置战斗状态
			m_nPKState = PK_STATE_FIGHT;
		}
		break;
	default:
		break;
	}
}

// 检测战斗范围
void CPlayerKillPart::checkFightDistance(void)
{
	// 不是决斗状态
	if (m_nPKType != PK_FIGHT)
	{
		return;
	}

	if (m_pEntity == NULL)
	{
		return;
	}

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
	if (pSchemePlayerKill == NULL)
	{
		return;
	}

	SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
	if (pPlayerKill == NULL)
	{
		return;
	}

	// 取得自己位置
	Vector3 vSelfLoc = m_pEntity->getLocation();
	// 取得怪物位置
	Vector3 vMonsterLoc = getLocation(m_uidPKMonster);
	// 调试发现取位置太快,没在findNPC里面发现,返回(0,0,0)
	if (equalto0(vMonsterLoc.getLength()))
	{
		return;
	}

	// 没有超过范围
	if (vSelfLoc.checkDistance(vMonsterLoc, (float)pPlayerKill->nFightDistance))
	{
		return;
	}

	// 提示玩家决斗结束
	g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_DISTANCE_FAR_FAIL);
	// 设置自己失败信息
	setPKResult(PK_RESULT_LOST);

	// 设置结束PK
	setEndPK(m_uidTarget, m_pEntity->getUID(), PK_RESULT_WIN, "玩家离开决斗范围,决斗结束");
	// 清除PK信息
	clearPKInfo();
}


// 检测战斗死亡
void CPlayerKillPart::checkFightDie(void)
{
	// 不是决斗状态
	if (m_nPKType != PK_FIGHT)
	{
		return;
	}

	// 提示玩家决斗结束
	g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_BY_KILL);
	// 设置自己失败信息
	setPKResult(PK_RESULT_LOST);

	// 设置结束PK
	setEndPK(m_uidTarget, m_pEntity->getUID(), PK_RESULT_WIN, "玩家被击杀,决斗结束");
	// 清除PK信息
	clearPKInfo();
}


// 检测战斗时间
void CPlayerKillPart::checkFightTime(void)
{
	// 不是决斗状态
	if (m_nPKType != PK_FIGHT)
	{
		return;
	}

	if (m_pEntity == NULL)
	{
		return;
	}

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
	if (pSchemePlayerKill == NULL)
	{
		return;
	}

	SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
	if (pPlayerKill == NULL)
	{
		return;
	}

	// 取得当前时间
	DWORD dwTick = getTickCountEx();
	if (dwTick <= m_dwPKStartTime + pPlayerKill->nFightTime)
	{
		return;
	}

	// 提示玩家决斗结束
	g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_TIME_OUT);
	// 设置自己决斗-平信息
	setPKResult(PK_RESULT_FLAT);

	// 设置结束PK
	setEndPK(m_uidTarget, m_pEntity->getUID(), PK_RESULT_FLAT, "超过战斗时间,决斗结束");
	// 清除PK信息
	clearPKInfo();
}


// 检测战斗登出
void CPlayerKillPart::checkFightLogout(void)
{
	// 不是决斗状态
	if (m_nPKType != PK_FIGHT)
	{
		return;
	}

	// 提示玩家决斗结束
	g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_LOGOUT);
	// 设置自己失败信息
	setPKResult(PK_RESULT_LOST);

	// 设置结束PK
	setEndPK(m_uidTarget, m_pEntity->getUID(), PK_RESULT_WIN, "玩家决斗中下线,决斗结束");
	// 清除PK信息
	clearPKInfo();
}


// 设置结束PK
void CPlayerKillPart::setEndPK(UID uidWin, UID uidLost, int nResult, char *szReason)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	if (szReason == NULL)
	{
		return;
	}

	// 告诉胜利者
	SEndPK_SS endPK;
	endPK.uidWin = uidWin;
	endPK.uidLost = uidLost;
	endPK.nResult = nResult;
	ssprintf_s(endPK.szReason, sizeof(endPK.szReason), szReason);
	g_EHelper.sendEntityMessage(uidWin, PART_PK, PK_MSG_END_PK, (char *)&endPK, sizeof(endPK));

	// 增加PK结束BUFF
	addPKBuff(false);

	do
	{
		ActorHelper WinHelper(uidWin);
		IActorService *pWinActorService = WinHelper.m_ptr;
		if (pWinActorService == NULL)
		{
			break;
		}

		ActorHelper LostHelper(uidLost);
		IActorService *pLostActorService = LostHelper.m_ptr;
		if (pLostActorService == NULL)
		{
			break;
		}

		char szParam[256] = { 0 };
		if (nResult == PK_RESULT_FLAT)
		{
			ssprintf_s(szParam, sizeof(szParam), "%d;%s;%s", CHAT_CHANNEL_ZONE, pWinActorService->getName().c_str(), pLostActorService->getName().c_str());
			g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_SCENE_FLAT, szParam, CHAT_BROADCASTTYPE_ZONE);
		}
		else
		{
			ssprintf_s(szParam, sizeof(szParam), "%d;%s;%s", CHAT_CHANNEL_ZONE, pWinActorService->getName().c_str(), pLostActorService->getName().c_str());
			g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_PK_SCENE_WIN, szParam, CHAT_BROADCASTTYPE_ZONE);
		}
	} while (false);
}


// 设置PK结果
void CPlayerKillPart::setPKResult(int nResult, bool bForceEnd)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	__IBankPart* pBankPart = (__IBankPart*)m_pEntity->getEntityPart(PART_BANK);
	if (pBankPart == NULL)
	{
		return;
	}
			

	SEndPK_SC endPK_SC;
	endPK_SC.uidTarget = m_uidTarget;
	endPK_SC.nResult = nResult;
	endPK_SC.bForceEnd = bForceEnd;
	g_EHelper.playerSend2Client(m_pEntity, PART_PK, PK_MSG_END_PK, (char *)&endPK_SC, sizeof(endPK_SC));

	// 服务器保存PK记录
	pBankPart->setActorBaseInfo(1, ENPLAYERINFOTYPE_PKTOTALNUM, ENCHANGEDATAMODE_ADD);
	if (PK_RESULT_WIN == nResult)
	{
		pBankPart->setActorBaseInfo(1, ENPLAYERINFOTYPE_PKWINNUM, ENCHANGEDATAMODE_ADD);
	}

	// 写文字日志,发送Lua PK信息
	do
	{
		IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
		if (pOSSLogServer == NULL)
		{
			break;
		}

		ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
		if (pSchemeCenter == NULL)
		{
			break;
		}

		ISchemePersonModel *pSchemePersonModel = pSchemeCenter->getSchemePersonModel();
		if (pSchemePersonModel == NULL)
		{
			break;
		}

		ActorHelper helper(m_uidTarget, ACTOR_UID);
		IActorService *pActorService = helper.m_ptr;
		if (pActorService == NULL)
		{
			break;
		}

		DWORD dwPDBID = m_pEntity->getIntProperty(PROPERTY_ID);
		int nVocation = m_pEntity->getIntProperty(PROPERTY_VOCATION);

		DWORD dwTargetPDBID = pActorService->getProperty_Integer(PROPERTY_ID);
		int nTargetVocation = pActorService->getProperty_Integer(PROPERTY_VOCATION);

		char szResult[32] = { 0 };
		if (nResult == PK_RESULT_LOST)
		{
			ssprintf_s(szResult, sizeof(szResult), "战斗失败");
		}
		else if (nResult == PK_RESULT_WIN)
		{
			ssprintf_s(szResult, sizeof(szResult), "战斗胜利");
		}
		else
		{
			ssprintf_s(szResult, sizeof(szResult), "战斗平");
		}

		// 算出耗时
		DWORD dwTime = (getTickCountEx() - m_dwPKStartTime) / 1000;

		ostrbuf logBuf;
		logBuf.reserve(512);
		logBuf << a2utf8("角色ID:") << dwPDBID << a2utf8(",角色名:") << m_pEntity->getName() << a2utf8(",使用英雄ID:") << nVocation <<
			a2utf8(",使用英雄名字:") << pSchemePersonModel->getHeroName(nVocation) << a2utf8(" 与 角色ID:") << dwTargetPDBID << a2utf8(",角色名:") << pActorService->getName().c_str() <<
			a2utf8(",使用英雄ID:") << nTargetVocation << a2utf8(",使用英雄名字:") << pSchemePersonModel->getHeroName(nTargetVocation) <<
			a2utf8("挑战,") << a2utf8(szResult) << a2utf8(",耗时:") << dwTime;
		pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, dwPDBID, logBuf.c_str());


		// 通知任务
		__IEntityEvent * pEntityEvent = m_pEntity->getEntityEvent();
		if (pEntityEvent != NULL)
		{
			event_game_actor_data_task eventdata;
			eventdata.nTaskLuaEvent = emTaskLuaEventType_PK;
			eventdata.dwPDBID = dwPDBID;
			eventdata.nParam1 = dwTargetPDBID;

			pEntityEvent->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
		}
	} while (false);
}


// 发送请求PK
void CPlayerKillPart::sendRequestPK(const SRequestPKInfo &Info)
{
	obuf ob;
	SRequestPK_SC requestPK;
	requestPK.nRequestCount = 1;
	ob.push_back(&requestPK, sizeof(requestPK));
	ob.push_back(&Info, sizeof(SRequestPKInfo));

	g_EHelper.playerSend2Client(m_pEntity, PART_PK, PK_MSG_REQUEST_PK, ob.data(), ob.size());
}

// 增加PKBUFF
void CPlayerKillPart::addPKBuff(bool bStart)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	__IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
	if (pBuffPart == NULL)
	{
		return;
	}

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
	if (pSchemePlayerKill == NULL)
	{
		return;
	}

	SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
	if (pPlayerKill == NULL)
	{
		return;
	}

	int nBuffID = 0;
	int nBuffLevel = 0;
	if (bStart)
	{
		nBuffID = pPlayerKill->nStartPKBuffID;
		nBuffLevel = pPlayerKill->nStartPKBuffLevel;
	}
	else
	{
		nBuffID = pPlayerKill->nEndPKBuffID;
		nBuffLevel = pPlayerKill->nEndPKBuffLevel;
	}

	if (nBuffID > 0 && nBuffLevel > 0)
	{
		// 增加BUFF
		pBuffPart->Add(nBuffID, nBuffLevel, m_pEntity->getUID());
	}
}

// 技能是否能作用于怪物上
inline bool CPlayerKillPart::checkCanCastSpellOnMonster(UID uidTarget)
{
	// 目前只针对怪物处理
	if (isMonster(uidTarget))
	{
		// 取得怪物ID 
		int nMask = getProperty_Integer(uidTarget, PROPERTY_MASK);

		return ((nMask & MASKFLAG_NOT_ATTACK) != MASKFLAG_NOT_ATTACK);
	}

	return true;
}

void CPlayerKillPart::restore()
{

}

