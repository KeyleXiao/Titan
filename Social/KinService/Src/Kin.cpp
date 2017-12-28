#include "stdafx.h"
#include "Kin.h"
#include "KinService.h"
#include "KinInviteManager.h"
#include "ISocialGlobal.h"
#include "ExternalHelper.h"
#include "IShareServer.h"
#include "ISchemeCenter.h"
#include "KinSchemeDef.h"
#include "EntityDef.h"
#include "ChatHelper.h"
#include "OrderHelper.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "ClanHelper.h"
#include "LegendCupHelper.h"
#include "MailHelper.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"

using namespace Kin_ManagerDef;

bool Less_Member_BattleValue(const SKinMember& member1, const SKinMember& member2)
{
    if (member1.dwBattleVal != member2.dwBattleVal)
    {
        return (member1.dwBattleVal > member2.dwBattleVal);
    }

    return (strncmp(member1.szName, member2.szName, GAME_NAME_MAXSIZE) < 0);
}

/// purpose: 构造函数
CKin::CKin()
{
    m_pKinService = NULL;
	m_bIsInit = false;
	m_dwReadFlag = 0;
	m_dwID = 0;
	memset(m_szName, 0, sizeof(m_szName));
	m_nState = emKinState_Invaild;
	m_nLevel = 0;
	memset(m_szNotice, 0, sizeof(m_szNotice));
	m_nEstablishTime = 0;
	m_nDismissApplyTime = 0;
	m_nShaikhID = 0;
	m_ShaikhState = KinShaikhState_Unknow;
	memset(m_szShaikhName, 0, sizeof(m_szShaikhName));
	memset(m_nElderIDList, 0, sizeof(m_nElderIDList));
	m_nElderNum = 0;
	m_nMemberIDList.clear();
	m_nMemberCount = 0;
	m_nMemberLastLogoutTime = 0;
	m_nApplyIDList.clear();
	memset(m_propAddress, 0, sizeof(m_propAddress));
	m_nEstablishTime = 0;
	m_nMoney = 0;
	m_nActivity = 0;
	m_nWeekActivity = 0;
	m_nWeekCount = 0;
	m_nWorldID = 0;
	m_nFightCount = 0;
	m_nHistoryActivity = 0;
	memset(m_szSlogan, 0, sizeof(m_szSlogan));
	memset(&m_AutoSetting, 0, sizeof(m_AutoSetting));
	m_nWorldID = 0;
    m_EventList.clear();
	memset(&m_szLabel, 0, sizeof(m_szLabel));
	memset(&m_nGlory, 0, sizeof(m_nGlory));
	m_cupAwardVec.clear();
	m_warRecordBaseVec.clear();

	// 成员地址初始化
	m_propAddress[emKinProp_ID] = (int*)&m_dwID;
	m_propAddress[emKinProp_State] = (int*)&m_nState;
	m_propAddress[emKinProp_Level] = &m_nLevel;
	m_propAddress[emKinProp_EstablishTime] = &m_nEstablishTime;
	m_propAddress[emKinProp_DismissApplyTime] = &m_nDismissApplyTime;
	m_propAddress[emKinProp_ShaikhID] = &m_nShaikhID;
	m_propAddress[emKinProp_MemberLastLogoutTime] = &m_nMemberLastLogoutTime;
	m_propAddress[emKinProp_ReadFlag] = (int*)&m_dwReadFlag;
	m_propAddress[emKinProp_Money] = (int*)&m_nMoney;
	m_propAddress[emKinProp_Activity] = (int*)&m_nActivity;
	m_propAddress[emKinProp_WeekActivity] = (int*)&m_nWeekActivity;
	m_propAddress[emKinProp_Place] = (int*)&m_nPlace;
	m_propAddress[emKinProp_WorldID] = (int*)&m_nWorldID;

	m_nSendSloganTime = 0;
    m_bIsReadPlusData = false;
}

/// purpose: 虚构函数
CKin::~CKin()
{

}


void CKin::release()
{
    reCalcBattleValOrder(true);

    delete this;
}

bool CKin::create(KinService *pKinService, LPCSTR pszContext, size_t nLen)
{
    if(pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": pKinService == NULL");
        return false;
    }
    m_pKinService = pKinService;

    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return false;
    }

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return false;
    }

    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();

    // 设置管理器里面的战队ID
    if (nLen != sizeof(SMsgKin_Create_SO))
    {
        return false;
    }

    SMsgKin_Create_SO* pContext = (SMsgKin_Create_SO*)pszContext;
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pContext->dwPDBID);
    m_nLevel = 1;
    addMember(pContext->dwPDBID, emKinIdentity_Shaikh);
    m_dwID = pContext->dwKinID;
    sstrcpyn(m_szName, pContext->szName, sizeof(m_szName));
    m_nEstablishTime = pContext->nEstablishTime;
    m_dwReadFlag = emKinRead_AllData;
    
    m_nShaikhID = pContext->dwPDBID;
    sstrcpyn(m_szShaikhName, pContext->szShaikhName, sizeof(m_szShaikhName));
    m_nActivity = 0;
    m_nWorldID = getThisGameWorldID();
	sstrcpyn(m_szLabel, pContext->szLabel, sizeof(m_szLabel));
    // 设置缺省的自动接收选项
    m_AutoSetting.dwAcceptFlag = emKinAuto_Default;
    m_AutoSetting.nAcceptLevel = 1;
    m_AutoSetting.bAutoAccept = true;
    

    // 初始化战队数据
    if (Init() == false)
    {
        return false;
    }

    // 把战队的基本信息发送到客户端
    updateDataToClient(pContext->dwPDBID);

    // 战队信息更新到场景服
    updateDataToScene();

    // 添加到全局成员列表
    SKinMember kinMember;
    kinMember.dwUDBID = pContext->dwUDBID;
    kinMember.dwKinID = m_dwID;
    kinMember.dwPDBID = pContext->dwPDBID;
    sstrcpyn(kinMember.szName, pContext->szShaikhName, sizeof(kinMember.szName));
    kinMember.nIdentity = emKinIdentity_Shaikh;
    kinMember.dwPopedomFlag = g_EHelper.GetIdentityPopedom(kinMember.nIdentity);
    if (shareProperty.dwPDBID == pContext->dwPDBID)
    {
        kinMember.nVocation     = shareProperty.nVocation;
        kinMember.nLevel        = shareProperty.wLevel;
        kinMember.nLogoutTime   = (int)time(NULL);
        kinMember.nFaceID       = shareProperty.nFaceID;
        kinMember.bySex         = shareProperty.bySex;
        kinMember.dwUDBID       = shareProperty.dwUDBID;
		kinMember.nDayActivity  = shareProperty.nDayActivity;
    }
    kinMember.bIsOnline = (shareProperty.dwPDBID == pContext->dwPDBID);
    kinMember.ExtendData.nReserveData = (int)time(NULL);
    pKinService->addKinMember(&kinMember, true);

    // 发起战队创建事件
    SEventKin_Create eventData;
    eventData.dwKinID = m_dwID;
    gSocialGlobal->getEventEngine()->FireExecute(EVENT_KIN_CREATE, SOURCE_TYPE_KIN, m_dwID, (LPCSTR)&eventData, sizeof(eventData));

    return true;
}

/// purpose: 初始化战队数据
bool CKin::Init()
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return false;
    }

	SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
	if (pConfig  == NULL )
	{
		return false;
	}

    // 默认都是正式的
    m_nState = emKinState_Formal;
	return true;
}

bool CKin::addMember(DWORD dwPDBID, int nIdentity)
{
    list<DWORD>::iterator it = m_nMemberIDList.begin();
    for (; it != m_nMemberIDList.end(); ++ it)
    {
        if (dwPDBID == (*it))
        {
            return true;
        }
    }

    if (nIdentity == emKinIdentity_Shaikh)
    {
        m_nShaikhID = dwPDBID;
    }
    else if (nIdentity == emKinIdentity_Elder)
    {
        if (m_nElderNum >= 0 && m_nElderNum < KIN_ELDER_LEN)
        {
            m_nElderIDList[m_nElderNum ++] = dwPDBID;
        }
    }

    m_nMemberIDList.push_back(dwPDBID);
    m_nMemberCount++;
    return true;
}

void CKin::updateDataToScene(CSID ServerID /*= INVALID_SERVER_ID*/)
{
    // 通知场景服更新战队
    obuf512 ob;
    fillKinData(ob, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEKIN_SCENE);

    SMsgKin_UpdateKinScene_OS sendData;
    sendData.dwID = m_dwID;
    sstrcpyn(sendData.szName, m_szName, sizeof(sendData.szName));
    sendData.nState = m_nState;
    sendData.nLevel = m_nLevel;
    sendData.nActivity = m_nActivity;
	sendData.nWeekActivity = m_nWeekActivity;
    sendData.nMoney = m_nMoney;
    sendData.nShaikhID = m_nShaikhID;
    sendData.nPlace = m_nPlace;
    sendData.nWorldID = m_nWorldID;
    sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
	sstrcpyn(sendData.szKinLabel, m_szLabel, sizeof(sendData.szKinLabel));

    ob.push_back(&sendData, sizeof(sendData));
    g_EHelper.SendDataToSceneSvr(ob.data(), ob.size(), ServerID);
}

void CKin::updateDataToClient(DWORD dwPDBID)
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);
    SMsgKin_UpdateClient_OC sendData;
    sendData.dwID = m_dwID;
    sendData.nState = (BYTE)m_nState;
    sstrcpyn(sendData.szName, m_szName, sizeof(sendData.szName));
    sendData.dwShaikhID = m_nShaikhID;
    sendData.nKinLevel = m_nLevel;
    sendData.nMemberCount = m_nMemberCount;
    sendData.nKinMaxMember = pLevelInfo == NULL ? 0 : pLevelInfo->nMaxMemberCount;
    sendData.nApplyCount = SIZE_INT(m_nApplyIDList.size());
    sendData.nOnlineCount = getOnlineMemberCount();
    sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
	sstrcpyn(sendData.szKinLabel, m_szLabel, sizeof(sendData.szKinLabel));
  
    obuf256 obuf;
    fillKinData(obuf, MSG_ENDPOINT_CLIENT, MSG_KIN_UPDATECLIENT);
    obuf.push_back(&sendData, sizeof(sendData));

    g_EHelper.SendDataToPerson(dwPDBID, obuf.data(), obuf.size());

}

