#include "StdAfx.h"
#include "MentorshipClient.h"
#include "ExternalHelper.h"
#include "MentorshipMsgDef.h"
#include "IClientEntity.h"
#include "GameViewDef.h"
#include "IBankPart.h"
#include "IChatClient.h"
#include "Mentorship_ManagedDef.h"

using namespace Mentorship_ManagerDef;

CMentorshipClient *g_pMentorshipClient = NULL;

// 构造函数
CMentorshipClient::CMentorshipClient()
{
}

// 虚构函数
CMentorshipClient::~CMentorshipClient()
{

}

bool CMentorshipClient::create(void)
{
	// 注册消息处理者
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_MENTORSHIP, static_cast<IMessageHandler*>(this));

	// 创建帮会
	if (m_Mentorship.create() == false)
		return false;

    // 注册显示层事件
    registerGameViewEvent();

	return true;
}

// 释放
void CMentorshipClient::release(void)
{
	// 取消注册消息处理者
	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_MENTORSHIP, static_cast<IMessageHandler*>(this));

    // 注销显示层事件
    unregisterGameViewEvent();

    m_Mentorship.release();

	g_pMentorshipClient = NULL;

	delete this;
}

////////////////////////IMessageHandler////////////////////
/** 消息处理
@param head : 消息头，通常为一个消息包的前4个字节
@param data : 消息的实际内容，除去了消息头的
@param len  : 内容长度
*/
void CMentorshipClient::onMessage(SNetMsgHead *pHead, void *pData, size_t nLen)
{
    // 客户端消息case宏
#define MSG_CASE(proc, msgID) \
		case msgID:\
    { proc((LPCSTR)pData, nLen);	break;}

    switch (pHead->byKeyAction)
    {
		// 师徒关系申请应答
		MSG_CASE(onApplyAnswerData, MSG_MENTORSHIP_APPLY_ANSWER)

		// 拜师收徒发布信息列表
		MSG_CASE(onIssueListData, MSG_MENTORSHIP_ISSUE_LIST)

		// 拜师收徒信息发布自动设置
		MSG_CASE(onIssueAutoSetData, MSG_MENTORSHIP_ISSUE_AUTOSET)

		// 导师的学徒列表
		MSG_CASE(onPrenticeListData, MSG_MENTORSHIP_MASTER_PRENTICELIST)

    default:
        m_Mentorship.onMessage(pHead, pData, nLen);
        break;
    }
}


bool CMentorshipClient::onViewEvent(int32 eventID,int nParam,const char *strParam,void *ptrParam)
{
    switch (eventID)
    {
	case GVIEWCMD_MENTORSHIP_REQUESTPERSONDATA:
		{
			mentorshipPersonDataRequest();
		}
		break;

    case GVIEWCMD_MENTORSHIP_REQUESTPRENTICELIST:
        {
			if (strParam == NULL)
			{
				return false;
			}
            cmd_mentorship_prenticelist_head *pData = (cmd_mentorship_prenticelist_head*)ptrParam;
			SMsgMentorshipMaster_PrenticeList sendData;
            prenticeListRequest(pData);
        }
        break;

	case GVIEWCMD_MENTORSHIP_REQUESTISSUELIST:
		{
			if (strParam == NULL)
			{
				return false;
			}
			cmd_mentorship_issuelist_head *pData = (cmd_mentorship_issuelist_head*)ptrParam;
			issueListRequest(pData);
		}
		break;

	case GVIEWCMD_MENTORSHIP_REQUESTISSUEINFO:
		{
			if (strParam == NULL)
			{
				return false;
			}
			cmd_mentorship_request_issue_info *pData = (cmd_mentorship_request_issue_info*)ptrParam;
			issueInfoRequest(pData);
		}
		break;

	case GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP:
		{
			if (strParam == NULL)
			{
				return false;
			}
			cmd_mentorship_request_apply *pData = (cmd_mentorship_request_apply*)ptrParam;
			applyMentorshipRequest(pData);
		}
		break;

	case GVIEWCMD_MENTORSHIP_REQUESTAPPLYANSWER:
		{
			if (strParam == NULL)
			{
				return false;
			}
			cmd_mentorship_request_applyanswer *pData = (cmd_mentorship_request_applyanswer*)ptrParam;
			applyAnswerRequest(pData);
		}
		break;

	case GVIEWCMD_MENTORSHIP_REQUESTDISMISS:
		{
			if (strParam == NULL)
			{
				return false;
			}
			cmd_mentorship_request_dismiss *pData = (cmd_mentorship_request_dismiss*)ptrParam;
			dismissRequest(pData);
		}
		break;

	case GVIEWCMD_MENTORSHIP_REQUESTPROMOTE:
		{
			if (strParam == NULL)
			{
				return false;
			}
			cmd_mentorship_request_promote *pData = (cmd_mentorship_request_promote*)ptrParam;
			promotePrenticeRequest(pData);
		}
		break;

	case GVIEWCMD_MENTORSHIP_REQUESTISSUEAUTOSET:
		{
			if (strParam == NULL)
			{
				return false;
			}
			cmd_mentorship_request_issueautoset *pData = (cmd_mentorship_request_issueautoset*)ptrParam;
			issueAutoSetRequest(pData);
		}
		break;

	case GVIEWCMD_MENTORSHIP_REQUESTFIRSTAWARD:
		{
			if (strParam == NULL)
			{
				return false;
			}
			cmd_mentorship_request_firstaward *pData = (cmd_mentorship_request_firstaward*)ptrParam;
			recvFirstAwardRequest(pData);
		}
		break;

    default:
        break;
    }

    return true;
}

