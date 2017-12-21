/*******************************************************************
** 文件名:	EffectGlobalVisible.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	9/20/2016
** 版  本:	1.0
** 描  述:	效果-全图可见
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityAOI.h"
using namespace EFFECT_SERVER;

class CEffectGlobalVisible : public IEffectEx
{
public:
    typedef  EffectServer_GlobalVisible    SCHEME_DATA;

    CEffectGlobalVisible( SCHEME_DATA & data ) : m_data(data),m_pMaster(0)
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

        __IEntityAOI* pAOIPart = (__IEntityAOI * )context->pEntity->getEntityPart(PART_AOI);
        if (pAOIPart == 0)
        {
            return false;
        }

        m_pMaster = context->pEntity;

        int m_nSelfCamp = m_pMaster->getIntProperty(PROPERTY_CAMP);
        for(int camp = 0; camp < CAMP_COUNT_MAX; ++camp)
        {
            if(camp != m_nSelfCamp)
            {
                pAOIPart->keepInsight(camp, true);
            }
        }

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
        if(m_pMaster != 0)
        {
            __IEntityAOI* pAOIPart = (__IEntityAOI * )m_pMaster->getEntityPart(PART_AOI);
            if (pAOIPart != 0)
            {
                int m_nSelfCamp = m_pMaster->getIntProperty(PROPERTY_CAMP);
                for(int camp = 0; camp < CAMP_COUNT_MAX; ++camp)
                {
                    if(camp != m_nSelfCamp)
                    {
                        pAOIPart->keepInsight(camp, false);
                    }
                }
            }            
        }
        m_pMaster = 0;
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectGlobalVisible(m_data);
    }

    // 释放
    virtual void			Release() 
    {
        Stop();
        delete this;
    }

private:
    SCHEME_DATA               m_data;
    __IEntity*                m_pMaster;
};
