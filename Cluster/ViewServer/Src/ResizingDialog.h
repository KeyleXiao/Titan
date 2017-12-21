﻿// ResizingDialog.h : header file
//

#ifndef _RESIZING_DIALOG_H
#define  _RESIZING_DIALOG_H

#define ANCHORE_LEFT	0x0000
#define ANCHORE_TOP		0x0000
#define ANCHORE_RIGHT	0x0001
#define ANCHORE_BOTTOM	0x0002
#define	RESIZE_HOR		0x0004
#define	RESIZE_VER		0x0008
#define	RESIZE_BOTH		(RESIZE_HOR | RESIZE_VER)


/////////////////////////////////////////////////////////////////////////////
// CResizingDialog dialog

class CResizingDialog : public CDialog
{
// Construction
public:
	CResizingDialog( UINT nIDTemplate, CWnd* pParentWnd = NULL );
	void SetControlInfo(WORD CtrlId,WORD Anchore);
	BOOL GetRememberSize() { return  m_bRememberSize;}
	void SetRememberSize(BOOL bRemember) { m_bRememberSize = bRemember;}
	virtual void GetDialogProfileEntry(CString &sEntry);
// Dialog Data
protected:
	// Set m_bRememberSize to FALSE if you don't want the dialog box 
	// to remember its size between sessions.
	BOOL	m_bRememberSize;
	// Set m_bDrawGipper if you don't want a Resizing Gripper
	BOOL	m_bDrawGripper;

private:
	int		m_minWidth,m_minHeight;
	int		m_old_cx,m_old_cy;
	BOOL	m_bSizeChanged;
	CDWordArray	m_control_info;
	UINT	m_nIDTemplate;
	CRect	m_GripperRect;

protected:
	// Generated message map functions
	//{{AFX_MSG(CResizingDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif

