/*******************************************************************
** 文件名:	EffectContinuedAddTalentPoint.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/22/2015
** 版  本:	1.0
** 描  述:	效果-持续增加天赋点数
需求描述：http://172.16.0.120/redmine/issues/786
实现描述：持续增长天赋点数，支持N秒内，获得N金币
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectContinuedAddTalentPoint : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_ContinuedAddTalentPoint SCHEME_DATA;

	CEffectContinuedAddTalentPoint( SCHEME_DATA & data ) : m_data(data), m_uidTarget(INVALID_UID)
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

		m_uidTarget = context->pEntity->getUID();
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

		if(m_data.nCount >= 1)
		{
			int nTimerCount = m_data.nCount - nCount;
			if (nTimerCount <= 0)
			{
				return true;
			}

			g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectContinuedAddTalentPoint");
		}
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectContinuedAddTalentPoint");
        }
		
		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        g_EHelper.KillTimer(0, this);

		m_uidTarget = INVALID_UID;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectContinuedAddTalentPoint(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		if (isInvalidUID(m_uidTarget))
		{
			return;
		}

        // 增加天赋点数
        updateTalentPoint(m_uidTarget, m_data.nValue, ENUM_TALENT_EFFECT);
	}

private:
	SCHEME_DATA               m_data;
	// 目标UID
	UID						  m_uidTarget;		
};
