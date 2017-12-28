#include "stdafx.h"
#include "LegendCupClient.h"
#include "IClientEntity.h"
#include "GameViewDef.h"
#include "ISchemeCenter.h"
#include "IChatClient.h"
#include "MatchSchemeDef.h"

LegendCupClient* g_pLegendCupClient = NULL;
void LegendCupClient::Release(void)
{
    gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_LEGENDCUP, this);

	// 显示层事件
	IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
	if(pEntityClient)
	{
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_REQUEST_CUP_LIST );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_REQUEST_CUP_CREATE );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_REQUEST_REGIST_MEMBERLIST );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_ADD_REGIST_KINLIST );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_QUIT_REGIST_KINLIST );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_ADD_REGIST_BLACKLIST );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_REQUEST_REGIST_BLACKLIST );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_DELETE_REGIST_BLACKLIST );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_RELEASE_SINGLE_CUP );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_VIEW_COMPETITION_NODE );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ );
		pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_QUITOBSERVE );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LEGENDCUP_OFFLINE_PRE_END_REGISTER );
        
		
	}

	// 全局指针NULL
	g_pLegendCupClient = NULL;

	delete this;
}


bool LegendCupClient::Create(void)
{
    IMessageDispatch* pMessageDispatch = gClientGlobal->getMessageDispatch();
    if (pMessageDispatch == NULL)
    {
        return false;
    }

    pMessageDispatch->registerMessageHandler(MSG_MODULEID_LEGENDCUP, this);

	// 显示层事件
	IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
	if(pEntityClient)
	{
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_REQUEST_CUP_LIST,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_REQUEST_CUP_CREATE,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_REQUEST_REGIST_MEMBERLIST,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_ADD_REGIST_KINLIST,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_QUIT_REGIST_KINLIST,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_ADD_REGIST_BLACKLIST,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_REQUEST_REGIST_BLACKLIST,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_DELETE_REGIST_BLACKLIST,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_RELEASE_SINGLE_CUP,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_VIEW_COMPETITION_NODE,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ,this );
		pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_QUITOBSERVE, this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_LEGENDCUP_OFFLINE_PRE_END_REGISTER, this );
        
	}

	return true;
}

