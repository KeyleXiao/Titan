/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉 
** 日  期:	13/2/2017
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#ifndef __EXTERNALHELPER_H__
#define __EXTERNALHELPER_H__

#include "MailDef.h"

// 辅助功能类
class CExternalHelper
{
public:
	// 构造函数
	CExternalHelper(void);
	// 析构函数
	~CExternalHelper(void);

	// 发送消息到社会服
	void sendMailSocial(BYTE byActionID, void* pData = NULL, int nLen = 0);
	// 发送消息
	void sendMailZone(BYTE byActionID, void* pData, int nLen);

};
extern CExternalHelper				g_EHelper;

#endif // __EXTERNALHELPER_H__