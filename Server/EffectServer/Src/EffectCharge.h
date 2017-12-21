/*******************************************************************
** 文件名:	EffectCharge.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/20/2016
** 版  本:	1.0
** 描  述:	效果-冲锋效果
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "SpellMessageDef.h"

using namespace EFFECT_SERVER;

class CEffectCharge : public IEffectEx
{
public:
	typedef  EffectServer_Charge    SCHEME_DATA;

	CEffectCharge( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_uidOperator(0),m_uidTarget(0),
		m_vOperatorStartTile(0,0,0),m_vOperatorEndTile(0,0,0)
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

        m_uidOperator = getTarget(pContext, m_data.nOperatorType);
        if (isInvalidUID(m_uidOperator))
        {
            return false;
        }

        m_uidTarget = getTarget(pContext, m_data.nTargetType);
        if (isInvalidUID(m_uidTarget))
        {
            return false;
        }

        SetChargeEffect(pContext);

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
		return new CEffectCharge(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

	bool SetChargeEffect(EFFECT_CONTEXT *pContext)
	{
        if (m_pEntity == NULL || pContext == NULL)
        {
            return false;
        }

        // 获得对时服务接口
        ITimeSyncService *pTimeSyncService = gServerGlobal->getTimeSyncService();
        if (pTimeSyncService == NULL)
        {
            return false;
        }

        // 取得操作者标志位
        int nOperatorMask = getProperty_Integer(m_uidOperator, PROPERTY_MASK);
        // 不可击退
        if ((nOperatorMask&MASKFLAG_NOT_BACK) == MASKFLAG_NOT_BACK)
        {
            return false;
        }

		// 起点位置
		Vector3 ptOperatorStartTile(0,0,0);
        switch (m_data.nStartPosType)
        {
        case CHOOS_POS_TILE:
            {
                ptOperatorStartTile = pContext->ptTargetTile;
            }
            break;
        case CHOOS_POS_SELF:
            {
                ptOperatorStartTile = m_pEntity->getLocation();
            }
            break;
        case CHOOS_POS_TARGET:
            {
                ptOperatorStartTile = getLocation(pContext->uidTarget);
            }
            break;
        case CHOOS_POS_OPERATOR:
            {
                ptOperatorStartTile = getLocation(pContext->uidOperator);
            }
            break;
        case UCHOOS_POS_ANGLE:
            {
                ptOperatorStartTile = getLocation(m_uidOperator);
            }
            break;
        default:
            {
                ErrorLn("CEffectCharge error nStartPosType="<<m_data.nStartPosType);
                return false;
            }
            break;
        }

        // 终点位置
        Vector3 ptOperatorEndTile(0,0,0);
        switch (m_data.nEndPosType)
        {
        case CHOOS_POS_TILE:
            {
                ptOperatorEndTile = pContext->ptTargetTile;
            }
            break;
        case CHOOS_POS_SELF:
            {
                ptOperatorEndTile = m_pEntity->getLocation();
            }
            break;
        case CHOOS_POS_TARGET:
            {
                ptOperatorEndTile = getLocation(pContext->uidTarget);
            }
            break;
        case CHOOS_POS_OPERATOR:
            {
                ptOperatorEndTile = getLocation(pContext->uidOperator);
            }
            break;
        case UCHOOS_POS_ANGLE:
            {
                Vector3 angle = getAngle(m_uidOperator);

                float fAngleY = (float)(angle.y*fPI/180);

                Vector3 dir(0,0,0);
                dir.x = sin(fAngleY);
                dir.y = 0;
                dir.z = cos(fAngleY);

                ptOperatorEndTile = ptOperatorStartTile + dir.normalize()*m_data.fDistance;
            }
            break;
        default:
            {
                ErrorLn("CEffectCharge error nEndPosType="<<m_data.nEndPosType);
                return false;
            }
            break;
        }

        // 小于后退距离，算出点会往后，不冲锋
        if ((ptOperatorEndTile - ptOperatorStartTile).getLength() <= abs(m_data.fLocationDistance))
        {
            return false;
        }

        // 方向
        Vector3 dirNomalize = (ptOperatorEndTile - ptOperatorStartTile).normalize();

        // 如果选的是冲锋点，就信任客户端的点，不然冲不上坡等等问题
        if (m_data.nEndPosType == CHOOS_POS_TILE)
        {
            ptOperatorEndTile = ptOperatorEndTile + dirNomalize*m_data.fLocationDistance;
        }
        
        m_vOperatorStartTile = ptOperatorStartTile;
        m_vOperatorEndTile = ptOperatorEndTile;

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
        data.shiftMode = m_data.nLockTarget == 1 ? shift_mode_forward_entity : shift_mode_forward_loc;
        data.nPriority = m_data.nPriority;
        data.nAnimation = MOVE_CHARGE;
        data.uidTarget = m_uidTarget;
        data.maxTime = m_data.nMaxTime;
        data.fHeight = m_data.fHeight;
        data.bUseHeight = m_data.bUseHeight;
        data.bPassive = nPassive == 1 ? true : false;
        data.fCloseDistance = m_data.fLocationDistance;
        data.fBreakDistance = m_data.fBreakDistance;
        data.startLoc = m_vOperatorStartTile;
        data.targetLoc = m_vOperatorEndTile;
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

        shift(m_uidOperator, &data, sizeof(data));

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
	
	Vector3					  m_vOperatorStartTile;
	Vector3					  m_vOperatorEndTile;
};
