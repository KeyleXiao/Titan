/*******************************************************************
** 文件名:	EffectWarAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/17/2015
** 版  本:	1.0
** 描  述:	效果-战场增加Buff
目前只实现取战场所有英雄增加BUFF效果，后期结构增加参数，对英雄，怪物操作等
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "IEntity.h"
#include "IBuffServer.h"
#include "BuffMessageDef.h"
#include "ExternalHelper.h"
#include "WarHelper.h"
#include "IWarPart.h"

class CEffectWarAddBuff : public IEffectEx
{
public:
	typedef  EffectServer_WarAddBuff    SCHEME_DATA;

	CEffectWarAddBuff( SCHEME_DATA & data ) : m_data(data)
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

        __IWarPart* pWarPart = (__IWarPart*)context->pEntity->getEntityPart(PART_WAR);
        if(pWarPart == NULL)
        {
            return false;
        }
        int nWarID = pWarPart->getWarID();

        WarHelper helper(nWarID);
        IWarService *pWarService = helper.m_ptr;
        if (pWarService == NULL)
        {
            return false;
        }

		UID uidArray[256]={0};
		int nArraySize= 256;
		int nCount = pWarService->getAllPerson(uidArray, nArraySize);

        UID uidSrcEntity = context->pEntity->getUID();

		for (int i=0; i<nCount; ++i)
		{
			UID uidTarget = uidArray[i];
			if (isInvalidUID(uidTarget))
			{
				continue;
			}

			// 检测目标
			if (!g_EHelper.chooseTarget(context->pEntity, m_data.nTargetFilter, uidTarget))
			{
				continue;
			}

			switch (m_data.nType)
			{
			case BUFF_ADD:
				{
					// 给实体添加buff
					AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidSrcEntity);
				}
				break;
			case BUFF_REMOVE:
				{
					// 给实体移除buff
					RemoveBuff(uidTarget, m_data.nBuffID, uidSrcEntity, uidSrcEntity);
				}
				break;
			case BUFF_OVERLAY:
				{
					// 叠加BUFF
					SetAccumulateBuff(uidTarget, m_data.nBuffID, uidSrcEntity, true);
				}
				break;
			case BUFF_DECREASE:
				{
					// 递减BUFF
					SetAccumulateBuff(uidTarget, m_data.nBuffID, uidSrcEntity, false);
				}
				break;
			default:
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
		return new CEffectWarAddBuff(m_data);
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
