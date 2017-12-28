#include "stdafx.h"
#include "KinClient.h"
#include "IClientEntity.h"
#include "GameViewDef.h"
#include "KinMsgDef.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"
#include "ExternalHelper.h"
#include "IChatClient.h"
#include "Kin_ManagedDef.h"
#include "KinSchemeDef.h"
using namespace Kin_ManagerDef;

KinClient* g_pKinClient = NULL;

/// purpose: 构造函数
KinClient::KinClient()
{
    m_bMemberVaild = false;
    m_myApplyKinID = 0;
    m_nCoolingTime = 0;
    memset(&m_MemberInfo, 0, sizeof(m_MemberInfo));
}

/// purpose: 虚构函数
KinClient::~KinClient()
{

}

void KinClient::release(void)
{
    gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_KIN, this);
    
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_SOCIAL_CLOSE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_OPEN);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_APPLY);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_CREATE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_QUIT);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_INVITE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_KICK);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_ADMIT);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_DISMISS);

    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_REQUESTKINLIST);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_MOTIFYNOTICE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_ANSWER_INVITE);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_LEGENDCUPKININFO_OPEN);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_LEGENDCUPKINGLORY_OPEN);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_APPOINT_IDENTITY);
    

	// 全局指针NULL
	g_pKinClient = NULL;

	delete this;
}


bool KinClient::Create(void)
{
    IMessageDispatch* pMessageDispatch = gClientGlobal->getMessageDispatch();
    if (pMessageDispatch == NULL)
    {
        return false;
    }

    pMessageDispatch->registerMessageHandler(MSG_MODULEID_KIN, this);	
    
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_SOCIAL_CLOSE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_OPEN,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_APPLY,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_CREATE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_QUIT,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_INVITE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_KICK,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_ADMIT,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_DISMISS,this );

    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_REQUESTKINLIST,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_MOTIFYNOTICE,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_ANSWER_INVITE,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_LEGENDCUPKININFO_OPEN,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_LEGENDCUPKINGLORY_OPEN,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_APPOINT_IDENTITY,this );
    

    // 请求打开排行榜
	return true;
}