void CKin::onMessage(DWORD dwMsgID, SKinMsgHead * pKinMsgHead, LPCSTR pszMsg, size_t nLen)
{
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    // 如果数据还没读出来，还是禁止操作好点
    // 如果数据还没读出来就不处理了
    if (isPublicGameWorld() == false && 
        ((m_dwReadFlag & emKinRead_AllData) != emKinRead_AllData && dwMsgID != MSG_KIN_APPLY))
    {
        return;
    }

    if (pKinMsgHead->nSrcEndPoint == MSG_ENDPOINT_CLIENT)
    {
        // 验证包的来源是否正确
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(pKinMsgHead->dwSrcPointID);

        if (shareProperty.dwPDBID == INVALID_PDBID)
        {
            return;
        }

        // 如果是公共区
        if (isPublicGameWorld() == true)
        {
            if (dwMsgID != MSG_KIN_KINBASEDATA && dwMsgID != MSG_KIN_ENTERTOBSERVE && dwMsgID != MSG_KIN_EXITOBSERVE)
            {
                return;
            }
        }

        // 处理具体客户端消息
        switch (dwMsgID)
        {
        case MSG_KIN_KINBASEDATA:
            {
                //// 请求战队基本数据 
                onClientKinBaseData(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_APPLY:
            {
                //// 申请加入
                onClientApply(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_ADMIT:
            {
                //// 接收成员 
                onClientAdmit(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_KICK:
            {
                //// 开除成员 
                onClientKick(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_QUIT:
            {
                //// 退出战队 
                onClientQuit(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_DISMISS:
            {
                //// 解散战队 
                onClientDismiss(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_ENTERTOBSERVE:
            {
                //// 进入观察 
                onClientEnterObserve(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_EXITOBSERVE:
            {
                //// 退出观察 
                onClientExitObserve(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_MOTIFY:
            {
                //// 修改公告，宣言
                onClientMotify(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_AUTO_ACCEPT_EDIT:
            {
                //// 编辑自动接收
                OnClientEditAutoAccept(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_AUTO_ACCEPT_REQUEST:
            {
                //// 编辑自动接收
                OnClientRequestAutoAccept(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
        case MSG_KIN_REQUEST_KINEVENT_LIST:
            {
                //// 请求战队事件列表
                OnClientRequestKinEventList(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;
		case MSG_KIN_REQUEST_KINGLORYDATA:
			{
				//// 请求战队荣誉数据
				OnClientRequestKinGloryData(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
			}
			break;
		case MSG_KIN_REQUEST_WARRECORD_TOTAL:
			{
				//// 请求战队总历史战绩
				OnClientRequestWarRecordTotal(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
			}
			break;

            //// 任命职位
        case MSG_KIN_APPOINT:
            {
                //// 任命职位
                OnClientAppoint(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
            }
            break;

		case MSG_KIN_LEGENDCUP_KIN_INFO:
			{
				//// 查看杯赛战队信息
				OnClientLegendCupKinInfo(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
			}
			break;

		case MSG_KIN_LEGENDCUP_KIN_GLORY:
			{
				//// 查看杯赛战队荣誉
				OnClientLegendCupKinGlory(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
			}
			break;
                //// 取消战队解散
                //MSG_CLIENT_CASE(OnClientCanelDismiss, MSG_KIN_CANEL_DISMISS)

                //// 响应战队
                //MSG_CLIENT_CASE(OnClientRespond, MSG_KIN_RESPOND)

                //// 修改成员备注
                //MSG_CLIENT_CASE(OnClientMotifyRemark, MSG_KIN_MODIFYREMARK)

                //// 查看成员信息
                //MSG_CLIENT_CASE(OnClientLookUpMember, MSG_KIN_LOOKUPMEMBER)

                //// 战队升级
                //MSG_CLIENT_CASE(OnClientUpgrade, MSG_KIN_UPGRADE)

                //// 设置是否自动接收成员
                //MSG_CLIENT_CASE(OnClientSetAuto, MSG_KIN_SETAUTOACCEPT)

                //// 编辑口号
                //MSG_CLIENT_CASE(OnClientEditSlogan, MSG_KIN_EDITSLOGAN)

                //// 发送宣传口号
                //MSG_CLIENT_CASE(OnClientSendSlogan, MSG_KIN_SENDSLOGAN)

                //// 请求宣传口号数据
                //MSG_CLIENT_CASE(OnClientSloganRequest, MSG_KIN_SLOGANREQUEST)

                //// 请求留言
                //MSG_CLIENT_CASE(OnClientWordRequest, MSG_KIN_WORDREQUEST)


        default:
            break;
        }
    }
    else if (pKinMsgHead->nSrcEndPoint == MSG_ENDPOINT_SCENE)
    {
        // 处理具体场景服消息
        switch (dwMsgID)
        {
            //// 设置战队属性（场景服）
            //MSG_SCENE_CASE(OnSceneSetNumProp, MSG_KIN_SETPROP_SO)

            //// 成员杀死高级玩家
            //MSG_SCENE_CASE(OnSceneMemberKiller, MSG_KIN_KILLER)

            //// 战队召回反馈
            //MSG_SCENE_CASE(OnSceneToSOMemberLargess, MSG_KIN_LARGESS_SO)

            //// 战队成员更新战力值数据
            //MSG_SCENE_CASE(OnSceneToSOMemberBattleValueSych, MSG_KIN_BATTLEVALE_SO)

		case MSG_KIN_SETPROP_SO:
		{
			// 场景服更新属性
			OnSceneUpdateProp(pKinMsgHead->dwSrcPointID, pszMsg, nLen);
		}
		break;

        default:
            break;
        }
    }
    else
    {
        return;
    }
}

// 场景服更新属性
void CKin::OnSceneUpdateProp(DWORD dwSeverID, LPCSTR pszData, size_t nLen)
{
	if (pszData == NULL || nLen != sizeof(SMsgKin_UpdatePropScene_OS))
	{
		ErrorLn(__FUNCTION__" nLen = " << nLen);
		return;
	}

	SMsgKin_UpdatePropScene_OS *pRecvData = (SMsgKin_UpdatePropScene_OS*)pszData;

	setNumProp(pRecvData->nPropID, pRecvData->nValue, true, true);
}

void CKin::OnClientAppoint(ClientID clientID, LPCSTR pszData, size_t nLen)
{
	SMsgKin_Appoint_CO* pRecvData = (SMsgKin_Appoint_CO*)pszData;

    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
	// 判断任命者是不是队长
	if (shareProperty.dwPDBID != m_nShaikhID)
	{
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_SHAIKH_CAN_APPOINT,"");
		return;
	}

	if (pRecvData->dwMemberPDBID == shareProperty.dwPDBID)
	{
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_CANT_APPOINT_SELF,"");
		return;
	}

	// 判断接收者是否是本战队成员
	SKinMember* pMember = m_pKinService->findKinMember(pRecvData->dwMemberPDBID);
	if (pMember == NULL || pMember->dwKinID != m_dwID)
	{
		return;
	}

	// 如果是队长和副队长
	char szText[128] = {0};
	if (pMember->nIdentity == pRecvData->nIdentity)
	{
        g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_KIN_MEMBER_HAVE_SAME_IDENTITY, a2utf8(m_pKinService->getIdentityName(pRecvData->nIdentity)));
		return;
	}

	if (pRecvData->nIdentity == emKinIdentity_Elder)
	{
		// 如果目前已经有4个副队长了，则返回
		if (m_nElderNum >= KIN_ELDER_LEN)
		{	
			sprintf_s(szText, 128, "%d", KIN_ELDER_LEN);
			g_EHelper.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_KIN_CANT_APPOINT_MORE_IDENTITY, szText);
			return;
		}	
	}

	// 给予玩家职位
	setMemberIdentity(pRecvData->dwMemberPDBID, pRecvData->nIdentity);
	if (pRecvData->nIdentity == emKinIdentity_Member)
	{
		g_EHelper.showSystemMessage(pRecvData->dwMemberPDBID, CHAT_TIP_KIN_APPOINT_LOW_IDENTITY, a2utf8(m_pKinService->getIdentityName(pRecvData->nIdentity)));
	}
	else
	{
		g_EHelper.showSystemMessage(pRecvData->dwMemberPDBID, CHAT_TIP_KIN_APPOINT_UP_IDENTITY, a2utf8(m_pKinService->getIdentityName(pRecvData->nIdentity)));
	}
    //记录职位日志
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        char szText[512] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("战队任职：角色id[%u]角色名[%s]被任职为[%s]战队的[%s]"), pRecvData->dwMemberPDBID, pMember->szName, m_szName, a2utf8(m_pKinService->getIdentityName(pRecvData->nIdentity)));

        IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
        if ( pOSSLogService != NULL )
        {
           pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_KIN, m_dwID, szText);
        }
    }
}

// 设置某个玩家的职位
void CKin::setMemberIdentity(int nPDBID, int nIdentity, char* szReason)
{
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
    if(pEventEngine == NULL)
    {
        return;
    }

    SKinMember* pMember = m_pKinService->findKinMember(nPDBID);
    SKinMember* pOldShaikh = m_pKinService->findKinMember(m_nShaikhID);
    ClanHelper clanHelper;
    
    if (pMember == NULL ||  pOldShaikh == NULL || clanHelper.m_ptr == NULL)
    {
        return;
    }

    DWORD dwOldShaikhID = 0;
    // 如果任命队长，先把以前的队长干掉
    if (nIdentity == emKinIdentity_Shaikh)
    {
        // 把以前的队长变成成员
        
        pOldShaikh->dwPopedomFlag = g_EHelper.GetIdentityPopedom(emKinIdentity_Member);
        pOldShaikh->nIdentity = emKinIdentity_Member;

        // 广播给客户端观察者
        SEventKin_UpdateMember_C UpdateShaikh;
        UpdateShaikh.dwUDBID        = pOldShaikh->dwUDBID;
        UpdateShaikh.dwPDBID        = pOldShaikh->dwPDBID;
        
        UpdateShaikh.nLevel			= pOldShaikh->nLevel;
        UpdateShaikh.nIdentity		= pOldShaikh->nIdentity;
        UpdateShaikh.bIsOnline		= pOldShaikh->bIsOnline ;
        UpdateShaikh.nLogoutTime	= pOldShaikh->nLogoutTime;
        UpdateShaikh.nVocation		= pOldShaikh->nVocation;

        UpdateShaikh.dwBattleVal	= pOldShaikh->dwBattleVal;
        UpdateShaikh.dwSrcBattleVal	= pOldShaikh->dwSrcBattleVal;
        UpdateShaikh.dwBVOrder		= pOldShaikh->dwBVOrder;
        UpdateShaikh.dwLastOrder	= pOldShaikh->dwLastOrder;
        
        UpdateShaikh.nRankScore     = pOldShaikh->nRankScore;
        UpdateShaikh.nMatchType     = pOldShaikh->nMatchType;
        UpdateShaikh.nGrade         = pOldShaikh->nGrade;
        UpdateShaikh.dwState        = pOldShaikh->dwState;

        sstrcpyn(UpdateShaikh.szName, pOldShaikh->szName, ACTORNAME_MAXSIZE);

        broadcastToObserve(EVENT_KIN_UPDATEMEMBER, (LPCSTR)&UpdateShaikh, sizeof(UpdateShaikh));

        // 更新到数据库
        m_pKinService->updateMemberToDB(pOldShaikh);

        // 同步到客户端
        m_pKinService->updateMemberToClient(pOldShaikh);

        // 同步到场景服
        m_pKinService->updateMemberToScene(pOldShaikh);

        // 同步到公共区

        // 更新场景服称号
        SSharePersonProperty ShaikhInfo = pShareReceiver->GetSharePerson(pOldShaikh->dwPDBID);
        if (ShaikhInfo.dwPDBID != INVALID_PDBID)
        {
            obuf128 obuf;
            SMsgKin_NameTitle_OS sendData;
            sendData.bRemove = false;
            sendData.dwPDBID = pOldShaikh->dwPDBID;
            sendData.nIdentity = pOldShaikh->nIdentity;
            sendData.nLevel = getNumProp(emKinProp_Level);
            sstrcpyn(sendData.szKinName, m_szName, sizeof(sendData.szKinName));
            fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_NAMETITLE);
            obuf.push_back(&sendData, sizeof(sendData));
            g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), ShaikhInfo.serverID);

            //if (pShaikhInfo->tribe > 0)
            //{
            //    // 帮会的也同步到客户端
            //    pClanService->UpdateMemberToClient_OC(pShaikhInfo->tribe, pOldShaikh);
            //}
        }

        dwOldShaikhID = m_nShaikhID;
        m_nShaikhID = nPDBID;
        SKinMember* pKinMember = m_pKinService->findKinMember(nPDBID);
        if (pKinMember != NULL)
        {
            sstrcpyn(m_szShaikhName, pKinMember->szName, sizeof(m_szShaikhName));
        }

        // 更新属性到客户端
        SEventKin_UpdateNumProp_C eventData;
        eventData.nPropID = emKinProp_ShaikhID;
        eventData.nValue = m_nShaikhID;
        broadcastToObserve(EVENT_KIN_UPDATE_NUMPROP, (LPCSTR)&eventData, sizeof(eventData));

        // 更新帮主名字到客户端
        SEventKin_UpdateStringProp_C shaikhNameEvent;
        shaikhNameEvent.nPropID = emKinShaikhName;
        sstrcpyn(shaikhNameEvent.szValue, m_szShaikhName, sizeof(shaikhNameEvent.szValue));
        broadcastToObserve(EVENT_KIN_UPDATE_STRINGPROP, (LPCSTR)&shaikhNameEvent, sizeof(shaikhNameEvent));

        // 更新属性到场景服
        obuf128 ob;
        fillKinData(ob, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEKINPROP_SCENE);
        SMsgKin_UpdatePropScene_OS sendData;
        sendData.dwKinID = m_dwID;
        sendData.nPropID = emKinProp_ShaikhID;
        sendData.nValue = m_nShaikhID;
        ob.push_back(&sendData, sizeof(sendData));
        g_EHelper.SendDataToSceneSvr(ob.data(), ob.size());
    }
    // 副队长不能超过4个
    else if (nIdentity == emKinIdentity_Elder)
    {
        if (m_nElderNum >= KIN_ELDER_LEN)
        {
            return;
        }

        m_nElderIDList[m_nElderNum ++] = nPDBID;
        TraceLn("[SetMemberIdentity] m_nElderNum = " << m_nElderNum);
    }

    // 给予玩家新职位
    int nOldIdentity = pMember->nIdentity; 
    pMember->nIdentity = nIdentity;
    pMember->dwPopedomFlag = g_EHelper.GetIdentityPopedom(nIdentity);

    // 如果该玩家原来是副队长
    if (nOldIdentity == emKinIdentity_Elder)
    {
        removeElder(pMember->dwPDBID);
    }

    
    if(nIdentity == emKinIdentity_Shaikh)
    {
        // 发出队长改变事件
        SEventKin_ChangeShaikh changeShaikh;
        changeShaikh.dwKinID = m_dwID;
        changeShaikh.dwOldShaikh = dwOldShaikhID;
        changeShaikh.dwNewShaikh = m_nShaikhID;
        pEventEngine->FireExecute(EVENT_KIN_CHANGE_SHAIKH, SOURCE_TYPE_KIN, m_dwID, (LPCSTR)&changeShaikh, sizeof(changeShaikh));
    }

    if( nIdentity == emKinIdentity_Elder )
    {
        // 发出副队长改变事件
        SEventKin_ChangeElder eventData;
        eventData.bAdd		= true;			// ture是添加		
        eventData.dwPDBID	= nPDBID;		// 副队长PDBID
        pEventEngine->FireExecute(EVENT_KIN_CHANGE_ELDER, SOURCE_TYPE_KIN, m_dwID, (LPCSTR)&eventData, sizeof(eventData));
    }

    // 更新到数据库
    m_pKinService->updateMemberToDB(pMember);
    // 同步到客户端
    m_pKinService->updateMemberToClient(pMember);

    // 同步到场景服
    m_pKinService->updateMemberToScene(pMember);

    // 同步到公共区
   

    // 广播给客户端观察者
    SEventKin_UpdateMember_C eventData;
	eventData.dwUDBID = pMember->dwUDBID;
    eventData.dwPDBID = pMember->dwPDBID;
    sstrcpyn(eventData.szName, pMember->szName, ACTORNAME_MAXSIZE);
    eventData.nLevel		= pMember->nLevel;
    eventData.nIdentity		= pMember->nIdentity;
    eventData.bIsOnline		= pMember->bIsOnline;
    eventData.nLogoutTime	= pMember->nLogoutTime;
    eventData.nVocation		= pMember->nVocation;

    eventData.dwBattleVal	= pMember->dwBattleVal;
    eventData.dwSrcBattleVal= pMember->dwSrcBattleVal;
    eventData.dwBVOrder		= pMember->dwBVOrder;
    eventData.dwLastOrder	= pMember->dwLastOrder;
   
    eventData.nRankScore    = pMember->nRankScore;
    eventData.nMatchType    = pMember->nMatchType;
    eventData.nGrade        = pMember->nGrade;
    eventData.dwState       = pMember->dwState;

    broadcastToObserve(EVENT_KIN_UPDATEMEMBER, (LPCSTR)&eventData, sizeof(eventData));
    // 通知场景服更改称号
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pMember->dwPDBID);
    if (shareProperty.dwPDBID != INVALID_PDBID)
    {
        obuf512 obuf;
        SMsgKin_NameTitle_OS sendData;
        sendData.bRemove = false;
        sendData.dwPDBID = pMember->dwPDBID;
        sendData.nIdentity = pMember->nIdentity;
        sendData.nLevel = getNumProp(emKinProp_Level);
        sstrcpyn(sendData.szKinName, m_szName, sizeof(sendData.szKinName));
        fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_NAMETITLE);
        obuf.push_back(&sendData, sizeof(sendData));
        g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), shareProperty.serverID);	

        //if (pUserInfo->tribe > 0)
        //{
        //    // 帮会的也同步到客户端
        //    pClanService->UpdateMemberToClient_OC(pUserInfo->tribe, pMember);
        //}
    }

    if (szReason)
    {
        SKinEvent kinEvent;
        kinEvent.tTime = ConvertTimeValueToStruct(time(NULL));
        sstrcpyn(kinEvent.szCoutext, szReason, sizeof(kinEvent.szCoutext));
        AddKinEvent(&kinEvent);
    }
}

void CKin::onClientApply(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }
    ISchemeKinDataCenter *pKinDataCenter = pSchemeCenter->getSchemeKinDataCenter();
    if(pKinDataCenter == NULL)
    {
        ErrorLn(__FUNCTION__" pKinDataCenter == NULL");
        return;
    }
    SKinLevel *pLevelInfo = pKinDataCenter->getLevelInfo(m_nLevel);
    if (pLevelInfo == NULL)
    {
        ErrorLn(__FUNCTION__" pLevelInfo == NULL m_nLevel ="<<m_nLevel<<" KinID ="<<m_dwID);
        return;
    }

    if (nLen != sizeof(SMsgKin_Apply_CO))
    {
        return;
    }

    SMsgKin_Apply_CO* pRecvData = (SMsgKin_Apply_CO*)pszData;
    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);

    if( pConfig == NULL || pShareReceiver == NULL)
    {
        return;
    }

    if (shareProperty.clientID != clientID)
    {
        WarningLn(__FUNCTION__": pSharePerson == NULL");
        return;
    }

    // 是否为低等级
    PDBID ApplyPersonPDBID = shareProperty.dwPDBID;
    int nQuitKinTime = m_pKinService->getQuitKinTime(ApplyPersonPDBID);
    int nCurTime = (int)time(NULL);
    int nDiffTime = pConfig->nJoinKinFreeze * 3600 - (nCurTime -  nQuitKinTime);
    if ( nDiffTime > 0)
    {
        char szText[26] = {0};
        sprintf_s(szText, "%d", pConfig->nJoinKinFreeze);
        g_EHelper.showSystemMessage(ApplyPersonPDBID,CHAT_TIP_KIN_LEAVE_TIME_NOT_SATISFY,szText);
        return;
    }

    // todo 本战队是否已经满人了
     if (m_nMemberIDList.size() >= pLevelInfo->nMaxMemberCount)
    {
        g_EHelper.showSystemMessage(ApplyPersonPDBID, CHAT_TIP_KIN_MEMBER_FULL, "");
        return;
    }

    if (find(m_nApplyIDList.begin(),m_nApplyIDList.end(), ApplyPersonPDBID) != m_nApplyIDList.end())
    {
        g_EHelper.showSystemMessage(ApplyPersonPDBID,CHAT_TIP_KIN_REPEAT_APPLY,"");
        return;
    }

    if (m_nApplyIDList.size() >= 10)
    {
        g_EHelper.showSystemMessage(ApplyPersonPDBID,CHAT_TIP_KIN_APPLY_OVER,"");
       
        TraceLn(__FUNCTION__": have apply this kin count over! try other kin! ApplyPersonPDBID = "<<ApplyPersonPDBID);
        return;
    }

    // 申请者是否已经加入了其他战队
    SKinMember* pMember = m_pKinService->findKinMember(ApplyPersonPDBID);
    if (pMember != NULL)
    {
        g_EHelper.showSystemMessage(ApplyPersonPDBID,CHAT_TIP_KIN_HAD_JOINED_KIN_MUST_QUIT,"");
        return;
    }
 
    int nLimitCount = 10;
    if (g_KinInviteManager.GetApplyKinCount(ApplyPersonPDBID) >= nLimitCount)
    {
        g_EHelper.showSystemMessage(ApplyPersonPDBID,CHAT_TIP_KIN_APPLY_TIMES_OVER,"");
        return;
    }

    // 把玩家加入申请列表
    addApplyMember(ApplyPersonPDBID);
    SKinApply kinApply;
    kinApply.dwUDBID        = shareProperty.dwUDBID;
    kinApply.dwApplyKinID	= m_dwID;
    kinApply.dwApplyPDBID	= ApplyPersonPDBID;
    sstrcpyn(kinApply.szApplyName, shareProperty.szName, sizeof(kinApply.szApplyName));
    kinApply.nLevel			= (BYTE)shareProperty.wLevel;
    kinApply.nVocation		= shareProperty.nVocation;
    kinApply.nNation		= (BYTE)0;
    kinApply.nFaceID        = shareProperty.nFaceID;
    kinApply.nSex           = shareProperty.bySex;
    sstrcpyn(kinApply.szWord, pRecvData->szWord, sizeof(kinApply.szWord));
    g_KinInviteManager.AddApply(&kinApply);

    // 提示申请加入战队
    g_EHelper.showSystemMessage(ApplyPersonPDBID,CHAT_TIP_KIN_APPLY_SUCCESS,"");
    // 接收相关
    if( m_AutoSetting.bAutoAccept )
    {
        // 如果当前没有读数据
        if( m_dwReadFlag != emKinRead_AllData )
        {
            g_KinInviteManager.RemoveApply(ApplyPersonPDBID, m_dwID);
            return ;
        }

        autoAccept(ApplyPersonPDBID);
    }
    else
    {
        // 往客户端发出添加申请消息 弹窗
        SEventKin_AddApply_OC sendData;
        sendData.dwPDBID = ApplyPersonPDBID;
        sendData.nLevel	 = (BYTE)shareProperty.wLevel;
        sendData.dwPower = shareProperty.nFight;
        sendData.nVocation = kinApply.nVocation;
        sstrcpyn(sendData.szName, shareProperty.szName, sizeof(sendData.szName));

        broadcastToHaveRightMember(emKinPopedom_AcceptMember, EVENT_KIN_ADDAPPLY, (LPCSTR)&sendData, sizeof(sendData));
		m_pKinService->updateKinList(m_dwID, clientID);
    }
}


/// purpose: 添加申请中成员
//@ note   : 不处理全局成员列表和数据
void CKin::addApplyMember(DWORD dwPDBID)
{
    // 添加申请中成员
    m_nApplyIDList.remove(dwPDBID);
    m_nApplyIDList.push_back(dwPDBID);
}

/// purpose: 编辑自动收人设置											-- 来自客户端
void CKin::OnClientEditAutoAccept(ClientID clientID, LPCSTR pszData, size_t nLen)
{
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

    PDBID dwPDBID = shareProperty.dwPDBID;

    if (nLen != sizeof(SMsgKin_EditAutoAccept))
    {
        return;
    }

    SMsgKin_EditAutoAccept* pEditData = (SMsgKin_EditAutoAccept*)pszData;
    if (pEditData->nAcceptLevel >= PERSON_MAX_LEVEL)
    {

        char szText[256] = {0};
        sprintf_s(szText, "等级限制最高等级不能大于[%d]级。", PERSON_MAX_LEVEL);
        g_EHelper.showSystemMessage(dwPDBID,CHAT_TIP_LOGIN_SERVER_MESSAGE,szText);
        return;
    }
    m_AutoSetting.bAutoAccept = pEditData->bAutoAccept;
    m_AutoSetting.dwAcceptFlag = pEditData->dwAcceptFlag;
    m_AutoSetting.nAcceptLevel = pEditData->nAcceptLevel;
    //保存到数据库
    updateDataToDB(true);

    // 通知玩家
    g_EHelper.showSystemMessage(dwPDBID,CHAT_TIP_KIN_AUTOACCEPT_SAVE_SUCCESS,"");

    // 把数据更新下去
    SEventKin_EditAutoAccept eventData;
    eventData.bAutoAccept = m_AutoSetting.bAutoAccept;
    eventData.nAcceptLevel = m_AutoSetting.nAcceptLevel;
    eventData.dwAcceptFlag = m_AutoSetting.dwAcceptFlag;
    broadcastToObserve(EVENT_KIN_EDITAUTOACCEPT, (LPCSTR)&eventData, sizeof(eventData));

    // 把数据保存到前端
    updateDataToDB(false);
}

void CKin::OnClientRequestAutoAccept(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    if (shareProperty.clientID  !=  clientID)
    {
        return;
    }

    PDBID dwPDBID = shareProperty.dwPDBID;
  
    // 把数据更新下去
    SEventKin_AutoAcceptRequest eventData;
    eventData.bAutoAccept = m_AutoSetting.bAutoAccept;
    eventData.nAcceptLevel = m_AutoSetting.nAcceptLevel;
    eventData.dwAcceptFlag = m_AutoSetting.dwAcceptFlag;

    g_EHelper.FireClientEvent(dwPDBID, EVENT_KIN_AUTOACCEPTREQUEST, SOURCE_TYPE_KIN, m_dwID, (LPCSTR)&eventData, sizeof(SEventKin_AutoAcceptRequest));
}



bool CKin::autoAccept(DWORD dwPDBID)
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return false;
    }

    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return false;
    }


    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID != dwPDBID)
    {
        return false;
    }

    int nSex = shareProperty.bySex;
    int nSexFlag = 0;

    if (nSex == SEX_MAX || nSex == SEX_MALE)
    {
        nSexFlag = emKinAuto_Male;
    }
    else
    {
        nSexFlag = emKinAuto_Female;
    }

    // todo 判断条件是否符合
    bool bSuccess = true;
    char szMsg[128] = {0};
    // 等级低
    if(shareProperty.wLevel < m_AutoSetting.nAcceptLevel )
    {
        bSuccess = false;
        sprintf_s( szMsg, sizeof(szMsg), "等级达到%d级才能加入[%s]战队，申请被拒绝。", m_AutoSetting.nAcceptLevel, m_szName );
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_LOGIN_SERVER_MESSAGE,szMsg);
        
    }

    // 性别
    if( (m_AutoSetting.dwAcceptFlag & nSexFlag) != nSexFlag )
    {
        bSuccess = false;
        sprintf_s( szMsg, sizeof(szMsg), "您的性别与[%s]战队招收条件不相符，申请被拒绝。", m_szName );
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_LOGIN_SERVER_MESSAGE,szMsg);
    }

    // 判断等级，职业，性别
    if ( bSuccess )
    {
        SKinMember* pApplyMember = m_pKinService->findKinMember(dwPDBID);
        SKinApply* pKinApply = g_KinInviteManager.FindApply(dwPDBID, m_dwID);
        if (pKinApply == NULL || pKinApply->dwApplyKinID != m_dwID)
        {
            return false;
        }

        if (pApplyMember != NULL)
        {
            return false;
        }

        // 离开战队不满24或1小时
        char szText[512]= {0};
        int nQuitKinTime = m_pKinService->getQuitKinTime(dwPDBID);
        int nCurTime = (int)time(NULL);
        int nDiffTime = pConfig->nJoinKinFreeze * 3600 - (nCurTime -  nQuitKinTime);
        if ( nDiffTime > 0)
        {
            char szText[26] = {0};
            sprintf_s(szText, "%d", pConfig->nJoinKinFreeze);
            g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_KIN_LEAVE_TIME_NOT_SATISFY,szText);
            return false;
        }

        // 判断列表是否已经满了
        ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
        if (nullptr == pSchemeCenter)
        {
            ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
            return false;
        }
        SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);
        if (pLevelInfo == NULL)
        {
            return false;
        }

        if (m_nMemberCount >= pLevelInfo->nMaxMemberCount)
        {
            return false;
        }

        acceptMember(pKinApply);

        // 通知玩家刷新窗口
        g_KinInviteManager.RemoveApply(dwPDBID);
    }
    else
    {
        // 自动拒绝
        g_EHelper.showSystemMessage(dwPDBID,CHAT_TIP_KIN_APPLY_FAILD,"");
        
        g_KinInviteManager.RemoveApply(dwPDBID, m_dwID);
    }

    return bSuccess;
}

void CKin::acceptMember( SKinApply* pKinApply )
{
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pKinApply->dwApplyPDBID);

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    if (pConfig == NULL)
    {
        ErrorLn(__FUNCTION__" pConfig == NULL");
        return;
    }

    // 判断列表是否已经满了
    SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);
    if (pLevelInfo == NULL)
    {
        ErrorLn(__FUNCTION__" pLevelInfo == NULL m_nLevel ="<<m_nLevel);
        return ;
    }

    if (m_nMemberCount >= pLevelInfo->nMaxMemberCount)
    {
        ErrorLn(__FUNCTION__" pLevelInfo == NULL member is full!");
        return;
    }

    // 添加到成员列表
    SKinMember* pMember = m_pKinService->findKinMember(pKinApply->dwApplyPDBID);
    SKinMember kinMember;
    memset(&kinMember, 0, sizeof(kinMember));
    if (pMember != NULL)
    {
        pMember->dwUDBID        = pKinApply->dwUDBID;
        pMember->dwKinID        = m_dwID;
        pMember->nIdentity      = emKinIdentity_Member;
        pMember->dwPopedomFlag  = g_EHelper.GetIdentityPopedom(emKinIdentity_Member);
        pMember->bIsOnline      = (shareProperty.dwPDBID == pKinApply->dwApplyPDBID);
        pMember->nLogoutTime    = (int)time(NULL) - 60;
        pMember->nLoginTime     = (int)time(NULL);
        pMember->nVocation      = pKinApply->nVocation;
        pMember->nLevel         = pKinApply->nLevel;
        pMember->nFaceID        = pKinApply->nFaceID;
        pMember->bySex          = pKinApply->nSex;
		pMember->nDayActivity   = shareProperty.nDayActivity;
        // 往客户端刷新自己信息
        m_pKinService->updateMemberToClient(pMember);

        // 存数据库
        m_pKinService->updateMemberToDB(pMember);

        // 同步到场景服
        m_pKinService->updateMemberToScene(pMember);

    }
    else
    {
        // 添加到全局成员列表
        kinMember.dwUDBID        = pKinApply->dwUDBID;
        kinMember.dwKinID       = m_dwID;
        kinMember.dwPDBID       = pKinApply->dwApplyPDBID;
        kinMember.nIdentity     = emKinIdentity_Member;
        kinMember.dwPopedomFlag = g_EHelper.GetIdentityPopedom(kinMember.nIdentity);
        kinMember.nVocation     = pKinApply->nVocation;
        kinMember.nLevel        = pKinApply->nLevel;
        kinMember.bIsOnline	    = (shareProperty.dwPDBID == pKinApply->dwApplyPDBID);
        kinMember.nLogoutTime	= (int)time(NULL) - 60;
        kinMember.nLoginTime    = (int)time(NULL);
        kinMember.nFaceID       = pKinApply->nFaceID;
        kinMember.bySex         = pKinApply->nSex;
        sstrcpyn(kinMember.szName, pKinApply->szApplyName, sizeof(kinMember.szName));
        m_pKinService->addKinMember(&kinMember, true);
		pMember = &kinMember;

		// 战队频道广播成员加入
		ChatHelper helper;
		if (helper.m_ptr != NULL)
		{
			SSharePersonProperty tempProperty;
			tempProperty.dwPDBID = kinMember.dwPDBID;
			sstrcpyn(tempProperty.szName, kinMember.szName, sizeof(tempProperty.szName));
			tempProperty.dwKinID = kinMember.dwKinID;
			helper.m_ptr->broadcastActorTipMessage(tempProperty, CHAT_CHANNEL_KIN, CHAT_TIP_KIN_ACTOR_JOIN_TIP);
		}
    }
    addMember(pKinApply->dwApplyPDBID, emKinIdentity_Member);

    // 记录加入战队日志
    //记录职位日志
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        char szText[128] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("加入战队：角色id[%u]角色名[%s]加入了战队[%s]"), pKinApply->dwApplyPDBID, kinMember.szName, m_szName);

        IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
        if ( pOSSLogService != NULL )
        {
            pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_KIN, m_dwID, szText);
        }
    }

    // 记录战队事件
    SKinEvent kinEvent;
    char szText[256] = {0};
    kinEvent.tTime =  ConvertTimeValueToStruct(time(NULL));
    ssprintf_s(szText,sizeof(szText),_GT("<color=\"#EEEA82FF\">[%s]</color><color=\"#FFFFFFFF\">加入战队</color>"), pKinApply->szApplyName);
    sstrcpyn(kinEvent.szCoutext, szText, sizeof(kinEvent.szCoutext));
    AddKinEvent(&kinEvent);

    // 通知公共区新增一个成员
    if (shareProperty.dwPDBID == pKinApply->dwApplyPDBID)
    {
        // 如果该战队不是非正式战队，通知客户端刷新头顶战队名字
        // 更新战队ID到场景服
        obuf64 obuf;
        SMsgKin_UdateID sendData;
        sendData.dwPDBID = pMember->dwPDBID;
        sendData.dwKinID = pMember->dwKinID;

        // 把ID更新到场景服
        fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEID);
        obuf.push_back(&sendData, sizeof(sendData));
        g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size(), shareProperty.serverID);

        // 发送战队ID改变事件
        IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
        if(pEventEngine)
        {
            obuf64 buf;
            SEventKin_UpdateKinID eventData;
            eventData.dwPDBID = pMember->dwPDBID;
            eventData.dwKinID = pMember->dwKinID;
            buf.push_back(&eventData, sizeof(eventData));
            pEventEngine->FireExecute(EVENT_KIN_UPDATEID, SOURCE_TYPE_KIN, pMember->dwPDBID, buf.data(), SIZE_INT(buf.size()));
        }

        // 把战队的基本信息发送到客户端
        updateDataToClient(pKinApply->dwApplyPDBID);

        //通知客户端发加入已经加入战队消息
        SEventKin_PersonJoinedKin eventData;
        eventData.bInSuccess = true;
        g_EHelper.FireClientEvent(pKinApply->dwApplyPDBID, EVENT_KIN_PERSONJOINEDKIN, SOURCE_TYPE_KIN, m_dwID, (LPCSTR)&eventData, sizeof(SEventKin_PersonJoinedKin));
    }

    //人员变动信息
    sendObClientKinBaseData();
}

