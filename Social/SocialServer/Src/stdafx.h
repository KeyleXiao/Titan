
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��


///////////////////////////////////////////////////////////////////////////////////////////
#define USE_SYSTEM_MEMORY

#include "Base.h"
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))
using namespace rkt;
#include "GameDef.h"

#include "ICenterServerProtocol.h"
#include "ICenterServerConnector.h"
using namespace CenterServer;


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
#define BEGIN_WINDOW()      CWnd  *pTempWnd = NULL;

#define ENABLE_WINDOW(itemid, flag)											\
    pTempWnd = GetDlgItem(itemid);											\
    if(pTempWnd != NULL)													\
{																		\
    pTempWnd->EnableWindow(flag);										\
}

#define SET_WINDOW_NAME(itemid, name)                                       \
    pTempWnd = GetDlgItem(itemid);                                          \
    if (pTempWnd != NULL)                                                   \
{                                                                       \
    pTempWnd->SetWindowText(name);                                      \
}                                                                       \


#define ENABLE_WINDOW_EX(dialog, itemid, flag)								\
    pTempWnd = dialog->GetDlgItem(itemid);									\
    if(pTempWnd != NULL)													\
{																		\
    pTempWnd->EnableWindow(flag);										\
}


#define VISIBLE_WINDOW(itemid, flag)										\
    pTempWnd = GetDlgItem(itemid);											\
    if(pTempWnd != NULL)													\
{																		\
    if(flag)															\
{																	\
    pTempWnd->ShowWindow(SW_SHOW);									\
}																	\
        else																\
{																	\
    pTempWnd->ShowWindow(SW_HIDE);									\
}																	\
}


#define SAFE_DELETEARRAY(data) if(data) { delete[] (data); data=NULL;};
