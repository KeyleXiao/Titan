/*******************************************************************
** 文件名:	SceneEventService_Stub.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "IServerGlobal.h"
#include "SceneEventService.h"
#include "SceneEventService_Stub.h"
#include "StubMacroDef.h"


SceneEventService_Stub::SceneEventService_Stub(SceneEventService * pReal)
	: m_real_service(pReal)
{

}

SceneEventService_Stub::~SceneEventService_Stub()
{

}


////////////////// IServiceStub ////////////////////////////////////////////////////////

// 启动
// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
bool SceneEventService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	m_real_service->on_start(data, len);
	return true;
}

// 停止
bool SceneEventService_Stub::on_stop()
{
	return m_real_service->on_stop();
}

// 处理消息
bool SceneEventService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf & resultBuf )
{
	if ( pMsg==0 || pMsg->context_len==0 || pMsg->context==0 )
	return false;

	ibuffer in(pMsg->context,pMsg->context_len);

	BYTE byType;
	const char * function_name;
	in >> byType >> function_name;

	// 校验一下类型，避免弄错
	if (byType != m_proxy.getServiceType())
	{
		ErrorLn("SceneEventService_Stub:handle_message() recieve error function name ["<< function_name <<"] , service type="<< byType);
		return false;
	}

	//TraceLn("SceneEventService_Stub::handle_message() func="<< function_name);
	////////////////////////////////////////////////////////////////////////////
	// 以下自动生成的消息解包，你需要检查
	if ( TEST_SAME_FUNCTION(function_name,"release") )
	{
		m_real_service->release();
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"Release") )
	{
		m_real_service->release();
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"FireVote") )
	{
		WORD wEventID; BYTE bSrcType; DWORD dwSrcID; int nLen;
		in >> wEventID >> bSrcType >> dwSrcID >> nLen;
		const bool & vReturn= m_real_service->FireVote(wEventID, bSrcType, dwSrcID, in.current(), nLen);
		resultBuf << vReturn;
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"FireExecute") )
	{
		WORD wEventID; BYTE bSrcType; DWORD dwSrcID; int nLen;
		in >> wEventID >> bSrcType >> dwSrcID >> nLen;
		m_real_service->FireExecute(wEventID, bSrcType, dwSrcID, in.current(), nLen);
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"SubscibeVote") )
	{
		IEventVoteSink * pSink; WORD wEventID; BYTE bSrcType; DWORD dwSrcID;
		in >> pSink >> wEventID >> bSrcType >> dwSrcID;
		const bool & vReturn= m_real_service->Subscibe(pSink, wEventID, bSrcType, dwSrcID, "");
		resultBuf << vReturn;
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"Subscibe") )
	{
		IEventExecuteSink * pSink; WORD wEventID; BYTE bSrcType; DWORD dwSrcID;
		in >> pSink >> wEventID >> bSrcType >> dwSrcID;
		const bool & vReturn= m_real_service->Subscibe(pSink, wEventID, bSrcType, dwSrcID, "");
		resultBuf << vReturn;
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"UnSubscibeVote") )
	{
		IEventVoteSink * pSink; WORD wEventID; BYTE bSrcType; DWORD dwSrcID;
		in >> pSink >> wEventID >> bSrcType >> dwSrcID;
		const bool & vReturn= m_real_service->UnSubscibe(pSink, wEventID, bSrcType, dwSrcID);
		resultBuf << vReturn;
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"UnSubscibe") )
	{
		IEventExecuteSink * pSink; WORD wEventID; BYTE bSrcType; DWORD dwSrcID;
		in >> pSink >> wEventID >> bSrcType >> dwSrcID;
		const bool & vReturn= m_real_service->UnSubscibe(pSink, wEventID, bSrcType, dwSrcID);
		resultBuf << vReturn;
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"OutputSubscibeEventStatInfo") )
	{
		m_real_service->OutputSubscibeEventStatInfo();
		return true;
	}

	if ( TEST_SAME_FUNCTION(function_name,"OutputAnalyseInfo") )
	{
		m_real_service->OutputAnalyseInfo();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////


	ErrorLn("SceneEventService_Stub::handle_message() has a function not handle, func="<< function_name);

	return false;
}

// 返回代理对象即可
void * SceneEventService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

// 销毁
void SceneEventService_Stub::release()
{
	if ( m_real_service )
	{
		m_real_service->release();
		m_real_service = 0;
	}

	delete this;
}


//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateSceneEventService()
{
	return new SceneEventService_Stub(new SceneEventService);
}