bool KinClient::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{
    IChatClient *pChatClient = gClientGlobal->getChatClient();
    if (pChatClient == NULL)
    {
        ErrorLn(__FUNCTION__ << "pChatClient == NULL" );
        return false;
    }

    switch ( eventID )
    {
    case  GVIEWCMD_SOCIAL_CLOSE :
        {
            
            // 移除观察战队
            exitObserveRequest();

            // 移除观察战队列表
            g_EHelper.SendKinSocial(emMsgKin_Module,  MSG_KIN_LIST_EXITOBSERVE, 0,"", 0);
        }
        break;   

    case  GVIEWCMD_KIN_OPEN :
        {
            // 请求打开战队信息
            if (m_Kin.IsVaild() == false)
            {
                pChatClient->showSystemMessage(CHAT_TIP_KIN_HAVE_NO_KIN);
                return false;
            }
            // 添加观察战队列表
            enterObserveRequest();
            g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_KINBASEDATA, m_Kin.getNumProp(emKinProp_ID), "", 0);
        }
        break;
        // 申请加入战队
    case GVIEWCMD_KIN_APPLY:
        {
            if (m_Kin.IsVaild())
            {
                // 提示已经有战队了
                pChatClient->showSystemMessage(CHAT_TIP_KIN_HAD_JOINED_KIN);
                return false;
            }

            if (ptrParam == NULL)
            {
                return false;
            }

            cmd_kin_apply * cmdData = (cmd_kin_apply *)ptrParam;
            joinRequest(cmdData->nKinID,"申请加入");
        }
        break;

    case  GVIEWCMD_KIN_CREATE :
        {
            if (m_Kin.IsVaild())
            {
                // 提示已经有战队了
                pChatClient->showSystemMessage(CHAT_TIP_KIN_HAD_JOINED_KIN);
                return false;
            }

            // 创建战队
            if (ptrParam == NULL)
            {
                return false;
            }
            cmd_kin_create * cmdData = (cmd_kin_create *)ptrParam;
            createKinRequest(cmdData->szKinName, cmdData->szKinLabel);
            
        }
        break;
    case  GVIEWCMD_KIN_REQUESTKINLIST :
        {
            // 申请战队列表
            if (ptrParam == NULL)
            {
                return false;
            }
            cmd_kin_requestkinlist * cmdData = (cmd_kin_requestkinlist *)ptrParam;
            SMsgKin_KinList_CO data;
            data.nCurIndex = cmdData->nCurIndex;
            data.nPageNum = cmdData->nPageNum;
            data.nState = cmdData->nState;
            memcpy(data.szKinName,cmdData->szKinName, sizeof(data.szKinName));

            g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_LIST, 0, &data, sizeof(data));

        }
        break;
        
    case GVIEWCMD_KIN_QUIT:
        {
            // 退出战队
            quitRequest();
            
        }
        break;
        // 邀请加入战队 
    case GVIEWCMD_KIN_INVITE:
        {
            if (ptrParam == NULL)
            {
                return false;
            }
            cmd_kin_invite * cmdData = (cmd_kin_invite *)ptrParam;
            SMsgKin_Invite_CO data;
            data.dwAnswerPDBID = cmdData->nAnswerPDBID;			// 邀请人员PDBID
            g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_INVITE, m_Kin.getNumProp(emKinProp_ID), &data, sizeof(data));
            
        }
        break;

        // 开除成员
    case GVIEWCMD_KIN_KICK:
        {
            if (ptrParam == NULL)
            {
                return false;
            }
            cmd_kin_kick * cmdData = (cmd_kin_kick *)ptrParam;
            SMsgKin_Kick_CO data;
            data.dwMemberPDBID = cmdData->nMemberPDBID;			// 踢出者PDBID
            g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_KICK, m_Kin.getNumProp(emKinProp_ID), &data, sizeof(data));
        }
        break;
        // 接收拒绝成员
    case GVIEWCMD_KIN_ADMIT:
        {
            // 申请战队列表
            if (ptrParam == NULL)
            {
                return false;
            }
            cmd_kin_admit * cmdData = (cmd_kin_admit *)ptrParam;
            SMsgKin_Admit_CO data;
            data.dwApplyPDBID = cmdData->nApplyPDBID;			// 申请者ID
            data.bAgree = cmdData->bAgree == 1? true :false;				// 同意/拒绝

            g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_ADMIT, m_Kin.getNumProp(emKinProp_ID), &data, sizeof(data));

        }
        break;

    case  GVIEWCMD_KIN_DISMISS :
        {
            // 解散请求 
            dismissRequest();
        }
        break;
    case  GVIEWCMD_KIN_MOTIFYNOTICE :
        {
            // 编辑公告 
            if (ptrParam == NULL)
            {
                return false;
            }

            cmd_kin_motifynotice * cmdData = (cmd_kin_motifynotice *)ptrParam;
            MotifyRequest(emKinNotice, cmdData->szText);
        }
        break;
    case  GVIEWCMD_KIN_APPOINT_IDENTITY :
        {
            // 设置家族职位
            if (ptrParam == NULL)
            {
                return false;
            }

            cmd_kin_appoint_identity * cmdData = (cmd_kin_appoint_identity *)ptrParam;
            appointRequest(cmdData->dwPDBID, cmdData->nIdentity);
        }
        break;
        

    case  GVIEWCMD_KIN_RENAME :
        {
            // 战队改名
            if (ptrParam == NULL)
            {
                return false;
            }

            cmd_kin_reqest_rename * cmdData = (cmd_kin_reqest_rename *)ptrParam;
            requestKinRename(cmdData->szKinName);
        }
        break;

    case  GVIEWCMD_KIN_ANSWER_INVITE :
        {
            // 邀请答复
            if (ptrParam == NULL)
            {
                return false;
            }

            cmd_kin_answer_Invite * cmdData = (cmd_kin_answer_Invite *)ptrParam;
            answerInvite(cmdData->dwPDBID, cmdData->bIsAgree);
            
        }
        break;

	case GVIEWCMD_KIN_LEGENDCUPKININFO_OPEN:
		{
			// 查看杯赛战队信息
			if (ptrParam == NULL)
			{
				return false;
			}

			cmd_kin_legendcup_kininfo * cmdData = (cmd_kin_legendcup_kininfo *)ptrParam;
			g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_LEGENDCUP_KIN_INFO, cmdData->nKinID, NULL, 0);
		}
		break;

	case GVIEWCMD_KIN_LEGENDCUPKINGLORY_OPEN:
		{
			// 查看杯赛战队荣誉
			if (ptrParam == NULL)
			{
				return false;
			}

			cmd_kin_legendcup_kinglory * cmdData = (cmd_kin_legendcup_kinglory *)ptrParam;
			g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_LEGENDCUP_KIN_GLORY, cmdData->nKinID, NULL, 0);
		}
		break;

    default:
        ErrorLn( __FUNCTION__": unhandled eventID=" << eventID );
        return false;
    }
        
    return true;
}

