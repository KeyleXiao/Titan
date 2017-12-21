/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\GlobalVoiceServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音服务器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "GlobalVoiceServer.h"
#include "GameDatabaseAccessor.h"
#include "DBRequester.h"
#include "IStackWalk.h"
#include "VoiceServer.h"
#include "VoiceCallList.h"
#include "DGlobalMessage.h"


// 频道配置结构
static const SVoiceChannelConfig g_ChannelUpdateConfig[VoiceChannel_Max] = 
{
	// 频道类型					是否开放	成员列表	任意进入	麦序模式	自由麦模式	召集需要的最小等级	文字冷却	管理个数	频道推送	最大人数	语音品质				语音召集令冷却时间
	{ VoiceChannel_None,		false,		false,		false,		false,		false,		0,					6000,		0,			false,		0,			VoiceQuality_Low,		3600 },		// 无效频道
	{ VoiceChannel_GM,			true,		true,		false,		true,		true,		0,					30,			100,		true,		50000,		VoiceQuality_Normal,	3600 },		// 官方
	{ VoiceChannel_World,		true,		true,		false,		true,		true,		0,					30,			100,		true,		30000,		VoiceQuality_Normal,	3600 },		// 世界
	{ VoiceChannel_Country,		true,		true,		false,		true,		true,		0,					30,			100,		true,		10000,		VoiceQuality_Default,	3600 },		// 国家
	{ VoiceChannel_Clan,		true,		true,		false,		true,		true,		60,					1,			100,		true,		3000,		VoiceQuality_Default,	1200 },		// 帮会
	{ VoiceChannel_Kin,			true,		true,		false,		true,		true,		60,					1,			100,		true,		3000,		VoiceQuality_Default,	600 },		// 家族
	{ VoiceChannel_Group,		true,		true,		false,		true,		true,		0,					1,			32,			true,		3000,		VoiceQuality_Default,	600	},		// 团
	{ VoiceChannel_Team,		true,		true,		false,		true,		true,		0,					1,			32,			true,		100,		VoiceQuality_Default,	600	},		// 队伍
    { VoiceChannel_War,		    true,		true,		false,		true,		true,		0,					1,			32,			true,		100,		VoiceQuality_Default,	600	},		// 战场
	{ VoiceChannel_Personal,	true,		true,		true,		true,		true,		0,					1,			100,		false,		30000,		VoiceQuality_Default,	600	},		// 个人
};

// 房间配置
static const SVoiceRoomConfig g_RoomUpdateConfig[VoiceChannel_Max][VoiceRoomType_Max] =
{

	// [默认麦序模式],[麦序时长秒],[多久不说话 自动下麦]

	// 无效频道
	{
		{	VoiceMicState_MicMaster,	1,	1, 0	},	// 大厅
		{	VoiceMicState_MicMaster,	1,	1, 0	},	// 统战
		{	VoiceMicState_MicMaster,	1,	1, 0	},	// 娱乐
		{	VoiceMicState_MicMaster,	1,	1, 0	},	// 聊天
		{	VoiceMicState_MicMaster,	1,	1, 0	},	// 挂机
		{	VoiceMicState_MicMaster,	1,	1, 0	},	// 会议
		{	VoiceMicState_MicMaster,	1,	1, 0	},	// 办公
		{	VoiceMicState_MicMaster,	1,	1, 0	},	// 密室
	},
		// 官方
	{
		{	VoiceMicState_MicMaster,	300,	30, 1	},	// 新区主播
		{	VoiceMicState_MicFree,		300,	30, 1	},	// 休闲聊天
		{	VoiceMicState_MicOrder,		300,	30, 1	},	// 活动专区
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 客服专区
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 主播专区
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 官方会议
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 远征办公
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 官方密室
	},
		// 世界
	{
		{	VoiceMicState_MicOrder,		300,	30, 1	},	// 世界大厅
		{	VoiceMicState_MicFree,		300,	30, 1	},	// 军事统战
		{	VoiceMicState_MicOrder,		300,	30, 1	},	// 娱乐活动
		{	VoiceMicState_MicFree,		300,	30, 1	},	// 休闲聊天
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 练级挂机
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 世界会议
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 世界总部
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 世界密室
	},
		// 国家
	{
		{	VoiceMicState_MicMaster,	300,	30, 1	},	// 国家大厅
		{	VoiceMicState_MicFree,		300,	30, 1	},	// 国家统战
		{	VoiceMicState_MicOrder,		300,	30, 1	},	// 娱乐活动
		{	VoiceMicState_MicFree,		300,	30, 1	},	// 休闲聊天
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 练级挂机
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 国家会议
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 国王办公
		{	VoiceMicState_MicMaster,	300,	30, 0	},	// 国家密室
	},
		// 帮会
	{
		{	VoiceMicState_MicOrder,		300,	60, 1	},	// 帮会大厅
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 帮会统战
		{	VoiceMicState_MicOrder,		300,	60, 1	},	// 娱乐活动
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 休闲聊天
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 练级挂机
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 帮会会议
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 帮主办公
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 帮会密室
	},
		// 家族
	{
		{	VoiceMicState_MicOrder,		300,	60, 1	},	// 家族大厅
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 家族统战
		{	VoiceMicState_MicOrder,		300,	60, 1	},	// 娱乐活动
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 休闲聊天
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 练级挂机
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 家族会议
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 族长办公
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 家族密室
	},
		// 团队
	{
		{	VoiceMicState_MicOrder,		300,	60, 1	},	// 团队大厅
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 团队统战
		{	VoiceMicState_MicOrder,		300,	60, 1	},	// 娱乐活动
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 休闲聊天
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 练级挂机
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 团队会议
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 团长办公
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 团队密室
	},
		// 队伍
	{
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 队伍大厅
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 队伍统战
		{	VoiceMicState_MicOrder,		300,	60, 1	},	// 娱乐活动
		{	VoiceMicState_MicFree,		300,	60, 1	},	// 休闲聊天
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 练级挂机
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 队伍会议
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 队长办公
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 队伍密室
	},
    // 战场频道
    {
        {	VoiceMicState_MicFree,		300,	60, 1	},	// 队伍大厅
        {	VoiceMicState_MicFree,		300,	60, 1	},	// 队伍统战
        {	VoiceMicState_MicOrder,		300,	60, 1	},	// 娱乐活动
        {	VoiceMicState_MicFree,		300,	60, 1	},	// 休闲聊天
        {	VoiceMicState_MicMaster,	300,	60, 0	},	// 练级挂机
        {	VoiceMicState_MicMaster,	300,	60, 0	},	// 队伍会议
        {	VoiceMicState_MicMaster,	300,	60, 0	},	// 队长办公
        {	VoiceMicState_MicMaster,	300,	60, 0	},	// 队伍密室
    },
		// 个人
	{
		{	VoiceMicState_MicFree,		300,	60, 0	},	// 个人大厅
		{	VoiceMicState_MicFree,		300,	60, 0	},	// 军事统战
		{	VoiceMicState_MicOrder,		300,	60, 0	},	// 娱乐活动
		{	VoiceMicState_MicFree,		300,	60, 0	},	// 休闲聊天
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 练级挂机
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 个人会议
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 主人办公
		{	VoiceMicState_MicMaster,	300,	60, 0	},	// 个人密室
	},
};


/// purpose: 构造函数
GlobalVoiceServer::GlobalVoiceServer()
{
	setGlobal(static_cast<IGlobalVoiceServer*>(this));

	m_dwTotalUseTimes=0;
	m_dwStartTime=0;
	m_dwEndTime=0;
	m_dwTotalAdd=0;

	m_dwMaxServerID = 0;
	m_dwControlCode = 0;	// 当前操作代码

	memset(szTraceFileName,0,sizeof(szTraceFileName));
	memset(szCrashFileName,0,sizeof(szCrashFileName));
	memset(szDumpFileName,0,sizeof(szDumpFileName));
	memset(m_szWorkDir,0,sizeof(m_szWorkDir));
	memset(m_szServerBinWorkDir,0,sizeof(m_szServerBinWorkDir));

	m_ManagerConnector = NULL;

	m_EventEngine =  NULL;

	// 关闭程序标识
	m_bCoseAppFlag	= false;
	// 是否初始化
	m_bInit = false;
	// 是否启动
	m_bStart = false;

	m_bIsAnswer	= true;						// 是否有回应
	m_bIsSendDeConnectAlert = false;		// 是否已发断线报警
	m_dwLastAnswerTime	= 0;				// 最后收到心跳Tick时间	

	m_bUpdate = false;
	m_bNewDataCome = false;

	m_dwRecvSpeedServer = 0;		// 服务器收包速度
	m_dwSendSpeedServer = 0;		// 服务器发包速度

	m_dwRecvSpeedGateway = 0;		// 网关收包速度
	m_dwSendSpeedGateway = 0;		// 网关发包速度

	m_dwRecvSpeedClient = 0;		// 客户端收包速度
	m_dwSendSpeedClient = 0;		// 客户端发包速度

	m_dwMaxConnection = 0;

	// 数据库状态 0:未初始化,1:启动,2:已关闭
	m_dwDBInitState	 = 0;

	m_mapBanPDBID.clear();

	m_bShutDowning = false;

	// 是否启动了自动流量控制
	m_bAutoNetSpeed = false;
	
	m_dwFansTopListUpdateTime = 0;
}

/// purpose: 析构函数
GlobalVoiceServer::~GlobalVoiceServer()
{
	Close();
	UnInitEnvironment();	// 反初始化环境
	setGlobal(NULL);
}

/// purpose: 初始化启动环境
bool GlobalVoiceServer::InitEnvironment(void)
{
	if (IsInit())
	{
		return false;
	}

	// 读入配置文件
	LoadConfigInfo();

	// dump
#ifdef SUPPORT_STACKWALK
	IStackWalk * pStackWalk = createStackWalkProc();
	if(pStackWalk == NULL)
	{
		return false;
	}	

	// dump类型
	pStackWalk->setDumpType(MiniDumpWithFullMemory);

	// dump文件
	pStackWalk->setLogFileName((LPCTSTR)szCrashFileName);
	pStackWalk->setDumpFileName((LPCTSTR)szDumpFileName);
#endif

	// 初始化数据库连接对像
	new GameDatabaseAccessor;
	new CDBRequester;

	/// 获取物理处理器个数（如双核就有2个）
	DWORD dwCPUCounts = getPhysicalProcessorCount();
	DWORD dwThreadNum = 8;	// 默认网络8个线程处理收发
	// 如果CPU个数多于4个就加多
	if (dwCPUCounts>4)
	{
		dwThreadNum = 2*dwCPUCounts + 8;	// 每个CPU跑2个线程
	}

	// 初始化网络层
	if ( InitializeNetwork(dwThreadNum,true)<0 )
	{
		ErrorLn(_GT("初始化网络层失败，请检查配置."));
		return false;
	}

	TraceLn(_GT("CPU个数:")<<dwCPUCounts);
	TraceLn(_GT("网络层线程数:") << dwThreadNum);

	// 挂接事件
	HANDLE hNetEvent = GetNetworkEventHandle();
	GetAsynIoReactor()->AddEvent(hNetEvent);
	GetAsynIoReactor()->RegisterEventHandler(hNetEvent,this);

	m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	LARGE_INTEGER DueTime;
	DueTime.QuadPart = 0;
	SetWaitableTimer(m_hTimer, &DueTime, 33, NULL, NULL, TRUE);

	GetAsynIoReactor()->AddEvent(m_hTimer);
	GetAsynIoReactor()->RegisterEventHandler(m_hTimer,this);

	// 初始化事件引擎
	m_EventEngine = CreateEventEngineProc();
	if (NULL==m_EventEngine)
	{
		ErrorLn(_GT("初始化事件引擎失败!"));
		return false;
	}

	// 是否初始化
	m_bInit = true;

	// 启动管理连接器
	CreateManagerConnector();

	return true;
}

/// purpose: 反初始化环境
bool GlobalVoiceServer::UnInitEnvironment(void)
{
	if (!IsInit())
	{
		return false;
	}

	if ( CDBRequester::getInstancePtr() )
	{
		delete CDBRequester::getInstancePtr();
	}

	if ( GameDatabaseAccessor::getInstancePtr() )
	{
		delete GameDatabaseAccessor::getInstancePtr();
	}

	// 事件引擎
	if (m_EventEngine)
	{
		m_EventEngine->Release();
	}

	GetAsynIoReactor()->RemoveEvent(GetNetworkEventHandle());

	if ( m_hTimer!=INVALID_HANDLE )
	{
		GetAsynIoReactor()->RemoveEvent(m_hTimer);
		CloseHandle(m_hTimer);
		m_hTimer = INVALID_HANDLE;
	}

	//UninitializeNetwork();

	m_bInit = false;

	return true;
}

/**
@name                    : 初始化网络
@brief                   :
@param wServerListenPort : 服务器监听端口
@param wClientListenPort : 客户端监听端口
@return                  :
*/
bool GlobalVoiceServer::Create(WORD wServerListenPort,WORD wClientListenPort)
{
	try{
		// 是否启动
		if (IsStart())
		{
			return false;
		}

		// 创建网络监听
		if ( !m_ServerListener.Create(wServerListenPort,this) )
		{
			ErrorLn(_GT("语音服务器监听服务器端口[") << wServerListenPort << _GT("]失败!"));

			// 绑定端口失败,直接关闭,用来程序互斥
			m_ServerListener.Release();
			// 通知服务器关闭
			CWnd * pMain = AfxGetApp()->m_pMainWnd;
			if (pMain!=NULL)
			{
				WPARAM wParam = 0;
				LPARAM lParam = 0;
				pMain->SendMessage(WM_MY_CLOSE_APP,wParam,lParam);
			}
			return false;
		}
		TraceLn(_GT("服务器监听端口:") << wServerListenPort);

		char szName[64] = {0};
		sprintf_s(szName, sizeof(szName), "Server Listen:%d",wServerListenPort);
		m_ServerListener.SetName(a2utf8(szName));

		if ( !m_ClientListener.Create(wClientListenPort,this,DEFAULT_PACK))
		{
			ErrorLn(_GT("语音服务器监听网关端口[") << wClientListenPort << _GT("]失败!"));

			// 绑定端口失败,直接关闭,用来程序互斥
			m_ServerListener.Release();
			m_ClientListener.Release();
			// 通知服务器关闭
			CWnd * pMain = AfxGetApp()->m_pMainWnd;
			if (pMain!=NULL)
			{
				WPARAM wParam = 0;
				LPARAM lParam = 0;
				pMain->SendMessage(WM_MY_CLOSE_APP,wParam,lParam);
			}
			return false;
		}
		TraceLn(_GT("网关监听端口:") << wClientListenPort);
		sprintf_s(szName, sizeof(szName), "VoiceGateway Listen:%d",wClientListenPort);
		m_ClientListener.SetName(a2utf8(szName));

		m_TotalInfo.dwStartTime		= (DWORD)time(NULL);		// 启动时间
		m_TotalInfo.dwServerPort	= wServerListenPort;		// 服务器监听端口
		m_TotalInfo.dwClientPort	= wClientListenPort;		// 客户端监听端口

		// 连接数据库
		getFileSystem()->addFindPath(".");

        // 请求DBServer
        m_ManagerConnector->RequestStart(MANAGER_STARTFLAG_DBSERVER);

		//// DBVoiceServer.dbs,DBVoiceProcedure.dbs	
		//if ( !GameDatabaseAccessor::getInstance().Create("DBVoiceServer.dbs","DBVoiceProcedure.dbs") )
		//{
		//	ErrorLn("GameDatabaseAccessor initialized failed!");
		//	//AlertBad("语音服务器加载数据库连接出错!");
		//	// 绑定端口失败,直接关闭,用来程序互斥
		//	m_ServerListener.Release();
		//	m_ClientListener.Release();

		//	// 通知服务器关闭
		//	CWnd * pMain = AfxGetApp()->m_pMainWnd;
		//	if (pMain!=NULL)
		//	{
		//		WPARAM wParam = 0;
		//		LPARAM lParam = 0;
		//		pMain->SendMessage(WM_MY_CLOSE_APP,wParam,lParam);
		//	}
		//	return false;
		//}

		m_dwDBInitState = 0;	// 数据库状态 0:未初始化,1:启动,2:已关闭

		// 是否启动
		m_bStart = true;


		return true;
	}
	catch (...)
	{
		Trace(endl);
		Error("GlobalVoiceServer::create, catch exception"<<endl);
	}

	return false;
}

/// purpose: 关闭操作
bool GlobalVoiceServer::Close()
{
#define MyRelease(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}

	try{

		// 是否启动
		if (!IsStart())
		{
			return false;
		}

		// 关闭数据库回调
		GameDatabaseAccessor::getInstance().RemoveDBRetSink(this);
		// 关闭数据库
		if ( GameDatabaseAccessor::getInstancePtr() )
		{
			GameDatabaseAccessor::getInstance().Close();
		}

		// 关闭管理连接器
		ReleaseManagerConnector();

		// 清除服务器列表
		ServerList::getInstance().Clear();

		// 清除网关列表
		GatewayList::getInstance().Clear();

		// 清除客户端列表
		CClientList::getInstance().Clear();

		// 清除频道列表
		CChannelList::getInstance().Clear();

		CChannelList::getInstance().Release();

		CChatList::getInstance().Clear();

		CVoiceCallList::getInstance().Release();

		m_ClientListener.Release();			// 释放网关连接
		m_ServerListener.Release();			// 释放服务器连接

		// 是否启动
		m_bStart = false;

		// 通知UI已关闭
		CWnd * pMain = AfxGetApp()->m_pMainWnd;
		if (pMain!=NULL)
		{
			WPARAM wParam = 0;
			LPARAM lParam = 0;
			pMain->SendMessage(WM_MY_STOP_RESULT,wParam,lParam);
		}

		return true;
	}
	catch (...)
	{
		Error("GlobalVoiceServer::close, catch exception"<<endl);
	}
	return false;
}

// 真正关闭服务器进程
void GlobalVoiceServer::ShutDownApp(void)
{
	if(Close())
	{
		Trace("停止语音服务器 OK ！"<<endl);
	}
	else
	{
		Error("停止语音服务器 失败！"<<endl);
	}

	// 通知服务器关闭
	CWnd * pMain = AfxGetApp()->m_pMainWnd;
	if (pMain!=NULL)
	{
		WPARAM wParam = 0;
		LPARAM lParam = 0;
		pMain->SendMessage(WM_MY_CLOSE_APP,wParam,lParam);
	}
}

/// purpose: 是否初始化
bool GlobalVoiceServer::IsInit(void)
{
	return m_bInit;
}

/// purpose: 是否已启动
bool GlobalVoiceServer::IsStart(void)
{
	return m_bStart;
}

/// purpose: 关闭服务器程序
void GlobalVoiceServer::CloseServerApp(void)
{
	SetControlCode(VS_ControlCode_ShutDown);
}

/// purpose: 启动管理连接器
void GlobalVoiceServer::CreateManagerConnector(void)
{
	// 加载服务器管理器连接器设定
	Ini ini("Manager.ini");
	std::string strManagerServerIP = ini.getString("Setting","ManagerServerIP","127.0.0.1");
	int nManagerServerPort = ini.getInt("Setting","ManagerServerPort",8700);

	// 子服务器连接
	TraceLn(_GT("连接子服务器管理器:")<<strManagerServerIP.data()<<":"<<nManagerServerPort);
	m_ManagerConnector = CreateManagerConnectorProc(this,&m_TimerAxis);
	m_ManagerConnector->Create(MSG_MG_ENDPOINT_VOICE,0,"Voice Server");
	if(!m_ManagerConnector->Connect(strManagerServerIP.data(),nManagerServerPort))
	{
		TraceLn(_GT("连接子服务器管理器失败!"));
		return;
	}
	TraceLn(_GT("连接子服务器管理器成功!"));

	// 订阅 要求更新实时数据事件
	m_ManagerConnector->registerEventHandler(EVENT_MG_REAL_TIME_DATA,static_cast<IManagerEventHandler *>(this));
}

/// purpose: 关闭管理连接器
void GlobalVoiceServer::ReleaseManagerConnector(void)
{
	if (m_ManagerConnector)
	{
		// 取消订阅 要求更新实时数据事件
		m_ManagerConnector->unregisterEventHandler(EVENT_MG_REAL_TIME_DATA,static_cast<IManagerEventHandler *>(this));
		// 关闭子服务器连接
		TraceLn(_GT("关闭子服务器管理器连接!"));
		m_ManagerConnector->Release();
		m_ManagerConnector = NULL;
	}
}


// 设置当前操作代码  DWORD dwCode 参考 EMManagerControlCode
void GlobalVoiceServer::SetControlCode(DWORD dwCode)
{
	// 当前操作代码
	m_dwControlCode = dwCode;
}


// 执行操作代码
bool GlobalVoiceServer::DoControlCodeTask(DWORD dwCode)
{
	switch(dwCode)
	{
	case VS_ControlCode_StartService:		// 启动服务
		{
			WORD wServerListenPort =(WORD) VoiceServerSetting::getInstance().m_dwServerPort;
			WORD wClientListenPort =(WORD) VoiceServerSetting::getInstance().m_dwClientPort;

			m_dwMaxConnection = VoiceServerSetting::getInstance().m_dwMaxConnection;

			if(Create(wServerListenPort,wClientListenPort))
			{
				Trace(_GT("启动语音服务器 OK ！")<<endl);
			}
			else
			{
				Error(_GT("启动语音服务器 失败！")<<endl);
			}
		}
		break;
	case VS_ControlCode_StopService:		// 停止服务
	case VS_ControlCode_ShutDown:		// 关闭程序,停止服务后要关闭进程
		{
			if( !m_bShutDowning )
			{
				m_bShutDowning = true;

				// 关服时，强制把所有人踢出服务器，以免人为操作导致数据改变
				CClientList::getInstance().Clear();

				SetTimer( ETimerEventID_WaitShutDown, 5000, this, "等待数据库保存完成后关闭" );
			}
		}
		break;
	default:
		{
			ErrorLn(_GT("尚有一个执行操作代码未处理，dwCode = ")<<dwCode);
		}
		break;
	}
	return true;
}

/** 载入配置信息,在程序启动时加载
@param   
@return  
@note     
@retval  
*/ 
bool GlobalVoiceServer::LoadConfigInfo()
{
	// 读入配置文件
	VoiceServerSetting::getInstance().LoadSetting();

	// 服务器监听端口
	m_wServerListenPort = (WORD)( VoiceServerSetting::getInstance().m_dwServerPort);

	return true;
}


// 封PDBID
bool GlobalVoiceServer::BanPDBID( LONGLONG nChannelID, DWORD dwPDBID, DWORD dwTime )
{
	if( dwPDBID <= 0 || nChannelID <= 0 )
	{
		return false;
	}

	if( dwTime == 0 )
	{
		dwTime = VOICE_BAN_PDBID_TIME;
	}

	SVoiceChannelBanInfo banInfo;
	banInfo.dwBanTime = (DWORD)time(NULL) + dwTime;

	// 先找是否存在
	TMAP_VoiceBanPBID::iterator it = m_mapBanPDBID.find(dwPDBID);
	if( it == m_mapBanPDBID.end() )
	{
		// 如果没有找到，添加一个
		TMAP_VoiceChannelBanList newBanList;
		pair<TMAP_VoiceBanPBID::iterator,bool> retPair = m_mapBanPDBID.insert(TMAP_VoiceBanPBID::value_type(dwPDBID,newBanList));

		// 取得插入的列表
		TMAP_VoiceChannelBanList & banList = retPair.first->second;
		banList.insert(TMAP_VoiceChannelBanList::value_type(nChannelID, banInfo));	// 插入数据
	}
	else
	{
		TMAP_VoiceChannelBanList & banList = it->second;
		TMAP_VoiceChannelBanList::iterator itChannelID = banList.find(nChannelID);
		if( itChannelID == banList.end() )
		{
			// 没找到，要添加
			banList.insert(TMAP_VoiceChannelBanList::value_type(nChannelID, banInfo));	// 插入数据
		}
		else
		{
			// 找到了，更新时间
			itChannelID->second = banInfo;
		}
	}

	return true;
}

