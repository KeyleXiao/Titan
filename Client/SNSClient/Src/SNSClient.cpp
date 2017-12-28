#include "stdafx.h"
#include "SNSClient.h"
#include "GameViewDef.h"
#include "ISchemeCenter.h"
#include "Match_Manageddef.h"
#include "MatchSchemeDef.h"


SNSClient* g_pSNSClient = NULL;

// 构造函数
SNSClient::SNSClient()
{
	
}

// 析构函数
SNSClient::~SNSClient()
{
	if (gClientGlobal->getMessageDispatch() != NULL)
		gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_SNS, this);

	if (gClientGlobal->getEntityClient() != NULL)
	{
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_LOGIN);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_UPDATE_BUDDY);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_RECOMMAND_BUDDY);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_SEND_MSG_DATA);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_UPDATE_REMARK);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_RSP_ADD_BUDDY);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_UPDATE_BUDDYGROUP);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_BUDDY_INFO);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_BUDDY_RELATION);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_ADD_BUDDY_BY_NAME);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_USER_INFO);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_CHANGE_PERMISSION);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_CHANGE_MOOD);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_CHANGE_BUDDYGROUP);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_UPDATE_BUDDYSTATUS);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_INVITE_PLAYGAME);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_REQ_INVITE_MATCH_TEAM);
		gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_SNS_SEND_DATA_TO_SOCIAL);
	}

	g_pSNSClient = NULL;
}

bool SNSClient::Create(void)
{
	// 注册SNS模块的消息
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_SNS, this);

	// 订阅逻辑层发过来的消息
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_LOGIN, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_UPDATE_BUDDY, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_RECOMMAND_BUDDY, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_SEND_MSG_DATA, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_UPDATE_REMARK, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_RSP_ADD_BUDDY, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_UPDATE_BUDDYGROUP, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_BUDDY_INFO, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_BUDDY_RELATION, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_ADD_BUDDY_BY_NAME, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_USER_INFO, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_CHANGE_PERMISSION, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_CHANGE_MOOD, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_CHANGE_BUDDYGROUP, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_UPDATE_BUDDYSTATUS, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_INVITE_PLAYGAME, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_REQ_INVITE_MATCH_TEAM, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_SNS_SEND_DATA_TO_SOCIAL, this);

	return true;
}

void SNSClient::release(void)
{
	delete this;
}

