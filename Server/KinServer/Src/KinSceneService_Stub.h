/*******************************************************************
** 文件名:	E:\speed\Server\KinServer\Src\KinSceneService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	战队 服务存根
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