// 取消封PDBID
bool GlobalVoiceServer::UnbanPDBID( LONGLONG nChannelID, DWORD dwPDBID )
{
	if( dwPDBID <= 0 || nChannelID <= 0 )
	{
		return false;
	}

	// 先找是否存在
	TMAP_VoiceBanPBID::iterator it = m_mapBanPDBID.find(dwPDBID);
	if( it != m_mapBanPDBID.end() )
	{
		TMAP_VoiceChannelBanList & banList = it->second;
		TMAP_VoiceChannelBanList::iterator itChannelID = banList.find(nChannelID);
		if( itChannelID != banList.end() )
		{
			banList.erase(itChannelID);
			return true;
		}
	}

	return false;
}

// 是否被封，dwTimeLeft 返回剩余时间
bool GlobalVoiceServer::IsPDBIDBan( LONGLONG nChannelID, DWORD dwPDBID, DWORD & dwTimeLeft )
{
	dwTimeLeft = 0;

	if( dwPDBID <= 0 || nChannelID <= 0 )
	{
		dwTimeLeft = VOICE_BAN_PDBID_TIME;
		return true;
	}

	// 先找是否存在
	TMAP_VoiceBanPBID::iterator it = m_mapBanPDBID.find(dwPDBID);
	if( it != m_mapBanPDBID.end() )
	{
		TMAP_VoiceChannelBanList & banList = it->second;
		TMAP_VoiceChannelBanList::iterator itChannelID = banList.find(nChannelID);
		if( itChannelID != banList.end() )
		{
			DWORD dwNow = (DWORD)time(NULL);

			// 如果已经过了这个时间
			if( dwNow >= itChannelID->second.dwBanTime )
			{
				return false;
			}
			else	// 否则返回true
			{
				dwTimeLeft = itChannelID->second.dwBanTime - dwNow;

				return true;
			}
		}
	}

	return false;
}

// 增加一条邀请粉丝记录
bool GlobalVoiceServer::AddCallFansInfo( DWORD dwUDBID, DWORD dwCallTime )
{
	SVoiceCallFansInfo info;
	info.dwLastCallFansTime = dwCallTime;
	info.dwFansAcceptCount = 0;
	info.dwFansCancelCount = 0;

	TMAP_VoiceCallFans::iterator it = m_mapCallFans.find(dwUDBID);
	if( it != m_mapCallFans.end() )
	{
		it->second = info;
	}
	else
	{
		m_mapCallFans.insert( TMAP_VoiceCallFans::value_type( dwUDBID, info ) );
	}

	return true;
}

// 获取邀请粉丝记录
SVoiceCallFansInfo * GlobalVoiceServer::GetCallFansInfo( DWORD dwUDBID )
{
	TMAP_VoiceCallFans::iterator it = m_mapCallFans.find(dwUDBID);
	if( it != m_mapCallFans.end() )
	{
		return &(it->second);
	}

	return NULL;
}

// 更新邀请粉丝信息
bool GlobalVoiceServer::UpdateCallFansInfo( DWORD dwUDBID, DWORD dwFansAcceptCount, DWORD dwFansCancelCount )
{
	TMAP_VoiceCallFans::iterator it = m_mapCallFans.find(dwUDBID);
	if( it != m_mapCallFans.end() )
	{
		it->second.dwFansAcceptCount = dwFansAcceptCount;
		it->second.dwFansCancelCount = dwFansCancelCount;
		return true;
	}

	return false;
}

// 增加一条语音召集令记录
bool GlobalVoiceServer::AddADInfo( DWORD dwUDBID, DWORD dwChannelType, DWORD dwCallTime )
{
	SVoiceADInfo time;
	time.dwLastADTime = dwCallTime;

	TMAP_VoiceAD::iterator it = m_mapAD.find(dwUDBID);
	if( it == m_mapAD.end() )	// 没找到记录，需要先添加一条
	{
		TMAP_ADInfo newADInfo;	
		newADInfo.insert(TMAP_ADInfo::value_type((BYTE)dwChannelType, time) );
		m_mapAD.insert( TMAP_VoiceAD::value_type(dwUDBID, newADInfo) );
	}
	else	// 找到了直接放入
	{	
		TMAP_ADInfo & ADInfo = it->second;
		ADInfo[(BYTE)dwChannelType] = time;
	}

	return true;
}

// 设置召集信息给用户
void GlobalVoiceServer::SetADInfoToUser( CClientUser & user )
{
	TMAP_VoiceAD::iterator it = m_mapAD.find(user.GetUserInfo().dwUDBID);
	if( it != m_mapAD.end() )	// 没找到记录，需要先添加一条
	{
		TMAP_ADInfo & ADInfo = it->second;
		for( TMAP_ADInfo::iterator itInfo = ADInfo.begin(); itInfo != ADInfo.end(); ++ itInfo )
		{
			user.SetLastADTime( itInfo->first, itInfo->second.dwLastADTime );
		}
	}
}






////////// IManagerEventHandler 处理子服务器连接器执行事件 ///////////////////////////////
/** 处理子服务器连接器执行事件
@param   wEventID ：事件ID
@param   pszContext : 上下文
@param   nLen : 上下文长度
@retval buffer 
*/
void GlobalVoiceServer::OnManagerEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen)
{
	if (NULL==m_ManagerConnector)
	{
		return;
	}

	switch(wEventID)
	{
	case EVENT_MG_REAL_TIME_DATA:		// 要求更新实时数据事件
		{
			/// 获取物理处理器个数（如双核就有2个）
			static size_t dwCPUCounts = 0;
			if (dwCPUCounts==0)
			{
				dwCPUCounts = getPhysicalProcessorCount();
			}

			/// 获得当前进程的CPU占用百分比
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_CPUCurrentUsedValue,(int)(MANAGER_REALTIMEDATA_CPU_MAX*dwCPUCounts*getCPUCurrentUsedValueByCurrentProcess()));
			/// 获取当前进程的物理内存已使用大小
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_PhysicalMemoryUsedSize,(int)getPhysicalMemoryUsedSizeByCurrentProcess());
			/// 获取当前进程的虚拟内存已使用大小
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_VirtualMemoryUsedSize,(int)getVirtualMemoryUsedSizeByCurrentProcess());
			// 更新实时数据 在线数
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_OnLineCounts,(int)(CClientList::getInstance().CountUser()));
			// 客户端每秒发包速度
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_ClientSendSpeed,(int)(m_TotalInfo.dwSendSpeedClient));
			// 客户端每秒收包速度
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_ClientRecvSpeed,(int)(m_TotalInfo.dwRecvSpeedClient));
			// 服务器每秒发包速度
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_ServerSendSpeed,(int)(m_TotalInfo.dwSendSpeedServer));
			// 服务器每秒收包速度
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_ServerRecvSpeed,(int)(m_TotalInfo.dwRecvSpeedServer));

		}
		break;
	default:
		{
			ErrorLn(_GT("尚有一个子服务器连接器执行事件订阅了但未处理，wEventID = ")<<wEventID);
		}
		break;
	}
}

////////// IManagerMessageHandler 处理子服务器发过来的消息 //////////////////
/**
@name         : 处理其他服务器通过子服务器转发的消息
@brief        : 具体内容不清楚
@param actionId  : 消息码
@param head  : 消息头
@param data  : 消息内容
@param len:消息长度
*/
void GlobalVoiceServer::HandleManagerMessage(ulong actionId, SManagerMsgHead* head, LPSTR data, size_t len)
{
	
	switch(actionId)
	{
	case MSG_MG_HANDSHAKE_RESPONSE:		//  子服务器连接成功回应握手消息
		{
			if (m_ManagerConnector)
			{
				m_ManagerConnector->SetServerState(MG_SERVERSTATE_RUNING);// 已启动，正在远行
			}
		}
		break;

	case MSG_MG_REQUESTSTART_RESULT:	// 软件服务器向桥服务器请求启动结果消息,调用 RequestStart()后收到
		{
			if(data==NULL|| len != sizeof(SMsgManagerRequestStartResult_M))
			{
				ErrorLn(_GT("向桥服务器请求启动结果数据出错,数据有误！"));
				return;
			}

			SMsgManagerRequestStartResult_M * pMsg = (SMsgManagerRequestStartResult_M *)(data);

			WarningLn(_GT("请求启动，结果代码=")<<pMsg->dwResultFlag<<_GT(",启动标识=")<<pMsg->dwResultFlag<<_GT(",用户数据=")<<pMsg->dwUserData<<_GT(",结果信息=")<<pMsg->szResultMsg);


			if (pMsg->bResult==false)
			{
				ErrorLn(_GT("请求启动失败，结果代码=")<<pMsg->dwResultFlag<<_GT(",启动标识=")<<pMsg->dwResultFlag<<_GT(",用户数据=")<<pMsg->dwUserData<<_GT(",结果信息=")<<pMsg->szResultMsg);

				// 正在加载脚本数据,要脚本的可定时重试请求启动,直到取得
				if (pMsg->dwResultFlag==MANAGER_STARTCODE_LOADSCRIPT)
				{
					// 定时重试请求启动 调用 RequestStart()
					// todotodo
				}
			} 
			else
			{
				//启动服务器
				// todotodo
			}
		}
		break;
	case MSG_MG_SENDSCRIPTDATA:		// 收到服务器启动脚本文件数据
		{
			if(data==NULL|| len < sizeof(SMsgManagerSendScriptData_RM))
			{
				ErrorLn(_GT("收到服务器启动脚本文件数据出错,数据有误！"));
				return;
			}

			SMsgManagerSendScriptData_RM * pMsg = (SMsgManagerSendScriptData_RM *)(data);

			TraceLn(_GT("收到服务器启动脚本文件数据，游戏世界ID=")<<pMsg->dwWorldID<<_GT(",服务器类型=")<<pMsg->nServerType<<_GT(",脚本文件组ID=")<<pMsg->dwScriptGroupID<<_GT(",最大配置号=")<<pMsg->nMaxSchemeIndex<<_GT(",配置号=")<<pMsg->nSchemeIndex<<_GT(",本配置号数据长度=")<<pMsg->dwDataLens);

			if (pMsg->nServerType!=MSG_MG_ENDPOINT_VOICE)
			{
				ErrorLn(_GT("服务器类型[")<<pMsg->nServerType<<_GT("]与本服务器[")<<MSG_MG_ENDPOINT_ROOT<<_GT("]不符!"));
				return;
			}

			// 脚本文件数据,本配置号数据长度 pMsg->dwDataLens
			LPSTR pDataBuffer = (LPSTR)(data+sizeof(SMsgManagerSendScriptData_RM));

			// 加入文件系统中
            // 资源类型
            int nResourceType = pMsg->nResourceType;

            // 加载内存DBEngine
            LoadMemoryDB(pDataBuffer, pMsg->dwDataLens);
		}
		break;
	case MSG_MG_CLOSESERVER:		// 要求软件服务器关闭
		{
			if(data==NULL|| len != sizeof(SMsgManagerCloseServer_Z))
			{
				ErrorLn(_GT("要求软件服务器关闭出错,数据有误！"));
				return;
			}

			SMsgManagerCloseServer_Z * pMsg = (SMsgManagerCloseServer_Z *)(data);

			// 关闭服务器
			// todotodo
			WarningLn(_GT("请求关闭，用户ID=")<<pMsg->dwUserID<<_GT(",软件服务器UID=")<<pMsg->dwServerUID<<_GT(",服务器类型=")<<pMsg->dwServerType<<_GT(",第n号=")<<pMsg->dwSubID<<_GT(",关闭标识=")<<pMsg->dwCloseFlag<<_GT(",延迟关闭毫秒数=")<<pMsg->dwDelays);

			// 请求关闭,如果没有请求，就关了服务器，会当作当机处理
			m_ManagerConnector->RequestClose();

			/// 关闭服务器程序
			CloseServerApp();


		}
		break;
	case MSG_MG_SERVERSENDAD:		// 要求软件服务器发布公告
		{
			if(data==NULL|| len < sizeof(SMsgManagerServerSendAd_Z))
			{
				ErrorLn(_GT("要求软件服务器发布公告出错,数据有误！"));
				return;
			}

			SMsgManagerServerSendAd_Z * pMsg = (SMsgManagerServerSendAd_Z *)(data);

			// 公告信息
			char szSendAdMsg[512] = {0};

			char * pStrSendAd = data+sizeof(SMsgManagerServerSendAd_Z);
			DWORD dwMsgStrLens = pMsg->nSendMsgLens +1;
			if (dwMsgStrLens>sizeof(szSendAdMsg))
			{
				dwMsgStrLens = sizeof(szSendAdMsg);
			}
			if (dwMsgStrLens>0)
			{
				sstrcpyn(szSendAdMsg, pStrSendAd, dwMsgStrLens);		// 公告信息
			}

			// 发布公告
			// todotodo
			WarningLn(_GT("发布公告，用户ID=")<<pMsg->dwUserID<<_GT(",软件服务器UID=")<<pMsg->dwServerUID<<_GT(",服务器类型=")<<pMsg->dwServerType<<_GT(",第n号=")<<pMsg->dwSubID<<_GT(",标识=")<<pMsg->dwSendFlag<<_GT(",延迟关闭毫秒数=")<<pMsg->dwDelays<<_GT(",公告=")<<szSendAdMsg);
		}
		break;
	case MSG_MG_SHOWWINDOW:		// 通知服务器显示程序窗口
		{
			if(data==NULL)
			{
				ErrorLn(_GT("通知服务器显示程序窗口出错,数据有误！"));
				return;
			}

			SMsgManagerShowServerWindow_Z * pMsg = (SMsgManagerShowServerWindow_Z *)(data);

			WarningLn(_GT("通知服务器显示程序窗口，标识=")<<pMsg->dwFlag);

			// 通知服务器关闭
			CWnd * pMain = AfxGetApp()->m_pMainWnd;
			if (pMain!=NULL)
			{
				if (pMsg->dwFlag==0)
				{
					ShowWindowAsync(pMain->m_hWnd,SW_HIDE);
				}
				else
				{
					ShowWindowAsync(pMain->m_hWnd,SW_SHOWNORMAL);
				}
			}
		}
		break;
	case MSG_MG_CONTROLSOFTSERVER:		// 操作软件服务器消息
		{
			if(len < sizeof(SMsgManagerControlSoftServer_C))
			{
				break;
			}

			SMsgManagerControlSoftServer_C * pMsg = (SMsgManagerControlSoftServer_C *)(data);

			switch(pMsg->dwControlCode)
			{
			case MG_SoftControl_SetServerUID:	// 设定软件服务器的UID
				{
					// m_ManagerConnector->GetServerUID();	取得在桥服务器上的UID
					// m_ManagerConnector->GetWorldID();	取得游戏世界ID
					// 管理连接器已更新游戏世界ID,在这里可处理相关事件处理

					if (m_ManagerConnector)
					{
						const SGameWorldConnectorInfo *pWorldInfo = m_ManagerConnector->GetGameWorldInfo();

						m_TotalInfo.bIsPublic		= isPublicGameWorld();			// 是否公共游戏世界
						m_TotalInfo.dwGameID		= pWorldInfo->dwGameID;			// 游戏ID
						m_TotalInfo.dwAreaID		= pWorldInfo->dwAreaID;			// 游戏区域ID
						m_TotalInfo.dwWorldID		= pWorldInfo->dwWorldID;			// 游戏世界ID
						// 游戏名称
						sstrcpyn(m_TotalInfo.szGameName, pWorldInfo->szGameName, sizeof(m_TotalInfo.szGameName));
						// 游戏区域名称
						sstrcpyn(m_TotalInfo.szAreaName, pWorldInfo->szAreaName, sizeof(m_TotalInfo.szAreaName));
						// 游戏世界名称
						sstrcpyn(m_TotalInfo.szWorldName, pWorldInfo->szWorldName, sizeof(m_TotalInfo.szWorldName));
					}
				}
				break;
			}
		}
		break;
	default:
		{
			ErrorLn(_GT("尚有一个服务器管理消息未处理，actionId = ")<<actionId);
		}
		break;
	}
}



///////////// IAcceptorHandler /////////////////////////////////////////////////////////////
void GlobalVoiceServer::OnAccept( WORD wListenPort,IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	const char * ip = 0;
	DWORD dwPort = 0;
	pIncomingConn->GetRemoteAddr(&ip,dwPort);
	sprintf_s(szBuf, sizeof(szBuf), "GlobalVoiceServer::OnAccept() IP=%s,Port=%d",ip,dwPort);
	TraceLn(szBuf);
#endif

	// 有游戏服务器连入
	if ( wListenPort==m_ServerListener.GetListenPort() )
	{
		//int nBufSize=1024*1024*12;
		//pIncomingConn->SetSocketOption(SOL_SOCKET,SO_SNDBUF,(char*)&nBufSize,sizeof(int));
		//pIncomingConn->SetSocketOption(SOL_SOCKET,SO_RCVBUF,(char*)&nBufSize,sizeof(int));

		ServerUser * server = new ServerUser(pIncomingConn);
		ServerList::getInstance().AddUser(server);
		*ppHandler = server;

		char szName[64] = {0};
		sprintf_s(szName, sizeof(szName), "服务器%d",server->GetServerID());
		pIncomingConn->setName(szName);
	}
	// 有网关连入
	else if (wListenPort==m_ClientListener.GetListenPort() )
	{
		GatewayUser *pNewUser = new GatewayUser(pIncomingConn);
		GatewayList::getInstance().AddUser(pNewUser);
		*ppHandler = pNewUser;

		char szName[64] = {0};
		sprintf_s(szName, sizeof(szName), "网关用户%d",pNewUser->GetServerID());
		pIncomingConn->setName(szName);
	}
	else
	{
		assert(false);
	}
}


/////////////// EventHandler //////////////////////////////////////////////
void GlobalVoiceServer::OnEvent( HANDLE event )
{
	if ( event==m_hTimer )
	{
		// 时间轴
		m_TimerAxis.CheckTimer();

		// 执行操作代码
		if (m_dwControlCode>VS_ControlCode_None && m_dwControlCode<VS_ControlCode_Max)
		{
			DoControlCodeTask(m_dwControlCode);
			m_dwControlCode = VS_ControlCode_None;
		}
	}
	else
	{
		DispatchNetworkEvents();
	}

	// 取数据库存取结果
	if ( GameDatabaseAccessor::getInstancePtr() )
	{
		GameDatabaseAccessor::getInstancePtr()->FetchDatabaseResult();
	}
}


/** 数据库请求返回回调方法
@param   nCmdID ：命令ＩＤ
@param   nDBRetCode：数据库请求返回值，参考上面定义
@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
@param   nQueueIndex：队列定义
@param   pOutData：输出数据
@param   nOutLen：输出数据长度
@return  
@note     
@warning 此对像千万不能在数据库返回前释放，否则会非法！
@retval buffer 
*/	
void GlobalVoiceServer::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	// 出错了就报一下
	if ( nDBRetCode!=DBRET_CODE_SUCCEED )
	{
		ErrorLn(_GT("GlobalVoiceServer::OnReturn() 出错了 nCmdID=") << nCmdID << " nDBRetCode=" << nDBRetCode);
		if ( pszDBRetDesc )
		{
			ErrorLn(pszDBRetDesc);
		}

		return;
	}

	switch(nCmdID)
	{
	case VOICEDB_REQUEST_VOICEUSER_START:		// 语音服务器开启了,要清理一下上线状态等初始化操作
		{
			if (nQueueIndex==1)
			{
				m_dwDBInitState = 1;
				TraceLn(_GT("启动数据库初始化处理完成!"));
			}
			else
			{
				m_dwDBInitState = 2;
				TraceLn(_GT("关闭数据库处理完成!"));
			}
		}
		break;

	case VOICEDB_REQUEST_VOICEFANS_TOPLIST:		// 粉丝排行榜
		{
#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"GlobalVoiceServer::OnReturn() 粉丝排行榜 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			if(pOutData == NULL || nOutLen < sizeof(DB_Result_VoiceFansUser_TopList))
			{
				ErrorLn(_GT("GlobalVoiceServer::OnReturn() 粉丝排行榜 数据大小非法，nOutLen = ")<<nOutLen);
				return;	
			}

			DB_Result_VoiceFansUser_TopList * pResult = (DB_Result_VoiceFansUser_TopList *)(pOutData);

			if( nOutLen != pResult->nRecordCount * sizeof(DB_Table_VoiceFansData) + sizeof(DB_Result_VoiceFansUser_TopList) )
			{
				ErrorLn(_GT("GlobalVoiceServer::OnReturn() 粉丝排行榜 数据条数与大小不符，RecordCount=")<<pResult->nRecordCount<<",nOutLen = "<<nOutLen);
				return;
			}

			m_FansTopList.clear();

#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn("GlobalVoiceServer::OnReturn() 粉丝排行榜 RecordCount=" << pResult->nRecordCount );
#endif

			DB_Table_VoiceFansData * pInfo = (DB_Table_VoiceFansData *)(pOutData + sizeof(DB_Result_VoiceFansUser_TopList));
			for (int i=0;i<pResult->nRecordCount;i++)
			{
				SVoiceFansTopListData data;
				data.dwUDBID = pInfo->dwUDBID;									// 通行证UDBID
				data.dwUserType = pInfo->dwUserType;								// 通行证帐号类型
				data.dwRank = pInfo->nRank;									// 名次
				data.dwFansCount = pInfo->dwFansCount;								// 粉丝总数
				data.dwPDBID = pInfo->dwPDBID;									// 最近登录的角色ID
				data.dwFromWorldID = pInfo->dwFromWorldID;							// 最近登录的角色来源世界ID
				data.dwCountryID = pInfo->nBelongID;								// 最近登录的角色所属ID(国家，大陆。。)
				sstrcpyn( data.szActorName, pInfo->szActorName, sizeof(data.szActorName) );			// 最近登录的角色角色名
				m_FansTopList.push_back(data);


#ifdef VOICE_PRINT_DEBUG_INF
				char szBuf[512]={0};
				sprintf_s(szBuf, _countof(szBuf),"CVoiceClient::OnReturn 粉丝排行榜 UDBID=%d, 名字=%s, 粉丝数=%d, 名次=%d", data.dwUDBID, data.szActorName, data.dwFansCount, data.dwRank );
				TraceLn(szBuf);
#endif

				pInfo ++;
			}

			m_dwFansTopListUpdateTime = (DWORD)time(NULL);

		}
		break;

	default:
		break;
	}

}


