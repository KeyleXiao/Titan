/*******************************************************************
** 文件名:	EffectDisableActiveTalent.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/18/2017
** 版  本:	1.0
** 描  述:	效果-禁止学习天赋
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

class CEffectDisableActiveTalent : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_DisableActiveTalent    SCHEME_DATA;

	CEffectDisableActiveTalent( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
		vector<string> tempVector;
		tempVector.clear();

		m_DisableActiveTalentIDMap.clear();
		StringHelper::split(tempVector, m_data.strTalentID, ';', " ");
		for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
		{
			int nTalentID = StringHelper::toInt(tempVector[nCount]);
			if (nTalentID <= 0)
			{
				continue;
			}
			m_DisableActiveTalentIDMap.insert(DISABLE_SPELL_MAP::value_type(nTalentID, nTalentID));
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

        // 注册玩家激活天赋
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ACTIVE_TALENT, "CEffectDisableActiveTalent");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销玩家激活天赋
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ACTIVE_TALENT);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectDisableActiveTalent(m_data);
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
		case EVENT_ENTITY_ACTIVE_TALENT:
			{
				if (pszContext == NULL || nLen != sizeof(event_entity_active_talent))
				{
					return false;
				}
                event_entity_active_talent *pTalent = (event_entity_active_talent *)pszContext;

                // 不配表示禁用所有技能
                if (m_DisableActiveTalentIDMap.empty())
                {
                    return false;
                }
				
				DISABLE_SPELL_MAP::iterator Iter = m_DisableActiveTalentIDMap.find(pTalent->nTalentID);
				if (Iter != m_DisableActiveTalentIDMap.end())
				{
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

	// 禁用天赋表
	typedef map<int, int>	  DISABLE_SPELL_MAP;	
	DISABLE_SPELL_MAP		  m_DisableActiveTalentIDMap;
};
