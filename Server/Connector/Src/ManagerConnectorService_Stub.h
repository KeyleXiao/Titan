/*******************************************************************
** 文件名:	ManagerConnectorService_Stub.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-07-09
** 版  本:	1.0
** 描  述:	管理器服务器连接服务
本文件为根据IManagerConnectorService接口定义，自动生成的ManagerConnectorService接口
代理文件，该文件是为符合Framework对接口进行的包装
			
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "IManagerConnectorService.h"
#include "ManagerConnectorService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现IManagerConnectorService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class ManagerConnectorService_Proxy : public IManagerConnectorService
{
public:
	SERVICE_PTR    m_pContainer;


	ManagerConnectorService_Proxy() : m_pContainer(0) {
	}

	virtual ~ManagerConnectorService_Proxy(){
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	/** 取得本地服务器在子服务器上的ID
	*/
	virtual DWORD  GetLocalServerID()
	{
        if (m_pContainer == 0)
            return DWORD();

		BUILD_MSG_CONTEXT_VOID(IManagerConnectorService::GetLocalServerID);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);

		return DWORD();     // 请检查默认返回值是否正确
	};


	/** 直接发送数据给子服务器
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  SendData(ibuffer & msg)
	{
        if (m_pContainer == 0)
            return;

		BUILD_MSG_BUFFER( IManagerConnectorService::SendData,msg );

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 发给主服务器数据
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  SendDataToMain(ibuffer & msg)
	{
        if (m_pContainer == 0)
            return;

		BUILD_MSG_BUFFER( IManagerConnectorService::SendDataToMain,msg );

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 发给根服务器数据
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  SendDataToRoot(ibuffer & msg)
	{
        if (m_pContainer == 0)
            return;

		BUILD_MSG_BUFFER( IManagerConnectorService::SendDataToRoot,msg );

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 请求启动
	@param dwStartFlag   : 启动标识 EMManagerStartServerFlag
	@param dwUserData    : 用户数据
	*/
	virtual bool RequestStart(DWORD dwStartFlag,DWORD dwUserData)
	{
        if (m_pContainer == 0)
            return bool();

		BUILD_MSG_CONTEXT_2(IManagerConnectorService::RequestStart,DWORD ,dwStartFlag,DWORD ,dwUserData);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return bool();     // 请检查默认返回值是否正确
	}

	/** 请求关闭,如果没有请求，就关了服务器，会当作当机处理
	@param dwCloseFlag   : 关闭标识 EMManagerCloseServerFlag
	@param dwUserData    : 用户数据
	*/
	virtual bool RequestClose(DWORD dwCloseFlag,DWORD dwUserData)
	{
        if (m_pContainer == 0)
            return bool();

		BUILD_MSG_CONTEXT_2(IManagerConnectorService::RequestClose,DWORD ,dwCloseFlag,DWORD ,dwUserData);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return bool();     // 请检查默认返回值是否正确
	}

	/** 取得在主服务器上的UID
	*/
	virtual DWORD  GetServerUID()
	{
        if (m_pContainer == 0)
            return DWORD();

		BUILD_MSG_CONTEXT_VOID(IManagerConnectorService::GetServerUID);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);

		return DWORD();     // 请检查默认返回值是否正确
	}

	/** 取得游戏世界UID
	*/
	virtual DWORD  GetWorldID()
	{
        if (m_pContainer == 0)
            return DWORD();

		BUILD_MSG_CONTEXT_VOID(IManagerConnectorService::GetWorldID);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);

		return DWORD();     // 请检查默认返回值是否正确
	}

	/** 取得游戏世界信息
	*/
	virtual SGameWorldConnectorInfo GetGameWorldInfo()
	{
        if (m_pContainer == 0)
            return SGameWorldConnectorInfo();

		BUILD_MSG_CONTEXT_VOID(IManagerConnectorService::GetGameWorldInfo);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(SGameWorldConnectorInfo);

		return SGameWorldConnectorInfo();     // 请检查默认返回值是否正确
	}

	///** 注册事件处理者
	//@param wEventID 事件ID	参考 EMEventManager
	//@param hander 事件处理者实例
	//*/
	//virtual bool registerEventHandler(WORD wEventID,IManagerEventHandler* handler)
	//{
	//	BUILD_MSG_CONTEXT_2(IManagerConnectorService::registerEventHandler,WORD ,wEventID,IManagerEventHandler* ,handler);

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	//	RETURN_FROM_MSG_BUF(bool);

	//	return bool();     // 请检查默认返回值是否正确
	//}

	///** 注销事件处理者
	//@param wEventID 事件ID	参考 EMEventManager
	//@param  hander 事件处理者实例
	//*/
	//virtual bool unregisterEventHandler(WORD wEventID,IManagerEventHandler* handler)
	//{
	//	BUILD_MSG_CONTEXT_2(IManagerConnectorService::unregisterEventHandler,WORD ,wEventID,IManagerEventHandler* ,handler);

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	//	RETURN_FROM_MSG_BUF(bool);

	//	return bool();     // 请检查默认返回值是否正确
	//}

	/** 设置实时数据
	DWORD dwDataID		: 实时数据类型ID,不同的服务器类型用不用的ID 参考 EMManagerRealTimeDataLogin
	int nValue			: 数值
	@return  是否成功设置
	*/
	virtual void SetRealTimeData(DWORD dwDataID,int nValue)
	{
        if (m_pContainer == 0)
            return;

		BUILD_MSG_CONTEXT_2( IManagerConnectorService::SetRealTimeData,DWORD, dwDataID,int, nValue );

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 取得实时数据
	DWORD dwDataID		: 实时数据类型ID
	@return  int nValue	: 数值
	*/
	virtual int GetRealTimeData(DWORD dwDataID)
	{
        if (m_pContainer == 0)
            return int();

		BUILD_MSG_CONTEXT_1(IManagerConnectorService::GetRealTimeData,DWORD, dwDataID);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(int);

		return int();     // 请检查默认返回值是否正确
	}


	/** 设定服务器状态
	DWORD dwStateFlag:服务器状态类型定义 EMManagerServerStateFlag
	*/
    virtual void SetServerState(DWORD dwStateFlag)
	{
        if (m_pContainer == 0)
            return;

		BUILD_MSG_CONTEXT_1( IManagerConnectorService::SetServerState,DWORD,dwStateFlag);

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}
};



