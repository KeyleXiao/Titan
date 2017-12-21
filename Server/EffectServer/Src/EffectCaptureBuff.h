/*******************************************************************
** 文件名:	EffectCaptureBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/23/2016
** 版  本:	1.0
** 描  述:	效果-夺取Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace EFFECT_SERVER;

class CEffectCaptureBuff : public IEffectEx
{
public:
	typedef  EffectServer_CaptureBuff    SCHEME_DATA;

	CEffectCaptureBuff( SCHEME_DATA & data ) : m_data(data)
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}

        UID uidOperator = INVALID_UID;
        switch (m_data.nOperatorType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidOperator = context->pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidOperator = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidOperator = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("EffectTimingAddBuff error nOperatorType="<<m_data.nOperatorType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidOperator))
        {
            return false;
        }

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
                ErrorLn("EffectTimingAddBuff error nTargetType="<<m_data.nTargetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidTarget))
        {
            return false;
        }

        // 检测目标
        if (!chooseTarget(uidOperator, m_data.nTargetFilter, uidTarget))
        {
            return false;
        }

        // 取得操作者buff信息
        BuffTypeList buffType = getBuff(uidOperator, m_data.nType);
        if (buffType.nCount <= 0)
        {
            return false;
        }

        // 从最新的开始选取
        for (int i=buffType.nCount-1; i>=0; --i)
        {
            // 取得玩家
            DWORD dwBuffID = buffType.dwBuffID[i];
            DWORD dwBuffLevel = buffType.dwBuffLevel[i];
            int nBuffTime = buffType.nBuffTime[i];

            if (dwBuffID > 0 && dwBuffLevel > 0 && nBuffTime >= 0)
            {
                // 先移除操作者buff
                RemoveBuff(uidOperator,dwBuffID,INVALID_UID,uidOperator);

                // 给增加目标buff
                AddBuff(uidTarget,dwBuffID,dwBuffLevel,uidTarget,nBuffTime);

				// 给操作者增加Buff
				if (m_data.nOperatorBuffID > 0 && m_data.nOperatorBuffLevel > 0)
				{
					AddBuff(uidOperator, m_data.nOperatorBuffID, m_data.nOperatorBuffLevel, uidOperator, 0);
				}

				// 给目标增加Buff
				if (m_data.nTargetBuffID > 0 && m_data.nTargetBuffLevel > 0)
				{
					AddBuff(uidTarget, m_data.nTargetBuffID, m_data.nTargetBuffLevel, uidTarget, 0);
				}

                break;
            }
        }
        

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectCaptureBuff(m_data);
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
