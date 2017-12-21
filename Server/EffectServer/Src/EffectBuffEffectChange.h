/*******************************************************************
** 文件名:	EffectBuffEffectChange.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/24/2015
** 版  本:	1.0
** 描  述:	效果-BUFF效果改变
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

class CEffectBuffEffectChange : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_BuffEffectChange    SCHEME_DATA;

	CEffectBuffEffectChange( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

		// 注册准备改变属性事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CHANGE_PROPERTY, "CEffectBuffEffectChange");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销准备改变属性事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CHANGE_PROPERTY);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectBuffEffectChange(m_data);
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
		case EVENT_ENTITY_PREPARE_CHANGE_PROPERTY:
			{
				if (m_pEntity == NULL)
				{
					return;
				}
				DWORD dwTick = getTickCountEx();
				if (dwTick < m_dwLastTick + m_data.nInterval)
				{
					return;
				}
				if (pszContext == NULL || nLen != sizeof(event_prepare_change_property))
				{
					return;
				}
				event_prepare_change_property *pPrepareChangeProperty = (event_prepare_change_property*)pszContext;

				if ((pPrepareChangeProperty->dwFlag&m_data.dwFlag) == 0)
				{
					return;
				}
				if (pPrepareChangeProperty->nID != m_data.nBuffEffectChangeType)
				{
					return;
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
						nValue = (int)(m_data.nValue/EFFECT_RATE*pPrepareChangeProperty->nValue);
					}
					break;
				default:
					{
						ErrorLn("CEffectBuffEffectChange error nValueType="<<m_data.nValueType);
						return;
					}
					break;
				}
				pPrepareChangeProperty->nValue -= nValue;

				m_dwLastTick = dwTick;
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

	// 上次时间
	DWORD					m_dwLastTick;
};
