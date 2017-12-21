/*******************************************************************
** 文件名:	SocialGlobal.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "ISocialGlobalService.h"
#include "SocialGlobal.h"
#include "SocialGlobalHelper.h"
#include "ShareServerHelper.h"
#include "ServerControl.h"

#include "DbgHelp.h"
#include "IStackWalk.h"

#include "SocialServer.h"

// 全局服务器不要释放，这样可以缓存全局指针
#ifndef SUPORT_SERVICE_CACHE
#define SUPORT_SERVICE_CACHE
#endif

SocialGlobal::SocialGlobal() 
		: m_nServerID(0)
		, m_SocialGlobalServiceID(INVALID_SERVICE_ID)
#ifdef SERVICE_HANDLE_MESSAGE
        ,m_pMessageDispatch(0)
#endif
		, m_pFramework(0)
		, m_pCenterConnector(0)
		, m_pGatewayAcceptor(0)
		, m_pBridgeConnector(0)
		, m_pManagerConnector(0)
		, m_pSNSConnector(0)
		, m_pSceneManager(0)
		, m_pSchemeCenter(0)
		, m_pSchemeEngine(0)
		, m_pEventEngine(0)
        , m_pResourceManager(0)
        , m_pMiscManager(0)
		, m_pMatchManager(0)
		, m_pDBEngineService(0)
		, m_pShareReceiver(0)
		, m_pShareSceneSvrSource(0)
        , m_pClanManager(0)
        , m_pKinManager(0)
		, m_pChatManager(0)
        , m_pTeamManager(0)
        , m_pOrderManager(0)
		, m_pVoiceConnector(0)
		, m_pMentorshipManager(0)
        , m_pLegendCupManager(0)
		, m_pSNSManager(0)
        , m_pLuaManager(0)
		, m_pOSSLogService(0)
        , m_pTaskManager(0)
		, m_pMailManager(0)
{
    // 工作目录
    memset(m_szWorkDir, 0, sizeof(m_szWorkDir));
}

SocialGlobal::~SocialGlobal()
{

}


/** 初始化启动环境
@param   
@param   
@return  
*/
bool SocialGlobal::InitEnvironment(void)
{
    // 取得本地路径
    int nDirLen = sizeof(m_szWorkDir);
    sstrcpyn(m_szWorkDir, rkt::getWorkDir(), nDirLen);	

    // 设置搜索路径
    getFileSystem()->addFindPath(m_szWorkDir);

    // 设置scp路径
    string stringScpDir = m_szWorkDir;
    stringScpDir += "\\Scp";
    getFileSystem()->addFindPath(stringScpDir.c_str());
    
    // 设置lua路径
    string stringLuaDir = m_szWorkDir;
    stringLuaDir += "\\Lua";
    getFileSystem()->addFindPath(stringLuaDir.c_str());

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
					3. 启动其它服务
@param				: 
@return
*/
bool SocialGlobal::Start(unsigned short nServerID )
{
	setGlobal((ISocialGlobal*)this);
	m_nServerID = nServerID;

    // 设定Trace相关
    setTrace();

	return Initialize();
}

/** 正式终止本服务器的运行
@name				: 按照以下流程来执行: 
					1. 通告所有服务停止运行
					2. 释放框架
@param				: 
@return
*/
void SocialGlobal::Stop()
{
	UnInitialize();
	m_pFramework->get_service_manager()->stop_all();
}

void SocialGlobal::OnTimer()
{

}

// 内部初始化接口
bool SocialGlobal::Initialize()
{
	TraceLn("Start loading Framework...");
	m_pFramework = GetFrameworkProc();
	if ( m_pFramework==0 )
	{
		ErrorLn("Framework load failure! Please check Framework.dll exists");
		return false;
	}

	TraceLn("Start loading social_services.xml...");
	char szPath[MAX_PATH];
	sprintf(szPath,"%s\\social_services.xml",getWorkDir());
	if ( !m_pFramework->start(szPath) )
	{
		ErrorLn("social_services.xml load failure!");
		return false;
	}

	// 创建IEventEngine对象
	createEventEngine();

	EmphasisLn("Framework start success!");
	return true;
}

void SocialGlobal::UnInitialize()
{
	TraceLn("Start UnInitialize ...");

	TraceLn("Start release EventEngine...");
	SAFERELEASE(m_pEventEngine);


	m_pShareSceneSvrSource = NULL;
	m_pShareReceiver = NULL;
	m_pDBEngineService = NULL;
	m_pMatchManager = NULL;
	m_pSchemeEngine	= NULL;
	m_pSchemeCenter = NULL;
	m_pManagerConnector = NULL;
	m_pSNSConnector = NULL;
	m_pBridgeConnector = NULL;
	m_pGatewayAcceptor = NULL;
	m_pCenterConnector = NULL;	
	m_SocialGlobalServiceID = INVALID_SERVICE_ID;
    m_pClanManager = NULL;
    m_pKinManager = NULL;
	m_pChatManager = NULL;
    m_pTeamManager = NULL;
    m_pOrderManager = NULL;
    m_pLegendCupManager= NULL;
    m_pMentorshipManager = NULL;
    m_pResourceManager = NULL;
    m_pMiscManager = NULL;
	m_pSNSManager = NULL;
    m_pLuaManager = NULL;
	m_pOSSLogService = NULL;
	m_pMailManager = NULL;
}

// 创建IEventEngine对象
void SocialGlobal::createEventEngine(void)
{
	m_pEventEngine = m_pFramework->create_event_engine();
}


// 设定Trace相关
bool SocialGlobal::setTrace()
{
    // 取得本地路径
    char szWorkDir[MAX_PATH];
    sstrcpyn(szWorkDir, getWorkDir(), MAX_PATH);
    getFileSystem()->addFindPath(szWorkDir);

    // 是否有log目录
    char szLogAbsolutePath[512] = { 0 };
    sprintf_s( szLogAbsolutePath, "%s\\Log", szWorkDir );
    bool isAbsolutePath = true; uint attrib = 0;

    if(!rkt::checkPath(szLogAbsolutePath, isAbsolutePath, attrib))
    {
        // 创建log目录
        rkt::createDirectoryRecursive(szLogAbsolutePath);
    }

    // trace文件地址	
    time_t	long_time; struct tm * local_time;
    time(&long_time); local_time = localtime(&long_time);

    char szTraceFileName[512] = { 0 };
    sprintf_s(szTraceFileName, "%s\\Log\\Social%02d%02d%02d%02d%02d_Trace.html", szWorkDir,
        local_time->tm_mon + 1,
        local_time->tm_mday,
        local_time->tm_hour,
        local_time->tm_min,
        local_time->tm_sec);

    char szCrashFileName[512] = { 0 };
    sprintf_s(szCrashFileName, "%s\\Log\\Social%02d%02d%02d%02d%02d_Crash.txt", szWorkDir,
        local_time->tm_mon + 1,
        local_time->tm_mday,
        local_time->tm_hour,
        local_time->tm_min,
        local_time->tm_sec);

    char szDumpFileName[512] = { 0 };
    sprintf_s(szDumpFileName, "%s\\Log\\Social%02d%02d%02d%02d%02d_Stack.dmp",  szWorkDir,
        local_time->tm_mon + 1,
        local_time->tm_mday,
        local_time->tm_hour,
        local_time->tm_min,
        local_time->tm_sec);

    TraceListener * pTraceListener = output2File(szTraceFileName);
    if(pTraceListener != NULL)
    {
        pTraceListener->setHasTime(true);
    }
    theApp.SetTraceFile(szTraceFileName);

    // 修改trace等级
    setTraceLevel(levelAll);

    // dump
#ifdef SUPPORT_STACKWALK
    IStackWalk * pStackWalk = createStackWalkProc();
    if(pStackWalk == NULL)
    {
        return false;
    }	

    // dump类型
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
////////////////////////////////////////////////////////////////////////

// 取得服务器ID
unsigned short SocialGlobal::getServerID() 
{
    return m_nServerID; 
}

/// 获取框架接口
IFramework *   SocialGlobal::getFramework()
{
	return m_pFramework;
}

/// 获得事件中心接口
IEventEngine *  SocialGlobal::getEventEngine()
{
	return m_pEventEngine;
}

/// 获得对时服务接口
ITimeSyncService *  SocialGlobal::getTimeSyncService()
{
	return m_pFramework->get_service_timesync();
}

/// 获取时间轴接口
ITimerService * SocialGlobal::getTimerService()
{
	return m_pFramework->get_service_timer();
}

/// 获取日期触发器接口
IDateTriggerService* SocialGlobal::getDateTriggerService()
{
    return m_pFramework->get_service_datetrigger();
}

/// 获取配置中心接口
ISchemeCenter * SocialGlobal::getSchemeCenter()
{
	if ( m_pSchemeCenter )
		return m_pSchemeCenter;

	ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pSchemeCenter = ptr->getSchemeCenter();
	}
	return m_pSchemeCenter;
}

/// 获取中心服连接器接口
ICenterConnectorService * SocialGlobal::getCenterConnectorService()
{
	if ( m_pCenterConnector )
		return m_pCenterConnector;

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("CenterConnector");
	if ( pContainer==0 )
		return 0;

	m_pCenterConnector = (ICenterConnectorService*)pContainer->query_interface();
	return m_pCenterConnector;
}

/// 获取语音服连接器接口
IVoiceConnectorService* SocialGlobal::getVoiceConnectorService()
{
    if ( m_pVoiceConnector )
        return m_pVoiceConnector;

    SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("VoiceConnector");
    if ( pContainer==0 )
        return 0;

    m_pVoiceConnector = (IVoiceConnectorService*)pContainer->query_interface();
    return m_pVoiceConnector;
}

/// 获取网关连接器接口
IGatewayAcceptorService * SocialGlobal::getGatewayAcceptorService()
{
	if ( m_pGatewayAcceptor )
		return m_pGatewayAcceptor;

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("GatewayConnector");
	if ( pContainer==0 )
		return 0;

	m_pGatewayAcceptor = (IGatewayAcceptorService*)pContainer->query_interface();
	return m_pGatewayAcceptor;
}


// 获取桥连接器接口
IBridgeConnectorService * SocialGlobal::getBridgeConnectorService()
{
	if ( m_pBridgeConnector )
		return m_pBridgeConnector;

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("BridgeConnector");
	if ( pContainer==0 )
		return 0;

	m_pBridgeConnector = (IBridgeConnectorService*)pContainer->query_interface();
	return m_pBridgeConnector;
}

// 获取管理器连接器接口
IManagerConnectorService * SocialGlobal::getManagerConnectorService()
{
	if ( m_pManagerConnector )
		return m_pManagerConnector;

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("ManagerConnector");
	if ( pContainer==0 )
		return 0;

	m_pManagerConnector = (IManagerConnectorService*)pContainer->query_interface();
	return m_pManagerConnector;
}


ISNSConnectorService * SocialGlobal::getSNSConnectorService()
{
	if ( m_pSNSConnector )
		return m_pSNSConnector;

	SERVICE_PTR pContainer = m_pFramework->get_service_manager()->find_service("SNSConnector");
	if ( pContainer==0 )
		return 0;

	m_pSNSConnector = (ISNSConnectorService*)pContainer->query_interface();
	return m_pSNSConnector;
}


/// 取得配置服务
ISchemeEngine * SocialGlobal::getSchemeEngine()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pSchemeEngine )
		return m_pSchemeEngine;
