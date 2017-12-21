/*******************************************************************
** 文件名: UnLockHandler.cpp 
** 版  权: (C) 深圳冰川网络技术有限公司
** 创建人: 李界华
** 日  期: 4/27/2017
** 版  本: 1.0
** 描  述: 默认数据库处理
** 应  用:    
    
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "IDBEngineService.h"
#include "IGameDatabaseDef.h"
#include "IOSSLogService.h"
#include "IShareServer.h"
#include "OssLogDef.h"
#include "SocialGlobalHelper.h"
#include "UnLockHandler.h"

//////////////////////////////////////////////////////////////////////////
// 服务器掉线回调函数
void ServerDisconnectCallback(WORD wSrvType, char* pData,  size_t nDataSize)
{
    ISocialGlobalService * pGlobalService = SocialGlobalHelper().m_ptr;
    if (pGlobalService)
    {
        pGlobalService->handleServerDisconnectShareCallback(wSrvType, pData, nDataSize);
    }
}

//////////////////////////////////////////////////////////////////////////

/** 
@param   
@param   
@return  
*/
CUnLockHandler::CUnLockHandler(void)
{

}

/** 
@param   
@param   
@return  
*/
CUnLockHandler::~CUnLockHandler(void)
{

}

/** 注册数据库处理
@return  
*/
bool CUnLockHandler::create(void)
{
    // 取得DBEngine服务
    IDBEngineService* pDBService = gSocialGlobal->getDBEngineService();
    // 数据库接口注册
    if(pDBService)
    {
        pDBService->registerHandler(this, __FUNCTION__);    
    }
    
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver)
    {
        pShareReceiver->SetServerDisconnectCallback(ServerDisconnectCallback);
    }
    return true;
}

/** 取消注册数据库处理
@return  
*/
bool CUnLockHandler::onStop(void)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver)
    {
        pShareReceiver->SetServerDisconnectCallback(nullptr);
    }

    // 取得DBEngine服务
    IDBEngineService* pDBService = gSocialGlobal->getDBEngineService();
    // 数据库接口反注册
    if(pDBService)
    {   
        pDBService->unregisterHandler(this);
    }

    return true;
}

/** 请求执行解锁
@return  
*/
bool CUnLockHandler::requstExecuteUnlock(void)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == nullptr)
    {
        return false;
    }

    // 用2万人员
    const DWORD ARRAY_SIZE = 20480;
    SPersonInfo* pPersonArray = new SPersonInfo[ARRAY_SIZE];
    DWORD dwPersonCount = pShareReceiver->GetALLPersonInfo(pPersonArray, ARRAY_SIZE);
    if(dwPersonCount == 0)
    {
        return false;
    }

    // 执行解锁
    bool bResult = handleExecute((char*)pPersonArray, dwPersonCount*sizeof(SPersonInfo) );
    
    SAFE_DELETEARRAY(pPersonArray);

    return bResult;
}

