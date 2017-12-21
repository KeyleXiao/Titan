/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\VoiceGatewayDlg.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音网关主程序界面
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "TrayIcon.h"
#include "GlobalVoiceGateway.h"
#include "ResizingDialog.h"
#include "afxwin.h"

// VoiceGatewayDlg.h : 头文件

// 树形图标ID列表
enum EMTreeIconIndex
{
	TREE_ICON_ROOT = 0,		// 根图标
	TREE_ICON_SERVER,		// 服务器图标
	TREE_ICON_SERVER0,		// 服务器图标0
	TREE_ICON_USER,			// 用户图标
	TREE_ICON_USER0,		// 用户图标0
	TREE_ICON_COMPUTER,		// 硬件服图标
	TREE_ICON_COMPUTER0,	// 硬件服图标0

	// 最大
	TREE_ICON_MAXID
};


#define WM_MY_TRAY_NOTIFICATION			WM_USER+0				//任务栏图标message 
#define SERVICE_TRAYICON_TIP		_T("语音网关")			//任务栏图标Tip

// CVoiceGatewayDlg 对话框
class CVoiceGatewayDlg : public CResizingDialog
{
// 构造
public:
	CVoiceGatewayDlg(CWnd* pParent = NULL);	// 标准构造函数

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

	//更新修改的中心服务器ＩＰ,端口到m_VoiceGateway.m_strCenterServerIP,m_VoiceGateway.m_nPort
	void UpdateCenterServerConfig();

	//更新界面中心服务器ＩＰ,端口
	void ShowCenterServerConfig();

	// 属性
	CImageList m_TreeImgList;
	CStatic m_ctrlAppIcon;

	CButton m_StartBt;				// 启动服务
	CButton m_StopBt;				// 停止服务

	GlobalVoiceGateway m_VoiceGateway;	//语音网关服务
	TraceListener*			mTrace;

	BOOL m_bStart;				//服务已启动
	DWORD nTimeCount;			// 计时计数
	int nClientCount;			// 计数
	bool m_bOnTimerUpdate;		// 是否定时更新
	DWORD m_dwServerID;			// 本服务器序号


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
	// 客户端数
	CString m_strClients;
	// 服务器监听端口
	CString m_strServerPort;
	// 客户端监听端口
	CString m_strClientPort;
	// 语音收包带宽
	CString m_strRecvSpeedServer;
	// 语音发包带宽
	CString m_strSendSpeedServer;

	// 语音收包带宽
	CString m_strRecvSpeedClient;
	// 语音发包带宽
	CString m_strSendSpeedClient;

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
	afx_msg void OnMgDlg();
	afx_msg void OnTracelevel();
};
