
// SocialServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SocialServer.h"
#include "SocialServerDlg.h"
#include "afxdialogex.h"

#include "ServerControl.h"
#include "ForceUnlockDialog.h"

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


// CSocialServerDlg 对话框



CSocialServerDlg * g_pServerDlg = NULL;

CSocialServerDlg::CSocialServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSocialServerDlg::IDD, pParent)
    , m_pTraceListener(NULL)
    , m_nTraceLevel(levelAll)
    , m_dwLastUpdateQueueState(0)
    , m_editUpdateLuaDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_stringTraceFile.clear();
    g_pServerDlg = this;
}

void CSocialServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICHEDIT_OUTPUT, m_richEditOutput);
    DDX_Control(pDX, IDC_COMBO_SCHEME, m_ComboBoxScheme);
    DDX_Text(pDX, IDC_EDIT_LUA, m_editUpdateLuaDir);
}

BEGIN_MESSAGE_MAP(CSocialServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CSocialServerDlg::OnBnClickedStartservice)
    ON_WM_CONTEXTMENU()
    ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SHOWTRACE, &CSocialServerDlg::OnBnClickedButtonShowtrace)
    ON_CBN_SETFOCUS(IDC_COMBO_SCHEME, &CSocialServerDlg::OnCbnSetfocusComboScheme)
    ON_BN_CLICKED(IDC_BTN_UPDATESCHEME, &CSocialServerDlg::OnBnClickedBtnUpdatescheme)
    ON_BN_CLICKED(IDC_BTN_STOPSERVICE, &CSocialServerDlg::OnBnClickedStopservice)
    ON_BN_CLICKED(IDC_BTN_OUTPUT_PERFORMACNE, &CSocialServerDlg::OnBnClickedBtnOutputPerformacne)
    ON_BN_CLICKED(IDC_BUTTON_RESETPRO_QUEUESTATE, &CSocialServerDlg::OnBnClickedButtonResetproQueuestate)
    ON_BN_CLICKED(IDC_BUTTON_SCANLUA, &CSocialServerDlg::OnBnClickedButtonScanlua)
    ON_BN_CLICKED(IDC_BUTTON_LUA, &CSocialServerDlg::OnBnClickedButtonLua)
    ON_BN_CLICKED(IDC_BUTTON_FORCEUNLOCK, &CSocialServerDlg::OnBnClickedButtonForceunlock)
END_MESSAGE_MAP()


// CSocialServerDlg 消息处理程序

BOOL CSocialServerDlg::OnInitDialog()
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
	// 执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_SHOWNORMAL);

	// TODO: 在此添加额外的初始化代码
    // 创建trace
    HWND hwndTrace = GetDlgItem(IDC_RICHEDIT_OUTPUT)->GetSafeHwnd();
    m_pTraceListener = output2RichEdit((void *)hwndTrace, levelAll, true);
    if(m_pTraceListener == NULL)
    {
        return false;
    }

    // 初始化服务器环境
    bool bInitResult = m_socialGlobal.InitEnvironment();
    if(!bInitResult)
    {	
        g_ServerControl.GotoState(SERVER_RUN_STATE_INIT_FAIL);
        return FALSE;
    }
    g_ServerControl.GotoState(SERVER_RUN_STATE_INIT_SUCCESS);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSocialServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSocialServerDlg::OnPaint()
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
HCURSOR CSocialServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 由于现在的框架要先启动Framework才能继续开启Service，所以此函数不设限制
void CSocialServerDlg::OnBnClickedStartservice()
{
    int nRunState = g_ServerControl.GetServerRunState();
    if(nRunState == SERVER_RUN_STATE_CREATE_SERVICE)
    {
        return;
    }

    g_pServerDlg->SetWindowText(_T("TITAN-社会服服务器"));
    // 开启GlobalService
    m_socialGlobal.Start(0);

    BEGIN_WINDOW();
    ENABLE_WINDOW(IDC_COMBO_SCHEME, true);
    ENABLE_WINDOW(IDOK, false);
    ENABLE_WINDOW(IDC_BTN_STOPSERVICE, true);

    // 初始化动作
    //g_ServerControl.SetUIControl(UICONTROL_INIT_SUCCESS);
}

void CSocialServerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
    int nNewLevel = trackTraceContextMenu(m_nTraceLevel);
    if (nNewLevel != m_nTraceLevel)
    {
        rkt::setTraceLevel(nNewLevel);
        EmphasisLn("TRACE LEVEL = " << nNewLevel);
        m_nTraceLevel = nNewLevel;
    }
}

