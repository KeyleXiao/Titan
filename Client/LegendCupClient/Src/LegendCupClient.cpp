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

	// ��ʾ���¼�
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

	// ȫ��ָ��NULL
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

	// ��ʾ���¼�
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
	case GVIEWCMD_LEGENDCUP_REQUEST_CUP_LIST:  // �����б�
		{
			requestCupList();
		}
		break;
	case GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP:  // ĳ����ս����Ϣ
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
	case GVIEWCMD_LEGENDCUP_REQUEST_CUP_CREATE:  // ��������
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
	case GVIEWCMD_LEGENDCUP_REQUEST_REGIST_MEMBERLIST:  // ĳ����ս�ӳ�Ա��Ϣ
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
	case GVIEWCMD_LEGENDCUP_ADD_REGIST_KINLIST:  // �������ĳ����
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
	case GVIEWCMD_LEGENDCUP_QUIT_REGIST_KINLIST:  // �����˳�ĳ����
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
	case GVIEWCMD_LEGENDCUP_ADD_REGIST_BLACKLIST:  // ���������
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
	case GVIEWCMD_LEGENDCUP_REQUEST_REGIST_BLACKLIST:  // �鿴������
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
	case GVIEWCMD_LEGENDCUP_DELETE_REGIST_BLACKLIST:  //  ɾ��������
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
	case GVIEWCMD_LEGENDCUP_RELEASE_SINGLE_CUP:  //  ��������
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
	case GVIEWCMD_LEGENDCUP_VIEW_COMPETITION_NODE:  //  �鿴�ڵ����
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
	case GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE:  //  ����ڵ����
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
	case GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE:  //  �þ�����
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
            pInfo.dwResetNewTime = cmdData->lResetNewTime;                       // ���ýڵ��µĿ���ʱ��
            pInfo.dwKin1= cmdData->nKin1;                                         // ����1
            pInfo.nKin1Score = cmdData->nKin1Score;									// ����1�÷�
            pInfo.dwKin2= cmdData->nKin2;                                         // ����2
            pInfo.nKin2Score = cmdData->nKin2Score;									// ����2�÷�
			reqJudgeSingalWarResult(&pInfo);
		}
		break;
	case GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ:	// ���󴴽�����
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
	case GVIEWCMD_LEGENDCUP_QUITOBSERVE:	// �˳������۲�
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

    case GVIEWCMD_LEGENDCUP_OFFLINE_PRE_END_REGISTER:	// Ԥ�Ƚ�������
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
    case OC_CUP_SEND_ALL_BALCKLIST:              // �������к������б��ͻ���
        {
            onMsgSendAllBalcklist(head, data, len);
        }
        break;

    case OC_CUP_SEND_KIN_MEMBER:              // ����ս����ҳ�Ա
        {
            onMsgSendKinMember(head, data, len);

        }
        break;

    case OC_CUP_SEND_ALL_LEGENDCUP:              // ȫ��������Ϣ
        {
            onMsgSendAllLegendcup(head, data, len);

        }
        break;

    case OC_CUP_SEND_ALL_KINLIST:              // ս���б�
        {
            onMsgSendAllKinlist(head, data, len);

        }
        break;

    case OC_CUP_SEND_ALL_KIN_MEMBERINFO:          // �������г�Ա��Ϣ
        {
            onMsgSendAllKinMemberinfo(head, data, len);
        }
		break;

    case OC_CUP_SEND_ALL_ROUNDTIME:             // ����������ʱ��
        {
            onMsgSendAllRoundTimeinfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_ALL_ROUNDINFO:              // �������б����ڵ���Ϣ
        {
            onMsgSendAllRoundInfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_NODE_LOOKUP_WARINFO:              // �������ڵ�ս����Ϣ
        {
            onMsgSendNodeLookupWarInfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_UPDATE_SINGEL_CUP_INFO:              // ���µ���������Ϣ
        {
            onMsgSendUpdateSingelCupInfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_UPDATE_SINGEL_CUP_NODE_INFO:              // ���½ڵ�
        {
            onMsgSendUpdateSingelCupNodeInfo(head, data, len);
        }
        break;

    case OC_CUP_SEND_REGISTER_SUCCESS:                      // �����������ɹ�
        {
            onMsgSendRegisterSuccess(head, data, len);
        }
        break;

    case SC_CUP_CREATE_CUP_TYPE:                      // ��Ӧ������������
        {
            onMsgGetCreateCupTypeInfo(head, data, len);
        }
        break;
    case OC_CUP_SEND_PUBLIC_SUCCESS:                      // �����������ɹ�
        {
            onMsgPublicSuccess(head, data, len);
        }
        break;
	case OC_CUP_SEND_SYSTEM_CANCEL_CUP:                      // �����������ɹ�
		{
			onMsgSystemCancelCup(head, data, len);
		}
		break;

	case OC_CUP_SEND_CREATER_PUBLIC_SUCCESS:			// �������������ɹ�
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

    // �ͻ�����ʾ������
    obuf obViewdata;
	cmd_legendcup_recv_regist_balcklisthead headData;
	headData.nLegendCupID = msg->llLegendCupID;
	headData.nCurBlacklistNum = msg->nCount;
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_recv_regist_balcklisthead));
    
    SMsgBlackKinInfo *pData = (SMsgBlackKinInfo *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        cmd_legendcup_recv_regist_balcklistnode nodeData;
		nodeData.nKinID = pData->nKinProp_ID;										// ս��ID
        nodeData.nKinWinNum = pData->nKinWinNum;								// ս�ӱ�����ʤ��
        nodeData.nKinMaxNum = pData->nKinMaxNum;								// ս�ӱ����ܳ���
		sstrcpyn(nodeData.szKinName, pData->szName, sizeof(nodeData.szKinName));	// ս������
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

    // �ͻ�����ʾ������
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
		nodeData.nPDBID = pData->dwActorID;												// ���PDBID
		nodeData.nActorLevel = pData->nLevel;											// ��ҵȼ�
		nodeData.nActorSex = pData->bSex ? 1 : 0;										// ����Ա�(0��1Ů)
		sstrcpyn(nodeData.szActorName, pData->szActorName, sizeof(nodeData.szActorName));// �������

		nodeData.nType = pData->nMatchType;												// ��������
		nodeData.nRankGrade = pData->nGrade;											// ���ݶ�λ
		nodeData.nRankScore = pData->nRankScore;										// ���ݷ���
		// ��ö�λ��Ϣ
		const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(nodeData.nType, nodeData.nRankGrade);
		if (pSchemeInfo != NULL)
		{
			nodeData.nRankIconId = pSchemeInfo->nIconID;								// ��λͼƬID
			sstrcpyn( nodeData.szGradeName,(char *)pSchemeInfo->szDes, sizeof(nodeData.szGradeName));// ��λ����
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

    // �ͻ�����ʾ������
    obuf obViewdata;

	cmd_legendcup_recv_cuplist_head headData;
	headData.nCount = msg->nCount;
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_recv_cuplist_head));

    SMsgLegendCupInfo *pData = (SMsgLegendCupInfo *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        // �߼������� ������˳����Ϊ��ֵ�洢
		cmd_legendcup_recv_cuplist_node nodeData;
		nodeData.nLegendCupID = pData->llLegendCupID;								// ����ID
		nodeData.nCreaterPDBID = pData->dwCreataActorID;							// ������ID
		nodeData.nIsSelfIn = pData->bIsJoinedCup;									// �Ƿ��Լ�����
		nodeData.nIsLock = pData->bHavePassword;									// �Ƿ����
		nodeData.nIsClanLimit = pData->bClanLimit;									// �Ƿ���ͬ��������
		nodeData.nIsOnline = pData->bIsOnlineCup;									// �Ƿ����ϱ���
		nodeData.nIsEnd = pData->bIsEnd;											// �Ƿ����
		nodeData.nIsSystemCreate = (pData->dwCreataActorID <= 0);					// �Ƿ�ϵͳ����
		nodeData.nCupConfigID = pData->nCreateConfigID;								// ��������ID
		nodeData.nRegistLevelLimit = pData->nLevelLimit;							// �����ȼ�����
		nodeData.nRegistLeftTime = pData->dwRegisterLeftTime;						// ������ֹʱ��
		nodeData.nMaxKinNum = pData->byMaxKinNumber;								// �ܶ�������
		nodeData.nCurrentKinNum = pData->nCurrentKinNum;							// ��ǰ��������
		nodeData.byCreateCupType = pData->byCreateCupType;							// �������ͣ����ˣ�ϵͳ��
		
		nodeData.nNeedRoundCount = pData->nNeedRoundCount;							// ������Ҫ����
		nodeData.nBeginRound = pData->nBeginRound;									// ���������ɵڼ��ֿ�ʼ
		nodeData.nCurrentRound = pData->nCurrentRound;								// ��ǰ�򵽵ڼ���
        nodeData.nCompetitionBonus = pData->dwTotalCompetitionBonus;                // �����ܽ���
        nodeData.nRegistGold = pData->dwRegistGold;                                 // ������Ҫ�Ľ��
		nodeData.nIsSelfCreate = pData->dwCreataActorID == dwSelfID;				// �Ƿ�������
		nodeData.nClanID = pData->dwClanID;
        
        sstrcpyn(nodeData.szCupName, pData->szCupName, sizeof(nodeData.szCupName));						// ��������
        sstrcpyn(nodeData.szCreateActorName, pData->szCreaterName, sizeof(nodeData.szCreateActorName));	// ����������

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

    // �ͻ�����ʾ������
    obuf obViewdata;
	cmd_legendcup_recv_regist_kinlisthead headData;
	headData.nLegendCupID = msg->llLegendCupID;								// ����ID
	headData.nRegistLeftTime = msg->dwRegisterLeftTime;						// ����ʣ��ʱ��
	headData.nMaxKinNum = msg->nMaxCount;									// �ܶ�������
	headData.nCurKinCount = msg->nCount;									// ��ǰս����
	headData.nConfirmKinLeftTime = msg->dwConfirmKinLeftTime;				// ��������ȷ��ʣ��ʱ��
	obViewdata.push_back(&headData, sizeof(cmd_legendcup_recv_regist_kinlisthead));

    SMsgRegisterKinInfo *pData = (SMsgRegisterKinInfo *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
        cmd_legendcup_recv_regist_kinlistnode nodeData;
		nodeData.nLegendCupID = msg->llLegendCupID;											// ����ID
		nodeData.nKinID = pData->nKinProp_ID;												// ս��ID
		nodeData.nKinLevel = pData->nKinProp_Level;											// ս�ӵȼ�
		nodeData.nKinWinNum = pData->nKinWinNum;											// ս�ӱ�����ʤ��
		nodeData.nKinMaxNum = pData->nKinMaxNum;											// ս�ӱ����ܳ���
		sstrcpyn(nodeData.szKinName, pData->szName, sizeof(nodeData.szKinName));			// ս������
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

    // �ͻ�����ʾ������
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

		memberInfo.nType = pData->nMatchType;												// ��������
		memberInfo.nRankGrade = pData->nGrade;											// ���ݶ�λ
		memberInfo.nRankScore = pData->nRankScore;										// ���ݷ���
		// ��ö�λ��Ϣ
		const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(memberInfo.nType, memberInfo.nRankGrade);
		if (pSchemeInfo != NULL)
		{
			memberInfo.nRankIconId = pSchemeInfo->nIconID;								// ��λͼƬID
			sstrcpyn( memberInfo.szGradeName,(char *)pSchemeInfo->szDes, sizeof(memberInfo.szGradeName));// ��λ����
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

    // �ͻ�����ʾ������
    obuf obViewdata;
    cmd_legendcup_competition_kinmemberhead headData;
    headData.nLegendCupID = msg->llLegendCupID;
    headData.nMemberCount = msg->nCount;
    obViewdata.push_back(&headData, sizeof(cmd_legendcup_competition_kinmemberhead));

    SRoundTimeInfo *pData = (SRoundTimeInfo *)(msg+1);
    for (int i = 0; i< msg->nCount;  i++)
    {
		//�ִ���Ϣ��ʾ�����ݸ�ֵ
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

    // �������е�����Ϣ
    obuf obViewdata;

	cmd_legendcup_recv_competition_head headInfo;	// ͷ��Ϣ
	obuf obRoundData;								// �ִ�ʱ����Ϣ
	obuf obNodeData;								// �ڵ���Ϣ

    // pFirstDataָ��ָ��ÿ�ֵ���������
    char *pFirstData = (char *)(msg+1);
    for (int n = 0 ; n < msg->nRoundCount; ++n)
    {
        SMsgCompetitionRoundHead * pData = (SMsgCompetitionRoundHead *)pFirstData;

		// ͷ��Ϣ��ʾ�����ݸ�ֵ
		if (pData->nRoundID == msg->nRoundCount)				//��ǰ�ִ���Ϣ��ֵ
		{
			headInfo.nLegendCupID = msg->llLegendCupID;										// ����ID
			headInfo.nCompetitionNodeCount = msg->nAllNodeCount;							// �����ڵ�����
			headInfo.nRoundID = pData->nRoundID;											// ��ǰ����
			headInfo.nIsFinalRound = pData->bIsFinal ? 1 : 0;								// �Ƿ������
			headInfo.nGroupCount = pData->nMaxGoup;											// �������� 
			headInfo.nGroupMaxKin = pData->nGroupMaxKin;									// ÿ�����ս������
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

			for (int j = 0; j< headInfo.nGroupMaxKin * headInfo.nGroupCount;  j++ )			// ��ǰ��ս����Ϣ
			{
				headInfo.kinInfos[j].nKinID = pData->KinIDList[j].dwKinID;
				sstrcpyn(headInfo.kinInfos[j].szKinName, pData->KinIDList[j].szKinName, sizeof(headInfo.kinInfos[j].szKinName));
			}

			//for (int j = 0; j< headInfo.nGroupMaxKin * headInfo.nGroupCount;  j++ )			// ��ǰ��ս����Ϣ
			//{
			//	int nRow = j % headInfo.nGroupCount;
			//	int nCol = j / headInfo.nGroupCount;
			//	int nPos = nRow * headInfo.nGroupMaxKin + nCol;

			//	headInfo.kinInfos[nPos].nKinID = pData->KinIDList[j].dwKinID;
			//	sstrcpyn(headInfo.kinInfos[nPos].szKinName, pData->KinIDList[j].szKinName, sizeof(headInfo.kinInfos[nPos].szKinName));
			//}
		}

		//�ִ���Ϣ��ʾ�����ݸ�ֵ
		cmd_legendcup_recv_competition_roundinfo roundInfo;
		roundInfo.nRoundID = pData->nRoundID;
		//roundInfo.nRoundBeginTime = pData->dwRoundBeginTime;
		obRoundData.push_back(&roundInfo, sizeof(cmd_legendcup_recv_competition_roundinfo));

		//�ڵ���Ϣ��ʾ�����ݸ�ֵ
        SMsgCompetitionNodeInfo *pNodeData = (SMsgCompetitionNodeInfo *)(pData + 1);
        for (int i = 0; i< pData->nCount;  i++)
        {
			cmd_legendcup_recv_competition_node nodeInfo;
			nodeInfo.byCompetitionType = pNodeData->byCompetitionType;							// 1С���� 2��̭��
			nodeInfo.nSearchID = pNodeData->nSerchID;											// ���ɵĿ��ٶ�λID
			nodeInfo.nRoundID = pNodeData->nRoundID;											// �ڼ��ֱ���
			nodeInfo.nGroupID = pNodeData->nGroupID;											// ���䵽�ڼ���
			nodeInfo.nDay = pNodeData->nDay;													// �ڼ���
			nodeInfo.nPos = pNodeData->nPos;													// λ������ ��1��ʼ
			nodeInfo.nKin1ID = pNodeData->dwKin1;												// ����ս��1
			nodeInfo.nKin1Score = pNodeData->nKin1Score;										//
			sstrcpyn(nodeInfo.szKin1Name, pNodeData->szKin1Name, sizeof(nodeInfo.szKin1Name));	//	
			nodeInfo.nKin2ID = pNodeData->dwKin2;												// ����ս��2
			nodeInfo.nKin2Score = pNodeData->nKin2Score;										//
			sstrcpyn(nodeInfo.szKin2Name, pNodeData->szKin2Name, sizeof(nodeInfo.szKin2Name));	//
			nodeInfo.byCompetitionNodeState = pNodeData->eNodeState;							// �ڵ�״̬ECompetitionNodeState
			nodeInfo.nWinnerKinID = pNodeData->dwWinnerKinID;									// ʤ��ս��ID
			nodeInfo.byBoType = pNodeData->byBoType;											// BO���� ELegendCupBO
			nodeInfo.nCurrentBo = pNodeData->nCurrentBo;										// �Ѿ����˶��ٳ�
			nodeInfo.nBeginTime = pNodeData->dwBeginTime;										// �ڵ������ʼʱ��
			nodeInfo.nEnterTime = pNodeData->dwEnterTime;										// �ڵ������ʼʱ��
			nodeInfo.nServerTime = pNodeData->dwServerTime;										// ��������ǰʱ��
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

    // �ͻ�����ʾ������
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

    // �ͻ�����ʾ������
    obuf obViewdata;
	cmd_legendcup_recv_cuplist_node nodeData;
	nodeData.nIsNewCreate = pData->bNewCreate;									// �Ƿ��´���
	nodeData.nLegendCupID = pData->llLegendCupID;								// ����ID
	nodeData.nCreaterPDBID = pData->dwCreataActorID;							// ������ID
	nodeData.nIsSelfIn = pData->bIsJoinedCup;									// �Ƿ��Լ�����
	nodeData.nIsLock = pData->bHavePassword;									// �Ƿ����
	nodeData.nIsClanLimit = pData->bClanLimit;									// �Ƿ���ͬ��������
	nodeData.nIsOnline = pData->bIsOnlineCup;									// �Ƿ����ϱ���
	nodeData.nIsEnd = pData->bIsEnd;											// �Ƿ����
	nodeData.nIsSystemCreate = (pData->dwCreataActorID <= 0);					// �Ƿ�ϵͳ����
	nodeData.nCupConfigID = pData->nCreateConfigID;								// ��������ID
	nodeData.nRegistLevelLimit = pData->nLevelLimit;							// �����ȼ�����
	nodeData.nRegistLeftTime = pData->dwRegisterLeftTime;						// ������ֹʱ��
	nodeData.nMaxKinNum = pData->byMaxKinNumber;								// �ܶ�������
	nodeData.nCurrentKinNum = pData->nCurrentKinNum;							// ��ǰ��������
	nodeData.nNeedRoundCount = pData->nNeedRoundCount;							// ������Ҫ����
	nodeData.nBeginRound = pData->nBeginRound;									// ���������ɵڼ��ֿ�ʼ
	nodeData.nCurrentRound = pData->nCurrentRound;								// ��ǰ�򵽵ڼ���
	nodeData.byCreateCupType = pData->byCreateCupType;							// �������ͣ����ˣ�ϵͳ��
    nodeData.nCompetitionBonus = pData->dwTotalCompetitionBonus;                // �����ܽ���
    nodeData.nRegistGold = pData->dwRegistGold;                                 // ������Ҫ�Ľ��
	nodeData.nIsSelfCreate = pData->dwCreataActorID == dwSelfID;				// �Ƿ�������
	nodeData.nClanID = pData->dwClanID;
	sstrcpyn(nodeData.szCupName, pData->szCupName, sizeof(nodeData.szCupName));						// ��������
	sstrcpyn(nodeData.szCreateActorName, pData->szCreaterName, sizeof(nodeData.szCreateActorName));	// ����������
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

    // �ͻ�����ʾ������
    obuf obViewdata;
	obViewdata.push_back(&llLegendCupId, sizeof(llLegendCupId));

	cmd_legendcup_recv_competition_node nodeInfo;
	nodeInfo.byCompetitionType = pNodeData->byCompetitionType;							// 1С���� 2��̭��
	nodeInfo.nSearchID = pNodeData->nSerchID;											// ���ɵĿ��ٶ�λID
	nodeInfo.nRoundID = pNodeData->nRoundID;											// �ڼ��ֱ���
	nodeInfo.nGroupID = pNodeData->nGroupID;											// ���䵽�ڼ���
	nodeInfo.nDay = pNodeData->nDay;													// �ڼ���
	nodeInfo.nPos = pNodeData->nPos;													// λ������ ��1��ʼ
	nodeInfo.nKin1ID = pNodeData->dwKin1;												// ����ս��1
	nodeInfo.nKin1Score = pNodeData->nKin1Score;										//
	sstrcpyn(nodeInfo.szKin1Name, pNodeData->szKin1Name, sizeof(nodeInfo.szKin1Name));	//	
	nodeInfo.nKin2ID = pNodeData->dwKin2;												// ����ս��2
	nodeInfo.nKin2Score = pNodeData->nKin2Score;										//
	sstrcpyn(nodeInfo.szKin2Name, pNodeData->szKin2Name, sizeof(nodeInfo.szKin2Name));	//
	nodeInfo.byCompetitionNodeState = pNodeData->eNodeState;							// �ڵ�״̬ECompetitionNodeState
	nodeInfo.nWinnerKinID = pNodeData->dwWinnerKinID;									// ʤ��ս��ID
	nodeInfo.byBoType = pNodeData->byBoType;											// BO���� ELegendCupBO
	nodeInfo.nCurrentBo = pNodeData->nCurrentBo;										// �Ѿ����˶��ٳ�
	nodeInfo.nBeginTime = pNodeData->dwBeginTime;										// �ڵ������ʼʱ��
	nodeInfo.nEnterTime = pNodeData->dwEnterTime;										// �ڵ������ʼʱ��
	nodeInfo.nServerTime = pNodeData->dwServerTime;										// ��������ǰʱ��
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

    // �ͻ�����ʾ������
    obuf obViewdata;
    cmd_legendcup_recv_create_cup_type_info nodeInfo;
    nodeInfo.byLegendCupType = pNodeData->byCreateType;							// ��������������
    nodeInfo.bIsFreeCreate = pNodeData->bIsFree == true? 1 : 0;					// �Ƿ����
    obViewdata.push_back(&nodeInfo, sizeof(cmd_legendcup_recv_create_cup_type_info));

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LEGENDCUP_CREATE_CUP_TYPEINFO, 0, 0, obViewdata.data(), obViewdata.size());
}


/** �������б�
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

// ���뱭��
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

// �˳�����
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

// ��ӵ�������
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

// ����鿴����
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

// �����˳������۲�
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


/** �������
@param   
@param   
@return  
*/
API_EXPORT ILegendCupClient * CreateLegendCupClient(void)
{
	// ��������ˣ���ֱ�ӷ���
	if(g_pLegendCupClient != NULL)
	{
		return g_pLegendCupClient;
	}

	// ʵ����legendCup�ͻ���
	LegendCupClient * pLegendCupClient = new LegendCupClient();
	if(pLegendCupClient == NULL || !pLegendCupClient->Create())
	{
		SAFERELEASE(pLegendCupClient);
		return NULL;
	}

	g_pLegendCupClient = pLegendCupClient;
	return g_pLegendCupClient;
}