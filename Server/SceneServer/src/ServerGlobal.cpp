/*******************************************************************
** 文件名:	ServerGlobal.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "DbgHelp.h"
#include "IStackWalk.h"

#include "ServerGlobal.h"
#include "ServerGlobalHelper.h"
#include "IActorMgr.h"
#include "ShareServerHelper.h"
#include "DialogPageStart.h"
#include "SceneServer.h"


// 全局服务器不要释放，这样可以缓存全局指针
#ifndef SUPORT_SERVICE_CACHE
#define SUPORT_SERVICE_CACHE
#endif

ServerGlobal::ServerGlobal() 
		: m_pFramework(0)
		,m_nServerID(0)
		,m_pCenterConnector(0)
        ,m_pVoiceConnector(0)
        ,m_pGuardConnector(0)
		,m_pGatewayAcceptor(0)
		,m_pBridgeConnector(0)
		,m_pManagerConnector(0)
		,m_pSceneManager(0)
		,m_pSchemeCenter(0)
		,m_pSchemeEngine(0)
		,m_pConditionFactory(0)
		,m_pEffectFactory(0)
		,m_pSpellFactory(0)
		,m_pFreezeServer(0)
		,m_pBuffServer(0)
		,m_pDamageServer(0)
		,m_pTankServer(0)
		,m_pEnmityServer(0)
        ,m_pTestServer(0)
		,m_pPathFindManager(0)
		,m_pMatchScene(0)
		,m_pWarManager(0)
		,m_pGameResService(0)
		,m_pEventEngine(0)
#ifdef SERVICE_HANDLE_MESSAGE
        ,m_pMessageDispatch(0)
#endif
		,m_ServerGlobalServiceID(INVALID_SERVICE_ID)
		,m_nState(STATE_STOPPED)
		,m_pMigrateService(0)
		,m_pDBEngineService(0)
		,m_pShareReceiver(0)
		,m_pShareSceneSvrSource(0)
        ,m_pActorMgr(0)
        ,m_pClanScene(0)
        ,m_pKinScene(0)
		,m_pChatScene(0)
        ,m_pAIFactory(0)
        ,m_pTeamScene(0)
        ,m_pOrderScene(0)
		,m_pOssLogService(0)
		,m_pAntiAddictServer(0)
        , m_pResourceManager(0)
        , m_pMiscManager(0)
		,m_pMentorshipScene(0)
        ,m_bAutoStart(true)          // 是否自动启动
        ,m_bOpenSupperStone(false)   // 是否开启七龙珠功能
        ,m_pLegendCupScene(0)
        ,m_pStaticWarScene(0)
		,m_pMailScene(0)
        , m_pHeightFieldMgr(0)
{
    // 工作目录
    memset(m_szWorkDir, 0, sizeof(m_szWorkDir));
}

ServerGlobal::~ServerGlobal()
{

}


/** 初始化启动环境
@param   
@param   
@return  
*/
bool ServerGlobal::InitEnvironment(void)
{
    // 取得本地路径
    int nDirLen = sizeof(m_szWorkDir);
    sstrcpyn(m_szWorkDir, rkt::getWorkDir(), nDirLen);	
	::SetCurrentDirectoryA( m_szWorkDir );

    // 设置搜索路径
    getFileSystem()->addFindPath(m_szWorkDir);

    string stringScpDir = m_szWorkDir;
    stringScpDir += "\\Scp";
    getFileSystem()->addFindPath(stringScpDir.c_str());

    string stringMapDir = m_szWorkDir;
    stringMapDir += "\\MapPath";
    getFileSystem()->addFindPath(stringMapDir.c_str());

    // 设置lua路径
    string stringLuaDir = m_szWorkDir;
    stringLuaDir += "\\Lua";
    getFileSystem()->addFindPath(stringLuaDir.c_str());


    Ini ini("config.ini");
    // 自动启动
    m_bAutoStart = ini.getInt("setting","autostart",0);

    // 测试配置
    Ini ini_test("test.ini");
    // 是否能使用七龙珠
    m_bOpenSupperStone = ini_test.getInt("setting","supperstone",0);
    if(m_bOpenSupperStone)
    {
        EmphasisLn("Current Scene open SupperStone!!!");
    }

    // 初始化通讯层
    if ( InitializeNetwork()<0 )
    {
        ErrorLn("The network layer initialization failed！");
        return false;
    }

    return true;
}

