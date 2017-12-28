#include "stdafx.h"
#include "MatchSceneService_Stub.h"

#if MatchScene_DisableShowDebug
#undef MATCH_TRACELN
#define MATCH_TRACELN(x)
#endif

bool MatchSceneService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool MatchSceneService_Stub::on_stop()
{
	MATCH_TRACELN("MatchSceneService_Stub::on_stop");
    m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool MatchSceneService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
	if (TEST_SAME_FUNCTION(IMatchSceneService::handleServerMsg))
	{
		rkt::ibuffer in(pMsg->context, pMsg->context_len);
		DWORD serverID; SNetMsgHead head; size_t len;
		in >> serverID >> head >> len;
		m_real_service->handleServerMsg(serverID, head, in.current(), len);
		return true;
	}

	if (TEST_SAME_FUNCTION(IMatchSceneService::handleClientMsg))
	{
		rkt::ibuffer in(pMsg->context, pMsg->context_len);
		DWORD client; SNetMsgHead head; size_t nLen;
		in >> client >> head >> nLen;

		m_real_service->handleClientMsg(client, head, in.current(), nLen);
		return true;
	}



	if ( TEST_SAME_FUNCTION(IMatchSceneService::start) )
	{
		m_real_service->start();
		return true;
	}

	if ( TEST_SAME_FUNCTION(IMatchSceneService::warEnd) )
	{
		DWORD dwWarID;LONGLONG llDBWarID;
		GET_MSG_PARAM_2(DWORD ,dwWarID, LONGLONG, llDBWarID);
		m_real_service->warEnd( dwWarID, llDBWarID);
		return true;
	}

	if ( TEST_SAME_FUNCTION(IMatchSceneService::sendCreateMobaResult) )
	{
		DWORD dwRoomID=0; int nWarID; int nSceneID = 0; bool bResult=false;
		GET_MSG_PARAM_4(DWORD, dwRoomID, int ,nWarID,int ,nSceneID,bool ,bResult);
		m_real_service->sendCreateMobaResult(dwRoomID, nWarID, nSceneID, bResult);
		return true;
	}

    if ( TEST_SAME_FUNCTION(IMatchSceneService::setClientSceneLoadingProgress) )
    {
        PDBID idActor;int nProgress;long lPing;
        GET_MSG_PARAM_3(PDBID,idActor,int, nProgress,long, lPing);
        m_real_service->setClientSceneLoadingProgress(idActor, nProgress, lPing);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::setWarProcessEnd) )
    {
        int nWarID;
        GET_MSG_PARAM_1(int, nWarID);
        m_real_service->setWarProcessEnd( nWarID );
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::warEndDestoryInvalidActor) )
    {
        int nWarID;
        GET_MSG_PARAM_1(int, nWarID);
        m_real_service->warEndDestoryInvalidActor(nWarID);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::setRuneTalentPageIndex) )
    {
        PDBID idActor;
	    int nRuneIndex;
        int nTalentIndex;     
        GET_MSG_PARAM_3(PDBID, idActor,int, nRuneIndex,int, nTalentIndex);
        m_real_service->setRuneTalentPageIndex(idActor,nRuneIndex,nTalentIndex);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::getTalentPageSelectIndex) )
    {
        PDBID idActor;
        GET_MSG_PARAM_1(PDBID, idActor);
        const int & vReTurn = m_real_service->getTalentPageSelectIndex(idActor);
        if ( resultBuf )*resultBuf << vReTurn;
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::getRunePageSelectIndex) )
    {
        PDBID idActor;
        GET_MSG_PARAM_1(PDBID, idActor);
        const int & vReTurn = m_real_service->getRunePageSelectIndex(idActor);
        if ( resultBuf )*resultBuf << vReTurn;
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::onUpdateRoomPlayerClient ))
    {
        PDBID idActor; 
        ClientID client;
        GET_MSG_PARAM_2(PDBID,idActor,ClientID ,client);
        m_real_service->onUpdateRoomPlayerClient(idActor, client);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::accordLeaveWar ))
    {
        PDBID idActor; 
        GET_MSG_PARAM_1(PDBID,idActor);
        m_real_service->accordLeaveWar(idActor);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::isActorInBattleField ))
    {
        PDBID idActor; 
        GET_MSG_PARAM_1(PDBID,idActor);
        const bool& vReTurn = m_real_service->isActorInBattleField(idActor);
        if ( resultBuf )*resultBuf << vReTurn;
        return true;
    }

    if ( TEST_SAME_FUNCTION(IMatchSceneService::getRoomID ))
    {
        PDBID idActor; 
        GET_MSG_PARAM_1(PDBID,idActor);
        const int& vReTurn = m_real_service->getRoomID(idActor);
        if ( resultBuf )*resultBuf << vReTurn;
        return true;
    }

	if ( TEST_SAME_FUNCTION(IMatchSceneService::setWarResult) )
	{
		SMsgSetWarResultData resultData;
		GET_MSG_PARAM_1(SMsgSetWarResultData, resultData);
		m_real_service->setWarResult(resultData);
		return true;
	}


	return false;
}

void MatchSceneService_Stub::release()
{
	TraceLn("MatchSceneService_Stub::release");

	if (nullptr != m_real_service)
	{
		m_real_service->release();
		m_real_service = nullptr;
	}

	delete this;
}

void * MatchSceneService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

MatchSceneService_Stub::MatchSceneService_Stub( MatchSceneService * pReal )
	: m_real_service( pReal )
    , m_proxy(pReal)
{

}

