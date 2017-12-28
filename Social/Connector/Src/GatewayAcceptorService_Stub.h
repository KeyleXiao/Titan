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

	GatewayAcceptorService_Proxy() : m_real_service(0), m_pContainer(0) {
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

	virtual void sendData( ClientID client,PACKAGE_PTR package )
	{
        if (m_pContainer == 0)
            return;

		//obuf256 t_data;
		//t_data << "sendData" << client << package;

		//rkt::obuf resultBuf;
		//m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_NO_BLOCK);
		if ( m_real_service )
		{
			m_real_service->sendData( client,package );
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

  //  // 踢人
  //  virtual void kickOutClient(ClientID client, int nReason)
  //  {
		//BUILD_MSG_CONTEXT_2( IGatewayAcceptorService::kickOutClient,ClientID, client, int, nReason );
  //      m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
  //  }   

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

    virtual SSceneNodeLoc getSceneNodeLoc(ClientID client)
    {
        if (m_pContainer == 0)
            return SSceneNodeLoc();

        BUILD_MSG_CONTEXT_1( IGatewayAcceptorService::getSceneNodeLoc,ClientID, client );

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(SSceneNodeLoc);
        return SSceneNodeLoc();     // 请检查默认返回值是否正确
    }

	virtual string getIpAddress(ClientID client)
	{
        if (m_pContainer == 0)
            return string();

		BUILD_MSG_CONTEXT_1( IGatewayAcceptorService::getIpAddress,ClientID, client );

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_STRING_FROM_MSG_BUF();
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
        m_proxy.m_real_service = pReal;
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		return m_real_service->start();
	}

	// 停止
	virtual bool on_stop()
	{
		bool bResult = m_real_service->stop();

        m_proxy.m_real_service = NULL;
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

        //if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::kickOutClient) )
        //{
        //    ClientID client;
        //    int nReason;
        //    GET_MSG_PARAM_2( ClientID, client,int, nReason );
        //    m_real_service->kickOutClient(client, nReason);
        //    return true;
        //}

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::getSession) )
        {
            ClientID client;
            GET_MSG_PARAM_1( ClientID, client );

            const SessionID vReturn = m_real_service->getSession(client);

            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGatewayAcceptorService::getSceneNodeLoc) )
        {
            ClientID client;
            GET_MSG_PARAM_1( ClientID, client );

            const SSceneNodeLoc vReturn = m_real_service->getSceneNodeLoc(client);

            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

		if (TEST_SAME_FUNCTION(IGatewayAcceptorService::getIpAddress))
		{
			ClientID client;
			GET_MSG_PARAM_1(ClientID, client);

			const string vReturn = m_real_service->getIpAddress(client);

			if (resultBuf) *resultBuf << vReturn;

			return true;
		}

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