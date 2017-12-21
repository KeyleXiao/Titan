/*******************************************************************
** 文件名:	EffectChargedStrike.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/19/2015
** 版  本:	1.0
** 描  述:	效果-充能一击
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "EntityHelper.h"
#include "ExternalHelper.h"
using namespace rkt;
using namespace EFFECT_SERVER;
class CEffectChargedStrike : public IEffectEx,public IEventExecuteSink,public TimerHandler
{
public:
	typedef  EffectServer_ChargedStrike    SCHEME_DATA;
	CEffectChargedStrike( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_nValue(0),m_bSpellBreakingTimeCharge(false),m_nAddedBuffID(0),m_bStopCharge(false),m_bFillAutoRelease(data.nChooseRelease > 0),m_bNeedReleaseCondition(false)
	{
	}

	enum ETimerIDChargedStrike
	{
		ETIMERID_TIMECHARGE,                // 时间充能
		ETIMERID_SPELLBREAKTIMECHARGE,      // 技能打断时间充能
		ETIMERID_MAX,				 
	};

	enum EChargedStrikeType
	{
		ECST_TIMECHARGE,               	   // 时间充能
		ECST_ATTACKCHARGE,			       // 攻击充能
		ECST_SPELLCHARGE,			       // 技能充能
		ECST_MOVECHARGE,			       // 移动充能

		ECST_MAX,				 
	};

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

		if (m_data.nMatchingChargedType != CHARGED_MATCHING_NULL)
		{
			// 注册充能匹配事件
			g_EHelper.Subscibe(m_pEntity, this, (EventKey)m_data.nMatchingChargedType, "EffectChargedStrike");	
		}

        // 注册充能事件
        if(m_data.nChargedType == CHARGED_CLIENTOPT_MOVE_SYN)
        {
            // 注册移动事件
            g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION), "EffectChargedStrike");
        }

		if (m_data.nChargedStrikeType != CHARGED_STRIKE_FILL_CAST)
		{
			// 注册充能一击释放事件
			g_EHelper.Subscibe(m_pEntity, this,(EventKey) m_data.nChargedStrikeType, "EffectChargedStrike");	
		}
		if (m_data.nTimeInterval>0&&m_data. nChargedTimeType!=CHARGED_TIME_NULL)
		{			
			//启动定时器，直到充能满停止计时
			g_EHelper.SetTimer(ETIMERID_TIMECHARGE, m_data.nTimeInterval, this, INFINITY_CALL,__FUNCTION__);
		}


		// 注册移除buff事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_REMOVEBUFF, "EffectChargedStrike");	

        m_ptLastPos = m_pEntity->getLocation();

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			g_EHelper.KillTimer( ETIMERID_TIMECHARGE, this);
			g_EHelper.KillTimer( ETIMERID_SPELLBREAKTIMECHARGE, this);
			if (m_data.nMatchingChargedType != CHARGED_MATCHING_NULL)
			{
				// 注销充能匹配事件
				g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)m_data.nMatchingChargedType);	
			}

            // 注销充能事件
            if(m_data.nChargedType == CHARGED_CLIENTOPT_MOVE_SYN)
            {
                // 注销移动事件
                g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION));
            }

			if (m_data.nChargedStrikeType != CHARGED_STRIKE_FILL_CAST)
			{
				// 注销充能一击释放事件
				g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)m_data.nChargedStrikeType);	
			}

			// 注册移除buff事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_REMOVEBUFF);	

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectChargedStrike(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		if(m_bStopCharge)
		{
			return;
		}

		switch(dwTimerID)
		{
		case ETIMERID_TIMECHARGE:
			{
				m_nValue +=m_data.nTimeValue;
				makesure_minmax(m_nValue, 0, m_data.nLimitValue);
				CheckChargeFilledToAutoRelease(ECST_TIMECHARGE);
			}
			break;
		case ETIMERID_SPELLBREAKTIMECHARGE:
			{
				m_bSpellBreakingTimeCharge = false;
				if(m_data.nTimeInterval <= 0||m_data. nChargedTimeType == CHARGED_TIME_NULL)
				{
					break;
				}

				g_EHelper.SetTimer(ETIMERID_TIMECHARGE, m_data.nTimeInterval, this, INFINITY_CALL, __FUNCTION__);			
			}
			break;
		default:
			break;
		}
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
				if(m_bStopCharge)
				{
					break;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_attack))
				{
					break;
				}
				event_entity_attack *pAttck = (event_entity_attack *)pszContext;

				int nValue = 0;
				if (wEventID == m_data.nMatchingChargedType)
				{
					int nType = m_data.nMatchingType;
					// 充能匹配类型不匹配
					if ((pAttck->nSpellEffectType&nType) != nType)
					{
						break;
					}
					nValue = m_data.nMatchingValue;
				}
				else if (wEventID == m_data.nChargedType)
				{
					nValue = m_data.nValue;
				}
				else
				{
					break;
				}
				if (nValue <= 0)
				{
					break;
				}

				m_nValue += nValue;
				makesure_minmax(m_nValue, 0, m_data.nLimitValue);
				// 增加充能BUFF
				CheckChargeFilledToAutoRelease(ECST_ATTACKCHARGE);
			}
			break;
		case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION:
			{
                // 移动充能
                if(m_data.nChargedType == CHARGED_CLIENTOPT_MOVE_SYN)
                {
                    if (pszContext == NULL || nLen != sizeof(event_entity_property_change))
                    {
                        break;
                    }

                    event_entity_property_change* pEvent = (event_entity_property_change*)pszContext;

                    if(pEvent->nPropertyID != PROPERTY_LOCATION)
                    {
                        ErrorLn(__FUNCTION__ << __LINE__);
                        break;
                    }

                    // 移动充能
                    if(m_bStopCharge)
                    {
                        break;
                    }

                    Vector3 newLoc;
                    memcpy(&newLoc, pEvent->data, pEvent->nLen);
                    if(newLoc.checkDistance(m_ptLastPos, 0.5))
                    {
                        break;
                    }

                    m_ptLastPos = newLoc;  

                    m_nValue += m_data.nValue;
                    makesure_minmax(m_nValue, 0, m_data.nLimitValue);

                    CheckChargeFilledToAutoRelease(ECST_MOVECHARGE);
                }
			}
			break;
		case EVENT_ENTITY_CAST_SPELL:
			{
				if(m_bStopCharge)
				{
					break;
				}

				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					break;
				}
				event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

				// 处理技能打断时间充能
				SpellEffectTypeBreakTimeCharge(pCastSpell->nSpellEffectType);

				// 消耗类型不匹配
				if ((pCastSpell->nSpellEffectType&m_data.nExpendType) != m_data.nExpendType)
				{
					break;
				}
				// 检测充满与否
				CheckChargeFilledToAutoRelease(ECST_SPELLCHARGE);
			}
			break;
		case EVENT_ENTITY_REMOVEBUFF:
			{
				if(m_nAddedBuffID <= 0)
				{
					break;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_remove_buff))
				{
					break;
				}

				event_entity_remove_buff *pRemoveBuff = (event_entity_remove_buff *)pszContext;
				if(pRemoveBuff->dwBuffID != m_nAddedBuffID)
				{
					break;
				}

				// 移除buff后 重新充能 恢复数值
				ResetChargedStrike();
			}
			break;
		default:	
			break;
		}
	}

	void CheckChargeFilledToAutoRelease(EChargedStrikeType eType)
	{
		if(m_pEntity==NULL)
		{
			return;
		}

		if (m_nValue <  m_data.nLimitValue)
		{
			return;
		}

		// 1、自动释放 自动充满后直接 加buff  停止充能
		// 2、手动释放 自动充满后改为自动释放 增加下次释放过滤条件 下次攻击和释放技能ok移动pass    加buff 停止充能 	
		if(m_bFillAutoRelease)
		{
			if(m_bNeedReleaseCondition)
			{
				if(eType == ECST_MOVECHARGE)
				{
					return;
				}
			}

			AddBuffAndStopCharge();
		}
		else
		{
			m_bFillAutoRelease = true;
			m_bNeedReleaseCondition = true;
		}

		return;
	}

	void AddBuffAndStopCharge()
	{
		m_bStopCharge = true;

		UID uid = m_pEntity->getUID();
		AddBuff(uid, m_data.nBuffID, m_data.nBuffLevel, uid);
		m_nAddedBuffID = m_data.nBuffID;

		return;
	}

	void SpellEffectTypeBreakTimeCharge(int nSpellEffectType)
	{
		if(m_data.nSpellEffectBreakTime <= 0 || m_data.nSpellEffectBreakType <= 0 )
		{
			return;
		}

		int nFilterResult = nSpellEffectType&m_data.nSpellEffectBreakType;
		if ((nFilterResult & m_data.nSpellEffectBreakType) != nFilterResult)
		{
			return;
		}

		if(m_data.nTimeInterval <= 0||m_data. nChargedTimeType == CHARGED_TIME_NULL)
		{
			return;
		}

		if(m_bSpellBreakingTimeCharge)
		{
			return;
		}

		g_EHelper.KillTimer(ETIMERID_TIMECHARGE, this);
		g_EHelper.SetTimer(ETIMERID_SPELLBREAKTIMECHARGE, m_data.nSpellEffectBreakTime, this, 1,__FUNCTION__);		
		m_bSpellBreakingTimeCharge = true;
	}

	void ResetChargedStrike() 
	{
		m_nValue -= m_data.nReleaseValue;
		m_bSpellBreakingTimeCharge = false;
		m_nAddedBuffID = 0;
		m_bStopCharge = false;
		m_bFillAutoRelease = m_data.nChooseRelease > 0;
		m_bNeedReleaseCondition = false;
	}


private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 充能值
	int						m_nValue;

	bool					m_bSpellBreakingTimeCharge;

	// 已经添加到身上的buff 用于buff移除的时候重新充能
	int						m_nAddedBuffID;
	// 充能满足后停止充能
	bool					m_bStopCharge;

	bool					m_bFillAutoRelease;

	bool					m_bNeedReleaseCondition;

    Vector3                 m_ptLastPos;
};