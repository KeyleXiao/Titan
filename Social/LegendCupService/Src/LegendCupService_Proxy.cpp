#include "stdafx.h"
#include "LegendCupService_Proxy.h"
#include "StubMacroDef.h"

#if MatchServer_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif


void LegendCupService_Proxy::handleServerMsg( DWORD serverID,SNetMsgHead head,PACKAGE_PTR msg )
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( ILegendCupService::handleServerMsg,DWORD ,serverID,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
};

//////////////////////////////////////////////////////////////////////////////////
void LegendCupService_Proxy::handleClientMsg( DWORD client,SNetMsgHead head,PACKAGE_PTR msg )
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( ILegendCupService::handleClientMsg,DWORD ,client,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
};


void LegendCupService_Proxy::exitStartCompetitionNode( LONGLONG llLegendCupID, int nSerchID, DWORD dwNewStartTime )
{
    
    BUILD_MSG_CONTEXT_3( ILegendCupService::handleClientMsg,LONGLONG ,llLegendCupID,int, nSerchID,DWORD, dwNewStartTime );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
};

void LegendCupService_Proxy::endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID)
{
    BUILD_MSG_CONTEXT_1( ILegendCupService::endCupRemoveCreaterRecInfo,LONGLONG ,llLegendCupID);

    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK);
}

void LegendCupService_Proxy::removeServiceCupInfo(LONGLONG llLegendCupID)
{
    BUILD_MSG_CONTEXT_1( ILegendCupService::removeServiceCupInfo,LONGLONG ,llLegendCupID);

    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK);
}

// 设置杯赛比赛结果
void LegendCupService_Proxy::setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult)
{
	BUILD_MSG_CONTEXT_1( ILegendCupService::setLegendCupMatchResult,SMsgRecordNodeWarResult ,sNodeWarResult);

	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK);
}

void LegendCupService_Proxy::setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState)
{
    BUILD_MSG_CONTEXT_1( ILegendCupService::setLegendCupNodeState,SMsgSetCompetitionNodeState ,sNodeState);

    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK);
}

void LegendCupService_Proxy::luaAddCupRegister(LONGLONG llLegendCupID, PACKAGE_PTR pkKinList)
{
    PACKAGE_PTR::T_BAG bag(pkKinList);
    BUILD_MSG_CONTEXT_2( ILegendCupService::luaAddCupRegister,LONGLONG ,llLegendCupID,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

void LegendCupService_Proxy::luaRemoveCupRegister(LONGLONG llLegendCupID)
{
    BUILD_MSG_CONTEXT_1( ILegendCupService::luaRemoveCupRegister,LONGLONG ,llLegendCupID );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

SCreaterLegendCupInfo LegendCupService_Proxy::getCreaterLegendCupInfo(BYTE byCupCreateType,DWORD dwCreaterID)
{
    BUILD_MSG_CONTEXT_2( ILegendCupService::getCreaterLegendCupInfo,BYTE, byCupCreateType,DWORD ,dwCreaterID );
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(SCreaterLegendCupInfo);
    return SCreaterLegendCupInfo();
}

// 获取免费联盟杯赛的奖励次数
int	LegendCupService_Proxy::getPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID)
{
	BUILD_MSG_CONTEXT_1( ILegendCupService::getPlayerFreeClanLegendCupPrizeCount,DWORD, dwPDBID);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(int);
	return 0;
}

// 设置免费联盟杯赛的奖励次数
bool LegendCupService_Proxy::setPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID, int nRecvPrizeCount)
{
	BUILD_MSG_CONTEXT_2( ILegendCupService::setPlayerFreeClanLegendCupPrizeCount,DWORD, dwPDBID,int, nRecvPrizeCount);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(bool);
	return false;
}

// 设置免费联盟杯赛的奖励次数
bool LegendCupService_Proxy::isKinInLegendCup(DWORD dwKinID)
{
    BUILD_MSG_CONTEXT_1( ILegendCupService::isKinInLegendCup,DWORD, dwKinID);

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(bool);
    return false;
}