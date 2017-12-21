
// TimeSyncDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TimeSync.h"
#include "TimeSyncDlg.h"
#include "afxdialogex.h"
#include "strconv.h"
#include "TimeSync_setting.h"
#include "DManagerModule.h"
#include "NetMessageDef.h"
#include "IStackWalk.h"
#include "DbgHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTimeSyncDlg 对话框




CTimeSyncDlg::CTimeSyncDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTimeSyncDlg::IDD, pParent)
	, m_nCurTick(0)
	, m_nUDPPort(6600)
    , m_nTCPPort(6601)
	, m_bEnableRefresh(FALSE)
	, m_strServerIP(_T("127.0.0.1"))
	, m_nServerPort(6000)
	, m_nLatency(0)
    , m_nTraceLevel(levelAll)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(m_szWorkDir, 0, sizeof(m_szWorkDir));
}

void CTimeSyncDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SERVER_IP2, m_nCurTick);
    DDX_Text(pDX, IDC_PORT_UDP, m_nUDPPort);
    DDX_Text(pDX, IDC_PORT_TCP, m_nTCPPort);
    DDX_Check(pDX, IDC_ENABLE_REFRESH, m_bEnableRefresh);
    DDX_Text(pDX, IDC_SERVER_IP, m_strServerIP);
    DDX_Text(pDX, IDC_SERVER_PORT, m_nServerPort);
    DDX_Text(pDX, IDC_LATENCY, m_nLatency);
    DDX_Control(pDX, IDC_RICHEDIT21, m_richEditOutput);
}

BEGIN_MESSAGE_MAP(CTimeSyncDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CTimeSyncDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_DO_SYNC, &CTimeSyncDlg::OnBnClickedDoSync)
	ON_BN_CLICKED(IDC_ENABLE_REFRESH, &CTimeSyncDlg::OnBnClickedEnableRefresh)
	ON_BN_CLICKED(IDC_OPEN_TRACE, &CTimeSyncDlg::OnBtnClickedOpenTrace)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTimeSyncDlg 消息处理程序

BOOL CTimeSyncDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    // 创建trace
    HWND hwndTrace = GetDlgItem(IDC_RICHEDIT21)->GetSafeHwnd();
    m_pTraceListener = output2RichEdit((void *)hwndTrace, levelAll, true);
    if(m_pTraceListener == NULL)
    {
        return false;
    }


    CTimeSync_setting::getInstance().LoadSetting();
    m_nUDPPort = CTimeSync_setting::getInstance().m_dwServerUDPPort;
    m_nTCPPort = CTimeSync_setting::getInstance().m_dwServerTCPPort;
    UpdateData(FALSE);

	GetDlgItem(IDC_ENABLE_REFRESH)->EnableWindow(FALSE);

	OnBnClickedStart();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTimeSyncDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTimeSyncDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTimeSyncDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTimeSyncDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
    int nNewLevel = trackTraceContextMenu(m_nTraceLevel);
    if (nNewLevel != m_nTraceLevel)
    {
        rkt::setTraceLevel(nNewLevel);
        EmphasisLn("TRACE LEVEL = " << nNewLevel);
        m_nTraceLevel = nNewLevel;
    }
}

