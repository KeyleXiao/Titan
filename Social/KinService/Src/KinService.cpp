#include "stdafx.h"
#include "KinService.h"
//#include "ICenterConnectorService.h"
#include "ISocialGlobal.h"
#include "Event_ManagedDef.h"
#include "IShareServer.h"
#include "ExternalHelper.h"
#include "KinInviteManager.h"
#include "ISchemeCenter.h"
#include "IMessageDispatch.h"
#include "KinSchemeDef.h"
#include "ISocialGlobal.h"
#include "ClanHelper.h"
#include "ClanDef.h"
#include "OrderEvent.h"
#include "EventDef.h"
#include "Simple_Atom_Lock.h"
#include "KinHelper.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "IDateTriggerService.h"

struct IDBEngineService;

KinService::KinService()
{
    m_mutex = 0;
    m_KinList.clear();
    m_KinMemberList.clear();
    m_KinUserLoginList.clear();
    m_UserQuitTimeTable.clear();
    m_bFinalList = false;
    m_bDBKinAllDataFinal = false;
    m_DeleteKinList.clear();
    m_mapObserveList.clear();
}


KinService::~KinService()
{

}



void KinService::release()
{
	TraceLn(__FUNCTION__);

    //取消注册
    gSocialGlobal->getEventEngine()->UnSubscibe(this, EVENT_ORDER_UPDATE_RANK, SOURCE_TYPE_ODER, 0);
    gSocialGlobal->getEventEngine()->UnSubscibe(this, EVENT_CLAN_UPDATEID, SOURCE_TYPE_CLAN, 0);
 
    // 数据库接口反注册
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->unregisterHandler(this);
    }

    // 共享信息注册
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->UnregisterHandler(this);
    }

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        // 服务器消息分发反注册
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_KIN);
        // 客户端消息分发反注册
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_KIN);
    }

	IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
    if (pDateTriggerService != NULL)
    {
        pDateTriggerService->UnRegisterDateTrigger(DCT_DataTrigger_WeekActivityReset, this);
		pDateTriggerService->UnRegisterDateTrigger(DCT_DataTrigger_AutoSetShaikhState, this);
	}

	delete this;
}

bool KinService::create()
{
	TraceLn(__FUNCTION__);
    // 战队事件注册
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return false;

    pEventEngine->Subscibe(this, EVENT_ORDER_UPDATE_RANK, SOURCE_TYPE_ODER, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_CLAN_UPDATEID, SOURCE_TYPE_CLAN, 0, __FUNCTION__);

    // 数据库接口注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService != NULL)
    {
        pDBEngineService->registerHandler(this);
    }

    // 共享信息注册
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->RegisterHandler(this, __FUNCTION__);
    }

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        // 服务器消息分发注册
        pMessageDispatch->registerTransmitHandler(MSG_MODULEID_KIN,this);
        // 客户端消息分发注册
        pMessageDispatch->registerMessageHandler(MSG_MODULEID_KIN, this);
    }

	// 设置每周日23:59清一次活跃度
	IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
    if (pDateTriggerService != NULL)
    {
        SDateTriggerInfo sTriggerInfo;
        sTriggerInfo.year = -1;
        sTriggerInfo.month = -1;
        sTriggerInfo.day = -1;
        sTriggerInfo.hour = 23;
        sTriggerInfo.min = 59;
        sTriggerInfo.wday = 0;
		pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_WeekActivityReset, this, sTriggerInfo);
        sTriggerInfo.hour = -1;
        sTriggerInfo.min = 20;
        sTriggerInfo.wday = -1;
        pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_AutoSetShaikhState, this, sTriggerInfo);
	}

	tm localNowTime;
	time_t currentTime = time(NULL);
	localtime_s(&localNowTime, &currentTime);
	if ((localNowTime.tm_wday == 0 && localNowTime.tm_hour >= 23)
		|| (localNowTime.tm_wday == 1 && localNowTime.tm_hour <= 1)) // 若在周日23：00 - 周一1：00 之间，则周活跃度清零
	{
		ResetWeekActivity();
	}

    // 设置战队邀请请求管理相关服务
    g_KinInviteManager.setKinService(this);

    Trace("read initalkindata ...."<<endl);
    // 循环读取不同类型的排行榜
    readInitalKinData();

	return true;
}

void KinService::send2Client( ClientID clientID, obuf256& obuf)
{
    IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
	if (nullptr == pGatewayAcceptorService)
	{
		ErrorLn(__FUNCTION__": failed!  PDBID=" << clientID);
		return;
	}

    if(clientID == INVALID_CLIENT)
	{
		ErrorLn(__FUNCTION__": failed! clientID == INVALID_CLIENT !");
		return;
	}

	pGatewayAcceptorService->sendData(clientID, PACKAGE_PTR(new std::string((char const *)obuf.data(), obuf.size())));
}


void KinService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    if (nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__":error return! nDBRetCode = " << nDBRetCode << "  reason：" << pszDBRetDesc);
	}

	switch (nCmdID)
	{ 
		// 读取战队列表                 -- 数据库返回处理
	case GAMEDB_REQUEST_READKINLIST:
		{
			onReturnReadKinList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 读取战队基本数据
	case GAMEDB_REQUEST_READKINBASE:
		{
			onReturnReadKinBase(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 读取战队成员列表             -- 数据库返回处理
	case GAMEDB_REQUEST_READ_KIN_MEMBERLIST:
		{
			onReturnReadMemberList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 读取战队杯赛奖励列表          -- 数据库返回处理
	case GAMEDB_REQUEST_READ_KIN_LEGENDAWARD:
		{
			onReturnReadLegendAwardList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 读取战队杯赛战绩列表          -- 数据库返回处理
	case GAMEDB_REQUEST_READ_KIN_LEGENDWAR:
		{
			onReturnReadLegendWarList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 更新成员（没有则添加）       -- 数据库返回处理
	case GAMEDB_REQUEST_UPDATE_KINMEMBER:
		{
			onReturnUpdateMember(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 删除成员                     -- 数据库返回处理
	case GAMEDB_REQUEST_DELETE_KINMEMBER:
		{
			onReturnRemoveMember(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 更新战队基本数据
	case GAMEDB_REQUEST_UPDATEKIN:
		{
			onReturnUpdateKin(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 解散联盟
	case GAMEDB_REQUEST_DISMISSKIN:
		{
			onReturnDismissKin(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 增加杯赛奖励数据       -- 数据库返回处理
	case GAMEDB_REQUEST_ADD_KINLEGENDAWARD:
		{
			onReturnAddLegendAward(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}

		// 增加杯赛战绩数据       -- 数据库返回处理
	case GAMEDB_REQUEST_ADD_KINLEGENDWAR:
		{
			onReturnAddLegendWar(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}
    default:
        break;
	}
}

void KinService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
        
    case EVENT_ORDER_UPDATE_RANK:
        {
            // 更新战队Rank数据
            
            if(pszContext == NULL || nLen != sizeof(SEventOrder_UpdateRank))
            {
                return;
            }
            UpdatePersonRank((void*)pszContext);
        }
        break;
    case EVENT_CLAN_UPDATEID:
        {
            // 更新战队Rank数据
            
            if(pszContext == NULL || nLen != sizeof(event_clan_update_id))
            {
                return;
            }
            UpdatePersonUpdateClanID((void*)pszContext);
        }
        break;

    default:
        break;
    }
}

void KinService::readInitalKinData()
{
    // 读取战队基本数据列表
    if (readKinList() == false)
    {
        ErrorLn(__FUNCTION__": readKinList() failed!");
    }
}

/// purpose: 读取战队列表信息
bool KinService::readKinList()
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return false;
    }

    DBREQ_PARAM_READKINLIST refer;
    refer.nPageSize = 300;
    refer.nPageIndex = 1;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READKINLIST, 7, (LPCSTR)&refer, sizeof(refer), this) == false)
    {
        ErrorLn(__FUNCTION__": GAMEDB_REQUEST_READKINLIST failed!!");
        return false;
    }
    return true;
}

void KinService::onModuleMessage(ulong nEndpointId, DWORD dwMsgID, SNetMsgHead* pHead, LPCSTR pszData, size_t nLen)
{
    //IUserManager* pUserManager = gSocialGlobal->getUserManager();
    //if (pUserManager == NULL)
    //{
    //    return;
    //}

    // 验证客户端消息的有效性
    if (pHead->bySrcEndPoint == MSG_ENDPOINT_CLIENT)
    {
        // 根据客户端ID取得人物信息

        switch (dwMsgID)
        {

            // 创建战队
        case MSG_KIN_CREATE:
            {
                onClientCreateKin(nEndpointId, pszData, nLen);
            }
            break;
            // 邀请加入战队 
        case MSG_KIN_INVITE:
            {
                onClientInvite(nEndpointId, pszData, nLen);
            }
            break;
            // 邀请答复 
        case MSG_KIN_ANSWER_INVITE:
            {
                onClientAnswerInvite(nEndpointId, pszData, nLen);
            }
            break;
            // 申请战队列表 
        case MSG_KIN_LIST:
            {
                onClientKinList(nEndpointId, pszData, nLen);
            }
            break;
            // 战队改名 
        case MSG_KIN_RENAME:
            {
                onClientKinRename(nEndpointId, pszData, nLen);
            }
            break;
        case MSG_KIN_LIST_EXITOBSERVE:
            {
                onClientRemoveKinListOb(nEndpointId, pszData, nLen);
            }
            break;
        default:
            break;
        }
    }
    else if (pHead->bySrcEndPoint == MSG_ENDPOINT_SCENE)
    {
        // 场景服消息处理
        switch (dwMsgID)
        {

        case MSG_KIN_ACTOR_CHANGESEX:
            {
                onSceneActorChangeSex(nEndpointId, pszData, nLen);
            }
            break;
        case MSG_KIN_ACTOR_RENAME:
            {
                onSceneActorRename(nEndpointId, pszData, nLen);
                
            }
            break;
            // 创建战队
        case MSG_KIN_CREATE:
            {
                onSceneCreateKin(nEndpointId, pszData, nLen);
            }
            break;
        case MSG_KIN_RENAME:
            {
                onSceneKinRename(nEndpointId, pszData, nLen);
                break;
            }
        default:
            break;
        }
    }
    else
    {
        return;
    }
}

void KinService::OnMergeMessage(ulong nEndpointId, DWORD dwMsgID, SNetMsgHead* pHead, LPCSTR pszData, size_t nLen)
{

}

void KinService::onClientInvite(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": pSharePerson == NULL")
        return;
    }

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if(pConfig == NULL || nLen != sizeof(SMsgKin_Invite_CO) || pShareReceiver == NULL)
    {
        return;
    }

    SMsgKin_Invite_CO* pRecvData = (SMsgKin_Invite_CO*)pszData; 
    SKinMember* pMember = findKinMember(shareProperty.dwPDBID);
    
    // 邀请者是否有战队
    if (pMember == NULL)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_HAVE_NOT_KIN_CANT_INVITE,"");
        return;
    }

    // 邀请者是否要权限
    if ((pMember->dwPopedomFlag & emKinPopedom_AcceptMember) != emKinPopedom_AcceptMember)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_HAVE_NOT_RIGHT_INVITE,"");
        return;
    }

    // 战队成员是否已经满员
    if ((pMember->dwPopedomFlag & emKinPopedom_AcceptMember) != emKinPopedom_AcceptMember)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_HAVE_NOT_RIGHT_INVITE,"");
        return;
    }

    SSharePersonProperty sAnswerProperty = pShareReceiver->GetSharePerson(pRecvData->dwAnswerPDBID);
    // 被邀请者是否在线
    if (sAnswerProperty.dwPDBID == INVALID_PDBID)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_INVITE_PERON_NOT_ONLINE,"");
        return;
    }

    // 对方已经有战队了，不能邀请
    SKinMember* pAnswerMember = findKinMember(pRecvData->dwAnswerPDBID);
    if (pAnswerMember != NULL)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_INVITE_PERON_HAVE_KIN,"");
        return;
    }

	// 拒绝加入战队的邀请后，该战队的邀请在服务器重启前收不到，
	//SKinInvite* pInvite = g_KinInviteManager.FindInvite(pMember->dwPDBID, pRecvData->dwAnswerPDBID);
	//if (pInvite)
	//{
	//	g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_KIN_INVITE_HAVE_SEND, "");
	//	return;
	//}

    // 建立邀请关系
    g_KinInviteManager.AddInvite(shareProperty.dwPDBID, pMember->dwKinID, pRecvData->dwAnswerPDBID);

    // 通知被邀请者生成deeder
    CKin* pKin = findKin(shareProperty.dwKinID);
    if (pKin == NULL)
    {
        ErrorLn(__FUNCTION__": can't find kin! KinID ="<<shareProperty.dwKinID);
        return;
    }

    SMsgKin_Invite_OC sendData;
    sendData.dwInviterPDBID = shareProperty.dwPDBID;
    sstrcpyn(sendData.szInviteName, shareProperty.szName, sizeof(sendData.szInviteName));
    sstrcpyn(sendData.szKinName, pKin->getName(), sizeof(sendData.szKinName));

    // 通知客户端刷新响应界面
    obuf256 obuf;
    fillKinData(obuf, MSG_ENDPOINT_CLIENT, MSG_KIN_INVITE);
    obuf.push_back(&sendData, sizeof(sendData));
    g_EHelper.SendDataToPerson(pRecvData->dwAnswerPDBID, obuf.data(), obuf.size());

    // 通知邀请者邀请成功
    g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_INVITE_HAVE_SEND,"");
}