/** 正式启动本服务器
@name				: 启动按照以下流程来执行: 
					1. 初始化网络环境
					2. 初始化框架，并启动基础服务
					3. 启动具体服务
					4. 启动子模块
@param				: 
@return
@NOTICE				1和4由本线程(Windows主线程)启动，2和3由各自工作线程启动
*/
bool ServerGlobal::Start(unsigned short nServerID )
{
	setGlobal((IServerGlobal*)this);
    EmphasisLn("Current scene server index is "<< nServerID);
	m_nServerID = nServerID;

   // 校验本场景服是否用单线程模式开
    bool isMulitThreadMode = true;
    char szBuf[32] = {0};
    ssprintf_s(szBuf, sizeof(szBuf),"%d,",nServerID); 
    Ini ini("config.ini");
    // ;单线程场景服ID列表，用逗号隔开
    string strSingleThreadList = ini.getString("setting","singlethreadserverlist");
    if(!strSingleThreadList.empty() && strSingleThreadList.find(szBuf) != string::npos)
    {
        isMulitThreadMode = false;
    }

	m_nState = STATE_START;

    // 设定Trace相关
    SetTrace();

	return Initialize(isMulitThreadMode);
}

/** 正式终止本服务器的运行
@name				: 按照以下流程来执行: 
					1. 通告所有服务停止运行
					2. 定时轮询发现所有服务是否都已停止
					3. 释放各子模块（这里有个问题是子模块的容器对象是在其它线程创建，但可能最后由主线程来释放）
					4. 释放框架
@param				: 
@return
@NOTICE				1和4由本线程(Windows主线程)启动，2和3由各自工作线程启动
*/
void ServerGlobal::stop()
{
    TraceLn("-----------------"<<__FUNCTION__<<"-----------------");
	if (m_nState == STATE_START || m_nState == STATE_RUNNING)
	{
		m_nState = STATE_STOPPING;
        gServerGlobal->getFramework()->get_service_manager()->stop_all();
        UnInitialize();
	}
}


void ServerGlobal::OnTimer()
{
	// 当前状态为正在停止中~~则轮询服务状态
	if (m_nState != STATE_STOPPING)
	{
        return;
    }

    IFramework * pFramework = GetFrameworkProc();
    if(pFramework)
    {
        if(getCenterConnectorService(true)) return;
        if(getVoiceConnectorService(true)) return;
        if(getGuardConnectorService(true)) return;
        if(getGatewayAcceptorService(true)) return;
        if(getBridgeConnectorService(true)) return;
        if(getManagerConnectorService(true)) return;
		
        IServiceMgr * pSvMgr = pFramework->get_service_manager();
        if(pSvMgr)
        {
            if (pSvMgr->empty())
            {
                TraceLn("Release Framework...");
                // 最后释放框架
                safeRelease(m_pFramework);

                TraceLn("Server is shut down!");
                m_nState = STATE_STOPPED;

                // 关闭网络
                UninitializeNetwork();

                // 关闭窗口
                theApp.PostMessage(WM_CTR_CLOSEDLG, 0, 0);
            }
        }
    }
    else
    {
        // 关闭窗口
        theApp.PostMessage(WM_CTR_CLOSEDLG, 0, 0);
    }
}

//////////////////////////////////////////////////////////////////////////
// 内部初始化接口
bool ServerGlobal::Initialize(bool isMulitThreadMode)
{
	TraceLn("Start loading Framework...");
	m_pFramework = GetFrameworkProc();
	if ( m_pFramework==0 )
	{
		ErrorLn("Framework load failure! Please check Framework.dll exists");
		return false;
	}

	TraceLn("Start loading scene_services.xml...");
	char szPath[MAX_PATH];
	sprintf(szPath,"%s\\scene_services.xml",getWorkDir());
	if ( !m_pFramework->start(szPath, isMulitThreadMode) )
	{
		ErrorLn("scene_services.xml load failure!");
		return false;
	}
	
	EmphasisLn("Framework start success!");

	return true;

}

