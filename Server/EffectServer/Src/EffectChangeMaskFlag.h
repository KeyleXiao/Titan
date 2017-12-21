/*******************************************************************
** 文件名:	EffectChangeMaskFlag.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	6/28/2016
** 版  本:	1.0
** 描  述:	效果-改变实体标志
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"

class CEffectChangeMaskFlag : public IEffectEx
{
public:
	typedef  EffectServer_ChangeMaskFlag    SCHEME_DATA;

	CEffectChangeMaskFlag( SCHEME_DATA &data ) : m_data(data),m_pEntity(NULL),m_nChangeMask(0)
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

        if (m_data.bReduce)
        {
            int nNewMask = nMask&(~m_data.nValue);
            // 改变标志位
            m_nChangeMask = nMask - nNewMask;
            // 设置实体标志
            pEntityProperty->setProperty_Integer(PROPERTY_MASK, nNewMask);
        }
        else
        {
            int nNewMask = nMask|m_data.nValue;
            // 改变标志位
            m_nChangeMask = nNewMask - nMask;
            // 设置实体标志
            pEntityProperty->setProperty_Integer(PROPERTY_MASK, nNewMask);
        }

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

                if (m_data.bReduce)
                {
                    // 设置实体标志
                    pEntityProperty->setProperty_Integer(PROPERTY_MASK, nMask|m_nChangeMask);
                }
                else
                {
                    // 设置实体标志
                    pEntityProperty->setProperty_Integer(PROPERTY_MASK, nMask&(~m_nChangeMask));
                }
            }
        }
        m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectChangeMaskFlag(m_data);
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

    // 改变标志位
    int                     m_nChangeMask;
};
