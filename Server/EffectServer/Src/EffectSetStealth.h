/*******************************************************************
** 文件名:	EffectSetStealth.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	11/02/2015
** 版  本:	1.0
** 描  述:	效果-设置实体隐形
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityAOI.h"
using namespace EFFECT_SERVER;

class CEffectSetStealth : public IEffectEx
{
public:
    typedef  EffectServer_SetStealth    SCHEME_DATA;

    CEffectSetStealth( SCHEME_DATA & data ) : m_data(data),m_pMaster(0)
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

        if (!m_data.nAllStealth)
        {
            pAOIPart->setStealth(true, m_data.nDisp == STEALTH_ALPHA, m_data.nID);
        }
        else
        {
            pAOIPart->setGhostState(true);
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
                if (!m_data.nAllStealth)
                {
                    pAOIPart->setStealth(false, m_data.nDisp == STEALTH_ALPHA, m_data.nID);
                }
                else
                {
                    pAOIPart->setGhostState(false);
                }
            }
        }
        m_pMaster = 0;
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectSetStealth(m_data);
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
