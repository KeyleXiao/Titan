/*******************************************************************
** 文件名:	EffectTimingAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/10/2015
** 版  本:	1.0
** 描  述:	效果-定时增加Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "EntityHelper.h"
#include "BuffDef.h"

class CEffectTimingAddBuff : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_TimingAddBuff SCHEME_DATA;

	CEffectTimingAddBuff( SCHEME_DATA & data ) : m_data(data), m_uid(INVALID_UID), m_uidAdd(INVALID_UID)
	{
        memset(&m_BuffContext, 0, sizeof(m_BuffContext));
	}

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
	{
		if( context == 0 || context->pEntity == NULL)
			return false;

        m_uid = INVALID_UID;
        switch (m_data.nTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                m_uid = context->pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uid = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                m_uid = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("EffectTimingAddBuff error nTargetType="<<m_data.nTargetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(m_uid))
        {
            return false;
        }

        // 增加buff者
        m_uidAdd = INVALID_UID;
        switch (m_data.nAddTargetType)
        {
        case MA_TAGET_TO_SELF:
        case MA_TAGET_TO_OPERATOR:
            {
                m_uidAdd = context->uidOperator;
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uidAdd = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_ALL:
            {
                m_uidAdd = INVALID_UID;
            }
            break;
        default:
            {
                ErrorLn("EffectTimingAddBuff error nAddTargetType="<<m_data.nAddTargetType);
                return false;
            }
            break;
        }

        m_BuffContext.nAttackCount = context->nAttackCount;
        m_BuffContext.ptTargetTile = context->ptTargetTile;
        m_BuffContext.fFocoEnergiaPCT = context->fFocoEnergiaPCT;
        m_BuffContext.nID = context->nID;

		// 首次执行，也算次数
		int nCount = 0;
		// 首次立即执行
		if (m_data.nFirst > 0)
		{
			OnTimer(0);

			nCount = 1;
		}

		if(m_data.nCount >= 1)
		{
			int nTimerCount = m_data.nCount - nCount;
			if (nTimerCount <= 0)
			{
				return true;
			}

			g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectTimingAddBuff");
		}
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectTimingAddBuff");
        }
		
		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		g_EHelper.KillTimer(0, this);
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectTimingAddBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
        switch (m_data.nType)
        {
        case BUFF_ADD:
            {
                if (isInvalidUID(m_uidAdd))
                {
                    return;
                }
                // 给实体添加buff
                AddBuff(m_uid, m_data.nBuffID, m_data.nBuffLevel, m_uidAdd, 0, &m_BuffContext, sizeof(m_BuffContext));
            }
            break;
        case BUFF_REMOVE:
            {
                // 移除时m_uidAdd为INVALID_UID表示移除所有m_data.nBuffID，所以不判断
                // 给实体移除buff
                RemoveBuff(m_uid, m_data.nBuffID, m_uidAdd, m_uidAdd);
            }
            break;
        case BUFF_OVERLAY:
            {
                if (isInvalidUID(m_uidAdd))
                {
                    return;
                }
                // 叠加BUFF
                SetAccumulateBuff(m_uid, m_data.nBuffID, m_uidAdd, true, 0, &m_BuffContext, sizeof(m_BuffContext));
            }
            break;
        case BUFF_DECREASE:
            {
                // 移除时m_uidAdd为INVALID_UID表示移除所有m_data.nBuffID，所以不判断
                // 递减BUFF
                SetAccumulateBuff(m_uid, m_data.nBuffID, m_uidAdd, false, 0, &m_BuffContext, sizeof(m_BuffContext));
            }
            break;
        default:
            break;
        }
	}

private:
	SCHEME_DATA               m_data;
	// UID
	UID						  m_uid;
	// Buff增加者UID
	UID						  m_uidAdd;
    // Buff数据
    SBuffContext              m_BuffContext;
};
