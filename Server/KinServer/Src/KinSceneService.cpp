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

    // �Լ���Ҫ������Ϣ
    pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_KIN);

    // ȡ��ע����Ӧ�ͻ�����Ϣ
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
    // �Լ���Ҫ������Ϣ
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_KIN, this);

    // ����ͻ��˷��͹�������Ϣ
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
        sstrcpyn(szName, "�ӳ�", nLen);
    }
    else if (nIdentity == emKinIdentity_Elder)
    {
        sstrcpyn(szName, "���ӳ�", nLen);
    }
    else
    {
        sstrcpyn(szName, "��Ա", nLen);
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
    // ������ս����ս��
    int nKinID = pPerson->getProperty_Integer(PROPERTY_KIN);
    if (nKinID > 0)
    {
        g_EHelper.showSystemMessage(pdbid, CHAT_TIP_KIN_HAD_JOINED_KIN, "");
        return;
    }

    // �����ҵȼ�
    if (pPerson->getProperty_Integer(PROPERTY_LEVEL) < pConfig->nCreateLevel)
    {
        // ֪ͨ�ͻ���
        char szText[26] = {0};
        sprintf_s(szText, "%d", pConfig->nCreateLevel);
        g_EHelper.showSystemMessage(pdbid,CHAT_TIP_KIN_CREATE_LEVEL_LIMIT,szText);

        return;
    }
    

    // �����ҽ�Ǯ�Ƿ��㹻
    int nUseNoBindValue = 0; int nUseBindValue = 0;
    if ( !pPerson->canAddMoney(0, -(pConfig->nCreateMoney)) )
    {
        // ֪ͨ�ͻ���
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
    // ���������ݿⴴ��ս�Ӳ���
    if ( pDBEngine->executeSP(GAMEDB_REQUEST_CENTERDBCREATEKIN, pdbid,
        (LPCSTR)&referData, sizeof(referData), static_cast<IDBRetSink *>(this)) )
    {
        TraceLn(__FUNCTION__": GAMEDB_REQUEST_CENTERDBCREATEKIN pdbid = " << pdbid);
    }
    else
    {
        ErrorLn("GAMEDB_REQUEST_CENTERDBCREATEKIN ����ʧ�ܣ�pdbid = " << pdbid);
    }
}

void KinScenService::onTransmitCreateKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen)
{
    if (pszMsg == NULL || nLen != sizeof(SMsgKin_Create_OS))
    {
        return;
    }

    SMsgKin_Create_OS* pRecvData = (SMsgKin_Create_OS*)pszMsg;
    // �������pdbid��ս����, ս�ӱ�ǩ����һ��ս��
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

    // �����ɫ���ߣ�ֱ�ӷ���
    SMsKin_Rename_OS* pRecvData = (SMsKin_Rename_OS*)pszMsg;

    //ErrorLn("onTransmitKinRename check center db kin name");

    // ���������ݿⴴ��ս�Ӳ���
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
    // ����ս��ID����
    IActorService* pPerson = g_EHelper.getActorByPDBID(pRecvData->dwPDBID);
    if (pPerson == NULL)
    {
        return;
    }

    //ɾ��title
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
    // ����ս��ID����
    IActorService* pPerson = g_EHelper.getActorByPDBID(dwActorID);
    if (pPerson == NULL)
    {
        return;
    }
    pPerson->setProperty_Integer(PROPERTY_KIN, dwKinID);

    //���title

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

    // ��ӵ�ս�ӳ�Ա�б�
    CKin* pKin = (CKin*)getKin(pRecvData->dwKinID);
    if (pKin != NULL)
    {
        pKin->removeMember(pRecvData->dwPDBID);
    }
    else
    {
        WarningLn(_GT("[����ɾ��ս�ӳ�Ա]ʧ�ܣ�ս�Ӷ��󲻴��ڣ�ս��ID" )<< pRecvData->dwKinID);
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

    // ��ӵ�ս�ӳ�Ա�б�
    CKin* pKin = (CKin*)getKin(pRecvData->dwKinID);
    if (pKin != NULL)
    {
        pKin->addMember(pRecvData->dwPDBID);
    }

    // ����ս��ID��Ϣ
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

    // �ͷ�ս�Ӷ���
    pKin->release();

    // �Ƴ���¼
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
    // �ͷ����е�ս�Ӷ���
    map<DWORD, CKin*>::iterator it;
    for (it = m_KinList.begin(); it != m_KinList.end(); ++ it)
    {
        delete ((*it).second);
    }

    // ���б���������Ѿ��ͷŵĶ���
    m_KinList.clear();

    // �����Ա�б�
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

    // ��֤���ݿⷵ�ؽ��
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
        ErrorLn("[onReturnCenterKinNameCheck] ���Ȳ���,���ݿⷵ�س��ȣ�" << nOutLen << "�ṹ�峤�ȣ�" << nStructLen);
        return;
    }

    if(NULL == pOutData)
    {
        return;
    }

    DBREQ_RESULT_CENTERKINNAMECHK* pRetData = (DBREQ_RESULT_CENTERKINNAMECHK*)pOutData;
    // �жϷ���ֵ�Ƿ�Ϊ1��1��ʾ���ֿ��ã�0��ʾ���ֲ����û��ظ�
    if( 1 != pRetData->nFlag )
    {
        ErrorLn("��ս�������Ѵ��ڻ�Ƿ������������ս�����֣�");
        return;
    }

    IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn("��ң�" << nPDBID << "�޸�ս����ʱ����ȡpDBEngineʧ�ܣ�ս��ID��" << pRetData->dwKinID << "����ս������" << pRetData->szKinName );
        return;
    }

    bool bSuccess = false;

    // ��ȡ�ɵ�ս����
    char szText[512] = {0};
    char szKinOldName[KIN_NAME_SIZE] = {0};
    IKin * pKin = getKin(pRetData->dwKinID);
    if(NULL == pKin)
    {
        ErrorLn( "����ս���Ѳ����ڣ��޷��޸ģ�");
        return;
    }
    sstrcpyn(szKinOldName, pKin->getName(), sizeof(szKinOldName));

    // ������ս����ս��
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
        ErrorLn("ս�������ļ��������������ʽ�ΪС�ڵ���0��pConfig->nRenameMoney��" << pConfig->nRenameMoney);
        return;
    }

    int nRenameMoney = -1 * (pConfig->nRenameMoney);
    if(!pPerson->canAddMoney(0, nRenameMoney))
    {
        sprintf_s(szText, sizeof(szText), "���Ľ�Ҳ���%d���޷��޸�ս�����֣�", pConfig->nRenameMoney);
        g_EHelper.showSystemMessage(nPDBID,CHAT_TIP_KIN_SHAIKH_CAN_RENAME_KIN,"");
        return;
    }

    pPerson->addMoney(0, -pConfig->nRenameMoney, OSSRES_DEL_KIN_RENAME, 0, 0, 0);

    //ErrorLn("onReturnCenterKinNameCheck get sys handle");

    // ֪ͨ�����޸�ս������
    SMsKin_Rename_SO createData;
    createData.dwPDBID = nPDBID;
    createData.dwKinID = pRetData->dwKinID;
    createData.nRenameTime = (int)(time(NULL));
    sstrcpyn(createData.szName, pRetData->szKinName, sizeof(createData.szName));

    //ErrorLn("onReturnCenterKinNameCheck return to social");
    g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_RENAME, 0, &createData, sizeof(createData));

    //sprintf_s(szText, sizeof(szText), "��ң�%d  �޸�ս�����Ƴɹ�, ս��ID��%d  ս������%s  ��ս������%s", 
    //    nPDBID, createData.dwKinID, szKinOldName, createData.szName);
    //pOSSLogServer->TrackGameNote(ENoteTrackType_Kin, createData.dwKinID, szText);

    ////ErrorLn("onReturnCenterKinNameCheck recorde sys info ");
    //sprintf_s(szText, sizeof(szText), "�޸�ս�����ֳɹ����۳����%d", pConfig->nRenameMoney);
    //gServerGlobal->GetChatServer()->sendSystemTips(TipType_Operate, nPDBID, InfoPos_ChatWindow|InfoPos_ActorUnder, szText);

    bSuccess = true;

    // �����ݿ��ύ�޸�ȷ��
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
        ErrorLn("GAMEDB_REQUEST_CENTERKINNAMEFLAG failed��nPDBID = " << nPDBID);
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

    // ��֤���ݿⷵ�ؽ��
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
        ErrorLn("[onReturnCenterCreateKin] error outlen��" << nOutLen << " length��" << nStructLen);
        return;
    }
    DBREQ_RESULT_CENTERDBCREATEKIN* pRetData = (DBREQ_RESULT_CENTERDBCREATEKIN*)pOutData;

    // ��Ϸ������ս��
    DBREQ_PARAM_CREATEKIN referData;
    referData.dwKinID = pRetData->dwKinID;
    referData.dwPDBID = pRetData->dwPDBID;
    sstrcpyn(referData.szName, pRetData->szName, sizeof(referData.szName));
    referData.nState = emKinState_Formal;
    referData.nLevel = 1;
    referData.nEstablishTime = (int)time(NULL);
	sstrcpyn(referData.szLabel, pRetData->szLabel, sizeof(referData.szLabel));
    // �����ݿⴴ��ս�Ӳ���
    if (!pDBEngine->executeSP(GAMEDB_REQUEST_CREATEKIN, pRetData->dwPDBID, (LPCSTR)&referData, 
        sizeof(referData), static_cast<IDBRetSink *>(this)))
    {
        ErrorLn("GAMEDB_REQUEST_CREATEKIN failed��nPDBID = " << pRetData->dwPDBID);
        return;
    }

    bSucc = true;
 

}

