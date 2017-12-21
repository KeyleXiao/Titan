/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/10/2015
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/
#pragma once

#ifndef __EXTERNALHELPER_H__
#define __EXTERNALHELPER_H__

#include "ITimerService.h"
#include "IEntity.h"
#include "ChatDef.h"
#include "IEntityEvent.h"

// 定义
class CExternalHelper
{
public:
	CExternalHelper();
	~CExternalHelper();

	/** 启用定时器
	*/
	bool SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes = INFINITY_CALL, const char *debugInfo=0);

	/** 销毁定时器
	*/
	bool KillTimer(DWORD dwTimerID, ITimerHandler *pHandler);

    /** 订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	*/
	virtual bool Subscibe(__IEntity *pEntity, IEventVoteSink *pSink, EventKey eventID, LPCSTR pszDesc);

	/** 订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	*/
	virtual bool Subscibe(__IEntity *pEntity, IEventExecuteSink *pSink, EventKey eventID, LPCSTR pszDesc);

	/** 取消订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@return  成功返回true,失败返回false
	*/
	virtual bool UnSubscibe(__IEntity *pEntity, IEventVoteSink *pSink, EventKey eventID);

	/** 取消订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@return  成功返回true,失败返回false
	*/
	virtual bool UnSubscibe(__IEntity *pEntity, IEventExecuteSink *pSink, EventKey eventID);

	/** 发送实体数据
	*/
	void sendEntityMessage(UID uid, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

    /** 英雄发送数据到客户端
	*/
	void playerSend2Client(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

    /** 英雄发送数据到客户端
	*/
	void playerSend2Client(UID uid, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

	/** 发送数据
	*/
	bool broadCastInAOI(__IEntity *pEntity, Vector3 ptTargetTile, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

	/** 发送数据
	*/
	bool broadCastDataToAllPlayer(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

    /** 显示提示文字
	*/
	void showSystemMessage(__IEntity *pEntity, EMChatTipID nTipID, LPCSTR szParam = NULL, EMChatBroadcastType nBroadcastType = CHAT_BROADCASTTYPE_SINGLE, int nDelayTime = 0);
    
    /** 显示提示文字
    */
    void showSystemMessage(UID uid, EMChatTipID nTipID, LPCSTR szParam = NULL, EMChatBroadcastType nBroadcastType = CHAT_BROADCASTTYPE_SINGLE, int nDelayTime = 0);
};

extern CExternalHelper				g_EHelper;

#endif // __EXTERNALHELPER_H__