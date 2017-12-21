/*******************************************************************
** 文件名:	Splash.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2008/11/8 12:12
** 版  本:	1.0
** 描  述:	启动Logo处理类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

//
// Splash.cpp : implementation file
//

#include <atlbase.h>
#include <afxwin.h>
#include <afxpriv2.h>

#include "stdafx.h"  // e. g. stdafx.h
#include "Splash.h"  // e.g. splash.h

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   CSplashWnd class

////////////////////////////////////////////////////////////////////////////
//constructor
//Load image from the given file
//
CSplashWnd::CSplashWnd(LPCTSTR lpszFileName)
{
	fileIsValid = pic.Load(lpszFileName);
	if(fileIsValid)
	{
		CSize cz = pic.GetImageSize(NULL);
		width = cz.cx;
		height = cz.cy;
	}
	m_bStart=TRUE;
}

CSplashWnd::CSplashWnd(UINT nIDRes)
{
	fileIsValid = pic.Load(nIDRes);
	if(fileIsValid)
	{
		CSize cz = pic.GetImageSize(NULL);
		width = cz.cx;
		height = cz.cy;
	}
	m_bStart=TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//nothing to do
//deconstructor
//
CSplashWnd::~CSplashWnd()
{
}

////////////////////////////////////////////////////////////////////////////////
//message map
//
BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()

END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////////////
//ShowSplash
//to display the given image on screen
//
BOOL CSplashWnd::ShowSplash()
{
	
	if(fileIsValid)
	{
		if (!Create(AfxGetMainWnd()))
			return false;
		else
		{
			UpdateWindow();
			return true;
		}
	}
	else
	{
		return false;
	}
}


////////////////////////////////////////////////////////////////////////////////
//PreTranslateAppMessage
//
BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		CloseSplash();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}


////////////////////////////////////////////////////////////////////////////////
//Create
//make a popup splash window
//
BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{

	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, width, height, pParentWnd->GetSafeHwnd(), NULL);
}


////////////////////////////////////////////////////////////////////////////////
//CloseSplash
//Quit the splash window
//
void CSplashWnd::CloseSplash()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
}


////////////////////////////////////////////////////////////////////////////////
//do nothing
//
void CSplashWnd::PostNcDestroy()
{

}

////////////////////////////////////////////////////////////////////////////////
//OnCreate
//put the splash window on center
//
int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//OnPaint
//Display the given image
//
void CSplashWnd::OnPaint()
{
	if(fileIsValid)
	{
		CPaintDC dc(this);
		CRect rc(0,0,0,0);;
		pic.Render(&dc, rc);
	}
}


////////////////////////////////////////////////////////////////////////////////
//ShowText
//sometimes if we show what we are doing (I display the information on the center of //the picture ), the customer will be more patient
//
//
void CSplashWnd::ShowText(LPCTSTR lpStr)
{
	if(fileIsValid)
	{
		Invalidate();
		CPaintDC dc(this);
		dc.SetBkMode(TRANSPARENT);
		SIZE sz;
		sz = (SIZE)dc.GetTextExtent(lpStr,lstrlen(lpStr));
		dc.TextOut((width-sz.cx)/2,height/2,lpStr);
	}
}

////////////////////////////////////////////////////////////////
// MSDN Magazine - October 2001
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0 for Windows 98 and probably Windows 2000 
// too.
// Set tabsize = 3 in your editor.
//

////////////////////////////////////////////////////////////////
// CPicture implementation
//

CPicture::CPicture()
{
}

CPicture::~CPicture()
{
}

//////////////////
// Load from resource. Looks for "IMAGE" type.
//
BOOL CPicture::Load(UINT nIDRes)
{
   // find resource in resource file
   HINSTANCE hInst = AfxGetResourceHandle();
   HRSRC hRsrc = ::FindResource(hInst,
      MAKEINTRESOURCE(nIDRes),
      _T("IMAGE")); // type
   if (!hRsrc)
      return FALSE;

   // load resource into memory
   DWORD len = SizeofResource(hInst, hRsrc);
   BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
   if (!lpRsrc)
      return FALSE;

   // create memory file and load it
   CMemFile file(lpRsrc, len);
   BOOL bRet = Load(file);
   FreeResource(hRsrc);
   return bRet;
}

//////////////////
// Load from path name.
//
BOOL CPicture::Load(LPCTSTR pszPathName)
{
   CFile file;
   if (!file.Open(pszPathName, CFile::modeRead|CFile::shareDenyWrite))
      return FALSE;
   BOOL bRet = Load(file);
   file.Close();
   return bRet;
}

//////////////////
// Load from CFile
//
BOOL CPicture::Load(CFile& file)
{
   CArchive ar(&file, CArchive::load | CArchive::bNoFlushOnDelete);
   return Load(ar);
}

//////////////////
// Load from archive-create stream and load from stream.
//
BOOL CPicture::Load(CArchive& ar)
{
   CArchiveStream arcstream(&ar);
   return Load((IStream*)&arcstream);
}

//////////////////
// Load from stream (IStream). This is the one that really does it: call
// OleLoadPicture to do the work.
//
BOOL CPicture::Load(IStream* pstm)
{
   Free();
   HRESULT hr = OleLoadPicture(pstm, 0, FALSE,
      IID_IPicture, (void**)&m_spIPicture);
   ASSERT(SUCCEEDED(hr) && m_spIPicture); 
   return TRUE;
}


//////////////////
// Get image size in pixels. Converts from HIMETRIC to device coords.
//
CSize CPicture::GetImageSize(CDC* pDC) const
{
   if (!m_spIPicture)
      return CSize(0,0);
   
   LONG hmWidth, hmHeight; // HIMETRIC units
   m_spIPicture->get_Width(&hmWidth);
   m_spIPicture->get_Height(&hmHeight);
   CSize sz(hmWidth,hmHeight);
   if (pDC==NULL) {
      CWindowDC dc(NULL);
      dc.HIMETRICtoDP(&sz); // convert to pixels
   } else {
      pDC->HIMETRICtoDP(&sz);
   }
   return sz;
}

//////////////////
// Render to device context. Covert to HIMETRIC for IPicture.
//
BOOL CPicture::Render(CDC* pDC, CRect rc, LPCRECT prcMFBounds) const
{
   ASSERT(pDC);

   if (rc.IsRectNull()) {
      CSize sz = GetImageSize(pDC);
      rc.right = sz.cx;
      rc.bottom = sz.cy;
   }
   long hmWidth,hmHeight; // HIMETRIC units
   GetHIMETRICSize(hmWidth, hmHeight);
   m_spIPicture->Render(*pDC, rc.left, rc.top, rc.Width(), rc.Height(),
      0, hmHeight, hmWidth, -hmHeight, prcMFBounds);

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//  message handlers

void CSplashWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bStart==FALSE)
		CloseSplash();
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSplashWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bStart==FALSE)
		CloseSplash();
	
	CWnd::OnLButtonDown(nFlags, point);
}

