
// TimeSyncDlg.h : 头文件
//

#pragma once

#include "NetworkHandler.h"
#include "TimeSyncServer.h"
#include "TimeSyncClient.h"
#include "afxcmn.h"

class SManagerMsgHead;

#include "IManagerConnector.h"
using namespace Manager;

// CTimeSyncDlg 对话框
class CTimeSyncDlg : public CDialogEx, public IManagerEventHandler, public IManagerMessageHandler
{
// 构造
public:
	CTimeSyncDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TIMESYNC_DIALOG };

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
    afx_msg void OnBnClickedEnableRefresh();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedStart();
    afx_msg void OnBnClickedDoSync();
	afx_msg void OnBtnClickedOpenTrace();



    virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 管理连接器
public:
	/// 启动管理连接器
	void CreateManagerConnector(void);
	/// 关闭管理连接器
	void ReleaseManagerConnector(void);

	////////// IManagerEventHandler 处理子服务器连接器执行事件 ///////////////////////////////
	/** 处理子服务器连接器执行事件
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@retval buffer
	*/
	virtual void OnManagerEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen);

	////////// IManagerMessageHandler 处理子服务器发过来的消息 //////////////////
	/**
	@name         : 处理其他服务器通过子服务器转发的消息
	@brief        : 具体内容不清楚
	@param actionId  : 消息码
	@param head  : 消息头
	@param data  : 消息内容
	@param len:消息长度
	*/
	virtual void HandleManagerMessage(ulong actionId, SManagerMsgHead* head, LPSTR data, size_t len);
public:
	DWORD m_nLatency;
	DWORD m_nCurTick;
	DWORD m_nUDPPort;       // 服务器监听UDP端口
    DWORD m_nTCPPort;       // 服务器监听TCP端口

    CString m_strServerIP;
    DWORD m_nServerPort;
	BOOL m_bEnableRefresh;

    int                 m_nTraceLevel;

private:
	TimerAxis 	      m_TimerAxis;
	CNetworkHandler   m_NetworkHandler;

    TraceListener*		m_pTraceListener;

	TimeSyncServer    m_TimeSyncServer;
	TimeSyncClient    m_TimeSyncClient;

	IManagerConnector *	        m_ManagerConnector;         // 服务器管理连接器

	string                        m_szTraceFileName;
	string                        m_szCrashFileName;
	string                        m_szDumpFileName;

	// 取得本地路径
	char                        m_szWorkDir[MAX_PATH];
public:
    CRichEditCtrl m_richEditOutput;
};
