/*******************************************************************
** 文件名:	EffectCheckTargetRange.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/30/2015
** 版  本:	1.0
** 描  述:	效果-检测目标范围
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectCheckTargetRange : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_CheckTargetRange    SCHEME_DATA;

	CEffectCheckTargetRange( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_dwStartTime(0),m_uidTarget(0),m_nSpellID(0),m_bAddFlag(false)
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

        UID uidTarget = INVALID_UID;
        switch (m_data.nTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidTarget = context->pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidTarget = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidTarget = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
        }

        if(isInvalidUID(uidTarget))
        {
            return false;
        }
        Stop();

        m_uidTarget = uidTarget;
        m_pEntity = context->pEntity;
        m_nSpellID =context->nID;

        // 开始时间
        m_dwStartTime = getTickCountEx();

        m_bAddFlag = false;
        g_EHelper.SetTimer(0, m_data.nTime, this, INFINITY_CALL, "CEffectCheckTargetRange", false);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        g_EHelper.KillTimer(0, this);

        m_pEntity = 0;
        m_dwStartTime = 0;
        m_uidTarget = 0;
        m_bAddFlag = false;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectCheckTargetRange(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        bool bCheckDistanceResult = checkTargetDistance();

        // 检测距离 添加buff
        if(!m_bAddFlag && bCheckDistanceResult)
        {
            SBuffContext BuffContext;
            BuffContext.nID = m_nSpellID;
            AddBuff(m_uidTarget, m_data.nBuffID, m_data.nBuffLevel, m_pEntity->getUID(), 0, &BuffContext, sizeof(BuffContext));
            m_bAddFlag = true;
        }

        // 离开删除buff
        if(m_bAddFlag && !bCheckDistanceResult && m_data.nRemoveBuffOnTargetLeave == MA_TRUE)
        {
            RemoveBuff(m_uidTarget, m_data.nBuffID, m_pEntity->getUID(), m_pEntity->getUID());
            Stop();
            return;
        }

        // 检测是否结束
        if(getTickCountEx() > m_dwStartTime + m_data.nDuration)
        {
            if(checkTargetDistance())
            {
                SBuffContext BuffContext;
                BuffContext.nID = m_nSpellID;
                // 给目标添加结束buff
                AddBuff(m_uidTarget, m_data.nBuffID_Final, m_data.nBuffLevel_Final, m_pEntity->getUID(), 0, &BuffContext, sizeof(BuffContext));
            }

            Stop();
            return;
        }
    }

    // 检测目标的距离
    bool checkTargetDistance()
    {
        if(m_pEntity == NULL)
        {
            return false;
        }

        Vector3 locSelf = m_pEntity->getLocation();

        Vector3 locTarget = getLocation(m_uidTarget);

        bool bInside = locSelf.checkDistance(locTarget, m_data.fDistance);

        if(m_data.nType == CHECK_TARGET_RANGE_OUTSIDE)
        {
            return !bInside;
        }
        else
        {
            return bInside;
        }
    }

    // 技能调用时 是否马上停止
    virtual bool            StopImmAsSpellEffect()
    {
        return false;
    }

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;

    // 开始时间
    DWORD                     m_dwStartTime;

    // 目标
    UID                       m_uidTarget;

    int                       m_nSpellID;

    // 是否已经给目标添加了buff
    bool                      m_bAddFlag;
};