/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class ManagerConnectorService_Stub : public IServiceStub
{
	ManagerConnectorService_Proxy	m_proxy;
	ManagerConnectorService *		m_real_service;
    obuf							m_return_buffer;

public:
	ManagerConnectorService_Stub(ManagerConnectorService * pReal) : m_real_service(pReal){
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
		
		if ( TEST_SAME_FUNCTION(IManagerConnectorService::GetLocalServerID) )
		{
			if ( resultBuf ) *resultBuf << m_real_service->GetLocalServerID();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::SendData) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			m_real_service->SendData(in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::SendDataToMain) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			m_real_service->SendDataToMain(in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::SendDataToRoot) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			m_real_service->SendDataToRoot(in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::RequestStart) )
		{
			DWORD dwStartFlag; DWORD dwUserData;
			GET_MSG_PARAM_2(DWORD ,dwStartFlag,DWORD ,dwUserData);
			bool ret = m_real_service->RequestStart(dwStartFlag, dwUserData);
			if ( resultBuf ) *resultBuf << ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::RequestClose) )
		{
			DWORD dwCloseFlag; DWORD dwUserData;
			GET_MSG_PARAM_2(DWORD ,dwCloseFlag,DWORD ,dwUserData);
			bool ret = m_real_service->RequestClose(dwCloseFlag, dwUserData);
			if ( resultBuf ) *resultBuf << ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::GetServerUID) )
		{
			if ( resultBuf ) *resultBuf << m_real_service->GetServerUID();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::GetWorldID) )
		{
			if ( resultBuf ) *resultBuf << m_real_service->GetWorldID();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::GetGameWorldInfo) )
		{
			if ( resultBuf ) *resultBuf << m_real_service->GetGameWorldInfo();
			return true;
		}

		//if ( TEST_SAME_FUNCTION(IManagerConnectorService::registerEventHandler) )
		//{
		//	WORD wEventID;IManagerEventHandler* handler;
		//	GET_MSG_PARAM_2(WORD, wEventID,IManagerEventHandler* ,handler);
		//	bool ret= m_real_service->registerEventHandler(wEventID,handler);
		//	if ( resultBuf ) *resultBuf << ret;
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IManagerConnectorService::unregisterEventHandler) )
		//{
		//	WORD wEventID;IManagerEventHandler* handler;
		//	GET_MSG_PARAM_2(WORD, wEventID,IManagerEventHandler* ,handler);
		//	bool ret= m_real_service->unregisterEventHandler(wEventID,handler);
		//	if ( resultBuf ) *resultBuf << ret;
		//	return true;
		//}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::SetRealTimeData) )
		{
			DWORD dwDataID;int nValue;
			GET_MSG_PARAM_2(DWORD, dwDataID,int ,nValue);
			m_real_service->SetRealTimeData(dwDataID,nValue);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::GetRealTimeData) )
		{
			DWORD dwDataID;
			GET_MSG_PARAM_1(DWORD, dwDataID);
			if( resultBuf ) *resultBuf << m_real_service->GetRealTimeData(dwDataID);
			return true;
		}

        if ( TEST_SAME_FUNCTION(IManagerConnectorService::SetServerState) )
        {
            DWORD dwStateFlag;
            GET_MSG_PARAM_1(DWORD, dwStateFlag);
            m_real_service->SetServerState(dwStateFlag);
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
extern "C" __declspec(dllexport) IServiceStub * CreateManagerConnectorService()
{
	return new ManagerConnectorService_Stub(new ManagerConnectorService);
}