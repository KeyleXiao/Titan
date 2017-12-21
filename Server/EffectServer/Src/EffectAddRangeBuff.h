/*******************************************************************
** �ļ���:	EffectAddRangeBuff.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/09/2015
** ��  ��:	1.0
** ��  ��:	Ч��-���ӷ�Χbuff
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

class CEffectAddRangeBuff : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_AddRangeBuff    SCHEME_DATA;

	CEffectAddRangeBuff( SCHEME_DATA & data ) : m_data(data),m_uidSrcEntity(0),m_pEntity(NULL),m_uidAddBuff(0),m_nSpellID(0)
	{
		// ����buff���ݱ��ʼ��
		m_addBuffDataMap.clear();
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

		__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
		if (pEntityProperty == NULL)
		{
			return false;
		}

		// Դʵ��UID
		m_uidSrcEntity = m_pEntity->getUID();

        switch (m_data.nAddBuffType)
        {
        case MA_TAGET_TO_SELF:
            {
                m_uidAddBuff = m_pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uidAddBuff = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                m_uidAddBuff = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectAddRangeBuff error nAddBuffType="<<m_data.nAddBuffType);
                return false;
            }
            break;
        }
        if (isInvalidUID(m_uidAddBuff))
        {
            return false;
        }

		// �״�ִ��
		int nRemainCount = m_data.nCount;
		if (m_data.nFirst > 0)
		{
			OnTimer(0);
			nRemainCount = m_data.nCount - 1;
		}

		if (m_data.nCount <= 0)
		{

			g_EHelper.SetTimer(0, m_data.nTime, this, INFINITY_CALL, "CEffectAddRangeBuff");
		}
		else
		{
			if (nRemainCount > 0)
			{
				g_EHelper.SetTimer(0, m_data.nTime, this, nRemainCount, "CEffectAddRangeBuff");
			}
		}

        m_nSpellID = context->nID;

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != NULL)
		{
			g_EHelper.KillTimer(0, this);

            // �Ƿ�������
            bool isDie = (m_pEntity->getState()==ENTITY_STATE_DIE);

            // �Ƿ���Ҫ�Ƴ�֮ǰ�����˼ӵ�BUFF(1���뿪��Χ������Buff�� 2�������Ƴ�)
            bool needRemove = (m_data.nLeaveReserveBuff==0) || ((m_data.nMaskFlag & EFFECT_MASK_DIE)  && isDie);

			// �뿪��Χ������Buff�������¼buff����
			if (needRemove)
			{
				ADD_BUFF_DATA_MAP::iterator Iter = m_addBuffDataMap.begin();
				while (Iter != m_addBuffDataMap.end())
				{
					const AddBuffData &data = Iter->second;
                    UID uidRemove = m_uidAddBuff;
                    if (m_data.nAllRemove)
                    {
                        uidRemove = INVALID_UID;
                    }
					// �Ƴ�buff
					RemoveBuff(data.uid, data.nBuffID, uidRemove, uidRemove);

					Iter = m_addBuffDataMap.erase(Iter);
				}
			}
			m_addBuffDataMap.clear();

			m_pEntity = NULL;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectAddRangeBuff(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
        // �Ƿ�������
        bool isDie = (m_pEntity==NULL || m_pEntity->getState()==ENTITY_STATE_DIE);

        // �Ƿ���Ҫ�Ƴ�֮ǰ�����˼ӵ�BUFF(1���뿪��Χ������Buff�� 2�������Ƴ�)
        bool needRemove = (m_data.nLeaveReserveBuff==0) || ((m_data.nMaskFlag & EFFECT_MASK_DIE)  && isDie);

		// �뿪��Χ������Buff,Ҫ�Ѳ��ڷ�Χ��buffȥ��
		if (needRemove)
		{
			// ȡ��ʵ��λ��
			Vector3 locSrc = getLocation(m_uidSrcEntity);

			ADD_BUFF_DATA_MAP::iterator Iter = m_addBuffDataMap.begin();
			while (Iter != m_addBuffDataMap.end())
			{
				const AddBuffData &data = Iter->second;
				// ȡ��ʵ��λ��
				Vector3 locEntity = getLocation(data.uid);

				// ���ڷ�Χ��
				if ( ((m_data.nMaskFlag & EFFECT_MASK_DIE)  && isDie) || ((m_data.nLeaveReserveBuff==0) && !checkDistance(locSrc, locEntity, m_data.fDistance)) )
				{
                    UID uidRemove = m_uidAddBuff;
                    if (m_data.nAllRemove)
                    {
                        uidRemove = INVALID_UID;
                    }
                    // �Ƴ�buff
                    RemoveBuff(data.uid, data.nBuffID, uidRemove, uidRemove);

					Iter = m_addBuffDataMap.erase(Iter);
				}
				else
				{
					++Iter;
				}
			}
		}

		// ����״̬�����ټӷ�ΧBUFF
		if (isDie)
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
		Vector3 locSrc = m_pEntity->getLocation();
		int num = sceneHelper.m_ptr->k_nearest(locSrc, m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL);
		if ( num <= 0 )
			return;

		rkt::uint16 selfVisibaleFlag = 0;
		__IEntityAOI* pEntityAOIPart = (__IEntityAOI*)m_pEntity->getEntityPart(ENTITY_PART::PART_AOI);
		if (pEntityAOIPart)
		{
			selfVisibaleFlag = pEntityAOIPart->getAOIProxy()->m_nVisibaleFlag;
		}
		
		int nCampSelf = m_pEntity->getIntProperty(PROPERTY_CAMP);

        SBuffContext BuffContext;
        BuffContext.nID = m_nSpellID;
        // ����BUFF����
        int nAddBuffCount = 0;
        int nMaxAddBuffCount = m_data.nAddBuffCount == 0 ? EFFECT_CHOOSE_TARGET_COUNT : m_data.nAddBuffCount;
        UID uidMonsterAddBuff[MAX_INTEREST_OBJECT] = {0};
        DWORD dwMonsterCount = 0;
		for ( int i=0; i<num; ++i )
		{
			// Ŀ��UID
			UID uidTarget = pProxyArray[i]->m_uid;
			if (isInvalidUID(uidTarget))
			{
				continue;
			}

            if (nAddBuffCount >= nMaxAddBuffCount)
            {
                break;
            }

			if (this->m_data.nVisible2EachOther)
			{
				bool bCanSeeYou = ((pProxyArray[i]->m_nVisibaleFlag & CAMP_MASK(nCampSelf)) != 0);
				bool bCanSeeMe = ((selfVisibaleFlag & CAMP_MASK(pProxyArray[i]->m_nCamp)) != 0);
				if (!bCanSeeYou || !bCanSeeMe)
				{
					continue;
				}
			}

			if (!checkDistance(locSrc, pProxyArray[i]->m_Loc, m_data.fDistance))
			{
				continue;
			}

			// ���Ŀ��
			if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
			{
				continue;
			}

            // �뿪��Χ������Buff,�Ǵ��ڼ�¼����,������д��ھͲ��ټ�BUFF�����Բ߻���������BUFF������ʱ��BUFF
            if (m_data.nLeaveReserveBuff == 0 && !m_data.nRepeatAdd)
            {
                ADD_BUFF_DATA_MAP::iterator Iter = m_addBuffDataMap.find(uidTarget);
                if (Iter != m_addBuffDataMap.end())
                {
                    continue;
                }
            }

            if (isMonster(uidTarget))
            {
                uidMonsterAddBuff[dwMonsterCount++] = uidTarget;
            }
            else
            {
			    // ����buff
			    AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, m_uidAddBuff, 0, &BuffContext, sizeof(BuffContext));
            }

            ++nAddBuffCount;

			// �뿪��Χ������Buff,Ҫ�Ѽ�Buff���ݼ�¼����
			if (m_data.nLeaveReserveBuff == 0)
			{
				AddBuffData data;
				data.uid = uidTarget;
				data.nBuffID = m_data.nBuffID;
				data.nBuffLevel = m_data.nBuffLevel;
				m_addBuffDataMap[uidTarget] = data;
			}
		}

        if (dwMonsterCount > 0)
        {
            g_EHelper.BatchAddBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nBuffID, m_data.nBuffLevel, m_uidAddBuff, 0, &BuffContext, sizeof(BuffContext));
        }
	}

	bool checkDistance(Vector3 locA, Vector3 locB, float fDistance)
	{
		if (m_data.bIgnoreY)
		{
			locA.y = 0;
			locB.y = 0;
		}

		return locA.checkDistance(locB, fDistance);
	}

private:
	SCHEME_DATA               m_data;

	// Դʵ��UID
	UID						  m_uidSrcEntity;

	struct AddBuffData
	{
		UID		uid;
		int		nBuffID;
		int		nBuffLevel;

		AddBuffData()
		{
			::memset(this, 0, sizeof(*this));
		}
	};
	// ����buff���ݱ�
	typedef map<UID, AddBuffData>		ADD_BUFF_DATA_MAP;
	ADD_BUFF_DATA_MAP			        m_addBuffDataMap;

	__IEntity			*m_pEntity;

    UID                 m_uidAddBuff;

    int                 m_nSpellID;
};