void KinService::onClientAnswerInvite(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    char szText[512] = {0};
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    if (shareProperty.clientID != clientID)
    {
        return;
    }

    if (nLen != sizeof(SMsgKin_Answer_CO))
    {
        return;
    }


    SMsgKin_Answer_CO* pRecvData = (SMsgKin_Answer_CO*)pszData;
    PDBID PersonDBID = shareProperty.dwPDBID;
    // 判断是否有邀请关系
    SKinInvite* pInvite = g_KinInviteManager.FindInvite(pRecvData->dwInviterPDBID, PersonDBID);
    if (pInvite == NULL)
    {
        return;
    }

    // 如果拒绝
    if (pRecvData->bIsAgree == false)
    {
        return;
    }

    // 如果对方的战队已经不存在了，不处理
    CKin* pKin = findKin(pInvite->dwInviteKinID);
    if (pKin == NULL)
    {
        g_EHelper.showSystemMessage(PersonDBID,CHAT_TIP_KIN_NOT_EXIST,"");
        g_KinInviteManager.RemoveInvite(pRecvData->dwInviterPDBID, PersonDBID);
        return;
    }

    if (pKin->getNumProp(emKinProp_State) == emKinState_Informal)
    {
        return;
    }

    // 如果您已经加入了其他战队，不处理
    SKinMember* pMember = findKinMember(PersonDBID);
    if (pMember != NULL)
    {
        g_EHelper.showSystemMessage(PersonDBID,CHAT_TIP_KIN_YOU_HAVE_KIN,"");
        g_KinInviteManager.RemoveInvite(pRecvData->dwInviterPDBID, PersonDBID);
        return;
    }

    // 判断列表是否已经满了
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }
    SKinConfig* pKinConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if (pKinConfig == NULL)
    {
        ErrorLn(__FUNCTION__" nullptr == pKinConfig");
        return;
    }

    // 离开战队冷却
    int nQuitKinTime = getQuitKinTime(PersonDBID);
    int nCurTime = (int)time(NULL);
    int nDiffTime =  pKinConfig->nJoinKinFreeze * 3600 - (nCurTime -  nQuitKinTime);
    if (nQuitKinTime != MAX_INT_VALUE &&  nDiffTime > 0)
    {
        char szText[26] = {0};
        sprintf_s(szText, "%d", pKinConfig->nJoinKinFreeze);
        g_EHelper.showSystemMessage(PersonDBID, CHAT_TIP_KIN_LEAVE_TIME_NOT_SATISFY,szText);
        return;
    }

    SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(pKin->getNumProp(emKinProp_Level));
    if (pLevelInfo == NULL)
    {
        return;
    }
    
    if (pKin->getMemberCount() >= pLevelInfo->nMaxMemberCount)
    {
        g_EHelper.showSystemMessage(PersonDBID,CHAT_TIP_KIN_MEMBER_IS_FULL,"");
        return;
    }

    // 如果玩家已经申请加入了其他战队，需要解除其关系
    g_KinInviteManager.RemoveApply(PersonDBID);

    // 战队接收成员
    SKinApply sKinInvite;
    sKinInvite.dwUDBID = shareProperty.dwUDBID;
    sKinInvite.dwApplyPDBID = PersonDBID;
    sKinInvite.nVocation = shareProperty.nVocation;
    sKinInvite.nLevel = shareProperty.wLevel;
    sKinInvite.nFaceID= shareProperty.nFaceID;
    sKinInvite.nSex = shareProperty.bySex;
    sstrcpyn(sKinInvite.szApplyName,shareProperty.szName,sizeof(sKinInvite.szApplyName));

    pKin->acceptMember(&sKinInvite);
    g_KinInviteManager.RemoveInvite(pRecvData->dwInviterPDBID, PersonDBID);

    // 往客户端发允许加入
    g_EHelper.FireClientEvent(pRecvData->dwInviterPDBID, EVENT_KIN_ANSWERINVITE_SUCCESS, SOURCE_TYPE_KIN, pKin->getNumProp(emKinProp_ID), NULL, 0);
}

void KinService::onClientKinRename(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    if (nLen != sizeof(SMsKin_Rename_CO) || pszData == NULL)
    {
        return;
    }

    SKinMember* pMember = findKinMember(shareProperty.dwPDBID);
    if(pMember == NULL || emKinIdentity_Shaikh != pMember->nIdentity)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_KIN_SHAIKH_CAN_RENAME_KIN,"");
        return;
    }

    CKin *pKin = findKin(pMember->dwKinID);
    if(pKin == NULL)
    {
        return;
    }

    SMsKin_Rename_CO* pRecvData = (SMsKin_Rename_CO*)pszData;
    // 判断战队名字格式
    if(NULL != strstr(pRecvData->szNewName, " "))
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_KIN_NAME_CANT_HAVE_SPACE,"");
        return;
    }

    // 往场景服发送修改战队名称消息
    obuf512 obuf;
    SMsKin_Rename_OS sendData;
    sendData.dwPDBID = shareProperty.dwPDBID;
    sendData.dwKinID = shareProperty.dwKinID;
    sstrcpyn(sendData.szNewName, pRecvData->szNewName, sizeof(sendData.szNewName));

    fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_RENAME);
    obuf.push_back(&sendData, sizeof(sendData));

    g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
}

void KinService::onClientRequestMerge(ClientID clientID, LPCSTR pszData, size_t nLen)
{

}

void KinService::onSceneCreateKin(DWORD dwSceneID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        ErrorLn(__FUNCTION__": ")
        return;
    }

    if (nLen != sizeof(SMsgKin_Create_SO))
    {
        return;
    }

    SMsgKin_Create_SO* pRecvData = (SMsgKin_Create_SO*)pszData;

    // 把该玩家从申请列表中删除
    g_KinInviteManager.RemoveApply(pRecvData->dwPDBID);	// 注意pKinApply已经不能用了

    // 判断一下 战队是否已经在列表中
    TMap_Kin ::iterator it = m_KinList.find(pRecvData->dwKinID);
    if (it != m_KinList.end())
    {
        ErrorLn("repeat KinID ="<<pRecvData->dwKinID);
        return ;
    }

    // 创建战队对象（此时数据库已经创建了，队长也添加到数据了）
    CKin* pKin = new CKin();
    if (pKin->create(this, pszData, nLen) == false)
    {
        ErrorLn("create kin faild :" << pRecvData->szName);
        return;
    }
    //设置保存下数据库
    pKin->updateDataToDB(true);

    // 把战队对象添加到列表
    m_KinList[pRecvData->dwKinID] = pKin;
    m_NameKinList[pRecvData->szName] = pKin;

    // 添加到正式战队
    if (pKin->getNumProp(emKinProp_State) == emKinState_Formal)
    {
        m_FormalKinArray[pKin->getNumProp(emKinProp_ID)] = pKin;
    }

    // 往场景服添加战队数据
    pKin->updateDataToScene();

    // 广播数据到客户端
    pKin->broadcastDataToClient();

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pRecvData->dwPDBID);
    if (shareProperty.dwPDBID == pRecvData->dwPDBID)
    {
        // 更新战队ID到场景服
        obuf512 obuf;
        SMsgKin_UdateID sceneData;
        sceneData.dwPDBID = pRecvData->dwPDBID;
        sceneData.dwKinID = pRecvData->dwKinID;

        // 把ID更新到场景服
        fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEID);
        obuf.push_back(&sceneData, sizeof(sceneData));
        g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), shareProperty.serverID);

        // 发送战队ID改变事件
        IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
        if(pEventEngine)
        {
            obuf512 buf;
            SEventKin_UpdateKinID eventData;
            eventData.dwPDBID = pRecvData->dwPDBID;
            eventData.dwKinID = pRecvData->dwKinID;
            buf.push_back(&eventData, sizeof(eventData));
            pEventEngine->FireExecute(EVENT_KIN_UPDATEID, SOURCE_TYPE_KIN, pRecvData->dwPDBID, buf.data(), SIZE_INT(buf.size()) );
        }

        // 通知客户端刷新战队列表
        SMsgKin_KinList_CO sendData;
        sendData.nPageNum = 1;
        sendData.nState = emKinState_Formal;
        sstrcpyn(sendData.szKinName, pKin->getName(),sizeof(sendData.szKinName));

        onClientKinList(shareProperty.clientID, (LPCSTR)&sendData, sizeof(sendData));
    }

    //记录创建日志
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        char szText[512] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("创建战队：角色id[%u]角色名创建战队名称[%s]，战队id[%u]"), pRecvData->dwPDBID, pRecvData->szName, pRecvData->dwKinID);

        IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
        if ( pOSSLogService != NULL )
        {
            pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_KIN, pRecvData->dwKinID, szText);
        }
        
    }

    g_EHelper.showSystemMessage(pRecvData->dwPDBID,CHAT_TIP_KIN_CREATE_SUCCESS,"");

    // 往客户端发出添加创建成功
    SEventKin_CreateSuccess_OC eventData;
    eventData.bSuccess = true;
    g_EHelper.FireClientEvent(pRecvData->dwPDBID, EVENT_KIN_CREATE_SUCCESS, SOURCE_TYPE_KIN, pKin->getNumProp(emKinProp_ID), (LPCSTR)&eventData, sizeof(SEventKin_CreateSuccess_OC));
}

