#include "stdafx.h"
#include "ClanSceneService_Stub.h"

bool ClanSceneService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool ClanSceneService_Stub::on_stop()
{
    m_real_service->onStop();
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool ClanSceneService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(IClanSceneService::handleServerMsg) )
    {
        DWORD serverID;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, serverID,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleServerMsg( serverID,head,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(IClanSceneService::handleClientMsg) )
    {
        DWORD client;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, client,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleClientMsg( client,head,bag.get() );
        return true;
    }

	if ( TEST_SAME_FUNCTION(IClanSceneService::getClanName) )
	{
		DWORD nClanID;
		GET_MSG_PARAM_1(DWORD ,nClanID);

		const string & vReturn= m_real_service->getClanName(nClanID);
		if(resultBuf) *resultBuf << vReturn;
		return true;
	}

    if ( TEST_SAME_FUNCTION(IClanSceneService::getNumProp) )
    {
        DWORD dwClaneID; DWORD dwProp;
        GET_MSG_PARAM_2(DWORD, dwClaneID, DWORD, dwProp);

        const int & vReturn= m_real_service->getNumProp(dwClaneID, dwProp);
        if(resultBuf) *resultBuf << vReturn;
        return true;
    }

	if ( TEST_SAME_FUNCTION(IClanSceneService::setNumProp) )
	{
		DWORD dwClanID;
		DWORD dwProp;
		int nValue;
		GET_MSG_PARAM_3(DWORD, dwClanID, DWORD, dwProp, int, nValue);

		const bool & vReturn= m_real_service->setNumProp(dwClanID, dwProp, nValue);
		if(resultBuf) *resultBuf << vReturn;
		return true;
	}

	if ( TEST_SAME_FUNCTION(IClanSceneService::checkCreateClanLegendCup) )
	{
		DWORD dwClanID;
        int LegendCupConfigID;
		GET_MSG_PARAM_2(DWORD ,dwClanID, int, LegendCupConfigID);

		const bool & vReturn = m_real_service->checkCreateClanLegendCup(dwClanID, LegendCupConfigID);
		if(resultBuf) *resultBuf << vReturn;
		return true;
	}

	return false;
}

void ClanSceneService_Stub::release()
{
	if (nullptr != m_real_service)
	{
		m_real_service->release();
		m_real_service = nullptr;
	}

	delete this;
}

void * ClanSceneService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

ClanSceneService_Stub::ClanSceneService_Stub( ClanScenService * pReal )
	: m_real_service( pReal )
{
}