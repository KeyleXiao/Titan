/*******************************************************************
** 文件名:	EffectWarMonsterAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/10/2017
** 版  本:	1.0
** 描  述:	效果-战场怪物增加Buff
            http://172.16.0.120/redmine/issues/3427
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

class CEffectWarMonsterAddBuff : public IEffectEx
{
public:
	typedef  EffectServer_WarMonsterAddBuff    SCHEME_DATA;

	CEffectWarMonsterAddBuff( SCHEME_DATA & data ) : m_data(data)
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

        int nSelfCamp = context->pEntity->getIntProperty(PROPERTY_CAMP);

		UID uidArray[MAX_INTEREST_OBJECT*2] = {0};
		int nArraySize= MAX_INTEREST_OBJECT*2;
		int nCount = pWarService->warSerchTypeUIDList(nSelfCamp, m_data.nCampType, m_data.nMonsterExec, uidArray, nArraySize);
        if (nCount == 0)
        {
            return false;
        }

        UID uidSrcEntity = context->pEntity->getUID();
        SBuffContext BuffContext;
        BuffContext.nID = context->nID;

        DWORD dwMaxCount = MAX_INTEREST_OBJECT*2;
        UID uidMonsterAddBuff[MAX_INTEREST_OBJECT*2] = {0};
        DWORD dwMonsterCount = 0;
		for (int i=0; i<nCount; ++i)
		{
            if (dwMonsterCount >= dwMaxCount)
            {
                break;
            }

			UID uidTarget = uidArray[i];
			if (isInvalidUID(uidTarget))
			{
				continue;
			}

			switch (m_data.nType)
			{
			case BUFF_ADD:
				{
                    if (isMonster(uidTarget))
                    {
                        uidMonsterAddBuff[dwMonsterCount++] = uidTarget;
                    }
                    else
                    {
					    // 给实体添加buff
					    AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidSrcEntity);
                    }
				}
				break;
			case BUFF_REMOVE:
				{
                    if (isMonster(uidTarget))
                    {
                        uidMonsterAddBuff[dwMonsterCount++] = uidTarget;
                    }
                    else
                    {
					    // 给实体移除buff
					    RemoveBuff(uidTarget, m_data.nBuffID, uidSrcEntity, uidSrcEntity);
                    }
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

        if (dwMonsterCount > 0)
        {
            switch (m_data.nType)
            {
            case BUFF_ADD:
                {
                    g_EHelper.BatchAddBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nBuffID, m_data.nBuffLevel, uidSrcEntity, 0, &BuffContext, sizeof(BuffContext));
                }
                break;
            case BUFF_REMOVE:
                {
                    g_EHelper.BatchRemoveBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nBuffID, uidSrcEntity, uidSrcEntity);
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
		return new CEffectWarMonsterAddBuff(m_data);
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
