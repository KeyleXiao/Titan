/*******************************************************************
** �ļ���:	E:\speed\Server\OrderServer\Src\StaticWarSceneService_Stub.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	����ս�����ݷ��� ������
********************************************************************/

#pragma once


#include "IStaticWarSceneService.h"
#include "StaticWarSceneService_Proxy.h"
#include "StaticWarSceneService.h"



class StaticWarSceneService_Stub : public IServiceStub
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
	StaticWarSceneService_Stub(StaticWarScenService * pReal); 


private:
	StaticWarSceneService_Proxy	m_proxy;
	StaticWarScenService*		m_real_service;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateStaticWarSceneService()
{
	return new StaticWarSceneService_Stub(new StaticWarScenService);
}
