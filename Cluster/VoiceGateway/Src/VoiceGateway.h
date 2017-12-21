/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\VoiceGateway.h
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


// VoiceGateway.h :  应用程序的主头文件
//

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

// CVoiceGatewayApp:
// 有关此类的实现，请参阅 VoiceGateway.cpp
//

class CVoiceGatewayApp : public CWinApp
{
public:
	CVoiceGatewayApp();



// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()

};

extern CVoiceGatewayApp theApp;