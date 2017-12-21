/*******************************************************************
** 文件名:	EffectContinuedSetProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭政林
** 日  期:	9/01/2015
** 版  本:	1.0
** 描  述:	效果-持续设置属性
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "TimerHandler.h"
#include "ExternalHelper.h"

class CEffectContinuedSetProperty : public IEffectEx, public rkt::TimerHandler
{
public:
	typedef  EffectServer_ContinuedSetProperty SCHEME_DATA;

	CEffectContinuedSetProperty( SCHEME_DATA & data ) : m_data(data), m_uidTarget(INVALID_UID)
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
		if( context==0 || context->pEntity==0 )
		{
			return false;
		}

		switch (m_data.nTagetType)
		{
		case MA_TAGET_TO_SELF:
			{
				 m_uidTarget = context->pEntity->getUID();
			}
			break;
		case MA_TAGET_TO_TARGET:
			{
				m_uidTarget = context->uidTarget;
			}
			break;
		default:
			{
				ErrorLn("CEffectContinuedSetProperty error nTagetType="<<m_data.nTagetType);
				return false;
			}
			break;
		}
		if (isInvalidUID(m_uidTarget))
		{
			return false;
		}

		// 首次执行，也算次数
		int nCount = 0;
		// 首次执行
		if (m_data.nFirst > 0)
		{
			OnTimer(0);

			nCount = 1;
		}

        char szText[64] = {0};
        ssprintf_s(szText, sizeof(szText), "CEffectContinuedSetProperty,nID=%d,m_uidTarget=%d",GetID(),m_uidTarget);
		if(m_data.nCount >= 1)
		{
			int nTimerCount = m_data.nCount - nCount;
			if (nTimerCount <= 0)
			{
				return true;
			}

			g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, szText);
		}
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, szText);
        }
		
		return true;
	}

	// 效果停止
	virtual void			Stop()
    {
        g_EHelper.KillTimer(0, this);
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectContinuedSetProperty(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		if(GetState(m_uidTarget) == ENTITY_STATE_DIE)
		{
			return;
		}

		// 取得当前值
		int nCur = getProperty_Integer(m_uidTarget, m_data.nPropertyType);
		// 取得最大值
		int nMax = getProperty_Integer(m_uidTarget, m_data.nPropertyType+1);

		int nValue = 0;
		switch (m_data.nValueType)
		{
		case MA_VALUE_NUM:
			{
				nValue = m_data.nAddValue;
			}
			break;
		case MA_VALUE_PER:
			{
				nValue = (int)(m_data.nAddValue/EFFECT_RATE*nMax);
			}
			break;
		case MA_VALUE_LOSE_PER:
			{
				nValue = (int)(m_data.nAddValue/EFFECT_RATE*(nMax-nCur));
			}
			break;
		case MA_VALUE_CUR_PER:
			{
				nValue = (int)(m_data.nAddValue*nCur/EFFECT_RATE);
			}
			break;
		default:
			{
				ErrorLn("CEffectContinuedSetProperty error nValueType="<<m_data.nValueType);
				return;
			}
			break;
		}

		if (nValue == 0)
		{
			return;
		}
        
        if(nCur + nValue > nMax)
        {
            nValue = nMax - nCur; 
        }

#ifdef ENTITY_CHECK_SKIN
        if(m_data.nPropertyType == PROPERTY_SKIN)
        {
            TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_uidTarget <<", value="<< nValue);
        }
#endif
		addProperty_Integer(m_uidTarget, m_data.nPropertyType, nValue);
	}

private:
	SCHEME_DATA               m_data;
	UID						  m_uidTarget;			
};
