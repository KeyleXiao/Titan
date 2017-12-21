/*******************************************************************
** 文件名:	EffectCleanEnmity.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/28/2015
** 版  本:	1.0
** 描  述:	效果-清除仇恨列表
需求描述：http://172.16.0.120/redmine/issues/841
实现描述：清除仇恨列表
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEnmityServer.h"

class CEffectCleanEnmity:public IEffectEx
{
public:
	typedef  EffectServer_CleanEnmity    SCHEME_DATA;

	CEffectCleanEnmity(SCHEME_DATA &data):m_data(data)
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

		__IEnmityPart *pEnmityPart = (__IEnmityPart *)pContext->pEntity->getEntityPart(PART_ENMITY);
		if (pEnmityPart == NULL)
		{
			return false;
		}

		switch (m_data.nTagetType)
		{
		case MA_TAGET_TO_SELF:
			{
				// 移除UID
				UID uid = pContext->pEntity->getUID();
				if (isInvalidUID(uid))
				{
					return false;
				}

				// 移除敌人
				pEnmityPart->RemoveOneEnemy(uid);
			}
			break;
		case MA_TAGET_TO_TARGET:
			{
				// 移除UID
				UID uid = pContext->uidTarget;
				if (isInvalidUID(uid))
				{
					return false;
				}

				// 移除敌人
				pEnmityPart->RemoveOneEnemy(uid);
			}
			break;
		case MA_TAGET_TO_BOTH:
			{
				// 移除UID
				UID uid = pContext->pEntity->getUID();
				if (!isInvalidUID(uid))
				{
					// 移除敌人
					pEnmityPart->RemoveOneEnemy(uid);
				}

				// 移除UID
				uid = pContext->uidTarget;
				if (!isInvalidUID(uid))
				{
					// 移除敌人
					pEnmityPart->RemoveOneEnemy(uid);
				}
			}
			break;
		case MA_TAGET_TO_ALL:
			{
				// 移除敌人
				pEnmityPart->CleanEnemyList();
			}
			break;
		default:
			{
				ErrorLn("CEffectCleanEnmity error m_data.nTagetType="<<m_data.nTagetType);
				return false;
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
		return new CEffectCleanEnmity(m_data);
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
