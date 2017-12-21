/*******************************************************************
** 文件名:	EffectCoolDown.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/03/2015
** 版  本:	1.0
** 描  述:	效果-冷却
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IFreezeClient.h"
#include "FreezeDef.h"
using namespace EFFECT_CLIENT;

class CEffectCoolDown : public IEffect
{
public:
	typedef  EffectClient_CoolDown    SCHEME_DATA;

	CEffectCoolDown( SCHEME_DATA & data ) : m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		// 取得冷却部件
		IFreezePart *pFreezePart = (IFreezePart*)context->pEntity->getEntityPart(PART_FREEZE);
		if (pFreezePart == NULL)
		{
			return false;
		}

		//  设置冷却
		return pFreezePart->Work(m_data.nClassID, m_data.nCoolDown);
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectCoolDown(m_data);
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