// IMessageHandler，服务器发过来的消息
void SNSClient::onMessage( SNetMsgHead* head, void* data, size_t len)
{
	size_t nLen = len;

	// 全局消息
	if ( head->byKeyModule == MSG_MODULEID_SNS )
	{
		switch( head->byKeyAction )
		{
		case MSG_SNS_MSG_DATA:
			{
				if (nLen < sizeof(SMsgSNSMsgData))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_MSG_DATA,len < sizeof(SMsgSNSMsgData).len=" << nLen);
					return ;
				}

				SMsgSNSMsgData *pMsgData = (SMsgSNSMsgData*)data;

				nLen -= sizeof(SMsgSNSMsgData);

				if (nLen < pMsgData->nContentLen)
				{
					WarningLn(__FUNCTION__ "MSG_SNS_MSG_DATA,ListViewPaginator.ContentLne=" << pMsgData->nContentLen << ",nLen=" << nLen);
					return;
				}

				gamelogic_sns_msg_data cmdInfo;
				cmdInfo.dwMsgID = pMsgData->dwMsgID;
				cmdInfo.dwTime = pMsgData->dwTime;
				cmdInfo.dwFromAccountID = pMsgData->dwFromAccountID;
				cmdInfo.dwToAccountID = pMsgData->dwToAccountID;
				sstrcpyn(cmdInfo.szFromName, pMsgData->szFromName, sizeof(cmdInfo.szFromName));
				cmdInfo.nFontColor = pMsgData->nFontColor;
				cmdInfo.nFontSize = pMsgData->nFontSize;
				sstrcpyn(cmdInfo.szFont, pMsgData->szFont, sizeof(cmdInfo.szFont));

				const char* strContent = (const char*)data + sizeof(SMsgSNSMsgData);
				sstrcpyn(cmdInfo.szContent, strContent, sizeof(cmdInfo.szContent));

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_MSG_DATA, 0, NULL, &cmdInfo, sizeof(cmdInfo));
			}
			break;
		case MSG_SNS_UPDATE_RELATIONSHIP:
			{
				if (nLen < sizeof(SMsgSNSUpdateRelationship))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_RELATIONSHIP_STATUS,len < sizeof(MSG_SNS_RELATIONSHIP_STATUS).len=" << nLen);
					return ;
				}

				SMsgSNSUpdateRelationship *pMsgData = (SMsgSNSUpdateRelationship*)data;

				gamelogic_sns_update_relationship cmdInfo;
				cmdInfo.dwUserID = pMsgData->dwUserID;
				cmdInfo.nStatus = pMsgData->nStatus;
				sstrcpyn(cmdInfo.szActorName, pMsgData->szActorName, sizeof(cmdInfo.szActorName));
				
				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_UPDATE_RELATIONSHIP, 0, NULL, &cmdInfo, sizeof(cmdInfo));
			}
			break;
		case MSG_SNS_BUDDY_STATUS:
			{
				if (nLen < sizeof(SMsgSNSBuddyStatus))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_BUDDY_STATUS,len < sizeof(MSG_SNS_BUDDY_STATUS).len=" << nLen);
					return ;
				}

				SMsgSNSBuddyStatus *pMsgData = (SMsgSNSBuddyStatus*)data;

				gamelogic_sns_buddy_status cmdInfo;
				cmdInfo.dwUserID = pMsgData->dwUserID;
				cmdInfo.nStatus = pMsgData->nStatus;
				sstrcpyn(cmdInfo.szStatusStr, pMsgData->szReverse, sizeof(cmdInfo.szStatusStr));
				cmdInfo.dwLastLogout = pMsgData->dwLastLogoutTime;

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDY_STATUS, 0, NULL, &cmdInfo, sizeof(cmdInfo));
			}
			break;
		case MSG_SNS_BUDDY_RELATION_LIST:
			{
				if (nLen < sizeof(SMsgSNSBuddyRelationList))
				{
					WarningLn(__FUNCTION__ " MSG_SNS_ALL_BUDDY_INFO,len < sizeof(MSG_SNS_BUDDY_STATUS).len=" << nLen);
					return ;
				}

				SMsgSNSBuddyRelationList *pMsgData = (SMsgSNSBuddyRelationList*)data;

				gamelogic_sns_buddyrelation_list_head headInfo;
				headInfo.nCount = pMsgData->nBuddyRelationCount;

				nLen -= sizeof(SMsgSNSBuddyRelationList);
				if (nLen < headInfo.nCount * sizeof(SSNSRelationship))
				{
					ErrorLn(__FUNCTION__ " MSG_SNS_BUDDY_RELATION_LIST, nLen < countInfo.nCount * sizeof(SSNSRelationship)" << nLen);
					return;
				}

				obuf ob;
				ob << headInfo;

				int nOffset = sizeof(SMsgSNSBuddyRelationList);

				for (int i = 0; i < headInfo.nCount; ++i)
				{
					SSNSRelationship *pRelation= (SSNSRelationship*)((const char*)data + nOffset);
					gamelogic_sns_buddyrelation cmdInfo;
					cmdInfo.dwUserID1 = pRelation->dwUserID1;
					cmdInfo.dwUserID2 = pRelation->dwUserID2;
					cmdInfo.dwBuddyGroupID = pRelation->dwBuddyGroupID;
					cmdInfo.nIntimacy = pRelation->nIntimacy;
					cmdInfo.nStatus = pRelation->nStatus;
					sstrcpyn(cmdInfo.szRemark, pRelation->szRemark, sizeof(cmdInfo.szRemark));

					ob.push_back(&cmdInfo, sizeof(cmdInfo));

					nOffset += sizeof(SSNSRelationship);
				}

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDYRELATION_LIST, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_BUDDY_LIST:
			{
				if (nLen < sizeof(SMsgSNSBuddyList))
				{
					WarningLn(__FUNCTION__ " MSG_SNS_BUDDY_LIST,len < sizeof(MSG_SNS_BUDDY_STATUS).len=" << nLen);
					return ;
				}

				SMsgSNSBuddyList *pMsgData = (SMsgSNSBuddyList*)data;

				gamelogic_sns_buddy_list_head headInfo;
				headInfo.nCount = pMsgData->nBuddyCount;

				nLen -= sizeof(SMsgSNSBuddyList);
				if (nLen < headInfo.nCount * sizeof(SSNSBuddyInfo))
				{
					ErrorLn(__FUNCTION__ " MSG_SNS_BUDDY_LIST, nLen < countInfo.nCount * sizeof(SSNSBuddyInfo)" << nLen);
					return;
				}

				obuf ob;
				ob << headInfo;

				int nOffset = sizeof(SMsgSNSBuddyList);

				for (int i = 0; i < headInfo.nCount; ++i)
				{
					SSNSBuddyInfo *pBuddyInfo = (SSNSBuddyInfo*)((const char*)data + nOffset);

					gamelogic_sns_buddy cmdInfo;
					cmdInfo.nUserID = pBuddyInfo->actorInfo.dwUserID;
					cmdInfo.nPermission = pBuddyInfo->userInfo.nPermission;
					cmdInfo.nStatus = pBuddyInfo->statusInfo.nStatus;
					cmdInfo.dwHeadID = pBuddyInfo->actorInfo.dwHeadID;
					cmdInfo.dwGameID = pBuddyInfo->actorInfo.dwGameID;
					cmdInfo.dwLastLogout = pBuddyInfo->actorInfo.dwLastLogout;
					cmdInfo.dwPdbID = pBuddyInfo->actorInfo.dwPdbID;
					cmdInfo.dwWorldID = pBuddyInfo->actorInfo.dwWorldID;
					cmdInfo.dwServerID = pBuddyInfo->actorInfo.dwServerID;
					cmdInfo.dwSceneID = pBuddyInfo->actorInfo.dwSceneID;
					cmdInfo.nLevel = pBuddyInfo->actorInfo.nLevel;
					cmdInfo.nMatchNum = pBuddyInfo->actorInfo.nMatchNum;
					cmdInfo.nWinNum = pBuddyInfo->actorInfo.nWinNum;
					cmdInfo.nSex = pBuddyInfo->actorInfo.nSex;
					cmdInfo.nRankIconID = getRankGradeIcon(pBuddyInfo->actorInfo.nRankGrade);

					sstrcpyn(cmdInfo.szWorldName, pBuddyInfo->actorInfo.szWorldName, sizeof(cmdInfo.szWorldName));
					sstrcpyn(cmdInfo.szStatusStr, pBuddyInfo->statusInfo.szReverse, sizeof(cmdInfo.szStatusStr));
					sstrcpyn(cmdInfo.szName, pBuddyInfo->actorInfo.szName, sizeof(cmdInfo.szName));
					sstrcpyn(cmdInfo.szClan, pBuddyInfo->actorInfo.szClan, sizeof(cmdInfo.szClan));
					sstrcpyn(cmdInfo.szTitle, pBuddyInfo->actorInfo.szTitle, sizeof(cmdInfo.szTitle));
					sstrcpyn(cmdInfo.szRankName, getRankGradeName(pBuddyInfo->actorInfo.nRankGrade).c_str(), sizeof(cmdInfo.szRankName));
					sstrcpyn(cmdInfo.szMood, pBuddyInfo->userInfo.szMood, sizeof(cmdInfo.szMood));

					ob.push_back(&cmdInfo, sizeof(cmdInfo));

					nOffset += sizeof(SSNSBuddyInfo);
				}

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDY_LIST, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_BUDDY_GROUP_LIST:
			{
				if (nLen < sizeof(SMsgSNSBuddyGroupList))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_BUDDY_GROUP_LIST,len < sizeof(MSG_SNS_BUDDY_STATUS).len=" << nLen);
					return ;
				}

				SMsgSNSBuddyGroupList *pMsgData = (SMsgSNSBuddyGroupList*)data;

				gamelogic_sns_buddygroup_list_head headInfo;
				headInfo.nCount = pMsgData->nBuddyGroupCount;

				nLen -= sizeof(SMsgSNSBuddyGroupList);
				if (nLen < headInfo.nCount * sizeof(SSNSBuddyGroupInfo))
				{
					ErrorLn(__FUNCTION__ " MSG_SNS_BUDDY_GROUP_LIST, nLen < countInfo.nCount * sizeof(SSNSBuddyGroupInfo)" << nLen);
					return;
				}

				obuf ob;
				ob << headInfo;

				int nOffset = sizeof(SMsgSNSBuddyGroupList);
				for (int i = 0; i < headInfo.nCount; ++i)
				{
					SSNSBuddyGroupInfo *pBuddyGroupInfo = (SSNSBuddyGroupInfo*)((const char*)data + nOffset);
					gamelogic_sns_buddygroup cmdInfo;
					cmdInfo.nBuddyGroupID = pBuddyGroupInfo->dwBuddyGroupID;
					cmdInfo.nRelationStatus = pBuddyGroupInfo->nRelationStatus;
					sstrcpyn(cmdInfo.szBuddyGroupName, pBuddyGroupInfo->szName, sizeof(cmdInfo.szBuddyGroupName));

					ob.push_back(&cmdInfo, sizeof(cmdInfo));

					nOffset += sizeof(SSNSBuddyGroupInfo);
				}

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDYGROUP_LIST, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_SYSTEM_MESSAGE:
			{
				if (nLen < sizeof(SMsgSNSSystemMsg))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_SYSTEM_MESSAGE,len < sizeof(SMsgSNSSystemMsg).len=" << nLen);
					return ;
				}

				SMsgSNSSystemMsg *pMsgData = (SMsgSNSSystemMsg*)data;
			}
			break;
		case MSG_SNS_BUDDY_INFO:
			{
				if (nLen < sizeof(SMsgSNSBuddyInfo))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_BUDDY_INFO,len < sizeof(SMsgSNSBuddyInfo).len=" << nLen);
					return ;
				}

				SMsgSNSBuddyInfo *pMsgData = (SMsgSNSBuddyInfo*)data;

				SSNSBuddyInfo *pBuddyInfo = &pMsgData->buddyInfo;

				gamelogic_sns_buddy cmdInfo;
				cmdInfo.nUserID = pBuddyInfo->actorInfo.dwUserID;
				cmdInfo.nPermission = pBuddyInfo->userInfo.nPermission;
				cmdInfo.nStatus = pBuddyInfo->statusInfo.nStatus;
				cmdInfo.dwHeadID = pBuddyInfo->actorInfo.dwHeadID;
				cmdInfo.dwGameID = pBuddyInfo->actorInfo.dwGameID;
				cmdInfo.dwLastLogout = pBuddyInfo->actorInfo.dwLastLogout;
				cmdInfo.dwPdbID = pBuddyInfo->actorInfo.dwPdbID;
				cmdInfo.dwWorldID = pBuddyInfo->actorInfo.dwWorldID;
				cmdInfo.dwServerID = pBuddyInfo->actorInfo.dwServerID;
				cmdInfo.dwSceneID = pBuddyInfo->actorInfo.dwSceneID;
				cmdInfo.nLevel = pBuddyInfo->actorInfo.nLevel;
				cmdInfo.nMatchNum = pBuddyInfo->actorInfo.nMatchNum;
				cmdInfo.nWinNum = pBuddyInfo->actorInfo.nWinNum;
				cmdInfo.nSex = pBuddyInfo->actorInfo.nSex;
				cmdInfo.nRankIconID = getRankGradeIcon(pBuddyInfo->actorInfo.nRankGrade);

				sstrcpyn(cmdInfo.szWorldName, pBuddyInfo->actorInfo.szWorldName, sizeof(cmdInfo.szWorldName));
				sstrcpyn(cmdInfo.szStatusStr, pBuddyInfo->statusInfo.szReverse, sizeof(cmdInfo.szStatusStr));
				sstrcpyn(cmdInfo.szName, pBuddyInfo->actorInfo.szName, sizeof(cmdInfo.szName));
				sstrcpyn(cmdInfo.szClan, pBuddyInfo->actorInfo.szClan, sizeof(cmdInfo.szClan));
				sstrcpyn(cmdInfo.szTitle, pBuddyInfo->actorInfo.szTitle, sizeof(cmdInfo.szTitle));
				sstrcpyn(cmdInfo.szRankName, getRankGradeName(pBuddyInfo->actorInfo.nRankGrade).c_str(), sizeof(cmdInfo.szRankName));
				sstrcpyn(cmdInfo.szMood, pBuddyInfo->userInfo.szMood, sizeof(cmdInfo.szMood));

				obuf ob;
				ob.push_back(&cmdInfo, sizeof(cmdInfo));

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDY, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_BUDDY_REMARK:
			{
				if (nLen < sizeof(SMsgSNSBuddyRemark))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_BUDDY_REMARK,len < sizeof(SMsgSNSBuddyRemark).len=" << nLen);
					return ;
				}

				SMsgSNSBuddyRemark *pMsgData = (SMsgSNSBuddyRemark*)data;

				gamelogic_sns_buddy_remark cmdInfo;
				cmdInfo.dwUserID1 = pMsgData->remark.dwUserID1;
				cmdInfo.dwUserID2 = pMsgData->remark.dwUserID2;
				sstrcpyn(cmdInfo.szRemark, pMsgData->remark.szRemark, sizeof(cmdInfo.szRemark));

				obuf ob;
				ob.push_back(&cmdInfo, sizeof(cmdInfo));

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDY_REMARK, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_BUDDY_RELATIONSHIP:
			{
				if (nLen < sizeof(SMsgSNSBuddyRelationship))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_BUDDY_RELATIONSHIP,len < sizeof(SMsgSNSBuddyRemark).len=" << nLen);
					return ;
				}

				SMsgSNSBuddyRelationship *pMsgData = (SMsgSNSBuddyRelationship*)data;

				gamelogic_sns_buddyrelation cmdInfo;
				cmdInfo.dwUserID1 = pMsgData->buddyRelation.dwUserID1;
				cmdInfo.dwUserID2 = pMsgData->buddyRelation.dwUserID2;
				cmdInfo.dwBuddyGroupID = pMsgData->buddyRelation.dwBuddyGroupID;
				cmdInfo.nIntimacy = pMsgData->buddyRelation.nIntimacy;
				cmdInfo.nStatus = pMsgData->buddyRelation.nStatus;
				sstrcpyn(cmdInfo.szRemark, pMsgData->buddyRelation.szRemark, sizeof(cmdInfo.szRemark));

				obuf ob;
				ob.push_back(&cmdInfo, sizeof(cmdInfo));

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDYRELATION, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_BUDDY_INFO_SELF:
			{
				if (nLen < sizeof(SMsgSNSBuddyInfo))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_BUDDY_INFO,len < sizeof(SMsgSNSBuddyInfo).len=" << nLen);
					return ;
				}

				SMsgSNSBuddyInfo *pMsgData = (SMsgSNSBuddyInfo*)data;

				SSNSBuddyInfo *pBuddyInfo = &pMsgData->buddyInfo;

				gamelogic_sns_buddy cmdInfo;
				cmdInfo.nUserID = pBuddyInfo->actorInfo.dwUserID;
				cmdInfo.nPermission = pBuddyInfo->userInfo.nPermission;
				cmdInfo.nStatus = pBuddyInfo->statusInfo.nStatus;
				cmdInfo.dwHeadID = pBuddyInfo->actorInfo.dwHeadID;
				cmdInfo.dwGameID = pBuddyInfo->actorInfo.dwGameID;
				cmdInfo.dwLastLogout = pBuddyInfo->actorInfo.dwLastLogout;
				cmdInfo.dwPdbID = pBuddyInfo->actorInfo.dwPdbID;
				cmdInfo.dwWorldID = pBuddyInfo->actorInfo.dwWorldID;
				cmdInfo.dwServerID = pBuddyInfo->actorInfo.dwServerID;
				cmdInfo.dwSceneID = pBuddyInfo->actorInfo.dwSceneID;
				cmdInfo.nLevel = pBuddyInfo->actorInfo.nLevel;
				cmdInfo.nMatchNum = pBuddyInfo->actorInfo.nMatchNum;
				cmdInfo.nWinNum = pBuddyInfo->actorInfo.nWinNum;
				cmdInfo.nSex = pBuddyInfo->actorInfo.nSex;
				cmdInfo.nRankIconID = getRankGradeIcon(pBuddyInfo->actorInfo.nRankScore);

				sstrcpyn(cmdInfo.szWorldName, pBuddyInfo->actorInfo.szWorldName, sizeof(cmdInfo.szWorldName));
				sstrcpyn(cmdInfo.szStatusStr, pBuddyInfo->statusInfo.szReverse, sizeof(cmdInfo.szStatusStr));
				sstrcpyn(cmdInfo.szName, pBuddyInfo->actorInfo.szName, sizeof(cmdInfo.szName));
				sstrcpyn(cmdInfo.szClan, pBuddyInfo->actorInfo.szClan, sizeof(cmdInfo.szClan));
				sstrcpyn(cmdInfo.szTitle, pBuddyInfo->actorInfo.szTitle, sizeof(cmdInfo.szTitle));
				sstrcpyn(cmdInfo.szRankName, getRankGradeName(pBuddyInfo->actorInfo.nRankScore).c_str(), sizeof(cmdInfo.szRankName));
				sstrcpyn(cmdInfo.szMood, pBuddyInfo->userInfo.szMood, sizeof(cmdInfo.szMood));


				obuf ob;
				ob.push_back(&cmdInfo, sizeof(cmdInfo));

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDY_SELF, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_USERINFO_ACK:
			{
				if (nLen < sizeof(SMsgSNSUserInfo_Ack))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_USERINFO_ACK,len < sizeof(SMsgSNSUserInfo_Ack).len=" << nLen);
					return ;
				}

				SMsgSNSUserInfo_Ack *pMsgData = (SMsgSNSUserInfo_Ack*)data;

				gamelogic_sns_userinfo cmdInfo;
				cmdInfo.dwUserID = pMsgData->userInfo.dwUserID;
				cmdInfo.nPermission = pMsgData->userInfo.nPermission;
				sstrcpyn(cmdInfo.szMood, pMsgData->userInfo.szMood, sizeof(cmdInfo.szMood));

				obuf ob;
				ob.push_back(&cmdInfo, sizeof(cmdInfo));

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_USERINFO, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_BUDDYINFO_CHANGE_NOTIFY:
			{
				if (nLen < sizeof(SMsgSNSBuddyInfoChange_Notify))
				{
					WarningLn(__FUNCTION__ "MSG_SNS_BUDDYINFO_CHANGE_NOTIFY,len < sizeof(SMsgSNSBuddyInfoChange_Notify).len=" << nLen);
					return ;
				}
				
				SMsgSNSBuddyInfoChange_Notify *pMsgData = (SMsgSNSBuddyInfoChange_Notify*)data;

				gamelogic_sns_buddyinfo_change_notify cmdInfo;
				cmdInfo.dwUserID = pMsgData->dwBuddyUserID;
				cmdInfo.nType = pMsgData->type;

				obuf ob;
				ob.push_back(&cmdInfo, sizeof(cmdInfo));

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_BUDDYINFO_CHNAGE_NOTIFY, 0, NULL, ob.data(), ob.size());
			}
			break;
		case MSG_SNS_INVITE_PLAY_GAME_NOTIFY:
			{
				if (nLen < sizeof(SMsgSNSInvitePlayGame_Notify))
				{
					ErrorLn(__FUNCTION__ "len < sizeof(SMsgSNSInvitePlayGame_Notify).len=" << nLen);
					return;
				}

				SMsgSNSInvitePlayGame_Notify *pMsgData = (SMsgSNSInvitePlayGame_Notify*)data;

				gamelogic_sns_invite_playgame_notify cmdInfo;
				cmdInfo.nRoomID = pMsgData->nRoomID;
                cmdInfo.dwInviteActorID = pMsgData->dwInviteActorID;
				sstrcpyn(cmdInfo.szInviterName, pMsgData->szInviterName, sizeof(cmdInfo.szInviterName));

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_INVITE_PLAYGAME_NOTIFY, 0, NULL, &cmdInfo, sizeof(cmdInfo));
			}
			break;
		case MSG_SNS_INVITE_MATCH_TEAM_NOTIFY:
			{
				if (nLen < sizeof(SMsgSNSInviteMatchTeam_Notify))
				{
					ErrorLn(__FUNCTION__ "len < sizeof(SMsgSNSInviteMatchTeam_Notify).len=" << nLen);
					return;
				}

				SMsgSNSInviteMatchTeam_Notify *pMsgData = (SMsgSNSInviteMatchTeam_Notify*)data;

				gamelogic_sns_invite_match_team_notify cmdInfo;
				cmdInfo.dwTargetActorID = pMsgData->dwTargetActorID;
				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_INVITE_MATCH_TEAM_NOTIFY, 0, NULL, &cmdInfo, sizeof(cmdInfo));
			}
			break;
		case MSG_SNS_UPDATE_SCENE_NOTIFY:
			{
				if (nLen < sizeof(SMsgSNSUpdateScene))
				{
					ErrorLn(__FUNCTION__ "len < sizeof(SMsgSNSUpdateScene).len=" << nLen);
					return;
				}

				SMsgSNSUpdateScene *pMsgData = (SMsgSNSUpdateScene*)data;

				gamelogic_sns_update_scene_notify cmdInfo;
				
				cmdInfo.nUserID = pMsgData->dwUserID;
				cmdInfo.nWroldID = pMsgData->dwWorldID;
				cmdInfo.nSceneID = pMsgData->dwSceneID;

				gClientGlobal->getRenderView()->sendControllerCommand(GLOGICCMD_SNS_UPDATE_SCENE_NOTIFY, 0, NULL, &cmdInfo, sizeof(cmdInfo));
			}
			break;
		default:
			break;
		}
	}
}

