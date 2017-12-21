/*******************************************************************
** �ļ���:	E:\speed\Server\LegendCupServer\Src\LegendCupSceneService_Stub.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а� ������
********************************************************************/

#pragma once


#include "ILegendCupSceneService.h"
#include "LegendCupSceneService_Proxy.h"
#include "LegendCupSceneService.h"



class LegendCupSceneService_Stub : public IServiceStub
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
	LegendCupSceneService_Stub(LegendCupScenService * pReal); 


private:
	LegendCupSceneService_Proxy	m_proxy;
	LegendCupScenService*		m_real_service;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateLegendCupSceneService()
{
	return new LegendCupSceneService_Stub(new LegendCupScenService);
}
