/*******************************************************************
** 文件名:	TimeSyncService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据ITimeSyncService接口定义，自动生成的TimeSyncService接口
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
#include "ITimeSyncService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "TimeSyncService.h"
#include "tinyxml.h"

using namespace rkt;
using namespace std;

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class TimeSyncService_Stub : public IServiceStub,public ITimeSyncService
{
	TimeSyncService*		m_real_service;
	SERVICE_PTR    m_pContainer;

public:
	TimeSyncService_Stub(TimeSyncService * pReal) 
		: m_real_service(pReal)
	{
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_pContainer = pContainer;

		TiXmlElement * pService = (TiXmlElement*)(pContainer->get_scheme().scheme->pExtend);
		if ( pService==0 )
		{
			return m_real_service->Start("127.0.0.1",6600,6601);
		}

		int nUDPPort = 0;
		int nTCPPort = 0;
		const char * szIP = pService->Attribute("ip");
		pService->QueryIntAttribute("udp_port",&nUDPPort);
		pService->QueryIntAttribute("nTCPPort",&nTCPPort);

		if ( szIP==0 )
			szIP = "127.0.0.1";

		return m_real_service->Start(szIP,nUDPPort,nTCPPort);
	}

	// 停止
	virtual bool on_stop()
	{
        m_pContainer = SERVICE_PTR(0);
		return true;
	}

	virtual DWORD GetTick()
	{
		if ( m_real_service )
			return m_real_service->GetTick();

		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////
	//virtual void  release(  )
	//{
	//	BUILD_MSG_CONTEXT_VOID(ITimeSyncService::release);

	//	m_pContainer->post_message( pMsg,nMsgLen,0);
	//};

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{

		return false;
	}

	// 返回代理对象即可
	virtual void * query_interface( int iid )
	{
		return (ITimeSyncService*)this;
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
extern "C" __declspec(dllexport) IServiceStub * CreateTimeSyncService()
{
	return new TimeSyncService_Stub(new TimeSyncService);
}
