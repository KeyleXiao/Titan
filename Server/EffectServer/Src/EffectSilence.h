/*******************************************************************
** 文件名:	EffectSilence.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/26/2015
** 版  本:	1.0
** 描  述:	效果-技能封魔
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IMovePart.h"
#include "IEventEngine.h"
#include "IProgressBarPart.h"

#define  SILENCE_DO_INNER_EFFECT( typeName,obj,context)  gServerGlobal->getEffectFactory()->DoInnerEffect( #typeName,&obj,sizeof(obj),context)

class CEffectSilence : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_Silence    SCHEME_DATA;

	CEffectSilence( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL, "EffectSilence");

        do 
        {
            // 不打断
            if (m_data.nBreak == MA_FALSE)
            {
                break;
            }

            __IProgressBarPart *pProgressBarPart = (__IProgressBarPart *)m_pEntity->getEntityPart(PART_PROGRESS_BAR);
            if (pProgressBarPart == NULL)
            {
                break;
            }

            ProgressBarContext *pProgressContext = pProgressBarPart->GetCurrentContext();
            if (pProgressContext == NULL)
            {
                break;
            }

            __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
            if (pSpellPart == NULL)
            {
                break;
            }

            if (pProgressContext->nSpellID <= 0)
            {
                break;
            }

            ISpell *pSpell = pSpellPart->findSpell(pProgressContext->nSpellID);
            if (pSpell == NULL)
            {
                break;
            }

            int	nSpellEffectType = pSpell->getSpellData(SPDATA_SPELL_EFFECT_TYPE);
            if ((nSpellEffectType&m_data.nSpellEffectType) == 0)
            {
                break;
            }

            // 执行打断技能效果
            EffectServer_BreakSpell BreakSpell;
            SILENCE_DO_INNER_EFFECT(EffectServer_BreakSpell, BreakSpell, context );
        } while (false);

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
		return new CEffectSilence(m_data);
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
		// 技能效果类型匹配
		if ((pCastSpell->nSpellEffectType&m_data.nSpellEffectType) != 0)
		{
			// 设置技能封魔
			pCastSpell->nFailResult = SpellCastFailResult_Silence;
			return false;
		}

		return true;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
