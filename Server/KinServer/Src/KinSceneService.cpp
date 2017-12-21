#include "stdafx.h"
#include "KinSceneService.h"
#include "KinDef.h"
#include "ICenterConnectorService.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "IDBEngineService.h"
#include "ExternalHelper.h"
#include "EventDef.h"
#include "IManagerConnectorService.h"
#include "IMessageDispatch.h"
#include "strconv.h"
#include "TitleDef.h"
#include "KinHelper.h"
#include "OssLogDef.h"

void KinScenService::release()
{
    TraceLn("KinSceneService::release");

    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__ ": failed! can not found the IMessageDispatch! ");
        return;
    }

    // 自己需要处理消息
    pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_KIN);

    // 取消注册响应客户端消息
    pMessageDispatch->unregisterMessageHandler( MSG_MODULEID_KIN );

    gServerGlobal->getDBEngineService()->unregisterHandler(this);


    delete this;
}

bool KinScenService::create()
{
    TraceLn("KinSceneService::create...");

    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__": failed! can not found the IMessageDispatch! ");
        return false;
    }
    // 自己需要处理消息
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_KIN, this);

    // 处理客户端发送过来的消息
    pMessageDispatch->registerMessageHandler(MSG_MODULEID_KIN, this);

    gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

    m_mutex = 0;

    return true;
}



void KinScenService::sendToSocial(BYTE nMsgID, void * data, size_t len)
{
	TraceLn( __FUNCTION__ ": msgid="<<nMsgID <<", len="<< len);
	ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
	if(pConnector == NULL)
	{
		ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
		return;
	}

	pConnector->sendToSocial(MSG_MODULEID_KIN,nMsgID, (const char *)data, len);
}


void KinScenService::getIdentityName(int nIdentity, char* szName, int nLen)
{
    if (szName == NULL || nLen == 0)
    {
        return;
    }

    if (nIdentity == emKinIdentity_Shaikh)
    {
        sstrcpyn(szName, "队长", nLen);
    }
    else if (nIdentity == emKinIdentity_Elder)
    {
        sstrcpyn(szName, "副队长", nLen);
    }
    else
    {
        sstrcpyn(szName, "成员", nLen);
    }
}

SSceneKinMember KinScenService::getKinMember(DWORD dwPDBID)
{
    SSceneKinMember kinMember;
    map<DWORD, CKinMember*>::iterator it = m_KinMemberList.find(dwPDBID);
    if (it == m_KinMemberList.end())
    {
        return kinMember;
    }

    CKinMember* pKinMember = it->second;
    if (pKinMember == NULL)
    {
        return kinMember;
    }
    memcpy(&kinMember, pKinMember->getContext(), sizeof(SSceneKinMember));
    
    return kinMember;
}

CKinMember* KinScenService::findKinMemberEx(DWORD dwPDBID)
{
    map<DWORD, CKinMember*>::iterator it = m_KinMemberList.find(dwPDBID);
    if (it == m_KinMemberList.end())
    {
        return NULL;
    }

    return (*it).second;
}

void KinScenService::removeKinMember(DWORD dwPDBID)
{
    map<DWORD, CKinMember*>::iterator it = m_KinMemberList.find(dwPDBID);
    if (it == m_KinMemberList.end())
    {
        return;
    }

    it->second->release();

    m_KinMemberList.erase(it);
}

void KinScenService::DBProcCreateKin(DWORD pdbid, const char* szKinName, const char* szKinLabel, bool bCrtAsFormal /*= false*/)
{
    IActorService* pPerson = g_EHelper.getActorByPDBID(pdbid);
    if (pPerson == NULL)
    {
        return;
    }

    if (szKinName == NULL || szKinLabel == NULL)
    {
        return;
    }

    IDBEngineService* pDBEngine	= gServerGlobal->getDBEngineService();
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pDBEngine == NULL || pSchemeCenter == NULL)
    {
        return;
    }

    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if (pConfig == NULL)
    {
        ErrorLn(__FUNCTION__": pConfig == NULL");
        return;
    }
    char szText[512] = {0};
    // 检查玩家战队有战队
    int nKinID = pPerson->getProperty_Integer(PROPERTY_KIN);
    if (nKinID > 0)
    {
        g_EHelper.showSystemMessage(pdbid, CHAT_TIP_KIN_HAD_JOINED_KIN, "");
        return;
    }

    // 检查玩家等级
    if (pPerson->getProperty_Integer(PROPERTY_LEVEL) < pConfig->nCreateLevel)
    {
        // 通知客户端
        char szText[26] = {0};
        sprintf_s(szText, "%d", pConfig->nCreateLevel);
        g_EHelper.showSystemMessage(pdbid,CHAT_TIP_KIN_CREATE_LEVEL_LIMIT,szText);

        return;
    }
    

    // 检查玩家金钱是否足够
    int nUseNoBindValue = 0; int nUseBindValue = 0;
    if ( !pPerson->canAddMoney(0, -(pConfig->nCreateMoney)) )
    {
        // 通知客户端
        char szText[26] = {0};
        sprintf_s(szText, "%d", pConfig->nCreateMoney);
        g_EHelper.showSystemMessage(pdbid,CHAT_TIP_KIN_CREATE_NEED_MONEY,szText);

        return;
    }

    DBREQ_PARAM_CENTERDBCREATEKIN referData;
    if (gServerGlobal->getManagerConnectorService() != NULL)
    {
        referData.dwWorldID = gServerGlobal->getManagerConnectorService()->GetWorldID();
    }

    referData.dwPDBID			= pdbid;
    referData.nEstablishTime	= (int)(time(NULL));
    sstrcpyn(referData.szName, szKinName, sizeof(referData.szName));
	sstrcpyn(referData.szLabel, szKinLabel, sizeof(referData.szLabel));
    // 往中心数据库创建战队操作
    if ( pDBEngine->executeSP(GAMEDB_REQUEST_CENTERDBCREATEKIN, pdbid,
        (LPCSTR)&referData, sizeof(referData), static_cast<IDBRetSink *>(this)) )
    {
        TraceLn(__FUNCTION__": GAMEDB_REQUEST_CENTERDBCREATEKIN pdbid = " << pdbid);
    }
    else
    {
        ErrorLn("GAMEDB_REQUEST_CENTERDBCREATEKIN 请求失败，pdbid = " << pdbid);
    }
}

