/*******************************************************************
** �ļ���:	E:\speed\Server\KinServer\Src\KinSceneService_Stub.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	ս�� ������
********************************************************************/

#pragma once


#include "IKinSceneService.h"
#include "KinSceneService_Proxy.h"
#include "KinSceneService.h"



class KinSceneService_Stub : public IServiceStub
{
public:
	//////////////////////////IServiceStub////////////////////////////////////////////////
	virtual bool	on_start( SERVICE_PTR pContainer,void * data,int len );
	virtual bool	on_stop();
	virtual bool	handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );	
	virtual void*	query_interface( int iid );
	virtual void	release();

public:
	KinSceneService_Stub(KinScenService * pReal); 

private:
	KinSceneService_Proxy	m_proxy;
	KinScenService*		m_real_service;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateKinSceneService()
{
	return new KinSceneService_Stub(new KinScenService);
}
