/*******************************************************************
** 文件名:	ClanSceneService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/6/2016
** 版  本:	1.0
** 描  述:	帮会场景存根
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
