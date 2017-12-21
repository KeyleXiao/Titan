#include "stdafx.h"
#include "ViewGateway.h"
#include "GlobalViewGateway.h"
#include "ViewGatewayDlg.h"
#include "ListInfoDlg.h"
#include "OperationDlg.h"
#include <time.h>
#include "ClientContainer.h"

static UINT gnIconImages[TREE_ICON_MAXID] = {
	IDR_MAINFRAME,		// 根图标
	IDI_ICON_SERVER,	// 服务器图标
	IDI_ICON_SERVER0,	// 服务器图标0
	IDI_ICON_USER,		// 用户图标
	IDI_ICON_USER0,		// 用户图标0
	IDI_ICON_HARD,		// 硬件服图标
	IDI_ICON_HARD0		// 硬件服图标0
};


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EXIT_CONFIRM_TEXT _T("观战网关正在运行中，你确定要停止服务并退出程序吗？")
#define EXIT_CONFIRM_TITLE _T("操作确认")


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CViewGatewayDlg 对话框


CViewGatewayDlg::CViewGatewayDlg(CWnd* pParent /*=NULL*/)
	: CResizingDialog(CViewGatewayDlg::IDD, pParent),m_trayIcon(IDR_TRAYICON)
	, m_strWorld(_T(""))
	, m_strIsTemp(_T(""))
	, m_strGame(_T(""))
	, m_strArea(_T(""))
	, m_strServers(_T(""))
	, m_strClients(_T(""))
	, m_strServerPort(_T(""))
	, m_strClientPort(_T(""))
	, m_strStartTime(_T(""))
	, m_strRecvSpeedServer(_T(""))
	, m_strSendSpeedServer(_T(""))
	, m_strRecvSpeedClient(_T(""))
	, m_strSendSpeedClient(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hGrayIcon = AfxGetApp()->LoadIcon(IDI_ICON_GRAY);

	SetControlInfo(IDC_OUTPUT,	ANCHORE_LEFT | RESIZE_BOTH);
	SetControlInfo(IDC_STATIC_LINE,	ANCHORE_LEFT | RESIZE_HOR);
	SetControlInfo(IDC_EDIT_STARTTIME,	ANCHORE_LEFT | ANCHORE_BOTTOM | RESIZE_HOR);
}

void CViewGatewayDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APP_ICONS, m_ctrlAppIcon);
	DDX_Control(pDX, IDC_BUTTON_START, m_StartBt);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_StopBt);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_ExitBt);
	DDX_Text(pDX, IDC_EDIT_TOTAL, m_strWorld);
	DDX_Text(pDX, IDC_EDIT_ONLINE, m_strIsTemp);
	DDX_Text(pDX, IDC_EDIT_RIGHTRATE, m_strGame);
	DDX_Text(pDX, IDC_EDIT_OUTRATE, m_strArea);
	DDX_Text(pDX, IDC_EDIT_WRONG, m_strServers);
	DDX_Text(pDX, IDC_EDIT_PASSRATE, m_strClients);
	DDX_Text(pDX, IDC_EDIT_IP, m_strServerPort);
	DDX_Text(pDX, IDC_EDIT_PORT, m_strClientPort);
	DDX_Text(pDX, IDC_EDIT_STARTTIME, m_strStartTime);

	DDX_Text(pDX, IDC_EDIT_IP2, m_strRecvSpeedServer);
	DDX_Text(pDX, IDC_EDIT_PORT2, m_strSendSpeedServer);
	DDX_Text(pDX, IDC_EDIT_IP3, m_strRecvSpeedClient);
	DDX_Text(pDX, IDC_EDIT_PORT3, m_strSendSpeedClient);
}