void KinScenService::onTransmitCreateKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (pszMsg == NULL || nLen != sizeof(SMsgKin_Create_OS))
    {
        return;
    }

    SMsgKin_Create_OS* pRecvData = (SMsgKin_Create_OS*)pszMsg;
    // 根据玩家pdbid和战队名, 战队标签创建一个战队
    DBProcCreateKin(pRecvData->dwPDBID, pRecvData->szName, pRecvData->szLabel);
}

void KinScenService::onTransmitKinRename(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    //ErrorLn("onTransmitKinRename start");
    if(NULL == pszMsg)
    {
        return;
    }

    IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pDBEngine == NULL || pSchemeCenter == NULL )
    {
        return;
    }

    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if (pConfig == NULL)
    {
        ErrorLn(__FUNCTION__": pConfig == NULL");
        return;
    }

    if (nLen != sizeof(SMsKin_Rename_OS))
    {
        ErrorLn(__FUNCTION__": size error sizeof(SMsgKin_Rename_OS):" << sizeof(SMsKin_Rename_OS) << "  nLen:" << nLen);
        return;
    }

    // 如果角色下线，直接返回
    SMsKin_Rename_OS* pRecvData = (SMsKin_Rename_OS*)pszMsg;

    //ErrorLn("onTransmitKinRename check center db kin name");

    // 往中心数据库创建战队操作
    DBREQ_PARAM_CENTERKINNAMECHK referData;
    referData.dwPDBID = pRecvData->dwPDBID;
    referData.dwKinID = (DWORD)pRecvData->dwKinID;
    sstrcpyn(referData.szKinName, pRecvData->szNewName, sizeof(referData.szKinName));

    //ErrorLn("onTransmitKinRename exeute sp");
    if(!pDBEngine->executeSP(GAMEDB_REQUEST_CENTERKINNAMECHK, pRecvData->dwPDBID, (LPCSTR)&referData, 
        sizeof(referData), static_cast<IDBRetSink *>(this)))
    {
        ErrorLn(__FUNCTION__": GAMEDB_REQUEST_CENTERKINNAMECHK request failed nPDBID = " << pRecvData->dwPDBID);
    }
}

void KinScenService::onTransmitNameTitle(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (nLen != sizeof(SMsgKin_NameTitle_OS))
    {
        return;
    }

    SMsgKin_NameTitle_OS* pRecvData = (SMsgKin_NameTitle_OS*)pszMsg;
    // 设置战队ID属性
    IActorService* pPerson = g_EHelper.getActorByPDBID(pRecvData->dwPDBID);
    if (pPerson == NULL)
    {
        return;
    }

    //删除title
    if (pRecvData->bRemove == true)
    {
        pPerson->deleteTitle(TITLE_INDEX_KIN);
    }
}

void KinScenService::onTransmitUpdateKinID(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
    if (nLen != sizeof(SMsgKin_UdateID) || pEventEngine == NULL)
    {
        return;
    }

    SMsgKin_UdateID* pRecvData = (SMsgKin_UdateID*)pszMsg;
    updateKinIDInfo(pRecvData->dwPDBID, pRecvData->dwKinID);
    
}


void KinScenService::updateKinIDInfo(DWORD dwActorID, DWORD dwKinID)
{
    // 设置战队ID属性
    IActorService* pPerson = g_EHelper.getActorByPDBID(dwActorID);
    if (pPerson == NULL)
    {
        return;
    }
    pPerson->setProperty_Integer(PROPERTY_KIN, dwKinID);

    //添加title

    if (dwKinID != 0)
    {
        SKinInfo sKin = getKinInfo(dwKinID);
        if (sKin.nKinProp_ID != 0)
        {
            pPerson->AddTitle(TITLE_INDEX_KIN, sKin.szName, sizeof(sKin.szName));
        }
    }
    else
    {
        pPerson->deleteTitle(TITLE_INDEX_KIN);
    }
}



void KinScenService::onTransmitPostMsg(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (nLen < sizeof(SMsgKin_ScenePostMsg_OS))
    {
        return;
    }

    SMsgKin_ScenePostMsg_OS* pRecvData = (SMsgKin_ScenePostMsg_OS*)pszMsg;
    //IPerson* pPerson = g_EHelper.GetPerson(pRecvData->dwPDBID);
    //if (pPerson != NULL)
    //{
    //    obuf512 ob;
    //    fillModuleHead(ob, MSG_MODULEID_KIN, pRecvData->dwMsgID);
    //    ob.push_back((pszMsg + sizeof(SMsgKin_ScenePostMsg_OS)), nLen - sizeof(SMsgKin_ScenePostMsg_OS));
    //    g_EHelper.SendToPerson(pPerson, ob.data(), ob.size());
    //}
}

