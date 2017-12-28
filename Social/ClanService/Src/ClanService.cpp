#include "stdafx.h"
#include "ClanService.h"
#include "ISocialGlobal.h"
#include "IMessageDispatch.h"
#include "Event_ManagedDef.h"
#include "ExternalHelper.h"
#include "Clan.h"
#include "ClanMessageDef.h"
#include "IShareServer.h"
#include "KinHelper.h"
#include "IKin.h"
#include "ClanInviteManager.h"
#include "Kin_ManagedDef.h"
#include "KinEvent.h"
#include "EventDef.h"
#include "ClanHelper.h"
#include "IDateTriggerService.h"

using namespace Kin_ManagerDef;
using namespace Clan_ManagerDef;

CClanService::CClanService()
{
    m_ClanList.clear();
	m_ClanArray.clear();
	m_NameClanList.clear();
	m_KinClanList.clear();

    m_ClanUserLoginList.clear();
    m_ReadingKinList.clear();
}

CClanService::~CClanService()
{

}

bool CClanService::create()
{
    // ���ݿ�ӿ�ע��
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService == NULL)
		return false;
	pDBEngineService->registerHandler(this);

    IMessageDispatch *pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if (pMessageDispatch == NULL)
        return false;
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_CLAN,this);
    pMessageDispatch->registerMessageHandler(MSG_MODULEID_CLAN, this);

    // ������Ϣע��
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
		return false;
	pShareReceiver->RegisterHandler(this, __FUNCTION__);

	// ս���¼�ע��
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return false;
	pEventEngine->Subscibe(this, EVENT_KIN_UPDATEID, SOURCE_TYPE_KIN, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_KIN_READ_DB_FINISHED, SOURCE_TYPE_KIN, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_KIN_CHANGE_SHAIKH, SOURCE_TYPE_KIN, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_KIN_DISMISS, SOURCE_TYPE_KIN, 0, __FUNCTION__);

    // ����������
    g_EHelper.setTimer(TimerID_Clan_Clear_User_Login_List, 30*60*1000, this, "CClanService::create");

	IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
    if (pDateTriggerService != NULL)
    {
		// ����ÿ����23:59��һ�λ�Ծ��
        SDateTriggerInfo sTriggerInfo;
        sTriggerInfo.year = -1;
        sTriggerInfo.month = -1;
        sTriggerInfo.day = -1;
        sTriggerInfo.hour = 23;
        sTriggerInfo.min = 59;
        sTriggerInfo.wday = 0;

		pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_WeekActivityReset, this, sTriggerInfo);

        sTriggerInfo.year = -1;
        sTriggerInfo.month = -1;
        sTriggerInfo.day = -1;
        sTriggerInfo.hour = -1;
        sTriggerInfo.min = 30;
        sTriggerInfo.wday = -1;
        pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_AutoSetShaikhState, this, sTriggerInfo);

		// ÿ����
        sTriggerInfo.year = -1;
        sTriggerInfo.month = -1;
        sTriggerInfo.day = -1;
        sTriggerInfo.hour = 0;
        sTriggerInfo.min = -1;
        sTriggerInfo.wday = -1;
        pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_LegendCupDidaUpdate, this, sTriggerInfo);


		// ÿ��1�ż��
		sTriggerInfo.year = -1;
		sTriggerInfo.month = -1;
		sTriggerInfo.day = 1;
		sTriggerInfo.hour = 0;
		sTriggerInfo.min = 0;
		sTriggerInfo.wday = -1;
		pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_MonsCheck, this, sTriggerInfo);
	}

	tm localNowTime;
	time_t currentTime = time(NULL);
	localtime_s(&localNowTime, &currentTime);
	if ((localNowTime.tm_wday == 0 && localNowTime.tm_hour >= 23)
		|| (localNowTime.tm_wday == 1 && localNowTime.tm_hour <= 1)) // ��������23��00 - ��һ1��00 ֮�䣬���ܻ�Ծ������
	{
		ResetWeekActivity();
	}

    return true;
}

void CClanService::release()
{
    // ���ݿ�ӿڷ�ע��
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->unregisterHandler(this);
    }

    IMessageDispatch *pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if (pMessageDispatch != NULL)
    {
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_CLAN);
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_CLAN);
    }

    // ������Ϣע��
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->UnregisterHandler(this);
	}

	// ս���¼�ע��
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine != NULL)
	{
		pEventEngine->UnSubscibe(this, EVENT_KIN_UPDATEID, SOURCE_TYPE_KIN, 0);
		pEventEngine->UnSubscibe(this, EVENT_KIN_READ_DB_FINISHED, SOURCE_TYPE_KIN, 0);
		pEventEngine->UnSubscibe(this, EVENT_KIN_CHANGE_SHAIKH, SOURCE_TYPE_KIN, 0);
		pEventEngine->UnSubscibe(this, EVENT_KIN_DISMISS, SOURCE_TYPE_KIN, 0);
		// ���� ��������������������
		pEventEngine->UnSubscibe(this, EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0);
	}

	IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
    if (pDateTriggerService != NULL)
    {
		pDateTriggerService->UnRegisterDateTrigger(DCT_DataTrigger_ClanUpdate, this);
		pDateTriggerService->UnRegisterDateTrigger(DCT_DataTrigger_WeekActivityReset, this);
        pDateTriggerService->UnRegisterDateTrigger(DCT_DataTrigger_AutoSetShaikhState, this);
        pDateTriggerService->UnRegisterDateTrigger(DCT_DataTrigger_LegendCupDidaUpdate, this);
		pDateTriggerService->UnRegisterDateTrigger(DCT_DataTrigger_MonsCheck, this);
		
        
	}

	g_EHelper.killTimer(TimerID_Clan_Clear_User_Login_List, this);

    delete this;
}

// �������ս��
DWORD CClanService::findKinClan(DWORD dwKinID)
{
	TMap_KinClanIt it = m_KinClanList.find(dwKinID);
	if (it == m_KinClanList.end())
	{
		return 0;
	}

	return it->second;
}

// �Ƴ����ڶ�ȡ��ս��
void CClanService::removeReadingKin(DWORD dwKinID)
{
	DWORD dwClanID = findKinClan(dwKinID);
	if (dwClanID == 0)
		return;

	map<DWORD, list<DWORD>>::iterator it = m_ReadingKinList.find(dwClanID);
	if (it == m_ReadingKinList.end())
		return;

	list<DWORD>& kinList = ((*it).second);
	kinList.remove(dwKinID);

	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return;

	// ɾ�������½����
	int nReadFlag = pClan->getNumProp(emClanProp_ReadFlag);
	if (nReadFlag == emClanRead_AllData && isReadingKinEmpty(dwClanID) == true)
	{
		// TraceLn(_GT("CClanService::removeReadingKin ��ȡ����������ɣ�����ID=") << dwClanID);
		onReadDBFinish(dwClanID);
	}
}

// ��ȡ���˳�Ա�б�
int CClanService::getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen)
{
	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return 0;

	return pClan->getKinList(pKinList, nLen);
}

// ȡ��������ֵ
int CClanService::getClanNumProp(DWORD dwClanID, int nNumIndex)
{
    CClan* pClan = findClan(dwClanID);
    if (pClan == NULL)
        return 0;

    return pClan->getNumProp(nNumIndex);
}

// ��ȡ��������
string CClanService::getClanName(DWORD dwClanID)
{
	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return "";
	return pClan->getName();
}

// ���˱�������
void CClanService::notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName)
{
	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return;

	pClan->broadcastClanCreateLegendCupToClient(llLegendCupID, nMaxKinCount, pCupName);

    pClan->recCreateLegendCupEvent(pCupName);
    
    
}

////////////////////////////////ITransmitHandler//////////////////////////////////////////
// ������֮����Ϣ����
void CClanService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead *head, void *data, size_t len)
{
    ClanHelper clanHelper;
    IClanService *pClanService = clanHelper.m_ptr;
    if (pClanService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    pClanService->handleServerMsg( server, *head, data, len );
}

////////////////////////////////IMessageHandler//////////////////////////////////////////
// �ͻ�����Ϣ����
void CClanService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead *head, void *data, size_t len)
{
    ClanHelper clanHelper;
    IClanService *pClanService = clanHelper.m_ptr;
    if (pClanService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    pClanService->handleClientMsg( clientID, *head, data, len );
}

void CClanService::handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len)
{
    // ������ת����������Ϣ
    if (data == NULL || len<sizeof(msg_clan_sub_head))
    {
        ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< len <<", sizeof="<< sizeof(msg_clan_sub_head) );
        return;
    }
    msg_clan_sub_head *pMsg = (msg_clan_sub_head*)(data);

    LPSTR pMsgData = (LPSTR)data + sizeof(msg_clan_sub_head);
    size_t nNewlen = len - sizeof(msg_clan_sub_head);

    // ����������������Ϣ
    switch ( head.byKeyAction )
    {
    case MSG_CLAN_MODULE:
        {
            onModuleHandleMessage(serverID, pMsg->dwMsgCode, pMsgData, nNewlen);
        }
        break;
    case MSG_CLAN_CLAN:
        {
            CClan* pClan = findClan(pMsg->dwClanID);
            if (pClan == NULL)
            {
                WarningLn(__FUNCTION__": not exists clan object, clanid="<<pMsg->dwClanID);
                return;
            }

            pClan->onClanHandleMessage(serverID, pMsg->dwMsgCode, pMsgData, nNewlen);
        }
        break;

    default:
        {
            // ������������Ϣ
            ErrorLn(__FUNCTION__": unknown byKeyAction= "<< head.byKeyAction);
        }
        break;
    }
}

