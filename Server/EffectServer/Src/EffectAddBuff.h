/*******************************************************************
** 文件名:	EffectAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/23/2015
** 版  本:	1.0
** 描  述:	效果-增加Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "IEntity.h"
#include "IBuffServer.h"
#include "BuffMessageDef.h"
#include "EntityHelper.h"

class CEffectAddBuff : public IEffectEx
{
public:
	typedef  EffectServer_AddBuff    SCHEME_DATA;

	CEffectAddBuff( SCHEME_DATA & data ) : m_data(data)
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
		if( context==0 || context->pEntity==0 )
        {
			return false;
        }
        __IEntity *pEntity = context->pEntity;

        __IBuffPart *pBuffPart = (__IBuffPart *)pEntity->getEntityPart(PART_BUFF);
        if (pBuffPart == NULL)
        {
            return false;
        }

        UID uidSelf = pEntity->getUID();

        //添加buff目标
		UID uidTarget = INVALID_UID;
		switch (m_data.nTargetType)
		{
		case MA_TAGET_TO_SELF:
			{
				uidTarget = uidSelf;
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
				ErrorLn("CEffectAddBuff error nTargetType="<<m_data.nTargetType);
				return false;
			}
			break;
		}

		if (isInvalidUID(uidTarget))
		{
			return false;
		}

		// 增加buff者
		UID uidAdd = INVALID_UID;
		switch (m_data.nAddTargetType)
		{
		case MA_TAGET_TO_SELF:
		case MA_TAGET_TO_OPERATOR:
			{
				uidAdd = context->uidOperator;
			}
			break;
		case MA_TAGET_TO_TARGET:
			{
				uidAdd = context->uidTarget;
			}
			break;
		case MA_TAGET_TO_ALL:
			{
				uidAdd = INVALID_UID;
			}
			break;
		default:
			{
				ErrorLn("CEffectAddBuff error nAddTargetType="<<m_data.nAddTargetType);
				return false;
			}
			break;
		}

        // 目标过滤
        if(TARGET_ALL != m_data.nTargetTypeFilter)
        {
            if (!chooseTarget(uidAdd, m_data.nTargetTypeFilter, uidTarget))
            {
                return false;
            }
        }

		switch (m_data.nType)
		{
		case BUFF_ADD:
			{
				if (isInvalidUID(uidAdd))
				{
					return false;
				}

				SBuffContext BuffContext;
				BuffContext.nAttackCount = context->nAttackCount;
				BuffContext.ptTargetTile = context->ptTargetTile;
                BuffContext.fFocoEnergiaPCT = context->fFocoEnergiaPCT;
                BuffContext.nID = context->nID;

                // 是自己时,是直接加BUFF,通过部件加
                if (uidSelf == uidTarget && m_data.nDirectAdd)
                {
                    pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, uidAdd, 0, &BuffContext, sizeof(BuffContext));
                }
                else
                {
				    // 给实体添加buff
				    AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidAdd, 0, &BuffContext, sizeof(BuffContext));
                }
			}
			break;
		case BUFF_REMOVE:
			{
                // 是自己时,是直接加BUFF,通过部件加
                if (uidSelf == uidTarget && m_data.nDirectAdd)
                {
                    pBuffPart->Remove(m_data.nBuffID, uidAdd, uidAdd);
                }
                else
                {
				    // 移除时uidAdd为INVALID_UID表示移除所有m_data.nBuffID，所以不判断
				    // 给实体移除buff
				    RemoveBuff(uidTarget, m_data.nBuffID, uidAdd, uidAdd);
                }
			}
			break;
		case BUFF_OVERLAY:
			{
				if (isInvalidUID(uidAdd))
				{
					return false;
				}

                SBuffContext BuffContext;
                BuffContext.nAttackCount = context->nAttackCount;
                BuffContext.ptTargetTile = context->ptTargetTile;
                BuffContext.fFocoEnergiaPCT = context->fFocoEnergiaPCT;
                BuffContext.nID = context->nID;

                // 是自己时,是直接加BUFF,通过部件加
                if (uidSelf == uidTarget && m_data.nDirectAdd)
                {
                    pBuffPart->SetAccumulateBuff(m_data.nBuffID, uidAdd, true, 0, &BuffContext, sizeof(BuffContext));
                }
                else
                {
				    // 叠加BUFF
				    SetAccumulateBuff(uidTarget, m_data.nBuffID, uidAdd, true, 0, &BuffContext, sizeof(BuffContext));
                }
			}
			break;
		case BUFF_DECREASE:
			{
                SBuffContext BuffContext;
                BuffContext.nAttackCount = context->nAttackCount;
                BuffContext.ptTargetTile = context->ptTargetTile;
                BuffContext.fFocoEnergiaPCT = context->fFocoEnergiaPCT;
                BuffContext.nID = context->nID;

                // 是自己时,是直接加BUFF,通过部件加
                if (uidSelf == uidTarget && m_data.nDirectAdd)
                {
                    pBuffPart->SetAccumulateBuff(m_data.nBuffID, uidAdd, false, 0, &BuffContext, sizeof(BuffContext));
                }
                else
                {
				    // 移除时uidAdd为INVALID_UID表示移除所有m_data.nBuffID，所以不判断
				    // 递减BUFF
				    SetAccumulateBuff(uidTarget, m_data.nBuffID, uidAdd, false, 0, &BuffContext, sizeof(BuffContext));
                }
			}
			break;
		default:
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
		return new CEffectAddBuff(m_data);
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
