/*******************************************************************
** �ļ���:	ClanSceneService_Stub.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/6/2016
** ��  ��:	1.0
** ��  ��:	��᳡�����
********************************************************************/

#pragma once

#include "IClanSceneService.h"
#include "ClanSceneService_Proxy.h"
#include "ClanSceneService.h"

class ClanSceneService_Stub : public IServiceStub
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
	ClanSceneService_Stub(ClanScenService * pReal); 

private:
	ClanSceneService_Proxy	m_proxy;
	ClanScenService*		m_real_service;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateClanSceneService()
{
	return new ClanSceneService_Stub(new ClanScenService);
}