bool LegendCupClient::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{
    switch ( eventID )
    {
	case GVIEWCMD_LEGENDCUP_REQUEST_CUP_LIST:  // 杯赛列表
		{
			requestCupList();
		}
		break;
	case GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP:  // 某杯赛战队信息
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_view_single_cup *cmdData = (cmd_legendcup_view_single_cup *)ptrParam;
			SMsgRequestLookOverCup pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			reqlookOverLegendCup(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_REQUEST_CUP_CREATE:  // 创建杯赛
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_req_cup_create *cmdData = (cmd_legendcup_req_cup_create *)ptrParam;
			SMsgCreateCup pInfo;
			pInfo.nCreateConfigID = cmdData->byCupConfigID;
			sstrcpyn(pInfo.szCupName, cmdData->szLegendCupName, sizeof(pInfo.szCupName));
			pInfo.byCreateCupType = cmdData->byCreateCupType;
			pInfo.bIsOnlineCup = cmdData->byCupIsOnine;
			pInfo.dwCreataActorID = cmdData->nCreaterPDBID;
			sstrcpyn(pInfo.szPassword, cmdData->szPassword, sizeof(pInfo.szPassword));
			pInfo.bClanLimit = cmdData->byClanLimit == 0?  false: true;
			pInfo.nLevelLimit = cmdData->nLevelLimit;
			pInfo.nDayInterval = cmdData->nDayInterval;
			pInfo.nContriBonus = cmdData->nContriBonus;
            pInfo.nRegisterNeedTicket = cmdData->nRegistGold;
			pInfo.dwClanID = cmdData->nClanID;
			pInfo.byTicketAllocateMode = cmdData->byTicketAllocateMode;
			reqCreateCup(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_REQUEST_REGIST_MEMBERLIST:  // 某杯赛战队成员信息
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_req_regist_memberlist *cmdData = (cmd_legendcup_req_regist_memberlist *)ptrParam;
			SMsgRequestKinMember pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.dwKinID = cmdData->nKinID;
			reqKinMemberList(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_ADD_REGIST_KINLIST:  // 请求加入某杯赛
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_add_regist_kinlist *cmdData = (cmd_legendcup_add_regist_kinlist *)ptrParam;
			SMsgJoinCup pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.dwKinID = cmdData->nKinID;
			pInfo.dwActorID = cmdData->nActorID;
			pInfo.byCreateCupType = cmdData->byCreateCupType;
			sstrcpyn(pInfo.szPassword, cmdData->szPassword, sizeof(pInfo.szPassword));
			reqJoinCup(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_QUIT_REGIST_KINLIST:  // 请求退出某杯赛
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_quit_regist_kinlist *cmdData = (cmd_legendcup_quit_regist_kinlist *)ptrParam;
			SMsgExitCup pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.dwKinID = cmdData->nKinID;
			pInfo.dwActorID = cmdData->nActorID;
			pInfo.byCreateCupType = cmdData->byCreateCupType;
			reqExitCup(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_ADD_REGIST_BLACKLIST:  // 加入黑名单
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_add_regist_blacklist *cmdData = (cmd_legendcup_add_regist_blacklist *)ptrParam;
			SMsgAddToBlackList pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.dwKinID = cmdData->nKinID;
			pInfo.dwOperateID = cmdData->nOperatorID;
			pInfo.byCreateCupType = cmdData->byCreateCupType;
			reqAddToBlacklist(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_REQUEST_REGIST_BLACKLIST:  // 查看黑名单
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_add_regist_blacklist *cmdData = (cmd_legendcup_add_regist_blacklist *)ptrParam;
			SMsgRequestAllBlackKinList pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			reqAllBalcklist(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_DELETE_REGIST_BLACKLIST:  //  删除黑名单
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_delete_regist_blacklist *cmdData = (cmd_legendcup_delete_regist_blacklist *)ptrParam;
			SMsgRemoveBlackList pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.dwKinID = cmdData->nKinID;
			pInfo.dwOperateID = cmdData->nOperatorID;
			pInfo.byCreateCupType = cmdData->byCreateCupType;
			reqRemoveBlacklist(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_RELEASE_SINGLE_CUP:  //  发布比赛
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_release_single_cup *cmdData = (cmd_legendcup_release_single_cup *)ptrParam;
			SMsgPublicCompetition pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.dwOperateID = cmdData->nOperatorID;
			pInfo.byCreateCupType = cmdData->byCreateCupType;
			reqPublickCompetition(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_VIEW_COMPETITION_NODE:  //  查看节点比赛
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_view_competition_node *cmdData = (cmd_legendcup_view_competition_node *)ptrParam;
			SMsgLookUpCupNodeWarInfo pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.nRoundID = cmdData->nRoundID;
			pInfo.nSerchID = cmdData->nSearchID;
			reqLookUpCupNodeWarInfo(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE:  //  加入节点比赛
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_enter_competition_node *cmdData = (cmd_legendcup_enter_competition_node *)ptrParam;
			SMsgJoinLegendCupRoom pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.nRoundID = cmdData->nRoundID;
			pInfo.nSerchID = cmdData->nSearchID;
			reqJoinLegendCupRoom(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE:  //  裁决比赛
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_judge_competition_node *cmdData = (cmd_legendcup_judge_competition_node *)ptrParam;
			SMsgJudgeWarResult pInfo;
			pInfo.llLegendCupID = cmdData->nLegendCupID;
			pInfo.nRoundID = cmdData->nRoundID;
			pInfo.nSerchID = cmdData->nSearchID;
			pInfo.nBoCount = cmdData->nBoCount;
			pInfo.llWarID = cmdData->llWarID;
			pInfo.byJudgeFlag = cmdData->byJudgeFlag;
			pInfo.dwWinnerKinID = cmdData->dwWinnerKinID;
            pInfo.dwResetNewTime = cmdData->lResetNewTime;                       // 重置节点新的开场时间
            pInfo.dwKin1= cmdData->nKin1;                                         // 队伍1
            pInfo.nKin1Score = cmdData->nKin1Score;									// 队伍1得分
            pInfo.dwKin2= cmdData->nKin2;                                         // 队伍2
            pInfo.nKin2Score = cmdData->nKin2Score;									// 队伍2得分
			reqJudgeSingalWarResult(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ:	// 请求创建类型
		{
			if(ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_req_create_cup_type *cmdData = (cmd_legendcup_req_create_cup_type *)ptrParam;
			SMsgCreateCupType pInfo;
			pInfo.byCreateType = ELegendCupType(cmdData->byLegendCupType);
			pInfo.dwActorID = cmdData->nActorID;
			reqCreateType(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_QUITOBSERVE:	// 退出杯赛观察
		{
			if (ptrParam == NULL)
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return false;
			}

			cmd_legendcup_quit_observe * data = (cmd_legendcup_quit_observe *) ptrParam;
			SMsgReqQuitObserve pInfo;
			pInfo.llLegendCupID = data->nLegendCupID;
			reqQuitObserve(&pInfo);
		}
		break;

    case GVIEWCMD_LEGENDCUP_OFFLINE_PRE_END_REGISTER:	// 预先结束报名
        {
            if (ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }

            cmd_legendcup_offline_pre_end_register * data = (cmd_legendcup_offline_pre_end_register *) ptrParam;
            SMsgOfflineEndRegister sMsgInfo;
            sMsgInfo.llLegendCupID = data->nLegendCupID;
            reqOfflinePreEndRegister(&sMsgInfo);
        }
        break;
    default:
        ErrorLn( __FUNCTION__": unhandled eventID=" << eventID );
        return false;
    }
        
    return true;
}

void LegendCupClient::onMessage(SNetMsgHead* head, void* data, size_t len)
{
    TraceLn(__FUNCTION__": len= " << len);

    switch ( head->byKeyAction )
    {
    case OC_CUP_SEND_ALL_BALCKLIST:              // 发送所有黑名单列表到客户端
        {
            onMsgSendAllBalcklist(head, data, len);
        }
        break;

    case OC_CUP_SEND_KIN_MEMBER:              // 请求战队玩家成员
        {
            onMsgSendKinMember(head, data, len);

        }
        break;

    case OC_CUP_SEND_ALL_LEGENDCUP:              // 全部杯赛信息
        {
            onMsgSendAllLegendcup(head, data, len);

        }
        break;

    case OC_CUP_SEND_ALL_KINLIST:              // 战队列表
        {
            onMsgSendAllKinlist(head, data, len);

        }
        break;

    case OC_CUP_SEND_ALL_KIN_MEMBERINFO:          // 比赛所有成员信息
        {
            onMsgSendAllKinMemberinfo(head, data, len);
        }
		break;

    case OC_CUP_SEND_ALL_ROUNDTIME:             // 比赛所有轮时间
        {
            onMsgSendAllRoundTimeinfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_ALL_ROUNDINFO:              // 请求所有比赛节点信息
        {
            onMsgSendAllRoundInfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_NODE_LOOKUP_WARINFO:              // 请求杯赛节点战场信息
        {
            onMsgSendNodeLookupWarInfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_UPDATE_SINGEL_CUP_INFO:              // 更新单个比赛信息
        {
            onMsgSendUpdateSingelCupInfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_UPDATE_SINGEL_CUP_NODE_INFO:              // 更新节点
        {
            onMsgSendUpdateSingelCupNodeInfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_REGISTER_SUCCESS:                      // 申请加入比赛成功
        {
            onMsgSendRegisterSuccess(head, data, len);
        }
        break;

    case SC_CUP_CREATE_CUP_TYPE:                      // 响应创建杯赛类型
        {
            onMsgGetCreateCupTypeInfo(head, data, len);
        }
        break;
    case OC_CUP_SEND_PUBLIC_SUCCESS:                      // 申请加入比赛成功
        {
            onMsgPublicSuccess(head, data, len);
        }
        break;
	case OC_CUP_SEND_SYSTEM_CANCEL_CUP:                      // 申请加入比赛成功
		{
			onMsgSystemCancelCup(head, data, len);
		}
		break;

	case OC_CUP_SEND_CREATER_PUBLIC_SUCCESS:			// 创建比赛发布成功
		{
			onMsgCreaterPublicSuccess(head, data, len);
		}
		break;

    default:
        {
            ErrorLn(__FUNCTION__": unknown head->byKeyAction= " << head->byKeyAction);
        }
    }
}

void LegendCupClient::onMsgSendAllBalcklist(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgBalckListHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsgBalckListHead* msg = (SMsgBalckListHead*)data;
    if ( len != sizeof(SMsgBalckListHead) + msg->nCount * sizeof(SMsgBlackKinInfo))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SMsgBalckListHead) + msg->nCount * sizeof(SMsgBlackKinInfo));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
	cmd_legendcup_recv_regist_balcklisthead headData;
	headData.nLegendCupID = msg->llLegendCupID;
	headData.nCurBlacklistNum = msg->nCount;
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_recv_regist_balcklisthead));
    
    SMsgBlackKinInfo *pData = (SMsgBlackKinInfo *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        cmd_legendcup_recv_regist_balcklistnode nodeData;
		nodeData.nKinID = pData->nKinProp_ID;										// 战队ID
        nodeData.nKinWinNum = pData->nKinWinNum;								// 战队杯赛总胜场
        nodeData.nKinMaxNum = pData->nKinMaxNum;								// 战队杯赛总场数
		sstrcpyn(nodeData.szKinName, pData->szName, sizeof(nodeData.szKinName));	// 战队名称
		obViewdata.push_back(&nodeData, sizeof(cmd_legendcup_recv_regist_balcklistnode));
			
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_RECV_REGIST_BLACKLIST, 0, 0, obViewdata.data(), obViewdata.size());
}

void LegendCupClient::onMsgSendKinMember(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(DWORD) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

	DWORD* idMsg = (DWORD*)data;
	DWORD* numMsg = idMsg + 1;
	DWORD dwKinID = (*idMsg);
	DWORD dwCount = (*numMsg);
    if ( len != 2 * sizeof(DWORD) + (dwCount) * sizeof(SLegendCupMemberInfo))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<2 * sizeof(DWORD) + (dwCount) * sizeof(SLegendCupMemberInfo));
        return;
    }

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

    // 客户端显示层数据
    obuf obViewdata;
	cmd_legendcup_recv_regist_memberlisthead headData;
	headData.nKinID = dwKinID;
	headData.nMemberCount = dwCount;
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_recv_regist_memberlisthead));

    SLegendCupMemberInfo *pData = (SLegendCupMemberInfo *)(numMsg+1);
    for (DWORD i = 0; i< dwCount;  i++)
    {
        cmd_legendcup_recv_regist_memberlistnode nodeData;
        nodeData.uDBID  = pData->dwUserID;
		nodeData.nPDBID = pData->dwActorID;												// 玩家PDBID
		nodeData.nActorLevel = pData->nLevel;											// 玩家等级
		nodeData.nActorSex = pData->bSex ? 1 : 0;										// 玩家性别(0男1女)
		sstrcpyn(nodeData.szActorName, pData->szActorName, sizeof(nodeData.szActorName));// 玩家名字

		nodeData.nType = pData->nMatchType;												// 天梯类型
		nodeData.nRankGrade = pData->nGrade;											// 天梯段位
		nodeData.nRankScore = pData->nRankScore;										// 天梯分数
		// 获得段位信息
		const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(nodeData.nType, nodeData.nRankGrade);
		if (pSchemeInfo != NULL)
		{
			nodeData.nRankIconId = pSchemeInfo->nIconID;								// 段位图片ID
			sstrcpyn( nodeData.szGradeName,(char *)pSchemeInfo->szDes, sizeof(nodeData.szGradeName));// 段位描述
		}
		obViewdata.push_back(&nodeData, sizeof(cmd_legendcup_recv_regist_memberlistnode));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_RECV_REGIST_MEMBERLIST, 0, 0, obViewdata.data(), obViewdata.size());
}

void LegendCupClient::onMsgSendAllLegendcup(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgLegendCupListHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsgLegendCupListHead* msg = (SMsgLegendCupListHead*)data;
    if ( len != sizeof(SMsgLegendCupListHead) + msg->nCount * sizeof(SMsgLegendCupInfo))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SMsgLegendCupListHead) + msg->nCount * sizeof(SMsgLegendCupInfo));
        return;
    }

	DWORD dwSelfID = gClientGlobal->getHero()->getIntProperty(PROPERTY_ID);

    // 客户端显示层数据
    obuf obViewdata;

	cmd_legendcup_recv_cuplist_head headData;
	headData.nCount = msg->nCount;
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_recv_cuplist_head));

    SMsgLegendCupInfo *pData = (SMsgLegendCupInfo *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // 逻辑层数据 按排名顺序作为键值存储
		cmd_legendcup_recv_cuplist_node nodeData;
		nodeData.nLegendCupID = pData->llLegendCupID;								// 杯赛ID
		nodeData.nCreaterPDBID = pData->dwCreataActorID;							// 创建者ID
		nodeData.nIsSelfIn = pData->bIsJoinedCup;									// 是否自己参赛
		nodeData.nIsLock = pData->bHavePassword;									// 是否加密
		nodeData.nIsClanLimit = pData->bClanLimit;									// 是否有同联盟限制
		nodeData.nIsOnline = pData->bIsOnlineCup;									// 是否线上比赛
		nodeData.nIsEnd = pData->bIsEnd;											// 是否结束
		nodeData.nIsSystemCreate = (pData->dwCreataActorID <= 0);					// 是否系统创建
		nodeData.nCupConfigID = pData->nCreateConfigID;								// 杯赛配置ID
		nodeData.nRegistLevelLimit = pData->nLevelLimit;							// 报名等级限制
		nodeData.nRegistLeftTime = pData->dwRegisterLeftTime;						// 报名截止时间
		nodeData.nMaxKinNum = pData->byMaxKinNumber;								// 总队伍数量
		nodeData.nCurrentKinNum = pData->nCurrentKinNum;							// 当前队伍数量
		nodeData.byCreateCupType = pData->byCreateCupType;							// 创建类型（个人，系统）
		
		nodeData.nNeedRoundCount = pData->nNeedRoundCount;							// 杯赛需要打几轮
		nodeData.nBeginRound = pData->nBeginRound;									// 本场比赛由第几轮开始
		nodeData.nCurrentRound = pData->nCurrentRound;								// 当前打到第几轮
        nodeData.nCompetitionBonus = pData->dwTotalCompetitionBonus;                // 比赛总奖金
        nodeData.nRegistGold = pData->dwRegistGold;                                 // 报名需要的金币
		nodeData.nIsSelfCreate = pData->dwCreataActorID == dwSelfID;				// 是否自身创建
		nodeData.nClanID = pData->dwClanID;
        
        sstrcpyn(nodeData.szCupName, pData->szCupName, sizeof(nodeData.szCupName));						// 杯赛名称
        sstrcpyn(nodeData.szCreateActorName, pData->szCreaterName, sizeof(nodeData.szCreateActorName));	// 发起人名称

		obViewdata.push_back(&nodeData, sizeof(cmd_legendcup_recv_cuplist_node));
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_RECV_CUP_LIST, 0, 0, obViewdata.data(), obViewdata.size());
}

void LegendCupClient::onMsgSendAllKinlist(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgRegisterHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsgRegisterHead* msg = (SMsgRegisterHead*)data;
    if ( len != sizeof(SMsgRegisterHead) + msg->nCount * sizeof(SMsgRegisterKinInfo))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SMsgRegisterHead) + msg->nCount * sizeof(SMsgRegisterKinInfo));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
	cmd_legendcup_recv_regist_kinlisthead headData;
	headData.nLegendCupID = msg->llLegendCupID;								// 杯赛ID
	headData.nRegistLeftTime = msg->dwRegisterLeftTime;						// 报名剩余时间
	headData.nMaxKinNum = msg->nMaxCount;									// 总队伍数量
	headData.nCurKinCount = msg->nCount;									// 当前战队数
	headData.nConfirmKinLeftTime = msg->dwConfirmKinLeftTime;				// 比赛名单确认剩余时间
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_recv_regist_kinlisthead));

    SMsgRegisterKinInfo *pData = (SMsgRegisterKinInfo *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        cmd_legendcup_recv_regist_kinlistnode nodeData;
		nodeData.nLegendCupID = msg->llLegendCupID;											// 杯赛ID
		nodeData.nKinID = pData->nKinProp_ID;												// 战队ID
		nodeData.nKinLevel = pData->nKinProp_Level;											// 战队等级
		nodeData.nKinWinNum = pData->nKinWinNum;											// 战队杯赛总胜场
		nodeData.nKinMaxNum = pData->nKinMaxNum;											// 战队杯赛总场数
		sstrcpyn(nodeData.szKinName, pData->szName, sizeof(nodeData.szKinName));			// 战队名称
		obViewdata.push_back(&nodeData, sizeof(cmd_legendcup_recv_regist_kinlistnode));		
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_RECV_REGIST_KINLIST, 0, 0, obViewdata.data(), obViewdata.size());
    
}

void LegendCupClient::onMsgSendAllKinMemberinfo(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgAllMemebrHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsgAllMemebrHead* msg = (SMsgAllMemebrHead*)data;
    if ( len != sizeof(SMsgAllMemebrHead) + msg->nMemberCount * sizeof(SLegendCupMemberInfo))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SLegendCupMemberInfo) + msg->nMemberCount * sizeof(SLegendCupMemberInfo));
        return;
    }

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

    // 客户端显示层数据
    obuf obViewdata;
	cmd_legendcup_competition_kinmemberhead headData;
	headData.nLegendCupID = msg->llLegendCupID;
	headData.nMemberCount = msg->nMemberCount;
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_competition_kinmemberhead));

    SLegendCupMemberInfo *pData = (SLegendCupMemberInfo *)(msg+1);
    for (int i = 0; i< msg->nMemberCount;  i++)
    {
		cmd_legendcup_competition_kinmembernode memberInfo;
		memberInfo.nKinID = pData->dwKinID;
		memberInfo.nUserID = pData->dwUserID;
		memberInfo.nPDBID = pData->dwActorID;
		memberInfo.nActorSex = pData->bSex ? 1 : 0;
		memberInfo.nActorLevel = pData->nLevel;
		sstrcpyn(memberInfo.szActorName, pData->szActorName, sizeof(memberInfo.szActorName));

		memberInfo.nType = pData->nMatchType;												// 天梯类型
		memberInfo.nRankGrade = pData->nGrade;											// 天梯段位
		memberInfo.nRankScore = pData->nRankScore;										// 天梯分数
		// 获得段位信息
		const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(memberInfo.nType, memberInfo.nRankGrade);
		if (pSchemeInfo != NULL)
		{
			memberInfo.nRankIconId = pSchemeInfo->nIconID;								// 段位图片ID
			sstrcpyn( memberInfo.szGradeName,(char *)pSchemeInfo->szDes, sizeof(memberInfo.szGradeName));// 段位描述
		}
		obViewdata.push_back(&memberInfo, sizeof(cmd_legendcup_competition_kinmembernode));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_RECV_COMPETITION_KIN, 0, 0, obViewdata.data(), obViewdata.size());
}

