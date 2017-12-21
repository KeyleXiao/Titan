/*******************************************************************
** 文件名:	EffectCoolDown.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/29
** 版  本:	1.0
** 描  述:	进入冷却
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IFreezeServer.h"
#include "FreezeDef.h"
using namespace EFFECT_SERVER;

class CEffectCoolDown : public IEffectEx
{
public:
	typedef  EffectServer_CoolDown    SCHEME_DATA;

	CEffectCoolDown( SCHEME_DATA & data ) : m_data(data)
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

		// 取得冷却部件
		__IFreezePart *pFreezePart = (__IFreezePart*)context->pEntity->getEntityPart(PART_FREEZE);
		if (pFreezePart == NULL)
		{
			return false;
		}

		//  设置冷却
		return pFreezePart->Work(m_data.nClassID, m_data.nCoolDownID);
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

private:
	SCHEME_DATA               m_data;
};
