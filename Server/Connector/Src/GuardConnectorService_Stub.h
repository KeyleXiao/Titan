/*******************************************************************
** 文件名:	GuardConnectorService_Stub.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-18
** 版  本:	1.0
** 描  述:	数据监护服务器连接服务
本文件为根据IGuardConnectorService接口定义，自动生成的GuardConnectorService接口
代理文件，该文件是为符合Framework对接口进行的包装
			
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "IGuardConnectorService.h"
#include "GuardConnectorService.h "

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现IGuardConnectorService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class GuardConnectorService_Proxy : public IGuardConnectorService
{
public:
	SERVICE_PTR    m_pContainer;


	GuardConnectorService_Proxy() : m_pContainer(0)
	{
	}

	virtual ~GuardConnectorService_Proxy()
	{
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	/** 关闭和主服务器之间的连接
	*/
	virtual void Close()
	{
        if (m_pContainer == 0)
            return;

		BUILD_MSG_CONTEXT_VOID( IGuardConnectorService::Close );
		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 注册消息处理者
	@param moduleId 模块ID
	@param hander 消息处理者实例
	*/
	virtual bool RegisterMessageHandler(DWORD dwNameSpace, IGuardMessageHandler* handler)
	{
        if (m_pContainer == 0)
            return bool();

		BUILD_MSG_CONTEXT_2( IGuardConnectorService::RegisterMessageHandler,DWORD ,dwNameSpace, IGuardMessageHandler*, handler );

        rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return bool();     // 请检查默认返回值是否正确
	}

	/** 注销消息处理者
	@param moduleId 模块ID
	*/
	virtual bool UnregisterMessageHandler(DWORD dwNameSpace, IGuardMessageHandler* handler)
	{
        if (m_pContainer == 0)
            return bool();

		BUILD_MSG_CONTEXT_2( IGuardConnectorService::UnregisterMessageHandler,DWORD ,dwNameSpace, IGuardMessageHandler*, handler );

        rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return bool();     // 请检查默认返回值是否正确
	}

	/** 往数据监护服务器写入数据
	@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
	@param nKey	         : 键值
	@param pData         : 数据
	@param dwLen         : 数据长度
	@return
	*/
	virtual bool WriteImageDataToServer(IGuardMessageHandler* handler, int nNameSpace, int nKey, const char * pData, DWORD dwLen)
	{
        if (m_pContainer == 0)
            return bool();

		obuf256 t_data;
		t_data <<handler<<nNameSpace<<nKey<<dwLen;
		if (pData)
		{
			t_data.push_back(pData, dwLen);
		}

		BUILD_MSG_BUFFER(IGuardConnectorService::WriteImageDataToServer,t_data);

		return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 从数据监护服务器异步读取数据
	@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
	@param nKey	         : 键值
	@return
	*/
	virtual bool ReadImageDataFromServer(IGuardMessageHandler* handler, int nNameSpace, int nKey)
	{
        if (m_pContainer == 0)
            return bool();

		BUILD_MSG_CONTEXT_3( IGuardConnectorService::ReadImageDataFromServer,IGuardMessageHandler* ,handler, int, nNameSpace, int, nKey );
		return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}



	/** 取得本地服务器在跨区桥服务器上的ID
	*/
	virtual DWORD  GetLocalServerCSID()
	{
        if (m_pContainer == 0)
            return DWORD();

		BUILD_MSG_CONTEXT_VOID( IGuardConnectorService::GetLocalServerCSID );

        rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);
		return DWORD();     // 请检查默认返回值是否正确
	};

	/** 是否与服务器断开连接了
	@param   
	@param   
	@return  
	*/
	virtual bool			IsDisconnectToServer(void)
	{
        if (m_pContainer == 0)
            return bool();

		BUILD_MSG_CONTEXT_VOID( IGuardConnectorService::IsDisconnectToServer );

        rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);
		return bool();     // 请检查默认返回值是否正确		
	}

	/** 是否已启动
	@param   
	@param   
	@return  
	*/
	virtual bool			IsStartup()
	{
        if (m_pContainer == 0)
            return bool();

		BUILD_MSG_CONTEXT_VOID( IGuardConnectorService::IsStartup );

        rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return bool();     // 请检查默认返回值是否正确	
	}
};



/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class GuardConnectorService_Stub : public IServiceStub
{
	GuardConnectorService_Proxy		m_proxy;
	GuardConnectorService *			m_real_service;
    obuf							m_return_buffer;

public:
	GuardConnectorService_Stub(GuardConnectorService * pReal)
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

		if ( TEST_SAME_FUNCTION(IGuardConnectorService::Close) )
		{
			m_real_service->Close();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGuardConnectorService::RegisterMessageHandler) )
		{
			DWORD dwNameSpace = 0; IGuardMessageHandler* handler = NULL;
			GET_MSG_PARAM_2( DWORD ,dwNameSpace,IGuardMessageHandler* ,handler );
			bool ret= m_real_service->RegisterMessageHandler(dwNameSpace, handler);
			if ( resultBuf)*resultBuf<<ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGuardConnectorService::UnregisterMessageHandler) )
		{
			DWORD dwNameSpace = 0; IGuardMessageHandler* handler = NULL;
			GET_MSG_PARAM_2( DWORD ,dwNameSpace,IGuardMessageHandler* ,handler );
			bool ret= m_real_service->UnregisterMessageHandler(dwNameSpace, handler);
			if ( resultBuf ) *resultBuf<<ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGuardConnectorService::WriteImageDataToServer) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			IGuardMessageHandler* handler=NULL; int nNameSpace =0; int nKey =0; DWORD dwLen = 0;
			in >> handler >> nNameSpace >> nKey >> dwLen;
			bool ret = m_real_service->WriteImageDataToServer(handler, nNameSpace, nKey, in.current(), dwLen);
			if ( resultBuf ) *resultBuf<<ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGuardConnectorService::ReadImageDataFromServer) )
		{
			IGuardMessageHandler* handler=NULL; int nNameSpace =0; int nKey =0;
			GET_MSG_PARAM_3( IGuardMessageHandler*, handler, int, nNameSpace,int, nKey );
			if( resultBuf )*resultBuf << m_real_service->ReadImageDataFromServer(handler, nNameSpace, nKey);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGuardConnectorService::GetLocalServerCSID) )
		{
			if( resultBuf )*resultBuf << m_real_service->GetLocalServerCSID();
			return true;
		}


		if ( TEST_SAME_FUNCTION(IGuardConnectorService::IsDisconnectToServer) )
		{
			if( resultBuf )*resultBuf << m_real_service->IsDisconnectToServer();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGuardConnectorService::IsStartup) )
		{
			if( resultBuf )*resultBuf << m_real_service->IsStartup();
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
extern "C" __declspec(dllexport) IServiceStub * CreateGuardConnectorService()
{
	return new GuardConnectorService_Stub(new GuardConnectorService);
}