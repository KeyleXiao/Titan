#include "stdafx.h"
#include "GlobalViewMng.h"
#include "IStackWalk.h"
#include "ViewMngApp.h"
#include "DGlobalMessage.h"
#include "ViewList.h"
#include "ViewUser.h"
#include "GateUser.h"
#include "GateList.h"
#include "GameUser.h"
#include "GameList.h"
#include "RedisMng.h"


IGlobalViewMng* g_pViewMng = nullptr;
/// purpose: 构造函数
GlobalViewMng::GlobalViewMng()
{
	g_pViewMng = dynamic_cast<IGlobalViewMng*>(this);

	m_dwTotalUseTimes=0;
	m_dwStartTime=0;
	m_dwEndTime=0;
	m_dwTotalAdd=0;

	m_dwControlCode = 0;	// 当前操作代码

	memset(szTraceFileName,0,sizeof(szTraceFileName));
	memset(szCrashFileName,0,sizeof(szCrashFileName));
	memset(szDumpFileName,0,sizeof(szDumpFileName));
	memset(m_szWorkDir,0,sizeof(m_szWorkDir));
	memset(m_szServerBinWorkDir,0,sizeof(m_szServerBinWorkDir));

	m_ManagerConnector = NULL;

	m_EventEngine =  NULL;

	// 是否初始化
	m_bInit = false;
	// 是否启动
	m_bStart = false;

	m_bUpdate = false;
	m_bNewDataCome = false;

	m_dwRecvSpeedServer = 0;		// 服务器收包速度
	m_dwSendSpeedServer = 0;		// 服务器发包速度

	m_dwRecvSpeedGateway = 0;		// 网关收包速度
	m_dwSendSpeedGateway = 0;		// 网关发包速度

	m_dwRecvSpeedClient = 0;		// 客户端收包速度
	m_dwSendSpeedClient = 0;		// 客户端发包速度

	m_dwMaxConnection = 0;

	m_bShutDowning = false;

	// 是否启动了自动流量控制
	m_bAutoNetSpeed = false;
}

/// purpose: 析构函数
GlobalViewMng::~GlobalViewMng()
{
	Close();
	UnInitEnvironment();	// 反初始化环境
	g_pViewMng = nullptr;
}

/// purpose: 初始化启动环境
bool GlobalViewMng::InitEnvironment(void)
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

	if (!gRedisMng.Init())
	{
		ErrorLn(_GT("Redis连接初始化失败!"));
		return false;
	}

	// 是否初始化
	m_bInit = true;

	// 启动管理连接器
	CreateManagerConnector();

	return true;
}

