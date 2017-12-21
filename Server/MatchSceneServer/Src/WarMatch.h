/*******************************************************************
** 文件名:	WarMatch.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2010-10-27
** 版  本:	1.0
** 描  述:	跨服竞技撮合系统服务器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __WarMatch_h__
#define __WarMatch_h__

#include "IServerGlobal.h"
#include "IMessageDispatch.h"
#include "IMatchScene.h"
#include "BridgeModuleDef.h"
#include "DGlobalMessage.h"
#include "MatchDef.h"
#include "IEntity.h"
#include "IBridgeConnectorService.h"
#include "ICenterConnectorService.h"

class CWarMatch : public IWarMatch
{
	enum
	{
		ETimerEventID_InitData = 0,			// 初始化的定时器
	};

public:
	// 构造函数
	CWarMatch(void);

	// 析构函数
	virtual ~CWarMatch(void);



public:
	// 创建
	bool Create(void);

	// 释放
	virtual void release(void);

	/** 处理跨区桥服务器发过来的消息
	@name         : 处理其他服务器通过跨区桥服务器转发的消息
	@param actionId  : 消息码
	@param bridgeHead  : 桥消息头
	@param head  : 消息头
	@param data  : 消息内容
	@param len:消息长度
	*/
	virtual void HandleBridgeMessage(unsigned long actionId, SMsgBridgeHead &bridgeHead, SNetMsgHead* head, LPSTR data, size_t len);

	//////////////////////////////////////////////////////////////////////////
	/** 服务器之间消息处理
	@param msg 网络消息
	*/
	virtual void onTransmit(DWORD server, ulong actionId, SNetMsgHead* head, void * data, size_t len);

	//////////////////////////////////////////////////////////////////////////
	/** 来自客户端消息处理
	@param msg 网络消息
	*/
	virtual void onMessage(ClientID clientID, void * data, size_t len);

private:
    // 初始化
	void    OnInitData(void);
	/////////////////////////////////////发送消息处理/////////////////////////////////////
	// 发本区撮合信息消息给社会服
	bool	SendDataToSocial(BYTE byKeyAction,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen);

	// 发本区撮合信息消息给场景服
	bool	SendDataToZone(DWORD dwZoneServerID,BYTE byKeyAction,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen);

	// 发跨区信息消息 dwServerID:服务器ID ,跨区信息专用
	bool	SendDataBridgeToServer(DWORD dwServerID,BYTE byKeyAction,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen);

    // 发送信息到客户端
    bool    SendDataToClient(DWORD dwPDBID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen);

	/////////////////////////////////////客户端消息/////////////////////////////////////
	/** 收到请求撮合组队
	@param  IPerson * pPerson		: 提交撮合信息的人物
	@param  DWORD dwMatchTypeID		: 撮合分类ID
	@param	BYTE nRateFlag			: 倍率位标识
	@return  bool :成功否
	*/
	bool OnMsgRequestAddMatch(ClientID clientID,DWORD dwMatchTypeID,BYTE nRateFlag);

	// 场景服发给社会服请求检测取消操作事件
	void OnMsgCheckCancelMatch(ClientID clientID,DWORD dwMatchTypeID);

    // 场景服发给社会服请求检测取消操作事件
    void OnMsgAcceptMatch(ClientID clientID,DWORD dwMatchTypeID, DWORD dwCheckID);

    // 场景服发给社会服请求检测取消操作事件
    void OnMsgRefuseMatch(ClientID clientID,DWORD dwMatchTypeID, DWORD dwCheckID);

	// 确认邀请玩家进入观察竞技
	bool OnMsgConfirmInviteViewStrike(ClientID clientID,DWORD dwWorldID,DWORD dwActorID,DWORD nResult,LPSTR data, size_t len);

	// 场景服发给社会服推送排位结果
	void OnMsgRecommendRankResult(ClientID clientID, BYTE byAccept);

	/////////////////////////////////////桥服务器消息/////////////////////////////////////
	// 撮合结果处理
	void OnMatchStrikeResult(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len);

    // 撮合结果处理
    void OnMatchRequestSceneCheck(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len);

    // 请求进入撮合
    void onRequestJoinStrike(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len);

    // 同步玩家房间状态
    void onSysPlayerGameSatate(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len);
    
    // 系统自动接受比赛
    void OnMsgAutoAcceptMatch(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len);
};


#endif // __WarMatch_h__