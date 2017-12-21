/*******************************************************************
** 文件名:	DialogPageStart.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-21
** 版  本:	1.0
** 描  述:	
            
********************************************************************/
//

#include "stdafx.h"
#include "SceneServer.h"
#include "SceneServerDlg.h"
#include "DialogPageStart.h"
#include "afxdialogex.h"
#include "CheckDialog.h"

#include "EntityHelper.h" 
#include "IAIService.h"
#include <sstream>

#ifdef OPEN_BVTTEST
#include "IPerfProf.h"
#include "mdl.h"
#endif

#ifndef globalObj
#define globalObj   g_pSceneServerDlg->m_global
#endif


CDialogPageStart *	g_pDialogPageStart = NULL;
// CDialogPageStart 对话框

IMPLEMENT_DYNAMIC(CDialogPageStart, CDialog)

CDialogPageStart::CDialogPageStart(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPageStart::IDD, pParent)
    , m_uidTarget(0)
    , m_editUpdateLuaDir(_T(""))
{
    m_pTraceListener = NULL;
    m_nTraceLevel = levelAll;

     // 上次点刷新队列信息的时间
    m_dwLastUpdateQueueState = 0;

    g_pDialogPageStart = this;
}

CDialogPageStart::~CDialogPageStart()
{
}

void CDialogPageStart::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_SCHEME, m_ComboBoxScheme);
    DDX_Control(pDX, IDC_RICHEDIT_TRACE, m_RichEditTrace);
    DDX_Control(pDX, IDC_PROGRESS_KICKOUT, m_ProgressKickout);
    DDX_Text(pDX, IDC_EDIT_UID, m_uidTarget);
    DDX_Text(pDX, IDC_EDIT_LUA, m_editUpdateLuaDir);
}


BEGIN_MESSAGE_MAP(CDialogPageStart, CDialog)
    ON_BN_CLICKED(IDC_BTN_SHOWTRACE, &CDialogPageStart::OnBnClickedBtnShowtrace)
    ON_BN_CLICKED(IDC_BTN_STARTSERVICE, &CDialogPageStart::OnBnClickedBtnStartservice)
    ON_BN_CLICKED(IDC_BTN_STOPSERVICE, &CDialogPageStart::OnBnClickedBtnStopservice)
    ON_WM_CONTEXTMENU()
    ON_BN_CLICKED(IDC_BTN_OUTPUT_PERFORMACNE, &CDialogPageStart::OnBnClickedBtnOutputPerformacne)
    ON_BN_CLICKED(IDC_BTN_AI, &CDialogPageStart::OnBnClickedBtnAi)
    ON_BN_CLICKED(IDC_BTN_UPDATESCHEME, &CDialogPageStart::OnBnClickedBtnUpdatescheme)
    ON_CBN_SETFOCUS(IDC_COMBO_SCHEME, &CDialogPageStart::OnCbnSetfocusComboScheme)
    ON_BN_CLICKED(IDC_BUTTON_RESETPRO_QUEUESTATE, &CDialogPageStart::OnBnClickedButtonResetproQueuestate)
    ON_BN_CLICKED(IDC_BUTTON_LUA, &CDialogPageStart::OnBnClickedButtonLua)
    ON_BN_CLICKED(IDC_BUTTON_SCANLUA, &CDialogPageStart::OnBnClickedButtonScanlua)
    ON_BN_CLICKED(IDC_BUTTON_MEMERYINFO, &CDialogPageStart::OnBnClickedButtonMemeryinfo)
END_MESSAGE_MAP()


// CDialogPageStart 消息处理程序


