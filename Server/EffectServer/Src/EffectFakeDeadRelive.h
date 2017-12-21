/*******************************************************************
** 文件名:	EffectFakeDeadRelive.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/12/1
** 版  本:	1.0
** 描  述:	效果-假死者复活
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "IWarPart.h"
#include "WarHelper.h"

class CEffectFakeDeadRelive: public IEffectEx
{
public:
	typedef  EffectServer_FakeDeadRelive    SCHEME_DATA;

	CEffectFakeDeadRelive( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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

		__IWarPart* pWarPart = (__IWarPart*)m_pEntity->getEntityPart(PART_WAR);
		if (pWarPart == NULL)
		{
			return false;
		}

		DWORD dwWarID = pWarPart->getWarID();
		if (dwWarID == INVALID_WAR_ID)
		{
			return false;
		}

		WarHelper warHelper(dwWarID);
		IWarService* pWarService = warHelper.m_ptr;
		if (pWarService == NULL)
		{
			return false;
		}

		pWarService->onFakeDeadRelive(m_pEntity->getUID());

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
		return new CEffectFakeDeadRelive(m_data);
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
