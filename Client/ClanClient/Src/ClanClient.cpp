#include "StdAfx.h"
#include "ClanClient.h"
#include "ExternalHelper.h"
#include "ClanMessageDef.h"
#include "IClientEntity.h"
#include "GameViewDef.h"
#include "IBankPart.h"
using namespace Clan_ManagerDef;

CClanClient *g_pClanClient = NULL;

// 构造函数
CClanClient::CClanClient()
{
	m_bMemberVaild = false;
}

// 虚构函数
CClanClient::~CClanClient()
{

}

// 查找帮会联盟信息
bool CClanClient::create(void)
{
	// 注册消息处理者
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_CLAN, static_cast<IMessageHandler*>(this));

	// 创建帮会
	if (m_Clan.create() == false)
		return false;

    // 注册显示层事件
    registerGameViewEvent();

	return true;
}

// 释放
void CClanClient::release(void)
{
	// 取消注册消息处理者
	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_CLAN, static_cast<IMessageHandler*>(this));

    // 注销显示层事件
    unregisterGameViewEvent();

    m_Clan.release();

	g_pClanClient = NULL;

	delete this;
}

////////////////////////IMessageHandler////////////////////
/** 消息处理
@param head : 消息头，通常为一个消息包的前4个字节
@param data : 消息的实际内容，除去了消息头的
@param len  : 内容长度
*/
void CClanClient::onMessage(SNetMsgHead *pHead, void *pData, size_t nLen)
{
    // 客户端消息case宏
#define MSG_CASE(proc, msgID) \
		case msgID:\
    { proc((LPCSTR)pData, nLen);	break;}

    switch (pHead->byKeyAction)
    {
        // 请求帮会列表
        MSG_CASE(onClanList, MSG_CLAN_LIST)

        // 更新自身联盟数据
        MSG_CASE(onUpdateSelfClanInfo, MSG_CLAN_UPDATESELFCLANINFO)

        // 退出帮会
        MSG_CASE(onQuit, MSG_CLAN_QUIT)

        // 接到申请消息
        MSG_CASE(onApply, MSG_CLAN_APPLY)

        // 接到回复申请消息
        MSG_CASE(onAdmit, MSG_CLAN_ADMIT)

        // 创建帮会成功事件消息
        MSG_CASE(onCreateSucceed, MSG_CLAN_CREATE_SUCCEED)

		// 申请返回
		MSG_CASE(onApplyRet, MSG_CLAN_APPLY_RET)

		// 邀请加入联盟
		MSG_CASE(onInviteJoinClan, MSG_CLAN_INVITE)

		// 邀请答复
		MSG_CASE(onAnswerInviteJoinClan, MSG_CLAN_ANSWER_INVITE)

    default:
        m_Clan.onMessage(pHead, pData, nLen);
        break;
    }
}

void CClanClient::onInviteJoinClan(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_invite_oc))
		return;

	msg_clan_invite_oc* pMsg = (msg_clan_invite_oc*)pszMsg;

	cmd_clan_invite_ret inviteRet;
	inviteRet.nInviterPDBID = pMsg->nInviterPDBID;
	sstrcpyn(inviteRet.szInviterName, pMsg->szInviterName, sizeof(inviteRet.szInviterName));
	sstrcpyn(inviteRet.szClanName, pMsg->szClanName, sizeof(inviteRet.szClanName));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_INVITE_RET, 0, 0, &inviteRet, sizeof(inviteRet));
}


void CClanClient::onAnswerInviteJoinClan(LPCSTR pszMsg, int nLen)
{
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_ANSWER_INVITE_SUCCESS, 0, 0, 0, 0);
}