void ServerGlobal::UnInitialize()
{
	TraceLn("Start UnInitialize ...");


	TraceLn("Start release EventEngine...");
	SAFERELEASE(m_pEventEngine);

	//// 共享数据接收器
	//TraceLn("Start release ShareReceiver...");
	//SAFE_RELEASE(m_pShareReceiver);

	//// 共享数据发生源
	//TraceLn("Start release ShareZoneSvrSource...");
	//SAFE_RELEASE(m_pShareSceneSvrSource);

#ifdef SERVICE_HANDLE_MESSAGE
    m_pMessageDispatch = NULL;
#endif
	m_pDBEngineService = NULL;
	m_pMigrateService = NULL;
	m_pGameResService = NULL;
	//TraceLn("Start release MatchManager...");
	//SAFERELEASE(m_pMatchManager);

	m_pManagerConnector = NULL;
	m_pBridgeConnector = NULL;
	m_pGatewayAcceptor = NULL;
    m_pGuardConnector = NULL;
	m_pCenterConnector = NULL;

	m_pWarManager = NULL;

	// 释放子模块的顺序最好和初始化顺序相反
	m_pSceneManager = NULL;
	m_pTestServer = NULL;
	m_pEnmityServer = NULL;
	m_pTankServer = NULL;
	m_pDamageServer = NULL;
	m_pBuffServer = NULL;
	m_pFreezeServer = NULL;
	m_pSpellFactory = NULL;
	m_pEffectFactory = NULL;
	m_pConditionFactory = NULL;
	m_pPathFindManager = NULL;
	m_pMatchScene = NULL;
	m_pSchemeCenter = NULL;
	m_pSchemeEngine = NULL;
    m_pClanScene = NULL;
    m_pKinScene = NULL;
	m_pChatScene = NULL;
    m_pTeamScene = NULL;
    m_pOrderScene = NULL;
	m_pAntiAddictServer = NULL;
    m_pResourceManager = NULL;
    m_pMiscManager = NULL;
    m_pLegendCupScene=NULL;
	m_pMentorshipScene = NULL;
    m_pStaticWarScene=NULL;
	m_pMailScene = NULL;
}

// 设定Trace相关
bool ServerGlobal::SetTrace()
{
    // 是否有log目录
    char szLogAbsolutePath[512] = { 0 };
    sprintf_s(szLogAbsolutePath, "%s\\Log\\Log%d\\Scene", m_szWorkDir, m_nServerID);
    bool isAbsolutePath = true; uint attrib = 0;
    if(!rkt::checkPath(szLogAbsolutePath, isAbsolutePath, attrib))
    {
        rkt::createDirectoryRecursive(szLogAbsolutePath);
    }

    // trace文件地址	
    time_t	long_time; struct tm * local_time;
    time(&long_time); local_time = localtime(&long_time);

    char szTraceFileName[512] = { 0 };
    sprintf_s(szTraceFileName, "%s\\Scene%02d%02d%02d%02d%02d_Trace.html", szLogAbsolutePath,
        local_time->tm_mon + 1,
        local_time->tm_mday,
        local_time->tm_hour,
        local_time->tm_min,
        local_time->tm_sec);
    char szCrashFileName[512] = { 0 };
    sprintf_s(szCrashFileName, "%s\\Scene%02d%02d%02d%02d%02d_Crash.txt", szLogAbsolutePath,
        local_time->tm_mon + 1,
        local_time->tm_mday,
        local_time->tm_hour,
        local_time->tm_min,
        local_time->tm_sec);

    char szDumpFileName[512] = { 0 };
    sprintf_s(szDumpFileName, "%s\\Scene%02d%02d%02d%02d%02d_Stack.dmp", szLogAbsolutePath,
        local_time->tm_mon + 1,
        local_time->tm_mday,
        local_time->tm_hour,
        local_time->tm_min,
        local_time->tm_sec);
    char szZoneSvrCrash[512] = { 0 };
    sprintf_s(szZoneSvrCrash, "%s\\SceneServerDownRecord.html", szLogAbsolutePath);

    TraceListener * pTraceListener = output2File(szTraceFileName);
    if(pTraceListener != NULL)
    {
        pTraceListener->setHasTime(true);
    }
    g_pDialogPageStart->m_stringTraceFile = szTraceFileName;

    // 场景服务器非法日志记录处
    crash_output2File(szZoneSvrCrash);	
    // 修改trace等级
    setTraceLevel(levelAll);

#ifdef SUPPORT_STACKWALK
    // dump
    IStackWalk * pStackWalk = createStackWalkProc();
    if(pStackWalk == NULL)
    {
        return false;
    }	

    Ini ini("config.ini");
    // dump类型
    long lMiniDumpType = ini.getInt("App", "MiniDumpType",
        MiniDumpWithPrivateReadWriteMemory |
        MiniDumpWithDataSegs |
        MiniDumpWithHandleData |
        MiniDumpWithUnloadedModules);
    pStackWalk->setDumpType(lMiniDumpType);

    // dump文件
    pStackWalk->setLogFileName(my_utf82t(szCrashFileName));
    pStackWalk->setDumpFileName(my_utf82t(szDumpFileName));
#endif

    return true;
}

