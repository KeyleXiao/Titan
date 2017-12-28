/*******************************************************************
** 文件名:	SNSConnectorService_Stub.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2016-11-30
** 版  本:	1.0
** 描  述:	SNS服务器连接服务
本文件为根据ISNSConnectorService接口定义，自动生成的SNSConnectorService接口
代理文件，该文件是为符合Framework对接口进行的包装
			
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "ISNSConnectorService.h"
#include "SNSConnectorService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现ISNSConnectorService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class SNSConnectorService_Proxy : public ISNSConnectorService
{
public:
	SERVICE_PTR    m_pContainer;


	SNSConnectorService_Proxy() : m_pContainer(0) {
	}

	virtual ~SNSConnectorService_Proxy(){
	}

	//   ////////////////////////////////////////////////////////////////////////////
	//   // 以下自动生成的消息打包函数，你需要检查
	//   ////////////////////////////////////////////////////////////////////////////

	/** 直接发送数据给SNS服务器
	@param msg: 要发送的数据
	*/
	virtual void  sendMessage(int nMessageID, void * pData, int nDataLen)
	{
        if (m_pContainer == 0)
            return;

		obuf256 t_data;
		t_data << nMessageID << nDataLen;
		t_data.push_back( pData, nDataLen );

		BUILD_MSG_BUFFER( ISNSService::recv_message,t_data );
		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}
};



/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class SNSConnectorService_Stub : public IServiceStub
{
	SNSConnectorService_Proxy	m_proxy;
	SNSConnectorService *		m_real_service;
    obuf							m_return_buffer;

public:
	SNSConnectorService_Stub(SNSConnectorService * pReal)
		: m_real_service(pReal)
	{
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		TiXmlElement * pConfig = (TiXmlElement *)pContainer->get_scheme().scheme->pExtend;
		if ( pConfig==0 )
			return false;

		const char * ip = pConfig->Attribute("server_ip");
		if ( ip==0 )
		{
			ErrorLn(_GT("启动服务失败！缺少配置项[server_ip]"));
			return false;
		}

		int port = 0;
		pConfig->QueryIntAttribute("server_port",&port);
		if ( port==0 )
		{
			ErrorLn(_GT("启动服务失败！缺少配置项[server_port]"));
			return false;
		}

		return m_real_service->Start( ip,port);
	}

	// 停止
	virtual bool on_stop()
	{
		m_real_service->Stop();
		return true;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
		////////////////////////////////////////////////////////////////////////////
		// 以下自动生成的消息解包，你需要检查
		if ( TEST_SAME_FUNCTION(ISNSConnectorService::sendMessage) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);

			int nMessageID = 0; 
			int nDataLen = 0;
			in >> nMessageID >> nDataLen;

			m_real_service->sendMessage(nMessageID, in.current(), nDataLen);
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
extern "C" __declspec(dllexport) IServiceStub * CreateSNSConnectorService()
{
	return new SNSConnectorService_Stub(new SNSConnectorService);
}