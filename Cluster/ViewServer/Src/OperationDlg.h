#pragma once
#include "afxwin.h"
#include "resource.h"       // 主符号

// COperationDlg 对话框

class COperationDlg : public CDialog
{
	DECLARE_DYNAMIC(COperationDlg)

public:
	COperationDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COperationDlg();

// 对话框数据
	enum { IDD = IDD_OPERATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	afx_msg void OnSelchangeFlagListCombo();

public:

	virtual BOOL OnInitDialog();

	// 操作备注最少长度
	int m_nMinTxtLens;
	// 操作备注最多长度
	int m_nMaxTxtLens;
	// 类型列表所选序号
	int m_nListSelect;
	// 操作备注的名称
	CString m_strTxtName;
	// 对话框标题
	CString m_strDlgTitle;
	// 是否要输入验证码
	bool m_bEnterVerifyCode;
	// 验证码数字
	DWORD m_dwVerifyCode;
	// 使用显示备用数字1
	bool m_bUseNumber1;
	// 使用显示备用数字2
	bool m_bUseNumber2;
	// 使用显示备用数字3
	bool m_bUseNumber3;
	// 备用数字1的标题文字
	CString m_strNumberTitle1;
	// 备用数字2的标题文字
	CString m_strNumberTitle2;

	// 提示标题信息
	CString m_strMsgTitle;
	// 类型列表
	CComboBox m_ctrlFlagList;
	// 操作延迟秒数
	DWORD m_dwDelays;
	// 操作备注标题
	CString m_strTxtLabel;
	// 操作备注
	CString m_strTxt;
	CEdit m_editDelays;
	CStatic m_staticDelays;
	CEdit m_editTxt;

	// 图标ID
	UINT m_InfoIcon;
	// 图标
	HICON m_hIcon;

	// 备用数字1
	int m_nNumber1;
	// 备用数字2
	int m_nNumber2;

	// 所选择的所有游戏世界ID列表
	std::map<DWORD,DWORD> m_mapSelectWorldList;
};
