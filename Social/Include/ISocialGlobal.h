/*******************************************************************
** 文件名:	ISocialGlobal.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#pragma once
#include "GatewayServiceDef.h"

struct IFramework;
struct IMessageDispatch;
struct rkt::IEventEngine;
struct ITimeSyncService;
struct ISchemeCenter;
struct ITimerService;
struct ICenterConnectorService;
struct IVoiceConnectorService;
struct IGatewayAcceptorService;
struct IBridgeConnectorService;
struct IManagerConnectorService;
struct ISNSConnectorService;
struct ISceneManager;
struct SERVICE_ID;
struct IMatchManager;
struct SDBPlayerInfo;
using Gateway::ClientID;

struct IDBEngineService;
struct IShareReceiver;
struct IShareSceneSvrSource;
struct IChatManager;
struct IKinManager;
struct IClanManager;
struct ITeamManager;
struct IMentorshipManager;

struct IServerController;
struct IOrderManager;
struct ILegendCupManager;
struct ISNSManager;
struct ILuaManager;
struct IMailManager;

// 网络消息处理接口
struct INetMessageHandler;
struct IMiscManager;
// 资源管理接口
struct IResourceManager;
struct IOSSLogService;
struct ITaskManager;
struct IDateTriggerService;


// 在SocialGlobal里的对象都必须是不会动态卸载的
struct ISocialGlobal
{
	/// 获取框架接口
	virtual IFramework*                 getFramework() = 0;

	/// 获得事件中心接口
	virtual IEventEngine*               getEventEngine() = 0;

	/// 获得对时服务接口
	virtual ITimeSyncService*           getTimeSyncService() = 0;

	/// 获取时间轴接口
	virtual ITimerService*              getTimerService() = 0;

	/// 获取配置中心接口
	virtual ISchemeCenter*              getSchemeCenter() = 0;

	/// 获取服务器ID
	virtual unsigned short              getServerID() = 0;

	/// 获取中心服连接器接口
	virtual ICenterConnectorService*    getCenterConnectorService() = 0;

	/// 获取网关连接器接口
	virtual IGatewayAcceptorService*    getGatewayAcceptorService() = 0;

	// 获取桥连接器接口
	virtual IBridgeConnectorService *	getBridgeConnectorService() = 0;

	// 获取管理器连接器接口
	virtual IManagerConnectorService *	getManagerConnectorService() = 0;

	/// 取得配置服务
	virtual ISchemeEngine*              getSchemeEngine() = 0;

	// 消息分发器
	virtual IMessageDispatch*	        getMessageDispatch() = 0;

	// 获取网络消息处理接口
	virtual INetMessageHandler*			getNetMessageHandler() = 0;

    // 取杂项管理对象
    virtual IMiscManager*				getMiscManager() = 0;

    // 获取资源管理接口
    virtual IResourceManager  *         getResourceManager() = 0;

	// 设置SocialGlobal的SERVICE_ID
	virtual void                        setSocialGlobalService(SERVICE_ID serviceID) = 0;

	// 取得SocialGlobal的SERVICE_ID
	virtual SERVICE_ID                  getSocialGlobalService() = 0;

	virtual IMatchManager*				getMatchManager() = 0;

    virtual IClanManager*				getClanManager() = 0;

    virtual IKinManager*				getKinManager() = 0;

	virtual IChatManager*				getChatManager() = 0;

    virtual IOrderManager*				getOrderManager() = 0;

	// 取得DBEngine服务
	virtual IDBEngineService*			getDBEngineService(void) = 0;

	// 取得数据共享接收器
	virtual IShareReceiver *			getShareReceiver(void) = 0;

	// 取得场景服的数据共享源
	virtual IShareSceneSvrSource *		getShareSceneSvrSource(void) = 0;

	// 设置共享对象
	virtual void						setShareObject(IShareReceiver * pShareReceiver, IShareSceneSvrSource *pShareSceneSvrSource) = 0;

	// 启动共享
	virtual bool						startShareServer(void) = 0;

    /// 获取日期触发器接口
    virtual IDateTriggerService*		getDateTriggerService() = 0;

    // 取得服务器控制器
    virtual IServerController*          getServerControllor() = 0;
	
	    // 取得组队管理
    virtual ITeamManager*				getTeamManager() = 0;

    /// 获取语音服连接器接口
    virtual IVoiceConnectorService*    getVoiceConnectorService() = 0;
	
	    // 取得组队管理
    virtual ILegendCupManager*			getLegendCupManager() = 0;
	virtual IMentorshipManager*			getMentorshipManager() = 0;


	// 获取SNS连接器接口
	virtual ISNSConnectorService *		getSNSConnectorService() = 0;

	// 获取SNS服务
	virtual ISNSManager *				getSNSManager() = 0;

    // 获取Lua服务
    virtual ILuaManager *				getLuaManager() = 0;

	// 获取OSS日志服务
	virtual IOSSLogService *            getOSSLogService() = 0;

    // 取任务数据操作管理对象
    virtual ITaskManager*				getTaskManager() = 0;

	// 获取邮件服务
	virtual IMailManager*				getMailManager() =  0;

};

#define gSocialGlobal	((ISocialGlobal*)::rkt::getGlobal())
