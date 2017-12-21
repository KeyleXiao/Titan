/*******************************************************************
** 文件名:	EffectContinuedAddProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/28/2016
** 版  本:	1.0
** 描  述:	效果-持续增加属性
需求描述：http://172.16.0.120/redmine/issues/2129
实现描述：持续增加属性
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"

class CEffectContinuedAddProperty : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_ContinuedAddProperty SCHEME_DATA;

	CEffectContinuedAddProperty( SCHEME_DATA & data ) : m_data(data), m_pEntity(0)
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
		if( context==0 || context->pEntity == 0)
			return false;

		m_pEntity = context->pEntity;

		// 首次执行，也算次数
		int nCount = 0;
		// 首次执行
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

			g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectContinuedAddProperty");
		}
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectContinuedAddProperty");
        }
		
		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        g_EHelper.KillTimer(0, this);

        m_pEntity = 0;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectContinuedAddProperty(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		if (m_pEntity == NULL)
		{
            return;
		}

        __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return;
        }
        // 增加属性
        pEntityProperty->addProperty_Integer(m_data.nType, m_data.nValue);
	}

private:
	SCHEME_DATA               m_data;

	__IEntity                 *m_pEntity;	
};
