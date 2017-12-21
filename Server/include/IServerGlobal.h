/*******************************************************************
** 文件名:	IServerGlobal.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
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
struct IVoiceConnectorService;
struct IGuardConnectorService;
struct IGatewayAcceptorService;
struct IBridgeConnectorService;
struct IManagerConnectorService;
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
struct IChatScene;
struct IWarManager;
struct IGameResService;
struct SERVICE_ID;
struct IMigrateService;
struct IDBEngineService;
struct IShareReceiver;
struct IShareSceneSvrSource;
struct IActorMgr;
struct IClanScene;
struct IKinScene;
struct IAIFactory;
struct IServerController;
struct ITeamScene;
struct IOSSLogService;
struct IOrderScene;
struct IAntiAddictServer;
struct ILegendCupScene;
struct IMentorshipScene;
struct IStaticWarScene;
struct IMailScene;

// 网络消息处理接口
struct INetMessageHandler;
struct IMiscManager;
// 资源管理接口
struct IResourceManager;

#include "MatchDef.h"

class HeightFieldMgr;

// 在ServerGlobal里的对象都必须是不会动态卸载的
struct IServerGlobal
{
    // 取得场景服的工作目录
    virtual const char* getWorkDir(void) = 0;

    // 是否开启七龙珠功能
    virtual bool isOpenSupperStone(void) = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual void stop() = 0;

    /// 获取框架接口
	virtual IFramework *   getFramework() = 0;

	/// 获得事件中心接口
	virtual IEventEngine *  getEventEngine() = 0;

	/// 获得对时服务接口
	virtual ITimeSyncService *  getTimeSyncService() = 0;

	/// 获取时间轴接口
	virtual ITimerService * getTimerService() = 0;

	/// 获取配置中心接口
	virtual ISchemeCenter * getSchemeCenter() = 0;

	/// 获取服务器ID
	virtual unsigned short getServerID() = 0;

	// 取得场景管理器
	virtual ISceneManager * getSceneManager() = 0;

	/// 获取中心服连接器接口
	virtual ICenterConnectorService * getCenterConnectorService(bool ignoreCache=false) = 0;

    // 获取语音服连接器
    virtual IVoiceConnectorService* getVoiceConnectorService(bool ignoreCache=false) = 0;

    // 获取数据监护连接器接口
    virtual IGuardConnectorService * getGuardConnectorService(bool ignoreCache=false) = 0;

	/// 获取网关连接器接口
	virtual IGatewayAcceptorService * getGatewayAcceptorService(bool ignoreCache=false) = 0;

	// 获取桥连接器接口
	virtual IBridgeConnectorService * getBridgeConnectorService(bool ignoreCache=false) = 0;

	// 获取管理器连接器接口
	virtual IManagerConnectorService * getManagerConnectorService(bool ignoreCache=false) = 0;

	/// 取得配置服务
	virtual ISchemeEngine * getSchemeEngine() = 0;

	// 取得条件工厂
	virtual IConditionFactory * getConditionFactory() = 0;

	// 取得效果工厂
	virtual IEffectFactory * getEffectFactory() = 0;

	// 取得技能工厂
	virtual ISpellFactory * getSpellFactory() = 0;

	// 取得冷却服务器
	virtual IFreezeServer * getFreezeServer() = 0;

	// 取得Buff服务
	virtual IBuffServer *getBuffServer() = 0;

	// 取得伤害服务器
	virtual IDamageServer *getDamageServer() = 0;

	// 取得tank服务器
	virtual ITankServer *getTankServer() = 0;

	// 取得仇恨服务器
	virtual IEnmityServer *getEnmityServer() = 0;

    // 取得测试服务器
	virtual ITestServer *getTestServer() = 0;

	// 取得寻路管理器
	virtual IPathFindManager *getPathFindManager() = 0;

	// 消息分发器
	virtual IMessageDispatch*	getMessageDispatch() = 0;

	// 获取网络消息处理接口
	virtual INetMessageHandler*	getNetMessageHandler() = 0;

	// 比赛大厅场景服
	virtual IMatchScene* getMatchScene() = 0;

	virtual IWarManager* getWarManager() = 0;

	virtual IGameResService* getGameResService() = 0;

	// 设置ServerGlobal的SERVICE_ID
	virtual void setServerGlobalService(SERVICE_ID serviceID) = 0;

	// 取得ServerGlobal的SERVICE_ID
	virtual SERVICE_ID getServerGlobalService() = 0;

    virtual IMigrateService*				getMigrateService() = 0;

	// 取得DBEngine服务
	virtual IDBEngineService*				getDBEngineService(void) = 0;

	// 取得数据共享接收器
	virtual IShareReceiver *				getShareReceiver(void) = 0;

	// 取得场景服的数据共享源
	virtual IShareSceneSvrSource *			getShareSceneSvrSource(void) = 0;
	
    // 启动共享
    virtual bool						    startShareServer(void) = 0;

    // 取得角色管理器
    virtual IActorMgr*                      getActorMgr() = 0;

    virtual IClanScene*                     getClanScene() = 0;

    virtual IKinScene*                      getKinScene() = 0;

    virtual IAIFactory*                     getAIFactory() = 0;

	// 聊天场景服
	virtual IChatScene*                     getChatScene() = 0;

    virtual ITeamScene*                     getTeamScene() = 0;

    virtual IOrderScene*                    getOrderScene() = 0;

    virtual ILegendCupScene*                getLegendCupScene() = 0;

    // 取得服务器控制器
    virtual IServerController*              getServerControllor() = 0;

    // 取得日记服务
    virtual IOSSLogService *                getOSSLogService() = 0;

	// 取防沉迷服务器
	virtual IAntiAddictServer*				getAntiAddictServer() = 0;

    // 取杂项管理对象
    virtual IMiscManager*				    getMiscManager() = 0;

    // 获取资源管理接口
    virtual IResourceManager  *             getResourceManager() = 0;

	// 取得师徒服务器
	virtual IMentorshipScene*               getMentorshipScene() = 0;

    // 战绩相关
    virtual IStaticWarScene*                getStaticWarScene() = 0;

	// 取得邮件服务
	virtual IMailScene*						getMailScene() = 0;

    virtual HeightFieldMgr*                 getHeightFieldMgr() = 0;
};

#define gServerGlobal	((IServerGlobal*)::rkt::getGlobal())