bool CClanClient::onViewEvent(int32 eventID,int nParam,const char *strParam,void *ptrParam)
{
    switch (eventID)
    {
	case GVIEWCMD_CLAN_REQUESTCLANLIST:
		{
			// 请求帮会列表请求
			cmd_clan_request_clanlist *pData = (cmd_clan_request_clanlist *)ptrParam;
			clanListRequest(pData->nPageNum, pData->szClanName);
		}
		break;

    case GVIEWCMD_CLAN_REQUESTCLANBASEINFO:
        {
			// 请求帮会详细信息
            openFormRequest();
        }
        break;

    case GVIEWCMD_CLAN_CREATE:          // 创建帮会
        {
            if (ptrParam == NULL)
            {
                break;
            }
            cmd_clan_create *pData = (cmd_clan_create*)ptrParam;
            // 创建帮会请求
            createClanRequest(pData);
        }
        break;

    case GVIEWCMD_CLAN_APPLY:           // 申请加入帮会
        {
            if (ptrParam == NULL)
                break;

            cmd_clan_apply *pData = (cmd_clan_apply *)ptrParam;
            // 加入帮会
            joinRequest(pData->nClanID);
        }
        break;

    case GVIEWCMD_CLAN_QUIT:
        {
            quitRequest();
        }
		break;

	case GVIEWCMD_CLAN_DISMISS:
		{
			dismissRequest();
		}
		break;
    case GVIEWCMD_CLAN_CANCEL_DISMISS:
        {
            cancelDismissRequest();
        }
        break;
	case GVIEWCMD_CLAN_SETLEADER:
		{
			if (ptrParam == NULL)
				break;

			cmd_clan_setleader *pData = (cmd_clan_setleader *)ptrParam;
			// 任命指挥官
			setClanLeader(pData->nActorID);
		}
		break;

	case GVIEWCMD_CLAN_KICKKIN:
		{
			if (ptrParam == NULL)
				break;

			cmd_clan_kickkin *pData = (cmd_clan_kickkin *)ptrParam;
			kickRequest(pData->nActorID);
		}
		break;

    case GVIEWCMD_CLAN_APPLY_LIST:
        {
            applyListRequest();
        }
        break;

    case GVIEWCMD_CLAN_ADMIT:
        {
            if (ptrParam == NULL)
                break;

            cmd_clan_admit *pData = (cmd_clan_admit *)ptrParam;
            // 接收成员
            acceptRequest(pData->nKinID, pData->bAgree > 0);
        }
        break;

    case GVIEWCMD_CLAN_REQUEST_CARDLIST:
        {
            // 请求卡牌列表
            requestCardList();
        }
        break;

    case GVIEWCMD_CLAN_REQUEST_CARD:
        {
            if (ptrParam == NULL)
                break;

            cmd_clan_request_card *pData = (cmd_clan_request_card *)ptrParam;
            // 请求支援
            requestCard(pData->nHeroID);
        }
        break;

    case GVIEWCMD_CLAN_DONATE_CARD:
        {
            if (ptrParam == NULL)
                break;

            cmd_clan_donate_card *pData = (cmd_clan_donate_card *)ptrParam;
            // 捐赠卡牌
            donateCard(pData->nDBID, pData->nCardID, pData->nCardCount);
        }
        break;

    case GVIEWCMD_CLAN_EVENT_LIST:
        {
            // 获取本帮会事件
            requestClanEvent();
        }
        break;

	case GVIEWCMD_CLAN_MODIFY_NOTICE:
		{
			// 修改公告
			if (ptrParam == NULL)
				break;

			cmd_clan_modify_notice *pData = (cmd_clan_modify_notice*)ptrParam;
			motifyRequest(emClanNotice, pData->szText);
		}
		break;

	case GVIEWCMD_CLAN_SET_AUTOACCEPT:
		{
			if (ptrParam == NULL)
				break;

			cmd_clan_set_autoacceptflag *pData = (cmd_clan_set_autoacceptflag*)ptrParam;
			requestSetAutoAcceptFlag(pData->nAutoAcceptFlag);
		}
		break;

	// 邀请加入联盟
	case GVIEWCMD_CLAN_INVITE:
	    {
		    if (ptrParam == NULL)
		    {
			    break;
		    }
		    cmd_clan_invite * cmdData = (cmd_clan_invite *)ptrParam;
		    msg_clan_invite_co sendData;
		    sendData.dwInviteePDBID = cmdData->nInviteePDBID;

		    g_EHelper.sendClanSocial(MSG_CLAN_MODULE, MSG_CLAN_INVITE, m_Clan.getNumProp(emClanProp_ID), &sendData, sizeof(sendData));
	    }
	    break;
	case  GVIEWCMD_CLAN_ANSWER_INVITE:
	    {
		    // 邀请答复
		    if (ptrParam == NULL)
		    {
			    return false;
		    }

		    cmd_clan_answer_invite * cmdData = (cmd_clan_answer_invite *)ptrParam;
		    msg_clan_answer_invite_co msg;
		    msg.dwInviterPDBID = cmdData->dwPDBID;
		    msg.bIsAgree = cmdData->bIsAgree;

		    g_EHelper.sendClanSocial(MSG_CLAN_MODULE, MSG_CLAN_ANSWER_INVITE, m_Clan.getNumProp(emClanProp_ID), &msg, sizeof(msg));
	    }
	    break;
    default:
        break;
    }

    return true;
}

