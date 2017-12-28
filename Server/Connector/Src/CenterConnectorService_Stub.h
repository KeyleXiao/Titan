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


   // 获取静态地图所在的场景服ID
   virtual ServerData getZoneServerOfStaticMap(int nMapID)
   {
       if (m_pContainer == 0)
           return ServerData();

	    BUILD_MSG_CONTEXT_1( ICenterConnectorService::getZoneServerOfStaticMap,int, nMapID );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( ServerData);

		return ServerData();     // 请检查默认返回值是否正确
   }

   // 获取场景序列号
   virtual int getSceneSerialNo(CSID server, int nSceneID)
   {
       if (m_pContainer == 0)
           return int();

	    BUILD_MSG_CONTEXT_2( ICenterConnectorService::getSceneSerialNo,CSID ,server, int, nSceneID );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF( int);

		return LargeID();     // 请检查默认返回值是否正确
   }

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

	/** 发送数据给所有除网关以外的服务器（比如登陆服，社会服）
	@param   bCloseSelf ：是将本场景服排除
	@param   
	@return  
	*/
	virtual void sendToAllServerForgetGateSvr(void* pData, int nLen, bool bCloseSelf)
	{
        if (m_pContainer == 0)
            return;

		obuf256 t_data;
		t_data << nLen<<bCloseSelf;
		if (pData && nLen > 0)
		{
			t_data.push_back(pData, nLen);
		}

		BUILD_MSG_BUFFER( ICenterConnectorService::sendToAllServerForgetGateSvr,t_data );

		m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
	}

	/** 发送数据给本区社会服
	@param   nModule：	模块ID
	@param   nMsgID：	消息ID 
	@param   pData：	数据
	@param   nLen：		长度
	@return  
	*/
	virtual void sendToSocial(BYTE nModule,BYTE nMsgID,const char * pData, size_t nLen)
	{
        if (m_pContainer == 0)
            return;

		obuf256 t_data;
		t_data << nModule<<nMsgID << nLen;
		t_data.push_back(pData, nLen);

		BUILD_MSG_BUFFER( ICenterConnectorService::sendToSocial,t_data );

		m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
	}

    void         sendToLoginServer(BYTE nModule, BYTE nMsgID, const char * pData, size_t nLen)
    {
        if (m_pContainer == 0)
            return;

        obuf256 t_data;
        t_data << nModule<<nMsgID << nLen;
        t_data.push_back(pData, nLen);

		BUILD_MSG_BUFFER( ICenterConnectorService::sendToLoginServer,t_data );

		m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

    /**
	@name                : 直接发送数据给世界服务器
	@brief               : 因为世界服务器上挂接了部分游戏逻辑模块,这些模块可能需要以自己的格式进行通讯
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	void            sendDataToWorldServer(const char * pData, size_t nLen)
    {
        if (m_pContainer == 0)
            return;

        obuf256 t_data;
        t_data << nLen;
        t_data.push_back(pData, nLen);

        BUILD_MSG_BUFFER( ICenterConnectorService::sendDataToWorldServer,t_data );

        m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

	/** 发送数据给本区全场景服
	@param   bCloseSelf ：是将本场景排除
	@param   
	@return  
	*/
	virtual void sendToSceneSvr(LPCSTR pData, size_t nLen, bool bCloseSelf)
	{
        if (m_pContainer == 0)
            return;

		obuf256 t_data;
		t_data << nLen<<bCloseSelf;
		t_data.push_back(pData, nLen);

		BUILD_MSG_BUFFER( ICenterConnectorService::sendToSceneSvr,t_data );

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
        t_data.push_back(pData, nLen);

        BUILD_MSG_BUFFER(ICenterConnectorService::sendDataToDBHttpSvr, t_data);

        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
    }

	/** 根据服务器ID 发送数据给某服务器
	@param   
	@param   
	@return  
	*/
	virtual void sendToServerByServerId(DWORD dwServerId, LPCSTR pData, size_t nLen)
	{
        if (m_pContainer == 0)
            return;

		obuf256 t_data;
		t_data << dwServerId<<nLen;
		t_data.push_back(pData, nLen);

		BUILD_MSG_BUFFER( ICenterConnectorService::sendToServerByServerId,t_data );

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
	CenterConnectorService_Stub(CenterConnectorService * pReal) : m_real_service(pReal){
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

		return m_real_service->Start( ip,port );
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

        if ( TEST_SAME_FUNCTION(ICenterConnectorService::getZoneServerOfStaticMap) )
        {
            int nMap;
            GET_MSG_PARAM_1(int ,nMap);
            const ServerData & vReturn= m_real_service->getZoneServerOfStaticMap(nMap);
            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(ICenterConnectorService::getSceneSerialNo) )
        {
            CSID server; int nSceneID;
            GET_MSG_PARAM_2(CSID, server,int, nSceneID);
            const int & vReturn= m_real_service->getSceneSerialNo(server, nSceneID);
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

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::sendToAllServerForgetGateSvr) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			int nLen = 0; bool bCloseSelf = false;
			in >> nLen >> bCloseSelf;
			m_real_service->sendToAllServerForgetGateSvr(in.current(),nLen, bCloseSelf);
			return true;
		}

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::sendToSocial) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			BYTE nModule = 0; BYTE nMsgID=0; size_t nLen = 0;
			in >> nModule >> nMsgID >> nLen;
			m_real_service->sendToSocial(nModule,nMsgID,in.current(),nLen);
			return true;
		}

        if ( TEST_SAME_FUNCTION(ICenterConnectorService::sendToLoginServer) )
        {
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
            BYTE nModule = 0; BYTE nMsgID=0; size_t nLen = 0;
            in >> nModule >> nMsgID >> nLen;
            m_real_service->sendToLoginServer(nModule,nMsgID,in.current(),nLen);
            return true;
        }

        if ( TEST_SAME_FUNCTION(ICenterConnectorService::sendDataToWorldServer) )
        {
            rkt::ibuffer in(pMsg->context,pMsg->context_len );
            size_t nLen = 0;
            in >> nLen;
            m_real_service->sendDataToWorldServer(in.current(),nLen);
            return true;
        }

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::sendToSceneSvr) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			size_t nLen = 0; bool bCloseSelf=false;
			in >> nLen >> bCloseSelf;
			m_real_service->sendToSceneSvr(in.current(),nLen, bCloseSelf);
			return true;
		}

        if (TEST_SAME_FUNCTION(ICenterConnectorService::sendDataToDBHttpSvr))
        {
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            BYTE byKeyModule; BYTE byKeyAction; size_t nLen;
            in >> byKeyModule >> byKeyAction >> nLen;
            m_real_service->sendDataToDBHttpSvr(byKeyModule, byKeyAction, in.current(), nLen);
            return true;
        }

		if ( TEST_SAME_FUNCTION(ICenterConnectorService::sendToServerByServerId) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			DWORD dwServerId = 0; size_t nLen = 0;
			in >> dwServerId >> nLen;
			m_real_service->sendToServerByServerId(dwServerId, in.current(),nLen);
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
extern "C" __declspec(dllexport) IServiceStub * CreateCenterConnectorService()
{
	return new CenterConnectorService_Stub(new CenterConnectorService);
}