////////////////////////// TimerHandler ///////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void GlobalVoiceServer::OnTimer(unsigned long dwTimerID)
{
	switch(dwTimerID)
	{
	case ETimerEventID_WindowClose:		// 窗口关闭
		{
			ShutDownApp();
		}
		break;

	case ETimerEventID_WaitShutDown:		// 等待数据库保存完成后关闭
		{
			OnTimerCheckShutDown();
		}
		break;

	case ETimerEventID_Keepalive:			//  检查心跳
		{
			//DWORD dwNowTimes = (DWORD)time(NULL);	// 时间
			//DWORD dwNowTicks = ::GetTickCount();	// Tick时间

		}
		break;
	case ETimerEventID_DBStart:			//  启动时数据库处理
		{
			TraceLn(_GT("启动后数据库初始化处理..."));

			// 语音服务器开启了,要清理一下上线状态等初始化操作
			DB_Param_VoiceUser_Start dbParam;
			dbParam.dwSvrWorldID = getThisGameWorldID();	// 语音服所属游戏世界ID,作来操作来源服ID
			dbParam.bIsStart = true;					// 是否启动，true为启动，false为关服
			CDBRequester::getInstance().DB_VoiceUser_Start(dbParam,static_cast<IDBRetSink *>(this) );

			// 第一次启动先读取粉丝排行榜
			DB_Param_VoiceFansUser_TopList fansPram;
			fansPram.dwSvrWorldID = getThisGameWorldID();
			CDBRequester::getInstance().DB_VoiceFansUser_TopList( fansPram, static_cast<IDBRetSink *>(this) );
		}
		break;

	case ETimerEventID_VoiceControl:	// 运行窗口控制命令
		{
			if( !m_bUpdate && m_bNewDataCome )
			{
				//ErrorLn("可以执行了，先加上锁");
				m_bUpdate = true;
			
				ExcutCommand( m_nNum1, m_nNum2, m_strTxt.c_str() );

				m_bNewDataCome = false;
				m_bUpdate = false;
				//ErrorLn("执行完了，解锁锁");
			}
		}
		break;

	case ETimerEventID_VoiceNetCount:	// 统计网络流量
		{
			m_TotalInfo.dwRecvSpeedServer = m_dwRecvSpeedServer;
			m_TotalInfo.dwSendSpeedServer = m_dwSendSpeedServer;

			m_TotalInfo.dwRecvSpeedGateway = m_dwRecvSpeedGateway;
			m_TotalInfo.dwSendSpeedGateway = m_dwSendSpeedGateway;

			m_TotalInfo.dwRecvSpeedClient = m_dwRecvSpeedClient;
			m_TotalInfo.dwSendSpeedClient = m_dwSendSpeedClient;

			if (m_dwSendSpeedClient<=0)
			{
				m_TotalInfo.dwSendAvgSpeedClient = m_dwSendSpeedClient;	// 没流量就变0
			}
			else if(m_TotalInfo.dwSendAvgSpeedClient<=0)
			{
				m_TotalInfo.dwSendAvgSpeedClient = m_dwSendSpeedClient;	// 之前为0就直接取当前
			}
			else
			{
				m_TotalInfo.dwSendAvgSpeedClient = (DWORD)(0.95*m_TotalInfo.dwSendAvgSpeedClient+0.05*m_dwSendSpeedClient);		// 客户端平均发包速度,20秒平均
			}

			DWORD dwMaxNetSpeed = VoiceServerSetting::getInstance().m_dwMaxNetSpeed;	//最大网络带宽(Mbps)
			// 大于零就要检测
			if (dwMaxNetSpeed>0)
			{
				DWORD dwNowTimes = (DWORD)time(NULL);
				static DWORD dwLastNetSpeedAlertTimes = 0;
				DWORD dwMaxNetSpeedSendClient = dwMaxNetSpeed*1024*128;
				// 超了并且冷却时间到(30秒)
				if (m_TotalInfo.dwSendAvgSpeedClient>=dwMaxNetSpeedSendClient )
				{
					if ( dwNowTimes>dwLastNetSpeedAlertTimes+60)
					{
						dwLastNetSpeedAlertTimes = dwNowTimes;
						AlertBad(MG_AlertSrcType_Network,"语音带宽占用超高! 平均:"<<m_TotalInfo.dwSendAvgSpeedClient<<",当前:"<<m_dwSendSpeedClient);

						// 已启动了就不处理
						if (!m_bAutoNetSpeed)
						{
							m_bAutoNetSpeed = true;	// 启动了自动流量控制


							DWORD dwDefaultQuality = VoiceServerSetting::getInstance().m_dwQualityType;
							// 把前50个在线的频道音质降一级
							CChannelList & channelList = CChannelList::getInstance();
							// 计算得到自动流量控制中的频道列表 dwMinOnline:最小人数要求,dwNeedMaxCount:最多取多少个频道
							channelList.GetAutoNetSpeedList(30,50);

							for(CChannelList::CHANNELSORT_LIST::iterator it = channelList.m_autoNetSpeed.begin(); it != channelList.m_autoNetSpeed.end(); ++it)
							{
								SVoiceChannelSortInfo &node = (*it);
								// 查找频道
								CChannel * pChannel = channelList.Get( node.nChannelID);
								if (pChannel!=NULL)
								{
									DWORD dwQuality = pChannel->GetBaseInfo().dwQuality;
									node.dwUserData = dwQuality;		// 保存一下原来的音质
									if (VoiceQuality_Default==dwQuality)
									{
										dwQuality = dwDefaultQuality;
									}
									if (dwQuality>VoiceQuality_Low)
									{
										pChannel->SetQuality(dwQuality-1);	// 频道音质降一级
									}
								}
							}

							AlertBad(MG_AlertSrcType_Network,"语音带宽占用超高! 启动了自动流量控制,在线排名前50的频道音质降1级,共处理:"<<channelList.m_autoNetSpeed.size());
							
						}
					}
				}
				else
				{
					// 流量下来了,并启动了自动流量控制,等流量很少到可复原的状态时,要复原
					if (m_bAutoNetSpeed)
					{
						if ( dwNowTimes>dwLastNetSpeedAlertTimes+60)
						{
							dwLastNetSpeedAlertTimes = dwNowTimes;

							DWORD dwNeedSpeed = 0;	// 需要的流量空间
							// 可用流量可支持
							DWORD nVBRTargetBitrate = VoiceServerSetting::getInstance().m_CodeInfo.Info.celt.nVBRTargetBitrate;
							DWORD dwDefaultQuality = VoiceServerSetting::getInstance().m_dwQualityType;

							// 把前50个在线的频道音质降一级
							CChannelList & channelList = CChannelList::getInstance();

							for(CChannelList::CHANNELSORT_LIST::iterator it = channelList.m_autoNetSpeed.begin(); it != channelList.m_autoNetSpeed.end(); ++it)
							{
								SVoiceChannelSortInfo &node = (*it);
								// 查找频道
								CChannel * pChannel = channelList.Get( node.nChannelID);
								if (pChannel!=NULL)
								{
									DWORD dwOnlines = pChannel->CountEx(true);	// 人数
									DWORD dwOldQuality = node.dwUserData;
									// 原来的不是默认或低
									if (dwOldQuality==VoiceQuality_Default )
									{
										dwOldQuality = dwDefaultQuality;
									}
									// 计算要求的流量
									if (dwOldQuality>VoiceQuality_Low)
									{
										if (VoiceQuality_High==dwOldQuality)	// 高语音品质
										{
											dwNeedSpeed += (nVBRTargetBitrate*dwOnlines);
										}
										else if (VoiceQuality_Normal==dwOldQuality)	// 普通语音品质
										{
											dwNeedSpeed += (nVBRTargetBitrate/2*dwOnlines);
										}
									}
								}
							}

							// 空出的流量
							DWORD dwSpaceNet = dwMaxNetSpeedSendClient-m_TotalInfo.dwSendAvgSpeedClient;
							// 如果空余比需求2倍多,可调回来
							if (dwSpaceNet*8>dwNeedSpeed*2)
							{
								m_bAutoNetSpeed = false;	// 关了自动流量控制

								for(CChannelList::CHANNELSORT_LIST::iterator it = channelList.m_autoNetSpeed.begin(); it != channelList.m_autoNetSpeed.end(); ++it)
								{
									SVoiceChannelSortInfo &node = (*it);
									// 查找频道
									CChannel * pChannel = channelList.Get( node.nChannelID);
									if (pChannel!=NULL)
									{
										pChannel->SetQuality(node.dwUserData);	// 频道音质回到原位
									}
								}

								AlertBad(MG_AlertSrcType_Network,"语音带宽占用减少! 关闭自动流量控制,在线排名前50的频道回复原音质,共处理:" <<
									channelList.m_autoNetSpeed.size()<< ",平均:"<<m_TotalInfo.dwSendAvgSpeedClient<<",当前:"<<m_dwSendSpeedClient);
								// 清空所有频道记录
								channelList.m_autoNetSpeed.clear();
							}

						}
					}
				}

			}

			m_dwRecvSpeedServer = 0;
			m_dwSendSpeedServer = 0;

			m_dwRecvSpeedGateway = 0;
			m_dwSendSpeedGateway = 0;

			m_dwRecvSpeedClient = 0;
			m_dwSendSpeedClient = 0;

		}	
		break;

	case ETimerEventID_CheckBanPDBID:		// 检测PDBID禁言
		{
			DWORD dwNow = (DWORD)time(NULL);

			for( TMAP_VoiceBanPBID::iterator it = m_mapBanPDBID.begin(); it != m_mapBanPDBID.end(); )
			{
				TMAP_VoiceChannelBanList & banList = it->second;
				for( TMAP_VoiceChannelBanList::iterator itChannel = banList.begin(); itChannel != banList.end(); )
				{
					if( dwNow >= itChannel->second.dwBanTime ) // 过了时间，要移除
					{
						CClientUser * pUser = CClientList::getInstance().GetClientByPDBID(it->first);
						if( pUser != NULL )
						{
							CRoom * pCurRoom = pUser->GetCurRoom();
							if( NULL != pCurRoom )
							{
								// 如果是当前频道
								if( itChannel->first == pCurRoom->GetChannel().GetID() )
								{
									pUser->SetFlag( VoiceUserStateFlag_Mute, false, true );
									// pUser->SendChatMsg("禁言时间已到，系统解除禁言！");
								}
							}
						}

#ifdef VOICE_PRINT_DEBUG_INF
						TraceLn(_GT("CClientUser::BanPDBID 解禁 用户=") << (it->first) );
#endif

						banList.erase(itChannel ++);
					}
					else
					{
						itChannel ++;
					}
				}

				if( banList.size() <= 0 )	// 如果没有了，要把用户结点删除
				{
					m_mapBanPDBID.erase(it ++);
				}
				else
				{
					it ++;
				}
			}
		}
		break;

	case ETimerEventID_CheckCallFansInfo:		// 检测粉丝邀请信息
		{
			DWORD dwTimeNow = (DWORD)time(NULL);
			DWORD dwTimeValid = VOICE_CALL_FANS_COOL_TIME*2;
			for( TMAP_VoiceCallFans::iterator it = m_mapCallFans.begin(); it != m_mapCallFans.end(); )
			{
				// 如果时间已经过了冷却的两倍，就移除掉
				if( dwTimeNow >= it->second.dwLastCallFansTime + dwTimeValid )
				{
					m_mapCallFans.erase(it ++);
				}
				else
				{
					it ++;
				}
			}
		}
		break;

	case ETimerEventID_GetFansTopList:		// 获取粉丝排行榜
		{
			DB_Param_VoiceFansUser_TopList fansPram;
			fansPram.dwSvrWorldID = getThisGameWorldID();
#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn( _GT("GlobalVoiceServer::OnTimer 获取粉丝排行榜 fansPram.dwSvrWorldID=") << fansPram.dwSvrWorldID );
#endif
			CDBRequester::getInstance().DB_VoiceFansUser_TopList( fansPram, static_cast<IDBRetSink *>(this) );
		}
		break;

	case ETimerEventID_CheclAD:				// 检测语音召集令信息
		{
			DWORD dwTimeNow = (DWORD)time(NULL);
			DWORD dwTimeValid = VOICE_AD_COOL_TIME*2;
			for( TMAP_VoiceAD::iterator it = m_mapAD.begin(); it != m_mapAD.end(); )
			{
				TMAP_ADInfo & ADInfo = it->second;
				for( TMAP_ADInfo::iterator itInfo = ADInfo.begin(); itInfo != ADInfo.end(); )
				{
					// 如果时间已经过了冷却的两倍，就移除掉
					if( dwTimeNow >= itInfo->second.dwLastADTime + dwTimeValid )
					{
						ADInfo.erase(itInfo ++);
					}
					else
					{
						++itInfo;
					}
				}

				if( ADInfo.size() <= 0 )
				{
					m_mapAD.erase(it++);
				}
				else
				{
					++it;
				}
			}
		}
		break;
	default:break;
	}

}