//////////////////////////////////////////////////////////////////////////
/** 取得场景服的工作目录
@param
@param
@return
*/
const char* ServerGlobal::getWorkDir(void)
{
    return m_szWorkDir;
}

// 是否开启自动运行场景服
bool ServerGlobal::isAutoStart(void)
{
    return m_bAutoStart;
}

// 是否开启七龙珠功能
bool ServerGlobal::isOpenSupperStone(void)
{
    return m_bOpenSupperStone;
}

/// 获取框架接口
IFramework *   ServerGlobal::getFramework()
{
	return m_pFramework;
}

/// 获得事件中心接口
IEventEngine *  ServerGlobal::getEventEngine()
{
	if (!m_pEventEngine)
	{
		m_pEventEngine = m_pFramework->create_event_engine();
	}
	return m_pEventEngine;
}

/// 获得对时服务接口
ITimeSyncService *  ServerGlobal::getTimeSyncService()
{
	return m_pFramework->get_service_timesync();
}

/// 获取时间轴接口
ITimerService * ServerGlobal::getTimerService()
{
	return m_pFramework->get_service_timer();
}

/// 获取配置中心接口
ISchemeCenter * ServerGlobal::getSchemeCenter()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pSchemeCenter )
		return m_pSchemeCenter;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pSchemeCenter = ptr->getSchemeCenter();
	}
	return m_pSchemeCenter;
}

// 取得服务器ID
unsigned short ServerGlobal::getServerID()
{
    return m_nServerID;
}

/// 获取中心服连接器接口
ICenterConnectorService * ServerGlobal::getCenterConnectorService( bool ignoreCache/*=false*/ )
{
    if(!ignoreCache)
    {
#ifdef SUPORT_SERVICE_CACHE
        if ( m_pCenterConnector )
            return m_pCenterConnector;
#endif
    }

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("CenterConnector");
	if ( pContainer==0 )
		return 0;

	m_pCenterConnector = (ICenterConnectorService*)pContainer->query_interface();
	return m_pCenterConnector;
}

// 获取语音服连接器
IVoiceConnectorService* ServerGlobal::getVoiceConnectorService( bool ignoreCache/*=false*/ )
{
    if(!ignoreCache)
    {
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pVoiceConnector )
        return m_pVoiceConnector;
#endif
    }

    SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("VoiceConnector");
    if ( pContainer==0 )
        return 0;

    m_pVoiceConnector = (IVoiceConnectorService*)pContainer->query_interface();
    return m_pVoiceConnector;
}

