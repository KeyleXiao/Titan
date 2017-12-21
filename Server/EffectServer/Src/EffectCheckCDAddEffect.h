/*******************************************************************
** 文件名:	EffectCheckCDAddEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/10/30
** 版  本:	1.0
** 描  述:	效果-检测CD增加BUFF
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "TimerHandler.h"
#include "ExternalHelper.h"
#include "EntityHelper.h"
#include "TimerAxis.h"
#include "StringHelper.h"
#include "IFreezeServer.h"

using namespace EFFECT_SERVER;

class CEffectCheckCDAddEffect: public IEffectEx, public rkt::TimerHandler
{
public:
	typedef  EffectServer_CheckCDAddEffect    SCHEME_DATA;

	CEffectCheckCDAddEffect( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_dwStartTime(0),m_uidOperator(0),
		m_uidTarget(0),m_nSlotID(0), m_nSlotType(0), m_isAddSuccess(false), m_pEffect(NULL)
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

        m_nSlotID =context->nSlotIndex;

        m_nSlotType =context->nSlotType;

        // 开始时间
        m_dwStartTime = getTickCountEx();

        m_uidOperator = context->uidOperator;
        if (isInvalidUID(m_uidOperator))
        {
            return false;
        }

        m_uidTarget = context->uidTarget;
        if (isInvalidUID(m_uidTarget))
        {
            return false;
        }

		__IFreezePart* pIFreezePart = (__IFreezePart*)m_pEntity->getEntityPart(PART_FREEZE);
		if (pIFreezePart == NULL)
			return false;

		DWORD dwLeftTime = pIFreezePart->GetFreezeLeftTime(EFreeze_ClassID_Goods, m_data.nFreezeID);
		if (dwLeftTime > 0)
		{
			// 冷却没好，启动定时器加效果
			g_EHelper.SetTimer(0, dwLeftTime, this, 1, "CEffectCheckCDAddEffect", false);
		}
		else
		{
			// 直接加效果
			OnTimer(0);
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity == NULL)
			return;

		if (m_isAddSuccess == false)
		{
			g_EHelper.KillTimer(0, this);
		}
		else
		{
            if (m_pEffect != NULL)
            {
                m_pEffect->Release();
            }
		}

        m_pEntity = 0;
        m_dwStartTime = 0;
        m_uidOperator = 0;
        m_uidTarget = 0;
        m_pEffect = NULL;
        m_isAddSuccess = false;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectCheckCDAddEffect(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
		EFFECT_CONTEXT context;
        context.pEntity = m_pEntity;
		context.uidOperator = m_uidOperator;
		context.uidTarget = m_uidTarget;
		context.nSlotIndex = m_nSlotID;
		context.nSlotType = m_nSlotType;

        IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
        if(pEffectFactory == NULL)
        {
            ErrorLn(__FUNCTION__": -- pEffectFactory == NULL")
            return;
        }

        m_pEffect = pEffectFactory->BuildEffect(m_data.nEffectID);
        if (m_pEffect == NULL)
        {
            // 处理效果加不上的情况
            ErrorLn(__FUNCTION__": -- EffectID = "<< m_data.nEffectID);
            return;	
        }

        if (!m_pEffect->Start(&context, NULL))
        {
            ErrorLn(__FUNCTION__": -- effect cant start EffectID = "<< m_data.nEffectID);
            return;
        }
		m_isAddSuccess = true;
	}

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;

    // 开始时间
    DWORD                     m_dwStartTime;

    // 操作者
    UID                       m_uidOperator;
    // 目标
    UID                       m_uidTarget;

    int                       m_nSlotID;

    int                       m_nSlotType;

	bool					  m_isAddSuccess;

	IEffect *                m_pEffect;
};
