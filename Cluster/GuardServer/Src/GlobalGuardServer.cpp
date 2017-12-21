/*******************************************************************
** 文件名:	GlobalGuardServer.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "GlobalGuardServer.h"
#include "IStackWalk.h"


GlobalGuardServer::GlobalGuardServer()
{
	setGlobal(static_cast<IGuardServer*>(this));

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
		
}

GlobalGuardServer::~GlobalGuardServer()
{
	Close();
	UnInitEnvironment();	// 反初始化环境
	setGlobal(NULL);
}

// 是否初始化
bool GlobalGuardServer::IsInit(void)
{
	return m_bInit;
}
// 是否初始化
bool GlobalGuardServer::IsStart(void)
{
	return m_bStart;
}


//////////////////////////////////////////////////////////////////////////

/// 启动管理连接器
void GlobalGuardServer::CreateManagerConnector(void)
{
	// 加载服务器管理器连接器设定
	Ini ini("Manager.ini");
	std::string strManagerServerIP = ini.getString("Setting","ManagerServerIP","127.0.0.1");
	int nManagerServerPort = ini.getInt("Setting","ManagerServerPort",8700);

	// 子服务器连接
	TraceLn(_GT("连接子服务器管理器:")<<strManagerServerIP.data()<<":"<<nManagerServerPort);
	m_ManagerConnector = CreateManagerConnectorProc(this,&m_TimerAxis);
	m_ManagerConnector->Create(MSG_MG_ENDPOINT_GUARD, 0, "GUARD SERVER");
	if(!m_ManagerConnector->Connect(strManagerServerIP.data(),nManagerServerPort))
	{
		TraceLn(_GT("连接子服务器管理器失败!"));
		return;
	}
	TraceLn(_GT("连接子服务器管理器成功!"));
}
/// 关闭管理连接器
void GlobalGuardServer::ReleaseManagerConnector(void)
{
	if (m_ManagerConnector)
	{
		// 关闭子服务器连接
		TraceLn(_GT("关闭子服务器管理器连接!"));
		m_ManagerConnector->Release();
		m_ManagerConnector = NULL;
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
void GlobalGuardServer::HandleManagerMessage(ulong actionId, SManagerMsgHead* head, LPSTR data, size_t len)
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
				ErrorLn("向服务器请求启动结果数据出错,数据有误！");
				return;
			}

			SMsgManagerRequestStartResult_M * pMsg = (SMsgManagerRequestStartResult_M *)(data);

			WarningLn("请求启动，结果代码="<<pMsg->dwResultFlag<<",启动标识="<<pMsg->dwResultFlag<<",用户数据="<<pMsg->dwUserData<<",结果信息="<<pMsg->szResultMsg);


			if (pMsg->bResult==false)
			{
				ErrorLn("请求启动失败，结果代码="<<pMsg->dwResultFlag<<",启动标识="<<pMsg->dwResultFlag<<",用户数据="<<pMsg->dwUserData<<",结果信息="<<pMsg->szResultMsg);

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
				ErrorLn("收到服务器启动脚本文件数据出错,数据有误！");
				return;
			}

			SMsgManagerSendScriptData_RM * pMsg = (SMsgManagerSendScriptData_RM *)(data);

			TraceLn("收到服务器启动脚本文件数据，游戏世界ID="<<pMsg->dwWorldID<<",服务器类型="<<pMsg->nServerType<<",脚本文件组ID="<<pMsg->dwScriptGroupID<<",最大配置号="<<pMsg->nMaxSchemeIndex<<",配置号="<<pMsg->nSchemeIndex<<",本配置号数据长度="<<pMsg->dwDataLens);

			if (pMsg->nServerType!=MSG_MG_ENDPOINT_ROOT)
			{
				ErrorLn("服务器类型["<<pMsg->nServerType<<"]与本服务器["<<MSG_MG_ENDPOINT_ROOT<<"]不符!");
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
				ErrorLn("要求软件服务器关闭出错,数据有误！");
				return;
			}

			SMsgManagerCloseServer_Z * pMsg = (SMsgManagerCloseServer_Z *)(data);

			// 关闭服务器
			// todotodo
			WarningLn("请求关闭，用户ID="<<pMsg->dwUserID<<",软件服务器UID="<<pMsg->dwServerUID<<",服务器类型="<<pMsg->dwServerType<<",第n号="<<pMsg->dwSubID<<",关闭标识="<<pMsg->dwCloseFlag<<",延迟关闭毫秒数="<<pMsg->dwDelays);

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
				ErrorLn("要求软件服务器发布公告出错,数据有误！");
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
			WarningLn("发布公告，用户ID="<<pMsg->dwUserID<<",软件服务器UID="<<pMsg->dwServerUID<<",服务器类型="<<pMsg->dwServerType<<",第n号="<<pMsg->dwSubID<<",标识="<<pMsg->dwSendFlag<<",延迟关闭毫秒数="<<pMsg->dwDelays<<",公告="<<szSendAdMsg);
		}
		break;

	case MSG_MG_SHOWWINDOW:		// 通知服务器显示程序窗口
		{
			if(data==NULL)
			{
				ErrorLn("通知服务器显示程序窗口出错,数据有误！");
				return;
			}

			SMsgManagerShowServerWindow_Z * pMsg = (SMsgManagerShowServerWindow_Z *)(data);

			WarningLn("通知服务器显示程序窗口，标识="<<pMsg->dwFlag);

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
				}
				break;
			}
		}
		break;

	default:
		{
			ErrorLn("HAVE A MESSAGE NO HANDLE，actionId = "<<actionId);
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

// 设置当前操作代码  DWORD dwCode 参考 EMManagerControlCode
void GlobalGuardServer::SetControlCode(DWORD dwCode)
{
	// 当前操作代码
	m_dwControlCode = dwCode;
}


// 执行操作代码
bool GlobalGuardServer::DoControlCodeTask(DWORD dwCode)
{
	switch(dwCode)
	{
	case GS_ControlCode_StartService:		// 启动服务
		{
			WORD wServerListenPort =(WORD) GuardSetting::getInstance().m_dwServerPort;

			if(Create(wServerListenPort))
			{
				TraceLn("GUARD SEVER START OK ！");
			}
			else
			{
                TraceLn("GUARD SEVER START FAILED！");
			}
		}
		break;

	case GS_ControlCode_StopService:		// 停止服务
		{
			if(Close())
			{
                TraceLn("GUARD SEVER STOP  OK ！");
			}
			else
			{
                TraceLn("GUARD SEVER STOP FAILED！");
			}
		}
		break;

	case GS_ControlCode_ShutDown:		// 关闭程序,停止服务后要关闭进程
		{
			if(Close())
			{
                TraceLn("GUARD SEVER SHUT DOWN OK ！");
			}
			else
			{
                TraceLn("GUARD SEVER SHUT DOWN FAILED！");
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
			ErrorLn("HAVE A OPT CODE NO HANDLE, dwCode="<<dwCode);
		}
		break;
	}
	return true;
}

/// 关闭服务器程序
void GlobalGuardServer::CloseServerApp(void)
{
	SetControlCode(GS_ControlCode_ShutDown);
}


/** 载入配置信息,在程序启动时加载
@param   
@return  
@note     
@retval  
*/ 
bool GlobalGuardServer::LoadConfigInfo()
{
	// 读入配置文件
	GuardSetting::getInstance().LoadSetting();

	// 服务器监听端口
	m_wServerListenPort = (WORD)( GuardSetting::getInstance().m_dwServerPort);

	return true;
}

