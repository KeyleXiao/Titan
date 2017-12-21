/*******************************************************************
** 文件名:	EffectSetProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/01/2015
** 版  本:	1.0
** 描  述:	效果-设置属性-血量,魔法,体力,能量等
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"

class CEffectSetProperty : public IEffectEx
{
public:
	typedef  EffectServer_SetProperty    SCHEME_DATA;

	CEffectSetProperty( SCHEME_DATA & data ) : m_data(data)
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

		__IEntityProperty *pProperty = (__IEntityProperty * )context->pEntity->getEntityPart(PART_PROPERTY);
		if (pProperty == 0)
		{
			return false;
		}

		// 不改变，就不处理了
		if (m_data.nValue == 0)
		{
			return true;
		}

#ifdef ENTITY_CHECK_SKIN
        if(m_data.nPropertyType == PROPERTY_SKIN)
        {
            TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< context->pEntity->getUID() <<", value="<< m_data.nValue);
        }
#endif
		// 增加属性
		pProperty->addProperty_Integer(m_data.nPropertyType, m_data.nValue);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetProperty(m_data);
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
