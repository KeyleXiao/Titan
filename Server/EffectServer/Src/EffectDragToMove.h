/*******************************************************************
** 文件名:	EffectDragToMove.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/06/2015
** 版  本:	1.0
** 描  述:	效果-拖动目标移动
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "EntityHelper.h"

class CEffectDragToMove : public IEffectEx, public TimerHandler, public IEventExecuteSink
{
public:
	typedef  EffectServer_DragToMove    SCHEME_DATA;

	CEffectDragToMove( SCHEME_DATA &data ) : m_data(data),m_uidDrag(INVALID_UID),m_uidByDrag(INVALID_UID),m_vAngle(0,0,0),m_bSetEffect(false),m_dwLastTick(0)
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

		if (m_bSetEffect)
		{
			ErrorLn(__FUNCTION__ << ", effect is running, effectid=" << m_data.nID);
			Stop();
		}

		switch (m_data.nDragOperatorType)
		{
		case MA_TAGET_TO_SELF:
			{
				m_uidDrag = context->uidOperator;
				m_uidByDrag = context->uidTarget;
			}
			break;
		case MA_TAGET_TO_TARGET:
			{
				m_uidDrag = context->uidTarget;
				m_uidByDrag = context->uidOperator;
			}
			break;
		default:
			{
				ErrorLn("CEffectDragToMove error nDragOperatorType="<<m_data.nDragOperatorType);
				return false;
			}
			break;
		}
		if (isInvalidUID(m_uidDrag) || isInvalidUID(m_uidByDrag) || m_uidDrag == m_uidByDrag)
		{
			return false;
		}

        DWORD dwSceneID = UID_2_SCENE(m_uidDrag);
        SceneLogicEventHelper eventHelper(dwSceneID);
        IEventEngine *pGlobalEvent = eventHelper.m_ptr;
        if (pGlobalEvent != NULL)
        {
            // 注册拖动者死亡事件
            pGlobalEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, UID_2_TYPE(m_uidDrag), m_uidDrag, "CEffectDragToMove");
            // 注册被拖动死亡事件
            pGlobalEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, UID_2_TYPE(m_uidByDrag), m_uidByDrag, "CEffectDragToMove");
        }

		// 设置启用效果
		m_bSetEffect = true;
		// 设置面向角度
		m_vAngle = (getLocation(m_uidByDrag) - getLocation(m_uidDrag)).normalize();

		m_dwLastTick = getTickCountEx();

		setEntityLoc(true);
	
		g_EHelper.SetTimer(0, 50, this, INFINITY_CALL, "CEffectDragToMove", false);

        // 通知被拖动者 不允许自主移动
        bool bFlag = true;
        g_EHelper.sendEntityMessage(m_uidByDrag, PART_MOVE, SS_MSG_FORBID_MOVE_INITIATIVELY, (LPCSTR)&bFlag, sizeof(bFlag));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_bSetEffect)
		{
			m_bSetEffect = false;

            relieveAddBuff();

			setEntityLoc(true);

            DWORD dwSceneID = UID_2_SCENE(m_uidDrag);
            SceneLogicEventHelper eventHelper(dwSceneID);
            IEventEngine *pGlobalEvent = eventHelper.m_ptr;
            if (pGlobalEvent != NULL)
            {
                // 注销拖动者死亡事件
                pGlobalEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, UID_2_TYPE(m_uidDrag), m_uidDrag);
                // 注销被拖动死亡事件
                pGlobalEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, UID_2_TYPE(m_uidByDrag), m_uidByDrag);
            }

			g_EHelper.KillTimer(0, this);

            bool bFlag = false;
            g_EHelper.sendEntityMessage(m_uidByDrag, PART_MOVE, SS_MSG_FORBID_MOVE_INITIATIVELY, (LPCSTR)&bFlag, sizeof(bFlag));

            m_uidDrag = 0;
            m_uidByDrag = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectDragToMove(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		// 取得当前时间
		DWORD dwTick = getTickCountEx();
		if (dwTick < m_dwLastTick + m_data.nTime)
		{
			// 更新坐标
			setEntityLoc(false);
		}
		else
		{
			Stop();
		}
	}

	void setEntityLoc(bool bTransport)
	{
        if (isInvalidUID(m_uidDrag))
        {
            return;
        }
        // 场景ID
        int nSceneID = UID_2_SCENE(m_uidDrag);
        PathFindHelper helper(nSceneID);
        IPathFindService *pPathService = helper.m_ptr;
        if (pPathService == NULL)
        {
            return;
        }

		// 算出被拖动者方向
		Vector3 vByDragAngle;
		switch (m_data.nDirectionType)
		{
		case DIRECTION_FRONT:
			{
				vByDragAngle = m_vAngle;
			}
			break;
		case DIRECTION_BACK:
			{
				vByDragAngle = m_vAngle*(-1.0);
			}
			break;
		case DIRECTION_LEFT:
			{
				Vector3 vUp(0,1,0);
				vByDragAngle = m_vAngle.getCrossProduct(vUp);
			}
			break;
		case DIRECTION_RIGHT:
			{
				Vector3 vUp(0,1,0);
				vByDragAngle = vUp.getCrossProduct(m_vAngle);
			}
			break;
		default:
			{
				ErrorLn("CEffectDragToMove error nDirectionType="<<m_data.nDirectionType);
				return;
			}
			break;
		}

		// 取得被拖动者位置
		Vector3 vNowLoc = pPathService->getRaycast(getLocation(m_uidDrag), vByDragAngle.normalize(), m_data.fDistance);

        gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(getMapID(m_uidDrag), vNowLoc, 100, 100);

        // 传送
		Transport(m_uidByDrag, vNowLoc, Vector3(-1,-1,-1), bTransport);
	}

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 容器事件订阅者事件执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	*/
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch (wEventID)
        {
        case EVENT_GAME_ENTITY_DIE:
            {
                Stop();
            }
            break;
        default:
            break;
        }
    }

    void relieveAddBuff()
    {
        if (!isInvalidUID(m_uidDrag) && m_data.nDragBuffID > 0 && m_data.nDragBuffLevel > 0)
        {
            AddBuff(m_uidDrag, m_data.nDragBuffID, m_data.nDragBuffLevel, m_uidDrag);
        }
        
        if (!isInvalidUID(m_uidByDrag) && m_data.nByDragBuffID > 0 && m_data.nByDragBuffLevel > 0)
        {
            AddBuff(m_uidByDrag, m_data.nByDragBuffID, m_data.nByDragBuffLevel, m_uidByDrag);
        }
    }

private:
	SCHEME_DATA             m_data;

	// 拖动者
	UID						m_uidDrag;
	// 被拖动者
	UID						m_uidByDrag;

	Vector3					m_vAngle;

	// 是否设置效果
	bool					m_bSetEffect;
	// 上次时间
	DWORD					m_dwLastTick;
};
