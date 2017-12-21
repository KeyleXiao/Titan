/*******************************************************************
** 文件名:	GuardServerDlg.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器主程序界面
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


// GuardServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GuardServer.h"
#include "GuardServerDlg.h"
#include <time.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



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


// GuardServerDlg 对话框


CGuardServerDlg::CGuardServerDlg(CWnd* pParent /*=NULL*/)
	: CResizingDialog(CGuardServerDlg::IDD, pParent)
	, m_trayIcon(IDR_TRAYICON)
	, m_strWorld(_T(""))
	, m_strIsTemp(_T(""))
	, m_strGame(_T(""))
	, m_strArea(_T(""))
	, m_strServers(_T(""))
	, m_strMaxServer(_T(""))
	, m_strServerPort(_T(""))
	, m_strStartTime(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hGrayIcon = AfxGetApp()->LoadIcon(IDI_ICON_GRAY);

	SetControlInfo(IDC_OUTPUT,	ANCHORE_LEFT | RESIZE_BOTH);
	SetControlInfo(IDC_STATIC_LINE,	ANCHORE_LEFT | RESIZE_HOR);
	SetControlInfo(IDC_EDIT_STARTTIME,	ANCHORE_LEFT | ANCHORE_BOTTOM | RESIZE_HOR);
}

void CGuardServerDlg::DoDataExchange(CDataExchange* pDX)
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
	DDX_Text(pDX, IDC_EDIT_PASSRATE, m_strMaxServer);
	DDX_Text(pDX, IDC_EDIT_IP, m_strServerPort);
	DDX_Text(pDX, IDC_EDIT_STARTTIME, m_strStartTime);

}

BEGIN_MESSAGE_MAP(CGuardServerDlg, CResizingDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_OPEN, &CGuardServerDlg::OnAppOpen)
	ON_COMMAND(ID_APP_ABOUT, &CGuardServerDlg::OnAppAbout)
	ON_COMMAND(ID_APP_SUSPEND, &CGuardServerDlg::OnAppSuspend)
	ON_BN_CLICKED(IDC_BUTTON_START, &CGuardServerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CGuardServerDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CGuardServerDlg::OnBnClickedButtonExit)
	ON_COMMAND(IDC_START, &CGuardServerDlg::OnStart)
	ON_COMMAND(IDC_STOP, &CGuardServerDlg::OnStop)
	ON_COMMAND(ID_OUT_TOTAL, &CGuardServerDlg::OnOutTotal)
	ON_COMMAND(ID_SHOW_LISTINFO, &CGuardServerDlg::OnShowListinfo)

	ON_MESSAGE(WM_MY_START_RESULT,&CGuardServerDlg::OnMsgStartResult)
	ON_MESSAGE(WM_MY_STOP_RESULT,&CGuardServerDlg::OnMsgStopResult)
	ON_MESSAGE(WM_MY_CLOSE_APP,&CGuardServerDlg::OnMsgCloseApp)
END_MESSAGE_MAP()


// CDataGuardServerDlg 消息处理程序

