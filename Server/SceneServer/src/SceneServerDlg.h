
// SceneServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "Trace.h"
#include "ServerGlobal.h"

#include "DialogPageStart.h"
#include "DialogPageSetting.h"
#include "DialogPageMapinfo.h"
#include "CheckDialog.h"



// CSceneServerDlg 对话框
class CSceneServerDlg : public CDialogEx
{
// 构造
public:
	CSceneServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SCENESERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	// 调试信息监听器
	TraceListener  *   m_pTraceListener;


// 实现
protected:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTcnSelchangeTabFrame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    // WM_CTR_CLOSEDLG
    afx_msg LRESULT OnCtrCloseDlg(WPARAM wParam,LPARAM lParam);
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    CTabCtrl m_TabFrame;

public:
	ServerGlobal       m_global;

protected:
	HICON m_hIcon;
    CDialogPageStart	m_PageDialogStart;
    CDialogPageSetting	m_PageDialogSetting;
    CDialogPageMapinfo	m_DialogPageMapinfo;
    CCheckDialog        m_DialogCheck;
};

extern CSceneServerDlg * g_pSceneServerDlg;