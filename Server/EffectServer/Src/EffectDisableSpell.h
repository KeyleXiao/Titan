/*******************************************************************
** 文件名:	EffectDisableSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	6/08/2015
** 版  本:	1.0
** 描  述:	效果-禁用技能
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

class CEffectDisableSpell : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_DisableSpell    SCHEME_DATA;

	CEffectDisableSpell( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
		vector<string> tempVector;
		tempVector.clear();

		m_DisableSpellIDMap.clear();
		StringHelper::split(tempVector, m_data.strSpellID, ';', " ");
		for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
		{
			int nSpellID = StringHelper::toInt(tempVector[nCount]);
			if (nSpellID <= 0)
			{
				continue;
			}
			m_DisableSpellIDMap.insert(DISABLE_SPELL_MAP::value_type(nSpellID, nSpellID));
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
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CAST_SPELL, "CEffectDisableSpell");
		// 注册施法技能
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL, "CEffectDisableSpell");

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
		return new CEffectDisableSpell(m_data);
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

                // 不配表示禁用所有技能
                if (m_DisableSpellIDMap.empty())
                {
                    return false;
                }

                event_prepare_cast_spell *pPrepareCastSpell = (event_prepare_cast_spell *)pszContext;
                DISABLE_SPELL_MAP::iterator Iter = m_DisableSpellIDMap.find(pPrepareCastSpell->nID);
                if (Iter != m_DisableSpellIDMap.end())
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
                if (m_DisableSpellIDMap.empty())
                {
                    // 设置技能封魔
                    pCastSpell->nFailResult = SpellCastFailResult_Silence;
                    return false;
                }
				
				DISABLE_SPELL_MAP::iterator Iter = m_DisableSpellIDMap.find(pCastSpell->nID);
				if (Iter != m_DisableSpellIDMap.end())
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
	DISABLE_SPELL_MAP		  m_DisableSpellIDMap;
};
