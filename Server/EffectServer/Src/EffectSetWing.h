/*******************************************************************
** 文件名:	EffectSetWing.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	效果-设置翅膀
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IWingPart.h"

using namespace EFFECT_SERVER;

class CEffectSetWing : public IEffectEx
{
public:
	typedef  EffectServer_SetWing    SCHEME_DATA;

	CEffectSetWing( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
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

		__IWingPart *pWingPart = (__IWingPart *)m_pEntity->getEntityPart(PART_WING);
		if (pWingPart == NULL)
		{
			return false;
		}
		// 设置翅膀ID
		return pWingPart->SetWingID(m_data.nWingID);
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            __IWingPart *pWingPart = (__IWingPart *)m_pEntity->getEntityPart(PART_WING);
            if (pWingPart != NULL)
            {
                // 清空翅膀设置
                pWingPart->SetWingID(0);
            }

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetWing(m_data);
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
