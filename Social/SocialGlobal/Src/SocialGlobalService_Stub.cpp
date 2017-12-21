/*******************************************************************
** 文件名:	SocialGlobalService_Stub.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "ISocialGlobal.h"
#include "SocialGlobalService.h"
#include "SocialGlobalService_Stub.h"


SocialGlobalService_Stub::SocialGlobalService_Stub(SocialGlobalService * pReal)
	: m_real_service(pReal)
{
}

SocialGlobalService_Stub::~SocialGlobalService_Stub()
{

}


////////////////// IServiceStub ////////////////////////////////////////////////////////

// 启动
// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
bool SocialGlobalService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	gSocialGlobal->setSocialGlobalService(pContainer->get_id());
	
	m_proxy.m_pContainer = pContainer;
	m_real_service->on_start(data, len);
	return true;
}

// 停止
bool SocialGlobalService_Stub::on_stop()
{
	return m_real_service->on_stop();
}

// 处理消息
bool SocialGlobalService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{	
	// 以下自动生成的消息解包，你需要检查

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::create) )
	{
		bool ret= m_real_service->create();
		if ( resultBuf )*resultBuf<< ret;
		return true;
	}

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::release) )
	{
		m_real_service->release();
		return true;
	}

    // 消息分发器
    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getMessageDispatch) )
    {
        *resultBuf << m_real_service->getMessageDispatch();
        return true;
    }

	//////////////////////////////////////////////////////////////////////////
	// 获取配置中心接口
	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getSchemeCenter) )
	{
		*resultBuf << m_real_service->getSchemeCenter();
		return true;
	}

	// 取得配置服务
	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getSchemeEngine) )
	{
		*resultBuf << m_real_service->getSchemeEngine();
		return true;
	}

	//////////////////////////////////////////////////////////////////////////

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getMatchManager) )
	{
		*resultBuf << m_real_service->getMatchManager();
		return true;
	}

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getChatManager) )
	{
		*resultBuf << m_real_service->getChatManager();
		return true;
	}

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getShareReceiver) )
	{
		*resultBuf << m_real_service->getShareReceiver();
		return true;
	}

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getShareSceneSvrSource) )
	{
		*resultBuf << m_real_service->getShareSceneSvrSource();
		return true;
	}

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getClanManager) )
    {
        *resultBuf << m_real_service->getClanManager();
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getKinManager) )
    {
        *resultBuf << m_real_service->getKinManager();
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getOrderManager) )
    {
        *resultBuf << m_real_service->getOrderManager();
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getTeamManager) )
    {
        *resultBuf << m_real_service->getTeamManager();
        return true;
    }

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getMentorshipManager) )
	{
		*resultBuf << m_real_service->getMentorshipManager();
		return true;
	}

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getLegendCupManager) )
    {
        *resultBuf << m_real_service->getLegendCupManager();
        return true;
    }
	
    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getResourceManager) )
    {
        *resultBuf << m_real_service->getResourceManager();
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getMiscManager) )
    {
        *resultBuf << m_real_service->getMiscManager();
        return true;
    }

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getOSSLogServer) )
	{
		*resultBuf << m_real_service->getOSSLogServer();
		return true;
	}

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getSNSManager) )
	{
		if (NULL == resultBuf)
		{
			return false;
		}
		*resultBuf << m_real_service->getSNSManager();
		return true;
	}

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getLuaManager) )
    {
        if (NULL == resultBuf)
        {
            return false;
        }
        *resultBuf << m_real_service->getLuaManager();
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::getTaskManager) )
    {
        if (NULL == resultBuf)
        {
            return false;
        }
        *resultBuf << m_real_service->getTaskManager();
        return true;
    }

	if ( TEST_SAME_FUNCTION(ISocialGlobalService::getMailManager) )
	{
		*resultBuf << m_real_service->getMailManager();
		return true;
	}

    //////////////////////////////////////////////////////////////////////////
    if ( TEST_SAME_FUNCTION(ISocialGlobalService::requestHandleServerControl) )
    {
        DWORD dwState;
        GET_MSG_PARAM_1(DWORD ,dwState);
        m_real_service->requestHandleServerControl(dwState);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::requestManagerStart) )
    {
        DWORD dwStartFlag; DWORD dwUserData;
        GET_MSG_PARAM_2(DWORD ,dwStartFlag, DWORD, dwUserData);
        *resultBuf << m_real_service->requestManagerStart(dwStartFlag, dwUserData);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::requstExecuteUnlock) )
    {
        m_real_service->requstExecuteUnlock();
        return true;
    }    
    ///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleServerListUpdated) )
    {
        m_real_service->handleServerListUpdated();
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleServerInfoUpdated) )
    {
		rkt::ibuffer in(pMsg->context,pMsg->context_len);
        DWORD ServerID; BYTE nState; size_t nInfolen;
        in >>ServerID>> nState >> nInfolen;
        m_real_service->handleServerInfoUpdated(ServerID, nState, in.current(), nInfolen);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handlePostMessage) )
    {
		rkt::ibuffer in(pMsg->context,pMsg->context_len);
        DWORD server;WORD wDataLen;
        in >>server>> wDataLen;
        m_real_service->handlePostMessage(server, in.current(), wDataLen);
        return true;
    }


    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleServerDisconnectShareCallback) )
    {
        rkt::ibuffer in(pMsg->context,pMsg->context_len);
        WORD wSrvType;size_t nDataSize;
        in >>wSrvType>> nDataSize;
        m_real_service->handleServerDisconnectShareCallback(wSrvType, in.current(), nDataSize);
        return true;
    }

    ///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleClientIncoming) )
    {
        ClientID client;
        GET_MSG_PARAM_1(ClientID, client);
        m_real_service->handleClientIncoming(client);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleGateWayReceivedData) )
    {
		rkt::ibuffer in(pMsg->context,pMsg->context_len);
        ClientID client;size_t len;
        in >>client>> len;
        m_real_service->handleGateWayReceivedData(client, in.current(), len);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleGateWayClientDisconnect) )
    {
        ClientID client;
        DWORD dwReason;
        GET_MSG_PARAM_2(ClientID ,client,DWORD ,dwReason);
        m_real_service->handleGateWayClientDisconnect(client, dwReason);
        return true;
    }

    ///////ManagerConnector///////////////////////////////////////////////////////////////////
    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleManagerSetGameWorldID) )
    {
        m_real_service->handleManagerSetGameWorldID();
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleManagerResourceData) )
    {
        rkt::ibuffer in(pMsg->context,pMsg->context_len);
        DWORD dwDataLen;
        in >>dwDataLen;
        m_real_service->handleManagerResourceData(in.current(), dwDataLen);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ISocialGlobalService::handleManagerResourceResult) )
    {
        rkt::ibuffer in(pMsg->context,pMsg->context_len);
        DWORD dwDataLen;
        in >>dwDataLen;
        m_real_service->handleManagerResourceResult(in.current(), dwDataLen);
        return true;
    }


	return false;
}

// 返回代理对象即可
void * SocialGlobalService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

// 销毁
void SocialGlobalService_Stub::release()
{
	if ( m_real_service )
	{
		m_real_service->release();
		m_real_service = 0;
	}

	delete this;
}


//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateSocialGlobalService()
{
	return new SocialGlobalService_Stub(new SocialGlobalService);
}