/// purpose: 增加一个事件
void CKin::AddKinEvent(SKinEvent* pKinEvent)
{
    // 如果已经超过上限了，删除第一个
    if (m_EventList.size() >= KIN_EVENT_COUNT)
    {
        m_EventList.pop_front();
    }

    m_EventList.push_back(*pKinEvent);
}


void CKin::onClientAdmit(ClientID clientID, LPCSTR pszData, size_t nLen)
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

    // 接收成员
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    
    if (nLen != sizeof(SMsgKin_Admit_CO))
    {
        return;
    }

    // 检测玩家是否有招收成员的权限
    SKinMember* pMember = m_pKinService->findKinMember(shareProperty.dwPDBID);
    if (pMember == NULL || pMember->dwKinID != m_dwID)
    {
        return;
    }

    if ((pMember->dwPopedomFlag & emKinPopedom_AcceptMember) != emKinPopedom_AcceptMember)
    {
        g_EHelper.showSystemMessage(pMember->dwPDBID,CHAT_TIP_KIN_HAD_NO_RIGHT,"");
        return;
    }

    SMsgKin_Admit_CO* pRecvData		= (SMsgKin_Admit_CO*)pszData;
    SKinMember* pApplyMember		= m_pKinService->findKinMember(pRecvData->dwApplyPDBID);
    SKinApply* pKinApply			= g_KinInviteManager.FindApply(pRecvData->dwApplyPDBID, m_dwID);

    if (pKinApply == NULL || pKinApply->dwApplyKinID != m_dwID)
    {
        g_EHelper.showSystemMessage(pMember->dwPDBID, CHAT_TIP_KIN_NOT_IN_APPLY_LIST,"");
        return;
    }

    if (pRecvData->bAgree == false)
    {
        g_EHelper.showSystemMessage(pMember->dwPDBID, CHAT_TIP_KIN_ADMIT_REJECT, pKinApply->szApplyName);

        // 发送拒绝消息给申请者
        char szText[512] = {0};
        sprintf_s(szText, "%s;%s", pMember->szName,m_szName);
        g_EHelper.showSystemMessage(pRecvData->dwApplyPDBID, CHAT_TIP_KIN_ADMIT_WAS_REJECT, szText);

        g_KinInviteManager.RemoveApply(pRecvData->dwApplyPDBID, m_dwID);
        return;
    }

    if (pApplyMember != NULL)
    {
        g_EHelper.showSystemMessage(pMember->dwPDBID, CHAT_TIP_KIN_APPLY_HAVE_KIN,"");
        return;
    }

    // 是否为低等级
    
    // 离开战队冷却
    int nQuitKinTime = m_pKinService->getQuitKinTime(pRecvData->dwApplyPDBID);
    int nCurTime = (int)time(NULL);
    int nDiffTime =  pKinConfig->nJoinKinFreeze * 3600 - (nCurTime -  nQuitKinTime);
    if ( nDiffTime > 0)
    {
        char szText[26] = {0};
        sprintf_s(szText, "%d", pKinConfig->nJoinKinFreeze);
        g_EHelper.showSystemMessage(pMember->dwPDBID, CHAT_TIP_KIN_LEAVE_TIME_NOT_SATISFY,szText);
        return;
    }

    // todo 判断列表战队是否已经满员
    acceptMember(pKinApply);

    // 通知大家某人加入战队
    broadcastKinMsgToPerson(CHAT_TIP_KIN_MEMBER_JOIN, pKinApply->szApplyName);

    // 通知玩家刷新窗口
    g_KinInviteManager.RemoveApply(pRecvData->dwApplyPDBID);
}