void KinScenService::onTransmitRemoveMember(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (nLen != sizeof(SMsgKin_RemoveMember_OS))
    {
        return;
    }

    SMsgKin_RemoveMember_OS* pRecvData = (SMsgKin_RemoveMember_OS*)pszMsg;
    m_KinMemberList.erase(pRecvData->dwPDBID);

    // 添加到战队成员列表
    CKin* pKin = (CKin*)getKin(pRecvData->dwKinID);
    if (pKin != NULL)
    {
        pKin->removeMember(pRecvData->dwPDBID);
    }
    else
    {
        WarningLn(_GT("[社会服删除战队成员]失败，战队对象不存在，战队ID" )<< pRecvData->dwKinID);
    }
}

IKin* KinScenService::getKin(DWORD dwKinID)
{
    map<DWORD, CKin*>::iterator it = m_KinList.find(dwKinID);
    if (it == m_KinList.end())
    {
        return NULL;
    }

    return (IKin*)it->second;
}


void KinScenService::onTransmitUpdateSceneMember(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (nLen != sizeof(SSceneKinMember))
    {
        return;
    }

    SSceneKinMember* pRecvData = (SSceneKinMember*)pszMsg;
    CKinMember *pKinMember = findKinMemberEx(pRecvData->dwPDBID);
    if (pKinMember == NULL)
    {
        pKinMember = new CKinMember();
        if (pKinMember == NULL)
        {
            WarningLn("new CKinMember() failed");
            return;
        }

        if (pKinMember->create(pRecvData) == false)
        {
            pKinMember->release();
            return;
        }

        m_KinMemberList[pRecvData->dwPDBID] = pKinMember;
    }
    else
    {

        pKinMember->updateData(pRecvData);
    }

    // 添加到战队成员列表
    CKin* pKin = (CKin*)getKin(pRecvData->dwKinID);
    if (pKin != NULL)
    {
        pKin->addMember(pRecvData->dwPDBID);
    }

    // 更新战队ID信息
    updateKinIDInfo(pRecvData->dwPDBID, pRecvData->dwKinID);
}

void KinScenService::onTransmitRemoveKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (nLen != sizeof(SMsgKin_RemoveKinScene_OS))
    {
        return;
    }

    SMsgKin_RemoveKinScene_OS* pRecvData = (SMsgKin_RemoveKinScene_OS*)pszMsg;
    IKin* pKin = getKin(pRecvData->dwKinID);
    if (pKin == NULL)
    {
        return;
    }

    // 释放战队对象
    pKin->release();

    // 移除记录
    m_KinList.erase(pRecvData->dwKinID);

    
}

void KinScenService::onTransmitUpdateKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (nLen < sizeof(SMsgKin_UpdateKinScene_OS))
    {
        return;
    }

    SMsgKin_UpdateKinScene_OS* pRecvData = (SMsgKin_UpdateKinScene_OS*)pszMsg;
    CKin* pKin = (CKin*)getKin(pRecvData->dwID);
    if (pKin == NULL)
    {
        pKin = new CKin();
        if (pKin == NULL)
        {
            WarningLn(__FUNCTION__": new CKin() faild KinID" << pRecvData->dwID);
            return;
        }

        if (pKin->create(this, pszMsg, nLen) == false)
        {
            WarningLn(__FUNCTION__": pKin->Create() faild KinID" << pRecvData->dwID);
            delete pKin;
        }

        m_KinList[pRecvData->dwID] = pKin;
    }
    else
    {

        pKin->updateData(pszMsg, nLen);
    }
}

void KinScenService::onTransmitUpdateKinProp(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (nLen != sizeof(SMsgKin_UpdatePropScene_OS))
    {
        return;
    }

    SMsgKin_UpdatePropScene_OS* pRecvData = (SMsgKin_UpdatePropScene_OS*)pszMsg;
    CKin* pKin = (CKin*)getKin(pRecvData->dwKinID);
    if (pKin == NULL)
    {
        return;
    }

    pKin->coverNumProp(pRecvData->nPropID, pRecvData->nValue);
}

void KinScenService::onTransmitClearKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    // 释放所有的战队对象
    map<DWORD, CKin*>::iterator it;
    for (it = m_KinList.begin(); it != m_KinList.end(); ++ it)
    {
        delete ((*it).second);
    }

    // 从列表清除所有已经释放的对象
    m_KinList.clear();

    // 清除成员列表
    map<DWORD, CKinMember*>::iterator itMember;
    for (itMember = m_KinMemberList.begin(); itMember != m_KinMemberList.end(); ++ itMember)
    {
        itMember->second->release();
    }
    m_KinMemberList.clear();
}