void CTimeSyncDlg::OnBnClickedStart()
{
	UpdateData(TRUE);

	// 取得本地路径
	sstrcpyn(m_szWorkDir, getWorkDir(), sizeof(m_szWorkDir));
	getFileSystem()->addFindPath(m_szWorkDir);

	// 是否有log目录
	char szLogAbsolutePath[512] = { 0 };
	uint attrib = 0;
	bool isAbsolutePath = true;
	sprintf_s(szLogAbsolutePath, "%s\\Log\\TimeSyncServer", m_szWorkDir);
	if (!rkt::checkPath(szLogAbsolutePath, isAbsolutePath, attrib))
	{
		rkt::createDirectoryRecursive(szLogAbsolutePath);
	}

	// trace文件地址	
	time_t	long_time; struct tm * local_time;
	time(&long_time); local_time = localtime(&long_time);

	char szTraceFileName[512] = { 0 };
	sprintf_s(szTraceFileName, "%s\\TimeSyncServer_%d%.2d%.2d_%.2d%.2d%.2d_Trace.html", szLogAbsolutePath,
		local_time->tm_year + 1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);

	TraceListener * pTraceListener = output2File(szTraceFileName);
	if (pTraceListener != NULL)
	{
		pTraceListener->setHasTime(true);
	}
	m_szTraceFileName = szTraceFileName;

	// 修改trace等级
	setTraceLevel(levelAll);

	// dump
#ifdef SUPPORT_STACKWALK
	char szCrashFileName[512] = { 0 };
	sprintf_s(szCrashFileName, "%s\\TimeSyncServer%02d%02d%02d%02d%02d_Crash.txt", szLogAbsolutePath,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);

	char szDumpFileName[512] = { 0 };
	sprintf_s(szDumpFileName, "%s\\TimeSyncServer%02d%02d%02d%02d%02d_Stack.dmp", szLogAbsolutePath,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);

	IStackWalk * pStackWalk = createStackWalkProc();
	if (pStackWalk == NULL)
	{
		return ;
	}

	// dump类型
	pStackWalk->setDumpType(MiniDumpWithFullMemory);

	m_szCrashFileName = szCrashFileName;
	m_szDumpFileName = szDumpFileName;
	// dump文件
	pStackWalk->setLogFileName(my_utf82t(m_szCrashFileName));
	pStackWalk->setDumpFileName(my_utf82t(m_szDumpFileName));
#endif

	// 初始化网络
	if ( !m_NetworkHandler.initialize(&m_TimerAxis,m_nLatency) )
	{
		MessageBox(_T("初始化网络环境失败!"),_T("错误"),MB_OK);
		return ;
	}
    TraceLn("Initialize network ok.........");

	if ( !m_TimeSyncServer.Start(m_nUDPPort, m_nTCPPort) )
	{
		MessageBox(_T("监听端口失败!"),_T("错误"),MB_OK);
        ErrorLn("listen port failed! udp="<<m_nUDPPort <<", tcp="<<m_nTCPPort);
		return;
	}
    TraceLn("listening port, udp="<<m_nUDPPort <<", tcp="<<m_nTCPPort);

	GetDlgItem(IDC_ENABLE_REFRESH)->EnableWindow(TRUE);
	GetDlgItem(IDC_START)->EnableWindow(FALSE);


	// 创建子管理服务器
	CreateManagerConnector();
}


void CTimeSyncDlg::OnBnClickedDoSync()
{
	UpdateData(TRUE);

#ifdef RKT_UTF8
	m_TimeSyncClient.Start( (const char *)my_t2utf8(m_strServerIP),m_nServerPort,m_nServerPort+1,&m_TimerAxis );
#else
	m_TimeSyncClient.Start( m_strServerIP,m_nServerPort,m_nServerPort+1,&m_TimerAxis );
#endif
	
	SetTimer(1,1000,0);
}

void CTimeSyncDlg::OnBtnClickedOpenTrace()
{
	shellExecute("open", m_szTraceFileName.c_str());
}


void CTimeSyncDlg::OnBnClickedEnableRefresh()
{
	UpdateData(TRUE);

	if ( m_bEnableRefresh )
	{
		SetTimer(0,15,0);
	}else
	{
		KillTimer(0);
	}
}


void CTimeSyncDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent==0 )
	{
		m_nCurTick = m_TimeSyncServer.GetTick();

		UpdateData(FALSE);

		CDialogEx::OnTimer(nIDEvent);
	}else
	{
		if ( m_TimeSyncClient.GetPing() )
		{
			m_TimeSyncServer.AdjustStart( m_TimeSyncClient.GetTick() );
			ErrorLn(_GT("服务器时间是:") << m_TimeSyncClient.Getctime() );
		}
	}
}


