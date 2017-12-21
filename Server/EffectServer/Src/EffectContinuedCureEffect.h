/*******************************************************************
** 文件名:	EffectContinuedCureEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/16/2015
** 版  本:	1.0
** 描  述:	效果-持续治疗-效果相关
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectContinuedCureEffect : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_ContinuedCureEffect SCHEME_DATA;

	CEffectContinuedCureEffect( SCHEME_DATA & data ) : m_data(data), m_uidOperator(INVALID_UID), m_uidTarget(INVALID_UID)
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
		if( context == 0 || context->pEntity == NULL)
			return false;

		m_uidOperator = context->uidOperator;
		if (isInvalidUID(m_uidOperator))
		{
			return false;
		}

        m_uidTarget = context->uidTarget;
		if (isInvalidUID(m_uidTarget))
		{
			return false;
		}

		// 首次执行，也算次数
		int nCount = 0;
		// 首次治疗立即执行
		if (m_data.nFirst > 0)
		{
			OnTimer(0);

			nCount = 1;
		}

		if(m_data.nCount >= 1)
		{
			int nTimerCount = m_data.nCount - nCount;
			if (nTimerCount <= 0)
			{
				return true;
			}

			g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectContinuedCureEffect");
		}
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectContinuedCureEffect");
        }
		
		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		g_EHelper.KillTimer(0, this);

		// 清理数据
		m_uidOperator = INVALID_UID;
		m_uidTarget = INVALID_UID;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectContinuedCureEffect(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 获得属性加成
    // 自己给别人治疗，治疗的加成应该是自己的属性影响的
    // 例如索罗斯的复活大招加血 加的血量的加成应该是由索罗斯的法强加成 而不是目标的法强加成
	int CreateApendCure()
	{  
		// 治疗受影响的属性值
		int nProperty=0;
		// 属性加成值
		int nAppendValue=0;
		// 治疗受属性影响类型
		switch (m_data.nExtraCureType)
		{  //因为之前的代码中已经有了最大血值和魔法值的属性加成，在此不重复,下面总共有物攻法攻等级当前血蓝以及损失百分比7项的加成
		case EXTRAATTACK_PA:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_PA);
			}
			break;
		case EXTRAATTACK_MA:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_MA);
			}
			break;
		case EXTRAATTACK_LEVEL:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_LEVEL);
			}
			break;
		case EXTRAATTACK_HP:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_HP);
			}
			break;
		case EXTRAATTACK_MAX_HP:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_MAX_MP);
			}
			break;
		case EXTRAATTACK_LOSE_HP:
			{
				int nHp = getProperty_Integer(m_uidOperator, PROPERTY_HP);
				int nMaxHp = getProperty_Integer(m_uidOperator, PROPERTY_MAX_HP);
				if (nMaxHp <= 0)
				{
					return 0;
				}

				nProperty = nMaxHp - nHp;
			}
			break;
		case EXTRAATTACK_LOSE_HP_PCT:
			{
				int nHp = getProperty_Integer(m_uidOperator, PROPERTY_HP);
				int nMaxHp = getProperty_Integer(m_uidOperator, PROPERTY_MAX_HP);
				if (nMaxHp <= 0)
				{
					return 0;
				}
				float fPercent = (float)(nMaxHp-nHp)/nMaxHp;
				nProperty = (int)(fPercent*EFFECT_RATE);
			}
			break;
		case EXTRAATTACK_MP:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_MP);
			}
			break;
		case EXTRAATTACK_MAX_MP:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_MAX_MP);
			}
			break;
		case EXTRAATTACK_LOSE_MP_PCT:
			{
				int nMp = getProperty_Integer(m_uidOperator, PROPERTY_MP);
				int nMaxMp = getProperty_Integer(m_uidOperator, PROPERTY_MAX_MP);
				if (nMaxMp <= 0)
				{
					return 0;
				}
				float fPercent = (float)(nMaxMp-nMp)/nMaxMp;
				nProperty = (int)(fPercent*EFFECT_RATE);
			}
			break;

		case EXTRAATTACK_MD:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_MD);
			}
			break;
		case EXTRAATTACK_PD:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_PD);
			}
			break;
		case EXTRAATTACK_SD:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_SD);
			}
			break;
		case EXTRAATTACK_EXTRA_PA:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_APPEND_PA);
			}
			break;
		case EXTRAATTACK_EXTRA_MA:
			{
				nProperty = getProperty_Integer(m_uidOperator, PROPERTY_APPEND_MA);
			}
			break;
		default:
			{
				return 0;
			}
			break;
		}
		nAppendValue = (int)(m_data.nCurePCT/EFFECT_RATE*nProperty);
		return nAppendValue;

	}
	virtual void OnTimer( unsigned long dwTimerID )
	{
		if (isInvalidUID(m_uidTarget))
		{
			return;
		}

        if(GetState(m_uidTarget) == ENTITY_STATE_DIE)
        {
            return;
        }

		int nCur = 0;
		int nMax = 0;
		switch (m_data.nCureType)
		{
		case CURE_HP:
			{
				nCur = getProperty_Integer(m_uidTarget, PROPERTY_HP);
				nMax = getProperty_Integer(m_uidTarget, PROPERTY_MAX_HP);
			}
			break;
		case CURE_MP:
			{
				nCur = getProperty_Integer(m_uidTarget, PROPERTY_MP);
				nMax = getProperty_Integer(m_uidTarget, PROPERTY_MAX_MP);
			}
			break;
		default:
			{
				ErrorLn("CEffectContinuedCureEffect error nCureType="<<m_data.nCureType);
				return;
			}
			break;
		}

		int nCureValue = 0;
		switch (m_data.nValueType)
		{
		case MA_VALUE_NUM:
			{
				nCureValue = m_data.nValue + (int)(m_data.nLevelFactor/SPELL_FLOAT_VALUE*getProperty_Integer(m_uidTarget, PROPERTY_LEVEL));
			}
			break;
        // 生命值和魔法值最大值的百分比属性加成
		case MA_VALUE_PER:
			{
				nCureValue = (int)(m_data.nValue/EFFECT_RATE*nMax);
			}
			break;
		// 损失的生命值和魔法值的百分比属性加成
		case MA_VALUE_LOSE_PER:
			{
				nCureValue = (int)(m_data.nValue/EFFECT_RATE*(nMax-nCur));
			}
			break;
		default:
			{
				ErrorLn("CEffectContinuedCureEffect error nValueType="<<m_data.nValueType);
				return;
			}
			break;
		}
		// 治疗消息
		msg_entity_cure cure;
		cure.uidOperator = m_uidOperator;
		cure.uidTarget = m_uidTarget;
		// 这个附加属性数值是针对其余剩下的物攻法攻等级和损失百分比得到的属性加成值（其实上面两个属性值可以合成一个，但是策划那边要重新配）
		int nAppendValue=0;
		//百分比为0不进入获得属性加成函数
		if(m_data.nCurePCT!=0)
		{
			 nAppendValue=CreateApendCure();
		}
		// 总治疗值
		int nCureTotal=nAppendValue+nCureValue;
		// 治疗值为0返回
		if (nCureTotal<= 0)
		{
			return;
		}
		switch (m_data.nCureType)
		{
		case CURE_HP:
			{
				cure.nCureHP = nCureTotal;

			}
			break;
		case CURE_MP:
			{
				cure.nCureMP = nCureTotal;
			}
			break;
		default:
			{
				ErrorLn("CEffectContinuedCureEffect error nCureType="<<m_data.nCureType);
				return;
			}
			break;
		}

		// 发送实体消息
		g_EHelper.sendEntityMessage(m_uidTarget, PART_DAMAGE, DAMAGE_MSG_CURE, (char *)&cure, sizeof(cure));
	}

private:
	SCHEME_DATA               m_data;
	// 操作者UID
	UID						  m_uidOperator;
	// 目标UID
	UID						  m_uidTarget;		
};