void KinScenService::onReturnCenterKinNameCheck(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    //ErrorLn("onReturnCenterKinNameCheck start");
    int nPDBID = nQueueIndex;
    
    IActorService* pPerson = g_EHelper.getActorByPDBID(nPDBID);
    if (pPerson == NULL)
    {
        ErrorLn(__FUNCTION__);
        return;
    }

    // 验证数据库返回结果
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        if (nDBRetCode == -1 && pszDBRetDesc != NULL && pszDBRetDesc[0] != '\0')
        {
            g_EHelper.showSystemMessage(nQueueIndex, CHAT_TIP_LOGIN_SERVER_MESSAGE, pszDBRetDesc);
            ErrorLn(pszDBRetDesc);
        }
        return;
    }

    // ErrorLn("onReturnCenterKinNameCheck check struct len");
    int nStructLen = sizeof(DBREQ_RESULT_CENTERKINNAMECHK);
    if (nOutLen != nStructLen)
    {
        ErrorLn("[onReturnCenterKinNameCheck] 长度不对,数据库返回长度：" << nOutLen << "结构体长度：" << nStructLen);
        return;
    }

    if(NULL == pOutData)
    {
        return;
    }

    DBREQ_RESULT_CENTERKINNAMECHK* pRetData = (DBREQ_RESULT_CENTERKINNAMECHK*)pOutData;
    // 判断返回值是否为1，1表示名字可用，0表示名字不可用或重复
    if( 1 != pRetData->nFlag )
    {
        ErrorLn("新战队名字已存在或非法，请更换其他战队名字！");
        return;
    }

    IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn("玩家：" << nPDBID << "修改战队名时，获取pDBEngine失败，战队ID：" << pRetData->dwKinID << "，新战队名：" << pRetData->szKinName );
        return;
    }

    bool bSuccess = false;

    // 获取旧的战队名
    char szText[512] = {0};
    char szKinOldName[KIN_NAME_SIZE] = {0};
    IKin * pKin = getKin(pRetData->dwKinID);
    if(NULL == pKin)
    {
        ErrorLn( "您的战队已不存在，无法修改！");
        return;
    }
    sstrcpyn(szKinOldName, pKin->getName(), sizeof(szKinOldName));

    // 检查玩家战队有战队
    int nKinID = pPerson->getProperty_Integer(PROPERTY_KIN);
    if (nKinID <= 0)
    {
        g_EHelper.showSystemMessage(nPDBID,CHAT_TIP_KIN_RENAME_KIN_HAVE_NOT_KIN,"");
        return;
    }

    if(nKinID != pRetData->dwKinID)
    {
        g_EHelper.showSystemMessage(nPDBID,CHAT_TIP_KIN_RENAME_KIN_IN_OTHER_KIN,"");
        return;
    }

    SSceneKinMember kinMember = getKinMember(nPDBID);
    if( emKinIdentity_Shaikh != kinMember.nIdentity)
    {
        g_EHelper.showSystemMessage(nPDBID,CHAT_TIP_KIN_SHAIKH_CAN_RENAME_KIN,"");
        return;
    }

        
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return;
    }

    SKinConfig*     pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if (pConfig == NULL)
    {
        ErrorLn(__FUNCTION__": pConfig == NULL");
        return;
    }

    if(pConfig->nRenameMoney <= 0)
    {
        ErrorLn("战队配置文件出错，改名消耗资金为小于等于0，pConfig->nRenameMoney：" << pConfig->nRenameMoney);
        return;
    }

    int nRenameMoney = -1 * (pConfig->nRenameMoney);
    if(!pPerson->canAddMoney(0, nRenameMoney))
    {
        sprintf_s(szText, sizeof(szText), "您的金币不足%d，无法修改战队名字！", pConfig->nRenameMoney);
        g_EHelper.showSystemMessage(nPDBID,CHAT_TIP_KIN_SHAIKH_CAN_RENAME_KIN,"");
        return;
    }

    pPerson->addMoney(0, -pConfig->nRenameMoney, OSSRES_DEL_KIN_RENAME, 0, 0, 0);

    //ErrorLn("onReturnCenterKinNameCheck get sys handle");

    // 通知社会服修改战队名称
    SMsKin_Rename_SO createData;
    createData.dwPDBID = nPDBID;
    createData.dwKinID = pRetData->dwKinID;
    createData.nRenameTime = (int)(time(NULL));
    sstrcpyn(createData.szName, pRetData->szKinName, sizeof(createData.szName));

    //ErrorLn("onReturnCenterKinNameCheck return to social");
    g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_RENAME, 0, &createData, sizeof(createData));

    //sprintf_s(szText, sizeof(szText), "玩家：%d  修改战队名称成功, 战队ID：%d  战队名：%s  新战队名：%s", 
    //    nPDBID, createData.dwKinID, szKinOldName, createData.szName);
    //pOSSLogServer->TrackGameNote(ENoteTrackType_Kin, createData.dwKinID, szText);

    ////ErrorLn("onReturnCenterKinNameCheck recorde sys info ");
    //sprintf_s(szText, sizeof(szText), "修改战队名字成功，扣除金币%d", pConfig->nRenameMoney);
    //gServerGlobal->GetChatServer()->sendSystemTips(TipType_Operate, nPDBID, InfoPos_ChatWindow|InfoPos_ActorUnder, szText);

    bSuccess = true;

    // 向数据库提交修改确认
    DBREQ_PARAM_CENTERKINNAMEFLAG referData;
    referData.dwKinID = pRetData->dwKinID;
    sstrcpyn(referData.szKinName, pRetData->szKinName, sizeof(referData.szKinName));
    if(bSuccess)
    {
        referData.nFlag = 1;
    }
    else
    {
        referData.nFlag = 0;
    }

    if(!pDBEngine->executeSP(GAMEDB_REQUEST_CENTERKINNAMEFLAG, nPDBID, (LPCSTR)&referData, 
        sizeof(referData), static_cast<IDBRetSink *>(this)))
    {
        ErrorLn("GAMEDB_REQUEST_CENTERKINNAMEFLAG failed，nPDBID = " << nPDBID);
    }

}

