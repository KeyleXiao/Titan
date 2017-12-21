
// SceneServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneServer.h"
#include "SceneServerDlg.h"
#include "afxdialogex.h"


#include "IPerformanceService.h"
#include "SceneServerControl.h"

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


// CSceneServerDlg 对话框


CSceneServerDlg * g_pSceneServerDlg = NULL;

CSceneServerDlg::CSceneServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pTraceListener = 0;

    g_pSceneServerDlg = this;
}

void CSceneServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB_FRAME, m_TabFrame);
}

BEGIN_MESSAGE_MAP(CSceneServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(WM_CTR_CLOSEDLG, OnCtrCloseDlg)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FRAME, &CSceneServerDlg::OnTcnSelchangeTabFrame)
END_MESSAGE_MAP()


// CSceneServerDlg 消息处理程序

BOOL CSceneServerDlg::OnInitDialog()
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

    ///////////////////////////////////////////////////////////////////
    // 创建第一个对话页
    TCITEM itemStart;
    itemStart.mask = TCIF_TEXT;
    itemStart.pszText = L"启动";
    m_TabFrame.InsertItem (0, &itemStart);
    m_PageDialogStart.Create(IDD_DIALOG_PAGESTART, &m_TabFrame);
    if(!m_PageDialogStart.InitDialog())
    {
        return FALSE;
    }	
    CRect rectTab; m_TabFrame.GetClientRect(&rectTab); CRect rectStart; m_PageDialogStart.GetClientRect(&rectStart);

    /*  if(theApp.m_nDefaultSvrID <= 0)
    {*/
        m_PageDialogStart.SetWindowPos(NULL, rectTab.left + 2, rectTab.top + 22, rectStart.right, rectStart.bottom, SWP_SHOWWINDOW);
    //}
    //else
    //{
    //    m_PageDialogStart.SetWindowPos(NULL, rectTab.left + 5, rectTab.top + 20, rectStart.right, rectStart.bottom, SWP_SHOWWINDOW);
    //    m_TabFrame.SetCurSel(0);
    //}

    ///////////////////////////////////////////////////////////////////
    // 创建第二个对话页
    TCITEM itemSetting;
    itemSetting.mask = TCIF_TEXT;
    itemSetting.pszText = L"配置";
    m_TabFrame.InsertItem (1, &itemSetting);
    m_PageDialogSetting.Create(IDD_DIALOG_PAGESETTING, &m_TabFrame);
    if(!m_PageDialogSetting.InitDialog())
    {
        return FALSE;
    }
    rectTab; m_TabFrame.GetClientRect(&rectTab); rectStart; m_PageDialogSetting.GetClientRect(&rectStart);
    //if(theApp.m_nDefaultSvrID < 0)
    //{
    //    m_PageDialogSetting.SetWindowPos(NULL, rectTab.left + 5, rectTab.top + 20, rectStart.right, rectStart.bottom, SWP_SHOWWINDOW);
    //    m_TabFrame.SetCurSel(1);
    //}
    //else
    //{
        m_PageDialogSetting.SetWindowPos(NULL, rectTab.left+2, rectTab.top + 22, rectStart.right, rectStart.bottom, SWP_HIDEWINDOW);
    //}


    ///////////////////////////////////////////////////////////////////
    // 创建第三个对话页
    TCITEM itemInfo;
    itemInfo.mask = TCIF_TEXT;
    itemInfo.pszText = L"信息";
    m_TabFrame.InsertItem (2, &itemInfo);
    m_DialogPageMapinfo.Create(IDD_DIALOG_PAGEINFO, &m_TabFrame);
    if(!m_DialogPageMapinfo.InitDialog())
    {
        return FALSE;
    }
    rectTab; m_TabFrame.GetClientRect(&rectTab); rectStart; m_DialogPageMapinfo.GetClientRect(&rectStart);
    m_DialogPageMapinfo.SetWindowPos(NULL, rectTab.left + 2, rectTab.top + 22, rectStart.right, rectStart.bottom, SWP_HIDEWINDOW);

    //if(theApp.m_nDefaultSvrID <= 0)
    //{
    //    m_PageDialogSetting.RefreshDialog(0/*默认显示场景服1的配置*/);
    //}
    //else
    //{
    //    m_PageDialogSetting.RefreshDialog(theApp.m_nDefaultSvrID);
    //}
    ///////////////////////////////////////////////////////////////////
    // 初始化服务器环境
    bool bInitResult = m_global.InitEnvironment();
    if(!bInitResult)
    {	
        g_SceneServerControl.GotoState(SERVER_RUN_STATE_INIT_FAIL);
        return FALSE;
    }
    g_SceneServerControl.GotoState(SERVER_RUN_STATE_INIT_SUCCESS);

//#ifdef OPEN_BVTTEST
//	// 性能按钮处理
//	GetDlgItem(IDC_BUTTON_PROFILE_START)->ShowWindow(SW_SHOW);
//	GetDlgItem(IDC_BUTTON_PROFILE_OPENUI)->ShowWindow(SW_SHOW);
//	GetDlgItem(IDC_BUTTON_PROFILE_STOP)->ShowWindow(SW_SHOW);
//#endif

	SetTimer(0,1000,0);

   UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSceneServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSceneServerDlg::OnPaint()
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
HCURSOR CSceneServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


////////////////////////////////////////////////////////////////////////////////////////////
// WM_CTR_CLOSEDLG
LRESULT  CSceneServerDlg::OnCtrCloseDlg(WPARAM wParam,LPARAM lParam)
{
    SendMessage(WM_CLOSE, 0, 0);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
void CSceneServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( m_pTraceListener )
	{
		rkt::dispatch2RichEdit(m_pTraceListener);
	}

	m_global.OnTimer();

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CSceneServerDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->wParam == VK_ESCAPE ||pMsg->wParam == VK_RETURN)
        return true;

    return CDialogEx::PreTranslateMessage(pMsg);
}

void CSceneServerDlg::OnTcnSelchangeTabFrame(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    switch(m_TabFrame.GetCurSel())
    {
    case 0:
        {
            m_PageDialogStart.ShowWindow(SW_SHOW);
            m_PageDialogSetting.ShowWindow(SW_HIDE);
            m_DialogPageMapinfo.ShowWindow(SW_HIDE);
        }		
        break;
    case 1:
        {
            m_PageDialogStart.ShowWindow(SW_HIDE);
            m_PageDialogSetting.ShowWindow(SW_SHOW);
            m_PageDialogSetting.RefreshDialog(m_global.getServerID());
            m_DialogPageMapinfo.ShowWindow(SW_HIDE);
        }
        break;
    case 2:
        {
            m_PageDialogStart.ShowWindow(SW_HIDE);
            m_PageDialogSetting.ShowWindow(SW_HIDE);
            m_DialogPageMapinfo.ShowWindow(SW_SHOW);
        }
        break;
    default:break;
    }

    *pResult = 0;
}
