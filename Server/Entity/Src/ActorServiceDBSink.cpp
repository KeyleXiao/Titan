/*******************************************************************
** 文件名: ActorServiceDBSink.cpp
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 4/27/2017
** 版  本: 1.0
** 描  述: ActorService数据库处理

********************************************************************/

#include "stdafx.h"
#include "IServerGlobal.h"
#include "ActorService.h"
#include "ActorServiceDBSink.h"

////////////////////* DiePart */////////////////////////////////////////////
CActorServiceDBSink::CActorServiceDBSink()
    : m_pActorService(nullptr)
{

}

CActorServiceDBSink::~CActorServiceDBSink()
{

}

/** 创建
*/
bool CActorServiceDBSink::on_start( ActorService* pActorService)
{
    if(pActorService == nullptr)
    {
        return false;
    }

    m_pActorService = pActorService;


    // 数据库接口注册
    IDBEngineService* pDBEngineService =  gServerGlobal->getDBEngineService();
    if( pDBEngineService )
    {
        pDBEngineService->registerHandler(CAST_TYPE(IDBRetSink*, this), __FUNCTION__);
    }
    

    return true;
}

/** 释放
*/
void CActorServiceDBSink::on_stop()
{
    // 数据库接口反注册
    IDBEngineService* pDBEngineService =  gServerGlobal->getDBEngineService();
    if( pDBEngineService )
    {
        pDBEngineService->unregisterHandler(this);
    }

    m_pActorService = nullptr;
}


/** 从数据中加载角色数据
*/
bool CActorServiceDBSink::loadActorDataFromDataBase(DWORD dwUser, PDBID dbid)
{
    do
    {
        if(dwUser == 0)
            break;

        if(dbid == INVALID_PDBID)
            break;

        IDBEngineService* pDBEngineService =  gServerGlobal->getDBEngineService();
        if (pDBEngineService == nullptr)
            break;

        DBREQ_PARAM_SELECTACTOR param;
        param.dwUserID = dwUser;
        param.dwActorID = dbid;
        return pDBEngineService->executeSP(GAMEDB_REQUEST_SELECTACTOR, 0, (LPCSTR)&param, sizeof(param), CAST_TYPE(IDBRetSink*, this));

    }while(false);

    return false;   
}

/** 玩家登陆设置锁标识
*/
bool CActorServiceDBSink::setActorLoginLockFlag(DWORD dwUser, PDBID dbid)
{
    if( IsComputerPDBID(dbid) )
    {
        return false;
    }
    IDBEngineService* pDBEngineService =  gServerGlobal->getDBEngineService();
    if( pDBEngineService == nullptr)
    {
        return false;
    }

    DBREQ_PARAM_SETUSERLOCK reqsetuserlock;
    reqsetuserlock.dwUserID         = dwUser;
    reqsetuserlock.nLockFlag        = 2/*永久保护锁*/;   
    reqsetuserlock.OnOffLine        = 1/*上线*/;
    reqsetuserlock.dwActorID        = dbid;
    reqsetuserlock.nCheckActorExist = 0/*不需要验证*/;
    reqsetuserlock.nZoneSvrHardID   = gServerGlobal->getServerID();
    reqsetuserlock.nGameWorldID     = getThisGameWorldID();
    if( !pDBEngineService->executeSP(GAMEDB_REQUEST_SETUSERLOCK, reqsetuserlock.dwUserID, (LPCSTR)&reqsetuserlock, sizeof(reqsetuserlock), CAST_TYPE(IDBRetSink*, this)) )
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_SETUSERLOCK login failed! PDBID="<<reqsetuserlock.dwActorID);
        return false;
    }
    return true;
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
void CActorServiceDBSink::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    if(nDBRetCode != DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__ << ":cmdid="<<nCmdID <<",nQueueIndex="<<nQueueIndex <<", desc="<< (pszDBRetDesc ? "" : pszDBRetDesc) );
        return;
    }

    switch(nCmdID)
    {
        case GAMEDB_REQUEST_SELECTACTOR:
            {
                m_pActorService->onDBReturnPlayerRoleData(pOutData, nOutLen);
            }
        break;

        case GAMEDB_REQUEST_SETUSERLOCK:
            {
                onReturnSetLockFlag(nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
            }
            break;

        default: break;
    }
}

void CActorServiceDBSink::onReturnSetLockFlag(int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    if(nCmdID != GAMEDB_REQUEST_SETUSERLOCK)
    {
        ErrorLn(__FUNCTION__"() db return command is not same, nCmdID="<< nCmdID <<", dwUDBID="<< nQueueIndex);
        return;
    }

    // TODO 这块的流程要重新梳理下
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        //char szUnlockReason[1024] = { 0 };
        //sprintf_s(szUnlockReason, "[登陆]加永久锁时DB返回失败, DBError Desc=%s", pszDBRetDesc);
        //g_CancelUserSolidLock.CancelSolidLock(nQueueIndex, getThisGameWorldID(), szUnlockReason);
        //pTerminalTunnel->Kickout(EKickoutClient_Reason_UserLocked, pszDBRetDesc);
        return;
    }

    if(nOutLen != sizeof(DBREQ_RESULT_SETUSERLOCK))
    {			
        //g_CancelUserSolidLock.CancelSolidLock(nQueueIndex, getThisGameWorldID(), "[登陆]加永久锁时返回数据大小不合法");
        //pTerminalTunnel->Kickout(EKickoutClient_Reason_UserLocked, "请求异常,请向游戏管理员请求支持[0x4F95].");
        return;
    }

}