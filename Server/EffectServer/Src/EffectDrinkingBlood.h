/*******************************************************************
** 文件名:	EffectDrinkingBlood.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/17/2015
** 版  本:	1.0
** 描  述:	效果-饮血抵挡效果
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectDrinkingBlood : public IEffectEx,public IEventExecuteSink,public rkt::TimerHandler
{
public:
	typedef  EffectServer_DrinkingBlood    SCHEME_DATA;

	CEffectDrinkingBlood( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0),m_nValue(0),m_nLimitValue(0),m_dwLastTransitTime(0),m_IsAddBuff(false)
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

		// 注册预治疗事件
		g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_PREPARE_CURE, "CEffectDrinkingBlood");

		// 注册预伤害事件
		g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_PREPARE_DAMAGE, "CEffectDrinkingBlood");

		// 启动定时器
		g_EHelper.SetTimer(0, 1000, this, INFINITY_CALL, "CEffectDrinkingBlood");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销预治疗事件
			g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_PREPARE_CURE);

			// 注销预伤害事件
			g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_PREPARE_DAMAGE);

			// 杀掉定时器定时器
			g_EHelper.KillTimer(0, this);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectDrinkingBlood(m_data);
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
        case EVENT_ENTITY_PREPARE_DAMAGE:
            {
                if (m_pEntity == NULL)
                {
                    return;
                }
                // 取得实体属性对象
                __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
                if (pEntityProperty == NULL)
                {
                    return;
                }

                if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
                {
                    return;
                }
                DamageCalcContext *pDamage = (DamageCalcContext*)pszContext;
                if (pDamage->nDamageHP <= 0)
                {
                    return;
                }

                int nAddValue = 0;
                // 伤害血量比抵挡血量多
                if (pDamage->nDamageHP > m_nValue)
                {
                    nAddValue = m_nValue;
                }
                else
                {
                    nAddValue = pDamage->nDamageHP;
                }
                pDamage->nDamageHP -= nAddValue;
                m_nValue -= nAddValue;
                //加光环
                addAuraBuff(); 
                if (nAddValue != 0)
                {
                    // 增加额外血量
                    pEntityProperty->addProperty_Integer(PROPERTY_SHIELD, -nAddValue);
                }

                // 设置上次过渡时间
                m_dwLastTransitTime = getTickCountEx();
            }
            break;
		case EVENT_ENTITY_PREPARE_CURE:
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				// 取得实体属性对象
				__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
				if (pEntityProperty == NULL)
				{
					return;
				}

				DWORD dwTick = getTickCountEx();
				if (dwTick < m_dwLastTick + m_data.nInterval)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(CureContext))
				{
					return;
				}
				CureContext *pCure = (CureContext*)pszContext;
				// 不是吸血
				if (pCure->nCureEffectType != CureEffect_Drains)
				{
					return;
				}

				// 溢出时，要求满血
				if (m_data.nOverflow == MA_TRUE)
				{
					// 取得当前血量
					int nHp = pEntityProperty->getProperty_Integer(PROPERTY_HP);
					// 取得最大血量
					int nMaxHp = pEntityProperty->getProperty_Integer(PROPERTY_MAX_HP);
					// 满血才可以
					if (nHp != nMaxHp)
					{
						return;
					}
				}

				// 算出当前上限值
				m_nLimitValue = (int)(pEntityProperty->getProperty_Integer(m_data.nDrinkingBloodType)*m_data.fValue + m_data.nFixedValue);
			
                // 增加护盾事件
                AddShieldEvent(m_nLimitValue);

				// 算出本次转化值
				int nAddValue = (int)(pCure->nCureHP*m_data.fConversionRate);

				int nResultValue = m_nValue + nAddValue;
				makesure_minmax(nResultValue, 0, m_nLimitValue);

				// 算出本次增加数量
				nAddValue = nResultValue - m_nValue;
				if (nAddValue != 0)
				{
					// 增加额外血量
					pEntityProperty->addProperty_Integer(PROPERTY_SHIELD, nAddValue);
				}

				// 设置当前值
				m_nValue = nResultValue;
				//加光环
				addAuraBuff(); 
				// 设置上次时间
				m_dwLastTick = dwTick;
				// 设置上次过渡时间
				m_dwLastTransitTime = dwTick;
			}
			break;
		default:
			break;
		}
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		// 取得Tick
		DWORD dwTick = getTickCountEx();
		if (dwTick < m_dwLastTransitTime + m_data.nTransitTime)
		{
			return;
		}
		if (m_nValue <= 0)
		{
			return;
		}
		if (m_pEntity == NULL)
		{
			return;
		}
		// 取得实体属性对象
		__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
		if (pEntityProperty == NULL)
		{
			return;
		}

		int nValue = 0;
		switch (m_data.nValueType)
		{
		case MA_VALUE_NUM:
			{
				nValue = m_data.nValue;
			}
			break;
		case MA_VALUE_PER:
			{
				nValue = (int)(m_data.nValue/EFFECT_RATE*m_nLimitValue);
			}
			break;
		default:
			{
				ErrorLn("CEffectDrinkingBlood error nValueType="<<m_data.nValueType);
				return;
			}
			break;
		}
		if (nValue <= 0)
		{
			return;
		}

		int nAddValue = 0;
		if (m_nValue > nValue)
		{
			nAddValue = nValue;
		}
		else
		{
			nAddValue = m_nValue;
		}
		m_nValue -= nAddValue;
		//加光环
		addAuraBuff(); 
		if (nAddValue != 0)
		{
			// 增加额外血量
			pEntityProperty->addProperty_Integer(PROPERTY_SHIELD, -nAddValue);
		}
	}

    void addAuraBuff()
    {
	     if (m_pEntity == NULL)
	     {
	 	     return;
	     }
	      //光环buff存在并且抵挡值小等于0，移除光环
	     if( m_IsAddBuff&&m_nValue<=0)
	     {
		     UID uidSelf=m_pEntity->getUID();
		     RemoveBuff(uidSelf, m_data.nBuffID, uidSelf, uidSelf);
		     m_IsAddBuff=false;
		     return;
	     }
	     //光环buff不存在并且抵挡值大于0，增加光环
	     if((!m_IsAddBuff)&&m_nValue>0)
	     {
		     UID uidSelf=m_pEntity->getUID();
		     AddBuff(uidSelf, m_data.nBuffID,m_data.nBuffLevel,uidSelf);
		     m_IsAddBuff=true;
		     return;
	     }
    }

    // 增加护盾事件
    void AddShieldEvent(int &nValue)
    {
        if (!m_data.nAddShieldEvent)
        {
            return;
        }

        if (m_pEntity == NULL)
        {
            return;
        }

        __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
        if (pEntityEvent == NULL)
        {
            return;
        }

        AddShield shield;
        shield.fValue = (float)nValue;
        pEntityEvent->FireExecute(EVENT_ENTITY_ADD_SHIELD, (char *)&shield, sizeof(shield));

        // 设置新增，因为上面事件可能会改变
        nValue = (int)shield.fValue;
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 上次时间
	DWORD					m_dwLastTick;

	// 当前值
	int						m_nValue;
	// 上限值
	int						m_nLimitValue;

	// 上次过渡时间
	DWORD					m_dwLastTransitTime;
	// 抵挡光环是否存在
	bool                    m_IsAddBuff;
};