// 注册显示层事件
void CClanClient::registerGameViewEvent(void)
{
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient == NULL)
		return;

	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_REQUESTCLANLIST, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_REQUESTCLANBASEINFO, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_CREATE, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_APPLY, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_QUIT, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_DISMISS, this );
    pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_CANCEL_DISMISS, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_SETLEADER, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_KICKKIN, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_APPLY_LIST, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_ADMIT, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_REQUEST_CARDLIST, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_REQUEST_CARD, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_DONATE_CARD, this );
    pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_EVENT_LIST, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_MODIFY_NOTICE, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_CLAN_SET_AUTOACCEPT, this );	
	pEntityClient->registerGameViewEventHandler(GVIEWCMD_CLAN_INVITE, this);
	pEntityClient->registerGameViewEventHandler(GVIEWCMD_CLAN_ANSWER_INVITE, this);
}

// 注销显示层事件
void CClanClient::unregisterGameViewEvent(void)
{
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient == NULL)
		return;

	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_REQUESTCLANLIST );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_REQUESTCLANBASEINFO );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_CREATE );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_APPLY );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_QUIT );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_DISMISS );
    pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_CANCEL_DISMISS );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_SETLEADER );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_KICKKIN );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_APPLY_LIST );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_ADMIT );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_REQUEST_CARDLIST );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_REQUEST_CARD );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_DONATE_CARD );
    pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_EVENT_LIST );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_MODIFY_NOTICE );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_CLAN_SET_AUTOACCEPT ); 
	pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_CLAN_INVITE);
	pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_CLAN_ANSWER_INVITE);
}

