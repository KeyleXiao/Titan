#include "stdafx.h"
#include "Stage_MatchBattle.h"
#include "net/net.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "IRenderView.h"
#include "NetMessageDef.h"
#include "MatchDef.h"
#include "ClientGlobal.h"
#include "ILoginServerProtocol.h"
#include "ActorClient.h"
#include "GameVoiceClient.h"

void Stage_MatchBattle::onEnter( void * context,int len )
{
	// 通知进入游戏状态
	//gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOAD_GAME,0,0,0,0 );

	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_USER_EXITGAME,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_USER_EXIT_WAR,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_USER_BACK_GAME, this);

    // 连接语音服务器
    IVoiceEngine* pVoiceEngine = CGameVoiceClient::getInstance().GetVoiceEngine();
    if(pVoiceEngine)
    {
        IVoiceClient* pVoiceClient = pVoiceEngine->GetVoiceClient();
        if(pVoiceClient)
        {
            // 语音网关ip 端口 读配置 todo.
            gameview_login& login = ActorClient::getInstance().getLoginInfo();
            pVoiceClient->ConnectToServer(login.svrip, 12900);
        }
    }

    m_pPathFind = gClientGlobal->getPathFindManager()->createServiceInterface(ActorClient::getInstance().getMapID(), 0); 
}

void Stage_MatchBattle::update()
{

}

void Stage_MatchBattle::onLeave()
{
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_USER_EXITGAME );
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_USER_EXIT_WAR );
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_USER_BACK_GAME);

    gClientGlobal->getPathFindManager()->releaseServiceInterface( m_pPathFind );
}

bool Stage_MatchBattle::onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam )
{
    switch ( eventID )
    {
    case GVIEWCMD_PROGRESSING:      // 进入到读条阶段
        {
            if ( nParam<100 )
                gClientGlobal->getMessageDispatch()->choke(true);
            else
                gClientGlobal->getMessageDispatch()->choke(false);
        }
        break;

    case GVIEWCMD_USER_EXITGAME:
        {
            onViewSendExitGameMsg();
        }
        break;

    case GVIEWCMD_USER_EXIT_WAR:
        {
            onViewSendExitWarMsg();
        }
        break;
    case GVIEWCMD_USER_BACK_GAME:
        {
            onViewSendBackGameMsg();
        }
        break;


    default:
        return false;
    }

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void Stage_MatchBattle::OnConnected( IConnection * conn )
{
     Stage_Base::OnConnected(conn);
}

void Stage_MatchBattle::OnDisconnected( IConnection * conn,DWORD reason )
{
    Stage_Base::OnDisconnected(conn, reason);
}

void Stage_MatchBattle::OnError( IConnection * conn,DWORD dwErrorCode )
{
    Stage_Base::OnError(conn, dwErrorCode);
}



// 离开
void Stage_MatchBattle::onViewSendExitGameMsg()
{
	obuf256 ob;
	CS_ExitGameMsg data;
	memset(&data,0,sizeof(CS_ExitGameMsg));

	data.uid = 0;

	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_SCENE;
	head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_GAME;
	head.byKeyAction    = CS_MSG_USER_EXITGAME;

	ob << head << data;

    net_senddata( ob.data(), ob.size() );
}

// 离开战场
void Stage_MatchBattle::onViewSendExitWarMsg()
{
    gClientGlobal->getEntityFactory()->destroyAllEntity();

    // 切到登录状态
    gClientGlobal->gotoState(ESS_LOGIN);

    // 关闭网络连接
    net_release();

    return;

}

bool Stage_MatchBattle::findPath(void* pCmd)
{
    if(pCmd == NULL)
    {
        return false;
    }
    cmd_map_findpath& data = *(cmd_map_findpath*)pCmd;

    Vector3 startPos(data.start);
    Vector3 endPos(data.end);

    vector<Vector3> paths;

    if ( m_pPathFind && m_pPathFind->GetPath(startPos,endPos,paths, CAMP_MASK(gClientGlobal->getHero()->getIntProperty(PROPERTY_CAMP))) )
    {
        if ( paths.size()>MAX_MAP_PATH_NUM )
        {
            MATCH_ERRORLN( "Stage_MatchBattle::findPath the result size is too big! paths.size()="<<paths.size() );
            return false;
        }

        data.pathcnt = paths.size(); 
        for ( int i=0;i<data.pathcnt;++i )
        {
            memcpy(&data.path[i*3], &paths[i], sizeof(Vector3));
        }
        return true;
    }
    return false;
}

void Stage_MatchBattle::onViewSendBackGameMsg()
{
    obuf256 ob;
    CS_ExitGameMsg data;
    memset(&data,0,sizeof(CS_ExitGameMsg));

    data.uid = 0;

    SNetMsgHead MsgHead;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_EXIT_WAR;						// 子消息代码  EMRoomMessageCode

    ob << MsgHead<<outData;

    net_senddata(ob.data(), ob.size());
}

