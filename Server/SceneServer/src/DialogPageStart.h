/*******************************************************************
** �ļ���:	DialogPageStart.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	2016-6-21
** ��  ��:	1.0
** ��  ��:	
            
********************************************************************/

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"


// CDialogPageStart �Ի���

class CDialogPageStart : public CDialog
{
	DECLARE_DYNAMIC(CDialogPageStart)

public:
	CDialogPageStart(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogPageStart();

// �Ի�������
	enum { IDD = IDD_DIALOG_PAGESTART };

public:
	/** ��ʼ���Ի���
	@param   
	@param   
	@return  
	*/
	bool			InitDialog(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    CComboBox       m_ComboBoxScheme;
    CRichEditCtrl   m_RichEditTrace;
    CProgressCtrl   m_ProgressKickout;
	string			m_stringTraceFile;

private:
    TraceListener * m_pTraceListener;
	int		        m_nTraceLevel;
    // �ϴε�ˢ�¶�����Ϣ��ʱ��
    DWORD			m_dwLastUpdateQueueState;

public:
	/** Ȩ������������
	@param   
	@param   
	@return  
	*/
	void			ControlStartServer(LPCSTR pszUserName, LPCSTR pszPassword);

    // ���迪��״̬
    void            ResetStartState(void);

    /** �ر�ʱ�����ý���
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