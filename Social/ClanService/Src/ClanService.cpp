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
    // 数据库接口注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService == NULL)
		return false;
	pDBEngineService->registerHandler(this);

    IMessageDispatch *pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if (pMessageDispatch == NULL)
        return false;
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_CLAN,this);
    pMessageDispatch->registerMessageHandler(MSG_MODULEID_CLAN, this);

    // 共享信息注册
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
		return false;
	pShareReceiver->RegisterHandler(this, __FUNCTION__);

	// 战队事件注册
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return false;
	pEventEngine->Subscibe(this, EVENT_KIN_UPDATEID, SOURCE_TYPE_KIN, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_KIN_READ_DB_FINISHED, SOURCE_TYPE_KIN, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_KIN_CHANGE_SHAIKH, SOURCE_TYPE_KIN, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_KIN_DISMISS, SOURCE_TYPE_KIN, 0, __FUNCTION__);

    // 启动服务器
    g_EHelper.setTimer(TimerID_Clan_Clear_User_Login_List, 30*60*1000, this, "CClanService::create");

	IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
    if (pDateTriggerService != NULL)
    {
		// 设置每周日23:59清一次活跃度
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

		// 每天检测
        sTriggerInfo.year = -1;
        sTriggerInfo.month = -1;
        sTriggerInfo.day = -1;
        sTriggerInfo.hour = 0;
        sTriggerInfo.min = -1;
        sTriggerInfo.wday = -1;
        pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_LegendCupDidaUpdate, this, sTriggerInfo);


		// 每月1号检测
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
		|| (localNowTime.tm_wday == 1 && localNowTime.tm_hour <= 1)) // 若在周日23：00 - 周一1：00 之间，则周活跃度清零
	{
		ResetWeekActivity();
	}

    return true;
}

void CClanService::release()
{
    // 数据库接口反注册
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

    // 共享信息注销
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->UnregisterHandler(this);
	}

	// 战队事件注销
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine != NULL)
	{
		pEventEngine->UnSubscibe(this, EVENT_KIN_UPDATEID, SOURCE_TYPE_KIN, 0);
		pEventEngine->UnSubscibe(this, EVENT_KIN_READ_DB_FINISHED, SOURCE_TYPE_KIN, 0);
		pEventEngine->UnSubscibe(this, EVENT_KIN_CHANGE_SHAIKH, SOURCE_TYPE_KIN, 0);
		pEventEngine->UnSubscibe(this, EVENT_KIN_DISMISS, SOURCE_TYPE_KIN, 0);
		// 订阅 中心连接器服务已启动
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

// 获得联盟战队
DWORD CClanService::findKinClan(DWORD dwKinID)
{
	TMap_KinClanIt it = m_KinClanList.find(dwKinID);
	if (it == m_KinClanList.end())
	{
		return 0;
	}

	return it->second;
}

// 移除正在读取的战队
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

	// 删除缓存登陆数据
	int nReadFlag = pClan->getNumProp(emClanProp_ReadFlag);
	if (nReadFlag == emClanRead_AllData && isReadingKinEmpty(dwClanID) == true)
	{
		// TraceLn(_GT("CClanService::removeReadingKin 读取联盟数据完成，联盟ID=") << dwClanID);
		onReadDBFinish(dwClanID);
	}
}

// 获取联盟成员列表
int CClanService::getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen)
{
	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return 0;

	return pClan->getKinList(pKinList, nLen);
}

// 取得联盟数值
int CClanService::getClanNumProp(DWORD dwClanID, int nNumIndex)
{
    CClan* pClan = findClan(dwClanID);
    if (pClan == NULL)
        return 0;

    return pClan->getNumProp(nNumIndex);
}

// 获取联盟名称
string CClanService::getClanName(DWORD dwClanID)
{
	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return "";
	return pClan->getName();
}

// 联盟杯赛创建
void CClanService::notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName)
{
	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return;

	pClan->broadcastClanCreateLegendCupToClient(llLegendCupID, nMaxKinCount, pCupName);

    pClan->recCreateLegendCupEvent(pCupName);
    
    
}

////////////////////////////////ITransmitHandler//////////////////////////////////////////
// 服务器之间消息处理
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
// 客户端消息处理
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
    // 服务器转发过来的消息
    if (data == NULL || len<sizeof(msg_clan_sub_head))
    {
        ErrorLn(__FUNCTION__": message data is null or length is invalid! len="<< len <<", sizeof="<< sizeof(msg_clan_sub_head) );
        return;
    }
    msg_clan_sub_head *pMsg = (msg_clan_sub_head*)(data);

    LPSTR pMsgData = (LPSTR)data + sizeof(msg_clan_sub_head);
    size_t nNewlen = len - sizeof(msg_clan_sub_head);

    // 服务器发过来的消息
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
            // 其他场景服消息
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

    // 客户端发过来的消息
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
            // 其他消息
            ErrorLn(__FUNCTION__": unknown byKeyAction= " << head.byKeyAction);
        }
        break;
    }
}

