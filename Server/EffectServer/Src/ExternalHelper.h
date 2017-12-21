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

#include "IActorService.h"
#include "IEntity.h"
#include "ITimerService.h"
#include "IWarService.h"
#include "IEntityEvent.h"
#include "ChatDef.h"

// 定义
class CExternalHelper
{
public:
	CExternalHelper();
	~CExternalHelper();

	/** 启用定时器
	*/
	bool SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes = INFINITY_CALL, const char *debugInfo=0, bool bCheck=true);

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
	void sendEntityMessage(UID uid, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen);

	/** 发送数据
	*/
	bool broadCastInAOI(__IEntity *pEntity, Vector3 ptTargetTile, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen);

	/** 发送数据
	*/
	bool broadCastDataToAllPlayer(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen);

	/** 显示提示文字
	*/
	void showSystemMessage(__IEntity *pEntity, EMChatTipID nTipID, LPCSTR szParam = NULL);

    /** 选择目标
	*/
	bool chooseTarget(__IEntity *pEntity, int nTargetFilter, UID uidTarget);

    // 给群体怪物增加BUFF
    void BatchAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0);
    
    // 给群体怪物移除BUFF
    void BatchRemoveBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, UID uidAdd, UID uidRemove);

    // 给群体怪物带检测指定BUFFID和BUFF类型来增加BUFF
    void BatchCheckAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwCheckBuff=0, DWORD dwCheckLevel=0, DWORD dwCheckBuffType=0, DWORD dwTime=0, void *pBuffContext=0, int nContextLen=0);

    // 传送
    void Transport(__IEntity *pEntity, Vector3 newLoc, bool bTransport, int nSceneID, int nAngle = INT_MAX, int nReason = TRANSPORT_REASON_FLASH);

    /** 英雄发送数据到客户端
	*/
	void playerSend2Client(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen);
};

extern CExternalHelper				g_EHelper;

#endif // __EXTERNALHELPER_H__