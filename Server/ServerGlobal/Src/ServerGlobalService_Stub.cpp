/*******************************************************************
** 文件名:	ServerGlobalService_Stub.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "IServerGlobal.h"
#include "ServerGlobalService.h"
#include "ServerGlobalService_Stub.h"


ServerGlobalService_Stub::ServerGlobalService_Stub(ServerGlobalService * pReal)
	: m_real_service(pReal)
{
}

ServerGlobalService_Stub::~ServerGlobalService_Stub()
{

}


////////////////// IServiceStub ////////////////////////////////////////////////////////

// 启动
// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
bool ServerGlobalService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	gServerGlobal->setServerGlobalService(pContainer->get_id());
	
	m_proxy.m_pContainer = pContainer;
	m_real_service->on_start(data, len);
	return true;
}

// 停止
bool ServerGlobalService_Stub::on_stop()
{
	bool bResult = m_real_service->on_stop();
    m_proxy.m_pContainer = SERVICE_PTR(0);
    return bResult;
}

// 处理消息
bool ServerGlobalService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
	// 以下自动生成的消息解包，你需要检查

	if ( TEST_SAME_FUNCTION(IServerGlobalService::create) )
	{
		bool ret= m_real_service->create();
		if ( resultBuf ) *resultBuf<<ret;
		return true;
	}

	if ( TEST_SAME_FUNCTION(IServerGlobalService::release) )
	{
		m_real_service->release();
		return true;
	}

    // 消息分发器
    if ( TEST_SAME_FUNCTION(IServerGlobalService::getMessageDispatch) )
    {
        if ( resultBuf ) *resultBuf << m_real_service->getMessageDispatch();
        return true;
    }


	// 获取框架接口
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getFramework) )
	{
		if ( resultBuf ) *resultBuf << m_real_service->getFramework();
		return true;
	}

	// 获取配置中心接口
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getSchemeCenter) )
	{
		if ( resultBuf ) *resultBuf<< m_real_service->getSchemeCenter();
		return true;
	}

	// 取得场景管理器
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getSceneManager) )
	{
		if ( resultBuf ) *resultBuf<<m_real_service->getSceneManager();
		return true;
	}
    if ( TEST_SAME_FUNCTION(IServerGlobalService::getActorMgr) )
    {
        if ( resultBuf ) *resultBuf <<m_real_service->getActorMgr();
        return true;
    }

	// 取得配置服务
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getSchemeEngine) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getSchemeEngine();
		return true;
	}

	// 取得条件工厂
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getConditionFactory) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getConditionFactory();
		return true;
	}

	// 取得效果工厂
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getEffectFactory) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getEffectFactory();
		return true;
	}

	// 取得技能工厂
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getSpellFactory) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getSpellFactory();
		return true;
	}

	// 取得冷却服务器
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getFreezeServer) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getFreezeServer();
		return true;
	}

	// 取得Buff服务
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getBuffServer) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getBuffServer();
		return true;
	}

	// 取得伤害服务器
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getDamageServer) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getDamageServer();
		return true;
	}

	// 取得tank服务器
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getTankServer) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getTankServer();
		return true;
	}

	// 取得仇恨服务器
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getEnmityServer) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getEnmityServer();
		return true;
	}

    // 取得测试服务器
    if ( TEST_SAME_FUNCTION(IServerGlobalService::getTestServer) )
    {
		if ( resultBuf ) *resultBuf <<m_real_service->getTestServer();
        return true;
    }

	// 取得寻路管理器
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getPathFindManager) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getPathFindManager();
		return true;
	}

	// 比赛大厅场景服
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getMatchScene) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getMatchScene();
		return true;
	}

	if ( TEST_SAME_FUNCTION(IServerGlobalService::getWarManager) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getWarManager();
		return true;
	}

	// 取得聊天服务器
	if ( TEST_SAME_FUNCTION(IServerGlobalService::getChatScene) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getChatScene();
		return true;
	}

    if ( TEST_SAME_FUNCTION(IServerGlobalService::getTeamScene) )
    {
		if ( resultBuf ) *resultBuf <<m_real_service->getTeamScene();
        return true;
    }

	if ( TEST_SAME_FUNCTION(IServerGlobalService::getShareReceiver) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getShareReceiver();
		return true;
	}

	if ( TEST_SAME_FUNCTION(IServerGlobalService::getShareSceneSvrSource) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getShareSceneSvrSource();
		return true;
	}

    // 取得排行榜服务器
    if ( TEST_SAME_FUNCTION(IServerGlobalService::getOrderScene) )
    {
		if ( resultBuf ) *resultBuf <<m_real_service->getOrderScene();
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::getStaticWarScene) )
    {
		if ( resultBuf ) *resultBuf <<m_real_service->getStaticWarScene();
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::getLegendCupScene) )
    {
        if ( resultBuf ) *resultBuf <<m_real_service->getLegendCupScene();
        return true;
    }

	if ( TEST_SAME_FUNCTION(IServerGlobalService::getAntiAddictServer) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getAntiAddictServer();
		return true;
	}

    if ( TEST_SAME_FUNCTION(IServerGlobalService::getResourceManager) )
    {
        *resultBuf << m_real_service->getResourceManager();
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::getMiscManager) )
    {
        *resultBuf << m_real_service->getMiscManager();
        return true;
    }

    
    if ( TEST_SAME_FUNCTION(IServerGlobalService::getAIFactory) )
    {
		if ( resultBuf ) *resultBuf <<m_real_service->getAIFactory();
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::getKinScene) )
    {
		if ( resultBuf ) *resultBuf <<m_real_service->getKinScene();
        return true;
	}

	if ( TEST_SAME_FUNCTION(IServerGlobalService::getClanScene) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getClanScene();
		return true;
	}

	if ( TEST_SAME_FUNCTION(IServerGlobalService::getMentorshipScene) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getMentorshipScene();
		return true;
	}

	if ( TEST_SAME_FUNCTION(IServerGlobalService::getMailScene) )
	{
		if ( resultBuf ) *resultBuf <<m_real_service->getMailScene();
		return true;
	}

    if ( TEST_SAME_FUNCTION(IServerGlobalService::getHeightFieldMgr) )
    {
        if ( resultBuf ) *resultBuf <<m_real_service->getHeightFieldMgr();
        return true;
    }
		
	//////////////////////////////////////////////////////////////////////////
    if ( TEST_SAME_FUNCTION(IServerGlobalService::requestHandleServerControl) )
    {
        DWORD dwState;
        GET_MSG_PARAM_1(DWORD ,dwState);
        m_real_service->requestHandleServerControl(dwState);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::requestManagerStart) )
    {
        DWORD dwStartFlag; DWORD dwUserData;
        GET_MSG_PARAM_2(DWORD ,dwStartFlag, DWORD, dwUserData);
        if ( resultBuf ) *resultBuf << m_real_service->requestManagerStart(dwStartFlag, dwUserData);
        return true;
    }
	///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
    if ( TEST_SAME_FUNCTION(IServerGlobalService::handleServerListUpdated) )
    {
        m_real_service->handleServerListUpdated();
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::handleServerInfoUpdated) )
    {
		rkt::ibuffer in(pMsg->context,pMsg->context_len);
        DWORD ServerID; BYTE nState; size_t nInfolen;
        in >>ServerID>> nState >> nInfolen;
        m_real_service->handleServerInfoUpdated(ServerID, nState, in.current(), nInfolen);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::handlePostMessage) )
    {
		rkt::ibuffer in(pMsg->context,pMsg->context_len);
        DWORD server;WORD wDataLen;
        in >>server>> wDataLen;
        m_real_service->handlePostMessage(server, in.current(), wDataLen);
        return true;
    }
	///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
    if ( TEST_SAME_FUNCTION(IServerGlobalService::handleClientIncoming) )
    {
        ClientID client;
        GET_MSG_PARAM_1(ClientID,client);
        m_real_service->handleClientIncoming(client);
        return true;
    }

	if ( TEST_SAME_FUNCTION(IServerGlobalService::handleGateWayReceivedData) )
	{
		rkt::ibuffer in(pMsg->context,pMsg->context_len);	
		ClientID client;size_t len;
		in >>client>> len;
        m_real_service->handleGateWayReceivedData(client, in.current(), len);
		return true;
	}

	if ( TEST_SAME_FUNCTION(IServerGlobalService::handleGateWayClientDisconnect) )
	{
		ClientID client;
		DWORD dwReason;
		GET_MSG_PARAM_2(ClientID,client,DWORD,dwReason);
		m_real_service->handleGateWayClientDisconnect(client, dwReason);
		return true;
	}


    ///////ManagerConnector///////////////////////////////////////////////////////////////////
    if ( TEST_SAME_FUNCTION(IServerGlobalService::handleManagerSetGameWorldID) )
    {
        m_real_service->handleManagerSetGameWorldID();
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::handleManagerResourceData) )
    {
        rkt::ibuffer in(pMsg->context,pMsg->context_len);
        DWORD dwDataLen;
        in >>dwDataLen;
        m_real_service->handleManagerResourceData(in.current(), dwDataLen);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IServerGlobalService::handleManagerResourceResult) )
    {
        rkt::ibuffer in(pMsg->context,pMsg->context_len);
        DWORD dwDataLen;
        in >>dwDataLen;
        m_real_service->handleManagerResourceResult(in.current(), dwDataLen);
        return true;
    }


	//////////////////////////////////////////////////////////////////////////


	ErrorLn("ServerGlobalService_Stub::handle_message() has a function not handle, func=");

	return false;
}

// 返回代理对象即可
void * ServerGlobalService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

// 销毁
void ServerGlobalService_Stub::release()
{
	if ( m_real_service )
	{
		m_real_service->release();
		m_real_service = 0;
	}

	delete this;
}


//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateServerGlobalService()
{
	return new ServerGlobalService_Stub(new ServerGlobalService);
}