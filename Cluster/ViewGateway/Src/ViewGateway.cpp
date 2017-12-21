// VoiceGateway.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ViewGateway.h"
#include "ViewGatewayDlg.h"
#include "GlobalViewGateway.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewGatewayApp

BEGIN_MESSAGE_MAP(CViewGatewayApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CViewGatewayApp 构造

CViewGatewayApp::CViewGatewayApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CViewGatewayApp 对象

CViewGatewayApp theApp;


void myPurecallHandler(void)
{
	//Error("虚函数调用异常！！！");

	__debugbreak();
}

// 判断相同ServerID的本进程是否已经开启
bool IsProcessExist(DWORD dwServerID)
{
	// 唯一标识字串，用来创建互斥
	CString strTemp;
	strTemp.Format( _T("观战网关[%d] 已经在运行!"), dwServerID );
	//干脆就用这个提示文本做互斥key
	HANDLE temp = ::CreateMutex(NULL,TRUE,strTemp);
	if(GetLastError()==ERROR_ALREADY_EXISTS)	
	{
		AfxMessageBox(strTemp,MB_ICONINFORMATION);
		return true;
	}

	return false;
}

bool IsOutOfRange(DWORD dwServerID)
{
	if (dwServerID > MAX_GATEWAY_COUNT)
	{
		CString strTemp;
		strTemp.Format(_T("观战网关本地ID[%d] 超出范围[%d]!"), dwServerID, MAX_GATEWAY_COUNT);
		AfxMessageBox(strTemp, MB_ICONINFORMATION);
		return true;
	}
	return false;
}

// CViewGatewayApp 初始化
BOOL CViewGatewayApp::InitInstance()
{
	_set_purecall_handler(myPurecallHandler);

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
	auto strFormat = "view%d";
	if(pszCommandLine != NULL)
	{
		std::string cmdline = " ";
		cmdline += pszCommandLine;
		cmdline += " ";
		StringHelper::toLower(cmdline);

		for( DWORD dwID = 255; dwID >= 1; dwID -- )
		{
			char section[128]; sprintf_s(section, strFormat, dwID);

			if(cmdline.find(section) != std::string::npos)
			{
				dwServerID = dwID; break;
			}
		}
	}

	// dwServerID不能超过2个字节
	if (IsOutOfRange(dwServerID))
		return FALSE;

	if (IsProcessExist(dwServerID))
		return FALSE;


	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("深圳冰川网络股份有限公司"));

	// 用new的方式以免对象太大堆栈不够用
	CViewGatewayDlg *pDlg = new CViewGatewayDlg;
	gSetting.SetServerID((WORD)dwServerID);
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
