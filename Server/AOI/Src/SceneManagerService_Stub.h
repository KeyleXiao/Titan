/*******************************************************************
** 文件名:	SceneManagerService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据ISceneManagerService接口定义，自动生成的SceneManagerService接口
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
#include "ISceneManagerService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "SceneManagerService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现ISceneManagerService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class SceneManagerService_Proxy : public ISceneManagerService
{
public:
	SERVICE_PTR    m_pContainer;


	SceneManagerService_Proxy() {
	}

	virtual ~SceneManagerService_Proxy(){
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	
   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  Load(  )
    {
	    obuf256 t_data;
	    t_data << "Load";
	    //t_data << ;

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
   virtual int  createScene( int mapID )
    {
	    obuf256 t_data;
	    t_data << "createScene";
	    t_data << mapID;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    int vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return int();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  closeScene( int sceneID )
    {
	    obuf256 t_data;
	    t_data << "closeScene";
	    t_data << sceneID;

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
   virtual SERVICE_PTR  getScene( int sceneID )
    {
	    obuf256 t_data;
	    t_data << "getScene";
	    t_data << sceneID;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    SERVICE_PTR vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return SERVICE_PTR();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getSceneCount(  )
    {
	    obuf256 t_data;
	    t_data << "getSceneCount";
	    //t_data << ;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    int vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return int();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getAllScenes( SERVICE_PTR* pReturnArray,int nArraySize )
    {
	    obuf256 t_data;
	    t_data << "getAllScenes";
	    t_data << pReturnArray<<nArraySize;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    int vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return int();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  release(  )
    {
	    obuf256 t_data;
	    t_data << "release";
	    //t_data << ;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
    };

};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class SceneManagerService_Stub : public IServiceStub
{
	SceneManagerService_Proxy  m_proxy;
	SceneManagerService *      m_real_service;
        obuf                  m_return_buffer;
public:
	SceneManagerService_Stub(SceneManagerService * pReal) : m_real_service(pReal){
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
		
		if ( strcmp(function_name,"Load")==0 )
		{
			;
			//in >> ;
			const bool & vReturn= m_real_service->Load();
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"createScene")==0 )
		{
			int mapID;
			in >> mapID;
			const int & vReturn= m_real_service->createScene(mapID);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"closeScene")==0 )
		{
			int sceneID;
			in >> sceneID;
			const bool & vReturn= m_real_service->closeScene(sceneID);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getScene")==0 )
		{
			int sceneID;
			in >> sceneID;
			const SERVICE_PTR & vReturn= m_real_service->getScene(sceneID);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getSceneCount")==0 )
		{
			;
			//in >> ;
			const int & vReturn= m_real_service->getSceneCount();
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getAllScenes")==0 )
		{
			SERVICE_PTR* pReturnArray;int nArraySize;
			in >> pReturnArray>>nArraySize;
			const int & vReturn= m_real_service->getAllScenes(pReturnArray,nArraySize);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"release")==0 )
		{
			;
			//in >> ;
			m_real_service->release();
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

