/*******************************************************************
** 文件名:	CEffectSetSight.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	11/02/2015
** 版  本:	1.0
** 描  述:	效果-设置实体视野距离
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityAOI.h"
using namespace EFFECT_SERVER;

class CEffectSetSight : public IEffectEx
{
public:
    typedef  EffectServer_SetSight    SCHEME_DATA;

    CEffectSetSight( SCHEME_DATA & data ) : m_data(data),m_pMaster(NULL),m_fAdd(0)
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
        if (pAOIPart == NULL)
        {
            return false;
        }

        m_pMaster = context->pEntity;
        float fOriSight = pAOIPart->getSight();
        m_fAdd = m_data.fSight - fOriSight;

        pAOIPart->setSight(m_data.fSight);

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
        if(m_pMaster == NULL)
        {
            return;
        }

        __IEntityAOI* pAOIPart = (__IEntityAOI * )m_pMaster->getEntityPart(PART_AOI);
        if (pAOIPart == NULL)
        {
            return;
        }

        pAOIPart->setSight(pAOIPart->getSight() - m_fAdd);
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectSetSight(m_data);
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
    float                     m_fAdd;
};
