
// AutoClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoClient.h"
#include "AutoClientDlg.h"
#include "afxdialogex.h"
#include "Scheme.h"
#include "Client.h"

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


// CAutoClientDlg 对话框




CAutoClientDlg::CAutoClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoClientDlg::IDD, pParent)
	, m_dwTotalCount(1)
	, m_dwLoginInterval(20)
	, m_dwCurLoginID(0)
	, m_dwLoginCount(0)
	, m_pTraceListener(NULL)
	, m_nTraceLevel(levelAll)
    , m_nGameMode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CAutoClientDlg::~CAutoClientDlg()
{
	for (vector<CClient*>::iterator itr = m_clients.begin(); itr != m_clients.end(); ++itr)
	{
		delete (*itr);
	}
}

int CAutoClientDlg::GetGameMode()
{
    return m_nGameMode;
};

void CAutoClientDlg::ClientLogin(int id)
{
	m_dwLoginCount++;
	CString strCount;
	strCount.Format("%d", m_dwLoginCount);
	GetDlgItem(IDC_STATIC_NUM)->SetWindowText(strCount);
}

void CAutoClientDlg::ClientLogout(int id)
{
	if (m_dwLoginCount > 0) m_dwLoginCount--;
	CString strCount;
	strCount.Format("%d", m_dwLoginCount);
	GetDlgItem(IDC_STATIC_NUM)->SetWindowText(strCount);
}

CClient* CAutoClientDlg::GetAroundClient(CClient* pMyClient, float fDistance)
{
	// 找到附近的一个客户端
	Vector3& loc = pMyClient->GetLoc();
	for (vector<CClient*>::iterator itr = m_clients.begin(); itr != m_clients.end(); ++itr)
	{
		if (pMyClient != (*itr) && loc.getDistance((*itr)->GetLoc()) < fDistance)
		{
			return (*itr);
		}
	}
	// 实在找不到了返回自己吧...
	return pMyClient;
}

CClient* CAutoClientDlg::GetClient(int id)
{
    for (vector<CClient*>::iterator itr = m_clients.begin(); itr != m_clients.end(); ++itr)
    {
        if ((*itr)->GetAccountID() == id)
            return *itr;
    }
    return NULL;
}

void CAutoClientDlg::ReleaseClient(CClient* pClient)
{
	for (vector<CClient*>::iterator itr = m_clients.begin(); itr != m_clients.end(); ++itr)
	{
		if ((*itr) == pClient)
		{
			ClientLogout(0);
			delete pClient;
			m_clients.erase(itr);
			break;
		}
	}
}

void CAutoClientDlg::Login()
{
    // TODO: 在此添加控件通知处理程序代码
    // 点击开启自动化客户端测试程序
    UpdateData(TRUE);

    GetDlgItem(IDC_MODE_MMO)->EnableWindow(FALSE);
    GetDlgItem(IDC_MODE_MOBA)->EnableWindow(FALSE);

    CString strIPAddr;
    m_ctrlIPAddr.GetWindowText(strIPAddr);
    m_TimeSyncer.Start(strIPAddr.GetString(), 6600, 6601, &m_timerAxis);
    SetTimer(1, m_dwLoginInterval, NULL);
}


void CAutoClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwTotalCount);
	DDV_MinMaxUInt(pDX, m_dwTotalCount, 1, 10000);
	DDX_Text(pDX, IDC_EDIT2, m_dwLoginInterval);
	DDV_MinMaxUInt(pDX, m_dwLoginInterval, 10, 2000);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ctrlIPAddr);
	DDX_Control(pDX, IDC_RICHEDIT21, m_RichEdit);
}

BEGIN_MESSAGE_MAP(CAutoClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_MODE_MMO, &CAutoClientDlg::OnLoginMMO)
    ON_BN_CLICKED(IDC_MODE_MOBA, &CAutoClientDlg::OnLoginMOBA)
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CAutoClientDlg 消息处理程序

BOOL CAutoClientDlg::OnInitDialog()
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

	// 创建视图窗体
	/*
	CWnd* pStaticWnd = GetDlgItem(IDC_RICHEDIT21);
	ASSERT(pStaticWnd);
	RECT rect;
	pStaticWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	pStaticWnd->DestroyWindow();
	m_RichEdit.Create(AFX_WS_DEFAULT_VIEW, rect, this, IDC_RICHEDIT21);
	*/
	m_pTraceListener = output2RichEdit(m_RichEdit,m_nTraceLevel,true);
	// TODO: 在此添加额外的初始化代码
	if (InitializeNetwork() < 0)
	{
		ErrorLn("Initialize Network failed!");
	}

	// 读取相关配置
	CString filePath;
	GetModuleFileName(NULL, filePath.GetBuffer(MAX_PATH), MAX_PATH);
	filePath.ReleaseBuffer();

	filePath = filePath.Left(filePath.ReverseFind('\\'));

	CString filePathName = filePath + "\\Account.csv";
	if (!g_accountReader.ReadFile(filePathName.GetString()))
	{
		ErrorLn("Read File Account.csv failed");
	}

	filePathName = filePath + "\\Action.csv";
	if (!g_actionReader.ReadFile(filePathName.GetString()))
	{
		ErrorLn("Read File Account.csv failed");
	}

	m_ctrlIPAddr.SetWindowText("127.0.0.1");

	SetTimer(0, 20, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutoClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAutoClientDlg::OnPaint()
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
HCURSOR CAutoClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAutoClientDlg::OnLoginMMO()
{
    m_nGameMode = MODE_MMO;
    Login();
}


void CAutoClientDlg::OnLoginMOBA()
{
    m_nGameMode = MODE_MOBA;
    Login();
}


void CAutoClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnTimer(nIDEvent);
	switch (nIDEvent)
	{
	case 0:
		{
			DispatchNetworkEvents(100, NetworkUserType_Default);
			m_timerAxis.CheckTimer();
		}
		break;

	case 1:
		{
			if (m_dwCurLoginID < m_dwTotalCount)
			{
				++m_dwCurLoginID;
				CCSVReader<int, SchemeAccount>::iterator itr = g_accountReader.find(m_dwCurLoginID);
				if (itr != g_accountReader.end())
				{
					CClient* pClient = new CClient(itr->second, this);
					CString strIPAddr;
					m_ctrlIPAddr.GetWindowText(strIPAddr);
					if (pClient->ConnectServer(strIPAddr.GetString(), 5678))
					{
						m_clients.push_back(pClient);
					}
					else
					{
						ErrorLn("Client " << m_dwCurLoginID << "connect to server " << strIPAddr.GetString() << ":5678 failed！");
						delete pClient;
					}
				}
			}
			else
			{
				KillTimer(1);
			}
		}

	default: break;
	}
}


void CAutoClientDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int nNewLevel = trackTraceContextMenu(m_nTraceLevel);
	if (nNewLevel != m_nTraceLevel)
	{
		rkt::setTraceLevel(nNewLevel);
		EmphasisLn("trace等级被修改为" << nNewLevel);
		m_nTraceLevel = nNewLevel;
	}
}

