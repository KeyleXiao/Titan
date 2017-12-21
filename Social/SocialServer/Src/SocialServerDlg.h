
// SocialServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "Trace.h"
#include "SocialGlobal.h"
#include "afxwin.h"
#include "resource.h"

using namespace rkt;

// CSocialServerDlg �Ի���
class CSocialServerDlg : public CDialogEx
{
// ����
public:
	CSocialServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SOCIALSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
    // �ϴε�ˢ�¶�����Ϣ��ʱ��
    DWORD			    m_dwLastUpdateQueueState;

public:
    afx_msg void OnBnClickedButtonScanlua();
    afx_msg void OnBnClickedButtonLua();
    CString m_editUpdateLuaDir;
    afx_msg void OnBnClickedButtonForceunlock();
};

extern CSocialServerDlg * g_pServerDlg;