/** 执行解锁
@return  
*/
bool CUnLockHandler::handleExecute(char* pData, size_t nDataSize)
{
    if(pData == nullptr || nDataSize < 0)
    {
        return false;
    }

    size_t dwPersonCount = nDataSize / sizeof(SPersonInfo);
    if(dwPersonCount == 0 || nDataSize % sizeof(SPersonInfo) != 0)
    {
        return false;
    }
    SPersonInfo * pPersonArray = (SPersonInfo *)pData;

    char szGameNote[512] = { 0 };
    // 当前游戏世界ID
    DWORD dwCurrentWorldID = getThisGameWorldID();

    typedef std::list<DWORD>  PERSON_PDBID_LIST;
    typedef std::map<DWORD, PERSON_PDBID_LIST> ALL_WORLD_PERSON_MAP;
    ALL_WORLD_PERSON_MAP  mapWorldPerson;

    // 如果在普通区，那么解所有不在线的帐号锁，并且需要判断游戏世界ID，公共区类似
    obuf8192 ob;
    int nSaveUserCount = 0;

    for (DWORD i=0; i<dwPersonCount; ++i)
    {
        if(ob.space() < sizeof(int))
        {
            break;
        }

        SPersonInfo & personInfo = pPersonArray[i];
        int nPDBID = personInfo.dwPlayerID;
        ob.push_back(&nPDBID, sizeof(int));     
        nSaveUserCount++;

        // 不是同一个游戏世界
        if(personInfo.dwWorldID != dwCurrentWorldID)
        {
            mapWorldPerson[personInfo.dwWorldID].push_back(personInfo.dwPlayerID);
        }
    }

    if( executeUnLock(dwCurrentWorldID, dwCurrentWorldID, nSaveUserCount, ob.data(), ob.size()) )
    {
        //sprintf_s(szGameNote, "调用强制解锁全游戏世界(gameworldid=%d)成功. 操作者=%s", dwCurrentWorldID, m_stringUnlockOperator.c_str());  
        TraceLn(szGameNote);
    }
    else
    {
        //sprintf_s(szGameNote, "调用强制解锁全游戏世界(gameworldid=%d)失败. 操作者=%s", dwCurrentWorldID, m_stringUnlockOperator.c_str());  
        ErrorLn(szGameNote);
    }

    // 在公共区，不仅要解公共区数据库内的帐号锁，还要解连接普通区的数据库锁
    for(ALL_WORLD_PERSON_MAP::iterator iter = mapWorldPerson.begin(); iter != mapWorldPerson.end(); ++iter) 
    {
        DWORD dwThisWorldID = iter->first;
        // 对每个游戏区进行解锁
        ob.reset();
        nSaveUserCount = 0;        

        PERSON_PDBID_LIST & person_list = iter->second;
        for(PERSON_PDBID_LIST::iterator it = person_list.begin(); it != person_list.end(); ++it )
        {
            if(ob.space() < sizeof(int))
            {
                break;
            }
            int nPDBID = (int)(*it);
            ob.push_back(&nPDBID, sizeof(int));     
            nSaveUserCount++;
        }

        if( executeUnLock(dwCurrentWorldID, dwThisWorldID, nSaveUserCount, ob.data(), ob.size()) )
        {
            //sprintf_s(szGameNote, "调用强制解锁全游戏世界(gameworldid=%d)成功. 操作者=%s", dwThisWorldID, m_stringUnlockOperator.c_str());  
            TraceLn(szGameNote);
        }
        else
        {
            //sprintf_s(szGameNote, "调用强制解锁全游戏世界(gameworldid=%d)失败. 操作者=%s", dwThisWorldID, m_stringUnlockOperator.c_str()); 
            ErrorLn(szGameNote);
        }
    }

    // 写入操作日记
    IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
    if ( pOSSLogService != NULL )
    {
        pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_SYSTEM, 0, szGameNote);
    }
    
    return true;
}

/** 执行强制解锁
@param   dwOperateWorldID：数据记录对应游戏世界ID
@param   dwSinkWorldID   ：要操作的目标游戏世界ID
@param   pszInData       ：输入数据
@param   nInDataLen      ：输入数据长度
@return         
@retval buffer 
*/
bool CUnLockHandler::executeUnLock(DWORD dwOperateWorldID, DWORD dwSinkWorldID, int nActorIDCount, LPCSTR pszInData, int nInDataLen)
{
    // 取得DBEngine服务
    IDBEngineService* pDBService = gSocialGlobal->getDBEngineService();
    // 数据库接口反注册
    if(pDBService == nullptr)
    {
        return false;
    }    

    DBREQ_PARAM_FORCEUNLOCK dbParam;
    dbParam.nGameWorldID = dwOperateWorldID;
    dbParam.nInGameUserCount = nActorIDCount;

    obuf ob;
    ob.push_back(&dbParam, sizeof(dbParam));
    int nBufLen = nActorIDCount*sizeof(int);
    ob.push_back(&nBufLen, sizeof(int));
    ob.push_back(pszInData, nInDataLen);

    if (pDBService->executeSP(GAMEDB_REQUEST_FORCEUNLOCK, 0, ob.data(), ob.size(), static_cast<IDBRetSink *>(this), dwSinkWorldID) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_FORCEUNLOCK failed!");
        return false;
    }
    return true;
}

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
void CUnLockHandler::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        if(nCmdID == GAMEDB_REQUEST_FORCEUNLOCK)
        {
            char szGameNote[512] = { 0 };
            if(pszDBRetDesc != NULL)
            {
                sprintf_s(szGameNote, "Force Unlock Game DataBase Execute Failed Reason:%s", pszDBRetDesc);
            }
            else
            {
                sprintf_s(szGameNote, "Force Unlock Game DataBase Execute Failed Reason.");
            }

            ErrorLn(szGameNote);
            //ICommonService * pCommonService = gGlobalSocialServer->GetCommonService();
            //if(pCommonService != NULL)
            //{
            //    pCommonService->TrackGameNote(ENoteTrackType_System, 0, szGameNote);
            //}
        }
    }
    else
    {
        if(nCmdID == GAMEDB_REQUEST_FORCEUNLOCK)
        {   
            TraceLn("Force Unlock Game DataBase Execute Success!");
/*            ICommonService * pCommonService = gGlobalSocialServer->GetCommonService();
            if(pCommonService != NULL)
            {
                pCommonService->TrackGameNote(ENoteTrackType_System, 0, "调用强制解锁全游戏世界数据库返回成功.");
            } */          
        }
    }
}