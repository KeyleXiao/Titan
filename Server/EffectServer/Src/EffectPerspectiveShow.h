/*******************************************************************
** �ļ���:	EffectPerspectiveShow.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	22/09/2016
** ��  ��:	1.0
** ��  ��:	Ч��-͸����ʾ
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

class CEffectPerspectiveShow : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_PerspectiveShow    SCHEME_DATA;

	CEffectPerspectiveShow( SCHEME_DATA & data ) : m_data(data), m_pEntity(NULL)
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

		if (m_data.nCount <= 0)
		{
			g_EHelper.SetTimer(0, m_data.nTime, this, INFINITY_CALL, "CEffectPerspectiveShow");
		}
		else
		{
			g_EHelper.SetTimer(0, m_data.nTime, this, m_data.nCount, "CEffectPerspectiveShow");
		}

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != NULL)
		{
			g_EHelper.KillTimer(0, this);

            SPerspectiveShow_SC PerspectiveShow;
            PerspectiveShow.bShow = false;
            PerspectiveShow.nCount = 0;
            g_EHelper.playerSend2Client(m_pEntity, PART_SPELL, SPELL_MSG_PERSPECTIVE_SHOW, (char *)&PerspectiveShow, sizeof(PerspectiveShow));

			m_pEntity = NULL;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectPerspectiveShow(m_data);
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

		// �Լ�UID
        UID uidSelf = m_pEntity->getUID();

        /** ȡ��ʵ��
		*/
		int nSceneID = UID_2_SCENE(uidSelf);
		SceneServiceHelper sceneHelper(nSceneID);
		if ( sceneHelper.m_ptr==0 )
		{
			return;
		}
        
		AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
		int num = sceneHelper.m_ptr->k_nearest(m_pEntity->getLocation(), m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true);
		if ( num <= 0 )
        {
			return;
        }

        // ����BUFF����
        int nChooseCount = 0;
        UID uidChoose[MAX_INTEREST_OBJECT] = {0};
		for ( int i=0; i<num; ++i )
		{
            if (m_data.nMaxCount != 0 && nChooseCount >= m_data.nMaxCount)
            {
                break;
            }

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

            uidChoose[nChooseCount++] = uidTarget;
		}

        SPerspectiveShow_SC PerspectiveShow;
        PerspectiveShow.bShow = true;
        PerspectiveShow.nCount = nChooseCount;

        obuf data;
        data.push_back(&PerspectiveShow, sizeof(PerspectiveShow));
        data.push_back(&uidChoose[0], nChooseCount*sizeof(UID));
        g_EHelper.playerSend2Client(m_pEntity, PART_SPELL, SPELL_MSG_PERSPECTIVE_SHOW, data.data(), data.size());
	}

private:
	SCHEME_DATA               m_data;

	__IEntity			      *m_pEntity;
};
