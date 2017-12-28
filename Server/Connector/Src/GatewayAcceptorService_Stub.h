/*******************************************************************
** 文件名:	GatewayAcceptorService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据IGatewayAcceptorService接口定义，自动生成的GatewayAcceptorService接口
            代理文件，该文件是为符合Framework对接口进行的包装

			
********************************************************************/

#pragma once

/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	            而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

#include "IServiceContainer.h"
#include "IGatewayAcceptorService.h"
#include "buffer.h"
#include "tinyxml.h"

// 这个是服务实现类，你需要自己实现
#include "GatewayAcceptorService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现IGatewayAcceptorService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class GatewayAcceptorService_Proxy : public IGatewayAcceptorService
{
public:
	SERVICE_PTR    m_pContainer;
	GatewayAcceptorService *      m_real_service;

	GatewayAcceptorService_Proxy() : m_real_service(0), m_pContainer(0){
	}

	virtual ~GatewayAcceptorService_Proxy(){
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
   	/**
	连接某个网关 : 这个函数可以多次调用，因为存在多个网关
	通常调用的时机是:连接中心服成功后，发现了网关服务器，就调用一下连接
	*/
	virtual bool connectGateway( string ip,int port )
	{
        if (m_pContainer == 0)
            return bool();

		obuf64 t_data;
		t_data << ip << port;

		BUILD_MSG_BUFFER( IGatewayAcceptorService::connectGateway,t_data );

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return false;
	}

	virtual void sendData( ClientID client,PACKAGE_PTR package,int nFlag = 0 )
	{
		if ( m_real_service )
		{
			m_real_service->sendData( client,package,nFlag );
		}
	}

	/**
	@name         : 向多个客户端广播数据,即使这些客户端不在同一个网关上
	@brief        : 使用这个函数可以显著减少服务器间的数据传输量,因为如果在同一个网关上的客户端只需发送一条消息
	@param pClientArray: 要发送的客户端ID数组
	@param wClientNum  : 客户端数目
	@param pData       : 数据缓冲区地址
	@param wDataLen    : 数据长度
	@return       : 
	*/
	virtual void broadcast(ClientID * pClientArray,WORD wClientNum,LPVOID pData,WORD wDataLen)
	{
        if (m_pContainer == 0)
            return;

		obuf256 t_data;
		t_data << wClientNum << wDataLen;
				
		t_data.push_back( pClientArray, (wClientNum*sizeof(ClientID)) );
		t_data.push_back( pData, wDataLen );

		BUILD_MSG_BUFFER( IGatewayAcceptorService::broadcast,t_data );
		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

    // 踢人
    virtual void kickOutClient(ClientID client, int nReason)
    {
        if (m_pContainer == 0)
            return;

		BUILD_MSG_CONTEXT_2( IGatewayAcceptorService::kickOutClient,ClientID, client, int, nReason );
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }   

    // 获取sessionID
    virtual SessionID getSession(ClientID client)
    {
        if (m_pContainer == 0)
            return SessionID();

		BUILD_MSG_CONTEXT_1( IGatewayAcceptorService::getSession,ClientID, client );

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(SessionID);
        return SessionID();     // 请检查默认返回值是否正确
    }

    // 某个session是否使用过
    virtual bool isSessionUsed(SessionID session)
    {
        if (m_pContainer == 0)
            return bool();

		BUILD_MSG_CONTEXT_1( IGatewayAcceptorService::isSessionUsed,SessionID, session );

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);
        return false;     // 请检查默认返回值是否正确
    }

    // 通知网关客户端迁移
    virtual void migrateClient(ClientID client, SessionID session, int nZoneServerID, int nSceneID)
    {
        if (m_pContainer == 0)
            return;

		BUILD_MSG_CONTEXT_4( IGatewayAcceptorService::migrateClient,ClientID, client, SessionID, session, int, nZoneServerID, int, nSceneID );
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    // 获取IP地址
    virtual string getIpAddress(ClientID client)
    {
        if (m_pContainer == 0)
            return string();

        BUILD_MSG_CONTEXT_1( IGatewayAcceptorService::getIpAddress, ClientID, client );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);

		RETURN_STRING_FROM_MSG_BUF();
    }

    virtual void plug(ClientID client, SERVICE_ID id)
    {
        if (m_pContainer == 0)
            return;

		BUILD_MSG_CONTEXT_2( IGatewayAcceptorService::plug,ClientID ,client, SERVICE_ID, id );
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    virtual void unplug(ClientID client)
    {
        if (m_pContainer == 0)
            return;

		BUILD_MSG_CONTEXT_1( IGatewayAcceptorService::unplug,ClientID ,client );
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }
};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class GatewayAcceptorService_Stub : public IServiceStub
{
	GatewayAcceptorService_Proxy  m_proxy;
	GatewayAcceptorService *      m_real_service;
    
public:
	GatewayAcceptorService_Stub(GatewayAcceptorService * pReal) : m_real_service(pReal){
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;
		m_proxy.m_real_service = m_real_service;

        string strIP="";
        TiXmlElement * pConfig = (TiXmlElement *)pContainer->get_scheme().scheme->pExtend;
        if ( pConfig )
        {
            const char * ip = pConfig->Attribute("server_ip");
            if ( ip )
            {
                strIP = ip;
            }
        }
            

		return m_real_service->Start(strIP);
	}

	// 停止
	virtual bool on_stop()
	{
		bool bResult = m_real_service->Stop();
        m_proxy.m_pContainer = SERVICE_PTR(0);
        return bResult;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf  )
	{
        ////////////////////////////////////////////////////////////////////////////
        // 以下自动生成的消息解包，你需要检查

		if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::connectGateway) )
		{
			string ip;
			int port = 0;
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			in >> ip >> port;
			const bool & vReturn= m_real_service->connectGateway(ip,port);
			if ( resultBuf ) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::broadcast) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			WORD wClientNum = 0; WORD wDataLen = 0;
			in >> wClientNum >> wDataLen;

			ClientID * pClient = (ClientID *)in.current();

			in.offset( wClientNum*sizeof(ClientID));
			LPVOID  pData = in.current();
			
			m_real_service->broadcast(pClient,wClientNum, pData, wDataLen);
			return true;
		}

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::kickOutClient) )
        {
            ClientID client;
            int nReason;
            GET_MSG_PARAM_2( ClientID, client,int, nReason );
            m_real_service->kickOutClient(client, nReason);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::getSession) )
        {
            ClientID client;
            GET_MSG_PARAM_1( ClientID, client );

            const SessionID vReturn = m_real_service->getSession(client);

            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::isSessionUsed) )
        {
            SessionID session;
            GET_MSG_PARAM_1( SessionID, session );

            const SessionID vReturn = m_real_service->isSessionUsed(session);

            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::migrateClient) )
        {
            ClientID client; SessionID session; int nZoneServerID; int nSceneID;  
            GET_MSG_PARAM_4( ClientID ,client, SessionID, session, int, nZoneServerID, int ,nSceneID );

            m_real_service->migrateClient(client, session, nZoneServerID, nSceneID);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::getIpAddress) )
        {
            ClientID client;
            GET_MSG_PARAM_1( ClientID, client );

            const string vReturn = m_real_service->getIpAddress(client);
            if ( resultBuf ) *resultBuf << vReturn;
			return true;
        }

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::plug) )
        {
            ClientID client; SERVICE_ID id;
            GET_MSG_PARAM_2( ClientID ,client, SERVICE_ID, id );

            m_real_service->plug(client, id);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::unplug) )
        {
            ClientID client;
            GET_MSG_PARAM_1( ClientID ,client );

            m_real_service->unplug(client);
            return true;
        }

		ErrorLn(__FUNCTION__":can not found function name:" << MsgFunctionName<SERVICE_MESSAGE>()(pMsg) );
		return false;
	}

	// 返回代理对象即可
	virtual void * query_interface( int iid )
	{
		return &m_proxy;
	}

	// 销毁
	virtual void release()
	{
		if ( m_real_service )
		{
			m_real_service->release();
			m_real_service = 0;
		}

		delete this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateGatewayAcceptorService()
{
	return new GatewayAcceptorService_Stub(new GatewayAcceptorService);
}