void CKin::onClientKick(ClientID clientID, LPCSTR pszData, size_t nLen)
{
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

    // 开除成员
    if (nLen != sizeof(SMsgKin_Kick_CO))
    {
        return;
    }

    // 检测玩家是否有招收成员的权限
    SKinMember* pManager = m_pKinService->findKinMember(shareProperty.dwPDBID);
    if (pManager == NULL)
    {
        return;
    }

    if ((pManager->dwPopedomFlag & emKinPopedom_KickMember) != emKinPopedom_KickMember)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_HAD_NO_RIGHT,"");
        return;
    }

    SMsgKin_Kick_CO* pRecvData = (SMsgKin_Kick_CO*)pszData;

    // 传进来的成员是否是本战队成员
    SKinMember* pMember = m_pKinService->findKinMember(pRecvData->dwMemberPDBID);
    if (pMember == NULL || pMember->dwKinID != m_dwID)
    {
        return;
    }

    // 队长和副队长不能开除
    if (pMember->nIdentity == emKinIdentity_Shaikh)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_CANT_KICK_SHAIKH,"");
        return;
    }

    if (pMember->nIdentity == emKinIdentity_Elder)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_CANT_KICK_ELDER,"");
        return;
    }

    // 记录离开战队时间
    int nCurTime = (int)time(NULL);
    m_pKinService->setQuitKinTime(pRecvData->dwMemberPDBID, nCurTime);

    // 真的删除
    delMember(pRecvData->dwMemberPDBID);

    // 战队成员退出事件
    SEventKin_DelMember eventData;
    eventData.dwKinID = m_dwID;
    eventData.dwActorID = pMember->dwPDBID;
    gSocialGlobal->getEventEngine()->FireExecute(EVENT_KIN_DELMEMBER,SOURCE_TYPE_KIN, m_dwID, (LPCSTR)&eventData, sizeof(eventData));

    //记录踢出日志
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        char szText[128] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("踢出成员：[%s]被[%s]踢出了战队[%s]"), pMember->szName,pManager->szName, m_szName);
        IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
        if ( pOSSLogService != NULL )
        {
            pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_KIN, m_dwID, szText);
        }
        
    }

    // 记录战队事件
    char szText[256] = {0};
    SKinEvent kinEvent;
    kinEvent.tTime =  ConvertTimeValueToStruct(time(NULL));
    ssprintf_s(szText,sizeof(szText),_GT("<color='#EEEA82FF'>[%s]</color>被<color='#EEEA82FF'>[%s]</color>踢出了战队"), pMember->szName,pManager->szName);

    sstrcpyn(kinEvent.szCoutext, szText, sizeof(kinEvent.szCoutext));
    AddKinEvent(&kinEvent);
    // 记录正式成立日志
    // 通知客户端删除人物成功
}

/// purpose: 修改宣言，公告										-- 来自客户端
void CKin::onClientMotify(ClientID clientID, LPCSTR pszData, size_t nLen)
{
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

    if (nLen != sizeof(SMsgKin_Motify))
    {
        return;
    }

    // 检测玩家是否有权限
    SKinMember* pMember = m_pKinService->findKinMember(shareProperty.dwPDBID);
    if (pMember == NULL || pMember->dwKinID != m_dwID)
    {
        return;
    }

    SMsgKin_Motify* pRecvData = (SMsgKin_Motify*)pszData;
    if (pRecvData->nPropID == emKinManifesto)
    {
        // 判断权限
        if ((pMember->dwPopedomFlag & emKinPopedom_ModifyManifesto) != emKinPopedom_ModifyManifesto)
        {
            return;
        }
    }
    else if (pRecvData->nPropID == emKinNotice)
    {
        // 判断权限
        if ((pMember->dwPopedomFlag & emKinPopedom_ModifyNotice) != emKinPopedom_ModifyNotice)
        {
            return;
        }
        sstrcpyn(m_szNotice, pRecvData->szText, KIN_NOTICE_LEN);

        // 修改公告成功
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_MODIFY_NOTICE_SUCCESS,"");
    }
    else
    {
        return;
    }

    // 往数据库存盘
    updateDataToDB();

    SEventKin_UpdateStringProp_C eventData;
    eventData.nPropID = pRecvData->nPropID;
    sstrcpyn(eventData.szValue, pRecvData->szText, KIN_MAX_PROPLEN);
    broadcastToObserve(EVENT_KIN_UPDATE_STRINGPROP, (LPCSTR)&eventData, sizeof(eventData));

}


/// purpose: 删除成员
//@ param  : nPDBID 玩家PDBID
//@ param  : szName 玩家名字
void CKin::delMember(DWORD dwPDBID)
{
    SKinMember* pMember = m_pKinService->findKinMember(dwPDBID);
    if (pMember == NULL)
    {
        return;
    }

    // 通知公共区删除一个成员

    // 删除成员
    m_nMemberIDList.remove(pMember->dwPDBID);
    m_nMemberCount --;

    // 删除全局成员列表数据
    m_pKinService->removeKinMember(pMember->dwPDBID, true);

    //人员变动信息
    sendObClientKinBaseData();

    // 更新到观察者
    SEventKin_RemoveMember_OC data;
    data.dwPDBID = pMember->dwPDBID;
    broadcastToObserve(EVENT_KIN_REMOVEMEMBER, (LPCSTR)&data, sizeof(data));

}

void CKin::onClientQuit(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);

    // 退出战队
    // 检测玩家是不是本战队的成员
    SKinMember* pMember = m_pKinService->findKinMember(shareProperty.dwPDBID);
    if (pMember == NULL || pMember->dwKinID != m_dwID)
    {
        return;
    }

    // 战队战期间不能开除成员



    // 战队副本期间不能开除成员


    // 队长不能退出战队
    if (pMember->nIdentity == emKinIdentity_Shaikh)
    {
        g_EHelper.showSystemMessage(pMember->dwPDBID,CHAT_TIP_KIN_CANT_QUIT,"");
        
        return;
    }

    // 如果是副队长
    if (pMember->nIdentity == emKinIdentity_Elder)
    {
        if (removeElder(pMember->dwPDBID) == false)
        {
            WarningLn(_GT("全局列表和战队成员列表数据不对应,副队长")<<" ID = " << pMember->dwPDBID);
        }
    }

    // 移除观察者
    removeObserve(pMember->dwPDBID, true);

    // 记录离开战队时间
    int nCurTime = (int)time(NULL);
    m_pKinService->setQuitKinTime(pMember->dwPDBID, nCurTime);
    //添加退出战队事件
    SKinEvent kinEvent;
    char szText[256] = {0};
    kinEvent.tTime =  ConvertTimeValueToStruct(time(NULL));
    ssprintf_s(szText,sizeof(szText),_GT("<color=\"#EEEA82FF\">[%s]</color><color=\"#FFFFFFFF\">退出了战队</color>"), pMember->szName);
    sstrcpyn(kinEvent.szCoutext, szText, sizeof(kinEvent.szCoutext));
    AddKinEvent(&kinEvent);

    // 记录正式离开战队日志
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        char szText[128] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("退出战队：角色id[%u][%s]退出了战队[%s]"), pMember->dwPDBID, pMember->szName, m_szName);
        IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
        if ( pOSSLogService != NULL )
        {
            pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_KIN, m_dwID, szText);
        }
        
    }

    // 战队成员退出事件
    SEventKin_Quit eventData;
    eventData.dwKinID = m_dwID;
    eventData.dwActorID = pMember->dwPDBID;
    gSocialGlobal->getEventEngine()->FireExecute(EVENT_KIN_QUIT,SOURCE_TYPE_KIN, m_dwID, (LPCSTR)&eventData, sizeof(eventData));

    delMember(pMember->dwPDBID);

    //通知客户端隐藏“快捷按钮”

}

/// purpose: 删除一个副队长
bool CKin::removeElder(int nElderID)
{
    if (m_nElderNum < 0 || m_nElderNum > KIN_ELDER_LEN)
    {
        ErrorLn(_GT("副队长个数非法，")<<" m_nElderNum = " << m_nElderNum);
        return false;
    }

    for (int i = 0; i < m_nElderNum; i ++)
    {
        if (m_nElderIDList[i] == nElderID)
        {
            if (i + 1 < m_nElderNum)
            {
                memcpy(&m_nElderIDList[i], &m_nElderIDList[i + 1], sizeof(int) * (m_nElderNum - (i + 1)));
            }

            m_nElderIDList[m_nElderNum - 1] = 0;
            m_nElderNum --;
            TraceLn("[removeElder] m_nElderNum = " << m_nElderNum);

            return true;
        }
    }

    return false;
}

int CKin::getNumProp(DWORD dwPropID)
{
    if (dwPropID < emKinProp_ID && dwPropID >= emKinProp_NumPropCount)
    {
        return 0;
    }

    if (m_propAddress[dwPropID] == 0)
    {
        return 0;
    }

    return *(m_propAddress[dwPropID]);
}

/// purpose: 设置战队的整型数据
typedef void * (CKin::* LEECHDOM_CHANGE_PROC)(DWORD dwPropID, int nValue);

bool CKin::setNumProp(DWORD dwPropID, int nValue, bool bSaveDB, bool syncToZone)
{
    static LEECHDOM_CHANGE_PROC changeproc[emKinProp_NumPropCount] =
    {
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 战队ID
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 状态
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 战队等级
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 战队创立时间
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 发起解散的时间
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 战队队长PDBID
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 成员的最后下线时间
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 已经从数据库读出数据的标志

        (LEECHDOM_CHANGE_PROC)&CKin::increment_NoNegative,	// 战队资金
        (LEECHDOM_CHANGE_PROC)&CKin::increment_Activity,	// 活跃度
		(LEECHDOM_CHANGE_PROC)&CKin::increment_NoNegative,  // 周活跃度
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 排名
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 游戏世界ID
        (LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 战队移民状态
		(LEECHDOM_CHANGE_PROC)&CKin::cover_NoNegative,		// 队长状态
    };

    if (dwPropID < emKinProp_ID || dwPropID >= emKinProp_NumPropCount || changeproc[dwPropID] == NULL)
    {
        return false;
    }

    if (m_propAddress[dwPropID] == 0)
    {
        return false;
    }
    (this->*changeproc[dwPropID])(dwPropID, nValue);

    if (bSaveDB == true)
    {
        updateDataToDB();
    }

	if (syncToZone)
	{
		// 更新属性到场景服
		obuf512 ob;
		fillKinData(ob, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEKINPROP_SCENE);
		SMsgKin_UpdatePropScene_OS sendData;
		sendData.dwKinID = m_dwID;
		sendData.nPropID = (BYTE)dwPropID;
		sendData.nValue = *m_propAddress[dwPropID];
		ob.push_back(&sendData, sizeof(sendData));
		g_EHelper.SendDataToSceneSvr(ob.data(), ob.size());
	}

    return true;
}

/// purpose: 战队数据存数据库（这个过程以后会改成其他类去做）
void CKin::updateDataToDB(bool bSoon)
{
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService == NULL)
    {
        return;
    }

    // 如果数据还没读出来就不处理了
    if ((m_dwReadFlag & emKinRead_Head) != emKinRead_Head)
    {
        return;
    }

    if (isPublicGameWorld() == true)
    {
        return;
    }

    DBREQ_PARAM_UPDATEKIN referData;
    referData.dwKinID = m_dwID;
    sstrcpyn(referData.szName, m_szName, sizeof(referData.szName));
    referData.nState = m_nState;
    referData.nLevel = m_nLevel;
    sstrcpyn(referData.szNotice, m_szNotice, sizeof(referData.szNotice));
    referData.nEstablishTime = m_nEstablishTime;
    referData.nDismissApplyTime = m_nDismissApplyTime;
    referData.nActivity = m_nActivity;
	referData.nWeekActivity = m_nWeekActivity;
    referData.nHisActivity = m_nHistoryActivity;

    sstrcpyn(referData.szSlogan, m_szSlogan, sizeof(referData.szSlogan));
    referData.nAutoAccept = (m_AutoSetting.bAutoAccept == true) ? 1 : 0;
    referData.nAcceptLevel = m_AutoSetting.nAcceptLevel;
    referData.dwAcceptSetting = m_AutoSetting.dwAcceptFlag;
	sstrcpyn(referData.szLabel, m_szLabel, sizeof(referData.szLabel));
	memcpy(referData.byGlory, m_nGlory, sizeof(referData.byGlory));
 
    // todo 更新到数据库
    m_pKinService->ExecSP(GAMEDB_REQUEST_UPDATEKIN,(LPCSTR)&referData,sizeof(DBREQ_PARAM_UPDATEKIN));

}

void CKin::cover_NoNegative(DWORD dwPropID, int nValue)
{
    nValue < 0 ? nValue = 0 : 0;

    *(m_propAddress[dwPropID]) = nValue;
}

void CKin::increment_NoNegative(DWORD dwPropID, int nValue)
{
    LONGLONG old = *(m_propAddress[dwPropID]);

    if (old + (LONGLONG)nValue > MAX_INT_VALUE)
    {
        return;
    }	

    *(m_propAddress[dwPropID]) += nValue;
    *(m_propAddress[dwPropID]) < 0 ? *(m_propAddress[dwPropID]) = 0 : 0;
}

/// purpose: 活跃度修改
void CKin::increment_Activity(DWORD dwPropID, int nValue)
{

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    LONGLONG old = *(m_propAddress[dwPropID]);
    if (old + (LONGLONG)nValue > MAX_INT_VALUE)
    {
        return;
    }

    int nMaxValue = 0;
    *(m_propAddress[dwPropID]) += nValue;
    *(m_propAddress[dwPropID]) < 0 ? *(m_propAddress[dwPropID]) = 0 : 0;
}

bool CKin::createFromDB(KinService *pKinService,LPCSTR pszContext, int nLen)
{
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": pKinService == NULL");
        return false;
    }
    if (nLen != sizeof(DBREQ_TABLE_KIN))
    {
        return false;
    }

    // 设置数据
    DBREQ_TABLE_KIN* pDBContext = (DBREQ_TABLE_KIN*)pszContext;
    m_pKinService = pKinService;
    m_dwID = pDBContext->dwID;
    sstrcpyn(m_szName, pDBContext->szName, sizeof(m_szName));
    m_nLevel = pDBContext->nLevel;
    m_nEstablishTime = pDBContext->nEstablishTime;
    m_nDismissApplyTime = pDBContext->nDismissApplyTime;
    m_nShaikhID = pDBContext->dwShaikhID;
    m_nActivity = pDBContext->nActivity;
	m_nWeekActivity = pDBContext->nWeekActivity;
    m_nHistoryActivity = pDBContext->nHisActivity;
    m_AutoSetting.bAutoAccept = (pDBContext->nAutoAccept == 1) ? true : false;
    m_AutoSetting.nAcceptLevel = pDBContext->nAcceptLevel;
    m_AutoSetting.dwAcceptFlag = pDBContext->dwAcceptSetting;
    m_nWorldID = getThisGameWorldID();
	sstrcpyn(m_szLabel, pDBContext->szLabel, sizeof(m_szLabel));
	memcpy(m_nGlory, pDBContext->byGlory, sizeof(m_nGlory));
    // 历史活跃度，如果历史活跃度为0，排名不为0，用当前活跃度代替
    if (m_nHistoryActivity == 0)
    {
        m_nHistoryActivity = m_nActivity;
    }

    sstrcpyn(m_szShaikhName, pDBContext->szShaikhName, sizeof(m_szShaikhName));

    // 已读标志
    m_dwReadFlag = 0;
    m_dwReadFlag = m_dwReadFlag | emKinRead_Head;

    // 初始化战队数据
    if (Init() == false)
    {
        return false;
    }

    return true;
}

/// purpose: 是否已经读取扩展数据
bool CKin::isReadPlusData()
{
    return m_bIsReadPlusData;
}

/// purpose: 设置已经读取扩展数据
void CKin::setReadPlusData()
{
    m_bIsReadPlusData = true;
}

