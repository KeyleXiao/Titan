/*******************************************************************
** �ļ���:	LuaService_Stub.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	12/27/2016
** ��  ��:	1.0
** ��  ��:	Lua������
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