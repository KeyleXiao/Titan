/*******************************************************************
** 文件名:	EffectChangeGodEyeMode.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/15/2015
** 版  本:	1.0
** 描  述:	效果-改变上帝模式
            http://172.16.0.120/redmine/issues/1453
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_CLIENT;

class CEffectChangeGodEyeMode : public IEffect
{
public:
	typedef  EffectClient_ChangeGodEyeMode    SCHEME_DATA;

	CEffectChangeGodEyeMode( SCHEME_DATA & data ) : m_data(data)
	{

	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if (context == 0 || context->pEntity == 0)
		{
			return false;
		}

		cmd_creature_change_god_eye_mode ChangeGodEyeMode;
		ChangeGodEyeMode.nEnter = m_data.nEnter;
		context->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_CHANGE_GOD_EYE_MODE, 0, 0, &ChangeGodEyeMode, sizeof(ChangeGodEyeMode));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectChangeGodEyeMode(m_data); }

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
