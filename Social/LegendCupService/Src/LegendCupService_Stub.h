/*******************************************************************
** �ļ���:	E:\speed\Social\LegendCupServer\Src\LegendCupService_Stub.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/16/2015	10��41
** ��  ��:	1.0
** ��  ��:	����������
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "LegendCupService_Proxy.h"
#include "LegendCupService.h"

class LegendCupService_Stub:public IServiceStub
{
public:
	// IServiceStub
	//////////////////////////////////////////////////////////////////////////
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len );
	virtual bool on_stop();
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );
	virtual void * query_interface( int iid );
	virtual void release();

public:
	LegendCupService_Stub(LegendCupService* pReal);

private:
	LegendCupService_Proxy m_proxy;
	LegendCupService* m_real_service;
};

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateLegendCupService()
{
	return new LegendCupService_Stub(new LegendCupService);
}