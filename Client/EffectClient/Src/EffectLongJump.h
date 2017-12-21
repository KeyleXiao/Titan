/*******************************************************************
** 文件名:	EffectLongJump
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	4/11/2017
** 版  本:	1.0
** 描  述:	效果-超级跳
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace rkt;
using namespace EFFECT_CLIENT;

class CEffectLongJump : public IEffect
{
public:
	typedef  EffectClient_LongJump    SCHEME_DATA;

	CEffectLongJump( SCHEME_DATA & data ) : m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		cmd_creature_longjump cmd;
        cmd.fUpTime = m_data.fUpTime;
        cmd.fUpHeight = m_data.fUpHeight;
		context->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_LONGJUMP, 0, 0, &cmd, sizeof(cmd));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectLongJump(m_data);
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
	SCHEME_DATA             m_data;
};