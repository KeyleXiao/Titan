#pragma once


// ForceUnlockDialog 对话框

class ForceUnlockDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ForceUnlockDialog)

public:
	ForceUnlockDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ForceUnlockDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_FORCEUNLOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedForce();
    afx_msg void OnBnClickedCancelForce();

private:
    void	BuildCheckCode(void);
public:
    int m_nCheckCode;
    virtual BOOL OnInitDialog();
};
