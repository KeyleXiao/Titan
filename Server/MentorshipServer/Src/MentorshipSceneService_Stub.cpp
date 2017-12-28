#include "stdafx.h"
#include "MentorshipSceneService_Stub.h"

bool MentorshipSceneService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool MentorshipSceneService_Stub::on_stop()
{
    m_real_service->onStop();
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool MentorshipSceneService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(IMentorshipSceneService::handleServerMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD serverID; SNetMsgHead head; size_t len;
        in >> serverID >> head >> len;

        m_real_service->handleServerMsg(serverID, head, in.current(), len);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMentorshipSceneService::handleClientMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD client; SNetMsgHead head; size_t len;
        in >> client >> head >> len;

        m_real_service->handleClientMsg(client, head, in.current(), len);
        return true;
    }

	if ( TEST_SAME_FUNCTION(IMentorshipSceneService::sendFightResult) )
	{
		DWORD dwPDBID; 
		bool bFightResult;
		bool bFirstWin;
		BYTE nMentorshipPost;
		GET_MSG_PARAM_4(DWORD, dwPDBID, bool, bFightResult, bool, bFirstWin, BYTE, nMentorshipPost);

		m_real_service->sendFightResult(dwPDBID, bFightResult, bFirstWin, nMentorshipPost);
		return true;
	}

	if ( TEST_SAME_FUNCTION(IMentorshipSceneService::getMyMentorshipPost) )
	{
		DWORD dwSrcPDBID;
		DWORD dwDesPDBID;
		GET_MSG_PARAM_2(DWORD, dwSrcPDBID,DWORD, dwDesPDBID);

		BYTE ret = m_real_service->getMyMentorshipPost(dwSrcPDBID, dwDesPDBID);
		if ( resultBuf ) *resultBuf<<ret;
		return true;
	}

	if ( TEST_SAME_FUNCTION(IMentorshipSceneService::getMentorshipAward))
	{
		DWORD dwPDBID;
		int *nMentorshipActorExpPer;
		int *nMentorshipHeroExpPer;
		int *nMentorhsipGoldPer;
		GET_MSG_PARAM_4(DWORD, dwPDBID, int*, nMentorshipActorExpPer, int*, nMentorshipHeroExpPer, int*, nMentorhsipGoldPer);

		m_real_service->getMentorshipAward(dwPDBID, nMentorshipActorExpPer, nMentorshipHeroExpPer, nMentorhsipGoldPer);
		return true;
	}

	if ( TEST_SAME_FUNCTION(IMentorshipSceneService::getPlayerMasterCount) )
	{
		DWORD dwPDBID;
		GET_MSG_PARAM_1(DWORD, dwPDBID);

		int ret = m_real_service->getPlayerMasterCount(dwPDBID);
		if ( resultBuf ) *resultBuf<<ret;
		return true;
	}

	if ( TEST_SAME_FUNCTION(IMentorshipSceneService::getPlayerPrenticeCount) )
	{
		DWORD dwPDBID;
		GET_MSG_PARAM_1(DWORD, dwPDBID);

		int ret = m_real_service->getPlayerPrenticeCount(dwPDBID);
		if ( resultBuf ) *resultBuf<<ret;
		return true;
	}

	if ( TEST_SAME_FUNCTION(IMentorshipSceneService::checkIsPlayerPrentice) )
	{
		DWORD dwPDBID, dwTargetDBID;
		GET_MSG_PARAM_2(DWORD, dwPDBID, DWORD, dwTargetDBID);

		int ret = m_real_service->checkIsPlayerPrentice(dwPDBID, dwTargetDBID);
		if ( resultBuf ) *resultBuf<<ret;
		return true;
	}

	return false;
}

void MentorshipSceneService_Stub::release()
{
	if (nullptr != m_real_service)
	{
		m_real_service->release();
		m_real_service = nullptr;
	}

	delete this;
}

void * MentorshipSceneService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

MentorshipSceneService_Stub::MentorshipSceneService_Stub( MentorshipSceneService * pReal )
	: m_real_service( pReal )
{
}