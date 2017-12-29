/*******************************************************************
** 文件名:	SocialGlobalService.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "ISocialGlobal.h"
#include "SocialGlobalService.h"
#include "IMatchManager.h"
#include "IMatchService.h"
#include "IChatManager.h"
#include "IServerController.h"
#include "ITeamManager.h"
#include "IManagerConnectorService.h"
#include "IVoiceConnectorService.h"
#include "ILuaManager.h"
#include "DManagerModule.h"
#include "MiscManager.h"
#include "ResourceManager.h"
#include "IOSSLogServer.h"
#include "ITaskManager.h"
#include "DateHandler.h"
#include "IDateTriggerService.h"
#include "DBHttpService.h"

SocialGlobalService::SocialGlobalService() 
		: m_pSchemeCenter(0)
		, m_pSchemeEngine(0)
		, m_pMatchManager(0)
		, m_pShareReceiver(0)
		, m_pShareSceneSvrSource(0)
        , m_pOrderManager(0)
        , m_pKinManager(0)
        , m_pClanManager(0)
        , m_pTeamManager(0)
		, m_pSNSManager(0)
        , m_pLuaManager(0)
        , m_nCheckDay(0)
        , m_dwRetryStartFlag(0)
        , m_dwRetryUserData(0)
        , m_pLegendCupManager(0)
		, m_pMentorshipManager(0)
        , m_pResourceManager(0)
        , m_pMiscManager(0)
        , m_pOSSLogServer(0)
        , m_pTaskManager(0)
		, m_pMailManager(0)
        , m_DBHttpService(0)
{
}

SocialGlobalService::~SocialGlobalService()
{

}

// 启动
// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
bool SocialGlobalService::on_start(void * data,int len )
{
	return create();
}

// 停止
bool SocialGlobalService::on_stop()
{
    // 杀掉定时器
    ITimerService*  pTimerService = gSocialGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->KillTimer(ETIMER_CHECK_BASESERVICE, CAST_TYPE(ITimerHandler*, this));
        pTimerService->KillTimer(ETIMER_CHECK_NEWDAY, CAST_TYPE(ITimerHandler*, this));
        pTimerService->KillTimer(ETIMER_MANAGER_RETRYSTART, CAST_TYPE(ITimerHandler*, this));
    }

    IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
    if(pEventEngine)
    {
        // 取消订阅 中心连接器服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_CENTERCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0);
        // 取消订阅 网关连接器服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0);
        // 取消订阅 桥连接器服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_BRIDGECONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0);
        // 取消订阅 DB引擎服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_DBENGINE_STARTED, SOURCE_TYPE_SYSTEM, 0);
    }

    m_UnLockHandler.onStop();

    // 释放子模块的顺序最好和初始化顺序相反
	//if(m_pMailManager)			m_pMailManager->onStop();
    if(m_pTaskManager)              m_pTaskManager->onStop();
	if(m_pOSSLogServer)				m_pOSSLogServer->onStop();
    if(m_pShareReceiver)            m_pShareReceiver->onStop();
    if(m_pShareSceneSvrSource)      m_pShareSceneSvrSource->onStop();
    if (m_DBHttpService)            m_DBHttpService->onStop();
    //if(m_pMatchManager)           m_pMatchManager->onStop();
    //if(m_pSchemeCenter)           m_pSchemeCenter->onStop();
    //if(m_pOrderManager)           m_pOrderManager->onStop();
    //if(m_pKinManager)             m_pKinManager->onStop();
    //if(m_pClanManager)            m_pClanManager->onStop();
    //if(m_pTeamManager)            m_pTeamManager->onStop();
    //if(m_pMentorshipManager)      m_pMentorshipManager->onStop();
    //if(m_pLegendCupManager)       m_pLegendCupManager->onStop();    
    if(m_pMiscManager)              m_pMiscManager->onStop();
    if(m_pResourceManager)          m_pResourceManager->onStop();

	return true;
}

////////////////// ISocialGlobalService ////////////////////////////////////////////////////////
// 释放
bool SocialGlobalService::create()
{
	IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
	if(pEventEngine)
	{
		// 订阅 中心连接器服务已启动
		pEventEngine->Subscibe(this, EVENT_SYSTEM_CENTERCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
        // 订阅 网关连接器服务已启动
        pEventEngine->Subscibe(this, EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
        // 订阅 桥连接器服务已启动
        pEventEngine->Subscibe(this, EVENT_SYSTEM_BRIDGECONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
        // 订阅 DB引擎服务已启动
        pEventEngine->Subscibe(this, EVENT_SYSTEM_DBENGINE_STARTED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
	}

	// 配置引擎
	TraceLn("Start loading SchemeEngine...");
	if ((m_pSchemeEngine = CreateSchemeEngineProc()) == NULL)
	{
		ErrorLn("SchemeEngine load failure!");
		return false;
	}

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

	TraceLn("SocialGlobal Service start success!");
	return true;
}

// 释放
void SocialGlobalService::release()
{
    m_MonitorOnline.release();

    TraceLn("Start release DBHttpService...");
    SAFERELEASE(m_DBHttpService);

	TraceLn("Start release ShareReceiver...");
	SAFERELEASE(m_pShareReceiver);

	TraceLn("Start release ShareSceneSvrSource...");
	SAFERELEASE(m_pShareSceneSvrSource);

	TraceLn("Start release MatchManager...");
	safeRelease(m_pMatchManager);

	TraceLn("Start release SchemeCenter...");
	safeRelease(m_pSchemeCenter);

	TraceLn("Start release SchemeEngine...");
	SAFERELEASE(m_pSchemeEngine);

    TraceLn("Start release OrderManager...");
    SAFERELEASE(m_pOrderManager);
    
    TraceLn("Start release KinManager...");
    SAFERELEASE(m_pKinManager);

    TraceLn("Start release ClanManager...");
    safeRelease(m_pClanManager);

    TraceLn("Start release TeamManager...");
    safeRelease(m_pTeamManager);

	TraceLn("Start release MentorshipManager...");
	SAFERELEASE(m_pMentorshipManager);
    
    TraceLn("Start release LegendCupManager...");
    safeRelease(m_pLegendCupManager);

	TraceLn("Start release SNSManager...");
	safeRelease(m_pSNSManager);

    TraceLn("Start release LuaManager...");
    safeRelease(m_pLuaManager);
	
    TraceLn("Start release MiscManager...");
    safeRelease(m_pMiscManager);
    
    TraceLn("Start release ResourceManager...");
    safeRelease(m_pResourceManager);
    
    TraceLn("Start release OSSLogServer...");
    safeRelease(m_pOSSLogServer);

    TraceLn("Start release TaskManger...");
    safeRelease(m_pTaskManager);

	TraceLn("Start release MailManager...");
	SAFERELEASE(m_pMailManager);

	delete this;
}

// 获取当前天
int SocialGlobalService::getDayOfMonth(void)
{
    time_t	long_time; 
    tm local_time;
    time(&long_time);
    localtime_s(&local_time,&long_time);

    return  local_time.tm_mday;
}

// 消息分发器
IMessageDispatch* SocialGlobalService::getMessageDispatch()
{
    return &m_MessageDispatch;
}

// 启动网关服务
inline void SocialGlobalService::startGateWayService()
{
    if(m_pMiscManager)
    {
        TraceLn("Start Create GatewayConnector ...");
        m_pMiscManager->create_service("GatewayConnector");
    }
}

// 启动桥服务
inline void SocialGlobalService::startBridgeService()
{
    if(m_pMiscManager)
    {
        TraceLn("Start Create BridgeConnector ...");
        m_pMiscManager->create_service("BridgeConnector");
    }
}

void SocialGlobalService::startSNSService()
{
	// 启动SNS服务
    TraceLn("Start Create SNSConnector ...");
      m_pMiscManager->create_service("SNSConnector");
}

// 后续启动
inline void SocialGlobalService::startEntries(void)
{
    TraceLn("Start loading SchemeCenter...");
    m_pSchemeCenter = createSchemeCenterProc();
    if ( m_pSchemeCenter ==0 || !m_pSchemeCenter->load())
    {
        ErrorLn("SchemeCenter load failure!");
        return;
    }

	// 启动共享
	TraceLn("Start Create ShareServerHelper...");
	if(!gSocialGlobal->startShareServer())
	{
		ErrorLn("ShareServerHelper Create Failed!");
		return;
	}

	// 创建大厅
	TraceLn("Start loading MatchService...");
	m_pMatchManager = CreateMatchManagerProc();
	if ( m_pMatchManager ==0 || !m_pMatchManager->load())
	{
		ErrorLn("MatchManager load Failed!");
		return;
	}

	// 创建聊天管理器
	TraceLn("Start Loading ChatService..." );
	m_pChatManager = CreateChatManagerProc();
	if ( m_pChatManager == 0 || !m_pChatManager->load() )
	{
		ErrorLn("ChatManager load Failed! " );
		return;
	}

    // 创建排行榜管理器
    TraceLn("Start Loading OrderService..." );
    m_pOrderManager = CreateOrderManagerProc();
    if ( m_pOrderManager == 0 || !m_pOrderManager->load() )
    {
        ErrorLn("OrderManager load Failed! " );
        return;
    }

	TraceLn("Start Loading KinService..." );
    m_pKinManager = CreateKinManagerProc();
    if ( m_pKinManager == 0 || !m_pKinManager->Load() )
    {
        ErrorLn("KinManager load Failed! " );
        return;
    }

    // 创建帮会管理器
    TraceLn("Start Loading ClanService...");
    m_pClanManager = CreateClanManagerProc();
    if (m_pClanManager == 0 || !m_pClanManager->load())
    {
        ErrorLn("ClanManager load Failed! ");
        return;
    }

    TraceLn("Start Loading TeamService..." );
    m_pTeamManager = CreateTeamManagerProc();
    if ( m_pTeamManager == 0 || !m_pTeamManager->Load() )
    {
        ErrorLn("TeamManager load Failed! " );
        return;
    }

    TraceLn("Start Loading LegendCupService..." );
    m_pLegendCupManager = CreateLegendCupManagerProc();
    if ( m_pLegendCupManager == 0 || !m_pLegendCupManager->load() )
    {
        ErrorLn("m_pLegendCupManager load Failed! " );
        return;
    }

	TraceLn("Start Loading MentorshipService..." );
	m_pMentorshipManager = CreateMentorshipManagerProc();
	if ( m_pMentorshipManager == 0 || !m_pMentorshipManager->Load() )
	{
		ErrorLn("MentorshipManager load Failed! " );
		return;
	}

	// 创建SNS管理器
	TraceLn("Start Loading SNSService..." );
	m_pSNSManager = CreateSNSManagerProc();
	if ( NULL == m_pSNSManager || !m_pSNSManager->load() )
	{
		ErrorLn("SNSManager load Failed! " );
		return;
	}

    // 创建Lua管理器
    TraceLn("Start Loading LuaService...");
    m_pLuaManager = CreateLuaManagerProc();
    if ( m_pLuaManager == 0 || !m_pLuaManager->load() )
    {
        ErrorLn("LuaManager load Failed! ");
        return;
    }

	// OSS日志
	TraceLn("Start loading OssLog Server...");
	m_pOSSLogServer = CreateOSSLogServerProc();
	if ( m_pOSSLogServer ==0 )
	{
		ErrorLn("OssLog Server load failure!");
		return;
	}

    // Task
    TraceLn("Start loading Task Manager...");
    m_pTaskManager = CreateTaskManagerProc();
    if ( m_pTaskManager ==0 )
    {
        ErrorLn("Task Manager load failure!");
        return;
    }

	// Mail
	TraceLn("Start Loading MailService..." );
	m_pMailManager = CreateMailManagerProc();
	if ( m_pMailManager == 0 || !m_pMailManager->Load() )
	{
		ErrorLn("MailManager load Failed! " );
		return;
	}

    TraceLn("Start Loading DBHttpService...");
    m_DBHttpService = CreateDBHttpServiceProc();
    if (m_DBHttpService == 0)
    {
        ErrorLn("DBHttpService load failure!");
        return;
    }

    //////////////////////////////////////////////////////////////////////////
    // 上次定时自动处理的时间日期数
    m_nCheckDay = getDayOfMonth();
    // 检测新一天
    ITimerService*  pTimerService = gSocialGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->SetTimer(ETIMER_CHECK_NEWDAY, CHECK_NEWDAY_INTERVAL, CAST_TYPE(ITimerHandler*, this));
        // 检测已创建的Service是否起好，如果OK-〉启动CenterConnecter
        pTimerService->SetTimer(ETIMER_CHECK_BASESERVICE, CHECK_BASESERVICE_INTERVAL, CAST_TYPE(ITimerHandler*, this));
    }

    m_MonitorOnline.create();
    m_UnLockHandler.create();
}


// 创建共享
bool SocialGlobalService::createShareService(void)
{
	ICenterConnectorService*  pCenterConnectorService = gSocialGlobal->getCenterConnectorService();
	if(!pCenterConnectorService)
	{
		ErrorLn( __FUNCTION__ <<": CenterConnectorService==NULL!");
		return false;
	}
	IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();

	// 共享数据接收器
	TraceLn("Start Create Share Receiver ...");
	m_pShareReceiver = CreateShareReceiverProc(pCenterConnectorService, pEventEngine);
	if(m_pShareReceiver == NULL)
	{
		ErrorLn("Create Share Receiver Failed!... ...");
		return false;
	}
	
	gSocialGlobal->setShareObject(m_pShareReceiver, m_pShareSceneSvrSource);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// 获取配置中心接口
ISchemeCenter * SocialGlobalService::getSchemeCenter()
{
	return m_pSchemeCenter;
}

/// 取得配置服务
ISchemeEngine * SocialGlobalService::getSchemeEngine()
{
	return m_pSchemeEngine;
}

IMatchManager* SocialGlobalService::getMatchManager()
{
	return m_pMatchManager;
}

IChatManager* SocialGlobalService::getChatManager()
{
	return m_pChatManager;
}

IOrderManager* SocialGlobalService::getOrderManager()
{
    return m_pOrderManager;
}

IShareReceiver*	SocialGlobalService::getShareReceiver()
{
	return m_pShareReceiver;
}
IShareSceneSvrSource* SocialGlobalService::getShareSceneSvrSource()
{
	return m_pShareSceneSvrSource;
}

IClanManager* SocialGlobalService::getClanManager()
{
    return m_pClanManager;
}

IKinManager* SocialGlobalService::getKinManager()
{
    return m_pKinManager;
}

ITeamManager* SocialGlobalService::getTeamManager()
{
    return m_pTeamManager;
}

ILegendCupManager* SocialGlobalService::getLegendCupManager()
{
    return m_pLegendCupManager;
}


IMentorshipManager* SocialGlobalService::getMentorshipManager()
{
	return m_pMentorshipManager;
}

ISNSManager* SocialGlobalService::getSNSManager()
{
	return m_pSNSManager;
}

ILuaManager* SocialGlobalService::getLuaManager()
{
    return m_pLuaManager;
}

// 取资源管理对象
IResourceManager* SocialGlobalService::getResourceManager()
{
    return m_pResourceManager;
}

// 取杂项管理对象
IMiscManager* SocialGlobalService::getMiscManager()
{
    return m_pMiscManager;
}

// 取OSS日志管理对象
IOSSLogServer* SocialGlobalService::getOSSLogServer()
{
	return m_pOSSLogServer;
}

// 取任务数据操作管理对象
ITaskManager* SocialGlobalService::getTaskManager()
{
    return m_pTaskManager;
}

// 获取邮件管理对象
IMailManager* SocialGlobalService::getMailManager()
{
	return m_pMailManager;
}

//////////////////////////////////////////////////////////////////////////
// 请求跨Service处理服务控制，有些数据需要在Service上才能处理
void SocialGlobalService::requestHandleServerControl(DWORD dwState)
{
    gSocialGlobal->getServerControllor()->OnDo(dwState);
}

// 请求管理器启动
bool SocialGlobalService::requestManagerStart(DWORD dwStartFlag, DWORD dwUserData)
{
    IManagerConnectorService * pManagerConnectorService =  gSocialGlobal->getManagerConnectorService();
    if(pManagerConnectorService && pManagerConnectorService->requestStart(dwStartFlag, dwUserData))
    {
        return true;
    }

    m_dwRetryStartFlag = dwStartFlag;
    m_dwRetryUserData = dwUserData;
    gSocialGlobal->getTimerService()->SetTimer(ETIMER_MANAGER_RETRYSTART, CHECK_RETRYSTART_INTERVAL, CAST_TYPE(ITimerHandler*, this), 1);

    return false;
}

// 请求执行解锁操作
void SocialGlobalService::requstExecuteUnlock(void)
{
    m_UnLockHandler.requstExecuteUnlock();
}
///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void SocialGlobalService::handleServerListUpdated()
{
    getMessageDispatch()->dispatchServerListUpdated();
}

/**
@name         : 通知服务器信息更新
@brief        : 例如服务器人数变化等等
*/
void SocialGlobalService::handleServerInfoUpdated(DWORD ServerID, BYTE nState, void* pInfo, size_t nInfolen)
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
void SocialGlobalService::handlePostMessage(DWORD server,const char * pData,WORD wDataLen)
{
    getMessageDispatch()->dispatchTransmit(server, (void*)pData, wDataLen);
}

