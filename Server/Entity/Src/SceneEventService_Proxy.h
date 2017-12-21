/*******************************************************************
** 文件名:	SceneEventService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            单个场景事件代理
********************************************************************/

#pragma once;

#include "IServiceContainer.h"
#include "ISceneEventService.h"
#include <map>


// 单个场景事件代理
class SceneEventService_Proxy : public ISceneEventService
{
public:
	////////////////// ISceneEventService //////////////////////////////////////////////////
	// 是否可调度到场景逻辑执行
	virtual bool		canDispatch(WORD wEventID, BYTE byType);

	// 释放
	virtual void		release();

	////////////////// IEventEngine ////////////////////////////////////////////////////////
	/** 释放
	*/
	virtual void		Release(void);

	/** 发送投票事件   
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	@return  
	*/
	virtual bool		FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/** 发送执行事件
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	@return  
	*/
	virtual void		FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/** 订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	*/
	virtual bool		Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc);

	/** 订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	*/
	virtual bool		Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc);

	/** 取消订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	*/
	virtual bool		UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID);

	/** 取消订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	*/
	virtual bool		UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID);

	/** 输出事件订阅的统计信息
	*/
	virtual void		OutputSubscibeEventStatInfo(void);

	/** 输出耗时统计信息 
	*/
	virtual void		OutputAnalyseInfo(void);

public:
	////////////////// SceneEventService_Proxy ////////////////////////////////////////////////////////

	SceneEventService_Proxy();

	~SceneEventService_Proxy();
	
	// 取得游戏逻辑服务类型
	BYTE getServiceType();

public:
	typedef std::map<DWORD, bool>  EVENT_SERVICE_KEY_MAP;
	EVENT_SERVICE_KEY_MAP  m_mapEventKey;

	BYTE			m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE
	SERVICE_PTR		m_pContainer;
};