/*******************************************************************
** 文件名:	%SERVICE_NAME%_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据I%SERVICE_NAME%接口定义，自动生成的%SERVICE_NAME%接口
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
#include "IServiceMgr.h"
#include "buffer.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现I%SERVICE_NAME%接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class %SERVICE_NAME%_Proxy : public I%SERVICE_NAME%
{
public:
	IServiceContainer *    m_pContainer;


	%SERVICE_NAME%_Proxy() : m_pContainer(0){
	}

	virtual ~%SERVICE_NAME%_Proxy(){
	}

	%SERVICE_SEGMENT_1%
};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class %SERVICE_NAME%_Stub : public IServiceStub
{
	%SERVICE_NAME%_Proxy  m_proxy;
	I%SERVICE_NAME% *     m_real_service;

public:
	%SERVICE_NAME%_Stub(I%SERVICE_NAME% * pReal) : m_real_service(pReal){
	}

	// 启动
	virtual bool on_start( IServiceContainer * pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		return true;
	}

	// 停止
	virtual bool on_stop()
	{
		return true;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,void *& pResult,int & nResultLen )
	{
		if ( pMsg->context_len==0 || pMsg->context==0 )
			return false;

		ibuffer in(pMsg->context,pMsg->context_len);
		std::string function_name;
		in >> function_name;

		%SERVICE_SEGMENT_2%

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
extern "C" __declspec(dllexport) I%SERVICE_NAME% * Create%SERVICE_NAME%()
{
	return new %SERVICE_NAME%_Stub();
}