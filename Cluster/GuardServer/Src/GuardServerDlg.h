/*******************************************************************
** 文件名:	GuardServerDlg.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器主程序界面
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "TrayIcon.h"
#include "GlobalGuardServer.h"
#include "ResizingDialog.h"
#include "afxwin.h"


#define WM_MY_TRAY_NOTIFICATION			WM_USER+0					//任务栏图标message 
#define SERVICE_TRAYICON_TIP			_T("数据监护服务器")		//任务栏图标Tip


// CGuardServerDlg 对话框
class CGuardServerDlg : public CResizingDialog
{
// 构造
public:
	CGuardServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVICE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	CTrayIcon	m_trayIcon;		// my tray icon
	BOOL		m_bShutdown;	// OK to terminate app

// 实现
protected:
	HICON m_hIcon;
	HICON m_hGrayIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:

// 方法

	// 更新日志到输出框
	void UpdateTraceInfo2RichEdit();

	// 改变tray 图标,bIsRuning=是否服务在运行
	void SetTrayIcon(BOOL bIsRuning);

	// 更新菜单状态
	void UpdateMenuState();

	// 刷新统计信息
	void UpdateTotalInfo();

	//停止服务
	bool StopService();

	//更新修改的中心服务器ＩＰ,端口到m_GuardServer.m_strCenterServerIP,m_GuardServer.m_nPort
	void UpdateCenterServerConfig();

	//更新界面中心服务器ＩＰ,端口
	void ShowCenterServerConfig();

	// 属性
	CImageList				m_TreeImgList;
	CStatic					m_ctrlAppIcon;

	CButton					m_StartBt;			// 启动服务
	CButton					m_StopBt;			// 停止服务

	GlobalGuardServer		m_GuardServer;		//数据监护服务器服务
	TraceListener*			mTrace;

	BOOL					m_bStart;			//服务已启动
	DWORD					nTimeCount;			// 计时计数
	int						nClientCount;		// 计数
	bool					m_bOnTimerUpdate;	// 是否定时更新


	// 退出
	CButton m_ExitBt;
	// 游戏世界
	CString m_strWorld;
	// 是否临时游戏世界
	CString m_strIsTemp;
	//所属游戏
	CString m_strGame;
	//游戏区域
	CString m_strArea;
	// 软件服务器数
	CString m_strServers;
	// 最大软件服ID
	CString m_strMaxServer;
	// 服务器监听端口
	CString m_strServerPort;

	// 启动时间
	CString m_strStartTime;

public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	// 起动结果message 
	afx_msg LRESULT OnMsgStartResult(WPARAM wParam, LPARAM lParam);
	// 关闭结果message 
	afx_msg LRESULT OnMsgStopResult(WPARAM wParam, LPARAM lParam);
	// 关闭程序message 
	afx_msg LRESULT OnMsgCloseApp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnAppOpen();
	afx_msg void OnAppAbout();
	afx_msg void OnAppSuspend();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnOutTotal();
	afx_msg void OnShowListinfo();
};
