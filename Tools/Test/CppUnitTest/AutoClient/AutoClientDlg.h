
// AutoClientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "Client.h"
#include <vector>
#include "MyRichEdit.h"
using std::vector;

// CAutoClientDlg 对话框
class CAutoClientDlg : public CDialogEx
{
// 构造
public:
	CAutoClientDlg(CWnd* pParent = NULL);	// 标准构造函数

	virtual ~CAutoClientDlg();

    int GetGameMode();

	void ClientLogin(int id);

	void ClientLogout(int id);

	CClient* GetAroundClient(CClient* pMyClient, float fDistance);

    CClient* GetClient(int id);

	void ReleaseClient(CClient* pClient);

protected:
    void Login();

// 对话框数据
	enum { IDD = IDD_AUTOCLIENT_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLoginMMO();
    afx_msg void OnLoginMOBA();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

public:
	DWORD				m_dwTotalCount;		// 总的登陆玩家数
	DWORD				m_dwLoginInterval; 	// 登陆间歇时间(ms)
	DWORD				m_dwCurLoginID;
	DWORD				m_dwLoginCount;		// 已登陆玩家数
	TraceListener*		m_pTraceListener;
	TimerAxis			m_timerAxis;
	TimeSyncClient      m_TimeSyncer;       // 时间同步器
	CIPAddressCtrl		m_ctrlIPAddr;

	vector<CClient*>	m_clients;			// 保存所有客户端
	CRichEditCtrl		m_RichEdit;
	int					m_nTraceLevel;

    int                 m_nGameMode;
};