// 注册显示层事件
void CMentorshipClient::registerGameViewEvent(void)
{
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient == NULL)
		return;

	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTPERSONDATA, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTPRENTICELIST, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTISSUELIST, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTISSUEINFO, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTAPPLYANSWER, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTDISMISS, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTPROMOTE, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTISSUEAUTOSET, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTFIRSTAWARD, this );
}

// 注销显示层事件
void CMentorshipClient::unregisterGameViewEvent(void)
{
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient == NULL)
		return;

	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTPERSONDATA );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTPRENTICELIST );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTISSUELIST );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTISSUEINFO  );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTAPPLYANSWER );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTDISMISS );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTPROMOTE );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTISSUEAUTOSET );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MENTORSHIP_REQUESTFIRSTAWARD );
}

// 师徒关系申请应答
void CMentorshipClient::onApplyAnswerData(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen != sizeof(SMsgMentorship_Apply_Anwser))
		return;
	SMsgMentorship_Apply_Anwser *pRecvData = (SMsgMentorship_Apply_Anwser*)pszMsg;

	obuf obData;
	cmd_mentorship_request_applyanswer sendData;
	sendData.nSrcPDBID = pRecvData->dwSrcPDBID;
	sstrcpyn(sendData.szSrcName, pRecvData->szSrcName, sizeof(sendData.szSrcName));
	sendData.nApplyPost = pRecvData->nApplyPost;
	sendData.nRelationType = pRecvData->nRelationType;
	sendData.nAnwser = (pRecvData->bAnwser == true) ? 1 : 0;
	obData.push_back(&sendData, sizeof(sendData));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MENTORSHIP_REQUESTAPPLYANSWER, obData.size(), 0, obData.data(), obData.size());
}

// 拜师收徒发布信息列表
void CMentorshipClient::onIssueListData(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen < sizeof(SMsgMentorshipIssueList))
		return;

	SMsgMentorshipIssueList *pRecvData = (SMsgMentorshipIssueList*)pszMsg;

	int offSet = sizeof(SMsgMentorshipIssueList);
	if (nLen != offSet + pRecvData->nCount * sizeof(SMsgMentorship_Issue_Node))
	{
		return;
	}

	obuf4096 obData;
	cmd_mentorship_issuelist_head sendData;
	sendData.nIssueType = pRecvData->nIssueType;
	sendData.nCount = pRecvData->nCount;
	obData.push_back(&sendData, sizeof(sendData));

	for (int i=0; i < pRecvData->nCount; i++)
	{
		SMsgMentorship_Issue_Node *pNode = (SMsgMentorship_Issue_Node*)(pszMsg + offSet);
		cmd_mentorship_issueinfo sendNode;
		sendNode.nPDBID = pNode->dwPDBID;
		sendNode.bySex = pNode->bySex;
		sendNode.nLevel = pNode->nLevel;
		sstrcpyn(sendNode.szName, pNode->szName, sizeof(sendNode.szName));
		sstrcpyn(sendNode.szClanName, pNode->szClanName, sizeof(sendNode.szClanName));
		sstrcpyn(sendNode.szCorpsName, pNode->szCorpsName, sizeof(sendNode.szCorpsName));
		sstrcpyn(sendNode.szIssueInfo, pNode->szIssueInfo, sizeof(sendNode.szIssueInfo));
		obData.push_back(&sendNode, sizeof(sendNode));

		offSet += sizeof(SMsgMentorship_Issue_Node);
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MENTORSHIP_REQUESTISSUELIST, obData.size(), 0, obData.data(), obData.size());
}