// 获取数据监护连接器接口
IGuardConnectorService * ServerGlobal::getGuardConnectorService( bool ignoreCache/*=false*/ )
{
    if(!ignoreCache)
    {
#ifdef SUPORT_SERVICE_CACHE
    if (m_pGuardConnector)
        return m_pGuardConnector;
#endif
    }

    SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("GuardConnector");
    if (pContainer == 0)
        return 0;

    m_pGuardConnector = (IGuardConnectorService*)pContainer->query_interface();
    return m_pGuardConnector;
}

/// 获取网关连接器接口
IGatewayAcceptorService * ServerGlobal::getGatewayAcceptorService( bool ignoreCache/*=false*/ )
{
    if(!ignoreCache)
    {
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pGatewayAcceptor )
		return m_pGatewayAcceptor;
#endif
    }

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("GatewayConnector");
	if ( pContainer==0 )
		return 0;

	m_pGatewayAcceptor = (IGatewayAcceptorService*)pContainer->query_interface();
	return m_pGatewayAcceptor;
}

// 获取桥连接器接口
IBridgeConnectorService * ServerGlobal::getBridgeConnectorService( bool ignoreCache/*=false*/ )
{
    if(!ignoreCache)
    {
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pBridgeConnector )
		return m_pBridgeConnector;
#endif
    }

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("BridgeConnector");
	if ( pContainer==0 )
		return 0;

	m_pBridgeConnector = (IBridgeConnectorService*)pContainer->query_interface();
	return m_pBridgeConnector;
}

// 获取管理器连接器接口
IManagerConnectorService * ServerGlobal::getManagerConnectorService( bool ignoreCache/*=false*/ )
{
    if(!ignoreCache)
    {
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pManagerConnector )
		return m_pManagerConnector;
#endif
    }

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("ManagerConnector");
	if ( pContainer==0 )
		return 0;

	m_pManagerConnector = (IManagerConnectorService*)pContainer->query_interface();
	return m_pManagerConnector;
}


/// 取得配置服务
ISchemeEngine * ServerGlobal::getSchemeEngine()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pSchemeEngine )
		return m_pSchemeEngine;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pSchemeEngine = ptr->getSchemeEngine();
	}
	return m_pSchemeEngine;
}

ISceneManager * ServerGlobal::getSceneManager()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pSceneManager )
		return m_pSceneManager;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pSceneManager = ptr->getSceneManager();
	}
	return m_pSceneManager;
}

IConditionFactory * ServerGlobal::getConditionFactory()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pConditionFactory )
		return m_pConditionFactory;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pConditionFactory = ptr->getConditionFactory();
	}
	return m_pConditionFactory;
}

IEffectFactory * ServerGlobal::getEffectFactory()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pEffectFactory )
		return m_pEffectFactory;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pEffectFactory = ptr->getEffectFactory();
	}
	return m_pEffectFactory;
}

ISpellFactory * ServerGlobal::getSpellFactory()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pSpellFactory )
		return m_pSpellFactory;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pSpellFactory = ptr->getSpellFactory();
	}
	return m_pSpellFactory;
}

// 取得冷却服务器
IFreezeServer *ServerGlobal::getFreezeServer()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pFreezeServer )
		return m_pFreezeServer;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pFreezeServer = ptr->getFreezeServer();
	}
	return m_pFreezeServer;
}

// 取得Buff服务
IBuffServer *ServerGlobal::getBuffServer()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pBuffServer )
		return m_pBuffServer;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pBuffServer = ptr->getBuffServer();
	}
	return m_pBuffServer;
}

// 取得伤害服务器
IDamageServer *ServerGlobal::getDamageServer()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pDamageServer )
		return m_pDamageServer;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pDamageServer = ptr->getDamageServer();
	}
	return m_pDamageServer;
}

ITankServer *ServerGlobal::getTankServer()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pTankServer )
		return m_pTankServer;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pTankServer = ptr->getTankServer();
	}
	return m_pTankServer;
}

// 取得仇恨服务器
IEnmityServer *ServerGlobal::getEnmityServer()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pEnmityServer )
		return m_pEnmityServer;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pEnmityServer = ptr->getEnmityServer();
	}
	return m_pEnmityServer;
}