bool CKin::setDBBaseContext(LPCSTR pszContext, int nLen)
{
    // 设置战队基本数据
    if (nLen != sizeof(DBREQ_RESULT_READKINBASE))
    {
        return false;
    }

    DBREQ_RESULT_READKINBASE* pRetData = (DBREQ_RESULT_READKINBASE*)pszContext;
    sstrcpyn(m_szNotice, pRetData->szNotice, sizeof(m_szNotice));
    sstrcpyn(m_szSlogan, pRetData->szSlogan, sizeof(m_szSlogan));
    m_AutoSetting.bAutoAccept   = (pRetData->nAutoAccept == 1);
    m_AutoSetting.nAcceptLevel  = pRetData->nAcceptLevel;
    m_AutoSetting.dwAcceptFlag  = pRetData->dwAcceptSetting;
    m_nLastRenameTime			= pRetData->nLastRenameTime;
    m_nMoney                    = pRetData->nMoney;

    m_dwReadFlag = m_dwReadFlag | emKinRead_BaseData;

    return true;
}

bool CKin::setDBMemberContext(LPCSTR pszContext, int nLen)
{
    if (nLen < sizeof(DBREQ_RESULT_READKINMEMBERLIST))
    {
        return false;
    }

    DBREQ_RESULT_READKINMEMBERLIST* pRetData = (DBREQ_RESULT_READKINMEMBERLIST*)pszContext;
    if (nLen != sizeof(DBREQ_RESULT_READKINMEMBERLIST) + pRetData->nMemberCount * sizeof(DBREQ_TABLE_KINMEMBER))
    {
        return false;
    }

    m_nShaikhID = 0;
    memset(m_nElderIDList, 0, sizeof(m_nElderIDList));
    m_nElderNum = 0;
    m_nMemberIDList.clear();
    m_nMemberCount = 0;

    // 设置战队成员数据
    int offset = sizeof(DBREQ_RESULT_READKINMEMBERLIST);
    for (int i = 0; i < pRetData->nMemberCount; i ++)
    {
        DBREQ_TABLE_KINMEMBER* pMember = (DBREQ_TABLE_KINMEMBER*)(pszContext + offset);

        addMember(pMember->dwPDBID, pMember->nIdentity);

        // 添加到全局成员列表
        SKinMember kinMember;
        kinMember.dwUDBID       = pMember->dwUDBID;
        kinMember.bySex         = pMember->nSex;
        kinMember.dwKinID       = pMember->dwKinID;
        kinMember.dwPDBID       = pMember->dwPDBID;
        kinMember.nIdentity     = pMember->nIdentity;
        kinMember.dwPopedomFlag = pMember->dwPopedomFlag;
        kinMember.nLogoutTime   = pMember->nLogoutTime;
        kinMember.nLevel        = pMember->nLevel;
        kinMember.nFaceID       = pMember->nFaceID;
        kinMember.nVocation     = pMember->nHeroID;
        kinMember.nClanCtrb     = pMember->nClanCtrb;
        kinMember.nTotalClanCtrb    = pMember->nTotalClanCtrb;
        kinMember.dwLastClanCtrbTime = pMember->dwLastClanCtrbTime;
        kinMember.dwBattleVal       = pMember->dwBattleVal;
        kinMember.dwSrcBattleVal    = pMember->dwSrcBattleVal;
        kinMember.dwBVOrder		    = pMember->dwBVOrder;
        kinMember.dwLastOrder	    = pMember->dwLastOrder;
        kinMember.nDayActivity      = pMember->nDayActivity;
        kinMember.nWeekActivity     = pMember->nWeekActivity;

        //段位信息
        kinMember.nMatchType	    = pMember->nMatchType;
        kinMember.nRankScore	    = pMember->nRankScore;
        kinMember.nGrade	        = pMember->nGrade;

        sstrcpyn(kinMember.szName, pMember->szName, sizeof(kinMember.szName));


        // 解析扩展数据
        int nExtendLen = sizeof(pMember->szRemark);
        if (sizeof(SKinMemberData) > nExtendLen)
        {
            WarningLn("设置战队成员出错，sizeof(SKinMemberData)大于" << nExtendLen);
            break;
        }
        memcpy(&kinMember.ExtendData, pMember->szRemark, sizeof(SKinMemberData));

        // 如果是队长或者副队长，把它的权限赋成设定的值
        int nIdentityPopedom = g_EHelper.GetIdentityPopedom(kinMember.nIdentity);
        if (nIdentityPopedom > 0)
        {
            kinMember.dwPopedomFlag = nIdentityPopedom;
        }

        // 社会服不计算战力值，以场景服计算的为准
        // kinMember.dwBattleVal =	CalcBattle(&kinMember);
        m_pKinService->addKinMember(&kinMember, false);
        offset += sizeof(DBREQ_TABLE_KINMEMBER);
    }

    m_dwReadFlag = m_dwReadFlag | emKinRead_Member;
    m_nMemberCount = pRetData->nMemberCount;

    reCalcBattleValOrder(false);
    return true;
}

bool CKin::setDBLegendAwardContext(LPCSTR pszContext, int nLen)
{
	if (nLen < sizeof(DBREQ_RESULT_READKINLEGENDAWARDLIST))
	{
		return false;
	}

	DBREQ_RESULT_READKINLEGENDAWARDLIST* pRetData = (DBREQ_RESULT_READKINLEGENDAWARDLIST*)pszContext;
	if (nLen != sizeof(DBREQ_RESULT_READKINLEGENDAWARDLIST) + pRetData->nLegendAwardCount * sizeof(DBREQ_TABLE_KIN_LEGENDAWARD))
	{
		return false;
	}


	// 设置杯赛奖励成员数据
	int offset = sizeof(DBREQ_RESULT_READKINLEGENDAWARDLIST);
	for (int i = 0; i < pRetData->nLegendAwardCount; i++)
	{
		DBREQ_TABLE_KIN_LEGENDAWARD* pLegend = (DBREQ_TABLE_KIN_LEGENDAWARD*)(pszContext + offset);
		SKinCupAwardNode kinGloryNode;
		kinGloryNode.llLegengCupID = pLegend->llLegendCupID;
		kinGloryNode.byCupRank = pLegend->byCupRank;
		kinGloryNode.nCupConfigID = pLegend->nCupConfigID;
		kinGloryNode.dwCupEndTime = pLegend->dwCupEndTime;
		sstrcpyn(kinGloryNode.szCupName, pLegend->szCupName, sizeof(kinGloryNode.szCupName));
		m_cupAwardVec.push_back(kinGloryNode);

		offset += sizeof(DBREQ_TABLE_KIN_LEGENDAWARD);
	}

	m_dwReadFlag = m_dwReadFlag | emKinRead_LegendAward;

	return true;
}

bool CKin::setDBLegendWarContext(LPCSTR pszContext, int nLen)
{
	if (nLen < sizeof(DBREQ_RESULT_READKINLEGENDWARLIST))
	{
		return false;
	}

	DBREQ_RESULT_READKINLEGENDWARLIST* pRetData = (DBREQ_RESULT_READKINLEGENDWARLIST*)pszContext;
	if (nLen != sizeof(DBREQ_RESULT_READKINLEGENDWARLIST) + pRetData->nLegendWarCount * sizeof(DBREQ_TABLE_KIN_LEGENDWAR))
	{
		return false;
	}


	// 设置杯赛奖励成员数据
	int offset = sizeof(DBREQ_RESULT_READKINLEGENDWARLIST);
	for (int i = 0; i < pRetData->nLegendWarCount; i++)
	{
		DBREQ_TABLE_KIN_LEGENDWAR* pWar = (DBREQ_TABLE_KIN_LEGENDWAR*)(pszContext + offset);
		SKinWarRecordNode kinWarNode;
		kinWarNode.dwKin1ID = pWar->dwKin1ID;
		kinWarNode.dwKin2ID = pWar->dwKin2ID;
		kinWarNode.dwWinKin = pWar->dwWinKin;
		kinWarNode.llWarID = pWar->llWarID;
		kinWarNode.dwBeginTime = pWar->dwBeginTime;
		sstrcpyn(kinWarNode.szKin1Name, pWar->szKin1Name, sizeof(kinWarNode.szKin1Name));
		sstrcpyn(kinWarNode.szKin2Name, pWar->szKin2Name, sizeof(kinWarNode.szKin2Name));
		m_warRecordBaseVec.push_back(kinWarNode);

		offset += sizeof(DBREQ_TABLE_KIN_LEGENDWAR);
	}

	m_dwReadFlag = m_dwReadFlag | emKinRead_LegendWar;

	return true;
}

/// purpose: 取得战队名字
LPCSTR CKin::getName()
{
    return m_szName;
}

/// purpose: 取得战队队长名字
LPCSTR CKin::getShaikhName()
{
    return m_szShaikhName;
}

/// purpose: 修改战队队长名字
void CKin::setShaikhName(LPCSTR szName)
{
	if (szName == NULL)
	{
		return;
	}
	sstrcpyn(m_szShaikhName, szName, sizeof(m_szShaikhName));
}

void CKin::removeApplyMember(DWORD nPDBID)
{
    // 删除申请中成员
    m_nApplyIDList.remove(nPDBID);

    // 往客户端发出移除申请消息
    SEventKin_RemoveApply_OC data;
    data.dwKinID = m_dwID;
    data.dwPDBID = nPDBID;
    data.bAutoAccpet = m_AutoSetting.bAutoAccept;
    data.nLevelLimit = (BYTE)m_AutoSetting.nAcceptLevel;
    data.dwVocationLimit = m_AutoSetting.dwAcceptFlag;

    broadcastToObserve(EVENT_KIN_REMOVEAPPLY, (LPCSTR)&data, sizeof(data));

}

void CKin::onClientKinBaseData(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    // 在共享服取一下信息
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

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);

    SMsgKin_KinBaseData_OC sendData;
    
    ///////////////////////////////基础信息/////////////////////////////////////////////////////////
    sendData.dwKinID            = m_dwID;                   // 战队ID
    sendData.dwShaikhID			= m_nShaikhID;				// 队长
    sendData.nKinLevel			= m_nLevel;					// 战队等级
    sendData.nSysTime			= (int)time(NULL);			// 系统时间
    sendData.nActivity			= m_nActivity;				// 活跃度
	sendData.nWeekActivity      = m_nWeekActivity;          // 周活跃度
    sendData.nMoney		        = m_nMoney;			        // 资金
    sendData.nPlace				= (short)m_nPlace;			// 战队荣誉值排名

    sendData.nKinMaxMember      = pLevelInfo==NULL ? 0 : pLevelInfo->nMaxMemberCount; // 战队允许最大总人数
    sendData.nKinLevel          = m_nLevel;
    sendData.dwShaikhID         = m_nShaikhID;
    sendData.nTotalFight        = getTotalFightScore();		// 总战力
    // 总战力排名
    OrderHelper helper;
    if (helper.m_ptr != NULL)
    {
        sendData.nTotalFightOrder =  helper.m_ptr->getKinOrder(m_dwID);
    }
    else
    {
        sendData.nTotalFightOrder = 0;
    }
    sstrcpyn(sendData.szKinName, m_szName, sizeof(sendData.szKinName));
    sstrcpyn(sendData.szNotice, m_szNotice, sizeof(sendData.szNotice));
    sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
	sstrcpyn(sendData.szKinLabel, m_szLabel, sizeof(sendData.szKinLabel));
	sendData.nLegendWinCount = m_nGlory[emLegendGlory_WinCount];
	sendData.nLegendTotaLCount = m_nGlory[emLegendGlory_WinCount] + m_nGlory[emLegendGlory_LoseCount];
	sendData.nGoldNum = m_nGlory[emLegendGlory_GoldCount];
	sendData.nSilverNum = m_nGlory[emLegendGlory_SilverCount];

    //////////////////////////////////////////////////////////////////////////////////////////////////

    // 发长度包
    int nBuffPackLen = sizeof(SNetMsgHead) + sizeof(SMsgKin_KinBaseData_OC);
    int nKinMemPackLen = sizeof(SNetMsgHead) + sizeof(SMsgKin_KinBaseData_OC);
    // 组包BUFF
    obuf8192 obufData;
    obuf8192 obufDataMem;

    // 遍历战队成员列表
    SClientKinMember clientMember;
    int nKinMemberLen = sizeof(SClientKinMember);	
    for (list<DWORD>::iterator it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++it)
    {
        if (nKinMemPackLen + nKinMemberLen> 8192)
        {
            ErrorLn("[onClientKinBaseData.KinMember] 缓存区大小不足，发送数据可能不全");
            break;
        }

        memset(&clientMember, 0, nKinMemberLen);
        SKinMember* pMember = m_pKinService->findKinMember(*it);
        if (pMember == NULL)
        {
            continue;
        }

        // 取共享数据
        SSharePersonProperty memberShareProperty = pShareReceiver->GetSharePerson(pMember->dwPDBID);

		// 填充客户端战队成员结构
		fillClientMemberInfo(clientMember, pMember, memberShareProperty);

        if (clientMember.bIsOnline)
        {
            sendData.nOnlineCount++;
        }

        // 压入战队成员信息
        // obufData.push_back(&clientMember, nKinMemberLen);
        obufDataMem.push_back(&clientMember, nKinMemberLen);
        // 战队成员计数
        ++sendData.nMemberCount;
        // 当前包长度
        nKinMemPackLen += nKinMemberLen;
    }

    // 申请中的成员列表
    SKinClientApplyMember applyMember;
    int nKinAppMemberLen = sizeof(SKinClientApplyMember);	
    for (list<DWORD>::iterator itApply = m_nApplyIDList.begin(); itApply != m_nApplyIDList.end(); ++itApply)
    {
        if (nBuffPackLen + nKinAppMemberLen > 8192)
        {
            ErrorLn("[onClientKinBaseData.KinAppMember] 缓存区大小不足，发送数据可能不全");
            break;
        }

        memset(&applyMember, 0, nKinAppMemberLen);
        SKinApply* pKinApply = g_KinInviteManager.FindApply(*itApply, m_dwID);
        if (pKinApply == NULL)
        {
            continue;
        }

        sstrcpyn(applyMember.szName, pKinApply->szApplyName, sizeof(applyMember.szName));
        applyMember.dwPDBID		= pKinApply->dwApplyPDBID;		
        applyMember.nLevel		= pKinApply->nLevel;
        applyMember.nVocation	= pKinApply->nVocation;

        // 压入战队申请成员信息
        obufData.push_back(&applyMember, nKinAppMemberLen);
        // 战队申请成员计数
        ++sendData.nApplyMemberNum;
        // 当前包长度
        nBuffPackLen += nKinAppMemberLen;
    }

    
    // 注意：由于数据太大，已超过8K,战队基本信息分两次发送
    /////////////////////////////////
    // 1、先发送战队成员数据
    /////////////////////////////////
    obuf8192 m_ob;
    SNetMsgHead msgHeadMem;
    msgHeadMem.bySrcEndPoint    = MSG_ENDPOINT_SOCIAL;
    msgHeadMem.byDestEndPoint   = MSG_ENDPOINT_CLIENT;
    msgHeadMem.byKeyModule      = MSG_MODULEID_KIN;
    msgHeadMem.byKeyAction      = MSG_KIN_KINMEMBERDATA;

    m_ob.reset();
    m_ob.push_back(&msgHeadMem, sizeof(SNetMsgHead));	
    m_ob.push_back(&sendData, sizeof(SMsgKin_KinBaseData_OC));
    // 压入成员和事件数据
    m_ob.push_back(obufDataMem.data(), obufDataMem.size());
    g_EHelper.SendDataToPerson(shareProperty.dwPDBID, m_ob.data(), m_ob.size());

    /////////////////////////////////
    // 2、再发送战队其它基本数据
    /////////////////////////////////

    SNetMsgHead msgHead;
    msgHead.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
    msgHead.byDestEndPoint = MSG_ENDPOINT_CLIENT;
    msgHead.byKeyModule = MSG_MODULEID_KIN;
    msgHead.byKeyAction = MSG_KIN_KINBASEDATA;

    m_ob.reset();
    m_ob.push_back(&msgHead, sizeof(SNetMsgHead));	
    m_ob.push_back(&sendData, sizeof(SMsgKin_KinBaseData_OC));
    // 压入成员和事件数据
    m_ob.push_back(obufData.data(), obufData.size());	
    g_EHelper.SendDataToPerson(shareProperty.dwPDBID, m_ob.data(), m_ob.size());	

    static int	nLastCheckTime = 0;
    int nCurTime = (int)time(NULL);
    if (nCurTime > nLastCheckTime + (1000 * 60 * 30) )
    {
        nLastCheckTime = nCurTime;
        if(m_nMemberCount != m_nMemberIDList.size())
        {
            ErrorLn("战队人数 m_nMemberCount = "<< m_nMemberCount <<" m_nMemberIDList size ="<< m_nMemberIDList.size());
        }
    }
}

