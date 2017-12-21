/*******************************************************************
** 文件名:	speed\Social\MailServer\Src\MailService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "MailService_Proxy.h"
#include "MailService.h"

class MailService_Stub:public IServiceStub
{
public:
	///////////////////////////////IServiceStub///////////////////////////////////////////
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len );
	virtual bool on_stop();
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );
	virtual void * query_interface( int iid );
	virtual void release();

public:
	MailService_Stub(MailService* pReal);

private:
	MailService_Proxy m_proxy;
	MailService* m_real_service;
};

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateMailService()
{
	return new MailService_Stub(new MailService);
}