void KinService::onReturnReadKinList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{

    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return;
    }

    // 验证数据库返回结果
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn("[onReturnReadKinList] read KinList error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
        return;
    }

    // 检查数据返回长度
    if (nOutLen < sizeof(DBREQ_RESULT_READKINLIST))
    {
        int tempLen = sizeof(DBREQ_RESULT_READKINLIST);
        ErrorLn("[onReturnReadKinList] error length outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READKINLIST) = " << tempLen);
        return;
    }

    DBREQ_RESULT_READKINLIST* pRetData = (DBREQ_RESULT_READKINLIST*)pOutData;
    EmphasisLn("return Kin count =" << pRetData->nKinCount);
    if (nOutLen != sizeof(DBREQ_RESULT_READKINLIST) + pRetData->nKinCount * sizeof(DBREQ_TABLE_KIN))
    {
        int tempLen = sizeof(DBREQ_RESULT_READKINLIST) + pRetData->nKinCount * sizeof(DBREQ_TABLE_KIN);
        ErrorLn("[onReturnReadKinList] length wrong outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READKINLIST) = " << tempLen);
        return;
    }

    // 如果还没读完，继续读
    if ((pRetData->nPageIndex - 1) * 300 + pRetData->nKinCount < pRetData->nReTotalCount)
    {
        DBREQ_PARAM_READKINLIST refer;
        refer.nPageSize = 300;
        refer.nPageIndex = pRetData->nPageIndex + 1;

        if (pDBEngine->executeSP(GAMEDB_REQUEST_READKINLIST, 0, (LPCSTR)&refer, sizeof(refer), this) == false)
        {
            ErrorLn(_GT("GAMEDB_REQUEST_READKINLIST request failed!"));
            return;
        }
    }
    else
    {
        // TODO 战队列表读取完成，通知帮会模块战队帮会列表
        m_bFinalList = true;
    }

    // 通知所有场景服清除所有的战队数据
    //obuf512 obuf;
    //fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_CLEAR_SCENE);
    //g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size());

    ostrbuf osbKinList;
    osbKinList << "kinlist = ";

    // 创建战队对象
    int offset = sizeof(DBREQ_RESULT_READKINLIST);
    char szText[512] = {0};
    static bool bOrder = false;
    for (int i = 0; i < pRetData->nKinCount; i ++)
    {

        DBREQ_TABLE_KIN* pKinData = (DBREQ_TABLE_KIN*)(pOutData + offset);
        
        osbKinList << pKinData->dwID << ",";

        // 创建战队对象
        CKin* pKin = new CKin();
        if (pKin == NULL)
        {
            ErrorLn(__FUNCTION__": pKin == NULL" << pKinData->szName << ", KinID" << pKinData->dwID);
            offset += sizeof(DBREQ_TABLE_KIN);
            continue;
        }

        if (pKin->createFromDB(this,(LPCSTR)pKinData, sizeof(DBREQ_TABLE_KIN)) == false)
        {
            ErrorLn(__FUNCTION__": name =" << pKinData->szName << ", KinID" << pKinData->dwID);
            offset += sizeof(DBREQ_TABLE_KIN);
            continue;
        }

        // 把战队对象添加到列表
        m_KinList[pKinData->dwID] = pKin;
        if (pKinData->nState == emKinState_Formal)
        {
            m_FormalKinArray[pKinData->dwID] = pKin;
        }
        m_NameKinList[pKinData->szName] = pKin;

        // 通知场景服添加战队
        obuf512 ob;
        fillKinData(ob, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEKIN_SCENE);
        SMsgKin_UpdateKinScene_OS sendData;
        sendData.dwID = pKinData->dwID;
        sstrcpyn(sendData.szName, pKinData->szName, sizeof(sendData.szName));
        sendData.nState = pKinData->nState;
        sendData.nLevel = pKinData->nLevel;
        sendData.nWorldID = pKin->getNumProp(emKinProp_WorldID);
		sendData.nWeekActivity = pKin->getNumProp(emKinProp_WeekActivity);
		ob.push_back(&sendData, sizeof(sendData));
        g_EHelper.SendDataToSceneSvr(ob.data(), ob.size());

        readIniKinPlusData(pKinData->dwID, true);

        offset += sizeof(DBREQ_TABLE_KIN);
    }

    //EmphasisLn(osbKinList.c_str());

    if ( m_bFinalList == true)
    {
        onReadKinListFinish();

    }

    //EmphasisLn("读取战队列表完成");
}

// 加载战队列表完成
void KinService::onReadKinListFinish()
{
   TraceLn(__FUNCTION__": all kin list read complete!");
   // 发送战队读取完成事件

   //把所有ID发送到排行榜
   obuf obAllData;
   event_kin_read_finished_head head;
   
   obuf obData;
   for (TMap_Kin::iterator iter = m_KinList.begin(); iter != m_KinList.end(); ++iter)
   {
       head.nCount++;
       obData.push_back(&iter->first,sizeof(iter->first));
   }
   obAllData.push_back(&head, sizeof(head));
   obAllData.push_back(obData.data(), obData.size());

   IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
   if(pEventEngine)
   {
       pEventEngine->FireExecute(EVENT_KIN_READ_DB_FINISHED, SOURCE_TYPE_KIN, 0, obAllData.data(), obAllData.size());
   }
}

/// purpose: 读取战队成员列表       -- 来自数据库
void KinService::onReturnReadKinBase(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{	
    // 验证数据库返回结果
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__": KinID = " << nQueueIndex << "pszDBRetDesc = " << a2utf8(pszDBRetDesc));
        return;
    }

    int nTempLen = sizeof(DBREQ_RESULT_READKINBASE);
    if (nOutLen != nTempLen)
    {
        ErrorLn(__FUNCTION__": outlength error! nOutLen = " << nOutLen << ", right length = " << nTempLen);
        return;
    }

    DWORD dwKinID = (DWORD)nQueueIndex;
    CKin* pKin = findKin(dwKinID);
    if (pKin == NULL)
    {
        WarningLn(__FUNCTION__":can't find kin object! dwKinID=" << dwKinID);
        return;
    }

    if (pKin->setDBBaseContext(pOutData, nOutLen) == false)
    {
        WarningLn("[onReturnReadKinBase] set kin base Kin data error!");
    }

    // 删除缓存登陆数据
    int nReadFlag = pKin->getNumProp(emKinProp_ReadFlag);
    if (nReadFlag == emKinRead_AllData)
    {
        onReadDBFinish(dwKinID);
    }
}

/// purpose: 战队的全部数据读取完成
void KinService::onReadDBFinish(DWORD dwKinID)
{	
    // 通知帮会模块，战队读取完成
    ClanHelper helper;
    IClanService *pClanService = helper.m_ptr;
    if (pClanService != NULL)
    {
        pClanService->removeReadingKin(dwKinID);
    }

    CKin* pKin = findKin(dwKinID);
    if (pKin == NULL)
    {
        return;
    }

    // 往场景服更新改战队的数据
    pKin->updateDataToScene();
    pKin->updateKinOrderInfo();
    // TraceLn("[跨区战队] 向公共区同步战队信息：kinID=" << pKin->getNumProp(emKinProp_ID));
}

/// purpose: 读取战队成员列表       -- 来自数据库
void KinService::onReturnReadMemberList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__": KinID = " << nQueueIndex << "pszDBRetDesc = " << a2utf8(pszDBRetDesc));
        return;
    }

    int nTempLen = sizeof(DBREQ_RESULT_READKINMEMBERLIST);
    if (nOutLen < nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_READKINMEMBERLIST] length = " << nTempLen);
        return;
    }

    DBREQ_RESULT_READKINMEMBERLIST* pRetData = (DBREQ_RESULT_READKINMEMBERLIST*)pOutData;
    nTempLen = sizeof(DBREQ_RESULT_READKINMEMBERLIST) + pRetData->nMemberCount * sizeof(DBREQ_TABLE_KINMEMBER);
    if (nOutLen != nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
        return;
    }
    DWORD dwKinID = (DWORD)nQueueIndex;

    CKin* pKin = findKin(dwKinID);
    if (pKin == NULL)
    {
        ErrorLn(__FUNCTION__": can't find the kin object, dwKinID=" << dwKinID);
        return;
    }

    // 里面把成员添加到m_KinMemberList里
    if (pKin->setDBMemberContext(pOutData, nOutLen) == false)
    {
        WarningLn(__FUNCTION__": set setDBMemberContext error!");
    }

    // 删除缓存登陆数据
    int nReadFlag = pKin->getNumProp(emKinProp_ReadFlag);
    if (nReadFlag == emKinRead_AllData)
    {
        onReadDBFinish(dwKinID);
    }

}

/// purpose: 读取战队杯赛奖励列表       			-- 来自数据库
void KinService::onReturnReadLegendAwardList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__": KinID = " << nQueueIndex << "pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}

	int nTempLen = sizeof(DBREQ_RESULT_READKINLEGENDAWARDLIST);
	if (nOutLen < nTempLen)
	{
		ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_READKINLEGENDAWARDLIST] length = " << nTempLen);
		return;
	}

	DBREQ_RESULT_READKINLEGENDAWARDLIST* pRetData = (DBREQ_RESULT_READKINLEGENDAWARDLIST*)pOutData;
	nTempLen = sizeof(DBREQ_RESULT_READKINLEGENDAWARDLIST) + pRetData->nLegendAwardCount * sizeof(DBREQ_TABLE_KIN_LEGENDAWARD);
	if (nOutLen != nTempLen)
	{
		ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
		return;
	}
	DWORD dwKinID = (DWORD)nQueueIndex;

	CKin* pKin = findKin(dwKinID);
	if (pKin == NULL)
	{
		ErrorLn(__FUNCTION__": can't find the kin object, dwKinID=" << dwKinID);
		return;
	}

	if (pKin->setDBLegendAwardContext(pOutData, nOutLen) == false)
	{
		WarningLn(__FUNCTION__": set setDBLegendAwardContext error!");
	}

	// 删除缓存登陆数据
	int nReadFlag = pKin->getNumProp(emKinProp_ReadFlag);
	if (nReadFlag == emKinRead_AllData)
	{
		onReadDBFinish(dwKinID);
	}
}

