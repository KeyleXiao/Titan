// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将它更改为适合 Windows Me 或更高版本的相应值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include "Base.h"
#include "Thread.h"
#include "buffer.h"
#include "Common.h"
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))
#define new RKT_NEW
using namespace rkt;

#include "ViewDef.h"
#include "ViewDef_.h"
#include "IGlobalViewMng.h"
#include "DbgHelp.h"
#include "MsgFactory.h"


#define WM_MY_START_RESULT				WM_USER+1				//起动结果message 
#define WM_MY_STOP_RESULT				WM_USER+2				//关闭结果message 
#define WM_MY_CLOSE_APP					WM_USER+3				//关闭程序message 


// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
#define __VOICE_GET_ID_BUFFER_START( V_MAX_COUNT )	\
DWORD dwUserCount = (V_MAX_COUNT);					\
DWORD szDataBuf[VOICE_MAX_CLIENT_COUNT];			\
DWORD * pClientIDArray = szDataBuf;					\
bool bIsNewMemory = false;							\
DWORD dwBuffSize = VOICE_MAX_CLIENT_COUNT;			\
if ( dwUserCount >= VOICE_MAX_CLIENT_COUNT )		\
{													\
	bIsNewMemory = true;							\
	dwBuffSize = dwUserCount;						\
	pClientIDArray = new DWORD[dwUserCount+8];		\
}

// ID缓存分配结束
#define __VOICE_GET_ID_BUFFER_END					\
if (bIsNewMemory)									\
{													\
	delete [](pClientIDArray);						\
	pClientIDArray=NULL;							\
}

inline void ListCtrl_InsertColumn(CListCtrl& ListCtrl, int nCol, LVCOLUMNA* lvc)
{
    ListCtrl.InsertColumn(nCol, my_utf82t(lvc->pszText), lvc->fmt, lvc->cx, lvc->iSubItem);
}

inline int ListCtrl_InsertItem(CListCtrl& ListCtrl, LVITEMA* lvi)
{
    return ListCtrl.InsertItem(lvi->mask, lvi->iItem, my_utf82t(lvi->pszText), lvi->state, lvi->stateMask, lvi->iImage, 0);
}

inline BOOL ListCtrl_SetItemText(CListCtrl& ListCtrl, int nItem, int nSubItem, const char* lpszText)
{
    return ListCtrl.SetItemText(nItem, nSubItem, my_utf82t(lpszText));
}

inline BOOL TreeCtrl_SetItemText(CTreeCtrl& TreeCtrl, HTREEITEM hItem, const char* lpszItem)
{
    return TreeCtrl.SetItemText(hItem, my_utf82t(lpszItem));
}