#include "stdafx.h"
#include "MatchSceneService_Proxy.h"

#if MatchScene_DisableShowDebug
#undef MATCH_TRACELN
#define MATCH_TRACELN(x)
#endif

MatchSceneService_Proxy::MatchSceneService_Proxy(MatchSceneService * pReal)
    : m_pContainer(0)
{
    m_pReal = pReal;
}

MatchSceneService_Proxy::~MatchSceneService_Proxy()
{
    m_pReal = NULL;
}

void MatchSceneService_Proxy::start()
{
	BUILD_MSG_CONTEXT_VOID( IMatchSceneService::start );

	m_pContainer->post_message( pMsg,nMsgLen,0);
}

void MatchSceneService_Proxy::warEnd(DWORD dwWarID, LONGLONG llDBWarID)
{
	BUILD_MSG_CONTEXT_2( IMatchSceneService::warEnd,DWORD, dwWarID,LONGLONG, llDBWarID );

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 设置战场结果
void MatchSceneService_Proxy::setWarResult(SMsgSetWarResultData resultData)
{
	BUILD_MSG_CONTEXT_1( IMatchSceneService::setWarResult,SMsgSetWarResultData, resultData);

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 发送创建结果
void MatchSceneService_Proxy::sendCreateMobaResult(DWORD dwRoomID, int nWarID, int nSceneID, bool bResult)
{
	BUILD_MSG_CONTEXT_4( IMatchSceneService::sendCreateMobaResult,DWORD ,dwRoomID, int ,nWarID, int, nSceneID, bool, bResult );

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

void MatchSceneService_Proxy::setClientSceneLoadingProgress(PDBID idActor, int nProgress, long lPing)
{
    
	BUILD_MSG_CONTEXT_3( IMatchSceneService::setClientSceneLoadingProgress,PDBID, idActor, int, nProgress, long, lPing);

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 角色是否在战场
bool MatchSceneService_Proxy::isActorInBattleField(PDBID idActor)
{
    BUILD_MSG_CONTEXT_1(IMatchSceneService::isActorInBattleField,PDBID, idActor);

    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(bool);

    return false;
}

int MatchSceneService_Proxy::getRoomID(PDBID idActor)
{
    BUILD_MSG_CONTEXT_1(IMatchSceneService::getRoomID,PDBID, idActor);

    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(int);

    return false;
}

void MatchSceneService_Proxy::setWarProcessEnd(int nWarID)
{
	BUILD_MSG_CONTEXT_1( IMatchSceneService::setWarProcessEnd,int, nWarID );

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

void MatchSceneService_Proxy::setRuneTalentPageIndex(PDBID idActor,int nRunePageIndex,int nTalentPageIndex)
{
	BUILD_MSG_CONTEXT_3( IMatchSceneService::setRuneTalentPageIndex,PDBID, idActor,int, nRunePageIndex,int, nTalentPageIndex );

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

int MatchSceneService_Proxy::getTalentPageSelectIndex(PDBID idActor)
{
    //obuf256 t_data;
    //t_data << "getTalentPageSelectIndex"<<idActor;

    //rkt::obuf resultBuf;
    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
    //if ( resultBuf.good() && resultBuf.size() > 0  )
    //{
    //    ibuffer t_in(resultBuf.data(),resultBuf.size());
    //    int vReturn;
    //    t_in >> vReturn;
    //    if ( t_in.good() ) return vReturn;
    //}

    //return int();
    return (m_pReal ? m_pReal->getTalentPageSelectIndex(idActor) : 0);
}

 int MatchSceneService_Proxy::getRunePageSelectIndex(PDBID idActor)
{
    //obuf256 t_data;
    //t_data << "getRunePageSelectIndex"<<idActor;

    //rkt::obuf resultBuf;
    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
    //if ( resultBuf.good() && resultBuf.size() > 0  )
    //{
    //    ibuffer t_in(resultBuf.data(),resultBuf.size());
    //    int vReturn;
    //    t_in >> vReturn;
    //    if ( t_in.good() ) return vReturn;
    //}

    //return int();
    return (m_pReal ? m_pReal->getRunePageSelectIndex(idActor) : 0);
 }  

 void MatchSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, void* data, size_t len)
 {
	 obuf256 t_data;
	 t_data << serverID << head << len;
	 t_data.push_back(data, len);

     BUILD_MSG_BUFFER( IMatchSceneService::handleServerMsg, t_data);

     m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
 }

 void MatchSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, void* data, size_t len)
 {
	 obuf256 t_data;
	 t_data << client << head << len;
	 t_data.push_back(data, len);
	 BUILD_MSG_BUFFER(IMatchSceneService::handleClientMsg, t_data);

	 m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
 }

 void MatchSceneService_Proxy::onUpdateRoomPlayerClient( PDBID idActor, ClientID client )
 {
     BUILD_MSG_CONTEXT_2( IMatchSceneService::onUpdateRoomPlayerClient,PDBID ,idActor,ClientID, client );
     m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
 }

 void MatchSceneService_Proxy::warEndDestoryInvalidActor(int nWarID)
 {
     BUILD_MSG_CONTEXT_1( IMatchSceneService::warEndDestoryInvalidActor,int,nWarID );
     m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
 }

 void MatchSceneService_Proxy::accordLeaveWar(PDBID idActor)
 {
     BUILD_MSG_CONTEXT_1( IMatchSceneService::accordLeaveWar,PDBID ,idActor );
     m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
 }