/// purpose: 读取战队杯赛战绩列表       			-- 来自数据库
void KinService::onReturnReadLegendWarList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__": KinID = " << nQueueIndex << "pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}

	int nTempLen = sizeof(DBREQ_RESULT_READKINLEGENDWARLIST);
	if (nOutLen < nTempLen)
	{
		ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_READKINLEGENDWARLIST] length = " << nTempLen);
		return;
	}

	DBREQ_RESULT_READKINLEGENDWARLIST* pRetData = (DBREQ_RESULT_READKINLEGENDWARLIST*)pOutData;
	nTempLen = sizeof(DBREQ_RESULT_READKINLEGENDWARLIST) + pRetData->nLegendWarCount * sizeof(DBREQ_TABLE_KIN_LEGENDWAR);
	if (nOutLen != nTempLen)
	{
		ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
		return;
	}
	DWORD dwKinID = (DWORD)nQueueIndex;

	CKin* pKin = findKin(dwKinID);
	if (pKin == NULL)
	{
		ErrorLn(__FUNCTION__": can't find the kin object, dwKinID=" << dwKinID);
		return;
	}

	if (pKin->setDBLegendWarContext(pOutData, nOutLen) == false)
	{
		WarningLn(__FUNCTION__": set setDBLegendWarContext error!");
	}

	// 删除缓存登陆数据
	int nReadFlag = pKin->getNumProp(emKinProp_ReadFlag);
	if (nReadFlag == emKinRead_AllData)
	{
		onReadDBFinish(dwKinID);
	}
}

/// purpose: 更新/添加成员信息       				-- 来自数据库
void KinService::onReturnUpdateMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__": error! PDBID = " << nQueueIndex << " reason :" << a2utf8(pszDBRetDesc));
        return;
    }
}

/// purpose: 删除成员信息			  				-- 来自数据库
void KinService::onReturnRemoveMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__": error! PDBID  = " << nQueueIndex << " reason:" << pszDBRetDesc);
        return;
    }
}

/// purpose: 更新战队基本数据			  			-- 来自数据库
void KinService::onReturnUpdateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__": error! KinID = " << nQueueIndex << " reason:" << pszDBRetDesc);
        return;
    }
}

/// purpose: 解散战队			  					-- 来自数据库
void KinService::onReturnDismissKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__": error! KinID = " << nQueueIndex << " reason:" << pszDBRetDesc);
        return;
    }
}

/// purpose: 添加杯赛奖励信息       				-- 来自数据库
void KinService::onReturnAddLegendAward(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__": error! dwKinID = " << nQueueIndex << " reason :" << a2utf8(pszDBRetDesc));
		return;
	}
}

/// purpose: 添加杯赛战绩信息       				-- 来自数据库
void KinService::onReturnAddLegendWar(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__": error! WarID = " << nQueueIndex << " reason :" << a2utf8(pszDBRetDesc));
		return;
	}
}

void KinService::onClientCreateKin(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    SKinConfig* pKinConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if (pKinConfig == NULL)
    {
        ErrorLn(__FUNCTION__" nullptr == pKinConfig");
        return;
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);

    if (nLen != sizeof(SMsgKin_Create_CO))
    {
        return;
    }

    if(shareProperty.clientID != clientID)
    {
        ErrorLn(__FUNCTION__": pSharePerson == NULL");
        return;    
    }

    SMsgKin_Create_CO* pRecvData = (SMsgKin_Create_CO*)pszData;

    // 如果当前国家的战队已经超过了x个，就不能再给你创建战队了
    if (m_FormalKinArray.size() >= KIN_MAX_COUNT)
    {
        TraceLn("the count of kin is over!");
        return;
    }

    PDBID PersonPDBID = shareProperty.dwPDBID;
	if (pKinConfig->nJoinKinFreeze > 0)
	{
		char szText[512] = { 0 };
		int nQuitKinTime = getQuitKinTime(PersonPDBID);
		int nCurTime = (int)time(NULL);
		int nDiffTime = pKinConfig->nJoinKinFreeze * 3600 - (nCurTime - nQuitKinTime);
		if (nQuitKinTime != 0 && nDiffTime > 0)
		{
			char szText[26] = { 0 };
			sprintf_s(szText, "%d", pKinConfig->nJoinKinFreeze);

			g_EHelper.showSystemMessage(PersonPDBID, CHAT_TIP_KIN_LEAVE_TIME_NOT_SATISFY, szText);
			return;
		} 
	}

    // 往场景服发送创建战队消息
    obuf512 obuf;
    fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_CREATE);

    SMsgKin_Create_OS sendData;
    sendData.dwPDBID = PersonPDBID;
    sstrcpyn(sendData.szName, pRecvData->szName, sizeof(sendData.szName));
	sstrcpyn(sendData.szLabel, pRecvData->szLabel, sizeof(sendData.szLabel));
    obuf.push_back(&sendData, sizeof(sendData));

    g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
    
}

void KinService::onClientKinList(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    if(shareProperty.clientID != clientID)
    {
        ErrorLn(__FUNCTION__": pSharePerson == NULL");
        return;    
    }
    bool canApplyJoin = (shareProperty.dwKinID == 0);
    // 把该玩家国家的战队发给玩家
    if (nLen != sizeof(SMsgKin_KinList_CO))
    {
        return;
    }

    char szText[512] = {0};
    SMsgKin_KinList_CO* pRecvData = (SMsgKin_KinList_CO*)pszData;

    // 构建临时表供客户端显示
    list<CKin*> displayKinList;

    if (pRecvData->nState == emKinState_Formal)
    {
        TMap_FormalKin::iterator itr = m_FormalKinArray.begin();
        for (;itr != m_FormalKinArray.end(); ++itr)
        {
            displayKinList.push_back(itr->second);
        }
    }

    list<CKin*>::iterator it = displayKinList.begin();

    SMsgKinRequestKinList_OC headData;
    headData.nKinCount = 0;
    int nIndex = 0;
    // 做下校验 
    if (pRecvData->nPageNum <= 0)
    {
        pRecvData->nPageNum = 1;
    }

    // 节点数据 需要分包
    obuf nodeBufData;
    nodeBufData.clear();
    SMsgKinListNode_OC node;
    while (it != displayKinList.end())
    {
        memset(&node, 0, sizeof(node));

        CKin* pCurKin = *it;
        if (pCurKin != NULL)
        {
            // 判定分包
            if (sizeof(SMsgKinRequestKinList_OC) + nodeBufData.size() + sizeof(SMsgKinListNode_OC) > 8192)
            {
                obuf ob;
                fillKinData(ob, MSG_ENDPOINT_CLIENT, MSG_KIN_LIST);
                ob.push_back(&headData, sizeof(headData));
                ob.push_back(nodeBufData.data(), nodeBufData.size());

                g_EHelper.SendDataToPerson(shareProperty.dwPDBID, ob.data(), ob.size());

                // 清空buff
                nodeBufData.clear();
                headData.nKinCount = 0;
            }

            // 填充每一条记录的数据
            node.dwID           = pCurKin->getNumProp(emKinProp_ID);
            node.nMemberCount   = pCurKin->getMemberCount();
            node.nLevel         = pCurKin->getNumProp(emKinProp_Level);
            node.nState         = pCurKin->getNumProp(emKinProp_State);
            node.nActivity      = pCurKin->getNumProp(emKinProp_Activity);
            node.bIsApply       = (g_KinInviteManager.FindApply(shareProperty.dwPDBID, node.dwID) != NULL);
			node.nWeekActivity  = pCurKin->getNumProp(emKinProp_WeekActivity);
			SAutoAccept* pAutoAccept = pCurKin->getAutoAccept();
            if (pAutoAccept != NULL)
            {
                node.bAutoAccpet = pAutoAccept->bAutoAccept;
                node.nLevelLimit = (BYTE)pAutoAccept->nAcceptLevel;
                node.dwAcceptFlag = pAutoAccept->dwAcceptFlag;
            }
            sstrcpyn(node.szName, pCurKin->getName(), sizeof(node.szName));
            sstrcpyn(node.szShaikhName,  pCurKin->getShaikhName(), sizeof(node.szShaikhName));

            nodeBufData.push_back(&node, sizeof(node));
            headData.nKinCount++;
        }

        ++it;
    }

    // 不需要判断，因为空列表也要通知客户端清空战队列表
    //if (headData.nKinCount == 0 && it == displayKinList.end())
    //{
    //    return;
    //}

    obuf512 ob;
    fillKinData(ob, MSG_ENDPOINT_CLIENT, MSG_KIN_LIST);
    ob.push_back(&headData, sizeof(headData));
    ob.push_back(nodeBufData.data(), nodeBufData.size());
    
    g_EHelper.SendDataToPerson(shareProperty.dwPDBID, ob.data(), ob.size());

    // 添加到战队列表观察者
    addObserve(shareProperty.dwPDBID);
}

// purpose: 更新杯赛列表节点			
void KinService::updateKinList(int nKinID, int nClientID)
{
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
	{
		return;
	}
	SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(nClientID);
	if(pShareReceiver == NULL)
	{
		return;
	}
	if (shareProperty.clientID != nClientID)
	{
		WarningLn(__FUNCTION__": pSharePerson == NULL");
		return;
	}
	CKin* pCurKin = findKin(nKinID);
	if (pCurKin == NULL)
	{
		return;
	}

	SMsgKinListNode_OC node;
	memset(&node, 0, sizeof(node));
	node.dwID           = pCurKin->getNumProp(emKinProp_ID);
	node.nMemberCount   = pCurKin->getMemberCount();
	node.nLevel         = pCurKin->getNumProp(emKinProp_Level);
	node.nState         = pCurKin->getNumProp(emKinProp_State);
	node.nActivity      = pCurKin->getNumProp(emKinProp_Activity);
	node.nWeekActivity = pCurKin->getNumProp(emKinProp_WeekActivity);
	node.bIsApply       = (g_KinInviteManager.FindApply(shareProperty.dwPDBID, node.dwID) != NULL);
	SAutoAccept* pAutoAccept = pCurKin->getAutoAccept();
	if (pAutoAccept != NULL)
	{
		node.bAutoAccpet = pAutoAccept->bAutoAccept;
		node.nLevelLimit = (BYTE)pAutoAccept->nAcceptLevel;
		node.dwAcceptFlag = pAutoAccept->dwAcceptFlag;
	}
	sstrcpyn(node.szName, pCurKin->getName(), sizeof(node.szName));
	sstrcpyn(node.szShaikhName,  pCurKin->getShaikhName(), sizeof(node.szShaikhName));

	obuf512 ob;
	fillKinData(ob, MSG_ENDPOINT_CLIENT, MSG_KIN_UPDATE_LIST);
	ob.push_back(&node, sizeof(node));

	g_EHelper.SendDataToPerson(shareProperty.dwPDBID, ob.data(), ob.size());
}