BOOL CGuardServerDlg::OnInitDialog()
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

	m_trayIcon.SetIcon(m_hGrayIcon,SERVICE_TRAYICON_TIP);
	//m_ctrlAppIcon.SetIcon(m_hGrayIcon);

	// 初始化图标列表
	m_TreeImgList.Create(16, 16,ILC_COLOR24|ILC_MASK,8,10);
	m_TreeImgList.SetBkColor(CLR_NONE);

	m_bShutdown=FALSE;
	m_bStart=FALSE;
	nTimeCount=0;
	nClientCount=1;
	m_bOnTimerUpdate=false;

	// center the dialog on the screen
	CenterWindow();

	//bool old = setTraceHasTime(false);

	// 取得本地路径
	int nDirLen = sizeof(m_GuardServer.m_szWorkDir);
	sstrcpyn(m_GuardServer.m_szWorkDir, getWorkDir(), nDirLen);
	getFileSystem()->addFindPath(m_GuardServer.m_szWorkDir);

	// 取得服务器程序Bin路径
	//m_szServerBinWorkDir
	CString strWorkPath = my_utf82t(getWorkDir());
	CString strUpPath;
	if (strWorkPath.GetLength()>0 && strWorkPath.Find('\\')>0)
	{
		int nPos = strWorkPath.ReverseFind('\\');
		strUpPath = strWorkPath.Mid(0,nPos);
	}

	sstrcpyn(m_GuardServer.m_szServerBinWorkDir, my_t2utf8(strUpPath.GetString()), sizeof(m_GuardServer.m_szServerBinWorkDir));

	TraceLn("Server Bin Path:"<<m_GuardServer.m_szServerBinWorkDir);

	// 是否有log目录
	char szLogAbsolutePath[512] = { 0 };
	sprintf_s(szLogAbsolutePath, "%s\\Log\\DataGuardServer",m_GuardServer.m_szWorkDir);
	bool isAbsolutePath = true; uint attrib = 0;
	if(!rkt::checkPath(szLogAbsolutePath, isAbsolutePath, attrib))
	{
		// 创建log目录
		rkt::createDirectoryRecursive(szLogAbsolutePath);
	}

	// trace文件地址	
	time_t	long_time; struct tm * local_time;
	time(&long_time); local_time = localtime(&long_time);

	sprintf_s(m_GuardServer.szTraceFileName, "%s\\Log\\GuardServer\\Guard%d年%.2d月%.2d日%.2d时%.2d分%.2d秒_Trace.html", m_GuardServer.m_szWorkDir,
		local_time->tm_year+1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);
	sprintf_s(m_GuardServer.szCrashFileName, "%s\\Log\\GuardServer\\Guard%d年%.2d月%.2d日%.2d时%.2d分%.2d秒_Crash.txt", m_GuardServer.m_szWorkDir,
		local_time->tm_year+1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);
	sprintf_s(m_GuardServer.szDumpFileName, "%s\\Log\\GuardServer\\Guard%d年%.2d月%.2d日%.2d时%.2d分%.2d秒_Stack.dmp",  m_GuardServer.m_szWorkDir,
		local_time->tm_year+1900,
		local_time->tm_mon + 1,
		local_time->tm_mday,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);



	TraceListener * pTraceListener = output2File(m_GuardServer.szTraceFileName);
	if(pTraceListener != NULL)
	{
		pTraceListener->setHasTime(true);
	}

	mTrace = output2RichEdit((void*)(HWND)GetDlgItem(IDC_OUTPUT)->GetSafeHwnd(), levelAll, true);


	TraceLn("TraceFileName："<<m_GuardServer.szTraceFileName);
	TraceLn("CrashFileName："<<m_GuardServer.szCrashFileName);
	TraceLn("DumpFileName："<<m_GuardServer.szDumpFileName);

	SetTimer( 1, 1000, NULL );

	// 初始化启动环境
	m_GuardServer.InitEnvironment();

	//  中心服务器ＩＰ,端口
	ShowCenterServerConfig();

	UpdateMenuState();

	TraceLn(_GT("提示：\n 1.点击窗口的关闭按钮时不是关闭程序，而是最小化到任务栏图标！\n 2.并且这样可大大减小界面更新频率从而节省界面处理的CPU占用率！\n 3.服务未启动时任务栏图标为灰色，服务启动变为彩色！"));

	TraceLn(_GT("10秒后如没有手动启动服务,将会自动启动服务，并最小化到任务栏图标！"));

#ifdef GUARD_PRINT_DEBUG_INF
// 打印调试信息,并设为测试模式
	WarningLn("当前为调试模式,会打印调试信息！");
