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

/// purpose: ���캯��
KinClient::KinClient()
{
    m_bMemberVaild = false;
    m_myApplyKinID = 0;
    m_nCoolingTime = 0;
    memset(&m_MemberInfo, 0, sizeof(m_MemberInfo));
}

/// purpose: �鹹����
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
    

	// ȫ��ָ��NULL
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
    

    // ��������а�
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
            
            // �Ƴ��۲�ս��
            exitObserveRequest();

            // �Ƴ��۲�ս���б�
            g_EHelper.SendKinSocial(emMsgKin_Module,  MSG_KIN_LIST_EXITOBSERVE, 0,"", 0);
        }
        break;   

    case  GVIEWCMD_KIN_OPEN :
        {
            // �����ս����Ϣ
            if (m_Kin.IsVaild() == false)
            {
                pChatClient->showSystemMessage(CHAT_TIP_KIN_HAVE_NO_KIN);
                return false;
            }
            // ��ӹ۲�ս���б�
            enterObserveRequest();
            g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_KINBASEDATA, m_Kin.getNumProp(emKinProp_ID), "", 0);
        }
        break;
        // �������ս��
    case GVIEWCMD_KIN_APPLY:
        {
            if (m_Kin.IsVaild())
            {
                // ��ʾ�Ѿ���ս����
                pChatClient->showSystemMessage(CHAT_TIP_KIN_HAD_JOINED_KIN);
                return false;
            }

            if (ptrParam == NULL)
            {
                return false;
            }

            cmd_kin_apply * cmdData = (cmd_kin_apply *)ptrParam;
            joinRequest(cmdData->nKinID,"�������");
        }
        break;

    case  GVIEWCMD_KIN_CREATE :
        {
            if (m_Kin.IsVaild())
            {
                // ��ʾ�Ѿ���ս����
                pChatClient->showSystemMessage(CHAT_TIP_KIN_HAD_JOINED_KIN);
                return false;
            }

            // ����ս��
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
            // ����ս���б�
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
            // �˳�ս��
            quitRequest();
            
        }
        break;
        // �������ս�� 
    case GVIEWCMD_KIN_INVITE:
        {
            if (ptrParam == NULL)
            {
                return false;
            }
            cmd_kin_invite * cmdData = (cmd_kin_invite *)ptrParam;
            SMsgKin_Invite_CO data;
            data.dwAnswerPDBID = cmdData->nAnswerPDBID;			// ������ԱPDBID
            g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_INVITE, m_Kin.getNumProp(emKinProp_ID), &data, sizeof(data));
            
        }
        break;

        // ������Ա
    case GVIEWCMD_KIN_KICK:
        {
            if (ptrParam == NULL)
            {
                return false;
            }
            cmd_kin_kick * cmdData = (cmd_kin_kick *)ptrParam;
            SMsgKin_Kick_CO data;
            data.dwMemberPDBID = cmdData->nMemberPDBID;			// �߳���PDBID
            g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_KICK, m_Kin.getNumProp(emKinProp_ID), &data, sizeof(data));
        }
        break;
        // ���վܾ���Ա
    case GVIEWCMD_KIN_ADMIT:
        {
            // ����ս���б�
            if (ptrParam == NULL)
            {
                return false;
            }
            cmd_kin_admit * cmdData = (cmd_kin_admit *)ptrParam;
            SMsgKin_Admit_CO data;
            data.dwApplyPDBID = cmdData->nApplyPDBID;			// ������ID
            data.bAgree = cmdData->bAgree == 1? true :false;				// ͬ��/�ܾ�

            g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_ADMIT, m_Kin.getNumProp(emKinProp_ID), &data, sizeof(data));

        }
        break;

    case  GVIEWCMD_KIN_DISMISS :
        {
            // ��ɢ���� 
            dismissRequest();
        }
        break;
    case  GVIEWCMD_KIN_MOTIFYNOTICE :
        {
            // �༭���� 
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
            // ���ü���ְλ
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
            // ս�Ӹ���
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
            // �����
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
			// �鿴����ս����Ϣ
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
			// �鿴����ս������
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

/// purpose: ����ս�Ӹ���
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

/// purpose: ��Ӧ����
void KinClient::answerInvite(DWORD dwPDBID, bool bIsAgree)
{
    SMsgKin_Answer_CO sendData;
    sendData.dwInviterPDBID = dwPDBID;
    sendData.bIsAgree = bIsAgree;
    g_EHelper.SendKinSocial(emMsgKin_Module, MSG_KIN_ANSWER_INVITE, 0, &sendData, sizeof(sendData));
}

// �ͻ�����Ӧ����
void KinClient::onClientInvite(LPCSTR pszMsg, int nLen)
{
    if (nLen != sizeof(SMsgKin_Invite_OC))
    {
        return;
    }
    SMsgKin_Invite_OC* pRecvData = (SMsgKin_Invite_OC*)pszMsg;

    // ֪ͨ�ͻ�����ʾ��Ӧ����
	cmd_recv_kin_invite inviteData;
	inviteData.dwInviterPDBID = pRecvData->dwInviterPDBID;
	sstrcpyn(inviteData.szInviteName, pRecvData->szInviteName, sizeof(inviteData.szInviteName));
	sstrcpyn(inviteData.szKinName, pRecvData->szKinName, sizeof(inviteData.szKinName));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_RECVKININVITE, 0, 0, &inviteData, sizeof(cmd_recv_kin_invite));

}

