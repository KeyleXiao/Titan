/*******************************************************************
** �ļ���:	SNSService_Stub.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:	6/12/2016
** ��  ��:	1.0
** ��  ��:	
            SNS������
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