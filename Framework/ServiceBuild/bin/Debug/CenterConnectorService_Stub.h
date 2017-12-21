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


	CenterConnectorService_Proxy() {
	}

	virtual ~CenterConnectorService_Proxy(){
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	
   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  UpdateServerInfo( ibuffer info )
    {
	    obuf256 t_data;
	    t_data << "UpdateServerInfo";
	    t_data << info;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    bool vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual CSID  GetLocalServerCSID(  )
    {
	    obuf256 t_data;
	    t_data << "GetLocalServerCSID";
	    //t_data << ;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    CSID vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return CSID();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual DWORD  GetAllServersCount(  )
    {
	    obuf256 t_data;
	    t_data << "GetAllServersCount";
	    //t_data << ;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    DWORD vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return DWORD();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual DWORD  GetServerList( ServerData** serverPtrArray,DWORD dwArratSize,CGID GroupFilter,WORD wTypeFilter )
    {
	    obuf256 t_data;
	    t_data << "GetServerList";
	    t_data << serverPtrArray<<dwArratSize<<GroupFilter<<wTypeFilter;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    DWORD vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return DWORD();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual ServerData  GetServerInfo( CSID ServerID )
    {
	    obuf256 t_data;
	    t_data << "GetServerInfo";
	    t_data << ServerID;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    ServerData vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return ServerData();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual ServerData  FindZoneServer( CGID groupID,DWORD wMapID,DWORD x,DWORD y )
    {
	    obuf256 t_data;
	    t_data << "FindZoneServer";
	    t_data << groupID<<wMapID<<x<<y;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    ServerData vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return ServerData();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  PostMessage( CSID DestServerID,ibuffer msg )
    {
	    obuf256 t_data;
	    t_data << "PostMessage";
	    t_data << DestServerID<<msg;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    bool vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  BroadcastMessage( ibuffer id_buff,ibuffer msg )
    {
	    obuf256 t_data;
	    t_data << "BroadcastMessage";
	    t_data << id_buff<<msg;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    bool vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return bool();     // 请检查默认返回值是否正确
    };

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

		return m_real_service->on_start();
	}

	// 停止
	virtual bool on_stop()
	{
		return m_real_service->on_stop();
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,void *& pResult,int & nResultLen )
	{
		if ( pMsg->context_len==0 || pMsg->context==0 )
			return false;

		ibuffer in(pMsg->context,pMsg->context_len);
		const char * function_name;
		in >> function_name;

                ////////////////////////////////////////////////////////////////////////////
                // 以下自动生成的消息解包，你需要检查
		
		if ( strcmp(function_name,"UpdateServerInfo")==0 )
		{
			ibuffer info;
			in >> info;
			const bool & vReturn= m_real_service->UpdateServerInfo(info);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"GetLocalServerCSID")==0 )
		{
			;
			//in >> ;
			const CSID & vReturn= m_real_service->GetLocalServerCSID();
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"GetAllServersCount")==0 )
		{
			;
			//in >> ;
			const DWORD & vReturn= m_real_service->GetAllServersCount();
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"GetServerList")==0 )
		{
			ServerData** serverPtrArray;DWORD dwArratSize;CGID GroupFilter;WORD wTypeFilter;
			in >> serverPtrArray>>dwArratSize>>GroupFilter>>wTypeFilter;
			const DWORD & vReturn= m_real_service->GetServerList(serverPtrArray,dwArratSize,GroupFilter,wTypeFilter);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"GetServerInfo")==0 )
		{
			CSID ServerID;
			in >> ServerID;
			const ServerData & vReturn= m_real_service->GetServerInfo(ServerID);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"FindZoneServer")==0 )
		{
			CGID groupID;DWORD wMapID;DWORD x;DWORD y;
			in >> groupID>>wMapID>>x>>y;
			const ServerData & vReturn= m_real_service->FindZoneServer(groupID,wMapID,x,y);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"PostMessage")==0 )
		{
			CSID DestServerID;ibuffer msg;
			in >> DestServerID>>msg;
			const bool & vReturn= m_real_service->PostMessage(DestServerID,msg);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"BroadcastMessage")==0 )
		{
			ibuffer id_buff;ibuffer msg;
			in >> id_buff>>msg;
			const bool & vReturn= m_real_service->BroadcastMessage(id_buff,msg);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}

		ErrorLn("can not found function name:" << function_name);
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