/// purpose: �鿴����ս����Ϣ
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

	// ��ʾ��ս�����ݸ���������װ
	obuf obData;
	int nPosNum = pRecvData->nMemberCount;
	cmd_kin_basedata headData;
    headData.nKinID = pRecvData->dwKinID;
	headData.nKinLevel = pRecvData->nKinLevel;              // ս�ӵȼ� 
	headData.dwShaikhID = pRecvData->dwShaikhID;            // �峤
	headData.nMemberCount = pRecvData->nMemberCount;        // ��Ա����
	headData.nOnlineCount = pRecvData->nOnlineCount;
	headData.nKinMaxMember = pRecvData->nKinMaxMember;
	headData.nTotalFight    = pRecvData->nTotalFight;                          // ս����ս��
	headData.nFightOrder    = pRecvData->nTotalFightOrder;                     // ս��ս������
	sstrcpyn(headData.szKinName, pRecvData->szKinName, sizeof(headData.szKinName));  // ս����
	sstrcpyn(headData.szShaikhName, pRecvData->szShaikhName, sizeof(headData.szShaikhName));  // �峤��
	sstrcpyn(headData.szNotice, pRecvData->szNotice, sizeof(headData.szNotice));  // ս�ӵ�ÿ�չ���
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

		//��װ��Ϣ����ʾ��
		cmd_kin_membernode nodeData;
		nodeData.dwPDBID        = pListMember->dwPDBID;						// ���ID
		nodeData.nLevel         = pListMember->nLevel;						// ��ҵȼ�
		nodeData.nIdentity      = pListMember->nIdentity;					// ����ְλ
		nodeData.bIsOnline      = pListMember->bIsOnline;					// ����Ƿ�����
		nodeData.nLogoutTime    = pListMember->nLogoutTime;					// ����������ʱ��
		nodeData.nVocation      = pListMember->nVocation;					// ���ְҵ
		nodeData.nFaceID        = pListMember->nFaceID;                     // ͷ��ID
		nodeData.bIsPublicOnline= pListMember->bIsPublicOnline;				// ����Ƿ��ڹ���������
		nodeData.dwBattleVal    = pListMember->dwBattleVal;					// ս��ֵ
		nodeData.dwSrcBattleVal = pListMember->dwSrcBattleVal;				// ��һ��ս��ֵ
		nodeData.dwBVOrder      = pListMember->dwBVOrder;					// ս��ֵ����
		nodeData.dwLastOrder    = pListMember->dwLastOrder;					// ��һ��ս��ֵ����
		nodeData.nSex           = pListMember->bySex;						// �Ա�
        nodeData.byGameState    = pListMember->dwState;                     // ��Ϸ״̬
		nodeData.dwWorldID      = pListMember->dwWorldID;                   // ����ID
		nodeData.dwSceneID      = pListMember->dwSceneID;                   // ����ID
		nodeData.serverID       = pListMember->serverID;                    // ������ID
		// ��ö�λ��Ϣ
		const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(pListMember->nMatchType, pListMember->nGrade);
		if (pSchemeInfo != NULL)
		{
			sstrcpyn( nodeData.szGradeName,(char *)pSchemeInfo->szDes, sizeof(nodeData.szGradeName));
		}
		else
		{    
			sstrcpyn( nodeData.szGradeName,a2utf8("��"), sizeof(nodeData.szGradeName));
		}
		sstrcpyn(nodeData.szName, pListMember->szName, sizeof(nodeData.szName));  // �������
		obData.push_back(&nodeData, sizeof(nodeData));

		offset += sizeof(SClientKinMember);
	}

	// ���ڵ�5������������ȥ��  �õڶ����������泤��
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_LEGENDCUPKININFO_RECV, obData.size(), 0, obData.data(), obData.size());

}

/// purpose: �鿴����ս������
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

	// ��ʾ��ս������������װ
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

