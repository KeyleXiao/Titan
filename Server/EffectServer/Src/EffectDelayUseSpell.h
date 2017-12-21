/*******************************************************************
** 文件名:	EffectDelayUseSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/19/2016
** 版  本:	1.0
** 描  述:	效果-延迟使用技能
			通过使用技能效果，可实现在技能事件中使用技能，从而实现后续技能使用
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ISpell.h"
#include "IAttackObject.h"
#include "SpellContextDef.h"

using namespace EFFECT_SERVER;

class CEffectDelayUseSpell : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_DelayUseSpell    SCHEME_DATA;

	CEffectDelayUseSpell( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_uidOperator(0),m_uidTarget(0),m_ptStartTile(0,0,0),m_ptTargetTile(0,0,0)
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

        Stop();

        m_pEntity = context->pEntity;
        m_uidOperator = context->uidOperator;
        m_uidTarget = context->uidTarget;
        m_ptStartTile = context->ptStartTile;

        if(context->ptTargetTileOri != INVALID_LOC)
        {
            // 击中事件ptTargetTile已经不是技能的终点位置 所以用备份的
            m_ptTargetTile = context->ptTargetTileOri;
        }
        else
        {
            m_ptTargetTile = context->ptTargetTile;
        }

        if(m_data.nCount >= 1)
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, m_data.nCount, "CEffectDelayUseSpell");
        }
        else
        {
            OnTimer(0);
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            g_EHelper.KillTimer(0, this);

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectDelayUseSpell(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

    Vector3 getLoc(DELAY_USE_SPELL_POS_TYPE eType)
    {
        switch(eType)
        {
        case DELAY_USE_SPELL_POS_SELF:  // 自己位置
            {
                if(m_pEntity == NULL)
                {
                    return INVALID_LOC;
                }
                return m_pEntity->getLocation();
            }
            break;
        case DELAY_USE_SPELL_POS_TARGET: // 目标位置
            {
                return getLocation(m_uidTarget);
            }
            break;
        case DELAY_USE_SPELL_POS_START_TILE: // 技能起点位置
            {
                return m_ptStartTile;
            }
            break;
        case DELAY_USE_SPELL_POS_TARGET_TILE: // 技能终点位置
            {
                return m_ptTargetTile;
            }
            break;
        default:
            {
                return INVALID_LOC;
            }
        }
    }

    virtual void OnTimer( unsigned long dwTimerID )
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return;
        }

        UID uidTarget = 0;
        switch (m_data.nTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidTarget = m_pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidTarget = m_uidTarget;  
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidTarget = m_uidOperator;
            }
            break;
        case MA_TAGET_NO:
            {
                uidTarget = INVALID_UID;
            }
            break;
        default:
            {
                ErrorLn("CEffectDelayUseSpell error nTargetType="<<m_data.nTargetType);
                return;
            }
            break;
        }

        Vector3 ptStartTile = getLoc(m_data.nStartPosType);
        Vector3 ptTargetTile = getLoc(m_data.nTargetPosType);
        UID uidSelf = m_pEntity->getUID();

        DWORD dwStartTick = getTickCountEx();
        // 技能施法现场
        SPELL_CONTEXT spellContext;
        spellContext.CC.pEntity = m_pEntity;
        spellContext.CC.uidOperator = uidSelf;
        spellContext.CC.uidTarget = uidTarget;
        spellContext.CC.ptStartTile = ptStartTile;
        spellContext.CC.ptTargetTile = ptTargetTile;
        spellContext.CC.nID = m_data.nSpellID;
        spellContext.CC.dwStartTick = dwStartTick;
        spellContext.CC.dwUpdateFrame = 0;
        spellContext.EC.pEntity = m_pEntity;
        spellContext.EC.uidOperator = uidSelf;
        spellContext.EC.uidTarget = uidTarget;
        spellContext.EC.ptStartTile = ptStartTile;
        spellContext.EC.ptTargetTile = ptTargetTile;
        spellContext.EC.nID = m_data.nSpellID;
        spellContext.EC.vSelfLoc = m_pEntity->getLocation();
        spellContext.EC.dwStartTick = dwStartTick;
        // 施法技能
        pSpellPart->castSpell(&spellContext);
    }

    // 技能调用时 是否马上停止
    virtual bool            StopImmAsSpellEffect() { return false; }

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;

    UID                       m_uidOperator;

    UID                       m_uidTarget;

    Vector3                   m_ptStartTile;

    Vector3                   m_ptTargetTile;
};
