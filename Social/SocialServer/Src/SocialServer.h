
// SocialServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSocialServerApp:
// �йش����ʵ�֣������ SocialServer.cpp
//

class CSocialServerApp : public CWinApp
{
public:
	CSocialServerApp();

// ��д
public:
	virtual BOOL InitInstance();

    void SetTraceFile(const char* szTraceFile);

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSocialServerApp theApp;