// 初始化启动环境
bool GlobalGuardServer::InitEnvironment(void)
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
	//////////////////////////////////////////////////////////////////////////

	// 初始化网络层
	if ( InitializeNetwork(8,true)<0 )
	{
		ErrorLn("Initialize network occured error!Please check your configuration.");
		return false;
	}

	// 挂接事件
	HANDLE hNetEvent = GetNetworkEventHandle();
	GetAsynIoReactor()->AddEvent(hNetEvent);
	GetAsynIoReactor()->RegisterEventHandler(hNetEvent,this);

	m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	LARGE_INTEGER DueTime;
	DueTime.QuadPart = 0;
	SetWaitableTimer(m_hTimer, &DueTime, 1000, NULL, NULL, TRUE);

	GetAsynIoReactor()->AddEvent(m_hTimer);
	GetAsynIoReactor()->RegisterEventHandler(m_hTimer,this);

	// 是否初始化
	m_bInit = true;

	return true;
}


// 反初始化环境
bool GlobalGuardServer::UnInitEnvironment(void)
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


bool GlobalGuardServer::Create( WORD wServerListenPort)
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
			TraceLn("DataGuardServer Listen Server port[" << wServerListenPort << "] failed!");

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
		TraceLn("Server Listen Port:" << wServerListenPort);

		// 启动管理连接器
		CreateManagerConnector();

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
		Error("GlobalGuardServer::create, catch exception"<<endl);
	}

	return false;
}

