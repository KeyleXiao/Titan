/*******************************************************************
** 文件名:	MentorshipSceneService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/29/2016
** 版  本:	1.0
** 描  述:	师徒系统场景存根
********************************************************************/

#pragma once

#include "IMentorshipSceneService.h"
#include "MentorshipSceneService_Proxy.h"
#include "MentorshipSceneService.h"

class MentorshipSceneService_Stub : public IServiceStub
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
	MentorshipSceneService_Stub(MentorshipSceneService * pReal); 

private:
	MentorshipSceneService_Proxy	m_proxy;
	MentorshipSceneService*		m_real_service;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateMentorshipSceneService()
{
	return new MentorshipSceneService_Stub(new MentorshipSceneService);
}
