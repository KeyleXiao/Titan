/*******************************************************************
** 文件名:	EffectSetWingProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/13/2017
** 版  本:	1.0
** 描  述:	效果-设置翅膀属性
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IWingPart.h"

using namespace EFFECT_SERVER;

class CEffectSetWingProperty : public IEffectEx
{
public:
	typedef  EffectServer_SetWingProperty    SCHEME_DATA;

	CEffectSetWingProperty( SCHEME_DATA & data ) : m_data(data)
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

		__IWingPart *pWingPart = (__IWingPart *)context->pEntity->getEntityPart(PART_WING);
		if (pWingPart == NULL)
		{
			return false;
		}
		// 设置翅膀属性
		pWingPart->SetWingProperty(m_data.nType, m_data.fValue);

        return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetWingProperty(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