void LegendCupClient::onMsgSendAllRoundTimeinfo(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgAllTimeHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsgAllTimeHead* msg = (SMsgAllTimeHead*)data;
    if ( len != sizeof(SMsgAllTimeHead) + msg->nCount * sizeof(SRoundTimeInfo))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SRoundTimeInfo) + msg->nCount * sizeof(SRoundTimeInfo));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
    cmd_legendcup_competition_kinmemberhead headData;
    headData.nLegendCupID = msg->llLegendCupID;
    headData.nMemberCount = msg->nCount;
    obViewdata.push_back(&headData, sizeof(cmd_legendcup_competition_kinmemberhead));

    SRoundTimeInfo *pData = (SRoundTimeInfo *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
		//轮次信息显示层数据赋值
		cmd_legendcup_recv_competition_roundinfo roundInfo;
		roundInfo.nRoundID = pData->nRoundID;
		roundInfo.nRoundCreateRoomTme = pData->dwRoundCreateRoomTime;
		roundInfo.nBeginInterval = pData->dwRoomBeginInterval;
		roundInfo.nRoundEndTime = pData->dwRoundEndTime;
		obViewdata.push_back(&roundInfo, sizeof(cmd_legendcup_recv_competition_roundinfo));

        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_ROUNDTIME_RECV, 0, 0, obViewdata.data(), obViewdata.size());
}