void CTimeSyncDlg::OnDestroy()
{
	ReleaseManagerConnector();
	m_TimeSyncClient.Stop();
	m_TimeSyncServer.Stop();
	m_NetworkHandler.Close();

	CDialogEx::OnDestroy();
}

BOOL CTimeSyncDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->wParam == VK_ESCAPE ||pMsg->wParam == VK_RETURN)
        return true;

    return CDialogEx::PreTranslateMessage(pMsg);
}



void CTimeSyncDlg::CreateManagerConnector(void)
{
	// 加载服务器管理器连接器设定
	Ini ini("Manager.ini");
	std::string strManagerServerIP = ini.getString("Setting", "ManagerServerIP", "127.0.0.1");
	int nManagerServerPort = ini.getInt("Setting", "ManagerServerPort", 8700);

	// 子服务器连接
	TraceLn(_GT("连接子服务器管理器:") << strManagerServerIP.data() << ":" << nManagerServerPort);
	m_ManagerConnector = CreateManagerConnectorProc(this, &m_TimerAxis);
	m_ManagerConnector->Create(MSG_MG_ENDPOINT_TIME, 0, _GT("时间同步服"));
	if (!m_ManagerConnector->Connect(strManagerServerIP.data(), nManagerServerPort))
	{
		TraceLn(_GT("连接子服务器管理器失败!"));
		return;
	}
	TraceLn(_GT("连接子服务器管理器成功!"));

	// 订阅 要求更新实时数据事件
	m_ManagerConnector->registerEventHandler(EVENT_MG_REAL_TIME_DATA, static_cast<IManagerEventHandler *>(this));
}

void CTimeSyncDlg::ReleaseManagerConnector(void)
{
	if (m_ManagerConnector)
	{
		// 取消订阅 要求更新实时数据事件
		m_ManagerConnector->unregisterEventHandler(EVENT_MG_REAL_TIME_DATA, static_cast<IManagerEventHandler *>(this));
		// 关闭子服务器连接
		TraceLn(_GT("关闭子服务器管理器连接!"));
		m_ManagerConnector->Release();
		m_ManagerConnector = NULL;
	}
}

void CTimeSyncDlg::OnManagerEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen)
{
	if (NULL == m_ManagerConnector)
	{
		return;
	}

	switch (wEventID)
	{
	case EVENT_MG_REAL_TIME_DATA:		// 要求更新实时数据事件
		{
			WORD wDatas[2] = { 0 };	// 0=高位,1=低位
			int nValue = 0;

			/// 获取物理处理器个数（如双核就有2个）
			static size_t dwCPUCounts = 0;
			if (dwCPUCounts == 0)
			{
				dwCPUCounts = getPhysicalProcessorCount();
			}

			/// 获得当前进程的CPU占用百分比
			m_ManagerConnector->SetRealTimeData(MG_LoginRTData_CPUCurrentUsedValue, (int)(MANAGER_REALTIMEDATA_CPU_MAX*dwCPUCounts*getCPUCurrentUsedValueByCurrentProcess()));
			/// 获取当前进程的物理内存已使用大小
			m_ManagerConnector->SetRealTimeData(MG_LoginRTData_PhysicalMemoryUsedSize, (int)getPhysicalMemoryUsedSizeByCurrentProcess());
			/// 获取当前进程的虚拟内存已使用大小
			m_ManagerConnector->SetRealTimeData(MG_LoginRTData_VirtualMemoryUsedSize, (int)getVirtualMemoryUsedSizeByCurrentProcess());
		}
		break;
	default:
		{
			ErrorLn(_GT("尚有一个子服务器连接器执行事件订阅了但未处理，wEventID = ") << wEventID);
		}
		break;
	}
}


