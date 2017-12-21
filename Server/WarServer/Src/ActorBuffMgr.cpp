/*******************************************************************
** 文件名: ActorBuffMgr.cpp 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 6/22/2017
** 版  本: 1.0
** 描  述: 人物增加BUFF收集管理

********************************************************************/

#include "stdafx.h"
#include "IEntity.h"
#include "IEntityEvent.h"
#include "IServerGlobal.h"
#include "IWarPart.h"
#include "ITimerService.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "EntityHelper.h"
#include "ActorBuffMgr.h"


/////////////////////////////////////////////////////////////////
CActorBuffMgr::CActorBuffMgr()
    : m_pWarPart(0)
{

}

CActorBuffMgr::~CActorBuffMgr()
{

}

/** 创建
*/
bool CActorBuffMgr::onLoad( __IWarPart* pWarPart)
{
    if ( !pWarPart || !pWarPart->getHost() )
    {
        return false;
    }

    m_pWarPart = pWarPart;

    __IEntity * pMaster = getHost();
    if(pMaster)
    {
        __IEntityEvent *pEntityEvent = pMaster->getEntityEvent();
        if (pEntityEvent)
        {
            // 订阅 增加BUFF事件
            pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ADDBUFF, __FUNCTION__);
        }
    }

    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService)
    {
        // 启动定时器（5分钟）
        pTimerService->SetTimer(NOTIFY_FLUSH_BUFFLIST_TIMER, TIMER_INTERVAL, this, INFINITY_CALL, __FUNCTION__);
    }

    return true;
}

/** 重置
*/
void CActorBuffMgr::restore()
{
    // 关闭定时器
    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->KillTimer(NOTIFY_FLUSH_BUFFLIST_TIMER, this);
    }

    __IEntity * pMaster = getHost();
    if(pMaster)
    {
        __IEntityEvent *pEntityEvent = pMaster->getEntityEvent();
        if (pEntityEvent )
        {
            // 取消订阅 增加BUFF事件
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ADDBUFF);
        }
    }

    m_pWarPart = 0;
}

/** 释放
*/
void CActorBuffMgr::release()
{
    restore();

    delete this;
}

__IEntity *	CActorBuffMgr::getHost()
{
    return m_pWarPart ? m_pWarPart->getHost() : NULL;
}

///////////////////////////////// IEventExecuteSink ////////////////////////////
/** 
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  
@note     
@warning 
@retval buffer 
*/
void CActorBuffMgr::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
    case EVENT_ENTITY_ADDBUFF:     // 角色增加BUFF事件
        {
            onEventActorAddBuff(wEventID, bSrcType, dwSrcID, pszContext, nLen);
        }
        break;

    default: break;
    }
}

/** 角色增加BUFF事件
*/
void CActorBuffMgr::onEventActorAddBuff(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    if(pszContext == nullptr || nLen != sizeof(event_entity_add_buff))
    {
        return;
    }

    event_entity_add_buff * pEvent = CAST_TYPE(event_entity_add_buff*, pszContext);
    // 加BUFF失败
    if( pEvent->nErrorID != 0 )
    {
        return;
    }
    // 排除给自己增加BUFF操作
    if( pEvent->uidEffect == pEvent->uidAdd )
    {
        return;
    }
    // 排除怪物
    if ( UID_2_TYPE(pEvent->uidEffect) == TYPE_MONSTER || UID_2_TYPE(pEvent->uidAdd) == TYPE_MONSTER )
    {
        return;
    }
    __IEntity * pMaster = getHost();
    if( !pMaster || pMaster->getUID() != pEvent->uidEffect)
    {
        return;
    }

    // 更新下增加BUFF时间
    m_mapBuffList[pEvent->uidAdd] = getTickCountEx();
}


///////////////////////////////// TimerHandler ////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param   dwTimerID: 定时器ID,用于区分是哪个定时器
@return           : empty
*/
void CActorBuffMgr::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case NOTIFY_FLUSH_BUFFLIST_TIMER:
        {
            onTimerFlushBuffList();
        }
        break;

    default: break;
    }
}


/** 通知战场刷新BUFF列表
*/
void CActorBuffMgr::onTimerFlushBuffList(void)
{
    __IEntity * pMaster = getHost();
    if( !pMaster)
    {
        return;
    }
    int nWarID = m_pWarPart->getWarID();
    // 非战场退出
    if( nWarID == -1 )
    {
        // 关闭定时器
        ITimerService * pTimerService = gServerGlobal->getTimerService();
        if(pTimerService)
        {
            pTimerService->KillTimer(NOTIFY_FLUSH_BUFFLIST_TIMER, this);
        }
        return;
    }

    if(m_mapBuffList.size() == 0) {
        return;
    }

	event_game_add_assist_buff event;
    event.uid = pMaster->getUID();
    event.nCount = (int)m_mapBuffList.size(); 

    obuf ob;
    ob.push_back(&event, sizeof(event));    

    for(map<UID, DWORD>::iterator iter = m_mapBuffList.begin(); iter != m_mapBuffList.end(); ++iter)
    {
        SActor_Add_Buff_Node node;
        node.uidAdd = iter->first;
        node.dwUpdateTick = iter->second;

        ob.push_back(&node, sizeof(node));
    }

    // 清空数据
    m_mapBuffList.clear();

	pMaster->getEntityEvent()->FireExecute(EVENT_GAME_ADD_ASSIST_BUFF, ob.data(), ob.size());
}