void CClanService::handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len)
{
    if (data == NULL || len < sizeof(msg_clan_sub_head))
    {
        ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< len <<", sizeof="<< sizeof(msg_clan_sub_head) );
        return;
    }
    msg_clan_sub_head *pMsg = (msg_clan_sub_head*)(data);

    LPSTR pMsgData = (LPSTR)data + sizeof(msg_clan_sub_head);
    size_t nNewlen = len - sizeof(msg_clan_sub_head);

    // �ͻ��˷���������Ϣ
    switch ( head.byKeyAction )
    {
    case MSG_CLAN_MODULE:
        {
            onModuleClientMsg(client, pMsg->dwMsgCode, pMsgData, nNewlen);
        }
        break;
    case MSG_CLAN_CLAN:
        {
            CClan* pClan = findClan(pMsg->dwClanID);
            if (pClan == NULL)
            {
                return;
            }

            pClan->onClanClientMsg(client, pMsg->dwMsgCode, pMsgData, nNewlen);
        }
        break;
    default:
        {
            // ������Ϣ
            ErrorLn(__FUNCTION__": unknown byKeyAction= " << head.byKeyAction);
        }
        break;
    }
}

// ֪ͨ�������б��Ѿ�����
void CClanService::onServerListUpdated()
{

}

// ֪ͨ��������Ϣ����
void CClanService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void *pServerData)
{
	ServerData *pInfo = (ServerData*)pServerData;
	if (nState == SERVER_CONNECTED && pInfo->wServerType == MSG_ENDPOINT_SCENE)
	{
		refreshClanInfoToZone(ServerID);
	}
	else if (nState == SERVER_CONNECTED && pInfo->wServerType == MSG_ENDPOINT_SOCIAL)
	{
		refreshClanInfoToZone();
	}
	return;
}



