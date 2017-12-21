/*******************************************************************
** �ļ���:	EffectGlobalVisible.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	�ΰ
** ��  ��:	9/20/2016
** ��  ��:	1.0
** ��  ��:	Ч��-ȫͼ�ɼ�
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

    // Ч��ֹͣ
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

    // ��¡һ����Ч��
    virtual IEffect *       Clone() 
    {
        return new CEffectGlobalVisible(m_data);
    }

    // �ͷ�
    virtual void			Release() 
    {
        Stop();
        delete this;
    }

private:
    SCHEME_DATA               m_data;
    __IEntity*                m_pMaster;
};
