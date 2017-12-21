
// SocialServerDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CSocialServerDlg �Ի���



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


// CSocialServerDlg ��Ϣ�������

BOOL CSocialServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	// ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_SHOWNORMAL);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    // ����trace
    HWND hwndTrace = GetDlgItem(IDC_RICHEDIT_OUTPUT)->GetSafeHwnd();
    m_pTraceListener = output2RichEdit((void *)hwndTrace, levelAll, true);
    if(m_pTraceListener == NULL)
    {
        return false;
    }

    // ��ʼ������������
    bool bInitResult = m_socialGlobal.InitEnvironment();
    if(!bInitResult)
    {	
        g_ServerControl.GotoState(SERVER_RUN_STATE_INIT_FAIL);
        return FALSE;
    }
    g_ServerControl.GotoState(SERVER_RUN_STATE_INIT_SUCCESS);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSocialServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSocialServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// �������ڵĿ��Ҫ������Framework���ܼ�������Service�����Դ˺�����������
void CSocialServerDlg::OnBnClickedStartservice()
{
    int nRunState = g_ServerControl.GetServerRunState();
    if(nRunState == SERVER_RUN_STATE_CREATE_SERVICE)
    {
        return;
    }

    g_pServerDlg->SetWindowText(_T("TITAN-����������"));
    // ����GlobalService
    m_socialGlobal.Start(0);

    BEGIN_WINDOW();
    ENABLE_WINDOW(IDC_COMBO_SCHEME, true);
    ENABLE_WINDOW(IDOK, false);
    ENABLE_WINDOW(IDC_BTN_STOPSERVICE, true);

    // ��ʼ������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	shellExecute("open", m_stringTraceFile.c_str());
}


BOOL CSocialServerDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if (pMsg->wParam == VK_ESCAPE ||pMsg->wParam == VK_RETURN)
        return true;

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSocialServerDlg::OnCbnSetfocusComboScheme()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    if(gSocialGlobal == NULL)
    {
        return;
    }

    ISchemeEngine * pSchemeEngine = gSocialGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return;
    }

    // �����ǰ���б�
    m_ComboBoxScheme.SetCurSel(-1);
    m_ComboBoxScheme.ResetContent();

    // ѹ��
    list<string> * plistScheme = pSchemeEngine->GetDynamicSchemeFileName();
    list<string>::iterator it = plistScheme->begin();
    for( ; it != plistScheme->end(); ++it)
    {
        m_ComboBoxScheme.AddString(CString((*it).c_str()));
    }
}


void CSocialServerDlg::OnBnClickedBtnUpdatescheme()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_socialGlobal.Stop();

    BEGIN_WINDOW();
    ENABLE_WINDOW(IDOK, true);
    ENABLE_WINDOW(IDC_BTN_STOPSERVICE, false);
}


void CSocialServerDlg::OnBnClickedBtnOutputPerformacne()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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

    // TODO: �ڴ���ӿؼ�֪ͨ����������
    g_ServerControl.SetUIControl(UICONTROL_QUEUESTATE);	
}


void CSocialServerDlg::OnBnClickedButtonScanlua()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    char szOldCurrentDirector[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, szOldCurrentDirector);	
    CFileDialog FileDialog(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Lua Files (*.lua)|*.lua|All Files (*.*)|*.*||"), this);
    FileDialog.DoModal();
    m_editUpdateLuaDir = FileDialog.GetPathName();
    UpdateData(FALSE);
}


void CSocialServerDlg::OnBnClickedButtonLua()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    ForceUnlockDialog	dlg;
    dlg.DoModal();
}