BEGIN_MESSAGE_MAP(CViewGatewayDlg, CResizingDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_OPEN, &CViewGatewayDlg::OnAppOpen)
	ON_COMMAND(ID_APP_ABOUT, &CViewGatewayDlg::OnAppAbout)
	ON_COMMAND(ID_APP_SUSPEND, &CViewGatewayDlg::OnAppSuspend)
	ON_BN_CLICKED(IDC_BUTTON_START, &CViewGatewayDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CViewGatewayDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CViewGatewayDlg::OnBnClickedButtonExit)
	ON_COMMAND(IDC_START, &CViewGatewayDlg::OnStart)
	ON_COMMAND(IDC_STOP, &CViewGatewayDlg::OnStop)
	ON_COMMAND(ID_OUT_TOTAL, &CViewGatewayDlg::OnOutTotal)
	ON_COMMAND(ID_SHOW_LISTINFO, &CViewGatewayDlg::OnShowListinfo)

	ON_MESSAGE(WM_MY_START_RESULT,&CViewGatewayDlg::OnMsgStartResult)
	ON_MESSAGE(WM_MY_STOP_RESULT,&CViewGatewayDlg::OnMsgStopResult)
	ON_MESSAGE(WM_MY_CLOSE_APP,&CViewGatewayDlg::OnMsgCloseApp)
	ON_COMMAND(ID_MG_DLG, &CViewGatewayDlg::OnMgDlg)
	ON_COMMAND(ID_TRACELEVEL, &CViewGatewayDlg::OnTracelevel)
END_MESSAGE_MAP()


// CViewGatewayDlg 消息处理程序

BOOL CViewGatewayDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// Set up tray icon
	m_trayIcon.SetNotificationWnd(this, WM_MY_TRAY_NOTIFICATION);

	DWORD dwServerID = gSetting.getServerID();
	CString strTip = SERVICE_TRAYICON_TIP;
	strTip.Format( _T("%s[%d]") , SERVICE_TRAYICON_TIP, dwServerID );
	m_trayIcon.SetIcon(m_hGrayIcon, strTip.GetString());
	//m_ctrlAppIcon.SetIcon(m_hGrayIcon);

	// 初始化图标列表
	m_TreeImgList.SetBkColor(CLR_NONE);
	m_TreeImgList.Create(16, 16,ILC_COLOR24|ILC_MASK,8,10);

	HICON hicon=NULL;
	for (int i=0;i<TREE_ICON_MAXID;i++)
	{
		hicon= AfxGetApp()->LoadIcon (gnIconImages[i]);
		m_TreeImgList.Add (hicon);
	}

	m_bShutdown=FALSE;
	m_bStart=FALSE;
	nTimeCount=0;
	nClientCount=1;
	m_bOnTimerUpdate=false;

	// center the dialog on the screen
	CenterWindow();

	//bool old = setTraceHasTime(false);

	// 取得本地路径
	int nDirLen = sizeof(m_VoiceGateway.m_szWorkDir);
	sstrcpyn(m_VoiceGateway.m_szWorkDir, getWorkDir(), nDirLen);
	getFileSystem()->addFindPath(m_VoiceGateway.m_szWorkDir);

	// 取得服务器程序Bin路径
	//m_szServerBinWorkDir
	string strWorkPath = getWorkDir();
	string strUpPath;
	if (strWorkPath.length()>0 && strWorkPath.find('\\')>0)
	{
		int nPos = strWorkPath.find_last_of('\\');
		strUpPath = strWorkPath.substr(0,nPos);
	}

	sstrcpyn(m_VoiceGateway.m_szServerBinWorkDir, strUpPath.c_str(), sizeof(m_VoiceGateway.m_szServerBinWorkDir));

	TraceLn(_GT("取得服务器程序Bin路径:")<<m_VoiceGateway.m_szServerBinWorkDir);

	// 是否有log目录
	char szLogAbsolutePath[512] = { 0 };
	sprintf_s(szLogAbsolutePath, "%s\\Log\\VoiceGateway%d",m_VoiceGateway.m_szWorkDir,dwServerID);
	bool isAbsolutePath = true; uint attrib = 0;
	if(!rkt::checkPath(szLogAbsolutePath, isAbsolutePath, attrib))
	{
		// 创建log目录
		rkt::createDirectoryRecursive(szLogAbsolutePath);
	}

	// trace文件地址	
	time_t	long_time; struct tm * local_time;
	time(&long_time); local_time = localtime(&long_time);

	sprintf_s(m_VoiceGateway.szTraceFileName, "%s\\Log\\VoiceGateway%d\\VoiceGT%d%.2d%.2d%.2d%.2d%.2d_Trace.html", m_VoiceGateway.m_szWorkDir,dwServerID,
		local_time->tm_year+1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);
	sprintf_s(m_VoiceGateway.szCrashFileName, "%s\\Log\\VoiceGateway%d\\VoiceGT%d%.2d%.2d%.2d%.2d%.2d_Crash.txt", m_VoiceGateway.m_szWorkDir,dwServerID,
		local_time->tm_year+1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);
	sprintf_s(m_VoiceGateway.szDumpFileName, "%s\\Log\\VoiceGateway%d\\VoiceGT%d%.2d%.2d%.2d%.2d%.2d_Stack.dmp",  m_VoiceGateway.m_szWorkDir,dwServerID,
		local_time->tm_year+1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);



	TraceListener * pTraceListener = output2File(m_VoiceGateway.szTraceFileName);
	if(pTraceListener != NULL)
	{
		pTraceListener->setHasTime(true);
	}

	mTrace = output2RichEdit((void*)(HWND)GetDlgItem(IDC_OUTPUT)->GetSafeHwnd(), levelAll, true);


	TraceLn(_GT("TraceFileName：")<<m_VoiceGateway.szTraceFileName);
	TraceLn(_GT("CrashFileName：")<<m_VoiceGateway.szCrashFileName);
	TraceLn(_GT("DumpFileName：")<<m_VoiceGateway.szDumpFileName);

	SetTimer( 1, 1000, NULL );

	// 初始化启动环境
	m_VoiceGateway.InitEnvironment();

	//  中心服务器ＩＰ,端口
	ShowCenterServerConfig();
	
	UpdateMenuState();

	SetWindowText(gSetting.m_strWinText.GetString());

	TraceLn(_GT("提示：\n 1.点击窗口的关闭按钮时不是关闭程序，而是最小化到任务栏图标！\n 2.并且这样可大大减小界面更新频率从而节省界面处理的CPU占用率！\n 3.服务未启动时任务栏图标为灰色，服务启动变为彩色！"));

	TraceLn(_GT("10秒后如没有手动启动服务,将会自动启动服务，并最小化到任务栏图标！"));

