/*******************************************************************
** �ļ���:	EffectSetCanPassBarrier.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	8/29/2017
** ��  ��:	1.0
** ��  ��:	Ч��-��ͨ����̬�ϰ�
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

    // ��ȡЧ��ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

    // Ч������
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

    // Ч��ֹͣ
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

    // ��¡һ����Ч��
    virtual IEffect *       Clone() 
    {
        return new CEffectSetCanPassBarrier(m_data);
    }

    // �ͷ�
    virtual void			Release() 
    {
        Stop();
        delete this;
    }

private:
    SCHEME_DATA               m_data;
    __IEntity*                m_pEntity;
};