/////////////////////////////////////IDBRetSink/////////////////////////////////////
void CClanService::OnReturn(IDBRetSink *pRealDBRetSink, int nCmdID, int nDBRetCode, char *pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen)
{
    switch (nCmdID)
    {
    case GAMEDB_REQUEST_READCLANLIST:
        {
            onReturnReadClanList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;       
    case GAMEDB_REQUEST_READMEMBERLIST:     // ��ȡ���˳�Ա�б�
        {
            onReturnReadMemberList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;        
    case GAMEDB_REQUEST_UPDATEMEMBER:       // ���³�Ա��û������ӣ�
        {
            onReturnUpdateMember(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);            
        }
        break;
    case GAMEDB_REQUEST_DELETEMEMBER:
        {
            onReturnRemoveMember(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;
    case GAMEDB_REQUEST_UPDATECLAN:
        {
            onReturnUpdateClan(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;
    case GAMEDB_REQUEST_DISMISSCLAN:
        {
            onReturnDismissClan(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;
    default:
        break;
    }
}

////////////////////////////////IEventExecuteSink//////////////////////////////////////////
void CClanService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch(wEventID)
	{
	case EVENT_SYSTEM_STARTFINISH:
		{
			IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
            if (pDateTriggerService != NULL)
            {
                SDateTriggerInfo sTriggerInfo;
                sTriggerInfo.year = -1;
                sTriggerInfo.month = -1;
                sTriggerInfo.day = 1;
                sTriggerInfo.hour = 0;
                sTriggerInfo.min = 0;
                sTriggerInfo.wday = -1;
				pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_ClanUpdate, this, sTriggerInfo);	// ����ʱ�̶�ʱ����ÿ�µ�1��00��00��
			}
		}
		break;
	case EVENT_KIN_UPDATEID:
		{
			if (bSrcType != SOURCE_TYPE_KIN || pszContext == NULL || nLen != sizeof(SEventKin_UpdateKinID))
				return;

			SEventKin_UpdateKinID *pUpdateKinID = (SEventKin_UpdateKinID*)pszContext;
			IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
			if (pShareReceiver==NULL)
				return;

            SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pUpdateKinID->dwPDBID);
			if (shareProperty.dwPDBID == pUpdateKinID->dwPDBID)
			{
				// ���ý�ɫ���ϵ�����ID
				if (pUpdateKinID->dwKinID == 0)
				{
					setActorClanID(pUpdateKinID->dwPDBID, 0, shareProperty.serverID);
				}
				else
				{
					DWORD dwClanID = findKinClan(pUpdateKinID->dwKinID);
					if (dwClanID == 0)
						return;

					setActorClanID(pUpdateKinID->dwPDBID, dwClanID, shareProperty.serverID);

                    // ��Ϣ���µ��ͻ���
                    CClan* pClan = findClan(dwClanID);
                    if (pClan != NULL)
                    {
                       pClan->updateDataToClient(pUpdateKinID->dwPDBID);
                    }
				}

                
			}
		}
		break;
	case EVENT_KIN_READ_DB_FINISHED:
		{
			if (bSrcType == SOURCE_TYPE_KIN)
			{
				// ��ȡ�����б���Ϣ
				readClanList();
			}
		}
		break;
	case EVENT_KIN_CHANGE_SHAIKH:
	{
		if (bSrcType != SOURCE_TYPE_KIN || pszContext == NULL || nLen != sizeof(SEventKin_ChangeShaikh))
			return;

		SEventKin_ChangeShaikh* pChangeShaikh = (SEventKin_ChangeShaikh*)pszContext;

		DWORD dwClanID = findKinClan(pChangeShaikh->dwKinID);
		if (dwClanID == 0)
			return;
		CClan* pClan = findClan(dwClanID);
		if (pClan != NULL && pChangeShaikh->dwOldShaikh == pClan->getNumProp(emClanProp_ShaikhID))
		{
			pClan->appointShaikh(pChangeShaikh->dwOldShaikh, pChangeShaikh->dwNewShaikh);
		}
	}
	break;
	case EVENT_KIN_DISMISS:
	{
		if (bSrcType != SOURCE_TYPE_KIN || pszContext == NULL || nLen != sizeof(SEventKin_Dismiss))
			return;

		SEventKin_Dismiss* pKinDismiss = (SEventKin_Dismiss*)pszContext;
		DWORD dwClanID = findKinClan(pKinDismiss->dwKinID);
		if (dwClanID == 0)
			return;
		CClan* pClan = findClan(dwClanID);
		if (pClan != NULL)
		{
			pClan->delMember(pKinDismiss->dwKinID, pKinDismiss->dwShaikhID, true);
		}
	}
	break;
	default:
		break;
	}
}

////////////////////////////////TimerHandler//////////////////////////////////////////
void CClanService::OnTimer( unsigned long dwTimerID )
{
    switch (dwTimerID)
    {
    case TimerID_Clan_Clear_User_Login_List:
        {
            // ɾ����ʱ��
            g_EHelper.killTimer(TimerID_Clan_Clear_User_Login_List, this);

            // ����������˵�½�����б�
            m_ClanUserLoginList.clear();
        }
        break;
    default:
        break;
    }
}

////////////////////////////////////ISharePersonHandler////////////////////////////////////
void CClanService::OnLogin(ISharePerson *pSharePerson, ELoginMode nLineType)
{
    if (pSharePerson == NULL)
        return;

    if (elogin_online != nLineType)
    {
        return;
    }

    DWORD dwPDBID = pSharePerson->GetNumProp(PROPERTY_ID);
    DWORD dwKinID = pSharePerson->GetNumProp(PROPERTY_KIN);
    DWORD dwClanID = pSharePerson->GetNumProp(PROPERTY_CLAN);

    if (dwKinID == 0 || dwClanID == 0)
        return;

    // �����Ա���ݶ���������ս������Ҳ��ȡ���
    DWORD dwKinClanID = findKinClan(dwKinID);
    if (dwKinClanID > 0 && isReadingKinEmpty(dwClanID) == true)
    {
        // ������������Ѿ��������ˣ�ֱ��֪ͨ��Ա����
        onMemberLogin(dwClanID, dwPDBID);
        return;
    }

    // �������������Ѿ��ڻ����б����ˣ����ô�����
    SClanUserLogin* pLoginUser = findLoginMember(dwClanID);
    if (pLoginUser != NULL)
        return;

    SClanUserLogin userLogin;
    CClan* pClan = findClan(dwClanID);
    // ������������Ѿ���ȡ��ϣ����ﷵ��
    if (pClan != NULL && pClan->isReadFinish() == true)
        return;

    // ������˶����Ѿ������ˣ��������ݿ��ȡ��������
    if (pClan != NULL)
    {
        // ��ȡ���˵ĸ�������
        readClanPlusData(dwClanID);
        userLogin.bIsRequest = true;
    }
    else
    {
        userLogin.bIsRequest = false;
    }

    // ��ӵ������б�
    userLogin.dwClanID = dwClanID;
    userLogin.userList.push_back(dwPDBID);
    m_ClanUserLoginList[dwClanID] = userLogin;
}

void CClanService::OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType)
{
    // ��ҵǳ�
    if (elogout_cross_process == nLineType || elogout_cross_gameworld == nLineType || elogout_in_process == nLineType)
    {
        return;
    }

	CClan * pClan = findClan(pSharePerson->GetNumProp(PROPERTY_CLAN));
	if (pClan == NULL)
	{
		return;
	}

	pClan->onMemberLogout(pSharePerson, nLineType);
}

void CClanService::OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
}

void CClanService::OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
	PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);
	if (PersonDBID == INVALID_PDBID)
	{
		return;
	}

	// ���Ը���
	switch(nUpdateReason)
	{
	case SHAREUPDATE_REASON_CHANGE_ACTORNAME:
		{
			// �㲥���ͻ��˸����������
			CClan * pClan = findClan(pSharePerson->GetNumProp(PROPERTY_CLAN));
			if (pClan == NULL)
			{
				return;
			}

			if (pClan->getNumProp(emClanProp_ShaikhID) != PersonDBID)
			{
				return;
			}

			pClan->setShaikhName(pSharePerson->GetName());
		}
		break;

	default:
		break;
	}

	return;
}

////////////////////////////////DateHandler//////////////////////////////////////////
void CClanService::DateCome(unsigned long nTriggerID)
{
	if (nTriggerID == DCT_DataTrigger_ClanUpdate)
	{
		UpdateAllClanLegendCupProp();
	}
	else if (nTriggerID == DCT_DataTrigger_WeekActivityReset)
	{
		autoDismissLowerActivity();
		ResetWeekActivity();
	}
    else if (nTriggerID == DCT_DataTrigger_AutoSetShaikhState)
    {
        AutoSetShaikhState();
    }
    else if (nTriggerID == DCT_DataTrigger_LegendCupDidaUpdate)
    {
        updateClanLegendDida();
    }
	else if(nTriggerID == DCT_DataTrigger_MonsCheck)
	{
		refreshCreateLegendCupCount();
	}
}

////////////////////////////////CClanService//////////////////////////////////////////
// ��������
CClan* CClanService::findClan(DWORD dwClanID)
{
    TMap_ClanIt it = m_ClanList.find(dwClanID);
    if (it == m_ClanList.end())
        return NULL;

    return (*it).second;
}

// ��������
CClan* CClanService::findClanByName(string szClanName)
{
    // ͨ���������ֲ�������
    TMap_NameClanIt it = m_NameClanList.find(szClanName);
    if (it == m_NameClanList.end())
        return NULL;

    return (*it).second;
}

// �������ս��
DWORD CClanService::findKinClanByPDBID(DWORD dwPDBID)
{
    SKinMember* pKinMember = g_EHelper.getActorKinMemberInfo(dwPDBID);
    if (pKinMember == NULL)
        return NULL;

    return findKinClan(pKinMember->dwKinID);
}

// �Ƴ�����
void CClanService::removeClan(DWORD dwClanID)
{
	string strName;
	TMap_ClanIt it = m_ClanList.find(dwClanID);
	if (it == m_ClanList.end())
		return;

	CClan* pClan = ((*it).second);
	strName = pClan->getName();
	m_ClanList.erase(it);

	TMap_NameClanIt itName = m_NameClanList.find(strName);
	if (itName != m_NameClanList.end())
	{
		m_NameClanList.erase(itName);
	}

	// �������б��в���ɾ��
	for (list<CClan*>::iterator itList= m_ClanArray.begin(); itList != m_ClanArray.end(); )
	{
		if ((*itList)->getNumProp(emClanProp_ID) == dwClanID)
			itList = m_ClanArray.erase(itList);
		else
			itList++;
	}
}

// ������˳�Ա��������£�
void CClanService::addClanMember(DWORD dwClanID, DWORD dwKinID, bool bNeedSaveDB)
{
    if (dwClanID == 0)
        return;

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

    // �ڹ����ȡһ����Ϣ
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
        return;

    // ��ӵ�ȫ�ֳ�Ա�б�
    m_KinClanList[dwKinID] = dwClanID;

    // �Ѹ�ս�ӵ����г�Ա������ID���óɵ�ǰ����
    SKinInfo sKin = pKinService->getKinInfo(dwKinID);
    if (sKin.nKinProp_ID == 0)
        return;

    CClan* pClan = findClan(dwClanID);
    if (pClan == NULL)
        return;

    PDBID pReturnArray[256] = {0};
    DWORD dwCount = pKinService->getKinMemberList(dwKinID, pReturnArray, 256);
    for (DWORD i = 0; i < dwCount; ++i)
    {
        DWORD dwPDBID = pReturnArray[i];

		SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
		if (shareProperty.dwPDBID == INVALID_PDBID)
			continue;

        SKinMember pKinMember = pKinService->getKinMemberInfo(dwPDBID);
        if (pKinMember.dwKinID == NULL)
            continue;

		// ��������ID��������
		setActorClanID(dwPDBID, dwClanID, shareProperty.serverID);

        // ͬ�����ͻ���
        updateMemberToClient_OC(dwClanID, &pKinMember);
    }

    // �洢���ݿ�
    if (bNeedSaveDB == true)
    {
        updateMemberToDB(dwClanID, dwKinID);

		// ��������ID�ı��¼�
		IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
		if (pEventEngine != NULL)
		{
			obuf buf;
			event_clan_update_id eventData;
			eventData.dwKinID = dwKinID;
			eventData.dwClanID = dwClanID;
			buf.push_back(&eventData, sizeof(eventData));
			pEventEngine->FireExecute(EVENT_CLAN_UPDATEID, SOURCE_TYPE_CLAN, dwClanID, buf.data(), buf.size());
		}
    }

    // ͬ����������
    updateMemberToZone(dwClanID, dwKinID);
}

// ɾ�����˳�Ա
void CClanService::removeClanMember(DWORD dwKinID, bool bNeedSaveDB)
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
        return;

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

    // �ڹ����ȡһ����Ϣ
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
        return;

    // �����ڸó�Ա
	DWORD dwClanID = findKinClan(dwKinID);
    if (dwClanID == 0)
        return;

    SKinInfo sKin = pKinService->getKinInfo(dwKinID);
    if (sKin.nKinProp_ID == 0)
        return;

    CClan* pClan = findClan(dwClanID);
    if (pClan == NULL)
    {
        // ���˳�Ա�б��Ƴ�
        m_KinClanList.erase(dwKinID);
		if (m_KinClanList.empty())
		{
			removeClan(dwClanID);
		}

        return;
    }

    PDBID pReturnArray[256] = {0};
    DWORD dwCount = pKinService->getKinMemberList(dwKinID, pReturnArray, 256);
    for (DWORD i = 0; i < dwCount; ++i)
    {
        DWORD dwPDBID = pReturnArray[i];

        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
        if (shareProperty.dwPDBID == INVALID_PDBID)
            continue;

        // ֪ͨ�ͻ�����ճ�Ա����
        {
            obuf256 ob;
            fillClanData(ob, MSG_ENDPOINT_CLIENT, MSG_CLAN_QUIT);
            g_EHelper.sendDataToPerson(dwPDBID, ob.data(), ob.size());
        }

        // ��������ID��������
		setActorClanID(dwPDBID, 0, shareProperty.serverID);
    }

    {
        // �����򳡾���ɾ����Ա
        obuf ob;
        msg_clan_removekin_os removeMember;
        removeMember.dwKinID = dwKinID;
        removeMember.dwClanID = dwClanID;

        fillClanData(ob, MSG_ENDPOINT_SCENE, MSG_CLAN_REMOVEMEMBER_ZONE);
        ob.push_back(&removeMember, sizeof(removeMember));
        g_EHelper.sendDataToZoneSvr(ob.data(), ob.size());
    }

	{
		// ��������ID�ı��¼�
		IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
		if (pEventEngine != NULL)
		{
			obuf buf;
			event_clan_update_id eventData;
			eventData.dwKinID = dwKinID;
			eventData.dwClanID = 0;
			buf.push_back(&eventData, sizeof(eventData));
			pEventEngine->FireExecute(EVENT_CLAN_UPDATEID, SOURCE_TYPE_CLAN, dwClanID, buf.data(), buf.size());
		}
	}

    if (!isPublicGameWorld())
    {
        // �洢���ݿ�
        if (bNeedSaveDB)
        {
            // ֪ͨ���ݿ���̳�Ա����
            DBREQ_PARAM_DELETEMEMBER referData;
            referData.dwKinID = dwKinID;
            if (pDBEngine->executeSP(GAMEDB_REQUEST_DELETEMEMBER, referData.dwKinID, (LPCSTR)(&referData), sizeof(referData), this) == false)
            {
                ErrorLn(_GT("GAMEDB_REQUEST_DELETEMEMBER ����ʧ��"));
            }
        }
    }

    // ���˳�Ա�б��Ƴ�
    m_KinClanList.erase(dwKinID);
}

