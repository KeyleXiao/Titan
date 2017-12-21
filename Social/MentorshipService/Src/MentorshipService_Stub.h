/*******************************************************************
** 文件名:	E:\speed\Social\MentorshipServer\Src\MentorshipService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/24/2016	17：00
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "MentorshipService_Proxy.h"
#include "MentorshipService.h"

class MentorshipService_Stub:public IServiceStub
{
public:
	///////////////////////////////IServiceStub///////////////////////////////////////////
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len );
	virtual bool on_stop();
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );
	virtual void * query_interface( int iid );
	virtual void release();

public:
	MentorshipService_Stub(MentorshipService* pReal);

private:
	MentorshipService_Proxy m_proxy;
	MentorshipService* m_real_service;
};

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateMentorshipService()
{
	return new MentorshipService_Stub(new MentorshipService);
}