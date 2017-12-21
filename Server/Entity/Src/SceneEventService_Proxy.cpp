/*******************************************************************
** 文件名:	SceneEventService_Proxy.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            单个场景事件代理
********************************************************************/

#include "stdafx.h"
#include "SceneEventService_Proxy.h"

#define OBUF(t_data)	\
	obuf256 t_data;		\
	t_data << m_nServiceType;

SceneEventService_Proxy::SceneEventService_Proxy()
	: m_nServiceType(GAMELOGIC_SERVICE_EVENT)
	, m_pContainer(0)
{

}

SceneEventService_Proxy::~SceneEventService_Proxy()
{

}

// 取得游戏逻辑服务类型
BYTE SceneEventService_Proxy::getServiceType()
{
	return m_nServiceType;
}

////////////////// ISceneEventService //////////////////////////////////////////////////
// 是否可调度到场景逻辑执行
bool SceneEventService_Proxy::canDispatch(WORD wEventID, BYTE byType)
{
	SEventServiceKey key(wEventID, byType);
	EVENT_SERVICE_KEY_MAP::iterator iter = m_mapEventKey.find(key);

	return (iter != m_mapEventKey.end());
}

// 释放
void SceneEventService_Proxy::release()
{
	OBUF(t_data);
	t_data << "release";

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
}

////////////////// IEventEngine ////////////////////////////////////////////////////////
/** 释放
*/
void SceneEventService_Proxy::Release(void)
{
	OBUF(t_data);
	t_data << "Release";

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
}

/** 发送投票事件   
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度	  
@return  
*/
bool SceneEventService_Proxy::FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	// 强制不订阅投票事件
	return true;

	// 进行事件过滤来优化系统性能
	if (!canDispatch(wEventID, bSrcType))
	{
		return true;
	}
	//TraceLn("SceneEventService_Proxy::FireVote(): EventID="<< wEventID);

	OBUF(t_data);
	t_data << "FireVote";
	t_data << wEventID;
	t_data << bSrcType;
	t_data << dwSrcID;
	t_data << nLen;
	t_data.push_back(pszContext, nLen);

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	if ( pReturn!=0 && nReturnLen >0 )
	{
		ibuffer t_in(pReturn,nReturnLen);
		bool vReturn;
		t_in >> vReturn;
		if ( t_in.good() ) return vReturn;
	}
	return bool();     // 请检查默认返回值是否正确
}

/** 发送执行事件
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度	  
@return  
*/
void SceneEventService_Proxy::FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	// 进行事件过滤来优化系统性能
	if (!canDispatch(wEventID, bSrcType))
	{
		return;
	}
	//TraceLn("SceneEventService_Proxy::FireExecute(): EventID="<< wEventID);

	OBUF(t_data);
	t_data << "FireExecute";
	t_data << wEventID;
	t_data << bSrcType;
	t_data << dwSrcID;
	t_data << nLen;
	t_data.push_back(pszContext, nLen);

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen,MSG_FLAG_NO_BLOCK);
}

/** 订阅投票事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool SceneEventService_Proxy::Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
{
	// 强制不订阅投票事件
	return true;

	OBUF(t_data);
	t_data << "SubscibeVote";
	t_data << pSink;
	t_data << wEventID;
	t_data << bSrcType;
	t_data << dwSrcID;
	//t_data << pszDesc;

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	if ( pReturn!=0 && nReturnLen >0 )
	{
		ibuffer t_in(pReturn,nReturnLen);
		bool vReturn;
		t_in >> vReturn;

		if ( t_in.good() ) return vReturn;
	}
	return bool();     // 请检查默认返回值是否正确
}

/** 订阅执行事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool SceneEventService_Proxy::Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
{
	OBUF(t_data);
	t_data << "Subscibe";
	t_data << pSink;
	t_data << wEventID;
	t_data << bSrcType;
	t_data << dwSrcID;
	//t_data << pszDesc;

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	if ( pReturn!=0 && nReturnLen >0 )
	{
		ibuffer t_in(pReturn,nReturnLen);
		bool vReturn;
		t_in >> vReturn;

		if (vReturn)
		{
			SEventServiceKey key(wEventID, bSrcType);
			m_mapEventKey[key] = true;
		}

		if ( t_in.good() ) return vReturn;
	}
	return bool();     // 请检查默认返回值是否正确
}

/** 取消订阅投票事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool SceneEventService_Proxy::UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	OBUF(t_data);
	t_data << "UnSubscibeVote";
	t_data << pSink;
	t_data << wEventID;
	t_data << bSrcType;
	t_data << dwSrcID;

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	if ( pReturn!=0 && nReturnLen >0 )
	{
		ibuffer t_in(pReturn,nReturnLen);
		bool vReturn;
		t_in >> vReturn;
		if ( t_in.good() ) return vReturn;
	}
	return bool();     // 请检查默认返回值是否正确
}

/** 取消订阅执行事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool SceneEventService_Proxy::UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	OBUF(t_data);
	t_data << "UnSubscibe";
	t_data << pSink;
	t_data << wEventID;
	t_data << bSrcType;
	t_data << dwSrcID;

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen);
	if ( pReturn!=0 && nReturnLen >0 )
	{
		ibuffer t_in(pReturn,nReturnLen);
		bool vReturn;
		t_in >> vReturn;
		if ( t_in.good() ) return vReturn;
	}
	return bool();     // 请检查默认返回值是否正确
}

/** 输出事件订阅的统计信息
*/
void SceneEventService_Proxy::OutputSubscibeEventStatInfo(void)
{
	OBUF(t_data);
	t_data << "OutputSubscibeEventStatInfo";

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen,MSG_FLAG_NO_BLOCK);
}

/** 输出耗时统计信息 
*/
void SceneEventService_Proxy::OutputAnalyseInfo(void)
{
	OBUF(t_data);
	t_data << "OutputAnalyseInfo";

	void * pReturn = 0;
	int nReturnLen = 0;
	m_pContainer->post_message( t_data.data(),t_data.size(),pReturn,nReturnLen,MSG_FLAG_NO_BLOCK);
}
