/*******************************************************************
** 文件名:	EffectGoodsRestartCharged.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	10/11/2015
** 版  本:	1.0
** 描  述:	效果-对限制物品重新充能 比如水晶瓶的重新充能
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IGameSceneService.h"
class CEffectGoodsRestartCharged : public IEffectEx
{
public:
	typedef  EffectServer_GoodsRestartCharged    SCHEME_DATA;

	CEffectGoodsRestartCharged( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
	{
	}

	// 获取效果ID
	virtual int             GetID()
	{
		return m_data.nID;
	}


	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;
		// 获取物品部件
		__IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)m_pEntity->getEntityPart(PART_MOBAGOODS);
		if (pMobaGoodsPart == NULL)
		{
			return false;
		}
		// 调用物品部件重置使用次数
		pMobaGoodsPart->ResetCanUseCount();

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
		return new CEffectGoodsRestartCharged(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
