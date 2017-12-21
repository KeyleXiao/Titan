#include "stdafx.h"
#include "LuaService_Stub.h"

bool LuaService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	return m_real_service->on_start();
}

bool LuaService_Stub::on_stop()
{
	TraceLn(__FUNCTION__);
    m_real_service->on_stop();
	m_proxy.m_pContainer = SERVICE_PTR(0);
	return true;
}

bool LuaService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
    if ( pMsg->context_len==0 || pMsg->context==0 )
        return false;

    ibuffer in(pMsg->context,pMsg->context_len);

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息解包，你需要检查

    if ( TEST_SAME_FUNCTION(ILuaService::release) )
    {
        m_real_service->release();
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILuaService::LoadLuaFile) )
    {
        char * szLuaFileName;
        in >> szLuaFileName;
        m_real_service->LoadLuaFile(szLuaFileName);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILuaService::RunMemoryLua) )
    {
        char * pLuaData;int nDataLen;
        in >> pLuaData >> nDataLen;
        m_real_service->RunMemoryLua(pLuaData,nDataLen);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILuaService::RunLuaFunction) )
    {
        char * szFunName;int nInNum; //CLuaParam pInParam;CLuaParam pOutParam;int nOutNum;
        in >> szFunName >> nInNum;
        CLuaParam pInParam[20];
        if (nInNum > 0)
        {
            for (int i=0; i<nInNum; ++i)
            {
                in >> pInParam[i];
            }
        }
        m_real_service->RunLuaFunction(szFunName,pInParam,nInNum);

        return true;
    }

    if ( TEST_SAME_FUNCTION(ILuaService::IsFunction) )
    {
        char * szFunName;
        in >> szFunName;
        m_real_service->IsFunction(szFunName);
        return true;
    }

    if ( TEST_SAME_FUNCTION(ILuaService::UpdateLuaFile) )
    {
        char * szLuaFileName;
        in >> szLuaFileName;
        m_real_service->UpdateLuaFile(szLuaFileName);
        return true;
    }

    ErrorLn(__FUNCTION__":can not found function name:" << MsgFunctionName<SERVICE_MESSAGE>()(pMsg) );
	return false;
}

void * LuaService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

void LuaService_Stub::release()
{
	TraceLn(__FUNCTION__);
	if ( nullptr != m_real_service )
	{
		m_real_service->release();
		m_real_service = nullptr;
	}
	delete this;
}

LuaService_Stub::LuaService_Stub(LuaService* pReal)
	:m_real_service(pReal)
{
}