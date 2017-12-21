/*******************************************************************
** 文件名:	EffectAddEnmityValue.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	10/28/2015
** 版  本:	1.0
** 描  述:	给目标添加仇恨
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEnmityServer.h"
#include "EntityHelper.h"

class CEffectAddEnmityValue:public IEffectEx
{
public:
	typedef  EffectServer_AddEnmity   SCHEME_DATA;

	CEffectAddEnmityValue(SCHEME_DATA &data):m_data(data)
	{
	}

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// 效果启用
	virtual bool			Start(EFFECT_CONTEXT *pContext,CONDITION_CONTEXT *pConditionContext)
	{
		if (pContext == NULL || pContext->pEntity == NULL)
		{
			return false;
		}

		//被击中的实体id
		UID uidTarget=pContext->uidTarget;
		//效果自身，操作者id
		UID uidOperator=pContext->uidOperator;

		if (isInvalidUID(uidTarget) || isInvalidUID(uidOperator))
		{
			return false;
		}
        //增加仇恨uidTarget
		AddEnmityValue(uidTarget,uidOperator,m_data.nEnmityValue);
		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAddEnmityValue(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
};