// ���³�Ա���ݵ����ݿ�
void CClanService::updateMemberToDB(DWORD dwClanID, DWORD dwKinID)
{
	IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
	if (dwClanID == 0 || pDBEngineService == NULL)
		return;

	// ֪ͨ���ݿ���̳�Ա����
	DBREQ_PARAM_UPDATEMEMBER memberData;
	memberData.dwClanID =  dwClanID;
	memberData.dwKinID = dwKinID;
	if (pDBEngineService->executeSP(GAMEDB_REQUEST_UPDATEMEMBER, memberData.dwKinID, (LPCSTR)(&memberData), sizeof(memberData), this) == false)
	{
		ErrorLn(_GT("GAMEDB_REQUEST_UPDATEMEMBER ����ʧ��"));
	}
}

// ����ȫ���������ݵ�������
bool CClanService::refreshClanInfoToZone(CSID dwServerID)
{
	// ֪ͨ�ó�����ɾ��ԭ����������
	{
		obuf512 obuf;
		msg_clan_removeclan_os sendData;
		sendData.dwClanID = 0;
		fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_REMOVECLAN_ZONE);
		obuf.push_back(&sendData, sizeof(sendData));
		g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), dwServerID);
	}

	// �����˵��������ݷ���������
	TMap_ClanIt itClan;
	for (itClan = m_ClanList.begin(); itClan != m_ClanList.end(); ++ itClan)
	{
		CClan *pClan = (*itClan).second;
		pClan->updateDataToZone(dwServerID);
	}

	return true;
}

// ���³�Ա���ݵ�������
void CClanService::updateMemberToZone(DWORD dwClanID, DWORD dwKinID, CSID ServerID)
{
	SZoneClanMember sendData;
	sendData.dwClanID = dwClanID;
	sendData.dwKinID = dwKinID;

	obuf512 obuf;
	fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATEMEMBER_ZONE);
	obuf.push_back(&sendData, sizeof(SZoneClanMember));
	g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), ServerID);
}

// ���³�Ա���ݵ��ͻ���
void CClanService::updateMemberToClient_OZC(DWORD dwClanID, SKinMember* pKinMember)
{
	if (pKinMember == NULL)
		return;

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	// �ڹ����ȡһ����Ϣ
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
		return;

	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return;

	SHeroClanInfo heroClanInfo;
	heroClanInfo.dwPDBID = pKinMember->dwPDBID;
	heroClanInfo.dwClanID = dwClanID;
	heroClanInfo.dwKinID = pKinMember->dwKinID;
	heroClanInfo.nClanCtrb = pKinMember->nClanCtrb;
	heroClanInfo.nTotalClanCtrb = pKinMember->nTotalClanCtrb;
	heroClanInfo.nIdentity = g_EHelper.getClanIdentity(pKinMember->dwPDBID, pClan->getNumProp(emClanProp_ShaikhID), pKinMember->nIdentity);

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pKinMember->dwPDBID);
	if (shareProperty.dwPDBID == pKinMember->dwPDBID)
	{
		obuf512 obuf;
		fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATESELFCLANINFO);
		obuf.push_back(&pKinMember->dwPDBID, sizeof(DWORD));
		obuf.push_back(&heroClanInfo, sizeof(SHeroClanInfo));
		g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
	}
}

// ���³�Ա���ݵ��ͻ���
void CClanService::updateMemberToClient_OC(DWORD dwClanID, SKinMember* pKinMember)
{
	if (pKinMember == NULL)
		return;

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	// �ڹ����ȡһ����Ϣ
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
		return;

	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return;

	SHeroClanInfo heroClanInfo;
	heroClanInfo.dwPDBID = pKinMember->dwPDBID;
	heroClanInfo.dwClanID = dwClanID;
	heroClanInfo.dwKinID = pKinMember->dwKinID;
	heroClanInfo.nClanCtrb = pKinMember->nClanCtrb;
	heroClanInfo.nTotalClanCtrb = pKinMember->nTotalClanCtrb;
	heroClanInfo.nIdentity = g_EHelper.getClanIdentity(pKinMember->dwPDBID, pClan->getNumProp(emClanProp_ShaikhID), pKinMember->nIdentity);

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pKinMember->dwPDBID);
	if (shareProperty.dwPDBID == pKinMember->dwPDBID)
	{
		obuf512 obuf;
		fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_UPDATESELFCLANINFO);
		obuf.push_back(&heroClanInfo, sizeof(SHeroClanInfo));
		g_EHelper.sendDataToPerson(pKinMember->dwPDBID, obuf.data(), obuf.size());
	}
}

// ��Ա���ߣ������ͳ���������Ҳ��֪ͨ��
void CClanService::onMemberLogin(DWORD dwClanID, DWORD dwPDBID)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	SClanConfig* pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
		return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	SKinMember sKinMember = pKinService->getKinMemberInfo(dwPDBID);
	if (sKinMember.dwKinID == 0)
		return;

	if (sKinMember.nLoginTime == 0)
	{
		sKinMember.nLoginTime = (int)time(NULL);
	}

	SKinInfo sKin = pKinService->getKinInfo(sKinMember.dwKinID);
	if (sKin.nKinProp_ID == 0)
		return;

	DWORD dwKinClanID = findKinClan(sKinMember.dwKinID);
	if (dwKinClanID != dwClanID)
		return;

	// ��������˲��Ƿ���ʽ���ˣ�֪ͨ�ͻ���ˢ��ͷ����������
	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return;

	// �����˵Ļ�����Ϣ���͵��ͻ���
	pClan->updateDataToClient(dwPDBID, shareProperty.serverID);

	// �����ߵ���Ϣ�㲥���۲���
	SClientClanMember sMember;
	pClan->fillClientMemberInfo(sMember, sKin, &sKinMember);
	pClan->broadcastMsgToObserve(MSG_CLAN_UPDATEMEMBERINFO, (LPCSTR)&sMember, sizeof(sMember));

	// ���Լ�����ϢҲ�����ͻ���
	updateMemberToClient_OZC(dwClanID, &sKinMember);

	pClan->onMemberLogin(shareProperty);

	// ͬ���ó�Ա��������
	updateMemberToZone(dwKinClanID, sKinMember.dwKinID);

	// ������Ա��½�¼�
	event_clan_memberlogin loginEvent;
	loginEvent.dwClanID = dwClanID;
	loginEvent.dwPDBID = dwPDBID;
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine != NULL)
	{
		pEventEngine->FireExecute(EVENT_CLAN_MEMBERLOGIN, SOURCE_TYPE_CLAN, dwPDBID, (LPCSTR)&loginEvent, sizeof(loginEvent));
	}

	// ֪ͨ���˱�����ʾ
	pClan->broadcastClanLegendCupInfoToClient(shareProperty.dwPDBID);

	// �����������峤���ҵ�ǰ��Ծ��С�ڽ�ɢ��Ծ������һ��Dida
	int nIdentity = g_EHelper.getClanIdentity(dwPDBID, pClan->getNumProp(emClanProp_ShaikhID), sKinMember.nIdentity);
	if (nIdentity == EMCLANIDENTITY_SHAIKH)
	{
		// ͬ������
		pClan->updateClientClanBaseData(dwPDBID);

		// �жϵ�ǰ��Ծ���Ƿ���ڽ�ɢ��Ծ��
		if (pClan->getNumProp(emClanProp_Activity) < pConfig->nDismissActivity)
		{
			// ����һ��Dida,����Ҫ��ɢ��
		}
	}
}

// �������˵ĵ�½�����Ա
SClanUserLogin* CClanService::findLoginMember(DWORD dwClanID)
{
	TMap_ClanUserLoginIt it = m_ClanUserLoginList.find(dwClanID);
	if (it == m_ClanUserLoginList.end())
		return NULL;

	return &((*it).second);
}

