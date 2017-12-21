#pragma once


// CCheckDialog 对话框

class CCheckDialog : public CDialog
{
	DECLARE_DYNAMIC(CCheckDialog)

public:
	CCheckDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedControlStart();

public:
    CString m_strUserName;
    CString m_strPassword;
};

extern CCheckDialog * g_pCheckDialog;