// 取得测试服务器
ITestServer *ServerGlobal::getTestServer()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pTestServer )
        return m_pTestServer;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pTestServer = ptr->getTestServer();
    }
    return m_pTestServer;
}

IPathFindManager * ServerGlobal::getPathFindManager()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pPathFindManager )
		return m_pPathFindManager;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pPathFindManager = ptr->getPathFindManager();
	}
	return m_pPathFindManager;
}

// 消息分发器
IMessageDispatch* ServerGlobal::getMessageDispatch()
{
#ifdef SERVICE_HANDLE_MESSAGE
#   ifdef SUPORT_SERVICE_CACHE
        if ( m_pMessageDispatch )
            return m_pMessageDispatch;
#   endif
    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pMessageDispatch = ptr->getMessageDispatch();
    }
    return m_pMessageDispatch;
#else
    return &m_MessageDispatch;
#endif
}

// 获取网络消息处理接口
INetMessageHandler*	ServerGlobal::getNetMessageHandler()
{
	return &m_NetMessageHandler;
}

IMatchScene* ServerGlobal::getMatchScene()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pMatchScene )
		return m_pMatchScene;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pMatchScene = ptr->getMatchScene();
	}
	return m_pMatchScene;
}

IChatScene* ServerGlobal::getChatScene()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pChatScene )
		return m_pChatScene;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pChatScene = ptr->getChatScene();
	}
	return m_pChatScene;
}

IClanScene* ServerGlobal::getClanScene()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pClanScene )
        return m_pClanScene;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pClanScene = ptr->getClanScene();
    }

    return m_pClanScene;
}

IKinScene* ServerGlobal::getKinScene()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pKinScene )
        return m_pKinScene;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pKinScene = ptr->getKinScene();
    }
    return m_pKinScene;
}

IAIFactory*  ServerGlobal::getAIFactory()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pAIFactory )
        return m_pAIFactory;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pAIFactory = ptr->getAIFactory();
    }
    return m_pAIFactory;
}

IWarManager * ServerGlobal::getWarManager()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pWarManager )
		return m_pWarManager;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pWarManager = ptr->getWarManager();
	}

	return m_pWarManager;
}
// 取得GameResService
IGameResService* ServerGlobal::getGameResService()
{
#ifdef SUPORT_SERVICE_CACHE
	if(m_pGameResService != NULL)
	{
		return m_pGameResService;
	}
#endif

	if(m_pFramework == 0)
	{
		return NULL;
	}

	SERVICE_PTR ptrService = (SERVICE_PTR)m_pFramework->get_service_manager()->find_service("GameResService");
	if(ptrService != 0)
	{
		m_pGameResService = (IGameResService*)ptrService->query_interface();
	}

	return m_pGameResService;
}



IMigrateService* ServerGlobal::getMigrateService()
{
#ifdef SUPORT_SERVICE_CACHE
    if(m_pMigrateService != NULL)
    {
        return m_pMigrateService;
    }
#endif

    if(m_pFramework == 0)
    {
        return NULL;
    }

    SERVICE_PTR ptrService = (SERVICE_PTR)m_pFramework->get_service_manager()->find_service("MigrateService");
    if(ptrService != 0)
    {
        m_pMigrateService = (IMigrateService*)ptrService->query_interface();
    }

    return m_pMigrateService;
}

// 取得DBEngine服务
IDBEngineService* ServerGlobal::getDBEngineService()
{
#ifdef SUPORT_SERVICE_CACHE
	if(m_pDBEngineService != NULL)
	{
		return m_pDBEngineService;
	}
#endif

	if(m_pFramework == 0)
	{
		return NULL;
	}

	SERVICE_PTR ptrService = (SERVICE_PTR)m_pFramework->get_service_manager()->find_service("DBEngineService");
	if(ptrService != 0)
	{
		m_pDBEngineService = (IDBEngineService*)ptrService->query_interface();
	}

	return m_pDBEngineService;
}

// 取得数据共享接收器
IShareReceiver * ServerGlobal::getShareReceiver(void)
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pShareReceiver )
		return m_pShareReceiver;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pShareReceiver = ptr->getShareReceiver();
	}
	return m_pShareReceiver;
}