// 请求帮会列表
void CClanClient::onClanList(LPCSTR pszMsg, int nLen)
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
        return;

    IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
    if (pEventEngine == NULL)
        return;

 	if (pszMsg == NULL || nLen < sizeof(msg_clan_clanlist_oc))
		return;
	msg_clan_clanlist_oc *pRecvData = (msg_clan_clanlist_oc*)pszMsg;
	if (nLen != sizeof(msg_clan_clanlist_oc) + pRecvData->nClanCount*sizeof(msg_clan_clanlist_node_oc))
		return;

    obuf obData;
    cmd_clan_list_head headData;
    headData.nClanCount = pRecvData->nClanCount;
    headData.nPageNum   = pRecvData->nPageNum;
    headData.nTotalNum = pRecvData->nTotalClanCount;
    headData.nApplyClanID = pRecvData->nApplyClanID;
    obData.push_back(&headData, sizeof(headData));
	
	int offset = sizeof(msg_clan_clanlist_oc);
	for (int nIndex = 0; nIndex < pRecvData->nClanCount; ++nIndex)
	{
		if (nIndex >= CLAN_LIST_PAGE_SIZE)
			break;

		msg_clan_clanlist_node_oc *pNode = (msg_clan_clanlist_node_oc*)(pszMsg + offset);

        cmd_clan_clanlist_node clanListNode;
		clanListNode.nClanID = pNode->dwID;
		sstrcpyn(clanListNode.szName, pNode->szName, sizeof(clanListNode.szName));
		sstrcpyn(clanListNode.szShaikhName, pNode->szShaikhName, sizeof(clanListNode.szShaikhName));
		clanListNode.nFlag = pNode->nFlag;
		clanListNode.nLevel = pNode->nLevel;
		clanListNode.nKinCount = pNode->nKinCount;
		sstrcpyn(clanListNode.szManifesto, pNode->szManifesto, sizeof(clanListNode.szManifesto));
		clanListNode.nMemberCount = pNode->nMemberCount;
		clanListNode.nState = pNode->nState;
        clanListNode.nClanTotalFight = pNode->nClanTotalFight;
		clanListNode.nWeekActivity = pNode->nWeekActivity;
        obData.push_back(&clanListNode, sizeof(clanListNode));

		offset += sizeof(msg_clan_clanlist_node_oc);
	}
    // 请求帮会列表
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_REQUESTCLANLIST, 0, 0, obData.data(), obData.size());
}

// 更新成员数据
void CClanClient::onUpdateSelfClanInfo(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(SHeroClanInfo))
		return;

	m_bMemberVaild = true;
	memcpy(&m_HeroClanInfo, pszMsg, sizeof(SHeroClanInfo));
}

// 退出帮会
void CClanClient::onQuit(LPCSTR pszMsg, int nLen)
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;
    IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
    if (pEventEngine == NULL)
        return;

	m_bMemberVaild = false;
	memset(&m_HeroClanInfo, 0, sizeof(SHeroClanInfo));

	m_Clan.reset();

	// 通知显示层已退出联盟
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_ONQUIT, 0, NULL, NULL, 0);
}

// 接到申请消息
void CClanClient::onApply(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_apply_oc))
        return;

	if (!m_Clan.isVaild())
		return;

	msg_clan_apply_oc *pRecvData = (msg_clan_apply_oc*)pszMsg;

	m_Clan.removeApplyMember(pRecvData->dwKinID);
	SApplyMember applyMember;
	applyMember.dwKinID = pRecvData->dwKinID;
	sstrcpyn(applyMember.szName, pRecvData->szName, sizeof(applyMember.szName));
	sstrcpyn(applyMember.szShaikhName, pRecvData->szShaikhName, sizeof(applyMember.szShaikhName));
    applyMember.nMemberCount = pRecvData->nMemberCount;
	m_Clan.addApplyMember(&applyMember);
}

// 接到申请加入返回消息
void CClanClient::onApplyRet(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_apply_ret_oc))
		return;

	msg_clan_apply_ret_oc *pRecvData = (msg_clan_apply_ret_oc*)pszMsg;

	obuf ob;
	cmd_clan_apply_ret applyRet;
	applyRet.nOldClanID = pRecvData->dwOldClanID;
	applyRet.nClanID = pRecvData->dwClanID;
	applyRet.nApplyState = pRecvData->nApplystate;

	ob.push_back(&applyRet, sizeof(cmd_clan_apply_ret));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_APPLY_RET, 0, 0, ob.data(), ob.size());
}

// 接到回复申请加入消息
void CClanClient::onAdmit(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_admit_oc))
		return;

	if (!m_Clan.isVaild())
		return;

	msg_clan_admit_oc *pRecvData = (msg_clan_admit_oc*)pszMsg;
	m_Clan.removeApplyMember(pRecvData->dwKinID);
}

