/*******************************************************************
** 文件名:	EffectAutoMove.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	7/4/2017
** 版  本:	1.0
** 描  述:	#4970 自动前进效果
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "IFreezeClient.h "
using namespace EFFECT_CLIENT;

class CEffectAutoMove : public IEffect
{
public:
	typedef EffectClient_AutoMove SCHEME_DATA;

	CEffectAutoMove(SCHEME_DATA & data) :m_data(data), m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start(EFFECT_CONTEXT * pContext)
	{
		if (pContext == 0 || pContext->pEntity == 0)
			return false;

		if (!pContext->pEntity->isHero())
			return false;

		m_pEntity = pContext->pEntity;

		cmd_auto_move cmd;
		cmd.nOpenFlag = 1;
		cmd.fStartSpeed = m_data.fStartSpeed;
		cmd.fAcceleration = m_data.fAcceleration;
		cmd.fMaxSpeed = m_data.fMaxSpeed;

		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_AUTO_MOVE, 0, 0, &cmd, sizeof(cmd));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != NULL)
		{
			cmd_auto_move cmd;
			cmd.nOpenFlag = 0;
			cmd.fStartSpeed = m_data.fStartSpeed;
			cmd.fAcceleration = m_data.fAcceleration;
			cmd.fMaxSpeed = m_data.fMaxSpeed;

			m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_AUTO_MOVE, 0, 0, &cmd, sizeof(cmd));

			m_pEntity = NULL;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectAutoMove(m_data); }

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
	IEntity*				m_pEntity;
};