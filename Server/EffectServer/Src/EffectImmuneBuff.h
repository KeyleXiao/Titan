/*******************************************************************
** 文件名:	EffectImmuneBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/15/2015
** 版  本:	1.0
** 描  述:	效果-免疫buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EventDef.h"
#include "IEntityEvent.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectImmuneBuff : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_ImmuneBuff    SCHEME_DATA;

	CEffectImmuneBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

		// 注册增加buff事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ADDBUFF, "EffectImmuneBuff");		

		return true;
	}

	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  如果返回false : 则中断执行，否则继续向下执行
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_ADDBUFF:
			{
				if (pszContext == NULL || nLen != sizeof(event_entity_add_buff))
				{
					return false;
				}
				event_entity_add_buff *pBuff = (event_entity_add_buff *)pszContext;
				// 免疫的buff类型
				if((pBuff->dwBuffFlag&m_data.nBuffType) > 0/*只要buff中有种需免疫的类型，则免疫掉*/)
				{
					// 计算概率
					int nRandNum = 0;
					rktRangeRand(nRandNum, 1, EFFECT_PROB_VALUE);
					if(nRandNum <= m_data.nPer)
					{
                        // 免疫成功给自己加buff-->控制光效表现
                        if(m_data.nBuffID != 0 && m_pEntity != NULL)
                        {
                            UID uidSelf = m_pEntity->getUID(); 
                            AddBuff(uidSelf, m_data.nBuffID, 1, uidSelf);
                        }
						return false;
					}
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
		if (m_pEntity != 0)
		{
			// 注销增加buff事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ADDBUFF);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectImmuneBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};