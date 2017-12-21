/*******************************************************************
** 文件名:	EffectBeatBack.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/09/2015
** 版  本:	1.0
** 描  述:	效果-击退
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

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// 效果启用
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

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != 0)
        {
            m_pEntity = 0;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectBeatBack(m_data);
	}

	// 释放
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

        // 获得对时服务接口
        ITimeSyncService *pTimeSyncService = gServerGlobal->getTimeSyncService();
        if (pTimeSyncService == NULL)
        {
            return false;
        }

        // 操作者对象
        m_uidOperator = getTarget(pContext, m_data.nOperatorType);
        if (isInvalidUID(m_uidOperator))
        {
            return false;
        }

        // 目标对象
        m_uidTarget = getTarget(pContext, m_data.nTargetType);

		// 取得自己点
		Vector3 myLoc = getLocation(m_uidOperator);
		// 方向
		Vector3 dirNomalize(0,0,0);
		// 取得目标点位置
		Vector3 ptTargetStartTile(0,0,0);

        bool bKeepLookDir = false;

		if (isInvalidUID(m_uidTarget))
		{
            if ( m_data.nMoveType == MOVE_BACK/*击退*/ || m_data.nMoveType == MOVE_FLY/*击飞*/ || m_data.nMoveType == MOVE_STRIKE/*撞飞*/ )
            {
                return false;
            }

            // 移动位置类型
			switch (m_data.nType)
			{
			case MOVE_LOC_TILE:     // 移动到目标点
				{
					m_vTargetTile = pContext->ptTargetTile;
				}
				break;
            case MOVE_LOC_DIR:      // 向目标点方向移动
                {
                    // 取得朝向角度
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

			// 如果是0向量，取玩家面向
			if (equalto0(dirNomalize.getLength()))
			{
				// 取得朝向角度
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
			case MOVE_LOC_TILE:     // 移动到目标点
				{
					m_vTargetTile = pContext->ptTargetTile;
				}
				break;
			case MOVE_LOC_TARGET:   // 移动到目标实体
				{
					m_vTargetTile = getLocation(m_uidTarget);
				}
				break;
            case MOVE_LOC_DIR:      // 向目标点方向移动
                {
                    // 取得朝向角度
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

			// 如果是0向量，取玩家面向
			if (equalto0(dirNomalize.getLength()))
			{
				// 取得朝向角度
				Vector3 angle = m_pEntity->getAngle();

				float fAngleY = (float)(angle.y*fPI/180);

				dirNomalize.x = sin(fAngleY);
				dirNomalize.y = 0;
				dirNomalize.z = cos(fAngleY);
			}

			switch (m_data.nMoveType)
			{
				// 冲锋
			case MOVE_CHARGE:
				{
					// 小于后退距离，算出点会往后，不冲锋
					if ((m_vTargetTile - myLoc).getLength() <= abs(m_data.fLocationDistance))
					{
						return false;
					}

                    // 算出自己点(把人和目标点距离，不然重合)
                    m_vTargetTile = m_vTargetTile + dirNomalize*m_data.fLocationDistance;

                    if ((m_vTargetTile - myLoc).getLength() > m_data.fDistance+m_data.fLocationDistance)
                    {
                        // 如果选的是冲锋点，就信任客户端的点，不然冲不上坡等等问题
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
				// 击退
			case MOVE_BACK:
				{
					// 击退时自己坐标不变
					m_vOperatorTile = myLoc;

					//// 算出目标点
                    switch (m_data.nMoveDir)    // 移动方向类型
                    {
                    case MOVE_DIR_LEFT:     // 朝左
                        {
                            float fConfigDist = abs(m_data.fDistance);
                            // ZX平面的距离
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

                    case MOVE_DIR_RIGHT:    // 朝右
                        {
                            float fConfigDist = abs(m_data.fDistance);
                            // ZX平面的距离
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
				// 击飞
			case MOVE_FLY:
				{
					// 击飞时自己坐标不变
					m_vOperatorTile = myLoc;
					// 击飞目标为原点，不处理，只是客户端表现腾空
				}
				break;
                // 撞飞
			case MOVE_STRIKE:
				{
					// 算出目标点
					m_vTargetTile = m_vTargetTile + dirNomalize*m_data.fDistance;
					// 算出自己点(把人和目标点距离，不然重合)
                    m_vOperatorTile = m_vTargetTile + dirNomalize*m_data.fLocationDistance;
				}
				break;
				// 翻滚
			case MOVE_ROLL:
				{
					// 翻滚取客户端目标位置
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

		// 取得操作者标志位
		int nOperatorMask = getProperty_Integer(m_uidOperator, PROPERTY_MASK);
		// 取得目标标志位
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

        // 计算起始点坐标和对象
		switch (m_data.nMoveType)
		{
			// 冲锋
		case MOVE_CHARGE:
			{
				// 非不可击退
				if ((nOperatorMask & MASKFLAG_NOT_BACK) != MASKFLAG_NOT_BACK)
				{
                    data.startLoc = m_vOperatorStartTile;
                    data.targetLoc = m_vOperatorTile;

                    uid = m_uidOperator;
                }
			}
			break;
			// 击退
		case MOVE_BACK:
			{
				// 非不可击退
				if ((nTargetMask & MASKFLAG_NOT_BACK) != MASKFLAG_NOT_BACK)
				{
                    data.startLoc = m_vTargetStartTile;
                    data.targetLoc = m_vTargetTile;

                    uid = m_uidTarget;
                }
			}
			break;
			// 击飞
		case MOVE_FLY:
			{
				// 非不可击飞
				if ((nTargetMask & MASKFLAG_NOT_FLY) != MASKFLAG_NOT_FLY)
                {
                    data.startLoc = m_vTargetStartTile;
                    data.targetLoc = m_vTargetTile;

                    uid = m_uidTarget;
                }
			}
			break;
			// 撞飞
		case MOVE_STRIKE:
			{
                // 非不可击退
                if ((nOperatorMask & MASKFLAG_NOT_BACK) != MASKFLAG_NOT_BACK)
                {
                    data.startLoc = m_vOperatorStartTile;
                    data.targetLoc = m_vOperatorTile;

                    uid = m_uidOperator;
                }

                // 非不可击退
                if ((nTargetMask & MASKFLAG_NOT_BACK) == MASKFLAG_NOT_BACK)
                {
                    data.startLoc = m_vTargetStartTile;
                    data.targetLoc = m_vTargetTile;

                    uid = m_uidTarget;
                }
			}
			break;
			// 翻滚
		case MOVE_ROLL:
			{
				// 非不可击退
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

        // 执行冲锋位移
        shift(uid, &data, sizeof(data));

        return true;
	}

private:
    UID getTarget(EFFECT_CONTEXT* pContext, BEAT_BACK_TARGET_TYPE eType)
    {
        // 目标对象
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