bool CKin::rename(char* szKinName, size_t nNameLen, int nRenameTime)
{
    if (NULL == szKinName)
    {
        return false;
    }

    if (0 == nNameLen || nNameLen != strlen(szKinName))
    {
        return false;
    }

    char szText[512] = {0};

    // 记录并入日志

    //更新新的战队名称
    sstrcpyn(m_szName, szKinName, sizeof(m_szName));
    m_nLastRenameTime = nRenameTime;

    sprintf_s(szText, "队长沉思良久，还是觉得[%s]更加霸气，便提笔把战队名字修改为[%s]。", szKinName, szKinName);

    // 往数据库存盘
    updateDataToDB(true);

    // 同步场景区
    updateDataToScene();

    // 更新所有成员的战队Title
   
    // 广播给所有查看战队的人

    // 更新战队排行榜
    return true;
}

void CKin::onClientDismiss(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    SKinConfig* pConfig = pSchemeCenter->getSchemeKinDataCenter()->getConfig();
    // 在共享服取一下信息
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

    // 判断玩家是否有解散战队的权限
    if (shareProperty.dwPDBID != m_nShaikhID)
    {
        g_EHelper.showSystemMessage(shareProperty.dwPDBID,CHAT_TIP_KIN_HAD_NO_RIGHT,"");
        return;
    }

    if (m_nState != emKinState_Formal)
    {
        return;
    }

    if (!canDisMissKin(shareProperty.dwPDBID))
    {
        return; 
    }
    dismiss(emKinDismiss_Public, "普通区战队解散");
}

bool CKin::canDisMissKin(DWORD dwActorID)
{
    LegendCupHelper legendCupHelper;
    ILegendCupService *pLegendCupService = legendCupHelper.m_ptr;
    if (pLegendCupService != NULL)
    {
        if (pLegendCupService->isKinInLegendCup(m_dwID))
        {
            g_EHelper.showSystemMessage(dwActorID,CHAT_TIP_KIN_CANT_DISMISS,"");
            return false;
        }
    }

    return true;
}

void CKin::dismiss(TKinDismissType nReason, LPCSTR pszReason)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver==NULL");
        return;
    }
    char szText[512] = {0};
    int nStartTick = getTickCount();

    int nCostTime = getTickCount() - nStartTick;
    TraceLn("[" << m_szName << "]dissmiss kin ：" <<  nCostTime << " ms");
    //记录解散日志
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        char szText[512] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("解散战队：角色id[%u]角色名[%s]解散战队名称[%s]解散原因[%s]"), m_nShaikhID, m_szShaikhName, m_szName, pszReason);
        IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
        if ( pOSSLogService != NULL )
        {
            pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_KIN, m_dwID, szText);
        }
    }

    // 通知成员解散战队
    broadcastKinMsgToPerson(CHAT_TIP_KIN_DISMISS, "");

    // 发起战队解散事件
    SEventKin_Dismiss eventData;
    eventData.dwKinID = m_dwID;
	eventData.dwShaikhID = m_nShaikhID;
    gSocialGlobal->getEventEngine()->FireExecute(EVENT_KIN_DISMISS,SOURCE_TYPE_KIN, m_dwID, (LPCSTR)&eventData, sizeof(eventData));

    // 删除全局的成员数据
    list<DWORD>::iterator it;
    for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
    {
        DWORD dwMemberID = *it;
        if (m_nState != emKinState_Informal || nReason == emKinDismiss_Merge)
        {
            int nCurTime = (int)time(NULL);
            m_pKinService->setQuitKinTime(dwMemberID, nCurTime);
        }
        // 更新在线成员的战队信息
        SSharePersonProperty sharePersonProperty = pShareReceiver->GetSharePerson(dwMemberID);
        if (sharePersonProperty.dwPDBID != INVALID_PDBID)
        {
            obuf32 oScenebuf;
            SMsgKin_UdateID sendData;
            sendData.dwPDBID = dwMemberID;
            sendData.dwKinID = 0;
            fillKinData(oScenebuf, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEID);
            oScenebuf.push_back(&sendData, sizeof(sendData));
            g_EHelper.SendDataToSceneSvr(oScenebuf.data(), oScenebuf.size(), sharePersonProperty.serverID);
        }

        {
            obuf32 oClientbuf;
            fillKinData(oClientbuf, MSG_ENDPOINT_CLIENT, MSG_KIN_DISMISS);
            g_EHelper.SendDataToPerson(dwMemberID, oClientbuf.data(), oClientbuf.size());
        }

        m_pKinService->removeKinMember(dwMemberID, false);
    }

    // 删除全局的申请中的成员数据
    for (it = m_nApplyIDList.begin(); it != m_nApplyIDList.end(); ++ it)
    {
        // 这里就不需要存数据库了，因为解散战队的存储过程会把成员，联盟，称号都清了，这里不用多做一次
        m_pKinService->removeKinMember(*it, false);
    }

    // 通知数据库解散战队
    m_pKinService->DBDismissKin(m_dwID);

    // 从删除全局战队对象
    m_pKinService->removeKin(m_dwID);

    // 通知所有场景删除战队相关数据
    obuf512 obuf;
    SMsgKin_RemoveKinScene_OS sendData;
    sendData.dwKinID = m_dwID;
    fillKinData(obuf, MSG_ENDPOINT_SCENE, MSG_KIN_REMOVEKIN_SCENE);
    obuf.push_back(&sendData, sizeof(sendData));
    g_EHelper.SendDataToSceneSvr(obuf.data(), obuf.size());

    sendEMailToAll(nReason == emKinDismiss_Activity ? emMailFill_KinAutoDismiss : emMailFill_KinDismiss);

    // 释放自己，注意
    release();
}

// 发送邮件给当前战队所有玩家
void CKin::sendEMailToAll(int mailID, LPCSTR szParam)
{
    MailHelper mailHelper;
    IMailService *pMailService = mailHelper.m_ptr;
    if (NULL == pMailService)
    {
        ErrorLn(__FUNCTION__": failed! pMailService == NULL");
        return;
    }

    for (list<DWORD>::iterator iter = m_nMemberIDList.begin(); iter != m_nMemberIDList.end(); ++iter)
    {
        SMailSendData mailInfo;
        mailInfo.nType = emMailType_System;							// 邮件类型
        mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
        mailInfo.dwMasterPDBID = *iter;

        pMailService->sendMail(mailInfo, mailID, szParam);
    }
}

void CKin::autoDismiss(int dismissActivity, int dismissWeeks)
{
	char szText[512] = { 0 };
	sprintf_s(szText, sizeof(szText), a2utf8("%s战队本周活跃值%d点"), m_szName, m_nWeekActivity);
	IOSSLogService* pOSSLog = gSocialGlobal->getOSSLogService();
	pOSSLog->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_KIN, m_dwID, szText);

	if (g_EHelper.DiffWeek((DWORD)m_nEstablishTime, (DWORD)time(NULL)))
	{
		if (m_nWeekActivity < dismissActivity)
		{
			if (++m_nWeekCount >= dismissWeeks)
			{
				char szText[256] = { 0 };
				sprintf_s(szText, sizeof(szText), a2utf8("%s战队活跃度连续%d周不满足%d点自动解散"), m_szName, dismissWeeks, dismissActivity);
				dismiss(emKinDismiss_Activity, szText);
			}
            else if (m_nWeekCount == dismissWeeks - 1)
            {
                char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
                ssprintf_s(szParam, sizeof(szParam), a2utf8("%d;%d;%d;%d"), m_nWeekCount, dismissActivity, dismissWeeks, dismissActivity);
                sendEMailToAll(emMailFill_KinDismiss_Hint, szParam);
            }
		}
		else
		{
			m_nWeekCount = 0;
		}
	}
}

/// purpose: 添加观察者
bool CKin::addObserve(DWORD dwPDBID)
{
    SKinObserve* pKinObserve = findObserve(dwPDBID);
    if (pKinObserve != NULL)
    {
        pKinObserve->nCount ++;
    }
    else
    {
        SKinObserve obs;
        obs.dwPDBID = dwPDBID;
        obs.nCount = 1;

        m_mapObserveList[dwPDBID] = obs;
    }

    return true;
}

/// purpose: 删除观察者
void CKin::removeObserve(DWORD dwPDBID, bool bCoerceRemove)
{
    map<DWORD,SKinObserve>::iterator it = m_mapObserveList.find(dwPDBID);
    if (it != m_mapObserveList.end())
    {
        SKinObserve* pKinObserve = &(it->second);
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

/// purpose: 查找观察者
SKinObserve* CKin::findObserve(DWORD dwPDBID)
{
    map<DWORD,SKinObserve>::iterator it = m_mapObserveList.find(dwPDBID);
    
    if (it != m_mapObserveList.end())
    {
        SKinObserve* pKinObserve = &(it->second);;
        return pKinObserve;
    }

    return NULL;
}

//bool CKin::broadcastSystemTips(TipsType type, InfoPos pos, const char* szText)
//{
//    //IChatService* pChatService = gSocialGlobal->getChatService();
//    //// 如果成员列表还没读出来，然后false
//    //if ((m_dwReadFlag & emKinRead_Member) != emKinRead_Member || pChatService == NULL || szText == NULL)
//    //{
//    //    return false;
//    //}
//
//    //// 往战队聊天广播消息
//    //list<DWORD>::iterator it;
//    //for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
//    //{
//    //    // 如果玩家在线
//    //    pChatService->sendSystemTips(type, *it, pos, szText);
//    //}
//
//    return true;
//}

void CKin::broadcastToObserve(int nEventID, LPCSTR pszData, size_t nLen)
{
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    map<DWORD, SKinObserve>::iterator it = m_mapObserveList.begin();
    while (it != m_mapObserveList.end())
    {
        DWORD dwPDBID = it->second.dwPDBID;
        // 发现玩家不在线了，不广播，把它从观察者里面删除
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
        if (shareProperty.dwPDBID != dwPDBID)
        {
            it = m_mapObserveList.erase(it);
            continue;
        }

        g_EHelper.FireClientEvent(dwPDBID, nEventID, SOURCE_TYPE_KIN, m_dwID, pszData, nLen);
        ++ it;
    }
}

void CKin::broadcastToHaveRightMember(TKinPopedom ePopedom,int nEventID, LPCSTR pszData, size_t nLen)
{
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    list<DWORD>::iterator it = m_nMemberIDList.begin();
    for (;it != m_nMemberIDList.end();++it)
    {
        DWORD dwPDBID = *it;
        // 发现玩家不在线了，发送事件
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
        if (shareProperty .dwPDBID != dwPDBID)
        {
            continue;
        }

        //找到KinMember
        SKinMember* pKinMember = m_pKinService->getKinMember(dwPDBID);
        if(pKinMember == NULL)
        {
            continue;
        }

        if((ePopedom & pKinMember->dwPopedomFlag) == ePopedom)
        {
            g_EHelper.FireClientEvent(dwPDBID, nEventID, SOURCE_TYPE_KIN, m_dwID, pszData, nLen);
        }
    }
}


void CKin::broadcastKinMsgToPerson(int nSystemMessageID, LPCSTR szParamList)
{
    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    list<DWORD>::iterator it = m_nMemberIDList.begin();
    for (;it != m_nMemberIDList.end();++it)
    {
        DWORD dwPDBID = *it;
        // 发现玩家不在线了，不广播
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
        if (shareProperty.dwPDBID != dwPDBID)
        {
            continue;
        }

        g_EHelper.showSystemMessage(dwPDBID, (EMChatTipID)nSystemMessageID, szParamList);
    }
}

void CKin::broadcastDataToClient()
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    IShareReceiver * pReceiver = gSocialGlobal->getShareReceiver();
    if (pReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pReceiver == NULL");
        return;
    }

    SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);

    SMsgKin_UpdateClient_OC sendData;
    sendData.dwID           = m_dwID;
    sendData.nState         = (BYTE)m_nState;
    sendData.dwShaikhID     = m_nShaikhID;
    sendData.nKinLevel      = m_nLevel;
    sendData.nMemberCount   = m_nMemberCount;
    sendData.nKinMaxMember  = pLevelInfo == NULL?0:pLevelInfo->nMaxMemberCount;
    sendData.nApplyCount    = SIZE_INT(m_nApplyIDList.size());
    sendData.nOnlineCount   = getOnlineMemberCount();
    sstrcpyn(sendData.szName, m_szName, sizeof(sendData.szName));
    sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));

    obuf256 obuf;
    fillKinData(obuf, MSG_ENDPOINT_CLIENT, MSG_KIN_UPDATECLIENT);
    obuf.push_back(&sendData, sizeof(sendData));

    list<DWORD>::iterator it;
    for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
    {
        g_EHelper.SendDataToPerson(*it, obuf.data(), obuf.size());
    }
}

/// purpose: 进入观察											-- 来自客户端
void CKin::onClientEnterObserve(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    } 
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    if (shareProperty.dwPDBID != INVALID_PDBID)
    {
        addObserve(shareProperty.dwPDBID);
    }
}

/// purpose: 退出观察											-- 来自客户端
void CKin::onClientExitObserve(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    } 
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
    if (shareProperty.dwPDBID != INVALID_PDBID)
    {
        removeObserve(shareProperty.dwPDBID, true);
    }
}

// 成员登录调用
void CKin::onMemberLogin(SSharePersonProperty& shareProperty)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	SKinMember* pKinMember = m_pKinService->getKinMember(shareProperty.dwPDBID);
	if (pKinMember)
	{
		pKinMember->dwWorldID = shareProperty.dwWorldID;
		pKinMember->dwSceneID = shareProperty.dwSceneID;
		pKinMember->serverID = shareProperty.serverID;
	}
}

// 成员注销调用
void CKin::onMemberLogout(ISharePerson * pSharePerson, ELogoutMode nLineType)
{
	PDBID pdbid = pSharePerson->GetNumProp(PROPERTY_ID);
	if (pdbid == INVALID_PDBID)
	{
		return;
	}

	if (nLineType == elogout_offline && pdbid == m_nShaikhID)
	{
		if (m_nMemberCount > 1)
		{
			enterOfflineState();
		}
	}
}

void CKin::ResetWeekActivity()
{
	m_nWeekActivity = 0;
	updateDataToDB();

	// 更新属性到场景服
	obuf512 ob;
	fillKinData(ob, MSG_ENDPOINT_SCENE, MSG_KIN_UPDATEKINPROP_SCENE);
	SMsgKin_UpdatePropScene_OS sendDataToS;
	sendDataToS.dwKinID = m_dwID;
	sendDataToS.nPropID = (BYTE)emKinProp_WeekActivity;
	sendDataToS.nValue = *m_propAddress[emKinProp_WeekActivity];
	ob.push_back(&sendDataToS, sizeof(sendDataToS));
	g_EHelper.SendDataToSceneSvr(ob.data(), ob.size());

	// 更新客户端
	SEventKin_UpdateNumProp_C sendDataToC;
	sendDataToC.nPropID = emKinProp_WeekActivity;
	sendDataToC.nValue = 0;
	broadcastToObserve(EVENT_KIN_UPDATE_NUMPROP, (LPCSTR)&sendDataToC, sizeof(sendDataToC));
}

/// purpose: 取得战队成员个数
int CKin::getMemberCount()
{
    return m_nMemberCount ;
}

DWORD CKin::getMemberList(PDBID* pReturnArray, DWORD dwArrayMaxSize)
{
    //获取成员列表数量
    DWORD dwArraySize = 0;
    list<DWORD>::iterator iter = m_nMemberIDList.begin();
    for (;iter != m_nMemberIDList.end(); ++iter)
    {
        if ( dwArraySize >= dwArrayMaxSize)
        {
            return dwArrayMaxSize;
        }
        pReturnArray[dwArraySize++] = *iter;
    }

    return dwArraySize;
}

