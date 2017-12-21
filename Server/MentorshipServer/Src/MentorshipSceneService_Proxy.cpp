﻿#include "stdafx.h"
#include "MentorshipSceneService_Proxy.h"
#include "MentorshipDef.h"

void MentorshipSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( IMentorshipSceneService::handleServerMsg,DWORD ,serverID,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

void MentorshipSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( IMentorshipSceneService::handleClientMsg,DWORD ,client,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

// 发送战斗结果
void MentorshipSceneService_Proxy::sendFightResult(DWORD dwPDBID, bool bFightResult, bool bFirstWin, BYTE nMentorshipPost)
{
	BUILD_MSG_CONTEXT_4( IMentorshipSceneService::sendFightResult,DWORD ,dwPDBID,bool, bFightResult,bool, bFirstWin,BYTE, nMentorshipPost);

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

// 师傅存在师徒关系
BYTE MentorshipSceneService_Proxy::getMyMentorshipPost(DWORD dwSrcPDBID, DWORD dwDesPDBID)
{
	BUILD_MSG_CONTEXT_2( IMentorshipSceneService::getMyMentorshipPost,DWORD, dwSrcPDBID,DWORD,dwDesPDBID );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(BYTE);
	return 0;
}

// 获取师徒奖励
void MentorshipSceneService_Proxy::getMentorshipAward(DWORD dwPDBID, int* nExpPercent, int* nHeroExpPercent, int* nGoldPercent)
{
	BUILD_MSG_CONTEXT_4( IMentorshipSceneService::getMentorshipAward,DWORD, dwPDBID,int* ,nExpPercent,int*, nHeroExpPercent, int*, nGoldPercent );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
}

// 获取师傅个数
int MentorshipSceneService_Proxy::getPlayerMasterCount(DWORD dwDBID)
{
	BUILD_MSG_CONTEXT_1( IMentorshipSceneService::getPlayerMasterCount,DWORD, dwDBID );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(int);
	return 0;
}

// 获取学徒个数
int MentorshipSceneService_Proxy::getPlayerPrenticeCount(DWORD dwDBID)
{
	BUILD_MSG_CONTEXT_1( IMentorshipSceneService::getPlayerPrenticeCount,DWORD, dwDBID );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(int);
	return 0;
}

// 检测目标是否为玩家徒弟
int MentorshipSceneService_Proxy::checkIsPlayerPrentice(DWORD dwDBID, DWORD dwTargetDBID)
{
	BUILD_MSG_CONTEXT_2( IMentorshipSceneService::checkIsPlayerPrentice, DWORD, dwDBID, DWORD, dwTargetDBID );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(int);
	return 0;
}
