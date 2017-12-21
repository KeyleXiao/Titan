/*******************************************************************
** 文件名:	EffectJump.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/30/2015
** 版  本:	1.0
** 描  述:	效果-跳跃
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectJump : public IEffectEx
{
public:
	typedef  EffectServer_Jump    SCHEME_DATA;

	CEffectJump( SCHEME_DATA & data ) : m_data(data)
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

        __IEntity *pEntity = context->pEntity;

        msg_creature_act_sync actSync;
        actSync.nCtrID = Jump;

        g_EHelper.broadCastInAOI(pEntity, pEntity->getLocation(), PART_MOVE, SC_MSG_ENTITY_CTRL, (char *)&actSync, sizeof(actSync));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectJump(m_data);
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