// 拜师收徒信息发布自动设置
void CMentorshipClient::onIssueAutoSetData(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen != sizeof(SMsgMentorshipIssue_AutoSet))
		return;
	SMsgMentorshipIssue_AutoSet *pRecvData = (SMsgMentorshipIssue_AutoSet*)pszMsg;

	if (pRecvData->nIssueType == emMentorshipIssueType_Master)
	{
		m_Mentorship.setNumProp(em_MshipPersonProp_AutoIssueMFlag, pRecvData->bAutoSet);
	}
	else if (pRecvData->nIssueType == emMentorshipIssueType_Prentice)
	{
		m_Mentorship.setNumProp(em_MshipPersonProp_AutoIssuePFlag, pRecvData->bAutoSet);
	}

	obuf obData;
	cmd_mentorship_request_issueautoset sendData;
	sendData.nIssueType = pRecvData->nIssueType;
	sendData.nAutoSet = (pRecvData->bAutoSet == true) ? 1 : 0;

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MENTORSHIP_REQUESTISSUEAUTOSET, obData.size(), 0, obData.data(), obData.size());
}

// 导师的学徒列表
void CMentorshipClient::onPrenticeListData(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen < sizeof(SMsgMentorshipMaster_PrenticeList))
		return;
	SMsgMentorshipMaster_PrenticeList *pRecvData = (SMsgMentorshipMaster_PrenticeList*)pszMsg;

	obuf4096 obData;
	cmd_mentorship_prenticelist_head sendData;
	sendData.nPDBID = pRecvData->dwMasterPDBID;
	sendData.nCount = pRecvData->nCount;
	obData.push_back(&sendData, sizeof(sendData));

	int offSet = sizeof(SMsgMentorshipMaster_PrenticeList);
	for (int i=0; i < pRecvData->nCount; i++)
	{
		SMsgMentorshipMaster_PrenticeNode *pNode = (SMsgMentorshipMaster_PrenticeNode*)(pszMsg + offSet);
		cmd_mentorship_prenticeinfo sendNode;
		sendNode.nMasterPDBID = pNode->dwMasterPDBID;
		sendNode.nPDBID = pNode->dwPDBID;
		sstrcpyn(sendNode.szName, pNode->szName, sizeof(sendNode.szName));
		sendNode.nFaceID = pNode->nFaceID;
		sendNode.nIsOnline = (pNode->bIsOnline == true) ? 1 : 0;
		sendNode.nRelationType = pNode->nRelationType;
		sendNode.nState = pNode->nState;
		obData.push_back(&sendNode, sizeof(sendNode));

		offSet += sizeof(SMsgMentorshipMaster_PrenticeNode);
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MENTORSHIP_REQUESTPRENTICELIST, obData.size(), 0, obData.data(), obData.size());
}

// 请求师徒数据请求
void CMentorshipClient::mentorshipPersonDataRequest()
{
	if (m_Mentorship.isVaild() == false)
	{
		SMsgMentorship_PersonData_OC sendData;
		sendData.nCount = 0;
		g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_PERSONDATA, &sendData, sizeof(sendData));
	}
	else
	{
		m_Mentorship.updatePersonDataToView();
	}
}

// 导师学徒列表请求
void CMentorshipClient::prenticeListRequest(cmd_mentorship_prenticelist_head *pData)
{
	SMsgMentorshipMaster_PrenticeList sendData;
	sendData.dwMasterPDBID = pData->nPDBID;
	sendData.nCount = 0;
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_MASTER_PRENTICELIST, &sendData, sizeof(sendData));
}

