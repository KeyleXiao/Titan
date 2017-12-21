/*******************************************************************
** 文件名:	E:\speed\Server\LegendCupServer\Src\LegendCupSceneService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜 服务存根
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
