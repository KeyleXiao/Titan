/*******************************************************************
** 文件名:	SNSService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	6/12/2016
** 版  本:	1.0
** 描  述:	
            SNS服务存根
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "SNSService_Proxy.h"
#include "SNSService.h"

class SNSService_Stub:public IServiceStub
{
public:
	// IServiceStub
	//////////////////////////////////////////////////////////////////////////
	virtual bool on_start( SERVICE_PTR pContainer, void *data, int len );
	virtual bool on_stop();
	virtual bool handle_message( SERVICE_MESSAGE *pMsg, rkt::obuf *resultBuf );
	virtual void * query_interface( int iid );
	virtual void release();

public:
	SNSService_Stub(SNSService *pReal);

private:
	SNSService_Proxy m_proxy;
	SNSService* m_real_service;
};

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateSNSService()
{
	return new SNSService_Stub(new SNSService);
}