#endif


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//更新修改的中心服务器ＩＰ,端口到m_BridgeServer.m_strCenterServerIP,m_BridgeServer.m_nPort
void CGuardServerDlg::UpdateCenterServerConfig()
{
	UpdateData(TRUE);
}

//更新界面中心服务器ＩＰ,端口
void CGuardServerDlg::ShowCenterServerConfig()
{
	UpdateData(FALSE);
}


void CGuardServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGuardServerDlg::OnPaint()
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
HCURSOR CGuardServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 关闭程序
void CGuardServerDlg::OnClose()
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
LRESULT CGuardServerDlg::OnMsgStartResult(WPARAM wParam, LPARAM lParam)
{
	m_bStart =(wParam==1?TRUE:FALSE);

	UpdateMenuState();

	SetTrayIcon(m_bStart);

	UpdateTotalInfo();

	return 0;	
}
// 关闭结果message 
LRESULT CGuardServerDlg::OnMsgStopResult(WPARAM wParam, LPARAM lParam)
{
	m_bStart =(wParam==1?TRUE:FALSE);

	UpdateMenuState();

	SetTrayIcon(m_bStart);

	return 0;
}
// 关闭程序message 
LRESULT CGuardServerDlg::OnMsgCloseApp(WPARAM wParam, LPARAM lParam)
{
	// 	退出程处理代码
	CResizingDialog::OnCancel();

	return 0;
}

// 点任务栏图标菜单的"打开"显示程序界面
void CGuardServerDlg::OnAppOpen()
{
	// TODO: 在此添加命令处理程序代码
	ShowWindow(SW_NORMAL);	
	SetForegroundWindow();
}

// 点任务栏图标菜单的"关于..."显示关于对话框
void CGuardServerDlg::OnAppAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

// 点任务栏图标菜单的"退出"关闭程序
void CGuardServerDlg::OnAppSuspend()
{
	// TODO: 在此添加命令处理程序代码

	// 停止服务
	if (m_GuardServer.IsStart())
	{
		CString strAlert=_T("数据监护服务器正在运行中，你确定要停止服务并退出程序吗？");
		if(MessageBox(strAlert,_T("操作确认"),MB_YESNO|MB_ICONQUESTION)!=IDYES) 
		{
			return;
		}

		Trace(_GT("停止数据监护服务器 ...")<<endl);

		// 关闭程序,停止服务后要关闭进程
		m_GuardServer.SetControlCode(GS_ControlCode_ShutDown);
	}
	else
	{
		// 	退出程处理代码
		CResizingDialog::OnCancel();
	}
}

// 截获 onCancel,不关闭程序,只隐藏
void CGuardServerDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	ShowWindow(SW_HIDE);
	//CResizingDialog::OnCancel();
}

// 截获 OnOK,不关闭程序,只隐藏
void CGuardServerDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	ShowWindow(SW_HIDE);
	//CResizingDialog::OnOK();
}

// 更新日志到输出框
void CGuardServerDlg::UpdateTraceInfo2RichEdit()
{
	if (mTrace)
		dispatch2RichEdit(mTrace);
}

//启动服务
void CGuardServerDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码

	//更新修改的中心服务器ＩＰ,端口到m_BridgeServer.m_strCenterServerIP,m_BridgeServer.m_nPort
	UpdateCenterServerConfig();

	TraceLn("Start DataGuardServer ...");

	// 启动服务
	m_GuardServer.SetControlCode(GS_ControlCode_StartService);

	m_bStart = TRUE;

	UpdateMenuState();

	SetTrayIcon(m_bStart);

	UpdateTotalInfo();
}

//停止服务
void CGuardServerDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	
	// 由于停服务后再开有问题，直接调用推出
	StopService();
	
	//OnAppSuspend();

}

