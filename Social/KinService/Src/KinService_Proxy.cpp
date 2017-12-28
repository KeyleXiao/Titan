#include "stdafx.h"
#include "KinService_Proxy.h"
#include "StubMacroDef.h"

#if MatchServer_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif


IKin* KinService_Proxy::getKin(DWORD dwKinID)
{
	BUILD_MSG_CONTEXT_1( IKinService::getKin,DWORD, dwKinID );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IKin*);
	return 0;
}

SKinInfo KinService_Proxy::getKinInfo(DWORD dwKinID)
{
	BUILD_MSG_CONTEXT_1( IKinService::getKinInfo,DWORD, dwKinID );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(SKinInfo);
    return SKinInfo();
}


SKinMember KinService_Proxy::getKinMemberInfo(DWORD dwKinID)
{
    BUILD_MSG_CONTEXT_1( IKinService::getKinMemberInfo,DWORD, dwKinID );

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(SKinMember);
    return SKinMember();
}


bool KinService_Proxy::deleteKinMember(DWORD dwKinID, DWORD ActorID)
{
	BUILD_MSG_CONTEXT_2( IKinService::deleteKinMember,DWORD, dwKinID,DWORD,ActorID );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(bool);
	return 0;
}

 DWORD KinService_Proxy::getKinMemberList(DWORD dwKinID, PDBID* pReturnArray, DWORD dwArrayMaxSize)
{
	BUILD_MSG_CONTEXT_3( IKinService::getKinMemberList,DWORD ,dwKinID, PDBID*, pReturnArray, DWORD, dwArrayMaxSize);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(DWORD);
	return 0;
}

 DWORD KinService_Proxy::getKinMemberCount(DWORD dwKinID)
 {
	BUILD_MSG_CONTEXT_1( IKinService::getKinMemberCount,DWORD ,dwKinID);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(DWORD);
	return 0;
 }

 DWORD KinService_Proxy::getTotalFightScore(DWORD dwKinID)
 {
     BUILD_MSG_CONTEXT_1( IKinService::getTotalFightScore,DWORD ,dwKinID);

     rkt::obuf resultBuf;
     m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
     RETURN_FROM_MSG_BUF(DWORD);
     return 0;
 }


 bool KinService_Proxy::addClanCtrb(DWORD dwPDBID, int nClanCtrb)
 {
	BUILD_MSG_CONTEXT_2( IKinService::addClanCtrb,DWORD, dwPDBID, int ,nClanCtrb);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(bool);
	return 0;
 }

void KinService_Proxy::addKinLegendAward(SKinCupAwardNode sAwardNode)
 {
	 BUILD_MSG_CONTEXT_1( IKinService::addKinLegendAward, SKinCupAwardNode, sAwardNode);

	 rkt::obuf resultBuf;
	 m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
 }

void KinService_Proxy::addKinLegendWarResult(SKinWarRecordNode sRecordNode)
{
	BUILD_MSG_CONTEXT_1( IKinService::addKinLegendWarResult, SKinWarRecordNode, sRecordNode);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// purpose:  战队杯赛荣誉增加
void KinService_Proxy::addKinLegendGlory(int nKinID, int nNum, int nGloryType)
{
	BUILD_MSG_CONTEXT_3(IKinService::addKinLegendGlory, int, nKinID, int, nNum, int, nGloryType);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// purpose: 取战队杯赛荣誉
int KinService_Proxy::getKinLegendGlory(int nKinID, int nGloryType)
{
	BUILD_MSG_CONTEXT_2(IKinService::getKinLegendGlory, int, nKinID, int, nGloryType);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(int);
	return 0;
}

/// purpose: 获取周活跃度
int KinService_Proxy::getWeekActivity(DWORD dwnKinID)
{
	BUILD_MSG_CONTEXT_1(IKinService::getWeekActivity, DWORD, dwnKinID);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
	RETURN_FROM_MSG_BUF(int);
	return 0;
}

 //////////////////////////////////////////////////////////////////////////////////
 void KinService_Proxy::handleServerMsg( DWORD serverID,SNetMsgHead head,void * data, size_t len )
 {
     obuf256 t_data;
     t_data << serverID << head << len;
     t_data.push_back(data, len);

     BUILD_MSG_BUFFER(IKinService::handleServerMsg, t_data);

     m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
 };

 //////////////////////////////////////////////////////////////////////////////////
 void KinService_Proxy::handleClientMsg( DWORD client,SNetMsgHead head,void * data, size_t len )
 {
     obuf256 t_data;
     t_data << client << head << len;
     t_data.push_back(data, len);

     BUILD_MSG_BUFFER(IKinService::handleClientMsg, t_data);

     m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
 };
