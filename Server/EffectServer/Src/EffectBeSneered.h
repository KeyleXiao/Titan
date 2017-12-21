/*******************************************************************
** 文件名:	EffectBeSneered.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	2/9/2017
** 版  本:	1.0
** 描  述:	效果-被嘲讽效果
需求描述：http://172.16.0.120/redmine/issues/3575
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "ISpell.h"

class CEffectBeSneered : public IEffectEx,public TimerHandler,public IEventVoteSink,public IEventExecuteSink
{
public:
	typedef  EffectServer_BeSneered    SCHEME_DATA;

	CEffectBeSneered( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_uidTarget(INVALID_UID),m_nSpellID(-1)
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

        m_uidTarget = context->uidOperator;
        if( m_uidTarget==INVALID_UID )
            return false;

        __ISlotPart* pSlotPart = (__ISlotPart*)m_pEntity->getEntityPart(PART_SLOTLOGIC);
        if( pSlotPart==NULL )
        {
            return false;
        }
        m_nSpellID = pSlotPart->GetSlotSkillID(SLOT_TYPE_SKILL, m_data.nForceAttackSpellSlot);

        SPELL_DATA* pData = gServerGlobal->getSpellFactory()->getSpellData(m_nSpellID);
        if( pData==NULL )
        {
            ErrorLn(__FUNCTION__ << __LINE__ << ", spell not found, id=" << m_nSpellID);
            return false;
        }

        // 注册客户端技能投票事件
        m_pEntity->getEntityEvent()->Subscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PREPARE_CAST_SPELL, __FUNCTION__);

        // 注册死亡事件
        SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
        if(eventHelper.m_ptr)
        {
            eventHelper.m_ptr->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
        }
        
        g_EHelper.SetTimer(0, 300, this, INFINITY_CALL, __FUNCTION__);

        // 禁止主动移动
        bool bFlag = true;
        g_EHelper.sendEntityMessage(m_pEntity->getUID(), PART_MOVE, SS_MSG_FORBID_MOVE_INITIATIVELY, (LPCSTR)&bFlag, sizeof(bFlag));

		g_EHelper.SetTimer(1, m_data.fTimeout, this, 1, __FUNCTION__);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            m_pEntity->getEntityEvent()->UnSubscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PREPARE_CAST_SPELL);

            // 注册死亡事件
            SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
            if(eventHelper.m_ptr)
            {
                eventHelper.m_ptr->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0);
            }

            // 停止移动
            __IMovePart* pMovePart = (__IMovePart*)m_pEntity->getEntityPart(PART_MOVE);
            if( pMovePart!=NULL )
            {
                pMovePart->stopMoving();
            }

            // 取消禁止主动移动
            bool bFlag = false;
            g_EHelper.sendEntityMessage(m_pEntity->getUID(), PART_MOVE, SS_MSG_FORBID_MOVE_INITIATIVELY, (LPCSTR)&bFlag, sizeof(bFlag));

			m_pEntity = 0;
		}

        g_EHelper.KillTimer(0, this);
		g_EHelper.KillTimer(1, this);
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectBeSneered(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
		if (dwTimerID == 0)
		{
			if (m_pEntity == NULL)
			{
				return;
			}

			__IMovePart* pMovePart = (__IMovePart*)m_pEntity->getEntityPart(PART_MOVE);
			if (pMovePart == NULL)
			{
				return;
			}

			// 目标死亡停止
			int nTargetState = GetState(m_uidTarget);
			if (nTargetState == ENTITY_STATE_DIE || nTargetState == ENTITY_STATE_UNKNOW)
			{
				Stop();
				return;
			}

			Vector3 ptSelf = m_pEntity->getLocation();
			Vector3 ptTarget = getLocation(m_uidTarget);
			float fDistance = (ptTarget - ptSelf).getLength();
			if (fDistance > m_data.fCloseDistance)
			{
				float fCloseDistance = m_data.fCloseDistance;
				if (fCloseDistance > 0.01f)
					fCloseDistance -= 0.01f;

				// 贴近
				Vector3 ptClose = ptSelf + (ptTarget - ptSelf) * ((fDistance - fCloseDistance) / fDistance);
				pMovePart->moveTo(ptClose);
			}
			else
			{
				// 普攻
				__ISpellPart* pSpellPart = (__ISpellPart*)m_pEntity->getEntityPart(PART_SPELL);
				if (pSpellPart == NULL)
				{
					return;
				}
				DWORD dwStartTick = getTickCountEx();
				SPELL_CONTEXT spellContext;
				spellContext.moveState = pMovePart->getMoveState();
				spellContext.CC.pEntity = m_pEntity;
				spellContext.CC.uidOperator = m_pEntity->getUID();
				spellContext.CC.uidTarget = m_uidTarget;
				spellContext.CC.ptStartTile = ptSelf;
				spellContext.CC.ptTargetTile = ptTarget;
				spellContext.CC.nID = m_nSpellID;
				spellContext.CC.dwStartTick = dwStartTick;
				spellContext.CC.dwUpdateFrame = 0;
				spellContext.EC.pEntity = m_pEntity;
				spellContext.EC.uidOperator = m_pEntity->getUID();
				spellContext.EC.uidTarget = m_uidTarget;
				spellContext.EC.ptStartTile = ptSelf;
				spellContext.EC.ptTargetTile = ptTarget;
				spellContext.EC.nID = m_nSpellID;
				spellContext.EC.dwStartTick = dwStartTick;

				pSpellPart->castSpell(&spellContext);
			}
		}

		if (dwTimerID == 1)
		{
			Stop();
		}
    }

    virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_ENTITY_PREPARE_CAST_SPELL:
            {
                // 被嘲讽不允许使用技能
                // return false;
            }
        }

        return true;
    }

    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_GAME_ENTITY_DIE:
            {
                if (pszContext == NULL || nLen != sizeof(event_entity_die))
                {
                    return;
                }
                event_entity_die* pEvent = (event_entity_die*)pszContext;
                if((m_pEntity != NULL && pEvent->uidTarget == m_pEntity->getUID()) || pEvent->uidTarget == m_uidTarget)
                {
                    Stop();
                }
            }
            break;
        }
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

    UID                     m_uidTarget;
    int                     m_nSpellID;
};
