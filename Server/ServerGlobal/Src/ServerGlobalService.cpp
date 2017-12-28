/*******************************************************************
** 文件名:	ServerGlobalService.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "IServerGlobal.h"
#include "ServerGlobalService.h"
#include "IActorService.h"
#include "IWarService.h"
#include "IActorMgr.h"
#include "IServerController.h"
#include "IManagerConnectorService.h"
#include "IVoiceConnectorService.h"
#include "DManagerModule.h"
#include "MiscManager.h"
#include "ResourceManager.h"
#include "EntityHelper.h"
#include "IMigrateService.h"
#include "DBHttpService.h"

ServerGlobalService::ServerGlobalService() 
		: m_pFramework(0)
		, m_nServerID(0)
		, m_pCenterConnector(0)
		, m_pGatewayAcceptor(0)
		, m_pSceneManager(0)
		, m_pSchemeCenter(0)
		, m_pSchemeEngine(0)
		, m_pConditionFactory(0)
		, m_pEffectFactory(0)
		, m_pSpellFactory(0)
		, m_pFreezeServer(0)
		, m_pBuffServer(0)
		, m_pDamageServer(0)
		, m_pTankServer(0)
		, m_pEnmityServer(0)
        , m_pTestServer(0)
		, m_pPathFindManager(0)
		, m_pMatchScene(0)
		, m_pWarManager(0)	
		, m_pGameResService(0)
		, m_pShareReceiver(0)
		, m_pShareSceneSvrSource(0)
		, m_pChatScene(0)
        , m_pOrderScene(0)
        , m_nCurPlayerNum(0)
        , m_pServerInfo(0)
        , m_pActorMgr(0)
		, m_pClanScene(0)
		, m_pKinScene(0)
        , m_pAIFactoryWrapper(0)
		, m_pTaskServer(0)
        , m_pTeamScene(0)
        , m_pOssLogServer(0)
		, m_pAntiAddictServer(0)
        , m_pResourceManager(0)
        , m_pMiscManager(0)
        , m_pLegendCupScene(0)
		, m_pMentorshipScene(0)
        , m_pStaticWarScene(0)		
        , m_dwRetryStartFlag(0)
        , m_dwRetryUserData(0)
		, m_pMailScene(0)
		, m_pActionLib(0)
        , m_pHeightFieldMgr(0)
        , m_pDBHttpService(0)
{

}

ServerGlobalService::~ServerGlobalService()
{

}

// 启动
// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
bool ServerGlobalService::on_start(void * data,int len )
{
	return create();
}

// 停止
bool ServerGlobalService::on_stop()
{
    TraceLn(__FUNCTION__);

	IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
	if(pEventEngine)
	{
		// Global服务已关闭
		pEventEngine->FireExecute(EVENT_SYSTEM_GLOBAL_STOP, SOURCE_TYPE_SYSTEM, 0, NULL, 0);

        // 取消订阅 数据守护连接器服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_GUARDCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0);

        // 取消订阅 迁移服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_MIGRATE_STARTED, SOURCE_TYPE_SYSTEM, 0);

        // 取消订阅 中心连接器服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_CENTERCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0);
        // 取消订阅 网关连接器服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0);
        // 取消订阅 桥连接器服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_BRIDGECONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0);
        // 取消订阅 DB引擎服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_DBENGINE_STARTED, SOURCE_TYPE_SYSTEM, 0);

        // 取消订阅 场景创建
        pEventEngine->UnSubscibe(this,EVENT_SCENE_CREATED, SOURCE_TYPE_SCENE, 0);

        // 取消订阅 场景销毁
        pEventEngine->UnSubscibe(this,EVENT_SCENE_DESTROY, SOURCE_TYPE_SCENE, 0);

        // 取消订阅 角色登进
        pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0);

        // 取消订阅 角色登出
        pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_LOGOUT, SOURCE_TYPE_PERSON, 0);

		// 取消订阅 预进入场景人数更新
        pEventEngine->UnSubscibe(this, EVENT_SCENE_PRE_ENTER_SCENE_UPDATE, SOURCE_TYPE_SCENE, 0);

		// 取消订阅 与中心服握手成功
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_CENTER_HANDSHAKE_OK, SOURCE_TYPE_SYSTEM, 0);
	}

    m_serverMessageHanlder.release();

    // 杀掉定时器
    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->KillTimer(ETIMER_CHECK_BASESERVICE, CAST_TYPE(ITimerHandler*, this));
        pTimerService->KillTimer(ETIMER_MANAGER_RETRYSTART, CAST_TYPE(ITimerHandler*, this));
    }

    // 释放子模块的顺序最好和初始化顺序相反
    if(m_pDBHttpService)            m_pDBHttpService->onStop();
    if(m_pShareReceiver)            m_pShareReceiver->onStop();
    if(m_pShareSceneSvrSource)      m_pShareSceneSvrSource->onStop();
    if(m_pWarManager)               m_pWarManager->onStop();
#ifdef OPEN_TEST
    if(m_pTestServer)               m_pTestServer->onStop();
#endif
    if(m_pEnmityServer)             m_pEnmityServer->onStop();
    if(m_pTankServer)               m_pTankServer->onStop();
    if(m_pDamageServer)             m_pDamageServer->onStop();
    if(m_pBuffServer)               m_pBuffServer->onStop();
    if(m_pFreezeServer)             m_pFreezeServer->onStop();
    if(m_pSpellFactory)             m_pSpellFactory->onStop();
    if(m_pEffectFactory)            m_pEffectFactory->onStop();
    if(m_pConditionFactory)         m_pConditionFactory->onStop();
    if(m_pMatchScene)               m_pMatchScene->onStop();
    if(m_pChatScene)                m_pChatScene->onStop();
    if(m_pOrderScene)               m_pOrderScene->onStop();
    if(m_pKinScene)                 m_pKinScene->onStop();
    if(m_pClanScene)                m_pClanScene->onStop();
    if(m_pAIFactoryWrapper)         m_pAIFactoryWrapper->close();
    if(m_pTaskServer)               m_pTaskServer->onStop();
    if(m_pTeamScene)                m_pTeamScene->onStop();
    if(m_pOssLogServer)             m_pOssLogServer->onStop();
	if (m_pAntiAddictServer)		m_pAntiAddictServer->onstop();
    if(m_pMiscManager)              m_pMiscManager->onStop();
    if(m_pResourceManager)          m_pResourceManager->onStop();

    if (m_pSceneManager)            m_pSceneManager->onStop();
    if (m_pLegendCupScene)          m_pLegendCupScene->onStop();
    
	if (m_pMentorshipScene)			m_pMentorshipScene->onStop();
    if (m_pStaticWarScene)          m_pStaticWarScene->onStop();
	if (m_pMailScene)				m_pMailScene->onStop();
    if (m_pActionLib)               m_pActionLib->onStop();
    if (m_pHeightFieldMgr)          m_pHeightFieldMgr->destroy();
    
    // 停掉由GLOBAL开启的服务
    stopServices();

    return true;
}

////////////////// IServerGlobalService ////////////////////////////////////////////////////////
// 释放
bool ServerGlobalService::create()
{	
	IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
	if(pEventEngine)
	{
        // 订阅  数据守护连接器服务已启动
        pEventEngine->Subscibe(this, EVENT_SYSTEM_GUARDCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);

		// 订阅 迁移服务已启动
		pEventEngine->Subscibe(this, EVENT_SYSTEM_MIGRATE_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
		
        // 订阅 中心连接器服务已启动
        pEventEngine->Subscibe(this, EVENT_SYSTEM_CENTERCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
        // 订阅 网关连接器服务已启动
        pEventEngine->Subscibe(this, EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
        // 订阅 桥连接器服务已启动
        pEventEngine->Subscibe(this, EVENT_SYSTEM_BRIDGECONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
        // 订阅 DB引擎服务已启动
        pEventEngine->Subscibe(this, EVENT_SYSTEM_DBENGINE_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);

        // 订阅场景创建事件
        pEventEngine->Subscibe(this,EVENT_SCENE_CREATED, SOURCE_TYPE_SCENE, 0, __FUNCTION__);
        
        // 订阅场景销毁事件
        pEventEngine->Subscibe(this,EVENT_SCENE_DESTROY, SOURCE_TYPE_SCENE, 0, __FUNCTION__);

        // 订阅角色登进事件
        pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0, __FUNCTION__);

        // 订阅角色登出事件
        pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_LOGOUT, SOURCE_TYPE_PERSON, 0, __FUNCTION__);

		// 订阅预进入场景人数更新
        pEventEngine->Subscibe(this, EVENT_SCENE_PRE_ENTER_SCENE_UPDATE, SOURCE_TYPE_SCENE, 0, __FUNCTION__);

        pEventEngine->Subscibe(this, EVENT_SYSTEM_CENTER_HANDSHAKE_OK, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);

		// Global服务已启动
		pEventEngine->FireExecute(EVENT_SYSTEM_GLOBAL_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
    }

    m_nServerID = gServerGlobal->getServerID();

	// 配置引擎
	TraceLn("Start loading SchemeEngine...");
	if ((m_pSchemeEngine = CreateSchemeEngineProc()) == NULL)
	{
		ErrorLn("SchemeEngine load failure!");
		return false;
	}

	//TraceLn("Start loading SchemeCenter...");
	//m_pSchemeCenter = createSchemeCenterProc();
	//if ( m_pSchemeCenter ==0 || !m_pSchemeCenter->load())
	//{
	//	ErrorLn("SchemeCenter load failure!");
	//	return false;
	//}

    TraceLn("Start Create ResourceManager...");
    m_pResourceManager = new CResourceManager();
    if ( m_pResourceManager ==0 || !m_pResourceManager->load())
    {
        ErrorLn("ResourceManager Create Failed!");
        return false;
    }

    TraceLn("Start Create MiscManager...");
    m_pMiscManager = new CMiscManager();
    if ( m_pMiscManager ==0 || !m_pMiscManager->load())
    {
        ErrorLn("MiscManager Create Failed!");
        return false;
    }	

    // 创建ManagerConnectorService
    TraceLn("Start Create ManagerConnector ...");
    m_pMiscManager->create_service("ManagerConnector");

    m_serverMessageHanlder.create();

	return true;
}

// 释放
void ServerGlobalService::release()
{
    if (m_pSceneManager)
        m_pSceneManager->clearAllScenes();

    TraceLn("Start release DBHttpService...");
    safeRelease(m_pDBHttpService);

    TraceLn("Start release ShareReceiver...");
    SAFERELEASE(m_pShareReceiver);

    TraceLn("Start release ShareSceneSvrSource...");
    SAFERELEASE(m_pShareSceneSvrSource);

    TraceLn("Start release WarManager...");
    safeRelease(m_pWarManager);

#ifdef OPEN_TEST
    TraceLn("Start release TestServer...");
    safeRelease(m_pTestServer);
#endif

    TraceLn("Start release EnmityServer...");
    safeRelease(m_pEnmityServer);

    TraceLn("Start release TankServer...");
    safeRelease(m_pTankServer);

    TraceLn("Start release DamageServer...");
    safeRelease(m_pDamageServer);

    TraceLn("Start release BuffServer...");
    safeRelease(m_pBuffServer);

    TraceLn("Start release FreezeServer...");
    safeRelease(m_pFreezeServer);

    TraceLn("Start releaseSpellFactory...");
    safeRelease(m_pSpellFactory);

    TraceLn("Start release EffectFactory...");
    safeRelease(m_pEffectFactory);

    TraceLn("Start release ConditionFactory...");
    safeRelease(m_pConditionFactory);

    TraceLn("Start release PathFindManager...");
    SAFERELEASE(m_pPathFindManager);

    TraceLn("Start release MatchScene...");
    safeRelease(m_pMatchScene);

    TraceLn("Start release SchemeCenter...");
    safeRelease(m_pSchemeCenter);

    TraceLn("Start release SchemeEngine...");
    SAFERELEASE(m_pSchemeEngine);

    TraceLn("Release SceneManager...");
    safeRelease(m_pSceneManager);

    TraceLn("Release CheatScene...");
    safeRelease(m_pChatScene);

    TraceLn("Release OrderScene...");
    safeRelease(m_pOrderScene);

    TraceLn("Release KinScene...");
    safeRelease(m_pKinScene);

    TraceLn("Release KinScene...");
    safeRelease(m_pClanScene);

    TraceLn("Release AIFactory...");
    safeRelease(m_pAIFactoryWrapper);

    TraceLn("Release TaskServer...");
    safeRelease(m_pTaskServer);

    TraceLn("Release TeamServer...");
    safeRelease(m_pTeamScene);

    TraceLn("Release OssLogServer...");
    safeRelease(m_pOssLogServer);

	TraceLn("Release AntiAddictServer...");
	safeRelease(m_pAntiAddictServer);
    
    TraceLn("Start release MiscManager...");
    safeRelease(m_pMiscManager);

    TraceLn("Start release ResourceManager...");
    safeRelease(m_pResourceManager);
	
    TraceLn("Release LegendCupScene...");
    safeRelease(m_pLegendCupScene);

	TraceLn("Release MentorshipScene...");
	safeRelease(m_pMentorshipScene);

    TraceLn("Release StaticWarScene...");
	safeRelease(m_pStaticWarScene);

	TraceLn("Release MailScene...");
	safeRelease(m_pMailScene);

	safeRelease(m_pActionLib);

    if(m_pServerInfo != NULL)
    {
        free(m_pServerInfo);
    }

    if(m_pHeightFieldMgr)
    {
        delete m_pHeightFieldMgr;
        m_pHeightFieldMgr = 0;
    }


	delete this;

    gServerGlobal->stop();
}

// 消息分发器
IMessageDispatch* ServerGlobalService::getMessageDispatch()
{
    return &m_MessageDispatch;
}

// 停掉本身启动的服务
inline void ServerGlobalService::stopServices()
{
    if(!m_pMiscManager)
    {
        return;
    }

    TraceLn("Stop BridgeService...");
    m_pMiscManager->stop_service("BridgeConnector");

    TraceLn("Stop GatewayService...");
    m_pMiscManager->stop_service("GatewayConnector");

    TraceLn("Stop VoiceService...");
    m_pMiscManager->stop_service("VoiceConnector");

    TraceLn("Stop CenterService...");
    m_pMiscManager->stop_service("CenterConnector");

    TraceLn("Stop MigrateService...");
    m_pMiscManager->stop_service("MigrateService");

    TraceLn("Stop DBEngineService...");
    m_pMiscManager->stop_service("DBEngineService");
}

// 创建服务
bool ServerGlobalService::createService(char * pszServiceSchemeName)
{
    return (m_pMiscManager->create_service(pszServiceSchemeName) != 0);
}

// 启动DB服务
inline void ServerGlobalService::startDBService()
{
    // 创建DBEngineService
    TraceLn("Start Create DBEngineService ...");
    createService("DBEngineService");
}  

// 启动迁移服务
inline void ServerGlobalService::startMigrateService()
{
    // 创建MigrateService
    TraceLn("Start Create MigrateService ...");
    createService("MigrateService");
}

// 启动中心服务
inline void ServerGlobalService::startCenterService()
{
    // 创建CeneterConnectorService
    TraceLn("Start Create CenterConnector ...");
    createService("CenterConnector");
}

// 启动网关服务
inline void ServerGlobalService::startGateWayService()
{
    // 创建GatewayConnectorService
    TraceLn("Start Create GatewayConnector ...");
    createService("GatewayConnector");
}

// 启动桥服务
inline void ServerGlobalService::startBridgeService()
{
    //////////////////////////////////////////////////////////////////////////
    // 创建BridgeConnector
    TraceLn("Start Create BridgeConnector ...");
    createService("BridgeConnector");
}

// 启动GUARD服务
inline void ServerGlobalService::startGuardService()
{
    //////////////////////////////////////////////////////////////////////////
    // 创建GuardConnector
    TraceLn("Start Create GuardConnector ...");
    createService("GuardConnector");
}

// 后续启动
inline void ServerGlobalService::startEntries(void)
{
    m_pHeightFieldMgr = new HeightFieldMgr();

    TraceLn("Start loading SchemeCenter...");
    m_pSchemeCenter = createSchemeCenterProc();
    if ( m_pSchemeCenter ==0 || !m_pSchemeCenter->load())
    {
        ErrorLn("SchemeCenter load failure!");
        return;
    }

    // 启动共享
    TraceLn("Start Create ShareServerHelper...");
    if(!gServerGlobal->startShareServer())
    {
        ErrorLn("ShareServerHelper Create Failed!");
        return;
    }

	TraceLn("Start loading PathFindManager...");
	m_pPathFindManager = CreatePathFindManagerProc();
	if ( m_pPathFindManager ==0 || !m_pPathFindManager->Load())
	{
		ErrorLn("PathFindManager load failure!");
		return;
	}

	TraceLn("Start loading MatchScene...");
	m_pMatchScene = CreateMatchSceneProc();
	if ( m_pMatchScene ==0 || !m_pMatchScene->load())
	{
		ErrorLn("MatchScene load failure!");
		return;
	}

    TraceLn("Start loading ClanScene...");
    m_pClanScene = CreateClanSceneProc();
    if ( m_pClanScene ==0 || !m_pClanScene->load())
    {
        ErrorLn("ClanScene load failure!");
        return;
    }

    TraceLn("Start loading KinScene...");
    m_pKinScene = CreateKinSceneProc();
    if ( m_pKinScene ==0 || !m_pKinScene->load())
    {
        ErrorLn("KinScene load failure!");
        return;
    }

	TraceLn("Start loading ChatScene...");
 	m_pChatScene = CreateChatSceneProc();
 	if ( m_pChatScene == 0 || !m_pChatScene->load())
 	{
 		ErrorLn("ChatScene load failure!");
 		return;
 	}

    TraceLn("Start loading AIFactory...");
    m_pAIFactoryWrapper = CreateAIFactoryWrapperProc();
    if(m_pAIFactoryWrapper == 0 || !m_pAIFactoryWrapper->load())
    {
        ErrorLn("AIFactory load failure!");
        return;
    }

	TraceLn("Start loading ConditionFactory...");
	m_pConditionFactory = CreateConditionFactoryProc();
	if ( m_pConditionFactory ==0 || !m_pConditionFactory->Load("Scp\\condition.bin"))
	{
		ErrorLn("ConditionFactory load failure!");
		return;
	}

	TraceLn("Start loading EffectFactory...");
	m_pEffectFactory = CreateEffectFactoryProc();
	if ( m_pEffectFactory ==0 || !m_pEffectFactory->Load("Scp\\effect.csv") || !m_pEffectFactory->Load("Scp\\effect.bin"))
	{
		ErrorLn("EffectFactory load failure!");
		return;
	}

	TraceLn("Start loading SpellFactory...");
	m_pSpellFactory = CreateSpellFactoryProc();
	if ( m_pSpellFactory ==0 || !m_pSpellFactory->Load(SPELL_FILENAME, SPELL_LOGIC_FILENAME))
	{
		ErrorLn("SpellFactory load failure!");
		return;
	}

	TraceLn("Start loading FreezeServer....");
	m_pFreezeServer = CreateFreezeServerProc();
	if ( m_pFreezeServer == 0)
	{
		ErrorLn("FreezeServer load failure!");
		return;
	}

	TraceLn("Start loading BuffServer ....");
	m_pBuffServer = CreateBuffServerProc();
	if ( m_pBuffServer == 0 || !m_pBuffServer->create())
	{
		ErrorLn("BuffServer load failure!");
		return;
	}

	TraceLn("Start loading DamageServer ....");
	m_pDamageServer = CreateDamageServerProc();
	if ( m_pDamageServer == 0)
	{
		ErrorLn("DamageServer load failure!");
		return;
	}
	TraceLn("Start loading TankServer...");
	m_pTankServer = CreateTankServerProc();
	if ( m_pTankServer == 0)
	{
		ErrorLn("TankServer load failure!");
		return;
	}
	TraceLn("Start loading Enmity Server...");
	m_pEnmityServer = CreateEnmityServerProc();
	if ( m_pEnmityServer == 0 )
	{
		ErrorLn("Start loading Enmity Server failure");
		return;
	}

    TraceLn("Start loading Enmity Server...");
    m_pOrderScene = CreateOrderSceneProc();
    if ( m_pOrderScene == 0 || !m_pOrderScene->load() )
    {
        ErrorLn("Start loading Order Server failure");
        return;
    }

    TraceLn("Start loading Team Server...");
    m_pTeamScene = CreateTeamSceneProc();
    if ( m_pTeamScene == 0 || !m_pTeamScene->load())
    {
        ErrorLn("Start loading Team Server failure");
        return;
    }

	TraceLn("Start loading MentorshipScene...");
	m_pMentorshipScene = CreateMentorshipSceneProc();
	if ( m_pMentorshipScene ==0 || !m_pMentorshipScene->load())
	{
		ErrorLn("MentorshipScene load failure!");
		return;
	}

    TraceLn("Start loading StaticWarScene...");
	m_pStaticWarScene = CreateStaticWarSceneProc();
	if ( m_pStaticWarScene ==0 || !m_pStaticWarScene->load())
	{
		ErrorLn("MentorshipScene load failure!");
		return;
	}    

#ifdef OPEN_TEST
    // 测试版本启动测试服务
    TraceLn("Start loading Test Server...");
    m_pTestServer = CreateTestServerProc();
    if ( m_pTestServer == 0 )
    {
        ErrorLn("Start loading Test Server failure");
        return;
    }
#endif

	// 创建战场管理
	TraceLn("Start loading WarManager...");
	m_pWarManager = CreateWarManagerProc();
	if ( m_pWarManager ==0)
	{
		ErrorLn("WarManager load failure!");
		return;
	}

	TraceLn("Start loading Task Server...");
	m_pTaskServer = CreateTaskServerProc();
	if ( m_pTaskServer == 0 )
	{
		ErrorLn("Start loading Task Server failure");
		return;
	}

    TraceLn("Start loading OssLog Server...");
    m_pOssLogServer = CreateOSSLogServerProc();
    if ( m_pOssLogServer ==0 )
    {
        ErrorLn("OssLog Server load failure!");
        return;
    }

	TraceLn("Start loading AntiAddict Server...");
	m_pAntiAddictServer = CreateAntiAddictServerProc();
	if ( m_pAntiAddictServer ==0 )
	{
		ErrorLn("AntiAddict Server load failure!");
		return;
	}

	TraceLn("Start loading MailScene...");
	m_pMailScene = CreateMailSceneProc();
	if ( m_pMailScene ==0 || !m_pMailScene->load())
	{
		ErrorLn("MailScene load failure!");
		return;
	}

	// 最后加载，先加载就创建怪物了，有些部件还没创建好
	TraceLn("Start loading SceneManager...");
	m_pSceneManager = CreateSceneManagerProc();
	if ( m_pSceneManager ==0 )
	{
		ErrorLn("SceneManager load failure!");
		return;
	}

    m_pLegendCupScene = CreateLegendCupSceneProc();
    if ( m_pLegendCupScene == 0 || !m_pLegendCupScene->load() )
    {
        ErrorLn("Start loading LegendCupScene Server failure");
        return;
    }

    TraceLn("Start loading DBHttpService...");
    m_pDBHttpService = CreateDBHttpServiceProc();
    if (m_pMailScene == 0)
    {
        ErrorLn("DBHttpService load failure!");
        return;
    }

	m_pActionLib = CreatePlayerActionRecordProc();
	if (m_pActionLib != 0)
	{
		m_pActionLib->load();
	}

    //// 创建角色管理器
    //TraceLn("Start create ActorManager...");
    //m_pActorMgr = CreateActorManagerProc();
    //if(m_pActorMgr == 0)
    //{
    //    ErrorLn("ActorManager load failure!");
    //    return;
    //}

	//////////////////////////////////////////////////////////////////////////
    // TODO 检测已创建的Service是否起好，如果OK-〉启动CenterConnecter-〉LoadScene（）
    gServerGlobal->getTimerService()->SetTimer(ETIMER_CHECK_BASESERVICE, CHECK_BASESERVICE_INTERVAL, CAST_TYPE(ITimerHandler*, this));
}



/// 获取框架接口
IFramework *   ServerGlobalService::getFramework()
{
	return gServerGlobal->getFramework();
}


/// 获取配置中心接口
ISchemeCenter * ServerGlobalService::getSchemeCenter()
{
	return m_pSchemeCenter;
}

/// 取得配置服务
ISchemeEngine * ServerGlobalService::getSchemeEngine()
{
	return m_pSchemeEngine;
}

ISceneManager * ServerGlobalService::getSceneManager()
{
	return m_pSceneManager;
}

IConditionFactory * ServerGlobalService::getConditionFactory()
{
	return m_pConditionFactory;
}

IEffectFactory * ServerGlobalService::getEffectFactory()
{
	return m_pEffectFactory;
}

ISpellFactory * ServerGlobalService::getSpellFactory()
{
	return m_pSpellFactory;
}

// 取得冷却服务器
IFreezeServer *ServerGlobalService::getFreezeServer()
{
	return m_pFreezeServer;
}

// 取得Buff服务
IBuffServer *ServerGlobalService::getBuffServer()
{
	return m_pBuffServer;
}

// 取得伤害服务器
IDamageServer *ServerGlobalService::getDamageServer()
{
	return m_pDamageServer;
}

ITankServer *ServerGlobalService::getTankServer()
{
	return m_pTankServer;
}

// 取得仇恨服务器
IEnmityServer *ServerGlobalService::getEnmityServer()
{
	return m_pEnmityServer;
}

// 取得测试服务器
ITestServer * ServerGlobalService::getTestServer()
{
    return m_pTestServer;
}

IPathFindManager * ServerGlobalService::getPathFindManager()
{
	return m_pPathFindManager;
}

IMatchScene* ServerGlobalService::getMatchScene()
{
	return m_pMatchScene;
}

IChatScene* ServerGlobalService::getChatScene()
{
	return m_pChatScene;
}

IWarManager * ServerGlobalService::getWarManager()
{
	return m_pWarManager;
}

IShareReceiver*	ServerGlobalService::getShareReceiver()
{
	return m_pShareReceiver;
}
IShareSceneSvrSource* ServerGlobalService::getShareSceneSvrSource()
{
	return m_pShareSceneSvrSource;
}

IOrderScene* ServerGlobalService::getOrderScene()
{
    return m_pOrderScene;
}

IStaticWarScene* ServerGlobalService::getStaticWarScene()
{
    return m_pStaticWarScene;
}

ILegendCupScene* ServerGlobalService::getLegendCupScene()
{
    return m_pLegendCupScene;
}

IActorMgr* ServerGlobalService::getActorMgr()
{
    return m_pActorMgr;
}

IClanScene* ServerGlobalService::getClanScene()
{
    return m_pClanScene;
}

IKinScene* ServerGlobalService::getKinScene()
{
    return m_pKinScene;
}

IAIFactory* ServerGlobalService::getAIFactory()
{
    if(m_pAIFactoryWrapper == NULL)
    {
        return NULL;
    }
    return m_pAIFactoryWrapper->get();
}

ITeamScene* ServerGlobalService::getTeamScene()
{
    return m_pTeamScene;
}

IAntiAddictServer* ServerGlobalService::getAntiAddictServer()
{
	return m_pAntiAddictServer;
}

// 取资源管理对象
IResourceManager* ServerGlobalService::getResourceManager()
{
    return m_pResourceManager;
}

// 取杂项管理对象
IMiscManager* ServerGlobalService::getMiscManager()
{
    return m_pMiscManager;
}

IMentorshipScene* ServerGlobalService::getMentorshipScene()
{
	return m_pMentorshipScene;
}

// 取得邮件场景服
IMailScene* ServerGlobalService::getMailScene()
{
	return m_pMailScene;
}

HeightFieldMgr* ServerGlobalService::getHeightFieldMgr()
{
    return m_pHeightFieldMgr;
}

//////////////////////////////////////////////////////////////////////////
// 请求跨Service处理服务控制，有些数据需要在Service上才能处理
void ServerGlobalService::requestHandleServerControl(DWORD dwState)
{
    gServerGlobal->getServerControllor()->OnDo(dwState);
}

// 请求管理器启动
bool ServerGlobalService::requestManagerStart(DWORD dwStartFlag, DWORD dwUserData)
{
    IManagerConnectorService * pManagerConnectorService =  gServerGlobal->getManagerConnectorService();
    if(pManagerConnectorService && pManagerConnectorService->RequestStart(dwStartFlag, dwUserData))
    {
        return true;
    }

    m_dwRetryStartFlag = dwStartFlag;
    m_dwRetryUserData = dwUserData;
    gServerGlobal->getTimerService()->SetTimer(ETIMER_MANAGER_RETRYSTART, CHECK_RETRYSTART_INTERVAL, CAST_TYPE(ITimerHandler*, this), 1);

    return false;
}
///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void ServerGlobalService::handleServerListUpdated()
{
    getMessageDispatch()->dispatchServerListUpdated();
}

/**
@name         : 通知服务器信息更新
@brief        : 例如服务器人数变化等等
*/
void ServerGlobalService::handleServerInfoUpdated(DWORD ServerID, BYTE nState, void* pInfo, size_t nInfolen)
{
    getMessageDispatch()->dispatchServerInfoUpdated(ServerID, nState, pInfo);
}