#endif

	ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pSchemeEngine = ptr->getSchemeEngine();
	}
	return m_pSchemeEngine;
}

// 消息分发器
IMessageDispatch* SocialGlobal::getMessageDispatch()
{
#ifdef SERVICE_HANDLE_MESSAGE
#   ifdef SUPORT_SERVICE_CACHE
    if ( m_pMessageDispatch )
        return m_pMessageDispatch;
#   endif
    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
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
INetMessageHandler*	SocialGlobal::getNetMessageHandler()
{
	return &m_NetMessageHandler;
}

// 取杂项管理对象
IMiscManager* SocialGlobal::getMiscManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pMiscManager )
        return m_pMiscManager;
#endif

    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pMiscManager = ptr->getMiscManager();
    }

    return m_pMiscManager;
}

// 取杂OSS日志管理对象
IOSSLogService* SocialGlobal::getOSSLogService()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pOSSLogService )
		return m_pOSSLogService;
#endif

	if(m_pFramework == 0)
	{
		return NULL;
	}

	SERVICE_PTR ptrService = (SERVICE_PTR)m_pFramework->get_service_manager()->find_service("OssLogService");
	if(ptrService != 0)
	{
		m_pOSSLogService = (IOSSLogService*)ptrService->query_interface();
	}

	return m_pOSSLogService;
}

