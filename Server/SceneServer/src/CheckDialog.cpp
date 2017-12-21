// CheckDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SceneServer.h"
#include "DialogPageStart.h"
#include "CheckDialog.h"
#include "afxdialogex.h"



CCheckDialog * g_pCheckDialog = NULL;

// CCheckDialog �Ի���

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


// CCheckDialog ��Ϣ�������

void CCheckDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
    if(SC_CLOSE == nID)
    {
        return;
    }

    CDialog::OnSysCommand(nID, lParam);
}

// Ȩ������
void CCheckDialog::OnBnClickedControlStart()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData(TRUE);

    CString strUserName = m_strUserName;
    CString strPassword = m_strPassword;

    // ����������룬�Ծ��󻼣���ñ�����ڴ�
    m_strUserName.Empty();
    m_strPassword.Empty();

    g_pDialogPageStart->ControlStartServer(CString2String(strUserName).data(), CString2String(strPassword).data());

    // ���½���
    UpdateData(FALSE);

    EndDialog(IDOK);

    SendMessage(WM_CLOSE, 0, 0);
}