void KinScenService::onReturnCenterCreateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    bool	bSucc	= false;
    DWORD	pdbid	= (DWORD)nQueueIndex;

    IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return;
    }

    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        if (nDBRetCode == -1 && pszDBRetDesc != NULL && pszDBRetDesc[0] != '\0')
        {
            //gServerGlobal->getChatService()->sendSystemTips(TipType_Operate, pdbid, InfoPos_ActorUnder, pszDBRetDesc);
            g_EHelper.showSystemMessage(nQueueIndex, CHAT_TIP_LOGIN_SERVER_MESSAGE, pszDBRetDesc);
            ErrorLn(pszDBRetDesc);
        }
        return;
    }

    int nStructLen = sizeof(DBREQ_RESULT_CENTERDBCREATEKIN);
    if (nOutLen != nStructLen)
    {
        ErrorLn("[onReturnCenterCreateKin] error outlen：" << nOutLen << " length：" << nStructLen);
        return;
    }
    DBREQ_RESULT_CENTERDBCREATEKIN* pRetData = (DBREQ_RESULT_CENTERDBCREATEKIN*)pOutData;

    // 游戏服创建战队
    DBREQ_PARAM_CREATEKIN referData;
    referData.dwKinID = pRetData->dwKinID;
    referData.dwPDBID = pRetData->dwPDBID;
    sstrcpyn(referData.szName, pRetData->szName, sizeof(referData.szName));
    referData.nState = emKinState_Formal;
    referData.nLevel = 1;
    referData.nEstablishTime = (int)time(NULL);
	sstrcpyn(referData.szLabel, pRetData->szLabel, sizeof(referData.szLabel));
    // 往数据库创建战队操作
    if (!pDBEngine->executeSP(GAMEDB_REQUEST_CREATEKIN, pRetData->dwPDBID, (LPCSTR)&referData, 
        sizeof(referData), static_cast<IDBRetSink *>(this)))
    {
        ErrorLn("GAMEDB_REQUEST_CREATEKIN failed，nPDBID = " << pRetData->dwPDBID);
        return;
    }

    bSucc = true;
 

}