// ������ڶ�ȡ��ս��
bool CClanService::addReadingKin(DWORD dwClanID, DWORD dwKinID)
{
	map<DWORD, list<DWORD>>::iterator it = m_ReadingKinList.find(dwClanID);
	if (it == m_ReadingKinList.end())
	{
		list<DWORD> kinlist;
		kinlist.push_back(dwKinID);

		m_ReadingKinList[dwClanID] = kinlist;
	}
	else
	{
		list<DWORD>& kinList = ((*it).second);
		kinList.remove(dwKinID);
		kinList.push_back(dwKinID);
	}

	return true;
}

// �Ƿ�Ϊ��
bool CClanService::isReadingKinEmpty(DWORD dwClanID)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return false;

	map<DWORD, list<DWORD>>::iterator it = m_ReadingKinList.find(dwClanID);
	if (it == m_ReadingKinList.end())
		return true;

	list<DWORD>& kinList = ((*it).second);
	list<DWORD>::iterator itKin;
	for (itKin = kinList.begin(); itKin != kinList.end(); ++ itKin)
	{
        SKinInfo sKin = pKinService->getKinInfo(*itKin);
        if (sKin.nKinProp_ID == 0)
            continue;

		DWORD dwKinReadFlag = sKin.nKinProp_ReadFlag;
		if (dwKinReadFlag != emKinRead_AllData)
			return false;
	}

	return true;
}

// ���˵�ȫ�����ݶ�ȡ���
void CClanService::onReadDBFinish(DWORD dwClanID)
{
	// EmphasisLn(_GT("����ȫ�����ݶ�ȡ��� ���ID=") << dwClanID);
	TMap_ClanUserLoginIt it = m_ClanUserLoginList.find(dwClanID);
	if (it != m_ClanUserLoginList.end())
	{
		SClanUserLogin* pUserLogin = &((*it).second);
		// ֪ͨ��Ա����
		list<DWORD>::iterator itMember= pUserLogin->userList.begin();
		for (; itMember != pUserLogin->userList.end(); ++ itMember)
		{
			onMemberLogin(dwClanID, (*itMember));
		}

		m_ClanUserLoginList.erase(it);
	}

	CClan *pClan = findClan(dwClanID);
	if (pClan == NULL)
		return;

	// ���ð���ȡ���
	pClan->setReadFinish(true);
    pClan->updateClanOrderInfo();
}

// �����ܻ�Ծ�ȼ���Ƿ���Ҫ��ɢ������
void CClanService::autoDismissLowerActivity()
{
	SClanConfig* pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;
	
	TMap_Clan clanList = m_ClanList;
	for (TMap_ClanIt it = clanList.begin(); it != clanList.end(); ++it)
	{
		CClan* pClan = ((*it).second);
		pClan->autoDismiss(pConfig->nDismissActivity, pConfig->nDismissWeeks);
	}

	return;
}

// ����Ƿ���Ҫ��ɢ������
bool CClanService::checkDBDismiss(DBREQ_TABLE_CLAN* pClanContext)
{
	SClanConfig* pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return false;

	// ����ǽ�ɢ������״̬
	if (pClanContext->nState == emClanState_Dismissing)
	{
		// ����뷢���ɢʱ���1��Сʱ�͵�7�죬���ɢ����
		int nCurTime = (int)time(NULL);
		int nInterval = pConfig->nDismissingLife - (nCurTime - pClanContext->nDismissApplyTime);
		if (nInterval < 60 * 60)
			return true;
	}

	return false;
}

// �ύ��ɢ��������
void CClanService::onDismissClan(DWORD dwClanID)
{
	// �����ݿ��ύ��ɢ��������
	IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
		return;

	// �����Ŀ����ݿ��ɢ���������
	DBREQ_PARAM_CENTERDBDISMISSCLAN centerDismiss;
	centerDismiss.dwClanID = dwClanID;
	if(!pDBEngineService->executeSP(GAMEDB_REQUEST_CENTERDBDISMISSCLAN, dwClanID, (LPCSTR)&centerDismiss,
		sizeof(centerDismiss), static_cast<IDBRetSink *>(this)))
	{
		ErrorLn(_GT("GAMEDB_REQUEST_CENTERDBDISMISSCLAN ����ʧ�ܣ�dwClanID = ") << dwClanID);
		return;
	}

	// �����ݿ��ύ��ɢ��������
	DBREQ_PARAM_DISMISSCLAN referData;
	referData.dwClanID = dwClanID;
	if (pDBEngineService->executeSP(GAMEDB_REQUEST_DISMISSCLAN, dwClanID, (LPCSTR)(&referData), sizeof(referData), this) == false)
	{
		ErrorLn(_GT("GAMEDB_REQUEST_DISMISSCLAN ����ʧ��"));
	}
}

/// ִ�д洢����
bool CClanService::onExecuteSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR data, int nLen)
{
	IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
		return false;

	if (!pDBEngineService->executeSP(dwCmdID, dwQueueIndex, data, nLen, this))
	{
		ErrorLn("onExecuteSP dwCmdID="<<dwCmdID<<" fail");
		return false;
	}

	return true;
}

// ��ȡ�����б�           -- �������ݿ�
void CClanService::onReturnReadClanList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // ��֤���ݿⷵ�ؽ��
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnReadClanList] ���ݿ��ȡ�����б�ʧ�ܣ�pszDBRetDesc = ") << pszDBRetDesc);
        return;
    }

    // ������ݷ��س���
    if (nOutLen < sizeof(DBREQ_RESULT_READCLANLIST))
    {
        int tempLen = sizeof(DBREQ_RESULT_READCLANLIST);
        ErrorLn(_GT("[onReturnReadClanList] ���Ȳ��� ���ݷ��س��� = ") << nOutLen << " sizeof(DBREQ_RESULT_READCLANLIST) = " << tempLen);
        return;
    }

    DBREQ_RESULT_READCLANLIST* pRetData = (DBREQ_RESULT_READCLANLIST*)pOutData;
    if (nOutLen != sizeof(DBREQ_RESULT_READCLANLIST) + pRetData->nClanCount * sizeof(DBREQ_TABLE_CLAN))
    {
        int tempLen = sizeof(DBREQ_RESULT_READCLANLIST) + pRetData->nClanCount * sizeof(DBREQ_TABLE_CLAN);
        ErrorLn(_GT("[onReturnReadClanList] ���Ȳ��� ���ݷ��س��� = ") << nOutLen << _GT(" ͷ�ļ��ṹ���� = ") << tempLen);
        return;
	}

    // �������˶���
    char szText[512] = {0};
    int offset = sizeof(DBREQ_RESULT_READCLANLIST);
    for (int i = 0; i < pRetData->nClanCount; i ++)
    {
        DBREQ_TABLE_CLAN* pClanData = (DBREQ_TABLE_CLAN*)(pOutData + offset);

        // ��������Ƿ���Ҫ��ɢ��
        if (checkDBDismiss(pClanData) == true)
        {
            // ��¼��ɢ������־
            //ssprintf_s(szText, sizeof(szText), "[%s]���˽�ɢ %s", pClanData->szName, "�����������ֹ�������");
            //g_EHelper.TrackGameNote(ENoteTrackType_Clan, pClanData->dwID, szText);

            // �����ݿ��ύ��ɢ��������
            onDismissClan(pClanData->dwClanID);
            offset += sizeof(DBREQ_TABLE_CLAN);
            continue;
        }

        // �������˶���
        CClan* pClan = new CClan();
        if (pClan == NULL)
        {
            ErrorLn(_GT("DB�������˶���ʧ�ܣ�pClan == NULL, ��������") << pClanData->szName);
            offset += sizeof(DBREQ_TABLE_CLAN);
            continue;
        }

        if (pClan->onDBCreate(this, (LPCSTR)pClanData, sizeof(DBREQ_TABLE_CLAN)) == false)
        {
            ErrorLn(_GT("DB�������˶���ʧ�ܣ���������") << pClanData->szName);
            offset += sizeof(DBREQ_TABLE_CLAN);
            continue;
        }

        // �����˶�����ӵ��б�
        m_ClanList[pClanData->dwClanID] = pClan;
        m_ClanArray.push_front(pClan);
        m_NameClanList[pClanData->szName] = pClan;

        // ֪ͨ�������������
        obuf512 ob;
        fillClanData(ob, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATECLAN_ZONE);
        msg_clan_updateclan_os sendData;
        sendData.dwID = pClanData->dwClanID;
        sstrcpyn(sendData.szName, pClanData->szName, sizeof(sendData.szName));
        sendData.nState = pClanData->nState;
        sendData.nLevel = pClanData->nLevel;
        sendData.nShaikhID = pClanData->nShaikhID;
        sendData.nWorldID = pClan->getNumProp(emClanProp_WorldID);
		sendData.nWeekActivity = pClan->getNumProp(emClanProp_WeekActivity);
		sendData.nLegendCupCount = pClanData->nLegendCupCount;
        sstrcpyn(sendData.szShaikhName, pClanData->szShaikhName, sizeof(sendData.szShaikhName));
		ob.push_back(&sendData, sizeof(sendData));
        g_EHelper.sendDataToZoneSvr(ob.data(), ob.size());

		// TODO : �������ģ������Ժ�����������Ϣ����ȥ���ĳ�buff����������Ҳ�ͼ�ʮ��ID
		readClanPlusData(pClanData->dwClanID);
		/*
        // ���һ���Ƿ��и����˵ĵ�½�������ݣ��еĻ��������ݿ�����
        // ���������Ѿ��ڻ����б����ˣ����ô�����
        SClanUserLogin* pLoginUser = findLoginMember(pClanData->dwClanID);
        if (pLoginUser != NULL && pLoginUser->bIsRequest == false)
        {
            readClanPlusData(pClanData->dwClanID);
            pLoginUser->bIsRequest = true;
        }
		*/

        offset += sizeof(DBREQ_TABLE_CLAN);
	}

	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine != NULL)
	{
		pEventEngine->FireExecute(EVENT_CLAN_READLIST_FINISH, SOURCE_TYPE_CLAN, 0, "", 0);
	}

	CLAN_EMPHASISLN(_GT("��ȡ�����б����, ����") << pRetData->nClanCount);
}

