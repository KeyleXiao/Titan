/*******************************************************************
** 文件名:	SkillDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	石亮
** 日  期:	12/13/2014
** 版  本:	1.0
** 描  述:	技能功能模块定义头文件

			
********************************************************************/


#ifndef __COMMON_SKILL_DEF_H__
#define __COMMON_SKILL_DEF_H__

#include "NetMessageDef.h"


#pragma pack(1)

/////////////////////////// 技能模块消息  ////////////////////////////

// 消息码定义键值
enum SKILL_NET_MESSAGE_KEYACTION
{
	SC_MSG_ACTION_INISKILL = SC_MSG_SPELL_START,	// 服务端-客户端 冷却初始化
	CS_MSG_ACTION_USE_SKILL,						// 客户端-服务器 使用技能
	SC_MSG_ACTION_USE_SKILL,						// 服务端-客户端 使用技能
	CS_MSG_HIT_NOW,									// 客户端-服务器
	SC_MSG_ACTION_PREPARE_DAMAGE,					// 服务端-客户端 发送伤害
};

//-----------------------------技能相关---------------------------------
// 使用技能
// 上下文：SkillContextImp
// CS_MSG_ACTION_USE_SKILL
// SC_MSG_ACTION_USE_SKILL
// CS_MSG_HIT_NOW
struct msg_creature_hitNow
{
	int32   nActorID;				// 角色ID   
	DWORD   nTick;					// 快照时间点
	int32	nHitTargetID;			// 击中目标ID
	int32	nSerialID;				// 序列号ID
	float	fCollisionPosPos[3];	// 碰撞位置
};

struct msg_creature_skillview
{
	int32 nSkillviewID;			//技能显示方面的配置表ID
	int32 nSrcEntityID;			//攻击者EntityID
	int32 nTargetEntityID;		//受击者EntityID
	int32 nFeedBackID;			//反馈ID，当发生碰撞时返回此ID

	float fTargetPos[3];		//技能指向的位置

	msg_creature_skillview()
	{
		memset(this, 0, sizeof(msg_creature_skillview));
	}
};

struct msg_close_skillview
{
	int32 nSkillviewID;			//技能显示方面的配置表ID

	msg_close_skillview()
	{
		memset(this, 0, sizeof(msg_close_skillview));
	}
};

// 服务器发送伤害
struct msg_creature_prepareDamage
{
	int32   nActorID;			// 角色ID   
	DWORD   nTick;				// 快照时间点
	int32	nHitTargetID;		// 击中目标ID
	int32	nSerialID;			// 序列号ID
};


#pragma pack()

#endif	// __COMMON_SKILL_DEF_H__