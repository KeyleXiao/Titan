/*******************************************************************
** 文件名:	GuardServer.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器主程序
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once


// GuardServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "GuardServer.h"
#include "GuardServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define one  _T("{A1E7801E-E2CC-455B-AD78-647BAC8C9543}")		//唯一标识字串，用来创建互斥
HANDLE g_applicationHandle;										//互斥句柄

// CGuardServerApp

BEGIN_MESSAGE_MAP(CGuardServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGuardServerApp 构造

CGuardServerApp::CGuardServerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CGuardServerApp 对象

CGuardServerApp theApp;


void myPurecallHandler(void)
{
	//Error("虚函数调用异常！！！");
	__debugbreak();
}

// CGuardServerApp 初始化

BOOL CGuardServerApp::InitInstance()
{
	_set_purecall_handler(myPurecallHandler);

	// 唯一标识字串，用来创建互斥
	g_applicationHandle=::CreateMutex(NULL,FALSE,one);
	if(GetLastError()==ERROR_ALREADY_EXISTS)	
	{
		//::MessageBox(NULL,"数据监护服务器已经在运行!","数据监护服务器",MB_ICONINFORMATION);
		//AfxMessageBox("数据监护服务器已经在运行!",MB_ICONINFORMATION);	
		return FALSE;
	}


	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	AfxInitRichEdit2();	// to initialize richedit2 library.

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("深圳冰川网络技术有限公司"));

	// 用new的方式以免对象太大堆栈不够用
	CGuardServerDlg *pDlg = new CGuardServerDlg;
	m_pMainWnd = pDlg;
	INT_PTR nResponse = pDlg->DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此处放置处理何时用“确定”来关闭
		//  对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//  对话框的代码
	}

	delete pDlg;

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

