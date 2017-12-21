/*******************************************************************
** �ļ���:	EffectCheckTargetFilterAddBuff.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	7/06/2016
** ��  ��:	1.0
** ��  ��:	Ч��-���Ŀ���������buff
            http://172.16.0.120/redmine/issues/2062
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "TimerHandler.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "ICamp.h"
#include "IEntityAOI.h"

using namespace EFFECT_SERVER;

class CEffectCheckTargetFilterAddBuff : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_CheckTargetFilterAddBuff    SCHEME_DATA;

	CEffectCheckTargetFilterAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL),m_nSpellID(0)
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}

		m_pEntity = context->pEntity;

        m_nSpellID = context->nID;

        // �״�ִ�У�Ҳ�����
        int nCount = 0;
        // �״�����ִ��
        if (m_data.nFirst > 0)
        {
            OnTimer(0);

            nCount = 1;
        }

        if(m_data.nCount >= 1)
        {
            int nTimerCount = m_data.nCount - nCount;
            if (nTimerCount <= 0)
            {
                return true;
            }

            g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectCheckDistanceAddBuff");
        }
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectCheckDistanceAddBuff");
        }

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != NULL)
		{
			g_EHelper.KillTimer(0, this);

			m_pEntity = NULL;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectCheckTargetFilterAddBuff(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		if (m_pEntity == NULL)
		{
			return;
		}
		// ����״̬�����ټӷ�ΧBUFF
		if (m_pEntity->getState() == ENTITY_STATE_DIE)
		{
			return;
		}

        // �Լ�UID
        UID uidSelf = m_pEntity->getUID();
        // ȡ���Լ�λ��
        Vector3 vSelfLoc = m_pEntity->getLocation();

         /** ȡ��ʵ��
		*/
		int nSceneID = UID_2_SCENE(uidSelf);
		SceneServiceHelper sceneHelper(nSceneID);
		if ( sceneHelper.m_ptr==0 )
		{
			return;
		}
        
		AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
        int num = sceneHelper.m_ptr->k_nearest(vSelfLoc, m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true );
        if ( num<=0 )
        {
            return;
        }

        SBuffContext BuffContext;
        BuffContext.nID = m_nSpellID;

		for ( int i=0; i<num; ++i )
		{
			// Ŀ��UID
			UID uidTarget = pProxyArray[i]->m_uid;
			if (isInvalidUID(uidTarget))
			{
				continue;
			}

			// ���Ŀ��
			if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
			{
				continue;
			}

			// ����buff
			AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
            break;
		}
	}

private:
	SCHEME_DATA               m_data;

	__IEntity			      *m_pEntity;

    int                       m_nSpellID;
};
