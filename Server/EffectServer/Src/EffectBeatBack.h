/*******************************************************************
** �ļ���:	EffectBeatBack.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/09/2015
** ��  ��:	1.0
** ��  ��:	Ч��-����
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "SpellMessageDef.h"
#include "ITimeSyncService.h"

using namespace EFFECT_SERVER;

class CEffectBeatBack : public IEffectEx
{
public:
	typedef  EffectServer_BeatBack    SCHEME_DATA;

	CEffectBeatBack( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_uidOperator(0),m_uidTarget(0),
		m_vOperatorStartTile(0,0,0),m_vOperatorTile(0,0,0),m_vTargetStartTile(0,0,0),m_vTargetTile(0,0,0)
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

        SetBeatBackEffect(pContext);

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
		return new CEffectBeatBack(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

	bool SetBeatBackEffect(EFFECT_CONTEXT *pContext)
	{
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

        // �����߶���
        m_uidOperator = getTarget(pContext, m_data.nOperatorType);
        if (isInvalidUID(m_uidOperator))
        {
            return false;
        }

        // Ŀ�����
        m_uidTarget = getTarget(pContext, m_data.nTargetType);

		// ȡ���Լ���
		Vector3 myLoc = getLocation(m_uidOperator);
		// ����
		Vector3 dirNomalize(0,0,0);
		// ȡ��Ŀ���λ��
		Vector3 ptTargetStartTile(0,0,0);

        bool bKeepLookDir = false;

		if (isInvalidUID(m_uidTarget))
		{
            if ( m_data.nMoveType == MOVE_BACK/*����*/ || m_data.nMoveType == MOVE_FLY/*����*/ || m_data.nMoveType == MOVE_STRIKE/*ײ��*/ )
            {
                return false;
            }

            // �ƶ�λ������
			switch (m_data.nType)
			{
			case MOVE_LOC_TILE:     // �ƶ���Ŀ���
				{
					m_vTargetTile = pContext->ptTargetTile;
				}
				break;
            case MOVE_LOC_DIR:      // ��Ŀ��㷽���ƶ�
                {
                    // ȡ�ó���Ƕ�
//                     Vector3 angle = m_pEntity->getAngle();
// 
//                     float fAngleY = (float)(angle.y*fPI/180);
// 
//                     dirNomalize.x = sin(fAngleY);
//                     dirNomalize.y = 0;
//                     dirNomalize.z = cos(fAngleY);

                    dirNomalize = (pContext->ptTargetTile - myLoc).normalize(); 
                    m_vTargetTile = myLoc + dirNomalize*m_data.fDistance;
                }
                break;
			default:
				{
					ErrorLn("InvalidUID The charge not other types,m_data.nType="<<m_data.nType);
					return false;
				}
				break;
			}
			dirNomalize = (m_vTargetTile - myLoc).normalize();

			// �����0������ȡ�������
			if (equalto0(dirNomalize.getLength()))
			{
				// ȡ�ó���Ƕ�
				Vector3 angle = m_pEntity->getAngle();

				float fAngleY = (float)(angle.y*fPI/180);

				dirNomalize.x = sin(fAngleY);
				dirNomalize.y = 0;
				dirNomalize.z = cos(fAngleY);
			}

			m_vOperatorTile = m_vTargetTile + dirNomalize*m_data.fLocationDistance;
		}
		else   // m_uidTarget is valid
		{
            ptTargetStartTile = getLocation(m_uidTarget);

			switch (m_data.nType)
			{
			case MOVE_LOC_TILE:     // �ƶ���Ŀ���
				{
					m_vTargetTile = pContext->ptTargetTile;
				}
				break;
			case MOVE_LOC_TARGET:   // �ƶ���Ŀ��ʵ��
				{
					m_vTargetTile = getLocation(m_uidTarget);
				}
				break;
            case MOVE_LOC_DIR:      // ��Ŀ��㷽���ƶ�
                {
                    // ȡ�ó���Ƕ�
//                     Vector3 angle = m_pEntity->getAngle();
// 
//                     float fAngleY = (float)(angle.y*fPI/180);
// 
//                     dirNomalize.x = sin(fAngleY);
//                     dirNomalize.y = 0;
//                     dirNomalize.z = cos(fAngleY);

                    dirNomalize = (pContext->ptTargetTile - myLoc).normalize();

                    m_vTargetTile = myLoc + dirNomalize*m_data.fDistance;
                }
                break;
			default:
				{
					ErrorLn("The charge not other types,m_data.nType="<<m_data.nType);
					return false;
				}
				break;
			}
			dirNomalize = (m_vTargetTile - myLoc).normalize();

			// �����0������ȡ�������
			if (equalto0(dirNomalize.getLength()))
			{
				// ȡ�ó���Ƕ�
				Vector3 angle = m_pEntity->getAngle();

				float fAngleY = (float)(angle.y*fPI/180);

				dirNomalize.x = sin(fAngleY);
				dirNomalize.y = 0;
				dirNomalize.z = cos(fAngleY);
			}

			switch (m_data.nMoveType)
			{
				// ���
			case MOVE_CHARGE:
				{
					// С�ں��˾��룬���������󣬲����
					if ((m_vTargetTile - myLoc).getLength() <= abs(m_data.fLocationDistance))
					{
						return false;
					}

                    // ����Լ���(���˺�Ŀ�����룬��Ȼ�غ�)
                    m_vTargetTile = m_vTargetTile + dirNomalize*m_data.fLocationDistance;

                    if ((m_vTargetTile - myLoc).getLength() > m_data.fDistance+m_data.fLocationDistance)
                    {
                        // ���ѡ���ǳ��㣬�����οͻ��˵ĵ㣬��Ȼ�岻���µȵ�����
                        if (m_data.nType == MOVE_LOC_TILE)
                        {
                            m_vOperatorTile = m_vTargetTile;
                        }
                        else
                        {
                            m_vOperatorTile = myLoc + dirNomalize*(m_data.fDistance+m_data.fLocationDistance);
                        }
                    }
                    else
                    {
                        m_vOperatorTile = m_vTargetTile;
                    }
				}
				break;
				// ����
			case MOVE_BACK:
				{
					// ����ʱ�Լ����겻��
					m_vOperatorTile = myLoc;

					//// ���Ŀ���
                    switch (m_data.nMoveDir)    // �ƶ���������
                    {
                    case MOVE_DIR_LEFT:     // ����
                        {
                            float fConfigDist = abs(m_data.fDistance);
                            // ZXƽ��ľ���
                            float fDis = m_vTargetTile.getDistance(m_vOperatorTile);
                            if(equalto0(fDis))
                            {
                                m_vTargetTile += dirNomalize*m_data.fDistance;
                            }
                            else
                            {
                                float foffx = ((m_vTargetTile.z - m_vOperatorTile.z) / fDis)*fConfigDist;
                                float foffz = ((m_vTargetTile.x - m_vOperatorTile.x) / fDis)*fConfigDist;

                                Vector3 targetPos;
                                targetPos.x = m_vTargetTile.x - foffx / fDis;
                                targetPos.y = m_vTargetTile.y;
                                targetPos.z = m_vTargetTile.z + foffz / fDis;

                                m_vTargetTile =  targetPos;
                            }
                        }
                        break;

                    case MOVE_DIR_RIGHT:    // ����
                        {
                            float fConfigDist = abs(m_data.fDistance);
                            // ZXƽ��ľ���
                            float fDis = m_vTargetTile.getDistance(m_vOperatorTile);
                            if(equalto0(fDis))
                            {
                                m_vTargetTile += dirNomalize*m_data.fDistance;
                            }
                            else
                            {
                                float foffx = ((m_vTargetTile.z - m_vOperatorTile.z) / fDis)*fConfigDist;
                                float foffz = ((m_vTargetTile.x - m_vOperatorTile.x) / fDis)*fConfigDist;

                                Vector3 targetPos;
                                targetPos.x = m_vTargetTile.x + foffx / fDis;
                                targetPos.y = m_vTargetTile.y;
                                targetPos.z = m_vTargetTile.z - foffz / fDis;

                                m_vTargetTile =  targetPos;
                            }
                        }
                        break;

                    default:
                        {
                            m_vTargetTile += dirNomalize*m_data.fDistance;
                        }
                        break;
                    }
				}
				break;
				// ����
			case MOVE_FLY:
				{
					// ����ʱ�Լ����겻��
					m_vOperatorTile = myLoc;
					// ����Ŀ��Ϊԭ�㣬������ֻ�ǿͻ��˱����ڿ�
				}
				break;
                // ײ��
			case MOVE_STRIKE:
				{
					// ���Ŀ���
					m_vTargetTile = m_vTargetTile + dirNomalize*m_data.fDistance;
					// ����Լ���(���˺�Ŀ�����룬��Ȼ�غ�)
                    m_vOperatorTile = m_vTargetTile + dirNomalize*m_data.fLocationDistance;
				}
				break;
				// ����
			case MOVE_ROLL:
				{
					// ����ȡ�ͻ���Ŀ��λ��
					m_vOperatorTile = m_vTargetTile;
                    bKeepLookDir = true;
				}
				break;
			default:
				break;
			}
		}

        m_vOperatorStartTile = myLoc;
        m_vTargetStartTile = ptTargetStartTile;

        int nPassive = 0;
        switch (m_data.nPassiveMoveType)
        {
        case PASSIVE_MOVE_DEFAULT:
            {
                if (m_data.nMoveType == MOVE_BACK || m_data.nMoveType == MOVE_FLY)
                {
                    nPassive = 1;
                }
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

		// ȡ�ò����߱�־λ
		int nOperatorMask = getProperty_Integer(m_uidOperator, PROPERTY_MASK);
		// ȡ��Ŀ���־λ
		int nTargetMask = getProperty_Integer(m_uidTarget, PROPERTY_MASK);

        ShiftCtx data;
        data.dwStartTick = pTimeSyncService->GetTick();
        data.fStartSpeed = m_data.fSpeed;
        data.bKeepLookDir = bKeepLookDir;
        data.shiftMode = m_data.nLockTarget == 1 ? shift_mode_forward_entity : shift_mode_forward_loc;
        data.nPriority = m_data.nPriority;
        data.nAnimation = m_data.nMoveType;
        data.uidTarget = m_uidTarget;
        data.maxTime = m_data.nMaxTime;
        data.fHeight = m_data.fHeight;
        data.bUseHeight = m_data.bUseHeight;
        data.bPassive = nPassive == 1 ? true : false;
        data.fCloseDistance = m_data.fLocationDistance;
        data.fBreakDistance = m_data.fBreakDistance;
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

        UID uid = INVALID_UID;

        // ������ʼ������Ͷ���
		switch (m_data.nMoveType)
		{
			// ���
		case MOVE_CHARGE:
			{
				// �ǲ��ɻ���
				if ((nOperatorMask & MASKFLAG_NOT_BACK) != MASKFLAG_NOT_BACK)
				{
                    data.startLoc = m_vOperatorStartTile;
                    data.targetLoc = m_vOperatorTile;

                    uid = m_uidOperator;
                }
			}
			break;
			// ����
		case MOVE_BACK:
			{
				// �ǲ��ɻ���
				if ((nTargetMask & MASKFLAG_NOT_BACK) != MASKFLAG_NOT_BACK)
				{
                    data.startLoc = m_vTargetStartTile;
                    data.targetLoc = m_vTargetTile;

                    uid = m_uidTarget;
                }
			}
			break;
			// ����
		case MOVE_FLY:
			{
				// �ǲ��ɻ���
				if ((nTargetMask & MASKFLAG_NOT_FLY) != MASKFLAG_NOT_FLY)
                {
                    data.startLoc = m_vTargetStartTile;
                    data.targetLoc = m_vTargetTile;

                    uid = m_uidTarget;
                }
			}
			break;
			// ײ��
		case MOVE_STRIKE:
			{
                // �ǲ��ɻ���
                if ((nOperatorMask & MASKFLAG_NOT_BACK) != MASKFLAG_NOT_BACK)
                {
                    data.startLoc = m_vOperatorStartTile;
                    data.targetLoc = m_vOperatorTile;

                    uid = m_uidOperator;
                }

                // �ǲ��ɻ���
                if ((nTargetMask & MASKFLAG_NOT_BACK) == MASKFLAG_NOT_BACK)
                {
                    data.startLoc = m_vTargetStartTile;
                    data.targetLoc = m_vTargetTile;

                    uid = m_uidTarget;
                }
			}
			break;
			// ����
		case MOVE_ROLL:
			{
				// �ǲ��ɻ���
				if ((nOperatorMask & MASKFLAG_NOT_BACK) != MASKFLAG_NOT_BACK)
				{
                    data.startLoc = m_vOperatorStartTile;
                    data.targetLoc = m_vOperatorTile;

                    uid = m_uidOperator;
				}
			}
			break;
		default:
			break;
		}

        if (uid == INVALID_UID)
        {
            return false;
        }

        // ִ�г��λ��
        shift(uid, &data, sizeof(data));

        return true;
	}

private:
    UID getTarget(EFFECT_CONTEXT* pContext, BEAT_BACK_TARGET_TYPE eType)
    {
        // Ŀ�����
        switch (eType)
        {
        case BEAT_BACK_TARGET_TO_SELF:
            {
                if(m_pEntity == NULL)
                {
                    return INVALID_UID;
                }
                return m_pEntity->getUID();
            }
            break;
        case BEAT_BACK_TARGET_TO_OPERATOR:
            {
                return pContext->uidOperator;
            }
            break;
        case BEAT_BACK_TARGET_TO_TARGET:
            {
                return pContext->uidTarget;
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

	Vector3					  m_vOperatorStartTile;
	Vector3					  m_vOperatorTile;
	Vector3					  m_vTargetStartTile;
	Vector3					  m_vTargetTile;
};
