/*******************************************************************
** 文件名:	EffectDisableSlotIndex.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/07/2017
** 版  本:	1.0
** 描  述:	效果-禁用槽位索引
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

class CEffectDisableSlotIndex : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_DisableSlotIndex    SCHEME_DATA;

	CEffectDisableSlotIndex( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
		vector<string> tempVector;
		tempVector.clear();

		m_DisableSlotIndexIDMap.clear();
		StringHelper::split(tempVector, m_data.strSlotIndex, ';', " ");
		for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
		{
			int nSlotIndex = StringHelper::toInt(tempVector[nCount]);
			if (nSlotIndex <= 0)
			{
				continue;
			}
			m_DisableSlotIndexIDMap.insert(DISABLE_SPELL_MAP::value_type(nSlotIndex, nSlotIndex));
		}
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

        // 注册准备施法技能
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CAST_SPELL, "CEffectDisableSlotIndex");
		// 注册施法技能
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL, "CEffectDisableSlotIndex");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 注销准备施法技能
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CAST_SPELL);
			// 注销施法技能
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectDisableSlotIndex(m_data);
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
        case EVENT_ENTITY_PREPARE_CAST_SPELL:
            {
                if (pszContext == NULL || nLen != sizeof(event_prepare_cast_spell))
                {
                    return false;
                }

                // 不配表示禁用所有槽位
                if (m_DisableSlotIndexIDMap.empty())
                {
                    return false;
                }

                event_prepare_cast_spell *pPrepareCastSpell = (event_prepare_cast_spell *)pszContext;

                if (pPrepareCastSpell->nSlotType != m_data.nSlotType)
                {
                    return true;
                }

                DISABLE_SPELL_MAP::iterator Iter = m_DisableSlotIndexIDMap.find(pPrepareCastSpell->nSlotIndex);
                if (Iter != m_DisableSlotIndexIDMap.end())
                {
                    return false;
                }
            }
            break;
		case EVENT_ENTITY_CAST_SPELL:
			{
				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					return false;
				}
                event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

                // 不配表示禁用所有技能
                if (m_DisableSlotIndexIDMap.empty())
                {
                    // 设置技能封魔
                    pCastSpell->nFailResult = SpellCastFailResult_Silence;
                    return false;
                }

                if (pCastSpell->nSlotType != m_data.nSlotType)
                {
                    return true;
                }
				
				DISABLE_SPELL_MAP::iterator Iter = m_DisableSlotIndexIDMap.find(pCastSpell->nSlotIndex);
				if (Iter != m_DisableSlotIndexIDMap.end())
				{
                    // 设置技能封魔
                    pCastSpell->nFailResult = SpellCastFailResult_Silence;
					return false;
				}
			}
			break;
		default:
			break;
		}

		return true;
	}

private:
	SCHEME_DATA               m_data;

	// 实体指针
	__IEntity				  *m_pEntity;

	// 禁用技能表
	typedef map<int, int>	  DISABLE_SPELL_MAP;	
	DISABLE_SPELL_MAP		  m_DisableSlotIndexIDMap;
};
