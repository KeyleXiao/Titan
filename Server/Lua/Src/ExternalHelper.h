/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/
#pragma once

#ifndef __EXTERNALHELPER_H__
#define __EXTERNALHELPER_H__
// 定义
class CExternalHelper
{
public:
	CExternalHelper();
	~CExternalHelper();

	/** 启用定时器
	*/
	bool SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes = INFINITY_CALL, const char *debugInfo=0);

	/** 销毁定时器
	*/
	bool KillTimer(DWORD dwTimerID, ITimerHandler *pHandler);

    /** 发送数据
    */
    void Send2Client(DWORD dwPDBID, BYTE byKeyModule, BYTE byKeyAction, void *pData, size_t nDataLen);

	/** 发送社会服
	*/
	void Send2Social(BYTE byKeyModule, BYTE byKeyAction, void * pData, size_t stLen);

	/** 显示提示文字
	*/
	void showSystemMessage(DWORD dwDBID, int nTipID, LPCSTR szParam = NULL);
};

extern CExternalHelper g_EHelper;

#endif // __EXTERNALHELPER_H__