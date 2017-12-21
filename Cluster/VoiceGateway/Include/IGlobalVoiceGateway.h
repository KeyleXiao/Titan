/*******************************************************************
** 文件名:	IGlobalVoiceGateway.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2012-09-17
** 版  本:	1.0
** 描  述:	语音网关全局接口
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "Common.h"
#include "VoiceDef.h"


#ifndef __I_GlobalVoiceGateway_H__
#define __I_GlobalVoiceGateway_H__

class CClientUser;
class CAudioUser;
struct SGameMsgHead;


/// 语音网关全局对象
struct IGlobalVoiceGateway
{
	/// 转发网络消息到语音服务器
	virtual void onTransVoiceServer(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;

	/// 客户端消息处理
	virtual void onClientMessage(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;

	/// 服务器网络消息处理
	virtual void onServerMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;


	/// 语音通道客户端网络消息处理
	virtual void onAudioClientMessage(CAudioUser &audio, ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;

	/// 语音通道转发网络消息到语音服务器
	virtual void onAudioTransVoiceServer(CAudioUser &audio, ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;


	/// 获取时间轴
	virtual TimerAxis* getTimerAxis() = 0;

	// 启用定时器
	virtual bool SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes = INFINITY_CALL)=0;

	// 销毁定时器
	virtual bool KillTimer(DWORD timerID, ITimerHandler * handler)=0;

	// 客户端连入
	virtual void OnClientUserEnter(CClientUser *pUser, DWORD dwReason = 0) = 0;

	// 服务器断线
	virtual void OnClientUserLeave(CClientUser * pUser, DWORD dwReason = 0) = 0;

	// 设置当前操作代码  DWORD dwCode 参考 EMVoiceGatewayControlCode
	virtual void SetControlCode(DWORD dwCode) = 0;

	// 取得网关ID
	virtual DWORD GetGatewayID(void) = 0;
};

#define gGlobalServer	((IGlobalVoiceGateway*)::rkt::getGlobal())


#endif	// __I_GlobalVoiceGateway_H__