/// purpose: 请求战队改名
void KinClient::requestKinRename(char * szNewName)
{
    if(NULL == szNewName || 0 == strlen(szNewName))
    {
        return;
    }

    SMsKin_Rename_CO sendData;
    sstrcpyn(sendData.szNewName, szNewName, sizeof(sendData.szNewName));
    g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_RENAME, 0, &sendData, sizeof(sendData));
}

/// purpose: 回应邀请
void KinClient::answerInvite(DWORD dwPDBID, bool bIsAgree)
{
    SMsgKin_Answer_CO sendData;
    sendData.dwInviterPDBID = dwPDBID;
    sendData.bIsAgree = bIsAgree;
    g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_ANSWER_INVITE, 0, &sendData, sizeof(sendData));
}

// 客户端响应邀请
void KinClient::onClientInvite(LPCSTR pszMsg, int nLen)
{
    if (nLen != sizeof(SMsgKin_Invite_OC))
    {
        return;
    }
    SMsgKin_Invite_OC* pRecvData = (SMsgKin_Invite_OC*)pszMsg;

    // 通知客户端显示响应邀请
	cmd_recv_kin_invite inviteData;
	inviteData.dwInviterPDBID = pRecvData->dwInviterPDBID;
	sstrcpyn(inviteData.szInviteName, pRecvData->szInviteName, sizeof(inviteData.szInviteName));
	sstrcpyn(inviteData.szKinName, pRecvData->szKinName, sizeof(inviteData.szKinName));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_RECVKININVITE, 0, 0, &inviteData, sizeof(cmd_recv_kin_invite));

}

