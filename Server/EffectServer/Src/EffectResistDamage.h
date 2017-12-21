/*******************************************************************
** 文件名:	EffectResistDamage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	效果-抵挡伤害
根据选择抵挡类型实现以下效果:
1、抵挡每次的百分比伤害
2、抵挡每次受到的固定伤害数值
3、抵挡总伤害，比如可以抵挡1000点伤害，受到伤害的总量达到1000点以后才会受到伤害,并会移除buff
4、正面抵挡伤害
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ExternalHelper.h"
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "IAttackObject.h"
#include <math.h>

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectResistDamage : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ResistDamage    SCHEME_DATA;

	CEffectResistDamage( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_dwIndex(0),m_bSubscibe(false),m_bAddBuff(false),m_fValue(0)
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
		m_dwIndex = context->dwIndex;

        UID uidTarget = 0;
        switch (m_data.nTagetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidTarget = m_pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidTarget = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidTarget = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectResistDamage error nTagetType="<<m_data.nTagetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidTarget))
        {
            return false;
        }

        __IEntityProperty* pProperty = (__IEntityProperty*)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pProperty == NULL)
        {
            return false;
        }
		
		if (m_data.nType == RESISTDAMAGE_TOTALVALUE_HP ||
			m_data.nType == RESISTDAMAGE_TOTALVALUE_PA ||
			m_data.nType == RESISTDAMAGE_TOTALVALUE_MA ||
			m_data.nType == RESISTDAMAGE_TOTALVALUE_PD ||
			m_data.nType == RESISTDAMAGE_TOTALVALUE_MD ||
            m_data.nType == RESISTDAMAGE_TOTALVALUE_LEVEL||
			m_data.nType == RESISTDAMAGE_TOTALVALUE ||
            m_data.nType == RESISTDAMAGE_TOTALVALUE_MAX_HP)
		{
            m_fValue = m_data.fValue;

			if (m_data.nType == RESISTDAMAGE_TOTALVALUE_HP)				// 血量
				m_fValue *= getProperty_Integer(uidTarget, PROPERTY_HP);		
			else if (m_data.nType == RESISTDAMAGE_TOTALVALUE_PA)		// 物攻
				m_fValue *= getProperty_Integer(uidTarget, PROPERTY_PA);		
			else if (m_data.nType == RESISTDAMAGE_TOTALVALUE_MA)		// 法攻		
				m_fValue *= getProperty_Integer(uidTarget, PROPERTY_MA);
			else if (m_data.nType == RESISTDAMAGE_TOTALVALUE_PD)		// 物防		
				m_fValue *= getProperty_Integer(uidTarget, PROPERTY_PD);		
			else if (m_data.nType == RESISTDAMAGE_TOTALVALUE_MD)		// 法防		
				m_fValue *= getProperty_Integer(uidTarget, PROPERTY_MD);
            else if (m_data.nType == RESISTDAMAGE_TOTALVALUE_LEVEL)		// 等级		
                m_fValue *= getProperty_Integer(uidTarget, PROPERTY_LEVEL);
            else if (m_data.nType == RESISTDAMAGE_TOTALVALUE_MAX_HP)	// 最大血量		
                m_fValue *= getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
			// 加上配置固定值
			m_fValue = m_fValue + m_data.nFixedValue + m_data.nLevelFactor/SPELL_FLOAT_VALUE*pProperty->getProperty_Integer(PROPERTY_LEVEL);

			if (m_fValue < 0.01f)
			{
				ErrorLn(__FUNCTION__<<" Sheld fValue = "<<m_fValue<<"! nType="<<m_data.nType);
				return false;
			}
            
            // 加上护盾提升值
            m_fValue +=  m_fValue * getProperty_Integer(uidTarget, PROPERTY_PROMOTE_SHIELD)/ZOOM_IN_MULTIPLE;
            // 增加护盾事件
            m_fValue = AddShieldEvent(m_fValue);

			pProperty->addProperty_Integer(PROPERTY_SHIELD, (int)m_fValue);
		}

		if (!m_bSubscibe)
		{
			// 注册准备伤害
			g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectResistDamage");

			m_bSubscibe = true;
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            if (m_data.nType == RESISTDAMAGE_TOTALVALUE_HP ||
                m_data.nType == RESISTDAMAGE_TOTALVALUE_PA ||
                m_data.nType == RESISTDAMAGE_TOTALVALUE_MA ||
                m_data.nType == RESISTDAMAGE_TOTALVALUE_PD ||
                m_data.nType == RESISTDAMAGE_TOTALVALUE_MD ||
                m_data.nType == RESISTDAMAGE_TOTALVALUE_LEVEL||
                m_data.nType == RESISTDAMAGE_TOTALVALUE ||
                m_data.nType == RESISTDAMAGE_TOTALVALUE_MAX_HP)
			{
				__IEntityProperty *pProperty = (__IEntityProperty*)m_pEntity->getEntityPart(PART_PROPERTY);
				if (pProperty != NULL)
				{
					pProperty->addProperty_Integer(PROPERTY_SHIELD, (int)-m_fValue);
				}
			}

			// 注销准备伤害
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE);
			m_bSubscibe = false;

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectResistDamage(m_data);
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
				if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
				{
					return;
				}

				if (m_pEntity == NULL)
				{
					return;
				}
				__IEntityProperty *pProperty = (__IEntityProperty*)m_pEntity->getEntityPart(PART_PROPERTY);
				if (pProperty == NULL)
				{
					return;
				}

				DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;

				// 伤害类型不匹配
				if ((pDamage->nDamageType&m_data.nDamageType) == 0)
				{
					return;
				}
				if (pDamage->nDamageHP <= 0)
				{
					return;
				}

				// 取得将要伤害血量
				int nDamageHP = pDamage->nDamageHP;
				if (m_data.nType == RESISTDAMAGE_PCT)
				{
					nDamageHP -= (int)(m_data.fValue/EFFECT_RATE*nDamageHP);
				}
				else if (m_data.nType == RESISTDAMAGE_VALUE)
				{
                    nDamageHP -= (int)m_data.fValue;
				}
                else if (m_data.nType == RESISTDAMAGE_TOTALVALUE_HP ||
                    m_data.nType == RESISTDAMAGE_TOTALVALUE_PA ||
                    m_data.nType == RESISTDAMAGE_TOTALVALUE_MA ||
                    m_data.nType == RESISTDAMAGE_TOTALVALUE_PD ||
                    m_data.nType == RESISTDAMAGE_TOTALVALUE_MD ||
                    m_data.nType == RESISTDAMAGE_TOTALVALUE_LEVEL||
                    m_data.nType == RESISTDAMAGE_TOTALVALUE ||
                    m_data.nType == RESISTDAMAGE_TOTALVALUE_MAX_HP)
				{
					// 抵挡伤害
					int nResistDamage = 0;
					// 还可以抵挡伤害
					if (m_fValue > nDamageHP)
					{
						nResistDamage = nDamageHP;

						m_fValue -= nDamageHP;
						nDamageHP = 0;
					}
					else
					{
						nResistDamage = (int)m_fValue;

						nDamageHP -= (int)m_fValue;
						m_fValue = 0;
					}

					pProperty->addProperty_Integer(PROPERTY_SHIELD, -nResistDamage);
					
					if (m_fValue <= 0.01f && !m_bAddBuff)
					{
						// 设置已增加Buff
						m_bAddBuff = true;

						UID uidSelf = m_pEntity->getUID();

						// 移除BUFF
						RemoveBuffByIndex(uidSelf, m_dwIndex);

						// 增加BUFF
						if (m_data.nBuffID > 0 && m_data.nBuffLvel > 0)
						{
                            SBuffContext BuffContext;
                            BuffContext.nID = pDamage->nID;
							// 增加BUFF
							AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLvel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
						}
					}
				}
				// 正面抵挡
				else if (m_data.nType == RESISTDAMAGE_POSITIVE_RESIST)
				{
					do 
					{
						// 技能不可被正向免疫
						if ((pDamage->nCastType&SpellCastType_PositiveResist) != SpellCastType_PositiveResist)
						{
							break;
						}

						// 技能对象起点
						Vector3 startTile = pDamage->vStartTile;
						// 技能对象当前点
						Vector3 TargetTile = pDamage->vTargetTile;
						// 方向
						Vector3 dir = TargetTile - startTile;
						// 技能方向向量化长度
						float fdirLength = dir.getLength();

						//// 算出弧度
						//angle.x = (360 - angle.x)*180/fPI;
						//angle.y = (360 - angle.y)*180/fPI;
						//angle.z = (360 - angle.z)*180/fPI;

						//// 算出点
						//angle.y=sin(angle.x)*cos(angle.z);
						//angle.x=cos(angle.x)*sin(angle.z)*sin(angle.y);
						//angle.z=cos(angle.x)*sin(angle.z)*cos(angle.y);
						//// 方向向量化长度
						//float fAngledirLength = Angledir.getLength();
						//// 有0向量
						//if (equalto0(fdirLength) || equalto0(fAngledirLength))
						//{
						//	break;
						//}
						//float fcosTheta = (dir*Angledir)/(fdirLength*fAngledirLength);

						// 取得朝向角度
						Vector3 angle = m_pEntity->getAngle();

						float fAngleY = (float)(angle.y*fPI/180);

						Vector3 Angledir;
						Angledir.x=sin(fAngleY);
						Angledir.z=cos(fAngleY);

						float fValue = dir*Angledir;
						// 同向
						if (fValue >= 0)
						{
							break;
						}

						nDamageHP = 0;

					} while (false);
				}
				else
				{
					ErrorLn("Effect Resist Damage not find nType="<<m_data.nType);
					return;
				}
				// 不要算出负了，安全处理下
				if (nDamageHP < 0)
				{
					nDamageHP = 0;
				}
				pDamage->nDamageHP = nDamageHP;
			}
			break;
		default:
			break;
		}
	}

    // 增加护盾事件
    float AddShieldEvent(float fValue)
    {
        do 
        {
            if (!m_data.nAddShieldEvent)
            {
                break;
            }

            if (m_pEntity == NULL)
            {
                break;
            }

            __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
            if (pEntityEvent == NULL)
            {
                break;
            }

            AddShield shield;
            shield.fValue = fValue;
            pEntityEvent->FireExecute(EVENT_ENTITY_ADD_SHIELD, (char *)&shield, sizeof(shield));

            fValue = shield.fValue;
        } while (false);

        return fValue;
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// buffIndex
	DWORD					m_dwIndex;

	// 是否已注册事件
	bool					m_bSubscibe;

	// 是否已增加Buff
	bool					m_bAddBuff;

    // 抵挡伤害值
    float                   m_fValue;
};