void KinScenService::onReturnCreateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    DWORD pdbid = (DWORD)nQueueIndex;

    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        if (nDBRetCode == -1 && pszDBRetDesc != NULL && pszDBRetDesc[0] != '\0')
        {
            g_EHelper.showSystemMessage(nQueueIndex, CHAT_TIP_LOGIN_SERVER_MESSAGE, pszDBRetDesc);
            ErrorLn(pszDBRetDesc);
        }
    }

    int nStuctLen = sizeof(DBREQ_RESULT_CREATEKIN);
    if (nOutLen != nStuctLen)
    {
        ErrorLn(__FUNCTION__":error nOutLen = " << nOutLen << " [DBREQ_RESULT_CREATEKIN] length= " << nStuctLen);
        return;
    }

    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return;
    }

    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if (pConfig == NULL )
    {
        ErrorLn(__FUNCTION__": pConfig == NULL ");
        return;
    }

    DBREQ_RESULT_CREATEKIN* pRetData = (DBREQ_RESULT_CREATEKIN*)pOutData;

    ActorHelper helper(pdbid, ACTOR_DBID);
    IActorService * pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__": pActorService == NULL dwActorID= "<<pdbid);
        return;
    }

    // 如果玩家不在线了
    SSharePersonProperty sSharePerson = g_EHelper.getSharePerson(pdbid);
    bool bCanCreate = true;
    LPCSTR strSzReason = "";
    if (sSharePerson.dwPDBID == INVALID_PDBID)
    {
        bCanCreate = false;
        strSzReason = _GT("person login out!");
        return;
    }

    // 如果玩家金钱突然不够了	
    int nUseNoBindValue = 0; int nUseBindValue = 0;

    if (pConfig->nCreateMoney < 0)
    {
        ErrorLn(__FUNCTION__": config check create Kin money is < 0!")
        return;
    }

    if (!pActorService->canAddMoney(0, -(pConfig->nCreateMoney)))	
    {
        bCanCreate = false;
        strSzReason =  "战队解散：创建战队数据库返回，玩家钱不够了";
    }

    //// 如果玩家加入其他战队了
    int nKinID = pActorService->getProperty_Integer(PROPERTY_KIN);
    if (nKinID > 0)
    {
        bCanCreate = false;
        strSzReason =  "战队解散：创建战队数据库返回，玩家已经加入其他战队了";
    }
        
    if (!bCanCreate)
    {
        backUpCreateKin(pRetData->dwKinID, strSzReason);
        return;
    }

	char szLogInfo[256] = { 0 };
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("创建战队:%s"), pRetData->szName);
    // 扣除玩家创建战队
    pActorService->addMoney(0, -pConfig->nCreateMoney, OSSRES_DEL_CREATE_FIGHT_TEAM, szLogInfo, 0, 0);// 创建战队

    //pPerson->UseMoney(pConfig->nCreateMoney, 3, nUseNoBindValue, nUseBindValue, OssResDel_KinCreate, nKinID, "创建战队");	
    //pOSSLogServer->MonitorNumDel(pPerson->GetFromGameWorldID(), OSS_NUMTYPE_MONEY, -abs(nUseNoBindValue), OSS_LAYER1_SYSTEM, OssResDel_KinCreate);
    //pOSSLogServer->MonitorNumDel(pPerson->GetFromGameWorldID(), OSS_NUMTYPE_BINDMONEY, -abs(nUseBindValue), OSS_LAYER1_SYSTEM, OssResDel_KinCreate);

    //char szText[512] = {0};

    //// 通知玩家创建战队成功
    //string strMoney;
    //strMoney.clear();
    //g_EHelper.FormatCurrency(nUseNoBindValue, nUseBindValue, true, strMoney);
    //ssprintf_s(szText, sizeof(szText), "创建战队成功，扣除%s", strMoney.c_str());
    //gServerGlobal->GetChatServer()->sendSystemTips(TipType_Operate, pdbid, InfoPos_ChatWindow, szText);

    //// 保存玩家数据到前端
    //g_EHelper.SaveToVanDataServer(pPerson);

    //// 通知社会服创建战队对象
    SMsgKin_Create_SO createData;
    sstrcpyn(createData.szShaikhName, sSharePerson.szName, sizeof(createData.szShaikhName));
    sstrcpyn(createData.szName, pRetData->szName, sizeof(createData.szName));
    createData.dwUDBID          = sSharePerson.dwUDBID;
    createData.dwPDBID			= pdbid;
    createData.dwKinID			= pRetData->dwKinID;
    createData.nEstablishTime	= (int)time(NULL);
    createData.bCrtAsFormal		= true;
	sstrcpyn(createData.szLabel, pRetData->szLabel, sizeof(createData.szLabel));

    g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_CREATE, 0, &createData, sizeof(createData));

    //// 设置玩家的战队ID（玩家上线时候，也需要把战队ID读取出来）
    //pPerson->SetNumProp(CREATURE_PROP_KINID, pRetData->dwKinID, true);

    //// 记录创建战队日志
    //char pcName[256] = {0};
    //DWORD dwSize = 256;
    //::GetComputerName(pcName, &dwSize);

    //ssprintf_s(szText, sizeof(szText), "创建战队成功, 战队ID：%d 战队名：%s 主机：%s", createData.dwKinID, createData.szName, pcName);
    //pOSSLogServer->TrackGameNote(ENoteTrackType_Kin, createData.dwKinID, szText);

    //ssprintf_s(szText, sizeof(szText), "战队需要%d个人响应，才可以变成正式战队，如果1个小时内凑不足%d个%d级以上的玩家来响应，战队将自动解散", pConfig->nRespondNeedCount, pConfig->nRespondNeedCount, pConfig->nRespondLevel);
    //gServerGlobal->GetChatServer()->sendSystemTips(TipType_Operate, pdbid, InfoPos_ChatWindow, szText);
}


void KinScenService::backUpCreateKin(DWORD dwKinID, LPCSTR szTrackNote)
{
    IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return;
    }

    // 往中心库数据库解散战队操作
    DBREQ_PARAM_CENTERDBDISMISSKIN centerDismiss;
    centerDismiss.dwKinID = dwKinID;
    if(!pDBEngine->executeSP(GAMEDB_REQUEST_CENTERDBDISMISSKIN, dwKinID, (LPCSTR)&centerDismiss,
        sizeof(centerDismiss), static_cast<IDBRetSink *>(this)))
    {
        ErrorLn("GAMEDB_REQUEST_CENTERDBDISMISSKIN faild, dwKinID = " << dwKinID);
        return;
    }

    // 往游戏库数据库解散战队操作
    DBREQ_PARAM_DISMISSKIN referData;
    referData.dwKinID = dwKinID;
    if(!pDBEngine->executeSP(GAMEDB_REQUEST_DISMISSKIN, dwKinID, (LPCSTR)&referData,
        sizeof(referData), static_cast<IDBRetSink *>(this)))
    {
        ErrorLn("GAMEDB_REQUEST_DISMISSKIN faild, dwKinID =" << dwKinID);
        return;
    }

    // 记录解散战队日志
    //pOSSLogServer->TrackGameNote(ENoteTrackType_Kin, dwKinID, szTrackNote);
}

