/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\GlobalVoiceGateway.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音网关
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "GlobalVoiceGateway.h"
#include "IStackWalk.h"
#include "VoiceGateway.h"
#include "AudioList.h"

// 语音通道连接端口相对 客户端监听基础端口 偏移数
#define VOICE_AUDIO_PORT_OFFSET		1000

/// purpose: 构造函数
GlobalVoiceGateway::GlobalVoiceGateway()
{
	setGlobal(static_cast<IGlobalVoiceGateway*>(this));

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

	m_bReady = false;

	m_dwRecvSpeedServer = 0;		// 服务器收包速度
	m_dwSendSpeedServer = 0;		// 服务器发包速度
	m_dwRecvSpeedClient = 0;		// 客户端收包速度
	m_dwSendSpeedClient = 0;		// 客户端发包速度

	m_dwAudioListenPort = 0;
	m_bOpenAudioConnect = false;

	m_dwMaxConnection = 0;
}

/// purpose: 析构函数
GlobalVoiceGateway::~GlobalVoiceGateway()
{
	Close();
	UnInitEnvironment();	// 反初始化环境
	setGlobal(NULL);
}

// purpose: 初始化启动环境
bool GlobalVoiceGateway::InitEnvironment(void)
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
	pStackWalk->setLogFileName(my_utf82t(szCrashFileName));
	pStackWalk->setDumpFileName(my_utf82t(szDumpFileName));
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
		ErrorLn(_GT("初始化网络失败，请检查配置"));
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

	// 是否初始化
	m_bInit = true;

	// 启动管理连接器
	CreateManagerConnector();

	return true;
}