/** 启用定时器
@param   
@param   
@return  
*/ 
bool GlobalVoiceServer::SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes)
{
	TimerAxis * pTimerAxis = getTimerAxis();
	if(pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->SetTimer(timerID, interval, handler, callTimes, debugInfo);
}

/** 销毁定时器
@param   
@param   
@return  
*/
bool GlobalVoiceServer::KillTimer(DWORD timerID, ITimerHandler * handler)
{
	TimerAxis * pTimerAxis = getTimerAxis();
	if(pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(timerID, handler);
}



/// 服务器网络消息处理
void GlobalVoiceServer::onServerMessage(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	switch(actionId)
	{
	case MSG_VOICE_KEEPALIVE:		// 心跳检查消息
		{
			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
			header.DestEndPoint= (BYTE)server.m_wServerType;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_ANSWER_KEEPALIVE;	//桥服务器回应心跳检查
			obuf obufData;
			obufData.push_back(&header, sizeof(SGameMsgHead));
			server.SendData(obufData.data(),obufData.size());
		}
		break;
	case MSG_VOICE_HANDSHAKE:		// 握手消息
		{
			OnMsgServerHandshake(server,actionId,head,data, len);
		}
		break;
	case MSG_VOICE_SENDDATA:		// 请求桥服务器网关服务发送数据给各软件服务器消息
		{
			OnMsgServerSendData(server,actionId,head,data, len);
		}
		break;
	case MSG_VOICE_BROADCASTDATA:		// 请求桥服务器网关服务广播数据给各软件服务器
		{
			OnMsgServerBroadcastData(server,actionId,head,data, len);
		}
		break;

	case MSG_VOICE_LOGIN:		// 社会服发送语音客户端登录信息
		{
			OnMsgSocialServerLogin( server, actionId, head, data, len );
		}
		break;

	case MSG_VOICE_USERINFO_UPDATE:		// 游戏服更新用户信息
		{
            OnMsgUserInfoUpdate(server, actionId, head, data, len);
		}
		break;

	case MSG_VOICE_UBINFO_UPDATE:  // 社会服更新用户统战信息(确切说应该是社会服转发来的)
		{
		}
		break;

	case MSG_VOICE_CHATINVITE:		// 邀请私聊
		{
			OnMsgSocialServerChatInvite( server, actionId, head, data, len );
		}
		break;

	case MSG_VOICE_ADDVOICEBLUEDIAMOND:	// 添加蓝钻
		{
			OnMsgServerAddVoiceBlueDiamond( server, actionId, head, data, len );
		}
		break;

	case MSG_VOICE_BROADCASTDATATOCLIENT:	// 服务器请求广播数据到客户端
		{
			OnMsgServerBroadcastDataToClient( server, actionId, head, data, len );
		}
		break;
	
	case MSG_VOICE_REQ_ROOM_ACTOR_LIST:    // 请求房间人员列表
		{
		}
		break;

	case MSG_VOICE_UB_SET_TEMP_RIGHT: // 请求给统战临时权限
		{
		}
		break;

	case MSG_VOICE_SOCIAL_NOTIFY_USER_LOGOUT: // 玩家在社会服下线
		{
			OnMsgSocialNotifyActorLogout(server, actionId, head, data, len);
		}
		break;
	case ENUM_MSG_VOICE_ALL_WORLD_BROADCAST:
		{
			if(data == NULL || len < sizeof(BYTE) + 2 * sizeof(WORD))
			{
				ErrorLn("ENUM_MSG_VOICE_ALL_WORLD_BROADCAST 包格式错误");
				return;
			}

			BYTE byFinalSrvType = *(BYTE*)data;
			WORD wFinalModule = *(WORD*)((BYTE*)data + 1);
			WORD wFinalAction = *((WORD*)((BYTE*)data + 1) + 1);

			void* pRealData = (BYTE*)data + sizeof(BYTE) + sizeof(WORD) * 2;
			int nRealLen = len - sizeof(BYTE) - sizeof(WORD) * 2;
		
			ServerList::SERVER_LIST& refList = ServerList::getInstance().m_list;
			for(ServerList::SERVER_LIST::iterator iter = refList.begin(); iter != refList.end(); ++iter)
			{
				ServerUser* pServer = iter->second;
				if(pServer == NULL || pServer->GetServerType() != byFinalSrvType)
				{
					continue;
				}
				
				obuf oBuf;
				SGameMsgHead msgHead;
				msgHead.SrcEndPoint = MSG_ENDPOINT_VOICE;
				msgHead.DestEndPoint = byFinalSrvType;
				msgHead.byKeyModule = wFinalModule;
				msgHead.byKeyAction = wFinalAction;

				oBuf.push_back(&msgHead, sizeof(SGameMsgHead));
				oBuf.push_back(pRealData, nRealLen);
				pServer->SendData(oBuf.data(), oBuf.size());
			}
		}
		break;
	default:
		{
			ErrorLn(_GT("尚有一个游戏服务器消息未处理，actionId = ")<<actionId<<_GT(",服务器类型:")<<GetServerTypeString(server.m_wServerType));
		}
		break;
	}
}


/// 语音网关消息处理
void GlobalVoiceServer::onGatewayMessage(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	switch(actionId)
	{
	case MSG_VOICE_KEEPALIVE:		// 心跳检查消息
		{
			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
			header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_ANSWER_KEEPALIVE;	//桥服务器回应心跳检查
			obuf obufData;
			obufData.push_back(&header, sizeof(SGameMsgHead));
			server.SendData(obufData.data(),obufData.size());
		}
		break;
	case MSG_VOICE_HANDSHAKE:		// 握手消息
		{
			OnMsgGatewayHandshake(server,actionId,head,data, len);
		}
		break;
	case MSG_VOICE_SENDDATA:		// 语音网关转发而来的客户端消息
		{
			OnMsgGatewaySendData(server,actionId,head,data, len);
		}
		break;
	case MSG_VOICE_CLIENTLIST_UPDATE:	// 语音网关同步客户端信息到语音服务器
		{
			OnMsgGatewayClientListUpdate(server,actionId,head,data, len);
		}
		break;
	case MSG_VOICE_PERFORMANCE:	// 请求性能检测消息包
		{
			OnMsgGatewayPerformance(server,actionId,head,data, len);
		}
		break;
	default:
		{
			ErrorLn(_GT("尚有一个语音网关服务器消息未处理，actionId = ")<<actionId<<_GT(",网关ID:")<<server.GetGatewayID());
		}
		break;
	}
}


/////////////客户端网络消息处理 /////////////////////////////////////////////////////////////////
void GlobalVoiceServer::onClientMessage(GatewayUser &server, CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	switch(actionId)
	{
	case MSG_VOICE_DATA_SUBMSG:
		{
			OnMsgClientVoiceSubMsg(client, actionId, head, data, len);
		}
		break;

	case MSG_VOICE_LOGIN:		// 握手消息	压力测试用
		{
			OnMsgClientLogin(client, actionId, head, data, len);
		}
		break;

	case MSG_VOICE_VOICEDATA:	// 客户端来的语音数据
		{
			OnMsgClientVoiceData( client, actionId, head, data, len );
		}
		break;

	default:
		{
			ErrorLn(_GT("尚有一个客户端消息未处理，actionId = ")<<actionId << ", dwClientID=" << client.GetClientID());
		}
		break;
	}

}



// 发送数据给指定服务器
bool GlobalVoiceServer::SendDataToServer(DWORD dwServerID,LPVOID pData,DWORD dwDataLen)
{
	ServerUser * pServerUser = ServerList::getInstance().GetUser(dwServerID);
	if (pServerUser==NULL)
	{
		return false;
	}
	return pServerUser->SendData((const char *)pData,dwDataLen);
}

// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
bool GlobalVoiceServer::BroadcastToServer(DWORD * pServerArray,WORD wServerNum,LPVOID pData,DWORD dwDataLen)
{

	if (pServerArray==NULL || wServerNum==0)
	{
		// 为空广播所有
		if (pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM)
		{
			for(ServerList::SERVER_LIST::iterator iter = ServerList::getInstance().m_list.begin(); iter!= ServerList::getInstance().m_list.end();++iter)
			{	
				if (iter->second)
				{
					iter->second->SendData((const char *)pData,dwDataLen);
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		for (WORD i=0;i<wServerNum;i++)
		{
			ServerUser * pServerUser = ServerList::getInstance().GetUser(pServerArray[i]);
			if (pServerUser==NULL)
			{
				continue;
			}
			pServerUser->SendData((const char *)pData,dwDataLen);
		}
	}
	return true;
}



// 发数据给指定语音网关 dwGatewayID:语音网关ID 
bool GlobalVoiceServer::SendDataToGateway(DWORD dwGatewayID,const char * pData,DWORD dwLen,bool bZeroBroadcast)
{
	GatewayUser *pGatewayUser = GatewayList::getInstance().GetUserByGatewayID(dwGatewayID);
	if( NULL == pGatewayUser )
	{
		return false;
	}

	return pGatewayUser->SendData( pData, dwLen );
}

// 广播数据给指定列表语音网关 pServerArray:语音网关ID列表,wClientNum:列表个数 pServerArray==NULL && wGatewayNum==VOICE_BROADCASTALL_NUM 广播所有
bool GlobalVoiceServer::BroadcastDataToGateway(DWORD * pGatewayArray,WORD wGatewayNum,const char * pData,DWORD dwLen)
{
	if (pGatewayArray==NULL || wGatewayNum==0)
	{
		// 为空广播所有
		if (pGatewayArray==NULL && wGatewayNum==VOICE_BROADCASTALL_NUM)
		{
			for(GatewayList::GATEWAY_LIST::iterator iter = GatewayList::getInstance().m_list.begin(); iter!= GatewayList::getInstance().m_list.end();++iter)
			{	
				if (iter->second)
				{
					iter->second->SendData((const char *)pData,dwLen);
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		for (WORD i=0;i<wGatewayNum;i++)
		{
			GatewayUser * pGatewayUser = GatewayList::getInstance().GetUserByGatewayID(pGatewayArray[i]);
			if (pGatewayUser==NULL)
			{
				continue;
			}

			pGatewayUser->SendData((const char *)pData,dwLen);
		}
	}

	return true;
}



// 发数据给指定游戏客户端 dwClientID:客户端ID
bool GlobalVoiceServer::SendDataToClient(DWORD dwClientID,const char * pData,DWORD dwLen, BYTE nSendChennel )
{
	DWORD dwGatewayID = ((VClientIDInfo *)&dwClientID)->dwGatewayID;
	GatewayUser *pGatewayUser = GatewayList::getInstance().GetUserByGatewayID(dwGatewayID);
	if( NULL == pGatewayUser )
	{
		return false;
	}
	
	return pGatewayUser->SendDataToClient(dwClientID, pData, dwLen, nSendChennel );
}

// 广播数据给指定列表游戏客户端 pServerArray:客户端ID列表,wClientNum:列表个数, dwBanClientID 要排除的
bool GlobalVoiceServer::BroadcastDataToClient(DWORD * pClientArray,DWORD wClientNum,const char * pData,DWORD dwLen,DWORD dwBanClientID, BYTE nSendChennel)
{
	if (NULL==pClientArray || 0==wClientNum || NULL==pData || 0==dwLen)
	{
		return false;
	}

	memset(&m_SendCountsArray,0,sizeof(m_SendCountsArray));

	// 只有排除的才判,提高点效率
	if(dwBanClientID>0)
	{
		// 分开存客户端ID
		for ( DWORD i=0;i<wClientNum;i++ )
		{
			if(dwBanClientID==pClientArray[i])
			{
				continue;
			}
			VClientIDInfo * clientInfo = (VClientIDInfo *)&pClientArray[i];
			if (clientInfo->dwGatewayID >= VOICE_MAX_GATEWAYID)
			{
				continue;
			}

			DWORD &dwTypeIndex = m_SendCountsArray[clientInfo->dwGatewayID];
			// 防越界
			if (dwTypeIndex>=VOICE_MAX_CLIENT_COUNT)
			{
				continue;
			}
			m_SendClientArray[clientInfo->dwGatewayID][dwTypeIndex] = pClientArray[i];
			dwTypeIndex++;
		}
	}
	else
	{
		// 分开存客户端ID
		for ( DWORD i=0;i<wClientNum;i++ )
		{
			VClientIDInfo * clientInfo = (VClientIDInfo *)&pClientArray[i];
			if (clientInfo->dwGatewayID >= VOICE_MAX_GATEWAYID)
			{
				continue;
			}

			DWORD &dwTypeIndex = m_SendCountsArray[clientInfo->dwGatewayID];
			// 防越界
			if (dwTypeIndex>=VOICE_MAX_CLIENT_COUNT)
			{
				continue;
			}
			m_SendClientArray[clientInfo->dwGatewayID][dwTypeIndex] = pClientArray[i];
			dwTypeIndex++;
		}
	}

	GatewayList &gatewayList = GatewayList::getInstance();

	// 处理所有网关ID
	for (DWORD j=0;j<VOICE_MAX_GATEWAYID;j++)
	{
		// 没有可发的
		DWORD dwSendCounts = m_SendCountsArray[j];
		if (0==dwSendCounts)
		{
			continue;
		}

		GatewayUser *pGatewayUser = gatewayList.GetUserByGatewayID(j);

		if (0 == pGatewayUser)
		{
			continue;
		}

		pGatewayUser->BroadcastDataToClient(m_SendClientArray[j],(WORD)dwSendCounts,pData,dwLen, nSendChennel);
	}

	return true;

}



// 服务器断线
void GlobalVoiceServer::OnServerUserLeave(ServerUser *pServer)
{
	if (NULL==pServer)
	{
		return;
	}
	// 向其它服务器更新 删除
	UpdateServerList(0,VS_SvrListUpdateType_Del,pServer->GetServerID());

	// 处理 游戏世界ID分类列表
	TMAP_VoiceWorldIDInfo::iterator iter1 = m_mapWorldList.find(pServer->m_dwWorldID);
	if ( iter1!=m_mapWorldList.end())
	{
		// 合成ID
		WORD wTypeSubData[2];
		wTypeSubData[0] = pServer->GetServerType();
		wTypeSubData[1] = pServer->m_wSubID;
		DWORD dwTypeSubID = 0;
		memcpy(&dwTypeSubID,wTypeSubData,sizeof(DWORD));

		TMAP_VoiceID2IDInfo::iterator iter2 = iter1->second.mapList.find(dwTypeSubID);		// 服务器ID对应列表
		if (iter2!=iter1->second.mapList.end())
		{
			if (pServer->GetServerID()==iter2->second)
			{
				iter1->second.mapList.erase(iter2);
			}
			else
			{
				// 不是自已的连接,网络有问题,要全部更新服务器列表
				// 向所有的服务器更新所有列表
				UpdateServerList(0,VS_SvrListUpdateType_All,0);
			}
		}

	}
}


// 网关断线
void GlobalVoiceServer::OnGatewayUserLeave(GatewayUser *pServer, DWORD dwReason )
{
	// todo

	// 设置该网关上所有用户的标记为离线
	CClientList::getInstance().OnGatewayLeave(pServer->GetGatewayID());
}


/// purpose: 客户端连入
void GlobalVoiceServer::OnClientUserEnter(CClientUser *pUser, DWORD dwReason )
{
	if( NULL == pUser )
	{
		return ;
	}

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_HANDSHAKE_RESPONSE;


	// 语音服务器回应语音客户端握手消息数据结构
	SMsgVoiceHandshakeResponse_VC sendData;
	sendData.dwClientID = pUser->GetClientID();								// 客户端ID
	sendData.dwVoiceQuality = (BYTE)(VoiceServerSetting::getInstance().m_dwQualityType);	// 频道默认语音品质类型 EMVoiceQualityType
	sendData.dwServerTime = (DWORD)time(NULL);
	sendData.codecInfo = VoiceServerSetting::getInstance().m_CodeInfo;		// 压缩参数
	sendData.nKeyLen = pUser->m_nKeyLen;									// 动态密码长度

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceHandshakeResponse_VC));
	obufData.push_back(pUser->m_pKeyData, pUser->m_nKeyLen);

	SendDataToClient( sendData.dwClientID, obufData.data(),obufData.size() );

	pUser->SetLogin(false);
	pUser->SetOnline(true);
	pUser->SetState(VoiceUserState_Online);
}

/// purpose: 客户端断线
void GlobalVoiceServer::OnClientUserLeave(CClientUser *pUser, DWORD dwReason )
{
	if (NULL==pUser)
	{
		return;
	}

	// 有私聊的，要关闭私聊
	pUser->LeaveChat();

	// 有频道有退出频道
	pUser->EnterRoom(0, NULL);

	pUser->SetOnline(false);
	pUser->SetState(VoiceUserState_Offline);
	pUser->DBLogout();

	//TraceLn( "用户离开：" << pUser->ToString() );
}


// 客户端登录
void GlobalVoiceServer::OnClientUserLogin(CClientUser *pUser, DWORD dwReason )
{

}

// 客户端登出
void GlobalVoiceServer::OnClientUserLogout(CClientUser * pUser, DWORD dwReason)
{

}

// 频道创建
void GlobalVoiceServer::OnChannelCreate( CChannel *pChannel, DWORD dwReason )
{

}

// 频道销毁
void GlobalVoiceServer::OnChannelDestroy( CChannel *pChannel, DWORD dwReason )
{

}

// 用户进入房间
void GlobalVoiceServer::OnUserEnterRoom( CRoom *pRoom, CClientUser* pUser, DWORD dwReason )
{

}

// 用户退出房间
void GlobalVoiceServer::OnUserLeaveRoom( CRoom *pRoom, CClientUser* pUser, DWORD dwReason )
{

}



// 发送语音模块数据给指定游戏客户端 dwClientID:客户端ID 
bool GlobalVoiceServer::SendVoiceDataToClient( DWORD dwClientID, WORD byKeyAction, DWORD dwMsgCode, DWORD dwUserData1,
											  DWORD dwUserData2, DWORD dwUserData3, const char * pData, DWORD dwLen, BYTE nSendChennel )
{
	return BroadcastVoiceDataToClient( &dwClientID, 1, byKeyAction, dwMsgCode, dwUserData1, dwUserData2, dwUserData3, pData, dwLen, nSendChennel);
}

// 广播语音模块数据到客户端 pClientArray: 要发送的客户端ID数组,wClientNum  : 客户端数目
bool GlobalVoiceServer::BroadcastVoiceDataToClient(DWORD * pClientArray,DWORD wClientNum,WORD byKeyAction,DWORD dwMsgCode,DWORD dwUserData1,
												   DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen, BYTE nSendChennel )
{
	DWORD dwMaxSendLens = dwLen+sizeof(SGameMsgHead)+sizeof(SMsgVoiceDataSubMsg)+128;	
	BYTE szDataBuf[32768];			// 32K的临时内存
	BYTE * pBufData = szDataBuf;	// 默认用32K
	bool bIsNewMemory = false;		// 是否是new的内存，用来用完释放
	if (dwMaxSendLens>=32768)		// 如果大于32K,就New内存
	{
		bIsNewMemory = true;
		// 使用外部new的内存发数据,用完释放
		pBufData = (BYTE*)new char[dwMaxSendLens];
	}
	ofixbuf obufData((char *)pBufData, dwMaxSendLens);

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = byKeyAction;			// 语音服务器信息子消息

	SMsgVoiceDataSubMsg outData;
	outData.dwMsgCode	= dwMsgCode;						// 子消息代码  EMVoiceDataSubMsgCode
	outData.dwUserData1	= dwUserData1;						// 用户数据1
	outData.dwUserData2	= dwUserData2;						// 用户数据2
	outData.dwUserData3	= dwUserData3;						// 用户数据3

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&outData, sizeof(SMsgVoiceDataSubMsg));
	if (pData!=NULL && dwLen>0)
	{
		obufData.push_back(pData, dwLen);
	}

	if( 1 == wClientNum )
	{
		SendDataToClient( pClientArray[0], obufData.data(),obufData.size(), nSendChennel );
	}
	else
	{
		BroadcastDataToClient( pClientArray, wClientNum, obufData.data(),obufData.size(), nSendChennel );
	}
	
	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

	return true;
}


// 发送聊天信息到客户端
bool GlobalVoiceServer::BroadcastChatMsgToClient(DWORD * pClientArray,DWORD wClientNum, WORD nPos, WORD nTipsType, const char * pMsg, DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3 )
{
	if( NULL == pMsg || NULL == pClientArray || wClientNum <= 0 )
	{
		return false;
	}

#ifdef RKT_UTF8
    string strText(a2utf8(pMsg));
#else
    string strText(pMsg);
#endif
	obuf obufData;
	SVoiceSubMsgSendChatMsg_VC sendData;
	sendData.nPos = nPos;				// 提示信息类型
	sendData.nTipsType = nTipsType;		// 系统信息显示位置
	sendData.dwTextLen = strlen(strText.c_str())+1;
	sendData.bTrace = dwUserData1 > 0 ? true : false;	// true 只trace到LOG中

	obufData.push_back( &sendData, sizeof(sendData) );
	obufData.push_back( strText.c_str(), sendData.dwTextLen );

	return BroadcastVoiceDataToClient( pClientArray, wClientNum, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_SendChatMsg, dwUserData1, dwUserData2,dwUserData3, obufData.data(), obufData.size() );
}





// 发送数据到指定的游戏服务器
bool GlobalVoiceServer::SendDataToGameServer(DWORD dwServerID, BYTE nDestEndPoint,WORD byKeyModule, WORD byKeyAction,DWORD dwMsgCode,
											  DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen)
{
	return BroadcastDataToGameServer( &dwServerID, 1, nDestEndPoint, byKeyModule, byKeyAction, dwMsgCode, dwUserData1, dwUserData2, dwUserData3, pData, dwLen);
}

// 广播数据到指定的游戏服务器
bool GlobalVoiceServer::BroadcastDataToGameServer(DWORD * pServerArray, WORD wServerNum,BYTE nDestEndPoint,WORD byKeyModule, WORD byKeyAction,DWORD dwMsgCode,
												   DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen)
{
	DWORD dwMaxSendLens = dwLen+sizeof(SGameMsgHead)+sizeof(SMsgVoiceDataSubMsg)+128;	
	BYTE szDataBuf[32768];			// 32K的临时内存
	BYTE * pBufData = szDataBuf;	// 默认用32K
	bool bIsNewMemory = false;		// 是否是new的内存，用来用完释放
	if (dwMaxSendLens>=32768)		// 如果大于32K,就New内存
	{
		bIsNewMemory = true;
		// 使用外部new的内存发数据,用完释放
		pBufData = (BYTE*)new char[dwMaxSendLens];
	}
	ofixbuf obufData((char *)pBufData, dwMaxSendLens);

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= nDestEndPoint;
	header.byKeyModule  = byKeyModule;
	header.byKeyAction  = byKeyAction;			// 语音服务器信息子消息

	SMsgVoiceDataSubMsg outData;
	outData.dwMsgCode	= dwMsgCode;						// 子消息代码  EMVoiceDataSubMsgCode
	outData.dwUserData1	= dwUserData1;						// 用户数据1
	outData.dwUserData2	= dwUserData2;						// 用户数据2
	outData.dwUserData3	= dwUserData3;						// 用户数据3

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&outData, sizeof(SMsgVoiceDataSubMsg));
	if (pData!=NULL && dwLen>0)
	{
		obufData.push_back(pData, dwLen);
	}

	BroadcastToServer( pServerArray, wServerNum, obufData.data(),obufData.size() );

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

	return true;
}



// 发送语音模块数据给指定语音网关
bool GlobalVoiceServer::SendVoiceDataToGateway(DWORD dwGatewayID,WORD byKeyAction,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen)
{
	DWORD dwMaxSendLens = dwLen+sizeof(SGameMsgHead)+sizeof(SMsgVoiceDataSubMsg)+128;	
	BYTE szDataBuf[32768];			// 32K的临时内存
	BYTE * pBufData = szDataBuf;	// 默认用32K
	bool bIsNewMemory = false;		// 是否是new的内存，用来用完释放
	if (dwMaxSendLens>=32768)		// 如果大于32K,就New内存
	{
		bIsNewMemory = true;
		// 使用外部new的内存发数据,用完释放
		pBufData = (BYTE*)new char[dwMaxSendLens];
	}
	ofixbuf obufData((char *)pBufData, dwMaxSendLens);

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = byKeyAction;			// 语音服务器信息子消息

	SMsgVoiceDataSubMsg outData;
	outData.dwMsgCode	= dwMsgCode;						// 子消息代码  EMVoiceDataSubMsgCode
	outData.dwUserData1	= dwUserData1;						// 用户数据1
	outData.dwUserData2	= dwUserData2;						// 用户数据2
	outData.dwUserData3	= dwUserData3;						// 用户数据3

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&outData, sizeof(SMsgVoiceDataSubMsg));
	if (pData!=NULL && dwLen>0)
	{
		obufData.push_back(pData, dwLen);
	}

	bool result = SendDataToGateway( dwGatewayID, obufData.data(), obufData.size() );

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

	return result;
}





////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////

/**  取得服务器ID列表
@param   DWORD* pServerArray	: 保存找到的ID数组
@param   WORD dwArrayLen		: 数组长度
@param   WORD &dwGetCounts		: 找到的个数
@param   DWORD dwWorldID		: 游戏世界ID，0为全部
@param   WORD wServerType		: 服务器类型,0为全部
@return  找到的个数
*/
DWORD GlobalVoiceServer::GetGameServerIDList(DWORD * pServerArray,DWORD dwArrayLen,DWORD &dwGetCounts,DWORD dwWorldID,WORD wServerType)
{
	if (pServerArray==NULL || dwArrayLen==0)
	{
		dwGetCounts = 0;
		return 0;
	}
	DWORD dwFindCounts = 0;
	DWORD * pServerID = pServerArray;
	// 游戏世界ID为0用遍历
	if (0==dwWorldID)
	{
		ServerList::SERVER_LIST & mList = ServerList::getInstance().m_list;
		for( ServerList::SERVER_LIST::iterator iter = mList.begin(); iter!= mList.end();++iter)
		{
			if (NULL==iter->second)
			{
				continue;
			}
			if ( (0==dwWorldID|| dwWorldID==iter->second->m_dwWorldID) && (0==wServerType|| wServerType==iter->second->m_wServerType))
			{
				if (dwFindCounts>=dwArrayLen)
				{
					break;
				}
				(*pServerID) = iter->second->GetServerID();
				pServerID++;
				dwFindCounts++;
			}
		}
	} 
	else
	{
		// 游戏世界ID分类列表
		TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(dwWorldID);
		if ( iter!=m_mapWorldList.end())
		{
			// 合成ID
			WORD wTypeSubData[2];
			DWORD dwTypeSubID = 0;
			for(TMAP_VoiceID2IDInfo::iterator iter1 = iter->second.mapList.begin(); iter1!= iter->second.mapList.end();++iter1)
			{
				dwTypeSubID = iter1->first;
				memcpy(wTypeSubData,&dwTypeSubID,sizeof(DWORD));
				if (0==wServerType || wTypeSubData[0]==wServerType)
				{
					if (dwFindCounts>=dwArrayLen)
					{
						break;
					}
					(*pServerID) = iter1->second;
					pServerID++;
					dwFindCounts++;
				}
			}
		}
	}
	dwGetCounts = dwFindCounts;
	return dwFindCounts;
}


/**  取得指定游戏世界的指定类型的服务器ID
@param   DWORD dwWorldID		: 游戏世界ID
@param   WORD wServerType		: 服务器类型
@param   WORD wSubID			: 服务器序号
@return  找到的服务器ID,为0表示没找到
*/
DWORD GlobalVoiceServer::GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID)
{
	// 处理 游戏世界ID分类列表
	TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(dwWorldID);
	if ( iter==m_mapWorldList.end())
	{
		return 0;
	}

	// 合成ID
	WORD wTypeSubData[2];
	wTypeSubData[0] = wServerType;
	wTypeSubData[1] = wSubID;
	DWORD dwTypeSubID = 0;
	memcpy(&dwTypeSubID,wTypeSubData,sizeof(DWORD));

	TMAP_VoiceID2IDInfo::iterator iter1 = iter->second.mapList.find(dwTypeSubID);
	if ( iter1==iter->second.mapList.end())
	{
		return 0;
	}
	return iter1->second;
}

/** 列出所有服务器信息
@param   DWORD dwWorldID		: 游戏世界ID
@param   WORD wServerType		: 服务器类型
@return 
*/
void GlobalVoiceServer::PrintGameServerListInfo(DWORD dwWorldID,WORD wServerType)
{
	ServerList::SERVER_LIST & mList = ServerList::getInstance().m_list;
	for( ServerList::SERVER_LIST::iterator iter = mList.begin(); iter!= mList.end();++iter)
	{
		if ( (0==dwWorldID|| dwWorldID==iter->second->m_dwWorldID) && (0==wServerType|| wServerType==iter->second->m_wServerType))
		{
			TraceLn("ID="<<iter->second->GetServerID()<<_GT(",游戏世界ID=")<<iter->second->m_dwWorldID<<_GT(",服务器类型=")<<GetServerTypeString(iter->second->m_wServerType)<<(",序号=")<<iter->second->m_wSubID);
		}
	}
}

////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

/**  取得游戏世界ID列表
@param   DWORD* pWorldArray	: 保存找到的ID数组
@param   WORD dwArrayLen		: 数组长度
@param   WORD &dwGetCounts		: 找到的个数
@return  找到的个数
*/
DWORD GlobalVoiceServer::GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts)
{
	if (pWorldArray==NULL || dwArrayLen==0)
	{
		dwGetCounts = 0;
		return 0;
	}
	DWORD dwFindCounts = 0;
	DWORD * pWorldID = pWorldArray;
	// 游戏世界ID分类列表,以dwWorldID为关键字
	for(TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.begin(); iter!= m_mapWorldList.end();++iter)
	{
		(*pWorldID) = iter->first;
		pWorldID++;
		dwFindCounts++;
	}
	dwGetCounts = dwFindCounts;
	return dwFindCounts;
}

/**  取得指定游戏世界信息
@param   DWORD dwWorldID		: 游戏世界ID
@return  找到的服务器ID,为0表示没找到
*/
IVoiceServerWorldInfo * GlobalVoiceServer::GetGameWorldInfo(DWORD dwWorldID)
{
	// 游戏世界ID分类列表
	TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(dwWorldID);
	if ( iter==m_mapWorldList.end())
	{
		return NULL;
	}
	return &(iter->second);
}

/** 列出所有游戏世界信息
@return 
*/
void GlobalVoiceServer::PrintGameWorldListInfo()
{
	// 游戏世界ID分类列表,以dwWorldID为关键字
	for(TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.begin(); iter!= m_mapWorldList.end();++iter)
	{
		TraceLn("游戏世界ID="<<iter->second.dwWorldID<<",名称="<<iter->second.strWorldName.c_str());
	}
}


//////////////////////////////////////////////////////////////////////////
// 命令操作
void GlobalVoiceServer::ExcutCommand( int nNum1, int nNum2, const char * szTxt )
{
	switch(nNum1)
	{
	case VoiceCommand_ExportUserList:		// 导出用户列表
		CClientList::getInstance().SaveUserListToCSV();
		break;

	case VoiceCommand_ExportChannelList:	// 导出频道列表
		CChannelList::getInstance().SaveUserListToCSV();
		break;

	case VoiceCommand_ChangeMaxConnection:	// 修改最大连接数
		{
			if( nNum2 < 0 )
			{
				nNum2 = 0;
			}

			m_dwMaxConnection = nNum2;

			ErrorLn( _GT("修改最大连接数为：") << m_dwMaxConnection );
		}
		break;
	case VoiceCommand_NetSpeedMax:	// 设置最大网络带宽(Mbps)
		{
			if( nNum2 < 0 )
			{
				nNum2 = 0;
			}

			VoiceServerSetting::getInstance().m_dwMaxNetSpeed = (DWORD)nNum2;

			ErrorLn( _GT("设置最大网络带宽：") << nNum2<<"Mbps" );
		}
		break;
	}
}


// 游戏服务器握手消息
void GlobalVoiceServer::OnMsgServerHandshake(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len != sizeof(SMsgVoiceHandshake_SB))
	{
		return;
	}

	SMsgVoiceHandshake_SB * pMsg = (SMsgVoiceHandshake_SB *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "GlobalVoiceServer::OnMsgServerHandshake() 服务器握手消息:游戏世界ID=%d,服务器类型=%d,序号=%d,服务器名=%s",
		pMsg->dwWorldID,pMsg->nServerType,pMsg->wSubID,pMsg->szName);
	TraceLn(szBuf);
#endif

	// 更新服务器信息
	server.m_dwWorldID		= pMsg->dwWorldID;
	server.m_wServerType	= pMsg->nServerType;
	server.m_wSubID			= pMsg->wSubID;
	server.m_bIsPublic		= pMsg->bIsPublic;
	sstrcpyn(server.m_szName, pMsg->szName,sizeof(server.m_szName));
	sstrcpyn(server.m_szWorldName, pMsg->szWorldName, sizeof(server.m_szWorldName));

	bool bCheckFail = false;
	if (head->DestEndPoint !=MSG_ENDPOINT_VOICE)
	{
		WarningLn(_GT("服务器握手失败! ")<<a2utf8(server.ToString().c_str()));
		bCheckFail = true;
	}
	else if(0==pMsg->dwWorldID)
	{
		ErrorLn(_GT("服务器握手失败! 服务器游戏世界ID[") <<pMsg->dwWorldID<<_GT("]为零,")<<server.ToString().data());
		AlertError(MG_AlertSrcType_Network,_GT("服务器握手失败! 服务器游戏世界ID[")<<pMsg->dwWorldID<<_GT("]为零,")<<server.ToString().data());
		bCheckFail = true;
	}

	if (bCheckFail)
	{
		server.Release();
		return;
	}

	// 处理 游戏世界ID分类列表
	TMAP_VoiceWorldIDInfo::iterator iter1 = m_mapWorldList.find(server.m_dwWorldID);
	if ( iter1==m_mapWorldList.end())
	{
		SVoiceWorldNode addNode;
		addNode.dwWorldID = server.m_dwWorldID;
		addNode.strWorldName = server.m_szWorldName;
		// 登录管理员帐号信息,以dwID为关键字,pair<TMAP_LoginUserInfo::iterator,bool> retPair=
		pair<TMAP_VoiceWorldIDInfo::iterator,bool> retPair = m_mapWorldList.insert(TMAP_VoiceWorldIDInfo::value_type(server.m_dwWorldID,addNode));
		iter1 = retPair.first;

		// 向其它服务器更新新加入游戏世界
		UpdateWorldList(0,VS_SvrListUpdateType_Add,server.m_dwWorldID);
	}
	// 合成ID
	WORD wTypeSubData[2];
	wTypeSubData[0] = server.m_wServerType;
	wTypeSubData[1] = server.m_wSubID;
	DWORD dwTypeSubID = 0;
	memcpy(&dwTypeSubID,wTypeSubData,sizeof(DWORD));

	iter1->second.mapList[dwTypeSubID] = server.GetServerID();

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= (BYTE)server.m_wServerType;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_HANDSHAKE_RESPONSE;

	// 桥服务器回应握手消息数据结构
	SMsgVoiceHandshakeResponse sendData;
	sendData.dwID = server.GetServerID();		// 连接ID
	sendData.dwServerTime = (DWORD)time(NULL);	// 服务器时间

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceHandshakeResponse));

	server.SendData(obufData.data(),obufData.size());

	TraceLn(_GT("回应服务器连接器握手! ")<<server.ToString().data());

	// 服务务器更新和相关处理
	m_TotalInfo.dwServers	= ServerList::getInstance().Count();	// 在线服务器数
	m_TotalInfo.dwWorldCounts = m_mapWorldList.size();		// 游戏世界数

	// 向新来的服务器更新所有游戏世界列表
	UpdateWorldList(server.GetServerID(),VS_SvrListUpdateType_All,0);

	// 向新来的服务器更新所有列表
	UpdateServerList(server.GetServerID(),VS_SvrListUpdateType_All,0);

	// 向其它服务器更新新加入
	UpdateServerList(0,VS_SvrListUpdateType_Add,server.GetServerID());

	// 如果是社会服，更新所有的在线用户
	if( server.m_wServerType == MSG_ENDPOINT_SOCIAL )
	{
		// 服务器连上来时要首先更新该游戏世界下所有用户的服务器ID
		CClientList::getInstance().UpdateUserServerInfo( server.GetServerID(), server.m_dwWorldID );
	}
}

// 请求桥服务器网关服务发送数据给各软件服务器消息
void GlobalVoiceServer::OnMsgServerSendData(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len <= sizeof(SMsgVoiceSendData))
	{
		return;
	}

	SMsgVoiceSendData * pMsg = (SMsgVoiceSendData *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalVoiceServer::OnMsgServerSendData() 发送数据给各软件服务器消息:软件服务器ID=%d,数据长度=%d",
		pMsg->dwServerID,pMsg->nDataLens);
	//TraceLn(szBuf);
