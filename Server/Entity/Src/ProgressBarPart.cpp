/*******************************************************************
** 文件名:	ProgressBarPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	2/26/2015
** 版  本:	1.0
** 描  述:	

			进度条部件
********************************************************************/

#include "stdafx.h"
#include "ProgressBarPart.h"
#include "IServerGlobal.h"
#include "ITimerService.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "EntityHelper.h"

bool ProgressBarPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	m_pMaster = pEntity;
	return true;
}

bool ProgressBarPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	return true;
}

void ProgressBarPart::release()
{
	CloseProgressBar(0,false);
	m_pMaster = 0;
	delete this;
}

bool ProgressBarPart::StartProgressBar( ProgressBarContext * context )
{
	if ( m_pContext!=0 )
		return false;

    if (m_pMaster == NULL)
    {
        return false;
    }
    m_vStartPoint.x = 0;
    m_vStartPoint.y = 0;
    m_vStartPoint.z = 0;

	// 保存现场
	m_pContext = new ProgressBarContext(*context);
    // 不能直接用sizeof(ProgressBarContext) 因为ProgressBarContext里有一个成员std::string 长度未定义
	//memcpy( m_pContext,context,sizeof(ProgressBarContext) );

    // 设置进度条开始时间
    m_dwStartTime = getTickCountEx();

	// 启动定时器
	gServerGlobal->getTimerService()->SetTimer( 0,m_pContext->nTime,this,1,__FILE__);

	// 订阅事件
	if ( !SubscibeEvents() )
		return false;

	// 发送给客户端
	if ( m_pContext->bSendClient )
	{

	}

	return true;
}

void ProgressBarPart::CloseProgressBar(IEventExecuteSink * pSink,bool bBreak)
{
	if ( m_pMaster==0 || m_pContext==0 )
		return;

	// 不要把别人的进度条停掉了
	if ( pSink!=0 && pSink!=m_pContext->pCallBack )
		return;

	// 停止计时
	gServerGlobal->getTimerService()->KillTimer( 0,this);

	// 退订事件
	UnSubscibeEvents();

	// 发送给客户端
	if ( m_pContext->bSendClient )
	{

	}

	// 发送打断事件
	if ( bBreak && m_pContext->pCallBack )
	{
		UID uidEntity = m_pMaster->getUID();
		/* 此事件后可能会释放m_pContext,
		   比如调用关闭进度条CloseProgressBar，
		   m_pContext使用要判断，或者移到上面实现
		*/
		m_pContext->pCallBack->OnExecute(EVENT_ENTITY_PROGRESS_BREAK, UID_2_TYPE(uidEntity), uidEntity, (LPCSTR)m_pContext, sizeof(ProgressBarContext));
	}

	// 清理现场
	safeDelete(m_pContext);
}

ProgressBarContext * ProgressBarPart::GetCurrentContext()
{
	return m_pContext;
}

void ProgressBarPart::OnTimer( unsigned long dwTimerID )
{
	if ( dwTimerID==0 )
	{
		// 发送完成事件
		if ( m_pMaster!=0 && m_pContext!=0 && m_pContext->pCallBack )
		{
			UID uidEntity = m_pMaster->getUID();
			/* 此事件后可能会释放m_pContext,
			   比如调用关闭进度条CloseProgressBar，
			   m_pContext使用要判断，或者移到上面实现
			*/
			m_pContext->pCallBack->OnExecute(EVENT_ENTITY_PROGRESS_OK, UID_2_TYPE(uidEntity), uidEntity, (LPCSTR)m_pContext, sizeof(ProgressBarContext));
		}

		CloseProgressBar(0,false);
	}

	if ( dwTimerID==1 )
	{
		int nBreakFlag = m_pContext->nBreakFlag;

		// 超出距离打断
		if ((nBreakFlag & BREAK_DISTANCE) == BREAK_DISTANCE )
		{
            // 超过时间，才允许打断
            if (getTickCountEx() > m_dwStartTime + BREAK_DISTANCE_TIME)
            {
                Vector3 ptMyLoc = m_pMaster->getLocation();
                Vector3 ptTarget= getLocation(m_pContext->uidTarget);
                if ( ptMyLoc.checkDistance( ptTarget,m_pContext->fDistanceRange )==false )
                {
                    CloseProgressBar( 0,true ); // 打断
                }
            }
		}
	}
}

void ProgressBarPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if ( m_pContext==0 || m_pMaster==0 )
		return;

	switch(wEventID)
	{
	case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION:
		{
			if (pszContext == NULL || nLen != sizeof(event_entity_property_change))
			{
				return;
			}

            event_entity_property_change* pEvent = (event_entity_property_change*)pszContext;

            if(pEvent->nPropertyID != PROPERTY_LOCATION)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return;
            }

            // 不超过时间，不允许打断
            if (getTickCountEx() <= m_dwStartTime + BREAK_DISTANCE_TIME)
            {
                return;
            }

            // 因为移动时,使用技能是客户端坐标，取那时候坐标，很容易被打断,不允许打断后时候的坐标
            if (equalto0(m_vStartPoint.x) && equalto0(m_vStartPoint.y) && equalto0(m_vStartPoint.z))
            {
                m_vStartPoint = m_pMaster->getLocation();
            }

            Vector3 loc;
            memcpy(&loc, pEvent->data, pEvent->nLen);

			// 距离过小，不打断
			if (m_vStartPoint.checkDistance(loc, 0.5))
			{
				return;
			}
		}
		break;
	case /*EVENT_ENTITY_CAST_SPELL*/EVENT_ENTITY_SPELL_PREPARE:
		{
			if (pszContext == NULL || nLen != sizeof(event_spell_prepare))
			{
				return;
			}
			event_spell_prepare *pCastSpell = (event_spell_prepare *)pszContext;
			// 不打断进度条
			if ((pCastSpell->nCastType&SpellCastType_BreakProgressBar) != SpellCastType_BreakProgressBar)
			{
				return;
			}

            // 同技能ID不被打断
            if (pCastSpell->nID == m_pContext->nSpellID)
            {
                return;
            }
		}
		break;
	default:
		break;
	}

	// 打断读条
	CloseProgressBar( 0,true );
}

bool ProgressBarPart::SubscibeEvents()
{
	if ( m_pContext==0 || m_pMaster==0 )
		return false;

	__IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
	if ( pEventEngine==0 )
		return false;

	int nBreakFlag = m_pContext->nBreakFlag;

	// 移动打断
	if ( nBreakFlag & BREAK_MOVE )
	{
        pEventEngine->Subscibe(this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION), "ProgressBar");
	}

	// 被击打断
	if ( nBreakFlag & BREAK_HIT )
	{
		pEventEngine->Subscibe( this,EVENT_ENTITY_DAMAGE,"ProgressBar");
	}

	// 死亡打断
	if ( nBreakFlag & BREAK_DEAD )
	{
		pEventEngine->Subscibe( this,EVENT_ENTITY_DIE,"ProgressBar");
	}

	// 超出距离则启动距离检测定时器
	if ( nBreakFlag & BREAK_DISTANCE )
	{
		gServerGlobal->getTimerService()->SetTimer( 1,UPDATE_FRAME_TIMER,this,INFINITY_CALL,__FILE__);
	}

	// 效果打断
	if ( nBreakFlag & BREAK_EFFECT )
	{
		pEventEngine->Subscibe( this,EVENT_ENTITY_EFFECT_BREAK,"ProgressBar");
	}

	// 取消施法打断
	if ( nBreakFlag & BREAK_CANCEL_CAST )
	{
		pEventEngine->Subscibe( this,EVENT_ENTITY_CANCEL_CAST,"ProgressBar");
	}

	// 施法技能打断
	if ( nBreakFlag & BREAK_CAST_SPELL )
	{
		pEventEngine->Subscibe( this,/*EVENT_ENTITY_CAST_SPELL*/EVENT_ENTITY_SPELL_PREPARE,"ProgressBar");
	}

	return true;
}

bool ProgressBarPart::UnSubscibeEvents()
{
	if ( m_pContext==0 || m_pMaster==0 )
		return false;

	__IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
	if ( pEventEngine==0 )
		return false;

	int nBreakFlag = m_pContext->nBreakFlag;

	// 移动打断
	if ( nBreakFlag & BREAK_MOVE )
	{
        pEventEngine->UnSubscibe(this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION));
	}

	// 被击打断
	if ( nBreakFlag & BREAK_HIT )
	{
		pEventEngine->UnSubscibe( this,EVENT_ENTITY_DAMAGE);
	}

	// 死亡打断
	if ( nBreakFlag & BREAK_DEAD )
	{
		pEventEngine->UnSubscibe( this,EVENT_ENTITY_DIE);
	}

	// 超出距离则启动距离检测定时器
	if ( nBreakFlag & BREAK_DISTANCE )
	{
		gServerGlobal->getTimerService()->KillTimer( 1,this );
	}

	// 效果打断
	if ( nBreakFlag & BREAK_EFFECT )
	{
		pEventEngine->UnSubscibe( this,EVENT_ENTITY_EFFECT_BREAK);
	}

	// 取消施法打断
	if ( nBreakFlag & BREAK_CANCEL_CAST )
	{
		pEventEngine->UnSubscibe( this,EVENT_ENTITY_CANCEL_CAST);
	}

	// 施法技能打断
	if ( nBreakFlag & BREAK_CAST_SPELL )
	{
		pEventEngine->UnSubscibe( this,/*EVENT_ENTITY_CAST_SPELL*/EVENT_ENTITY_SPELL_PREPARE);
	}

	return true;
}

void ProgressBarPart::restore()
{

}

//////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) __IProgressBarPart * CreateEntityProgressBar()
{
	return new ProgressBarPart;
}