/**
@name         : 处理其他服务器通过中心服务器转发的消息
@brief        : 具体内容不清楚
@param server : 该消息是哪个服务器发过来的
@param pData  : 消息内容
@param wDataLen:消息长度
*/
void ServerGlobalService::handlePostMessage(DWORD server,const char * pData,WORD wDataLen)
{
    getMessageDispatch()->dispatchTransmit(server, (void*)pData, wDataLen);
}

///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
// 网关服务器通知游戏服务器有一个新的客户端连进来
void ServerGlobalService::handleClientIncoming(ClientID client)
{

}

// 网关服务器通知游戏服务器有新的数据包到达
void ServerGlobalService::handleGateWayReceivedData(ClientID client, void* data, size_t len) 
{
    if ( data==0 || len<sizeof(SNetMsgHead) )
        return;

    SNetMsgHead * pHead = (SNetMsgHead *)data;
    if (pHead->bySrcEndPoint == MSG_ENDPOINT_CLIENT)
    {
        getMessageDispatch()->dispatchMessage(client, data, len);
    }
    else
    {
        ErrorLn(__FUNCTION__": A Message Not Handle, Module="<<pHead->byKeyModule<<",action="<<pHead->byKeyAction);
    }
}

// 网关服务器通知游戏服务器有连接断开
void ServerGlobalService::handleGateWayClientDisconnect(ClientID client,DWORD dwReason)
{

}


