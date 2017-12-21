#include "stdafx.h"
#include "SNSService_Proxy.h"
#include "StubMacroDef.h"

#if MatchServer_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

void SNSService_Proxy::recv_message( int msgID, const char* data, int nLen)
{
	obuf256 t_data;
	t_data << msgID << nLen;
	t_data.push_back( data, nLen );

	BUILD_MSG_BUFFER( ISNSService::recv_message,t_data );
	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};



void SNSService_Proxy::addBuddy(DWORD dwUserID1, DWORD dwUserID2,  const char* szActorName1, const char* szActorName2)
{
	BUILD_MSG_CONTEXT( ISNSService::addBuddy,addBuddy_Param );

	pParam->dwUserID1 = dwUserID1;
	pParam->dwUserID2 = dwUserID2;
	lstrcpynA(pParam->szActorName1, szActorName1, sizeof(pParam->szActorName1));
	lstrcpynA(pParam->szActorName2, szActorName2, sizeof(pParam->szActorName2));

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};

// 邀请一起游戏
// @param dwTargetUserID : 被邀请者UserID
// @param dwRoomID : 房间ID
// @param szInviterName : 邀请者名字
void SNSService_Proxy::invitePlayGame(DWORD dwInviterActorID, DWORD dwTargetActorID, DWORD dwRoomID)
{
	BUILD_MSG_CONTEXT_3(ISNSService::invitePlayGame, DWORD, dwInviterActorID, DWORD, dwTargetActorID, DWORD, dwRoomID)

	m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
}