void CSocialServerDlg::OnBnClickedButtonShowtrace()
{
	// TODO: 在此添加控件通知处理程序代码
	shellExecute("open", m_stringTraceFile.c_str());
}


BOOL CSocialServerDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->wParam == VK_ESCAPE ||pMsg->wParam == VK_RETURN)
        return true;

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSocialServerDlg::OnCbnSetfocusComboScheme()
{
    // TODO: 在此添加控件通知处理程序代码
    if(gSocialGlobal == NULL)
    {
        return;
    }

    ISchemeEngine * pSchemeEngine = gSocialGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return;
    }

    // 清除以前的列表
    m_ComboBoxScheme.SetCurSel(-1);
    m_ComboBoxScheme.ResetContent();

    // 压入
    list<string> * plistScheme = pSchemeEngine->GetDynamicSchemeFileName();
    list<string>::iterator it = plistScheme->begin();
    for( ; it != plistScheme->end(); ++it)
    {
        m_ComboBoxScheme.AddString(CString((*it).c_str()));
    }
}


void CSocialServerDlg::OnBnClickedBtnUpdatescheme()
{
    // TODO: 在此添加控件通知处理程序代码
    int nCurSel = m_ComboBoxScheme.GetCurSel();
    if(nCurSel < 0)
    {
        return;
    }

    CString rString;
    m_ComboBoxScheme.GetLBText(nCurSel, rString);
    if(rString.IsEmpty())
    {
        return;
    }

    g_ServerControl.m_stringWaitDynamic.clear();

#ifdef RKT_UTF8
    // Convert a TCHAR string to a LPCSTR
    CT2CA pszConvertedAnsiString (rString);
    // construct a std::string using the LPCSTR input
    std::string strStd (pszConvertedAnsiString);
    g_ServerControl.m_stringWaitDynamic = strStd;
#else
    g_ServerControl.m_stringScpPath = rString;
#endif    
    g_ServerControl.SetUIControl(UICONTROL_UPDATE_SCHEME);
}


void CSocialServerDlg::OnBnClickedStopservice()
{
    // TODO: 在此添加控件通知处理程序代码
    m_socialGlobal.Stop();

    BEGIN_WINDOW();
    ENABLE_WINDOW(IDOK, true);
    ENABLE_WINDOW(IDC_BTN_STOPSERVICE, false);
}


void CSocialServerDlg::OnBnClickedBtnOutputPerformacne()
{
    // TODO: 在此添加控件通知处理程序代码
    IFramework* pFramework = m_socialGlobal.getFramework();
    if(pFramework == NULL)
    {
        return;
    }
    pFramework->dump_service_runtime_info();
}


void CSocialServerDlg::OnBnClickedButtonResetproQueuestate()
{
    if(GetTickCount() - m_dwLastUpdateQueueState <= 5000)
    {
        return;
    }
    m_dwLastUpdateQueueState = GetTickCount();

    // TODO: 在此添加控件通知处理程序代码
    g_ServerControl.SetUIControl(UICONTROL_QUEUESTATE);	
}


void CSocialServerDlg::OnBnClickedButtonScanlua()
{
    // TODO: 在此添加控件通知处理程序代码
    char szOldCurrentDirector[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, szOldCurrentDirector);	
    CFileDialog FileDialog(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Lua Files (*.lua)|*.lua|All Files (*.*)|*.*||"), this);
    FileDialog.DoModal();
    m_editUpdateLuaDir = FileDialog.GetPathName();
    UpdateData(FALSE);
}


void CSocialServerDlg::OnBnClickedButtonLua()
{
    // TODO: 在此添加控件通知处理程序代码
    if(m_editUpdateLuaDir.IsEmpty())
    {
        return;
    }

    g_ServerControl.m_stringWaitDynamic.clear();

#ifdef RKT_UTF8
    // Convert a TCHAR string to a LPCSTR
    CT2CA pszConvertedAnsiString (m_editUpdateLuaDir);
    // construct a std::string using the LPCSTR input
    std::string strStd (pszConvertedAnsiString);
    g_ServerControl.m_stringWaitDynamic = strStd;
#else
    g_SceneServerControl.m_stringWaitDynamic = rString;
#endif  

    g_ServerControl.SetUIControl(UICONTROL_UPDATE_LUA);
}


void CSocialServerDlg::OnBnClickedButtonForceunlock()
{
    // TODO: 在此添加控件通知处理程序代码
    ForceUnlockDialog	dlg;
    dlg.DoModal();
}
