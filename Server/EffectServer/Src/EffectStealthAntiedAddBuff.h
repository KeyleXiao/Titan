/*******************************************************************
** �ļ���:	EffectStealthAntiedAddBuff.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	12/18/2017
** ��  ��:	1.0
** ��  ��:	Ч��-���Լ�����������Լ���buff(���ڱ���)
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityAOI.h"
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "EntityHelper.h"
#include "IBuffServer.h"
using namespace EFFECT_SERVER;

class CEffectStealthAntiedAddBuff : public IEffectEx, public IEventExecuteSink
{
public:
    typedef  EffectServer_StealthAntiedAddBuff    SCHEME_DATA;

    CEffectStealthAntiedAddBuff( SCHEME_DATA & data ) : m_data(data),m_pMaster(0),m_bBuffAdded(false)
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

        __IEntityEvent* pEntityEvent = context->pEntity->getEntityEvent();
        if(pEntityEvent == NULL)
        {
            return false;
        }

        m_pMaster = context->pEntity;

        m_bBuffAdded = false;

        // ע���Լ�����������Ӫ��Ұ�¼�
        pEntityEvent->Subscibe(this, EVENT_ENTITY_ENTER_A_CAMP_SIGHT, __FUNCTION__);
        // ע���Լ��뿪������Ӫ��Ұ�¼�
        pEntityEvent->Subscibe(this, EVENT_ENTITY_LEAVE_A_CAMP_SIGHT, __FUNCTION__);
        // ע���Լ�����״̬�ı��¼�
        pEntityEvent->Subscibe(this, EVENT_ENTITY_STEALTH_CHANGE, __FUNCTION__);

        return true;
    }

    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        if(m_pMaster == NULL)
        {
            WarningLn(__FUNCTION__ << __LINE__ << ", m_pMaster is null");
            return;
        }
        switch(wEventID)
        {
        case EVENT_ENTITY_ENTER_A_CAMP_SIGHT:
            {
                // �Ѿ������˷���buff
                if(m_bBuffAdded)
                {
                    return;
                }

                __IEntityAOI* pAOIPart = (__IEntityAOI*)m_pMaster->getEntityPart(PART_AOI);
                if(pAOIPart == NULL)
                {
                    return;
                }

                AOI_PROXY_PTR ptrAOIProxy = pAOIPart->getAOIProxy();
                if(ptrAOIProxy == 0)
                {
                    return;
                }

                // û�п������� �϶����ᱻ����
                if(!ptrAOIProxy->m_bIsStealing)
                {
                    return;
                }

                int nCamp = *((int*)pszContext);
                // ���Լ���Ӫ��Ұ ���㱻����
                if(nCamp == ptrAOIProxy->m_nCamp)
                {
                    return;
                }

                // ��ob��Ӫ��Ұ ���㱻����
                SceneHelper sHelper(m_pMaster->getSceneID());
                if(!sHelper.m_ptr)
                {
                    return;
                }
                int nObCamp = sHelper.m_ptr->getObCampID();
                if(nCamp == nObCamp)
                {
                    return;
                }

                // �Լ��������� ��buff
                __IBuffPart* pBuffPart = (__IBuffPart*)m_pMaster->getEntityPart(PART_BUFF);
                if(pBuffPart != NULL)
                {
                    pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, m_pMaster->getUID());
                }
                m_bBuffAdded = true;
            }
            break;
        case EVENT_ENTITY_LEAVE_A_CAMP_SIGHT:
            {
                if(!m_bBuffAdded)
                {
                    return;
                }

                __IEntityAOI* pAOIPart = (__IEntityAOI*)m_pMaster->getEntityPart(PART_AOI);
                if(pAOIPart == NULL)
                {
                    return;
                }

                AOI_PROXY_PTR ptrAOIProxy = pAOIPart->getAOIProxy();
                if(ptrAOIProxy == 0)
                {
                    return;
                }
                
                // ��ǰ�ɼ���
                int nVisible = ptrAOIProxy->m_nVisibaleFlag;

                // ��ȥ�Լ���Ӫ
                nVisible = nVisible & (~CAMP_MASK(ptrAOIProxy->m_nCamp));

                SceneHelper sHelper(m_pMaster->getSceneID());
                if(sHelper.m_ptr)
                {
                    int nObCamp = sHelper.m_ptr->getObCampID();
                    if(nObCamp > 0)
                    {
                        // ��ȥOB��Ӫ
                        nVisible = nVisible & (~CAMP_MASK(nObCamp));
                    }
                }
                
                // ��������Ӫ���ɼ��� ��û��������
                if(nVisible == 0)
                {
                    // �Ƴ�buff
                    __IBuffPart* pBuffPart = (__IBuffPart*)m_pMaster->getEntityPart(PART_BUFF);
                    if(pBuffPart != NULL)
                    {
                        pBuffPart->Remove(m_data.nBuffID, m_pMaster->getUID(), m_pMaster->getUID());
                    }
                    m_bBuffAdded = false;
                }
            }
            break;
        case EVENT_ENTITY_STEALTH_CHANGE:
            {
                // �����ڼ� �Լ�������Ч�������Ļ������Լ������Ǳ�������
                if(!m_bBuffAdded)
                {
                    return;
                }

                bool bSteathing = *((bool*)pszContext);
                if(!bSteathing)
                {
                    // �Ƴ�buff
                    __IBuffPart* pBuffPart = (__IBuffPart*)m_pMaster->getEntityPart(PART_BUFF);
                    if(pBuffPart != NULL)
                    {
                        pBuffPart->Remove(m_data.nBuffID, m_pMaster->getUID(), m_pMaster->getUID());
                    }
                    m_bBuffAdded = false;
                }
            }
            break;
        }
    }

    // Ч��ֹͣ
    virtual void			Stop()
    {
        if(m_pMaster != 0)
        {
            __IEntityEvent* pEntityEvent = m_pMaster->getEntityEvent();
            if(pEntityEvent != NULL)
            {
                pEntityEvent->UnSubscibe(this, EVENT_ENTITY_ENTER_A_CAMP_SIGHT);
                pEntityEvent->UnSubscibe(this, EVENT_ENTITY_LEAVE_A_CAMP_SIGHT);
                pEntityEvent->UnSubscibe(this, EVENT_ENTITY_STEALTH_CHANGE);
            }
        }
        m_pMaster = 0;
        m_bBuffAdded = false;
    }

    // ��¡һ����Ч��
    virtual IEffect *       Clone() 
    {
        return new CEffectStealthAntiedAddBuff(m_data);
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

    bool                      m_bBuffAdded; // �Ƿ��Ѿ�����buff
};