/// purpose: ����ս������
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

    // �����������ݸı��¼�
    // ��ʾ��ս�����ݸ���������װ
    obuf obData;
    cmd_kin_self_hero_member headData;

    headData.dwPDBID = m_MemberInfo.dwPDBID;								// ��ҽ�ɫID
    
    headData.dwKinID = m_MemberInfo.dwKinID;							    // ս��ID
    headData.nIdentity = m_MemberInfo.nIdentity;						    // ս��ְλ
    headData.nClanCtrb = m_MemberInfo.nClanCtrb;						    // ��ṱ�׶�
    headData.nTotalClanCtrb = m_MemberInfo.nTotalClanCtrb;					// �ۼư�ṱ�׶�

    headData.dwPopedomFlag = m_MemberInfo.dwPopedomFlag;					// Ȩ�ޱ�־��һ��λ��ʾһ��Ȩ�ޣ�һ��������ʾ�����ӵ�е�Ȩ�ޣ�
    headData.bIsOnline = (int)m_MemberInfo.bIsOnline;						// �Ƿ�����
    headData.nLogoutTime = m_MemberInfo.nLogoutTime;					    // �������ʱ��

    headData.nVocation = m_MemberInfo.nVocation;                            // ְҵ
    headData.nLevel = m_MemberInfo.nLevel;							        // �ȼ�
    headData.nFaceID = m_MemberInfo.nFaceID;							    // ���ͷ��

    headData.nLoginTime = m_MemberInfo.nLoginTime;						    // ��Ա����ʱ�䣨�ռ���ĳ�Ա������ʱ��Ϊ��������ʱ�䣩
    headData.dwBattleVal = m_MemberInfo.dwBattleVal;						// ս��ֵ

    headData.dwSrcBattleVal = m_MemberInfo.dwSrcBattleVal;					// ��ս��ֵ
    headData.dwBVOrder = m_MemberInfo.dwBVOrder;							// ս��ֵ����
    headData.dwLastOrder = m_MemberInfo.dwLastOrder;						// ��һ��ս��ֵ����
    
    sstrcpyn(headData.szName, m_MemberInfo.szName, sizeof(headData.szName));  // ��ɫ����


    obData.push_back(&headData, sizeof(cmd_kin_self_hero_member));

    // ���ڵ�5������������ȥ��  �õڶ����������泤��
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_UPDATESELF, obData.size(), 0, obData.data(), obData.size());

}


/** �������
@param   
@param   
@return  
*/
API_EXPORT IKinClient * CreateKinClient(void)
{
	// ��������ˣ���ֱ�ӷ���
	if(g_pKinClient != NULL)
	{
		return g_pKinClient;
	}

	// ʵ����BUFF�ͻ���
	KinClient * pKinClient = new KinClient();
	if(pKinClient == NULL || !pKinClient->Create())
	{
		safeRelease(pKinClient);
		return NULL;
	}

	g_pKinClient = pKinClient;
	return g_pKinClient;
}

/// purpose: �������ս��
void KinClient::joinRequest(DWORD dwKinID, string strLeaveWord)
{
    SMsgKin_Apply_CO sendData;
    sstrcpyn(sendData.szWord, strLeaveWord.c_str(), sizeof(sendData.szWord));
    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_APPLY, dwKinID, &sendData, sizeof(sendData));
}

/// purpose: ������Ա
void KinClient::kickRequest(DWORD dwPDBID)
{
    SMsgKin_Kick_CO sendData;
    sendData.dwMemberPDBID = dwPDBID;

    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_KICK, m_Kin.getNumProp(emKinProp_ID), &sendData, sizeof(sendData));
}

