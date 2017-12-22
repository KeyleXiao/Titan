/*******************************************************************
** 文件名:	WingPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	翅膀部件实现
********************************************************************/

#include "StdAfx.h"
#include "WingPart.h"
#include "WingMessageDef.h"
#include "Entity_ManagedDef.h"
#include "Event_ManagedDef.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "ISpell.h"
#include "ISchemeCenter.h"
#include "IServerGlobal.h"
#include "ExternalHelper.h"
#include "IEntityProperty.h"
#include "IBuffServer.h"
#include "SpellContextDef.h"
#include "EffectCommon_ManagedDef.h"
#include "IEntityAOI.h"
#include "EntityHelper.h"
#include "IMovePart.h"
#include "IAIPart.h"
#include "CampDef.h"

/////////////////////////////////////////ISerializable/////////////////////////////////////////
bool CWingPart::onSerialize(rkt::ibuffer &in, int flag)
{
    if(flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {
        return true;
    }

    BYTE nID, nState;
    in >> nID >> nState;

    ISchemeWing* pSchemeWing = gServerGlobal->getSchemeCenter()->getSchemeWing();
    if(pSchemeWing != NULL)
    {
        SWing* pWing = pSchemeWing->getWing(nID);
        if(pWing != NULL)
        {
            memcpy(&m_Wing, pWing, sizeof(m_Wing));
        }
    }

    m_nWingState = nState;

	return true;
}


bool CWingPart::deSerialize(rkt::obuf &out, int flag)
{
    if(flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {
        return true;
    }

    out << (BYTE)m_Wing.nID << (BYTE)m_nWingState;

	return true;
}


/////////////////////////////////////////__IEntityPart/////////////////////////////////////////
/** 取得部件ID
*/
short CWingPart::getPartID(void)
{
	return PART_WING;
}


/** 载入部件
*/
bool CWingPart::onLoad(__IEntity *pEntity, int nLoginMode)
{
	m_pEntity = pEntity;

    if (m_pEntity == NULL)
    {
        return false;
    }

    __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
    if (pEntityEvent == NULL)
    {
        ErrorLn("Wing part onLoad pEntityEvent==NULL");
        return false;
    }

    if (m_pEntity->getType() == TYPE_PLAYER_ROLE)
    {
        // 注册改变翅膀执行事件
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CHANGE_WING, "CWingPart");
        // 注册受到伤害执行事件
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DAMAGE, "CWingPart");
        // 注册受到死亡执行事件
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DIE, "CWingPart");
        // 注册使用技能执行事件
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CAST_SPELL, "CWingPart");
    }

    return true;
}


/** 取得部件实体
*/
__IEntity *CWingPart::getHost(void)
{
	return m_pEntity;
}


/** 部件消息
*/
bool CWingPart::onMessage(void *pEntityHead, int nMsgID, void *pData, size_t nLen)
{
	switch (nMsgID)
	{
    case WING_MSG_SET_WING_STATE:
        {
            OnClientMessageSetWingState(pData, nLen);
        }
        break;
	default:
		break;
	}
	return true;
}


/** 部件释放
*/
void CWingPart::release(void)
{
    if (m_pEntity != NULL)
    {
        if (m_pEntity->getType() == TYPE_PLAYER_ROLE)
        {
            __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
            if (pEntityEvent != NULL)
            {
                // 注销改变翅膀执行事件
                pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CHANGE_WING);
                // 注销受到伤害执行事件
                pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DAMAGE);
                // 注销受到死亡执行事件
                pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DIE);
                // 注销使用技能执行事件
                pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CAST_SPELL);
            }

            // 杀掉飞行定时器
            g_EHelper.KillTimer(FLY_CHECK_TIMER, this);
        }

        m_pEntity = NULL;
    }
	delete this;
}


/////////////////////////////////////////__IWingPart/////////////////////////////////////////
// 设置翅膀ID
bool CWingPart::SetWingID(int nWingID)
{
    if (m_pEntity == NULL)
    {
        return false;
    }

    __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
    if (pEntityEvent == NULL)
    {
        return false;
    }

    ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return false;
    }

    ISchemeWing *pSchemeWing = pSchemeCenter->getSchemeWing();
    if (pSchemeWing == NULL)
    {
        return false;
    }

    SWing *pWing = pSchemeWing->getWing(nWingID);
    if (pWing == NULL)
    {
        return false;
    }

    if (m_Wing.nID != 0)
    {
        // 改变状态
        OnChangeWingState(WING_STATE_FLOOR);
    }

    // 设置翅膀信息
    memcpy(&m_Wing, pWing, sizeof(m_Wing));

    // 设置改变翅膀信息
    m_bChangeWing = true;

    // 改变翅膀事件
    event_entity_change_wing changeWin;
    changeWin.nOldWingID = m_Wing.nID;
    changeWin.nNewWingID = nWingID;
    pEntityEvent->FireExecute(EVENT_ENTITY_CHANGE_WING, (char *)&changeWin, sizeof(changeWin));

    return true;
}

