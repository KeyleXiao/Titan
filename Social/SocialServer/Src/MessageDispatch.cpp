/*******************************************************************
** 文件名:	MessageDispatch.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-19
** 版  本:	1.0
** 描  述:	网络消息分发器实现

		
********************************************************************/


#include "stdafx.h"
#include "Alert.h"
#include "MessageDispatch.h"
#include "IMessageHandler.h"



// 最大统计消息码
#define MSG_STAT_MAXMSGID	500

/** 构造
@param moduleId
*/
CMessageDispatch::CMessageDispatch()
{
	memset(&m_TransmitHandlers, 0, sizeof(m_TransmitHandlers));
	memset(&m_MessageHandlers, 0, sizeof(m_MessageHandlers));
}

/** 构造
@param moduleId
*/
CMessageDispatch::~CMessageDispatch()
{

}

/** 分发转发的消息
@param pData 服务器转发消息的缓冲
@param nDataLen 缓冲区长度
*/
void CMessageDispatch::dispatchTransmit(DWORD server, void * buf, size_t len)
{
	const DWORD MSGHEAD_SIZE = sizeof(SNetMsgHead);

	Assert(buf != NULL && len >= MSGHEAD_SIZE);
	SNetMsgHead* head = (SNetMsgHead*)buf;
	BYTE moduleId	= head->byKeyModule;
	BYTE actionId	= head->byKeyAction;
	uchar* data= (uchar*)buf + MSGHEAD_SIZE;
	size_t new_len	= len - MSGHEAD_SIZE;

	if (new_len == 0) data = 0;
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);

	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_TransmitHandlers[moduleId])
		{
			DWORD dwBeginTicks = ::GetTickCount();

			m_TransmitHandlers[moduleId]->onTransmit(server, actionId, head, data, new_len);

			DWORD dwEndTicks = ::GetTickCount();
			double fLapse = (double)(dwEndTicks-dwBeginTicks);

			if(fLapse >= 100.0)
			{
				WarningLn("------------------Warning, module_ID="<<moduleId<<",Message_ID="<<actionId<<",cost_time="<<fLapse<<"ms");

				if(fLapse >= MANAGER_PERFORMANCEALERT_LAPSE)
				{
					AlertWarning(MG_AlertSrcType_Performance, "Performance Warning,module_ID="<<moduleId<<",Message_ID="<<actionId<<",cost_time="<<fLapse<<"ms");
				}
			}
		}
		else
		{
			static DWORD dwLastAlertTimes = 0;
			DWORD dwNowTimes = (DWORD)time(NULL);
			if (dwNowTimes>dwLastAlertTimes+60)
			{
				dwLastAlertTimes = dwNowTimes;
				ErrorLn("moduleId("<<moduleId<<") Handler not registered");
			}
		}
	}
	else
	{
		ErrorLn("moduleId("<<moduleId<<") range error");
	}
}

/** 注册消息处理者
@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
@param hander 消息处理者实例
*/
void CMessageDispatch::registerTransmitHandler(ulong moduleId, ITransmitHandler* handler)
{
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);
	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_TransmitHandlers[moduleId] != NULL)
			Warning("moduleId("<<moduleId<<") already registered registerTransmitHandler"<<endl);
		m_TransmitHandlers[moduleId] = handler;
	}
	else
	{
		ErrorLn( __FUNCTION__<<" moduleId("<<moduleId<<") range error" );
	}
}

/** 取消注册消息处理者
@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
*/
void CMessageDispatch::unregisterTransmitHandler(ulong moduleId)
{
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);
	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_TransmitHandlers[moduleId] == NULL)
			Warning("moduleId("<<moduleId<<") already unregistered unregisteTransmitHandler"<<endl);
		m_TransmitHandlers[moduleId] = 0;
	}
	else
	{
		ErrorLn( __FUNCTION__<<" moduleId("<<moduleId<<") range error" );
	}
}


/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void CMessageDispatch::dispatchServerListUpdated()
{
	for (int i=0; i<MSG_MODULEID_MAXID; i++)
	{
		if (m_TransmitHandlers[i])
		{
#ifdef OPEN_BVTTEST
			char szPerprofName[256] = { 0 };
			sprintf_s(szPerprofName, "dispatchServerInfoUpdate:");
			PP_BY_ID(szPerprofName, i);
#endif

			m_TransmitHandlers[i]->onServerListUpdated();
		}
	}
}