#ifdef VOICE_PRINT_DEBUG_INF
// 打印调试信息,并设为测试模式
	WarningLn("当前为调试模式,会打印调试信息！");
#endif

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//更新修改的中心服务器ＩＰ,端口到m_VoiceGateway.m_strCenterServerIP,m_VoiceGateway.m_nPort
void CViewGatewayDlg::UpdateCenterServerConfig()
{
	UpdateData(TRUE);
}

//更新界面中心服务器ＩＰ,端口
void CViewGatewayDlg::ShowCenterServerConfig()
{
	UpdateData(FALSE);
}


void CViewGatewayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		OnAppAbout();
	}
	else
	{
		CResizingDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CViewGatewayDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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
		CResizingDialog::OnPaint();

	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CViewGatewayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 关闭程序
void CViewGatewayDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 只有点任务栏图标菜单的"退出"才关闭程序
	if (m_bShutdown)
	{
		// 	退出程处理代码
		CResizingDialog::OnCancel();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}

	
}

// 起动结果message 
LRESULT CViewGatewayDlg::OnMsgStartResult(WPARAM wParam, LPARAM lParam)
{
	m_bStart =(wParam==1?TRUE:FALSE);

	UpdateMenuState();

	SetTrayIcon(m_bStart);

	UpdateTotalInfo();

	return 0;	
}
// 关闭结果message 
LRESULT CViewGatewayDlg::OnMsgStopResult(WPARAM wParam, LPARAM lParam)
{
	m_bStart =(wParam==1?TRUE:FALSE);

	UpdateMenuState();

	SetTrayIcon(m_bStart);

	return 0;
}
// 关闭程序message 
LRESULT CViewGatewayDlg::OnMsgCloseApp(WPARAM wParam, LPARAM lParam)
{
	// 	退出程处理代码
	CResizingDialog::OnCancel();

	return 0;
}

// 点任务栏图标菜单的"打开"显示程序界面
void CViewGatewayDlg::OnAppOpen()
{
	// TODO: 在此添加命令处理程序代码
	ShowWindow(SW_NORMAL);	
	SetForegroundWindow();
}

// 点任务栏图标菜单的"关于..."显示关于对话框
void CViewGatewayDlg::OnAppAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


// 点任务栏图标菜单的"退出"关闭程序
void CViewGatewayDlg::OnAppSuspend()
{
	// TODO: 在此添加命令处理程序代码

	// 停止服务
	if (m_VoiceGateway.IsStart())
	{
		if(MessageBox(EXIT_CONFIRM_TEXT, EXIT_CONFIRM_TITLE,MB_YESNO|MB_ICONQUESTION)!=IDYES) 
		{
			return;
		}

		TraceLn("Stop Voice Gateway ...");

		// 关闭程序,停止服务后要关闭进程
		m_VoiceGateway.SetControlCode(VS_ControlCode_ShutDown);
	}
	else
	{
		// 	退出程处理代码
		CResizingDialog::OnCancel();
	}
}

