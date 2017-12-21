#include "stdafx.h"
#include "Stage_Game.h"
#include "net/net.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "IRenderView.h"
#include "NetMessageDef.h"
#include "MatchDef.h"
#include "ClientGlobal.h"
#include "ILoginServerProtocol.h"
#include "Stage_Manager.h"
#include "ActorClient.h"
#include "MatchDef.h"
#include "GameVoiceClient.h"

Stage_Game::Stage_Game()
    : m_pPathFind(0)
{

    
}

Stage_Game::~Stage_Game()
{

}

void Stage_Game::onEnter( void * context,int len )
{
    TraceLn(__FUNCTION__);
    // 连接语音服务器(语音登录需要一些信息 要等角色创建好了才能取到)
    // 不会重复建立连接 ConnectToServer有判断
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

    IMessageDispatch * pMessageDispatch = gClientGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        pMessageDispatch->registerMessageHandler( MSG_MODULEID_GAME, this );
    }

    IPathFindManager * pPathFindManager = gClientGlobal->getPathFindManager();
    if(pPathFindManager)
    {
        m_pPathFind = pPathFindManager->createServiceInterface( ActorClient::getInstance().getMapID(),0 );
    }   

    
}


void Stage_Game::update()
{

}

void Stage_Game::onLeave()
{
    TraceLn(__FUNCTION__);
    gClientGlobal->getMessageDispatch()->unregisterMessageHandler( MSG_MODULEID_GAME,this );
    gClientGlobal->getPathFindManager()->releaseServiceInterface( m_pPathFind );
}

bool Stage_Game::onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam )
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

    default:
        ErrorLn( "Stage_Game::onViewEvent unhandled eventID=" << eventID );
        return false;
    }

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void Stage_Game::OnConnected( IConnection * conn )
{
     Stage_Base::OnConnected(conn);
}

void Stage_Game::OnDisconnected( IConnection * conn,DWORD reason )
{
    Stage_Base::OnDisconnected(conn, reason);
}

void Stage_Game::OnError( IConnection * conn,DWORD dwErrorCode )
{
    Stage_Base::OnError(conn, dwErrorCode);
}

void Stage_Game::onMessage(SNetMsgHead* head, void* data, size_t len)
{
	if (head == NULL || data == NULL)
	{
		return;
	}

    switch ( head->byKeyAction )
    {

    default:
        {
            ErrorLn(__FUNCTION__": not handle head->byKeyModule ="<<head->byKeyModule<<" head->byKeyAction ="<<head->byKeyAction);
        }
        break;
        
    }
}

bool Stage_Game::findPath(void* pCmd)
{
    cmd_map_findpath& data = *(cmd_map_findpath*)pCmd; 

    Vector3 startPos(data.start);
    Vector3 endPos(data.end);

    vector<Vector3> paths;

    if ( m_pPathFind && m_pPathFind->GetPath(startPos,endPos,paths,CAMP_MASK(gClientGlobal->getHero()->getIntProperty(PROPERTY_CAMP))) )
    {
        if ( paths.size()>MAX_MAP_PATH_NUM )
        {
            ErrorLn( "Stage_Game::findPath the result size is too big! paths.size()="<<paths.size() );
            return false;
        }

        data.pathcnt = (int)paths.size();
        for ( int i=0;i<data.pathcnt;++i )
        {
            memcpy(&data.path[i*3], &paths[i], sizeof(Vector3));
        }
        return true;
    }
    return false;
}
