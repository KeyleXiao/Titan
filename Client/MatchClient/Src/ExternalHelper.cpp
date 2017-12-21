﻿/*******************************************************************
** 文件名:	ExternalHelper.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/12/2015
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#include "stdafx.h"
#include "ExternalHelper.h"
#include "IRenderView.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;
#include "IEntityProperty.h"
#include "ISchemeCenter.h"
#include "IChatClient.h"

CExternalHelper				g_EHelper;

/** 取得实体
*/
IEntity *CExternalHelper::GetEntity(UID uidEntity)
{
	IEntityFactory * pEntityWorld = gClientGlobal->getEntityFactory();
	if(pEntityWorld == NULL)
	{
		return NULL;
	}

	return pEntityWorld->getEntity(uidEntity);
}


/** 启用定时器
*/
bool CExternalHelper::SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes)
{
	TimerAxis *pTimerAxis = gClientGlobal->getTimerAxis();
	if (pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->SetTimer(dwTimerID, dwInterval, pHandler, dwCallTimes);
}


/** 销毁定时器
*/
bool CExternalHelper::KillTimer(DWORD dwTimerID, ITimerHandler *pHandler)
{
	TimerAxis *pTimerAxis = gClientGlobal->getTimerAxis();
	if (pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(dwTimerID, pHandler);
}


/** 取得技能槽位信息
*/
SPELL_SLOT *CExternalHelper::getSpellSlot(int nVocation)
{
	ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return NULL;
	}

	ISchemeSpellSlot *pSchemeSpellSlot = pSchemeCenter->getSchemeSpellSlot();
	if (pSchemeSpellSlot == NULL)
	{
		return NULL;
	}

	return pSchemeSpellSlot->getSpellSlot(nVocation);
}


/** 显示提示文字
*/
void CExternalHelper::showSystemMessage(EMChatTipID nTextID, LPCSTR szParam)
{
    IChatClient *pChatClient = gClientGlobal->getChatClient();
    if (pChatClient == NULL)
    {
        return;
    }

	pChatClient->showSystemMessage(nTextID, szParam);
}