// 发布列表请求
void CMentorshipClient::issueListRequest(cmd_mentorship_issuelist_head *pData)
{
	SMsgMentorshipIssueList sendData;
	sendData.nIssueType = pData->nIssueType;
	sendData.nCount = 0;
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_ISSUE_LIST, &sendData, sizeof(sendData));
}

// 发布消息请求
void CMentorshipClient::issueInfoRequest(cmd_mentorship_request_issue_info *pData)
{
	if (pData->nIssueType != emMentorshipIssueType_Master && pData->nIssueType != emMentorshipIssueType_Prentice)
	{
		return;
	}

	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	IChatClient *pChatClient = gClientGlobal->getChatClient();
	if (pChatClient == NULL)
	{
		ErrorLn(__FUNCTION__ << "pChatClient == NULL" );
		return;
	}

	int nLevel = pClientEntity->getIntProperty(PROPERTY_LEVEL);

	if (pData->nIssueType == emMentorshipIssueType_Master)
	{
		if (nLevel < pConfig->nMasterNeedLevel)
		{
			pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_MASTER_NEED_LEVEL);
			return;
		}
	}
	else if (pData->nIssueType == emMentorshipIssueType_Prentice)
	{
		if (nLevel < pConfig->nPrenticeNeedLevel)
		{
			pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_PRENTICE_NEED_LEVEL);
			return;
		}
	}

	SMsgMentorship_Issue sendData;
	sendData.nIssueType = pData->nIssueType;
	sstrcpyn(sendData.szIssueInfo, pData->szIssueInfo, sizeof(sendData.szIssueInfo));
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_ISSUE, &sendData, sizeof(sendData));
}

// 拜师或收徒信息请求
void CMentorshipClient::applyMentorshipRequest(cmd_mentorship_request_apply *pData)
{
	if (pData->nRelationType != emMentorshipType_Formal && pData->nRelationType != emMentorshipType_Intern)
	{
		return;
	}

	if (pData->nApplyPost != emMentorshipPost_Master && pData->nApplyPost != emMentorshipPost_Prentice)
	{
		return;
	}

	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

	if (pClientEntity->getIntProperty(PROPERTY_ID) == pData->nDesPDBID)
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	IChatClient *pChatClient = gClientGlobal->getChatClient();
	if (pChatClient == NULL)
	{
		ErrorLn(__FUNCTION__ << "pChatClient == NULL" );
		return;
	}

	if (pData->nApplyPost == emMentorshipPost_Master)
	{
		if (pClientEntity->getIntProperty(PROPERTY_LEVEL) < pConfig->nMasterNeedLevel)
		{
			pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_MASTER_NEED_LEVEL);
			return;
		}
		if (pData->nRelationType == emMentorshipType_Formal && m_Mentorship.getFormalCount(emMentorshipPost_Prentice) >= pConfig->nFormalPrenticeCount)
		{
			pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_FORMAL_PRENTICE_FULL);
			return;
		}
		if (pData->nRelationType == emMentorshipType_Intern && m_Mentorship.getInternCount(emMentorshipPost_Prentice) >= pConfig->nInternPrenticeCount)
		{
			pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_INTERN_PRENTICE_FULL);
			return;
		}
	}
	else if (pData->nApplyPost == emMentorshipPost_Prentice)
	{
		if (pClientEntity->getIntProperty(PROPERTY_LEVEL) < pConfig->nPrenticeNeedLevel)
		{
			pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_PRENTICE_NEED_LEVEL);
			return;
		}
		if (pData->nRelationType == emMentorshipType_Formal && m_Mentorship.getFormalCount(emMentorshipPost_Master) >= pConfig->nFormalMasterCount)
		{
			pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_FORMAL_MASTER_FULL);
			return;
		}
		if (pData->nRelationType == emMentorshipType_Intern && m_Mentorship.getInternCount(emMentorshipPost_Master) >= pConfig->nInternMasterCount)
		{
			pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_INTERN_MASTER_FULL);
			return;
		}
	}

	SMsgMentorship_Apply sendData;
	sendData.dwDesPDBID = pData->nDesPDBID;
	sendData.nApplyPost = pData->nApplyPost;
	sendData.nRelationType = pData->nRelationType;
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_APPLY, &sendData, sizeof(sendData));
}