/// purpose: 查看杯赛战队信息
void KinClient::onViewLegendCupKinInfo(LPCSTR pszMsg, int nLen)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (NULL == pSchemeCenter)
	{
		ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
		return;
	}
	ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
	if (NULL == pMatchRankConfigInfo)
	{
		ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
		return;
	}

	if (nLen < sizeof(SMsgKin_KinBaseData_OC))
	{
		return;
	}

	SMsgKin_KinBaseData_OC* pRecvData = (SMsgKin_KinBaseData_OC*)pszMsg;
	if (nLen != sizeof(SMsgKin_KinBaseData_OC) + pRecvData->nMemberCount * sizeof(SClientKinMember))
	{
		return;
	}
	int offset = sizeof(SMsgKin_KinBaseData_OC);

	// 显示层战队数据更新数据组装
	obuf obData;
	int nPosNum = pRecvData->nMemberCount;
	cmd_kin_basedata headData;
    headData.nKinID = pRecvData->dwKinID;
	headData.nKinLevel = pRecvData->nKinLevel;              // 战队等级 
	headData.dwShaikhID = pRecvData->dwShaikhID;            // 族长
	headData.nMemberCount = pRecvData->nMemberCount;        // 成员数量
	headData.nOnlineCount = pRecvData->nOnlineCount;
	headData.nKinMaxMember = pRecvData->nKinMaxMember;
	headData.nTotalFight    = pRecvData->nTotalFight;                          // 战队总战力
	headData.nFightOrder    = pRecvData->nTotalFightOrder;                     // 战队战力排名
	sstrcpyn(headData.szKinName, pRecvData->szKinName, sizeof(headData.szKinName));  // 战队名
	sstrcpyn(headData.szShaikhName, pRecvData->szShaikhName, sizeof(headData.szShaikhName));  // 族长名
	sstrcpyn(headData.szNotice, pRecvData->szNotice, sizeof(headData.szNotice));  // 战队的每日公告
	sstrcpyn(headData.szKinLabel, pRecvData->szKinLabel, sizeof(headData.szKinLabel));
	headData.nMatchWinNum = pRecvData->nLegendWinCount;
	headData.nMatchTotalNum = pRecvData->nLegendTotaLCount;
	headData.nGoldNum = pRecvData->nGoldNum;
	headData.nSilverNum = pRecvData->nSilverNum;
	headData.nWeekActivity = pRecvData->nWeekActivity;

	obData.push_back(&headData, sizeof(cmd_kin_basedata));

	for (int i = 0; i < pRecvData->nMemberCount; ++i)
	{
		SClientKinMember* pListMember = (SClientKinMember*)(pszMsg + offset);

		//组装信息到显示层
		cmd_kin_membernode nodeData;
		nodeData.dwPDBID        = pListMember->dwPDBID;						// 玩家ID
		nodeData.nLevel         = pListMember->nLevel;						// 玩家等级
		nodeData.nIdentity      = pListMember->nIdentity;					// 族内职位
		nodeData.bIsOnline      = pListMember->bIsOnline;					// 玩家是否在线
		nodeData.nLogoutTime    = pListMember->nLogoutTime;					// 玩家最后下线时间
		nodeData.nVocation      = pListMember->nVocation;					// 玩家职业
		nodeData.nFaceID        = pListMember->nFaceID;                     // 头像ID
		nodeData.bIsPublicOnline= pListMember->bIsPublicOnline;				// 玩家是否在公共区在线
		nodeData.dwBattleVal    = pListMember->dwBattleVal;					// 战力值
		nodeData.dwSrcBattleVal = pListMember->dwSrcBattleVal;				// 上一次战力值
		nodeData.dwBVOrder      = pListMember->dwBVOrder;					// 战力值排行
		nodeData.dwLastOrder    = pListMember->dwLastOrder;					// 上一次战力值排行
		nodeData.nSex           = pListMember->bySex;						// 性别
        nodeData.byGameState    = pListMember->dwState;                     // 游戏状态
		nodeData.dwWorldID      = pListMember->dwWorldID;                   // 世界ID
		nodeData.dwSceneID      = pListMember->dwSceneID;                   // 场景ID
		nodeData.serverID       = pListMember->serverID;                    // 服务器ID
		// 获得段位信息
		const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(pListMember->nMatchType, pListMember->nGrade);
		if (pSchemeInfo != NULL)
		{
			sstrcpyn( nodeData.szGradeName,(char *)pSchemeInfo->szDes, sizeof(nodeData.szGradeName));
		}
		else
		{    
			sstrcpyn( nodeData.szGradeName,a2utf8("无"), sizeof(nodeData.szGradeName));
		}
		sstrcpyn(nodeData.szName, pListMember->szName, sizeof(nodeData.szName));  // 玩家名字
		obData.push_back(&nodeData, sizeof(nodeData));

		offset += sizeof(SClientKinMember);
	}

	// 由于第5个参数传不过去，  用第二个参数代替长度
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_LEGENDCUPKININFO_RECV, obData.size(), 0, obData.data(), obData.size());

}

