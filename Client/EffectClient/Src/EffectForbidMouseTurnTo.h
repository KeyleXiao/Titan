/*******************************************************************
** 文件名:	EffectForbidMouseTurnTo.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	效果-禁止鼠标带动人物转向
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

class CEffectForbidMouseTurnTo: public IEffect
{
public:
	typedef  EffectClient_ForbidMouseTurnTo    SCHEME_DATA;

	CEffectForbidMouseTurnTo(SCHEME_DATA &data):m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start(EFFECT_CONTEXT *pContext)
	{
		if (pContext == NULL || pContext->pEntity == 0)
			return false;

		cmd_creature_forbid_mouse_turn_to ForbidMouseTurnTo;
		ForbidMouseTurnTo.nTime = m_data.nTime;

		// 禁止鼠标带动人物转向
		pContext->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_FORBID_MOUSE_TURN_TO, 0, 0, &ForbidMouseTurnTo, sizeof(ForbidMouseTurnTo));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectForbidMouseTurnTo(m_data); }

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