void KinService::onSceneActorRename(DWORD dwSceneID, LPCSTR pszData, size_t nLen)
{
    // 改名处理
}

void KinService::onSceneActorChangeSex(DWORD dwSceneID, LPCSTR pszData, size_t nLen)
{
    // 改性别处理
}


int KinService::getQuitKinTime(DWORD dwPDBID)
{
    map<DWORD, int>::iterator it = m_UserQuitTimeTable.find(dwPDBID); 
    if (it == m_UserQuitTimeTable.end())
    {
        return 0;
    }

    return (*it).second;
}

/// purpose: 设置玩家离开战队的时间
void KinService::setQuitKinTime(DWORD dwPDBID, int nTime)
{
    map<DWORD, int>::iterator it = m_UserQuitTimeTable.find(dwPDBID); 
    if (it == m_UserQuitTimeTable.end())
    {
        m_UserQuitTimeTable[dwPDBID] = nTime;
    }
    else
    {
        (*it).second = nTime;
    }
}

/// purpose: 添加战队成员（有则更新）
//@ param  : pMember 成员信息结构
//@ param  : bNeedSaveDB 是否需要存数据库
void KinService::addKinMember(SKinMember* pMember, bool bNeedSaveDB)
{
    if (pMember == NULL )
    {
        return;
    }

    // 取得当前时间
    DWORD dwTime = (DWORD)time(NULL);

    // 防止登陆时间为0
    if (pMember->nLoginTime == 0)
    {
        pMember->nLoginTime = (int)dwTime;
    }
    
    if (dwTime > pMember->dwLastClanCtrbTime && g_EHelper.DiffWeek(dwTime, pMember->dwLastClanCtrbTime))
    {
        // 不同一周，清0
        pMember->nClanCtrb = 0;
        pMember->dwLastClanCtrbTime = dwTime;
    }

    // 添加到全局成员列表
    CKinMember kinMember(pMember);
    m_KinMemberList[pMember->dwPDBID] = kinMember;

    // 存储数据库
    if (bNeedSaveDB == true)
    {
        updateMemberToDB(pMember);
    }

    // 同步到客户端
    updateMemberToClient(pMember);

    // 同步到场景服
    updateMemberToScene(pMember);

    // 更新添加到观察者
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pMember->dwPDBID);

    SEventKin_AddMember_OC sendData;
    sendData.dwUDBID            = pMember->dwUDBID;
    sendData.dwPDBID		    = pMember->dwPDBID;
    sendData.nLevel			    = (BYTE)pMember->nLevel;
    sendData.bIsOnline          = (shareProperty.dwPDBID == pMember->dwPDBID);
    sendData.nLogoutTime	    = pMember->nLogoutTime;
    sendData.nVocation		    = (BYTE)pMember->nVocation;
    sendData.nFaceID            = (BYTE)pMember->nFaceID;

    sendData.nIdentity		    = (BYTE)pMember->nIdentity;
    sendData.dwBattleVal	    = pMember->dwBattleVal;
    sendData.dwSrcBattleVal	    = pMember->dwSrcBattleVal;
    sendData.dwBVOrder		    = (BYTE)pMember->dwBVOrder;
    sendData.dwLastOrder	    = (BYTE)pMember->dwLastOrder;
    sendData.dwState            = (shareProperty.dwPDBID == pMember->dwPDBID)? shareProperty.dwGameState : GAME_STATE_MAX;
    sendData.bySex              = pMember->bySex;
    //段位信息
    sendData.nMatchType	    = pMember->nMatchType;
    sendData.nRankScore	    = pMember->nRankScore;
    sendData.nGrade	        = pMember->nGrade;

    sstrcpyn(sendData.szName, pMember->szName, sizeof(sendData.szName));

    CKin *pKin = findKin(pMember->dwKinID);
    if (pKin == NULL)
    {
        return;
    }
    sstrcpyn(sendData.szKinName, pKin->getName(), sizeof(sendData.szKinName));
	pKin->broadcastToObserve(EVENT_KIN_ADDMEMBER, (LPCSTR)&sendData, sizeof(sendData));
}

/// purpose: 更新成员数据到客户端
//@ param  : pMember 成员的数据指针
//@ param  : nSceneID 不等于-1时，表示通过场景服发到客户端
void KinService::updateMemberToClient(SKinMember* pMember)
{
    obuf512 obuf;
    fillKinData(obuf, MSG_ENDPOINT_CLIENT, MSG_KIN_UPATEMEMBER);
    obuf.push_back(pMember, sizeof(SKinMember));
    g_EHelper.SendDataToPerson(pMember->dwPDBID, obuf.data(), obuf.size());
    
}

void KinService::updateMemberToScene(SKinMember* pMember, CSID ServerID /*= INVALID_SERVER_ID*/)
{
    if (pMember == NULL)
    {
        return;
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pMember->dwPDBID);    
    if (shareProperty.dwPDBID == pMember->dwPDBID)
    {
        SSceneKinMember sendData;
        sendData.dwUDBID = pMember->dwUDBID;
        sendData.dwPDBID = pMember->dwPDBID;
        sendData.nLevel = pMember->nLevel;
        sstrcpyn(sendData.szName, pMember->szName, sizeof(sendData.szName));
        sendData.dwKinID = pMember->dwKinID;
        sendData.nIdentity = pMember->nIdentity;
        sendData.dwPopedomFlag = pMember->dwPopedomFlag;
        sendData.nClanCtrb = pMember->nClanCtrb;
        sendData.nTotalClanCtrb = pMember->nTotalClanCtrb;
        sendData.dwBattleVal	= pMember->dwBattleVal;
        sendData.dwSrcBattleVal	= pMember->dwSrcBattleVal;
        sendData.dwBVOrder		= pMember->dwBVOrder;
        sendData.dwLastOrder	= pMember->dwLastOrder;

        memcpy(&sendData.ExtendData, &pMember->ExtendData, sizeof(SKinMemberData));

        obuf512 obuf;
        fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEMEMBER_SCENE);
        obuf.push_back(&sendData, sizeof(SSceneKinMember));
        g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), ServerID);
    }
}


/// purpose: 查找战队
CKin* KinService::findKin(DWORD dwKinID)
{
    Simple_Atom_Lock loc(&m_mutex);
    TMap_KinIt it = m_KinList.find(dwKinID);
    if (it == m_KinList.end())
    {
        return NULL;
    }

    return (*it).second;
}

SKinMember* KinService::findKinMember(DWORD dwPDBID)
{
    Simple_Atom_Lock loc(&m_mutex);
    TMap_KinMemberIt it = m_KinMemberList.find(dwPDBID);
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return NULL;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);

    if (it == m_KinMemberList.end())
    {
        return NULL;
    }

    SKinMember* pMember = ((*it).second).getContext();

    return pMember;
}

/// purpose: 更新成员数据到数据库
void KinService::updateMemberToDB(SKinMember* pMember)
{
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pMember == NULL || pBEngineService == NULL)
    {
        return;
    }

    // 通知数据库存盘成员数据
    DBREQ_PARAM_UPDATEKINMEMBER memberData;
    memberData.dwPDBID          = pMember->dwPDBID;
    memberData.dwKinID          = pMember->dwKinID;
    memberData.nIdentity        = pMember->nIdentity;
    memberData.dwPopedomFlag    = pMember->dwPopedomFlag;
    memberData.nClanCtrb        = pMember->nClanCtrb;
    memberData.nTotalClanCtrb   = pMember->nTotalClanCtrb;
    memberData.dwLastClanCtrbTime = pMember->dwLastClanCtrbTime;
    memberData.dwBattleVal	    = pMember->dwBattleVal;
    memberData.dwSrcBattleVal   = pMember->dwSrcBattleVal;
    memberData.dwBVOrder	    = pMember->dwBVOrder;
    memberData.dwLastOrder	    = pMember->dwLastOrder;
    memberData.nDayActivity     = pMember->nDayActivity;
    memberData.nWeekActivity    = pMember->nWeekActivity;

    int nExtendLen = sizeof(memberData.szRemark);
    if (sizeof(SKinMemberData) > nExtendLen)
    {
        ErrorLn(__FUNCTION__": sizeof(SKinMemberData) length error! nExtendLen ="<<nExtendLen);
        return;
    }
    memcpy(memberData.szRemark, &(pMember->ExtendData), sizeof(SKinMemberData));

    if (pBEngineService->executeSP(GAMEDB_REQUEST_UPDATE_KINMEMBER, memberData.dwPDBID, (LPCSTR)(&memberData), sizeof(memberData), this) == false)
    {
        ErrorLn(__FUNCTION__": GAMEDB_REQUEST_UPDATE_KINMEMBER failed! ");
    }
}

void KinService::removeKinMember(DWORD dwMemberPDBID, bool bNeedSaveDB)
{
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return ;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwMemberPDBID);

    // 不存在该成员
    TMap_KinMemberIt it = m_KinMemberList.find(dwMemberPDBID);
    if (it == m_KinMemberList.end())
    {
        return;
    }
    SKinMember* pMember = ((*it).second).getContext();

    // 不存在该战队
    IKin* pKin = findKin(pMember->dwKinID);
    if (pKin == NULL)
    {
        ErrorLn(__FUNCTION__": pKin == NULL");
        return;
    }

    if (shareProperty.dwPDBID == dwMemberPDBID)
    {
        // 删除人物头顶战队名
        obuf512 obuf;
        SMsgKin_NameTitle_OS sendData;
        sendData.dwPDBID = dwMemberPDBID;
        sendData.bRemove = true;
        sendData.nLevel = 0;
        fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_NAMETITLE);
        obuf.push_back(&sendData, sizeof(sendData));

        g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), shareProperty.serverID);

        // 通知客户端清空成员数据
        {
            obuf256 ob;
            fillKinData(ob, MSG_ENDPOINT_CLIENT, MSG_KIN_QUIT);
            g_EHelper.SendDataToPerson(dwMemberPDBID, ob.data(), ob.size());
        }

        // 更新战队ID到场景服
        {
            obuf512 obuf;
            SMsgKin_UdateID sendData;
            sendData.dwPDBID = dwMemberPDBID;
            sendData.dwKinID = 0;
            fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEID);
            obuf.push_back(&sendData, sizeof(sendData));
            g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
        }

        // 发送战队ID改变事件
        IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
        if(pEventEngine)
        {
            obuf512 buf;
            SEventKin_UpdateKinID eventData;
            eventData.dwPDBID = dwMemberPDBID;
            eventData.dwKinID = 0;
            buf.push_back(&eventData, sizeof(eventData));
            pEventEngine->FireExecute(EVENT_KIN_UPDATEID, SOURCE_TYPE_KIN, dwMemberPDBID, buf.data(), SIZE_INT(buf.size()) );
        }
    }

    // 解决外网公共区跨服国战收到其它战队烟火的问题(by penglu 2013-07-01)
    // 无论玩家是否在线都需要向场景服通告删除成员
    obuf512 ob;
    SMsgKin_RemoveMember_OS removeMember;
    removeMember.dwPDBID = dwMemberPDBID;
    removeMember.dwKinID = pMember->dwKinID;
    fillKinData(ob, MSG_ENDPOINT_SCENE, MSG_KIN_REMOVEMEMBER_SCENE);
    ob.push_back(&removeMember, sizeof(removeMember));
    g_EHelper.SendDataToSceneSvr(ob.data(), ob.size());

    // 从全局成员列表中删除
    m_KinMemberList.erase(it);

    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService == NULL)
    {
        return;
    }

    // 存储数据库
    if (bNeedSaveDB == true)
    {
        // 通知数据库存盘成员数据
        DBREQ_PARAM_DELETE_KINMEMBER referData;
        referData.dwPDBID = dwMemberPDBID;

        if (pBEngineService->executeSP(GAMEDB_REQUEST_DELETE_KINMEMBER, referData.dwPDBID, (LPCSTR)(&referData), sizeof(referData), this) == false)
        {
            ErrorLn(__FUNCTION__": GAMEDB_REQUEST_DELETE_KINMEMBER failed! dwMemberPDBID ="<<dwMemberPDBID);
        }
    }
}