bool GlobalGuardServer::Close()
{
	try{

		// 是否启动
		if (!IsStart())
		{
			return false;
		}

		// 关闭管理连接器
		ReleaseManagerConnector();

		ServerList::getInstance().Clear();

		m_ServerListener.Release();			// 监听服务器连接

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
		Error("GlobalGuardServer::close, catch exception"<<endl);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
void GlobalGuardServer::OnEvent( HANDLE event )
{
	if ( event==m_hTimer )
	{
		m_TimerAxis.CheckTimer();

		// 执行操作代码
		if (m_dwControlCode>GS_ControlCode_None && m_dwControlCode<GS_ControlCode_Max)
		{
			DoControlCodeTask(m_dwControlCode);
			m_dwControlCode = GS_ControlCode_None;
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
void GlobalGuardServer::OnTimer(unsigned long dwTimerID)
{
	switch(dwTimerID)
	{
	case ETimerEventID_Keepalive:			//  检查心跳
		{
			DWORD dwNowTimes = (DWORD)time(NULL);	// 时间
			DWORD dwNowTicks = ::GetTickCount();	// Tick时间

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
bool GlobalGuardServer::SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes)
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
bool GlobalGuardServer::KillTimer(DWORD timerID, ITimerHandler * handler)
{
	TimerAxis * pTimerAxis = getTimerAxis();
	if(pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(timerID, handler);
}

// 发送数据给指定服务器
bool GlobalGuardServer::SendDataToServer(DWORD dwServerID,LPVOID pData,DWORD dwDataLen)
{
	ServerUser * pServerUser = ServerList::getInstance().GetUser(dwServerID);
	if (pServerUser==NULL)
	{
		return false;
	}
	return pServerUser->SendData((const char *)pData,dwDataLen);
}

// 广播数据给服务器 pServerArray==NULL && wServerNum==BRIDGE_BROADCASTALL_NUM 广播所有
bool GlobalGuardServer::BroadcastToServer(DWORD * pServerArray,WORD wServerNum,LPVOID pData,DWORD dwDataLen)
{

	if (pServerArray==NULL || wServerNum==0)
	{
		// 为空广播所有
		if (pServerArray==NULL && wServerNum==GUARD_BROADCASTALL_NUM)
		{
			for(ServerList::SERVER_LIST::iterator iter = ServerList::getInstance().m_list.begin(); iter!= ServerList::getInstance().m_list.end();iter++)
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

//////////////////////////////////////////////////////////////////////////

// 服务器断线
void GlobalGuardServer::OnServerUserLeave(ServerUser *pServer)
{
	if (NULL==pServer)
	{
		return;
	}

}


//////////////////////////////////////////////////////////////////////////////
void GlobalGuardServer::OnAccept( WORD wListenPort,IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{
#ifdef GUARD_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	const char * ip = 0;
	DWORD dwPort = 0;
	pIncomingConn->GetRemoteAddr(&ip,dwPort);
	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalGuardServer::OnAccept() IP=%s,Port=%d",ip,dwPort);
	TraceLn(szBuf);
#endif

	if ( wListenPort==m_ServerListener.GetListenPort() )
	{
		//int nBufSize=1024*1024*12;
		//pIncomingConn->SetSocketOption(SOL_SOCKET,SO_SNDBUF,(char*)&nBufSize,sizeof(int));
		//pIncomingConn->SetSocketOption(SOL_SOCKET,SO_RCVBUF,(char*)&nBufSize,sizeof(int));

		ServerUser * server = new ServerUser(pIncomingConn);
		ServerList::getInstance().AddUser(server);
		*ppHandler = server;
	}
	else
	{
		assert(false);
	}
}


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


//////////////////IManagerMessageHandler 网络消息处理者 ////////////////////////////////////////////////////////

/// 服务器网络消息处理
void GlobalGuardServer::onServerMessage(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	switch(actionId)
	{
	case MSG_GUARD_KEEPALIVE:			// 心跳检查消息
		{
			SGameMsgHead header;
			header.SrcEndPoint	= MSG_ENDPOINT_GUARD;
			header.DestEndPoint	= (BYTE)server.m_wServerType;
			header.byKeyModule  = MSG_MODULEID_GUARD;
			header.byKeyAction  = MSG_GUARD_ANSWER_KEEPALIVE;
			obuf obufData;
			obufData.push_back(&header, sizeof(SGameMsgHead));
			server.SendData(obufData.data(),obufData.size());
		}
		break;

	case MSG_GUARD_HANDSHAKE:			// 握手消息
		{
			OnMsgServerHandshake(server,actionId,head,data, len);
		}
		break;
		
	case MSG_GUARD_SERVERCHECKREQ:		// 收到发送服务器性能检测请求
		{
			obuf1024 buf; 
			buf.push_back(head, sizeof(SGameMsgHead));
			buf.push_back(data, len);
			server.HandleServerCheckReq(buf.data(), buf.size());
		}
		break;

	case MSG_GUARD_SERVERCHECKRES:		// 收到响应服务器性能检测请求
		{
			obuf1024 buf; 
			buf.push_back(head, sizeof(SGameMsgHead));
			buf.push_back(data, len);
			server.HandleServerCheckRes(buf.data(), buf.size());
		}
		break;

	default:
		{
			ErrorLn("Has a message not handle, actionId = "<<actionId);
		}
		break;
	}
}

///////////// 服务器网络消息处理 /////////////////////////////////////////////////////////////

// 握手消息
void GlobalGuardServer::OnMsgServerHandshake(ServerUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if(data==NULL|| len != sizeof(SMsgGuardHandshake_SG))
	{
		return;
	}

	SMsgGuardHandshake_SG * pMsg = (SMsgGuardHandshake_SG *)(data);

#ifdef GUARD_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf, _countof(szBuf),_NGT"GlobalGuardServer::OnMsgServerHandshake() :worldid=%d,server_type=%d,index=%d,name=%s",
		pMsg->dwWorldID,pMsg->nServerType,pMsg->wSubID,pMsg->szName);
	TraceLn(szBuf);
#endif

	// 更新服务器信息
	server.m_dwWorldID		= pMsg->dwWorldID;
	server.m_wServerType	= pMsg->nServerType;
	server.m_wSubID			= pMsg->wSubID;
	sstrcpyn(server.m_szName, pMsg->szName,sizeof(server.m_szName));

	bool bCheckFail = false;
	if (head->DestEndPoint != MSG_ENDPOINT_GUARD)
	{
		WarningLn(__FUNCTION__": HandShake Failed! "<<server.ToString().data());
		bCheckFail = true;
	}
	else if(0==pMsg->dwWorldID)
	{
		//ErrorLn(__FUNCTION__": HandShake Failed! WorldID"<<pMsg->dwWorldID<<"=0,"<<server.ToString().data());
		AlertError(MG_AlertSrcType_Network,_GT("GuardServer handShake Failed! worldlD=")<<pMsg->dwWorldID<<","<<server.ToString().data());
		bCheckFail = true;
	}

	if (bCheckFail)
	{
		server.Release();
		return;
	}	

	//////////////////////////////////////////////////////////////////////////
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_GUARD;
	header.DestEndPoint= (BYTE)server.m_wServerType;
	header.byKeyModule  = MSG_MODULEID_GUARD;
	header.byKeyAction  = MSG_GUARD_HANDSHAKE_RESPONSE;

	// 桥服务器回应握手消息数据结构
	SMsgGuardHandshakeResponse sendData;
	sendData.dwID = server.GetServerID();		// 连接ID
	sendData.dwServerTime = (DWORD)time(NULL);	// 服务器时间

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgGuardHandshakeResponse));

	server.SendData(obufData.data(),obufData.size());

	WarningLn(__FUNCTION__": Reply handShake! "<<server.ToString().data());
}

//////////////////////////////////////////////////////////////////////////
