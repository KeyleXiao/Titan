/*******************************************************************
** 文件名:	ServerGlobalService_Proxy.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "ServerGlobalService_Proxy.h"

ServerGlobalService_Proxy::ServerGlobalService_Proxy()
	: m_pContainer(0)
{
}

ServerGlobalService_Proxy::~ServerGlobalService_Proxy()
{

}


//////////////////////////////////////////////////////////////////////////
// 创建
bool ServerGlobalService_Proxy::create()
{
    if(!m_pContainer)   return false;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::create);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(bool);

	return false;
}

void ServerGlobalService_Proxy::release()
{
    if(!m_pContainer)
        return;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::release);
	m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}

// 消息分发器
IMessageDispatch* ServerGlobalService_Proxy::getMessageDispatch()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getMessageDispatch);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IMessageDispatch*);

	return 0;
}

// 获取框架接口
IFramework * ServerGlobalService_Proxy::getFramework()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getFramework);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IFramework *);
	return 0;
}

/// 获取配置中心接口
ISchemeCenter * ServerGlobalService_Proxy::getSchemeCenter()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getSchemeCenter);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ISchemeCenter *);
	return 0;
}

/// 取得配置服务
ISchemeEngine * ServerGlobalService_Proxy::getSchemeEngine()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getSchemeEngine);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ISchemeEngine *);
	return 0;
}

ISceneManager * ServerGlobalService_Proxy::getSceneManager()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getSceneManager);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ISceneManager *);
	return 0;
}

IActorMgr* ServerGlobalService_Proxy::getActorMgr()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getActorMgr);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IActorMgr *);
	return 0;
}

IConditionFactory * ServerGlobalService_Proxy::getConditionFactory()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getConditionFactory);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IConditionFactory *);
	return 0;
}

IEffectFactory * ServerGlobalService_Proxy::getEffectFactory()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getEffectFactory);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IEffectFactory *);
	return 0;
}

ISpellFactory * ServerGlobalService_Proxy::getSpellFactory()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getSpellFactory);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ISpellFactory *);
	return 0;
}

// 取得冷却服务器
IFreezeServer *ServerGlobalService_Proxy::getFreezeServer()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getFreezeServer);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IFreezeServer *);
	return 0;
}

// 取得Buff服务
IBuffServer *ServerGlobalService_Proxy::getBuffServer()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getBuffServer);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IBuffServer *);
	return 0;
}

// 取得伤害服务器
IDamageServer *ServerGlobalService_Proxy::getDamageServer()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getDamageServer);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IDamageServer *);
	return 0;
}

ITankServer *ServerGlobalService_Proxy::getTankServer()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getTankServer);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ITankServer *);
	return 0;
}

// 取得仇恨服务器
IEnmityServer *ServerGlobalService_Proxy::getEnmityServer()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getEnmityServer);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IEnmityServer *);
	return 0;
}

// 取得测试服务器
ITestServer *ServerGlobalService_Proxy::getTestServer()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getTestServer);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ITestServer *);
	return 0;
}

IPathFindManager * ServerGlobalService_Proxy::getPathFindManager()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getPathFindManager);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IPathFindManager *);
	return 0;
}

IMatchScene* ServerGlobalService_Proxy::getMatchScene()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getMatchScene);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IMatchScene *);
	return 0;
}

IChatScene*	ServerGlobalService_Proxy::getChatScene()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getChatScene);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IChatScene *);
	return 0;
}

ITeamScene*	ServerGlobalService_Proxy::getTeamScene()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getTeamScene);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ITeamScene *);
	return 0;
}

IWarManager * ServerGlobalService_Proxy::getWarManager()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getWarManager);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IWarManager *);
	return 0;
}

IShareReceiver*	ServerGlobalService_Proxy::getShareReceiver()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getShareReceiver);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IShareReceiver *);
	return 0;
}
IShareSceneSvrSource* ServerGlobalService_Proxy::getShareSceneSvrSource()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getShareSceneSvrSource);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IShareSceneSvrSource *);
	return 0;
}

IOrderScene*	ServerGlobalService_Proxy::getOrderScene()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getOrderScene);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IOrderScene *);
	return 0;
}

IStaticWarScene*	ServerGlobalService_Proxy::getStaticWarScene()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getStaticWarScene);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IStaticWarScene *);
	return 0;
}

ILegendCupScene*	ServerGlobalService_Proxy::getLegendCupScene()
{
    if(!m_pContainer)
        return 0;

    BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getLegendCupScene);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ILegendCupScene *);
    return 0;
}



IClanScene* ServerGlobalService_Proxy::getClanScene()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getClanScene);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IClanScene *);
	return 0;
}

IKinScene* ServerGlobalService_Proxy::getKinScene()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getKinScene);
    rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IKinScene *);
	return 0;
}

IAIFactory* ServerGlobalService_Proxy::getAIFactory()
{
    if(!m_pContainer)
        return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getAIFactory);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IAIFactory *);
	return 0;
}

IAntiAddictServer*	ServerGlobalService_Proxy::getAntiAddictServer()
{
	if(!m_pContainer)
		return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getAntiAddictServer);
	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IAntiAddictServer *);
	return 0;
}


// 取资源管理对象
IResourceManager* ServerGlobalService_Proxy::getResourceManager()
{
    if (!m_pContainer)
        return 0;

    BUILD_MSG_CONTEXT_VOID( IServerGlobalService::getResourceManager );

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IResourceManager*);

    return 0;
}

// 取杂项管理对象
IMiscManager* ServerGlobalService_Proxy::getMiscManager()
{
    if (!m_pContainer)
        return 0;

    BUILD_MSG_CONTEXT_VOID( IServerGlobalService::getMiscManager );

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IMiscManager*);

    return 0;
}

IMentorshipScene* ServerGlobalService_Proxy::getMentorshipScene()
{
	if(!m_pContainer)
		return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getMentorshipScene);
	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IMentorshipScene *);
	return 0;
}

IMailScene* ServerGlobalService_Proxy::getMailScene()
{
	if(!m_pContainer)
		return 0;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getMailScene);
	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IMailScene *);
	return 0;
}

HeightFieldMgr* ServerGlobalService_Proxy::getHeightFieldMgr()
{
    if(!m_pContainer)
        return 0;

    BUILD_MSG_CONTEXT_VOID(IServerGlobalService::getHeightFieldMgr);
    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(HeightFieldMgr *);
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// 请求跨Service处理服务控制，有些数据需要在Service上才能处理
void ServerGlobalService_Proxy::requestHandleServerControl(DWORD dwState)
{
    if(!m_pContainer)
        return;

	BUILD_MSG_CONTEXT_1(IServerGlobalService::requestHandleServerControl,DWORD ,dwState);
	m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}


// 请求管理器启动
bool ServerGlobalService_Proxy::requestManagerStart(DWORD dwStartFlag, DWORD dwUserData)
{
    if (!m_pContainer)
        return 0;

    BUILD_MSG_CONTEXT_2( IServerGlobalService::requestManagerStart,DWORD,dwStartFlag, DWORD, dwUserData);
	rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(bool);
    return false;
}

///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void  ServerGlobalService_Proxy::handleServerListUpdated()
{
    if(!m_pContainer)
        return;

	BUILD_MSG_CONTEXT_VOID(IServerGlobalService::handleServerListUpdated);
	m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}

/**
@name         : 通知服务器信息更新
@brief        : 例如服务器人数变化等等
*/
void ServerGlobalService_Proxy::handleServerInfoUpdated(DWORD ServerID, BYTE nState, void* pInfo, size_t nInfolen)
{
    if(!m_pContainer)
        return;

    obuf256 t_data;
    t_data <<ServerID<<nState<<nInfolen;
    t_data.push_back(pInfo,nInfolen);
    
	BUILD_MSG_BUFFER( IServerGlobalService::handleServerInfoUpdated,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}

/**
@name         : 处理其他服务器通过中心服务器转发的消息
@brief        : 具体内容不清楚
@param server : 该消息是哪个服务器发过来的
@param pData  : 消息内容
@param wDataLen:消息长度
*/
void ServerGlobalService_Proxy::handlePostMessage(DWORD server,const char * pData,WORD wDataLen)
{
    if(!m_pContainer)
        return;

    obuf256 t_data;
    t_data <<server<<wDataLen;
    t_data.push_back(pData,wDataLen);

	BUILD_MSG_BUFFER( IServerGlobalService::handlePostMessage,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}

///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
// 网关服务器通知游戏服务器有一个新的客户端连进来
void ServerGlobalService_Proxy::handleClientIncoming(ClientID client)
{
    if(!m_pContainer)
        return;

	BUILD_MSG_CONTEXT_1( IServerGlobalService::handleClientIncoming,ClientID, client);
    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}

// 网关服务器通知游戏服务器有新的数据包到达
void ServerGlobalService_Proxy::handleGateWayReceivedData(ClientID client, void* data, size_t len)
{
    if(!m_pContainer)
        return;

    obuf256 t_data;
    t_data <<client<<len;
    t_data.push_back(data,len);

	BUILD_MSG_BUFFER( IServerGlobalService::handleGateWayReceivedData,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}

// 网关服务器通知游戏服务器有连接断开
void ServerGlobalService_Proxy::handleGateWayClientDisconnect(ClientID client,DWORD dwReason)
{
    if(!m_pContainer)
        return;

	BUILD_MSG_CONTEXT_2( IServerGlobalService::handleGateWayClientDisconnect,ClientID, client,DWORD ,dwReason);
    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}


///////ManagerConnector///////////////////////////////////////////////////////////////////
// 收到设置游戏ID
void ServerGlobalService_Proxy::handleManagerSetGameWorldID()
{
    if (!m_pContainer)
        return;

    BUILD_MSG_CONTEXT_VOID( IServerGlobalService::handleManagerSetGameWorldID );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 收到管理器下发的资源数据
void ServerGlobalService_Proxy::handleManagerResourceData(LPVOID pData,DWORD dwDataLen)
{
    if (!m_pContainer)
        return;

    if(dwDataLen == 0)
        return;

    obuf256 t_data;
    t_data <<dwDataLen;
    t_data.push_back(pData,dwDataLen);

    BUILD_MSG_BUFFER( IServerGlobalService::handleManagerResourceData,t_data );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}


// 收到管理器下发的资源数据结果
void  ServerGlobalService_Proxy::handleManagerResourceResult(LPVOID pData,DWORD dwDataLen)
{
    if (!m_pContainer)
        return;

    if(dwDataLen == 0)
        return;

    obuf256 t_data;
    t_data <<dwDataLen;
    t_data.push_back(pData,dwDataLen);

    BUILD_MSG_BUFFER( IServerGlobalService::handleManagerResourceResult,t_data );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}