/**
@name           : 处理其他服务器掉线时共享对象回调
@param wSrvType : 服务器类型
@param pData    : 消息内容
@param nDataSize:消息长度
*/
void SocialGlobalService::handleServerDisconnectShareCallback(WORD wSrvType, char* pData,  size_t nDataSize)
{
    // 场景服需要将玩家
    if(wSrvType == MSG_ENDPOINT_SCENE)
    {
        m_UnLockHandler.handleExecute(pData, nDataSize);
    }
    
}

///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
// 网关服务器通知游戏服务器有一个新的客户端连进来
void SocialGlobalService::handleClientIncoming(ClientID client)
{

}

// 网关服务器通知游戏服务器有新的数据包到达
void SocialGlobalService::handleGateWayReceivedData(ClientID client, void* data, size_t len) 
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
void SocialGlobalService::handleGateWayClientDisconnect(ClientID client,DWORD dwReason)
{
    // 通知matchservice角色断开
    SERVICE_PTR ptr = m_pMatchManager->getMatchService();
    if (ptr == NULL)
        return;
    
    IMatchService * pMatchService = (IMatchService*)ptr->query_interface();
    if (NULL != pMatchService)
    {
        // 断开连接
        pMatchService->onClientDisconnect(client, dwReason);
    }
}