///////ManagerConnector///////////////////////////////////////////////////////////////////
// 收到设置游戏ID
void ServerGlobalService::handleManagerSetGameWorldID()
{
    static bool bSetWorldID = false;
    if(bSetWorldID)
    {
        return;
    }
    bSetWorldID = true;

    DWORD dwWorldID = getThisGameWorldID();
    WarningLn(__FUNCTION__": worldid="<< dwWorldID);

    if( dwWorldID > 0 )
    {
        IServerController* pController = gServerGlobal->getServerControllor();
        if(pController)
        {
            pController->GotoState(SERVER_RUN_STATE_GET_AUTH_RESOURCE);
        }
    }
}

// 收到管理器下发的资源数据
void ServerGlobalService::handleManagerResourceData(LPVOID pData,DWORD dwDataLen)
{
    if(m_pResourceManager)
    {
        m_pResourceManager->onRecvMessage((const char*)pData, dwDataLen);
    }
}

// 收到管理器下发的资源数据结果
void ServerGlobalService::handleManagerResourceResult(LPVOID pData,DWORD dwDataLen)
{
    if(dwDataLen != sizeof(SMsgManagerRequestStartResult_M) )
    {
        return;
    }
    SMsgManagerRequestStartResult_M * pMsg = (SMsgManagerRequestStartResult_M *)(pData);
    //EMRequestStartResultCode
    WarningLn(_GT("请求启动，结果代码=")<<pMsg->dwResultFlag<<",StartFlag="<<pMsg->dwStartFlag<<",UserData="<<pMsg->dwUserData<<",Result="<<pMsg->szResultMsg);

    DWORD dwStartFlag = pMsg->dwStartFlag;
    DWORD dwResultFlag = pMsg->dwResultFlag;
    DWORD dwUserData = pMsg->dwUserData;

    bool canNextStep = false;
    if (pMsg->bResult==false)
    {
        WarningLn(_GT("请求启动失败，结果代码=")<<pMsg->dwResultFlag<<",StartFlag="<<pMsg->dwStartFlag<<", UserData="<<pMsg->dwUserData<<", Result="<<pMsg->szResultMsg);

        // 正在加载脚本数据,要脚本的可定时重试请求启动,直到取得
        if (dwResultFlag==MANAGER_STARTCODE_LOADSCRIPT)
        {
            m_dwRetryStartFlag = pMsg->dwStartFlag;
            m_dwRetryUserData = pMsg->dwUserData;
            gServerGlobal->getTimerService()->SetTimer(ETIMER_MANAGER_RETRYSTART, CHECK_RETRYSTART_INTERVAL, CAST_TYPE(ITimerHandler*, this), 1);
        }
        
        if((dwStartFlag & MANAGER_STARTFLAG_GETSCRIPT) && dwResultFlag==MANAGER_STARTCODE_NOSCRIPT)
        {
            canNextStep = true;
        }
    }
    else
    {
        if(dwStartFlag & MANAGER_STARTFLAG_GETSCRIPT)
        {
            TraceLn(__FUNCTION__": continue next step");
            canNextStep = true;
            // 设置从PACK中读取LUA
            if(m_pResourceManager)
            {
                m_pResourceManager->setFromPackLoadLua(true);
            }
        }
        WarningLn(__FUNCTION__":StartFlag="<<pMsg->dwStartFlag<<",UserData="<<pMsg->dwUserData);
    }

    if(canNextStep)
    {
        // 启动中心服
        if(m_pMiscManager)
        {
            m_pMiscManager->create_service("MigrateService");
        }
        else
        {
            WarningLn(__FUNCTION__": MiscManager==NULL");
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 创建共享
bool ServerGlobalService::createShareService(void)
{
	ICenterConnectorService*  pCenterConnectorService = gServerGlobal->getCenterConnectorService();
	if(!pCenterConnectorService)
	{
		ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL!");
		return false;
	}
	IEventEngine* pEventEngine = gServerGlobal->getEventEngine();

	// 共享数据接收器
	TraceLn("Start Create Share Receiver ...");
	m_pShareReceiver = CreateShareReceiverProc(pCenterConnectorService, pEventEngine);
	if(m_pShareReceiver == NULL)
	{
		ErrorLn( __FUNCTION__ ":m_pShareReceiver == NULL, Create Share Receiver Failed!... ...");
		return false;
	}

	// 共享数据发生源
	TraceLn("Start Create  Share SceneSvrSource ...");
	m_pShareSceneSvrSource = CreateShareSceneSvrSourceProc();
	if(m_pShareSceneSvrSource == NULL)
	{
		ErrorLn( __FUNCTION__ ":m_pShareSceneSvrSource == NULL, Create Share SceneSvrSource Failed!.. ...");
		return false;
	}

	return true;
}


////////////////// IEventExecuteSink ////////////////////////////////////////////////////////
/** 
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  
@note     
@warning 
@retval buffer 
*/
void ServerGlobalService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(MAKEWORD(bSrcType, wEventID))
    {
    case MAKEWORD(SOURCE_TYPE_SYSTEM, EVENT_SYSTEM_DBENGINE_STARTED): // DB引擎服务已启动
        {   
            if(m_pMiscManager->registerDBEngineService())
            {
                // 对游戏ID进行校验
                m_pMiscManager->checkWorldID();
            }
        }
        break;
    case MAKEWORD(SOURCE_TYPE_SYSTEM, EVENT_SYSTEM_MIGRATE_STARTED):  // 迁移服务已启动
        {
            // 启动中心服务
            startCenterService();
            createService("VoiceConnector");
        }
        break;
    case MAKEWORD(SOURCE_TYPE_SYSTEM, EVENT_SYSTEM_CENTERCONNECTOR_STARTED): // 中心连接器服务已启动
        {
            // 启动网关服务
            startGateWayService();
        }
        break;
    case MAKEWORD(SOURCE_TYPE_SYSTEM, EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED): // 网关连接器服务已启动
        {
            // 启动桥服务
            startBridgeService();
        }
        break;
    case MAKEWORD(SOURCE_TYPE_SYSTEM, EVENT_SYSTEM_BRIDGECONNECTOR_STARTED): // 桥连接器服务已启动
        {
			createShareService();
            startEntries(); 

            // TODO 创建角色管理器  现在临时放到这个地方，后面找到具体原因再来调整
            TraceLn("Start create ActorManager...");
            m_pActorMgr = CreateActorManagerProc();
            if(m_pActorMgr == 0)
            {
                ErrorLn("ActorManager load failure!");
                return;
            }
        }
        break;
    case MAKEWORD(SOURCE_TYPE_SYSTEM, EVENT_SYSTEM_CENTER_HANDSHAKE_OK):
		{
			// 开始加载场景资源
			if (m_pSceneManager)
			{
				m_pSceneManager->load();
			}

			// 服务器启动完成
			rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
			if (pEventEngine)
			{
				pEventEngine->FireExecute(EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0, "", 0);
			}
			// 启动完成
			gServerGlobal->getServerControllor()->GotoState(SERVER_RUN_STATE_START_SUCCESS);
		}
		break;
    case MAKEWORD(SOURCE_TYPE_SCENE, EVENT_SCENE_CREATED):
    case MAKEWORD(SOURCE_TYPE_SCENE, EVENT_SCENE_DESTROY):
        {
            makeServerInfo();
            updateZoneServerInfo2Center();
        }
        break;
    case MAKEWORD(SOURCE_TYPE_PERSON, EVENT_GAME_ACTOR_LOGIN):
        {
            if(nLen < sizeof(event_actor_login))
            {
                return;
            }
            event_actor_login* pEvent = (event_actor_login*)pszContext;
			ELoginMode eMode = pEvent->eMode;

			if (eMode == elogin_online || eMode == elogin_cross_process)
				m_nCurPlayerNum++;

            if(eMode == elogin_online || eMode == elogin_cross_process || eMode == elogin_in_process)
            {
                updateZoneServerInfo2Center(pEvent->nSceneID, MAP_UPDATEPLAYER_ADD);
            }
        }
        break;
    case MAKEWORD(SOURCE_TYPE_PERSON, EVENT_GAME_ACTOR_LOGOUT):
        {
            if(nLen < sizeof(event_actor_logout))
            {
                return;
            }

            event_actor_logout* pEvent = (event_actor_logout*)pszContext;
			ELogoutMode eMode = pEvent->eMode;

			if(eMode == elogout_offline || eMode == elogout_cross_process)
				if (m_nCurPlayerNum > 0) --m_nCurPlayerNum;

            if(eMode == elogout_offline || eMode == elogout_cross_process || eMode == elogout_in_process)
            {
                updateZoneServerInfo2Center(pEvent->nSceneID, MAP_UPDATEPLAYER_REMOVE);
            }
        }
        break;
	case MAKEWORD(SOURCE_TYPE_SCENE, EVENT_SCENE_PRE_ENTER_SCENE_UPDATE):
		{
			if (nLen != sizeof(event_scene_pre_enter_scene))
				return;

			event_scene_pre_enter_scene* pEvent = (event_scene_pre_enter_scene*)pszContext;

			updateZoneServerInfo2Center(pEvent->nSceneID, MAP_UPDATEPLAYER_PREENTER_UPDATE);
		}
		break;
    default:
        {
            ErrorLn(__FUNCTION__ << ", event unhandled=" << wEventID);
        }
        break;
    }
}

////////////////// TimerHandler ////////////////////////////////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void ServerGlobalService::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case ETIMER_CHECK_BASESERVICE:
        {
            onTimerCheckBaseService();
        }
        break;

    case ETIMER_MANAGER_RETRYSTART:
        {
            if( requestManagerStart(m_dwRetryStartFlag, m_dwRetryUserData) )
            {
                m_dwRetryStartFlag = 0; 
                m_dwRetryUserData = 0;
            }
        }
        break;
		
    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
void ServerGlobalService::makeServerInfo()
{
    if(m_pServerInfo != NULL)
    {
        free(m_pServerInfo);
        m_pServerInfo = 0;
    }

    Ini ini("config.ini");
    // 场景可登陆最大人数控制
    int nMaxUserCount = ini.getInt("setting","zoneMaxUserCount");

    ISceneManager* pSceneMgr = gServerGlobal->getSceneManager(); 
    ServerInfo_Zone info;
    info.dwZoneServerID     = gServerGlobal->getCenterConnectorService()->getLocalServerCSID();
    info.dwUserCount        = m_nCurPlayerNum;  // 本场景服玩家数量
    info.dwMaxUserCount     = nMaxUserCount == 0 ? 2000: nMaxUserCount;   // 本场景服最大承载人数(读配置) 
    info.wZoneServerIndex   = m_nServerID;    // 场景序号
    
    // 场景服线程模式标识
    info.dwFlags = (getFramework()->isMulitThreadMode() ? 0x1 : 0x0);

    int arrayList[4096];
    int nSceneCount = pSceneMgr->getAllSceneID(arrayList, sizeof(arrayList) / sizeof(int)); 

    info.wMapNum = 0;
    info.wStaticMapNum = 0;
    obuf obMapArray;
    for(int i = 0; i < nSceneCount; ++i)
    {
        int nSceneID = arrayList[i];

        // 场景逻辑没启动好的场景 暂时不能开放 要等启动好了同步
        SceneLogicHelper sceneLogic(nSceneID);
        if(sceneLogic.m_ptr == 0)
        {
            continue;
        }

        ServerInfo_Zone::MapZone mapzone;
        mapzone.dwZoneID = nSceneID;
        mapzone.dwMapID = pSceneMgr->sceneIDToMapID(nSceneID);
        mapzone.nSerialNo = pSceneMgr->sceneIDToSerialNo(nSceneID);
        mapzone.dwMapPlayerCount = 0;
        SERVICE_PTR pGameSceneService = pSceneMgr->getScene(nSceneID);
        if (pGameSceneService != 0)
        {
            IGameSceneService * pScene = (IGameSceneService*)pGameSceneService->query_interface();
            if (pScene != 0)
            {
                mapzone.dwMapPlayerCount = pScene->getPlayerCount();
            }
        }

        if(!IS_DYNAMIC_SCENE(nSceneID))
        {
            ++info.wStaticMapNum;
        }
        obMapArray << mapzone;

        ++info.wMapNum;
    }

    m_pServerInfo = malloc(sizeof(ServerInfo_Zone) + obMapArray.size());

    memcpy(m_pServerInfo, &info, sizeof(ServerInfo_Zone));
    memcpy((ServerInfo_Zone*)m_pServerInfo + 1, obMapArray.data(), obMapArray.size());
}

// 场景服信息更新到中心服
void ServerGlobalService::updateZoneServerInfo2Center(int nSceneID, EMapUpdatePlayer eUpdatePlayer)
{
    ISceneManager* pSceneMgr = gServerGlobal->getSceneManager();
    if (pSceneMgr == NULL)
    {
        return;
    }

    if(m_pServerInfo == NULL)
    {
        makeServerInfo();
    }

    ServerInfo_Zone* pInfo = (ServerInfo_Zone*)m_pServerInfo;
    pInfo->dwUserCount = m_nCurPlayerNum; 

    // 更新地图人数
    if (nSceneID > 0)
    {
		int nMapPlayeCount = 0;
		if (eUpdatePlayer == MAP_UPDATEPLAYER_ADD || eUpdatePlayer == MAP_UPDATEPLAYER_REMOVE)
		{
			SERVICE_PTR pGameSceneService = pSceneMgr->getScene(nSceneID);
			if (pGameSceneService != 0)
			{
				IGameSceneService * pScene = (IGameSceneService*)pGameSceneService->query_interface();
				if (pScene != 0)
				{
					nMapPlayeCount = pScene->getPlayerCount();
				}
			}
		}

        if (pInfo->wMapNum >0)
        {
            ServerInfo_Zone::MapZone *pZoneMapInfo = (ServerInfo_Zone::MapZone *)(pInfo + 1);
            for (int n = 0; n < pInfo->wMapNum; ++n)
            {
                if (pZoneMapInfo->dwZoneID == nSceneID)
                {
                    if (eUpdatePlayer == MAP_UPDATEPLAYER_ADD)
                    {
                        pZoneMapInfo->dwMapPlayerCount = ++nMapPlayeCount;
                    }

                    if ( eUpdatePlayer == MAP_UPDATEPLAYER_REMOVE)
                    {
                        pZoneMapInfo->dwMapPlayerCount = --nMapPlayeCount;
                    }

					if (eUpdatePlayer == MAP_UPDATEPLAYER_PREENTER_UPDATE)
					{
						IMigrateService* pMigrate = gServerGlobal->getMigrateService();
						if (pMigrate)
						{
							DWORD nCount = pMigrate->getLocalPreEnterPlayerCount(nSceneID);
							pZoneMapInfo->dwPreEnterPlayerCount = nCount;
							WarningLn(__FUNCTION__":switchscene,PreEnterPlayerCount=" << pZoneMapInfo->dwPreEnterPlayerCount);
						}
					}

                    break;
                }

                ++pZoneMapInfo;
            }
        }
    }
    

    obuf obResult;
    obResult.push_back(m_pServerInfo, sizeof(ServerInfo_Zone) + pInfo->wMapNum * sizeof(ServerInfo_Zone::MapZone));

    ibuffer in( obResult.data(), obResult.size() );
    gServerGlobal->getCenterConnectorService()->updateServerInfo( in );
}

// 定时检测基础服务是否已启好
void ServerGlobalService::onTimerCheckBaseService(void)
{
    IServiceMgr * pServiceMgr = getFramework()->get_service_manager();
    if(pServiceMgr == NULL)
    {
        return;
    }

    SERVICE_ID arrServiceID[ARRAY_LENGTH] = {0};
    int nCount = pServiceMgr->get_all_service_ids(arrServiceID, ARRAY_LENGTH);
    if(nCount == 0)
    {
        return;
    }

    // 检测基础的服务是否都已启动好
    bool bBaseServicesStartOk = true;
    for (int i=0; i<nCount; ++i)
    {
        SERVICE_PTR pService = pServiceMgr->get_service(arrServiceID[i]);
        if(pService==NULL || pService->get_status() != SERVICE_STATUS_WORKING)
        {
            bBaseServicesStartOk = false;
            break;
        }
    }
    if(!bBaseServicesStartOk)
    {
        return;
    }

    // 杀掉定时器
    gServerGlobal->getTimerService()->KillTimer(ETIMER_CHECK_BASESERVICE, CAST_TYPE(ITimerHandler*, this));

    // 连接中心服
    if(!gServerGlobal->getCenterConnectorService()->connectServer())
    {
        return;
    }

    // 连接语音服
    if(!gServerGlobal->getVoiceConnectorService()->connectServer())
    {
        return;
    }


}