// 通知服务器列表已经更新
void CClanService::onServerListUpdated()
{

}

// 通知服务器信息更新
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
    case GAMEDB_REQUEST_READMEMBERLIST:     // 读取联盟成员列表
        {
            onReturnReadMemberList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;        
    case GAMEDB_REQUEST_UPDATEMEMBER:       // 更新成员（没有则添加）
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
				pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_ClanUpdate, this, sTriggerInfo);	// 订阅时刻定时器（每月第1天00：00）
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
				// 设置角色身上的联盟ID
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

                    // 信息更新到客户端
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
				// 读取联盟列表信息
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
            // 删除定时器
            g_EHelper.killTimer(TimerID_Clan_Clear_User_Login_List, this);

            // 清除所有联盟登陆换成列表
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

    // 如果成员数据读出，并且战队数据也读取完成
    DWORD dwKinClanID = findKinClan(dwKinID);
    if (dwKinClanID > 0 && isReadingKinEmpty(dwClanID) == true)
    {
        // 如果联盟数据已经读出来了，直接通知成员上线
        onMemberLogin(dwClanID, dwPDBID);
        return;
    }

    // 如果该玩家联盟已经在缓存列表列了，则不用处理了
    SClanUserLogin* pLoginUser = findLoginMember(dwClanID);
    if (pLoginUser != NULL)
        return;

    SClanUserLogin userLogin;
    CClan* pClan = findClan(dwClanID);
    // 如果联盟数据已经读取完毕，这里返回
    if (pClan != NULL && pClan->isReadFinish() == true)
        return;

    // 如果联盟对象已经创建了，请求数据库读取附加数据
    if (pClan != NULL)
    {
        // 读取联盟的附加数据
        readClanPlusData(dwClanID);
        userLogin.bIsRequest = true;
    }
    else
    {
        userLogin.bIsRequest = false;
    }

    // 添加到缓存列表
    userLogin.dwClanID = dwClanID;
    userLogin.userList.push_back(dwPDBID);
    m_ClanUserLoginList[dwClanID] = userLogin;
}

