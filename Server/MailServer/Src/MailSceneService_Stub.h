/*******************************************************************
** 文件名:	MailSceneService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/7/2017
** 版  本:	1.0
** 描  述:	邮件系统场景存根
********************************************************************/

#pragma once

#include "IMailSceneService.h"
#include "MailSceneService_Proxy.h"
#include "MailSceneService.h"

class MailSceneService_Stub : public IServiceStub
{
public:
	// IServiceStub
	//////////////////////////////////////////////////////////////////////////
	virtual bool	on_start( SERVICE_PTR pContainer,void * data,int len );
	virtual bool	on_stop();
	virtual bool	handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );	
	virtual void*	query_interface( int iid );
	virtual void	release();

public:
	MailSceneService_Stub(MailSceneService * pReal); 

private:
	MailSceneService_Proxy	m_proxy;
	MailSceneService*		m_real_service;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateMailSceneService()
{
	return new MailSceneService_Stub(new MailSceneService);
}
