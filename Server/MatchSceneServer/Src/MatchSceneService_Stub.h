/*******************************************************************
** 文件名:	E:\speed\Server\MatchSceneServer\Src\MatchSceneService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	14:43
** 版  本:	1.0
** 描  述:	
            比赛大厅场景服 服务存根
********************************************************************/

#pragma once


#include "IMatchSceneService.h"
#include "MatchSceneService_Proxy.h"
#include "MatchSceneService.h"

 

class MatchSceneService_Stub : public IServiceStub
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
	MatchSceneService_Stub(MatchSceneService * pReal); 


private:
	MatchSceneService_Proxy	m_proxy;
	MatchSceneService*		m_real_service;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateMatchSceneService()
{
	return new MatchSceneService_Stub(new MatchSceneService);
}