#endif

	DWORD dwSendDataLens = (DWORD)(len-sizeof(SMsgVoiceSendData) );

	if (dwSendDataLens!=pMsg->nDataLens)
	{
		ErrorLn(_GT("请求桥服务器网关服务发送数据给各软件服务器消息长度错误! 实际长度=")<<dwSendDataLens<<_GT(",指示长度=")<<pMsg->nDataLens);
	}

	// 转发数据,为了提高效率,用修改头数据方法,必须结构体大小一样
	Assert(sizeof(SMsgVoiceSendData)==sizeof(SMsgVoiceHead));

	// 保留转发参数,下面的代码不要用 pMsg了,已被覆盖了
	SMsgVoiceSendData msgHead = (*pMsg);

	head->SrcEndPoint = MSG_ENDPOINT_VOICE;
	head->DestEndPoint= MSG_ENDPOINT_VOICE;
	head->byKeyModule  = MSG_MODULEID_VOICE;
	head->byKeyAction  = MSG_VOICE_SENDDATA;

	// 通过跨区桥服务器中转消息必须加入此消息体
	SMsgVoiceHead * pBhead =(SMsgVoiceHead *)data;
	pBhead->dwWorldID	= server.m_dwWorldID;		// 游戏世界ID
	pBhead->dwServerID	= server.GetServerID();		// 服务器ID

	LPSTR pSendData = (LPSTR)head;
	dwSendDataLens = (DWORD)(len+sizeof(SGameMsgHead));

	if (msgHead.dwServerID==0)
	{
		// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
		BroadcastToServer(NULL,VOICE_BROADCASTALL_NUM,pSendData,dwSendDataLens);
	} 
	else
	{
		// 发送数据给指定服务器
		SendDataToServer(msgHead.dwServerID,pSendData,dwSendDataLens);
	}

}

// 请求桥服务器网关服务广播数据给各软件服务器
void GlobalVoiceServer::OnMsgServerBroadcastData(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{

	if(data==NULL|| len <= sizeof(SMsgVoiceBroadcastData))
	{
		return;
	}

	SMsgVoiceBroadcastData * pMsg = (SMsgVoiceBroadcastData *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalVoiceServer::OnMsgServerBroadcastData() 广播数据给各软件服务器消息:软件服务器ID列表个数=%d,数据长度=%d",
		pMsg->dwServerCounts,pMsg->nDataLens);
	//TraceLn(szBuf);
#endif

	if (pMsg->dwServerCounts<=0)
	{
		ErrorLn(_GT("请求桥服务器网关服务广播数据给各软件服务器消息时要广播服务器个数为0"));
		return;
	}

	DWORD dwServerNum = pMsg->dwServerCounts;
	DWORD * pServerArray = (DWORD *)((LPSTR)data + sizeof(SMsgVoiceBroadcastData));
	DWORD dwSendDataLens = (DWORD)(len-sizeof(SMsgVoiceBroadcastData)-dwServerNum*sizeof(DWORD));

	if (dwSendDataLens!=pMsg->nDataLens)
	{
		ErrorLn(_GT("请求桥服务器网关服务广播数据给各软件服务器消息长度错误! 实际长度=")<<dwSendDataLens<<_GT(",指示长度=")<<pMsg->nDataLens);
	}

	LPSTR pSendData = (LPSTR)((LPSTR)data + sizeof(SMsgVoiceBroadcastData)+dwServerNum*sizeof(DWORD));


	// 转发数据,为了提高效率,用修改头数据方法,必须结构体大小一样
	Assert(sizeof(SMsgVoiceBroadcastData)>=sizeof(SMsgVoiceHead));

	// 拷贝列表,否则会被中转消息头覆盖
	DWORD dwServerList[1024];
	memcpy(dwServerList,pServerArray,dwServerNum*sizeof(DWORD));


	SGameMsgHead* pHead = (SGameMsgHead*)(pSendData - sizeof(SGameMsgHead)- sizeof(SMsgVoiceHead));

	pHead->SrcEndPoint = MSG_ENDPOINT_VOICE;
	pHead->DestEndPoint= MSG_ENDPOINT_VOICE;
	pHead->byKeyModule  = MSG_MODULEID_VOICE;
	pHead->byKeyAction  = MSG_VOICE_SENDDATA;

	// 通过跨区桥服务器中转消息必须加入此消息体
	SMsgVoiceHead * pBhead =(SMsgVoiceHead*)(pSendData - sizeof(SMsgVoiceHead));
	pBhead->dwWorldID	= server.m_dwWorldID;		// 游戏世界ID
	pBhead->dwServerID	= server.GetServerID();		// 服务器ID

	pSendData = (LPSTR)pHead;
	dwSendDataLens = (DWORD)(dwSendDataLens+sizeof(SGameMsgHead)+sizeof(SMsgVoiceHead));

	// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
	BroadcastToServer(dwServerList,(WORD)dwServerNum,pSendData,dwSendDataLens);

}

// 添加蓝钻
void GlobalVoiceServer::OnMsgServerAddVoiceBlueDiamond( ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len )
{
	if( data == NULL )
	{
		return ;
	}

	if( len < sizeof(SMsgVoiceAddVoiceBlueDiamond_VV) )
	{
		ErrorLn( _GT("GlobalVoiceServer::OnMsgServerAddVoiceBlueDiamond 添加蓝钻 数据太小 len=") << len );
		return;
	}

	SMsgVoiceAddVoiceBlueDiamond_VV * pMsg = (SMsgVoiceAddVoiceBlueDiamond_VV*)data;
	if( len != sizeof(SMsgVoiceAddVoiceBlueDiamond_VV) + pMsg->nReasonLen )
	{
		ErrorLn( _GT("GlobalVoiceServer::OnMsgServerAddVoiceBlueDiamond 添加蓝钻 数据太小错误 len=") << len << _GT("，校验大小=") << (sizeof(SMsgVoiceAddVoiceBlueDiamond_VV) + pMsg->nReasonLen) );
		return;
	}

	char szText[512] = {0};

	// 判断添加的蓝钻数
	if( pMsg->dwCount >= VOICE_MAX_DIAMOND_COUNT )
	{
		sprintf_s( szText, sizeof(szText), "蓝钻数过大，增加的通行证[%d]-角色[%d]，赠送人[%d]，礼物ID[%d]，蓝钻数[%d]", pMsg->dwUDBID, pMsg->dwPDBID, pMsg->dwPDBIDSend, pMsg->dwSpare1, pMsg->dwCount );
		ErrorLn(szText);
		return;
	}
	
	CClientList & clientList = CClientList::getInstance();
	CClientUser * pUser = clientList.GetClientByPDBID(pMsg->dwPDBID);
	if( pUser == NULL )
	{
		sprintf_s( szText, sizeof(szText), "蓝钻添加者不在线，增加的通行证[%d]-角色[%d]，赠送人[%d]，礼物ID[%d]，蓝钻数[%d]", pMsg->dwUDBID, pMsg->dwPDBID, pMsg->dwPDBIDSend, pMsg->dwSpare1, pMsg->dwCount );
		ErrorLn(szText);
		return;
	}

	pUser = clientList.GetClientByPDBID(pMsg->dwPDBIDSend);
	if( pUser == NULL )
	{
		sprintf_s( szText, sizeof(szText), "赠送人不在线，增加的通行证[%d]-角色[%d]，赠送人[%d]，礼物ID[%d]，蓝钻数[%d]", pMsg->dwUDBID, pMsg->dwPDBID, pMsg->dwPDBIDSend, pMsg->dwSpare1, pMsg->dwCount );
		ErrorLn(szText);
		return;
	}
	
	
	DB_Param_VoiceNumValue_AddBlueDiamond param;
	param.dwSvrWorldID = getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
	param.dwUDBID = pMsg->dwUDBID;			// 增加的通行证
	param.dwPDBID = pMsg->dwPDBID;			// 得到蓝钻的通行证下对应的角色ID，用来记录日志用
	param.dwPDBIDSend = pMsg->dwPDBIDSend;		// 赠送礼物的人PDBID，用来记录日志用
	param.dwDiamondCount = pMsg->dwCount;			// 增加的蓝钻数，一次最大不超过500000，超过的当作无效并报警
	param.dwGiftID = pMsg->dwSpare1;

	CDBRequester::getInstance().DB_VoiceNumValue_AddBlueDiamond(param);
}

// 服务器请求广播数据到客户端
void GlobalVoiceServer::OnMsgServerBroadcastDataToClient( ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len )
{
	if( data == NULL )
	{
		return;
	}

	if( len <= sizeof(SMsgVoiceBroadcastDataToClient) )
	{
		ErrorLn( _GT("GlobalVoiceServer::OnMsgServerBroadcastDataToClient 无数据可以广播") );
		return;
	}

	SMsgVoiceBroadcastDataToClient * pMsg = (SMsgVoiceBroadcastDataToClient*)data;

	if( len != pMsg->dwDataLen + sizeof(SMsgVoiceBroadcastDataToClient) )
	{
		ErrorLn( _GT("GlobalVoiceServer::OnMsgServerBroadcastDataToClient 数据长度错误 len=") << len << _GT("，校验长度=") << (pMsg->dwDataLen + sizeof(SMsgVoiceBroadcastDataToClient)) );
		return;
	}

	if( pMsg->dwDataLen < sizeof(SGameMsgHead) )
	{
		ErrorLn( _GT("GlobalVoiceServer::OnMsgServerBroadcastDataToClient 附加广播数据太小 pMsg->dwDataLen=") << pMsg->dwDataLen );
		return;
	}

	CClientList & clientList = CClientList::getInstance();
	DWORD * pClientIDArrayAll = m_dwClientIDBuff;
	DWORD * pMergeIDArray = m_dwMergeIDBuff;
	DWORD dwSendCount = 0;
	bool bCheck = false;

	switch(pMsg->nType)
	{
	case VoiceChannel_GM:		// 广播所有人
		{
			clientList.GetClientIDList( pClientIDArrayAll, VOICE_MAX_ROOM_USERCOUNT, dwSendCount, VoiceChannel_GM );
		}
		break;

	case VoiceChannel_World:	// 广播对应的游戏世界
	case VoiceChannel_Country:
	case VoiceChannel_Clan:
	case VoiceChannel_Kin:
	case VoiceChannel_Group:
	case VoiceChannel_Team:
		{
			SVoiceChannelID idInfo;
			idInfo.dwRoomID = 0;
			if( pMsg->nWorldID1 > 0 && pMsg->dwValueID1 > 0 )
			{
				idInfo.dwType = pMsg->nType;
				idInfo.dwWorldID = pMsg->nWorldID1;
				idInfo.dwID = pMsg->dwValueID1;

				LONGLONG * pID = (LONGLONG*)(&idInfo);

				clientList.GetClientIDList( pMergeIDArray, VOICE_MAX_ROOM_USERCOUNT, dwSendCount, pMsg->nType, *pID );
				
				bCheck = true;
			}

			// 如果两个组合ID值不相等才查找
			if( pMsg->nWorldID2 > 0 && pMsg->dwValueID2 > 0 && ( pMsg->nWorldID1 != pMsg->nWorldID2 || pMsg->dwValueID1 != pMsg->dwValueID2 ) )
			{
				idInfo.dwType = pMsg->nType;
				idInfo.dwWorldID = pMsg->nWorldID2;
				idInfo.dwID = pMsg->dwValueID2;

				LONGLONG * pID = (LONGLONG*)(&idInfo);

				// 修改BUFF起始位置，计算好剩余空间，把新的列表放入缓存
				DWORD * pArrayStart = pMergeIDArray + dwSendCount;
				DWORD dwCount = 0;

				clientList.GetClientIDList( pArrayStart, VOICE_MAX_ROOM_USERCOUNT-dwSendCount, dwCount, pMsg->nType, *pID );

				// 修改总数
				dwSendCount += dwCount;
				
				bCheck = true;
			}
		}
		break;

	case VoiceChannel_Personal:		// 个人频道要特殊处理下
		{
			SVoiceChannelID idInfo;
			idInfo.dwRoomID = 0;
			if( pMsg->dwValueID1 > 0 )
			{
				idInfo.dwType = pMsg->nType;
				idInfo.dwWorldID = getThisGameWorldID();
				idInfo.dwID = pMsg->dwValueID1;

				LONGLONG * pID = (LONGLONG*)(&idInfo);

				clientList.GetClientIDList( pMergeIDArray, VOICE_MAX_ROOM_USERCOUNT, dwSendCount, pMsg->nType, *pID );

				bCheck = true;
			}

			// 如果两个组合ID值不相等才查找
			if( pMsg->dwValueID2 > 0 && pMsg->dwValueID1 != pMsg->dwValueID2 )
			{
				idInfo.dwType = pMsg->nType;
				idInfo.dwWorldID = getThisGameWorldID();
				idInfo.dwID = pMsg->dwValueID2;

				LONGLONG * pID = (LONGLONG*)(&idInfo);

				// 修改BUFF起始位置，计算好剩余空间，把新的列表放入缓存
				DWORD * pArrayStart = pMergeIDArray + dwSendCount;
				DWORD dwCount = 0;

				clientList.GetClientIDList( pArrayStart, VOICE_MAX_ROOM_USERCOUNT-dwSendCount, dwCount, pMsg->nType, *pID );

				// 修改总数
				dwSendCount += dwCount;

				bCheck = true;
			}
		}
		break;
	
	// 默认，只广播双方		
	case VoiceChannel_None:
		{
			if( pMsg->dwPDBID1 > 0 )
			{
				CClientUser * pUser = clientList.GetClientByPDBID(pMsg->dwPDBID1);
				if( pUser != NULL )
				{
					DWORD dwClientID = pUser->GetClientID();
					pMergeIDArray[dwSendCount] = dwClientID;	// 直接把ID赋值到BUFF中
					dwSendCount ++;
					bCheck = true;
				}
			}

			if( pMsg->dwPDBID2 > 0 )
			{
				CClientUser * pUser = clientList.GetClientByPDBID(pMsg->dwPDBID2);
				if( pUser != NULL )
				{
					DWORD dwClientID = pUser->GetClientID();

					// 直接把ID赋值到BUFF中
					pMergeIDArray[dwSendCount] = dwClientID;	// 直接把ID赋值到BUFF中
					dwSendCount ++;
					bCheck = true;
				}
			}
		}
		break;

	default:	
		break;
	}

	// 如果有广播用户dwPDBID1所在房间
	if( pMsg->bRoom && pMsg->dwPDBID1 > 0 && pMsg->nType != VoiceChannel_GM )		// 如果要找房间的人，并且pMsg->nType不是VoiceChannel_GM
	{
		CClientUser * pUser = clientList.GetClientByPDBID(pMsg->dwPDBID1);
		if( pUser != NULL )
		{
			CRoom * pRoom = pUser->GetCurRoom();	// 获取当前房间里的人，并合并到pClientIDArrayAll
			if( pRoom != NULL )
			{
				TMAP_VoiceID2IDInfo TmpCheckList;

				if( dwSendCount > 0 )	// 如果之前有ID列表了，就需要先合并，如果没有列表，就直接查找放如列表即可
				{
					// 先把前面的列表合并进去，第一次合并
					dwSendCount = MergeClientIDList( pClientIDArrayAll, VOICE_MAX_ROOM_USERCOUNT, 0, TmpCheckList, pMergeIDArray, dwSendCount );

					DWORD dwCount = 0;
					pRoom->GetUserIDList( pMergeIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount );

					// 如果前面有记录，就需要与前面的ID列表查重
					dwSendCount = MergeClientIDList( pClientIDArrayAll, VOICE_MAX_ROOM_USERCOUNT, dwSendCount, TmpCheckList, pMergeIDArray, dwCount, true );
				}
				else
				{
					pRoom->GetUserIDList( pClientIDArrayAll, VOICE_MAX_ROOM_USERCOUNT, dwSendCount );
				}
			}
		}
	}
	
	// 如果取的了用户
	if( dwSendCount > 0 )
	{
		char * pNewData = (char*)data + sizeof(SMsgVoiceBroadcastDataToClient);
		BroadcastDataToClient( pClientIDArrayAll, dwSendCount, pNewData, pMsg->dwDataLen );
	}
}

//////////////////////////////////////////////////////////////////////////


// 服务器请求登录
void GlobalVoiceServer::OnMsgSocialServerLogin( ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len )
{
	if( data == NULL )
	{
		return ;
	}
	if( len < sizeof(SMsgVoiceZoneLogin_ZS) )
	{
        ErrorLn(__FUNCTION__ << ", len=" << len);
		return ;
	}
	SMsgVoiceZoneLogin_ZS *pLoginData = (SMsgVoiceZoneLogin_ZS *)data;

	DWORD dwClientID = pLoginData->userInfo.dwClientID;

	char szKeyData[VOICE_RC4_MAX_SIZE] = {0};
	memcpy( szKeyData, pLoginData->key, VOICE_RC4_MAX_SIZE );

	CClientList & clientList = CClientList::getInstance();

	// 获取登录用户对象
	CClientUser *pUser = clientList.FindUser(dwClientID);
	if( NULL == pUser )
	{
		ErrorLn(_GT("登录时，对象不存在！dwClientID=") << dwClientID);
		return ;
	}

	// 服务务器更新和相关处理
	DWORD dwTotalClients = clientList.CountUser();	// 用户数

	// 如果人数超了,就踢
	if( dwTotalClients > m_dwMaxConnection )
	{
		pUser->SendChatMsg("人数已满，请稍后再尝试登录！");
		KickUser( pUser, VoiceKickReason_Full );
		return ;
	}

	// 检验KEY值
	if( !pUser->CheckKeyData( szKeyData, VOICE_RC4_MAX_SIZE ) )
	{
		//ErrorLn("CheckKeyData 用户数据校验失败，dwClientID=" << dwClientID);
		pUser->SendChatMsg("你的用户信息错误，验证失败！");
		KickUser( pUser, VoiceKickReason_LoginError );
		return ;
	}

	pUser->SetLogin(true);
	pUser->SetState(VoiceUserState_Login);

	// 设置服务器信息，游戏世界ID，服务器ID，用来加速查找服务器
	pUser->SetServerInfo( server.GetServerID(), server.m_dwWorldID );

	// 创建PDBID索引
	DWORD dwOldClientID = clientList.AddPDBIDInfo( pLoginData->userInfo.dwPDBID, dwClientID );
	if( dwOldClientID > 0 )
	{
		// 如果找到了，说明有重复登录，要踢掉旧的用户
		CClientUser * pOldUser = clientList.GetUser(dwOldClientID);
		if( pOldUser != NULL )
		{
			pOldUser->SendChatMsg("您的帐号已在别处登录，被强制踢出语音系统！", VoiceInfoPos_ActorRight|VoiceInfoPos_ChatWindow, VoiceTipType_Operate );
			gGlobalServer->KickUser( pOldUser, VoiceKickReason_UserExist );
		}
	}

	// 创建UDBID索引
	dwOldClientID = clientList.AddUDBIDInfo( pLoginData->userInfo.dwUDBID, dwClientID );
	if( dwOldClientID > 0 )
	{
		// 如果找到了，说明有重复登录，要踢掉旧的用户
		CClientUser * pOldUser = clientList.GetUser(dwOldClientID);
		if( pOldUser != NULL )
		{
			pOldUser->SendChatMsg("您的帐号已在别处登录，被强制踢出语音系统！", VoiceInfoPos_ActorRight|VoiceInfoPos_ChatWindow, VoiceTipType_Operate );
			gGlobalServer->KickUser( pOldUser, VoiceKickReason_UserExist );
		}
	}

	// 设置上次召集粉丝的时间
	SVoiceCallFansInfo * pCallFansInfo = GetCallFansInfo(pLoginData->userInfo.dwUDBID);
	if( pCallFansInfo != NULL )
	{
		pUser->SetLastCallFansInfo( pCallFansInfo->dwLastCallFansTime, pCallFansInfo->dwFansAcceptCount, pCallFansInfo->dwFansCancelCount );
	}

	// 更新用户信息
    SVoiceUserInfo_Table contextTable;
    memcpy(&(contextTable.context), &(pLoginData->userInfo), sizeof(pLoginData->userInfo));
    
    bool bMask[VOICE_FIELD_MAX];
    memset(bMask, 1, sizeof(bMask));
    obuf ob;
    contextTable.getFields(ob, bMask);
    pUser->OnUpdateUserInfo(ob.data(), ob.size());

	// 设置上次发送语音召集令的时间
	SetADInfoToUser(*pUser);

	// 通知客户端登录成功，默认无频道
	SendVoiceDataToClient( dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_LoginResponse, dwClientID );

	// 把游戏世界列表发到客户端
	UpdateWorldListToClient( &dwClientID, 1 );

	m_TotalInfo.dwClients	= dwTotalClients+1;

	// 从DB登录一下,用来记录在线状态,和加载一下用户信息
	pUser->DBLogin();

	// 客户端登录
	OnClientUserLogin(pUser);

	//TraceLn( "用户登录：" << pUser->ToString().data() );
}

// 游戏服更新语音用户信息
void GlobalVoiceServer::OnMsgUserInfoUpdate(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
    if(data == NULL || len < sizeof(PDBID))
    {
        ErrorLn(__FUNCTION__ << ", msg format error");
        return;
    }
    ibuffer in(data, len);
    PDBID pdbid;
    in >> pdbid;
    CClientUser* pUser = CClientList::getInstance().GetClientByPDBID(pdbid);
    if(pUser == NULL)
    {
        WarningLn(__FUNCTION__ << ", find user failed, dbid=" << pdbid);
        return;
    }
    pUser->OnUpdateUserInfo(in.current(), in.space());
}

// 社会服发送私聊邀请
void GlobalVoiceServer::OnMsgSocialServerChatInvite( ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len )
{
	if( data == NULL )
	{
		return ;
	}

	if( len < sizeof(SVoiceChatInvite_SV) )
	{
		ErrorLn( _GT("数据包无效GlobalVoiceServer::OnMsgSocialServerChatInvite，len=") << len );
		return ;
	}

	SVoiceChatInvite_SV *pInviteData = (SVoiceChatInvite_SV *)data;

	CClientUser * pInviter = CClientList::getInstance().GetClientByPDBID(pInviteData->dwInviter);
	if( NULL == pInviter )
	{
		return;
	}

	pInviter->ChatInvite( pInviteData->dwAcceptor, pInviteData->bStranger );
}

void GlobalVoiceServer::OnMsgSocialNotifyActorLogout(ServerUser& server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data == NULL || len < sizeof(SMsgVoiceSocialNotifyActorLogout_OV))
	{
		ErrorLn(_GT("OnMsgSocialNotifyActorLogout --- 消息格式错误"));
		return;
	}

	SMsgVoiceSocialNotifyActorLogout_OV* pMsg = (SMsgVoiceSocialNotifyActorLogout_OV*)data;

	CClientUser* pUser = CClientList::getInstance().GetClientByPDBID(pMsg->dwPDBID);
	if(pUser == NULL)
	{
		return;
	}	

	// 通知语音网关踢人
	pUser->Kick(VoiceKickReason_Default);

	// pUser->Release();
}


// 语音网关握手消息
void GlobalVoiceServer::OnMsgGatewayHandshake(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len != sizeof(SMsgVoiceHandshake_GV))
	{
		server.Release();
		return;
	}

	SMsgVoiceHandshake_GV * pMsg = (SMsgVoiceHandshake_GV *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "GlobalVoiceServer::OnMsgGatewayHandshake() 服务器握手消息:网关ID=%d,最大人数=%d",
		pMsg->dwGatewayID,pMsg->dwMaxCounts);
	TraceLn(szBuf);
#endif

	// 更新服务器信息
	server.m_dwMaxCounts	= pMsg->dwMaxCounts;

	DWORD dwGatewayID = pMsg->dwGatewayID;

	bool bCheckFail = false;
	if (head->DestEndPoint !=MSG_ENDPOINT_VOICE)
	{
		WarningLn(_GT("语音网关握手失败! ")<<server.ToString().data());
		bCheckFail = true;
	}
	else if(dwGatewayID <= 0 || dwGatewayID >= VOICE_MAX_GATEWAYID )
	{
		ErrorLn(_GT("语音网关握手失败!语音网关ID[")<<dwGatewayID<<_GT("]无效,")<<server.ToString().data());
		AlertError(MG_AlertSrcType_Network,"语音网关握手失败!语音网关ID["<<dwGatewayID<<"]无效,"<<server.ToString().data());
		bCheckFail = true;
	}
	else if(!(GatewayList::getInstance().AddUserByGatewayID( dwGatewayID, &server )))
	{
		ErrorLn(_GT("语音网关握手失败!语音网关ID[")<<dwGatewayID<<_GT("]已经存在,")<<server.ToString().data());
		AlertError(MG_AlertSrcType_Network,_GT("语音网关握手失败!语音网关ID[")<<dwGatewayID<<_GT("]已经存在,")<<server.ToString().data());
		bCheckFail = true;
	}

	if (bCheckFail)
	{
		server.Release();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_HANDSHAKE_RESPONSE;

	// 桥服务器回应握手消息数据结构
	SMsgVoiceHandshakeResponse_VG sendData;
	sendData.dwID = server.GetServerID();		// 连接ID
	sendData.dwServerTime = (DWORD)time(NULL);	// 服务器时间

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceHandshakeResponse_VG));

	server.SendData(obufData.data(),obufData.size());

	TraceLn(_GT("回应语音网关连接握手! ")<< a2utf8(server.ToString().c_str()));

	// 服务务器更新和相关处理
	m_TotalInfo.dwGateways	= GatewayList::getInstance().Count();	// 网关数

}