void KinService::readIniKinPlusData(DWORD dwKinID, bool bAddReadFlag)
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    
    if (pDBEngine == NULL)
    {
        return;
    }

    CKin* pKin = (CKin*)findKin(dwKinID);
    if (pKin == NULL)
    {
        WarningLn("pKin == NULL dwKinID=" << dwKinID);
        return;
    }

    // 如果已经读取数据了，不要再读
    if (pKin->isReadPlusData() == true)
    {
        return;
    }
    pKin->setReadPlusData();

    // 读取战队基本信息请求
    DBREQ_PARAM_READKINBASE readBase;
    readBase.dwKinID = dwKinID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READKINBASE, dwKinID, (LPCSTR)(&readBase), sizeof(readBase), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_READKINBASE request error!");
    }

    // 读取战队成员列表请求
    DBREQ_PARAM_READKINMEMBERLIST readMemberList;
    readMemberList.dwKinID = dwKinID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_KIN_MEMBERLIST, dwKinID, (LPCSTR)(&readMemberList), sizeof(readMemberList), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_READ_KIN_MEMBERLIST request error!");
    }

	// 读取战队杯赛奖励列表请求
	DBREQ_PARAM_READKINLEGENDAWARDLIST readLegendAwardList;
	readLegendAwardList.dwKinID = dwKinID;

	if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_KIN_LEGENDAWARD, dwKinID, (LPCSTR)(&readLegendAwardList), sizeof(readLegendAwardList), this) == false)
	{
		ErrorLn("GAMEDB_REQUEST_READ_KIN_LEGENDAWARD request error!");
	}

	// 读取战队杯赛战绩列表请求
	DBREQ_PARAM_READKINLEGENDWARLIST readLegendWarList;
	readLegendWarList.dwKinID = dwKinID;

	if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_KIN_LEGENDWAR, dwKinID, (LPCSTR)(&readLegendWarList), sizeof(readLegendWarList), this) == false)
	{
		ErrorLn("GAMEDB_REQUEST_READ_KIN_LEGENDWAR request error!");
	}
}

void KinService::onSceneKinRename(DWORD dwSceneID, LPCSTR pszData, size_t nLen)
{
    if (nLen != sizeof(SMsKin_Rename_SO))
    {
        //ErrorLn("sizeof(SMsKin_Rename_SO):" << sizeof(SMsKin_Rename_SO) << " is not equal to nLen:" << nLen);
        return;
    }

    if(pszData == NULL)
    {
        return;
    }

    SMsKin_Rename_SO *pRecvData = (SMsKin_Rename_SO*)pszData;

    CKin *pKin = findKin(pRecvData->dwKinID);
    if(pKin->rename(pRecvData->szName, strlen(pRecvData->szName), pRecvData->nRenameTime))
    {
        m_NameKinList[pRecvData->szName] = pKin;
    }
}

void KinService::onSceneKinActorPropertyUpdate(DWORD dwSceneID, LPCSTR pszData, size_t nLen)
{
    // 人物属性有改变单独广播到场景服 客户端 更新一条战队人员数据


}

void KinService::DBDismissKin(DWORD dwKinID)
{
    // 往数据库提交解散战队请求
	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
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
		ErrorLn(__FUNCTION__": ExecuteSP GAMEDB_REQUEST_CENTERDBDISMISSKIN failed，dwKinID = " << dwKinID);
		return;
	}

	// 向游戏数据库提交解散战队请求
	DBREQ_PARAM_DISMISSKIN referData;
	referData.dwKinID = dwKinID;

	if (pDBEngine->executeSP(GAMEDB_REQUEST_DISMISSKIN, dwKinID, (LPCSTR)(&referData), sizeof(referData), this) == false)
	{
		ErrorLn(__FUNCTION__": ExecuteSP GAMEDB_REQUEST_DISMISSKIN failed! dwKinID = "<<dwKinID);
	}
}

void KinService::removeKin(DWORD dwKinID)
{
    string strName;
    DWORD dwCountryID = -1;
    TMap_KinIt it = m_KinList.find(dwKinID);
    if (it != m_KinList.end())
    {
        CKin* pKin = ((*it).second);
        strName = pKin->getName();
        m_KinList.erase(it);
    }
    else
    {
        return;
    }

    TMap_NameKinIt itName = m_NameKinList.find(strName);
    if (itName != m_NameKinList.end())
    {
        m_NameKinList.erase(itName);
    }

    // 从正式战队中查找删除
    m_FormalKinArray.erase(dwKinID);

}

void KinService::onMemberLogin(DWORD dwKinID, DWORD dwMemberPDBID)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwMemberPDBID);
    if (shareProperty.dwPDBID != dwMemberPDBID)
    {
        ErrorLn(__FUNCTION__": pSharePerson == NULL")
        return;
    }

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    SKinMember* pMember = findKinMember(dwMemberPDBID);
    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if (pMember == NULL ||  pConfig == NULL)
    {
        return;
    }
    pMember->nLoginTime = (int)time(NULL);

    // 如果该战队不是非正式战队，通知客户端刷新头顶战队名字
    CKin* pKin = findKin(dwKinID);
    if (pKin == NULL)
    {
        WarningLn(__FUNCTION__": the kin object not exist! dwKinID =" << dwKinID<<" dwMemberPDBID = "<<dwMemberPDBID);
        return;
    }

    int nState = pKin->getNumProp(emKinProp_State);
    if (nState != emKinState_Invaild && nState != emKinState_Informal)
    {
        obuf512 obuf;
        SMsgKin_NameTitle_OS sendData;
        sendData.bRemove = false;
        sendData.dwPDBID = pMember->dwPDBID;
        sendData.nIdentity = pMember->nIdentity;
        sendData.nLevel = pKin->getNumProp(emKinProp_Level);

        sstrcpyn(sendData.szKinName, pKin->getName(), sizeof(sendData.szKinName));

        fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_NAMETITLE);
        obuf.push_back(&sendData, sizeof(sendData));

        g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
    }

    // 把战队的基本信息发送到客户端
    pKin->sendDataToLoginClient( dwMemberPDBID);

    // 把上线的信息广播到战队观察者
    pMember->bIsOnline = true;
    pMember->dwState = GAME_STATE_IDLE;
    UpdatePersonInfoToKinOb(pMember);

    // 下发更新基础数据
    pKin->broadcastKinBaseInfoToOb();

    // 发出成员登陆事件
    SEventKin_MemberLogin loginEvent;
    loginEvent.dwKinID = dwKinID;
    loginEvent.dwPDBID = dwMemberPDBID;
    gSocialGlobal->getEventEngine()->FireExecute(EVENT_KIN_LOGIN, SOURCE_TYPE_KIN, dwMemberPDBID, (LPCSTR)&loginEvent, sizeof(loginEvent));
	pKin->onMemberLogin(shareProperty);
}

void KinService::onMemberLogout(DWORD dwKinID, SKinMember* pMember)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pMember == NULL  || pShareReceiver == NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pMember->dwPDBID);

    // 成员退出游戏
    pMember->nLogoutTime = (int)time(NULL);
    pMember->nLevel = shareProperty.wLevel;
    pMember->nVocation = 0;
    pMember->bIsOnline = false;
    pMember->dwState = GAME_STATE_OFFLINE;

    // 发出成员退出事件
    SEventKin_MemberLogout logoutEvent;
    logoutEvent.dwKinID = dwKinID;
    logoutEvent.dwPDBID = pMember->dwPDBID;
    gSocialGlobal->getEventEngine()->FireExecute(EVENT_KIN_LOGOUT, SOURCE_TYPE_KIN, pMember->dwPDBID, (LPCSTR)&logoutEvent, sizeof(logoutEvent));

    // 移除战队列表观察者
    removeObserve(pMember->dwPDBID,true);

    // 下发更新基础数据
    UpdatePersonInfoToKinOb(pMember);
}


/// purpose: 往场景服刷新战队的信息
bool KinService::refreshKinInfoToScene(CSID dwServerID)
{

    // 通知该场景服删除战队数据
    {
        obuf512 obuf;
        fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_CLEAR_SCENE);
        g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), dwServerID);
    }

    // 把战队的所有数据发到场景服
    TMap_KinIt itKin;
    for (itKin = m_KinList.begin(); itKin != m_KinList.end(); ++ itKin)
    {
        CKin* pKin = (*itKin).second;
        pKin->updateDataToScene(dwServerID);
    }

    // 把所以在线的成员数据都发到场景服
    TMap_KinMemberIt itMember;
    for (itMember = m_KinMemberList.begin(); itMember != m_KinMemberList.end(); ++ itMember)
    {
        updateMemberToScene(((*itMember).second).getContext(), dwServerID);
    }

    return true;
}

IKin* KinService::getKin(DWORD dwKinID)
{
    return (IKin *)findKin(dwKinID);
}

SKinInfo KinService::getKinInfo(DWORD dwKinID)
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

SKinMember* KinService::getKinMember(DWORD dwPBID)
{
    return findKinMember(dwPBID);
}

DWORD KinService::getKinMemberList(DWORD dwKinID, PDBID* pReturnArray,DWORD dwArrayMaxSize)
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