void LegendCupClient::onMsgSendAllRoundInfo(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgAllRoundHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsgAllRoundHead* msg = (SMsgAllRoundHead*)data;
    if ( len != sizeof(SMsgAllRoundHead) + msg->nRoundCount * sizeof(SMsgCompetitionRoundHead) + msg->nAllNodeCount *sizeof(SMsgCompetitionNodeInfo) )
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SMsgAllRoundHead) + msg->nRoundCount * sizeof(SMsgCompetitionRoundHead) + msg->nAllNodeCount *sizeof(SMsgCompetitionNodeInfo));
        return;
    }

    // 比赛所有的轮信息
    obuf obViewdata;

	cmd_legendcup_recv_competition_head headInfo;	// 头信息
	obuf obRoundData;								// 轮次时间信息
	obuf obNodeData;								// 节点信息

    // pFirstData指针指向每轮的数据区域
    char *pFirstData = (char *)(msg+1);
    for (int n = 0 ; n < msg->nRoundCount; ++n)
    {
        SMsgCompetitionRoundHead * pData = (SMsgCompetitionRoundHead *)pFirstData;

		// 头信息显示层数据赋值
		if (pData->nRoundID == msg->nRoundCount)				//当前轮次信息赋值
		{
			headInfo.nLegendCupID = msg->llLegendCupID;										// 杯赛ID
			headInfo.nCompetitionNodeCount = msg->nAllNodeCount;							// 杯赛节点数量
			headInfo.nRoundID = pData->nRoundID;											// 当前轮数
			headInfo.nIsFinalRound = pData->bIsFinal ? 1 : 0;								// 是否决赛轮
			headInfo.nGroupCount = pData->nMaxGoup;											// 分组数量 
			headInfo.nGroupMaxKin = pData->nGroupMaxKin;									// 每组最大战队数量
			headInfo.byCompetitionType = pData->eCompetitionType;							// ERoundCompetitionType

			//int nIndex = 0;
			//for (int j = 0; j < headInfo.nGroupMaxKin; j++)
			//{
			//	for (int i = 0; i < headInfo.nGroupCount; i++)
			//	{
			//		int nPos = j * headInfo.nGroupCount + i;
			//		headInfo.kinInfos[nIndex].nKinID = pData->KinIDList[nPos].dwKinID;
			//		sstrcpyn(headInfo.kinInfos[nIndex].szKinName, pData->KinIDList[nPos].szKinName, sizeof(headInfo.kinInfos[nIndex].szKinName));
			//		nIndex++;
			//	}
			//}

			for (int j = 0; j< headInfo.nGroupMaxKin * headInfo.nGroupCount;  j++ )			// 当前轮战队信息
			{
				headInfo.kinInfos[j].nKinID = pData->KinIDList[j].dwKinID;
				sstrcpyn(headInfo.kinInfos[j].szKinName, pData->KinIDList[j].szKinName, sizeof(headInfo.kinInfos[j].szKinName));
			}

			//for (int j = 0; j< headInfo.nGroupMaxKin * headInfo.nGroupCount;  j++ )			// 当前轮战队信息
			//{
			//	int nRow = j % headInfo.nGroupCount;
			//	int nCol = j / headInfo.nGroupCount;
			//	int nPos = nRow * headInfo.nGroupMaxKin + nCol;

			//	headInfo.kinInfos[nPos].nKinID = pData->KinIDList[j].dwKinID;
			//	sstrcpyn(headInfo.kinInfos[nPos].szKinName, pData->KinIDList[j].szKinName, sizeof(headInfo.kinInfos[nPos].szKinName));
			//}
		}

		//轮次信息显示层数据赋值
		cmd_legendcup_recv_competition_roundinfo roundInfo;
		roundInfo.nRoundID = pData->nRoundID;
		//roundInfo.nRoundBeginTime = pData->dwRoundBeginTime;
		obRoundData.push_back(&roundInfo, sizeof(cmd_legendcup_recv_competition_roundinfo));

		//节点信息显示层数据赋值
        SMsgCompetitionNodeInfo *pNodeData = (SMsgCompetitionNodeInfo *)(pData + 1);
        for (int i = 0; i< pData->nCount;  i++)
        {
			cmd_legendcup_recv_competition_node nodeInfo;
			nodeInfo.byCompetitionType = pNodeData->byCompetitionType;							// 1小组赛 2淘汰赛
			nodeInfo.nSearchID = pNodeData->nSerchID;											// 生成的快速定位ID
			nodeInfo.nRoundID = pNodeData->nRoundID;											// 第几轮比赛
			nodeInfo.nGroupID = pNodeData->nGroupID;											// 分配到第几组
			nodeInfo.nDay = pNodeData->nDay;													// 第几天
			nodeInfo.nPos = pNodeData->nPos;													// 位置索引 从1开始
			nodeInfo.nKin1ID = pNodeData->dwKin1;												// 比赛战队1
			nodeInfo.nKin1Score = pNodeData->nKin1Score;										//
			sstrcpyn(nodeInfo.szKin1Name, pNodeData->szKin1Name, sizeof(nodeInfo.szKin1Name));	//	
			nodeInfo.nKin2ID = pNodeData->dwKin2;												// 比赛战队2
			nodeInfo.nKin2Score = pNodeData->nKin2Score;										//
			sstrcpyn(nodeInfo.szKin2Name, pNodeData->szKin2Name, sizeof(nodeInfo.szKin2Name));	//
			nodeInfo.byCompetitionNodeState = pNodeData->eNodeState;							// 节点状态ECompetitionNodeState
			nodeInfo.nWinnerKinID = pNodeData->dwWinnerKinID;									// 胜利战队ID
			nodeInfo.byBoType = pNodeData->byBoType;											// BO类型 ELegendCupBO
			nodeInfo.nCurrentBo = pNodeData->nCurrentBo;										// 已经打了多少场
			nodeInfo.nBeginTime = pNodeData->dwBeginTime;										// 节点比赛开始时间
			nodeInfo.nEnterTime = pNodeData->dwEnterTime;										// 节点进场开始时间
			nodeInfo.nServerTime = pNodeData->dwServerTime;										// 服务器当前时间
			obNodeData.push_back(&nodeInfo, sizeof(cmd_legendcup_recv_competition_node));

            pNodeData++;
        }
        int nOffset =  sizeof(SMsgCompetitionRoundHead) + pData->nCount * sizeof(SMsgCompetitionNodeInfo);
        pFirstData += nOffset;  
        
    }

	obViewdata.push_back(&headInfo, sizeof(cmd_legendcup_recv_competition_head));
	obViewdata.push_back(obRoundData.data(),obRoundData.size());
	obViewdata.push_back(obNodeData.data(),obNodeData.size());
    
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_RECV_COMPETITION_NODE, 0, 0, obViewdata.data(), obViewdata.size());
    
}