/// purpose: 反初始化环境
bool GlobalViewMng::UnInitEnvironment(void)
{
	if (!IsInit())
	{
		return false;
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

// 通知App实例关闭
void NotifyAppClose()
{
	CWnd * pMain = AfxGetApp()->m_pMainWnd;
	if (pMain != NULL)
	{
		WPARAM wParam = 0;
		LPARAM lParam = 0;
		pMain->SendMessage(WM_MY_CLOSE_APP, wParam, lParam);
	}
}

/**
@name               : 初始化网络
@brief              :
@param wPortForGame	: 监听游戏服务器的端口
@param wPortForGate : 监听Gate的端口
@param wPortForView	: 监听ViewServer的端口
@return             :
*/
bool GlobalViewMng::Create(WORD wPortForGame,WORD wPortForGate, WORD wPortForView)
{
	try{
		// 是否启动
		if (IsStart())
		{
			return false;
		}
		char szName[64] = { 0 };

		// 1.0 创建网络监听
		// 1.1 ViewServer
		if (!m_ViewServerListener.Create(wPortForView, this, DEFAULT_PACK))
		{
			ErrorLn(_GT("观战管理服监听ViewServer端口[") << wPortForView << _GT("]失败!"));
			// 绑定端口失败,直接关闭,用来程序互斥
			ReleaseListeners();
			// 通知App实例关闭
			NotifyAppClose();

			return false;
		}
		TraceLn(_GT("监听ViewServer的端口:") << wPortForView);
		sprintf_s(szName, sizeof(szName), "ViewServer Listen:%d", wPortForView);
		m_ViewServerListener.SetName(a2utf8(szName));

		// 1.2 网关
		if (!m_ClientListener.Create(wPortForGate, this, DEFAULT_PACK))
		{
			ErrorLn(_GT("观战管理服监听网关端口[") << wPortForGate << _GT("]失败!"));

			// 绑定端口失败,直接关闭,用来程序互斥
			ReleaseListeners();
			// 通知App实例关闭
			NotifyAppClose();

			return false;
		}
		TraceLn(_GT("网关监听端口:") << wPortForGate);
		sprintf_s(szName, sizeof(szName), "ViewGateway Listen:%d", wPortForGate);
		m_ClientListener.SetName(a2utf8(szName));

		// 1.3 游戏服
		if ( !m_ServerListener.Create(wPortForGame,this) )
		{
			ErrorLn(_GT("观战管理服监听游戏服务器端口[") << wPortForGame << _GT("]失败!"));

			// 绑定端口失败,直接关闭,用来程序互斥
			ReleaseListeners();
			// 通知App实例关闭
			NotifyAppClose();

			return false;
		}
		TraceLn(_GT("游戏服务器监听端口:") << wPortForGame);
		sprintf_s(szName, sizeof(szName), "Game Server Listen:%d",wPortForGame);
		m_ServerListener.SetName(a2utf8(szName));

		
		// 2.0 统计数据
		m_TotalInfo.dwStartTime		= (DWORD)time(NULL);		// 启动时间
		m_TotalInfo.dwServerPort	= wPortForGame;		// 服务器监听端口
		m_TotalInfo.dwClientPort	= wPortForGate;		// 客户端监听端口

		// 连接数据库
		getFileSystem()->addFindPath(".");

		SetTimer( ETimerEventID_VoiceControl, 2000, this, "GlobalViewMng::Create" );
		SetTimer( ETimerEventID_VoiceNetCount, 1000, this, "GlobalViewMng::Create" );
		SetTimer( ETimerEventID_CheckBanPDBID, 60000, this, "GlobalViewMng::Create" );		// 一分钟一次
		SetTimer( ETimerEventID_CheckCallFansInfo, 600000, this, "GlobalViewMng::Create" );	// 10分钟一次
		SetTimer( ETimerEventID_GetFansTopList, 3600000, this, "GlobalViewMng::Create" );	//从数据库读取粉丝排行榜 1个小时一次
		SetTimer( ETimerEventID_CheclAD, 600000, this, "GlobalViewMng::Create" );			// 10分钟一次
		
		SetTimer( ETimerEventID_CheckUBCommanderSpeak, 30000, this, "GlobalViewMng::Create" );	// 30秒一次 检测统战发言

		// 是否启动
		m_bStart = true;

		// 通知UI已启动
		CWnd * pMain = AfxGetApp()->m_pMainWnd;
		if (pMain!=NULL)
		{
			WPARAM wParam = 1;
			LPARAM lParam = 0;
			pMain->SendMessage(WM_MY_START_RESULT,wParam,lParam);
		}

		return true;
	}
	catch (...)
	{
		Trace(endl);
		Error("GlobalViewMng::create, catch exception"<<endl);
	}

	return false;
}

// 释放所有Listener
void GlobalViewMng::ReleaseListeners()
{
	m_ClientListener.Release();		// 释放网关连接
	m_ServerListener.Release();		// 释放Game服务器连接
	m_ViewServerListener.Release();	// 释放View服务器连接
}

/// purpose: 关闭操作
bool GlobalViewMng::Close()
{
#define MyRelease(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}

	try{

		// 是否启动
		if (!IsStart())
		{
			return false;
		}

		// 关闭管理连接器
		ReleaseManagerConnector();

		// 清除服务器列表
		gGameList.shutdown();

		// 清除网关列表
		gGateList.shutdown();

		// 清除ViewServer列表
		gViewList.shutdown();

		ReleaseListeners();

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
		Error("GlobalViewMng::close, catch exception"<<endl);
	}
	return false;
}

// 真正关闭服务器进程
void GlobalViewMng::ShutDownApp(void)
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
bool GlobalViewMng::IsInit(void)
{
	return m_bInit;
}

/// purpose: 是否已启动
bool GlobalViewMng::IsStart(void)
{
	return m_bStart;
}

/// purpose: 关闭服务器程序
void GlobalViewMng::CloseServerApp(void)
{
	SetControlCode(VS_ControlCode_ShutDown);
}

/// purpose: 启动管理连接器
void GlobalViewMng::CreateManagerConnector(void)
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
void GlobalViewMng::ReleaseManagerConnector(void)
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
void GlobalViewMng::SetControlCode(DWORD dwCode)
{
	// 当前操作代码
	m_dwControlCode = dwCode;
}


// 执行操作代码
bool GlobalViewMng::DoControlCodeTask(DWORD dwCode)
{
	switch(dwCode)
	{
	case VS_ControlCode_StartService:		// 启动服务
		{
			WORD wPortForGame = gSetting.m_wPortForGame;
			WORD wPortForView = gSetting.m_wPortForView;
			WORD wPortForGate = gSetting.m_wPortForGate;

			m_dwMaxConnection = USHRT_MAX;	// TODO 废弃

			if(Create(wPortForGame,wPortForGate, wPortForView))
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
bool GlobalViewMng::LoadConfigInfo()
{
	// 读入配置文件
	gSetting.LoadSetting();

	return true;
}



////////// IManagerEventHandler 处理子服务器连接器执行事件 ///////////////////////////////
/** 处理子服务器连接器执行事件
@param   wEventID ：事件ID
@param   pszContext : 上下文
@param   nLen : 上下文长度
@retval buffer 
*/
void GlobalViewMng::OnManagerEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen)
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
			m_ManagerConnector->SetRealTimeData(MG_VoiceRTData_OnLineCounts,0);// TODO
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
void GlobalViewMng::HandleManagerMessage(ulong actionId, SManagerMsgHead* head, LPSTR data, size_t len)
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

			if (pMsg->nServerType!=MSG_MG_ENDPOINT_ROOT)
			{
				ErrorLn(_GT("服务器类型[")<<pMsg->nServerType<<_GT("]与本服务器[")<<MSG_MG_ENDPOINT_ROOT<<_GT("]不符!"));
				return;
			}

			// 脚本文件数据,本配置号数据长度 pMsg->dwDataLens
			LPSTR pDataBuffer = (LPSTR)(data+sizeof(SMsgManagerSendScriptData_RM));

			// 加入文件系统中
			// todotodo

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
void GlobalViewMng::OnAccept( WORD wListenPort,IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	const char * ip = 0;
	DWORD dwPort = 0;
	pIncomingConn->GetRemoteAddr(&ip,dwPort);
	sprintf_s(szBuf, sizeof(szBuf), "GlobalViewMng::OnAccept() IP=%s,Port=%d",ip,dwPort);
	TraceLn(szBuf);
#endif

	// ViewServer连入
	if (wListenPort== m_ViewServerListener.GetListenPort())
	{
		ViewUser* pNewUser = new ViewUser(pIncomingConn, gViewList, gViewList.GetProcesser(), m_TimerAxis, nullptr);
		gViewList.AddUser(pNewUser);

		*ppHandler = pNewUser;

		char szName[64] = { 0 };
		sprintf_s(szName, sizeof(szName), "ViewUser_%d", pNewUser->GetID());
		pIncomingConn->setName(szName);
	}
	// 有网关连入
	else if (wListenPort==m_ClientListener.GetListenPort() )
	{
		GateUser *pNewUser = new GateUser(pIncomingConn, gGateList, gGateList.GetProcesser(), m_TimerAxis, nullptr);
		gGateList.AddUser(pNewUser);
		*ppHandler = pNewUser;

		char szName[64] = {0};
		sprintf_s(szName, sizeof(szName), "网关用户_%d",pNewUser->GetID());
		pIncomingConn->setName(szName);
	}
	// 有游戏服务器连入
	else if (wListenPort == m_ServerListener.GetListenPort())
	{
		GameUser* pNewUser = new GameUser(pIncomingConn, gGameList, gGameList.GetProcesser(), m_TimerAxis, nullptr);
		if (!gGameList.AddUser(pNewUser))
		{
			ErrorLn(_GT("Game Sessions is Full!"));
			pNewUser->Release();
			return;
		}

		*ppHandler = pNewUser;

		char szName[64] = { 0 };
		sprintf_s(szName, sizeof(szName), "游戏服务器%d", pNewUser->GetID());
		pIncomingConn->setName(szName);
	}
	else
	{
		assert(false);
	}
}


/////////////// EventHandler //////////////////////////////////////////////
void GlobalViewMng::OnEvent( HANDLE event )
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
}


////////////////////////// TimerHandler ///////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void GlobalViewMng::OnTimer(unsigned long dwTimerID)
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
		}
		break;

	case ETimerEventID_CheckCallFansInfo:		// 检测粉丝邀请信息
		{
		}
		break;

	case ETimerEventID_GetFansTopList:		// 获取粉丝排行榜
		{
		}
		break;

	case ETimerEventID_CheclAD:				// 检测语音召集令信息
		{
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
bool GlobalViewMng::SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes)
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
bool GlobalViewMng::KillTimer(DWORD timerID, ITimerHandler * handler)
{
	TimerAxis * pTimerAxis = getTimerAxis();
	if(pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(timerID, handler);
}


///// 语音网关消息处理
//void GlobalViewMng::onGatewayMessage(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
//{
//	switch(actionId)
//	{
//	case MSG_VOICE_KEEPALIVE:		// 心跳检查消息
//		{
//			SGameMsgHead header;
//			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
//			header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
//			header.byKeyModule  = MSG_MODULEID_VOICE;
//			header.byKeyAction  = MSG_VOICE_ANSWER_KEEPALIVE;	//桥服务器回应心跳检查
//			obuf obufData;
//			obufData.push_back(&header, sizeof(SGameMsgHead));
//			server.SendData(obufData.data(),obufData.size());
//		}
//		break;
//	case MSG_VOICE_HANDSHAKE:		// 握手消息
//		{
//			OnMsgGatewayHandshake(server,actionId,head,data, len);
//		}
//		break;
//	case MSG_VOICE_SENDDATA:		// 语音网关转发而来的客户端消息
//		{
//			OnMsgGatewaySendData(server,actionId,head,data, len);
//		}
//		break;
//	case MSG_VOICE_PERFORMANCE:	// 请求性能检测消息包
//		{
//			OnMsgGatewayPerformance(server,actionId,head,data, len);
//		}
//		break;
//	default:
//		{
//			ErrorLn(_GT("尚有一个语音网关服务器消息未处理，actionId = ")<<actionId<<_GT(",网关ID:")<<server.GetGatewayID());
//		}
//		break;
//	}
//}


///////////////客户端网络消息处理 /////////////////////////////////////////////////////////////////
//void GlobalViewMng::onClientMessage(GatewayUser &server, CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
//{
//	switch(actionId)
//	{
//	case MSG_VOICE_DATA_SUBMSG:
//		{
//		}
//		break;
//
//	case MSG_VOICE_LOGIN:		// 握手消息	压力测试用
//		{
//			OnMsgClientLogin(client, actionId, head, data, len);
//		}
//		break;
//
//	case MSG_VOICE_VOICEDATA:	// 客户端来的语音数据
//		{
//			OnMsgClientVoiceData( client, actionId, head, data, len );
//		}
//		break;
//
//	default:
//		{
//			ErrorLn(_GT("尚有一个客户端消息未处理，actionId = ")<<actionId << ", dwClientID=" << client.GetClientID());
//		}
//		break;
//	}
//
//}



//// 发数据给指定语音网关 dwGatewayID:语音网关ID 
//bool GlobalViewMng::SendDataToGateway(DWORD dwGatewayID,const char * pData,DWORD dwLen,bool bZeroBroadcast)
//{
//	GatewayUser *pGatewayUser = GatewayList::getInstance().GetUserByGatewayID(dwGatewayID);
//	if( NULL == pGatewayUser )
//	{
//		return false;
//	}
//
//	return pGatewayUser->SendData( pData, dwLen );
//}

// TODO 重新实现
// 广播数据给指定列表语音网关 pServerArray:语音网关ID列表,wClientNum:列表个数 pServerArray==NULL && wGatewayNum==VOICE_BROADCASTALL_NUM 广播所有
bool GlobalViewMng::BroadcastDataToGateway(DWORD * pGatewayArray,WORD wGatewayNum,const char * pData,DWORD dwLen)
{
	//if (pGatewayArray==NULL || wGatewayNum==0)
	//{
	//	// 为空广播所有
	//	if (pGatewayArray==NULL && wGatewayNum==VOICE_BROADCASTALL_NUM)
	//	{
	//		for(GatewayList::GATEWAY_MAP::iterator iter = GatewayList::getInstance().m_list.begin(); iter!= GatewayList::getInstance().m_list.end();++iter)
	//		{	
	//			if (iter->second)
	//			{
	//				iter->second->SendData((const char *)pData,dwLen);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}
	//else
	//{
	//	for (WORD i=0;i<wGatewayNum;i++)
	//	{
	//		GatewayUser * pGatewayUser = GatewayList::getInstance().GetUserByGatewayID(pGatewayArray[i]);
	//		if (pGatewayUser==NULL)
	//		{
	//			continue;
	//		}

	//		pGatewayUser->SendData((const char *)pData,dwLen);
	//	}
	//}

	return true;
}


// TODO
//// 发数据给指定游戏客户端 dwClientID:客户端ID
//bool GlobalViewMng::SendDataToClient(DWORD dwClientID,const char * pData,DWORD dwLen, BYTE nSendChennel )
//{
//	DWORD dwGatewayID = ((VClientIDInfo *)&dwClientID)->dwGatewayID;
//	GatewayUser *pGatewayUser = gGateList.GetUserByGatewayID(dwGatewayID);
//	if( NULL == pGatewayUser )
//	{
//		return false;
//	}
//	
//	return pGatewayUser->SendDataToClient(dwClientID, pData, dwLen, nSendChennel );
//}


// 网关断线
void GlobalViewMng::OnGatewayUserLeave(GatewayUser *pServer, DWORD dwReason )
{
	// todo

	// 设置该网关上所有用户的标记为离线
	//CClientList::getInstance().OnGatewayLeave(pServer->GetGatewayID());
}


////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////


/**  取得指定游戏世界的指定类型的服务器ID
@param   DWORD dwWorldID		: 游戏世界ID
@param   WORD wServerType		: 服务器类型
@param   WORD wSubID			: 服务器序号
@return  找到的服务器ID,为0表示没找到
*/
DWORD GlobalViewMng::GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID)
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

///** 列出所有服务器信息
//@param   DWORD dwWorldID		: 游戏世界ID
//@param   WORD wServerType		: 服务器类型
//@return 
//*/
//void GlobalViewMng::PrintGameServerListInfo(DWORD dwWorldID,WORD wServerType)
//{
//	ServerList::SERVER_LIST & mList = ServerList::getInstance().m_list;
//	for( ServerList::SERVER_LIST::iterator iter = mList.begin(); iter!= mList.end();++iter)
//	{
//		if ( (0==dwWorldID|| dwWorldID==iter->second->m_dwWorldID) && (0==wServerType|| wServerType==iter->second->m_wServerType))
//		{
//			TraceLn("ID="<<iter->second->GetServerID()<<_GT(",游戏世界ID=")<<iter->second->m_dwWorldID<<_GT(",服务器类型=")<<GetServerTypeString(iter->second->m_wServerType)<<(",序号=")<<iter->second->m_wSubID);
//		}
//	}
//}

////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

/**  取得游戏世界ID列表
@param   DWORD* pWorldArray	: 保存找到的ID数组
@param   WORD dwArrayLen		: 数组长度
@param   WORD &dwGetCounts		: 找到的个数
@return  找到的个数
*/
DWORD GlobalViewMng::GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts)
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
IVoiceServerWorldInfo * GlobalViewMng::GetGameWorldInfo(DWORD dwWorldID)
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
void GlobalViewMng::PrintGameWorldListInfo()
{
	// 游戏世界ID分类列表,以dwWorldID为关键字
	for(TMAP_VoiceWorldIDInfo::iterator iter = m_mapWorldList.begin(); iter!= m_mapWorldList.end();++iter)
	{
		TraceLn("游戏世界ID="<<iter->second.dwWorldID<<",名称="<<iter->second.strWorldName.c_str());
	}
}


//////////////////////////////////////////////////////////////////////////
// 命令操作
void GlobalViewMng::ExcutCommand( int nNum1, int nNum2, const char * szTxt )
{
	switch(nNum1)
	{
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

			gSetting.m_dwMaxNetSpeed = (DWORD)nNum2;

			ErrorLn( _GT("设置最大网络带宽：") << nNum2<<"Mbps" );
		}
		break;
	}
}


