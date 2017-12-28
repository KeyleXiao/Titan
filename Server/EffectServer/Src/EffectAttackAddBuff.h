/*******************************************************************
** 文件名:	EffectAttackAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/22/2015
** 版  本:	1.0
** 描  述:	效果-攻击获得BUFF
需求描述：http://172.16.0.120/redmine/issues/785
实现描述：指定类型技能攻击，有概率获得BUFF
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "ISpell.h"
#include "EntityHelper.h"

class CEffectAttackAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AttackAddBuff    SCHEME_DATA;

	CEffectAttackAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
	{
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
			return false;

		m_pEntity = context->pEntity;


		if (m_data.nAttackAddBuffType == ATTACK_ADD_BUFF_DAMAGE)
		{
			//DWORD dwSceneID = m_pEntity->getSceneID();

			//SceneLogicEventHelper eventHelper(dwSceneID);
			//IEventEngine *pGlobalEvent = eventHelper.m_ptr;
			//if (pGlobalEvent == NULL)
			//{
			//	ErrorLn("pGlobalEvent == NULL");
			//	return false;
			//}

			//// 注册英雄伤害事件
			//pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_PLAYER_ROLE, 0, "CEffectAttackAddBuff");
			//// 注册怪物伤害事件
			//pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, 0, "CEffectAttackAddBuff");
			//// 注册Tank伤害事件
			//pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_TANK, 0, "CEffectAttackAddBuff");

            // 订阅自己的伤害事件
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, __FUNCTION__);
            // 订阅目标的伤害事件
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_TARGET_DAMAGE_RESULT, __FUNCTION__);

            // 增加目标伤害通知操作者
            IUseFlag * pUseFlag = m_pEntity->getUseFlag();
            if (pUseFlag) {
                pUseFlag->add(USE_FLAG_DAMAGE_NOTIFY_OPERATOR);
            }
	    }
		else {
			// 注册选择事件
			g_EHelper.Subscibe(m_pEntity, this, (EventKey)m_data.nAttackAddBuffType, "CEffectAttackAddBuff");
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			if (m_data.nAttackAddBuffType == ATTACK_ADD_BUFF_DAMAGE)
			{
				//DWORD dwSceneID = m_pEntity->getSceneID();

				//SceneLogicEventHelper eventHelper(dwSceneID);
				//IEventEngine *pGlobalEvent = eventHelper.m_ptr;
				//if (pGlobalEvent != NULL)
				//{
				//	// 反注册英雄伤害事件
				//	pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_PLAYER_ROLE, 0);
				//	// 反注册怪物伤害事件
				//	pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, 0);
				//	// 反注册Tank伤害事件
				//	pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_TANK, 0);
				//}
                // 移除目标伤害通知操作者
                IUseFlag * pUseFlag = m_pEntity->getUseFlag();
                if (pUseFlag)
                {
                    pUseFlag->remove(USE_FLAG_DAMAGE_NOTIFY_OPERATOR);
                }

                // 取消订阅自己的伤害事件
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);
                // 取消订阅目标的伤害事件
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_TARGET_DAMAGE_RESULT);
			}
			else
			{
				// 注销选择事件
				g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)m_data.nAttackAddBuffType);
			}

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAttackAddBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
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
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_ATTACK:			// 攻击事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_attack))
				{
					return;
				}

				event_entity_attack *pAttack = (event_entity_attack *)pszContext;

				UID uidTarget = INVALID_UID;
				switch (m_data.nTagetType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidTarget = m_pEntity->getUID();
					}
					break;
				case MA_TAGET_TO_TARGET:
					{
						uidTarget = pAttack->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectAttackAddBuff error m_data.nTagetType="<<m_data.nTagetType);
						return;
					}
					break;
				}
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				int	nSpellEffectType = pAttack->nSpellEffectType;
				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}
				
				// 计算概率
				int nRandNum = 0;
				rktRangeRand(nRandNum, 1, EFFECT_PROB_VALUE);
				if(nRandNum > m_data.nProb)
				{
					return;
				}

				// 取得当前时间
				DWORD dwTick = getTickCountEx();
				if (m_dwLastTick + m_data.nInterval > dwTick)
				{
					return;
				}

				UID uidOperator = pAttack->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}
				// 检测目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
				{
					return;
				}

				switch (m_data.nCheckType)
				{
				case CHECK_LESS_PCT_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp <= 0)
						{
							return;
						}
						float fPercent = (float)nHp/nMaxHp;
						if (fPercent >= m_data.nValue/EFFECT_RATE)
						{
							return;
						}
					}
					break;
				case CHECK_LESS_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						if (nHp >= m_data.nValue)
						{
							return;
						}
					}
					break;
				case CHECK_GREATER_PCT_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp <= 0)
						{
							return;
						}
						float fPercent = (float)nHp/nMaxHp;
						if (fPercent <= m_data.nValue/EFFECT_RATE)
						{
							return;
						}
					}
					break;
				case CHECK_GREATER_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						if (nHp <= m_data.nValue)
						{
							return;
						}
					}
					break;
				case CHECK_NO_CHECK_HP:
					{
						// 不检测血量，不需要处理
					}
					break;
                case CHECK_MAX_HP_LESS_THAN:
                    {
                        int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                        if(nMaxHp >= m_data.nValue)
                        {
                            return;
                        }
                    }
                    break;
                case CHECK_MAX_HP_GREATER_THAN:
                    {
                        int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                        if(nMaxHp <= m_data.nValue)
                        {
                            return;
                        }
                    }
                    break;
				default:
					{
						return;
					}
					break;
				}

				// 设置上次时间
				m_dwLastTick = dwTick;

				UID uid = m_pEntity->getUID();
				UID uidAddBuffTarget = INVALID_UID;
				switch (m_data.nAddBuffTagetType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidAddBuffTarget = m_pEntity->getUID();
					}
					break;
				case MA_TAGET_TO_TARGET:
					{
						uidAddBuffTarget = pAttack->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectAttackAddBuff error m_data.nAddBuffTagetType="<<m_data.nAddBuffTagetType);
						return;
					}
					break;
				}
				if (isInvalidUID(uidAddBuffTarget))
				{
					return;
				}

                SBuffContext BuffContext;
                BuffContext.nID = pAttack->nID;
				// 增加buff
				AddBuff(uidAddBuffTarget, m_data.nBuffID, m_data.nBuffLevel, uid, 0, &BuffContext, sizeof(BuffContext));
			}
			break;
		case EVENT_ENTITY_CAST_SPELL:		// 施法技能事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					return;
				}

				event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

				UID uidTarget = INVALID_UID;
				switch (m_data.nTagetType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidTarget = m_pEntity->getUID();
					}
					break;
				case MA_TAGET_TO_TARGET:
					{
						uidTarget = pCastSpell->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectAttackAddBuff error m_data.nTagetType="<<m_data.nTagetType);
						return;
					}
					break;
				}
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				int	nSpellEffectType = pCastSpell->nSpellEffectType;
				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

                int nDamageType = pCastSpell->nDamageType;
                if ((nDamageType&m_data.nDamageType) == 0)
                {
                    return;
                }

				// 计算概率
				int nRandNum = 0;
				rktRangeRand(nRandNum, 1, EFFECT_PROB_VALUE);
				if(nRandNum > m_data.nProb)
				{
					return;
				}

				// 取得当前时间
				DWORD dwTick = getTickCountEx();
				if (m_dwLastTick + m_data.nInterval > dwTick)
				{
					return;
				}

				UID uidOperator = pCastSpell->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				// 检测目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
				{
					return;
				}

				switch (m_data.nCheckType)
				{
				case CHECK_LESS_PCT_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp <= 0)
						{
							return;
						}
						float fPercent = (float)nHp/nMaxHp;
						if (fPercent >= m_data.nValue/EFFECT_RATE)
						{
							return;
						}
					}
					break;
				case CHECK_LESS_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						if (nHp >= m_data.nValue)
						{
							return;
						}
					}
					break;
				case CHECK_GREATER_PCT_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp <= 0)
						{
							return;
						}
						float fPercent = (float)nHp/nMaxHp;
						if (fPercent <= m_data.nValue/EFFECT_RATE)
						{
							return;
						}
					}
					break;
				case CHECK_GREATER_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						if (nHp <= m_data.nValue)
						{
							return;
						}
					}
					break;
				case CHECK_NO_CHECK_HP:
					{
						// 不检测血量，不需要处理
					}
					break;
				case CHECK_MAX_HP_LESS_THAN:
					{
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp >= m_data.nValue)
						{
							return;
						}
					}
					break;
				case CHECK_MAX_HP_GREATER_THAN:
					{
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp <= m_data.nValue)
						{
							return;
						}
					}
					break;
				default:
					{
						return;
					}
					break;
				}

				UID uid = m_pEntity->getUID();
				UID uidAddBuffTarget = INVALID_UID;
				switch (m_data.nAddBuffTagetType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidAddBuffTarget = m_pEntity->getUID();
					}
					break;
				case MA_TAGET_TO_TARGET:
					{
						uidAddBuffTarget = pCastSpell->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectAttackAddBuff error m_data.nAddBuffTagetType="<<m_data.nAddBuffTagetType);
						return;
					}
					break;
				}
				if (isInvalidUID(uidAddBuffTarget))
				{
					return;
				}

				// 设置上次时间
				m_dwLastTick = dwTick;

                SBuffContext BuffContext;
                BuffContext.nID = pCastSpell->nID;
				// 增加buff
				AddBuff(uidAddBuffTarget, m_data.nBuffID, m_data.nBuffLevel, uid, 0, &BuffContext, sizeof(BuffContext));
			}
			break;

        case EVENT_ENTITY_DAMAGE:
        case EVENT_ENTITY_TARGET_DAMAGE_RESULT:
		case EVENT_GAME_ENTITY_DAMAGE:
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				event_entity_damage * pDamage = (event_entity_damage*)pszContext;

				if (pDamage->uidOperator != m_pEntity->getUID())
					return;

				UID uidTarget = INVALID_UID;
				switch (m_data.nTagetType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidTarget = m_pEntity->getUID();
					}
					break;
				case MA_TAGET_TO_TARGET:
					{
						uidTarget = pDamage->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectAttackAddBuff error m_data.nTagetType=" << m_data.nTagetType);
						return;
					}
					break;
				}
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				int	nSpellEffectType = pDamage->nSpellEffectType;
				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

				int nDamageType = pDamage->nDamageType;
				if ((nDamageType&m_data.nDamageType) == 0)
				{
					return;
				}

				// 计算概率
				int nRandNum = 0;
				rktRangeRand(nRandNum, 1, EFFECT_PROB_VALUE);
				if (nRandNum > m_data.nProb)
				{
					return;
				}

				// 取得当前时间
				DWORD dwTick = getTickCountEx();
				if (m_dwLastTick + m_data.nInterval > dwTick)
				{
					return;
				}

				UID uidOperator = pDamage->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				// 检测目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
				{
					return;
				}

				switch (m_data.nCheckType)
				{
				case CHECK_LESS_PCT_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp <= 0)
						{
							return;
						}
						float fPercent = (float)nHp / nMaxHp;
						if (fPercent >= m_data.nValue / EFFECT_RATE)
						{
							return;
						}
					}
					break;
				case CHECK_LESS_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						if (nHp >= m_data.nValue)
						{
							return;
						}
					}
					break;
				case CHECK_GREATER_PCT_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp <= 0)
						{
							return;
						}
						float fPercent = (float)nHp / nMaxHp;
						if (fPercent <= m_data.nValue / EFFECT_RATE)
						{
							return;
						}
					}
					break;
				case CHECK_GREATER_HP:
					{
						int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
						if (nHp <= m_data.nValue)
						{
							return;
						}
					}
					break;
				case CHECK_NO_CHECK_HP:
					{
						// 不检测血量，不需要处理
					}
					break;
				case CHECK_MAX_HP_LESS_THAN:
					{
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp >= m_data.nValue)
						{
							return;
						}
					}
					break;
				case CHECK_MAX_HP_GREATER_THAN:
					{
						int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
						if (nMaxHp <= m_data.nValue)
						{
							return;
						}
					}
					break;
				default:
					{
						return;
					}
					break;
				}

				UID uid = m_pEntity->getUID();
				UID uidAddBuffTarget = INVALID_UID;
				switch (m_data.nAddBuffTagetType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidAddBuffTarget = m_pEntity->getUID();
					}
					break;
				case MA_TAGET_TO_TARGET:
					{
						uidAddBuffTarget = pDamage->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectAttackAddBuff error m_data.nAddBuffTagetType=" << m_data.nAddBuffTagetType);
						return;
					}
					break;
				}
				if (isInvalidUID(uidAddBuffTarget))
				{
					return;
				}

				// 设置上次时间
				m_dwLastTick = dwTick;

				SBuffContext BuffContext;
				BuffContext.nID = pDamage->nID;
				// 增加buff
				AddBuff(uidAddBuffTarget, m_data.nBuffID, m_data.nBuffLevel, uid, 0, &BuffContext, sizeof(BuffContext));

                //TraceLn(__FUNCTION__": uid=" << uid << ",wEventID=" << wEventID << ", buff=" << m_data.nBuffID << ", level=" << m_data.nBuffLevel);
			}
			break;
		default:
			break;
		}
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 上次时间
	DWORD					m_dwLastTick;
};