/// purpose: 查看杯赛战队荣誉
void KinClient::onViewLegendCupKinGlory(LPCSTR pszMsg, int nLen)
{
	if (nLen < sizeof(SMsgKinGloryHead_OC))
	{
		return;
	}

	SMsgKinGloryHead_OC* pRecvData = (SMsgKinGloryHead_OC*)pszMsg;
	if (nLen != sizeof(SMsgKinGloryHead_OC) + pRecvData->nCount * sizeof(SMsgKinGloryNode_OC))
	{
		return;
	}

	// 显示层战队荣誉数据组装
	obuf obData;

	cmd_kin_glory_head gloryHead;
	sstrcpyn(gloryHead.szKinName, pRecvData->szKinName, sizeof(gloryHead.szKinName));
	gloryHead.nWinNum = pRecvData->nWinNum;
	gloryHead.nLoseNum = pRecvData->nLoseNum;
	gloryHead.nGodNum = pRecvData->nGodNum;
	gloryHead.n5KillNum = pRecvData->n5KillNum;
	gloryHead.n4KillNum = pRecvData->n4KillNum;
	gloryHead.n3KillNum = pRecvData->n3KillNum;
	gloryHead.nDeathNum = pRecvData->nDeathNum;
	gloryHead.nKillNum = pRecvData->nKillNum;
	gloryHead.nAssistNum = pRecvData->nAssistNum;
	gloryHead.nCount = pRecvData->nCount;
	obData.push_back(&gloryHead, sizeof(gloryHead));

	SMsgKinGloryNode_OC *pData  = (SMsgKinGloryNode_OC *)(pRecvData + 1);
	for (int i = 0; i < pRecvData->nCount; ++i)
	{
		cmd_kin_glory_node gloryNode;
		gloryNode.llLegendCupId = pData->llLegengCupID;
		gloryNode.nCupConfigID = pData->nCupConfigID;
		gloryNode.nEndTime = pData->dwCupEndTime;
		gloryNode.nCupRank = pData->byCupRank;
		sstrcpyn(gloryNode.szCupName, pData->szCupName , sizeof(gloryNode.szCupName));
		obData.push_back(&gloryNode, sizeof(gloryNode));

		pData++;
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_LEGENDCUPKINGLORY_RECV, 0, 0, obData.data(), obData.size());
}

void KinClient::onMessage(SNetMsgHead* head, void* data, size_t len)
{
    TraceLn(__FUNCTION__":"<<"head->byKeyAction ="<<head->byKeyAction<<" len= " << len);

    switch ( head->byKeyAction )
    {
    case MSG_KIN_MEMBER_LOGIN :
    case MSG_KIN_UPDATECLIENT :
    case MSG_KIN_UPATEKINPORP:
    case MSG_KIN_KINBASEDATA:
    case MSG_KIN_KINBASEDATA_UPDATE:
    case MSG_KIN_DISMISS:
    case MSG_KIN_KINMEMBERDATA:
    case MSG_KIN_REQUEST_KINEVENT_LIST:
	case MSG_KIN_REQUEST_KINGLORYDATA:
	case MSG_KIN_REQUEST_WARRECORD_TOTAL:
		{
			m_Kin.onMessage(head, data, len);
		}
        break;

    case MSG_KIN_UPATEMEMBER:
        {
            onUpdateMember(LPCSTR(data), len);
        }
        break;
    case MSG_KIN_LIST:
        {
            onMsgKinList(LPCSTR(data), len);
        }
        break;
    case MSG_KIN_QUIT:
        {
            onQuit(LPCSTR(data), len);
        }
        break;

    case MSG_KIN_CLIENTEVENT:
        {
            onClientEvent(LPCSTR(data), len);
        }
        break;

    case MSG_KIN_INVITE:
        {
            onClientInvite(LPCSTR(data), len);
        }
        break;
        
	case MSG_KIN_LEGENDCUP_KIN_INFO:
		{
			onViewLegendCupKinInfo(LPCSTR(data), len);
		}
		break;

	case MSG_KIN_LEGENDCUP_KIN_GLORY:
		{
			onViewLegendCupKinGlory(LPCSTR(data), len);
		}
		break;
	case MSG_KIN_UPDATE_LIST:
		{
			onMsgUpdateKinList(LPCSTR(data), len);
		}
		break;
    
    default:
        {
            ErrorLn(__FUNCTION__": unknown head->byKeyAction= " << head->byKeyAction);
        }
    }
}

/// purpose: 创建战队请求
void KinClient::createKinRequest(const char* szName, const char* szLabel)
{
    SMsgKin_Create_CO sendData;
    sstrcpyn(sendData.szName, szName, sizeof(sendData.szName));
	sstrcpyn(sendData.szLabel, szLabel, sizeof(sendData.szLabel));
    g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_CREATE, 0, &sendData, sizeof(sendData));
}

