/*******************************************************************
** 文件名:	EffectChangeBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/18/2015
** 版  本:	1.0
** 描  述:	效果-改变buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "EntityHelper.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectChangeBuff : public IEffectEx
{
public:
	typedef  EffectServer_ChangeBuff    SCHEME_DATA;

	CEffectChangeBuff( SCHEME_DATA & data ) : m_data(data)
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

		__IEntity *pEntity = context->pEntity;

        __IBuffPart *pBuffPart = (__IBuffPart *)pEntity->getEntityPart(PART_BUFF);
        if (pBuffPart == NULL)
        {
            return false;
        }

		// 取得玩家UID
		UID uid = pEntity->getUID();
		// Buff等级
		DWORD dwBuffLevel = pBuffPart->GetBuffLevel(m_data.nCheckBuffID, INVALID_UID);

		int nBuffValue = 0;
		switch (m_data.nChangeBuffType)
		{
		case CHANGE_BUFF_LEVEL:
			{
				nBuffValue = dwBuffLevel;
			}
			break;
		case CHANGE_BUFF_TIME:
			{
				nBuffValue = pBuffPart->GetLeftTime(m_data.nCheckBuffID, INVALID_UID);
				if (nBuffValue < 0)
				{
					return false;
				}
			}
			break;
		default:
			{
				ErrorLn("CEffectChangeBuff error nChangeBuffType="<<m_data.nChangeBuffType);
				return false;
			}
			break;
		}

		int nValue = 0;
		switch (m_data.nValueType)
		{
		case MA_VALUE_NUM:
			{
				nValue = m_data.nValue;
			}
			break;
		case MA_VALUE_PER:
			{
				nValue = (int)(m_data.nValue/EFFECT_RATE*nBuffValue);
			}
			break;
		default:
			{
				ErrorLn("CEffectChangeBuff error nValueType="<<m_data.nValueType);
				return false;
			}
			break;
		}

		switch (m_data.nChangeType)
		{
		case MA_CHANGE_ADD:
			{
				nBuffValue += nValue;
			}
			break;
		case MA_CHANGE_SET:
			{
				nBuffValue = nValue;
			}
			break;
		default:
			{
				ErrorLn("CEffectChangeBuff error nChangeType="<<m_data.nChangeType);
				return false;
			}
			break;
		}

		// 删除检测BUFF
		pBuffPart->Remove(m_data.nCheckBuffID, uid, uid);

		switch (m_data.nChangeBuffType)
		{
		case CHANGE_BUFF_LEVEL:
			{
                if (nBuffValue >= m_data.nMaxBuffLevel)
                {
                    nBuffValue = m_data.nMaxBuffLevel;
                }

				if (nBuffValue <= 0)
				{
					return true;
				}
                
				// 增加BUFF
				pBuffPart->Add(m_data.nAddBuffID, nBuffValue, uid);
			}
			break;
		case CHANGE_BUFF_TIME:
			{
				if (dwBuffLevel <= 0)
				{
					return true;
				}
				// 增加BUFF，其实主要是改变时间
				pBuffPart->Add(m_data.nAddBuffID, dwBuffLevel, uid, nBuffValue);
			}
			break;
		default:
			{
				ErrorLn("CEffectChangeBuff error nChangeBuffType="<<m_data.nChangeBuffType);
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
		return new CEffectChangeBuff(m_data);
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