// 取得翅膀ID
int CWingPart::GetWingID(void)
{
    return m_Wing.nID;
}

// 设置翅膀状态
void CWingPart::SetWingState(int nWingState)
{
    // 改变翅膀状态
    OnChangeWingState(nWingState);
}

// 取得翅膀状态
int CWingPart::GetWingState(void)
{
    return m_nWingState;
}

// 设置翅膀飞行时间
// @param nTime : 飞行时间
void CWingPart::SetFlyTime(int nTime)
{
    m_nFlyTime = nTime;

    // 更新飞行时间
    UpdateFlyTime();
}

// 增加翅膀飞行时间
// @param nTime : 增加飞行时间
void CWingPart::AddFlyTime(int nTime)
{
    SetFlyTime(m_nFlyTime + nTime);
}

// 取得翅膀飞行时间
int CWingPart::GetFlyTime(void)
{
    return m_nFlyTime;
}

// 检测改变翅膀状态
bool CWingPart::CheckChangeWingState(int nStateType)
{
    switch (nStateType)
    {
    case WING_STATE_START_FLY:
        {
            if (m_nWingState != WING_STATE_INVALID)
            {
                return false;
            }

            if (m_pEntity == NULL)
            {
                return false;
            }
            __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
            if (pSpellPart == NULL)
            {
                return false;
            }
            __IEntityProperty *pProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
            if (pProperty == NULL)
            {
                return false;
            }
            if (m_Wing.nID <= 0)
            {
                return false;
            }

            switch (m_Wing.nFlyNeedType)
            {
            case FLY_NEED_STAMINA:
                {
                    // 取得玩家当前体力
                    int nValue = pProperty->getProperty_Integer(PROPERTY_STAMINA);
                    if (nValue < m_Wing.nFlyNeedStamina)
                    {
                        char szParam[256]={0};
                        ssprintf_s(szParam, sizeof(szParam), "%d", m_Wing.nFlyNeedStamina);
                        g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_WING_NEEDSTAMINA, szParam);
                        return false;
                    }
                }
                break;
            case FLY_NEED_MP:
                {
                    // 取得玩家当前法力
                    int nValue = pProperty->getProperty_Integer(PROPERTY_MP);
                    if (nValue < m_Wing.nFlyNeedStamina)
                    {
                        char szParam[256]={0};
                        ssprintf_s(szParam, sizeof(szParam), "%d", m_Wing.nFlyNeedStamina);
                        g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_WING_NEEDMP, szParam);
                        return false;
                    }
                }
                break;
            case FLY_NEED_ENERGY:
                {
                    // 取得玩家当前能量
                    int nValue = pProperty->getProperty_Integer(PROPERTY_ENERGY);
                    if (nValue < m_Wing.nFlyNeedStamina)
                    {
                        char szParam[256]={0};
                        ssprintf_s(szParam, sizeof(szParam), "%d", m_Wing.nFlyNeedStamina);
                        g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_WING_NEEDENERGY, szParam);
                        return false;
                    }
                }
                break;
            default:
                {
                    ErrorLn("CheckChange WingState WING_STATE_START_FLY error nFlyNeedType=" << m_Wing.nFlyNeedType);
                    return false;
                }
                break;
            }
            
            // 起飞技能ID
            int nStartFlySpellID = m_Wing.nStartFlySpellID;
            if (nStartFlySpellID <= 0)
            {
                return false;
            }
            // 飞行类型
            int nFlyType = m_Wing.nFlyType;
            if ((nFlyType&WING_FLY_TIME) == WING_FLY_TIME)
            {
                if (m_nFlyTime <= 0)
                {
					g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_WING_NEEDTIME);
                    return false;
                }
            }

            exposeSelf(true);

            UID uidSelf = m_pEntity->getUID();
            Vector3 vLoc = m_pEntity->getLocation();
            DWORD dwStartTick = getTickCountEx();
            // 技能施法现场
            SPELL_CONTEXT spellContext;
            spellContext.CC.pEntity = m_pEntity;
            spellContext.CC.uidOperator = uidSelf;
            spellContext.CC.uidTarget = uidSelf;
            spellContext.CC.ptStartTile = vLoc;
            spellContext.CC.ptTargetTile = vLoc;
            spellContext.CC.nID = nStartFlySpellID;		
            spellContext.CC.dwStartTick = dwStartTick;
            spellContext.CC.dwUpdateFrame = 0;
            spellContext.EC.pEntity = m_pEntity;
            spellContext.EC.uidOperator = uidSelf;
            spellContext.EC.uidTarget = uidSelf;
            spellContext.EC.ptStartTile = vLoc;
            spellContext.EC.ptTargetTile = vLoc;
            spellContext.EC.nID = nStartFlySpellID;
            spellContext.EC.vSelfLoc = vLoc;
            spellContext.EC.dwStartTick = dwStartTick;
            if (!pSpellPart->castSpell(&spellContext))
            {
                exposeSelf(false);
                return false;
            }
        }
        break;
    case WING_STATE_NORMAL_FLIGHT:
    case WING_STATE_ACCELERATION:
    case WING_STATE_FLOOR:
        {
            // 无效状态,不可直接进入下面状态
            if (m_nWingState == WING_STATE_INVALID)
            {
                return false;
            }

            // 加速要判断是否能加速
            do 
            {
                if (nStateType != WING_STATE_ACCELERATION)
                {
                    break;
                }
                if (m_pEntity == NULL)
                {
                    return false;
                }
                __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
                if (pSpellPart == NULL)
                {
                    return false;
                }
                __IEntityProperty *pProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
                if (pProperty == NULL)
                {
                    return false;
                }
                if (m_Wing.nID <= 0)
                {
                    return false;
                }
                // 不允许加速
                if (!m_Wing.nAllowAcceleration)
                {
					g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_WING_CANNOTACCELERATION);
                    return false;
                }
                // 加速技能ID
                if (m_Wing.nAccelerationID > 0)
                {
                    UID uid = m_pEntity->getUID();
                    Vector3 vLoc = m_pEntity->getLocation();
                    DWORD dwStartTick = getTickCountEx();
                    // 技能施法现场
                    SPELL_CONTEXT spellContext;
                    spellContext.CC.pEntity = m_pEntity;
                    spellContext.CC.uidOperator = uid;
                    spellContext.CC.uidTarget = uid;
                    spellContext.CC.ptStartTile = vLoc;
                    spellContext.CC.ptTargetTile = vLoc;
                    spellContext.CC.nID = m_Wing.nAccelerationID;		
                    spellContext.CC.dwStartTick = dwStartTick;
                    spellContext.CC.dwUpdateFrame = 0;
                    spellContext.EC.pEntity = m_pEntity;
                    spellContext.EC.uidOperator = uid;
                    spellContext.EC.uidTarget = uid;
                    spellContext.EC.ptStartTile = vLoc;
                    spellContext.EC.ptTargetTile = vLoc;
                    spellContext.EC.nID = m_Wing.nAccelerationID;
                    spellContext.EC.vSelfLoc = vLoc;
                    spellContext.EC.dwStartTick = dwStartTick;
                    if (!pSpellPart->castSpell(&spellContext))
                    {
                        return false;
                    }
                }
                switch (m_Wing.nFlyNeedType)
                {
                case FLY_NEED_STAMINA:
                    {
                        // 取得玩家当前体力
                        int nValue = pProperty->getProperty_Integer(PROPERTY_STAMINA);
                        if (nValue < m_Wing.nAccelerationNeedStamina)
                        {
                            char szParam[256]={0};
                            ssprintf_s(szParam, sizeof(szParam), "%d", m_Wing.nAccelerationNeedStamina);
					        g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_WING_ACCELERATIONNEEDSTAMINA, szParam);
                            return false;
                        }
                    }
                    break;
                case FLY_NEED_MP:
                    {
                        // 取得玩家当前魔法
                        int nValue = pProperty->getProperty_Integer(PROPERTY_MP);
                        if (nValue < m_Wing.nAccelerationNeedStamina)
                        {
                            char szParam[256]={0};
                            ssprintf_s(szParam, sizeof(szParam), "%d", m_Wing.nAccelerationNeedStamina);
                            g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_WING_ACCELERATIONNEEDMP, szParam);
                            return false;
                        }
                    }
                    break;
                case FLY_NEED_ENERGY:
                    {
                        // 取得玩家当前能量
                        int nValue = pProperty->getProperty_Integer(PROPERTY_ENERGY);
                        if (nValue < m_Wing.nAccelerationNeedStamina)
                        {
                            char szParam[256]={0};
                            ssprintf_s(szParam, sizeof(szParam), "%d", m_Wing.nAccelerationNeedStamina);
                            g_EHelper.showSystemMessage(m_pEntity, CHAT_TIP_ENTITY_WING_ACCELERATIONNEEDENERGY, szParam);
                            return false;
                        }
                    }
                    break;
                default:
                    {
                        ErrorLn("CheckChange WingState WING_STATE_ACCELERATION error nFlyNeedType=" << m_Wing.nFlyNeedType);
                        return false;
                    }
                    break;
                }
            } while (false);

            // 改变状态
            OnChangeWingState(nStateType);
        }
        break;
    default:
        {
            return false;
        }
        break;
    }

    return true;
}