// purpose: �˳�ս��
void KinClient::onQuit(LPCSTR pszMsg, int nLen)
{

    m_bMemberVaild = false;
    memset(&m_MemberInfo, 0, sizeof(SKinMember));

    m_Kin.reset();

    // �����˳�ս������
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
    // ������ʾ����ʾ
    SMsgKinRequestKinList_OC *pMsgHead = (SMsgKinRequestKinList_OC *)pszMsg;
    SMsgKinListNode_OC *pPosFirst = (SMsgKinListNode_OC *)(pMsgHead + 1);

    if (nLen != sizeof(SMsgKinRequestKinList_OC) + pMsgHead->nKinCount * sizeof(SMsgKinListNode_OC))
    {
        ErrorLn(__FUNCTION__": data length is wrong!");
        return;
    }

    obuf obData;
    cmd_kin_list_head headData;
    headData.nKinCount = pMsgHead->nKinCount;										// ս�Ӹ���
    
    int nPosNum = headData.nKinCount;
    obData.push_back(&headData, sizeof(cmd_kin_list_head));

    for (int i = 0; i < nPosNum; ++i)
    { 
        //��װ��Ϣ����ʾ��
        cmd_kin_list_node nodeData;
        nodeData.dwID               = pPosFirst->dwID;										// ս��ID
        nodeData.nMemberCount       = pPosFirst->nMemberCount;								// ս������
        nodeData.nLevel             = pPosFirst->nLevel;									// ս�ӵȼ�
        nodeData.nState             = pPosFirst->nState;									// ս��״̬
        nodeData.nActivity          = pPosFirst->nActivity;									// ��Ծ��
		nodeData.nWeekActivity      = pPosFirst->nWeekActivity;                             // �ܻ�Ծ�� 
        nodeData.bIsApply           = pPosFirst->bIsApply;									// �Ƿ��Ѿ�����
        nodeData.bAutoAccpet        = pPosFirst->bAutoAccpet;								// �Ƿ��Զ�����
        nodeData.nLevelLimit        = pPosFirst->nLevelLimit;								// ���˵ȼ�����
        nodeData.dwAcceptFlag       = pPosFirst->dwAcceptFlag;							    // ��������

        SKinLevel *pKinLevelInfo = pSchemeKinDataCenter->getLevelInfo(pPosFirst->nLevel);
        if (pKinLevelInfo != NULL)
        {
            // ����ʣ��λ��
            nodeData.dwLeftMemberCount  = pKinLevelInfo->nMaxMemberCount - pPosFirst->nMemberCount;
        }

        memcpy(nodeData.szName, pPosFirst->szName, sizeof(nodeData.szName));                // ս����
        memcpy(nodeData.szShaikhName, pPosFirst->szShaikhName, sizeof(nodeData.szName));     // ս���峤����   
        obData.push_back(&nodeData, sizeof(nodeData));
        ++pPosFirst;
    }
    // ���ڵ�5������������ȥ��  �õڶ����������泤��
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REQUESTKINLIST, obData.size(), 0, obData.data(), obData.size());


}

// purpose: ����ս���б�
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
	nodeData.dwID               = pPosFirst->dwID;										// ս��ID
	nodeData.nMemberCount       = pPosFirst->nMemberCount;								// ս������
	nodeData.nLevel             = pPosFirst->nLevel;									// ս�ӵȼ�
	nodeData.nState             = pPosFirst->nState;									// ս��״̬
	nodeData.nActivity          = pPosFirst->nActivity;									// ��Ծ��
	nodeData.bIsApply           = pPosFirst->bIsApply;									// �Ƿ��Ѿ�����
	nodeData.bAutoAccpet        = pPosFirst->bAutoAccpet;								// �Ƿ��Զ�����
	nodeData.nLevelLimit        = pPosFirst->nLevelLimit;								// ���˵ȼ�����
	nodeData.dwAcceptFlag       = pPosFirst->dwAcceptFlag;							    // ��������
	nodeData.nWeekActivity      = pPosFirst->nWeekActivity;
	SKinLevel *pKinLevelInfo = pSchemeKinDataCenter->getLevelInfo(pPosFirst->nLevel);
	if (pKinLevelInfo != NULL)
	{
		// ����ʣ��λ��
		nodeData.dwLeftMemberCount  = pKinLevelInfo->nMaxMemberCount - pPosFirst->nMemberCount;
	}
	memcpy(nodeData.szName, pPosFirst->szName, sizeof(nodeData.szName));                // ս����
	memcpy(nodeData.szShaikhName, pPosFirst->szShaikhName, sizeof(nodeData.szName));     // ս���峤����   
	obData.push_back(&nodeData, sizeof(nodeData));

	// ���ڵ�5������������ȥ��  �õڶ����������泤��
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_UPDATEKINLIST, obData.size(), 0, obData.data(), obData.size());
}

void KinClient::dismissRequest()
{
    // ��ɢս��
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

/// purpose: ����۲�
void KinClient::enterObserveRequest()
{
    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_ENTERTOBSERVE, m_Kin.getNumProp(emKinProp_ID),"", 0);
}

/// purpose: �˳��۲죨�رմ��ڣ�
void KinClient::exitObserveRequest()
{
    g_EHelper.SendKinSocial(emMsgKin_Kin,  MSG_KIN_EXITOBSERVE, m_Kin.getNumProp(emKinProp_ID),"", 0);
}

void KinClient::quitRequest()
{
    g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_QUIT, m_Kin.getNumProp(emKinProp_ID), "", 0);
}

/// purpose: ����ְλ
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

/// purpose: �޸Ĺ���
void KinClient::MotifyRequest(int nPropID, char* szText)
{
    SMsgKin_Motify sendData;
    sendData.nPropID = nPropID;
    sstrcpyn(sendData.szText, szText, KIN_MANIFESTO_LEN);

    g_EHelper.SendKinSocial(emMsgKin_Kin,MSG_KIN_MOTIFY, m_Kin.getNumProp(emKinProp_ID), &sendData, sizeof(sendData));
}