/*******************************************************************
** �ļ���:	E:\speed\Social\KinServer\Src\KinService_Stub.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/16/2015	10��41
** ��  ��:	1.0
** ��  ��:	
            ����ϵͳ������
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "KinService_Proxy.h"
#include "KinService.h"

class KinService_Stub:public IServiceStub
{
public:
	///////////////////////////////IServiceStub///////////////////////////////////////////
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len );
	virtual bool on_stop();
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );
	virtual void * query_interface( int iid );
	virtual void release();

public:
	KinService_Stub(KinService* pReal);

private:
	KinService_Proxy m_proxy;
	KinService* m_real_service;
};

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateKinService()
{
	return new KinService_Stub(new KinService);
}