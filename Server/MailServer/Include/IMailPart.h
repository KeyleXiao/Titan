/*******************************************************************
** 文件名:	__IMailPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/7/2017
** 版  本:	1.0
** 描  述:	邮件	

********************************************************************/

#pragma once
#include "IEntity.h"
#include "MailDef.h"

struct __IMailPart:public __IEntityPart
{
    // 删除邮件
    virtual bool deleteMail(LONGLONG llMailID) = 0;
	// 处理邮件详细数据
	virtual bool dealPlayerMailDetailData(LONGLONG llMailID, bool isAutoRecvPlus = false) = 0;
	// 添加邮件详细数据
	virtual void addPlayerMailDetailData(SMailDetailData sDetailData, bool isAutoRecvPlus = false) = 0;
	// 处理邮件发送附件数据
	virtual bool dealSendMailPlusData(SMailSendData sendData) = 0;

	// 获取领取附件时间
	virtual DWORD getLastRecvPlusTime() = 0;

	// 设置领取附件时间
	virtual void setLastRecvPlusTime(DWORD dwTime) = 0;

	// 邮件是否已领取
	virtual bool isAreadyRecvMail(LONGLONG llMailID) = 0;

	// 是否在快速取件
	virtual bool isFastRecvMailPlus() = 0;

	// 设置快速取件标识
	virtual void setFastRecvMailPlus(bool bFlag, int nRecvCount) = 0;
};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IMailPart * CreateMailPart();
#	define	CreateMailPartProc	CreateMailPart
#else													/// 动态库版本
typedef __IMailPart * (RKT_CDECL * procCreateMailPart)();
#	define	CreateMailPartProc	DllApi<procCreateMailPart>::load(MAKE_DLL_NAME(MailServer), "CreateMailPart")
#endif
