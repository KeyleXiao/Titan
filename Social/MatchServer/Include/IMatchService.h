/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IMatchService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/18/2015	12:40
** 版  本:	1.0
** 描  述:	
            比赛大厅服务提供的接口
********************************************************************/

#pragma once
 
#include "Match_Define.h"
#include "GatewayServiceDef.h"
#include "BridgeModuleDef.h"
#include "MatchDef.h"
#include "LegendCupDef.h"

class MatchService;
using Gateway::ClientID;

struct  IMatchService
{
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void* data, size_t len) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void* data, size_t len) = 0;

    // 释放
    virtual void release() = 0;

	// 断开连接
	virtual void onClientDisconnect(ClientID client,DWORD dwReason) = 0;

    virtual ClientID getClientID(DWORD dwDBID) = 0;

    // 设置角色对应的ROOMID
    virtual bool setActorRoomID(PDBID idActor, DWORD dwRoomID)= 0;

    // 移除角色对应的ROOMID
    virtual bool removeActorRoomID(PDBID idActor, DWORD dwRoomID)= 0;

    // 获取玩家所在房间ID
    // 返回房间ID
    virtual DWORD getRoomIDByActorID(PDBID idActor)= 0;

    // 根据房间ID获取房间人员列表
    // 返回人员数量
    virtual DWORD getPlayerListByRoomID(DWORD dwRoomID, PDBID* pReturnArray, DWORD dwArrayMaxSize)= 0;

    // 根据房间ID获取房间人员列表
    // 返回是否创建成功
    virtual DWORD createLegendCupRoom(SCreateLegendCupRoomInfo sCreateCupRoomInfo)= 0;

	// 移除线下杯赛房间
	virtual bool removeLegendCupRoom(DWORD dwRoomID)= 0;

    // 进入杯赛房间
    virtual bool joinLegendCupRoom(SMsgJoinLegendCupRoom sJoinLegendCupRoom)= 0;

    // 根据房间ID获取房间聊天列表
    // 返回人员数量
    virtual DWORD getChatRoomPlayerList(DWORD dwRoomID, DWORD dwActorID, PDBID* pReturnArray, DWORD dwArrayMaxSize)= 0;

};



// 跨服竞技撮合系统服务接口定义
struct IWarMatch
{

	// 释放
	virtual void Release() = NULL;

    // 释放
    virtual bool Create(MatchService *pWarService, EMMatchType eMatchType) = NULL;

    // 玩家登出
    virtual void ActorLogOut(PDBID ActorID, string szName) = NULL;

	/** 设定可进入公共区
	@param   bool bFlag	: 是否可进
	*/
	virtual void SetCanEnterPublicWorldStrike(bool bFlag=true) = NULL;

    //////////////////////////////////////////////////////////////////////////
    // 服务器竞技撮合信息子消息
    virtual void onTransmit(ulong endpointId, ulong actionId, SNetMsgHead* head, void* data, size_t len) = NULL;

    //  处理跨区桥服务器发过来的消息
    virtual void HandleBridgeMessage(unsigned long actionId, SMsgBridgeHead &bridgeHead, SGameMsgHead* head, LPSTR data, size_t len) = NULL;

    // 退出撮合
    virtual void exitMatchNode(DWORD dwMatchID, PDBID pExitPDBID) = NULL;

    // 回收匹配节点
    virtual bool recycleMatchNodeID(DWORD dwMatchID) = NULL;

    // 整理合并撮合节点
    virtual bool trimAndMergeMatchNode(DWORD dwMatchNodeID,map<DWORD,bool> &mapMatchNodeHaveProcess) = NULL;

    // 检测节点撮合开始条件
    virtual bool checkMatchTogetherResult(DWORD dwMatchType, DWORD dwID, vector<DWORD> &MatchNodeList, bool &overTimeAddComputer) = NULL;

    // 主任务处理
    virtual void DoTask() = NULL;

    // 全部检测成功创建撮合房间
    virtual bool allAcceptCreateMatchRoom(bool overTimeAddComputer, vector<DWORD> &MatchNodeList) = NULL;

    // 退出检测
    virtual void refuseAcceptCheck(DWORD dwMatchNodeID, PDBID refusePDBID) = NULL;

    // 移除确定检测相关
    virtual void deleteCheckIDInfo(DWORD dwCheckID, vector<DWORD> MatchNodeList) = NULL;

    // 接收参赛检测
    virtual void acceptCheck(vector<DWORD> &MatchNodeList) = NULL;

    // 发送撮合开放标示
    virtual void sendActorLoadedInfo(DWORD dwFromWorldID,DWORD dwActorID) = NULL;
};




// Match工厂类接口
struct IMatchFactory
{
	// 加载Match工厂
	virtual bool            Load()= 0;

	// 释放
	virtual void			release() = 0;

    /**
	 创建所有Match相关
	*/
	virtual bool		    createMatch(MatchService *pWarService) = 0;

	// 某个Match是否存在
	virtual bool			IsExistMatchTypeID(EMMatchType eMatchTypeID) = 0;

    // 取得ID是否存在
    virtual IWarMatch*		getMatchByTypeID(EMMatchType eMatchTypeID) = 0;

    // 登出处理
    virtual void		    ActorLogOut(PDBID ActorID,string szName) = 0;

    // 客户端加载完成信息
    virtual void            sendActorLoadedInfo(DWORD dwFromWorldID,DWORD dwActorID) = 0;
};