/*******************************************************************
** 文件名:	EffectKeepInsight.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	6/23/2016
** 版  本:	1.0
** 描  述:	效果-保持可见
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityAOI.h"
using namespace EFFECT_SERVER;

class CEffectKeepInsight : public IEffectEx
{
public:
    typedef  EffectServer_KeepInsight    SCHEME_DATA;

    CEffectKeepInsight( SCHEME_DATA & data ) : m_data(data),m_pMaster(0),m_nCampMask(0)
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

        m_nCampMask = 0;

        __IEntityAOI* pAOIPart = (__IEntityAOI * )context->pEntity->getEntityPart(PART_AOI);
        if (pAOIPart == 0)
        {
            return false;
        }

        m_pMaster = context->pEntity;

        int nCamp = m_pMaster->getIntProperty(PROPERTY_CAMP); 

        switch (m_data.eCamp)
        {
        case MA_CAMP_RS_FRIEND:
            {
                m_nCampMask = CAMP_MASK(nCamp);
            }
            break;
        case MA_CAMP_RS_ENEMY:
            {
                m_nCampMask = (CAMP_EXCEPT_MASK(nCamp) & CAMP_EXCEPT_MASK(CAMP_NEUTRAL));
            }
            break;
        case MA_CAMP_RS_ALL:
            {
                m_nCampMask = CAMP_MASK_ALL;
            }
            break;
        default:
            {
                ErrorLn("CEffectKeepInsight error camp="<<m_data.eCamp);
                return false;
            }
            break;
        }
        
        for(int camp = 0; camp < CAMP_COUNT_MAX; ++camp)
        {
            if(CAMP_MASK(camp) & m_nCampMask)
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
                for(int camp = 0; camp < CAMP_COUNT_MAX; ++camp)
                {
                    if(CAMP_MASK(camp) & m_nCampMask)
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
        return new CEffectKeepInsight(m_data);
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
    int                       m_nCampMask;
};
