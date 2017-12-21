/*******************************************************************
** 文件名:	SpellContextDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	

技能系统定义
********************************************************************/

#pragma once

#include "ICondition.h"
#include "IEffect.h"

// 技能施法现场
struct SPELL_CONTEXT
{
    int                 nKey;
    DWORD               dwOPTick;       // 施法时的tick数
    Move3               moveState;      // 施法时的移动状态(包括位置、速度、朝向)
	CONDITION_CONTEXT   CC;				// 条件现场
	EFFECT_CONTEXT		EC;				// 效果现场
	
#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
    DWORD               dwCViewTick;        // 显示层触发时间
    DWORD               dwCCmdQueueTick;    // 命令队列收包时间
    DWORD               dwCMsgSendTick;     // 技能消息发送时间
	DWORD				dwGSTickCS;		    // 到达Gateway时间 Client->Gateway
	DWORD				dwGCTickCS;		    // 到达GatewayConnector时间 Gateway->Connector
	DWORD				dwSVTickCS;		    // 到达实体时间 Connector->Player
#endif
	SPELL_CONTEXT()
	{
		memset(this, 0, sizeof(*this));
        EC.ptTargetTileOri = INVALID_LOC;
	}
};

// 技能数据标志位
struct SPELL_DATA_FLAG
{
    int         nFlag[SPELL_FLAG_MAX];        // 技能标志位

    SPELL_DATA_FLAG()
    {
        memset(this, 0, sizeof(*this));
    }
};