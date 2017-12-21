#pragma once

#include "IServiceContainer.h"
#include "IVoiceConnectorService.h"
#include "buffer.h"
#include "tinyxml.h"
#include "IVoiceConnector.h"

// 这个是服务实现类，你需要自己实现
#include "VoiceConnectorService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现IVoiceConnectorService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class VoiceConnectorService_Proxy : public IVoiceConnectorService
{
public:
	SERVICE_PTR    m_pContainer;
	VoiceConnectorService *      m_real_service;

	VoiceConnectorService_Proxy() : m_real_service(0) {
	}

	virtual ~VoiceConnectorService_Proxy(){
	}

     /**
    @name                : 连接语音服
    @brief  
	*/
	virtual bool connectServer(void)
    {
        BUILD_MSG_CONTEXT_VOID( IVoiceConnectorService::connectServer );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( bool);

        return false;     // 请检查默认返回值是否正确
    }

	/**
	@name                : 直接发送数据给语音服务器
	@brief               : 
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  SendData(const char * pData,DWORD dwLen)
    {
        obuf256 t_data;
        t_data << dwLen;
        t_data.push_back(pData, dwLen);

        BUILD_MSG_BUFFER( IVoiceConnectorService::SendData,t_data );

        m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }


	/** 注册消息处理者
	@param moduleId 模块ID，参考InputMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual bool registerMessageHandler(DWORD moduleId, IVoiceMessageHandler* handler)
    {
        BUILD_MSG_CONTEXT_2( IVoiceConnectorService::registerMessageHandler,DWORD ,moduleId, IVoiceMessageHandler*, handler );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( bool);

        return bool();     // 请检查默认返回值是否正确
    }

	/** 注销消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual bool unregisterMessageHandler(DWORD moduleId)
    {
        BUILD_MSG_CONTEXT_1( IVoiceConnectorService::unregisterMessageHandler,DWORD ,moduleId );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( bool);

        return bool();     // 请检查默认返回值是否正确
    }

	/** 注册事件处理者
	@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
	@param hander 事件处理者实例
	*/
	virtual bool registerEventHandler(WORD wEventID,IVoiceEventHandler* handler)
    {
        BUILD_MSG_CONTEXT_2( IVoiceConnectorService::registerEventHandler,WORD ,wEventID, IVoiceEventHandler*, handler );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( bool);

        return bool();     // 请检查默认返回值是否正确
    }

	/** 注销事件处理者
	@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
	@param  hander 事件处理者实例
	*/
	virtual bool unregisterEventHandler(WORD wEventID,IVoiceEventHandler* handler)
    {
        BUILD_MSG_CONTEXT_2( IVoiceConnectorService::unregisterEventHandler,WORD ,wEventID, IVoiceEventHandler*, handler);

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( bool);

        return bool();     // 请检查默认返回值是否正确
    }


	////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

	// 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
	virtual void SendDataToServer(DWORD dwServerID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false)
    {
        obuf256 t_data;
        t_data << dwServerID << bZeroBroadcast << dwLen;
        t_data.push_back(pData, dwLen);

        BUILD_MSG_BUFFER( IVoiceConnectorService::SendDataToServer,t_data );

        m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

	// 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
	virtual void BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,const char * pData,DWORD dwLen)
    {
        obuf256 t_data;
        t_data << wServerNum << dwLen;
        t_data.push_back(pServerArray, sizeof(DWORD) * wServerNum);
        t_data.push_back(pData, dwLen);

        BUILD_MSG_BUFFER( IVoiceConnectorService::BroadcastDataToServer,t_data );

        m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

    /** 广播数据到客户端
	@param	pData		: 数据
	@param	dwLen		: 数据长度
	@param	nBroadcastType : 广播类型 EMVoiceChannelType
	@param	dwPDBID1/dwPDBID2 : 用户PDBID
	@param	dwWorldID1/dwWorldID2 : 世界ID，
	@param  dwValueID1/dwValueID2 : ID值,参考SVoiceChannelID
	@param  bRoom : 是否广播用户dwPDBID1所在房间
	*/
	virtual void BroadcastDataToClient( BYTE nBroadcastType, void * pData, DWORD dwLen, DWORD dwPDBID1 = 0, DWORD dwPDBID2 = 0, DWORD dwWorldID1 = 0, DWORD dwWorldID2 = 0, DWORD dwValueID1 = 0, DWORD dwValueID2 = 0, bool bRoom = false )
    {
        obuf256 t_data;
        t_data << nBroadcastType << dwPDBID1 << dwPDBID2 << dwWorldID1 << dwWorldID2 << dwValueID1 << dwValueID2 << bRoom;
        t_data << dwLen;
        t_data.push_back(pData, dwLen);

        BUILD_MSG_BUFFER( IVoiceConnectorService::BroadcastDataToClient,t_data );

        m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }
};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class VoiceConnectorService_Stub : public IServiceStub
{
	VoiceConnectorService_Proxy  m_proxy;
	VoiceConnectorService *      m_real_service;
    
public:
	VoiceConnectorService_Stub(VoiceConnectorService * pReal) : m_real_service(pReal){
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;
		m_proxy.m_real_service = m_real_service;

        TiXmlElement * pConfig = (TiXmlElement *)pContainer->get_scheme().scheme->pExtend;
        if ( pConfig==0 )
            return false;

        const char * ip = pConfig->Attribute("voice_server_ip");
        if ( ip==0 )
        {
            ErrorLn(_GT("启动服务失败！缺少配置项[voice_server_ip]"));
            return false;
        }

        int port = 0;
        pConfig->QueryIntAttribute("voice_server_port",&port);
        if ( port==0 )
        {
            ErrorLn(_GT("启动服务失败！缺少配置项[voice_server_port]"));
            return false;
        }

		return m_real_service->Start(ip, port);
	}

	// 停止
	virtual bool on_stop()
	{
		m_real_service->Stop();
        m_proxy.m_pContainer = SERVICE_PTR(0);
        return true;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf  )
	{
        ////////////////////////////////////////////////////////////////////////////
        // 以下自动生成的消息解包，你需要检查

		if ( TEST_SAME_FUNCTION(IVoiceConnectorService::connectServer) )
		{
            bool ret = m_real_service->connectServer();
            if ( resultBuf ) *resultBuf << ret;
            return true;
		}

		if ( TEST_SAME_FUNCTION(IVoiceConnectorService::SendData) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
            DWORD dwLen = 0;
            in >> dwLen;

            m_real_service->SendData(in.current(), dwLen);
			
			return true;
		}

        if ( TEST_SAME_FUNCTION(IVoiceConnectorService::registerMessageHandler) )
        {
            DWORD moduleId; IVoiceMessageHandler* handler;
            GET_MSG_PARAM_2( DWORD, moduleId,IVoiceMessageHandler*, handler );
            bool vReturn = m_real_service->registerMessageHandler(moduleId, handler);
            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IVoiceConnectorService::unregisterMessageHandler) )
        {
            DWORD moduleid;
            GET_MSG_PARAM_1( DWORD, moduleid );

            bool vReturn = m_real_service->unregisterMessageHandler(moduleid);

            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IVoiceConnectorService::registerEventHandler) )
        {
            WORD wEventID; IVoiceEventHandler* handler;
            GET_MSG_PARAM_2( WORD, wEventID,  IVoiceEventHandler*, handler);

            bool vReturn = m_real_service->registerEventHandler(wEventID, handler);

            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IVoiceConnectorService::unregisterEventHandler) )
        {
            WORD wEventID; IVoiceEventHandler* handler;
            GET_MSG_PARAM_2( WORD, wEventID,  IVoiceEventHandler*, handler);

            bool vReturn = m_real_service->unregisterEventHandler(wEventID, handler);

            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IVoiceConnectorService::SendDataToServer) )
        {
            rkt::ibuffer in(pMsg->context,pMsg->context_len );
            DWORD dwServerID; bool bZeroBroadcast; DWORD dwLen;
            in >> dwServerID >> bZeroBroadcast >> dwLen;

            m_real_service->SendDataToServer(dwServerID, in.current(), dwLen, bZeroBroadcast);
			return true;
        }

        if ( TEST_SAME_FUNCTION(IVoiceConnectorService::BroadcastDataToServer) )
        {
            rkt::ibuffer in(pMsg->context,pMsg->context_len );
            WORD wServerNum; DWORD dwLen;
            in >> wServerNum >> dwLen;

            DWORD* pServerArray = (DWORD*)in.current();
            in.offset(sizeof(DWORD) * wServerNum);
            m_real_service->BroadcastDataToServer(pServerArray, wServerNum, in.current(), dwLen);

            return true;
        }

        if ( TEST_SAME_FUNCTION(IVoiceConnectorService::BroadcastDataToClient) )
        {
            rkt::ibuffer in(pMsg->context,pMsg->context_len );

            BYTE nBroadcastType; DWORD dwLen; DWORD dwPDBID1; DWORD dwPDBID2; DWORD dwWorldID1;
            DWORD dwWorldID2; DWORD dwValueID1; DWORD dwValueID2; bool bRoom ;

            in >> nBroadcastType >> dwPDBID1 >> dwPDBID2 >> dwWorldID1 >> dwWorldID2 >> dwValueID1 >> dwValueID2 >> bRoom >> dwLen;
            
            m_real_service->BroadcastDataToClient(nBroadcastType, in.current(), dwLen, dwPDBID1, dwPDBID2, dwWorldID1, dwWorldID2, dwValueID1, dwValueID2, bRoom);

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
extern "C" __declspec(dllexport) IServiceStub * CreateVoiceConnectorService()
{
	return new VoiceConnectorService_Stub(new VoiceConnectorService);
}