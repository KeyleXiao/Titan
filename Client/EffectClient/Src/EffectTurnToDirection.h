/*******************************************************************
** 文件名:	EffectTurnToDirection.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/28/2015
** 版  本:	1.0
** 描  述:	效果-转向方向
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
#include "EntityViewDef.h"
using namespace EFFECT_CLIENT;
#include "ExternalHelper.h"

class CEffectTurnToDirection: public IEffect
{
public:
	typedef  EffectClient_TurnToDirection    SCHEME_DATA;

	CEffectTurnToDirection(SCHEME_DATA &data):m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start(EFFECT_CONTEXT *pContext)
	{
		if (pContext == NULL || pContext->pEntity == 0)
			return false;
		
		Vector3 ptLoc;
		switch (m_data.nTagetType)
		{
		case TURN_TO_POS_POS_SELF:
			{
				ptLoc = pContext->pEntity->getLocation();
				break;
			}
		case TURN_TO_POS_TARGET:
			{
				if (pContext->uidTarget == INVALID_UID || pContext->uidTarget == 0)
				{
					return false;
				}

				IEntity *pEntity = g_EHelper.GetEntity(pContext->uidTarget);
				if (pEntity == NULL)
				{
					return false;
				}

				ptLoc = pEntity->getLocation();
				break;
			}
        case TURN_TO_POS_TILE:
            {
                ptLoc = pContext->ptTargetTile;
            }
            break;
        case TURN_TO_POS_OPERATOR:
            {
                if (pContext->uidOperator == INVALID_UID || pContext->uidOperator == 0)
                {
                    return false;
                }

                IEntity *pEntity = g_EHelper.GetEntity(pContext->uidOperator);
                if (pEntity == NULL)
                {
                    return false;
                }
                ptLoc = pEntity->getLocation();
            }
            break;
		default:
			{
				return false;
				break;
			}
		}
		cmd_creature_turn_to_direction TurnToDirection;
		TurnToDirection.ptLoc[0] = ptLoc[0];
		TurnToDirection.ptLoc[1] = ptLoc[1];
		TurnToDirection.ptLoc[2] = ptLoc[2];

		// 转向目标
		pContext->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_TURN_TO_DIRECTION, 0, 0, &TurnToDirection, sizeof(TurnToDirection));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectTurnToDirection(m_data); }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID() { return m_data.nID; }

private:
	SCHEME_DATA             m_data;
};