/** 通知服务器信息更新
@param nState		: SERVER_STATE
@param pServerData	: ServerData
@brief        : 例如服务器人数变化等等
*/
void CMessageDispatch::dispatchServerInfoUpdated(DWORD ServerID,BYTE nState,void * pServerData)
{
	for (int i=0; i<MSG_MODULEID_MAXID; i++)
	{
		if (m_TransmitHandlers[i])
		{
#ifdef OPEN_BVTTEST
			char szPerprofName[256] = { 0 };
			sprintf_s(szPerprofName, "dispatchServerInfoUpdated:");
			PP_BY_ID(szPerprofName, i);
#endif

			m_TransmitHandlers[i]->onServerInfoUpdated(ServerID, nState, pServerData);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

/** 分发指定的消息
@param buf 网络层输入的消息缓冲
@param len 缓冲区长度
*/
void CMessageDispatch::dispatchMessage(ClientID clientID,void* buf, size_t len)
{
	if(buf == NULL || len < sizeof(SNetMsgHead))
	{
		return;
	}

	SNetMsgHead* head = (SNetMsgHead*)buf;
	BYTE moduleId = head->byKeyModule;
	uchar* data = (uchar*)buf + sizeof(SNetMsgHead);
	size_t new_len = len-sizeof(SNetMsgHead);

	if (new_len == 0) data = 0;
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);
	if (moduleId > MSG_MODULEID_MAXID)
	{
		WarningLn(__FUNCTION__ ":moduleId("<<moduleId<<") range error");
		return;
	}

#ifdef OPEN_BVTTEST
		char szPerprofName[256] = { 0 };
		sprintf_s(szPerprofName, "Message:%d:", moduleId);
		PP_BY_ID(szPerprofName, head->wKeyAction);
#endif

	if (m_MessageHandlers[moduleId])
	{
#ifdef OPEN_BVTTEST
		// TODOTODO：如果服务器的停顿的问题解决了，就可以去掉，这地方不允许有太多操作存在
		LARGE_INTEGER litmpBegin; 
		::QueryPerformanceCounter(&litmpBegin);
#else
		DWORD dwBeginTicks = ::GetTickCount();
#endif

		m_MessageHandlers[moduleId]->onMessage(clientID, head->byKeyAction, head, data, new_len);

		double fLapse = 0.0;
#ifdef OPEN_BVTTEST 
		LARGE_INTEGER litmpEnd; 
		::QueryPerformanceCounter(&litmpEnd);
		fLapse = ((double(litmpEnd.QuadPart -  litmpBegin.QuadPart)) / m_fCPUFreq) * 1000;
#else
		DWORD dwEndTicks = ::GetTickCount();
		fLapse = (double)(dwEndTicks-dwBeginTicks);	// QueryPerformanceCounter比GetTickCount慢40倍,发布版还是用GetTickCount好了
#endif 

		if(fLapse >= 100.0)
		{
			WarningLn("------------------性能警报,模块ID="<<head->byKeyModule<<" 消息ID="<<head->byKeyAction<<" 耗时="<<fLapse);

			if(fLapse >= MANAGER_PERFORMANCEALERT_LAPSE)
			{
				AlertWarning(MG_AlertSrcType_Performance, "性能警报,模块ID="<<head->byKeyModule<<" 消息ID="<<head->byKeyAction<<" 耗时="<<fLapse)
			}
		}
	}
	else
	{
		static DWORD dwLastAlertTimes = 0;
		DWORD dwNowTimes = (DWORD)time(NULL);
		if (dwNowTimes>dwLastAlertTimes+60)
		{
			dwLastAlertTimes = dwNowTimes;
			WarningLn("moduleId("<<moduleId<<") Handler not registered");
		}
	}
}


/** 注册消息处理者
@param moduleId 模块ID，参考InputMessage.h中的枚举定义
@param hander 消息处理者实例
*/
void CMessageDispatch::registerMessageHandler(ulong moduleId, IMessageHandler* handler)
{
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);

	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_MessageHandlers[moduleId] != NULL)
			WarningLn("moduleId("<<moduleId<<") already registered"<<endl);

		m_MessageHandlers[moduleId] = handler;
	}
	else
	{
		WarningLn("moduleId("<<moduleId<<") range error");
	}
}

/** 注销消息处理者
@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
*/
void CMessageDispatch::unregisterMessageHandler(ulong moduleId)
{
	Assert(moduleId >= 0 && moduleId < MSG_MODULEID_MAXID);

	if (moduleId >= 0 && moduleId < MSG_MODULEID_MAXID)
	{
		if (m_MessageHandlers[moduleId] == NULL)
			WarningLn("moduleId("<<moduleId<<") already unregistered");

		m_MessageHandlers[moduleId] = 0;
	}
	else
	{
		WarningLn("moduleId("<<moduleId<<") range error");
	}
}