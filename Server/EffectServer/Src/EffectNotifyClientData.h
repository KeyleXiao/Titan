/*******************************************************************
** 文件名:	EffectNotifyClientData.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	3/8/2017
** 版  本:	1.0
** 描  述:	数据通知客户端
            http://172.16.0.120/redmine/issues/3939
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ICommonPart.h"

using namespace EFFECT_SERVER;

class CEffectNotifyClientData : public IEffectEx
{
public:
	typedef  EffectServer_NotifyClientData    SCHEME_DATA;

	CEffectNotifyClientData( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}

        m_pEntity = context->pEntity;
        // 人物
        if (m_pEntity->getType() == TYPE_PLAYER_ROLE)
        {
             __ICommonPart * pPart = (__ICommonPart *)m_pEntity->getEntityPart(PART_COMMON);
            if(pPart)
            {
                pPart->notifyClientData(m_data.nType, m_data.nValue);
            }
        }

        return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectNotifyClientData(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
    __IEntity                 *m_pEntity;
};
