#pragma once


// CDialogPageMapinfo 对话框

class CDialogPageMapinfo : public CDialog
{
	DECLARE_DYNAMIC(CDialogPageMapinfo)

public:
	CDialogPageMapinfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogPageMapinfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_PAGEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
