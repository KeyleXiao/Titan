/*******************************************************************
** 文件名:	SceneServerAPI.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			场景服API封装
********************************************************************/

#pragma once

#include "EntityDef.h"

#define  MY_LUA_API  extern "C" 

// 创建NPC
// @param sceneID : 场景ID
// @param npc_id  : NPCID
// @param 
MY_LUA_API UID createNPC( int sceneID,int npc_id,float loc_x,float loc_y,float loc_z );

// 销毁NPC
// @param sceneID : 场景ID
// @param uid     :  NPCUID
// @param 
MY_LUA_API UID destroyNPC(int sceneID, int uid);

// 创建Tank
// @param sceneID : 场景ID
// @param npc_id  : NPCID
// @param 
MY_LUA_API UID createTank( int sceneID,int npc_id,float loc_x,float loc_y,float loc_z );

// 发送任务列表
// @param dwPDBID : 玩家DBID
// @param strTask : 任务字符串
// @param 
MY_LUA_API void sendTaskList(DWORD dwPDBID, const char *strTask);

// 更新任务列表
// @param dwPDBID : 玩家DBID
// @param strTask : 任务字符串
// @param 
MY_LUA_API void updateTaskList(DWORD dwPDBID, const char *strTask);

// 删除任务列表
// @param dwPDBID : 玩家DBID
// @param strTask : 任务字符串
// @param 
MY_LUA_API void deleteTaskList(DWORD dwPDBID, const char *strTask);

// 发送任务奖励
// @param dwPDBID : 玩家DBID
// @param strTask : 任务奖励字符串
// @param 
MY_LUA_API void sendTaskPrize(DWORD dwPDBID, const char *strTask);

/**
@name    增加/减少经验值(战场经验)
@param   : dwPDBID:         玩家DBID
@param   : nExp:            经验值（负值减少，正值增加）
@param   : szLogMark:       传入交易日志，供查询
*/
MY_LUA_API void addExp(DWORD dwDBID, int nExp, const char *pszRemark=0);

/**
@name    设置玩家经验(主城经验)
@param   : dwPDBID:         玩家DBID
@param   : lgValue:         要设置数值
@param   : szLogMark:       传入交易日志，供查询
*/
MY_LUA_API void setActorBaseExp(DWORD dwDBID, LONG lgValue, LPCSTR szLogMark=0);

/** 是否可增加或扣除货币
@param  : dwPDBID:        玩家DBID
@param  nTicket: 添加的点券，负数表示扣除点卷
@param  nGlod  : 添加的金币，负数表示扣除金币
@return : 是否可以添加点券
*/
MY_LUA_API bool canAddMoney(DWORD dwDBID, int nTicket, int nGlod);

/** 增加或扣除货币
@param  : dwPDBID:       玩家DBID
@param  nTicket: 添加的点券，负数表示扣除点卷
@param  nGlod  : 添加的金币，负数表示扣除金币
@param  : pszRemark:     添加的点券日志的备注（记日志用）
@note   : 在添加点券操作提交数据库（包括其他模块提交的）还没有返回前，这时候你提交点券是会失败的
*/
MY_LUA_API void addMoney(DWORD dwDBID, int nTicket, int nGlod, const char *pszRemark=0);

/** 是否可以给角色奖励
@param  : dwPDBID:       玩家DBID
@param  : nPrizeID:      奖励物品ID
@param  : nNum:          奖励物品数量
@note   : 是否可以给角色奖励
*/
MY_LUA_API bool canPrizeActor(DWORD dwDBID, int nPrizeID, int nNum);

/** 给角色奖励
@param  : dwPDBID:       玩家DBID
@param  : nPrizeID:      奖励物品ID
@param  : nNum:          奖励物品数量
@param  : dwTargetID:    如果有目标者，这个就填入目标者ID
@param  : szLogMark:     传入交易日志，供查询 
@note   : 给角色奖励
*/
MY_LUA_API bool prizeActor(DWORD dwDBID, int nPrizeID, int nNum, DWORD dwTargetID, LPCSTR szLogDesc);

