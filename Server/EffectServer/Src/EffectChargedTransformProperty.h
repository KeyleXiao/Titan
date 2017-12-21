/*******************************************************************
** 文件名:	EffectChargedStrike.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	9/29/2015
** 版  本:	1.0
** 描  述:	效果-充能转化属性
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "EntityHelper.h"
#include "ExternalHelper.h"
#include "TimerHandler.h"
#include "TimerAxis.h "
#include "BuffMessageDef.h"
#include "ICommonPart.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectChargedTransformProperty : public IEffectEx,public IEventExecuteSink,public TimerHandler
{
public:
    typedef  EffectServer_ChargedToProPerty    SCHEME_DATA;


    CEffectChargedTransformProperty( SCHEME_DATA & data )
        : m_data(data),m_pEntity(0),m_Property(0),m_pWarPart(0),m_ChargedCount(0),m_SelfUID(INVALID_UID),m_dwIndex(0),m_nChargeValue(0)
    {
        vector<string> vevTempValue;
        StringHelper::split(vevTempValue, m_data.szActiveValues, ';', " ");

        vector<string> vevTempBuffID;
        StringHelper::split(vevTempBuffID, m_data.szActiveBuffIDs, ';', " ");

        vector<string> vevTempLevel;
        StringHelper::split(vevTempLevel, m_data.szActiveBuffLvs, ';', " ");

        size_t nCount = min( vevTempValue.size(), vevTempBuffID.size());
        nCount = min(nCount, vevTempLevel.size());

        for (int i=0; i < nCount; ++i)
        {
            int nActiveVal = StringHelper::toInt(vevTempValue[i]);
            if(nActiveVal <= 0)
                continue;

            INT32 nBuffID = StringHelper::toInt(vevTempBuffID[i]);
            if(nBuffID <= 0)                
                continue;

            INT32 nLevel = StringHelper::toInt(vevTempLevel[i]);
            if(nLevel <= 0)
                continue;

            UINT64 nValue = (UINT64)nBuffID << 32 | nLevel;

            m_mapActiveBuffs[nActiveVal] = nValue;
        }
    }

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

    // 效果启用
    virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
    {
        if ( context==0 || context->pEntity==0 )
        {
            return false;
        }

        m_pEntity = context->pEntity;
        m_SelfUID = m_pEntity->getUID();
        m_dwIndex = context->dwIndex;

        m_Property = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (m_Property == 0)
        {
            return false;
        }

        m_pWarPart = (__IWarPart*)m_pEntity->getEntityPart(PART_WAR);
        if(m_pWarPart == NULL)
        {
            return false;
        }

        int nWarID = m_pWarPart->getWarID();
        WarHelper helper(nWarID);
        IWarService *pWarService = helper.m_ptr;
        if (pWarService == NULL)
        {
            return false;
        }

        DWORD dwSceneID = m_pEntity->getSceneID();
        SceneLogicEventHelper eventHelper(dwSceneID);
        IEventEngine *pGlobalEvent = eventHelper.m_ptr;
        if (pGlobalEvent == NULL)
        {
            ErrorLn(_GT("CEffectChargedTransformProperty Start 取得全局事件失败，问题严重！dwSceneID=") << dwSceneID);
            return false;
        }

        // 时间充能 如果充能间隔为正数才具有意义,开启定时器
        if (m_data.nTimeInterval > 0 && (m_data.nChargedType & CHARGED_TIME) == CHARGED_TIME)
        {
            //启动定时器，直到充能满停止计时
            g_EHelper.SetTimer(0, m_data.nTimeInterval, this, INFINITY_CALL, "CEffectChargedTransformProperty");
        }

        if (m_data.nChargedType != CHARGED_NULL)
        {
            // 技能打中人充能
            if((m_data.nChargedType & CHARGED_ATTACK) == CHARGED_ATTACK)
            {
                g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK, "CEffectChargedTransformProperty");
            }
            // 只要施放技能就充能
            if((m_data.nChargedType & CHARGED_CAST_SPELL) == CHARGED_CAST_SPELL)
            {
                g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL, "CEffectChargedTransformProperty");
            }
            // 移动充能
            if((m_data.nChargedType & CHARGED_MOVE) == CHARGED_MOVE)
            {
                g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE, "CEffectChargedTransformProperty");
            }

            // 注册英雄死亡事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0, "CEffectChargedTransformProperty");
            // 注册怪物死亡事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0, "CEffectChargedTransformProperty");
        }

        // 技能上次效果的
        if (m_data.nEffectID != 0)
        {
            // 取配置效果充能
            int nEffectValue = pWarService->getChargedValue(m_SelfUID, m_data.nEffectID);
            // 取本效果充能
            int nValue = pWarService->getChargedValue(m_SelfUID, m_data.nID);
            // 取最大的
            nValue = max(nEffectValue, nValue);
            // 设置充能
            AddChargedStrikeProperty(nValue);
        }
        else
        {
            // 取本效果充能
            int nValue = pWarService->getChargedValue(m_SelfUID, m_data.nID);
            // 设置充能
            AddChargedStrikeProperty(nValue);
        }

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
        if (m_pEntity != 0)
        {
            g_EHelper.KillTimer(0, this);

            if (m_data.nChargedType != CHARGED_NULL)
            {
                // 技能打中人充能
                if((m_data.nChargedType&CHARGED_ATTACK) == CHARGED_ATTACK)
                {
                    g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK);
                }
                // 只要施放技能就充能
                if((m_data.nChargedType&CHARGED_CAST_SPELL) == CHARGED_CAST_SPELL)
                {
                    g_EHelper.UnSubscibe(m_pEntity, this,EVENT_ENTITY_CAST_SPELL);
                }
                // 移动充能
                if((m_data.nChargedType&CHARGED_MOVE) == CHARGED_MOVE)
                {
                    g_EHelper.UnSubscibe(m_pEntity, this,EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE);
                }

                DWORD dwSceneID = m_pEntity->getSceneID();
                SceneLogicEventHelper eventHelper(dwSceneID);
                IEventEngine *pGlobalEvent = eventHelper.m_ptr;
                if (pGlobalEvent == NULL)
                {
                    ErrorLn(__FUNCTION__": pGlobalEvent == NULL,  dwSceneID="<< dwSceneID);
                }else
                {
                    // 注销英雄死亡事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0);
                    // 注销怪物死亡事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0);
                }
            }
            
            if (m_Property != 0)
            {
                m_Property->addProperty_Integer(m_data.nChargedPropertyType, -m_nChargeValue);
            }
            // 充能值保存至实体
            __ICommonPart* pCommonPart = CAST_TYPE( __ICommonPart*, m_pEntity->getEntityPart(PART_COMMON) );
            if ( pCommonPart )
            {
                pCommonPart->setChargeValue(0);   
            }
        }

        m_pWarPart = 0;
        m_Property = 0;
        m_pEntity = 0;
    }

    // 克隆一个新效果
    virtual IEffect *       Clone()
    {
        return new CEffectChargedTransformProperty(m_data);
    }

    // 释放
    virtual void			Release()
    {
        Stop();
        delete this;
    }

    virtual void OnTimer( unsigned long dwTimerID )
    {
        AddChargedStrikeProperty(m_data.nTimeValue);
    }

    virtual int getPropertyValue(CHARGED_PROPERTY_TYPE nType)
    {
        if (m_Property == NULL)
        {
            return 0;
        }

        switch(nType)
        {
        case CHARGED_TRANSLATE_APPEND_HP:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_HP);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_MP:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_MP);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_PA:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_PA);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_MA:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_MA);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_PD:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_PD);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_MD:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_MD);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_SD:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_SD);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_PASD:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_PASD);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_AF:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_AF);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_CRC:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_CRC);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_HP_REPLY:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_HP_REPLY);
            }
            break;
        case CHARGED_TRANSLATE_APPEND_MP_REPLY:
            {
                return m_Property->getProperty_Integer(ENTITY_PROPERTY::PROPERTY_MP_REPLY);
            }
            break;
        default:
            break;
        }

        return 0;
    }

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
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch (wEventID)
        {
        case EVENT_ENTITY_ATTACK:
            {
                if (pszContext == NULL || nLen != sizeof(event_entity_attack))
                {
                    break;
                }
                event_entity_attack *pAttack = (event_entity_attack *)pszContext;
                // 技能效果类型匹配
                if ( (m_data.nSpellEffectType & pAttack->nSpellEffectType) != m_data.nSpellEffectType )
                {
                    break;
                }

                int nValue = m_data.nPAValue;
                if (nValue <= 0)
                {
                    break;
                }

                // 增加充能BUFF
                AddChargedStrikeProperty(nValue);
            }
            break;
        case EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE:
            {
                if (pszContext == NULL || nLen != sizeof(event_entity_move_behaviour_change))
                {
                    break;
                }

                int nValue = m_data.nMoveValue;
                if (nValue <= 0)
                {
                    break;
                }

                // 增加充能BUFF
                AddChargedStrikeProperty(nValue);
            }
            break;
        case EVENT_ENTITY_CAST_SPELL:
            {
                if (pszContext == NULL || nLen != sizeof(event_cast_spell))
                {
                    break;
                }
                event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;
                // 技能效果类型匹配
                if ((m_data.nSpellEffectType & pCastSpell->nSpellEffectType) != m_data.nSpellEffectType)
                {
                    break;
                }
                int nValue = m_data.nMAValue;
                if (nValue <= 0)
                {
                    break;
                }

                // 增加充能BUFF
                AddChargedStrikeProperty(nValue); 
            }
            break;
            // 实体死亡事件
        case EVENT_GAME_ENTITY_DIE:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_die))
                {
                    break;
                }
                event_entity_die *pDie = (event_entity_die *)pszContext;
                // 目标UID
                UID uidTarget = pDie->uidTarget;
                // 如果死亡者是自己，减充能层数，减充能值
                if(uidTarget == m_SelfUID)
                {
                    DieDeductChargedStrikeProperty();
                    break;
                }

                // 检测目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
                {
                    break;
                }

                // 充能值
                int nValue = 0;
                // 死亡范围优先
                if ((m_data.nChargedType & CHARGED_DIE_RANGE) == CHARGED_DIE_RANGE) // 死亡范围充能
                {
                    // 取得玩家位置
                    Vector3 vLoc = m_pEntity->getLocation();
                    // 取得死亡者位置
                    Vector3 vDieLoc = getLocation(uidTarget);
                    if (!vLoc.checkDistance(vDieLoc, m_data.fDieDistance))
                    {
                        break;
                    }

                    nValue = m_data.nDieValue;
                }
                else if ((m_data.nChargedType & CHARGED_KILL) == CHARGED_KILL)  // 击杀充能
                {
                    switch(m_data.nKillValueType)
                    {
                    case MA_VALUE_NUM:  // 固定值
                        {
                            nValue = (int)m_data.fKIllValue;        // 击杀充能值(float)
                        }
                        break;
                    case MA_VALUE_PER:  // 百分比
                        {
                            int nProperty = getPropertyValue(m_data.nChargedPropertyType) - m_nChargeValue;;
                            nValue = (int)(m_data.fKIllValue * (float)nProperty);
                        }
                        break;
                    default:
                        break;
                    }
                }
                

                if (nValue <= 0)
                {
                    break;
                }

				// 增加充能BUFF
				AddChargedStrikeProperty(nValue); 
			}
			break;
		default:
			break;
		}
	}
	
	// 增加充能数值
	void AddChargedStrikeProperty(int nAddValue)
	{
		if(m_pEntity==NULL || m_Property==NULL || m_pWarPart == NULL)
		{
			return;
		}

		if(nAddValue == 0)
		{
			return;
		}
        if( m_data.nLimitValue <= 0)
        {
            return;
        }
		if(m_nChargeValue >= m_data.nLimitValue)
		{
			return;
		}

        int nWarID = m_pWarPart->getWarID();
        WarHelper helper(nWarID);
        IWarService *pWarService = helper.m_ptr;
        if (pWarService == NULL)
        {
            return;
        }

        if( m_nChargeValue + nAddValue >= m_data.nLimitValue )
        {
            nAddValue = m_data.nLimitValue - m_nChargeValue;
        }
		m_nChargeValue += nAddValue;
        // 前面已控制上限，没有必要重复校验
		//makesure_minmax(m_nChargeValue, 0, m_data.nLimitValue);

		// 设置属性
		m_Property->addProperty_Integer(m_data.nChargedPropertyType, nAddValue);

        // 设置本效果充能
        pWarService->setChargedValue(m_SelfUID, m_data.nID, m_nChargeValue);

        // 充能值保存至实体
        __ICommonPart* pCommonPart = CAST_TYPE( __ICommonPart*, m_pEntity->getEntityPart(PART_COMMON) );
        if ( pCommonPart )
        {
            pCommonPart->setChargeValue(m_nChargeValue);   
        }

		// 充能层数增加
		m_ChargedCount++;

        // 达到阀值增加状态
        ChangeValueActiveBuff();

		// 发送BUFF上充能层数显示的数据
		sendChargeCountMessage();
    }

    // 达到阀值增加状态
    void ChangeValueActiveBuff()
    {
        if (m_nChargeValue == 0 || m_SelfUID == INVALID_UID)
        {
            return;
        }

        map<int, UINT64>::iterator iterFind = m_mapActiveBuffs.end();
        for (map<int, UINT64>::iterator iter = m_mapActiveBuffs.begin(); iter != m_mapActiveBuffs.end(); ++iter)
        {
            if(m_nChargeValue < iter->first)
                break;

            iterFind = iter;
        }
        if(iterFind == m_mapActiveBuffs.end())
        {
            return;
        }

        UINT64 & nBuffLevel = iterFind->second;
        int nBuffID = nBuffLevel >> 32;
        int nLevel = (nBuffLevel & 0xFFFFFFFF);
        if(nBuffID <= 0 || nLevel<= 0)
        {
            return;
        }

        // 死亡者增加BUFF
        AddBuff(m_SelfUID, nBuffID, nLevel, m_SelfUID);
    }

	// 主英雄死亡减少充能值
	void DieDeductChargedStrikeProperty()
	{   
		if(m_pEntity==NULL || m_Property==NULL)
		{
			return;
		}
		// 充能层数0返回
		if (m_ChargedCount==0)
		{
			return;
		}
        // 没有配置
        if( fabs(m_data.fDieValue) < 1e-6 )
        {
            return;
        }

        // 死亡扣百分比(float)
		float fLoseChargedCount = m_data.fDieValue * (float)m_ChargedCount;
		int nLoseChargeValue = (int)(fLoseChargedCount/m_ChargedCount*m_nChargeValue);

		m_ChargedCount -= (int)fLoseChargedCount;
		m_nChargeValue -= nLoseChargeValue;

		// 设置属性
		m_Property->addProperty_Integer(m_data.nChargedPropertyType, -nLoseChargeValue);

		// 发送BUFF上充能层数显示的数据
		sendChargeCountMessage();
	}

	// 发送BUFF上充能层数显示的数据
	void sendChargeCountMessage()
	{
		SBuffMsgBuffChargeCountChange_SC changeChargeCount;
		changeChargeCount.nChargeCount = m_ChargedCount;
		changeChargeCount.dwIndex = m_dwIndex;
		changeChargeCount.nChargeValue = m_nChargeValue;
		g_EHelper.broadCastInAOI(m_pEntity, m_pEntity->getLocation(), BUFF_MSG_CHARGE_COUNT_CHANGE, PART_BUFF, (char *)&changeChargeCount, sizeof(changeChargeCount));
	}

private:
    SCHEME_DATA             m_data;

    // 实体指针
    __IEntity				*m_pEntity;

    // 实体属性指针
    __IEntityProperty       *m_Property;

    __IWarPart              *m_pWarPart;

    // 充能能量层数
    int                     m_ChargedCount;
    // 自己的UID
    UID                     m_SelfUID; 
    // buffIndex
    DWORD					m_dwIndex;
    // 当前充能值
    int                     m_nChargeValue;

    // 激活状态列表，阀值为KEY，INT64=BUFFID<<32|LEVEL
    map<int, UINT64>      m_mapActiveBuffs;
};
