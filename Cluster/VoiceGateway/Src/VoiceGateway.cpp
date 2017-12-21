/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\VoiceGateway.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音网关主程序
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


// VoiceGateway.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "VoiceGateway.h"
#include "VoiceGatewayDlg.h"
#include "GlobalVoiceGateway.h"
#include "ClientUser.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define one  "{6BB0CCE2-CC84-4a59-BB65-BC73B527C95C}"		//唯一标识字串，用来创建互斥
HANDLE g_handleVoiceGateway;								//互斥句柄

// CVoiceGatewayApp

BEGIN_MESSAGE_MAP(CVoiceGatewayApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVoiceGatewayApp 构造

CVoiceGatewayApp::CVoiceGatewayApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CVoiceGatewayApp 对象

CVoiceGatewayApp theApp;


void myPurecallHandler(void)
{
	//Error("虚函数调用异常！！！");

	__debugbreak();
}

// CVoiceGatewayApp 初始化

BOOL CVoiceGatewayApp::InitInstance()
{
	_set_purecall_handler(myPurecallHandler);

	// 唯一标识字串，用来创建互斥
	//g_handleVoiceGateway=::CreateMutex(NULL,FALSE,one);
	//if(GetLastError()==ERROR_ALREADY_EXISTS)	
	//{
		//::MessageBox(NULL,"语音网关已经在运行!","语音网关",MB_ICONINFORMATION);
		//AfxMessageBox("语音网关已经在运行!",MB_ICONINFORMATION);	
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

	//__asm { int 3};

	// 处理命令行参数
	LPCSTR pszCommandLine = GetCommandLineA();
	if(pszCommandLine != NULL)
	{
		std::string cmdline = " ";
		cmdline += pszCommandLine;
		cmdline += " ";
		StringHelper::toLower(cmdline);

		for( DWORD dwID = 255; dwID >= 1; dwID -- )
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
	SetRegistryKey(_T("深圳冰川网络技术有限公司"));

	// 用new的方式以免对象太大堆栈不够用
	CVoiceGatewayDlg *pDlg = new CVoiceGatewayDlg;
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
