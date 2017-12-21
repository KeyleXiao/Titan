#include "stdafx.h"
#include "ClanService_Stub.h"


bool ClanService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->create();
}

bool ClanService_Stub::on_stop()
{
	TraceLn(__FUNCTION__);
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool ClanService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( TEST_SAME_FUNCTION(IClanService::handleServerMsg) )
    {
        DWORD serverID;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, serverID,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleServerMsg( serverID,head,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(IClanService::handleClientMsg) )
    {
        DWORD client;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, client,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleClientMsg( client,head,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(IClanService::release) )
    {
        m_real_service->release();
        return true;
    }
	if ( TEST_SAME_FUNCTION(IClanService::findKinClan) )
	{
		DWORD dwKinID;
		GET_MSG_PARAM_1(DWORD,dwKinID);

		*resultBuf << m_real_service->findKinClan(dwKinID);
		return true;
	}
	if ( TEST_SAME_FUNCTION(IClanService::removeReadingKin) )
	{
		DWORD dwKinID;
		GET_MSG_PARAM_1(DWORD,dwKinID);

		m_real_service->removeReadingKin(dwKinID);
		return true;
	}
	if ( TEST_SAME_FUNCTION(IClanService::getClanKinList) )
	{
		DWORD dwClanID;
		DWORD *pKinList;
		int nLen;
		GET_MSG_PARAM_3(DWORD ,dwClanID,DWORD *,pKinList,int, nLen);

		*resultBuf << m_real_service->getClanKinList(dwClanID, pKinList, nLen);
		return true;
	}
    if ( TEST_SAME_FUNCTION(IClanService::getClanNumProp) )
    {
        DWORD dwClanID;
        int nNumIndex;
        GET_MSG_PARAM_2(DWORD ,dwClanID,int, nNumIndex);

        *resultBuf << m_real_service->getClanNumProp(dwClanID, nNumIndex);
        return true;
    }
	if ( TEST_SAME_FUNCTION(IClanService::getClanName) )
	{
		DWORD dwClanID;
		GET_MSG_PARAM_1(DWORD ,dwClanID);

		const string & vReturn= m_real_service->getClanName(dwClanID);
		if(resultBuf) *resultBuf << vReturn;
		return true;
	}
	if ( TEST_SAME_FUNCTION(IClanService::notifyCreateClanLegendCup) )
	{
		if ( pMsg->context_len==0 || pMsg->context==0 )
			return false;

		ibuffer in(pMsg->context, pMsg->context_len);

		DWORD dwClanID;
		LONGLONG llLegendCupID;
		int nMaxKinCount;
		size_t nLen;
		in >> dwClanID >> llLegendCupID >> nMaxKinCount >> nLen;
		m_real_service->notifyCreateClanLegendCup(dwClanID, llLegendCupID, nMaxKinCount, in.current());
		return true;
	}

	return false;
}

void * ClanService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

void ClanService_Stub::release()
{
	TraceLn(__FUNCTION__);
	if ( nullptr != m_real_service )
	{
		m_real_service->release();
		m_real_service = nullptr;
	}
	delete this;
}

ClanService_Stub::ClanService_Stub(CClanService* pReal)
	:m_real_service(pReal)
{

}