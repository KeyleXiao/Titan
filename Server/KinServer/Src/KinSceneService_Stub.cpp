#include "stdafx.h"
#include "KinSceneService_Stub.h"

#if KinScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

bool KinSceneService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool KinSceneService_Stub::on_stop()
{
	TraceLn("KinSceneService_Stub::on_stop");
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool KinSceneService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
	if ( pMsg->context_len==0 || pMsg->context==0 )
		return false;

	ibuffer in(pMsg->context,pMsg->context_len);
	const char * function_name;
	in >> function_name;

#ifdef SERVICE_STUB_FUNCTION_OUTPUT
	TraceLn( __FUNCTION__ "() func="<< function_name);
#endif

    if ( TEST_SAME_FUNCTION(IKinSceneService::handleServerMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD serverID; SNetMsgHead head; size_t len;
        in >> serverID >> head >> len;
        m_real_service->handleServerMsg(serverID, head, in.current(), len);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IKinSceneService::handleClientMsg) )
    {
        rkt::ibuffer in(pMsg->context, pMsg->context_len);
        DWORD client; SNetMsgHead head; size_t len;
        in >> client >> head >> len;
        m_real_service->handleClientMsg(client, head, in.current(), len);

        return true;
    }

    if ( TEST_SAME_FUNCTION(IKinSceneService::getKinMemberList) )
    {
        DWORD dwKinID; 
        PDBID* pReturnArray;
        DWORD dwArrayMaxSize;
        GET_MSG_PARAM_3(DWORD, dwKinID, PDBID*, pReturnArray, DWORD, dwArrayMaxSize);

        if ( resultBuf ) *resultBuf<< m_real_service->getKinMemberList( dwKinID, pReturnArray, dwArrayMaxSize);
        return true;
    }
  
	if ( TEST_SAME_FUNCTION(IKinSceneService::sendToSocial) )
	{
		rkt::ibuffer in(pMsg->context,pMsg->context_len);
		BYTE nMsgID = 0; size_t len = 0;
		in >> nMsgID >> len;
		m_real_service->sendToSocial(nMsgID, in.current(), len );
		return true;
	}

    if ( TEST_SAME_FUNCTION(IKinSceneService::getKinInfo) )
    {
        DWORD dwKin;
        GET_MSG_PARAM_1(DWORD,dwKin);

        if ( resultBuf ) *resultBuf<< m_real_service->getKinInfo(dwKin);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IKinSceneService::getKinMember) )
    {
        DWORD dwPDBID;
        GET_MSG_PARAM_1(DWORD,dwPDBID);

        if ( resultBuf ) *resultBuf<< m_real_service->getKinMember(dwPDBID);
        return true;
    }

	if (TEST_SAME_FUNCTION(IKinSceneService::setWeekActivity))
	{
		DWORD dwKinID;
		int nValue;
		GET_MSG_PARAM_2(DWORD, dwKinID, int, nValue);

		bool nIsSuccess = m_real_service->setWeekActivity(dwKinID, nValue);
		if (resultBuf) *resultBuf << nIsSuccess;
		return true;
	}

	if (TEST_SAME_FUNCTION(IKinSceneService::getWeekActivity))
	{
		DWORD dwKinID;
		GET_MSG_PARAM_1(DWORD, dwKinID);

		int nValue = m_real_service->getWeekActivity(dwKinID);
		if (resultBuf) *resultBuf << nValue;
		return true;
	}

	ErrorLn( __FUNCTION__ ":can not found function name:" << function_name);
	return false;
}

void KinSceneService_Stub::release()
{
	TraceLn("KinSceneService_Stub::release");

	if (nullptr != m_real_service)
	{
		m_real_service->release();
		m_real_service = nullptr;
	}

	delete this;
}

void * KinSceneService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

KinSceneService_Stub::KinSceneService_Stub( KinScenService * pReal )
	: m_real_service( pReal )
{

}

