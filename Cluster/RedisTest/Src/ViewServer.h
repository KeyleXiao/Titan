/*******************************************************************
** 文件名:	ViewServer.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/21
** 版  本:	1.0
** 描  述:	语音网关主程序
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


// ViewServer.h :  应用程序的主头文件
//

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

// CViewServerApp:
// 有关此类的实现，请参阅 VoiceGateway.cpp
//

class CViewServerApp : public CWinApp
{
public:
	CViewServerApp();



// 重写
	public:
	virtual BOOL InitInstance();



// 实现

	DECLARE_MESSAGE_MAP()
};

extern CViewServerApp theApp;