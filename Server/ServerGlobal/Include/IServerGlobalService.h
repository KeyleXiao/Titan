/*******************************************************************
** 文件名:	IServerGlobalService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#pragma once

struct IFramework;
struct rkt::IEventEngine;
struct ITimeSyncService;
struct ISchemeCenter;
struct ITimerService;
struct ICenterConnectorService;
struct IGatewayAcceptorService;
struct ISceneManager;
namespace rkt{ struct IConditionFactory; }
struct IEffectFactory;
struct ISpellFactory;
struct IFreezeServer;
struct IBuffServer;
struct IDamageServer;
struct IAIServer;
struct ITankServer;
struct IEnmityServer;
struct ITestServer;
struct IPathFindManager;
struct IMessageDispatch;
struct IMatchScene;
struct IWarManager;
struct IGameResService;
struct IMigrateService;
struct IShareReceiver;
struct IShareSceneSvrSource;
struct IChatScene;
struct IOrderScene;
struct IActorMgr;
struct IClanScene;
struct IKinScene;
struct IAIFactory;
struct ITeamScene;
struct IOSSLogServer;
struct IAntiAddictServer;
struct ILegendCupScene;
struct IMentorshipScene;
struct IStaticWarScene;
struct IMailScene;
struct IResourceManager;
struct IMiscManager;
class HeightFieldMgr;

#include "PackagePtr.h"

// 接口
struct IServerGlobalService
{
	typedef unsigned long  ClientID; 
	//////////////////////////////////////////////////////////////////////////
	// 创建
	virtual bool						create() = 0;

	// 释放
	virtual void						release() = 0;

    // 消息分发器
    virtual IMessageDispatch*           getMessageDispatch() = 0;

	// 获取框架接口
	virtual IFramework *				getFramework() = 0;

	// 获取配置中心接口
	virtual ISchemeCenter *				getSchemeCenter() = 0;

	// 取得场景管理器
	virtual ISceneManager *				getSceneManager() = 0;

	// 取得配置服务
	virtual ISchemeEngine *				getSchemeEngine() = 0;

	// 取得条件工厂
	virtual IConditionFactory *			getConditionFactory() = 0;

	// 取得效果工厂
	virtual IEffectFactory *			getEffectFactory() = 0;

	// 取得技能工厂
	virtual ISpellFactory *				getSpellFactory() = 0;

	// 取得冷却服务器
	virtual IFreezeServer *				getFreezeServer() = 0;

	// 取得Buff服务
	virtual IBuffServer *				getBuffServer() = 0;

	// 取得伤害服务器
	virtual IDamageServer *				getDamageServer() = 0;

	// 取得tank服务器
	virtual ITankServer *				getTankServer() = 0;

	// 取得仇恨服务器
	virtual IEnmityServer *				getEnmityServer() = 0;

    // 取得测试服务器
    virtual ITestServer *               getTestServer() = 0;

	// 取得寻路管理器
	virtual IPathFindManager *			getPathFindManager() = 0;

	// 比赛大厅场景服
	virtual IMatchScene*				getMatchScene() = 0;

	virtual IWarManager*				getWarManager() = 0;

	virtual IShareReceiver*				getShareReceiver() = 0;
	virtual IShareSceneSvrSource*		getShareSceneSvrSource() = 0;

    virtual IOrderScene*                getOrderScene() = 0;

    virtual IActorMgr*                  getActorMgr() = 0;

    virtual IClanScene*                 getClanScene() = 0;

    virtual IKinScene*                  getKinScene() = 0;

    virtual IAIFactory*                 getAIFactory() = 0;                

    // 取得聊天场景服
    virtual IChatScene*					getChatScene() = 0;

    // 取得聊天场景服
    virtual ITeamScene*					getTeamScene() = 0; 

	// 取得防沉迷服务器
	virtual IAntiAddictServer*			getAntiAddictServer() = 0;

    // 取资源管理对象
    virtual IResourceManager*			getResourceManager() = 0;

    // 取杂项管理对象
    virtual IMiscManager*				getMiscManager() = 0;

    virtual ILegendCupScene*            getLegendCupScene() = 0;

	// 取得师徒场景服
	virtual IMentorshipScene*           getMentorshipScene() = 0;

    // 取得师徒场景服
	virtual IStaticWarScene*            getStaticWarScene() = 0;

	// 取得邮件场景服
	virtual IMailScene*					getMailScene() = 0;

    virtual HeightFieldMgr*             getHeightFieldMgr() = 0;

    //////////////////////////////////////////////////////////////////////////
    // 请求跨Service处理服务控制，有些数据需要在Service上才能处理
    virtual void                        requestHandleServerControl(DWORD dwState) = 0;
    
    // 请求管理器启动
    virtual bool                        requestManagerStart(DWORD dwStartFlag, DWORD dwUserData) = 0;
	///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void                        handleServerListUpdated() = 0;

	/**
	@name         : 通知服务器信息更新
	@brief        : 例如服务器人数变化等等
	*/
	virtual void                        handleServerInfoUpdated(DWORD ServerID,BYTE nState,void* pInfo, size_t nInfolen) = 0;

    /**
	@name         : 处理其他服务器通过中心服务器转发的消息
	@brief        : 具体内容不清楚
	@param server : 该消息是哪个服务器发过来的
	@param pData  : 消息内容
	@param wDataLen:消息长度
	*/
	virtual void                        handlePostMessage(DWORD server,const char * pData,WORD wDataLen) = 0;
	///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
    // 网关服务器通知游戏服务器有一个新的客户端连进来
	virtual void                        handleClientIncoming(ClientID client) = 0;

	// 网关服务器通知游戏服务器有新的数据包到达
	virtual void						handleGateWayReceivedData(ClientID client, void* data, size_t len) = 0;

	// 网关服务器通知游戏服务器有连接断开
	virtual void						handleGateWayClientDisconnect(ClientID client,DWORD dwReason) = 0;

    ///////ManagerConnector///////////////////////////////////////////////////////////////////
    // 收到设置游戏ID
    virtual void                        handleManagerSetGameWorldID() = 0;

    // 收到管理器下发的资源数据
    virtual void                        handleManagerResourceData(LPVOID pData,DWORD dwDataLen) = 0;

    // 收到管理器下发的资源数据结果
    virtual void                        handleManagerResourceResult(LPVOID pData,DWORD dwDataLen) = 0;
    ////////////////////////////////////////////////////////////////////////////////////////////////    
};