// IGameViewEventHandler，逻辑层发过来的消息
bool SNSClient::onViewEvent( int eventID, int nParam, const char *strParam, void *ptrParam )
{
	switch (eventID)
	{
	case GVIEWCMD_SNS_LOGIN:
		{
			return false;
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_login *pMsgInfo = (gameview_sns_login*)ptrParam;

			SMsgSNSUserLogin msg;
			msg.actorInfo.dwHeadID = pMsgInfo->nHeadID;
			msg.actorInfo.dwPdbID = pMsgInfo->nPdbid;
			msg.actorInfo.nLevel = pMsgInfo->nLevel;
			msg.actorInfo.nMatchNum = pMsgInfo->nMatchNum;
			msg.actorInfo.nWinNum = pMsgInfo->nWinNum;
			//msg.actorInfo.nRankScore = pMsgInfo->nRankScore;
			sstrcpyn(msg.actorInfo.szClan, pMsgInfo->szClan, sizeof(msg.actorInfo.szClan));
			sstrcpyn(msg.actorInfo.szName, pMsgInfo->szName, sizeof(msg.actorInfo.szName));
			sstrcpyn(msg.actorInfo.szRankName, pMsgInfo->szRankName, sizeof(msg.actorInfo.szRankName));
			sstrcpyn(msg.actorInfo.szTitle, pMsgInfo->szTitle, sizeof(msg.actorInfo.szTitle));

			sendMsgToSocial(MSG_SNS_USER_LOGIN, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_UPDATE_BUDDY:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_update_buddy *pMsgInfo = (gameview_sns_update_buddy*)ptrParam;

			SMsgSNSUpdateBuddy msg;
			msg.dwOtherAccountID = pMsgInfo->dwAddAccountID;
			msg.nOptType = pMsgInfo->nOptType;
			sstrcpyn(msg.szActorName, pMsgInfo->szActorName, sizeof(msg.szActorName));

			sendMsgToSocial(MSG_SNS_UPDATE_BUDDY, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_RECOMMAND_BUDDY:
		{
			SMsgSNSReqRecommandBuddy msg;	
			
			sendMsgToSocial(MSG_SNS_REQ_RECOMMAND_BUDDY, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_UPDATE_REMARK:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_update_remark *pMsgInfo = (gameview_sns_update_remark*)ptrParam;

			SMsgSNSUpdateRemark msg;
			msg.dwUserID = pMsgInfo->dwUserID;
			sstrcpyn(msg.szName, pMsgInfo->szRemark, sizeof(msg.szName));
			sstrcpyn(msg.szActorName, pMsgInfo->szActorName, sizeof(msg.szActorName));

			sendMsgToSocial(MSG_SNS_UPDATE_REMARK, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_RSP_ADD_BUDDY:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_rsp_add_buddy *pMsgInfo = (gameview_sns_rsp_add_buddy *)ptrParam;

			SMsgSNSRspAddBuddy msg;
			msg.dwOtherAccountID = pMsgInfo->dwOtherAccountID;
			msg.nFlag = pMsgInfo->nFlag;
			sstrcpyn(msg.szActorName, pMsgInfo->szActorName, sizeof(pMsgInfo->szActorName));

			sendMsgToSocial(MSG_SNS_RSP_ADD_BUDDY, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_UPDATE_BUDDYGROUP:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_update_buddygroup *pMsgInfo = (gameview_sns_update_buddygroup *)ptrParam;

			SMsgSNSUpdateBuddyGroup msg;
			msg.dwBuddyGroupID = pMsgInfo->dwBuddyGroupID;
			msg.nOptType = pMsgInfo->nOptType;
			sstrcpyn(msg.szName, pMsgInfo->szName, sizeof(msg.szName));

			sendMsgToSocial(MSG_SNS_UPDATE_BUDDYGROUP, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_SEND_MSG_DATA:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_send_msg_data *pMsgInfo = (gameview_sns_send_msg_data *)ptrParam;

			int contentLen = strlen(pMsgInfo->szContent);
			// 加上'\0'，消息的最大长度为SNS_CHAT_MSG_MAXSIZE
			if (contentLen <= 0 || contentLen > SNS_CHAT_MSG_MAXSIZE - 1)
			{
				WarningLn(__FUNCTION__ " GVIEWCMD_SNS_SEND_MSG_DATA, The lenght of chat content is 0 or too long.");
				return false;
			}
			pMsgInfo->szContent[contentLen] = '\0';
			contentLen += 1;


			SMsgSNSMsgData msg;
			msg.dwFromAccountID = pMsgInfo->dwFromAccountID;
			msg.dwMsgID = pMsgInfo->dwMsgID;
			msg.dwTime = pMsgInfo->dwTime;
			msg.dwToAccountID = pMsgInfo->dwToAccountID;
			msg.nContentLen = contentLen;
			msg.nFontColor = pMsgInfo->nFontColor;
			msg.nFontSize = pMsgInfo->nFontSize;
			sstrcpyn(msg.szFont, pMsgInfo->szFromName, sizeof(msg.szFont));
			sstrcpyn(msg.szFromName, pMsgInfo->szFromName, sizeof(msg.szFont));

			sendMsgToSocial(MSG_SNS_SEND_MSG_DATA, pMsgInfo->szContent, contentLen, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_BUDDY_INFO:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_req_buddy_info *pMsgInfo = (gameview_sns_req_buddy_info *)ptrParam;

			SMsgSNSReqBuddyInfo msg;
			msg.dwUserID = pMsgInfo->dwUserID;

			sendMsgToSocial(MSG_SNS_REQ_BUDDY_INFO, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_ADD_BUDDY_BY_NAME:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_req_add_buddy_by_name *pMsgInfo = (gameview_sns_req_add_buddy_by_name *)ptrParam;

			SMsgSNSReqAddBuddyByName msg;
			sstrcpyn(msg.szActorName, pMsgInfo->szActorName, sizeof(msg.szActorName));

			sendMsgToSocial(MSG_SNS_REQ_ADD_BUDDY_BY_NAME, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_BUDDY_RELATION:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_req_buddyrelation *pMsgInfo = (gameview_sns_req_buddyrelation *)ptrParam;

			SMsgSNSReqBuddyRelation msg;
			msg.dwUserID = pMsgInfo->dwUserID;

			sendMsgToSocial(MSG_SNS_REQ_BUDDYRELATION, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_USER_INFO:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_req_userinfo *pMsgInfo = (gameview_sns_req_userinfo *)ptrParam;

			SMsgSNSReqUserInfo msg;
			msg.dwUserID = pMsgInfo->dwUserID;

			sendMsgToSocial(MSG_SNS_REQ_USERINFO, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_CHANGE_PERMISSION:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_req_change_permission *pMsgInfo = (gameview_sns_req_change_permission *)ptrParam;

			SMsgSNSReqChangePermission msg;
			msg.nPermission = pMsgInfo->nPermission;

			sendMsgToSocial(MSG_SNS_REQ_CHANGE_PERMISSION, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_CHANGE_MOOD:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_req_change_mood *pMsgInfo = (gameview_sns_req_change_mood *)ptrParam;

			// 最大长度包含'\0'
			if (strlen(pMsgInfo->szMood) >= MOOD_CONTENT_MAXSIZE)
			{
				return false;
			}

			SMsgSNSReqChangeMood msg;
			sstrcpyn(msg.szMood, pMsgInfo->szMood, sizeof(msg.szMood));

			sendMsgToSocial(MSG_SNS_REQ_CHANGE_MOOD, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_CHANGE_BUDDYGROUP:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_req_change_buddygroup *pMsgInfo = (gameview_sns_req_change_buddygroup *)ptrParam;

			SMsgSNSReqChangeBuddyGroup msg;
			msg.dwUserID = pMsgInfo->dwUserID;
			msg.dwBuddyGroupID = pMsgInfo->dwBuddyGroupID;

			sendMsgToSocial(MSG_SNS_CHANGE_BUDDYGROUP, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_UPDATE_BUDDYSTATUS:
		{
			if (NULL == ptrParam)
			{
				return false;
			}

			gameview_sns_req_update_buddystatus *pMsgInfo = (gameview_sns_req_update_buddystatus *)ptrParam;
			SMsgSNSReqUpdateBuddyStatus msg;
			msg.nStatus = pMsgInfo->nStatus;
			sstrcpyn(msg.szReverse, pMsgInfo->szReverse, sizeof(msg.szReverse));

			sendMsgToSocial(MSG_SNS_REQ_UPDATE_BUDDY_STATUS, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_INVITE_PLAYGAME:
		{
			if (NULL == ptrParam)
				return false;

			gameview_sns_req_invite_play_game* pMsgInfo = (gameview_sns_req_invite_play_game*)ptrParam;
			SMsgSNSReqInvitePlayGame msg;
			msg.nUserID = pMsgInfo->nUserID;
			msg.nRoomID = pMsgInfo->nRoomID;
			sstrcpyn(msg.szInviterName, pMsgInfo->szInviterName, sizeof(msg.szInviterName));

			sendMsgToSocial(MSG_SNS_REQ_INVITE_PLAYGAME, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_REQ_INVITE_MATCH_TEAM:
		{
			if (NULL == ptrParam)
				return false;

			gameview_sns_req_invite_match_team* pMsgInfo = (gameview_sns_req_invite_match_team*)ptrParam;
			SMsgSNSReqInviteMatchTeam msg;
			msg.nUserID = pMsgInfo->nUserID;
			sstrcpyn(msg.szInviterName, pMsgInfo->szInviterName, sizeof(msg.szInviterName));

			sendMsgToSocial(MSG_SNS_REQ_INVITE_MATCH_TEAM, NULL, 0, &msg, sizeof(msg));
		}
		break;
	case GVIEWCMD_SNS_SEND_DATA_TO_SOCIAL:
		{
			if (NULL == ptrParam)
				return false;

			gameview_sns_data_to_social* pMsgInfo = (gameview_sns_data_to_social*)ptrParam;

			SMsgSNSSendDataToSocial msg;
			msg.nMsgType = pMsgInfo->nMsgType;

			obuf ob;
			ob << msg;
			ob << pMsgInfo->nReverse1;
			sendMsgToSocial(MSG_SNS_SEND_DATA_TO_SOCIAL, NULL, 0, ob.data(), ob.size());
		}
		break;
	}
	
	return true;
}



// 填充发向场景服的聊天信息
void SNSClient::fillSNSData(obuf& ob, int destEndPoint, BYTE msgId)
{
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	head.byDestEndPoint = destEndPoint;
	head.byKeyModule = MSG_MODULEID_SNS;
	head.byKeyAction = msgId;
	ob << head;
}

void SNSClient::sendMsgToSocial( BYTE byActionID, const char* strParam /*= NULL*/, int strLen /*= 0*/, void* pData /*= NULL*/, int nLen /*= 0*/ )
{

	//SNetMsgHead gamemsghead;
	//gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	//gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
	//gamemsghead.byKeyModule = MSG_MODULEID_SNS;
	//gamemsghead.byKeyAction = byActionID;

	obuf ob;
	
	fillSNSData(ob, MSG_ENDPOINT_SOCIAL, byActionID);

	if (pData != NULL && nLen > 0)
	{
		ob.push_back(pData, nLen);
	}

	if (strParam != NULL && strLen > 0)
	{
		ob.push_back(strParam, strLen);
	}

    net_senddata( ob.data(), ob.size() );
}

int SNSClient::getRankGradeIcon(int nRankGrade)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return -1;

	ISchemeMatchRankConfigInfo* pRankConfig = pSchemeCenter->getSchemeMatchRankConfig();
	if (pRankConfig == NULL)
		return -1;

	const SMatchRankConfigSchemeInfo* pConfig = pRankConfig->getMatchRankConfigShemeInfo(Match_ManagerDef::MatchType_Rank, nRankGrade);
	if (pConfig == NULL)
		return -1;
	
	return pConfig->nIconID;
}

string SNSClient::getRankGradeName(int nRankGrade)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return "";

	ISchemeMatchRankConfigInfo* pRankConfig = pSchemeCenter->getSchemeMatchRankConfig();
	if (pRankConfig == NULL)
		return "";

	const SMatchRankConfigSchemeInfo* pConfig = pRankConfig->getMatchRankConfigShemeInfo(Match_ManagerDef::MatchType_Rank, nRankGrade);
	if (pConfig == NULL)
		return "";

	return pConfig->szDes;
}

// 输出函数
API_EXPORT ISNSClient * CreateSNSClient(void)
{
	// 如果创建了，则直接返回
	if (g_pSNSClient != NULL)
	{
		return g_pSNSClient;
	}

	// 实例化BUFF客户端
	SNSClient * pSNSClient = new SNSClient();
	if (pSNSClient == NULL || !pSNSClient->Create())
	{
		safeRelease(pSNSClient);
		return NULL;
	}

	g_pSNSClient = pSNSClient;
	return g_pSNSClient;
}