// 获取资源管理接口
IResourceManager  * SocialGlobal::getResourceManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pResourceManager )
        return m_pResourceManager;
#endif

    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pResourceManager = ptr->getResourceManager();
    }

    return m_pResourceManager;
}

// 取任务数据操作管理对象
ITaskManager* SocialGlobal::getTaskManager()
{
    
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pTaskManager )
        return m_pTaskManager;
#endif

    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pTaskManager = ptr->getTaskManager();
    }

    return m_pTaskManager;
}

// 设置SocialGlobal的SERVICE_ID
void SocialGlobal::setSocialGlobalService(SERVICE_ID serviceID)
{
	m_SocialGlobalServiceID = serviceID;
}

// 取得SocialGlobal的SERVICE_ID
SERVICE_ID SocialGlobal::getSocialGlobalService()
{
	return m_SocialGlobalServiceID;
}

IMatchManager* SocialGlobal::getMatchManager()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pMatchManager )
		return m_pMatchManager;
#endif

	ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pMatchManager = ptr->getMatchManager();
	}

	return m_pMatchManager;
}


IClanManager* SocialGlobal::getClanManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pClanManager )
        return m_pClanManager;
#endif

    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pClanManager = ptr->getClanManager();
    }

    return m_pClanManager;
}


IKinManager* SocialGlobal::getKinManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pKinManager )
        return m_pKinManager;