void KinScenService::onReturnCreateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    DWORD pdbid = (DWORD)nQueueIndex;

    // ��֤���ݿⷵ�ؽ��
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

    // �����Ҳ�������
    SSharePersonProperty sSharePerson = g_EHelper.getSharePerson(pdbid);
    bool bCanCreate = true;
    LPCSTR strSzReason = "";
    if (sSharePerson.dwPDBID == INVALID_PDBID)
    {
        bCanCreate = false;
        strSzReason = _GT("person login out!");
        return;
    }

    // �����ҽ�ǮͻȻ������	
    int nUseNoBindValue = 0; int nUseBindValue = 0;

    if (pConfig->nCreateMoney < 0)
    {
        ErrorLn(__FUNCTION__": config check create Kin money is < 0!")
        return;
    }

    if (!pActorService->canAddMoney(0, -(pConfig->nCreateMoney)))	
    {
        bCanCreate = false;
        strSzReason =  "ս�ӽ�ɢ������ս�����ݿⷵ�أ����Ǯ������";
    }

    //// �����Ҽ�������ս����
    int nKinID = pActorService->getProperty_Integer(PROPERTY_KIN);
    if (nKinID > 0)
    {
        bCanCreate = false;
        strSzReason =  "ս�ӽ�ɢ������ս�����ݿⷵ�أ�����Ѿ���������ս����";
    }
        
    if (!bCanCreate)
    {
        backUpCreateKin(pRetData->dwKinID, strSzReason);
        return;
    }

	char szLogInfo[256] = { 0 };
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("����ս��:%s"), pRetData->szName);
    // �۳���Ҵ���ս��
    pActorService->addMoney(0, -pConfig->nCreateMoney, OSSRES_DEL_CREATE_FIGHT_TEAM, szLogInfo, 0, 0);// ����ս��

    //pPerson->UseMoney(pConfig->nCreateMoney, 3, nUseNoBindValue, nUseBindValue, OssResDel_KinCreate, nKinID, "����ս��");	
    //pOSSLogServer->MonitorNumDel(pPerson->GetFromGameWorldID(), OSS_NUMTYPE_MONEY, -abs(nUseNoBindValue), OSS_LAYER1_SYSTEM, OssResDel_KinCreate);
    //pOSSLogServer->MonitorNumDel(pPerson->GetFromGameWorldID(), OSS_NUMTYPE_BINDMONEY, -abs(nUseBindValue), OSS_LAYER1_SYSTEM, OssResDel_KinCreate);

    //char szText[512] = {0};

    //// ֪ͨ��Ҵ���ս�ӳɹ�
    //string strMoney;
    //strMoney.clear();
    //g_EHelper.FormatCurrency(nUseNoBindValue, nUseBindValue, true, strMoney);
    //ssprintf_s(szText, sizeof(szText), "����ս�ӳɹ����۳�%s", strMoney.c_str());
    //gServerGlobal->GetChatServer()->sendSystemTips(TipType_Operate, pdbid, InfoPos_ChatWindow, szText);

    //// ����������ݵ�ǰ��
    //g_EHelper.SaveToVanDataServer(pPerson);

    //// ֪ͨ��������ս�Ӷ���
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

    //// ������ҵ�ս��ID���������ʱ��Ҳ��Ҫ��ս��ID��ȡ������
    //pPerson->SetNumProp(CREATURE_PROP_KINID, pRetData->dwKinID, true);

    //// ��¼����ս����־
    //char pcName[256] = {0};
    //DWORD dwSize = 256;
    //::GetComputerName(pcName, &dwSize);

    //ssprintf_s(szText, sizeof(szText), "����ս�ӳɹ�, ս��ID��%d ս������%s ������%s", createData.dwKinID, createData.szName, pcName);
    //pOSSLogServer->TrackGameNote(ENoteTrackType_Kin, createData.dwKinID, szText);

    //ssprintf_s(szText, sizeof(szText), "ս����Ҫ%d������Ӧ���ſ��Ա����ʽս�ӣ����1��Сʱ�ڴղ���%d��%d�����ϵ��������Ӧ��ս�ӽ��Զ���ɢ", pConfig->nRespondNeedCount, pConfig->nRespondNeedCount, pConfig->nRespondLevel);
    //gServerGlobal->GetChatServer()->sendSystemTips(TipType_Operate, pdbid, InfoPos_ChatWindow, szText);
}


