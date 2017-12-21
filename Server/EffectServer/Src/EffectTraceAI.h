/*******************************************************************
** 文件名:	EffectTraceAI.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	ljw
** 日  期:	5/11/2017
** 版  本:	1.0
** 描  述:	trace ai 信息
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
#include "EntityHelper.h"
#include "IAIService.h"
using namespace EFFECT_SERVER;

class CEffectTraceAI : public IEffectEx
{
public:
	typedef  EffectServer_TraceAI    SCHEME_DATA;

	CEffectTraceAI( SCHEME_DATA & data ) : m_data(data), m_pEntity(0)
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
        if(context->pEntity == NULL)
            return false;

        m_pEntity = context->pEntity;

        UID uidSelf = m_pEntity->getUID();
        int nSceneID = UID_2_SCENE(uidSelf);

        AIServiceHelper aiHelper(nSceneID);
        if(aiHelper.m_ptr)
        {
            aiHelper.m_ptr->traceAIInfo(uidSelf, true);
        }

        return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if(m_pEntity == NULL)
            return;

        UID uidSelf = m_pEntity->getUID();
        int nSceneID = UID_2_SCENE(uidSelf);

        AIServiceHelper aiHelper(nSceneID);
        if(aiHelper.m_ptr)
        {
            aiHelper.m_ptr->traceAIInfo(uidSelf, false);
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectTraceAI(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
    __IEntity*                m_pEntity;
};