// 取得场景服的数据共享源
IShareSceneSvrSource * ServerGlobal::getShareSceneSvrSource(void)
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pShareSceneSvrSource )
		return m_pShareSceneSvrSource;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pShareSceneSvrSource = ptr->getShareSceneSvrSource();
	}
	return m_pShareSceneSvrSource;
}

// 启动共享
bool ServerGlobal::startShareServer(void)
{
    return g_ShareServerHelper.Create();
}

// 设置ServerGlobal的SERVICE_ID
void ServerGlobal::setServerGlobalService(SERVICE_ID serviceID)
{
	m_ServerGlobalServiceID = serviceID;
}

// 取得ServerGlobal的SERVICE_ID
SERVICE_ID ServerGlobal::getServerGlobalService()
{
	return m_ServerGlobalServiceID;
}

IActorMgr*  ServerGlobal::getActorMgr()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pActorMgr )
        return m_pActorMgr;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pActorMgr = ptr->getActorMgr();
    }
    return m_pActorMgr;
}

// 取得服务器控制器
IServerController*  ServerGlobal::getServerControllor()
{
    return &g_SceneServerControl;
}

ITeamScene* ServerGlobal::getTeamScene()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pTeamScene )
        return m_pTeamScene;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pTeamScene = ptr->getTeamScene();
    }
    return m_pTeamScene;
}


// 取得日记服务
IOSSLogService * ServerGlobal::getOSSLogService()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pOssLogService )
        return m_pOssLogService;
#endif

    if(m_pFramework == 0)
    {
        return NULL;
    }

    SERVICE_PTR ptrService = (SERVICE_PTR)m_pFramework->get_service_manager()->find_service("OssLogService");
    if(ptrService != 0)
    {
        m_pOssLogService = (IOSSLogService*)ptrService->query_interface();
    }

    return m_pOssLogService;
}

IOrderScene* ServerGlobal::getOrderScene()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pOrderScene )
        return m_pOrderScene;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pOrderScene = ptr->getOrderScene();
    }
    return m_pOrderScene; 
}

IStaticWarScene* ServerGlobal::getStaticWarScene()
{
    #ifdef SUPORT_SERVICE_CACHE
    if ( m_pStaticWarScene )
        return m_pStaticWarScene;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pStaticWarScene = ptr->getStaticWarScene();
    }
    return m_pStaticWarScene; 
}

ILegendCupScene* ServerGlobal::getLegendCupScene()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pLegendCupScene )
        return m_pLegendCupScene;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pLegendCupScene = ptr->getLegendCupScene();
    }
    return m_pLegendCupScene; 
}

IAntiAddictServer* ServerGlobal::getAntiAddictServer()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pAntiAddictServer )
		return m_pAntiAddictServer;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pAntiAddictServer = ptr->getAntiAddictServer();
	}
	return m_pAntiAddictServer;
}

// 取杂项管理对象
IMiscManager* ServerGlobal::getMiscManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pMiscManager )
        return m_pMiscManager;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pMiscManager = ptr->getMiscManager();
    }

    return m_pMiscManager;
}

// 获取资源管理接口
IResourceManager  * ServerGlobal::getResourceManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pResourceManager )
        return m_pResourceManager;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pResourceManager = ptr->getResourceManager();
    }

    return m_pResourceManager;
}

// 取得师徒服务器
IMentorshipScene* ServerGlobal::getMentorshipScene()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pMentorshipScene )
		return m_pMentorshipScene;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pMentorshipScene = ptr->getMentorshipScene();
	}
	return m_pMentorshipScene;
}

// 取得邮件服务
IMailScene* ServerGlobal::getMailScene()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pMailScene )
		return m_pMailScene;
#endif

	IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pMailScene = ptr->getMailScene();
	}
	return m_pMailScene;
}

HeightFieldMgr* ServerGlobal::getHeightFieldMgr()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pHeightFieldMgr )
        return m_pHeightFieldMgr;
#endif

    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pHeightFieldMgr = ptr->getHeightFieldMgr();
    }
    return m_pHeightFieldMgr;
}
