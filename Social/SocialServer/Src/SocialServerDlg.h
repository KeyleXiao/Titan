
// SocialServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "Trace.h"
#include "SocialGlobal.h"
#include "afxwin.h"
#include "resource.h"

using namespace rkt;

// CSocialServerDlg 对话框
class CSocialServerDlg : public CDialogEx
{
// 构造
public:
	CSocialServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SOCIALSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedStartservice();
	afx_msg void OnBnClickedButtonShowtrace();
    afx_msg void OnBnClickedBtnUpdatescheme();
    afx_msg void OnBnClickedStopservice();
    afx_msg void OnCbnSetfocusComboScheme();
    afx_msg void OnBnClickedBtnOutputPerformacne();
    afx_msg void OnBnClickedButtonResetproQueuestate();

public:
    CRichEditCtrl       m_richEditOutput;

    TraceListener*		m_pTraceListener;
    int                 m_nTraceLevel;
	string              m_stringTraceFile;

    SocialGlobal        m_socialGlobal;
    CComboBox m_ComboBoxScheme;

private:
    // 上次点刷新队列信息的时间
    DWORD			    m_dwLastUpdateQueueState;

public:
    afx_msg void OnBnClickedButtonScanlua();
    afx_msg void OnBnClickedButtonLua();
    CString m_editUpdateLuaDir;
    afx_msg void OnBnClickedButtonForceunlock();
};

extern CSocialServerDlg * g_pServerDlg;
