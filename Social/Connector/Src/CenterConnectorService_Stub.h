/*******************************************************************
** 文件名:	CenterConnectorService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据ICenterConnectorService接口定义，自动生成的CenterConnectorService接口
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
#include "ICenterConnectorService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "CenterConnectorService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现ICenterConnectorService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class CenterConnectorService_Proxy : public ICenterConnectorService
{
public:
	SERVICE_PTR    m_pContainer;


	CenterConnectorService_Proxy() : m_pContainer(0) {
	}

	virtual ~CenterConnectorService_Proxy(){
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////

	/**
	@name                 : 连接中心服务器
	@brief                :
	@return               :
	*/
	virtual bool            connectServer(void)
    {
        if (m_pContainer == 0)
            return bool();

        BUILD_MSG_CONTEXT_VOID( ICenterConnectorService::connectServer );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( bool);

        return false;     // 请检查默认返回值是否正确
    }

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  updateServerInfo( ibuffer & info )
    {
        if (m_pContainer == 0)
            return bool();

	    obuf256 t_data;
        int size = info.size();
	    t_data << size;
        t_data.push_back( info.data(),info.size() );

		BUILD_MSG_BUFFER( ICenterConnectorService::updateServerInfo,t_data );
		return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual CSID  getLocalServerCSID(  )
    {
        if (m_pContainer == 0)
            return CSID();

       BUILD_MSG_CONTEXT_VOID( ICenterConnectorService::getLocalServerCSID );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( CSID);

        return 0;     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual DWORD  getAllServersCount(  )
    {
        if (m_pContainer == 0)
            return DWORD();

	    BUILD_MSG_CONTEXT_VOID( ICenterConnectorService::getAllServersCount );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( DWORD);

        return 0;     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual DWORD  getServerList( ServerData* serverPtrArray[],DWORD dwArratSize,CGID GroupFilter,WORD wTypeFilter )
    {
        if (m_pContainer == 0)
            return DWORD();

	    BUILD_MSG_CONTEXT_4( ICenterConnectorService::getServerList,ServerData**, serverPtrArray,DWORD ,dwArratSize,CGID ,GroupFilter,WORD, wTypeFilter );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( DWORD);

        return 0;     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual ServerData  getServerInfo( CSID ServerID )
    {
        if (m_pContainer == 0)
            return ServerData();

	    BUILD_MSG_CONTEXT_1( ICenterConnectorService::getServerInfo,CSID ,ServerID );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( ServerData);

	    return ServerData();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual WORD  getServerType( )
   {
       if (m_pContainer == 0)
           return WORD();

       BUILD_MSG_CONTEXT_VOID( ICenterConnectorService::getServerType );

       rkt::obuf resultBuf;
       m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
       RETURN_FROM_MSG_BUF( WORD);

       return WORD();     // 请检查默认返回值是否正确
   };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  postMessage( CSID DestServerID,void * data, int len )
    {
        if (m_pContainer == 0)
            return;

	    obuf256 t_data;
	    t_data << DestServerID << len;
		t_data.push_back(data, len);

		BUILD_MSG_BUFFER( ICenterConnectorService::postMessage,t_data );

		m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  broadcastMessage( string & id_buff,ibuffer & msg )
    {
        if (m_pContainer == 0)
            return;

	    obuf256 t_data;
	    t_data << id_buff<<msg;

		BUILD_MSG_BUFFER( ICenterConnectorService::broadcastMessage,t_data );

		m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    };

   virtual void  sendDataToZoneSvr(LPCSTR pData, size_t nLen, CSID ServerID = INVALID_SERVER_ID)
   {
       if (m_pContainer == 0)
           return;

	    obuf256 t_data;
	    t_data << nLen<<ServerID;
		t_data.push_back( pData,nLen);

		BUILD_MSG_BUFFER( ICenterConnectorService::sendDataToZoneSvr,t_data );

		m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
   }

    /** 向HttpServer服发送消息
    @param   
    @param   
    @return  
    */
    virtual void sendDataToDBHttpSvr(BYTE byKeyModule, BYTE byKeyAction, LPCSTR pData, size_t nLen)
    {
        if (m_pContainer == 0)
            return;

        obuf256 t_data;
        t_data << byKeyModule << byKeyAction << nLen;
        t_data.push_back( pData,nLen);

        BUILD_MSG_BUFFER( ICenterConnectorService::sendDataToDBHttpSvr,t_data );

        m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }
};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class CenterConnectorService_Stub : public IServiceStub
{
	CenterConnectorService_Proxy  m_proxy;
	CenterConnectorService *      m_real_service;
        obuf                  m_return_buffer;
public:
	CenterConnectorService_Stub(CenterConnectorService * pReal) 
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

		const char * ip = pConfig->Attribute("center_server_ip");
		if ( ip==0 )
		{
			ErrorLn(_GT("启动服务失败！缺少配置项[center_server_ip]"));
			return false;
		}

		int port = 0;
		pConfig->QueryIntAttribute("center_server_port",&port);
		if ( port==0 )
		{
			ErrorLn(_GT("启动服务失败！缺少配置项[center_server_port]"));
			return false;
		}

		return m_real_service->Start(ip,port);
	}

	// 停止
	virtual bool on_stop()
	{
		m_real_service->Stop();
        m_proxy.m_pContainer = SERVICE_PTR(0);
		return true;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
		////////////////////////////////////////////////////////////////////////////
		// 以下自动生成的消息解包，你需要检查
		
        if ( TEST_SAME_FUNCTION(ICenterConnectorService::connectServer) )
        {
            bool ret = m_real_service->connectServer();
			if ( resultBuf ) *resultBuf << ret;
            return true;
        }

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::updateServerInfo) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
            int size = 0;
            in >> size;
            ibuffer inbuf( in.current(),size );
			const bool & vReturn= m_real_service->updateServerInfo( inbuf );
			if ( resultBuf ) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::getLocalServerCSID) )
		{
			const CSID & vReturn= m_real_service->getLocalServerCSID();
			if ( resultBuf ) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::getAllServersCount) )
		{
			const DWORD & vReturn= m_real_service->getAllServersCount();
			if ( resultBuf ) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::getServerList) )
		{
			ServerData** serverPtrArray;DWORD dwArratSize;CGID GroupFilter;WORD wTypeFilter;
			GET_MSG_PARAM_4(ServerData** ,serverPtrArray,DWORD, dwArratSize,CGID ,GroupFilter,WORD ,wTypeFilter);

			const DWORD & vReturn= m_real_service->getServerList(serverPtrArray,dwArratSize,GroupFilter,wTypeFilter);
			if ( resultBuf ) *resultBuf << vReturn;
			return true;
		}
		if ( TEST_SAME_FUNCTION(ICenterConnectorService::getServerInfo) )
		{
			CSID ServerID;
			GET_MSG_PARAM_1(CSID,ServerID);

			const ServerData & vReturn= m_real_service->getServerInfo(ServerID);
			if ( resultBuf ) *resultBuf << vReturn;
			return true;
		}
        if ( TEST_SAME_FUNCTION(ICenterConnectorService::getServerType) )
        {
            const DWORD & vReturn= m_real_service->getServerType();
            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }
        

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::postMessage) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			CSID DestServerID; int size = 0;
			in >> DestServerID >> size;

			m_real_service->postMessage(DestServerID, in.current(), size);
			return true;
		}
		if ( TEST_SAME_FUNCTION(ICenterConnectorService::broadcastMessage) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			string id_buff;
			in >> id_buff;
			m_real_service->broadcastMessage(id_buff,in);
			return true;
		}

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::sendDataToZoneSvr) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			size_t nLen = 0; CSID ServerID = INVALID_SERVER_ID;
			in >> nLen >> ServerID;
			m_real_service->sendDataToZoneSvr(in.current(), nLen, ServerID);
			return true;
		}

        if ( TEST_SAME_FUNCTION(ICenterConnectorService::sendDataToDBHttpSvr) )
        {
            rkt::ibuffer in(pMsg->context,pMsg->context_len );
            BYTE byKeyModule; BYTE byKeyAction;size_t nLen = 0;
            in >> byKeyModule >> byKeyAction >> nLen;
            m_real_service->sendDataToDBHttpSvr(byKeyModule, byKeyAction, in.current(), nLen);
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
extern "C" __declspec(dllexport) IServiceStub * CreateCenterConnectorService()
{
	return new CenterConnectorService_Stub(new CenterConnectorService);
}