//////////////////////////////////////////////////////////////////////////

//// 语音网关握手消息
//void GlobalViewMng::OnMsgGatewayHandshake(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
//{
//	if(data==NULL|| len != sizeof(SMsgVoiceHandshake_GV))
//	{
//		server.Release();
//		return;
//	}
//
//	SMsgVoiceHandshake_GV * pMsg = (SMsgVoiceHandshake_GV *)(data);
//
//#ifdef VOICE_PRINT_DEBUG_INF
//	// 调式代码
//	char szBuf[512]={0};
//	sprintf_s(szBuf,sizeof(szBuf), "GlobalViewMng::OnMsgGatewayHandshake() 服务器握手消息:网关ID=%d,最大人数=%d",
//		pMsg->dwGatewayID,pMsg->dwMaxCounts);
//	TraceLn(szBuf);
//#endif
//
//	// 更新服务器信息
//	server.m_dwMaxCounts	= pMsg->dwMaxCounts;
//
//	DWORD dwGatewayID = pMsg->dwGatewayID;
//
//	bool bCheckFail = false;
//	if (head->DestEndPoint !=MSG_ENDPOINT_VOICE)
//	{
//		WarningLn(_GT("语音网关握手失败! ")<<server.ToString().data());
//		bCheckFail = true;
//	}
//	else if(dwGatewayID <= 0 || dwGatewayID >= VIEW_MAX_GATEWAYID )
//	{
//		ErrorLn(_GT("语音网关握手失败!语音网关ID[")<<dwGatewayID<<_GT("]无效,")<<server.ToString().data());
//		AlertError(MG_AlertSrcType_Network,"语音网关握手失败!语音网关ID["<<dwGatewayID<<"]无效,"<<server.ToString().data());
//		bCheckFail = true;
//	}
//	else if(!(gGateList.AddUserByGatewayID( dwGatewayID, &server )))
//	{
//		ErrorLn(_GT("语音网关握手失败!语音网关ID[")<<dwGatewayID<<_GT("]已经存在,")<<server.ToString().data());
//		AlertError(MG_AlertSrcType_Network,_GT("语音网关握手失败!语音网关ID[")<<dwGatewayID<<_GT("]已经存在,")<<server.ToString().data());
//		bCheckFail = true;
//	}
//
//	if (bCheckFail)
//	{
//		server.Release();
//		return;
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	SGameMsgHead header;
//	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
//	header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
//	header.byKeyModule  = MSG_MODULEID_VOICE;
//	header.byKeyAction  = MSG_VOICE_HANDSHAKE_RESPONSE;
//
//	// 桥服务器回应握手消息数据结构
//	SMsgVoiceHandshakeResponse_VG sendData;
//	sendData.dwID = server.GetServerID();		// 连接ID
//	sendData.dwServerTime = (DWORD)time(NULL);	// 服务器时间
//
//	obuf obufData;
//	obufData.push_back(&header, sizeof(SGameMsgHead));
//	obufData.push_back(&sendData, sizeof(SMsgVoiceHandshakeResponse_VG));
//
//	server.SendData(obufData.data(),obufData.size());
//
//	TraceLn(_GT("回应语音网关连接握手! ")<< a2utf8(server.ToString().c_str()));
//
//	// 服务务器更新和相关处理
//	m_TotalInfo.dwGateways	= gGateList.Count();	// 网关数
//
//}