void CKin::reCalcBattleValOrder(bool bSaveDB)
{
    if (m_nMemberIDList.empty())
    {
        return;
    }

    // 战队成员战力值排行
    list<DWORD>::iterator it;
    list<DWORD> tempMemberList(m_nMemberIDList);

    vector<SKinMember> tempKinMember;
    // 将战队成员数据全部取出
    for (it = tempMemberList.begin(); it != tempMemberList.end(); ++ it)
    {
        SKinMember* pMember = m_pKinService->findKinMember(*it);
        if (pMember != NULL)
        {
            tempKinMember.push_back(*pMember);
        }
    }
    // 根据战力值进行排名
    std::sort(tempKinMember.begin(), tempKinMember.end(), Less_Member_BattleValue);
    int nOrder = 1;
    vector<SKinMember>::iterator iter = tempKinMember.begin();
    for ( ; iter != tempKinMember.end(); ++iter)
    {
        SKinMember* pMember = m_pKinService->findKinMember(iter->dwPDBID);
        if (pMember != NULL)
        {

            // 先保存前排名
            pMember->dwLastOrder =	pMember->dwBVOrder;
            // 赋值给战队成员数据 完成排名
            pMember->dwBVOrder = nOrder;
            pMember->dwSrcBattleVal = pMember->dwBattleVal;

            // 更新到数据库
            if (bSaveDB)
            {
                m_pKinService->updateMemberToDB(pMember);
            }
            // 往客户端刷新
            m_pKinService->updateMemberToClient(pMember);

            nOrder++;
        }
    }
    tempKinMember.clear();
}

void CKin::sendDataToLoginClient(DWORD dwPDBID)
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    IShareReceiver * pReceiver = gSocialGlobal->getShareReceiver();
    if (pReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pReceiver == NULL idActor ="<< dwPDBID);
        return;
    }

    SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);
    SMsgKin_MemberLogin_OC sendData;
    sendData.dwID           = m_dwID;
    sendData.nState         = (BYTE)m_nState;
    sendData.dwShaikhID     = m_nShaikhID;
    sendData.nKinLevel      = m_nLevel;
    sendData.nMemberCount   = m_nMemberCount;
    sendData.nKinMaxMember  = pLevelInfo == NULL ? 0 : pLevelInfo->nMaxMemberCount;
    sendData.nApplyCount    = SIZE_INT(m_nApplyIDList.size());
    sendData.nOnlineCount   = getOnlineMemberCount();
    sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
    sstrcpyn(sendData.szName, m_szName, sizeof(sendData.szName));

    obuf256 obuf;
    fillKinData(obuf, MSG_ENDPOINT_CLIENT, MSG_KIN_MEMBER_LOGIN);
    obuf.push_back(&sendData, sizeof(sendData));

    g_EHelper.SendDataToPerson(dwPDBID, obuf.data(), obuf.size());
}

void CKin::broadcastKinBaseInfoToOb()
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }


    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);
    SNetMsgHead msgHead;
    msgHead.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
    msgHead.byDestEndPoint = MSG_ENDPOINT_CLIENT;
    msgHead.byKeyModule = MSG_MODULEID_KIN;
    msgHead.byKeyAction = MSG_KIN_KINBASEDATA;

    SMsgKin_KinBaseData_OC sendData;
    sstrcpyn(sendData.szKinName, m_szName, sizeof(sendData.szKinName));
    sstrcpyn(sendData.szNotice, m_szNotice, sizeof(sendData.szNotice));
    sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
    sendData.dwKinID            = m_dwID;                   // 战队ID
    sendData.dwShaikhID			= m_nShaikhID;				// 队长
    sendData.nKinLevel			= m_nLevel;					// 战队等级
    sendData.nSysTime			= (int)time(NULL);			// 系统时间
    sendData.nActivity			= m_nActivity;				// 活跃度
	sendData.nWeekActivity      = m_nWeekActivity;          // 周活跃度
    sendData.nMoney		        = m_nMoney;			        // 资金
    sendData.nPlace				= (short)m_nPlace;			// 战队荣誉值排名

    sendData.nMemberCount       = m_nMemberCount;			// 成员个数
    sendData.nOnlineCount       = getOnlineMemberCount();   // 在线成员数量
    sendData.nKinMaxMember      = pLevelInfo==NULL ? 0 : pLevelInfo->nMaxMemberCount; // 战队允许最大总人数
    sendData.nKinLevel          = m_nLevel;
    sendData.dwShaikhID         = m_nShaikhID;
    sendData.nTotalFight        = getTotalFightScore();		// 总战力
    // 总战力排名
    OrderHelper helper;
    if (helper.m_ptr != NULL)
    {
        sendData.nTotalFightOrder =  helper.m_ptr->getKinOrder(m_dwID);
    }
    else
    {
        sendData.nTotalFightOrder = 0;
    }
	sstrcpyn(sendData.szKinLabel, m_szLabel, sizeof(sendData.szKinLabel));
	sendData.nLegendWinCount = m_nGlory[emLegendGlory_WinCount];
	sendData.nLegendTotaLCount = m_nGlory[emLegendGlory_WinCount] + m_nGlory[emLegendGlory_LoseCount];
	sendData.nGoldNum = m_nGlory[emLegendGlory_GoldCount];
	sendData.nSilverNum = m_nGlory[emLegendGlory_SilverCount];

    // 申请中的成员列表
    obuf8192 obufData;
    int nBuffPackLen = sizeof(SNetMsgHead) + sizeof(SMsgKin_KinBaseData_OC);
    SKinClientApplyMember applyMember;
    int nKinAppMemberLen = sizeof(SKinClientApplyMember);	
    for (list<DWORD>::iterator itApply = m_nApplyIDList.begin(); itApply != m_nApplyIDList.end(); ++itApply)
    {
        if (nBuffPackLen + nKinAppMemberLen > 8192)
        {
            ErrorLn("[onClientKinBaseData.KinAppMember] 缓存区大小不足，发送数据可能不全");
            break;
        }

        memset(&applyMember, 0, nKinAppMemberLen);
        SKinApply* pKinApply = g_KinInviteManager.FindApply(*itApply, m_dwID);
        if (pKinApply == NULL)
        {
            continue;
        }

        sstrcpyn(applyMember.szName, pKinApply->szApplyName, sizeof(applyMember.szName));
        applyMember.dwPDBID		= pKinApply->dwApplyPDBID;		
        applyMember.nLevel		= pKinApply->nLevel;
        applyMember.nVocation	= pKinApply->nVocation;

        // 压入战队申请成员信息
        obufData.push_back(&applyMember, nKinAppMemberLen);
        // 战队申请成员计数
        ++sendData.nApplyMemberNum;
        // 当前包长度
        nBuffPackLen += nKinAppMemberLen;
    }


    obuf  ob;
    ob.reset();
    ob.push_back(&msgHead, sizeof(SNetMsgHead));	
    ob.push_back(&sendData, sizeof(SMsgKin_KinBaseData_OC));
    // 压入成员和事件数据
    ob.push_back(obufData.data(), obufData.size());


    map<DWORD, SKinObserve>::iterator it = m_mapObserveList.begin();
    while (it != m_mapObserveList.end())
    {
        DWORD dwPDBID = it->second.dwPDBID;
        // 发现玩家不在线了，不广播，把它从观察者里面删除
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
        if (shareProperty.dwPDBID != dwPDBID)
        {
            it = m_mapObserveList.erase(it);
            continue;
        }
        g_EHelper.SendDataToPerson(shareProperty.dwPDBID, ob.data(), ob.size());
        ++ it;
    }
}

void CKin::updateKinOrderInfo()
{
    IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
    if(!pEventEngine)
    {
        return;
    }

    obuf buf;
    event_kin_update_orderinfo eventData;
    eventData.dwKinID = m_dwID;			                    // 战队ID
    eventData.dwTotalFightCapacity = getTotalFightScore();  // 战力总和
    eventData.dwShaikhID = m_nShaikhID;						// 队长ID
    eventData.dwPlayerCount = m_nMemberCount;               // 战队人数

    sstrcpyn(eventData.szShaikhName,m_szShaikhName,sizeof(eventData.szShaikhName));	        // 队长名字
    sstrcpyn(eventData.szKinName ,m_szName,sizeof(eventData.szKinName));                    // 战队名称	                 
    buf.push_back(&eventData, sizeof(eventData));
    pEventEngine->FireExecute(EVENT_KIN_UPDATE_ORDERINFO, SOURCE_TYPE_KIN, 0, buf.data(), SIZE_INT(buf.size()));
}

DWORD CKin::getTotalFightScore()
{
     list<DWORD>::iterator it = m_nMemberIDList.begin();
     DWORD dwTotalFight = 0;
     for (; it != m_nMemberIDList.end(); ++it)
     {
         if (*it != INVALID_PDBID )
         {
             SKinMember* pKinMember = m_pKinService->getKinMember(*it);
             if (pKinMember != NULL)
             {
                 // 不在线且超过计算间隔的不记录战力
                 DWORD dwInterval = time(NULL) - pKinMember->nLogoutTime;
                 if (!pKinMember->bIsOnline && pKinMember->nLogoutTime != 0 && dwInterval > KIN_CALCULATE_FIGHT_INVTERVAL_TIME)
                 {
                     continue;
                 }
                 dwTotalFight += pKinMember->dwBattleVal;
             }
         }
     }
     
     return dwTotalFight;
}

/** 获取战队总战力
	@param dwKinID		:战队ID 
	*/
SAutoAccept* CKin::getAutoAccept()
{
    return &m_AutoSetting;
}

void CKin::OnClientRequestKinEventList(ClientID clientID, LPCSTR pszData, size_t nLen)
{
    obuf obData;
    obData.reset();

    SNetMsgHead msgHead;
    msgHead.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
    msgHead.byDestEndPoint = MSG_ENDPOINT_CLIENT;
    msgHead.byKeyModule = MSG_MODULEID_KIN;
    msgHead.byKeyAction = MSG_KIN_REQUEST_KINEVENT_LIST;

    // 战队事件数量
    SKinEventHead DataHead;
    DataHead.nCount = m_EventList.size();

    obData.push_back(&msgHead, sizeof(SNetMsgHead));	
    obData.push_back(&DataHead, sizeof(SKinEventHead));

    // 战队事件数据
    for (list<SKinEvent>::iterator itEvent = m_EventList.begin(); itEvent != m_EventList.end(); ++itEvent)
    {
        // 压入成员和事件数据
        obData.push_back(&(*itEvent),sizeof(*itEvent));
    }

    IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
    if (nullptr == pGatewayAcceptorService)
    {
        ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService clientID=" << clientID);
        return;
    }

    pGatewayAcceptorService->sendData(clientID, PACKAGE_PTR(new std::string((char const *)obData.data(), obData.size())));
}

void CKin::OnClientRequestKinGloryData(ClientID clientID, LPCSTR pszData, size_t nLen)
{
	send2ClientKinGloryData(MSG_KIN_REQUEST_KINGLORYDATA, clientID);
}

void CKin::OnClientRequestWarRecordTotal(ClientID clientID, LPCSTR pszData, size_t nLen)
{
	obuf obData;
	obData.reset();

	SNetMsgHead msgHead;
	msgHead.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
	msgHead.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	msgHead.byKeyModule = MSG_MODULEID_KIN;
	msgHead.byKeyAction = MSG_KIN_REQUEST_WARRECORD_TOTAL;
	obData.push_back(&msgHead, sizeof(msgHead));

    int nSize = m_warRecordBaseVec.size();
	obData << nSize;

	for (vector<SKinWarRecordNode>::iterator iter = m_warRecordBaseVec.begin(); iter != m_warRecordBaseVec.end(); iter++)
	{
		SMsgKinTotalWarNode_OC warnode;
		warnode.nWarID = iter->llWarID;
		warnode.nKin1ID = iter->dwKin1ID;
		warnode.nKin2ID = iter->dwKin2ID;
		warnode.nWinKin = iter->dwWinKin;
		warnode.nBeginTime = iter->dwBeginTime;
		//SKinInfo kin1Info = m_pKinService->getKinInfo(warnode.nKin1ID);
		//SKinInfo kin2Info = m_pKinService->getKinInfo(warnode.nKin2ID);
		//sstrcpyn(warnode.szKin1Name, kin1Info.szName, sizeof(warnode.szKin1Name));
		//sstrcpyn(warnode.szKin2Name, kin2Info.szName, sizeof(warnode.szKin2Name));
		sstrcpyn(warnode.szKin1Name, iter->szKin1Name, sizeof(warnode.szKin1Name));
		sstrcpyn(warnode.szKin2Name, iter->szKin2Name, sizeof(warnode.szKin2Name));
		obData.push_back(&warnode, sizeof(warnode));
	}

	IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
	if (nullptr == pGatewayAcceptorService)
	{
		ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService clientID=" << clientID);
		return;
	}

	pGatewayAcceptorService->sendData(clientID, PACKAGE_PTR(new std::string((char const *)obData.data(), obData.size())));
}

void CKin::sendObClientKinBaseData()
{
   
    
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);
    if (pLevelInfo ==NULL)
    {
        ErrorLn(__FUNCTION__" pLevelInfo ==NULL");
        return;
    }
    SNetMsgHead msgHead;
    msgHead.bySrcEndPoint       = MSG_ENDPOINT_SOCIAL;
    msgHead.byDestEndPoint      = MSG_ENDPOINT_CLIENT;
    msgHead.byKeyModule         = MSG_MODULEID_KIN;
    msgHead.byKeyAction         = MSG_KIN_KINBASEDATA_UPDATE;

    SMsgKin_KinBaseData_OC sendData;
    sendData.dwKinID            = m_dwID;                   // 战队ID
    sendData.dwShaikhID			= m_nShaikhID;				// 队长
    sendData.nKinLevel			= m_nLevel;					// 战队等级
    sendData.nSysTime			= (int)time(NULL);			// 系统时间
    sendData.nActivity			= m_nActivity;				// 活跃度
	sendData.nWeekActivity      = m_nWeekActivity;			// 活跃度
    sendData.nMoney		        = m_nMoney;			        // 资金
    sendData.nPlace				= (short)m_nPlace;			// 战队荣誉值排名

    sendData.nMemberCount       = m_nMemberCount;			// 成员个数
    sendData.nOnlineCount       = getOnlineMemberCount();   // 在线成员数量
    sendData.nApplyMemberNum    = m_nApplyIDList.size();						 // 申请中成员的个数
    sendData.nKinMaxMember      = pLevelInfo==NULL ? 0 : pLevelInfo->nMaxMemberCount; // 战队允许最大总人数
    sendData.nKinLevel          = m_nLevel;
    sendData.dwShaikhID         = m_nShaikhID;
    sendData.nTotalFight        = getTotalFightScore();		// 总战力
    // 总战力排名
    OrderHelper helper;
    if (helper.m_ptr != NULL)
    {
        sendData.nTotalFightOrder =  helper.m_ptr->getKinOrder(m_dwID);
    }
    else
    {
        sendData.nTotalFightOrder = 0;
    }
					
    sstrcpyn(sendData.szKinName, m_szName, sizeof(sendData.szKinName));
    sstrcpyn(sendData.szNotice, m_szNotice, sizeof(sendData.szNotice));
    sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
	sstrcpyn(sendData.szKinLabel, m_szLabel, sizeof(sendData.szKinLabel));
	sendData.nLegendWinCount = m_nGlory[emLegendGlory_WinCount];
	sendData.nLegendTotaLCount = m_nGlory[emLegendGlory_WinCount] + m_nGlory[emLegendGlory_LoseCount];
	sendData.nGoldNum = m_nGlory[emLegendGlory_GoldCount];
	sendData.nSilverNum = m_nGlory[emLegendGlory_SilverCount];

    obuf obData;
    obData.push_back(&msgHead,sizeof(msgHead));
    obData.push_back(&sendData,sizeof(sendData));
    
    // 广播给观察者数据
    broadcastDataToObserve(obData.data(),obData.size());
}

void CKin::broadcastDataToObserve(LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    map<DWORD, SKinObserve>::iterator it = m_mapObserveList.begin();
    while (it != m_mapObserveList.end())
    {
        DWORD dwPDBID = it->second.dwPDBID;
        // 发现玩家不在线了，不广播，把它从观察者里面删除
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
        if (shareProperty.dwPDBID != dwPDBID)
        {
            it = m_mapObserveList.erase(it);
            continue;
        }

        g_EHelper.SendDataToPerson(dwPDBID, pszData, nLen);
        ++ it;
    }
}

DWORD CKin::getOnlineMemberCount()
{
    DWORD dwOnline = 0;
    IShareReceiver * pReceiver = gSocialGlobal->getShareReceiver();
    if (pReceiver == NULL)
    {
        return 0;
    }
    for (list<DWORD>::iterator it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++it)
    {
        SSharePersonProperty shareProperty =  pReceiver->GetSharePerson(*it);
        if (shareProperty.dwPDBID != INVALID_PDBID)
        {
            dwOnline++;
        }
    }
    return dwOnline;
}