void KinService::OnLogin(ISharePerson * pSharePerson, ELoginMode nLineType)
{
    // 玩家正常登陆
    if (pSharePerson == NULL)
    {
        return;
    }
    
    if (elogin_online != nLineType)
    {
        return;
    }

    PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);

    // 查找战队
    SKinMember *pKinMember = findKinMember(PersonDBID);
    if (pKinMember == NULL)
    {
        if (pSharePerson->GetNumProp(PROPERTY_KIN) > 0)
        {
            //通知场景服更新战队ID
            {
                obuf512 obuf;
                SMsgKin_UdateID sendData;
                sendData.dwPDBID = PersonDBID;
                sendData.dwKinID = 0;
                fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEID);
                obuf.push_back(&sendData, sizeof(sendData));
                g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), pSharePerson->GetServerID());
            }
        }
        return;
    }

    onMemberLogin(pKinMember->dwKinID, PersonDBID);

}

void KinService::OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType)
{
    // 玩家登出

    if (elogout_cross_process == nLineType || elogout_cross_gameworld == nLineType || elogout_in_process == nLineType)
    {
        return;
    }

    if (pSharePerson == NULL)
    {
        return;
    }
    PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);

    SKinMember *pKinMember = findKinMember(PersonDBID);
    if (pKinMember == NULL)
    {
        return;
    }

    onMemberLogout(pKinMember->dwKinID, pKinMember);

	CKin* pKin = findKin(pKinMember->dwKinID);
	if (pKin == NULL)
	{
		return;
	}
	pKin->onMemberLogout(pSharePerson, nLineType);
}

void KinService::OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
    
    return;
}

void KinService::OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
    PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);
    if (PersonDBID == INVALID_PDBID)
    {
        return;
    }
    // 属性更新
    switch(nUpdateReason)
    {
    case SHAREUPDATE_REASON_CHANGE_GAMESTATE:
        {
            // 广播给客户端更新游戏状态
            CKin* pKin = findKin(pSharePerson->GetNumProp(PROPERTY_KIN));
            if (pKin == NULL)
            {

                return;
            }

            // 如果您已经加入了其他战队，不处理
            SKinMember* pMember = findKinMember(PersonDBID);
            if (pMember == NULL)
            {
                return;
            }
            pMember->dwState = pSharePerson->GetGameState();
            pMember->bIsOnline = pMember->dwState != GAME_STATE_OFFLINE;
            // 更新到自己
            updateMemberToClient(pMember);
            // 更新到观察者
            UpdatePersonInfoToKinOb(pMember);
        }
        break;
    case SHAREUPDATE_REASON_CHANGE_FIGHT:
        {
            // 广播给客户端更新游戏状态
            CKin* pKin = findKin(pSharePerson->GetNumProp(PROPERTY_KIN));
            if (pKin == NULL)
            {

                return;
            }

            // 如果您已经加入了其他战队，不处理
            SKinMember* pMember = findKinMember(PersonDBID);
            if (pMember == NULL)
            {
                return;
            }
            pMember->dwSrcBattleVal = pMember->dwBattleVal;
            pMember->dwBattleVal = pSharePerson->GetNumProp(PROPERTY_FIGHTCAPACITY);
            //更新到数据库
            updateMemberToDB(pMember);
            //更新到自己
            updateMemberToClient(pMember);
            //更新到观察者
            UpdatePersonInfoToKinOb(pMember);
        }
        break;
	case SHAREUPDATE_REASON_CHANGE_ACTORNAME:
		{
			// 广播给客户端更新玩家名字
			CKin * pKin = findKin(pSharePerson->GetNumProp(PROPERTY_KIN));
			if (pKin == NULL)
			{
				return;
			}
			SKinMember* pMember = findKinMember(PersonDBID);
			if (pMember == NULL)
			{
				return;
			}

			sstrcpyn(pMember->szName, pSharePerson->GetName(), sizeof(pMember->szName));
			//更新到自己
			updateMemberToClient(pMember);
			//更新到观察者
			UpdatePersonInfoToKinOb(pMember);

			// 如果是队长 则改变队长名字
			if (pMember->nIdentity != emKinIdentity_Shaikh)
			{
				return;
			}
			pKin->setShaikhName(pMember->szName);
			pKin->broadcastDataToClient();
		}
        break;
	case SHAREUPDATE_REASON_SET_ACTIVITY:
	{
		SKinMember* pMember = findKinMember(PersonDBID);
		if (pMember == NULL)
		{
			return;
		}

		pMember->nDayActivity = pSharePerson->GetDayActivity();
        pMember->nWeekActivity = pSharePerson->GetWeekActivity();
        updateMemberToDB(pMember);
	}
	break;
	case SHAREUPDATE_REASON_CHANGE_LEVEL:
		{ 
			CKin* pKin = findKin(pSharePerson->GetNumProp(PROPERTY_KIN));
			if (pKin == NULL)
			{
				return;
			}

			SKinMember* pMember = findKinMember(PersonDBID);
			if (pMember == NULL)
			{
				return;
			}
			pMember->nLevel = pSharePerson->GetLevel();
			
			updateMemberToClient(pMember);
			UpdatePersonInfoToKinOb(pMember);
		}
		break;
    default:
        break;
    }

    return;
}

////////////////////////////////ITransmitHandler//////////////////////////////////////////
/** 消息处理
@param server	源服务器ID
@param uMsgID	消息ID
@param head		消息头
@param data		消息
@param len		消息长度
*/
void KinService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    KinHelper kinHelper;
    IKinService *pKinService = kinHelper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pKinService->handleServerMsg( server, *head, pkg );
}

////////////////////////////////IMessageHandler//////////////////////////////////////////
/** 消息处理
@param msg 网络消息
*/
void KinService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{
    KinHelper kinHelper;
    IKinService *pKinService = kinHelper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pKinService->handleClientMsg( clientID, *head, pkg );
}


void KinService::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    size_t len = msg->size();
    void *data = (void *)msg->c_str();
    // 服务器转发过来的消息
    if (data == NULL || len < sizeof(SMsgKinDataSubMsg))
    {
        return;
    }
    SMsgKinDataSubMsg * pMsg = (SMsgKinDataSubMsg *)(data);

    LPSTR pMsgData = (LPSTR)data + sizeof(SMsgKinDataSubMsg);
    size_t nNewlen = len - sizeof(SMsgKinDataSubMsg);    

    // 服务器发过来的消息
    switch ( head.byKeyAction )
    {
    case emMsgKin_Module:
        {
            onModuleMessage(serverID, pMsg->dwMsgCode, &head, pMsgData, nNewlen);
        }
        break;

    case emMsgKin_Kin:
        {
            CKin* pKin = findKin(pMsg->dwKinID);
            if (pKin != NULL)
            {
                SKinMsgHead KinMsgHead;
                KinMsgHead.dwSrcPointID = serverID;
                KinMsgHead.nSrcEndPoint = head.bySrcEndPoint;
                pKin->onMessage(pMsg->dwMsgCode, &KinMsgHead, pMsgData, nNewlen);
            }
        }
        break;

    default:
        {
            // 其他场景服消息
            ErrorLn(__FUNCTION__": unknown byKeyAction="<< head.byKeyAction);
        }
    }
}

void KinService::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    size_t len = msg->size();
    void *data = (void *)msg->c_str();
    if (data == NULL || len < sizeof(SMsgKinDataSubMsg))
    {
        return;
    }
    SMsgKinDataSubMsg * pMsg = (SMsgKinDataSubMsg *)(data);

    LPSTR pMsgData = (LPSTR)data + sizeof(SMsgKinDataSubMsg);
    size_t nNewlen = len - sizeof(SMsgKinDataSubMsg);


    // 服务器发过来的消息
    switch ( head.byKeyAction )
    {
    case emMsgKin_Module:
        {
            onModuleMessage(client, pMsg->dwMsgCode, &head, pMsgData, nNewlen);
        }
        break;

    case emMsgKin_Kin:
        {
            CKin* pKin = findKin(pMsg->dwKinID);
            if (pKin != NULL)
            {
                SKinMsgHead KinMsgHead;
                KinMsgHead.dwSrcPointID = client;
                KinMsgHead.nSrcEndPoint = head.bySrcEndPoint;
                pKin->onMessage(pMsg->dwMsgCode, &KinMsgHead, pMsgData, nNewlen);
            }
        }
        break;

    default:
        {
            // 其他场景服消息
            ErrorLn(__FUNCTION__": unknown byKeyAction= "<< head.byKeyAction);
        }
    }
}

/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void KinService::onServerListUpdated()
{

}

/** 通知服务器信息更新
@param nState		: SERVER_STATE
@param pServerData	: ServerData
@brief        : 例如服务器人数变化等等
*/
void KinService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{
    ServerData *pInfo = (ServerData *)pServerData;
    if (nState == SERVER_CONNECTED && pInfo->wServerType == MSG_ENDPOINT_SCENE)
    {
        // 往单个场景服更新成交记录
        refreshKinInfoToScene(ServerID);
    }
    else if (nState == SERVER_CONNECTED && pInfo->wServerType == MSG_ENDPOINT_SOCIAL)
    {
        refreshKinInfoToScene();
    }
    return;
}


void KinService::UpdatePersonRank(void* pEventData)
{
    SEventOrder_UpdateRank* pEvent = (SEventOrder_UpdateRank*)pEventData;
    if (pEvent->ActorID == INVALID_PDBID)
    {
        return;
    }

    //查找人物的信息
    SKinMember* pMember = findKinMember(pEvent->ActorID);
    if (pMember == NULL)
    {
        return;
    }

    pMember->nGrade = pEvent->nGrade;
    pMember->nMatchType = pEvent->nMatchType;
    pMember->nRankScore = pEvent->nRankScore;
    // 广播给观察战队的成员
    UpdatePersonInfoToKinOb(pMember);
}

void KinService::UpdatePersonUpdateClanID(void* pEventData)
{
    //更新帮会ID

}


void KinService::UpdatePersonInfoToKinOb(SKinMember* pMember)
{
    // 广播给观察战队的成员
    CKin* pKin = findKin(pMember->dwKinID);
    if (pKin == NULL)
    {
        return;
    }
    // 通知客户端更新成员信息
    SEventKin_UpdateMember_C eventData;
    eventData.dwPDBID = pMember->dwPDBID;
    sstrcpyn(eventData.szName, pMember->szName, sizeof(eventData.szName));
    eventData.nLevel		= pMember->nLevel;
    eventData.nIdentity		= pMember->nIdentity;
    eventData.bIsOnline		= pMember->bIsOnline;
    eventData.nLogoutTime	= pMember->nLogoutTime;
    eventData.nVocation		= pMember->nVocation;
    eventData.nFaceID       = (BYTE)pMember->nFaceID;
    eventData.dwBattleVal	= pMember->dwBattleVal;
    eventData.dwSrcBattleVal = pMember->dwSrcBattleVal;
    eventData.dwBVOrder		= pMember->dwBVOrder;
    eventData.dwLastOrder	= pMember->dwLastOrder;
    eventData.dwState       = pMember->dwState;
    eventData.bySex         = pMember->bySex;

    // 以下是最大段位信息（nGrade）
    eventData.nMatchType    = pMember->nMatchType;
    eventData.nRankScore    = pMember->nRankScore;
    eventData.nGrade        = pMember->nGrade;

    pKin->broadcastToObserve(EVENT_KIN_UPDATEMEMBER, (LPCSTR)&eventData, sizeof(eventData));

    // 把自己的信息也发到客户端
    updateMemberToClient(pMember);

    // 同步该成员到场景服
    updateMemberToScene(pMember);
}

