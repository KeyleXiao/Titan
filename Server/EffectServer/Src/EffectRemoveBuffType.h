/*******************************************************************
** 文件名:	EffectRemoveBuffType.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/28/2015
** 版  本:	1.0
** 描  述:	效果-移除Buff类型
需求描述：http://172.16.0.120/redmine/issues/841
实现描述：移除buff类型
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IBuffServer.h"

class CEffectRemoveBuffType:public IEffectEx
{
public:
	typedef  EffectServer_RemoveBuffType    SCHEME_DATA;

	CEffectRemoveBuffType(SCHEME_DATA &data):m_data(data)
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

		__IBuffPart *pBuffPart = (__IBuffPart *)pContext->pEntity->getEntityPart(PART_BUFF);
		if (pBuffPart == NULL)
		{
			return false;
		}

		// 移除者
		UID uidRemove = pContext->pEntity->getUID();
		if (isInvalidUID(uidRemove))
		{
			return false;
		}

		switch (m_data.nTagetType)
		{
		case MA_TAGET_TO_SELF:
			{
				// 增加者
				UID uidAdd = uidRemove;

				// 清除Buff
				pBuffPart->CleanBuff(m_data.nBuffType, uidAdd, uidRemove);
			}
			break;
		case MA_TAGET_TO_TARGET:
			{
				// 增加者
				UID uidAdd = pContext->uidTarget;

				if (isInvalidUID(uidAdd))
				{
					return false;
				}
				// 清除Buff
				pBuffPart->CleanBuff(m_data.nBuffType, uidAdd, uidRemove);
			}
			break;
		case MA_TAGET_TO_BOTH:
			{
				// 移除自己加的Buff
				// 增加者
				UID uidAdd = uidRemove;
				// 清除Buff
				pBuffPart->CleanBuff(m_data.nBuffType, uidAdd, uidRemove);

				// 移除目标加的Buff
				// 增加者
				uidAdd = pContext->uidTarget;
				if (!isInvalidUID(uidAdd))
				{
					// 清除Buff
					pBuffPart->CleanBuff(m_data.nBuffType, uidAdd, uidRemove);
				}
			}
			break;
		case MA_TAGET_TO_ALL:
			{
				// 清除Buff,增加者为0，清除此类所有Buff
				pBuffPart->CleanBuff(m_data.nBuffType, 0, uidRemove);
			}
			break;
		default:
			{
				ErrorLn("CEffectRemoveBuffType error m_data.nTagetType="<<m_data.nTagetType);
				return false;
			}
			break;
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
		return new CEffectRemoveBuffType(m_data);
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