bool CDialogPageStart::InitDialog(void)
{
    // 创建trace
    HWND hwndTrace = GetDlgItem(IDC_RICHEDIT_TRACE)->GetSafeHwnd();
    m_pTraceListener = output2RichEdit((void *)hwndTrace, levelAll, true);
    if(m_pTraceListener == NULL)
    {
        return false;
    }

    // 使其一些按扭变灰
    BEGIN_WINDOW();
    ENABLE_WINDOW(IDC_COMBO_SCHEME, false);

    m_ProgressKickout.SetRange32(0, 100);

    return true;
}
//////////////////////////////////////////////////////////////////////////
/** 权限启动服务器
@param   
@param   
@return  
*/
void CDialogPageStart::ControlStartServer(LPCSTR pszUserName, LPCSTR pszPassword)
{
    // 设定账号信息
    g_SceneServerControl.m_stringUserName = pszUserName;
    g_SceneServerControl.m_stringPassword = pszPassword;

    EmphasisLn("UserName="<< pszUserName << ", Password="<< pszPassword);

    if(g_SceneServerControl.GetServerRunState() == SERVER_RUN_STATE_CHECK_ACCOUNT_FAIL)
    {
        g_SceneServerControl.SetUIControl(UICONTROL_CHECK_ACCOUNT);
    }
    else
    {
        // 启动FrameWork
        globalObj.Start(theApp.m_nDefaultSvrID );
    }

}


// 重设开启状态
void  CDialogPageStart::ResetStartState(void)
{
    // 校验不通过时只能重试5次
    static int s_retry_count = 0;
    if(s_retry_count >= 5)
        return;
    ++s_retry_count;

    BEGIN_WINDOW();
    ENABLE_WINDOW(IDC_COMBO_SCHEME, false);
    ENABLE_WINDOW(IDC_BTN_STARTSERVICE, true);
    ENABLE_WINDOW(IDC_BTN_STOPSERVICE, false);
}

/** 关闭时，禁用界面
@param   
@param   
@return  
*/
void CDialogPageStart::CloseDisabledUI(void)
{
    g_SceneServerControl.SetUIControl(UICONTROL_STOP_SERVICE);
}

//////////////////////////////////////////////////////////////////////////
void CDialogPageStart::OnBnClickedBtnShowtrace()
{
	shellExecute("open", m_stringTraceFile.c_str());
}


void CDialogPageStart::OnBnClickedBtnStartservice()
{
    wstringstream ss;
    ss << (_T("TITAN-场景服务器（"))<< theApp.m_nDefaultSvrID << _T("）");
#ifdef VERSEION_DEBUG
    ss << (_T("-- *开发版*"));
#endif    
    g_pSceneServerDlg->SetWindowText(ss.str().c_str());

    BEGIN_WINDOW();
    ENABLE_WINDOW(IDC_COMBO_SCHEME, true);
    ENABLE_WINDOW(IDC_BTN_STARTSERVICE, false);
    ENABLE_WINDOW(IDC_BTN_STOPSERVICE, true);
    
    ENABLE_WINDOW_EX(g_pDialogPageMapinfo, IDC_BUTTON_UPDATEMAPINFO, true);

    // 发布版
    if(theApp.m_strUserName.length() > 0)
    {
        ControlStartServer( theApp.m_strUserName.data() , theApp.m_strPassword.data() );
        // 清掉
        theApp.m_strUserName.clear();
        theApp.m_strPassword.clear();
    }
    else
    {
        INT_PTR nResponse = g_pCheckDialog->DoModal();
        if (nResponse != IDOK)
        {
            ResetStartState();
        }
    }
}


void CDialogPageStart::OnBnClickedBtnStopservice()
{
    // TODO: 在此添加控件通知处理程序代码
    DWORD dwServerState = g_SceneServerControl.GetServerRunState();
    if(dwServerState == SERVER_RUN_STATE_START_SUCCESS)
    {
        int nRet = MessageBox(_T("确认关闭场景服？"), _T("警告"), MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2);
        if(IDNO == nRet)
        {
            return;
        }

        nRet = MessageBox(_T("＝＝＝＝＝＝＝ 请再次确认是否关闭场景服？＝＝＝＝＝＝＝"), _T("警告"), MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2);
        if(IDNO == nRet)
        {
            return;
        }

        TraceLn(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        EmphasisLn( _GT("手动关闭服务... ...开启关闭场景服流程。"));
        TraceLn(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<< endl);

        CloseDisabledUI();

        BEGIN_WINDOW();
        ENABLE_WINDOW(IDC_BTN_STARTSERVICE, true);
        ENABLE_WINDOW(IDC_BTN_STOPSERVICE, false);
    }
    else
    {
        g_pSceneServerDlg->SendMessage(WM_CLOSE, 0, 0);
    }
}


void CDialogPageStart::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
    int nNewLevel = trackTraceContextMenu(m_nTraceLevel);
    if (nNewLevel != m_nTraceLevel)
    {
        rkt::setTraceLevel(nNewLevel);
        EmphasisLn("ContextMenu modify trace level, level="<<nNewLevel);
        m_nTraceLevel = nNewLevel;
    }
}

void CDialogPageStart::OnBnClickedBtnOutputPerformacne()
{
    // TODO: 在此添加控件通知处理程序代码
    IFramework* pFramework = globalObj.getFramework();
    if(pFramework == NULL)
    {
        return;
    }
    pFramework->dump_service_runtime_info();
}


void CDialogPageStart::OnBnClickedBtnAi()
{
    UpdateData(true);

    static bool s_print_ai = true;

    AIServiceHelper helper(UID_2_SCENE(m_uidTarget));
    if(!helper.m_ptr)
    {
        return;
    }
    helper.m_ptr->traceAIInfo(m_uidTarget, s_print_ai);

    BEGIN_WINDOW();
    SET_WINDOW_NAME(IDC_BTN_AI, (s_print_ai ? CString("取消") : CString("确定")) );
    UpdateData(FALSE);

    s_print_ai = !s_print_ai;
}


void CDialogPageStart::OnBnClickedBtnUpdatescheme()
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

    g_SceneServerControl.m_stringWaitDynamic.clear();

#ifdef RKT_UTF8
    g_SceneServerControl.m_stringWaitDynamic = CString2String(rString);
#else
    g_SceneServerControl.m_stringWaitDynamic = rString;
#endif    
    g_SceneServerControl.SetUIControl(UICONTROL_UPDATE_SCHEME);
}


void CDialogPageStart::OnCbnSetfocusComboScheme()
{
    // TODO: 在此添加控件通知处理程序代码
    if(gServerGlobal == NULL)
    {
        return;
    }

    ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
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


void CDialogPageStart::OnBnClickedButtonResetproQueuestate()
{
    if(GetTickCount() - m_dwLastUpdateQueueState <= 5000)
    {
        return;
    }
    m_dwLastUpdateQueueState = GetTickCount();

    // TODO: 在此添加控件通知处理程序代码
    g_SceneServerControl.SetUIControl(UICONTROL_QUEUESTATE);	
}


void CDialogPageStart::OnBnClickedButtonLua()
{
    // TODO: 在此添加控件通知处理程序代码
    if(m_editUpdateLuaDir.IsEmpty())
    {
        return;
    }

    g_SceneServerControl.m_stringWaitDynamic.clear();

#ifdef RKT_UTF8
    g_SceneServerControl.m_stringWaitDynamic = CString2String(m_editUpdateLuaDir);
#else
    g_SceneServerControl.m_stringWaitDynamic = rString;
#endif  

    g_SceneServerControl.SetUIControl(UICONTROL_UPDATE_LUA);
}


void CDialogPageStart::OnBnClickedButtonScanlua()
{
    // TODO: 在此添加控件通知处理程序代码
    char szOldCurrentDirector[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, szOldCurrentDirector);	
    CFileDialog FileDialog(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Lua Files (*.lua)|*.lua|All Files (*.*)|*.*||"), this);
    FileDialog.DoModal();
    m_editUpdateLuaDir = FileDialog.GetPathName();
    UpdateData(FALSE);
}


void CDialogPageStart::OnBnClickedButtonMemeryinfo()
{
#ifdef OPEN_BVTTEST
    // 统计内存数据
    mdl_stat();
    WarningLn(__FUNCTION__": end memory stat!");
#endif

    g_SceneServerControl.SetUIControl(UICONTROL_MEMORYINFO);
}
