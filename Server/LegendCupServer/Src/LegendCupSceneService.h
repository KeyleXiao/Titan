/*******************************************************************
** 文件名:	E:\speed\Server\LegendCupServer\Src\LegendCupSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜
********************************************************************/

#pragma once
#include "ILegendCupSceneService.h"
#include "NetMessageDef.h"
#include "IMessageHandler.h"
#include "IMessageDispatch.h"
#include "IDBRetSink.h"

class LegendCupScenService:public ILegendCupSceneService, public ITransmitHandler,public IMessageHandler, public IDBRetSink
{

public:

	LegendCupScenService();
     //
	////////////////////////////////////////ITransmitHandler//////////////////////////////////
	/** 消息处理
	@param server	源服务器ID
	@param uMsgID	消息ID
	@param head		消息头
	@param data		消息
	@param len		消息长度
	*/
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void	onServerListUpdated();

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	// 
	///////////////////////////////////IMessageHandler///////////////////////////////////////
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

    ///////////////////////////////////ILegendCupSceneService///////////////////////////////////////
    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen);

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen);

    //////////////////////////////////IDBRetSink///////////////////////
    // 数据库请求返回回调方法
    virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);


	// LegendCupSceneServer
	//////////////////////////////////////////////////////////////////////////
	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();
private:
    
    // purpose: 生成比赛ID
    LONGLONG createDBLegendCupId(int nWorld, int nServerID );

    // purpose: 处理房间相关消息
    void	onMsgCreateCup( ClientID clientID, void* pData, size_t stLen );

    // purpose: 处理请求创建杯赛类型
    void	onMsgCreateCupType( ClientID clientID, void* pData, size_t stLen );

    // purpose: 处理房间相关消息
    void	onMsgReqJoinLegendcupRoom( ClientID clientID, void* pData, size_t stLen );
    
    // purpose: 创建杯赛				-- 来自数据库
	void    onReturnCreateLegendCup(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

    //  purpose: 校验创建杯赛成功
    void	onMsgCreateCupResult( void* pData, size_t nLen );

    // purpose: 申请加入杯赛扣除相关
    void	onMsgReqJoinCup( void* pData, size_t nLen );

private:
	unsigned short m_u16ServerID;
	volatile	LONG  m_mutex;
};