// 语音网关转发而来的客户端消息
void GlobalViewMng::OnMsgGatewaySendData(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
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
	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalViewMng::OnMsgGatewaySendData() 发送数据给各软件服务器消息:软件服务器ID=%d,数据长度=%d",
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

//	CClientUser * pClientUser = CClientList::getInstance().GetUser(pMsg->dwServerID);
//	if (NULL ==pClientUser)
//	{
//		return;
//	}
//
//	// 是否验证通过的
//	if (!pClientUser->GetLogin())
//	{
//		// 如果没有验证通过，只收握手消息
//		if( pHead->byKeyAction != MSG_VOICE_LOGIN )
//		{
//#ifdef VOICE_PRINT_DEBUG_INF
//			ErrorLn(_GT("未登录用户发来错误消息，actionId:") << actionId);
//#endif
//			return ;
//		}
//	}
//
//	onClientMessage( server, *pClientUser, pHead->byKeyAction, pHead, clientdata, new_len );

}


//// 请求性能检测消息包
//void GlobalViewMng::OnMsgGatewayPerformance(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
//{
//
//	if(data==NULL|| len != sizeof(SMsgVoicePerformance_S))
//	{
//		return;
//	}
//
//	SMsgVoicePerformance_S * pMsg = (SMsgVoicePerformance_S *)(data);
//
//#ifdef VOICE_PRINT_DEBUG_INF
//	// 调式代码
//	char szBuf[512]={0};
//	sprintf_s(szBuf,sizeof(szBuf), "GlobalViewMng::OnMsgGatewayPerformance() 请求性能检测消息包:ClientID=%d",pMsg->dwClientID);
//	TraceLn(szBuf);
//#endif
//
//	SGameMsgHead header;
//	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
//	header.DestEndPoint= MSG_ENDPOINT_VOICEGATE;
//	header.byKeyModule  = MSG_MODULEID_VOICE;
//	header.byKeyAction  = MSG_VOICE_PERFORMANCE;
//
//	SMsgVoicePerformance_S sendData;
//	sendData = *pMsg;
//
//	DWORD dwType = VoicePerformanceType_Server;
//	bool bSend = false;
//
//	SMsgVoicePerformanceNode &myNode = sendData.perData.data[dwType];
//	// 取得高性能计数
//	LARGE_INTEGER litmpTims; 
//	::QueryPerformanceCounter(&litmpTims);
//	myNode.nFrequency = getPerformanceFrequency();
//	myNode.nRecvTicks = litmpTims.QuadPart;
//
//	IConnection * pConnection = server.GetConnection();
//	if (pConnection!=NULL)
//	{
//		myNode.nSendTicks = pConnection->GetLastRecvDataTimes();
//		myNode.nSendCounts = (WORD)pConnection->GetPendingCount(true);
//		myNode.nRecvCounts = (WORD)pConnection->GetPendingCount(false);
//	}
//
//	obuf obufData;
//	obufData.push_back(&header, sizeof(SGameMsgHead));
//	obufData.push_back(&sendData, sizeof(SMsgVoicePerformance_S));
//
//	server.SendData(obufData.data(),obufData.size());
//}


// 定时检测是否可以关闭语音服务器
void GlobalViewMng::OnTimerCheckShutDown(void)
{
	// 如果不是关机，直接返回
	if( !m_bShutDowning )
	{
		return;
	}


	// 后期扩展可用否决事件来提醒保存
	
	
	KillTimer( ETimerEventID_WaitShutDown, this );
	SetTimer( ETimerEventID_WindowClose, 10000, this, "等待关闭语音服务器窗口", 1 );
}


LPCSTR GlobalViewMng::GetKickReasonText( DWORD dwReason )
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