void KinClient::onUpdateMember(LPCSTR pszMsg, int nLen)
{
    if (nLen != sizeof(SKinMember))
    {
        return;
    }

    int nOldKinID = m_MemberInfo.dwKinID;
    m_bMemberVaild = true;
    memcpy(&m_MemberInfo, pszMsg, sizeof(SKinMember));

    // 发出主角数据改变事件
    // 显示层战队数据更新数据组装
    obuf obData;
    cmd_kin_self_hero_member headData;

    headData.dwPDBID = m_MemberInfo.dwPDBID;								// 玩家角色ID
    
    headData.dwKinID = m_MemberInfo.dwKinID;							    // 战队ID
    headData.nIdentity = m_MemberInfo.nIdentity;						    // 战队职位
    headData.nClanCtrb = m_MemberInfo.nClanCtrb;						    // 帮会贡献度
    headData.nTotalClanCtrb = m_MemberInfo.nTotalClanCtrb;					// 累计帮会贡献度

    headData.dwPopedomFlag = m_MemberInfo.dwPopedomFlag;					// 权限标志（一个位表示一种权限，一个整数表示玩家所拥有的权限）
    headData.bIsOnline = (int)m_MemberInfo.bIsOnline;						// 是否在线
    headData.nLogoutTime = m_MemberInfo.nLogoutTime;					    // 最近下线时间

    headData.nVocation = m_MemberInfo.nVocation;                            // 职业
    headData.nLevel = m_MemberInfo.nLevel;							        // 等级
    headData.nFaceID = m_MemberInfo.nFaceID;							    // 玩家头像

    headData.nLoginTime = m_MemberInfo.nLoginTime;						    // 成员上线时间（刚加入的成员，加入时候为他的上线时间）
    headData.dwBattleVal = m_MemberInfo.dwBattleVal;						// 战力值

    headData.dwSrcBattleVal = m_MemberInfo.dwSrcBattleVal;					// 旧战力值
    headData.dwBVOrder = m_MemberInfo.dwBVOrder;							// 战力值排名
    headData.dwLastOrder = m_MemberInfo.dwLastOrder;						// 上一次战力值排行
    
    sstrcpyn(headData.szName, m_MemberInfo.szName, sizeof(headData.szName));  // 角色名字


    obData.push_back(&headData, sizeof(cmd_kin_self_hero_member));

    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_UPDATESELF, obData.size(), 0, obData.data(), obData.size());

}


/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT IKinClient * CreateKinClient(void)
{
	// 如果创建了，则直接返回
	if(g_pKinClient != NULL)
	{
		return g_pKinClient;
	}

	// 实例化BUFF客户端
	KinClient * pKinClient = new KinClient();
	if(pKinClient == NULL || !pKinClient->Create())
	{
		safeRelease(pKinClient);
		return NULL;
	}

	g_pKinClient = pKinClient;
	return g_pKinClient;
}

/// purpose: 申请加入战队
void KinClient::joinRequest(DWORD dwKinID, string strLeaveWord)
{
    SMsgKin_Apply_CO sendData;
    sstrcpyn(sendData.szWord, strLeaveWord.c_str(), sizeof(sendData.szWord));
    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_APPLY, dwKinID, &sendData, sizeof(sendData));
}

/// purpose: 开除成员
void KinClient::kickRequest(DWORD dwPDBID)
{
    SMsgKin_Kick_CO sendData;
    sendData.dwMemberPDBID = dwPDBID;

    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_KICK, m_Kin.getNumProp(emKinProp_ID), &sendData, sizeof(sendData));
}

// purpose: 退出战队
void KinClient::onQuit(LPCSTR pszMsg, int nLen)
{

    m_bMemberVaild = false;
    memset(&m_MemberInfo, 0, sizeof(SKinMember));

    m_Kin.reset();

    // 发送退出战队命令
    obuf obData;
    cmd_kin_quit headData;
    headData.bQuit = true;
    obData.push_back(&headData, sizeof(cmd_kin_quit));
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_QUIT, obData.size(), 0, obData.data(), obData.size());
}

