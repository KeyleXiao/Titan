/*******************************************************************
** 文件名:	EffectDieEventAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/19/2017
** 版  本:	1.0
** 描  述:	效果-死亡事件增加Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "EntityHelper.h"
#include "ExternalHelper.h"

class CEffectDieEventAddBuff : public IEffectEx, public IEventExecuteSink
{
public:
	typedef  EffectServer_DieEventAddBuff    SCHEME_DATA;

	CEffectDieEventAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_nSpellID(0)
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

        m_nSpellID = context->nID;

		// 注册实体死亡事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DIE, "CEffectDieEventAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != 0)
        {
			// 注销实体死亡事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DIE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectDieEventAddBuff(m_data);
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
		case EVENT_ENTITY_DIE:
			{
                if (pszContext == NULL || nLen != sizeof(event_entity_die))
                {
                    break;
                }
                event_entity_die *pDie = (event_entity_die *)pszContext;

                __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
                if (pBuffPart == NULL)
                {
                    break;
                }

                SBuffContext BuffContext;
                BuffContext.nID = m_nSpellID;

                // 增加buff
                pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, m_pEntity->getUID(), 0, &BuffContext, sizeof(BuffContext));
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

    int                     m_nSpellID;
};
