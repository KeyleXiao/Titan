/*******************************************************************
** �ļ���:	ClanService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/31/2016	17:19
** ��  ��:	1.0
** ��  ��:	��������
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "ClanService_Proxy.h"
#include "ClanService.h"

class ClanService_Stub:public IServiceStub
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
	ClanService_Stub(CClanService* pReal);

private:
	ClanService_Proxy m_proxy;
	CClanService* m_real_service;
};

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateClanService()
{
	return new ClanService_Stub(new CClanService);
}