/*******************************************************************
** 文件名:	ActorInOutLog.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/22/2016
** 版  本:	1.0
** 描  述:	人物上下线日记

********************************************************************/

#include "stdafx.h"
#include "IEntity.h"
#include "IEntityEvent.h"
#include "IBankPart.h"
#include "IServerGlobal.h"
#include "EventDef.h"

#include "ActorInOutLog.h"

////////////////////* DiePart */////////////////////////////////////////////
CActorInOutLog::CActorInOutLog()
    : m_pMaster(0)
    , m_nLoginIdentity(0)
{

}

CActorInOutLog::~CActorInOutLog()
{

}

/** 创建
*/
bool CActorInOutLog::onLoad( __IEntity* pEntity, int nLoginMode)
{
    if(pEntity == NULL)
    {
        return false;
    }

    m_pMaster = pEntity;
    __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent)
    {
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_LOGIN, __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_PREPARE_LOGOUT, __FUNCTION__);
    }

    // 数据库接口注册
    gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);


    return true;
}

void CActorInOutLog::restore()
{
    // 数据库接口反注册
    gServerGlobal->getDBEngineService()->unregisterHandler(this);

    if( m_pMaster )
    {
        __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
        if (pEntityEvent )
        {
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_LOGIN);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_PREPARE_LOGOUT);
        }
    }
    m_pMaster = 0;
}

/** 释放
*/
void CActorInOutLog::release()
{


    //delete this;
}

/** 序列化
*/
bool CActorInOutLog::onSerialize( rkt::ibuffer & in,int flag )
{
    if(flag&TO_OTHER_SEVER)
    { 
        in >> m_nLoginIdentity;
    }

    return true;
}

/** 反序列化
*/
bool CActorInOutLog::deSerialize( rkt::obuf & out,int flag )
{
    if(flag&TO_OTHER_SEVER)
    { 
        out << m_nLoginIdentity;
    }

    return true;
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
void CActorInOutLog::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    if(m_pMaster == NULL)
    {
        return;
    }

    switch(wEventID)
    {
    case EVENT_ACTOR_LOGIN:     // 角色上线
        {
            onEventActorLogin(wEventID, bSrcType, dwSrcID, pszContext, nLen);
        }
        break;

    case EVENT_ACTOR_PREPARE_LOGOUT:    // 角色下线
        {
            onEventActorLogout(wEventID, bSrcType, dwSrcID, pszContext, nLen);
        }
        break;

    default: break;
    }
}

/** 上线事件
*/
void CActorInOutLog::onEventActorLogin(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    if(pszContext == nullptr || nLen != sizeof(event_actor_login))
    {
        return;
    }

    event_actor_login * pEvent = CAST_TYPE(event_actor_login*, pszContext);
    // 上线
    if(pEvent->eMode != elogin_online)
    {
        return;
    }
    // 电脑
    if( IsComputerPDBID(pEvent->dwDBID) )
    {
        return;
    }

    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return;
    }
    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
    if(pBankPart == NULL)
    {
        return;
    }
    SPlayerInfo * pPlayerInfo = pBankPart->getPlayerInfo();

    m_nLoginIdentity = (int)time(NULL);
    
    // 角色上线日志
    GAMEDB_PARAM_ONLINE_LOG log;
    log.nWorldID        = m_pMaster->getFromGameWorldID();		//世界ID
    log.nUserID         = pBankPart->getUserID();				//用户ID
    log.nActorID        = pEvent->dwDBID;                       //角色ID
    log.nLevel          = pPlayerInfo->nLevel;                  //等级
    log.nLoginIdentity  = m_nLoginIdentity;    	                //登录标识    
    log.byLogoutFlag    = 0;			                        //下线状态 0默认无效值 1正常下线 2关服下线 3断线自动下线 4挤号下线 5GM号踢下线 6其它

#ifdef VERSEION_DEBUG
    if(pEvent->dwDBID == 0)
    {
        ErrorLn(__FUNCTION__": UserID="<< log.nUserID <<",pdbid="<<pEvent->dwDBID <<", nWorldID="<< log.nWorldID <<", nLevel="<<log.nLevel);
        __debugbreak();
    }
#endif


    if (pDBEngine->executeSP(GAMEDB_REQUEST_ONLINE_LOG, log.nActorID, (LPCSTR)&log, sizeof(log), static_cast<IDBRetSink *>(this), log.nWorldID ) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_ONLINE_LOG Login failed! PDBID="<<log.nActorID);
    }
}

/** 下线事件
*/
void CActorInOutLog::onEventActorLogout(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    if(m_pMaster == NULL)
    {
        return;
    }
    if( pszContext == nullptr || nLen != sizeof(int) )
    {
        return;
    }

    int * pReason = CAST_TYPE(int*, pszContext);
    // 下线
    if( *pReason != elogout_offline)
    {
        return;
    }

    PDBID dwDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    // 电脑
    if( IsComputerPDBID(dwDBID) )
    {
        return;
    }

    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return;
    }
    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
    if(pBankPart == NULL)
    {
        return;
    }
    SPlayerInfo * pPlayerInfo = pBankPart->getPlayerInfo();

    // 角色下线日志
    GAMEDB_PARAM_ONLINE_LOG log;
    log.nWorldID        = m_pMaster->getFromGameWorldID();		//世界ID
    log.nUserID         = pBankPart->getUserID();				//用户ID
    log.nActorID        = dwDBID;                       //角色ID
    log.nLevel          = pPlayerInfo->nLevel;                  //等级
    log.nLoginIdentity  = m_nLoginIdentity;    	                //登录标识 
    // TODO，后面要细化下具体下线情况以便于后台统计
    log.byLogoutFlag    = 1;			                        //下线状态 0默认无效值 1正常下线 2关服下线 3断线自动下线 4挤号下线 5GM号踢下线 6其它

#ifdef VERSEION_DEBUG
    if(dwDBID == 0)
    {
        ErrorLn(__FUNCTION__": UserID="<< log.nUserID <<",pdbid="<< dwDBID <<", nWorldID="<< log.nWorldID <<", nLevel="<<log.nLevel);
        __debugbreak();
    }
#endif

    if (pDBEngine->executeSP(GAMEDB_REQUEST_ONLINE_LOG, log.nActorID, (LPCSTR)&log, sizeof(log), static_cast<IDBRetSink *>(this), log.nWorldID) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_ONLINE_LOG Logout failed! PDBID="<<log.nActorID);
    }
}

///////////////////////////////// IDBRetSink ////////////////////////////
/** 数据库请求返回回调方法
@param   pDBRetSink		：结果返回接口	
@param   nCmdID ：命令ＩＤ
@param   nDBRetCode：数据库请求返回值，参考上面定义
@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
@param   nQueueIndex：队列定义
@param   pOutData：输出数据
@param   nOutLen：输出数据长度
@return  
@note     
@warning 此对像千万不能在数据库返回前释放，否则会非法！
@retval buffer 
*/	
void CActorInOutLog::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{

}