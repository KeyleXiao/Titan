/*******************************************************************
** 文件名:	EffectUpgrade.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/07/2016
** 版  本:	1.0
** 描  述:	效果-持续升级
            http://172.16.0.120/redmine/issues/1521
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "EntityHelper.h"

class CEffectUpgrade : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_Upgrade SCHEME_DATA;

	CEffectUpgrade( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
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

			g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectUpgrade");
		}
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectUpgrade");
        }
		
		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		g_EHelper.KillTimer(0, this);
        m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectUpgrade(m_data);
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

        // 取得等级
        int nLevel = pEntityProperty->getProperty_Integer(PROPERTY_LEVEL);
        // 设置实体等级
        pEntityProperty->setProperty_Integer(PROPERTY_LEVEL,nLevel+m_data.nValue);	
	}

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;
};
