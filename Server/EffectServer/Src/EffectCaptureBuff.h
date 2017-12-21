/*******************************************************************
** �ļ���:	EffectCaptureBuff.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	2/23/2016
** ��  ��:	1.0
** ��  ��:	Ч��-��ȡBuff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace EFFECT_SERVER;

class CEffectCaptureBuff : public IEffectEx
{
public:
	typedef  EffectServer_CaptureBuff    SCHEME_DATA;

	CEffectCaptureBuff( SCHEME_DATA & data ) : m_data(data)
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

        UID uidOperator = INVALID_UID;
        switch (m_data.nOperatorType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidOperator = context->pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidOperator = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidOperator = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("EffectTimingAddBuff error nOperatorType="<<m_data.nOperatorType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidOperator))
        {
            return false;
        }

        UID uidTarget = INVALID_UID;
        switch (m_data.nTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidTarget = context->pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidTarget = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidTarget = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("EffectTimingAddBuff error nTargetType="<<m_data.nTargetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidTarget))
        {
            return false;
        }

        // ���Ŀ��
        if (!chooseTarget(uidOperator, m_data.nTargetFilter, uidTarget))
        {
            return false;
        }

        // ȡ�ò�����buff��Ϣ
        BuffTypeList buffType = getBuff(uidOperator, m_data.nType);
        if (buffType.nCount <= 0)
        {
            return false;
        }

        // �����µĿ�ʼѡȡ
        for (int i=buffType.nCount-1; i>=0; --i)
        {
            // ȡ�����
            DWORD dwBuffID = buffType.dwBuffID[i];
            DWORD dwBuffLevel = buffType.dwBuffLevel[i];
            int nBuffTime = buffType.nBuffTime[i];

            if (dwBuffID > 0 && dwBuffLevel > 0 && nBuffTime >= 0)
            {
                // ���Ƴ�������buff
                RemoveBuff(uidOperator,dwBuffID,INVALID_UID,uidOperator);

                // ������Ŀ��buff
                AddBuff(uidTarget,dwBuffID,dwBuffLevel,uidTarget,nBuffTime);

				// ������������Buff
				if (m_data.nOperatorBuffID > 0 && m_data.nOperatorBuffLevel > 0)
				{
					AddBuff(uidOperator, m_data.nOperatorBuffID, m_data.nOperatorBuffLevel, uidOperator, 0);
				}

				// ��Ŀ������Buff
				if (m_data.nTargetBuffID > 0 && m_data.nTargetBuffLevel > 0)
				{
					AddBuff(uidTarget, m_data.nTargetBuffID, m_data.nTargetBuffLevel, uidTarget, 0);
				}

                break;
            }
        }
        

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectCaptureBuff(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