// 设置翅膀属性
void CWingPart::SetWingProperty(int nType, float fValue)
{
    switch (nType)
    {
    case SET_WING_SPEED:
        {
            m_Wing.fSpeed = fValue;
        }
        break;
    case SET_WING_ACCELERATION:
        {
            m_Wing.fAcceleration = fValue;
        }
        break;
    case SET_WING_MAX_SPEED:
        {
            m_Wing.fMaxSpeed = fValue;
        }
        break;
    case SET_WING_UP_SPEED:
        {
            m_Wing.fUpSpeed = fValue;
        }
        break;
    case SET_WING_MIN_FLY_HEIGHT:
        {
            m_Wing.fMinFlyHeight = fValue;
        }
        break;
    case SET_WING_MAX_FLY_HEIGHT:
        {
            m_Wing.fMaxFlyHeight = fValue;
        }
        break;
    case SET_WING_MIN_CAMERA_ANGLE:
        {
            m_Wing.fMinCameraAngle = fValue;
        }
        break;
    case SET_WING_MAX_CAMERA_ANGLE:
        {
            m_Wing.fMaxCameraAngle = fValue;
        }
        break;
    case SET_WING_FLY_NEED_STAMINA:
        {
            m_Wing.nFlyNeedStamina = (int)fValue;
        }
        break;
    case SET_WING_FLY_COST_STAMINA:
        {
            m_Wing.nFlyCostStamina = (int)fValue;
        }
        break;
    case SET_WING_ACCELERATION_NEED_STAMINA:
        {
            m_Wing.nAccelerationNeedStamina = (int)fValue;
        }
        break;
    case SET_WING_ACCELERATION_COST_STAMINA:
        {
            m_Wing.nAccelerationCostStamina = (int)fValue;
        }
        break;
    case SET_WING_EXIT_ACCELERATION_STAMINA:
        {
            m_Wing.nExitAccelerationStamina = (int)fValue;
        }
        break;
    case SET_WING_FLOOR_STAMINA:
        {
            m_Wing.nFloorStamina = (int)fValue;
        }
        break;
    default:
        ErrorLn("CWingPart error nType="<<nType);
        break;
    }
}

