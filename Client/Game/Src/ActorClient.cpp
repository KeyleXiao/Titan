/*******************************************************************
** 文件名:	ActorClient.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	8/26/2015
** 版  本:	1.0
** 描  述:	账号下的角色信息管理类

********************************************************************/

#include "stdafx.h"
#include "ActorClient.h"
#include "ILoginServerProtocol.h"
#include "IClientGlobal.h"

ActorClient::ActorClient()
    : m_mapid( 0 )
    , m_dwUserID(0)
{
}

ActorClient::~ActorClient()
{
}

void ActorClient::setMapID(int mapid)
{
    m_mapid = mapid;
}

int ActorClient::getMapID()
{
    return m_mapid;
}

// 取得首个可用的英雄ID
int ActorClient::getFirstHeroID()
{
    for ( size_t i=0;i<m_curPlayerClient.m_heros.size();++i )
    {
        if ( m_curPlayerClient.m_heros[i].dwFlags!=0 )
            return m_curPlayerClient.m_heros[i].nHeroID;
    }

    return 0;
}

int ActorClient::getFirstActorID()
{
    if(m_players.size() == 0)
    {
        return INVALID_PDBID;
    }

    return m_players[0].m_info.dwActorID;
}

SMsgLoginServer_UpdateActor::ActorInfo & ActorClient::getCurPlayer()
{
    return m_curPlayerClient.m_info;
}

bool ActorClient::selectPlayer( int pdbid )
{
    for ( vector< PlayerClient >::iterator itr=m_players.begin();itr!=m_players.end();++itr )
    {
        if ( itr->m_info.dwActorID==pdbid )
        {
            getCurPlayer() = itr->m_info;
            return true;
        }
    }
    return false;
}

gameview_login & ActorClient::getLoginInfo()
{
    return m_loginInfo;
}

void ActorClient::updatePlayers( WORD count, SMsgLoginServer_UpdateActor::ActorInfo  * players,int reason )
{
    m_players.clear();
    for ( WORD i=0;i<count;++i )
    {
        PlayerClient player;
        player.m_info = players[i];
        m_players.push_back( player );
    }

    // 登录成功
    gamelogic_updateplayers ll;
    ll.playercnt = count;
    for ( int i=0;i<ll.playercnt;++i )
    {
        ll.players[i].pid = players[i].dwActorID;
        ll.players[i].heroid = players[i].nHeroID;
        ll.players[i].skinid = players[i].nSkinID;
        strcpy( ll.players[i].name,players[i].szActorName );
    }

    int cmdid = getResultCmdID( reason );

    gClientGlobal->getRenderView()->sendControllerCommand( cmdid,0,"Load_wan",&ll,sizeof(ll) );
}

// 设置账号信息
void ActorClient::setUserInfo(DWORD dwUserID, char* szUserName)
{
    sstrcpyn(m_loginInfo.account, szUserName, sizeof(m_loginInfo.account));
    m_dwUserID = dwUserID;
}
//void ActorClient::updateSelectActor( void * data,size_t len )
//{
//	return;
//    if ( data==NULL || len<sizeof(SPlayerDatabasesInfo) )
//        return;
//
//    SPlayerDatabasesInfo * pSelectActor = (SPlayerDatabasesInfo *)data;
//    if ( len<=sizeof(SPlayerDatabasesInfo)+sizeof(SHeroInfo)*pSelectActor->herocnt )
//    {
//        return;
//    }
//
//    if ( getCurPlayer().dwActorID!=pSelectActor->player.dwActorID )
//    {
//        WarningLn( __FUNCTION__<<" client pdbid="<<getCurPlayer().dwActorID<<", but update pdbid="<<pSelectActor->player.dwActorID );
//    }
//
//    // 直接使用此数据进行更新
//    getCurPlayer() = pSelectActor->player;
//
//
//	int* pRuneStoreCount =(int*)(pSelectActor+1);
//	int nRuneStoreCount = (*pRuneStoreCount)/sizeof(EntityHeroRuneStore);
//	char* pHead = (char*)(pRuneStoreCount+1);
//	for(int i = 0; i < nRuneStoreCount; ++i)
//	{
//		EntityHeroRuneStore* node = (EntityHeroRuneStore*)(pHead);
//		++pHead;
//	}
//
//    m_curPlayerClient.m_heros.clear();
//    for ( WORD i=0;i<pSelectActor->herocnt;++i )
//    {
//        SHeroInfo & hero = ((SHeroInfo*)(pHead))[i];
//        m_curPlayerClient.m_heros.push_back( hero );
//    }
//}

void ActorClient::getPlayersError ( int error,const char * tips,int reason )
{
    int cmdid = getResultCmdID( reason );

    // 登录失败，报错
    gClientGlobal->getRenderView()->sendControllerCommand( cmdid,error,tips,0,0 );
}

int ActorClient::getResultCmdID( int reason )
{
    int cmdid = 0;
    switch ( reason )
    {
    case LOGIN_UPDATE_REASON_LOGIN:
        { cmdid = GVIEWCMD_USER_LOGIN_RESULT; } break;

    case LOGIN_UPDATE_REASON_CREATE:
        { cmdid = GVIEWCMD_USER_CREATE_PLAYER_RESULT; } break;

    case LOGIN_UPDATE_REASON_DELETE:
        { cmdid = GVIEWCMD_USER_DELETE_PLAYER_RESULT; } break;

    default:
        break;
    }
    return cmdid;
}
SHeroInfo * ActorClient::getSystemHeroCount(WORD &systemHeroCount)
{
    systemHeroCount=(WORD)VOCATION_MAX;
    // 目前先置空
    return NULL;
}