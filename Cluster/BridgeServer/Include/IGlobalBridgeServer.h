/*******************************************************************
** 文件名:	E:\Rocket\Manager\BridgeServer\Include\IGlobalBridgeServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-6-24
** 版  本:	1.0
** 描  述:	跨区桥服务器全局接口
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "Common.h"
//#include "DGlobalGame.h"
#include "BridgeModuleDef.h"


#ifndef __I_GlobalBridgeServer_H__
#define __I_GlobalBridgeServer_H__

class ServerUser;


/// 跨区桥服务器全局对象
struct IGlobalBridgeServer
{
	/// 服务器网络消息处理
	virtual void onServerMessage(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;

	/// 获取时间轴
	virtual TimerAxis* getTimerAxis() = 0;

	// 启用定时器
	virtual bool SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes = INFINITY_CALL)=0;

	// 销毁定时器
	virtual bool KillTimer(DWORD timerID, ITimerHandler * handler)=0;

	// 发送数据给指定服务器
	virtual bool SendDataToServer(DWORD dwServerID,LPVOID pData,DWORD dwDataLen) = 0;

	// 广播数据给服务器 pServerArray==NULL && wServerNum==BRIDGE_BROADCASTALL_NUM 广播所有
	virtual bool BroadcastToServer(DWORD * pServerArray,WORD wServerNum,LPVOID pData,DWORD dwDataLen) = 0;

	// 服务器断线
	virtual void OnServerUserLeave(ServerUser *pServer) = 0;

	// 设置当前操作代码  DWORD dwCode 参考 EMManagerControlCode
	virtual void SetControlCode(DWORD dwCode) = 0;


};

#define gGlobalServer	((IGlobalBridgeServer*)::rkt::getGlobal())


#endif	// __I_GlobalBridgeServer_H__
