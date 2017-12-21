﻿/*******************************************************************
** 文件名:	BridgeConnectorService_Stub.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-07-09
** 版  本:	1.0
** 描  述:	桥服务器连接服务
本文件为根据IBridgeConnectorService接口定义，自动生成的BridgeConnectorService接口
代理文件，该文件是为符合Framework对接口进行的包装
			
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "IBridgeConnectorService.h"
#include "BridgeConnectorService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现IBridgeConnectorService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class BridgeConnectorService_Proxy : public IBridgeConnectorService
{
public:
	SERVICE_PTR    m_pContainer;


	BridgeConnectorService_Proxy() {
	}

	virtual ~BridgeConnectorService_Proxy(){
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
		/** 取得本地服务器在跨区桥服务器上的ID
		*/
		virtual DWORD  GetServerID()
		{
			BUILD_MSG_CONTEXT_VOID(IBridgeConnectorService::GetServerID);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(DWORD);

			return DWORD();     // 请检查默认返回值是否正确
		};

		/** 取得公共游戏世界ID
		*/
		virtual DWORD GetPublicWorldID()
		{
			BUILD_MSG_CONTEXT_VOID(IBridgeConnectorService::GetPublicWorldID);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(DWORD);

			return DWORD();     // 请检查默认返回值是否正确
		};

		/** 直接发送数据给跨区桥服务器
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void  SendData(ibuffer & msg)
		{
			BUILD_MSG_BUFFER( IBridgeConnectorService::SendData,msg);

			m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
		};

		////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

		/** 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
		*/
		virtual void SendDataToServer(DWORD dwServerID,ibuffer & msg,bool bZeroBroadcast)
		{
			obuf128 t_data;
			t_data << dwServerID<< bZeroBroadcast << msg ;

			BUILD_MSG_BUFFER( IBridgeConnectorService::SendDataToServer,t_data);

			m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
		}

		/** 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
		*/
		virtual void BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,ibuffer & msg)
		{
			obuf128 t_data;
			t_data << pServerArray << wServerNum << msg ;

			BUILD_MSG_BUFFER( IBridgeConnectorService::BroadcastDataToServer,t_data);

			m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
		}

		/**
		@name         : 发服务器扩展信息消息 dwServerID:服务器ID 为0时广播给所有桥服务器连接器,跨区桥服务器连接器同步服务器扩展信息专用
		@brief        :
		*/
		virtual bool SendExtDataToServer(DWORD dwServerID,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen)
		{
			obuf128 t_data;
			t_data << dwServerID << dwMsgCode << dwUserData1 << dwUserData2 << dwUserData3 << dwLen;
			t_data.push_back(pData, dwLen);

			BUILD_MSG_BUFFER( IBridgeConnectorService::SendExtDataToServer,t_data);

			m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);

			return bool();     // 请检查默认返回值是否正确
		};
		
		////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////
		/**  取得服务器ID列表
		@param   DWORD* pServerArray	: 保存找到的ID数组
		@param   WORD dwArrayLen		: 数组长度
		@param   WORD &dwGetCounts		: 找到的个数
		@param   DWORD dwWorldID		: 游戏世界ID，0为全部
		@param   WORD wServerType		: 服务器类型,0为全部
		@return  找到的个数
		*/
		virtual DWORD GetGameServerIDList(DWORD * pServerArray,DWORD dwArrayLen,DWORD &dwGetCounts,DWORD dwWorldID,WORD wServerType)
		{
			obuf256 t_data;
			t_data << dwArrayLen << dwGetCounts << dwWorldID << wServerType;
			t_data.push_back(pServerArray, dwArrayLen*sizeof(DWORD));

			BUILD_MSG_BUFFER( IBridgeConnectorService::GetGameServerIDList,t_data);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(DWORD);

			return DWORD();     // 请检查默认返回值是否正确
		};

		/**  取得指定游戏世界的指定类型的服务器ID
		@param   DWORD dwWorldID		: 游戏世界ID
		@param   WORD wServerType		: 服务器类型
		@param   WORD wSubID			: 服务器序号
		@return  找到的服务器ID,为0表示没找到
		*/
		virtual DWORD GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID)
		{
			BUILD_MSG_CONTEXT_3( IBridgeConnectorService::GetGameServerIDByTypeSubID,DWORD ,dwWorldID,WORD ,wServerType,WORD, wSubID);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(DWORD);

			return DWORD();     // 请检查默认返回值是否正确
		};


		////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

		/**  取得游戏世界ID列表
		@param   DWORD* pWorldArray	: 保存找到的ID数组
		@param   WORD dwArrayLen		: 数组长度
		@param   WORD &dwGetCounts		: 找到的个数
		@return  找到的个数
		*/
		virtual DWORD GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts)
		{
			obuf128 t_data;
			t_data << dwArrayLen << dwGetCounts;
			t_data.push_back(pWorldArray, dwArrayLen*sizeof(DWORD));

			BUILD_MSG_BUFFER( IBridgeConnectorService::GetGameWorldIDList,t_data);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(DWORD);

			return DWORD();     // 请检查默认返回值是否正确
		}

		/**  取得指定游戏世界信息
		@param   DWORD dwWorldID		: 游戏世界ID
		@return  找到的服务器ID,为0表示没找到
		*/
		virtual IBridgeWorldInfo * GetGameWorldInfo(DWORD dwWorldID)
		{
			BUILD_MSG_CONTEXT_1( IBridgeConnectorService::GetGameWorldInfo,DWORD ,dwWorldID);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(IBridgeWorldInfo *);

			return 0;     // 请检查默认返回值是否正确
		}

		////// 常用扩展信息类列表相关接口 ////////////////////////////////////////////////////////////////////

		/** 用服务器ID 取得扩展信息类
		@param   DWORD dwServerID		: 服务器ID
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart* GetGameServerPart(DWORD dwServerID)
		{
			BUILD_MSG_CONTEXT_1( IBridgeConnectorService::GetGameServerPart,DWORD ,dwServerID);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(IBridgePart *);

			return 0;     // 请检查默认返回值是否正确
		}

		/** 用TypeSubID取得扩展信息类
		@param   DWORD dwWorldID		: 游戏世界ID
		@param   WORD wServerType		: 服务器类型
		@param   WORD wSubID			: 服务器序号
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart* GetGameServerPartByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID)
		{
			BUILD_MSG_CONTEXT_3( IBridgeConnectorService::GetGameServerPartByTypeSubID,DWORD ,dwWorldID,WORD, wServerType,WORD, wSubID);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(IBridgePart *);

			return 0;     // 请检查默认返回值是否正确
		}

		/** 本服务器扩展信息类指针
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart* GetLocalBridgePart() 
		{
			BUILD_MSG_CONTEXT_VOID( IBridgeConnectorService::GetLocalBridgePart);

			rkt::obuf resultBuf;
			m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
			RETURN_FROM_MSG_BUF(IBridgePart *);

			return 0;     // 请检查默认返回值是否正确
		}

		// 通过指定服务器发中转消息数据给目标游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,DWORD dwTransitServerID 中转到服务器ID
		virtual void SendTransitDataToServer(DWORD dwServerID,DWORD dwTransitServerID,const char * pData,DWORD dwLen)
		{
			obuf128 t_data;
			t_data << dwServerID << dwTransitServerID << dwLen;
			t_data.push_back(pData, dwLen);

			BUILD_MSG_BUFFER( IBridgeConnectorService::SendTransitDataToServer,t_data);

			m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
		}
};



/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class BridgeConnectorService_Stub : public IServiceStub
{
	BridgeConnectorService_Proxy	m_proxy;
	BridgeConnectorService *		m_real_service;
    obuf							m_return_buffer;

public:
	BridgeConnectorService_Stub(BridgeConnectorService * pReal) : m_real_service(pReal){
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

		int nOpenBridge = 1;
		pConfig->QueryIntAttribute("enable",&nOpenBridge);

		return m_real_service->Start( ip,port, (nOpenBridge==1));
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
		
		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetServerID) )
		{
			if ( resultBuf ) *resultBuf << m_real_service->GetServerID();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetPublicWorldID) )
		{
			if ( resultBuf ) *resultBuf << m_real_service->GetPublicWorldID();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::SendData) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			m_real_service->SendData(in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::SendDataToServer) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			DWORD dwServerID; bool bZeroBroadcast;
			in >> dwServerID>>bZeroBroadcast;
			m_real_service->SendDataToServer(dwServerID, in, bZeroBroadcast);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::BroadcastDataToServer) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			DWORD * pServerArray; WORD wServerNum;
			in >> pServerArray >> wServerNum;
			m_real_service->BroadcastDataToServer(pServerArray,wServerNum, in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::SendExtDataToServer) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			DWORD dwServerID; DWORD dwMsgCode; DWORD dwUserData1; DWORD dwUserData2; DWORD dwUserData3; DWORD dwLen;
			in >> dwServerID >> dwMsgCode >> dwUserData1 >> dwUserData2 >> dwUserData3 >> dwLen;
			bool ret = m_real_service->SendExtDataToServer(dwServerID,dwMsgCode,dwUserData1,dwUserData2,dwUserData3, in.current(),dwLen);
			if ( resultBuf ) *resultBuf<<ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetGameServerIDList) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			DWORD dwArrayLen; DWORD dwGetCounts; DWORD dwWorldID; WORD wServerType;
			in >> dwArrayLen >> dwGetCounts >> dwWorldID >> wServerType;
			DWORD ret = m_real_service->GetGameServerIDList( (DWORD *)(in.current()),dwArrayLen,dwGetCounts,dwWorldID,wServerType);
			if ( resultBuf ) *resultBuf<<ret;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetGameServerIDByTypeSubID) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			DWORD dwWorldID; WORD wServerType; WORD wSubID;
			in >> dwWorldID >> wServerType >> wSubID;
			if ( resultBuf ) *resultBuf<< m_real_service->GetGameServerIDByTypeSubID(dwWorldID,wServerType,wSubID);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetGameWorldIDList) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			DWORD dwArrayLen, dwGetCounts;
			in >> dwArrayLen >> dwGetCounts;
			if ( resultBuf ) *resultBuf << m_real_service->GetGameWorldIDList((DWORD*)(in.current()), dwArrayLen, dwGetCounts);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetGameWorldInfo) )
		{
			DWORD dwWorldID;
			GET_MSG_PARAM_1(DWORD ,dwWorldID);
			if ( resultBuf ) *resultBuf << m_real_service->GetGameWorldInfo(dwWorldID);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetGameServerPart) )
		{
			DWORD dwServerID;
			GET_MSG_PARAM_1(DWORD ,dwServerID);
			if ( resultBuf ) *resultBuf << m_real_service->GetGameServerPart(dwServerID);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetGameServerPartByTypeSubID) )
		{
			DWORD dwWorldID;WORD wServerType; WORD wSubID;
			GET_MSG_PARAM_3(DWORD ,dwWorldID,WORD ,wServerType,WORD, wSubID);
			if ( resultBuf ) *resultBuf << m_real_service->GetGameServerPartByTypeSubID(dwWorldID, wServerType, wSubID);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::GetLocalBridgePart) )
		{
			if ( resultBuf ) *resultBuf << m_real_service->GetLocalBridgePart();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IBridgeConnectorService::SendTransitDataToServer) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			DWORD dwServerID;DWORD dwTransitServerID; DWORD dwLen;
			in >> dwServerID >> dwTransitServerID >> dwLen;
			m_real_service->SendTransitDataToServer(dwServerID,dwTransitServerID, in.current(), dwLen);
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
extern "C" __declspec(dllexport) IServiceStub * CreateBridgeConnectorService()
{
	return new BridgeConnectorService_Stub(new BridgeConnectorService);
}