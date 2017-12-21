/*******************************************************************
** 文件名:	EffectCastSpellCheckBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/24/2016
** 版  本:	1.0
** 描  述:	效果-施法技能检测BUFF
            http://172.16.0.120/redmine/issues/2159
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"

class CEffectCastSpellCheckBuff : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_CastSpellCheckBuff    SCHEME_DATA;

	CEffectCastSpellCheckBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

		// 注册施法技能
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL, "EffectCastSpellCheckBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销施法技能
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectCastSpellCheckBuff(m_data);
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
		if (m_pEntity == NULL)
		{
			return false;
		}
		if (pszContext == NULL || nLen != sizeof(event_cast_spell))
		{
			return false;
		}

		event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

        UID uidTarget = pCastSpell->uidTarget;

		// 技能效果类型不匹配,可使用技能
		if ((pCastSpell->nSpellEffectType&m_data.nSpellEffectType) == 0)
		{
			return true;
		}

        // 检测目标不匹配,可使用技能
        if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
        {
            return true;
        }

        bool bExist = false;
        if (m_data.bExist == MA_TRUE)
        {
            bExist = true;
        }

        // buff检测不匹配
        if (bExist != IsExistBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, INVALID_UID))
        {
            // 设置技能条件不足
            pCastSpell->nFailResult = SpellCastFailResult_Condition;
            return false;
        }

		return true;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