void CKin::addKinLegendAward(SKinCupAwardNode* pAwardNode)
{
	if (pAwardNode == NULL)
	{
		return;
	}
	m_cupAwardVec.push_back(*pAwardNode);

	// 荣誉数据存入数据库
	if (pAwardNode->byCupRank == 1)
	{
		m_nGlory[emLegendGlory_GoldCount] += 1;
	}
	if (pAwardNode->byCupRank == 2)
	{
		m_nGlory[emLegendGlory_SilverCount] += 1;
	}
	updateDataToDB(true);
}

void CKin::addKinLegendWarResult(SKinWarRecordNode* pWarNode)
{
	if (pWarNode == NULL)
	{
		return;
	}
	m_warRecordBaseVec.push_back(*pWarNode);

	// 荣誉数据存入数据库
	if (m_dwID == pWarNode->dwKin1ID)
	{
		for (int i= 0; i < emLegendGlory_Max; i++)
		{
			m_nGlory[i] += pWarNode->nKin1Glory[i];
		}
	}
	else if (m_dwID == pWarNode->dwKin2ID)
	{
		for (int i= 0; i < emLegendGlory_Max; i++)
		{
			m_nGlory[i] += pWarNode->nKin2Glory[i];
		}
	}
	updateDataToDB(true);
}

void CKin::OnClientLegendCupKinInfo(ClientID clientID, LPCSTR pszData, size_t nLen)
{
	// 在共享服取一下信息
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

	ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
	if (nullptr == pSchemeCenter)
	{
		ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
		return;
	}

	SKinLevel* pLevelInfo = pSchemeCenter->getSchemeKinDataCenter()->getLevelInfo(m_nLevel);

	SMsgKin_KinBaseData_OC sendData;

	///////////////////////////////基础信息/////////////////////////////////////////////////////////
    sendData.dwKinID            = m_dwID;                   // 战队ID
	sendData.dwShaikhID			= m_nShaikhID;				// 队长
	sendData.nKinLevel			= m_nLevel;					// 战队等级
	sendData.nSysTime			= (int)time(NULL);			// 系统时间
	sendData.nActivity			= m_nActivity;				// 活跃度
	sendData.nWeekActivity      = m_nWeekActivity;		    // 周活跃度
	sendData.nMoney		        = m_nMoney;			        // 资金
	sendData.nPlace				= (short)m_nPlace;			// 战队荣誉值排名

	sendData.nKinMaxMember      = pLevelInfo==NULL ? 0 : pLevelInfo->nMaxMemberCount; // 战队允许最大总人数
	sendData.nKinLevel          = m_nLevel;
	sendData.dwShaikhID         = m_nShaikhID;
	sendData.nTotalFight        = getTotalFightScore();		// 总战力
	// 总战力排名
	OrderHelper helper;
	if (helper.m_ptr != NULL)
	{
		sendData.nTotalFightOrder =  helper.m_ptr->getKinOrder(m_dwID);
	}
	else
	{
		sendData.nTotalFightOrder = 0;
	}
	sstrcpyn(sendData.szKinName, m_szName, sizeof(sendData.szKinName));
	sstrcpyn(sendData.szNotice, m_szNotice, sizeof(sendData.szNotice));
	sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
	sstrcpyn(sendData.szKinLabel, m_szLabel, sizeof(sendData.szKinLabel));
	sendData.nLegendWinCount = m_nGlory[emLegendGlory_WinCount];
	sendData.nLegendTotaLCount = m_nGlory[emLegendGlory_WinCount] + m_nGlory[emLegendGlory_LoseCount];
	sendData.nGoldNum = m_nGlory[emLegendGlory_GoldCount];
	sendData.nSilverNum = m_nGlory[emLegendGlory_SilverCount];

	//////////////////////////////////////////////////////////////////////////////////////////////////

	// 发长度包
	int nKinMemPackLen = sizeof(SNetMsgHead) + sizeof(SMsgKin_KinBaseData_OC);
	// 组包BUFF
	obuf8192 obufDataMem;

	// 遍历战队成员列表
	SClientKinMember clientMember;
	int nKinMemberLen = sizeof(SClientKinMember);	
	for (list<DWORD>::iterator it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++it)
	{
		if (nKinMemPackLen + nKinMemberLen> 8192)
		{
			ErrorLn("[onClientKinBaseData.KinMember] 缓存区大小不足，发送数据可能不全");
			break;
		}

		memset(&clientMember, 0, nKinMemberLen);
		SKinMember* pMember = m_pKinService->findKinMember(*it);
		if (pMember == NULL)
		{
			continue;
		}

		// 取共享数据
		SSharePersonProperty memberShareProperty = pShareReceiver->GetSharePerson(pMember->dwPDBID);

		// 填充客户端战队成员结构
		fillClientMemberInfo(clientMember, pMember, memberShareProperty);

		// 获取游戏状态
		if (memberShareProperty.dwPDBID != pMember->dwPDBID)
		{
			// 在线状态 （不在线是默认 GAME_STATE_IDLE） 根据在线状态更新
			clientMember.dwState = GAME_STATE_IDLE;
		}
		else
		{
			sendData.nOnlineCount++;
			clientMember.dwState = memberShareProperty.dwGameState;
		}

		// 压入战队成员信息
		obufDataMem.push_back(&clientMember, nKinMemberLen);
		// 战队成员计数
		++sendData.nMemberCount;
		// 当前包长度
		nKinMemPackLen += nKinMemberLen;
	}

	obuf8192 m_ob;
	SNetMsgHead msgHeadMem;
	msgHeadMem.bySrcEndPoint    = MSG_ENDPOINT_SOCIAL;
	msgHeadMem.byDestEndPoint   = MSG_ENDPOINT_CLIENT;
	msgHeadMem.byKeyModule      = MSG_MODULEID_KIN;
	msgHeadMem.byKeyAction      = MSG_KIN_LEGENDCUP_KIN_INFO;

	m_ob.reset();
	m_ob.push_back(&msgHeadMem, sizeof(SNetMsgHead));	
	m_ob.push_back(&sendData, sizeof(SMsgKin_KinBaseData_OC));
	// 压入成员和事件数据
	m_ob.push_back(obufDataMem.data(), obufDataMem.size());
	g_EHelper.SendDataToPerson(shareProperty.dwPDBID, m_ob.data(), m_ob.size());
}

void CKin::fillClientMemberInfo(SClientKinMember& clientMember, const SKinMember* pMember, const SSharePersonProperty& memberShareProperty)
{
	sstrcpyn(clientMember.szName, pMember->szName, sizeof(clientMember.szName));
	clientMember.dwUDBID = pMember->dwUDBID;
	clientMember.dwPDBID = pMember->dwPDBID;
	clientMember.nLevel = (BYTE)pMember->nLevel;
    clientMember.bIsOnline = pMember->bIsOnline;
    clientMember.dwState = pMember->dwState;
	clientMember.nLogoutTime = pMember->nLogoutTime;
	clientMember.nVocation = (BYTE)pMember->nVocation;
	clientMember.nFaceID = (BYTE)pMember->nFaceID;

	clientMember.nIdentity = (BYTE)pMember->nIdentity;
	clientMember.dwBattleVal = pMember->dwBattleVal;
	clientMember.dwSrcBattleVal = pMember->dwSrcBattleVal;
	clientMember.dwBVOrder = (BYTE)pMember->dwBVOrder;
	clientMember.dwLastOrder = (BYTE)pMember->dwLastOrder;

	clientMember.nMatchType = pMember->nMatchType;
	clientMember.nRankScore = pMember->nRankScore;
	clientMember.nGrade = pMember->nGrade;
	clientMember.bySex = pMember->bySex;

	clientMember.dwWorldID = pMember->dwWorldID;
	clientMember.dwSceneID = pMember->dwSceneID;
	clientMember.serverID = pMember->serverID;
}

void CKin::OnClientLegendCupKinGlory(ClientID clientID, LPCSTR pszData, size_t nLen)
{
	send2ClientKinGloryData(MSG_KIN_LEGENDCUP_KIN_GLORY, clientID);
}

void CKin::send2ClientKinGloryData(int nKeyAction, int nClientID)
{
	obuf obData;
	obData.reset();

	SNetMsgHead msgHead;
	msgHead.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
	msgHead.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	msgHead.byKeyModule = MSG_MODULEID_KIN;
	msgHead.byKeyAction = nKeyAction;
	obData.push_back(&msgHead, sizeof(msgHead));

	// 战队荣誉头
	SMsgKinGloryHead_OC DataHead;
	sstrcpyn(DataHead.szKinName, m_szName, sizeof(DataHead.szKinName));
	DataHead.nWinNum = m_nGlory[emLegendGlory_WinCount];		// 杯赛胜场
	DataHead.nLoseNum = m_nGlory[emLegendGlory_LoseCount];		// 杯赛负场
	DataHead.nGodNum = m_nGlory[emLegendGlory_GodCount];		// 团队超神次数
	DataHead.n5KillNum = m_nGlory[emLegendGlory_5KillCount];	// 团队5杀次数
	DataHead.n4KillNum = m_nGlory[emLegendGlory_4KillCount];	// 团队4杀次数
	DataHead.n3KillNum = m_nGlory[emLegendGlory_3KillCount];	// 团队3杀次数
	DataHead.nDeathNum = m_nGlory[emLegendGlory_DeathCount];	// 团队死亡次数
	DataHead.nKillNum = m_nGlory[emLegendGlory_KillCount];		// 团队击杀次数
	DataHead.nAssistNum = m_nGlory[emLegendGlory_AssistCount];	// 团队助攻次数
	DataHead.nCount = m_cupAwardVec.size();						// 比赛获奖次数
	obData.push_back(&DataHead, sizeof(DataHead));

	// 战队荣誉节点
	for (int i = 0; i < m_cupAwardVec.size(); i++)
	{
		SMsgKinGloryNode_OC DataNode;
		DataNode.llLegengCupID = m_cupAwardVec[i].llLegengCupID;
		DataNode.byCupRank = m_cupAwardVec[i].byCupRank;
		DataNode.nCupConfigID = m_cupAwardVec[i].nCupConfigID;
		DataNode.dwCupEndTime = m_cupAwardVec[i].dwCupEndTime;
		sstrcpyn(DataNode.szCupName, m_cupAwardVec[i].szCupName, sizeof(DataNode.szCupName));

		obData.push_back(&DataNode, sizeof(SMsgKinGloryNode_OC));
	}

	IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
	if (nullptr == pGatewayAcceptorService)
	{
		ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService clientID=" << nClientID);
		return;
	}

	pGatewayAcceptorService->sendData(nClientID, PACKAGE_PTR(new std::string((char const *)obData.data(), obData.size())));
}

void CKin::addKinLegendGlory(int nNum, int nGloryType)
{
	if (nGloryType < emLegendGlory_GoldCount || nGloryType >= emLegendGlory_Max)
		return;

	m_nGlory[nGloryType] += nNum;

	updateDataToDB(true);
}

int CKin::getKinLegendGlory(int nGloryType)
{
	if (nGloryType < emLegendGlory_GoldCount || nGloryType >= emLegendGlory_Max)
		return 0;

	return m_nGlory[nGloryType];
}

void CKin::enterOfflineState()
{
	SKinConfig* pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

    SKinMember* pKinMember = m_pKinService->getKinMember(m_nShaikhID);
    if (pKinMember == NULL)
        return;

    DWORD dwInterval = time(NULL) - pKinMember->nLogoutTime;
    DWORD offlineSecs = (pConfig->nOfflineDays > 0 ? pConfig->nOfflineDays : 0) * 3600 * 24;
    DWORD outgoingSecs = (pConfig->nOutgoingDays > 0 ? pConfig->nOutgoingDays : 0) * 3600 * 24;
    if (dwInterval >= offlineSecs + outgoingSecs)
    {
        m_ShaikhState = KinShaikhState_Reigning;
        executeOutgoing();
    }
    else if (dwInterval >= offlineSecs)
    {
        m_ShaikhState = KinShaikhState_Outgoing;
    }
    else
    {
        m_ShaikhState = KinShaikhState_Offline;
    }
}

// 队长自动卸任
void CKin::executeOutgoing()
{
	m_ShaikhState = KinShaikhState_Reigning;
    SKinMember* pKinMember = chooseNewShaikh();
	if (!pKinMember)
	{
		return;
	}

    char szText[256] = { 0 };
    ssprintf_s(szText, sizeof(szText), _GT("<color='#EEEA82FF'>[%s]</color>由于长时间不在线，系统自动将队长职位交与<color='#EEEA82FF'>[%s]</color>"), m_szShaikhName, pKinMember->szName);
	setMemberIdentity(pKinMember->dwPDBID, emKinIdentity_Shaikh, szText);
}

/*
* 队长卸任规则：
* 队长位置会给与战队内当天活跃度最高的玩家，相同则随机选择
*/
SKinMember* CKin::chooseNewShaikh()
{
    SKinConfig* pConfig = g_EHelper.getConfig();
    if (pConfig == NULL)
    {
        ErrorLn("KinConfig is null.");
        return NULL;
    }

	int maxActivity = -1;
	// 获得战队成员中当前周的最大活跃度值
	for (list<DWORD>::iterator iter = m_nMemberIDList.begin(); iter != m_nMemberIDList.end(); ++iter)
	{
        DWORD dwPDBID = *iter;
        if (dwPDBID == m_nShaikhID)
            continue;
		int activity = getMemberActivity(dwPDBID);
		if (maxActivity < activity)
		{
			maxActivity = activity;
		}
	}

    if (maxActivity < 0)
        return NULL;

	vector<DWORD> maxActivityList;
	for (list<DWORD>::iterator iter = m_nMemberIDList.begin(); iter != m_nMemberIDList.end(); ++iter)
	{
        DWORD dwPDBID = *iter;
        if (dwPDBID == m_nShaikhID)
            continue;
		if (maxActivity == getMemberActivity(dwPDBID))
		{
			maxActivityList.push_back(dwPDBID);
		}
	}

	if (maxActivityList.empty())
	{
		TraceLn("maxActivityList is empty.maxActivity=" << maxActivity << ", m_nMemberIDList.size=" << m_nMemberIDList.size());
		return NULL;
	}

    DWORD offlineSecs = (pConfig->nOfflineDays > 0 ? pConfig->nOfflineDays : 0) * 3600 * 24;
    DWORD outgoingSecs = (pConfig->nOutgoingDays > 0 ? pConfig->nOutgoingDays : 0) * 3600 * 24;
    vector<SKinMember*> filterMemberList;
    for(vector<DWORD>::iterator iter = maxActivityList.begin(); iter != maxActivityList.end(); ++iter)
    {
        DWORD dwPDBID = *iter;
        SKinMember* pKinMember = m_pKinService->getKinMember(dwPDBID);
        if (pKinMember == NULL)
        {
            TraceLn(__FUNCTION__ << "pKinMember is null.pdbid=" << dwPDBID);
            return NULL;
        }

        DWORD dwInterval = time(NULL) - pKinMember->nLogoutTime;
        if (dwInterval >= offlineSecs + outgoingSecs)
        {
            continue;
        }

        filterMemberList.push_back(pKinMember);
    }

    if (filterMemberList.empty())
    {
        TraceLn(__FUNCTION__ << " no have meet shaikh");
        return NULL;
    }

	int index;
	if (filterMemberList.size() == 1)
	{
		index = 0;
	}
	else
	{
		// 4. 在相同活跃度列表里随机选择
		index = rkt::mt_range_rand(0, maxActivityList.size() - 1);
	}

    SKinMember* pKinMember = filterMemberList[index];

    TraceLn(__FUNCTION__ << "maxActivity=" << maxActivity << ",count=" << filterMemberList.size() << ",newShaikh(actorID=" << pKinMember->dwPDBID << ",actorName=" << pKinMember->szName << ")");

	return pKinMember;
}

int CKin::getMemberActivity(DWORD dwPDBID)
{
	SKinMember* pKinMember = m_pKinService->getKinMember(dwPDBID);
	if (pKinMember == NULL)
	{
		return 0;
	}

	return pKinMember->nWeekActivity;
}

// 检测队长状态
void CKin::CheckShaikhState()
{
    SKinMember* pKinMember = m_pKinService->getKinMember(m_nShaikhID);
    if (pKinMember && pKinMember->bIsOnline)
        return;

    enterOfflineState();
}
