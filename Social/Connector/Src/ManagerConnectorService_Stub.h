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


	ManagerConnectorService_Proxy() {
	}

	virtual ~ManagerConnectorService_Proxy(){
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	/** 取得本地服务器在子服务器上的ID
	*/
	virtual DWORD  getLocalServerID()
	{
		BUILD_MSG_CONTEXT_VOID( IManagerConnectorService::getLocalServerID);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);
		return 0;
	};


	/** 直接发送数据给子服务器
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  sendData(ibuffer & msg)
	{
		BUILD_MSG_BUFFER( IManagerConnectorService::sendData,msg);

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 发给主服务器数据
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  sendDataToMain(ibuffer & msg)
	{
		BUILD_MSG_BUFFER( IManagerConnectorService::sendDataToMain,msg);

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 发给根服务器数据
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  sendDataToRoot(ibuffer & msg)
	{
		BUILD_MSG_BUFFER( IManagerConnectorService::sendDataToRoot,msg);

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 请求启动
	@param dwStartFlag   : 启动标识 EMManagerStartServerFlag
	@param dwUserData    : 用户数据
	*/
	virtual bool requestStart(DWORD dwStartFlag,DWORD dwUserData)
	{
		BUILD_MSG_CONTEXT_2( IManagerConnectorService::requestStart,DWORD ,dwStartFlag,DWORD ,dwUserData);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);
		return false;
	}

	/** 请求关闭,如果没有请求，就关了服务器，会当作当机处理
	@param dwCloseFlag   : 关闭标识 EMManagerCloseServerFlag
	@param dwUserData    : 用户数据
	*/
	virtual bool requestClose(DWORD dwCloseFlag,DWORD dwUserData)
	{
		BUILD_MSG_CONTEXT_2( IManagerConnectorService::requestStart,DWORD ,dwCloseFlag,DWORD ,dwUserData);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);
		return false;
	}

	/** 取得在主服务器上的UID
	*/
	virtual DWORD  getServerUID()
	{
		BUILD_MSG_CONTEXT_VOID( IManagerConnectorService::getServerUID);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);
		return false;
	}

	/** 取得游戏世界UID
	*/
	virtual DWORD  getWorldID()
	{
		BUILD_MSG_CONTEXT_VOID( IManagerConnectorService::getWorldID);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);
		return false;
	}

	/** 取得游戏世界信息
	*/
	virtual SGameWorldConnectorInfo getGameWorldInfo()
	{
		BUILD_MSG_CONTEXT_VOID( IManagerConnectorService::getGameWorldInfo);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(SGameWorldConnectorInfo);
		return SGameWorldConnectorInfo();     // 请检查默认返回值是否正确
	}

	///** 注册事件处理者
	//@param wEventID 事件ID	参考 EMEventManager
	//@param hander 事件处理者实例
	//*/
	//virtual bool registerEventHandler(WORD wEventID,IManagerEventHandler* handler)
	//{
	//	BUILD_MSG_CONTEXT_2( IManagerConnectorService::registerEventHandler,WORD ,wEventID,IManagerEventHandler*, handler);

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	//	RETURN_FROM_MSG_BUF(bool);

	//	return bool();     // 请检查默认返回值是否正确
	//}

	///** 注销事件处理者
	//@param wEventID 事件ID	参考 EMEventManager
	//@param  hander 事件处理者实例
	//*/
	//virtual bool unregisterEventHandler(WORD wEventID,IManagerEventHandler* handler)
	//{
	//	BUILD_MSG_CONTEXT_2( IManagerConnectorService::unregisterEventHandler,WORD ,wEventID,IManagerEventHandler*, handler);

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	//	RETURN_FROM_MSG_BUF(bool);
	//	return false;
	//}

	/** 设置实时数据
	DWORD dwDataID		: 实时数据类型ID,不同的服务器类型用不用的ID 参考 EMManagerRealTimeDataLogin
	int nValue			: 数值
	@return  是否成功设置
	*/
	virtual void setRealTimeData(DWORD dwDataID,int nValue)
	{
		BUILD_MSG_CONTEXT_2( IManagerConnectorService::setRealTimeData,DWORD, dwDataID,int, nValue);

		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	}

	/** 取得实时数据
	DWORD dwDataID		: 实时数据类型ID
	@return  int nValue	: 数值
	*/
	virtual int getRealTimeData(DWORD dwDataID)
	{
		BUILD_MSG_CONTEXT_1( IManagerConnectorService::getRealTimeData,DWORD ,dwDataID);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(int);
		return false;
	}


	/** 设定服务器状态
	DWORD dwStateFlag:服务器状态类型定义 EMManagerServerStateFlag
	*/
    virtual void setServerState(DWORD dwStateFlag)
	{
		BUILD_MSG_CONTEXT_1( IManagerConnectorService::setServerState,DWORD,dwStateFlag);

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
		
		if ( TEST_SAME_FUNCTION(IManagerConnectorService::getLocalServerID) )
		{
			*resultBuf << m_real_service->getLocalServerID();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::sendData) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			m_real_service->sendData(in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::sendDataToMain) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			m_real_service->sendDataToMain(in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::sendDataToRoot) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			m_real_service->sendDataToRoot(in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::requestStart) )
		{
			DWORD dwStartFlag; DWORD dwUserData;
			GET_MSG_PARAM_2(DWORD, dwStartFlag, DWORD ,dwUserData);
			bool ret = m_real_service->requestStart(dwStartFlag, dwUserData);
			if ( resultBuf ) *resultBuf<<ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::requestClose) )
		{
			DWORD dwCloseFlag; DWORD dwUserData;
			GET_MSG_PARAM_2(DWORD ,dwCloseFlag, DWORD, dwUserData);
			bool ret = m_real_service->requestClose(dwCloseFlag, dwUserData);
			if ( resultBuf ) *resultBuf<<ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::getServerUID) )
		{
			*resultBuf << m_real_service->getServerUID();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::getWorldID) )
		{
			*resultBuf << m_real_service->getWorldID();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::getGameWorldInfo) )
		{
			*resultBuf << m_real_service->getGameWorldInfo();
			return true;
		}

		/*if ( TEST_SAME_FUNCTION(IManagerConnectorService::registerEventHandler) )
		{
			WORD wEventID;IManagerEventHandler* handler;
			GET_MSG_PARAM_2(WORD ,wEventID,IManagerEventHandler*, handler);
			bool ret= m_real_service->registerEventHandler(wEventID,handler);
			if ( resultBuf ) *resultBuf<<ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::unregisterEventHandler) )
		{
			WORD wEventID;IManagerEventHandler* handler;
			GET_MSG_PARAM_2(WORD ,wEventID,IManagerEventHandler*, handler);
			bool ret= m_real_service->unregisterEventHandler(wEventID,handler);
			if ( resultBuf ) *resultBuf<<ret;
			return true;
		}*/

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::setRealTimeData) )
		{
			DWORD dwDataID;int nValue;
			GET_MSG_PARAM_2(DWORD, dwDataID,int, nValue);
			m_real_service->setRealTimeData(dwDataID,nValue);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IManagerConnectorService::getRealTimeData) )
		{
			DWORD dwDataID;
			GET_MSG_PARAM_1(DWORD, dwDataID);
			*resultBuf << m_real_service->getRealTimeData(dwDataID);
			return true;
		}

        if ( TEST_SAME_FUNCTION(IManagerConnectorService::setServerState) )
        {
            DWORD dwStateFlag;
            GET_MSG_PARAM_1(DWORD, dwStateFlag);
            m_real_service->setServerState(dwStateFlag);
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
extern "C" __declspec(dllexport) IServiceStub * CreateManagerConnectorService()
{
	return new ManagerConnectorService_Stub(new ManagerConnectorService);
}