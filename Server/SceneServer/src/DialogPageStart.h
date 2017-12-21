/*******************************************************************
** 文件名:	DialogPageStart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-21
** 版  本:	1.0
** 描  述:	
            
********************************************************************/

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"


// CDialogPageStart 对话框

class CDialogPageStart : public CDialog
{
	DECLARE_DYNAMIC(CDialogPageStart)

public:
	CDialogPageStart(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogPageStart();

// 对话框数据
	enum { IDD = IDD_DIALOG_PAGESTART };

public:
	/** 初始化对话框
	@param   
	@param   
	@return  
	*/
	bool			InitDialog(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    CComboBox       m_ComboBoxScheme;
    CRichEditCtrl   m_RichEditTrace;
    CProgressCtrl   m_ProgressKickout;
	string			m_stringTraceFile;

private:
    TraceListener * m_pTraceListener;
	int		        m_nTraceLevel;
    // 上次点刷新队列信息的时间
    DWORD			m_dwLastUpdateQueueState;

public:
	/** 权限启动服务器
	@param   
	@param   
	@return  
	*/
	void			ControlStartServer(LPCSTR pszUserName, LPCSTR pszPassword);

    // 重设开启状态
    void            ResetStartState(void);

    /** 关闭时，禁用界面
    @param   
    @param   
    @return  
    */
    void            CloseDisabledUI(void);

public:
    afx_msg void OnBnClickedBtnShowtrace();
    afx_msg void OnBnClickedBtnStartservice();
    afx_msg void OnBnClickedBtnStopservice();
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    afx_msg void OnBnClickedBtnOutputPerformacne();
    afx_msg void OnBnClickedBtnAi();
    int m_uidTarget;
    afx_msg void OnBnClickedBtnUpdatescheme();
    afx_msg void OnCbnSetfocusComboScheme();
    afx_msg void OnBnClickedButtonResetproQueuestate();
    afx_msg void OnBnClickedButtonLua();
    afx_msg void OnBnClickedButtonScanlua();
    CString m_editUpdateLuaDir;
    afx_msg void OnBnClickedButtonMemeryinfo();
};

extern CDialogPageStart *	g_pDialogPageStart;