void KinClient::onMsgKinList(LPCSTR pszMsg, int nLen)
{
    ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return;
    }

    ISchemeKinDataCenter *pSchemeKinDataCenter = pSchemeCenter->getSchemeKinDataCenter();
    if (pSchemeKinDataCenter == NULL)
    {
        return;
    }

    if (nLen < sizeof(SMsgKinRequestKinList_OC))
    {
        ErrorLn(__FUNCTION__": data length is wrong!");
        return;
    }
    // 发到显示层显示
    SMsgKinRequestKinList_OC *pMsgHead = (SMsgKinRequestKinList_OC *)pszMsg;
    SMsgKinListNode_OC *pPosFirst = (SMsgKinListNode_OC *)(pMsgHead + 1);

    if (nLen != sizeof(SMsgKinRequestKinList_OC) + pMsgHead->nKinCount * sizeof(SMsgKinListNode_OC))
    {
        ErrorLn(__FUNCTION__": data length is wrong!");
        return;
    }

    obuf obData;
    cmd_kin_list_head headData;
    headData.nKinCount = pMsgHead->nKinCount;										// 战队个数
    
    int nPosNum = headData.nKinCount;
    obData.push_back(&headData, sizeof(cmd_kin_list_head));

    for (int i = 0; i < nPosNum; ++i)
    { 
        //组装信息到显示层
        cmd_kin_list_node nodeData;
        nodeData.dwID               = pPosFirst->dwID;										// 战队ID
        nodeData.nMemberCount       = pPosFirst->nMemberCount;								// 战队人数
        nodeData.nLevel             = pPosFirst->nLevel;									// 战队等级
        nodeData.nState             = pPosFirst->nState;									// 战队状态
        nodeData.nActivity          = pPosFirst->nActivity;									// 活跃度
		nodeData.nWeekActivity      = pPosFirst->nWeekActivity;                             // 周活跃度 
        nodeData.bIsApply           = pPosFirst->bIsApply;									// 是否已经申请
        nodeData.bAutoAccpet        = pPosFirst->bAutoAccpet;								// 是否自动收人
        nodeData.nLevelLimit        = pPosFirst->nLevelLimit;								// 招人等级限制
        nodeData.dwAcceptFlag       = pPosFirst->dwAcceptFlag;							    // 招人限制

        SKinLevel *pKinLevelInfo = pSchemeKinDataCenter->getLevelInfo(pPosFirst->nLevel);
        if (pKinLevelInfo != NULL)
        {
            // 计算剩余位置
            nodeData.dwLeftMemberCount  = pKinLevelInfo->nMaxMemberCount - pPosFirst->nMemberCount;
        }

        memcpy(nodeData.szName, pPosFirst->szName, sizeof(nodeData.szName));                // 战队名
        memcpy(nodeData.szShaikhName, pPosFirst->szShaikhName, sizeof(nodeData.szName));     // 战队族长名字   
        obData.push_back(&nodeData, sizeof(nodeData));
        ++pPosFirst;
    }
    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REQUESTKINLIST, obData.size(), 0, obData.data(), obData.size());


}

// purpose: 更新战队列表
void KinClient::onMsgUpdateKinList(LPCSTR pszMsg, int nLen)
{
	ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	ISchemeKinDataCenter *pSchemeKinDataCenter = pSchemeCenter->getSchemeKinDataCenter();
	if (pSchemeKinDataCenter == NULL)
	{
		return;
	}
	if (nLen != sizeof(SMsgKinListNode_OC))
	{
		ErrorLn(__FUNCTION__": data length is wrong!");
		return;
	}

	SMsgKinListNode_OC * pPosFirst = (SMsgKinListNode_OC *)pszMsg;
	obuf obData;
	cmd_kin_list_node nodeData;
	nodeData.dwID               = pPosFirst->dwID;										// 战队ID
	nodeData.nMemberCount       = pPosFirst->nMemberCount;								// 战队人数
	nodeData.nLevel             = pPosFirst->nLevel;									// 战队等级
	nodeData.nState             = pPosFirst->nState;									// 战队状态
	nodeData.nActivity          = pPosFirst->nActivity;									// 活跃度
	nodeData.bIsApply           = pPosFirst->bIsApply;									// 是否已经申请
	nodeData.bAutoAccpet        = pPosFirst->bAutoAccpet;								// 是否自动收人
	nodeData.nLevelLimit        = pPosFirst->nLevelLimit;								// 招人等级限制
	nodeData.dwAcceptFlag       = pPosFirst->dwAcceptFlag;							    // 招人限制
	nodeData.nWeekActivity      = pPosFirst->nWeekActivity;
	SKinLevel *pKinLevelInfo = pSchemeKinDataCenter->getLevelInfo(pPosFirst->nLevel);
	if (pKinLevelInfo != NULL)
	{
		// 计算剩余位置
		nodeData.dwLeftMemberCount  = pKinLevelInfo->nMaxMemberCount - pPosFirst->nMemberCount;
	}
	memcpy(nodeData.szName, pPosFirst->szName, sizeof(nodeData.szName));                // 战队名
	memcpy(nodeData.szShaikhName, pPosFirst->szShaikhName, sizeof(nodeData.szName));     // 战队族长名字   
	obData.push_back(&nodeData, sizeof(nodeData));

	// 由于第5个参数传不过去，  用第二个参数代替长度
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_UPDATEKINLIST, obData.size(), 0, obData.data(), obData.size());
}

