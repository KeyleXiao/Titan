/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\VoiceServer.cpp
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音服务器主程序
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


// VoiceServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ViewMngApp.h"
#include "ViewMngDlg.h"
#include "GlobalViewMng.h"
#include "ILoginServerProtocol.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define one  "{667BA55B-DA60-47c1-BF17-901A097B821C}"		//唯一标识字串，用来创建互斥
HANDLE g_handleVoiceServer;								//互斥句柄

// CViewMngApp

BEGIN_MESSAGE_MAP(CViewMngApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CViewMngApp 构造

CViewMngApp::CViewMngApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CViewMngApp 对象

CViewMngApp theApp;


void myPurecallHandler(void)
{
	//Error("虚函数调用异常！！！");

	//__asm{int 3};
}

// CViewMngApp 初始化

BOOL CViewMngApp::InitInstance()
{
	_set_purecall_handler(myPurecallHandler);

	// 唯一标识字串，用来创建互斥
	//g_handleVoiceServer=::CreateMutex(NULL,FALSE,one);
	//if(GetLastError()==ERROR_ALREADY_EXISTS)	
	//{
		//::MessageBox(NULL,"语音服务器已经在运行!","语音服务器",MB_ICONINFORMATION);
		//AfxMessageBox("语音服务器已经在运行!",MB_ICONINFORMATION);	
	//	return FALSE;
	//}


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

	DWORD dwServerID = 1;
	// 处理命令行参数
	LPCSTR pszCommandLine = GetCommandLineA();
	if(pszCommandLine != NULL)
	{
		std::string cmdline = " ";
		cmdline += pszCommandLine;
		cmdline += " ";
		StringHelper::toLower(cmdline);
		for( DWORD dwID = 255; dwID > 0; dwID -- )
		{
			char section[128]; sprintf_s(section, "voice%d", dwID);

			if(cmdline.find(section) != std::string::npos)
			{
				dwServerID = dwID; break;
			}
		}
	}

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("深圳冰川网络股份有限公司"));

	// 用new的方式以免对象太大堆栈不够用
	CViewMngDlg *pDlg = new CViewMngDlg;
	pDlg->m_dwServerID = dwServerID;
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