// ��ȡ���˳�Ա�б�       -- �������ݿ�
void CClanService::onReturnReadMemberList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // ��֤���ݿⷵ�ؽ��
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnReadMemberList] ���ݿ��ȡ���˳�Ա�б�ʧ�ܣ� ����ID = ") << nQueueIndex << "pszDBRetDesc = " << pszDBRetDesc);
        return;
    }

    int nTempLen = sizeof(DBREQ_RESULT_READMEMBERLIST);
    if (nOutLen < nTempLen)
    {
        ErrorLn(_GT("[onReturnReadMemberList] ���Ȳ��� nOutLen = ") << nOutLen << _GT(" ͷ�ļ��ṹ���� = ") << nTempLen);
        return;
    }

    DBREQ_RESULT_READMEMBERLIST* pRetData = (DBREQ_RESULT_READMEMBERLIST*)pOutData;
    nTempLen = sizeof(DBREQ_RESULT_READMEMBERLIST) + pRetData->nMemberCount * sizeof(DBREQ_TABLE_CLANMEMBER);
    if (nOutLen != nTempLen)
    {
        ErrorLn(_GT("[onReturnReadMemberList] ���Ȳ��� nOutLen = ") << nOutLen << _GT(" ͷ�ļ��ṹ���� = ") << nTempLen);
        return;
    }

    DWORD dwClanID = (DWORD)nQueueIndex;

    CClan* pClan = findClan(dwClanID);
    if (pClan == NULL)
    {
        WarningLn(_GT("[onReturnReadMemberList] ����ʱ���Ҳ������˶���"));
        return;
    }

    // ����ѳ�Ա��ӵ�m_ClanMemberList��
    if (pClan->setDBMemberContext(pOutData, nOutLen) == false)
    {
        WarningLn(_GT("[onReturnReadMemberList] �������˳�Ա�б�ʧ��"));
    }

    // ɾ�������½����
    int nReadFlag = pClan->getNumProp(emClanProp_ReadFlag);
    if (nReadFlag == emClanRead_AllData && isReadingKinEmpty(dwClanID) == true)
    {
        // EmphasisLn(_GT("CClanService::onReturnReadMemberList ��ȡ����������ɣ�����ID=") << dwClanID);
        onReadDBFinish(dwClanID);
    }
}

// ����/��ӳ�Ա��Ϣ       				-- �������ݿ�
void CClanService::onReturnUpdateMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // ��֤���ݿⷵ�ؽ��
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnUpdateMember] ���ݿ����/��ӳ�Ա��Ϣʧ�ܣ� ���ID = ") << nQueueIndex << "pszDBRetDesc��" << pszDBRetDesc);
        return;
    }
}

// ɾ����Ա��Ϣ			  				-- �������ݿ�
void CClanService::onReturnRemoveMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // ��֤���ݿⷵ�ؽ��
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnRemoveMember] ���ݿ�ɾ����Ϣʧ�ܣ� ���ID = ") << nQueueIndex << "pszDBRetDesc��" << pszDBRetDesc);
        return;
    }
}

// �������˻�������			  			-- �������ݿ�
void CClanService::onReturnUpdateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // ��֤���ݿⷵ�ؽ��
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnUpdateClan] �������˻������ݣ� ����ID = ") << nQueueIndex << "pszDBRetDesc��" << pszDBRetDesc);
        return;
    }
}

// ��ɢ����			  					-- �������ݿ�
void CClanService::onReturnDismissClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // ��֤���ݿⷵ�ؽ��
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnDismissClan] ��ɢ���ˣ� ����ID = ") << nQueueIndex << "pszDBRetDesc��" << pszDBRetDesc);
        return;
    }
}

// ���˷�����ģ����Ϣ��
void CClanService::onModuleHandleMessage(DWORD dwServerID, DWORD dwMsgID, LPCSTR pszData, size_t nLen)
{
	// ����������������Ϣ
	switch ( dwMsgID )
	{
	case MSG_CLAN_CREATE:
		{
			onSceneCreateClan(dwServerID, pszData, nLen);
		}
		break;

	default:
		{
			// ������������Ϣ
			ErrorLn(__FUNCTION__": unknown dwMsgID= " << dwMsgID);
		}
		break;
	}
}

// ����������Ϣ����					-- ���Գ�����
void CClanService::onSceneCreateClan(DWORD dwServerID, LPCSTR pszData, size_t nLen)
{
	if (pszData == NULL || nLen != sizeof(msg_clan_create_so))
	{
		return;
	}
	msg_clan_create_so *pRecvData = (msg_clan_create_so*)pszData;

	// �Ѹ���Ҵ������б���ɾ��
	SClanApply *pApply = g_ClanInviteManager.findApply(pRecvData->dwKinID);
	if (pApply != NULL)
	{
		CClan *pClan = findClan(pApply->dwClanID);
		g_ClanInviteManager.removeApply(pClan, pRecvData->dwKinID);
	}

	// �������˶��󣨴�ʱ���ݿ��Ѿ������ˣ�����Ҳ��ӵ������ˣ�
	CClan *pClan = new CClan();
	if (pClan->create(this, pszData, (int)nLen) == false)
	{
		WarningLn(__FUNCTION__"�������˶���ʧ�ܣ���������" << pRecvData->szName);
		return;
	}

	// �����˶�����ӵ��б�
	m_ClanList[pRecvData->dwClanID] = pClan;
	m_ClanArray.push_front(pClan);
	m_NameClanList[pRecvData->szName] = pClan;

	// ��ӵ�ȫ�ֳ�Ա�б�
	addClanMember(pRecvData->dwClanID, pRecvData->dwKinID, true);

	// �������������������
	pClan->updateDataToZone();

	// �㲥���ݵ��ͻ���
	pClan->broadcastDataToClient();

	// ֪ͨ���˴����ɹ�
	g_EHelper.broadcastClanSystemTips(pRecvData->dwClanID, CHAT_TIP_CLAN_CREATE_SUCCEED, pRecvData->szName);

	// �����ͻ���
	obuf ob;
	fillClanData(ob, MSG_ENDPOINT_CLIENT, MSG_CLAN_CREATE_SUCCEED);
	msg_clan_oncreatesucceed_oc createData;
	createData.bSucceed = true;
	ob.push_back(&createData, sizeof(createData));
	g_EHelper.sendDataToPerson(pRecvData->dwPDBID, ob.data(), ob.size());

	// �������˻�������
	pClan->updateClientClanBaseData(pRecvData->dwPDBID);

    ostrbuf logBuf;
    logBuf.reserve(512);
    logBuf << a2utf8("��������:��ɫID:") << pRecvData->dwPDBID << a2utf8(",��ɫ��:") << pRecvData->szShaikhName << a2utf8(",������������:") << pRecvData->szName << a2utf8(",����ID:") << pRecvData->dwClanID << a2utf8(",��������ս��ID:") << pRecvData->dwKinID;
    g_EHelper.addClanGameNoteLog(pRecvData->dwClanID, logBuf.c_str());

	WarningLn(_GT("�������˳ɹ���szName=") << pRecvData->szName << ",dwClanID=" << pRecvData->dwClanID);
}