void CClanService::OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType)
{
    // 玩家登出
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

	// 属性更新
	switch(nUpdateReason)
	{
	case SHAREUPDATE_REASON_CHANGE_ACTORNAME:
		{
			// 广播给客户端更新玩家名字
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
// 查找联盟
CClan* CClanService::findClan(DWORD dwClanID)
{
    TMap_ClanIt it = m_ClanList.find(dwClanID);
    if (it == m_ClanList.end())
        return NULL;

    return (*it).second;
}

// 查找联盟
CClan* CClanService::findClanByName(string szClanName)
{
    // 通过联盟名字查找联盟
    TMap_NameClanIt it = m_NameClanList.find(szClanName);
    if (it == m_NameClanList.end())
        return NULL;

    return (*it).second;
}

// 获得联盟战队
DWORD CClanService::findKinClanByPDBID(DWORD dwPDBID)
{
    SKinMember* pKinMember = g_EHelper.getActorKinMemberInfo(dwPDBID);
    if (pKinMember == NULL)
        return NULL;

    return findKinClan(pKinMember->dwKinID);
}

// 移除联盟
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

	// 从联盟列表中查找删除
	for (list<CClan*>::iterator itList= m_ClanArray.begin(); itList != m_ClanArray.end(); )
	{
		if ((*itList)->getNumProp(emClanProp_ID) == dwClanID)
			itList = m_ClanArray.erase(itList);
		else
			itList++;
	}
}

// 添加联盟成员（有则更新）
void CClanService::addClanMember(DWORD dwClanID, DWORD dwKinID, bool bNeedSaveDB)
{
    if (dwClanID == 0)
        return;

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
        return;

    // 添加到全局成员列表
    m_KinClanList[dwKinID] = dwClanID;

    // 把该战队的所有成员的联盟ID设置成当前联盟
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

		// 更新联盟ID到场景服
		setActorClanID(dwPDBID, dwClanID, shareProperty.serverID);

        // 同步到客户端
        updateMemberToClient_OC(dwClanID, &pKinMember);
    }

    // 存储数据库
    if (bNeedSaveDB == true)
    {
        updateMemberToDB(dwClanID, dwKinID);

		// 发送联盟ID改变事件
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

    // 同步到场景服
    updateMemberToZone(dwClanID, dwKinID);
}

// 删除联盟成员
void CClanService::removeClanMember(DWORD dwKinID, bool bNeedSaveDB)
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
        return;

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
        return;

    // 不存在该成员
	DWORD dwClanID = findKinClan(dwKinID);
    if (dwClanID == 0)
        return;

    SKinInfo sKin = pKinService->getKinInfo(dwKinID);
    if (sKin.nKinProp_ID == 0)
        return;

    CClan* pClan = findClan(dwClanID);
    if (pClan == NULL)
    {
        // 联盟成员列表移除
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

        // 通知客户端清空成员数据
        {
            obuf256 ob;
            fillClanData(ob, MSG_ENDPOINT_CLIENT, MSG_CLAN_QUIT);
            g_EHelper.sendDataToPerson(dwPDBID, ob.data(), ob.size());
        }

        // 更新联盟ID到场景服
		setActorClanID(dwPDBID, 0, shareProperty.serverID);
    }

    {
        // 社会服向场景服删除成员
        obuf ob;
        msg_clan_removekin_os removeMember;
        removeMember.dwKinID = dwKinID;
        removeMember.dwClanID = dwClanID;

        fillClanData(ob, MSG_ENDPOINT_SCENE, MSG_CLAN_REMOVEMEMBER_ZONE);
        ob.push_back(&removeMember, sizeof(removeMember));
        g_EHelper.sendDataToZoneSvr(ob.data(), ob.size());
    }

	{
		// 发送联盟ID改变事件
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
        // 存储数据库
        if (bNeedSaveDB)
        {
            // 通知数据库存盘成员数据
            DBREQ_PARAM_DELETEMEMBER referData;
            referData.dwKinID = dwKinID;
            if (pDBEngine->executeSP(GAMEDB_REQUEST_DELETEMEMBER, referData.dwKinID, (LPCSTR)(&referData), sizeof(referData), this) == false)
            {
                ErrorLn(_GT("GAMEDB_REQUEST_DELETEMEMBER 请求失败"));
            }
        }
    }

    // 联盟成员列表移除
    m_KinClanList.erase(dwKinID);
}

// 更新成员数据到数据库
void CClanService::updateMemberToDB(DWORD dwClanID, DWORD dwKinID)
{
	IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
	if (dwClanID == 0 || pDBEngineService == NULL)
		return;

	// 通知数据库存盘成员数据
	DBREQ_PARAM_UPDATEMEMBER memberData;
	memberData.dwClanID =  dwClanID;
	memberData.dwKinID = dwKinID;
	if (pDBEngineService->executeSP(GAMEDB_REQUEST_UPDATEMEMBER, memberData.dwKinID, (LPCSTR)(&memberData), sizeof(memberData), this) == false)
	{
		ErrorLn(_GT("GAMEDB_REQUEST_UPDATEMEMBER 请求失败"));
	}
}

// 更新全部联盟数据到场景服
bool CClanService::refreshClanInfoToZone(CSID dwServerID)
{
	// 通知该场景服删除原有联盟数据
	{
		obuf512 obuf;
		msg_clan_removeclan_os sendData;
		sendData.dwClanID = 0;
		fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_REMOVECLAN_ZONE);
		obuf.push_back(&sendData, sizeof(sendData));
		g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), dwServerID);
	}

	// 把联盟的所有数据发到场景服
	TMap_ClanIt itClan;
	for (itClan = m_ClanList.begin(); itClan != m_ClanList.end(); ++ itClan)
	{
		CClan *pClan = (*itClan).second;
		pClan->updateDataToZone(dwServerID);
	}

	return true;
}

// 更新成员数据到场景服
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

// 更新成员数据到客户端
void CClanService::updateMemberToClient_OZC(DWORD dwClanID, SKinMember* pKinMember)
{
	if (pKinMember == NULL)
		return;

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	// 在共享服取一下信息
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

// 更新成员数据到客户端
void CClanService::updateMemberToClient_OC(DWORD dwClanID, SKinMember* pKinMember)
{
	if (pKinMember == NULL)
		return;

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	// 在共享服取一下信息
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

// 成员上线（社会服和场景服重连也会通知）
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

	// 如果该联盟不是非正式联盟，通知客户端刷新头顶联盟名字
	CClan* pClan = findClan(dwClanID);
	if (pClan == NULL)
		return;

	// 把联盟的基本信息发送到客户端
	pClan->updateDataToClient(dwPDBID, shareProperty.serverID);

	// 把上线的信息广播到观察者
	SClientClanMember sMember;
	pClan->fillClientMemberInfo(sMember, sKin, &sKinMember);
	pClan->broadcastMsgToObserve(MSG_CLAN_UPDATEMEMBERINFO, (LPCSTR)&sMember, sizeof(sMember));

	// 把自己的信息也发到客户端
	updateMemberToClient_OZC(dwClanID, &sKinMember);

	pClan->onMemberLogin(shareProperty);

	// 同步该成员到场景服
	updateMemberToZone(dwKinClanID, sKinMember.dwKinID);

	// 发出成员登陆事件
	event_clan_memberlogin loginEvent;
	loginEvent.dwClanID = dwClanID;
	loginEvent.dwPDBID = dwPDBID;
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine != NULL)
	{
		pEventEngine->FireExecute(EVENT_CLAN_MEMBERLOGIN, SOURCE_TYPE_CLAN, dwPDBID, (LPCSTR)&loginEvent, sizeof(loginEvent));
	}

	// 通知联盟杯赛提示
	pClan->broadcastClanLegendCupInfoToClient(shareProperty.dwPDBID);

	// 如果该玩家是族长，且当前活跃度小于解散活跃，给他一个Dida
	int nIdentity = g_EHelper.getClanIdentity(dwPDBID, pClan->getNumProp(emClanProp_ShaikhID), sKinMember.nIdentity);
	if (nIdentity == EMCLANIDENTITY_SHAIKH)
	{
		// 同步数据
		pClan->updateClientClanBaseData(dwPDBID);

		// 判断当前活跃度是否低于解散活跃度
		if (pClan->getNumProp(emClanProp_Activity) < pConfig->nDismissActivity)
		{
			// 给他一个Dida,联盟要解散了
		}
	}
}

