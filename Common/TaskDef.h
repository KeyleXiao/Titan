/*******************************************************************
** 文件名:	E:\speed\Common\TaskDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	任务相关定义
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "NameLenDef.h"
#include "GameDef.h"

//输出DEBUG信息
//#define TASK_DEBUG_OUTPUT
//#ifdef TASK_DEBUG_OUTPUT
//#	define	TASK_TRACELN(x)			TraceLn(x)
//#	define	TASK_WARNINGLN(x)		WarningLn(x)
//#	define	TASK_ERRORLN(x)			ErrorLn(x)
//#	define	TASK_EMPHASISLN(x)		EmphasisLn(x)
//#else
//#	define	TASK_TRACELN(x)
//#	define	TASK_WARNINGLN(x)
//#	define	TASK_ERRORLN(x)
//#	define	TASK_EMPHASISLN(x)
//#endif

#pragma pack(1)

//============================= 基本系统 ====================================

// 任务Lua触发事件 (与lua中的一一对应)
ENUM EMTaskLuaEventType
{
	emTaskLuaEventType_Level = 1,					// 等级变化
	emTaskLuaEventType_FinishMatch,					// 完成一场比赛
	emTaskLuaEventType_AddHero,						// 获得一个永久英雄
	emTaskLuaEventType_AddSkin,						// 获得一个永久皮肤
	emTaskLuaEventType_OpenChest,					// 开启宝箱
	emTaskLuaEventType_InlayGem,					// 镶嵌宝石
	emTaskLuaEventType_HeroUpGrade,					// 英雄晋级
	emTaskLuaEventType_BuildMentorship,				// 建立师徒关系
	emTaskLuaEventType_ClanDonate,					// 联盟捐赠
	emTaskLuaEventType_ApplyFriend,					// 发起好友申请
	emTaskLuaEventType_ContactReply,				// 回复交互任务
	emTaskLuaEventType_PK,							// PK
	emTaskLuaEventType_NoviceGuide,					// 新手引导
};

// 交互任务
typedef DBREQ_TABLE_CONTACTTASK_NODE SContactTaskNode;

// 交互任务回复
typedef DBREQ_TABLE_CONTACTTASK_NODE_REPLY SContactTaskNodeReply;

#pragma pack()