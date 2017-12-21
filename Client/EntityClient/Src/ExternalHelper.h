/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/12/2015
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#pragma once

#ifndef __EXTERNALHELPER_H__
#define __EXTERNALHELPER_H__

#include "IEntity.h"
#include "SpellDef.h"
#include "ChatDef.h"

using namespace SPELL;

class CExternalHelper
{
public:
	/** 取得实体
	*/
	IEntity *GetEntity(UID uidEntity);

	/** 启用定时器
	*/
	bool SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD nCallTimes=INFINITY_CALL);

	/** 销毁定时器
	*/
	bool KillTimer(DWORD dwTimerID, ITimerHandler *pHandler);

    /** 显示提示文字
	*/
	void showFloatText(IEntity *pEntity, EMChatTipID nTextID);

    /** 发消息给场景服
	*/
    void sendMessageToScene(BYTE byKeyModule, BYTE byKeyAction, void *pData, int nDataLen);

	/** 发消息给社会服
	*/
    void sendMessageToSocial(BYTE byKeyModule, BYTE byKeyAction, void *pData, int nDataLen);
};

extern CExternalHelper				g_EHelper;

#endif // __EXTERNALHELPER_H__