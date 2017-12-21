/*******************************************************************
** �ļ���:	EffectRepulse.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	9/20/2016
** ��  ��:	1.0
** ��  ��:	Ч��-����,����Ч��
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "SpellMessageDef.h"

using namespace EFFECT_SERVER;

class CEffectRepulse : public IEffectEx
{
public:
	typedef  EffectServer_Repulse    SCHEME_DATA;

	CEffectRepulse( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_uidOperator(0),m_uidTarget(0),
		m_vTargetStartTile(0,0,0),m_vTargetEndTile(0,0,0)
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
        m_pEntity = pContext->pEntity;

        m_uidOperator = pContext->uidOperator;

        m_uidTarget = getTarget(pContext, m_data.nTargetType);
        if (isInvalidUID(m_uidTarget))
        {
            return false;
        }

        SetRepulseEffect(pContext);

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
        if (m_pEntity != 0)
        {
            m_pEntity = 0;
        }
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectRepulse(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

	bool SetRepulseEffect(EFFECT_CONTEXT *pContext)
	{
        if (m_pEntity == NULL)
        {
            return false;
        }

		if (pContext == NULL)
		{
			return false;
		}

        // ��ö�ʱ����ӿ�
        ITimeSyncService *pTimeSyncService = gServerGlobal->getTimeSyncService();
        if (pTimeSyncService == NULL)
        {
            return false;
        }

        // ȡ��Ŀ���־λ
        int nTargetMask = getProperty_Integer(m_uidTarget, PROPERTY_MASK);
        // ���ɻ���
        if ((nTargetMask&MASKFLAG_NOT_BACK) == MASKFLAG_NOT_BACK)
        {
            return false;
        }

        Vector3 vTargetLoc = getLocation(m_uidTarget);

		// ���λ��
		Vector3 ptTargetStartTile(0,0,0);
        switch (m_data.nStartPosType)
        {
        case CHOOS_POS_TILE:
            {
                ptTargetStartTile = pContext->ptTargetTile;
            }
            break;
        case CHOOS_POS_SELF:
            {
                ptTargetStartTile = m_pEntity->getLocation();
            }
            break;
        case CHOOS_POS_TARGET:
            {
                UID uidTarget = pContext->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }
                ptTargetStartTile = getLocation(uidTarget);
            }
            break;
        case CHOOS_POS_OPERATOR:
            {
                UID uidOperator = pContext->uidOperator;
                if (isInvalidUID(uidOperator))
                {
                    return false;
                }
                ptTargetStartTile = getLocation(uidOperator);
            }
            break;
        case UCHOOS_POS_ANGLE:
            {
                ptTargetStartTile = vTargetLoc;
            }
            break;
        default:
            {
                ErrorLn("CEffectRepulse error nStartPosType="<<m_data.nStartPosType);
                return false;
            }
            break;
        }

        // �յ�λ��
        Vector3 ptTargetEndTile(0,0,0);
        switch (m_data.nEndPosType)
        {
        case CHOOS_POS_TILE:
            {
                ptTargetEndTile = pContext->ptTargetTile;
            }
            break;
        case CHOOS_POS_SELF:
            {
                ptTargetEndTile = m_pEntity->getLocation();
            }
            break;
        case CHOOS_POS_TARGET:
            {
                UID uidTarget = pContext->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }
                ptTargetEndTile = getLocation(uidTarget);
            }
            break;
        case CHOOS_POS_OPERATOR:
            {
                UID uidOperator = pContext->uidOperator;
                if (isInvalidUID(uidOperator))
                {
                    return false;
                }
                ptTargetEndTile = getLocation(uidOperator);
            }
            break;
        case UCHOOS_POS_ANGLE:
            {
                Vector3 angle = getAngle(m_uidTarget);

                float fAngleY = (float)(angle.y*fPI/180);

                Vector3 dir(0,0,0);
                dir.x = sin(fAngleY);
                dir.y = 0;
                dir.z = cos(fAngleY);

                ptTargetEndTile = ptTargetStartTile + dir.normalize();
            }
            break;
        case UCHOOS_POS_OPERATOR_TARGET_DIR:
            {
                UID uidOperator = pContext->uidOperator;
                if (isInvalidUID(uidOperator))
                {
                    return false;
                }

                UID uidTarget = pContext->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }
                Vector3 vOperatorTile = getLocation(pContext->uidOperator);
                Vector3 vTargetTile = getLocation(pContext->uidTarget);

                if(vTargetTile == vOperatorTile)
                {
                    // �غϵĻ� ����ȡ����������
                    Vector3 angle = getAngle(pContext->uidOperator);
                    float fAngleY = (float)(angle.y*fPI/180);

                    Vector3 dir(0,0,0);
                    dir.x = sin(fAngleY);
                    dir.y = 0;
                    dir.z = cos(fAngleY);
                    ptTargetEndTile = ptTargetStartTile + dir.normalize(); 
                }
                else
                {
                    ptTargetEndTile = ptTargetStartTile + (vTargetTile - vOperatorTile).normalize();
                }
            }
            break;
        case UCHOOS_POS_TARGET_OPERATOR_DIR:
            {
                UID uidOperator = pContext->uidOperator;
                if (isInvalidUID(uidOperator))
                {
                    return false;
                }

                UID uidTarget = pContext->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }
                Vector3 vOperatorTile = getLocation(pContext->uidOperator);
                Vector3 vTargetTile = getLocation(pContext->uidTarget);

                if(vTargetTile == vOperatorTile)
                {
                    // �غϵĻ� ����ȡĿ������
                    Vector3 angle = getAngle(pContext->uidTarget);
                    float fAngleY = (float)(angle.y*fPI/180);

                    Vector3 dir(0,0,0);
                    dir.x = sin(fAngleY);
                    dir.y = 0;
                    dir.z = cos(fAngleY);
                    ptTargetEndTile = ptTargetStartTile + dir.normalize(); 
                }
                else
                {
                    ptTargetEndTile = ptTargetStartTile + (vOperatorTile - vTargetTile).normalize();
                }
            }
            break;
        default:
            {
                ErrorLn("CEffectRepulse error nEndPosType="<<m_data.nEndPosType);
                return false;
            }
            break;
        }

        // ����
        Vector3 dirNomalize = (ptTargetEndTile - ptTargetStartTile).normalize();
        

        // Ŀ�꿪ʼ��
        m_vTargetStartTile = ptTargetStartTile;
        // ���Ŀ���
        m_vTargetEndTile = ptTargetStartTile + dirNomalize*m_data.fDistance;
        // �ƶ�����
        MOVE_TYPE nMoveType = MOVE_BACK;
        // �߶�С��0,��ʾ����,�����ǻ���
        if (m_data.fHeight >= 0.01f)
        {
            // ���û���
            nMoveType = MOVE_FLY;
        }

        int nPassive = 0;
        switch (m_data.nPassiveMoveType)
        {
        case PASSIVE_MOVE_DEFAULT:
            {
                nPassive = 1;
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
        data.bKeepLookDir = true;
        data.fStartSpeed = m_data.fSpeed;
        data.shiftMode = shift_mode_forward_loc;
        data.nPriority = m_data.nPriority;
        data.nAnimation = nMoveType;
        data.uidTarget = m_uidTarget;
        data.maxTime = m_data.nMaxTime;
        data.fHeight = m_data.fHeight;
        data.bUseHeight = m_data.bUseHeight;
        data.bPassive = nPassive == 1 ? true : false;
        data.fCloseDistance = m_data.fLocationDistance;
        data.startLoc = getLocation(m_uidTarget);
        data.targetLoc = m_vTargetEndTile;
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

        shift(m_uidTarget, &data, sizeof(data));

        return true;
	}

private:
    UID getTarget(EFFECT_CONTEXT* pContext, int nType)
    {
        switch (nType)
        {
        case MA_TAGET_TO_SELF:
            {
                if(m_pEntity == NULL)
                {
                    return INVALID_UID;
                }
                return m_pEntity->getUID();
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

    __IEntity                 *m_pEntity;

    UID                       m_uidOperator;
    UID                       m_uidTarget;
	
	Vector3					  m_vTargetStartTile;
	Vector3					  m_vTargetEndTile;
};