///////ManagerConnector///////////////////////////////////////////////////////////////////
// 收到设置游戏ID
void SocialGlobalService::handleManagerSetGameWorldID()
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
        IServerController* pController = gSocialGlobal->getServerControllor();
        if(pController)
        {
            pController->GotoState(SERVER_RUN_STATE_GET_AUTH_RESOURCE);
        }
    }
}

// 收到管理器下发的资源数据
void SocialGlobalService::handleManagerResourceData(LPVOID pData,DWORD dwDataLen)
{
    if(m_pResourceManager)
    {
        m_pResourceManager->onRecvMessage((const char*)pData, dwDataLen);
    }
}

// 收到管理器下发的资源数据结果
void SocialGlobalService::handleManagerResourceResult(LPVOID pData,DWORD dwDataLen)
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
            m_dwRetryStartFlag = dwStartFlag;
            m_dwRetryUserData = dwUserData;
            gSocialGlobal->getTimerService()->SetTimer(ETIMER_MANAGER_RETRYSTART, CHECK_RETRYSTART_INTERVAL, CAST_TYPE(ITimerHandler*, this), 1);
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
    }

    if(canNextStep)
    {
        // 启动中心服
        if(m_pMiscManager)
        {
            m_pMiscManager->create_service("CenterConnector");
        }
        else
        {
            WarningLn(__FUNCTION__": MiscManager==NULL");
        }
    }
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
void SocialGlobalService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if(SOURCE_TYPE_SYSTEM == bSrcType)
	{
		switch(wEventID)
		{
        case EVENT_SYSTEM_MANAGECONNECTOR_STARTED:      // 管理器连接器已启动
            {
                gSocialGlobal->getServerControllor()->OnDo(UICONTROL_INIT_SUCCESS);
            }
            break;

        case EVENT_SYSTEM_DBENGINE_STARTED:             // DB引擎服务已启动
            {
                if(m_pMiscManager->registerDBEngineService())
                {
                    // 对游戏ID进行校验
                    m_pMiscManager->checkWorldID();
                }
                // startCenterService();
            }
            break;

		case EVENT_SYSTEM_CENTERCONNECTOR_STARTED:		// 中心连接器服务已启动
			{
                gSocialGlobal->getMiscManager()->create_service("VoiceConnector");
                startGateWayService();
			}
			break;

        case EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED:		// 网关连接器服务已启动
            {
                startBridgeService();
            }
            break;

        case EVENT_SYSTEM_BRIDGECONNECTOR_STARTED:		// 桥连接器服务已启动
            {
				startSNSService();
                createShareService();
                startEntries();
            }
            break;

		default:
			break;
		}
	}
}

