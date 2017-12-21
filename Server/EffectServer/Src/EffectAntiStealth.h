/*******************************************************************
** 文件名:	EffectAntiStealth.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	11/02/2015
** 版  本:	1.0
** 描  述:	效果-设置实体反隐形效果(可以侦测隐形单位)
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityAOI.h"
using namespace EFFECT_SERVER;

class CEffectAntiStealth : public IEffectEx
{
public:
    typedef  EffectServer_AntiStealth    SCHEME_DATA;

    CEffectAntiStealth( SCHEME_DATA & data ) : m_data(data),m_pMaster(0)
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

        m_pMaster = context->pEntity;

        __IEntityAOI* pAOIPart = (__IEntityAOI * )m_pMaster->getEntityPart(PART_AOI);
        if (pAOIPart == 0)
        {
            return false;
        }

        pAOIPart->antiStealth(true);

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
                pAOIPart->antiStealth(false);
            }
        }
        m_pMaster = 0;
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectAntiStealth(m_data);
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