// purpose: 反初始化环境
bool GlobalVoiceGateway::UnInitEnvironment(void)
{
	if (!IsInit())
	{
		return false;
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
@param dwID				 : 网关ID
@param szServerIP		 : 服务器IP
@param wServerPort		 : 服务器端口
@param wClientListenPort : 客户端监听端口
@return                  :
*/
bool GlobalVoiceGateway::Create( DWORD dwID, const char * szServerIP, WORD wServerPort, WORD wClientListenPort )
{
	try{
		// 是否启动
		if (IsStart())
		{
			return false;
		}

		if( dwID <= 0 || dwID >= VOICE_MAX_GATEWAYID )
		{
			return false;
		}

		m_dwGetwayID = dwID;
		CClientList::getInstance().SetGatewayID(dwID);
		CAudioList::getInstance().SetGatewayID(dwID);

		// 创建网络监听
		char szName[64] = {0};
		if ( !m_ClientListener.Create(wClientListenPort,this,DEFAULT_PACK))
		{
			ErrorLn(_GT("监听客户端端口[") << wClientListenPort << _GT("]失败!"));

			// 绑定端口失败,直接关闭,用来程序互斥
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
		TraceLn(_GT("客户端监听端口:") << wClientListenPort);

		sprintf_s(szName, sizeof(szName), "Client Listen:%d",wClientListenPort);
		m_ClientListener.SetName(szName);

		// 开启语音专用通道
		if( VoiceGatewaySetting::getInstance().m_bOpenAudioConnect )
		{
			m_bOpenAudioConnect = true;
			// 语音通道连接端口相对 客户端监听基础端口 偏移数
			m_dwAudioListenPort = VoiceGatewaySetting::getInstance().m_dwClientPort + VOICE_AUDIO_PORT_OFFSET;

			if ( !m_AudioListener.Create( (WORD)m_dwAudioListenPort,this,DEFAULT_PACK))
			{
				ErrorLn(_GT("监听语音通道端口[") << m_dwAudioListenPort << _GT("]失败!"));

				// 绑定端口失败,直接关闭,用来程序互斥
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
			TraceLn(_GT("语音通道监听端口:") << m_dwAudioListenPort);

			sprintf_s(szName, sizeof(szName), "Voice channel Listen:%d", m_dwAudioListenPort);
			m_AudioListener.SetName(szName);
		}

		// 语音服务器连接器初始化
		m_VoiceConnector.Create((WORD)dwID);
		m_VoiceConnector.Connect( szServerIP, wServerPort );

		SetTimer( ETimerEventID_VoiceControl, 2000, this, "GlobalVoiceGateway::Create" );
		SetTimer( ETimerEventID_VoiceNetCount, 1000, this, "GlobalVoiceGateway::Create" );

		m_TotalInfo.dwStartTime		= (DWORD)time(NULL);		// 启动时间
		m_TotalInfo.dwServerPort	= wServerPort;				// 服务器监听端口
		m_TotalInfo.dwClientPort	= wClientListenPort;		// 客户端监听端口
		m_TotalInfo.dwAudioPort		= m_dwAudioListenPort;		// 语音通道监听端口

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
		Error("GlobalVoiceGateway::create, catch exception"<<endl);
	}

	return false;
}

/// purpose: 关闭
bool GlobalVoiceGateway::Close()
{
#define MyRelease(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}

	try{

		// 是否启动
		if (!IsStart())
		{
			return false;
		}

		m_VoiceConnector.Close();

		// 关闭管理连接器
		ReleaseManagerConnector();

		// 清除所有用户
		CClientList::getInstance().Clear();
		CAudioList::getInstance().Clear();

		m_ClientListener.Release();			// 释放客户端监听连接

		if( m_bOpenAudioConnect )
		{
			m_AudioListener.Release();		// 释放语音通道监听连接
		}


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
		Error("GlobalVoiceGateway::close, catch exception"<<endl);
	}
	return false;
}

/// purpose: 是否初始化
bool GlobalVoiceGateway::IsInit(void)
{
	return m_bInit;
}

/// purpose: 是否初始化
bool GlobalVoiceGateway::IsStart(void)
{
	return m_bStart;
}

/// purpose: 关闭服务器程序
void GlobalVoiceGateway::CloseServerApp(void)
{
	SetControlCode(VS_ControlCode_ShutDown);
}

/// purpose: 启动管理连接器
void GlobalVoiceGateway::CreateManagerConnector(void)
{
	// 加载服务器管理器连接器设定
	Ini ini("Manager.ini");
	std::string strManagerServerIP = ini.getString("Setting","ManagerServerIP","127.0.0.1");
	int nManagerServerPort = ini.getInt("Setting","ManagerServerPort",8700);

	// 子服务器连接
	TraceLn(_GT("连接子服务器管理器:")<<strManagerServerIP.data()<<":"<<nManagerServerPort);
	m_ManagerConnector = CreateManagerConnectorProc(this,&m_TimerAxis);
	DWORD dwServerID = VoiceGatewaySetting::getInstance().m_dwServerID;
	char szGWName[64] = {0};
	sprintf_s( szGWName, sizeof(szGWName), "VoiceGateway%d", dwServerID );
	m_ManagerConnector->Create(MSG_MG_ENDPOINT_VOICEGATE, (WORD)dwServerID, szGWName );
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
void GlobalVoiceGateway::ReleaseManagerConnector(void)
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

/// purpose: 设置当前操作代码  DWORD dwCode 参考 EMManagerControlCode
void GlobalVoiceGateway::SetControlCode(DWORD dwCode)
{
	// 当前操作代码
	m_dwControlCode = dwCode;
}


// purpose: 取得网关ID
DWORD GlobalVoiceGateway::GetGatewayID(void)
{
	return m_dwGetwayID;
}

// purpose: 执行操作代码
bool GlobalVoiceGateway::DoControlCodeTask(DWORD dwCode)
{
	switch(dwCode)
	{
	case VS_ControlCode_StartService:		// 启动服务
		{
			DWORD dwID = VoiceGatewaySetting::getInstance().m_dwServerID;
			string strServerIP = VoiceGatewaySetting::getInstance().m_strServerIp;
			WORD wServerPort =(WORD) VoiceGatewaySetting::getInstance().m_dwServerPort;
			WORD wClientListenPort =(WORD) VoiceGatewaySetting::getInstance().m_dwClientPort;
			m_dwMaxConnection = VoiceGatewaySetting::getInstance().m_dwMaxConnection;

			if(Create( dwID, strServerIP.c_str(), wServerPort, wClientListenPort ))
			{
				Trace(_GT("启动语音网关 OK ！")<<endl);
			}
			else
			{
				Error(_GT("启动语音网关 失败！")<<endl);
			}
		}
		break;
	case VS_ControlCode_StopService:		// 停止服务
		{
			if(Close())
			{
				Trace(_GT("停止语音网关 OK ！")<<endl);
			}
			else
			{
				Error(_GT("停止语音网关 失败！")<<endl);
			}
		}
		break;
	case VS_ControlCode_ShutDown:		// 关闭程序,停止服务后要关闭进程
		{
			if(Close())
			{
				Trace(_GT("停止语音网关 OK ！")<<endl);
			}
			else
			{
				Error(_GT("停止语音网关 失败！")<<endl);
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
bool GlobalVoiceGateway::LoadConfigInfo()
{
	// 读入配置文件
	//VoiceGatewaySetting::getInstance().LoadSetting();

	// 服务器监听端口
	m_wServerListenPort = (WORD)( VoiceGatewaySetting::getInstance().m_dwServerPort);

	return true;
}


////////// IManagerEventHandler 处理子服务器连接器执行事件 ///////////////////////////////
/** 处理子服务器连接器执行事件
@param   wEventID ：事件ID
@param   pszContext : 上下文
@param   nLen : 上下文长度
@retval buffer 
*/
void GlobalVoiceGateway::OnManagerEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen)
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
			m_ManagerConnector->SetRealTimeData(MG_VoiceGatewayRTData_CPUCurrentUsedValue,(int)(MANAGER_REALTIMEDATA_CPU_MAX*dwCPUCounts*getCPUCurrentUsedValueByCurrentProcess()));
			/// 获取当前进程的物理内存已使用大小
			m_ManagerConnector->SetRealTimeData(MG_VoiceGatewayRTData_PhysicalMemoryUsedSize,(int)getPhysicalMemoryUsedSizeByCurrentProcess());
			/// 获取当前进程的虚拟内存已使用大小
			m_ManagerConnector->SetRealTimeData(MG_VoiceGatewayRTData_VirtualMemoryUsedSize,(int)getVirtualMemoryUsedSizeByCurrentProcess());
			// 更新实时数据 在线数
			m_ManagerConnector->SetRealTimeData(MG_VoiceGatewayRTData_OnLineCounts,(int)(CClientList::getInstance().CountUser()));
			// 客户端每秒发包速度
			m_ManagerConnector->SetRealTimeData(MG_VoiceGatewayRTData_ClientSendSpeed,(int)(m_TotalInfo.dwSendSpeedClient));
			// 客户端每秒收包速度
			m_ManagerConnector->SetRealTimeData(MG_VoiceGatewayRTData_ClientRecvSpeed,(int)(m_TotalInfo.dwRecvSpeedClient));
			// 服务器每秒发包速度
			m_ManagerConnector->SetRealTimeData(MG_VoiceGatewayRTData_ServerSendSpeed,(int)(m_TotalInfo.dwSendSpeedServer));
			// 服务器每秒收包速度
			m_ManagerConnector->SetRealTimeData(MG_VoiceGatewayRTData_ServerRecvSpeed,(int)(m_TotalInfo.dwRecvSpeedServer));
		}
		break;
	default:
		{
			ErrorLn(_GT("尚有一个子服务器连接器执行事件订阅了但未处理，wEventID = ")<<wEventID);
		}
		break;
	}
}

/**
@name         : 处理其他服务器通过子服务器转发的消息
@brief        : 具体内容不清楚
@param actionId  : 消息码
@param head  : 消息头
@param data  : 消息内容
@param len:消息长度
*/
void GlobalVoiceGateway::HandleManagerMessage(ulong actionId, SManagerMsgHead* head, LPSTR data, size_t len)
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

			TraceLn(_GT("收到服务器启动脚本文件数据，游戏世界ID=")<<pMsg->dwWorldID<<_GT(",服务器类型=")<<pMsg->nServerType<<_GT(",脚本文件组ID=")<<pMsg->dwScriptGroupID<<_GT(",最大配置号=")<<pMsg->nMaxSchemeIndex<<",配置号="<<pMsg->nSchemeIndex<<_GT(",本配置号数据长度=")<<pMsg->dwDataLens);

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
			WarningLn(_GT("请求关闭，用户ID=")<<pMsg->dwUserID<<_GT(",软件服务器UID=")<<pMsg->dwServerUID<<_GT(",服务器类型=")<<pMsg->dwServerType<<_GT(",第n号=")<<pMsg->dwSubID<<_GT(",关闭标识=")<<pMsg->dwCloseFlag<<",延迟关闭毫秒数="<<pMsg->dwDelays);

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
						m_TotalInfo.dwWorldID		= pWorldInfo->dwWorldID;		// 游戏世界ID
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



///////////////////////// IAcceptorHandler ///////////////////////////////////
void GlobalVoiceGateway::OnAccept( WORD wListenPort,IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{
	if (wListenPort==m_ClientListener.GetListenPort() )
	{
		CClientUser *pNewUser = new CClientUser(pIncomingConn);

		// 如果语音网关未与语音服务器握手完毕，不接受连接
		if( !m_bReady )
		{
			pNewUser->Release();
			return ;
		}

		CClientList & clientList = CClientList::getInstance();
		// 超过最大连接数，要断开
		if( clientList.CountUser() >= m_dwMaxConnection )
		{
			ErrorLn(_GT("用户连接已满！"));
			pNewUser->Release();
			return ;
		}

		// 添加用户到列表
		if( !clientList.AddUser(pNewUser) )
		{
			pNewUser->Release();
			return ;
		}
		else
		{
			*ppHandler = pNewUser;
		}

		char szName[64] = {0};
		sprintf_s(szName, sizeof(szName), "语音用户%d",pNewUser->GetID());
		pIncomingConn->setName(szName);

	}
	else if( wListenPort == m_AudioListener.GetListenPort() )
	{
		CAudioUser *pNewUser = new CAudioUser(pIncomingConn);

		// 如果语音网关未与语音服务器握手完毕，不接受连接
		if( !m_bReady )
		{
			pNewUser->Release();
			return ;
		}

		// 添加语音通道用户到列表
		if( !CAudioList::getInstance().AddUser(pNewUser) )
		{
			pNewUser->Release();
			return ;
		}
		else
		{
			*ppHandler = pNewUser;
		}

		char szName[64] = {0};
		sprintf_s(szName, sizeof(szName), "语音通道%d",pNewUser->GetID());
		pIncomingConn->setName(szName);
	}
	else
	{
		assert(false);
	}
}



////////////////////////// EventHandler ///////////////////////////
void GlobalVoiceGateway::OnEvent( HANDLE event )
{
	if ( event==m_hTimer )
	{
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



/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void GlobalVoiceGateway::OnTimer(unsigned long dwTimerID)
{
	switch(dwTimerID)
	{
	case ETimerEventID_Keepalive:			//  检查心跳
		{
			//DWORD dwNowTimes = (DWORD)time(NULL);	// 时间
			//DWORD dwNowTicks = ::GetTickCount();	// Tick时间

		}
		break;

	case ETimerEventID_VoiceControl:
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

	case ETimerEventID_VoiceNetCount:
		{
			m_TotalInfo.dwRecvSpeedServer = m_dwRecvSpeedServer;
			m_TotalInfo.dwSendSpeedServer = m_dwSendSpeedServer;
			m_TotalInfo.dwRecvSpeedClient = m_dwRecvSpeedClient;
			m_TotalInfo.dwSendSpeedClient = m_dwSendSpeedClient;
			
			m_dwRecvSpeedServer = 0;
			m_dwSendSpeedServer = 0;
			m_dwRecvSpeedClient = 0;
			m_dwSendSpeedClient = 0;

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
bool GlobalVoiceGateway::SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes)
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
bool GlobalVoiceGateway::KillTimer(DWORD timerID, ITimerHandler * handler)
{
	TimerAxis * pTimerAxis = getTimerAxis();
	if(pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(timerID, handler);
}



/// purpose: 客户端网络消息处理
void GlobalVoiceGateway::onClientMessage(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	switch(actionId)
	{
	case MSG_VOICE_KEEPALIVE:		// 心跳检查消息
		{
			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
			header.DestEndPoint= MSG_ENDPOINT_CLIENT;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_ANSWER_KEEPALIVE;	//回应心跳检查
			obuf obufData;
			obufData.push_back(&header, sizeof(SGameMsgHead));
			client.SendData(obufData.data(),obufData.size());

			// 记录最后保持心跳时间
			client.m_dwLastKeepaliveTime = ::GetTickCount();
		}
		break;

	case MSG_VOICE_HANDSHAKE:		// 握手消息
		{
			OnMsgClientHandshake(client,actionId,head,data, len);
		}
		break;

	case MSG_VOICE_PERFORMANCE:		// 请求性能检测消息包
		{
			OnMsgClientPerformance(client,actionId,head,data, len);
		}
		break;

		//case MSG_VOICE_SENDDATA:		// 请求桥服务器网关服务发送数据给各软件服务器消息
		//	{
		//		OnMsgClientSendData(client,actionId,head,data, len);
		//	}
		//	break;

		//case MSG_VOICE_BROADCASTDATA:		// 请求桥服务器网关服务广播数据给各软件服务器
		//	{
		//		OnMsgClientBroadcastData(client,actionId,head,data, len);
		//	}
		//	break;

	default:
		{
			ErrorLn(_GT("尚有一个客户端的消息未处理，actionId = ")<<actionId);
		}
		break;
	}
}



/// purpose: 服务器网络消息处理
void GlobalVoiceGateway::onServerMessage( ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	switch(actionId)
	{
	case MSG_VOICE_ANSWER_KEEPALIVE:		// 心跳检查
		{
			m_VoiceConnector.m_bIsAnswer = true;							// 是否有回应
			m_VoiceConnector.m_dwLastAnswerTime = ::GetTickCount();		// 最后收到心跳Tick时间
			//TraceLn("收到心跳回应!");
		}
		break;
	case MSG_VOICE_HANDSHAKE_RESPONSE:		// 回应握手消息
		{
			OnMsgServerHandshakeResponse(actionId,head,data, len);
		}
		break;

	case MSG_VOICE_SENDDATA:		// 请求语音网关服务发送数据给各客户端消息
		{
			OnMsgServerSendData(actionId,head,data, len);
		}
		break;

	case MSG_VOICE_BROADCASTDATA:		// 请求语音网关服务广播数据给客户端
		{
			OnMsgServerBroadcastData(actionId,head,data, len);
		}
		break;

	case MSG_VOICE_DATA_SUBMSG:			// 语音网关子消息
		{
			OnMsgServerSubMsg( actionId,head,data, len );
		}
		break;

	case MSG_VOICE_PERFORMANCE:		// 请求性能检测消息包
		{
			OnMsgServerPerformance(actionId,head,data, len);
		}
		break;

	default:
		{
			ErrorLn(_GT("尚有一个语音服务器消息未处理，actionId = ")<<actionId);
		}
		break;
	}
}



/// purpose: 转发网络消息到语音服务器
void GlobalVoiceGateway::onTransVoiceServer(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	DWORD dwMaxSendLens = len+sizeof(SGameMsgHead)+sizeof(SMsgVoiceSendData)+128;	
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
	header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_SENDDATA;

	// 请求语音服务器网关服务发送数据给各软件服务器数据结构
	SMsgVoiceSendData sendData;
	sendData.dwServerID = client.GetID();		// 客户端ID
	sendData.nDataLens = len;				// 数据长度

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceSendData));
	obufData.push_back(data, len);

	m_VoiceConnector.SendData(obufData.data(),obufData.size());

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}

}


/// purpose: 语音通道客户端网络消息处理
void GlobalVoiceGateway::onAudioClientMessage(CAudioUser &audio, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	switch(actionId)
	{
	case MSG_VOICE_KEEPALIVE:		// 心跳检查消息
		{
			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
			header.DestEndPoint= MSG_ENDPOINT_CLIENT;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_ANSWER_KEEPALIVE;	//回应心跳检查
			obuf obufData;
			obufData.push_back(&header, sizeof(SGameMsgHead));
			audio.SendData(obufData.data(),obufData.size());
			
			// 记录最后保持心跳时间，用来处理死连接
			audio.m_dwLastKeepaliveTime = ::GetTickCount();
		}
		break;

	case MSG_VOICE_HANDSHAKE:		// 握手消息
		{
			OnMsgAudioClientHandshake(audio,actionId,head,data, len);
		}
		break;

	case MSG_VOICE_PERFORMANCE:		// 请求性能检测消息包
		{
			CClientUser * pUser = CClientList::getInstance().FindUser(audio.GetClientID());
			if( NULL != pUser )
			{
				OnMsgClientPerformance( *pUser, actionId, head, data, len );
			}
		}
		break;

	default:
		{
			ErrorLn(_GT("尚有一个客户端的消息未处理，actionId = ")<<actionId);
		}
		break;
	}
}



/// purpose: 转发语音通道网络消息到语音服务器
void GlobalVoiceGateway::onAudioTransVoiceServer(CAudioUser &audio, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	DWORD dwMaxSendLens = len+sizeof(SGameMsgHead)+sizeof(SMsgVoiceSendData)+128;	
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
	header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_SENDDATA;

	// 请求语音服务器网关服务发送数据给各软件服务器数据结构
	SMsgVoiceSendData sendData;
	sendData.dwServerID = audio.GetClientID();		// 客户端ID
	sendData.nDataLens = len;				// 数据长度

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceSendData));
	obufData.push_back(data, len);

	m_VoiceConnector.SendData(obufData.data(),obufData.size());

	// 是new的内存,用完释放
	if (bIsNewMemory)
	{
		delete [](pBufData);
		pBufData=NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
/// purpose: 客户端连入
void GlobalVoiceGateway::OnClientUserEnter(CClientUser *pUser, DWORD dwReason )
{
	if( NULL == pUser )
	{
		return ;
	}

	UpdateClientInfoToServer( VoiceUpdateType_Add, pUser->GetID() );
}

/// purpose: 客户端断线
void GlobalVoiceGateway::OnClientUserLeave(CClientUser *pUser, DWORD dwReason )
{
	if (NULL==pUser)
	{
		return;
	}

	UpdateClientInfoToServer( VoiceUpdateType_Delete, pUser->GetID() );
}


/// purpose: 更新语音网关客户端到语音服务器，nUpdateType:更新类型 EMVoiceClientUpdateType，dwClientID:要更新信息的客户端ID
void GlobalVoiceGateway::UpdateClientInfoToServer( BYTE nUpdateType, DWORD dwClientID )
{
	// 更新类型 EMVoiceClientUpdateType
	switch(nUpdateType)
	{
	case VoiceUpdateType_All:		// 全部
		{
			DWORD dwClientIDList[VOICE_MAX_CLIENT_COUNT] = {0};
			CClientList & clientList = CClientList::getInstance();

			obuf4096 obufData;
			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
			header.DestEndPoint= MSG_ENDPOINT_VOICE;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_CLIENTLIST_UPDATE;

			// 游戏世界列表更新数据结构
			SMsgVoiceGatewayClientListUpdate sendData;
			sendData.nUpdateType		= nUpdateType;			// 更新类型
			sendData.dwCounts			= clientList.CountUser();		// 客户端个数

			obufData.push_back(&header, sizeof(SGameMsgHead));
			obufData.push_back(&sendData, sizeof(SMsgVoiceGatewayClientListUpdate));

			DWORD dwListCouts = 0;
			for(DWORD i=0;i<VOICE_MAX_CLIENT_COUNT;i++) 
			{
				CClientUser *pClientUser = clientList.GetUserByIndex(i);
				if (NULL != pClientUser)
				{
					SMsgVoiceGatewayClientNode nodeInfo;
					nodeInfo.dwClientID		= pClientUser->GetID();				// 客户端ID
					nodeInfo.dwRemoteIP		= pClientUser->GetIP();				// 客户端IP
					//nodeInfo.dwPDBID		= pClientUser->m_dwPDBID;			// PDBID
					obufData.push_back(&nodeInfo, sizeof(SMsgVoiceGatewayClientNode));
					dwListCouts++;
				}
			}

			// 更新正确的个数
			SMsgVoiceGatewayClientListUpdate * pMsgData = (SMsgVoiceGatewayClientListUpdate *)(obufData.data()+sizeof(SGameMsgHead));
			pMsgData->dwCounts = dwListCouts;

			m_VoiceConnector.SendData( obufData.data(), obufData.size() );
		}
		break;

	case VoiceUpdateType_Add:
	case VoiceUpdateType_Edit:// 新增 修改
	case VoiceUpdateType_Delete:
		{
			CClientList & clientList = CClientList::getInstance();
			CClientUser *pClientUser = clientList.FindUser(dwClientID);
			if( NULL == pClientUser )
			{
				return ;
			}

			obuf obufData;
			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
			header.DestEndPoint= MSG_ENDPOINT_VOICE;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_CLIENTLIST_UPDATE;

			// 游戏世界列表更新数据结构
			SMsgVoiceGatewayClientListUpdate sendData;
			sendData.nUpdateType		= nUpdateType;			// 更新类型
			sendData.dwCounts			= 1;		// 客户端个数

			obufData.push_back(&header, sizeof(SGameMsgHead));
			obufData.push_back(&sendData, sizeof(SMsgVoiceGatewayClientListUpdate));

			SMsgVoiceGatewayClientNode nodeInfo;
			nodeInfo.dwClientID		= pClientUser->GetID();				// 客户端ID
			nodeInfo.dwRemoteIP		= pClientUser->GetIP();				// 客户端IP
			//nodeInfo.dwPDBID		= pClientUser->m_dwPDBID;			// PDBID
			obufData.push_back(&nodeInfo, sizeof(SMsgVoiceGatewayClientNode));

			m_VoiceConnector.SendData( obufData.data(), obufData.size() );
		}
		break;

	default:
		{
			ErrorLn(_GT("nUpdateType 不是有效的更新类型 nUpdateType:") << nUpdateType);
		}
		break;
	}
}


// 命令操作
void GlobalVoiceGateway::ExcutCommand( int nNum1, int nNum2, const char * szTxt )
{
	switch(nNum1)
	{
	case VoiceCommand_ExportUserList:		// 导出用户列表
		CClientList::getInstance().SaveUserListToCSV();
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
	}
}



// purpose: 客户端握手消息
void GlobalVoiceGateway::OnMsgClientHandshake(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len != sizeof(SMsgVoiceHandshake_CG))
	{
		return;
	}

	SMsgVoiceHandshake_CG * pMsg = (SMsgVoiceHandshake_CG *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "GlobalVoiceGateway::OnMsgClientHandshake() 客户端握手消息:昵称=%s,网卡地址=%s,pdbid=%d",
		pMsg->szNickname,pMsg->szMacAddress,pMsg->dwPDBID );
	TraceLn(szBuf);
#endif

	client.m_szNickname = pMsg->szNickname;		// 昵称
	client.m_szMacAddres = pMsg->szMacAddress;	// 网卡地址
	client.m_dwPDBID = pMsg->dwPDBID;			// pdbid

	bool bCheckFail = false;
	if (head->DestEndPoint != MSG_ENDPOINT_VOICEGATE)
	{
		WarningLn("客户端握手失败! "<<client.ToString().data());
		bCheckFail = true;
	}

	if (bCheckFail)
	{
		client.Release();
		return;
	}

	// 设为已校验，否则无法收发包
	client.m_bVerifyed = true;

	obuf obufData;
	//////////////////////////////////////////////////////////////////////////
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
	header.DestEndPoint= head->SrcEndPoint;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_HANDSHAKE_RESPONSE;
	obufData.push_back(&header, sizeof(SGameMsgHead));

	// 桥客户端回应握手消息数据结构
	SMsgVoiceHandshakeResponse_GC sendData;
	sendData.dwID = client.GetID();		// 连接ID
	sendData.dwServerTime = (DWORD)time(NULL);	// 客户端时间
	sendData.dwAudioPort = m_dwAudioListenPort;
	sendData.nKeyLen = client.m_nKeyLen;
	obufData.push_back(&sendData, sizeof(SMsgVoiceHandshakeResponse_GC));

	obufData.push_back( client.m_pKeyData, sendData.nKeyLen );

	client.SendData(obufData.data(),obufData.size());

	TraceLn(_GT("回应客户端连接器握手! ")<<client.ToString().data());

	OnClientUserEnter(&client);
}


// purpose: 语音通道客户端握手消息
void GlobalVoiceGateway::OnMsgAudioClientHandshake(CAudioUser &audio, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len < sizeof(SMsgVoiceDataHandShake_CG))
	{
		return;
	}

	SMsgVoiceDataHandShake_CG * pMsg = (SMsgVoiceDataHandShake_CG *)(data);
	if( len != sizeof(SMsgVoiceDataHandShake_CG) + pMsg->nKeyLen )
	{
		ErrorLn( _GT("GlobalVoiceGateway::OnMsgClientHandshake 语音通道握手失败，len=") << len );
		return ;
	}

	// 拷贝KEY值
	char *pKeyData = (char *)data + sizeof(SMsgVoiceDataHandShake_CG);
	if( pMsg->nKeyLen > VOICE_RC4_MAX_SIZE )
	{	
		ErrorLn( _GT("GlobalVoiceGateway::OnMsgClientHandshake KEY的长度无效=") << pMsg->nKeyLen );
		return ;
	}

	char szKeyData[VOICE_RC4_MAX_SIZE] = {0};
	memcpy( szKeyData, pKeyData, pMsg->nKeyLen );

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	char szKeyData2[128] = {0};
	memcpy( szKeyData2, szKeyData, pMsg->nKeyLen );
	sprintf_s(szBuf,sizeof(szBuf), "GlobalVoiceGateway::OnMsgClientHandshake() 语音通道握手消息:所属客户端ID=%d, KEY=[%s]",
								pMsg->dwClientID, szKeyData2 );
	TraceLn(szBuf);
#endif
	
	// 先找到所属对象
	CClientUser * pClientUser = CClientList::getInstance().FindUser(pMsg->dwClientID);
	if( NULL == pClientUser )
	{
		WarningLn( _GT("语音通道连接上来找不到ClientUser！所属ClientID=") << pMsg->dwClientID );
		audio.Release();
		return;
	}

	if( !pClientUser->CheckKeyData( szKeyData, pMsg->nKeyLen ) )
	{
		ErrorLn( _GT("语音通道动态密码验证失败！所属ClientID=") << pMsg->dwClientID );
		audio.Release();
		return ;
	}

	CAudioUser * pAudio = pClientUser->GetAudioUser();
	if( pAudio != NULL )
	{
		pAudio->Release();
	}

	DWORD dwClientID = pClientUser->GetID();

	// 设置客户端用户的语音通道ID
	pClientUser->SetAudioUserID(audio.GetID());

	// 设置语音通道的所属客户端ID
	audio.SetClientID(dwClientID);

	// 设为已校验，否则无法收发包
	audio.m_bVerifyed = true;

	//////////////////////////////////////////////////////////////////////////
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
	header.DestEndPoint= head->SrcEndPoint;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_HANDSHAKE_RESPONSE;

	// 桥客户端回应握手消息数据结构
	SMsgVoiceHandshakeResponse_GC sendData;
	sendData.dwID = dwClientID;		// 连接ID
	sendData.dwServerTime = (DWORD)time(NULL);	// 客户端时间

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceHandshakeResponse_GC));

	audio.SendData(obufData.data(),obufData.size());

	TraceLn(_GT("回应语音通道握手! ")<<audio.ToString().data() << _GT("，客户端ID=") << dwClientID );

}



// 请求性能检测消息包
void GlobalVoiceGateway::OnMsgClientPerformance(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{

	if(data==NULL|| len != sizeof(SMsgVoicePerformance_S))
	{
		return;
	}

	SMsgVoicePerformance_S * pMsg = (SMsgVoicePerformance_S *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "GlobalVoiceGateway::OnMsgClientPerformance() 请求性能检测消息包:ClientID=%d",	client.GetID() );
	TraceLn(szBuf);
#endif

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_PERFORMANCE;

	SMsgVoicePerformance_S sendData;
	sendData = *pMsg;

	sendData.dwClientID = client.GetID();

	DWORD dwType = VoicePerformanceType_Gateway;
	bool bSend = true;
	DWORD dwCounts = m_VoiceConnector.GetPendingCount(bSend);

	SMsgVoicePerformanceNode &myNode = sendData.perData.data[dwType];
	// 取得高性能计数
	LARGE_INTEGER litmpTims; 
	::QueryPerformanceCounter(&litmpTims);
	myNode.nFrequency = getPerformanceFrequency();
	if (bSend)
	{
		myNode.nSendTicks = litmpTims.QuadPart;
		myNode.nSendCounts = (WORD)dwCounts;
	}
	else
	{
		myNode.nRecvTicks = litmpTims.QuadPart;
		myNode.nRecvCounts = (WORD)dwCounts;
	}

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoicePerformance_S));

	m_VoiceConnector.SendData(obufData.data(),obufData.size());
}


/// purpose: 预留
void GlobalVoiceGateway::OnMsgClientSendData(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	//	if(data==NULL|| len <= sizeof(SMsgVoiceSendData))
	//	{
	//		return;
	//	}
	//
	//	SMsgVoiceSendData * pMsg = (SMsgVoiceSendData *)(data);
	//
	//#ifdef VOICE_PRINT_DEBUG_INF
	//	// 调式代码
	//	char szBuf[512]={0};
	//	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalVoiceGateway::OnMsgClientSendData() 发送数据给各软件客户端消息:软件客户端ID=%d,数据长度=%d",
	//		pMsg->dwServerID,pMsg->nDataLens);
	//	//TraceLn(szBuf);
	//#endif
	//
	//	DWORD dwSendDataLens = (DWORD)(len-sizeof(SMsgVoiceSendData) );
	//
	//	if (dwSendDataLens!=pMsg->nDataLens)
	//	{
	//		ErrorLn("请求桥客户端网关服务发送数据给各软件客户端消息长度错误! 实际长度="<<dwSendDataLens<<",指示长度="<<pMsg->nDataLens);
	//	}
	//
	//	// 转发数据,为了提高效率,用修改头数据方法,必须结构体大小一样
	//	Assert(sizeof(SMsgVoiceSendData)==sizeof(SMsgVoiceHead));
	//
	//	// 保留转发参数,下面的代码不要用 pMsg了,已被覆盖了
	//	SMsgVoiceSendData msgHead = (*pMsg);
	//
	//	head->SrcEndPoint = MSG_ENDPOINT_VOICE;
	//	head->DestEndPoint= MSG_ENDPOINT_VOICE;
	//	head->byKeyModule  = MSG_MODULEID_VOICE;
	//	head->byKeyAction  = MSG_VOICE_SENDDATA;
	//
	//	// 通过跨区桥客户端中转消息必须加入此消息体
	//	SMsgVoiceHead * pBhead =(SMsgVoiceHead *)data;
	//	pBhead->dwWorldID	= client.m_dwWorldID;		// 游戏世界ID
	//	pBhead->dwServerID	= client.GetServerID();		// 客户端ID
	//
	//	LPSTR pSendData = (LPSTR)head;
	//	dwSendDataLens = (DWORD)(len+sizeof(SGameMsgHead));
	//
	//	if (msgHead.dwServerID==0)
	//	{
	//		// 广播数据给客户端 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
	//		BroadcastToServer(NULL,VOICE_BROADCASTALL_NUM,pSendData,dwSendDataLens);
	//	} 
	//	else
	//	{
	//		// 发送数据给指定客户端
	//		SendDataToServer(msgHead.dwServerID,pSendData,dwSendDataLens);
	//	}

}

/// purpose: 预留
void GlobalVoiceGateway::OnMsgClientBroadcastData(CClientUser &client, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{

	//	if(data==NULL|| len <= sizeof(SMsgVoiceBroadcastData))
	//	{
	//		return;
	//	}
	//
	//	SMsgVoiceBroadcastData * pMsg = (SMsgVoiceBroadcastData *)(data);
	//
	//#ifdef VOICE_PRINT_DEBUG_INF
	//	// 调式代码
	//	char szBuf[512]={0};
	//	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalVoiceGateway::OnMsgClientBroadcastData() 广播数据给各软件客户端消息:软件客户端ID列表个数=%d,数据长度=%d",
	//		pMsg->dwServerCounts,pMsg->nDataLens);
	//	//TraceLn(szBuf);
	//#endif
	//
	//	if (pMsg->dwServerCounts<=0)
	//	{
	//		ErrorLn("请求桥客户端网关服务广播数据给各软件客户端消息时要广播客户端个数为0");
	//		return;
	//	}
	//
	//	DWORD dwServerNum = pMsg->dwServerCounts;
	//	DWORD * pServerArray = (DWORD *)((LPSTR)data + sizeof(SMsgVoiceBroadcastData));
	//	DWORD dwSendDataLens = (DWORD)(len-sizeof(SMsgVoiceBroadcastData)-dwServerNum*sizeof(DWORD));
	//
	//	if (dwSendDataLens!=pMsg->nDataLens)
	//	{
	//		ErrorLn("请求桥客户端网关服务广播数据给各软件客户端消息长度错误! 实际长度="<<dwSendDataLens<<",指示长度="<<pMsg->nDataLens);
	//	}
	//
	//	LPSTR pSendData = (LPSTR)((LPSTR)data + sizeof(SMsgVoiceBroadcastData)+dwServerNum*sizeof(DWORD));
	//
	//
	//	// 转发数据,为了提高效率,用修改头数据方法,必须结构体大小一样
	//	Assert(sizeof(SMsgVoiceBroadcastData)>=sizeof(SMsgVoiceHead));
	//
	//	// 拷贝列表,否则会被中转消息头覆盖
	//	DWORD dwServerList[1024];
	//	memcpy(dwServerList,pServerArray,dwServerNum*sizeof(DWORD));
	//
	//
	//	SGameMsgHead* pHead = (SGameMsgHead*)(pSendData - sizeof(SGameMsgHead)- sizeof(SMsgVoiceHead));
	//
	//	pHead->SrcEndPoint = MSG_ENDPOINT_VOICE;
	//	pHead->DestEndPoint= MSG_ENDPOINT_VOICE;
	//	pHead->byKeyModule  = MSG_MODULEID_VOICE;
	//	pHead->byKeyAction  = MSG_VOICE_SENDDATA;
	//
	//	// 通过跨区桥客户端中转消息必须加入此消息体
	//	SMsgVoiceHead * pBhead =(SMsgVoiceHead*)(pSendData - sizeof(SMsgVoiceHead));
	//	pBhead->dwWorldID	= client.m_dwWorldID;		// 游戏世界ID
	//	pBhead->dwServerID	= client.GetServerID();		// 客户端ID
	//
	//	pSendData = (LPSTR)pHead;
	//	dwSendDataLens = (DWORD)(dwSendDataLens+sizeof(SGameMsgHead)+sizeof(SMsgVoiceHead));
	//
	//	// 广播数据给客户端 pServerArray==NULL && wServerNum==VOICE_BROADCASTALL_NUM 广播所有
	//	BroadcastToServer(dwServerList,(WORD)dwServerNum,pSendData,dwSendDataLens);

}


//////////////////////////////////////////////////////////////////////////

/// purpose: 语音服务器发来握手回应消息
void GlobalVoiceGateway::OnMsgServerHandshakeResponse(ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len != sizeof(SMsgVoiceHandshakeResponse_VG))
	{
		return;
	}

	SMsgVoiceHandshakeResponse_VG * pMsg = (SMsgVoiceHandshakeResponse_VG *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "GlobalVoiceGateway::OnMsgServerHandshake() 服务器握手消息:ID=%d",pMsg->dwID);
	TraceLn(szBuf);
#endif

	m_VoiceConnector.SetID(pMsg->dwID);

	m_TotalInfo.dwGatewayID = pMsg->dwID;

	// 同步所有用户
	UpdateClientInfoToServer( VoiceUpdateType_All, 0 );	

	m_bReady = true;

	TraceLn(_GT("收到语音服务器回应握手信息! ") << a2utf8(m_VoiceConnector.ToString().data()));
}

/// purpose: 语音服务器请求发送数据到客户端
void GlobalVoiceGateway::OnMsgServerSendData(ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len <= sizeof(SMsgVoiceSendData))
	{
		return;
	}

	SMsgVoiceSendData * pMsg = (SMsgVoiceSendData *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalVoiceGateway::OnMsgServerSendData() 发送数据给各软件客户端消息:软件服务器ID=%d,数据长度=%d",
		pMsg->dwServerID,pMsg->nDataLens);
	TraceLn(szBuf);
#endif

	DWORD dwSendDataLens = (DWORD)(len-sizeof(SMsgVoiceSendData) );

	if (dwSendDataLens!=pMsg->nDataLens)
	{
		ErrorLn(_GT("语音服务器请求发送数据到客户端长度错误! 实际长度=")<<dwSendDataLens<<_GT(",指示长度=")<<pMsg->nDataLens);
		return ;
	}

	// 取得客户端ID，并找到客户端
	DWORD dwClientID = pMsg->dwServerID;
	CClientUser *pClientUser = CClientList::getInstance().FindUser(dwClientID);
	if( NULL == pClientUser )
	{
		return ;
	}

	LPSTR pSendData = (LPSTR)data + sizeof(SMsgVoiceSendData);

	switch(pMsg->nSendChennel)
	{
	case VoiceSendChannel_Voice:	// 用语音通道发
		{
			// 如果语音通道开放，用语音通道发，如果没开放，直接走控制通道
			if( m_bOpenAudioConnect )
			{
				CAudioUser * pAudioUser = pClientUser->GetAudioUser();
				if( NULL != pAudioUser )
				{
					pAudioUser->SendData( pSendData, dwSendDataLens );
				}
			}
			else
			{
				pClientUser->SendData( pSendData, dwSendDataLens );
			}
		}
		break;

	case VoiceSendChannel_Control:
	default:
		pClientUser->SendData( pSendData, dwSendDataLens );
		break;
	}
}

/// purpose: 语音服务器广播数据到客户端
void GlobalVoiceGateway::OnMsgServerBroadcastData(ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len <= sizeof(SMsgVoiceBroadcastData))
	{
		return;
	}

	SMsgVoiceBroadcastData * pMsg = (SMsgVoiceBroadcastData *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalVoiceGateway::OnMsgClientBroadcastData() 语音服务器广播数据给各客户端消息:客户端ID列表个数=%d,数据长度=%d",
		pMsg->dwServerCounts,pMsg->nDataLens);
	//TraceLn(szBuf);
#endif

	if (pMsg->dwServerCounts<=0)
	{
		ErrorLn(_GT("语音服务器广播数据给各客户端个数为0"));
		return;
	}

	DWORD dwClientNum = pMsg->dwServerCounts;
	DWORD dwHeadSize = sizeof(SMsgVoiceBroadcastData)+dwClientNum*sizeof(DWORD);
	DWORD * pServerArray = (DWORD *)((LPSTR)data + sizeof(SMsgVoiceBroadcastData));
	DWORD dwSendDataLens = (DWORD)(len-dwHeadSize);

	if (dwSendDataLens!=pMsg->nDataLens)
	{
		ErrorLn(_GT("语音服务器广播数据给各客户端消息长度错误! 实际长度=")<<dwSendDataLens<<_GT(",指示长度=")<<pMsg->nDataLens);
	}

	LPSTR pSendData = (LPSTR)((LPSTR)data + dwHeadSize);

	CClientList & clientList = CClientList::getInstance();

	switch(pMsg->nSendChennel)
	{
	case VoiceSendChannel_Voice:	// 用语音通道发
		{
			// 如果语音通道开放，用语音通道发，如果没开放，直接走控制通道
			if( m_bOpenAudioConnect )
			{
				// 广播消息给所有的客户端
				for( DWORD i = 0; i < dwClientNum; i ++ )
				{
					CClientUser *pClientUser = clientList.FindUser(pServerArray[i]);
					if( NULL != pClientUser )
					{
						CAudioUser * pAudioUser = pClientUser->GetAudioUser();
						if( NULL != pAudioUser )
						{
							pAudioUser->SendData(  pSendData, dwSendDataLens );
						}
					}
				}
			}
			else
			{
				// 广播消息给所有的客户端
				for( DWORD i = 0; i < dwClientNum; i ++ )
				{
					CClientUser *pClientUser = clientList.FindUser(pServerArray[i]);
					if( NULL != pClientUser )
					{
						pClientUser->SendData( pSendData, dwSendDataLens );
					}
				}
			}
		}
		break;

	case VoiceSendChannel_Control:
	default:
		{
			// 广播消息给所有的客户端
			for( DWORD i = 0; i < dwClientNum; i ++ )
			{
				CClientUser *pClientUser = clientList.FindUser(pServerArray[i]);
				if( NULL != pClientUser )
				{
					pClientUser->SendData( pSendData, dwSendDataLens );
				}
			}
		}
		break;
	}
}


// 语音网关子消息
void GlobalVoiceGateway::OnMsgServerSubMsg(ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len < sizeof(SMsgVoiceDataSubMsg))
	{
		return;
	}

	SMsgVoiceDataSubMsg * pMsg = (SMsgVoiceDataSubMsg *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[1024]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"CGlobalVoiceServer::OnMsgVoiceSubMsg()  语音网关子消息:子消息代码=%d,用户数据=%d,%d,%d",
		pMsg->dwMsgCode,pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3);
	TraceLn(szBuf);
#endif

	LPSTR pMsgData = (LPSTR)data + sizeof(SMsgVoiceDataSubMsg);
	size_t nNewlen = len - sizeof(SMsgVoiceDataSubMsg);

	// EMVoiceDataSubMsgCode
	switch(pMsg->dwMsgCode)
	{
	case VoiceSubMsg_KickUser:		// 服务器请求踢出玩家
		{
			DWORD dwClientID = pMsg->dwUserData1;
			DWORD dwReason = pMsg->dwUserData2;


			CClientUser *pClientUser = CClientList::getInstance().FindUser(dwClientID);
			if( NULL != pClientUser )
			{
				TraceLn( _GT("踢出用户：原因[") << dwReason << _GT("]，") << pClientUser->ToString().data() );
				pClientUser->Disconnect();
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


// 请求性能检测消息包
void GlobalVoiceGateway::OnMsgServerPerformance(ulong actionId, SGameMsgHead* head, void* data, size_t len)
{

	if(data==NULL|| len != sizeof(SMsgVoicePerformance_S))
	{
		return;
	}

	SMsgVoicePerformance_S * pMsg = (SMsgVoicePerformance_S *)(data);

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "GlobalVoiceGateway::OnMsgServerPerformance() 请求性能检测消息包:ClientID=%d",pMsg->dwClientID );
	TraceLn(szBuf);
#endif

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_PERFORMANCE;

	SMsgVoicePerformance_S sendData;
	sendData = *pMsg;

	DWORD dwType = VoicePerformanceType_Gateway;
	bool bSend = false;
	DWORD dwCounts = m_VoiceConnector.GetPendingCount(bSend);

	SMsgVoicePerformanceNode &myNode = sendData.perData.data[dwType];
	// 取得高性能计数
	LARGE_INTEGER litmpTims; 
	::QueryPerformanceCounter(&litmpTims);
	myNode.nFrequency = getPerformanceFrequency();
	if (bSend)
	{
		myNode.nSendTicks = litmpTims.QuadPart;
		myNode.nSendCounts = (WORD)dwCounts;
	}
	else
	{
		myNode.nRecvTicks = litmpTims.QuadPart;
		myNode.nRecvCounts = (WORD)dwCounts;
	}

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoicePerformance_S));

	// 取得客户端ID，并找到客户端
	DWORD dwClientID = pMsg->dwClientID;
	CClientUser *pClientUser = CClientList::getInstance().FindUser(dwClientID);
	if( NULL == pClientUser )
	{
		return ;
	}

	if( m_bOpenAudioConnect )
	{
		CAudioUser * pAudioUser = pClientUser->GetAudioUser();
		if( NULL != pAudioUser )
		{
			pAudioUser->SendData( obufData.data(),obufData.size());
		}
	}
	else
	{
		pClientUser->SendData( obufData.data(),obufData.size());
	}
	
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
_TCHAR * GetServerTypeString(DWORD dwServerType)
{
	//游戏软件服务器
	static _TCHAR *_gszGameSoftServerName[MSG_SERVERID_MAXID] =
	{
		_T("未知游戏服务器"),        // 未定义	
		_T("场景服务器"),            // 场景服务器	    ［简称：S］
		_T("网关服务器"),            // 网关服务器	    ［简称：G］
		_T("登录服务器"),            // 登录服务器	    ［简称：L］
		_T("世界服务器"),            // 世界服务器	    ［简称：W］
		_T("游戏客户端"),            // 客户端			［简称：C］
		_T("中心服务器"),            // 中心服务器	    ［简称：E］
		_T("验证码服务器"),          // 验证服务器	    ［简称：Y］
        _T("社会服务器"),            // 社会服务器	    ［简称：O］
		_T("跨区桥服务器"),          // 跨区桥服务器	［简称：B］
		_T("DB应用服务器"),          // DB应用服务器		［简称：D］
		_T("通行证服务器"),          // 通行证服务器		［简称：P］
		_T("数据监护服务器"),        // 数据监护服务器	［简称：G］
		_T("语音服务器"),            // 语音服务器	［简称：V］	
		_T("语音网关"),              // 语音网关		［简称：VG］
	};
	if (dwServerType<MSG_SERVERID_MAXID)
	{
		return _gszGameSoftServerName[dwServerType];
	}
	return _gszGameSoftServerName[0];
}
