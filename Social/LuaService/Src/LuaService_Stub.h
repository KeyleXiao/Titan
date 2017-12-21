/*******************************************************************
** 文件名:	LuaService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	Lua服务存根
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "LuaService_Proxy.h"
#include "LuaService.h"

class LuaService_Stub:public IServiceStub
{
public:
	// IServiceStub
	//////////////////////////////////////////////////////////////////////////
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len );
	virtual bool on_stop();
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );
	virtual void * query_interface( int iid );
	virtual void release();

public:
	LuaService_Stub(LuaService* pReal);

private:
	LuaService_Proxy m_proxy;
	LuaService* m_real_service;
};

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateLuaService()
{
	return new LuaService_Stub(new LuaService);
}