void KinClient::dismissRequest()
{
    // 解散战队
    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_DISMISS, m_Kin.getNumProp(emKinProp_ID), "", 0);
}

void KinClient::onClientEvent(LPCSTR pszMsg, int nLen)
{
    if (nLen < sizeof(SMsgKin_EventHead_OC))
    {
        return;
    }

    if (m_Kin.IsVaild() != false)
    {
        m_Kin.dealClientEvent(pszMsg, nLen);
    }

    SMsgKin_EventHead_OC* pRecvData = (SMsgKin_EventHead_OC*)pszMsg;
    if (nLen > sizeof(SMsgKin_EventHead_OC))
    {
        gClientGlobal->getEventEngine()->FireExecute(pRecvData->wEventID, pRecvData->bSrcType, pRecvData->dwSrcID, (pszMsg + sizeof(SMsgKin_EventHead_OC)), nLen - sizeof(SMsgKin_EventHead_OC));
    }
    else
    {
        gClientGlobal->getEventEngine()->FireExecute(pRecvData->wEventID, pRecvData->bSrcType, pRecvData->dwSrcID, "", 0);
    }
}

/// purpose: 进入观察
void KinClient::enterObserveRequest()
{
    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_ENTERTOBSERVE, m_Kin.getNumProp(emKinProp_ID),"", 0);
}

/// purpose: 退出观察（关闭窗口）
void KinClient::exitObserveRequest()
{
    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_EXITOBSERVE, m_Kin.getNumProp(emKinProp_ID),"", 0);
}

void KinClient::quitRequest()
{
    g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_QUIT, m_Kin.getNumProp(emKinProp_ID), "", 0);
}

/// purpose: 设置职位
void KinClient::appointRequest(DWORD dwPDBID, int nIdentity)
{	
    SMsgKin_Appoint_CO sendData;
    sendData.dwMemberPDBID = dwPDBID;
    sendData.nIdentity = nIdentity;

    g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_APPOINT, m_Kin.getNumProp(emKinProp_ID), &sendData, sizeof(sendData));
}

void KinClient::dragonBallCreateKin(const char* szName)
{
    if (szName == NULL)
    {
        ErrorLn(__FUNCTION__": szName == NULL")
        return;
    }
    createKinRequest(szName, "");
}

void KinClient::dragonBallJoinKin(DWORD dwKinID, string strLeaveWord)
{
    if (dwKinID == 0)
    {
        ErrorLn(__FUNCTION__": dwKinID == 0")
        return;
    }
    joinRequest(dwKinID, strLeaveWord);
}

void KinClient::dragonBallQuitKin()
{
    quitRequest();
}

void KinClient::dragonBallDissmissKin()
{
    dismissRequest();
}

/// purpose: 修改公告
void KinClient::MotifyRequest(int nPropID, char* szText)
{
    SMsgKin_Motify sendData;
    sendData.nPropID = nPropID;
    sstrcpyn(sendData.szText, szText, KIN_MANIFESTO_LEN);

    g_EHelper.SendKinSocial(emMsgKin_Kin,MSG_KIN_MOTIFY, m_Kin.getNumProp(emKinProp_ID), &sendData, sizeof(sendData));
}