void LegendCupClient::onMsgSendNodeLookupWarInfo(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgLookUpClientNodeHead) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }

    SMsgLookUpClientNodeHead* msg = (SMsgLookUpClientNodeHead*)data;
    if ( len != sizeof(SMsgLookUpClientNodeHead) + msg->nCount * sizeof(SNodeWarRecord))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SMsgLookUpClientNodeHead) + msg->nCount * sizeof(SNodeWarRecord));
        return;
    }

    // 客户端显示层数据
    obuf obViewdata;
	cmd_legendcup_recv_competitionnode_head headData;
	headData.nKin1ID = msg->dwKin1;
    headData.nKin1Score = msg->nKin1Score;
	headData.nKin2ID = msg->dwKin2;
    headData.nKin2Score = msg->nKin2Score;
    headData.nResetEndTime = msg->dwResetEndTime;
    headData.nCount = msg->nCount;
    sstrcpyn(headData.szKin1Name, msg->szKin1Name, sizeof(headData.szKin1Name));
    sstrcpyn(headData.szKin2Name, msg->szKin2Name, sizeof(headData.szKin2Name));
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_recv_competitionnode_head));

    SNodeWarRecord *pData = (SNodeWarRecord *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
		cmd_kin_warrecord_total_warnode baseData;
        baseData.nBoCount = pData->nBoCount;
		baseData.nWarID = pData->llWarID;
		baseData.nBeginTime = pData->dwBeginTime;
		baseData.nWinKinID = pData->dwWinnerKin;
		baseData.nKin1ID = msg->dwKin1;
		baseData.nKin2ID = msg->dwKin2;
		sstrcpyn(baseData.szKin1Name, msg->szKin1Name, sizeof(baseData.szKin1Name));
		sstrcpyn(baseData.szKin2Name, msg->szKin2Name, sizeof(baseData.szKin2Name));
		baseData.byWinFlag = pData->byWinFlag;
		obViewdata.push_back(&baseData, sizeof(cmd_kin_warrecord_total_warnode));
        
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_RECV_COMPETITION_RESULT, 0, 0, obViewdata.data(), obViewdata.size());
}


