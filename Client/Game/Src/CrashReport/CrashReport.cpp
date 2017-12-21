#include "stdafx.h"
#include "CrashReport.h"
#include "SendMailThread.h"
#include "IClientGlobal.h"
#ifdef SUPPORT_STACKWALK

//#include "GlobalClient.h"

#ifdef _DEBUG
#   ifdef RKT_UTF8
	#   define CRASH_REPORT_APP	("CrashReport_d.exe")
#   else
	#   define CRASH_REPORT_APP	_T("CrashReport_d.exe")
#   endif
#else
#   ifdef RKT_UTF8
#       define CRASH_REPORT_APP	("CrashReport.exe")
#   else
#       define CRASH_REPORT_APP	_T("CrashReport.exe")
#   endif
#endif

//extern string g_stringTracePath;

void CrashReport::onDump(IStackWalk* pDump,PEXCEPTION_POINTERS pExceptionInfo)
{
}

long CrashReport::onDumpFinish(IStackWalk* pDump,PEXCEPTION_POINTERS pExceptionInfo)
{
	TCHAR pszFilePart[MAX_PATH];
	std::string exefile = getWorkDir();
	exefile += "\\";
	exefile += CRASH_REPORT_APP;
	if (!isFile(exefile.c_str()))
		return EXCEPTION_EXECUTE_HANDLER;

	lstrcpy(pszFilePart, my_utf82t(exefile.c_str()));

	TCHAR szModuleName[MAX_PATH];
	lstrcpy(szModuleName,pDump->getCrashModuleName(pExceptionInfo));

	TCHAR szCommandLine[1024];
	lstrcpy(szCommandLine,pszFilePart);
	lstrcat(szCommandLine,_T(" "));

	lstrcat(szCommandLine, _T(" \""));	// surround app name with quotes
	ZeroMemory(szModuleName, sizeof(szModuleName));
	GetModuleFileName(0, szModuleName, _countof(szModuleName)-2);

	TCHAR* pModuleName = _tcsrchr(szModuleName,_T('\\'));
	if(pModuleName)
	{
		pModuleName++;
		lstrcat(szCommandLine,pModuleName);
	}
	else
	{
// _GAMEMARKET_VER_BEGIN_		
	lstrcat(szCommandLine,_T("TITAN.exe"));
// _GAMEMARKET_VER_END_
	}
	lstrcat(szCommandLine, _T("\""));

	//取得崩溃模块的文件版本号
	WORD pwVer[4];
	pDump->getCrashModuleFileVer(pExceptionInfo,pwVer);

	//将版本号的第3位加到标题
	char szSubject[256];
	LPTSTR szCrashModuleName = (LPTSTR)pDump->getCrashModuleName(pExceptionInfo);

	//将这些改成小写
	for(unsigned int i = 0;i < _tcslen(szCrashModuleName);i++)
	{
		if(szCrashModuleName[i] >= 'A' && szCrashModuleName[i] <= 'Z')
			szCrashModuleName[i] -= ('A' - 'a');
	}

	TCHAR szFileName[1024];
	GetModuleFileName(GetModuleHandle(NULL), szFileName, sizeof(szFileName));
	std::string ver;
	getModuleFileVersion(my_t2utf8(szFileName), ver);

	std::string qqinfo;
	getIMInfo(qqinfo);

	if (!qqinfo.empty()) // 没开qq或者win版本不合适？
	{
		qqinfo.insert(0, "QQ");
	}
	else
	{
		qqinfo = "无QQ";
	}

	DWORD major = 0, minor = 0;
	getWinVersion(&major, &minor);

	std::string strMajor, strMinor;
	StringHelper::fromInt(strMajor, major);
	StringHelper::fromInt(strMinor, minor);

	TCHAR pcName[256] = {0};
	DWORD dwSize = 256;
	::GetComputerName(pcName, &dwSize);

	Ini ini("config.ini");
	std::string serverName;
	std::string server = ini.getString("Login", "Server", "");
	if (!server.empty())
	{
		std::vector<std::string> vs;
		StringHelper::split(vs, server, " ");
		if (vs.size() == 2)
		{
			serverName = vs[1];
		}
	}

	//strcpy(szSubject,szCrashModuleName);
	sprintf(szSubject,"TITAN非法[%s][%s]%s[%d.%d.%d.%d](%s.%s)[%s][%s] %08x",/*pDump->GetExceptionDesc(pExceptionInfo),*/
		serverName.c_str(),
		ver.c_str(), 
		szCrashModuleName,
		pwVer[0],pwVer[1],pwVer[2],pwVer[3],
		strMajor.c_str(),
		strMinor.c_str(),
		pcName, 
		qqinfo.c_str(), 
		pDump->getCrashFileOffset(pExceptionInfo));

	lstrcat(szCommandLine, _T(" \""));	// surround app name with quotes
	lstrcat(szCommandLine,a2w(szSubject));
	lstrcat(szCommandLine, _T("\""));

	// 邮箱
	std::string strMailTo = ini.getString("App", "CrashMail", "bugtitan@szgla.cn");
	lstrcat(szCommandLine, _T(" \""));	// surround app name with quotes
	lstrcat(szCommandLine,a2w(strMailTo.c_str()) );
	lstrcat(szCommandLine, _T("\""));

	// 发件人信息
	char mailinfo[256];
	//int i = rand() % 10 + 1;
	//sprintf_s(mailinfo, "bccr%03d@21cn.com,107628,bccr%03d@21cn.com,smtp.21cn.com,25", i, i);
	sprintf_s(mailinfo, "bugtitan@szgla.cn,#d9l1dD.&29k,bugtitan@szgla.cn,mail.szgla.cn,25");	

	std::string strAuthLogin = ini.getString("App", "AuthLogin", mailinfo);
	lstrcat(szCommandLine, _T(" \""));	// surround app name with quotes
	lstrcat(szCommandLine,a2w(strAuthLogin.c_str()) );
	lstrcat(szCommandLine, _T("\""));

	// trace文件路径
	lstrcat(szCommandLine, _T(" \""));	// surround app name with quotes
	//lstrcat(szCommandLine,my_utf82t(g_stringTracePath.c_str()));
	lstrcat(szCommandLine,my_utf82t(gClientGlobal->getLogFileName()));
	lstrcat(szCommandLine, _T("\""));

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	//EmphasisLn(szCommandLine);

	if (CreateProcess(
		NULL,					// name of executable module
		szCommandLine,			// command line string
		NULL,					// process attributes
		NULL,					// thread attributes
		FALSE,					// handle inheritance option
		0,						// creation flags
		NULL,					// new environment block
		NULL,					// current directory name
		&si,					// startup information
		&pi))					// process information
	{
		if(WAIT_OBJECT_0 == WaitForSingleObject(pi.hProcess,INFINITE))
		{
			DWORD exitCode;
			if(GetExitCodeProcess(pi.hProcess,&exitCode))
			{
				if(exitCode == (DWORD)-1)return EXCEPTION_CONTINUE_SEARCH;
			}
		}
		EmphasisLn("CrashReport End")
		return EXCEPTION_EXECUTE_HANDLER;
	}
	else
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

void CrashReport::onDumpStart(IStackWalk* pDump)
{
	EmphasisLn("CrashReport Start")
	EmphasisLn(gClientGlobal->getLogFileName())
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	::GlobalMemoryStatusEx(&ms);
	EmphasisLn("TotalPhys:"<<ms.ullTotalPhys<<" AvailPhys:"<<ms.ullAvailPhys<<" MemoryLoad:"<<ms.dwMemoryLoad);
	if(!pDump)return;

	pDump->setLogFileName(_T("crash.txt"));
	pDump->setDumpFileName(_T("crash.dmp"));

	try
	{
		//static_cast<GlobalClient*>(gGlobalClient)->close();
	}
	catch (...)
	{
	}
}

CrashReport g_CrashReport;

#endif // #ifdef SUPPORT_STACKWALK