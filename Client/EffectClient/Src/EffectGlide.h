/*******************************************************************
** 文件名:	EffectGlide.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/13/2017
** 版  本:	1.0
** 描  述:	效果-滑翔功能
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace rkt;
using namespace EFFECT_CLIENT;

class CEffectGlide : public IEffect
{
public:
	typedef  EffectClient_Glide    SCHEME_DATA;

	CEffectGlide( SCHEME_DATA & data ) : m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		cmd_creature_glide glide;
        glide.fGlideSpeed = m_data.fGlideSpeed;
        glide.fFallSpeed = m_data.fFallSpeed;
		context->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_GLIDE, 0, 0, &glide, sizeof(glide));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectGlide(m_data);
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