void LegendCupClient::onMsgSendUpdateSingelCupInfo(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgLegendCupInfo) > len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsg_MatchTypeOrderInfoHead) > len" );
        return;
    }


    SMsgLegendCupInfo* pData = (SMsgLegendCupInfo*)data;
    if ( len != sizeof(SMsgLegendCupInfo))
    {
        ErrorLn(__FUNCTION__ ": len error!! len ="<<len);
        ErrorLn(__FUNCTION__": right len ="<<sizeof(SMsgLegendCupInfo) );
        return;
    }

	DWORD dwSelfID = gClientGlobal->getHero()->getIntProperty(PROPERTY_ID);

    // 客户端显示层数据
    obuf obViewdata;
	cmd_legendcup_recv_cuplist_node nodeData;
	nodeData.nIsNewCreate = pData->bNewCreate;									// 是否新创建
	nodeData.nLegendCupID = pData->llLegendCupID;								// 杯赛ID
	nodeData.nCreaterPDBID = pData->dwCreataActorID;							// 创建者ID
	nodeData.nIsSelfIn = pData->bIsJoinedCup;									// 是否自己参赛
	nodeData.nIsLock = pData->bHavePassword;									// 是否加密
	nodeData.nIsClanLimit = pData->bClanLimit;									// 是否有同联盟限制
	nodeData.nIsOnline = pData->bIsOnlineCup;									// 是否线上比赛
	nodeData.nIsEnd = pData->bIsEnd;											// 是否结束
	nodeData.nIsSystemCreate = (pData->dwCreataActorID <= 0);					// 是否系统创建
	nodeData.nCupConfigID = pData->nCreateConfigID;								// 杯赛配置ID
	nodeData.nRegistLevelLimit = pData->nLevelLimit;							// 报名等级限制
	nodeData.nRegistLeftTime = pData->dwRegisterLeftTime;						// 报名截止时间
	nodeData.nMaxKinNum = pData->byMaxKinNumber;								// 总队伍数量
	nodeData.nCurrentKinNum = pData->nCurrentKinNum;							// 当前队伍数量
	nodeData.nNeedRoundCount = pData->nNeedRoundCount;							// 杯赛需要打几轮
	nodeData.nBeginRound = pData->nBeginRound;									// 本场比赛由第几轮开始
	nodeData.nCurrentRound = pData->nCurrentRound;								// 当前打到第几轮
	nodeData.byCreateCupType = pData->byCreateCupType;							// 创建类型（个人，系统）
    nodeData.nCompetitionBonus = pData->dwTotalCompetitionBonus;                // 比赛总奖金
    nodeData.nRegistGold = pData->dwRegistGold;                                 // 报名需要的金币
	nodeData.nIsSelfCreate = pData->dwCreataActorID == dwSelfID;				// 是否自身创建
	nodeData.nClanID = pData->dwClanID;
	sstrcpyn(nodeData.szCupName, pData->szCupName, sizeof(nodeData.szCupName));						// 杯赛名称
	sstrcpyn(nodeData.szCreateActorName, pData->szCreaterName, sizeof(nodeData.szCreateActorName));	// 发起人名称
	obViewdata.push_back(&nodeData, sizeof(cmd_legendcup_recv_cuplist_node));

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_UPDATE_SINGLE_CUP, 0, 0, obViewdata.data(), obViewdata.size());
}