// 语音网关转发而来的客户端消息
void GlobalVoiceServer::OnMsgGatewaySendData(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len <= sizeof(SMsgVoiceSendData))
	{
		return;
	}

	m_dwRecvSpeedClient += len;

	SMsgVoiceSendData * pMsg = (SMsgVoiceSendData *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalVoiceServer::OnMsgGatewaySendData() 发送数据给各软件服务器消息:软件服务器ID=%d,数据长度=%d",
		pMsg->dwServerID,pMsg->nDataLens);
	//TraceLn(szBuf);
#endif

	DWORD dwDataLen = (DWORD)(len-sizeof(SMsgVoiceSendData) );

	if (dwDataLen!=pMsg->nDataLens)
	{
		ErrorLn(_GT("语音网关中转语音服务器消息长度错误! 实际长度=")<<dwDataLen<<_GT(",指示长度=")<<pMsg->nDataLens);
		return;
	}

	if ( dwDataLen<sizeof(SGameMsgHead))
	{
		WarningLn(_GT("语音网关中转语音服务器消息长度错误! Package length is too short."));
		return;
	}

	uchar* pData = (uchar*)data+sizeof(SMsgVoiceSendData);

	SGameMsgHead* pHead = (SGameMsgHead*)pData;
	ulong moduleId = pHead->byKeyModule;
	uchar* clientdata = (uchar*)pData + sizeof(SGameMsgHead);
	size_t new_len =(size_t)(dwDataLen-sizeof(SGameMsgHead));
	if (new_len == 0) clientdata = 0;

	CClientUser * pClientUser = CClientList::getInstance().GetUser(pMsg->dwServerID);
	if (NULL ==pClientUser)
	{
		return;
	}

	// 是否验证通过的
	if (!pClientUser->GetLogin())
	{
		// 如果没有验证通过，只收握手消息
		if( pHead->byKeyAction != MSG_VOICE_LOGIN )
		{
#ifdef VOICE_PRINT_DEBUG_INF
			ErrorLn(_GT("未登录用户发来错误消息，actionId:") << actionId);
#endif
			return ;
		}
	}

	onClientMessage( server, *pClientUser, pHead->byKeyAction, pHead, clientdata, new_len );

}


// 语音网关同步客户端信息到语音服务器
void GlobalVoiceServer::OnMsgGatewayClientListUpdate(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if( data == NULL || len < sizeof(SMsgVoiceGatewayClientListUpdate) )
	{
		return ;
	}

	SMsgVoiceGatewayClientListUpdate *pMsg = (SMsgVoiceGatewayClientListUpdate *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf), _NGT"GlobalVoiceServer::OnMsgGatewayClientListUpdate()  语音网关更新消息:更新类型=%d,客户端个数=%d",
		pMsg->nUpdateType,pMsg->dwCounts);
	TraceLn(szBuf);
#endif

	// 游戏世界信息列表
	SMsgVoiceGatewayClientNode * pClientNode = (SMsgVoiceGatewayClientNode *)((LPCSTR)data+ sizeof(SMsgVoiceGatewayClientListUpdate));
	DWORD nListCounts = pMsg->dwCounts;

	CClientList &clientList = CClientList::getInstance();

	// 更新类型
	switch(pMsg->nUpdateType)
	{
	case VoiceUpdateType_All:		// 全部
		{
			//更新该网关对应的所有用户
			DWORD dwOpGatewayID = server.GetGatewayID();
			clientList.OnVoiceGatewayUpdateUserList( dwOpGatewayID, pClientNode, nListCounts );
		}
		break;

	case VoiceUpdateType_Add:		// 新增 修改
	case VoiceUpdateType_Edit:
		{
			for (DWORD i=0;i<nListCounts;i++ )
			{
				// 找到用户
				CClientUser * pClientUser = clientList.FindUser(pClientNode->dwClientID);
				if (pClientUser!=NULL)
				{
					// 有更新
					pClientUser->SetClientIP(pClientNode->dwRemoteIP);
				}
				else
				{
					// 没有就加入
					pClientUser = clientList.AddUser(pClientNode->dwClientID);
					if(pClientUser!=NULL)
					{
						pClientUser->SetClientIP(pClientNode->dwRemoteIP);
						OnClientUserEnter(pClientUser);
					}
				}

				pClientNode++;
			}
		}
		break;

	case VoiceUpdateType_Delete:
		{
			for (DWORD i=0;i<nListCounts;i++ )
			{
				CClientUser * pClientUser = clientList.FindUser(pClientNode->dwClientID);
				if( NULL != pClientUser )	// 如果不存在，添加
				{

					pClientUser->Release();
				}
				pClientNode++;
			}
		}
		break;
	}
}

// 请求性能检测消息包
void GlobalVoiceServer::OnMsgGatewayPerformance(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{

	if(data==NULL|| len != sizeof(SMsgVoicePerformance_S))
	{
		return;
	}

	SMsgVoicePerformance_S * pMsg = (SMsgVoicePerformance_S *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "GlobalVoiceServer::OnMsgGatewayPerformance() 请求性能检测消息包:ClientID=%d",pMsg->dwClientID);
	TraceLn(szBuf);
#endif

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_PERFORMANCE;

	SMsgVoicePerformance_S sendData;
	sendData = *pMsg;

	DWORD dwType = VoicePerformanceType_Server;
	bool bSend = false;

	SMsgVoicePerformanceNode &myNode = sendData.perData.data[dwType];
	// 取得高性能计数
	LARGE_INTEGER litmpTims; 
	::QueryPerformanceCounter(&litmpTims);
	myNode.nFrequency = getPerformanceFrequency();
	myNode.nRecvTicks = litmpTims.QuadPart;

	IConnection * pConnection = server.GetConnection();
	if (pConnection!=NULL)
	{
		myNode.nSendTicks = pConnection->GetLastRecvDataTimes();
		myNode.nSendCounts = (WORD)pConnection->GetPendingCount(true);
		myNode.nRecvCounts = (WORD)pConnection->GetPendingCount(false);
	}

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoicePerformance_S));

	server.SendData(obufData.data(),obufData.size());
}


/// 客户端登录
void GlobalVoiceServer::OnMsgClientLogin(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
#ifndef VOICE_BIG_TEST
	ErrorLn(_GT("GlobalVoiceServer::OnMsgClientLogin 没有开启压力测试模式，拒绝登录！"));
	return ;
#endif
}