////////////////// TimerHandler ////////////////////////////////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void SocialGlobalService::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case ETIMER_CHECK_BASESERVICE:          // 检测服务器启动时要加载的服务
        {
            onTimerCheckBaseService();
        }
        break;

    case ETIMER_CHECK_NEWDAY:               // 检测新一天
        {
            onTimerCheckNewDay();
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

// 定时检测基础服务是否已启好
void SocialGlobalService::onTimerCheckBaseService(void)
{
    IServiceMgr * pServiceMgr = gSocialGlobal->getFramework()->get_service_manager();
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
    gSocialGlobal->getTimerService()->KillTimer(ETIMER_CHECK_BASESERVICE, CAST_TYPE(ITimerHandler*, this));

    // 连接中心服
    if(!gSocialGlobal->getCenterConnectorService()->connectServer())
    {
        return;
    }

    // 连接语音服
    gSocialGlobal->getVoiceConnectorService()->connectServer();

    // 服务器启动完成
    gSocialGlobal->getEventEngine()->FireExecute(EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0, "", 0);
    TraceLn(_GT("============================社会服启动成功============================"));
}

// 定时检测是否新一天
void SocialGlobalService::onTimerCheckNewDay(void)
{
    int nCurrentDay = getDayOfMonth();
    // 如果不是新的一天,跳过
    if (nCurrentDay == m_nCheckDay )
    {
        return;
    }

    // 保存新的日期数
    m_nCheckDay = nCurrentDay;

    // 触发新一天事件
    gSocialGlobal->getEventEngine()->FireExecute(EVENT_NEW_DAY, SOURCE_TYPE_SYSTEM, 0, "", 0);
    TraceLn("Fire New Day Event");

    // 社会服务器发送场景服新的一天消息
    SNetMsgHead gamemsghead;
    gamemsghead.bySrcEndPoint   = MSG_ENDPOINT_SOCIAL;
    gamemsghead.byDestEndPoint  = MSG_ENDPOINT_SCENE;
    gamemsghead.byKeyModule     = MSG_MODULEID_COMMON;
    gamemsghead.byKeyAction     = SS_MSG_SYSTEM_NEWDAY;

    obuf obufData;
    obufData.push_back(&gamemsghead, sizeof(SGameMsgHead));
    if(!obufData.good())
    {
        ErrorLn(__FUNCTION__": send new day message to servers failed! reason：obufData.good()=false" );
        return;
    }
    ICenterConnectorService* pCenterConnector = gSocialGlobal->getCenterConnectorService();
    if(pCenterConnector == nullptr)
    {
        ErrorLn(__FUNCTION__": send new day message to servers failed! reason：pCenterConnector == nullptr" );
        return;
    }
    // 广播给所有的场景服
    pCenterConnector->sendDataToZoneSvr(obufData.data(), obufData.size(), INVALID_SERVER_ID);
}
