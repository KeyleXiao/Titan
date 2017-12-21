/*******************************************************************
** 文件名:	EffectChangeReliveTime.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-07-15
** 版  本:	1.0
** 描  述:	更改复活时间效果
** 应  用: 

2015-07-15 第一版需求：http://172.16.0.120/redmine/issues/790
	复活时间
	当目标身上有复活时间降低的状态或者装备时，当目标死亡后会按照固定数值或者复活时间的百分比缩短所需要的复活时间
	需要支持配置：复活时间缩短是固定或者百分比、具体数值、冷却时间（多少毫秒触发1次，0为无冷却）
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EntityHelper.h"

using namespace EFFECT_SERVER;



// 更改复活时间效果
class CEffectChangeReliveTime : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ReliveTime    SCHEME_DATA;


	CEffectChangeReliveTime( SCHEME_DATA & data ) : m_data(data)
	{
		m_dwLastTick = 0;
		m_pEntity = 0;
	}

	//////// IEffect //////////////////////////////////////////////////////////////////
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

		// 注册预复活事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_BEFORE_RELIVE, "CEffectChangeReliveTime");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销预复活事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_BEFORE_RELIVE);

			m_dwLastTick = 0;
			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectChangeReliveTime(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	//////// IEventExecuteSink //////////////////////////////////////////////////////////////////
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
		case EVENT_ENTITY_BEFORE_RELIVE:	// 预复活事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}
				if (pszContext == NULL || nLen != sizeof(SEntityReliveInfo))
				{
					return;
				}

				SEntityReliveInfo * pContext = (SEntityReliveInfo*)pszContext;
				
				DWORD dwCurrentTick = getTickCountEx();
				if (dwCurrentTick > m_dwLastTick + m_data.nInterval)
				{
					m_dwLastTick = dwCurrentTick;

					int nOldValue = pContext->nActiveInterval;
					// 更改复活时间
					if (m_data.eValueType == MA_VALUE_PER)
					{
						pContext->nActiveInterval += (int)(m_data.nValue/EFFECT_RATE*pContext->nActiveInterval);
					}
					else
					{
						pContext->nActiveInterval += m_data.nValue;
					}

					TraceLn("Adjust entity["<< m_pEntity->getName() <<"] relive time，the end relive time="<<pContext->nActiveInterval<<" ms");
				}

				// 处理异常复活时间
				if (pContext->nActiveInterval < 0)
				{
					pContext->nActiveInterval = 0;
				}
			}
			break;

		default:
			{
			}
			break;
		}
	}


private:
	SCHEME_DATA             m_data;
	DWORD					m_dwLastTick;	
	__IEntity				*m_pEntity;		// 实体指针
};