// 创建帮会成功
void CClanClient::onCreateSucceed(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_oncreatesucceed_oc))
        return;
    msg_clan_oncreatesucceed_oc *pData = (msg_clan_oncreatesucceed_oc*)pszMsg;

    obuf obData;

    cmd_clan_create_succeed createSucceed;
    createSucceed.nSucceed = pData->bSucceed;
    obData.push_back(&createSucceed, sizeof(createSucceed));
    // 创建帮会成功
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_CREATE_SUCCEED, 0, 0, obData.data(), obData.size());
}

// 请求帮会列表请求
void CClanClient::clanListRequest(int nPageNum, char* szClanName)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	msg_clan_clanlist_co sendData;
	sendData.nPageNum = nPageNum;
	sstrcpyn(sendData.szClanName, szClanName, sizeof(sendData.szClanName));
	g_EHelper.sendClanSocial(MSG_CLAN_MODULE, MSG_CLAN_LIST, 0, &sendData, sizeof(sendData));
}

// 打开帮会界面
void CClanClient::openFormRequest()
{
	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_CLANBASEDATA, m_Clan.getNumProp(emClanProp_ID), "", 0);
}

// 创建帮会请求
void CClanClient::createClanRequest(cmd_clan_create *pData)
{
	msg_clan_create_co sendData;
	sstrcpyn(sendData.szName, pData->szClanName, sizeof(sendData.szName));
	sstrcpyn(sendData.szManifesto, pData->szManifesto, sizeof(sendData.szManifesto));
	sendData.nClanFlag = pData->nClanFlag;
	g_EHelper.sendClanSocial(MSG_CLAN_MODULE, MSG_CLAN_CREATE, 0, &sendData, sizeof(sendData));
}

// 加入帮会
void CClanClient::joinRequest(DWORD dwClanID)
{
	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_APPLY, dwClanID, "", 0);
}

// 退出帮会
void CClanClient::quitRequest()
{
	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_QUIT, m_Clan.getNumProp(emClanProp_ID), "", 0);
}

// 解散帮会
void CClanClient::dismissRequest()
{
	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_DISMISS, m_Clan.getNumProp(emClanProp_ID), "", 0);
}

// 任命指挥官
void CClanClient::setClanLeader(int nDBID)
{
    msg_clan_appoint_co appoint;
    appoint.dwMemberPDBID = nDBID;
    g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_APPOINT, m_Clan.getNumProp(emClanProp_ID), &appoint, sizeof(appoint));
}

// 加入列表请求
void CClanClient::applyListRequest()
{
	if (m_Clan.isVaild() == false)
		return;

    obuf obData;
    int nApplyMemberCount = m_Clan.getApplyMemberCount();
    cmd_clan_applymember_head applyMemberHead;
    applyMemberHead.nCount = nApplyMemberCount;
    obData.push_back(&applyMemberHead, sizeof(applyMemberHead));

    for (int i=0; i<nApplyMemberCount; ++i)
    {
        SApplyMember *pApplyMember = m_Clan.getApplyMemberByIndex(i);
        if (pApplyMember == NULL)
            continue;

        cmd_clan_applymember applymember;
        applymember.nKinID = pApplyMember->dwKinID;
        sstrcpyn(applymember.szName, pApplyMember->szName, sizeof(applymember.szName));
        sstrcpyn(applymember.szShaikhName, pApplyMember->szShaikhName, sizeof(applymember.szShaikhName));
		applymember.nMemberCount = pApplyMember->nMemberCount;
        obData.push_back(&applymember, sizeof(applymember));
    }

    // 请求帮会申请人员列表
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_APPLY_LIST, 0, 0, obData.data(), obData.size());
}

// 接收成员
void CClanClient::acceptRequest(DWORD dwKinID, bool bAgree)
{
	msg_clan_admit_co sendData;
	sendData.dwKinID = dwKinID;
	sendData.bAgree = bAgree;

	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_ADMIT, m_Clan.getNumProp(emClanProp_ID), &sendData, sizeof(sendData));
}

