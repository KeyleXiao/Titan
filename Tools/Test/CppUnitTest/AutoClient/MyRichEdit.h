#pragma once


// CMyRichEdit

class CMyRichEdit : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CMyRichEdit)

public:
	CMyRichEdit();
	virtual ~CMyRichEdit();

	virtual void OnFinalRelease();

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};


