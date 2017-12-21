// CheckDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneServer.h"
#include "DialogPageStart.h"
#include "CheckDialog.h"
#include "afxdialogex.h"



CCheckDialog * g_pCheckDialog = NULL;

// CCheckDialog 对话框

IMPLEMENT_DYNAMIC(CCheckDialog, CDialog)

CCheckDialog::CCheckDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckDialog::IDD, pParent)
    , m_strUserName(_T(""))
    , m_strPassword(_T(""))
{
    g_pCheckDialog = this;
}

CCheckDialog::~CCheckDialog()
{
}

void CCheckDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_strUserName);
    DDX_Text(pDX, IDC_EDIT2, m_strPassword);
}


BEGIN_MESSAGE_MAP(CCheckDialog, CDialog)
    ON_BN_CLICKED(ID_CONTROL_START, &CCheckDialog::OnBnClickedControlStart)
END_MESSAGE_MAP()


// CCheckDialog 消息处理程序

void CCheckDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
    if(SC_CLOSE == nID)
    {
        return;
    }

    CDialog::OnSysCommand(nID, lParam);
}

// 权限启动
void CCheckDialog::OnBnClickedControlStart()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);

    CString strUserName = m_strUserName;
    CString strPassword = m_strPassword;

    // 立即清掉密码，以绝后患，免得被监控内存
    m_strUserName.Empty();
    m_strPassword.Empty();

    g_pDialogPageStart->ControlStartServer(CString2String(strUserName).data(), CString2String(strPassword).data());

    // 更新界面
    UpdateData(FALSE);

    EndDialog(IDOK);

    SendMessage(WM_CLOSE, 0, 0);
}