void KinScenService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    switch (nCmdID)
    {
        // 往中心服提交创建战队操作	-- 数据库返回处理
    case GAMEDB_REQUEST_CENTERDBCREATEKIN:
        {
            onReturnCenterCreateKin(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
            break;
        }

        // 创建战队					-- 数据库返回处理
    case GAMEDB_REQUEST_CREATEKIN:							// 创建战队
        {
            onReturnCreateKin(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
            break;
        }

        // 战队改名
    case GAMEDB_REQUEST_CENTERKINNAMECHK:					// 校验和修改中心数据库战队名
        {
            onReturnCenterKinNameCheck(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
            break;
        }

  //  case GAMEDB_REQUEST_ADDSERVICES_USER_UPDATE:				// 修改玩家信息 游戏库
  //      {
  //          // todo 修改玩家姓名 性别
  //          //AddService_ResponseUpdateUserInfoFromGameDB(nCmdID, nDBRetCode,pszDBRetDesc,nQueueIndex,pOutData,nOutLen);
  //      }
  //      break;
  //  case GAMEDB_REQUEST_ADDSERVICES_CHK:
		//{
		//	if(nOutLen < sizeof(DBREQ_RESULT_ADDSERVICES_CHK))
		//	{
		//		ErrorLn("数据库返回长度不对，nCmdID = " << nCmdID << " 需要长度：" << sizeof(DBREQ_RESULT_ADDSERVICES_CHK) << "返回长度" << nOutLen);
		//		return;
		//	}

		//	DBREQ_RESULT_ADDSERVICES_CHK * pResut = (DBREQ_RESULT_ADDSERVICES_CHK *)pOutData;
		//	//AddService_ResponseCheckRule(*pResut, pszDBRetDesc);
		//}
		//break;

    default:
        break;
    }
}

//// 游戏修改玩家信息返回
//bool KinScenService::AddService_ResponseUpdateUserInfoFromGameDB(const DBREQ_RESULT_ADDSERVICES_USER_UPDATE & param)
//{
//    char szBuf[512] = {0};
//
//    // 取得人物对象
//    IActorService * pPerson = g_EHelper.getActorByPDBID(param.dwActorID);
//    // "改名"DB返回时人物可以不在线
//    if (NULL == pPerson && param.nType != ActorServiceType_Rename)
//    {
//        sprintf_s(szBuf, sizeof(szBuf), "增值服务游戏库返回时查找玩家对象失败：玩家名:[%s] 增值服务类型:[%d] 执行结果[%d]", param.szCurValues, param.nType, param.nFlags);
//        ErrorLn(szBuf);
//        AlertBad(MG_AlertSrcType_DataBase, szBuf);
//
//        return false;
//    }
//
//    // 增值服务类型
//    switch(param.nType)
//    {
//    case ActorServiceType_ChangeSex:			// 改变性别
//        {
//            // 通知玩家角色性别修改结果
//            //// 通知社会服创建战队对象
//            SMsgKinActorChangeSex_SO data;
//            data.dwPDBID  = param.dwActorID;
//            data.nSex     = atoi(param.szNewValues);
//
//            g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_ACTOR_CHANGESEX, 0, &data, sizeof(data));
//            
//        }
//        break;
//    case ActorServiceType_Rename:				// 改角色名称
//        {
//            // 通知玩家角色名修改结果
//            // 通知玩家角色性别修改结果
//            //// 通知社会服创建战队对象
//            SMsgKinActorRename_SO data;
//            data.dwPDBID    = param.dwActorID;
//            memcpy(data.szName, param.szNewValues, sizeof(data.szName));
//
//            g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_ACTOR_RENAME, 0, &data, sizeof(data));
//        }
//        break;
//   
//    default:
//        {
//        }
//        break;
//    }
//
//    return true;
//}



void KinScenService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch (wEventID)
    {
        
    default:
        break;
    }
}

void KinScenService::OnTransmitClearKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    // 释放所有的战队对象
    map<DWORD, CKin*>::iterator it;
    for (it = m_KinList.begin(); it != m_KinList.end(); ++ it)
    {
        SAFE_RELEASE((*it).second);
    }

    // 从列表清除所有已经释放的对象
    m_KinList.clear();

    // 清除成员列表
    map<DWORD, CKinMember*>::iterator itMember;
    for (itMember = m_KinMemberList.begin(); itMember != m_KinMemberList.end(); ++ itMember)
    {
        SAFE_RELEASE((*itMember).second);
    }
    m_KinMemberList.clear();
}

void KinScenService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    KinHelper kinSceneHelper;
    IKinSceneService *pKinSceneService = kinSceneHelper.m_ptr;
    if (pKinSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pKinSceneService->handleServerMsg( server, *head, pkg );
    
}

