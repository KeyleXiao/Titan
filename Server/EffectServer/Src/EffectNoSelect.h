/*******************************************************************
** 文件名:	EffectNoSelect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/20/2015
** 版  本:	1.0
** 描  述:	效果-不可选择
需求描述：将实体设置不可选择状态，也不可对其伤害
实现描述：改变实体标志位
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"

class CEffectNoSelect : public IEffectEx
{
public:
	typedef  EffectServer_NoSelect    SCHEME_DATA;

	CEffectNoSelect( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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
        {
			return false;
        }

        m_pEntity = context->pEntity;

        __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return false;
        }

        // 取得实体标志
        int nMask = pEntityProperty->getProperty_Integer(PROPERTY_MASK);

        // 设置实体标志(设置不可选择状态)
        pEntityProperty->setProperty_Integer(PROPERTY_MASK, (nMask|MASKFLAG_NOT_SELECT));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
            if (pEntityProperty != NULL)
            {
                // 取得实体标志
                int nMask = pEntityProperty->getProperty_Integer(PROPERTY_MASK);

                // 设置实体标志(取消不可选择状态)
                pEntityProperty->setProperty_Integer(PROPERTY_MASK, (nMask&(~MASKFLAG_NOT_SELECT)));
            }
        }
        m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectNoSelect(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

    __IEntity               *m_pEntity;
};
