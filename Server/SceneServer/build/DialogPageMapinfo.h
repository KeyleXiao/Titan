#pragma once


// CDialogPageMapinfo �Ի���

class CDialogPageMapinfo : public CDialog
{
	DECLARE_DYNAMIC(CDialogPageMapinfo)

public:
	CDialogPageMapinfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogPageMapinfo();

// �Ի�������
	enum { IDD = IDD_DIALOG_PAGEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
