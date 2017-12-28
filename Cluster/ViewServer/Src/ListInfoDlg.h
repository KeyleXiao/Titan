#pragma once

#include "resource.h"       // 主符号
#include "ResizingDialog.h"

// CListInfoDlg 对话框

class CListInfoDlg : public CResizingDialog
{
	DECLARE_DYNAMIC(CListInfoDlg)

public:
	CListInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CListInfoDlg();

// 对话框数据
	enum { IDD = IDD_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	// 初始化列表头
	void InitColumns(int nListType=0);
	// 服务器列表
	CListCtrl m_InfoList;

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedServerButton();
	afx_msg void OnBnClickedButtonWorld();
};
