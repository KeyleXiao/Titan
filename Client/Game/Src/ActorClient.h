/*******************************************************************
** 文件名:	ActorClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	8/26/2015
** 版  本:	1.0
** 描  述:	账号下的角色信息管理类
**          由于全局只需一个ActorClient对象，所以声明为单例

**          1. 管理账号信息，包括账号名，登录信息，
            2. 管理当前账号下有哪些角色信息
            3. 管理当前玩家选择的角色信息
********************************************************************/

#include "GameDef.h"
#include "IGameDatabaseDef.h"
#include "GameViewDef.h"
#include <vector>
#include "ILoginServerProtocol.h"

using std::vector;

#pragma once

class PlayerClient
{
public:
    SMsgLoginServer_UpdateActor::ActorInfo    m_info;
    vector< SHeroInfo>  m_heros;
};

class ActorClient : public Singleton<ActorClient>
{
public:
    ActorClient();

    ~ActorClient();

    void setMapID(int mapid);

    int getMapID();

    int getFirstHeroID();

    int getFirstActorID();

    SMsgLoginServer_UpdateActor::ActorInfo & getCurPlayer();

    bool selectPlayer( int pdbid );

    gameview_login & getLoginInfo();

    void updatePlayers( WORD count, SMsgLoginServer_UpdateActor::ActorInfo * players,int reason );

    // 设置账号信息
    void setUserInfo(DWORD dwUserID, char* szUserName);

    void getPlayersError ( int error,const char * tips,int reason );

    SHeroInfo * getSystemHeroCount(WORD &systemHeroCount);
private:
    int getResultCmdID( int reason );

private:
    int                     m_mapid;
    DWORD                   m_dwUserID;
    gameview_login          m_loginInfo;
    PlayerClient            m_curPlayerClient;
    vector< PlayerClient >  m_players;
};
