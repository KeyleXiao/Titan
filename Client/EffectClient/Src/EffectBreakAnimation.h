/*******************************************************************
** 文件名:	EffectBreakAnimation.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/09/2015
** 版  本:	1.0
** 描  述:	效果-打断动作
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectBreakAnimation : public IEffect
{
public:
	typedef  EffectClient_BreakAnimation    SCHEME_DATA;

	CEffectBreakAnimation( SCHEME_DATA & data ) : m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

        cmd_creature_stopAnimation cmd;
        sstrcpyn(cmd.szName, m_data.strName, sizeof(cmd.szName));

		// 中断正在播放的攻击动作
		context->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_BREAK_ATTACK_ANIMATION, 0, 0, &cmd, sizeof(cmd));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectBreakAnimation(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID()
	{
		return m_data.nID;
	}

private:
	SCHEME_DATA               m_data;
};