void KinScenService::backUpCreateKin(DWORD dwKinID, LPCSTR szTrackNote)
{
    IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return;
    }

    // �����Ŀ����ݿ��ɢս�Ӳ���
    DBREQ_PARAM_CENTERDBDISMISSKIN centerDismiss;
    centerDismiss.dwKinID = dwKinID;
    if(!pDBEngine->executeSP(GAMEDB_REQUEST_CENTERDBDISMISSKIN, dwKinID, (LPCSTR)&centerDismiss,
        sizeof(centerDismiss), static_cast<IDBRetSink *>(this)))
    {
        ErrorLn("GAMEDB_REQUEST_CENTERDBDISMISSKIN faild, dwKinID = " << dwKinID);
        return;
    }

    // ����Ϸ�����ݿ��ɢս�Ӳ���
    DBREQ_PARAM_DISMISSKIN referData;
    referData.dwKinID = dwKinID;
    if(!pDBEngine->executeSP(GAMEDB_REQUEST_DISMISSKIN, dwKinID, (LPCSTR)&referData,
        sizeof(referData), static_cast<IDBRetSink *>(this)))
    {
        ErrorLn("GAMEDB_REQUEST_DISMISSKIN faild, dwKinID =" << dwKinID);
        return;
    }

    // ��¼��ɢս����־
    //pOSSLogServer->TrackGameNote(ENoteTrackType_Kin, dwKinID, szTrackNote);
}

