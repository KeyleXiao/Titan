/*******************************************************************
** 文件名:	NPCService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据INPCService接口定义，自动生成的NPCService接口
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
#include "INPCService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "NPCService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现INPCService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class NPCService_Proxy : public INPCService
{
public:
	SERVICE_PTR    m_pContainer;


	NPCService_Proxy() {
	}

	virtual ~NPCService_Proxy(){
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	
   //////////////////////////////////////////////////////////////////////////////////
   virtual UID  createNPC( int id,Vector3 loc,Void* context,int len )
    {
	    obuf256 t_data;
	    t_data << "createNPC";
	    t_data << id<<loc<<context<<len;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    UID vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return UID();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  destroyNPC( UID uid )
    {
	    obuf256 t_data;
	    t_data << "destroyNPC";
	    t_data << uid;

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
   virtual string  getName( UID uid )
    {
	    obuf256 t_data;
	    t_data << "getName";
	    t_data << uid;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    string vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return string();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getSceneID(  )
    {
	    obuf256 t_data;
	    t_data << "getSceneID";
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
   virtual Vector3  getLocation( UID uid )
    {
	    obuf256 t_data;
	    t_data << "getLocation";
	    t_data << uid;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    Vector3 vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return Vector3();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  sendCommand( UID uid,int cmdid,Void* context,int len )
    {
	    obuf256 t_data;
	    t_data << "sendCommand";
	    t_data << uid<<cmdid<<context<<len;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual string  getPropertyType( UID uid,int prop_id )
    {
	    obuf256 t_data;
	    t_data << "getPropertyType";
	    t_data << uid<<prop_id;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    string vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return string();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getPropertyFlag( UID uid,int prop_id )
    {
	    obuf256 t_data;
	    t_data << "getPropertyFlag";
	    t_data << uid<<prop_id;

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
   virtual bool  getProperty_Bool( UID uid,int prop_id )
    {
	    obuf256 t_data;
	    t_data << "getProperty_Bool";
	    t_data << uid<<prop_id;

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
   virtual bool  setProperty_Bool( UID uid,int prop_id,bool value )
    {
	    obuf256 t_data;
	    t_data << "setProperty_Bool";
	    t_data << uid<<prop_id<<value;

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
   virtual int  getProperty_Integer( UID uid,int prop_id )
    {
	    obuf256 t_data;
	    t_data << "getProperty_Integer";
	    t_data << uid<<prop_id;

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
   virtual bool  setProperty_Integer( UID uid,int prop_id,int value )
    {
	    obuf256 t_data;
	    t_data << "setProperty_Integer";
	    t_data << uid<<prop_id<<value;

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
   virtual bool  addProperty_Integer( UID uid,int prop_id,int delta )
    {
	    obuf256 t_data;
	    t_data << "addProperty_Integer";
	    t_data << uid<<prop_id<<delta;

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
   virtual string  getProperty_String( UID uid,int prop_id )
    {
	    obuf256 t_data;
	    t_data << "getProperty_String";
	    t_data << uid<<prop_id;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    string vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return string();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_String( UID uid,int prop_id,string value )
    {
	    obuf256 t_data;
	    t_data << "setProperty_String";
	    t_data << uid<<prop_id<<value;

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
   virtual string  getProperty_custom( UID uid,int prop_id )
    {
	    obuf256 t_data;
	    t_data << "getProperty_custom";
	    t_data << uid<<prop_id;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	    if ( pReturn!=0 && nReturnLen >0 )
	    {
		    ibuffer t_in(pReturn,nReturnLen);
		    string vReturn;
		    t_in >> vReturn;
		    if ( t_in.good() ) return vReturn;
	    }

	    return string();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_custom( int prop_id,string value )
    {
	    obuf256 t_data;
	    t_data << "setProperty_custom";
	    t_data << prop_id<<value;

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
   virtual int  getSight( UID uid )
    {
	    obuf256 t_data;
	    t_data << "getSight";
	    t_data << uid;

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
   virtual void  setSight( UID uid,int radii )
    {
	    obuf256 t_data;
	    t_data << "setSight";
	    t_data << uid<<radii;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  isHide( UID uid )
    {
	    obuf256 t_data;
	    t_data << "isHide";
	    t_data << uid;

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
   virtual void  setHide( UID uid,bool state )
    {
	    obuf256 t_data;
	    t_data << "setHide";
	    t_data << uid<<state;

	    void * pReturn = 0;
	    int nReturnLen = 0;
	    m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
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
class NPCService_Stub : public IServiceStub
{
	NPCService_Proxy  m_proxy;
	NPCService *      m_real_service;
        obuf                  m_return_buffer;
public:
	NPCService_Stub(NPCService * pReal) : m_real_service(pReal){
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
		
		if ( strcmp(function_name,"createNPC")==0 )
		{
			int id;Vector3 loc;Void* context;int len;
			in >> id>>loc>>context>>len;
			const UID & vReturn= m_real_service->createNPC(id,loc,context,len);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"destroyNPC")==0 )
		{
			UID uid;
			in >> uid;
			const bool & vReturn= m_real_service->destroyNPC(uid);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getName")==0 )
		{
			UID uid;
			in >> uid;
			const string & vReturn= m_real_service->getName(uid);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getSceneID")==0 )
		{
			;
			//in >> ;
			const int & vReturn= m_real_service->getSceneID();
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getLocation")==0 )
		{
			UID uid;
			in >> uid;
			const Vector3 & vReturn= m_real_service->getLocation(uid);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"sendCommand")==0 )
		{
			UID uid;int cmdid;Void* context;int len;
			in >> uid>>cmdid>>context>>len;
			m_real_service->sendCommand(uid,cmdid,context,len);
			return true;
		}

		if ( strcmp(function_name,"getPropertyType")==0 )
		{
			UID uid;int prop_id;
			in >> uid>>prop_id;
			const string & vReturn= m_real_service->getPropertyType(uid,prop_id);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getPropertyFlag")==0 )
		{
			UID uid;int prop_id;
			in >> uid>>prop_id;
			const int & vReturn= m_real_service->getPropertyFlag(uid,prop_id);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getProperty_Bool")==0 )
		{
			UID uid;int prop_id;
			in >> uid>>prop_id;
			const bool & vReturn= m_real_service->getProperty_Bool(uid,prop_id);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"setProperty_Bool")==0 )
		{
			UID uid;int prop_id;bool value;
			in >> uid>>prop_id>>value;
			const bool & vReturn= m_real_service->setProperty_Bool(uid,prop_id,value);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getProperty_Integer")==0 )
		{
			UID uid;int prop_id;
			in >> uid>>prop_id;
			const int & vReturn= m_real_service->getProperty_Integer(uid,prop_id);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"setProperty_Integer")==0 )
		{
			UID uid;int prop_id;int value;
			in >> uid>>prop_id>>value;
			const bool & vReturn= m_real_service->setProperty_Integer(uid,prop_id,value);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"addProperty_Integer")==0 )
		{
			UID uid;int prop_id;int delta;
			in >> uid>>prop_id>>delta;
			const bool & vReturn= m_real_service->addProperty_Integer(uid,prop_id,delta);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getProperty_String")==0 )
		{
			UID uid;int prop_id;
			in >> uid>>prop_id;
			const string & vReturn= m_real_service->getProperty_String(uid,prop_id);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"setProperty_String")==0 )
		{
			UID uid;int prop_id;string value;
			in >> uid>>prop_id>>value;
			const bool & vReturn= m_real_service->setProperty_String(uid,prop_id,value);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getProperty_custom")==0 )
		{
			UID uid;int prop_id;
			in >> uid>>prop_id;
			const string & vReturn= m_real_service->getProperty_custom(uid,prop_id);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"setProperty_custom")==0 )
		{
			int prop_id;string value;
			in >> prop_id>>value;
			const bool & vReturn= m_real_service->setProperty_custom(prop_id,value);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"getSight")==0 )
		{
			UID uid;
			in >> uid;
			const int & vReturn= m_real_service->getSight(uid);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"setSight")==0 )
		{
			UID uid;int radii;
			in >> uid>>radii;
			m_real_service->setSight(uid,radii);
			return true;
		}

		if ( strcmp(function_name,"isHide")==0 )
		{
			UID uid;
			in >> uid;
			const bool & vReturn= m_real_service->isHide(uid);
			m_return_buffer.clear();
			m_return_buffer << vReturn;
			pResult = m_return_buffer.data();
			nResultLen = m_return_buffer.size();
			return true;
		}
		if ( strcmp(function_name,"setHide")==0 )
		{
			UID uid;bool state;
			in >> uid>>state;
			m_real_service->setHide(uid,state);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateNPCService()
{
	return new NPCService_Stub(new NPCService);
}