//停止服务
bool CGuardServerDlg::StopService()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_bStart)
	{
		return false;
	}

	CString strAlert=_T("数据监护服务器正在运行中，你确定要停止服务并退出程序吗？");
	if(MessageBox(strAlert,_T("操作确认"),MB_YESNO|MB_ICONQUESTION)!=IDYES) 
	{
		return false;
	}

	TraceLn("Stop DataGuardServer ...");

	// 停止服务
	m_GuardServer.SetControlCode(GS_ControlCode_StopService);

	m_bStart = FALSE;

	UpdateMenuState();

	SetTrayIcon(m_bStart);

	return true;
}

void CGuardServerDlg::OnTimer(UINT_PTR nIDEvent)
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
void CGuardServerDlg::OnBnClickedButtonExit()
{
	// TODO: 在此添加控件通知处理程序代码
	OnAppSuspend();
}


// 改变tray 图标,bIsRuning=是否服务在运行
void CGuardServerDlg::SetTrayIcon(BOOL bIsRuning)
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

void CGuardServerDlg::OnStart()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedButtonStart();
}

void CGuardServerDlg::UpdateMenuState()
{
	CString pstr;

	CMenu* pMenu=GetMenu();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bStart);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bStart);

	pMenu->EnableMenuItem(IDC_START,(m_bStart==FALSE)?MF_ENABLED:MF_GRAYED);
	pMenu->EnableMenuItem(IDC_STOP,(m_bStart==TRUE)?MF_ENABLED:MF_GRAYED);
	pMenu->EnableMenuItem(ID_RECONNECT,(m_bStart==TRUE)?MF_ENABLED:MF_GRAYED);

}
void CGuardServerDlg::OnStop()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedButtonStop();
}


// 刷新统计信息
void CGuardServerDlg::UpdateTotalInfo()
{

	// 服务务器更新和相关处理
	//m_GuardServer.m_TotalInfo.dwServers	= ServerList::getInstance().Count();	// 在线服务器数
	//m_GuardServer.m_TotalInfo.dwMaxServerID = ServerList::getInstance().GetMaxServerID();	// 最大服务器ID

	//SBridgeTotalInfo pTInfo=m_GuardServer.m_TotalInfo;

	//m_strWorld.Format(_T(" %d.%s"),pTInfo.dwWorldID,pTInfo.szWorldName);				// 游戏世界
	//m_strIsTemp.Format(_T(" %s"),(pTInfo.bIsPublic? _T("公共"):_T("普通")));			// 所属是否公共游戏世界
	//m_strGame.Format(_T(" %d.%s"),pTInfo.dwGameID,pTInfo.szGameName);					// 所属游戏
	//m_strArea.Format(_T(" %d.%s"),pTInfo.dwAreaID,pTInfo.szAreaName);					// 游戏区域
	//m_strServers.Format(_T(" %d/%d"),pTInfo.dwWorldCounts,pTInfo.dwServers);			// 软件服务器数
	//m_strMaxServer.Format(_T(" %d"),pTInfo.dwMaxServerID);								// 最大软件服ID
	//m_strServerPort.Format(_T(" %d"),pTInfo.dwServerPort);								// 服务器监听端口
	//m_strPubWorldID.Format(_T(" %d"),pTInfo.dwPublicWorldID);							// 公共游戏世界ID


	//// 启动时间
	//if (pTInfo.dwStartTime==0)
	//{
	//	m_strStartTime.Empty();
	//}
	//else
	//{	
	//	char szBuf[512] = {0};
	//	GetRemainTimeString(szBuf,sizeof(szBuf),(DWORD)time(NULL)-pTInfo.dwStartTime);
	//	m_strStartTime.Format(_T("服务器启动时间：%s 已运行：%s"),GetTimeString(pTInfo.dwStartTime),my_utf82t(szBuf));
	//}

	UpdateData(FALSE);
}


void CGuardServerDlg::OnOutTotal()
{
	// TODO: 在此添加命令处理程序代码

}

void CGuardServerDlg::OnShowListinfo()
{
	// TODO: 在此添加命令处理程序代码
}

