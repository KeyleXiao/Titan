#include "stdafx.h"
#include "Stage_LoadScene.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include "ILoginServerProtocol.h"
#include "ActorClient.h"
#include "Stage_Manager.h"
#include "IChatClient.h"
#include "IMatchClient.h"
#include "IMiscManager.h"

Stage_LoadScene::Stage_LoadScene()
{
    m_nMapID = INVALID_MAPID;
    m_idActor = INVALID_PDBID;
}

// 进入状态
void Stage_LoadScene::onEnter( void * context,int len )
{
    // 显示层事件
    IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
    if(pEntityClient)
    {
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_LOAD_GAME,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_MATCH_LOAD_SCENE,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE,this );
    }

    // 消息处理注册
    IMessageDispatch * pMessageDispatch = gClientGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        pMessageDispatch->registerMessageHandler(MSG_MODULEID_MIGRATE, this);
    }

    if(context == NULL)
    {
        return;
    }

    TraceLn("server notify client switchscene...");
    if(len < sizeof(SMsgSwitchScene_SC))
    {
        ErrorLn(__FUNCTION__ << ", notify message format error!");
        return;
    }

    // 删除所有的实体
    IEntityFactory * pEntityFactory = gClientGlobal->getEntityFactory();
    if(pEntityFactory)
    {
        pEntityFactory->destroyAllEntity();
    }    

    SMsgSwitchScene_SC* pMsg = (SMsgSwitchScene_SC*)context;
    m_nMapID = pMsg->nMapID;
    m_idActor = pMsg->idActor;
    gClientGlobal->getMiscManager()->SetContinuePlay(pMsg->isContinuePlay);

    if(pMsg->nSceneID != pMsg->nMapID)
    {
        IRenderViewProxy * pRenderViewProxy = gClientGlobal->getRenderView(); 
        if(pRenderViewProxy)
        {
            pRenderViewProxy->sendControllerCommand(GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE, 0, 0, 0,0 );
        }
        
        IMatchClient* pMatchClient = gClientGlobal->getMatchClient();
        if(pMatchClient)
        {
            pMatchClient->loadSenece(pMsg->nMapID, pMsg->vecBornPos);
        }
    }
    else    // 静态场景
    {
        int nSkinID = pMsg->wSkinID;
        // 预加载皮肤
        if(nSkinID > 0)
        {
            gClientGlobal->getMiscManager()->preloadSkin(&nSkinID, 1);
        }

        // 加载场景
        gClientGlobal->getMiscManager()->loadScene(GVIEWCMD_LOAD_GAME, pMsg->nMapID);
    }

    ActorClient::getInstance().setMapID(m_nMapID);
}

// 离开状态
void Stage_LoadScene::onLeave()
{
    IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
    if(pEntityClient)
    {
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_LOAD_GAME );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MATCH_LOAD_SCENE );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE );
    }

    IMessageDispatch * pMessageDispatch = gClientGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_MIGRATE, this);
    }
}

// 逻辑更新
void Stage_LoadScene::update()
{

}

// 逻辑层处理VIEW命令
bool Stage_LoadScene::onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam )
{
    switch ( eventID )
    {
    case GVIEWCMD_LOAD_GAME:
    case GVIEWCMD_MATCH_LOAD_SCENE:
        {
            TraceLn("client load scene ok...");
            // 通知加载完成
            sendSceneResouceLoadProgress(100);
        }
        break;
    case GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE:
        {
            // 通知场景服加载进度
            sendSceneResouceLoadProgress(nParam);
        }
        break;  
    
    default:
        break;
    }

    return true;
}

// 发送场景加载进度
void Stage_LoadScene::sendSceneResouceLoadProgress(int nProgress)
{
    SNetMsgHead head;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.byKeyModule    = MSG_MODULEID_MIGRATE;
    head.byKeyAction    = MSG_SCENE_LOADING_PROGRESS_CS;

    SMsgSceneLoadingProgress_CS msg;
    msg.nProgress = nProgress;
    msg.lPing = gClientGlobal->getNetLatency();
    msg.idActor = m_idActor;

    obuf256 ob;
    ob << head << msg;
    net_senddata( ob.data(),ob.size() );
}

void Stage_LoadScene::onMessage(SNetMsgHead* head, void* data, size_t len)
{
    switch(MAKEWORD(head->byKeyModule, head->byKeyAction))
    {
    case MAKEWORD(MSG_MODULEID_MIGRATE, MSG_SCENE_LOADING_PROGRESS_SC):
        {
            if(len != sizeof(SMsgSceneLoadingProgress_SC))
            {
                ErrorLn(__FUNCTION__ << ": MSG_SCENE_LOADING_PROGRESS_SC msg format error!");
                return;
            }

            SMsgSceneLoadingProgress_SC* pMsg = (SMsgSceneLoadingProgress_SC*)data;

            // 更新指定玩家的加载进度
            cmd_load_scene_rate cmdData;
            cmdData.idActor = pMsg->idActor;
            cmdData.nRate = pMsg->nProgress;
            cmdData.nPing = pMsg->lPing;

            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE, 0, nullptr, &cmdData, sizeof(cmdData));
        }
        break;
    default:
        {
            WarningLn(__FUNCTION__ << ", unhandled msg,module=" << head->byKeyModule << ", action=" << head->byKeyAction);
        }
        break;
    }
}
