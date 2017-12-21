/*******************************************************************
** 文件名:	SceneEventService.cpp 
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


////////////////// SceneEventService ////////////////////////////////////////////////////////

SceneEventService::SceneEventService()
	: m_nSceneID(0)
	, m_pEventEngine(0)
{

}

SceneEventService::~SceneEventService()
{

}

// 启动
// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
bool SceneEventService::on_start(void * data,int len )
{
	if ( m_pEventEngine==0 )
	{
		m_pEventEngine = CreateEventEngine();
	}

	return true;
}

// 停止
bool SceneEventService::on_stop()
{
	return true;
}

////////////////// ISceneEventService ////////////////////////////////////////////////////////
// 是否可调度到场景逻辑执行
bool SceneEventService::canDispatch(WORD wEventID, BYTE byType)
{
	SEventServiceKey key(wEventID, byType);
	EVENT_SERVICE_KEY_MAP::iterator iter = m_mapEventKey.find(key);
	
	return (iter != m_mapEventKey.end());
}

// 释放
void SceneEventService::release()
{
	Release();

	delete this;
}

////////////////// IEventEngine ////////////////////////////////////////////////////////
/** 释放
*/
void SceneEventService::Release(void)
{
	if ( m_pEventEngine )
	{
		m_pEventEngine->Release();
		m_pEventEngine = 0;
	}
}

/** 发送投票事件   
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度	  
@return  
*/
bool SceneEventService::FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	// 进行事件过滤来优化系统性能
	if (!canDispatch(wEventID, bSrcType))
	{
		return true;
	}

	//TraceLn("SceneEventService::FireVote(): EventID="<< wEventID);

	return true;//	m_pEventEngine->FireVote(wEventID, bSrcType, dwSrcID, pszContext, nLen);
}

/** 发送执行事件
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度	  
@return  
*/
void SceneEventService::FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	// 进行事件过滤来优化系统性能
	if (!canDispatch(wEventID, bSrcType))
	{
		return;
	}

	//TraceLn("SceneEventService::FireExecute(): EventID="<< wEventID);
	m_pEventEngine->FireExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen);
}

/** 订阅投票事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool SceneEventService::Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
{
	TraceLn("SceneEventService::VoteSubscibe(): EventID="<< wEventID);
	return true; // m_pEventEngine->Subscibe(pSink, wEventID, bSrcType, dwSrcID, pszDesc);
}

/** 订阅执行事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool SceneEventService::Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
{
	bool bResult = m_pEventEngine->Subscibe(pSink, wEventID, bSrcType, dwSrcID, pszDesc);

	if (bResult)
	{
		TraceLn("SceneEventService::Subscibe(): EventID="<< wEventID);
		SEventServiceKey key(wEventID, bSrcType);
		m_mapEventKey[key] = true;
	}

	return bResult;
}

/** 取消订阅投票事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool SceneEventService::UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	TraceLn("SceneEventService::VoteUnSubscibe(): EventID="<< wEventID);
	return true; // m_pEventEngine->UnSubscibe(pSink, wEventID, bSrcType, dwSrcID);
}

/** 取消订阅执行事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool SceneEventService::UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	TraceLn("SceneEventService::UnSubscibe(): EventID="<< wEventID);
	return m_pEventEngine->UnSubscibe(pSink, wEventID, bSrcType, dwSrcID);
}

/** 输出事件订阅的统计信息
*/
void SceneEventService::OutputSubscibeEventStatInfo(void)
{
	m_pEventEngine->OutputSubscibeEventStatInfo();
}

/** 输出耗时统计信息 
*/
void SceneEventService::OutputAnalyseInfo(void)
{
	m_pEventEngine->OutputAnalyseInfo();
}