#endif

    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pKinManager = ptr->getKinManager();
    }

    return m_pKinManager;
}

ITeamManager* SocialGlobal::getTeamManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pTeamManager )
        return m_pTeamManager;
#endif

    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pTeamManager = ptr->getTeamManager();
    }

    return m_pTeamManager;
}


ILegendCupManager* SocialGlobal::getLegendCupManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pLegendCupManager )
        return m_pLegendCupManager;
#endif

    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr)
    {
        m_pLegendCupManager = ptr->getLegendCupManager();
    }

    return m_pLegendCupManager;
}



IChatManager* SocialGlobal::getChatManager()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pChatManager )
	{
		return m_pChatManager;
	}
#endif
	ISocialGlobalService* ptr = SocialGlobalHelper().m_ptr;
	if ( ptr )
	{
		m_pChatManager = ptr->getChatManager();
	}
	return m_pChatManager;
}


ISNSManager* SocialGlobal::getSNSManager()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pSNSManager )
	{
		return m_pSNSManager;
	}
#endif
	ISocialGlobalService* ptr = SocialGlobalHelper().m_ptr;
	if ( ptr )
	{
		m_pSNSManager = ptr->getSNSManager();
	}
	return m_pSNSManager;
}

// 获取Lua服务
ILuaManager *SocialGlobal::getLuaManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pLuaManager )
    {
        return m_pLuaManager;
    }
#endif
    ISocialGlobalService* ptr = SocialGlobalHelper().m_ptr;
    if ( ptr )
    {
        m_pLuaManager = ptr->getLuaManager();
    }
    return m_pLuaManager;
}

IOrderManager* SocialGlobal::getOrderManager()
{
#ifdef SUPORT_SERVICE_CACHE
    if ( m_pOrderManager )
    {
        return m_pOrderManager;
    }
#endif
    ISocialGlobalService* ptr = SocialGlobalHelper().m_ptr;
    if ( ptr )
    {
        m_pOrderManager = ptr->getOrderManager();
    }
    return m_pOrderManager;
}


IMentorshipManager* SocialGlobal::getMentorshipManager()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pMentorshipManager )
		return m_pMentorshipManager;
#endif

	ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pMentorshipManager = ptr->getMentorshipManager();
	}

	return m_pMentorshipManager;
}


IMailManager* SocialGlobal::getMailManager()
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pMailManager )
		return m_pMailManager;
#endif

	ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pMailManager = ptr->getMailManager();
	}

	return m_pMailManager;
}


// 取得DBEngine服务
IDBEngineService* SocialGlobal::getDBEngineService()
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
IShareReceiver * SocialGlobal::getShareReceiver(void)
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pShareReceiver )
		return m_pShareReceiver;
#endif

	ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pShareReceiver = ptr->getShareReceiver();
	}

	return m_pShareReceiver;
}

// 取得场景服的数据共享源
IShareSceneSvrSource * SocialGlobal::getShareSceneSvrSource(void)
{
#ifdef SUPORT_SERVICE_CACHE
	if ( m_pShareSceneSvrSource )
		return m_pShareSceneSvrSource;
#endif

	ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
	if (ptr)
	{
		m_pShareSceneSvrSource = ptr->getShareSceneSvrSource();
	}
	return m_pShareSceneSvrSource;
}

// 设置共享对象
void SocialGlobal::setShareObject(IShareReceiver * pShareReceiver, IShareSceneSvrSource *pShareSceneSvrSource)
{
	m_pShareReceiver = pShareReceiver;
	m_pShareSceneSvrSource = pShareSceneSvrSource;
}

// 启动共享
bool SocialGlobal::startShareServer(void)
{
	return g_ShareServerHelper.Create();
}


// 取得服务器控制器
IServerController*  SocialGlobal::getServerControllor()
{
    return &g_ServerControl;
}

