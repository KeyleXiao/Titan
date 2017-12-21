/*******************************************************************
** 文件名:	E:\speed\Server\OrderServer\Src\StaticWarSceneService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	所有战场数据服务 服务存根
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