// ���˿ͻ���ģ����Ϣ��
void CClanService::onModuleClientMsg(DWORD dwClientID, DWORD dwMsgID, LPCSTR pszData, size_t nLen)
{
	// �ڹ����ȡһ����Ϣ
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(dwClientID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	// �ͻ��˷���������Ϣ
	switch ( dwMsgID )
	{
	case MSG_CLAN_CREATE:
		{
			onClientCreateClan(shareProperty, pszData, nLen);
		}
		break;
		// ���������б�
	case MSG_CLAN_LIST:
		{
			onClientClanList(shareProperty, pszData, nLen);
		}
		break;
		// �����������
	case MSG_CLAN_INVITE:
		{
			onClientClanInvite(shareProperty, pszData, nLen);
		}
		break;
		// �����
	case MSG_CLAN_ANSWER_INVITE:
		{
			onClientAnswerInvite(shareProperty, pszData, nLen);
		}
		break;
	default:
		{
			// ������Ϣ
			ErrorLn(__FUNCTION__": unknown dwMsgID= " << dwMsgID);
		}
		break;
	}
}

// ����������Ϣ����					-- ���Կͻ���
void CClanService::onClientCreateClan(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	// �����ǰ�����Ѿ�������x�����Ͳ����ٸ��㴴��������
	if (m_ClanList.size() >= CLAN_MAX_COUNT)
		return;

	if (pszData == NULL || nLen != sizeof(msg_clan_create_co))
		return;

	msg_clan_create_co* pRecvData = (msg_clan_create_co*)pszData;

	// ���PDBID
	DWORD dwPDBID = shareProperty.dwPDBID;

	// �������ս�ӵ��峤�����ܴ�������
	SKinMember* pKinMember = g_EHelper.getActorKinMemberInfo(dwPDBID);
	if (pKinMember == NULL)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_NO_KIN);
		return;
	}
    SKinInfo sKin = pKinService->getKinInfo(pKinMember->dwKinID);
	if (sKin.nKinProp_ID == NULL || sKin.nKinProp_State == emKinState_Informal)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_NO_KIN);
		return;
	}

	if (pKinMember == NULL || pKinMember->nIdentity != emKinIdentity_Shaikh)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_NO_KIN_SHAIKH);
		return;
	}

	// ս���Ѿ�����ĳ������
	DWORD dwClanID = findKinClan(pKinMember->dwKinID);
	if (dwClanID != 0)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_HAVE_CLAN);
		return;
	}

	// �����������ʹ���������Ϣ
	obuf512 obuf;
	msg_clan_create_os sendData;
	sendData.dwPDBID = dwPDBID;
	sendData.dwKinID = pKinMember->dwKinID;
	sstrcpyn(sendData.szName, pRecvData->szName, sizeof(sendData.szName));
	sstrcpyn(sendData.szManifesto, pRecvData->szManifesto, sizeof(sendData.szManifesto));
	sendData.nClanFlag = pRecvData->nClanFlag;

	fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_CREATE);
	obuf.push_back(&sendData, sizeof(sendData));

	g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
}

// ���������б�							-- ���Կͻ���
void CClanService::onClientClanList(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(msg_clan_clanlist_co))
		return;

	DWORD dwPDBID = shareProperty.dwPDBID;
	// ȡ��ս��ID
	int nKinID = shareProperty.dwKinID;
	SClanApply* pClanApply = g_ClanInviteManager.findApply(nKinID);
	msg_clan_clanlist_co* pRecvData = (msg_clan_clanlist_co*)pszData;

    // ����ҳ������
    if (pRecvData->nPageNum <= 0)
    {
        return;
    }

	m_ClanArray.sort();
	
    // ��ʱ�����б�
    list<CClan*> tempClanArray;
    int nTotalClanCount = 0;
    int nClanCount = 0;
	// ��������
	if (strlen(pRecvData->szClanName) > 0)
	{
        list<CClan*>::iterator it = m_ClanArray.begin();
        while (it !=  m_ClanArray.end())
        {
            // �ð���ƥ��
            if (strstr((*it)->getName(), pRecvData->szClanName) != NULL)
            {
                // ������ʱ�����б�
                tempClanArray.push_back(*it);
            }

            ++it;
        }

		if (tempClanArray.empty())
		{
			g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_FIND_NO,pRecvData->szClanName);
			return;
		}

        nTotalClanCount = (int)tempClanArray.size();
        nClanCount = nTotalClanCount - (pRecvData->nPageNum-1) * CLAN_LIST_PAGE_SIZE;
        if (nClanCount <= 0)
        {
            nClanCount = 0;
        }

        int nIndex = 0;
        it = tempClanArray.begin();
        while (it != tempClanArray.end())
        {
            // �Ȱѱ�ҳǰ����޳�
            if (nIndex < (pRecvData->nPageNum-1) * CLAN_LIST_PAGE_SIZE)
            {
                it = tempClanArray.erase(it);
                ++nIndex;
                continue;
            }

            ++it;
            ++nIndex;
        }
	}
	// ��ҳ����
	else
	{
        int nIndex = 0;
        list<CClan*>::iterator it = m_ClanArray.begin();
		while (it != m_ClanArray.end())
		{
			// �Ȱ�ǰ����ʾ���޳�
			if (nIndex < (pRecvData->nPageNum-1) * CLAN_LIST_PAGE_SIZE)
            {
                ++it;
                ++nIndex;
				continue;
            }

            // ������ʱ�����б�
            tempClanArray.push_back(*it);

			++it;
			++nIndex;
		}

        nTotalClanCount = (int)m_ClanArray.size();
        nClanCount = nTotalClanCount - (pRecvData->nPageNum-1) * CLAN_LIST_PAGE_SIZE;
        if (nClanCount <= 0)
        {
            nClanCount = 0;
        }
	}

    obuf2048 ob;
    fillClanData(ob, MSG_ENDPOINT_CLIENT, MSG_CLAN_LIST);

    msg_clan_clanlist_oc sendData;
    sendData.nPageNum = pRecvData->nPageNum;
    sendData.nTotalClanCount = nTotalClanCount;
    sendData.nClanCount = nClanCount >= CLAN_LIST_PAGE_SIZE ? CLAN_LIST_PAGE_SIZE : nClanCount;
    sendData.nApplyClanID = (pClanApply != NULL) ? (int)pClanApply->dwClanID : 0;
    ob.push_back(&sendData, sizeof(sendData));

	// ��ʼ����
    int nSendCount = 0;
	msg_clan_clanlist_node_oc node;
    list<CClan*>::iterator it = tempClanArray.begin();
	while (it != tempClanArray.end())
	{
		if (nSendCount >= CLAN_LIST_PAGE_SIZE)
			break;

		// ���ÿһ����¼������
		node.dwID = (*it)->getNumProp(emClanProp_ID);
		sstrcpyn(node.szName, (*it)->getName(), sizeof(node.szName));
		sstrcpyn(node.szShaikhName, (*it)->getShaikhName(), sizeof(node.szShaikhName));
		node.nFlag = (*it)->getNumProp(emClanProp_Flag);
		node.nLevel = (*it)->getNumProp(emClanProp_Level);
		node.nKinCount = (*it)->getKinCount();
		sstrcpyn(node.szManifesto, (*it)->getManifesto(), sizeof(node.szManifesto));
		node.nMemberCount = (*it)->getMemberCount();
		node.nState = CLAN_LIST_STATUS_NORMAL;
		if ((*it)->getNumProp(emClanProp_State) == emClanState_Dismissing)
		{
			node.nState = CLAN_LIST_STATUS_DISMISSING;
		}
		else if (node.dwID == sendData.nApplyClanID)
		{
			node.nState = CLAN_LIST_STATUS_REQUESTING;
		}
		else
		{
			SClanLevel* pLevelInfo = g_EHelper.getLevelInfo(node.nLevel);
			if (pLevelInfo != NULL && node.nKinCount >= pLevelInfo->nMaxKinCount)
			{
				node.nState = CLAN_LIST_STATUS_FULL;
			}
		}
        node.nClanTotalFight = (*it)->getTotalFightScore();
		node.nWeekActivity = (*it)->getNumProp(emClanProp_WeekActivity);
		ob.push_back(&node, sizeof(node));

		++it;
        ++nSendCount;
	}

	if (nClanCount == 0 && tempClanArray.empty())
	{
		if (strlen(pRecvData->szClanName) > 0)
		{
			g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_FIND_EXIST);
		}
		else if (pRecvData->nPageNum > 1)
		{
			g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_FIND_END);
		}
	}

	// �����ͻ���
	g_EHelper.sendDataToPerson(shareProperty.dwPDBID, ob.data(), ob.size());
}