// 截获 onCancel,不关闭程序,只隐藏
void CViewGatewayDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	ShowWindow(SW_HIDE);
	//CResizingDialog::OnCancel();
}

// 截获 OnOK,不关闭程序,只隐藏
void CViewGatewayDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	ShowWindow(SW_HIDE);
	//CResizingDialog::OnOK();
}

// 更新日志到输出框
void CViewGatewayDlg::UpdateTraceInfo2RichEdit()
{
	if (mTrace)
		dispatch2RichEdit(mTrace);
}

//启动服务
void CViewGatewayDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码

	//更新修改的中心服务器ＩＰ,端口到m_VoiceGateway.m_strCenterServerIP,m_VoiceGateway.m_nPort
	UpdateCenterServerConfig();

	Trace(_GT("启动语音网关 ...")<<endl);

	// 启动服务
	m_VoiceGateway.SetControlCode(VS_ControlCode_StartService);

	m_bStart = TRUE;

	UpdateMenuState();

	SetTrayIcon(m_bStart);

	UpdateTotalInfo();
}

//停止服务
void CViewGatewayDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	
	// 由于停服务后再开有问题，直接调用推出
	StopService();
	
	//OnAppSuspend();

}

//停止服务
bool CViewGatewayDlg::StopService()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_bStart)
	{
		return false;
	}
	if(MessageBox(EXIT_CONFIRM_TEXT, EXIT_CONFIRM_TITLE,MB_YESNO|MB_ICONQUESTION)!=IDYES) 
	{
		return false;
	}

	TraceLn("Stop Voice Gateway ...");

	// 停止服务
	m_VoiceGateway.SetControlCode(VS_ControlCode_StopService);

	m_bStart = FALSE;

	UpdateMenuState();

	SetTrayIcon(m_bStart);

	return true;
}

void CViewGatewayDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	// 运行后10秒 如没开服务就自动开
	if (nTimeCount==0 && m_bStart==FALSE)
	{
		OnBnClickedButtonStart();
		OnClose();
	}

	nTimeCount++;
	
	BOOL bVisible=GetStyle()&WS_VISIBLE;


	if (bVisible)
	{
		UpdateTotalInfo();
	}

	//CResizingDialog::OnTimer(nIDEvent);
}


// 退出
void CViewGatewayDlg::OnBnClickedButtonExit()
{
	// TODO: 在此添加控件通知处理程序代码
	OnAppSuspend();
}


// 改变tray 图标,bIsRuning=是否服务在运行
void CViewGatewayDlg::SetTrayIcon(BOOL bIsRuning)
{
	if (bIsRuning)
	{
		//服务在运行,设置tray icon
		m_trayIcon.SetIcon(IDR_MAINFRAME);
		//m_ctrlAppIcon.SetIcon(m_hIcon);
	} 
	else
	{
		//服务没运行,设置tray icon
		m_trayIcon.SetIcon(IDI_ICON_GRAY);
		//m_ctrlAppIcon.SetIcon(m_hGrayIcon);
	}
}

void CViewGatewayDlg::OnStart()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedButtonStart();
}

void CViewGatewayDlg::UpdateMenuState()
{
	CString pstr;

	CMenu* pMenu=GetMenu();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bStart);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bStart);

	pMenu->EnableMenuItem(IDC_START,(m_bStart==FALSE)?MF_ENABLED:MF_GRAYED);
	pMenu->EnableMenuItem(IDC_STOP,(m_bStart==TRUE)?MF_ENABLED:MF_GRAYED);
	pMenu->EnableMenuItem(ID_RECONNECT,(m_bStart==TRUE)?MF_ENABLED:MF_GRAYED);

}
void CViewGatewayDlg::OnStop()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedButtonStop();
}