// 语音客户端子消息
void GlobalVoiceServer::OnMsgClientVoiceSubMsg(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{

	if(data==NULL|| len < sizeof(SMsgVoiceDataSubMsg))
	{
		return;
	}

	SMsgVoiceDataSubMsg * pMsg = (SMsgVoiceDataSubMsg *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[1024]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"CGlobalVoiceServer::OnMsgVoiceSubMsg()  语音客户端信息子消息:子消息代码=%d,用户数据=%d,%d,%d",
		pMsg->dwMsgCode,pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3);
	TraceLn(szBuf);
#endif

	LPSTR pMsgData = (LPSTR)data + sizeof(SMsgVoiceDataSubMsg);
	size_t nNewlen = len - sizeof(SMsgVoiceDataSubMsg);

	// EMVoiceDataSubMsgCode
	switch(pMsg->dwMsgCode)
	{
	case VoiceSubMsg_OpenMic:	// 麦操作
		{
			client.OpenMic( pMsg->dwUserData1 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_OpenChatMic:	// 私聊开麦
		{
			client.OpenChatMic( pMsg->dwUserData1 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_OpenVideo:	// 操作视频
		{
			client.OpenVideo( pMsg->dwUserData1 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_OpenHeadset:	// 听筒操作
		{
			client.OpenHeadset( pMsg->dwUserData1 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_RequestVisitor:		// 请求观察
		{
			client.RequestVisitor( pMsg->dwUserData1 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_ChangeMicState:				// 设置房间麦序模式
		{
			DWORD dwMicState = pMsg->dwUserData1;

			if( dwMicState >= VoiceMicState_Max )
			{
				client.SendChatMsg("麦模式不存在！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			if( NULL == pRoom )
			{
				ErrorLn( _GT("VoiceSubMsg_ChangeMicState 当前房间对象为空？ClientID=") << client.GetClientID() << _GT("，当前房间ID") << client.GetCurRoomID() );
				break ;
			}

			// 判断是否有权限
			if( !client.GetRight(VoiceRight_Mic) )
			{
				client.SendChatMsg("你没有麦序管理权限！");
				break ;
			}

			const SVoiceChannelConfig & channelConfig = GetChannelConfig(pRoom->GetType());
			// 进入之前要退出上一个模式
			switch(dwMicState)
			{
			case VoiceMicState_MicFree:				/// 自由麦
				{
					// 不支持自由模式
					if( !channelConfig.bMicFree )
					{
						client.SendChatMsg("当前频道不支持自由模式！");
						return;
					}
				}
				break;

			case VoiceMicState_MicOrder:			/// 麦序
				{
					// 不支持自由模式
					if( !channelConfig.bMicOrder )
					{
						client.SendChatMsg("当前频道不支持麦序模式！");
						return;
					}
				}
				break;
			}

			pRoom->SetMicState(dwMicState);
		}
		break;

	case VoiceSubMsg_SetRoomStateFlag:				// 设置房间状态标记
		{
			DWORD dwRoomFlag = pMsg->dwUserData1;
			bool bSet = pMsg->dwUserData2 > 0 ? true : false;
			
			CRoom * pRoom = client.GetCurRoom();
			if( NULL == pRoom )
			{
				ErrorLn( _GT("VoiceSubMsg_SetRoomStateFlag 当前房间对象为空？ClientID=") << client.GetClientID() << _GT("，当前房间ID") << client.GetCurRoomID() );
				break ;
			}

			// 判断是否有权限
			switch(dwRoomFlag)
			{
			case VoiceRoomState_AddMicOrder:	/// 麦序模式下可抢麦
				{
					if( !client.GetRight(VoiceRight_Mic) )
					{
						client.SendChatMsg("你没有麦序管理权限！");
						return ;
					}
				}
				break;

			case VoiceRoomState_HoldMic:		/// 麦序模式下控麦
				{
					if( !client.GetRight(VoiceRight_Mic) )
					{
						client.SendChatMsg("你没有麦序管理权限！");
						return ;
					}

					// 控麦
					pRoom->HoldMic(bSet);
				}
				return ;

			case VoiceRoomState_KeySpeak:	/// 自由麦下只能按键发音
				{
					if( !client.GetRight(VoiceRight_Mic) )
					{
						client.SendChatMsg("你没有麦序管理权限！");
						return ;
					}

					if( pRoom->GetMicState() == VoiceMicState_MicOrder )
					{
						client.SendChatMsg("麦序模式下不能设置按键发音！");
						return ;
					}
				}
				break;

			case VoiceRoomState_Lock:	/// 有密码
				{
					if( !client.GetRight(VoiceRight_Lock) )
					{
						client.SendChatMsg("你没有加密房间权限！");
						return ;
					}
				}
				break;

			default:	// 无效操作
				{
#ifdef VOICE_PRINT_DEBUG_INF
					ErrorLn( _GT("VoiceSubMsg_SetRoomStateFlag 无效Flag=") << dwRoomFlag );
#endif
					break ;
				}
			}

			pRoom->SetFlag( dwRoomFlag, bSet );
		}	
		break;

	case VoiceSubMsg_EnterRoom:   // 客户端请求进入房间
		{
			if( nNewlen != sizeof(SVoiceSubMsgEnterRoom_CV) )
			{
				ErrorLn(_GT("VoiceSubMsg_EnterRoom 进入频道的附加参数大小无效，nNewLen=") << nNewlen << _GT(", 标准大小=") << sizeof(SVoiceSubMsgEnterRoom_CV) );
				break ;
			}

			SVoiceSubMsgEnterRoom_CV *pInfo = (SVoiceSubMsgEnterRoom_CV *)pMsgData;
            client.EnterRoom(pInfo->nRoomID, pInfo->szPassword);
		}
		break;

	case VoiceSubMsg_GMEnterRoom:	// GM进入房间
		{
			if( nNewlen != sizeof(LONGLONG) )
			{
				ErrorLn(_GT("VoiceSubMsg_GMEnterRoom GM进入频道的附加参数大小无效，nNewLen=") << nNewlen << _GT(", 标准大小=") << sizeof(LONGLONG) );
				break ;
			}

			// 判断权限
			if( client.GetType() != VoiceUserType_GM )
			{
				client.SendChatMsg("你没有跳转频道的权限！");
				break ;
			}

			LONGLONG * pRoomID = (LONGLONG * )pMsgData;
			
#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn("VoiceSubMsg_GMEnterRoom GM请求进入频道ID=" << (*pRoomID));
			SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(pRoomID);
			ErrorLn( "VoiceSubMsg_GMEnterRoom GM[" << client.GetClientID() << "]请求进入频道，频道类型=" << pIDInfo->dwType << ", 子房间ID=" << pIDInfo->dwRoomID << ", 世界ID=" << pIDInfo->dwWorldID << ", ID=" << pIDInfo->dwID );
#endif

			// 计算该房间所属频道ID
			LONGLONG nChannelID = *pRoomID;
			// 判断是否有频道
			if( nChannelID <= 0 )
			{
				client.SendChatMsg("无效的房间，ID为0！");
				break;
			}

			SVoiceChannelID * pChannelIDInfo = (SVoiceChannelID *)(&nChannelID);
			pChannelIDInfo->dwRoomID = 0;

#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn("VoiceSubMsg_GMEnterRoom 频道ID=" << nChannelID);
#endif

			// 查找频道是否存在
			CChannel * pChannel = CChannelList::getInstance().Get(nChannelID);
			if( NULL == pChannel )
			{
				client.SendChatMsg("频道不存在！");
				break ;
			}

			// 强制进入房间
			client.EnterRoom( *pRoomID, NULL, true );
		}
		break;

	case VoiceSubMsg_GMEnterRoomByPDBID:	// GM进入玩家所在房间
		{
			// 判断权限
			if( client.GetType() != VoiceUserType_GM )
			{
				client.SendChatMsg("你没有跳转频道的权限！");
				break ;
			}

			DWORD dwPDBID = pMsg->dwUserData1;
			CClientUser * pUser = CClientList::getInstance().GetClientByPDBID(dwPDBID);
			if( NULL == pUser )
			{
				client.SendChatMsg("此用户未登录语音服务器！");
				break;
			}

			// 获取玩家当前所在房间ID
			CRoom * pCurRoom = pUser->GetCurRoom();
			if( NULL == pCurRoom )
			{
				client.SendChatMsg("此用户未进入语音房间！");
				break;
			}

			// 强制进入房间
			client.EnterRoom( pCurRoom->GetIDEx(), NULL, true );
		}
		break;

	case VoiceSubMsg_SetRoomLock:	// 设置房间锁
		{	
			DWORD dwRoomID = pMsg->dwUserData1;

			bool bLock = pMsg->dwUserData2 > 0 ? true : false;
			
			if( nNewlen + 1 > VOICE_NAME_MAXSIZE )
			{
				break;
			}

			char szPassword[VOICE_NAME_MAXSIZE] = {0};
			sstrcpyn( szPassword, pMsgData, nNewlen + 1 );

			CRoom * pRoom = client.GetCurRoom();
			if( NULL == pRoom )
			{
				ErrorLn( _GT("VoiceSubMsg_SetRoomLock 当前房间对象为空？ClientID=") << client.GetClientID() << _GT("，当前房间ID") << client.GetCurRoomID() );
				break ;
			}

			if( !client.GetRight(VoiceRight_Lock) )
			{
				client.SendChatMsg("你没有房间加锁权限！");
				break ;
			}

			CRoom * pLockRoom = pRoom->GetChannel().GetRoomPtr(dwRoomID);
			if( pLockRoom == NULL )
			{
				client.SendChatMsg("不能操作一个无效的房间！");
				break ;
			}

			// 修改密码
			if( strlen(szPassword) > 0 )
			{
				pLockRoom->SetPassword(szPassword);
			}

			// 修改标记
			pLockRoom->SetFlag( VoiceRoomState_Lock, bLock );
		}
		break;

	case VoiceSubMsg_AddMicOrder:	// 抢/下麦
		{
			client.AddMicOrder( pMsg->dwUserData1 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_AddMicOrderTime:	// 增加/减少麦上人说话时间
		{
			// 判断是否有权限
			if( !client.GetRight(VoiceRight_Mic) )
			{
				client.SendChatMsg("你没有麦序管理权限！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在，且为麦序模式
			if( pRoom != NULL && VoiceMicState_MicOrder == pRoom->GetMicState() )
			{
				if( !(pRoom->AddMicOrderTime( pMsg->dwUserData1 > 0 ? true : false, pMsg->dwUserData2 )) )
				{
					client.SendChatMsg("麦序时长已达上限！");
				}
			}
		}
		break;

	case VoiceSubMsg_SetMicTime:
		{
			// 判断是否有权限
			if( !client.GetRight(VoiceRight_Mic) )
			{
				client.SendChatMsg("你没有设置麦序时间权限！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在
			if( pRoom != NULL )
			{
				CChannel & channel = pRoom->GetChannel();
				CRoom * pUpdateRoom = channel.GetRoomPtr(pMsg->dwUserData1);
				if( pUpdateRoom == NULL )
				{
					client.SendChatMsg("你操作的房间无效！");
					break;
				}

				DWORD dwCurTime = pUpdateRoom->SetDefaultMicOrderTime(pMsg->dwUserData2);
				char szText[128];
				sprintf_s( szText, sizeof(szText), "该房间的默认麦序时间已修改为%d秒", dwCurTime );
				client.SendChatMsg(szText);
			}
		}
		break;

	case VoiceSubMsg_AddClientToMicOrder:	// 把一个用户添加到麦序，或从麦序上移除
		{
			// 判断是否有权限
			if( !client.GetRight(VoiceRight_Mic) )
			{
				client.SendChatMsg("你没有麦序管理权限！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在，且为麦序模式
			if( pRoom != NULL && VoiceMicState_MicOrder == pRoom->GetMicState() )
			{
				pRoom->AddClientToMicOrder( pMsg->dwUserData1, pMsg->dwUserData2 > 0 ? true : false );
			}
		}
		break;

	case VoiceSubMsg_MoveMicOrder:			// 移动麦序
		{
			// 判断是否有权限
			if( !client.GetRight(VoiceRight_Mic) )
			{
				client.SendChatMsg("你没有麦序管理权限！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在，且为麦序模式
			if( pRoom != NULL && VoiceMicState_MicOrder == pRoom->GetMicState() )
			{
				pRoom->MoveMicOrder( pMsg->dwUserData1, pMsg->dwUserData2 );
			}
		}
		break;

	case VoiceSubMsg_ClearAllMicOrder:		// 清空麦序
		{
			// 判断是否有权限
			if( !client.GetRight(VoiceRight_Mic) )
			{
				client.SendChatMsg("你没有麦序管理权限！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在，且为麦序模式
			if( pRoom != NULL && VoiceMicState_MicOrder == pRoom->GetMicState() )
			{
				pRoom->ClearAllMicOrder();
			}
		}
		break;

	case VoiceSubMsg_Move:				// 成员调度
		{
			client.Move( pMsg->dwUserData1 > 0 ? true : false, pMsg->dwUserData2 );
		}
		break;

	case VoiceSubMsg_Broadcast:
		{
			client.OpenBroadcast( pMsg->dwUserData1 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_AD:				// 频道拉人
		{
			if( pMsg->dwUserData1 != nNewlen )
			{
				ErrorLn(_GT("GlobalVoiceServer::OnMsgClientVoiceSubMsg VoiceSubMsg_AD 附带文字长度校验失败 nNewlen=") << nNewlen << "，pMsg->dwUserData1=" << pMsg->dwUserData1 );
				break ;
			}

			// client.SendADToSocial( pMsg->dwUserData2 , pMsg->dwUserData3, pMsgData, pMsg->dwUserData1 );
			client.SendADToClient(pMsg->dwUserData4, pMsg->dwUserData5, pMsg->dwUserData2 , pMsg->dwUserData3, pMsgData, pMsg->dwUserData1 );
		}
		break;

	case VoiceSubMsg_Ban:		// 封PDBID 禁言
		{
			client.BanPDBID( pMsg->dwUserData1, pMsg->dwUserData2 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_Kick:		// 踢出频道
		{
			client.KickPDBID(pMsg->dwUserData1);
		}
		break;

	case VoiceSubMsg_SetRemark:		// 设置频道备注
		{
			// 判断是否有权限
			if( !client.GetRight(VoiceRight_Mic) )
			{
				client.SendChatMsg("你没有设置频道公告权限！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在
			if( pRoom != NULL )
			{
				char szRemark[VOICE_TEXTSTR_MAXSIZE] = {0};
				if( nNewlen + 1 > sizeof(szRemark) )
				{
					break;
				}

				sstrcpyn( szRemark, pMsgData, nNewlen + 1 );
#ifdef VOICE_PRINT_DEBUG_INF
				TraceLn("GlobalVoiceServer::OnMsgClientVoiceSubMsg VoiceSubMsg_SetRemark 公告=" << szRemark);
#endif
				
				pRoom->SetRemark(szRemark);
			}
			else
			{
				client.SendChatMsg("你不在房间里！");
			}
		}
		break;

	case VoiceSubMsg_SetTitleName:	// 设置称号
		{
		}
		break;

	case VoiceSubMsg_OpenUserList:					// 开启用户列表
		{
			// 判断权限
			if( client.GetType() != VoiceUserType_GM )
			{
				client.SendChatMsg("你没有操作用户列表的权限！");
				break ;
			}

			bool bOpen = pMsg->dwUserData1 > 0 ? true : false;

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在
			if( pRoom != NULL )
			{
				CChannel & channel = pRoom->GetChannel();

				// 如果是关闭，则要设置标记，否则取消标记
				channel.SetFlag( VoiceChannelState_NoUserList, !bOpen );

				// 如果是打开用户列表，要清除所有的观察者
				// 等待客户端的下次观察者请求上来后，就可以把所有的用户列表发给客户端
				if( bOpen )
				{
					channel.ClearObserver();
				}
			}
			else
			{
				client.SendChatMsg("你不在房间里！");
			}

		}
		break;
	
	case VoiceSubMsg_OpenChannel:					// 开启频道
		{
			// 判断权限
			if( client.GetType() != VoiceUserType_GM )
			{
				client.SendChatMsg("你没有管理频道的权限！");
				break ;
			}
			
			bool bOpen = pMsg->dwUserData1 > 0 ? true : false;

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在
			if( pRoom != NULL )
			{
				CChannel & channel = pRoom->GetChannel();

				if( bOpen )
				{
					// 已经开启
					if( !(channel.GetFlag(VoiceChannelState_Close)) )
					{
						client.SendChatMsg("频道已开启，请不要重复操作！");
						break;
					}

					// 关闭定时器
					channel.StopCloseTimer();

					// 打开频道，设置标记即可
					channel.SetFlag( VoiceChannelState_Close, false );
				}
				else	// 如果是关闭频道，提示频道里所有玩家，并且启动踢人定时器
				{
					// 已经关闭了，不再重新关闭
					if( channel.GetFlag(VoiceChannelState_Close) )
					{
						client.SendChatMsg("频道已关闭，或者正在关闭中！");
						break;
					}

					// 拷贝信息
					char szCloseInfo[VOICE_TITLE_MAXSIZE] = {0};
					if( nNewlen + 1 > sizeof(szCloseInfo) )
					{
						break;
					}

					sstrcpyn( szCloseInfo, pMsgData, nNewlen + 1 );

					if( strlen(szCloseInfo) <= 0 )
					{
						client.SendChatMsg("请填写关闭频道理由！");
						break;
					}

#ifdef VOICE_PRINT_DEBUG_INF
					ErrorLn(_GT("GlobalVoiceServer::OnMsgClientVoiceSubMsg 关闭频道，关闭信息=") << szCloseInfo << _GT("，踢人延时时间=") << (pMsg->dwUserData2) );
#endif

					// 设置关闭信息
					channel.SetCloseInfo( szCloseInfo, pMsg->dwUserData2 );

					// 打开频道，设置标记即可
					channel.SetFlag( VoiceChannelState_Close, true );

					// 关闭频道关闭定时器
					channel.StartCloseTimer();
				}
			}
			else
			{
				client.SendChatMsg("你不在房间里！");
			}

		}
		break;

	case VoiceSubMsg_Chat:
		{	
			char szText[VOICE_LONGSTR_MAXSIZE] = {0};
			if( nNewlen + 1 > sizeof(szText) )
			{
				break;
			}
			
			sstrcpyn( szText, pMsgData, nNewlen + 1 );

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在
			if( pRoom != NULL )
			{
				// 如果是私聊 则不走冷却
				if ( pMsg->dwUserData2  > 0 )
				{
					pRoom->sendChatMessage( client, pMsg->dwUserData1, szText, pMsg->dwUserData2 );
				}
				// 判断冷却
				else 
				{
					// 房间已关闭文字聊天
					if( pRoom->GetFlag(VoiceRoomState_NoChat) )
					{
						client.SendChatMsg( "本房间已禁止文字聊天！", VoiceInfoPos_ChatWindow|VoiceInfoPos_Rosponse);
					}
					else
					{
						DWORD dwTimeLeft = 0;
						if( client.CanChat( *pRoom, dwTimeLeft ) )
						{
							DWORD dwBanTimeLeft = 0;
							LONGLONG nChannelID = pRoom->GetChannel().GetID();

							// 要判断是不是被禁言，如果禁言了，只发给自己
							bool bBan = IsPDBIDBan( nChannelID, client.GetUserInfo().dwPDBID, dwBanTimeLeft );
							
							pRoom->sendChatMessage( client, pMsg->dwUserData1, szText, pMsg->dwUserData2, bBan );
						}
						else
						{
							char szText[128] = {0};
							sprintf_s( szText, sizeof(szText), "您发送消息过于频繁，请在%s后再发！", GetRemainTimeString(dwTimeLeft) );
							client.SendChatMsg(szText, VoiceInfoPos_ChatWindow|VoiceInfoPos_Rosponse);
						}
					}
				}
			}
		}
		break;

	case VoiceSubMsg_VoiceList:		// 正在语音列表
		{
			// 判断权限
			if( client.GetType() != VoiceUserType_GM )
			{
				client.SendChatMsg("你没有权限！");
				break ;
			}

			DWORD dwCount = (pMsg->dwUserData1 <= 0) ? 100 : pMsg->dwUserData1;
			DWORD dwSecond = (pMsg->dwUserData2 <= 0 || pMsg->dwUserData2 > 3600) ? 5 : pMsg->dwUserData2;
			CClientList::getInstance().UpdateVoiceListToClient( client, dwCount, dwSecond );
		}
		break;

	case VoiceSubMsg_ChatList:	// 私聊列表
		{
			// 判断权限
			if( client.GetType() != VoiceUserType_GM )
			{
				client.SendChatMsg("你没有权限！");
				break ;
			}

			CChatList::getInstance().UpdateChatListToClient(client.GetClientID());
		}
		break;

	case VoiceSubMsg_EnterRoomByPDBID:	// 进入他的个人频道
		{
			CClientUser *pOtherUser = CClientList::getInstance().GetClientByPDBID(pMsg->dwUserData1);
			if( pOtherUser == NULL )
			{
				client.SendChatMsg("此用户未登录语音系统");
				break;
			}
			else
			{
				client.EnterRoomByUser( *pOtherUser );
			}
		}
		break;

	case VoiceSubMsg_AddVest:		// 添加马甲（临时管理）
		{
			client.AddVest( pMsg->dwUserData1, pMsg->dwUserData2 > 0 ? true : false, pMsg->dwUserData3 );
		}
		break;

	case VoiceSubMsg_GetVestList:		// 获取马甲（临时管理）列表
		{
			CRoom *pRoom = client.GetCurRoom();
			if( NULL != pRoom )
			{
				pRoom->GetChannel().UpdateChannelVestListToClient( client.GetClientID(), VoiceUpdateType_All );
			}
		}
		break;

	case VoiceSubMsg_SetPersonalChannelName:	// 修改个人频道名称
		{
			CRoom *pRoom = client.GetCurRoom();
			if( NULL != pRoom )
			{
				if( pRoom->GetType() != VoiceChannel_Personal )
				{
					client.SendChatMsg( "请先进入您的个人频道！" );
					break;
				}

				if( !client.GetRight(VoiceRight_Owner) )
				{
					client.SendChatMsg( "请先进入您的个人频道！" );
					return;
				}

				char szChannelName[VOICE_NAME_MAXSIZE] = {0};
				if( nNewlen + 1 > sizeof(szChannelName) )
				{
					break;
				}

				sstrcpyn( szChannelName, pMsgData, nNewlen + 1 );
#ifdef VOICE_PRINT_DEBUG_INF
				TraceLn("GlobalVoiceServer::OnMsgClientVoiceSubMsg VoiceSubMsg_SetPersonalChannelName 新频道名称=" << szChannelName);
#endif
				
				if( strlen(szChannelName) <= 0 )
				{
					client.SendChatMsg( "新名称不能为空！" );
					break;
				}

				pRoom->GetChannel().SetOwnerName(szChannelName);
			}
		}
		break;

	case VoiceSubMsg_SetPersonalChannelPublic:	// 设置个人频道公开，参加排行榜
		{
			CRoom *pRoom = client.GetCurRoom();
			if( NULL != pRoom )
			{
				if( pRoom->GetType() != VoiceChannel_Personal )
				{
					client.SendChatMsg( "请先进入您的个人频道！" );
					return;
				}

				if( !client.GetRight(VoiceRight_Owner) )
				{
					client.SendChatMsg( "请先进入您的个人频道！" );
					return;
				}

				pRoom->GetChannel().SetFlag( VoiceChannelState_Public, pMsg->dwUserData1 > 0 ? true : false );
			}
		}
		break;

	case VoiceSubMsg_SendPersonalConfig:	// 上传个人频道配置
		{
			//char szChannelName[16] = {0};
			//if( nNewlen + 1 > sizeof(szChannelName) )
			//{
			//	break;
			//}

			//sstrcpyn( szChannelName, pMsgData, nNewlen + 1 );

			//CRoom *pRoom = client.GetCurRoom();
			//if( NULL != pRoom )
			//{
			//	if( pRoom->GetType() == VoiceChannel_Personal )
			//	{
			//		if( client.GetType() != VoiceUserType_OW )	// 如果不是频道的OW
			//		{
			//			break;
			//		}

			//		CChannel & channel = pRoom->GetChannel();
			//		// 设置公开状态
			//		channel.SetFlag( VoiceChannelState_Public, pMsg->dwUserData1 > 0 ? true : false );

			//		if( strlen(szChannelName) > 0 )
			//		{
			//			channel.SetOwnerName(szChannelName, false, false);
			//		}
			//	}
			//}
		}
		break;

	case VoiceSubMsg_RequestPersonalTopList:	// 请求个人频道排行榜
		{
			CChannelList & channelList = CChannelList::getInstance();
			if( pMsg->dwUserData1 != channelList.GetPersonalTopListUpdateTime() )	// 如果请求来的时间和本地生成时间不相等，才发列表
			{
				channelList.UpdatePersonalTopListToClient(client.GetClientID());
			}
			
		}
		break;

	case VoiceSubMsg_ChatInviteResponse:		// 私聊邀请回应
		{
			client.ChatInviteResponse( pMsg->dwUserData1, pMsg->dwUserData2 > 0 ? true : false );
		}
		break;

	case VoiceSubMsg_ChatClose:		// 关闭私聊
		{
			client.LeaveChat();
		}
		break;

	case VoiceSubMsg_SetChatConfig:	// 私聊设置
		{
			client.SetChatConfig(pMsg->dwUserData1);
		}
		break;

	case VoiceSubMsg_GMEnterChat:		// GM旁听私聊
		{
			// 判断权限
			if( client.GetType() != VoiceUserType_GM )
			{
				client.SendChatMsg("你没有权限！");
				break ;
			}

			CChat * pChat = CChatList::getInstance().Get(pMsg->dwUserData1);
			if( NULL == pChat )
			{
				client.SendChatMsg("该私聊不存在");
				break;
			}

			pChat->AddVisitor(client);
		}
		break;

	case VoiceSubMsg_ShareMic:		// 连麦
		{
			// 判断权限
			if( !client.GetRight(VoiceRight_Mic) )
			{
				client.SendChatMsg("你没有设置连麦权限！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			// 房间存在，且为麦序模式
			if( pRoom != NULL && VoiceMicState_MicOrder == pRoom->GetMicState() )
			{
				pRoom->ShareMic( pMsg->dwUserData1, pMsg->dwUserData2 > 0 ? true : false  );
			}
		}
		break;

	case VoiceSubMsg_AddAtten:		// 添加/取消关注
		{
			CRoom * pRoom = client.GetCurRoom();
			if( pRoom == NULL )
			{
				break;
			}

			CClientUser * pMicUser = CClientList::getInstance().GetUser(pMsg->dwUserData1);
			if( NULL != pMicUser )
			{
				bool bAdd = pMsg->dwUserData2 > 0 ? true : false;
				if( bAdd )
				{
					client.AddFansUser(pMicUser->GetClientID());
				}
				else
				{
					if( client.DelFansUser(pMicUser->GetClientID()) )
					{
						// 如果需要计数
						bool bCount = pMsg->dwUserData3 > 0 ? true : false;
						if( bCount )
						{
							pMicUser->AddFansCancelCount();
						}
					}
				}
			}
		}
		break;

	case VoiceSubMsg_CallFans:		// 召集粉丝
		{
			char szText[VOICE_TEXTSTR_MAXSIZE] = {0};
			if( nNewlen + 1 > sizeof(szText) )
			{
				break;
			}
			
			sstrcpyn( szText, pMsgData, nNewlen + 1 );
			client.CallFans( szText, pMsg->dwUserData1 > 0 ? true : false,pMsg->dwUserData2,pMsg->dwUserData3 );
		}
		break;

	case VoiceSubMsg_RequestFansOnlineCount:	// 请求在线粉丝数量
		{
			client.SendFansOnlineCount();
		}
		break;

	case VoiceSubMsg_AcceptFansCall:		// 接受粉丝召集
		{
			CClientUser * pUser = CClientList::getInstance().GetUser(pMsg->dwUserData1);
			if( NULL != pUser )
			{
				pUser->AddFansAcceptCount();
			}
		}
		break;

	case VoiceSubMsg_RequestFansCallResult:			// 请求召集粉丝结果
		{
			client.SendFansCallResult();
		}
		break;

	case VoiceSubMsg_SetRoomName:				// 设置房间名称
		{
			DWORD dwRoomID = pMsg->dwUserData1;

			if( nNewlen + 1 > VOICE_NAME_MAXSIZE )
			{
				break;
			}

			char szNewName[VOICE_NAME_MAXSIZE] = {0};
			sstrcpyn( szNewName, pMsgData, nNewlen + 1 );

			CRoom * pRoom = client.GetCurRoom();
			if( NULL == pRoom )
			{
				ErrorLn( _GT("VoiceSubMsg_SetRoomLock 当前房间对象为空？ClientID=") << client.GetClientID() << _GT("，当前房间ID") << client.GetCurRoomID() );
				break ;
			}

			if( !client.GetRight(VoiceRight_Owner) )
			{
				client.SendChatMsg("你没有修改房间名称权限！");
				break ;
			}

			CRoom * pUpdateRoom = pRoom->GetChannel().GetRoomPtr(dwRoomID);
			if( pUpdateRoom == NULL )
			{
				client.SendChatMsg("不能操作一个无效的房间！");
				break ;
			}

			pUpdateRoom->SetName(szNewName);
		}
		break;

	case VoiceSubMsg_SetChannelMaxCount:	// 设置频道人数上限
		{
			// 判断权限
			if( client.GetType() != VoiceUserType_GM )
			{
				client.SendChatMsg("你没有设置频道人数上限权限！");
				break ;
			}

			CRoom * pRoom = client.GetCurRoom();
			if( NULL != pRoom )
			{
				pRoom->GetChannel().SetMaxCount(pMsg->dwUserData1);
				char szMsg[128] = {0};
				sprintf_s( szMsg, sizeof(szMsg), "成功设置当前频道最大人数上限为%d", pMsg->dwUserData1 );
				client.SendChatMsg(szMsg);
			}
		}
		break;

	case VoiceSubMsg_SetRoomChatCoolDown:		// 设置房间文字聊天冷却时间
		{
			if( !client.GetRight(VoiceRight_Lock) )
			{
				client.SendChatMsg("你没有设置房间文字聊天冷却时间的权限！");
				break;
			}

			CRoom * pRoom = client.GetCurRoom();
			if( NULL != pRoom )
			{
				CChannel & channel = pRoom->GetChannel();
				CRoom * pUpdateRoom = channel.GetRoomPtr(pMsg->dwUserData1);
				if( NULL == pUpdateRoom )
				{
					client.SendChatMsg("你操作的房间无效！");
					break;
				}

				DWORD dwCoolDown = pUpdateRoom->SetChatCoolDown(pMsg->dwUserData2);
				char szMsg[128] = {0};
				sprintf_s( szMsg, sizeof(szMsg), "管理员设置房间文字聊天速度，每句间隔为%d秒。", dwCoolDown );
				channel.SendChatMsg( szMsg, pUpdateRoom->GetID() );
			}
		}
		break;

	case VoiceSubMsg_CloseRoomChat:		// 关闭/开启房间文字聊天
		{
			if( !client.GetRight(VoiceRight_Lock) )
			{
				client.SendChatMsg("你没有关闭/开启房间文字聊天的权限！");
				break;
			}

			CRoom * pRoom = client.GetCurRoom();
			if( NULL != pRoom )
			{
				CChannel & channel = pRoom->GetChannel();
				CRoom * pUpdateRoom = channel.GetRoomPtr(pMsg->dwUserData1);
				if( NULL == pUpdateRoom )
				{
					client.SendChatMsg("你操作的房间无效！");
					break;
				}

				bool bClose = pMsg->dwUserData2 > 0 ? true : false;

				pUpdateRoom->SetFlag( VoiceRoomState_NoChat, bClose );

				if( bClose )
				{
					channel.SendChatMsg( "管理员已关闭本房间文字聊天", pUpdateRoom->GetID() );
				}
				else
				{
					channel.SendChatMsg( "管理员已开启本房间文字聊天", pUpdateRoom->GetID() );
				}
			}
		}
		break;

	case VoiceSubMsg_SetChannelQuality:	// 设置频道语音品质
		{
			// 判断权限
			//if( client.GetType() != VoiceUserType_GM )
			//{
			//	client.SendChatMsg("你没有设置频道语音品质权限！");
			//	break ;
			//}
			// 只有主人才能设置频道语音品质
			if( !client.GetRight(VoiceRight_Owner) )
			{
				client.SendChatMsg("你没有设置频道语音品质权限！");
				break;
			}

			CRoom * pRoom = client.GetCurRoom();
			if( NULL != pRoom )
			{
				bool bOK = pRoom->GetChannel().SetQuality(pMsg->dwUserData1);
				if (bOK)
				{
					char szMsg[128] = {0};
					sprintf_s( szMsg, sizeof(szMsg), "成功设置频道语音品质!");
					client.SendChatMsg(szMsg);
				}
			}
		}
		break;

	case VoiceSubMsg_RequestDiamondInfo:	// 请求蓝钻等信息
		{
		}
		break;

	case VoiceSubMsg_RequestFansTopList:		// 粉丝排行榜
		{
			if( pMsg->dwUserData1 != m_dwFansTopListUpdateTime )	// 如果时间不一致，则发送排行榜列表
			{
				UpdateFansTopListToClient( client.GetClientID() );
			}
		}
		break;

	default:
		{
			ErrorLn(_GT("有一个客户端信息子消息未处理，dwMsgCode = ")<< pMsg->dwMsgCode);
		}
		break;
	}	

}

// 客户端发来语音数据
void GlobalVoiceServer::OnMsgClientVoiceData( CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len )
{
	DWORD dwFromClientID = client.GetClientID();

	// 可以在频道里说话
	if( client.CanSpeak() )
	{
		// 得到房间对象
		CRoom * pRoom = client.GetCurRoom();
		if( NULL != pRoom )
		{
			CChannel & channel = pRoom->GetChannel();

			// 最后语音Tick时间,用来处理自由抢麦处理老不说话的人,以空出位置给别人说
			DWORD dwNow = ::GetTickCount();
			client.m_dwLastAudioTime = dwNow;
			pRoom->m_dwLastAudioTime = dwNow;

			// 30秒钟检测一次
			if( dwNow >= channel.m_dwAudioCheckTime + 30000 )
			{
				channel.m_dwAudioCheckTime = dwNow;
				channel.OnAudioCheck(dwNow, pRoom->GetID());
			}
			channel.m_dwLastAudioTime = dwNow;

			// 记录统战发言时间
			SVoiceUserInfo& refInfo = client.GetUserInfo();

			// 是否频道广播
			if( client.NeedBroadcast() )
			{
				DWORD dwCount = 0;		// 人数
				channel.GetClientIDList( m_dwClientIDBuff, VOICE_MAX_ROOM_USERCOUNT, dwCount, true );

				if( dwCount > 0 )
				{
					char *pSendData = (char *)head;
					// 直接修改游戏头用来转发
					head->SrcEndPoint = MSG_ENDPOINT_VOICE;
					head->DestEndPoint= MSG_ENDPOINT_CLIENT;
					head->byKeyModule  = MSG_MODULEID_VOICE;
					head->byKeyAction  = MSG_VOICE_VOICEDATA;

					DWORD dwDataLen = len + sizeof(SGameMsgHead);

					// 广播,要排除自己 用语音通道发送
					BroadcastDataToClient( m_dwClientIDBuff, dwCount, pSendData, dwDataLen, dwFromClientID, VoiceSendChannel_Voice );
				}
			}
			else
			{
				DWORD *pClientArray = NULL;
				DWORD dwCount = 0;		// 人数

#ifdef VOICE_CLIENTID_BUFFLIST	// 使用用户列表缓冲机制
				// 获取广播语音用户ID列表，内部有缓存机制，有变化才重新计算列表，提高效率
				pClientArray = (DWORD *)(pRoom->GetAudioList(dwCount));
#else
				pClientArray = m_dwClientIDBuff;
				// 获取房间里所有用户ID列表 使用内部专用缓冲m_dwClientIDBuff
				pRoom->GetUserIDList( pClientArray, VOICE_MAX_ROOM_USERCOUNT, dwCount, true );
#endif

				if( dwCount > 0 )
				{
					char *pSendData = (char *)head;
					// 直接修改游戏头用来转发
					head->SrcEndPoint = MSG_ENDPOINT_VOICE;
					head->DestEndPoint= MSG_ENDPOINT_CLIENT;
					head->byKeyModule  = MSG_MODULEID_VOICE;
					head->byKeyAction  = MSG_VOICE_VOICEDATA;

					DWORD dwDataLen = len + sizeof(SGameMsgHead);

					// 广播,要排除自己 用语音通道发送
					BroadcastDataToClient( pClientArray, dwCount, pSendData, dwDataLen,dwFromClientID, VoiceSendChannel_Voice );
				}
			}
		}
	}

	// 如果可以私聊
	if( client.CanPrivateChat() )
	{
		CChat * pChat = client.GetChatInstance();
		if( NULL != pChat )
		{
			DWORD *pClientArray = NULL;
			DWORD dwCount = 0;		// 人数

			pClientArray = m_dwClientIDBuff;

			pChat->GetClientIDList( pClientArray, VOICE_MAX_ROOM_USERCOUNT, dwCount );

			if( dwCount > 0 )
			{
				char *pSendData = (char *)head;
				// 直接修改游戏头用来转发
				head->SrcEndPoint = MSG_ENDPOINT_VOICE;
				head->DestEndPoint= MSG_ENDPOINT_CLIENT;
				head->byKeyModule  = MSG_MODULEID_VOICE;
				head->byKeyAction  = MSG_VOICE_VOICEDATA;

				DWORD dwDataLen = len + sizeof(SGameMsgHead);

				// 广播,要排除自己 用语音通道发送
				BroadcastDataToClient( pClientArray, dwCount, pSendData, dwDataLen,dwFromClientID, VoiceSendChannel_Voice );
			}	
		}
	}
}



// 更新服务器列表,dwServerID:发向的服务器ID,为0时广播到所有服务器,nUpdateType:更新类型 EMVoiceServerListUpdateType,dwUpdateServerID:要更新信息的服务器ID
void GlobalVoiceServer::UpdateServerList(DWORD dwServerID,BYTE nUpdateType,DWORD dwUpdateServerID)
{
	ServerList::SERVER_LIST &mapServerList = ServerList::getInstance().m_list;

	// 更新类型,EMVoiceServerListUpdateType
	switch(nUpdateType)
	{
	case VS_SvrListUpdateType_All:		// 全部
		{
			obuf4096 obufData;

			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
			header.DestEndPoint= MSG_ENDPOINT_VOICE;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_SERVERLIST_UPDATE;

			// 服务器列表更新数据结构
			SMsgVoiceServerListUpdate sendData;
			sendData.nUpdateType		= nUpdateType;		// 更新类型,EMVoiceServerListUpdateType
			sendData.dwServerCounts		= mapServerList.size();		// 软件服务器个数

			obufData.push_back(&header, sizeof(SGameMsgHead));
			obufData.push_back(&sendData, sizeof(SMsgVoiceServerListUpdate));

			ServerUser * pServerUser = NULL;
			DWORD dwListCouts = 0;
			for(ServerList::SERVER_LIST::iterator iter = mapServerList.begin(); iter!= mapServerList.end();++iter)
			{
				pServerUser = iter->second;
				if (NULL==pServerUser)
				{
					continue;
				}
				// 没有握手好的不发
				if (0==pServerUser->m_dwWorldID||0==pServerUser->m_wServerType)
				{
					continue;
				}
				// 游戏服务器信息数据结构
				SMsgVoiceGameServerNode nodeInfo;
				nodeInfo.dwID			= pServerUser->GetServerID();			// 服务器ID
				nodeInfo.dwWorldID		= pServerUser->m_dwWorldID;				// 服务器所在游戏世界ID
				nodeInfo.wServerType	= pServerUser->m_wServerType;			// 服务器类型
				nodeInfo.wSubID			= pServerUser->m_wSubID;				// 第n号服务器
				nodeInfo.dwStateFlag	= pServerUser->m_dwStateFlag;			// 服务器状态

				obufData.push_back(&nodeInfo, sizeof(SMsgVoiceGameServerNode));

				dwListCouts++;
			}

			// 更新正确的个数
			SMsgVoiceServerListUpdate * pMsgData = (SMsgVoiceServerListUpdate *)(obufData.data()+sizeof(SGameMsgHead));
			pMsgData->dwServerCounts = dwListCouts;

			if (dwServerID==0)
			{
				// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
				BroadcastToServer(NULL,VOICE_BROADCASTALL_NUM,obufData.data(),obufData.size());
			} 
			else
			{
				// 发送数据给指定服务器
				SendDataToServer(dwServerID,obufData.data(),obufData.size());
			}

		}
		break;
	case VS_SvrListUpdateType_Add:		// 新增
	case VS_SvrListUpdateType_Edit:		// 修改
		{
			ServerUser * pServerUser =ServerList::getInstance().GetUser(dwUpdateServerID);

			if (NULL==pServerUser)
			{
				break;
			}

			// 没有握手好的不发
			if (0==pServerUser->m_dwWorldID||0==pServerUser->m_wServerType)
			{
				break;
			}

			obuf obufData;

			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
			header.DestEndPoint= MSG_ENDPOINT_VOICE;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_SERVERLIST_UPDATE;

			// 服务器列表更新数据结构
			SMsgVoiceServerListUpdate sendData;
			sendData.nUpdateType		= nUpdateType;		// 更新类型,EMVoiceServerListUpdateType
			sendData.dwServerCounts		= 1;				// 软件服务器个数

			obufData.push_back(&header, sizeof(SGameMsgHead));
			obufData.push_back(&sendData, sizeof(SMsgVoiceServerListUpdate));

			// 游戏服务器信息数据结构
			SMsgVoiceGameServerNode nodeInfo;
			nodeInfo.dwID			= pServerUser->GetServerID();			// 服务器ID
			nodeInfo.dwWorldID		= pServerUser->m_dwWorldID;				// 服务器所在游戏世界ID
			nodeInfo.wServerType	= pServerUser->m_wServerType;			// 服务器类型
			nodeInfo.wSubID			= pServerUser->m_wSubID;				// 第n号服务器
			nodeInfo.dwStateFlag	= pServerUser->m_dwStateFlag;			// 服务器状态

			obufData.push_back(&nodeInfo, sizeof(SMsgVoiceGameServerNode));

			if (dwServerID==0)
			{
				// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
				BroadcastToServer(NULL,VOICE_BROADCASTALL_NUM,obufData.data(),obufData.size());
			} 
			else
			{
				// 发送数据给指定服务器
				SendDataToServer(dwServerID,obufData.data(),obufData.size());
			}
		}
		break;
	case VS_SvrListUpdateType_Del:		// 删除
		{
			obuf obufData;

			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
			header.DestEndPoint= MSG_ENDPOINT_VOICE;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_SERVERLIST_UPDATE;

			// 服务器列表更新数据结构
			SMsgVoiceServerListUpdate sendData;
			sendData.nUpdateType		= nUpdateType;		// 更新类型,EMVoiceServerListUpdateType
			sendData.dwServerCounts		= 1;				// 软件服务器个数

			obufData.push_back(&header, sizeof(SGameMsgHead));
			obufData.push_back(&sendData, sizeof(SMsgVoiceServerListUpdate));
			obufData.push_back(&dwUpdateServerID, sizeof(DWORD));

			if (dwServerID==0)
			{
				// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
				BroadcastToServer(NULL,VOICE_BROADCASTALL_NUM,obufData.data(),obufData.size());
			} 
			else
			{
				// 发送数据给指定服务器
				SendDataToServer(dwServerID,obufData.data(),obufData.size());
			}
		}
		break;
	}
}

// 更新游戏世界列表,dwServerID:发向的服务器ID,为0时广播到所有服务器,nUpdateType:更新类型 EMVoiceServerListUpdateType,dwUpdateWorldID:要更新信息的游戏世界ID
void GlobalVoiceServer::UpdateWorldList(DWORD dwServerID,BYTE nUpdateType,DWORD dwUpdateWorldID)
{
	// 更新类型,EMVoiceServerListUpdateType
	switch(nUpdateType)
	{
	case VS_SvrListUpdateType_All:		// 全部
		{
			obuf1024 obufData;
			obuf1024 obufNameData;

			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
			header.DestEndPoint= MSG_ENDPOINT_VOICE;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_WORLDLIST_UPDATE;

			// 游戏世界列表更新数据结构
			SMsgVoiceWorldListUpdate sendData;
			sendData.nUpdateType		= nUpdateType;		// 更新类型,EMVoiceServerListUpdateType
			sendData.dwWorldCounts		= m_mapWorldList.size();		// 游戏世界个数

			obufData.push_back(&header, sizeof(SGameMsgHead));
			obufData.push_back(&sendData, sizeof(SMsgVoiceWorldListUpdate));

			DWORD dwListCouts = 0;
			for(TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.begin(); iter!= m_mapWorldList.end();++iter)
			{
				// 游戏世界信息数据结构
				SMsgVoiceGameWorldNode nodeInfo;
				nodeInfo.dwWorldID		= iter->second.dwWorldID;				// 游戏世界ID
				nodeInfo.nNameStrlens	= iter->second.strWorldName.size();		// 游戏世界名称字串长度
				obufData.push_back(&nodeInfo, sizeof(SMsgVoiceGameWorldNode));
				obufNameData.push_back(iter->second.strWorldName.c_str(),nodeInfo.nNameStrlens);
				dwListCouts++;
			}
			// 游戏世界名称字串
			if (obufNameData.size()>0)
			{
				obufData.push_back(obufNameData.data(),obufNameData.size());
			}

			// 更新正确的个数
			SMsgVoiceWorldListUpdate * pMsgData = (SMsgVoiceWorldListUpdate *)(obufData.data()+sizeof(SGameMsgHead));
			pMsgData->dwWorldCounts = dwListCouts;

			if (dwServerID==0)
			{
				// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
				BroadcastToServer(NULL,VOICE_BROADCASTALL_NUM,obufData.data(),obufData.size());
			} 
			else
			{
				// 发送数据给指定服务器
				SendDataToServer(dwServerID,obufData.data(),obufData.size());
			}

		}
		break;
	case VS_SvrListUpdateType_Add:		// 新增
	case VS_SvrListUpdateType_Edit:		// 修改
		{
			TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.find(dwUpdateWorldID);
			if (iter==m_mapWorldList.end())
			{
				break;
			}

			obuf obufData;

			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
			header.DestEndPoint= MSG_ENDPOINT_VOICE;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_WORLDLIST_UPDATE;

			// 游戏世界列表更新数据结构
			SMsgVoiceWorldListUpdate sendData;
			sendData.nUpdateType		= nUpdateType;		// 更新类型,EMVoiceServerListUpdateType
			sendData.dwWorldCounts		= 1;				// 游戏世界个数

			obufData.push_back(&header, sizeof(SGameMsgHead));
			obufData.push_back(&sendData, sizeof(SMsgVoiceWorldListUpdate));

			// 游戏世界信息数据结构
			SMsgVoiceGameWorldNode nodeInfo;
			nodeInfo.dwWorldID		= iter->second.dwWorldID;				// 游戏世界ID
			nodeInfo.nNameStrlens	= iter->second.strWorldName.size();		// 游戏世界名称字串长度
			obufData.push_back(&nodeInfo, sizeof(SMsgVoiceGameWorldNode));
			obufData.push_back(iter->second.strWorldName.c_str(),nodeInfo.nNameStrlens);

			if (dwServerID==0)
			{
				// 广播数据给服务器 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
				BroadcastToServer(NULL,VOICE_BROADCASTALL_NUM,obufData.data(),obufData.size());
			} 
			else
			{
				// 发送数据给指定服务器
				SendDataToServer(dwServerID,obufData.data(),obufData.size());
			}
		}
		break;
	}
}


// 更新世界列表到客户端
void GlobalVoiceServer::UpdateWorldListToClient( DWORD * pClientArray, DWORD wClientNum )
{
	if( pClientArray == NULL || wClientNum <= 0 )
	{
		return ;
	}

	obuf1024 obufData;
	obuf1024 obufName;

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_WORLDLIST;

	SMsgVoiceWorldList_VC listData;
	listData.dwCount = m_mapWorldList.size();

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&listData, sizeof(SMsgVoiceWorldList_VC));

	for( TMAP_VoiceWorldIDInfo::iterator it = m_mapWorldList.begin(); it != m_mapWorldList.end(); ++it  )
	{
		SVoiceWorldNode & info = it->second;
		SMsgVoiceWorldNode_VC nodeInfo;
		nodeInfo.dwID = info.GetWorldID();
		nodeInfo.nNameLen = strlen(info.GetWorldName());

#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("ID=" << nodeInfo.dwID << ", name=" << info.GetWorldName() );
#endif

		obufData.push_back(&nodeInfo, sizeof(SMsgVoiceWorldNode_VC));

		// 把名字存起来
		obufName.push_back( info.GetWorldName(), nodeInfo.nNameLen );
	}

	if( obufName.size() > 0 )
	{
		obufData.push_back( obufName.data(), obufName.size() );
	}

	if( wClientNum == 1 )
	{
		SendDataToClient( pClientArray[0], obufData.data(), obufData.size() );
	}
	else
	{
		BroadcastDataToClient( pClientArray, wClientNum, obufData.data(), obufData.size() );
	}
}

// 更新粉丝排行榜到客户端
void GlobalVoiceServer::UpdateFansTopListToClient( DWORD dwClientID )
{
	if( dwClientID <= 0 )
	{
		return;
	}

	// 使用全局缓冲内存
	GlobalVoiceServer* pVoiceServer = (GlobalVoiceServer*)gGlobalServer;
	ofixbuf obufData(pVoiceServer->m_szDataBuf, sizeof(pVoiceServer->m_szDataBuf));
	ofixbuf obufName(pVoiceServer->m_szNameBuf, sizeof(pVoiceServer->m_szNameBuf));

	// 组个头
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_FANSTOPLIST;		// 粉丝排行榜
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	SMsgVoiceFansTopList_VC listData;
	listData.dwUpdateTime = m_dwFansTopListUpdateTime;
	listData.nCount = (BYTE)m_FansTopList.size();
	obufData.push_back( &listData, sizeof(SMsgVoiceFansTopList_VC) );

	for( list<SVoiceFansTopListData>::iterator it = m_FansTopList.begin(); it != m_FansTopList.end(); ++it  )
	{
		SVoiceFansTopListData & topData = *it;

		SMsgVoiceFansTopNode_VC nodeData;
		nodeData.dwUDBID = topData.dwUDBID;						// 通行证UDBID
		nodeData.nUserType = (BYTE)topData.dwUserType;						// 通行证帐号类型
		nodeData.nRank = (BYTE)topData.dwRank;							// 名次
		nodeData.dwFansCount = topData.dwFansCount;					// 粉丝总数
		nodeData.dwFromWorldID = (WORD)topData.dwFromWorldID;					// 最近登录的角色来源世界ID
		nodeData.nCountryID = (BYTE)topData.dwCountryID;					// 最近登录的角色所属ID(国家，大陆。。)
		nodeData.nNameLen = strlen(topData.szActorName);

		obufData.push_back( &nodeData, sizeof(SMsgVoiceFansTopNode_VC) );

		if( nodeData.nNameLen > 0 )	// 名称
		{
			obufName.push_back( topData.szActorName, nodeData.nNameLen );
		}
	}

	obufData.push_back( obufName.data(), obufName.size() );

	// 发送
	gGlobalServer->SendDataToClient( dwClientID, obufData.data(), obufData.size() );
}

// 定时检测是否可以关闭语音服务器
void GlobalVoiceServer::OnTimerCheckShutDown(void)
{
	// 如果不是关机，直接返回
	if( !m_bShutDowning )
	{
		return;
	}

	// 是否有数据没保存
	bool bIsSaveFinished = true;
	
	// 有一个频道没有保存数据，继续等待
	CChannelList::CHANNEL_LIST & channelList = CChannelList::getInstance().GetList();
	for( CChannelList::CHANNEL_LIST::iterator it = channelList.begin(); it != channelList.end(); ++ it )
	{
		CChannel * pChannel = it->second;
		if( NULL == pChannel )
		{
			continue;
		}

		// 先调数据保存，保证有数据变化会保存
		pChannel->OnTimerCheckSaveDB();

		if( !pChannel->IsDBSaved() )
		{
			bIsSaveFinished = false;
		}
	}

	// 后期扩展可用否决事件来提醒保存

	// 如果有数据未保存
	if( !bIsSaveFinished )
	{
		WarningLn(_GT("正在保存数据库..."));
		return;
	}

	if( !GameDatabaseAccessor::getInstance().IsDBEngineQueueEmpty() )
	{
		WarningLn(_GT("数据库引擎请求队列有数据未保存，请等待..."));
		return;
	}
	
	WarningLn(_GT("保存数据库全部完成！请等待窗口关闭。"));
	
	KillTimer( ETimerEventID_WaitShutDown, this );
	SetTimer( ETimerEventID_WindowClose, 10000, this, "等待关闭语音服务器窗口", 1 );
}


void GlobalVoiceServer::LoadMemoryDB(void* pDataBuff, size_t nSize)
{
    Trace(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl);
    Trace(_GT("开始创建数据库引擎... ...") << endl);
    if(!GameDatabaseAccessor::getInstance().Create(MSG_ENDPOINT_VOICE, pDataBuff, nSize, "DBVoiceProcedure.dbs") )
    {
        Error(_GT("创建数据库引擎失败... ...") << endl);			
        return;
    }
    Trace(_GT("创建数据库引擎成功... ...") << endl);
    Trace(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl << endl);

    SetTimer( ETimerEventID_VoiceControl, 2000, this, "GlobalVoiceServer::Create" );
    SetTimer( ETimerEventID_VoiceNetCount, 1000, this, "GlobalVoiceServer::Create" );
    SetTimer( ETimerEventID_CheckBanPDBID, 60000, this, "GlobalVoiceServer::Create" );		// 一分钟一次
    SetTimer( ETimerEventID_CheckCallFansInfo, 600000, this, "GlobalVoiceServer::Create" );	// 10分钟一次
    SetTimer( ETimerEventID_DBStart, 1000, this, "GlobalVoiceServer::Create",1);			// 一秒后初始化数据库
    SetTimer( ETimerEventID_GetFansTopList, 3600000, this, "GlobalVoiceServer::Create" );	//从数据库读取粉丝排行榜 1个小时一次
    SetTimer( ETimerEventID_CheclAD, 600000, this, "GlobalVoiceServer::Create" );			// 10分钟一次

    SetTimer( ETimerEventID_CheckUBCommanderSpeak, 30000, this, "GlobalVoiceServer::Create" );	// 30秒一次 检测统战发言


    CChannelList::getInstance().Create();
    CVoiceCallList::getInstance().Create();

    // 通知UI已启动
    CWnd * pMain = AfxGetApp()->m_pMainWnd;
    if (pMain!=NULL)
    {
        WPARAM wParam = 1;
        LPARAM lParam = 0;
        pMain->SendMessage(WM_MY_START_RESULT,wParam,lParam);
    }
    EmphasisLn("VoiceServer Start Successful......");
}

// 踢掉用户
void GlobalVoiceServer::KickUser( CClientUser *pClientUser, DWORD dwReason )
{
	if( pClientUser == NULL )
	{
		return ;
	}

	DWORD dwTimeLen = 0;	
	switch(dwReason)
	{
	case VoiceKickReason_Full:				// 人数满了
		dwTimeLen = 300000;
		break;

	case VoiceKickReason_LoginTimeout:		// 登录超时
		dwTimeLen = 300000;
		break;

	case VoiceKickReason_UserExist:			// 用户存在
		dwTimeLen = VOICE_INVALID_ID;		// DWORD 的最大数
		break;

	default:
		dwTimeLen = 120000;		// 默认2分钟内禁止连接
		break;
	}

	SendVoiceDataToClient( pClientUser->GetClientID(), MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_KickUser, pClientUser->GetClientID(), dwReason, dwTimeLen );
	
	pClientUser->Kick(dwReason);

	//TraceLn( "踢出用户：原因[" << GetKickReasonText(dwReason) << "]，" << pClientUser->ToString().data() );
}


LPCSTR GlobalVoiceServer::GetKickReasonText( DWORD dwReason )
{
	static _TCHAR *_gszKickText[VoiceKickReason_Max] =
	{
		_T("未知错误"),
		_T("登录超时"),
		_T("登录失败"),
	};
	if (dwReason<VoiceKickReason_Max)
	{
		return (LPCSTR)_gszKickText[dwReason];
	}

	return (LPCSTR)_gszKickText[0];
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////			公共方法					///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string GetLastErrorString(DWORD dwErrorCode)
{
	void* lpMsgBuf = NULL;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*) &lpMsgBuf,  // LPTSTR
		0, NULL );
	static std::string strErr;
	if ( NULL == lpMsgBuf )
	{
		string strErrCode;
		strErr = "未知错误!错误码为" + StringHelper::fromUlong(strErrCode, dwErrorCode);
	}
	else
	{
		strErr = (char*)lpMsgBuf;
		LocalFree(lpMsgBuf);
	}

	return strErr;
}


/** 取得字节大小显示字串
@param  ULONGLONG dwSize:字节大小
@return  LPCSTR
*/	
LPCSTR GetFileSizeString(ULONGLONG dwSize)
{
	static char szBuf[64] = {0};

	double fSize = (double)dwSize;

	memset(szBuf,0,sizeof(szBuf));

	if(dwSize<1024)
	{
		sprintf_s(szBuf, sizeof(szBuf), "%I64d 字节",dwSize);
	}
	else if(dwSize<1048576)
	{
		fSize = fSize/1024.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.1f KB",fSize);
	}
	else if (dwSize<1073741824)
	{
		fSize = fSize/1048576.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.1f MB",fSize);
	}
	else
	{
		fSize = fSize/1073741824.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.1f GB",fSize);
	}
	return szBuf;
}

/** 取得剩余时间字串
@param  char * szBuf:字串buffer
@param  int nBufSize:字串buffer长度
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  
*/	

void GetRemainTimeString(char * szBuf,int nBufSize,DWORD nRemainSeconds)
{
	// 剩余时间计数
	int bNums=0,sNums=0;

	// 小于等于60秒
	if (nRemainSeconds<60)
	{
		sprintf_s(szBuf, nBufSize, "%d秒",nRemainSeconds);
	}
	else if(nRemainSeconds<3600)
	{// 小于1小时
		bNums=(int)(nRemainSeconds / 60) ;
		sNums =(int)(nRemainSeconds % 60) ;
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d分钟",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d分钟%d秒",bNums,sNums);
		}

	}
	else if (nRemainSeconds<86400)
	{ // 小于1天
		bNums=(int)(nRemainSeconds / 3600) ;
		sNums =(int)((nRemainSeconds % 3600)/60) ;	
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d小时",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d小时%d分钟",bNums,sNums);
		}
	} 
	else
	{//大于等于1天
		bNums=(int)(nRemainSeconds / 86400) ;
		sNums =(int)((nRemainSeconds % 86400)/3600) ;
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d天",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d天%d小时",bNums,sNums);
		}
	}
}

/** 取得剩余时间字串
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  LPCSTR
*/	
LPCSTR GetRemainTimeString(DWORD nRemainSeconds)
{
	static char szTimebuf[10][32] = {0};
	static BYTE nBufIndex = 0;

	nBufIndex++;
	if (nBufIndex>=10)
	{
		nBufIndex =0;
	}

	memset(szTimebuf[nBufIndex],0,sizeof(szTimebuf[nBufIndex]));
	GetRemainTimeString(szTimebuf[nBufIndex],sizeof(szTimebuf[nBufIndex]),nRemainSeconds);
	return szTimebuf[nBufIndex];
}

/** 取得时间字串
@param   DWORD dwTime:时间
@param   
@return  LPCSTR
*/
LPCSTR GetTimeString(DWORD dwTime)
{
	static char szTimebuf[10][32] = {0};
	static BYTE nBufIndex = 0;

	nBufIndex++;
	if (nBufIndex>=10)
	{
		nBufIndex =0;
	}

	memset(szTimebuf[nBufIndex],0,sizeof(szTimebuf[nBufIndex]));
	if (dwTime==0)
	{
		return szTimebuf[nBufIndex];
	}
	time_t t = (time_t) dwTime;
	tm local;
	memset(&local,0,sizeof(local));
	localtime_s(&local,&t);

	sprintf_s(szTimebuf[nBufIndex], sizeof(szTimebuf[nBufIndex]), "%d-%.2d-%.2d %.2d:%.2d:%.2d",
		1900+local.tm_year,local.tm_mon+1,local.tm_mday,local.tm_hour,local.tm_min,local.tm_sec);

	return szTimebuf[nBufIndex];
}

// 取得服务器类型名称
LPCSTR GetServerTypeString(DWORD dwServerType)
{
	//游戏软件服务器
	static _TCHAR *_gszGameSoftServerName[MSG_SERVERID_MAXID] =
	{
		_T("未知游戏服务器"),
		_T("场景服务器"),
		_T("网关服务器"),
		_T("登录服务器"),
		_T("世界服务器"),
		_T("游戏客户端"),
		_T("中心服务器"),
        _T("验证码服务器"),
		_T("社会服务器"),
        _T("跨区桥服务器"),
        _T("DB应用服务器"),
        _T("通行证服务器"),
        _T("数据监护服务器"),		
		_T("语音服务器"),
		_T("语音网关服务器"),
	};
	if (dwServerType<MSG_SERVERID_MAXID)
	{
		return (LPCSTR)_gszGameSoftServerName[dwServerType];
	}
	return (LPCSTR)_gszGameSoftServerName[0];
}

// 获取一个频道的配置
const SVoiceChannelConfig & GetChannelConfig( DWORD dwChannelType )
{
	// 频道类型无效
	if( dwChannelType >= VoiceChannel_Max )
	{
		return g_ChannelUpdateConfig[VoiceChannel_None];
	}

	return g_ChannelUpdateConfig[dwChannelType];
}

// 获取一个房间的配置
const SVoiceRoomConfig & GetRoomConfig( DWORD dwChannelType, DWORD dwRoomID )
{
	DWORD dwCType = VoiceChannel_None;
	DWORD dwID = VoiceRoomType_Main;

	if( dwChannelType < VoiceChannel_Max )
	{
		dwCType = dwChannelType;
	}

	if( dwRoomID < VoiceRoomType_Max )
	{
		dwID = dwRoomID;
	}

	return g_RoomUpdateConfig[dwCType][dwRoomID];
}

// 根据用户类型获取名称
LPCSTR GetNameByUserType( DWORD dwType )
{
	static char *_gszNameText[VoiceUserType_Max] =
	{
		"普通成员",
		"普通成员",
		"嘉宾主持",
		"主播",
		"管理员",
		"拥有者",
		"官方管理员",
	};

	if ( dwType < VoiceUserType_Max )
	{
		return (LPCSTR)_gszNameText[dwType];
	}

	return (LPCSTR)_gszNameText[0];
}

// 把一个ClientID列表合并到一个数组中，去掉重复的
// 把pClientIDIn里的ID合并到pClientIDArray中
// dwIDArrayLen pClientIDArray的值个数
// 此处借用TMAP_VoiceID2IDInfo定义
// 返回合并后的长度
DWORD MergeClientIDList( DWORD * pClientIDArray, DWORD dwIDArraySize, DWORD dwIDArrayCount, TMAP_VoiceID2IDInfo & TmpCheckList, DWORD * pClientIDIn, DWORD dwIDCountIn, bool bCheck )
{
	if( pClientIDArray == NULL || pClientIDIn == NULL || dwIDArraySize == 0 || dwIDCountIn == 0 )
	{
		return dwIDArrayCount;
	}

	DWORD dwResult = dwIDArrayCount;
	if( dwIDArrayCount == 0 || !bCheck )	// 如果原始的为0，或者是不需要检测重复值
	{
		for( DWORD i = 0; i < dwIDCountIn; i ++ )
		{
			// 如果已经超过了pClientIDArray的大小，跳出
			if( i >= dwIDArraySize )
			{
				break;
			}

			pClientIDArray[dwResult] = pClientIDIn[i];
			TmpCheckList.insert( TMAP_VoiceID2IDInfo::value_type(pClientIDIn[i], pClientIDIn[i]) );
			dwResult ++;
		}
	}
	else
	{
		for( DWORD i = 0; i < dwIDCountIn; i ++ )
		{
			// 如果已经超过了pClientIDArray的大小，跳出
			if( dwResult >= dwIDArraySize )
			{
				break;
			}

			// 查找值是不是已经存在，不存在才添加进去
			if( TmpCheckList.find(pClientIDIn[i]) == TmpCheckList.end() )
			{
				pClientIDArray[dwResult] = pClientIDIn[i];
				TmpCheckList.insert( TMAP_VoiceID2IDInfo::value_type(pClientIDIn[i], pClientIDIn[i]) );
				dwResult ++;
			}
		}
	}

	return dwResult;
}
