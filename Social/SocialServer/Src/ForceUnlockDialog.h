#pragma once


// ForceUnlockDialog �Ի���

class ForceUnlockDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ForceUnlockDialog)

public:
	ForceUnlockDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ForceUnlockDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_FORCEUNLOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
