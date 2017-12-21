/*******************************************************************
** 文件名:	EffectSetCanPassBarrier.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	8/29/2017
** 版  本:	1.0
** 描  述:	效果-可通过动态障碍
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IMovepart.h"

using namespace EFFECT_SERVER;

class CEffectSetCanPassBarrier : public IEffectEx
{
public:
    typedef  EffectServer_SetCanPassBarrier    SCHEME_DATA;

    CEffectSetCanPassBarrier( SCHEME_DATA & data ) : m_data(data), m_pEntity(NULL)
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

        m_pEntity = context->pEntity; 

        __IMovePart* pMovePart = (__IMovePart*)(m_pEntity->getEntityPart(PART_MOVE));
        if(pMovePart != NULL)
        {
            pMovePart->setCanPassBarrier(true);
        }

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
        if(m_pEntity != NULL)
        {
            __IMovePart* pMovePart = (__IMovePart*)(m_pEntity->getEntityPart(PART_MOVE));
            if(pMovePart != NULL)
            {
                pMovePart->setCanPassBarrier(false);
            }

            m_pEntity = NULL;
        }
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectSetCanPassBarrier(m_data);
    }

    // 释放
    virtual void			Release() 
    {
        Stop();
        delete this;
    }

private:
    SCHEME_DATA               m_data;
    __IEntity*                m_pEntity;
};