void KinScenService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{
    KinHelper kinSceneHelper;
    IKinSceneService *pKinSceneService = kinSceneHelper.m_ptr;
    if (pKinSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pKinSceneService->handleClientMsg( clientID, *head, pkg );
}

void KinScenService::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    size_t len = msg->size();
    void *data = (void *)msg->c_str();
    switch( head.byKeyAction )
    {
        // 创建战队
    case MSG_KIN_CREATE:
        {
            onTransmitCreateKin(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // 战队改名
    case MSG_KIN_RENAME:
        {
            onTransmitKinRename(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // 添加/删除 战队名称号
    case MSG_KIN_NAMETITLE:
        {
            onTransmitNameTitle(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // 更新玩家ID
    case MSG_KIN_UPDATEID:
        {
            onTransmitUpdateKinID(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // 删除战队
    case MSG_KIN_REMOVEKIN_SCENE:
        {
            onTransmitRemoveKin(head.byKeyAction, (LPCSTR)data, len); 
        }
        break;
        // 更新战队（没有则添加）
    case MSG_KIN_UPDATEKIN_SCENE:
        {
            onTransmitUpdateKin(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // 转发消息
    case MSG_KIN_SCENEPOSTMSG:
        {
            onTransmitPostMsg(head.byKeyAction, (LPCSTR)data, len);

        }
        break;
        // 社会服向场景服更新成员数据
    case MSG_KIN_UPDATEMEMBER_SCENE:
        {
            onTransmitUpdateSceneMember(head.byKeyAction, (LPCSTR)data, len); 
        }
        break;
        // 社会服向场景服删除成员
    case MSG_KIN_REMOVEMEMBER_SCENE:
        {
            onTransmitRemoveMember(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // 社会服向场景服清除战队数据
    case MSG_KIN_CLEAR_SCENE:
        {
            OnTransmitClearKin(head.byKeyAction, (LPCSTR)data, len);
            
        }
        break;

    case MSG_KIN_UPDATEKINPROP_SCENE:
        {
            onTransmitUpdateKinProp(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        
    default:
        {
            ErrorLn("KinSceneService::onKinServerMsg unknown KeyAction=" << head.byKeyAction);
        }
    }
}

void KinScenService::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    //TraceLn("KinSceneService::onClientMsg clientID=" << clientID);

    //TraceLn(__FUNCTION__": clientID=" << clientID << " byKeyAction=" << pHead->byKeyAction << " stLen=" << stLen);
    if (isPublicGameWorld() == true)
    {
        return;
    }
    size_t len = msg->size();
    void *data = (void *)msg->c_str();
    switch( head.byKeyAction )
    {
    default:
        {
            ErrorLn(__FUNCTION__": unknown pHead->byKeyAction=" << head.byKeyAction);
        }
    }
}


void KinScenService::onServerListUpdated()
{
    return;
}

void KinScenService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{
    return;
}

SKinInfo KinScenService::getKinInfo(DWORD dwKinID)
{
    SKinInfo sKinInfo;
    IKin *pKin = (IKin *)findKin(dwKinID);
    if (pKin != NULL)
    {
        memcpy(sKinInfo.szName,pKin->getName(),sizeof(sKinInfo.szName));                            // 名称
        sKinInfo.nKinProp_ID = pKin->getNumProp(emKinProp_ID);							            // 战队ID
        sKinInfo.nKinProp_State =pKin->getNumProp(emKinProp_State);						            // 状态
        sKinInfo.nKinProp_Level =pKin->getNumProp(emKinProp_Level);                                 // 战队等级
        sKinInfo.nKinProp_EstablishTime =pKin->getNumProp(emKinProp_EstablishTime);                 // 战队创立时间
        sKinInfo.nKinProp_DismissApplyTime =pKin->getNumProp(emKinProp_DismissApplyTime);           // 发起解散的时间
        sKinInfo.nKinProp_ShaikhID =pKin->getNumProp(emKinProp_ShaikhID);                           // 战队队长PDBID
        sKinInfo.nKinProp_MemberLastLogoutTime =pKin->getNumProp(emKinProp_MemberLastLogoutTime);   // 成员的最后下线时间
        sKinInfo.nKinProp_ReadFlag =pKin->getNumProp(emKinProp_ReadFlag);                           // 已经从数据库读出数据的标志
        sKinInfo.nKinProp_Money =pKin->getNumProp(emKinProp_Money);                                 // 资金
        sKinInfo.nKinProp_Activity = pKin->getNumProp(emKinProp_Activity);                          // 活跃度
        sKinInfo.nKinProp_Place =pKin->getNumProp(emKinProp_Place);                                 // 排名
        sKinInfo.nKinProp_WorldID =pKin->getNumProp(emKinProp_WorldID);                             // 游戏世界ID
    }

    return sKinInfo;
}

DWORD KinScenService::getKinMemberList(DWORD dwKinID,PDBID* pReturnArray, DWORD dwArrayMaxSize)
{
    CKin* pKin = findKin(dwKinID);
    if (pKin == NULL)
    {
        return 0;
    }
    else
    {
        return pKin->getMemberList(pReturnArray, dwArrayMaxSize);
    }
}

DWORD KinScenService::getKinMemberCount(DWORD dwKinID)
{
    CKin* pKin = findKin(dwKinID);
    if (pKin == NULL)
    {
        return 0;
    }
    else
    {
        return pKin->getMemberCount();
    }
}

bool KinScenService::setWeekActivity(DWORD dwKinID, int nValue)
{
	CKin * pKin = findKin(dwKinID);
	if (pKin == NULL)
	{
		return false;
	}

	nValue = nValue < 0 ? 0 : nValue;

	if (!pKin->coverNumProp(emKinProp_WeekActivity, nValue + pKin->getNumProp(emKinProp_WeekActivity)))
	{
		return false;
	}

	SMsgKin_UpdatePropScene_OS sendData;
	sendData.dwKinID = dwKinID;
	sendData.nPropID = (BYTE)emKinProp_WeekActivity;
	sendData.nValue = nValue;

	g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_SETPROP_SO, dwKinID, &sendData, sizeof(sendData));
	TraceLn(__FUNCTION__" set kin weekActivity success=" << nValue);
	return true;
}

int KinScenService::getWeekActivity(DWORD dwKinID)
{
	CKin * pKin = findKin(dwKinID);
	if (pKin == NULL)
	{
		return 0;
	}

	return pKin->getNumProp(emKinProp_WeekActivity);
}

CKin* KinScenService::findKin(DWORD dwKinID)
{
    Simple_Atom_Lock loc(&m_mutex);
    map<DWORD, CKin*>::iterator it = m_KinList.find(dwKinID);
    if (it == m_KinList.end())
    {
        return NULL;
    }

    return (*it).second;
}