void LegendCupClient::onMsgSendUpdateSingelCupNodeInfo(SNetMsgHead* head, void* data, size_t len)
{
	if (len != sizeof(LONGLONG) + sizeof(SMsgCompetitionNodeInfo))
	{
		ErrorLn(__FUNCTION__": this message is invalid, len error! len = " << len << " right len = " << sizeof(LONGLONG) + sizeof(SMsgCompetitionNodeInfo));
		return;
	}

	LONGLONG * pLegendCupId = (LONGLONG *)data;
	LONGLONG llLegendCupId = *pLegendCupId;

    SMsgCompetitionNodeInfo* pNodeData = (SMsgCompetitionNodeInfo*)(pLegendCupId + 1);

    // 客户端显示层数据
    obuf obViewdata;
	obViewdata.push_back(&llLegendCupId, sizeof(llLegendCupId));

	cmd_legendcup_recv_competition_node nodeInfo;
	nodeInfo.byCompetitionType = pNodeData->byCompetitionType;							// 1小组赛 2淘汰赛
	nodeInfo.nSearchID = pNodeData->nSerchID;											// 生成的快速定位ID
	nodeInfo.nRoundID = pNodeData->nRoundID;											// 第几轮比赛
	nodeInfo.nGroupID = pNodeData->nGroupID;											// 分配到第几组
	nodeInfo.nDay = pNodeData->nDay;													// 第几天
	nodeInfo.nPos = pNodeData->nPos;													// 位置索引 从1开始
	nodeInfo.nKin1ID = pNodeData->dwKin1;												// 比赛战队1
	nodeInfo.nKin1Score = pNodeData->nKin1Score;										//
	sstrcpyn(nodeInfo.szKin1Name, pNodeData->szKin1Name, sizeof(nodeInfo.szKin1Name));	//	
	nodeInfo.nKin2ID = pNodeData->dwKin2;												// 比赛战队2
	nodeInfo.nKin2Score = pNodeData->nKin2Score;										//
	sstrcpyn(nodeInfo.szKin2Name, pNodeData->szKin2Name, sizeof(nodeInfo.szKin2Name));	//
	nodeInfo.byCompetitionNodeState = pNodeData->eNodeState;							// 节点状态ECompetitionNodeState
	nodeInfo.nWinnerKinID = pNodeData->dwWinnerKinID;									// 胜利战队ID
	nodeInfo.byBoType = pNodeData->byBoType;											// BO类型 ELegendCupBO
	nodeInfo.nCurrentBo = pNodeData->nCurrentBo;										// 已经打了多少场
	nodeInfo.nBeginTime = pNodeData->dwBeginTime;										// 节点比赛开始时间
	nodeInfo.nEnterTime = pNodeData->dwEnterTime;										// 节点进场开始时间
	nodeInfo.nServerTime = pNodeData->dwServerTime;										// 服务器当前时间
	obViewdata.push_back(&nodeInfo, sizeof(cmd_legendcup_recv_competition_node));

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_UPDATE_SINGLE_COMPETITION_NODE, 0, 0, obViewdata.data(), obViewdata.size());
    
}


void LegendCupClient::onMsgSendRegisterSuccess(SNetMsgHead* head, void* data, size_t len)
{
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_REGISTER_SUCCESS, 0, 0, 0, 0);
}

void LegendCupClient::onMsgPublicSuccess(SNetMsgHead* head, void* data, size_t len)
{
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_PUBLIC_SUCCESS, 0, 0, 0, 0);
}

void LegendCupClient::onMsgSystemCancelCup(SNetMsgHead* head, void* data, size_t len)
{
	if (sizeof(LONGLONG) != len)
	{
		ErrorLn(__FUNCTION__ << "sizeof(SMsgClientCreateCupType) > len");
		return;
	}

	LONGLONG* pLegendCupID = (LONGLONG*)data;

	cmd_legendcup_system_cancel_cup cmd;
	cmd.nLegendCupID = *pLegendCupID;
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_LEGENDCUP_SYSTEM_CANCEL_CUP, 0, 0, &cmd, sizeof(cmd));
}