// 刷新统计信息
void CViewGatewayDlg::UpdateTotalInfo()
{

	// 服务务器更新和相关处理
	//m_VoiceGateway.m_TotalInfo.dwServers	= 0;		// 在线服务器数
	m_VoiceGateway.m_TotalInfo.dwClients = gClientContainer.Count();	// 客户端数
	m_VoiceGateway.m_TotalInfo.dwVoiceClients	= 0;	// 在线客户端数	TODO

	DWORD dwMaxUserCounts = gSetting.m_dwMaxConnection;	// 最大连接数

	const SViewTotalInfo& tInfo = m_VoiceGateway.m_TotalInfo;

    char szBuf[256];
    memset(szBuf, 0, sizeof(szBuf));
    sprintf_s(szBuf, _GT("%d.%s"), tInfo.dwWorldID, utf82a(tInfo.szWorldName));
    m_strWorld = szBuf;                                                                  // 游戏世界

    memset(szBuf, 0, sizeof(szBuf));
    sprintf_s(szBuf, _GT("%d.%s"), tInfo.dwGameID, utf82a(tInfo.szGameName));
    m_strGame = szBuf;                                                                  // 所属游戏

    memset(szBuf, 0, sizeof(szBuf));
    sprintf_s(szBuf, _GT("%d.%s"), tInfo.dwAreaID, utf82a(tInfo.szAreaName));
    m_strArea = szBuf;              

	m_strIsTemp.Format(_T(" %s"),(tInfo.bIsPublic?_T("公共"):_T("普通")));				// 所属是否公共游戏世界
	m_strServers.Format(_T(" %d 最大:%d"),tInfo.dwGatewayID,dwMaxUserCounts);			// 网关动态ID
	m_strClients.Format(_T(" %d 语音:%d"),tInfo.dwClients,tInfo.dwVoiceClients);		// 客户端数
	m_strServerPort.Format(_T(" %s:%d"), a2t(gSetting.m_strServerIp.c_str()), gSetting.m_dwServerPort);								// 服务器IP
	m_strClientPort.Format(_T(" %d  语音:%d"),tInfo.dwClientPort,tInfo.dwAudioPort);				// 客户端监听端口,语音端监听端口
	m_strRecvSpeedServer.Format(_T(" %s/s"),a2t(GetFileSizeString(tInfo.dwRecvSpeedServer)));			// 语音收包带宽
	m_strSendSpeedServer.Format(_T(" %s/s"),a2t(GetFileSizeString(tInfo.dwSendSpeedServer)));			// 语音发包带宽
	m_strRecvSpeedClient.Format(_T(" %s/s"),a2t(GetFileSizeString(tInfo.dwRecvSpeedClient)));			// 语音收包带宽
	m_strSendSpeedClient.Format(_T(" %s/s"),a2t(GetFileSizeString(tInfo.dwSendSpeedClient)));			// 语音发包带宽


	// 启动时间
	if (tInfo.dwStartTime==0)
	{
		m_strStartTime.Empty();
	}
	else
	{	
		char szBuf[512] = {0};
		GetRemainTimeString(szBuf,sizeof(szBuf),(DWORD)time(NULL)-tInfo.dwStartTime);
		m_strStartTime.Format(_T("服务器启动时间：%s 已运行：%s"),a2t(GetTimeString(tInfo.dwStartTime)),a2t(szBuf));
	}

	UpdateData(FALSE);
}


void CViewGatewayDlg::OnOutTotal()
{
	// TODO: 在此添加命令处理程序代码
	char szBuf[512] = {0};

	SViewTotalInfo pTInfo=m_VoiceGateway.m_TotalInfo;

	string strSendSpeed = GetFileSizeString(pTInfo.dwSendSpeedServer);
	string strRecvSpeed = GetFileSizeString(pTInfo.dwRecvSpeedServer);

	sprintf_s(szBuf,sizeof(szBuf),"输出统计信息:\n  游戏世界=%d.%s\n  所属是否公共=%d\n  所属游戏=%d.%s\n  游戏区域=%d.%s\n  服务器数=%d\n  客户端数=%d\n  服务器监听端口=%d\n  客户端端口=%d\n  收包速度=%s/s\n 发包速度=%s/s\n \n",
		pTInfo.dwWorldID,pTInfo.szWorldName,pTInfo.bIsPublic,pTInfo.dwGameID,pTInfo.szGameName,pTInfo.dwAreaID,pTInfo.szAreaName,
		pTInfo.dwServers,pTInfo.dwClients,pTInfo.dwServerPort,pTInfo.dwClientPort,strRecvSpeed.c_str(),strSendSpeed.c_str());
	TraceLn(szBuf);
}

