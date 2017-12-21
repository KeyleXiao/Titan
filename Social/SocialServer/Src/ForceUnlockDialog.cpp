// ForceUnlockDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SocialServer.h"
#include "ForceUnlockDialog.h"
#include "afxdialogex.h"
#include "ServerControl.h"

// ForceUnlockDialog �Ի���

IMPLEMENT_DYNAMIC(ForceUnlockDialog, CDialogEx)

ForceUnlockDialog::ForceUnlockDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(ForceUnlockDialog::IDD, pParent)
    , m_nCheckCode(0)
{

}

ForceUnlockDialog::~ForceUnlockDialog()
{
}

void ForceUnlockDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SYSTEMCHECK, m_nCheckCode);
}


BEGIN_MESSAGE_MAP(ForceUnlockDialog, CDialogEx)
    ON_BN_CLICKED(IDOK_FORCE, &ForceUnlockDialog::OnBnClickedForce)
    ON_BN_CLICKED(IDCANCEL_FORCE, &ForceUnlockDialog::OnBnClickedCancelForce)
END_MESSAGE_MAP()



BOOL ForceUnlockDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    CenterWindow(GetParent());

    BuildCheckCode();
    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

// ForceUnlockDialog ��Ϣ�������
void ForceUnlockDialog::BuildCheckCode(void)
{
    m_nCheckCode = rkt::rkt_range_rand(100000, 9999999);
    CString strCode; 
    strCode.Format(_T("%d"), m_nCheckCode);

    CWnd * pWnd = GetDlgItem(IDC_EDIT_SYSTEMCHECK);
    if(pWnd)
    {
        pWnd->SetWindowText(strCode);
    }

}


void ForceUnlockDialog::OnBnClickedForce()
{
    CString stringOprator; CString stringInput;
    GetDlgItem(IDC_EDIT_OPERATOR)->GetWindowText(stringOprator);
    GetDlgItem(IDC_EDIT_INPUTCHECK)->GetWindowText(stringInput);
    if(stringOprator.GetLength() < 4 || stringInput.GetLength() < 6)
    {
        BuildCheckCode();
        return;
    }

    int nInputValue = _ttoi(stringInput);
    if(nInputValue != m_nCheckCode)
    {
        BuildCheckCode();
        return;
    }

#ifdef RKT_UTF8
    // Convert a TCHAR string to a LPCSTR
    CT2CA pszConvertedAnsiString (stringOprator);
    // construct a std::string using the LPCSTR input
    std::string strStd (pszConvertedAnsiString);
    g_ServerControl.m_stringUnlockOperator = strStd;
#else
    g_ServerControl.m_stringUnlockOperator = stringOprator;
#endif

    g_ServerControl.SetUIControl(UICONTROL_FORCEUNLOCK);

    EndDialog(IDOK);
}

void ForceUnlockDialog::OnBnClickedCancelForce()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    EndDialog(IDCANCEL);
}