void LegendCupClient::onMsgCreaterPublicSuccess(SNetMsgHead* head, void* data, size_t len)
{
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_CREATER_PUBLIC_SUCCESS, 0, 0, 0, 0);
}


void LegendCupClient::onMsgGetCreateCupTypeInfo(SNetMsgHead* head, void* data, size_t len)
{
    if ( sizeof(SMsgClientCreateCupType) != len )
    {
        ErrorLn(__FUNCTION__ << "sizeof(SMsgClientCreateCupType) > len" );
        return;
    }

    SMsgClientCreateCupType* pNodeData = (SMsgClientCreateCupType*)data;

    // 客户端显示层数据
    obuf obViewdata;
    cmd_legendcup_recv_create_cup_type_info nodeInfo;
    nodeInfo.byLegendCupType = pNodeData->byCreateType;							// 创建杯赛的类型
    nodeInfo.bIsFreeCreate = pNodeData->bIsFree == true? 1 : 0;					// 是否免费
    obViewdata.push_back(&nodeInfo, sizeof(cmd_legendcup_recv_create_cup_type_info));

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_CREATE_CUP_TYPEINFO, 0, 0, obViewdata.data(), obViewdata.size());
}


/** 请求杯赛列表
@param   
@param   
@return  
*/
void LegendCupClient::requestCupList()
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQUEST_CUP_LIST;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));

    net_senddata( obuf.data(), obuf.size() );
}

// 加入杯赛
void LegendCupClient::reqJoinCup(SMsgJoinCup *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_JOIN_CUP;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgJoinCup));

    net_senddata( obuf.data(), obuf.size() );
}

// 退出杯赛
void LegendCupClient::reqExitCup(SMsgExitCup *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_EXIT_CUP;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgExitCup));

    net_senddata( obuf.data(), obuf.size() );
}

// 添加到黑名单
void LegendCupClient::reqAddToBlacklist(SMsgAddToBlackList *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_ADD_TO_BLACKLIST;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgAddToBlackList));

    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqRemoveBlacklist(SMsgRemoveBlackList *pMsgInfo)
{
    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_REMOVE_BLACKLIST;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgRemoveBlackList));

    net_senddata( obuf.data(), obuf.size() );
}

// 请求查看比赛
void LegendCupClient::reqlookOverLegendCup(SMsgRequestLookOverCup *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_LOOK_OVER_LEGENDCUP;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgRequestLookOverCup));

    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqAllBalcklist(SMsgRequestAllBlackKinList *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_ALL_BALCKLIST;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgRequestAllBlackKinList));

    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqKinMemberList(SMsgRequestKinMember *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_KIN_MEMBER_LIST;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgRequestKinMember));

    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqJoinLegendCupRoom(SMsgJoinLegendCupRoom *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SCENE;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CS_REQ_JOIN_LEGENDCUP_ROOM;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgJoinLegendCupRoom));

    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqLookUpCupNodeWarInfo(SMsgLookUpCupNodeWarInfo *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_LOOK_UP_CUP_NODE_WARINFO;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgLookUpCupNodeWarInfo));

    net_senddata( obuf.data(), obuf.size() );
}


void LegendCupClient::reqJudgeSingalWarResult(SMsgJudgeWarResult *pMsgInfo)
{

    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_CUP_JUDGE_SINGAL_WAR_RESULT;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgJudgeWarResult));

    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqCreateCup(SMsgCreateCup *pMsgInfo)
{
    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SCENE;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CS_REQUEST_CREATE_CUP;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgCreateCup));

    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqPublickCompetition(SMsgPublicCompetition *pMsgInfo)
{
    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_PUBLIC_COMPETTIONNODE;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgPublicCompetition));
    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqCreateType(SMsgCreateCupType *pMsgInfo)
{
	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SCENE;
	gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
	gamemsghead.byKeyAction  = CS_REQUEST_CREATE_CUP_TYPE;

	obuf512 obuf;
	obuf.push_back(&gamemsghead, sizeof(gamemsghead));
	obuf.push_back(pMsgInfo, sizeof(SMsgCreateCupType));
    net_senddata( obuf.data(), obuf.size() );
}

// 请求退出杯赛观察
void LegendCupClient::reqQuitObserve(SMsgReqQuitObserve *pMsgInfo)
{
	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
	gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
	gamemsghead.byKeyAction  = CO_REQ_QUIT_OBSERVE;

	obuf512 obuf;
	obuf.push_back(&gamemsghead, sizeof(gamemsghead));
	obuf.push_back(pMsgInfo, sizeof(SMsgReqQuitObserve));
    net_senddata( obuf.data(), obuf.size() );
}

void LegendCupClient::reqOfflinePreEndRegister(SMsgOfflineEndRegister *pMsgInfo)
{
    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
    gamemsghead.byKeyModule  = MSG_MODULEID_LEGENDCUP;
    gamemsghead.byKeyAction  = CO_REQ_OFFLINE_PRE_END_REGISTER;

    obuf512 obuf;
    obuf.push_back(&gamemsghead, sizeof(gamemsghead));
    obuf.push_back(pMsgInfo, sizeof(SMsgOfflineEndRegister));
    net_senddata( obuf.data(), obuf.size() );
}


/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT ILegendCupClient * CreateLegendCupClient(void)
{
	// 如果创建了，则直接返回
	if(g_pLegendCupClient != NULL)
	{
		return g_pLegendCupClient;
	}

	// 实例化legendCup客户端
	LegendCupClient * pLegendCupClient = new LegendCupClient();
	if(pLegendCupClient == NULL || !pLegendCupClient->Create())
	{
		SAFERELEASE(pLegendCupClient);
		return NULL;
	}

	g_pLegendCupClient = pLegendCupClient;
	return g_pLegendCupClient;
}