void CViewGatewayDlg::OnShowListinfo()
{
	// TODO: 在此添加命令处理程序代码
	CListInfoDlg listInfoDlg;
	listInfoDlg.DoModal();
}


void CViewGatewayDlg::OnMgDlg()
{
	// TODO: 在此添加命令处理程序代码
	CString strMsgTitle;
	strMsgTitle.Format(_T("操作命令：1、导出用户列表  2、修改最大连接数"));

	// 关闭软件服务器
	COperationDlg opDlg;
	opDlg.m_InfoIcon		= 0;					// 取服务器类型取得图标ID
	opDlg.m_dwDelays		= 0;					// 操作延迟秒数
	opDlg.m_nMinTxtLens		= 0;					// 操作备注最少长度
	opDlg.m_strTxtName		= _T("角色PDBID或角色名称");			// 操作备注的名称
	opDlg.m_strDlgTitle		= _T("管理员操作");			// 对话框标题
	opDlg.m_strMsgTitle		= strMsgTitle;			// 提示标题信息
	opDlg.m_strTxtLabel		= _T("操作备注：(必需输入,内容不少于2个字符)");	// 操作备注标题

	opDlg.m_bUseNumber1		= true;					// 使用显示备用数字1
	opDlg.m_strNumberTitle1	= _T("操作命令:");			// 备用数字1的标题文字
	opDlg.m_nNumber1		= 3;					// 备用数字1
	opDlg.m_bUseNumber2		= true;					// 使用显示备用数字2
	opDlg.m_strNumberTitle2	= _T("操作参数:");			// 备用数字2的标题文字
	opDlg.m_nNumber2		= 0;					// 备用数字2
	opDlg.m_bEnterVerifyCode= true;


	// 类型列表
	opDlg.m_ctrlFlagList.AddString(_T("无选项"));
	opDlg.m_ctrlFlagList.EnableWindow(false);
	opDlg.m_editDelays.EnableWindow(false);

	if(IDOK!=opDlg.DoModal())
	{
		return;
	}

    // Convert a TCHAR string to a LPCSTR
    CT2CA pszConvertedAnsiString (opDlg.m_strTxt);
    // construct a std::string using the LPCSTR input
    std::string strStd (pszConvertedAnsiString);
	ErrorLn( "Manage Operate:Type:"<<opDlg.m_nNumber1<<",Parameter:"<<opDlg.m_nNumber2<<",Remark:"<< strStd);

	//opDlg.m_nNumber1		// 操作类型
	//opDlg.m_nNumber2		// 操作参数
	//opDlg.m_strTxt		// 操作备注

	if( opDlg.m_nNumber1 < VoiceCommand_First || opDlg.m_nNumber1 >= VoiceCommand_Max )
	{
		ErrorLn( _GT("操作命令错误！命令参数[") << opDlg.m_nNumber1 << "][" << opDlg.m_nNumber2 << "]" );
		return ;
	}

	while(true)
	{
		if( !m_VoiceGateway.m_bUpdate )
		{
			m_VoiceGateway.m_bUpdate = true;

			m_VoiceGateway.m_nNum1 = opDlg.m_nNumber1;
			m_VoiceGateway.m_nNum2 = opDlg.m_nNumber2;
			m_VoiceGateway.m_strTxt = strStd;
			m_VoiceGateway.m_bNewDataCome = true;

			m_VoiceGateway.m_bUpdate = false;

			break;
		}

		::Sleep(50);
	}
}

void CViewGatewayDlg::OnTracelevel()
{
	// TODO: 在此添加命令处理程序代码

	CString strAlert=_T("你要修改全局(文件和窗口)Trace级别吗？ 注：选否只修改窗口Trace级别");
	if(MessageBox(strAlert,_T("操作确认"),MB_YESNO|MB_ICONQUESTION)!=IDYES) 
	{
		int oldLevel = mTrace->getTraceLevel();
		int nlevel = trackTraceContextMenu(oldLevel);
		if (oldLevel!=nlevel)
		{
			mTrace->setTraceLevel(nlevel);
			WarningLn("Modify Trace Len："<<nlevel);
		}

		return;
	}

	int oldLevel = getTraceLevel();
	int nlevel = trackTraceContextMenu(oldLevel);
	if (oldLevel!=nlevel)
	{
		setTraceLevel(nlevel);
		WarningLn("Modify Trace Level："<<nlevel);
	}
}
