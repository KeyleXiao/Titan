/*******************************************************************
** 文件名:	EffectDisableSpellUseCount.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/13/2016
** 版  本:	1.0
** 描  述:	效果-限制技能使用次数
            http://172.16.0.120/redmine/issues/2337

********************************************************************
** 修改: http://172.16.0.120/redmine/issues/5005   李界华
********************************************************************/

#pragma once

#include "IEffect.h"
#include "IEntity.h"
#include "EffectDef.h"
#include "IEntityEvent.h"
#include "IEventEngine.h"
#include "EventDef.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectDisableSpellUseCount : public IEffectEx,public IEventVoteSink, public IEventExecuteSink
{
public:
	typedef  EffectServer_DisableSpellUseCount    SCHEME_DATA;

	CEffectDisableSpellUseCount( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_nSpellCount(0),m_nResetTick(0)
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}
		m_pEntity = context->pEntity;

		// 注册施法技能投票事件
		g_EHelper.Subscibe(m_pEntity, static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CAST_SPELL, "CEffectDisableSpellUseCount");

        g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CAST_SPELL, "CEffectDisableSpellUseCount");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 注销施法技能投票事件
			g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CAST_SPELL);

            g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CAST_SPELL);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectDisableSpellUseCount(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
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
		case EVENT_ENTITY_CAST_SPELL:
			{
				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					return false;
				}
                event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

                // 技能不匹配
                if (pCastSpell->nID != m_data.nSpellID)
                {
                    return true;
                }

                if (m_nSpellCount >= m_data.nSpellCount)
                {
                    // 已经过期
                    if (m_nResetTick > 0 && getTickCountEx() >= m_nResetTick)
                    {
                        // 清除重置时间
                        m_nResetTick = 0;
                        // 重置使用次数
                        m_nSpellCount = 0;
                    }
                    else
                    {
                        if(m_data.nTipID > 0)
                        {
                            pCastSpell->nFailResult = m_data.nTipID;
                        }
                        else
                        {
                            // 使用次数限制
                            pCastSpell->nFailResult = SpellCastFailResult_CountLimit;
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
		case EVENT_ENTITY_CAST_SPELL:
			{
				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					break;
				}
				event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

                // 技能不匹配
                if (pCastSpell->nID != m_data.nSpellID)
                {
                    break;
                }

                ++m_nSpellCount;

                // 达到最大使用次数了
                if(m_nSpellCount >= m_data.nSpellCount)
                {
                    // 设定重置时间
                    if(m_data.nResetInterval > 0)
                    {
                        m_nResetTick = getTickCountEx() + m_data.nResetInterval;
                    }
                }
			}
			break;
		default:
			break;
		}
	}

private:
	SCHEME_DATA                 m_data;

	// 实体指针
	__IEntity	 *              m_pEntity;

    // 使用技能次数
    int                         m_nSpellCount;

    // 重置时间
    ulong                       m_nResetTick;
};
