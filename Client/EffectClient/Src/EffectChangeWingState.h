/*******************************************************************
** 文件名:	EffectChangeWingState.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/20/2016
** 版  本:	1.0
** 描  述:	效果-改变翅膀状态
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IWingPart.h"
using namespace EFFECT_CLIENT;

class CEffectChangeWingState : public IEffect
{
public:
	typedef EffectClient_ChangeWingState SCHEME_DATA;

	CEffectChangeWingState( SCHEME_DATA & data ) : m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

        // 改变翅膀状态
        cmd_Entity_Change_Wing_State ChangeWingState;
        ChangeWingState.nType = m_data.nType;
        sstrcpyn(ChangeWingState.szPath, m_data.szPath, sizeof(ChangeWingState.szPath));
        ChangeWingState.fSpeed = m_data.fSpeed;
        ChangeWingState.fAcceleration = m_data.fAcceleration;
        ChangeWingState.fMaxSpeed = m_data.fMaxSpeed;
        ChangeWingState.fMinFlyHeight = m_data.fMinFlyHeight;
        ChangeWingState.fMaxFlyHeight = m_data.fMaxFlyHeight;
        ChangeWingState.fUpSpeed = m_data.fUpSpeed;
        ChangeWingState.fMinCameraAngle = m_data.fMinCameraAngle;
        ChangeWingState.fMaxCameraAngle = m_data.fMaxCameraAngle;
        ChangeWingState.fCameraDistance = m_data.fCameraDistance;
        
        if(pContext->pEntity->isHero())
        {
            IWingPart* pWingPart = (IWingPart*)pContext->pEntity->getEntityPart(PART_WING);
            if(pWingPart != NULL)
            {
                SWing* pWing = pWingPart->getWing();
                if(pWing != NULL)
                {
                    ChangeWingState.nEncounterDistance = pWing->nEncounterDistance;
                }
            }
        }
        
        pContext->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_CHANGE_WING_STATE, 0, 0, &ChangeWingState, sizeof(ChangeWingState));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectChangeWingState(m_data); }

	// 取得效果ID
	virtual int				GetEffectID(){ return m_data.nID; }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
};
