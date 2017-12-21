#include "stdafx.h"
#include "LegendCupService_Stub.h"
#include "StubMacroDef.h"


bool LegendCupService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->Create();
}
bool LegendCupService_Stub::on_stop()
{
	TraceLn(__FUNCTION__);
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}
bool LegendCupService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{

    if ( TEST_SAME_FUNCTION(ILegendCupService::handleServerMsg) )
    {
        DWORD serverID;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, serverID,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleServerMsg( serverID,head,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILegendCupService::handleClientMsg) )
    {
        DWORD client;SNetMsgHead head;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_3(DWORD, client,SNetMsgHead ,head,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->handleClientMsg( client,head,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILegendCupService::endCupRemoveCreaterRecInfo) )
    {
        LONGLONG llLegendCupID;
        GET_MSG_PARAM_1(int ,llLegendCupID);
        m_real_service->endCupRemoveCreaterRecInfo(llLegendCupID);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILegendCupService::removeServiceCupInfo) )
    {
        LONGLONG llLegendCupID;
        GET_MSG_PARAM_1(int ,llLegendCupID);
        m_real_service->removeServiceCupInfo(llLegendCupID);
        return true;
    }

	if ( TEST_SAME_FUNCTION(ILegendCupService::setLegendCupMatchResult) )
	{
		SMsgRecordNodeWarResult sNodeWarResult;
		GET_MSG_PARAM_1(SMsgRecordNodeWarResult ,sNodeWarResult);
		m_real_service->setLegendCupMatchResult(sNodeWarResult);
		return true;
	}

    if ( TEST_SAME_FUNCTION(ILegendCupService::setLegendCupNodeState) )
    {
        SMsgSetCompetitionNodeState sNodeState;
        GET_MSG_PARAM_1(SMsgSetCompetitionNodeState ,sNodeState);
        m_real_service->setLegendCupNodeState(sNodeState);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILegendCupService::luaAddCupRegister) )
    {
        LONGLONG llLegendCupID;PACKAGE_PTR::T_BAG bag;
        GET_MSG_PARAM_2(LONGLONG, llLegendCupID,PACKAGE_PTR::T_BAG ,bag );

        m_real_service->luaAddCupRegister( llLegendCupID,bag.get() );
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILegendCupService::exitStartCompetitionNode) )
    {
        LONGLONG llLegendCupID;int nSerchID;DWORD dwNewStartTime;
        GET_MSG_PARAM_3(LONGLONG, llLegendCupID,int,nSerchID, DWORD, dwNewStartTime);

        m_real_service->exitStartCompetitionNode( llLegendCupID,nSerchID, dwNewStartTime);
        return true;
    }

  
    if ( TEST_SAME_FUNCTION(ILegendCupService::luaRemoveCupRegister) )
    {
        LONGLONG llLegendCupID;
        GET_MSG_PARAM_1(LONGLONG, llLegendCupID );

        m_real_service->luaRemoveCupRegister( llLegendCupID);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILegendCupService::getCreaterLegendCupInfo) )
    {
        DWORD dwCreaterID;
        BYTE byCupCreateType;
        GET_MSG_PARAM_2(BYTE, byCupCreateType,DWORD, dwCreaterID );
        *resultBuf<<m_real_service->getCreaterLegendCupInfo(byCupCreateType, dwCreaterID);
        return true;
    }

	if ( TEST_SAME_FUNCTION(ILegendCupService::getPlayerFreeClanLegendCupPrizeCount) )
	{
		DWORD dwPDBID;
		GET_MSG_PARAM_1(DWORD ,dwPDBID);

		*resultBuf << m_real_service->getPlayerFreeClanLegendCupPrizeCount(dwPDBID);
		return true;
	}

    if ( TEST_SAME_FUNCTION(ILegendCupService::setPlayerFreeClanLegendCupPrizeCount) )
    {
        DWORD dwPDBID;
        int nRecvPrizeCount;
        GET_MSG_PARAM_2(DWORD, dwPDBID,int, nRecvPrizeCount);

        *resultBuf << m_real_service->setPlayerFreeClanLegendCupPrizeCount(dwPDBID, nRecvPrizeCount);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILegendCupService::isKinInLegendCup) )
    {
        DWORD dwKinID;
        
        GET_MSG_PARAM_1(DWORD, dwKinID);

        *resultBuf << m_real_service->isKinInLegendCup(dwKinID);
        return true;
    }

	return false;
}
void * LegendCupService_Stub::query_interface( int iid )
{
	return &m_proxy;
}
void LegendCupService_Stub::release()
{
	TraceLn(__FUNCTION__);
	if ( nullptr != m_real_service )
	{
		m_real_service->Release();
		m_real_service = nullptr;
	}
	delete this;
}

LegendCupService_Stub::LegendCupService_Stub(LegendCupService* pReal)
	:m_real_service(pReal)
{
    
}