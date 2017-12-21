/*******************************************************************
** 文件名:	EffectDecBulletCount.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	7/21/2017
** 版  本:	1.0
** 描  述:	效果-减少子弹数量
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityProperty.h"
using namespace EFFECT_SERVER;

class CEffectDecBulletCount : public IEffectEx
{
public:
	typedef  EffectServer_DecBulletCount    SCHEME_DATA;

	CEffectDecBulletCount( SCHEME_DATA & data ) : m_data(data)
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
		if( context==0 || context->pEntity==0 )
		{
			return false;
		}

		__IEntityProperty* pProperty = (__IEntityProperty*)(context->pEntity->getEntityPart(PART_PROPERTY));

		int nOriCount = pProperty->getProperty_Integer(PROPERTY_BULLET_COUNT);
		int nNowCount = nOriCount - m_data.nCount;
		if(nNowCount < 0)
		{
			nNowCount = 0;
		}

		pProperty->setProperty_Integer(PROPERTY_BULLET_COUNT, nNowCount);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectDecBulletCount(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
