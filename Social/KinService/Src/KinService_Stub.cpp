#include "stdafx.h"
#include "KinService_Stub.h"


bool KinService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}
bool KinService_Stub::on_stop()
{
	TraceLn(__FUNCTION__);
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}
bool KinService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(IKinService::handleServerMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD serverID; SNetMsgHead head; size_t len;
        in >> serverID >> head >> len;

        m_real_service->handleServerMsg(serverID, head, in.current(), len);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IKinService::handleClientMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD client; SNetMsgHead head; size_t len;
        in >> client >> head >> len;

        m_real_service->handleClientMsg(client, head, in.current(), len);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IKinService::getKin) )
    {
        DWORD dwKin;
        GET_MSG_PARAM_1(DWORD,dwKin);

        *resultBuf<< m_real_service->getKin(dwKin);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IKinService::getKinInfo) )
    {
        DWORD dwKin;
        GET_MSG_PARAM_1(DWORD,dwKin);

        *resultBuf<< m_real_service->getKinInfo(dwKin);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IKinService::getKinMemberInfo) )
    {
        DWORD dwPDBID;
        GET_MSG_PARAM_1(DWORD,dwPDBID);

        *resultBuf<< m_real_service->getKinMemberInfo(dwPDBID);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IKinService::getKinMemberList) )
    {
        DWORD dwKinID;PDBID* pReturnArray;DWORD dwArrayMaxSize;
        GET_MSG_PARAM_3(DWORD, dwKinID,PDBID*, pReturnArray,DWORD, dwArrayMaxSize);
           
        *resultBuf<< m_real_service->getKinMemberList(dwKinID, pReturnArray, dwArrayMaxSize);
        return true;
    }
    if ( TEST_SAME_FUNCTION(IKinService::getKinMemberCount) )
    {
        DWORD dwKinID;
        GET_MSG_PARAM_1(DWORD,dwKinID);
           
        *resultBuf<< m_real_service->getKinMemberCount(dwKinID);
        return true;
    }
    if ( TEST_SAME_FUNCTION(IKinService::getTotalFightScore) )
    {
        DWORD dwKinID;
        GET_MSG_PARAM_1(DWORD,dwKinID);

        *resultBuf<< m_real_service->getTotalFightScore(dwKinID);
        return true;
    }
    
    if ( TEST_SAME_FUNCTION(IKinService::addClanCtrb) )
    {
        DWORD dwPDBID;
        int nClanCtrb;
        GET_MSG_PARAM_2(DWORD, dwPDBID,int, nClanCtrb);

        bool ret = m_real_service->addClanCtrb(dwPDBID, nClanCtrb);
		if ( resultBuf ) *resultBuf<<ret;
        return true;
    }
    if ( TEST_SAME_FUNCTION(IKinService::deleteKinMember) )
    {
        DWORD dwKinID;
        DWORD ActorID;
        GET_MSG_PARAM_2(DWORD, dwKinID,DWORD, ActorID);

        bool ret = m_real_service->deleteKinMember(dwKinID, ActorID);
		if ( resultBuf ) *resultBuf<<ret;
        return true;
    }

	if ( TEST_SAME_FUNCTION(IKinService::addKinLegendAward) )
	{
		SKinCupAwardNode sAwardNode;
		GET_MSG_PARAM_1(SKinCupAwardNode, sAwardNode);

		m_real_service->addKinLegendAward(sAwardNode);
		return true;
	}

	if ( TEST_SAME_FUNCTION(IKinService::addKinLegendWarResult) )
	{
		SKinWarRecordNode sRecordNode;
		GET_MSG_PARAM_1(SKinWarRecordNode, sRecordNode);

		m_real_service->addKinLegendWarResult(sRecordNode);
		return true;
	}

	if ( TEST_SAME_FUNCTION(IKinService::addKinLegendGlory) )
	{
		int nKinID;
		int nNum;
		int nGloryType;
		GET_MSG_PARAM_3(int, nKinID, int, nNum, int, nGloryType);

		m_real_service->addKinLegendGlory(nKinID, nNum, nGloryType);
		return true;
	}

	if ( TEST_SAME_FUNCTION(IKinService::getKinLegendGlory) )
	{
		int nKinID;
		int nGloryType;
		GET_MSG_PARAM_2(int, nKinID, int, nGloryType);

		int nNum = m_real_service->getKinLegendGlory(nKinID, nGloryType);
		if ( resultBuf ) *resultBuf<<nNum;
		return true;
	}

	if (TEST_SAME_FUNCTION(IKinService::getWeekActivity))
	{
		DWORD dwKinID;
		GET_MSG_PARAM_1(DWORD, dwKinID);

		int nValue = m_real_service->getWeekActivity(dwKinID);
		if (resultBuf) *resultBuf << nValue;
		return true;
	}

	return false;
}
void * KinService_Stub::query_interface( int iid )
{
	return &m_proxy;
}
void KinService_Stub::release()
{
	TraceLn(__FUNCTION__);
	if ( nullptr != m_real_service )
	{
		m_real_service->release();
		m_real_service = nullptr;
	}
	delete this;
}

KinService_Stub::KinService_Stub(KinService* pReal)
	:m_real_service(pReal)
{

}