// 请求卡牌列表
void CClanClient::requestCardList()
{
	int nClanID = m_Clan.getNumProp(emClanProp_ID);
	if (nClanID <= 0)
		return;

	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_REQUEST_CARD_LIST, nClanID);
}

// 请求支援
void CClanClient::requestCard(int nHeroID)
{
	msg_clan_requestcard_co card;
	card.nCardID = nHeroID;
	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_REQUEST_CARD, m_Clan.getNumProp(emClanProp_ID), &card, sizeof(card));
}

// 捐赠卡牌
void CClanClient::donateCard(int nDBID, int nCardID, int nCardCount)
{
	msg_clan_donatecard_co card;
	card.dwByDBID = nDBID;
	card.nCardID = nCardID;
	card.nCardCount = nCardCount;
	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_DONATE_CARD, m_Clan.getNumProp(emClanProp_ID), &card, sizeof(card));
}

// 修改公告
void CClanClient::motifyRequest(int nPropID, char* szText)
{
	msg_clan_setstringprop_co sendData;
	sendData.nPropID = nPropID;
	sstrcpyn(sendData.szText, szText, sizeof(sendData.szText));

	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_SETSTRINGPROP, m_Clan.getNumProp(emClanProp_ID), &sendData, sizeof(sendData));
}

// 开除成员
void CClanClient::kickRequest(DWORD dwPDBID)
{
	msg_clan_kick_co sendData;
	sendData.dwMemberPDBID = dwPDBID;

	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_KICK, m_Clan.getNumProp(emClanProp_ID), &sendData, sizeof(sendData));
}

// 开除帮会内的战队成员
void CClanClient::kickKinMemberRequest(DWORD dwPDBID)
{
	msg_clan_kickkinmember_co sendData;
	sendData.dwMemberPDBID = dwPDBID;

	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_KICKKINMEMBER, m_Clan.getNumProp(emClanProp_ID), &sendData, sizeof(sendData));
}

// 获取本帮会事件
void CClanClient::requestClanEvent()
{
	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_UPDATEEVENT, m_Clan.getNumProp(emClanProp_ID), "", 0);
}

// 取消解散帮会
void CClanClient::cancelDismissRequest()
{
	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_CANEL_DISMISS, m_Clan.getNumProp(emClanProp_ID), "", 0);
}

// 请求设置自动接受
void CClanClient::requestSetAutoAcceptFlag(int nAutoAcceptFlag)
{
	msg_clan_setintprop_co sendData;
	sendData.nPropID = emClanProp_AutoAcceptFlag;
	sendData.nValue = nAutoAcceptFlag;

	g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_SETINTPROP, m_Clan.getNumProp(emClanProp_ID), &sendData, sizeof(sendData));
}

bool CClanClient::canPublicClanLegenCup()
{
    SClanConfig* pConfig = g_EHelper.getConfig();
    if (pConfig == NULL)
    {
        ErrorLn(__FUNCTION__": pConfig == NULL");
        return false;
    }

    if (!m_Clan.isVaild())
    {
        return false;
    }
    int nLegendCupCount = m_Clan.getNumProp(emClanProp_LegendCupCount);
    int nWeekActivity = m_Clan.getNumProp(emClanProp_WeekActivity);
    bool bCanPublicCup = true;
    do 
    {
        if ( nLegendCupCount <= 0)
        {
            bCanPublicCup = false;
            break;
        }

        if (nWeekActivity < pConfig->nClanCupMiniWeekActivity)
        {
            bCanPublicCup = false;
            break;
        }
    } while (false);

    return bCanPublicCup;
}

API_EXPORT IClanClient* CreateClanClient(void)
{
	// 如果创建了，则直接返回
	if(g_pClanClient != NULL)
	{
		return g_pClanClient;
	}

	// 实例化系统模块
	g_pClanClient = new CClanClient();
	if(g_pClanClient == NULL || !g_pClanClient->create())
	{
		SAFE_RELEASE(g_pClanClient);

		return NULL;
	}

	return g_pClanClient;
}