void KinScenService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    switch (nCmdID)
    {
        // �����ķ��ύ����ս�Ӳ���	-- ���ݿⷵ�ش���
    case GAMEDB_REQUEST_CENTERDBCREATEKIN:
        {
            onReturnCenterCreateKin(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
            break;
        }

        // ����ս��					-- ���ݿⷵ�ش���
    case GAMEDB_REQUEST_CREATEKIN:							// ����ս��
        {
            onReturnCreateKin(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
            break;
        }

        // ս�Ӹ���
    case GAMEDB_REQUEST_CENTERKINNAMECHK:					// У����޸��������ݿ�ս����
        {
            onReturnCenterKinNameCheck(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
            break;
        }

  //  case GAMEDB_REQUEST_ADDSERVICES_USER_UPDATE:				// �޸������Ϣ ��Ϸ��
  //      {
  //          // todo �޸�������� �Ա�
  //          //AddService_ResponseUpdateUserInfoFromGameDB(nCmdID, nDBRetCode,pszDBRetDesc,nQueueIndex,pOutData,nOutLen);
  //      }
  //      break;
  //  case GAMEDB_REQUEST_ADDSERVICES_CHK:
		//{
		//	if(nOutLen < sizeof(DBREQ_RESULT_ADDSERVICES_CHK))
		//	{
		//		ErrorLn("���ݿⷵ�س��Ȳ��ԣ�nCmdID = " << nCmdID << " ��Ҫ���ȣ�" << sizeof(DBREQ_RESULT_ADDSERVICES_CHK) << "���س���" << nOutLen);
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

//// ��Ϸ�޸������Ϣ����
//bool KinScenService::AddService_ResponseUpdateUserInfoFromGameDB(const DBREQ_RESULT_ADDSERVICES_USER_UPDATE & param)
//{
//    char szBuf[512] = {0};
//
//    // ȡ���������
//    IActorService * pPerson = g_EHelper.getActorByPDBID(param.dwActorID);
//    // "����"DB����ʱ������Բ�����
//    if (NULL == pPerson && param.nType != ActorServiceType_Rename)
//    {
//        sprintf_s(szBuf, sizeof(szBuf), "��ֵ������Ϸ�ⷵ��ʱ������Ҷ���ʧ�ܣ������:[%s] ��ֵ��������:[%d] ִ�н��[%d]", param.szCurValues, param.nType, param.nFlags);
//        ErrorLn(szBuf);
//        AlertBad(MG_AlertSrcType_DataBase, szBuf);
//
//        return false;
//    }
//
//    // ��ֵ��������
//    switch(param.nType)
//    {
//    case ActorServiceType_ChangeSex:			// �ı��Ա�
//        {
//            // ֪ͨ��ҽ�ɫ�Ա��޸Ľ��
//            //// ֪ͨ��������ս�Ӷ���
//            SMsgKinActorChangeSex_SO data;
//            data.dwPDBID  = param.dwActorID;
//            data.nSex     = atoi(param.szNewValues);
//
//            g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_ACTOR_CHANGESEX, 0, &data, sizeof(data));
//            
//        }
//        break;
//    case ActorServiceType_Rename:				// �Ľ�ɫ����
//        {
//            // ֪ͨ��ҽ�ɫ���޸Ľ��
//            // ֪ͨ��ҽ�ɫ�Ա��޸Ľ��
//            //// ֪ͨ��������ս�Ӷ���
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
    // �ͷ����е�ս�Ӷ���
    map<DWORD, CKin*>::iterator it;
    for (it = m_KinList.begin(); it != m_KinList.end(); ++ it)
    {
        SAFE_RELEASE((*it).second);
    }

    // ���б���������Ѿ��ͷŵĶ���
    m_KinList.clear();

    // �����Ա�б�
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
        // ����ս��
    case MSG_KIN_CREATE:
        {
            onTransmitCreateKin(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // ս�Ӹ���
    case MSG_KIN_RENAME:
        {
            onTransmitKinRename(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // ���/ɾ�� ս�����ƺ�
    case MSG_KIN_NAMETITLE:
        {
            onTransmitNameTitle(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // �������ID
    case MSG_KIN_UPDATEID:
        {
            onTransmitUpdateKinID(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // ɾ��ս��
    case MSG_KIN_REMOVEKIN_SCENE:
        {
            onTransmitRemoveKin(head.byKeyAction, (LPCSTR)data, len); 
        }
        break;
        // ����ս�ӣ�û������ӣ�
    case MSG_KIN_UPDATEKIN_SCENE:
        {
            onTransmitUpdateKin(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // ת����Ϣ
    case MSG_KIN_SCENEPOSTMSG:
        {
            onTransmitPostMsg(head.byKeyAction, (LPCSTR)data, len);

        }
        break;
        // �����򳡾������³�Ա����
    case MSG_KIN_UPDATEMEMBER_SCENE:
        {
            onTransmitUpdateSceneMember(head.byKeyAction, (LPCSTR)data, len); 
        }
        break;
        // �����򳡾���ɾ����Ա
    case MSG_KIN_REMOVEMEMBER_SCENE:
        {
            onTransmitRemoveMember(head.byKeyAction, (LPCSTR)data, len);
        }
        break;
        // �����򳡾������ս������
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
        memcpy(sKinInfo.szName,pKin->getName(),sizeof(sKinInfo.szName));                            // ����
        sKinInfo.nKinProp_ID = pKin->getNumProp(emKinProp_ID);							            // ս��ID
        sKinInfo.nKinProp_State =pKin->getNumProp(emKinProp_State);						            // ״̬
        sKinInfo.nKinProp_Level =pKin->getNumProp(emKinProp_Level);                                 // ս�ӵȼ�
        sKinInfo.nKinProp_EstablishTime =pKin->getNumProp(emKinProp_EstablishTime);                 // ս�Ӵ���ʱ��
        sKinInfo.nKinProp_DismissApplyTime =pKin->getNumProp(emKinProp_DismissApplyTime);           // �����ɢ��ʱ��
        sKinInfo.nKinProp_ShaikhID =pKin->getNumProp(emKinProp_ShaikhID);                           // ս�Ӷӳ�PDBID
        sKinInfo.nKinProp_MemberLastLogoutTime =pKin->getNumProp(emKinProp_MemberLastLogoutTime);   // ��Ա���������ʱ��
        sKinInfo.nKinProp_ReadFlag =pKin->getNumProp(emKinProp_ReadFlag);                           // �Ѿ������ݿ�������ݵı�־
        sKinInfo.nKinProp_Money =pKin->getNumProp(emKinProp_Money);                                 // �ʽ�
        sKinInfo.nKinProp_Activity = pKin->getNumProp(emKinProp_Activity);                          // ��Ծ��
        sKinInfo.nKinProp_Place =pKin->getNumProp(emKinProp_Place);                                 // ����
        sKinInfo.nKinProp_WorldID =pKin->getNumProp(emKinProp_WorldID);                             // ��Ϸ����ID
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

