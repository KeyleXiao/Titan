/*******************************************************************
** 文件名:	EffectAddCureEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/16/2015
** 版  本:	1.0
** 描  述:	效果-增加治疗效果
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectAddCureEffect : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AddCureEffect    SCHEME_DATA;

	CEffectAddCureEffect( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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

		// 注册准备治疗事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CURE, "CEffectAddCureEffect");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销准备治疗事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CURE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAddCureEffect(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_PREPARE_CURE:	// 准备治疗事件
			{
				if (pszContext == NULL || nLen != sizeof(CureContext))
				{
					return;
				}
				if (m_pEntity == NULL)
				{
					return;
				}

				CureContext *pContext = (CureContext *)pszContext;

                if ((m_data.nSpellEffectType&pContext->nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

				switch (m_data.nValueType)
				{
				case MA_VALUE_NUM:
					{
						pContext->nCureHP += m_data.nValue;
					}
					break;
				case MA_VALUE_PER:
					{
						pContext->nCureHP += (int)(m_data.nValue/EFFECT_RATE*pContext->nCureHP);
					}
					break;
				default:
					{
						ErrorLn("EffectServer_AddCureEffect error nValueType="<<m_data.nValueType);
						return;
					}
					break;
				}
				if (pContext->nCureHP <= 0)
				{
					pContext->nCureHP = 0;
				}
			}
			break;
		default:
			break;
		}
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
