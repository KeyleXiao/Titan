/*******************************************************************
** 文件名:	EffectCreateMoveBarrier.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	10/17/2017
** 版  本:	1.0
** 描  述:	效果-创建移动障碍
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "IEntity.h"
#include "EntityHelper.h"
#include "EventDef.h"
#include "ISpell.h"
#include "TimerHandler.h"

class CEffectCreateMoveBarrier : public IEffectEx, public TimerHandler
{
public:
    typedef  EffectServer_CreateMoveBarrier    SCHEME_DATA;

    CEffectCreateMoveBarrier( SCHEME_DATA & data ) : m_data(data), m_pEntity(NULL), m_dwSN(0)
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
        if( context==0 || context->pEntity==0 )
        {
            return false;
        }

        Stop();

        m_pEntity = context->pEntity;
        
        __ISpellPart* pSpellPart = (__ISpellPart*)m_pEntity->getEntityPart(PART_SPELL);
        if(pSpellPart == NULL)
        {
            return false;
        }

        m_dwSN = pSpellPart->CreateAttackObjectSerialID(); 

        event_block_barrier_create evCreate;
        evCreate.nBarrierShape = m_data.eShape;
        evCreate.nBlockType = BARRIER_BLOCK_MOVE;
        evCreate.uidCreater = m_pEntity->getUID();
        evCreate.dwSerial = m_dwSN;
        evCreate.nTargetFilter = m_data.nTargetFilter;

        switch(m_data.eShape)
        {
        case BARRIER_SHAPE_CIRCLE:
            {
                evCreate.circle.locCenter = chooseLoc(m_data.locCircleCenterLoc, context); 
                evCreate.circle.fInnerRadius = m_data.fCircleInnerRadius;
                evCreate.circle.fOutterRadius = m_data.fCircleOutterRadius;
            }
            break;

        case BARRIER_SHAPE_LINE:
            {
                evCreate.line.locStart = chooseLoc(m_data.locLineStart, context);
                evCreate.line.locEnd = chooseLoc(m_data.locLineEnd, context);
                evCreate.line.fWidth = m_data.fLineWidth;
                evCreate.line.fHeight = m_data.fLineHeight;
            }
            break;

        case BARRIER_SHAPE_CYLINDER:
            {
                evCreate.cylinder.locCenter = chooseLoc(m_data.locCircleCenterLoc, context);
                evCreate.cylinder.fRadius = m_data.fCylinderRadius;
                evCreate.cylinder.fHeight = m_data.fCylinderHeight;
            }
            break;
        default:
            {
                ErrorLn("undefined shape...");
                return false;
            }
        }

        // 发出事件
        int nSceneID = m_pEntity->getSceneID(); 
        SceneLogicEventHelper eventHelper(nSceneID);
        if(eventHelper.m_ptr)
        {
            eventHelper.m_ptr->FireExecute(EVENT_GAME_BLOCK_BARRIER_CREATE, SOURCE_TYPE_SCENE, nSceneID, (LPCSTR)&evCreate, sizeof(evCreate));
        }

        // 启动销毁定时器
        if(m_data.nMaxTime > 0)
        {
            g_EHelper.SetTimer(0, m_data.nMaxTime, this, 1, "CEffectCreateMoveBarrier");
        }

        return true;
    }

    virtual void OnTimer( unsigned long dwTimerID )
    {
        Stop();
    }

    // 效果停止
    virtual void			Stop()
    {
        if(m_pEntity != NULL)
        {
            event_block_barrier_destroy evDestroy;
            evDestroy.uidCreater = m_pEntity->getUID();
            evDestroy.dwSerial = m_dwSN;
            evDestroy.nBlockType = BARRIER_BLOCK_MOVE;

            int nSceneID = m_pEntity->getSceneID(); 
            SceneLogicEventHelper eventHelper(nSceneID);
            if(eventHelper.m_ptr)
            {
                eventHelper.m_ptr->FireExecute(EVENT_GAME_BLOCK_BARRIER_DESTROY, SOURCE_TYPE_SCENE, nSceneID, (LPCSTR)&evDestroy, sizeof(evDestroy));
            }

            m_pEntity = NULL;
        }
        
        m_dwSN = 0;
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectCreateMoveBarrier(m_data);
    }

    // 释放
    virtual void			Release() 
    {
        Stop();
        delete this;
    }
private:
    Vector3 chooseLoc(CHOOS_POS_TYPE eT, EFFECT_CONTEXT* pContext)
    {
        switch(eT)
        {
        case CHOOS_POS_TILE:
            {
                return pContext->ptTargetTile;
            }
            break;
        case CHOOS_POS_SELF:
            {
                if(m_pEntity == NULL)
                {
                    return INVALID_LOC;
                }
                return m_pEntity->getLocation();
            }
            break;
        case CHOOS_POS_TARGET:
            {
                return getLocation(pContext->uidTarget);
            }
            break;
        case CHOOS_POS_OPERATOR:
            {
                return getLocation(pContext->uidOperator);
            }
            break;
        case UCHOOS_POS_ANGLE:
            {
                return m_pEntity->getLocation();
            }
            break;
        }

        return INVALID_LOC;
    }

private:
    SCHEME_DATA               m_data;
    __IEntity*                m_pEntity;
    DWORD                     m_dwSN;
};
