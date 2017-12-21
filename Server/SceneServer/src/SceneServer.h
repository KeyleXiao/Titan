
// SceneServer.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

// CSceneServerApp:
// 有关此类的实现，请参阅 SceneServer.cpp
//

class CSceneServerApp : public CWinApp
{
public:
	CSceneServerApp();

// 重写
public:
	virtual BOOL InitInstance();

	/** 
	@param   
	@param   
	@return  
	*/
	void		PostMessage(UINT message, WPARAM wParam, LPARAM lParam);
// 实现

	DECLARE_MESSAGE_MAP()

public:
    // 默认启动的服务器ID
    int			    m_nDefaultSvrID;
    // 账号
    std::string    m_strUserName;
    // 密码
    std::string    m_strPassword;
};

extern CSceneServerApp theApp;