extern "C" __declspec(dllexport) __IWingPart *CreateWingPart()
{
    return new CWingPart;
}


/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
void CWingPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch (wEventID)
    {
    case EVENT_ENTITY_CHANGE_WING:
        {
            COST_TIME_TRACE_START

            if (m_pEntity == NULL)
            {
                break;
            }
            __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
            if (pSpellPart == NULL)
            {
                break;
            }

            if (pszContext == NULL || nLen != sizeof(event_entity_change_wing))
            {
                break;
            }
            event_entity_change_wing *ChangeWing = (event_entity_change_wing *)pszContext;

            ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
            if (pSchemeCenter == NULL)
            {
                break;
            }
            ISchemeWing *pSchemeWing = pSchemeCenter->getSchemeWing();
            if (pSchemeWing == NULL)
            {
                break;
            }

            COST_TIME_TRACE(__LINE__);

            // 移除旧翅膀起飞技能
            do 
            {
                // 旧翅膀ID
                int nOldWingID = ChangeWing->nOldWingID;
                SWing *pWing = pSchemeWing->getWing(nOldWingID);
                if (pWing == NULL)
                {
                    break;
                }
                // 起飞技能ID
                int nStartFlySpellID = pWing->nStartFlySpellID;
                if (nStartFlySpellID <= 0)
                {
                    break;
                }
                // 移除技能
                pSpellPart->removeSpell(nStartFlySpellID);
            } while (false);

            COST_TIME_TRACE(__LINE__)

            // 学会新翅膀起飞技能
            do 
            {
                // 新翅膀ID
                int nNewWingID = ChangeWing->nNewWingID;
                SWing *pWing = pSchemeWing->getWing(nNewWingID);
                if (pWing == NULL)
                {
                    break;
                }
                // 起飞技能ID
                int nStartFlySpellID = pWing->nStartFlySpellID;
                if (nStartFlySpellID <= 0)
                {
                    break;
                }
                // 移除技能
                pSpellPart->addSpell(nStartFlySpellID);
            } while (false);

            COST_TIME_TRACE(__LINE__)

            // 设置翅膀信息
            SetWingInfo();

            COST_TIME_TRACE(__LINE__)
        }
        break;
    case EVENT_ENTITY_DAMAGE:
		{
			if (m_nWingState <= WING_STATE_INVALID || m_nWingState >= WING_STATE_MAX)
			{
				break;
			}

			// 配了被击掉落
			if (m_Wing.bFloorOnBeHit)
			{
				// 改变状态
				OnChangeWingState(WING_STATE_FLOOR);
			}
		}
		break;
    case EVENT_ENTITY_DIE:
        {
            if (m_nWingState <= WING_STATE_INVALID || m_nWingState >= WING_STATE_MAX)
            {
                break;
            }
            // 改变状态
            OnChangeWingState(WING_STATE_FLOOR);
        }
        break;
    case EVENT_ENTITY_CAST_SPELL:
        {
            if (m_pEntity == NULL)
            {
                break;
            }

            // 不在飞行状态不处理
            if (m_nWingState <= WING_STATE_INVALID || m_nWingState >= WING_STATE_MAX)
            {
                break;
            }

            if (pszContext == NULL || nLen != sizeof(event_cast_spell))
            {
                break;
            }
            event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;
            // 未配置使用技能不掉落标识
            if ((pCastSpell->nSpellFlag&SpellFlag_WingNoLand) != SpellFlag_WingNoLand)
            {
                // 改变状态
                OnChangeWingState(WING_STATE_FLOOR);

                // 加buff下落不会被扣血
                SWing *pWing = gServerGlobal->getSchemeCenter()->getSchemeWing()->getWing(m_Wing.nID);
                if(pWing == NULL)
                    return;

                __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
                if(pBuffPart != NULL)
                {
                    if(!pBuffPart->IsExist(pWing->nReqFloorAddBuffID, pWing->nReqFloorAddBuffLevel, m_pEntity->getUID()))
                    {
                        pBuffPart->Add(pWing->nReqFloorAddBuffID, pWing->nReqFloorAddBuffLevel, m_pEntity->getUID());
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

/////////////////////////////////////////TimerHandler/////////////////////////////////////////
void CWingPart::OnTimer( unsigned long dwTimerID )
{
    if (m_pEntity == NULL || m_Wing.nID <= 0)
        return;

    // 检查体力
    checkFlyStamina();
    // 检查时间
    checkFlyTime();
    // 检查遭遇
    checkFlyEncounter();
}

// 检查体力
void CWingPart::checkFlyStamina()
{
    if(m_pEntity == NULL || !(m_Wing.nFlyType & WING_FLY_STAMINA) || (m_nWingState <= WING_STATE_INVALID) || (m_nWingState >= WING_STATE_FLOOR))
        return;

    __IEntityProperty* pProperty = (__IEntityProperty*)m_pEntity->getEntityPart(PART_PROPERTY);
    if(pProperty == NULL)
        return;

    // 取得消耗
    int nCost = 0;
    if (m_nWingState == WING_STATE_ACCELERATION)
    {
        nCost = m_Wing.nAccelerationCostStamina;
    }
    else
    {
        nCost = m_Wing.nFlyCostStamina;
    }

    int nCurValue = 0;
    switch (m_Wing.nFlyNeedType)
    {
    case FLY_NEED_STAMINA:
        {
            pProperty->addProperty_Integer(PROPERTY_STAMINA, -nCost);
            nCurValue = pProperty->getProperty_Integer(PROPERTY_STAMINA);
        }
        break;
    case FLY_NEED_MP:
        {
            pProperty->addProperty_Integer(PROPERTY_MP, -nCost);
            nCurValue = pProperty->getProperty_Integer(PROPERTY_MP);
        }
        break;
    case FLY_NEED_ENERGY:
        {
            pProperty->addProperty_Integer(PROPERTY_ENERGY, -nCost);
            nCurValue = pProperty->getProperty_Integer(PROPERTY_ENERGY);
        }
        break;
    }

    if(m_nWingState == WING_STATE_ACCELERATION && nCurValue <= m_Wing.nExitAccelerationStamina)
    {
        OnChangeWingState(WING_STATE_NORMAL_FLIGHT);
    }

    if(nCurValue <= m_Wing.nFloorStamina)
    {
        OnChangeWingState(WING_STATE_FLOOR);
    }
}

// 检查飞行时间
void CWingPart::checkFlyTime()
{
    if(!(m_Wing.nFlyType & WING_FLY_TIME) || (m_nWingState <= WING_STATE_INVALID) || (m_nWingState >= WING_STATE_FLOOR))
        return;

    // 取得当前时间
    DWORD dwTick = getTickCountEx();
    // 算出飞行时间
    int nTime = dwTick - m_dwLastFlyTime;

    // 消耗时间
    m_nFlyTime -= nTime;
    // 设置最后飞行时间
    m_dwLastFlyTime = dwTick;

    // 飞行时间到了
    if (m_nFlyTime <= 0)
    {
        // 改变状态
        OnChangeWingState(WING_STATE_FLOOR);
    }
}

// 检查遭遇敌人
void CWingPart::checkFlyEncounter()
{
    if((m_Wing.nEncounterDistance <= 0) || (m_nWingState <= WING_STATE_INVALID) || (m_nWingState >= WING_STATE_FLOOR))
        return;

    __IEntityAOI* pEntityAOI = (__IEntityAOI*)m_pEntity->getEntityPart(PART_AOI);
    AOI_PROXY_PTR pAOIProxy = pEntityAOI->getAOIProxy();
    if(pAOIProxy == 0)
    {
        return;
    }

    // 对敌方不可见
    int nEnemeyCampMask = CAMP_EXCEPT_MASK(pAOIProxy->m_nCamp) & CAMP_EXCEPT_MASK(CAMP_NEUTRAL);
    if(!(pAOIProxy->m_nVisibaleFlag & nEnemeyCampMask))
    {
        return;
    }

    SceneHelper sceneHelper(m_pEntity->getSceneID());
    if(!sceneHelper.m_ptr)
        return;

    AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
    int num = sceneHelper.m_ptr->k_nearest(m_pEntity->getLocation(), m_Wing.nEncounterDistance, 
        pProxyArray, MAX_INTEREST_OBJECT, nEnemeyCampMask);
    for(int i = 0; i < num; ++i)
    {
        AOI_PROXY_PTR pProxy = pProxyArray[i];
        if(pProxy->m_nState == ENTITY_STATE_DIE)
            continue;

        // 遇到人或者建筑物 自动下落
        if((UID_2_TYPE(pProxy->m_uid) == TYPE_PLAYER_ROLE) 
            || (UID_2_TYPE(pProxy->m_uid) == TYPE_MONSTER && pProxy->m_nVocation == MONSTER_SUB_TYPE_DEFENSIVE_TOWER))
        {
            // 改变状态
            OnChangeWingState(WING_STATE_FLOOR);

            // 加buff下落不会被扣血
            SWing *pWing = gServerGlobal->getSchemeCenter()->getSchemeWing()->getWing(m_Wing.nID);
            if(pWing == NULL)
                return;

            __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
            if(pBuffPart != NULL)
            {
                if(!pBuffPart->IsExist(pWing->nReqFloorAddBuffID, pWing->nReqFloorAddBuffLevel, m_pEntity->getUID()))
                {
                    pBuffPart->Add(pWing->nReqFloorAddBuffID, pWing->nReqFloorAddBuffLevel, m_pEntity->getUID());
                }
            }

            return;
        }
    }
}

// 设置自己对全图可见
void CWingPart::exposeSelf(bool bExpose)
{
    if(m_pEntity == NULL)
        return;

    __IEntityAOI* pAOIPart = (__IEntityAOI*)m_pEntity->getEntityPart(PART_AOI);
    if(pAOIPart == NULL)
        return;

    for(int nCamp = 0; nCamp < 3; ++nCamp)
    {
        pAOIPart->keepInsight(nCamp, bExpose);
    }       
}

/////////////////////////////////////////CWingPart/////////////////////////////////////////
/** 构造函数
*/
CWingPart::CWingPart(void)
{
	// 翅膀部件实体初始化
	m_pEntity = NULL;
    // 初始化翅膀状态
    m_nWingState = WING_STATE_INVALID;
    // 初始化飞行时间
    m_nFlyTime = 0;
    // 最后飞行时间
    m_dwLastFlyTime = 0;
    // 初始化改变翅膀信息
    m_bChangeWing = false;
}

/** 析构函数
*/
CWingPart::~CWingPart(void)
{
}

// 客户端消息设置翅膀状态
void CWingPart::OnClientMessageSetWingState(void *pData, size_t nLen)
{
    if (pData == NULL || nLen != sizeof(SSetWingState_CS))
    {
        return;
    }
    SSetWingState_CS *pSetWingState = (SSetWingState_CS *)pData;

    if(pSetWingState->nStateType == WING_STATE_FLOOR && pSetWingState->bEncounterFlag)
    {
        checkFlyEncounter();
        return;
    }

    // todo 此处主城支持自动上翅膀，特殊处理，后期改了再优化
    do
    {
        if (m_pEntity == NULL)
        {
            break;
        }
        int nMapID = m_pEntity->getMapID();
        if ( !IsMainMap(nMapID) )
        {
            break;
        }

        __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
        if (pBuffPart == NULL)
        {
            break;
        }
        
        // 增加飞行BUFF
        DWORD dwBuffID = 1379;
        DWORD dwBuffLevel = 4;
        UID uid =  m_pEntity->getUID();
        if (!pBuffPart->IsExist(dwBuffID, dwBuffLevel, uid))
        {
            pBuffPart->Add(dwBuffID, dwBuffLevel, m_pEntity->getUID());
        }
    } while (false);

    // 检测改变翅膀状态
    if(CheckChangeWingState(pSetWingState->nStateType))
    {
        // T降落给自己加buff
        if(pSetWingState->nStateType == WING_STATE_FLOOR)
        {
            SWing *pWing = gServerGlobal->getSchemeCenter()->getSchemeWing()->getWing(m_Wing.nID);
            if(pWing == NULL)
                return;

            __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
            if(pBuffPart != NULL)
            {
                if(!pBuffPart->IsExist(pWing->nReqFloorAddBuffID, pWing->nReqFloorAddBuffLevel, m_pEntity->getUID()))
                {
                    pBuffPart->Add(pWing->nReqFloorAddBuffID, pWing->nReqFloorAddBuffLevel, m_pEntity->getUID());
                }
            }
        }
    }
}

// 改变翅膀状态
void CWingPart::OnChangeWingState(int nWingState)
{
    // 相同状态
    if (m_nWingState == nWingState)
    {
        return;
    }

    int nOldWingState = m_nWingState;
    // 设置新状态
    m_nWingState = nWingState;

    if(nWingState == WING_STATE_START_FLY)
    {
        onLoadWing();
    }
    else if(nWingState == WING_STATE_INVALID)
    {
        // 这里取消暴露视野 在本线程中是在广播floor后面执行。但是并不能保证广播floor的时候还是暴露视野的
        // 因为floor广播是post过去的 而暴露视野是直接设置的变量 这个变量会影响AOI那边计算可见性
        onUnloadWing();
    }

    // 新状态，做一些处理
    switch (nWingState)
    {
    case WING_STATE_START_FLY:
        {
            do 
            {
                if (m_Wing.nID <= 0)
                {
                    break;
                }

                // 飞行类型
                int nFlyType = m_Wing.nFlyType;
                if ((nFlyType&WING_FLY_TIME) == WING_FLY_TIME)
                {
                    // 设置开始飞行
                    m_dwLastFlyTime = getTickCountEx();

                    // 更新飞行时间
                    UpdateFlyTime();
                }

                // 体力飞行 或者时间飞行 或者遭遇时自动下落 启动检测timer
                if((nFlyType & WING_FLY_STAMINA) || (nFlyType & WING_FLY_TIME) || (m_Wing.nEncounterDistance > 0))
                {
                    g_EHelper.SetTimer(FLY_CHECK_TIMER, FLY_CHECK_INTERVAL, this);
                }
            } while (false);
        }
        break;
    default:
        break;
    }
   
    if(m_nWingState == WING_STATE_INVALID)
    {
        fireFlyStateChangeEvent(false);
    }
    else
    {
        fireFlyStateChangeEvent(true);
    }

    // 更新翅膀信息
    UpdateWingInfo();

    // 如果是下落状态，现在自动切换成无效状态
    if (m_nWingState == WING_STATE_FLOOR)
    {
        // 清除飞行信息
        ClearFlyInfo();
        // 更新飞行时间
        UpdateFlyTime();
        // 改变状态
        OnChangeWingState(WING_STATE_INVALID);
    }
}

// 设置翅膀信息
void CWingPart::SetWingInfo()
{
    if (!m_bChangeWing)
    {
        return;
    }
    m_bChangeWing = false;

    SSetWingInfo_SC WingInfo;
    WingInfo.byWingID = m_Wing.nID;
    WingInfo.byStateType = m_nWingState;
    g_EHelper.broadCastInAOI(m_pEntity, m_pEntity->getLocation(), PART_WING, WING_MSG_SET_WING_INFO, (char *)&WingInfo, sizeof(WingInfo));
}

// 更新翅膀信息
void CWingPart::UpdateWingInfo(void)
{
    // 设置翅膀信息
    SetWingInfo();

    SUpdateWingInfo_SC WingInfo;
    WingInfo.nStateType = m_nWingState;
    g_EHelper.broadCastInAOI(m_pEntity, m_pEntity->getLocation(), PART_WING, WING_MSG_UPDATE_WING_INFO, (char *)&WingInfo, sizeof(WingInfo));
}

void CWingPart::fireFlyStateChangeEvent(bool bFlying)
{
    // 发出飞行状态改变事件
    event_entity_fly_state_change event;
    event.uid = m_pEntity->getUID();
    event.bFly = bFlying;
    m_pEntity->getEntityEvent()->FireExecute(EVENT_ENTITY_FLY_STATE_CHANGE, (LPCSTR)&event, sizeof(event));
}

// 清除飞行信息
void CWingPart::ClearFlyInfo(void)
{
    // 清除飞行时间
    m_nFlyTime = 0;
    // 清除最后飞行时间
    m_dwLastFlyTime = 0;
    // 杀掉飞行定时器
    g_EHelper.KillTimer(FLY_CHECK_TIMER, this);
}

// 更新飞行时间
void CWingPart::UpdateFlyTime(void)
{
    // 判断飞行状态
    if (m_nWingState <= WING_STATE_INVALID || m_nWingState >= WING_STATE_FLOOR)
    {
        return;
    }

    if (m_Wing.nID <= 0)
    {
        return;
    }

    if (!m_Wing.bShowProgressBar)
    {
        return;
    }

    SUpdateFlyTime_SC FlyTime;
    FlyTime.nStateType = m_nWingState;
    FlyTime.nTime = m_nFlyTime;
    g_EHelper.playerSend2Client(m_pEntity, PART_WING, WING_MSG_UPDATE_FLY_TIME, (char *)&FlyTime, sizeof(FlyTime));
}


// 戴上翅膀
void CWingPart::onLoadWing()
{
    // 上翅膀 暴露
    exposeSelf(true);

    do
    {
        if (m_Wing.nID <= 0)
            break;
        if (!m_pEntity)
            break;
        __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
        if (!pBuffPart)
            break;

        const DWORD BUFF_LEVEL = 1;
          
        UID uid = m_pEntity->getUID();
        // 戴上翅膀附加BUFF
        for (int i=0; i<_countof(m_Wing.nLoadWingBuffs); ++i)
        {
            int &nBuffID = m_Wing.nLoadWingBuffs[i];
            if (nBuffID == INVALID_BUFF_ID)
                continue;
            if (pBuffPart->IsExist(nBuffID, BUFF_LEVEL, uid))
                continue;
            pBuffPart->Add(nBuffID, BUFF_LEVEL, uid);
        }
    }while(false);
}

// 卸装翅膀
void CWingPart::onUnloadWing()
{
    // 下翅膀 取消暴露
    exposeSelf(false);

    do
    {
        if (!m_pEntity)
            break;
        __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
        if (!pBuffPart)
            break;

        const DWORD BUFF_LEVEL = 1;

        UID uid = m_pEntity->getUID();
        // 卸装翅膀附加BUFF
        for (int i=0; i<_countof(m_Wing.nUnloadWingBuffs); ++i)
        {
            int &nBuffID = m_Wing.nUnloadWingBuffs[i];
            if (nBuffID == INVALID_BUFF_ID)
                continue;
            if (pBuffPart->IsExist(nBuffID, BUFF_LEVEL, uid))
                continue;
            pBuffPart->Add(nBuffID, BUFF_LEVEL, uid);
        }
    }while(false);
}

void CWingPart::restore()
{

}