void KinService::ExecSP(DWORD cmdID,LPCSTR pData, int len)
{
    if (pData == NULL || len <= 0)
    {
        return;
    }
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return;
    }

    obuf oBuff;
    oBuff.push_back(pData, len);

    if (pDBEngine->executeSP(cmdID, 0, oBuff.data(), SIZE_INT(oBuff.size()), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<cmdID<<" failed");
    }
}


/// purpose: 添加观察者
bool KinService::addObserve(DWORD dwPDBID)
{
    SKinListObserve* pKinObserve = findObserve(dwPDBID);
    if (pKinObserve != NULL)
    {
        pKinObserve->nCount ++;
    }
    else
    {
        SKinListObserve obs;
        obs.dwPDBID = dwPDBID;
        obs.nCount = 1;

        m_mapObserveList[dwPDBID] = obs;
    }

    return true;
}

/// purpose: 删除观察者
void KinService::removeObserve(DWORD dwPDBID, bool bCoerceRemove)
{
    map<DWORD,SKinListObserve>::iterator it = m_mapObserveList.find(dwPDBID);
    if (it != m_mapObserveList.end())
    {
        SKinListObserve* pKinObserve = &(it->second);
        if (pKinObserve->dwPDBID == dwPDBID)
        {
            if (bCoerceRemove == true)
            {
                m_mapObserveList.erase(it);
            }
            else
            {
                pKinObserve->nCount --;

                if (pKinObserve->nCount <= 0)
                {
                    m_mapObserveList.erase(it);
                }
            }
            return;
        }
    }
}

SKinListObserve* KinService::findObserve(DWORD dwPDBID)
{
    /// purpose: 查找观察者
    map<DWORD,SKinListObserve>::iterator it = m_mapObserveList.find(dwPDBID);

    if (it != m_mapObserveList.end())
    {
        SKinListObserve* pKinObserve = &(it->second);;
        return pKinObserve;
    }

    return NULL;
}

void KinService::onClientRemoveKinListOb(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    // 共享信息注册
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    if (shareProperty.clientID != clientID)
    {
        ErrorLn(__FUNCTION__": pSharePerson == NULL");
        return;
    }
    PDBID PersonPDBID = shareProperty.dwPDBID;

    removeObserve(PersonPDBID,true);
}

////////////////////////////////DateHandler//////////////////////////////////////////
void KinService::DateCome(unsigned long nTriggerID)
{
	if (nTriggerID == DCT_DataTrigger_WeekActivityReset)
	{
		AutoDismissLowerActivity();
		ResetWeekActivity();
	}
    else if (nTriggerID == DCT_DataTrigger_AutoSetShaikhState)
    {
        AutoSetShaikhState();
    }
}

bool KinService::addClanCtrb(DWORD dwPDBID, int nClanCtrb)
{
    // 设置帮会贡献度
    SKinMember* pKinMember = findKinMember(dwPDBID);
    if (pKinMember == NULL)
    {
        ErrorLn(__FUNCTION__": pKinMember == NULL dwPDBID = "<<dwPDBID);
        return false;
    }

    if(pKinMember->nClanCtrb + nClanCtrb < 0)
    {
        ErrorLn(__FUNCTION__": have not enough ClanCtrb dwPDBID = "<<dwPDBID);
        return false;
    }

    DWORD dwTime = (DWORD)time(NULL);
    if (dwTime > pKinMember->dwLastClanCtrbTime && g_EHelper.DiffWeek(dwTime, pKinMember->dwLastClanCtrbTime))
    {
        // 不同一周，清0
        pKinMember->nClanCtrb = 0;
    }
    pKinMember->nClanCtrb += nClanCtrb;

    if (nClanCtrb > 0)
    {
        pKinMember->nTotalClanCtrb += nClanCtrb;
    }
    // 设置上次捐赠时间
    pKinMember->dwLastClanCtrbTime = dwTime;

    updateMemberToDB(pKinMember);

    return true;
}

DWORD KinService::getKinMemberCount(DWORD dwKinID)
{
    IKin *pKin = (IKin *)findKin(dwKinID);
    if (pKin == NULL)
    {
        return 0;
    }

    return pKin->getMemberCount();
}

// 获取总战力
DWORD KinService::getTotalFightScore(DWORD dwKinID)
{
    IKin *pKin = (IKin *)findKin(dwKinID);
    if (pKin == NULL)
    {
        return 0;
    }

    return pKin->getTotalFightScore();
}

bool KinService::deleteKinMember(DWORD dwKinID, DWORD ActorID)
{
    IKin *pKin = (IKin *)findKin(dwKinID);
    if (pKin == NULL)
    {
        return 0;
    }

    pKin->delMember(ActorID);
    return true;
}

SKinMember KinService::getKinMemberInfo(DWORD dwPBID)
{
    SKinMember* pKinMemeber = findKinMember(dwPBID);
    if (pKinMemeber == NULL)
    {
        return SKinMember();
    }
    else
    {
        return *pKinMemeber;
    }
}

// purpose: 新增战队杯赛奖励
void KinService::addKinLegendAward(SKinCupAwardNode sAwardNode)
{
	IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
	if (pBEngineService == NULL)
	{
		return;
	}

	CKin* pKin = findKin(sAwardNode.dwKinID);
	if (pKin == NULL)
	{
		return;
	}
	pKin->addKinLegendAward(&sAwardNode);

	// 通知数据库存盘杯赛奖励数据
	DBREQ_PARAM_ADDKINLEGENDAWARD awardData;
	awardData.dwKinID = sAwardNode.dwKinID;
	awardData.llLegendCupID = sAwardNode.llLegengCupID;
	awardData.byCupRank = sAwardNode.byCupRank;
	awardData.nCupConfigID = sAwardNode.nCupConfigID;
	awardData.dwCupEndTime = sAwardNode.dwCupEndTime;
	sstrcpyn(awardData.szCupName, sAwardNode.szCupName, sizeof(awardData.szCupName));

	if (pBEngineService->executeSP(GAMEDB_REQUEST_ADD_KINLEGENDAWARD, awardData.dwKinID, (LPCSTR)(&awardData), sizeof(awardData), this) == false)
	{
		ErrorLn(__FUNCTION__": GAMEDB_REQUEST_ADD_KINLEGENDAWARD failed! ");
	}
}

// purpose: 新增战队杯赛战绩
void KinService::addKinLegendWarResult(SKinWarRecordNode sRecordNode)
{
	IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
	if (pBEngineService == NULL)
	{
		return;
	}

	CKin* pKin1 = findKin(sRecordNode.dwKin1ID);
	CKin* pKin2 = findKin(sRecordNode.dwKin2ID);
	if (pKin1 != NULL)
	{
		pKin1->addKinLegendWarResult(&sRecordNode);
	}
	if (pKin2 != NULL)
	{
		pKin2->addKinLegendWarResult(&sRecordNode);
	}

	// 通知数据库存盘杯赛奖励数据
	DBREQ_PARAM_ADDKINLEGENDWAR warData;
	warData.dwKin1ID = sRecordNode.dwKin1ID;
	warData.dwKin2ID = sRecordNode.dwKin2ID;
	warData.dwWinKin = sRecordNode.dwWinKin;
	warData.llWarID = sRecordNode.llWarID;
	warData.dwBeginTime = sRecordNode.dwBeginTime;
	sstrcpyn(warData.szKin1Name, sRecordNode.szKin1Name, sizeof(warData.szKin1Name));
	sstrcpyn(warData.szKin2Name, sRecordNode.szKin2Name, sizeof(warData.szKin2Name));

	if (pBEngineService->executeSP(GAMEDB_REQUEST_ADD_KINLEGENDWAR, warData.llWarID, (LPCSTR)(&warData), sizeof(warData), this) == false)
	{
		ErrorLn(__FUNCTION__": GAMEDB_REQUEST_ADD_KINLEGENDWAR failed! ");
	}
}

LPCSTR KinService::getIdentityName(int nIdentity)
{
    if (nIdentity == emKinIdentity_Shaikh)
    {
        return "队长";
    }
    else if (nIdentity == emKinIdentity_Elder)
    {
        return "副队长";
    }
    else
    {
        return "成员";
    }
}

// purpose:  战队杯赛荣誉增加
void KinService::addKinLegendGlory(int nKinID, int nNum, int nGloryType)
{
	CKin * pKin = findKin(nKinID);
	if (pKin == NULL)
	{
		return;
	}
	
	pKin->addKinLegendGlory(nNum, nGloryType);
}

// purpose: 取战队杯赛荣誉
int KinService::getKinLegendGlory(int nKinID, int nGloryType)
{
	CKin * pKin = findKin(nKinID);
	if (pKin == NULL)
	{
		return 0;
	}

	return pKin->getKinLegendGlory(nGloryType);
}

int KinService::getWeekActivity(DWORD dwKinID)
{
	CKin * pKin = findKin(dwKinID);
	if (pKin == NULL)
	{
		return 0;
	}

	return pKin->getNumProp(emKinProp_WeekActivity);
}

// 重置周活跃度
void KinService::ResetWeekActivity()
{
	for (TMap_Kin::iterator iter = m_KinList.begin(); iter != m_KinList.end(); ++iter)
	{
		CKin * pKin = iter->second;
		if (pKin)
			pKin->ResetWeekActivity();
	}
}

// 根据周活跃度检查是否需要解散的帮会
void KinService::AutoDismissLowerActivity()
{
	ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
	if (nullptr == pSchemeCenter)
	{
		ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
		return;
	}

	SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	TMap_Kin kinList = m_KinList;
	for (TMap_KinIt it = kinList.begin(); it != kinList.end(); ++it)
	{
		CKin* pKin = ((*it).second);
		pKin->autoDismiss(pConfig->nDismissActivity, pConfig->nDismissWeeks);
	}

	return;
}

void KinService::AutoSetShaikhState()
{
    for (TMap_Kin::iterator iter = m_KinList.begin(); iter != m_KinList.end(); ++iter)
    {
        CKin * pKin = iter->second;
        if (pKin && pKin->getMemberCount() > 1)
        {
            pKin->CheckShaikhState();
        }
    }
}