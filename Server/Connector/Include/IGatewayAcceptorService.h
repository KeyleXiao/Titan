/*******************************************************************
** 文件名:	IGatewayAcceptorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/28/2014
** 版  本:	1.0
** 描  述:	

			用户连接接受服务
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "GatewayServiceDef.h"
#include "packagePtr.h"
using namespace Gateway;


// 组合网关角色数据
#define COMBIN_ACTORDATA(ScenceID,Service_ID)		(( ((LONGLONG)Service_ID) << 32) | ScenceID)
// 取得角色所在的服务ID
#define GATEWAY_ACTORDATA_TO_SERVICE_ID(ActorData)	( ((LONGLONG)ActorData) >> 32)
// 取得角色所在的场景ID
#define GATEWAY_ACTORDATA_TO_SCENEID(ActorData)		(ActorData&0xFFFFFFFF)

struct SBattleResult;
	
/**
	职责: 
	1.接受用户连接，创建Actor服务
	2.将收到的网络消息，转发给Actor处理
	3.根据UID,获取Actor代理接口
*/
struct IGatewayAcceptorService
{
	/**
	连接某个网关 : 这个函数可以多次调用，因为存在多个网关
	通常调用的时机是:连接中心服成功后，发现了网关服务器，就调用一下连接
	*/
	virtual bool connectGateway( string ip,int port ) = 0;

    // nFlag:SEND_DATA_FLAG
	virtual void sendData( ClientID client,PACKAGE_PTR package,int nFlag = 0 ) = 0;

	/**
	@name         : 向多个客户端广播数据,即使这些客户端不在同一个网关上
	@brief        : 使用这个函数可以显著减少服务器间的数据传输量,因为如果在同一个网关上的客户端只需发送一条消息
	@param pClientArray: 要发送的客户端ID数组
	@param wClientNum  : 客户端数目
	@param pData       : 数据缓冲区地址
	@param wDataLen    : 数据长度
	@return       : 
	*/
	virtual void broadcast(ClientID * pClientArray,WORD wClientNum,LPVOID pData,WORD wDataLen) = 0;     

    virtual void plug(ClientID client, SERVICE_ID id) = 0;

    virtual void unplug(ClientID client) = 0;

    // 踢人
    virtual void kickOutClient(ClientID client, int nReason) = 0;

    // 获取sessionID
    virtual SessionID getSession(ClientID client) = 0;

    // 某个session是否使用过
    virtual bool isSessionUsed(SessionID session) = 0;

    // 通知网关客户端迁移
    virtual void migrateClient(ClientID client, SessionID session, int nZoneServerID, int nSceneID) = 0;

    // 获取IP地址
    virtual string getIpAddress(ClientID client) = 0;
}; 

