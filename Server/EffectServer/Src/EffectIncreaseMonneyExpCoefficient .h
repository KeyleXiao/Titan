/*******************************************************************
** 文件名:	CEffectIncreaseMonneyExpCoefficient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	9/29/2015
** 版  本:	1.0
** 描  述:	效果---金币经验效果提升（只要启用效果就设置金钱经验系数）
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"
#include "IAttackObject.h"
#include "buffer.h"
#include "IGameSceneService.h"
#include "EventDef.h"
class CEffectIncreaseMonneyExpCoefficient: public IEffectEx
{
public:
	typedef  EffectServer_DieIncreaseMonneyExp    SCHEME_DATA;

	CEffectIncreaseMonneyExpCoefficient( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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
		//只要启动效果（Buff/技能/天赋调用），就对目标实体设置额外的金币和经验系数
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;

		UID uidSelf = m_pEntity->getUID();
		if (isInvalidUID(uidSelf))
		{
			return false;
		}

		//对击杀者给予设置金币系数
		setProperty_Integer( uidSelf,ENTITY_PROPERTY::PROPERTY_MONNEY_COEFFICIENT , m_data.nMonneyCoefficient );

		//对击杀者给予设置经验系数
		setProperty_Integer( uidSelf,ENTITY_PROPERTY::PROPERTY_EXP_COEFFICIENT , m_data.nExpCoefficient);


		return true;
	}


	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			UID uidSelf = m_pEntity->getUID();
			if (isInvalidUID(uidSelf))
			{
				return;
			}
			//效果结束，系数恢复默认值0
			setProperty_Integer( uidSelf,ENTITY_PROPERTY::PROPERTY_MONNEY_COEFFICIENT ,0);

			setProperty_Integer( uidSelf,ENTITY_PROPERTY::PROPERTY_EXP_COEFFICIENT ,0);

			m_pEntity = 0;
		}
		return;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectIncreaseMonneyExpCoefficient(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:

	SCHEME_DATA             m_data;
	__IEntity				*m_pEntity;
};