void CTimeSyncDlg::HandleManagerMessage(ulong actionId, SManagerMsgHead * head, LPSTR data, size_t len)
{
	switch (actionId)
	{
	case MSG_MG_HANDSHAKE_RESPONSE:		//  子服务器连接成功回应握手消息
		{
			if (m_ManagerConnector)
			{
				m_ManagerConnector->SetServerState(MG_SERVERSTATE_RUNING);// 已启动，正在远行
			}
		}
		break;
	case MSG_MG_REQUESTSTART_RESULT:	// 软件服务器向主服务器请求启动结果消息,调用 RequestStart()后收到
		{
			if (data == NULL || len != sizeof(SMsgManagerRequestStartResult_M))
			{
				ErrorLn(_GT("向主服务器请求启动结果数据出错,数据有误！"));
				return;
			}

			SMsgManagerRequestStartResult_M * pMsg = (SMsgManagerRequestStartResult_M *)(data);

			ErrorLn(_GT("请求启动，结果代码=") << pMsg->dwResultFlag << _GT(",启动标识=") << pMsg->dwResultFlag << _GT(",用户数据=") << pMsg->dwUserData << _GT(",结果信息=") << pMsg->szResultMsg);

			if (pMsg->bResult == false)
			{
				ErrorLn(_GT("请求启动失败，结果代码=") << pMsg->dwResultFlag << _GT(",启动标识=") << pMsg->dwResultFlag << _GT(",用户数据=") << pMsg->dwUserData << _GT(",结果信息=") << pMsg->szResultMsg);

				// 正在加载脚本数据,要脚本的可定时重试请求启动,直到取得
				if (pMsg->dwResultFlag == MANAGER_STARTCODE_LOADSCRIPT)
				{
					// 定时重试请求启动 调用 RequestStart()
					// todotodo
				}
			}
			else
			{
			}
		}
		break;
	case MSG_MG_SENDSCRIPTDATA:		// 收到服务器启动脚本文件数据
		{
			//if (data == NULL || len < sizeof(SMsgManagerSendScriptData_RM))
			//{
			//	ErrorLn(_GT("收到服务器启动脚本文件数据出错,数据有误！"));
			//	return;
			//}

			//SMsgManagerSendScriptData_RM * pMsg = (SMsgManagerSendScriptData_RM *)(data);

			//TraceLn(_GT("收到服务器启动脚本文件数据，游戏世界ID=") << pMsg->dwWorldID << _GT(",服务器类型=") << pMsg->nServerType << _GT(",脚本文件组ID=") << pMsg->dwScriptGroupID << _GT(",最大配置号=") << pMsg->nMaxSchemeIndex << _GT(",配置号=") << pMsg->nSchemeIndex << _GT(",本配置号数据长度=") << pMsg->dwDataLens);

			//if (pMsg->nServerType != MSG_MG_ENDPOINT_LOGIN)
			//{
			//	ErrorLn(_GT("服务器类型[") << pMsg->nServerType << _GT("]与本服务器[") << MSG_MG_ENDPOINT_LOGIN << _GT("]不符!"));
			//	return;
			//}

			//// 脚本文件数据,本配置号数据长度 pMsg->dwDataLens
			//LPSTR pDataBuffer = (LPSTR)(data + sizeof(SMsgManagerSendScriptData_RM));

			//// 资源类型
			//int nResourceType = pMsg->nResourceType;
		}
		break;
	case MSG_MG_CLOSESERVER:		// 要求软件服务器关闭
		{
			if (data == NULL || len != sizeof(SMsgManagerCloseServer_Z))
			{
				ErrorLn(_GT("要求软件服务器关闭出错,数据有误！"));
				return;
			}

			SMsgManagerCloseServer_Z * pMsg = (SMsgManagerCloseServer_Z *)(data);

			// 关闭服务器
			// todotodo
			ErrorLn(_GT("请求关闭，用户ID=") << pMsg->dwUserID << _GT(",软件服务器UID=") << pMsg->dwServerUID << _GT(",服务器类型=") << pMsg->dwServerType << _GT(",第n号=") << pMsg->dwSubID << _GT(",关闭标识=") << pMsg->dwCloseFlag << _GT(",延迟关闭毫秒数=") << pMsg->dwDelays);

			// 请求关闭,如果没有请求，就关了服务器，会当作当机处理
			m_ManagerConnector->RequestClose();

			// 通知服务器关闭
			CWnd * pMain = AfxGetApp()->m_pMainWnd;
			if (pMain != NULL)
			{
				pMain->SendMessage(WM_CLOSE);
			}


		}
		break;
	case MSG_MG_SERVERSENDAD:		// 要求软件服务器发布公告
		{
			if (data == NULL || len < sizeof(SMsgManagerServerSendAd_Z))
			{
				ErrorLn(_GT("要求软件服务器发布公告出错,数据有误！"));
				return;
			}

			SMsgManagerServerSendAd_Z * pMsg = (SMsgManagerServerSendAd_Z *)(data);

			// 公告信息
			char szSendAdMsg[512] = { 0 };

			char * pStrSendAd = data + sizeof(SMsgManagerServerSendAd_Z);
			DWORD dwMsgStrLens = pMsg->nSendMsgLens + 1;
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
			ErrorLn(_GT("发布公告，用户ID=") << pMsg->dwUserID << _GT(",软件服务器UID=") << pMsg->dwServerUID << _GT(",服务器类型=") << pMsg->dwServerType << _GT(",第n号=") << pMsg->dwSubID << _GT(",标识=") << pMsg->dwSendFlag << _GT(",延迟关闭毫秒数=") << pMsg->dwDelays << _GT(",公告=") << szSendAdMsg);

		}
		break;
	case MSG_MG_SHOWWINDOW:		// 通知服务器显示程序窗口
		{
			if (data == NULL)
			{
				ErrorLn(_GT("通知服务器显示程序窗口出错,数据有误！"));
				return;
			}

			SMsgManagerShowServerWindow_Z * pMsg = (SMsgManagerShowServerWindow_Z *)(data);

			ErrorLn(_GT("通知服务器显示程序窗口，标识=") << pMsg->dwFlag);

			// 通知服务器关闭
			CWnd * pMain = AfxGetApp()->m_pMainWnd;
			if (pMain != NULL)
			{
				if (pMsg->dwFlag == 0)
				{
					ShowWindowAsync(pMain->m_hWnd, SW_HIDE);
				}
				else
				{
					ShowWindowAsync(pMain->m_hWnd, SW_SHOWNORMAL);
				}
			}

		}
		break;
	case MSG_MG_CONTROLSOFTSERVER:		// 操作软件服务器消息
		{
			//ErrorLn(_GT("MSG_MG_CONTROLSOFTSERVER，操作软件服务器消息"));
			//if (len < sizeof(SMsgManagerControlSoftServer_C))
			//{
			//	break;
			//}

			//SMsgManagerControlSoftServer_C * pMsg = (SMsgManagerControlSoftServer_C *)(data);

			//switch (pMsg->dwControlCode)
			//{
			//case MG_SoftControl_SetServerUID:	// 设定软件服务器的UID
			//	{
			//		// 连接器连接跨区桥服务器
			//		if (NULL != m_ManagerConnector && m_ManagerConnector->GetWorldID() != 0)
			//		{
			//			// 还没连
			//			if (NULL == m_BridgeConnector)
			//			{
			//				// 启动跨区桥服务器连接器
			//				m_BridgeConnector = CreateBridgeConnectorProc(NULL);
			//			}
			//		}

			//		OnSetGameWorldID();
			//	}
			//	break;
			//}
		}
		break;
	default:
		{
			ErrorLn(_GT("尚有一个服务器管理消息未处理，actionId = ") << actionId);
		}
		break;
	}
}