/** 显示提示文字
@param  : dwPDBID:       玩家DBID
@param  : nPrizeID:      奖励物品ID
@param  : szParam:       描述文字
*/
MY_LUA_API void showSystemMessage(DWORD dwDBID, int nTipID, LPCSTR szParam);

/** 增加任务文字日志
@param  : dwPDBID:       玩家DBID
@param  : szParam:       日志文字
*/
MY_LUA_API void addTaskGameNoteLog(DWORD dwDBID, LPCSTR szParam);

/** 获取玩家等级
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerLevel(DWORD dwDBID);

/** 获取玩家联盟
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerClanID(DWORD dwDBID);

/** 获取玩家比赛场数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerMatchCount(DWORD dwDBID);

/** 获取玩家英雄个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerHeroCount(DWORD dwDBID);

/** 获取玩家英雄皮肤个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerSkinCount(DWORD dwDBID);

/** 获取玩家师傅个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerMasterCount(DWORD dwDBID);

/** 获取玩家学徒个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerPrenticeCount(DWORD dwDBID);

/** 获取玩家英雄某个等级个数
@param  : dwPDBID:       玩家DBID
@param  : nLevel:        玩家英雄等级
*/
MY_LUA_API int getPlayerHeroCountByLevel(DWORD dwDBID, int nLevel);

/** 获取玩家击杀大龙个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerKillBigDragoCount(DWORD dwDBID);

/** 获取玩家击杀小龙个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerKillSmallDragoCount(DWORD dwDBID);

/** 获取玩家战场使用翅膀记录
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerWarUseWringCount(DWORD dwDBID);

/** 获取玩家战场使用XP
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerWarUseXPCount(DWORD dwDBID);

/** 获取玩家当前的新手引导步骤
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerGuideStep(DWORD dwDBID);

/** 监控任务接收，完成等情况
@ param nTaskID	   `: 任务id
@ param nAcceptNum	: 接受次数
@ param nFinishNum	: 完成次数
@ param nGiveUpNum  : 放弃次数
*/
MY_LUA_API void OssMonitorTask(int nTaskID, int nAcceptNum, int nFinishNum, int nGiveUpNum);

/** 添加交互任务
@ param dwDBID			: 玩家DBID
@ param bySubType		: 任务子类型
@ param bySpecificType	: 任务子类型下具体类型
@ param nResult			: 可否添加
*/
MY_LUA_API void onCheckAddContactTask(DWORD dwDBID, int bySubType, int bySpecificType, int nTagIDHigh32, int nTagIDLow32, LPCSTR szEmotionWords, int nResult);

/** 发送添加成功
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API void sendAddTaskSuccess(DWORD dwPDBID, const char *strTask);

/** 发送预获取任务结果
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API void sendPreCatchTaskResult(DWORD dwPDBID, const char *strTask);

/** 获取交互任务
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API void sendCatchTaskResult(DWORD dwDBID, const char *strTask);

/** 刷新操作信息
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API void sendReflushOptInfo(DWORD dwDBID, const char *strTask);

/** 检测刷新任务操作扣钱是否成功
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API bool checkAddMoneyOfReflushOpt(DWORD dwDBID);

/** 检测目标是否为玩家徒弟
@param  : dwPDBID:       玩家DBID
@param  : dwTargetDBID	 目标DBID
*/
MY_LUA_API int checkIsPlayerPrentice(DWORD dwDBID, DWORD dwTargetDBID);

/** 给玩家发送奖励邮件
@param  : dwPDBID		玩家DBID
@param  : nGold			金币
@param  : nExp			经验
@param  : nPrizeID1		奖励ID1
@param  : nPrizeID2		奖励ID2
*/
MY_LUA_API void sendPlayerMail(DWORD dwDBID, int nGold, int nExp, int nPrizeID1, int nPrizeID2);