// 查找联盟的登陆缓存成员
SClanUserLogin* CClanService::findLoginMember(DWORD dwClanID)
{
	TMap_ClanUserLoginIt it = m_ClanUserLoginList.find(dwClanID);
	if (it == m_ClanUserLoginList.end())
		return NULL;

	return &((*it).second);
}

// 添加正在读取的战队
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

// 是否为空
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

// 联盟的全部数据读取完成
void CClanService::onReadDBFinish(DWORD dwClanID)
{
	// EmphasisLn(_GT("帮会的全部数据读取完成 帮会ID=") << dwClanID);
	TMap_ClanUserLoginIt it = m_ClanUserLoginList.find(dwClanID);
	if (it != m_ClanUserLoginList.end())
	{
		SClanUserLogin* pUserLogin = &((*it).second);
		// 通知成员上线
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

	// 设置帮会读取完成
	pClan->setReadFinish(true);
    pClan->updateClanOrderInfo();
}

// 根据周活跃度检查是否需要解散的联盟
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

// 检查是否需要解散的联盟
bool CClanService::checkDBDismiss(DBREQ_TABLE_CLAN* pClanContext)
{
	SClanConfig* pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return false;

	// 如果是解散中联盟状态
	if (pClanContext->nState == emClanState_Dismissing)
	{
		// 如果离发起解散时间差1个小时就到7天，则解散联盟
		int nCurTime = (int)time(NULL);
		int nInterval = pConfig->nDismissingLife - (nCurTime - pClanContext->nDismissApplyTime);
		if (nInterval < 60 * 60)
			return true;
	}

	return false;
}

// 提交解散联盟请求
void CClanService::onDismissClan(DWORD dwClanID)
{
	// 往数据库提交解散联盟请求
	IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
		return;

	// 往中心库数据库解散联盟族操作
	DBREQ_PARAM_CENTERDBDISMISSCLAN centerDismiss;
	centerDismiss.dwClanID = dwClanID;
	if(!pDBEngineService->executeSP(GAMEDB_REQUEST_CENTERDBDISMISSCLAN, dwClanID, (LPCSTR)&centerDismiss,
		sizeof(centerDismiss), static_cast<IDBRetSink *>(this)))
	{
		ErrorLn(_GT("GAMEDB_REQUEST_CENTERDBDISMISSCLAN 请求失败，dwClanID = ") << dwClanID);
		return;
	}

	// 向数据库提交解散联盟请求
	DBREQ_PARAM_DISMISSCLAN referData;
	referData.dwClanID = dwClanID;
	if (pDBEngineService->executeSP(GAMEDB_REQUEST_DISMISSCLAN, dwClanID, (LPCSTR)(&referData), sizeof(referData), this) == false)
	{
		ErrorLn(_GT("GAMEDB_REQUEST_DISMISSCLAN 请求失败"));
	}
}

/// 执行存储过程
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

// 读取联盟列表           -- 来自数据库
void CClanService::onReturnReadClanList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnReadClanList] 数据库读取联盟列表失败！pszDBRetDesc = ") << pszDBRetDesc);
        return;
    }

    // 检查数据返回长度
    if (nOutLen < sizeof(DBREQ_RESULT_READCLANLIST))
    {
        int tempLen = sizeof(DBREQ_RESULT_READCLANLIST);
        ErrorLn(_GT("[onReturnReadClanList] 长度不对 数据返回长度 = ") << nOutLen << " sizeof(DBREQ_RESULT_READCLANLIST) = " << tempLen);
        return;
    }

    DBREQ_RESULT_READCLANLIST* pRetData = (DBREQ_RESULT_READCLANLIST*)pOutData;
    if (nOutLen != sizeof(DBREQ_RESULT_READCLANLIST) + pRetData->nClanCount * sizeof(DBREQ_TABLE_CLAN))
    {
        int tempLen = sizeof(DBREQ_RESULT_READCLANLIST) + pRetData->nClanCount * sizeof(DBREQ_TABLE_CLAN);
        ErrorLn(_GT("[onReturnReadClanList] 长度不对 数据返回长度 = ") << nOutLen << _GT(" 头文件结构长度 = ") << tempLen);
        return;
	}

    // 创建联盟对象
    char szText[512] = {0};
    int offset = sizeof(DBREQ_RESULT_READCLANLIST);
    for (int i = 0; i < pRetData->nClanCount; i ++)
    {
        DBREQ_TABLE_CLAN* pClanData = (DBREQ_TABLE_CLAN*)(pOutData + offset);

        // 检查联盟是否需要解散了
        if (checkDBDismiss(pClanData) == true)
        {
            // 记录解散联盟日志
            //ssprintf_s(szText, sizeof(szText), "[%s]联盟解散 %s", pClanData->szName, "启动社会服发现过期联盟");
            //g_EHelper.TrackGameNote(ENoteTrackType_Clan, pClanData->dwID, szText);

            // 向数据库提交解散联盟请求
            onDismissClan(pClanData->dwClanID);
            offset += sizeof(DBREQ_TABLE_CLAN);
            continue;
        }

        // 创建联盟对象
        CClan* pClan = new CClan();
        if (pClan == NULL)
        {
            ErrorLn(_GT("DB创建联盟对象失败，pClan == NULL, 联盟名：") << pClanData->szName);
            offset += sizeof(DBREQ_TABLE_CLAN);
            continue;
        }

        if (pClan->onDBCreate(this, (LPCSTR)pClanData, sizeof(DBREQ_TABLE_CLAN)) == false)
        {
            ErrorLn(_GT("DB创建联盟对象失败，联盟名：") << pClanData->szName);
            offset += sizeof(DBREQ_TABLE_CLAN);
            continue;
        }

        // 把联盟对象添加到列表
        m_ClanList[pClanData->dwClanID] = pClan;
        m_ClanArray.push_front(pClan);
        m_NameClanList[pClanData->szName] = pClan;

        // 通知场景服添加联盟
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

		// TODO : 先这样改，考虑以后把这个附加信息请求去掉改成buff，反正内容也就几十个ID
		readClanPlusData(pClanData->dwClanID);
		/*
        // 检查一下是否有改联盟的登陆缓存数据，有的话，向数据库请求
        // 如果该玩家已经在缓存列表列了，则不用处理了
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

	CLAN_EMPHASISLN(_GT("读取联盟列表完成, 总数") << pRetData->nClanCount);
}

// 读取联盟成员列表       -- 来自数据库
void CClanService::onReturnReadMemberList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnReadMemberList] 数据库读取联盟成员列表失败！ 联盟ID = ") << nQueueIndex << "pszDBRetDesc = " << pszDBRetDesc);
        return;
    }

    int nTempLen = sizeof(DBREQ_RESULT_READMEMBERLIST);
    if (nOutLen < nTempLen)
    {
        ErrorLn(_GT("[onReturnReadMemberList] 长度不对 nOutLen = ") << nOutLen << _GT(" 头文件结构长度 = ") << nTempLen);
        return;
    }

    DBREQ_RESULT_READMEMBERLIST* pRetData = (DBREQ_RESULT_READMEMBERLIST*)pOutData;
    nTempLen = sizeof(DBREQ_RESULT_READMEMBERLIST) + pRetData->nMemberCount * sizeof(DBREQ_TABLE_CLANMEMBER);
    if (nOutLen != nTempLen)
    {
        ErrorLn(_GT("[onReturnReadMemberList] 长度不对 nOutLen = ") << nOutLen << _GT(" 头文件结构长度 = ") << nTempLen);
        return;
    }

    DWORD dwClanID = (DWORD)nQueueIndex;

    CClan* pClan = findClan(dwClanID);
    if (pClan == NULL)
    {
        WarningLn(_GT("[onReturnReadMemberList] 返回时候找不到联盟对象"));
        return;
    }

    // 里面把成员添加到m_ClanMemberList里
    if (pClan->setDBMemberContext(pOutData, nOutLen) == false)
    {
        WarningLn(_GT("[onReturnReadMemberList] 设置联盟成员列表失败"));
    }

    // 删除缓存登陆数据
    int nReadFlag = pClan->getNumProp(emClanProp_ReadFlag);
    if (nReadFlag == emClanRead_AllData && isReadingKinEmpty(dwClanID) == true)
    {
        // EmphasisLn(_GT("CClanService::onReturnReadMemberList 读取联盟数据完成，联盟ID=") << dwClanID);
        onReadDBFinish(dwClanID);
    }
}

// 更新/添加成员信息       				-- 来自数据库
void CClanService::onReturnUpdateMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnUpdateMember] 数据库更新/添加成员信息失败！ 玩家ID = ") << nQueueIndex << "pszDBRetDesc：" << pszDBRetDesc);
        return;
    }
}

// 删除成员信息			  				-- 来自数据库
void CClanService::onReturnRemoveMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnRemoveMember] 数据库删除信息失败！ 玩家ID = ") << nQueueIndex << "pszDBRetDesc：" << pszDBRetDesc);
        return;
    }
}

// 更新联盟基本数据			  			-- 来自数据库
void CClanService::onReturnUpdateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnUpdateClan] 更新联盟基本数据！ 联盟ID = ") << nQueueIndex << "pszDBRetDesc：" << pszDBRetDesc);
        return;
    }
}

// 解散联盟			  					-- 来自数据库
void CClanService::onReturnDismissClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(_GT("[onReturnDismissClan] 解散联盟！ 联盟ID = ") << nQueueIndex << "pszDBRetDesc：" << pszDBRetDesc);
        return;
    }
}

// 联盟服务器模块消息码
void CClanService::onModuleHandleMessage(DWORD dwServerID, DWORD dwMsgID, LPCSTR pszData, size_t nLen)
{
	// 服务器发过来的消息
	switch ( dwMsgID )
	{
	case MSG_CLAN_CREATE:
		{
			onSceneCreateClan(dwServerID, pszData, nLen);
		}
		break;

	default:
		{
			// 其他场景服消息
			ErrorLn(__FUNCTION__": unknown dwMsgID= " << dwMsgID);
		}
		break;
	}
}

// 创建联盟消息处理					-- 来自场景服
void CClanService::onSceneCreateClan(DWORD dwServerID, LPCSTR pszData, size_t nLen)
{
	if (pszData == NULL || nLen != sizeof(msg_clan_create_so))
	{
		return;
	}
	msg_clan_create_so *pRecvData = (msg_clan_create_so*)pszData;

	// 把该玩家从申请列表中删除
	SClanApply *pApply = g_ClanInviteManager.findApply(pRecvData->dwKinID);
	if (pApply != NULL)
	{
		CClan *pClan = findClan(pApply->dwClanID);
		g_ClanInviteManager.removeApply(pClan, pRecvData->dwKinID);
	}

	// 创建联盟对象（此时数据库已经创建了，帮主也添加到数据了）
	CClan *pClan = new CClan();
	if (pClan->create(this, pszData, (int)nLen) == false)
	{
		WarningLn(__FUNCTION__"创建联盟对象失败，联盟名：" << pRecvData->szName);
		return;
	}

	// 把联盟对象添加到列表
	m_ClanList[pRecvData->dwClanID] = pClan;
	m_ClanArray.push_front(pClan);
	m_NameClanList[pRecvData->szName] = pClan;

	// 添加到全局成员列表
	addClanMember(pRecvData->dwClanID, pRecvData->dwKinID, true);

	// 往场景服添加联盟数据
	pClan->updateDataToZone();

	// 广播数据到客户端
	pClan->broadcastDataToClient();

	// 通知联盟创建成功
	g_EHelper.broadcastClanSystemTips(pRecvData->dwClanID, CHAT_TIP_CLAN_CREATE_SUCCEED, pRecvData->szName);

	// 发往客户端
	obuf ob;
	fillClanData(ob, MSG_ENDPOINT_CLIENT, MSG_CLAN_CREATE_SUCCEED);
	msg_clan_oncreatesucceed_oc createData;
	createData.bSucceed = true;
	ob.push_back(&createData, sizeof(createData));
	g_EHelper.sendDataToPerson(pRecvData->dwPDBID, ob.data(), ob.size());

	// 更新联盟基本数据
	pClan->updateClientClanBaseData(pRecvData->dwPDBID);

    ostrbuf logBuf;
    logBuf.reserve(512);
    logBuf << a2utf8("创建联盟:角色ID:") << pRecvData->dwPDBID << a2utf8(",角色名:") << pRecvData->szShaikhName << a2utf8(",创建联盟名称:") << pRecvData->szName << a2utf8(",联盟ID:") << pRecvData->dwClanID << a2utf8(",创建联盟战队ID:") << pRecvData->dwKinID;
    g_EHelper.addClanGameNoteLog(pRecvData->dwClanID, logBuf.c_str());

	WarningLn(_GT("创建联盟成功，szName=") << pRecvData->szName << ",dwClanID=" << pRecvData->dwClanID);
}

// 联盟客户端模块消息码
void CClanService::onModuleClientMsg(DWORD dwClientID, DWORD dwMsgID, LPCSTR pszData, size_t nLen)
{
	// 在共享服取一下信息
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(dwClientID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	// 客户端发过来的消息
	switch ( dwMsgID )
	{
	case MSG_CLAN_CREATE:
		{
			onClientCreateClan(shareProperty, pszData, nLen);
		}
		break;
		// 申请联盟列表
	case MSG_CLAN_LIST:
		{
			onClientClanList(shareProperty, pszData, nLen);
		}
		break;
		// 邀请加入联盟
	case MSG_CLAN_INVITE:
		{
			onClientClanInvite(shareProperty, pszData, nLen);
		}
		break;
		// 邀请答复
	case MSG_CLAN_ANSWER_INVITE:
		{
			onClientAnswerInvite(shareProperty, pszData, nLen);
		}
		break;
	default:
		{
			// 其他消息
			ErrorLn(__FUNCTION__": unknown dwMsgID= " << dwMsgID);
		}
		break;
	}
}

// 创建联盟消息处理					-- 来自客户端
void CClanService::onClientCreateClan(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	// 如果当前联盟已经超过了x个，就不能再给你创建联盟了
	if (m_ClanList.size() >= CLAN_MAX_COUNT)
		return;

	if (pszData == NULL || nLen != sizeof(msg_clan_create_co))
		return;

	msg_clan_create_co* pRecvData = (msg_clan_create_co*)pszData;

	// 玩家PDBID
	DWORD dwPDBID = shareProperty.dwPDBID;

	// 如果不是战队的族长，不能创建联盟
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

	// 战队已经属于某个联盟
	DWORD dwClanID = findKinClan(pKinMember->dwKinID);
	if (dwClanID != 0)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_HAVE_CLAN);
		return;
	}

	// 往场景服发送创建联盟消息
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

// 申请联盟列表							-- 来自客户端
void CClanService::onClientClanList(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	if (pszData == NULL || nLen != sizeof(msg_clan_clanlist_co))
		return;

	DWORD dwPDBID = shareProperty.dwPDBID;
	// 取得战队ID
	int nKinID = shareProperty.dwKinID;
	SClanApply* pClanApply = g_ClanInviteManager.findApply(nKinID);
	msg_clan_clanlist_co* pRecvData = (msg_clan_clanlist_co*)pszData;

    // 请求页数不对
    if (pRecvData->nPageNum <= 0)
    {
        return;
    }

	m_ClanArray.sort();
	
    // 临时联盟列表
    list<CClan*> tempClanArray;
    int nTotalClanCount = 0;
    int nClanCount = 0;
	// 搜索请求
	if (strlen(pRecvData->szClanName) > 0)
	{
        list<CClan*>::iterator it = m_ClanArray.begin();
        while (it !=  m_ClanArray.end())
        {
            // 用包含匹配
            if (strstr((*it)->getName(), pRecvData->szClanName) != NULL)
            {
                // 插入临时联盟列表
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
            // 先把本页前面的剔除
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
	// 翻页请求
	else
	{
        int nIndex = 0;
        list<CClan*>::iterator it = m_ClanArray.begin();
		while (it != m_ClanArray.end())
		{
			// 先把前面显示的剔除
			if (nIndex < (pRecvData->nPageNum-1) * CLAN_LIST_PAGE_SIZE)
            {
                ++it;
                ++nIndex;
				continue;
            }

            // 插入临时联盟列表
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

	// 开始拷贝
    int nSendCount = 0;
	msg_clan_clanlist_node_oc node;
    list<CClan*>::iterator it = tempClanArray.begin();
	while (it != tempClanArray.end())
	{
		if (nSendCount >= CLAN_LIST_PAGE_SIZE)
			break;

		// 填充每一条记录的数据
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

	// 发往客户端
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

	// 不是盟主返回
	if (pClan->getNumProp(TClanProp::emClanProp_ShaikhID) != shareProperty.dwPDBID)
	{
		return;
	}
	
	// 联盟在解散状态返回
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
	// 被邀请者不在线，返回
	if (inviteeShareProperty.dwPDBID == INVALID_PDBID)
		return;

	// 被邀请者没有战队，返回
	if (inviteeShareProperty.dwKinID == 0)
	{
		return;
	}

	// 被邀请者有联盟，返回
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

	// 获取战队队长的PDBID
	DWORD inviteeKinShaikhID = inviteeKinInfo.nKinProp_ShaikhID;
	if (inviteeKinShaikhID != inviteeShareProperty.dwPDBID)
	{
		SSharePersonProperty sharePerson = pShareReceiver->GetSharePerson(inviteeKinShaikhID);
		// 队长不在线，返回飘字提示
		if (sharePerson.dwPDBID == INVALID_PDBID)
		{
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_CLAN_INVITE_KIN_SHAIKH_OFFLINE, "");
			return;
		}	
	}

	// 给战队队长发送邀请信息
	msg_clan_invite_oc sendData;
	sendData.nInviterPDBID = shareProperty.dwPDBID;
	sstrcpyn(sendData.szInviterName, shareProperty.szName, sizeof(sendData.szInviterName));
	sstrcpyn(sendData.szClanName, getClanName(shareProperty.dwClanID).c_str(), sizeof(sendData.szClanName));

	obuf256 obuf;
	fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_INVITE);
	obuf.push_back(&sendData, sizeof(sendData));
	g_EHelper.sendDataToPerson(inviteeKinShaikhID, obuf.data(), obuf.size());

	// 通知邀请者邀请成功
	g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_KIN_INVITE_HAVE_SEND, "");
}

void CClanService::onClientAnswerInvite(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen)
{
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	// 被邀请者已经有联盟了，返回
	if (shareProperty.dwClanID != 0)
	{
		return;
	}

	// 被邀请者还没有战队，返回
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
	// 邀请者不在线，返回
	if (inviterShareProperty.dwPDBID == INVALID_PDBID)
		return;

	// 邀请者没有战队，返回
	if (inviterShareProperty.dwKinID == 0)
	{
		return;
	}

	// 邀请者没有联盟，返回
	if (inviterShareProperty.dwClanID == 0)
	{
		return;
	}

	CClan* pClan = findClan(inviterShareProperty.dwClanID);
	if (!pClan)
	{
		return;
	}
	
	// 邀请者不是盟主，返回
	if (pClan->getNumProp(emClanProp_ShaikhID) != inviterShareProperty.dwPDBID)
	{
		return;
	}

	// 邀请者联盟在解散状态，返回
	if (pClan->getNumProp(emClanProp_State) == EMClanState::emClanState_Dismissing)
	{
		return;
	}

	// 判断列表是否已经满了
	SClanLevel* pLevelInfo = g_EHelper.getLevelInfo(pClan->getNumProp(emClanProp_Level));
	if (pLevelInfo == NULL)
		return;

	// 邀请者联盟成员已满返回
	if (pClan->getKinCount() >= pLevelInfo->nMaxKinCount)
		return;

	// 把该玩家从申请列表中删除
	SClanApply *pApply = g_ClanInviteManager.findApply(shareProperty.dwKinID);
	if (pApply != NULL)
	{
		CClan *pClan = findClan(pApply->dwClanID);
		g_ClanInviteManager.removeApply(pClan, shareProperty.dwKinID);
	}

	pClan->acceptMember(shareProperty.dwKinID);

	// 回复邀请者，邀请加入成功
	obuf256 obuf;
	fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_ANSWER_INVITE);
	g_EHelper.sendDataToPerson(pRecvData->dwInviterPDBID, obuf.data(), obuf.size());
}


// 读取联盟列表信息
bool CClanService::readClanList()
{
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService == NULL)
    {
        return false;
	}

    if (pDBEngineService->executeSP(GAMEDB_REQUEST_READCLANLIST, 0, "", 0, this) == false)
    {
        ErrorLn(__FUNCTION__" 请求失败");
        return false;
    }

	CLAN_EMPHASISLN(_GT("开始读取联盟列表"));
    return true;
}

// 读取联盟附加数据（成员列表）
void CClanService::readClanPlusData(DWORD dwClanID)
{
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService == NULL)
    {
        return;
    }

    // 读取联盟成员列表请求
    DBREQ_PARAM_READMEMBERLIST readMemberList;
    readMemberList.dwClanID = dwClanID;
    if (pDBEngineService->executeSP(GAMEDB_REQUEST_READMEMBERLIST, dwClanID, (LPCSTR)(&readMemberList), sizeof(readMemberList), this) == false)
    {
        ErrorLn(_GT("GAMEDB_REQUEST_READMEMBERLIST 请求失败"));
    }
}

// 通知场景服更改角色身上的联盟ID
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

// 修改所有联盟杯赛次数
void CClanService::UpdateAllClanLegendCupProp()
{
	TMap_ClanIt it = m_ClanList.begin();
	for (; it != m_ClanList.end(); ++it)
	{
		CClan* pClan = ((*it).second);
		if (pClan->setNumProp(emClanProp_LegendCupCount, 1, true, true))
		{
			// 更新客户端
			msg_clan_updateintprop_oc sendData;
			sendData.nPropID = emClanProp_LegendCupCount;
			sendData.nValue = 1;
			pClan->broadcastMsgToObserve(MSG_CLAN_UPDATECLANINTPROP, (LPCSTR)&sendData, sizeof(sendData));
		}
	}
}

// 重置周活跃度
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
