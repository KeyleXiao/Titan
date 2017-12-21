/*******************************************************************
** �ļ���:	EffectRoll.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	9/20/2016
** ��  ��:	1.0
** ��  ��:	Ч��-����Ч��
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "SpellMessageDef.h"

using namespace EFFECT_SERVER;

class CEffectRoll : public IEffectEx
{
public:
	typedef  EffectServer_Roll    SCHEME_DATA;

	CEffectRoll( SCHEME_DATA & data ) : m_data(data)
	{
	}

    // ��ȡЧ��ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// Ч������
	virtual bool			Start( EFFECT_CONTEXT *pContext,CONDITION_CONTEXT *pConditionContext )
	{
		if (pContext == NULL || pContext->pEntity == NULL)
		{
			return false;
		}

        SetRollEffect(pContext);

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectRoll(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

	bool SetRollEffect(EFFECT_CONTEXT *pContext)
	{
        if (pContext == NULL || pContext->pEntity == NULL)
        {
            return false;
        }
        __IEntity *pEntity = pContext->pEntity;

        // ��ö�ʱ����ӿ�
        ITimeSyncService *pTimeSyncService = gServerGlobal->getTimeSyncService();
        if (pTimeSyncService == NULL)
        {
            return false;
        }

        UID uidOperator = pEntity->getUID();
        // ȡ�ò����߱�־λ
        int nOperatorMask = getProperty_Integer(uidOperator, PROPERTY_MASK);
        // ���ɻ���
        if ((nOperatorMask&MASKFLAG_NOT_BACK) == MASKFLAG_NOT_BACK)
        {
            return false;
        }

        int nPassive = 0;
        switch (m_data.nPassiveMoveType)
        {
        case PASSIVE_MOVE_DEFAULT:
            {
                nPassive = 0;
            }
            break;
        case PASSIVE_MOVE_NO:
            {
                nPassive = 0;
            }
            break;
        case PASSIVE_MOVE_YES:
            {
                nPassive = 1;
            }
            break;
        default:
            break;
        }

        ShiftCtx data;
        data.dwStartTick = pTimeSyncService->GetTick();
        data.fStartSpeed = m_data.fSpeed;
        data.shiftMode = shift_mode_forward_loc;
        data.nPriority = m_data.nPriority;
        data.nAnimation = MOVE_ROLL;
        data.uidTarget = uidOperator;
        data.maxTime = m_data.nMaxTime;
        data.fHeight = m_data.fHeight;
        data.bUseHeight = m_data.bUseHeight;
        data.bPassive = nPassive == 1 ? true : false;
        data.fCloseDistance = m_data.fLocationDistance;
        data.dwSpellStartTick = pContext->dwStartTick;

        data.nMaxUpAngle = m_data.nMaxUpAngle;
        data.bCanCrossObstacle = m_data.bCanCrossObstacle;
        data.nHitObstacleAddBuffID = m_data.nHitObstacleAddBuffID;
        data.nHitObstacleAddBuffLevel = m_data.nHitObstacleAddBuffLevel;

        data.nFinishBuff_ID = m_data.nFinishBuff_ID;
        data.nFinishBuff_Level = m_data.nFinishBuff_Level;
        if(data.nFinishBuff_ID > 0)
        {
            data.uidFinishBuff_Target = getTarget(pContext, m_data.nFinishBuff_Target);
            data.uidFinishBuff_Operator = getTarget(pContext, m_data.nFinishBuff_Operator);
        } 

        shift(uidOperator, &data, sizeof(data));

        return true;
	}

private:
    UID getTarget(EFFECT_CONTEXT* pContext, int nType)
    {
        switch (nType)
        {
        case MA_TAGET_TO_SELF:
            {
                if(pContext->pEntity == NULL)
                {
                    return INVALID_UID;
                }
                return pContext->pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                return pContext->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                return pContext->uidOperator;
            }
            break;
        }

        return INVALID_UID;
    }

private:
	SCHEME_DATA               m_data;
};