// 拜师或收徒应答信息请求
void CMentorshipClient::applyAnswerRequest(cmd_mentorship_request_applyanswer *pData)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

	SMsgMentorship_Apply_Anwser sendData;
	sendData.dwSrcPDBID = pData->nSrcPDBID;
	sendData.bAnwser = (pData->nAnwser == 1) ? true : false;
	sendData.nApplyPost = pData->nApplyPost;
	sendData.nRelationType = pData->nRelationType;
	sstrcpyn(sendData.szSrcName, pData->szSrcName, sizeof(sendData.szSrcName));
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_APPLY_ANSWER, &sendData, sizeof(sendData));
}

// 提示学徒信息请求
void CMentorshipClient::promotePrenticeRequest(cmd_mentorship_request_promote *pData)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	IChatClient *pChatClient = gClientGlobal->getChatClient();
	if (pChatClient == NULL)
	{
		ErrorLn(__FUNCTION__ << "pChatClient == NULL" );
		return;
	}

	if (m_Mentorship.getFormalCount(emMentorshipPost_Prentice) >= pConfig->nFormalPrenticeCount)
	{
		pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_FORMAL_PRENTICE_FULL);
		return;
	}

	SMsgMentorship_Promote sendData;
	sendData.dwMentorshipID = pData->nMentorshipID;
	sendData.dwDesPDBID = pData->nDesPDBID;
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_PROMOTE, &sendData, sizeof(sendData));
}

// 自动发布信息请求
void CMentorshipClient::issueAutoSetRequest(cmd_mentorship_request_issueautoset *pData)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

	SMentorshipConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
	{
		return;
	}

	IChatClient *pChatClient = gClientGlobal->getChatClient();
	if (pChatClient == NULL)
	{
		ErrorLn(__FUNCTION__ << "pChatClient == NULL" );
		return;
	}

	if (pData->nIssueType != emMentorshipIssueType_Master && pData->nIssueType != emMentorshipIssueType_Prentice)
	{
		return;
	}

	if (pData->nIssueType == emMentorshipIssueType_Master && pClientEntity->getIntProperty(PROPERTY_LEVEL) < pConfig->nMasterNeedLevel)
	{
		pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_MASTER_NEED_LEVEL);
		return;
	}
	else if (pData->nIssueType == emMentorshipIssueType_Prentice && pClientEntity->getIntProperty(PROPERTY_LEVEL) < pConfig->nPrenticeNeedLevel)
	{
		pChatClient->showSystemMessage(CHAT_TIP_MENTORSHIP_PRENTICE_NEED_LEVEL);
		return;
	}

	SMsgMentorshipIssue_AutoSet sendData;
	sendData.nIssueType = pData->nIssueType;
	sendData.bAutoSet = (pData->nAutoSet == 1) ? true :false;
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_ISSUE_AUTOSET, &sendData, sizeof(sendData));
}

// 解散师徒
void CMentorshipClient::dismissRequest(cmd_mentorship_request_dismiss *pData)
{
	SMsgMentorship_Dismiss sendData;
	sendData.dwMentorshipID = pData->nMentorshipID;
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_DISMISS, &sendData, sizeof(sendData));
}

// 领取师徒首胜奖励
void CMentorshipClient::recvFirstAwardRequest(cmd_mentorship_request_firstaward *pData)
{
	SMsgMentorship_Req_FirstAward sendData;
	sendData.nPost = pData->nPost;
	g_EHelper.sendMentorshipSocial(emMsgMentorship_Module, MSG_MENTORSHIP_REQUEST_FIRST_AWARD, &sendData, sizeof(sendData));
}

API_EXPORT IMentorshipClient* CreateMentorshipClient(void)
{
	// 如果创建了，则直接返回
	if(g_pMentorshipClient != NULL)
	{
		return g_pMentorshipClient;
	}

	// 实例化系统模块
	g_pMentorshipClient = new CMentorshipClient();
	if(g_pMentorshipClient == NULL || !g_pMentorshipClient->create())
	{
		SAFE_RELEASE(g_pMentorshipClient);

		return NULL;
	}

	return g_pMentorshipClient;
}