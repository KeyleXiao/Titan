
// SceneServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "SceneServer.h"
#include "SceneServerDlg.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSceneServerApp

BEGIN_MESSAGE_MAP(CSceneServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSceneServerApp 构造

CSceneServerApp::CSceneServerApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

    // 默认启动的服务器ID
    m_nDefaultSvrID = 0;
}


// 唯一的一个 CSceneServerApp 对象

CSceneServerApp theApp;

void myPurecallHandler(void)
{
    Error("虚函数调用异常！！！");

    __debugbreak();
}

// CSceneServerApp 初始化


BOOL CSceneServerApp::InitInstance()
{
    _set_purecall_handler(myPurecallHandler);

	AfxInitRichEdit2();

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

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

    //////////////////////////////////////////////////////////////////////////
    // 处理命令行参数
    LPCSTR pszCommandLine = GetCommandLineA();
    if(pszCommandLine != NULL)
    {
        std::string strCmdline = " ";
        strCmdline += pszCommandLine;
        strCmdline += " ";
        StringHelper::toLower(strCmdline);

        istringstream iss(strCmdline);
        //copy(istream_iterator<string>(iss), istream_iterator<string>(), ostream_iterator<string>(cout, "\n"));

        vector<string> tokens;
        copy(istream_iterator<string>(iss),
            istream_iterator<string>(),
            back_inserter(tokens));

        size_t nArgs = tokens.size();
        const char scene_flag[] = "sceneserver"; 
        const char auth_flag[] = "auth"; 

        bool has_find_scene = false;
        bool has_find_auth = false;

        for(size_t i=1; i<nArgs; i++) 
        {
            string & pszText = tokens[i];

            // 解析场景ID
            if(!has_find_scene) {
                const char* ret =  strstr(pszText.data(), scene_flag);
                if ( ret ) {
                    m_nDefaultSvrID = atoi(ret+ strlen(scene_flag));
                    has_find_scene = true;
                    continue;
                }
            }

            // 解析认证数据
            if(!has_find_auth) {
                const char* ret =  strstr(pszText.data(), auth_flag);
                if ( ret ) {
                    has_find_auth = true;
                    if(i+2 < nArgs)
                    {
                        m_strUserName = tokens[++i];        
                        m_strPassword = tokens[++i];
                        continue;
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////

    // 标准初始化
    // 如果未使用这些功能并希望减小
    // 最终可执行文件的大小，则应移除下列
    // 不需要的特定初始化例程
    // 更改用于存储设置的注册表项
    // TODO: 应适当修改该字符串，
    // 例如修改为公司或组织名
    SetRegistryKey(_T("深圳冰川网络技术有限公司"));


	CSceneServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


/** 
@param   
@param   
@return  
*/
void CSceneServerApp::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
    g_pSceneServerDlg->PostMessage(message, wParam, lParam);
}