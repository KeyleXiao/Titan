#pragma once


// CCheckDialog �Ի���

class CCheckDialog : public CDialog
{
	DECLARE_DYNAMIC(CCheckDialog)

public:
	CCheckDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCheckDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_CHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedControlStart();

public:
    CString m_strUserName;
    CString m_strPassword;
};

extern CCheckDialog * g_pCheckDialog;