void CClanService::onClientClanInvite(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (shareProperty.dwClanID == 0)
	{
		return;
	}

	CClan* pClan = findClan(shareProperty.dwClanID);
	if (!pClan)
		return;

	// ������������
	if (pClan->getNumProp(TClanProp::emClanProp_ShaikhID) != shareProperty.dwPDBID)
	{
		return;
	}
	
	// �����ڽ�ɢ״̬����
	if (pClan->getNumProp(TClanProp::emClanProp_State) == EMClanState::emClanState_Dismissing)
	{
		return;
	}

	if (pszData == NULL || nLen != sizeof(msg_clan_invite_co))
	{
		return;
	}

	msg_clan_invite_co* pRecvData = (msg_clan_invite_co*)pszData;
	SSharePersonProperty inviteeShareProperty = pShareReceiver->GetSharePerson(pRecvData->dwInviteePDBID);
	// �������߲����ߣ�����
	if (inviteeShareProperty.dwPDBID == INVALID_PDBID)
		return;

	// ��������û��ս�ӣ�����
	if (inviteeShareProperty.dwKinID == 0)
	{
		return;
	}

	// �������������ˣ�����
	if (inviteeShareProperty.dwClanID != 0)
	{
		return;
	}

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (!pKinService)
		return;
	
	SKinInfo inviteeKinInfo = pKinService->getKinInfo(inviteeShareProperty.dwKinID);
	if (inviteeKinInfo.nKinProp_ID != inviteeShareProperty.dwKinID)
		return;

	// ��ȡս�Ӷӳ���PDBID
	DWORD inviteeKinShaikhID = inviteeKinInfo.nKinProp_ShaikhID;
	if (inviteeKinShaikhID != inviteeShareProperty.dwPDBID)
	{
		SSharePersonProperty sharePerson = pShareReceiver->GetSharePerson(inviteeKinShaikhID);
		// �ӳ������ߣ�����Ʈ����ʾ
		if (sharePerson.dwPDBID == INVALID_PDBID)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_CLAN_INVITE_KIN_SHAIKH_OFFLINE, "");
			return;
		}	
	}

	// ��ս�Ӷӳ�����������Ϣ
	msg_clan_invite_oc sendData;
	sendData.nInviterPDBID = shareProperty.dwPDBID;
	sstrcpyn(sendData.szInviterName, shareProperty.szName, sizeof(sendData.szInviterName));
	sstrcpyn(sendData.szClanName, getClanName(shareProperty.dwClanID).c_str(), sizeof(sendData.szClanName));

	obuf256 obuf;
	fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_INVITE);
	obuf.push_back(&sendData, sizeof(sendData));
	g_EHelper.sendDataToPerson(inviteeKinShaikhID, obuf.data(), obuf.size());

	// ֪ͨ����������ɹ�
	g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_KIN_INVITE_HAVE_SEND, "");
}

void CClanService::onClientAnswerInvite(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	// ���������Ѿ��������ˣ�����
	if (shareProperty.dwClanID != 0)
	{
		return;
	}

	// �������߻�û��ս�ӣ�����
	if (shareProperty.dwKinID == 0)
	{
		return;
	}

	if (pszData == NULL || nLen != sizeof(msg_clan_answer_invite_co))
	{
		return;
	}

	msg_clan_answer_invite_co* pRecvData = (msg_clan_answer_invite_co*)pszData;
	if (!pRecvData->bIsAgree)
	{
		return;
	}

	SSharePersonProperty inviterShareProperty = pShareReceiver->GetSharePerson(pRecvData->dwInviterPDBID);
	// �����߲����ߣ�����
	if (inviterShareProperty.dwPDBID == INVALID_PDBID)
		return;

	// ������û��ս�ӣ�����
	if (inviterShareProperty.dwKinID == 0)
	{
		return;
	}

	// ������û�����ˣ�����
	if (inviterShareProperty.dwClanID == 0)
	{
		return;
	}

	CClan* pClan = findClan(inviterShareProperty.dwClanID);
	if (!pClan)
	{
		return;
	}
	
	// �����߲�������������
	if (pClan->getNumProp(emClanProp_ShaikhID) != inviterShareProperty.dwPDBID)
	{
		return;
	}

	// �����������ڽ�ɢ״̬������
	if (pClan->getNumProp(emClanProp_State) == EMClanState::emClanState_Dismissing)
	{
		return;
	}

	// �ж��б��Ƿ��Ѿ�����
	SClanLevel* pLevelInfo = g_EHelper.getLevelInfo(pClan->getNumProp(emClanProp_Level));
	if (pLevelInfo == NULL)
		return;

	// ���������˳�Ա��������
	if (pClan->getKinCount() >= pLevelInfo->nMaxKinCount)
		return;

	// �Ѹ���Ҵ������б���ɾ��
	SClanApply *pApply = g_ClanInviteManager.findApply(shareProperty.dwKinID);
	if (pApply != NULL)
	{
		CClan *pClan = findClan(pApply->dwClanID);
		g_ClanInviteManager.removeApply(pClan, shareProperty.dwKinID);
	}

	pClan->acceptMember(shareProperty.dwKinID);

	// �ظ������ߣ��������ɹ�
	obuf256 obuf;
	fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_ANSWER_INVITE);
	g_EHelper.sendDataToPerson(pRecvData->dwInviterPDBID, obuf.data(), obuf.size());
}


// ��ȡ�����б���Ϣ
bool CClanService::readClanList()
{
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService == NULL)
    {
        return false;
	}

    if (pDBEngineService->executeSP(GAMEDB_REQUEST_READCLANLIST, 0, "", 0, this) == false)
    {
        ErrorLn(__FUNCTION__" ����ʧ��");
        return false;
    }

	CLAN_EMPHASISLN(_GT("��ʼ��ȡ�����б�"));
    return true;
}

// ��ȡ���˸������ݣ���Ա�б�
void CClanService::readClanPlusData(DWORD dwClanID)
{
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService == NULL)
    {
        return;
    }

    // ��ȡ���˳�Ա�б�����
    DBREQ_PARAM_READMEMBERLIST readMemberList;
    readMemberList.dwClanID = dwClanID;
    if (pDBEngineService->executeSP(GAMEDB_REQUEST_READMEMBERLIST, dwClanID, (LPCSTR)(&readMemberList), sizeof(readMemberList), this) == false)
    {
        ErrorLn(_GT("GAMEDB_REQUEST_READMEMBERLIST ����ʧ��"));
    }
}

// ֪ͨ���������Ľ�ɫ���ϵ�����ID
void CClanService::setActorClanID(DWORD dwPDBID, DWORD dwClanID, DWORD dwServerID)
{
	obuf obuf;
	msg_clan_setclanid_os sendData;
	sendData.dwPDBID = dwPDBID;
	sendData.dwClanID = dwClanID;
	fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATEID);
	obuf.push_back(&sendData, sizeof(sendData));
	g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), dwServerID);
}

// �޸��������˱�������
void CClanService::UpdateAllClanLegendCupProp()
{
	TMap_ClanIt it = m_ClanList.begin();
	for (; it != m_ClanList.end(); ++it)
	{
		CClan* pClan = ((*it).second);
		if (pClan->setNumProp(emClanProp_LegendCupCount, 1, true, true))
		{
			// ���¿ͻ���
			msg_clan_updateintprop_oc sendData;
			sendData.nPropID = emClanProp_LegendCupCount;
			sendData.nValue = 1;
			pClan->broadcastMsgToObserve(MSG_CLAN_UPDATECLANINTPROP, (LPCSTR)&sendData, sizeof(sendData));
		}
	}
}

// �����ܻ�Ծ��
void CClanService::ResetWeekActivity()
{
	for (TMap_ClanIt it = m_ClanList.begin(); it != m_ClanList.end(); ++it)
	{
		CClan* pClan = ((*it).second);
		if (pClan)
			pClan->ResetWeekActivity();
	}
}

void CClanService::AutoSetShaikhState()
{
    for (TMap_ClanIt itClan = m_ClanList.begin(); itClan != m_ClanList.end(); ++itClan)
    {
        CClan *pClan = (*itClan).second;
        if (pClan && pClan->getKinCount() > 1)
        {
            pClan->CheckShaikhState();
        }
    }
}

void CClanService::updateClanLegendDida()
{
    for (TMap_ClanIt itClan = m_ClanList.begin(); itClan != m_ClanList.end(); ++itClan)
    {
        CClan *pClan = (*itClan).second;
        if (pClan != NULL)
        {
            pClan->resetClanLegendDidaSend();
        }
    }
}

void CClanService::refreshCreateLegendCupCount()
{
	for (TMap_ClanIt itClan = m_ClanList.begin(); itClan != m_ClanList.end(); ++itClan)
	{
		CClan *pClan = (*itClan).second;
		if (pClan != NULL)
		{
			pClan->refreshCreateLegendCupCount();
		}
	}
}
