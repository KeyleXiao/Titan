/*******************************************************************
** 文件名:	WarMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/23/2015
** 版  本:	1.0
** 描  述:	War消息定义
********************************************************************/

#ifndef __WAR_STATICMESSAGE_DEF_H__
#define __WAR_STATICMESSAGE_DEF_H__

#pragma pack(1)
using namespace War;

/////////////////////////// War模块消息  ////////////////////////////
// 消息码定义键值
enum SC_STATICWAR_NET_MESSAGE
{
    SC_NET_MSG_WARSTATIC = SC_MSG_WARSTATIC_START,		// 战场消息

	// 战绩相关
	CS_MSG_WAR_GET_WARSTATICS_TOTAL,	// 获取战绩总览
	SC_MSG_WAR_SEND_WARSTATICS_TOTAL,	// 发送战绩总览到客户端
	CS_MSG_WAR_GET_WARSTATICS_WAR,		// 获取指定战场信息
	SC_MSG_WAR_SEND_WARSTATICS_WAR,		// 发送指定战场信息到客户端
	SS_MSG_WAR_UPDATE_WARSTATICS,		// 更新战绩缓存信息
	CS_MSG_WAR_GET_HISTORY_RECORD,      // 获取玩家最近的历史战绩，在新手引导过渡期，用于入选本命英雄时的判断依据
	SC_MSG_WAR_SEND_HISTORY_RECORD,		// 发送玩家最近的历史战绩到客户端
	SC_MSG_WAR_OTHER_CROWN_PAGE_INLAY,	// 其他玩家的宝石镶嵌情况
    CS_MSG_WAR_CROWNDOTHER_PAGE,        // 获取其他玩家的核晶信息
};


// CS_MSG_WAR_GET_WARSTATICS_TOTAL
struct msg_war_get_warstatics_total
{
	PDBID actorID;	// 目标角色PDBID

	msg_war_get_warstatics_total(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
// SC_MSG_WAR_SEND_WARSTATICS_TOTAL
struct msg_war_send_warstatics_total_heroinfo
{
	WORD	wHeroID;				// 英雄ID
	DWORD	dwHeroWinNum;			// 胜利场次
	DWORD	dwHeroMatchNum;			// 总场次

	msg_war_send_warstatics_total_heroinfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct msg_war_send_warstatics_total
{
	PDBID	nActorID;				// 目标角色PDBID
	DWORD	dwRecentKillCount;		// 近30天杀敌数
	DWORD	dwRecentDieCount;		// 近30天死亡数
	DWORD	dwRecentAssistCount;	// 近30天助攻数
	DWORD	dwRecentScore;			// 近30天战绩评分
	WORD	wRecentKDAScore;		// 近30天KDA分数
	WORD	wRecentWinCount;		// 近30天胜场
	WORD	wRecentLoseCount;		// 近30天负场

	WORD	wWarInfoCount;			// 战绩信息数量
	//wWarInfoCount * DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO

	msg_war_send_warstatics_total(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
// CS_MSG_WAR_GET_WARSTATICS_WAR
struct msg_war_get_warstatics_war
{
	LONGLONG nWarID;				// 战场历史ID
    BYTE    byWnd;                  // 返回到哪个窗体 EGetSingleWarType

	msg_war_get_warstatics_war(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
// SC_MSG_WAR_SEND_WARSTATICS_WAR
struct msg_war_send_warstatics_war
{
	LONGLONG nWarID;				// 战场历史ID
    BYTE    byWnd;                  // 返回到哪个界面 EGetSingleWarType
	DWORD	dwBeginTime;            // 战场开始时间
	DWORD	dwEndTime;              // 战场结束时间
	WORD	wWarType;               // 战场类型
	WORD	wEndBecause;            // 结束原因 1、代表阵营1胜利 2、代表阵营2胜利 3、代表阵营1投降 4、代表阵营2投降
	WORD	wBanHeroID1[DBDEF_BAN_HERO_SIZE];		// 阵营1禁用英雄列表
	WORD	wBanHeroID2[DBDEF_BAN_HERO_SIZE];		// 阵营2禁用英雄列表

	DWORD	dwActorCount;			// 参与玩家数量
	// dwActorCount * DBREQ_PARAM_WAR_ACTOR_INFO

	msg_war_send_warstatics_war(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
// SS_MSG_WAR_UPDATE_WARSTATICS
struct msg_war_update_warstatics
{
	LONGLONG nWarID;				// 战场历史ID

	DWORD	dwMapID;				// 地图ID
	DWORD	dwBeginTime;            // 战场开始时间
	DWORD	dwEndTime;              // 战场结束时间
	WORD	wWarType;               // 战场类型
	WORD	wEndBecause;            // 结束原因 1、代表阵营1胜利 2、代表阵营2胜利 3、代表阵营1投降 4、代表阵营2投降
	WORD	wBanHeroID1[DBDEF_BAN_HERO_SIZE];		// 阵营1禁用英雄列表
	WORD	wBanHeroID2[DBDEF_BAN_HERO_SIZE];		// 阵营2禁用英雄列表

	DWORD	dwActorCount;			// 参与玩家数量
	// dwActorCount * DBREQ_PARAM_WAR_ACTOR_INFO

	msg_war_update_warstatics(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_WAR_GET_HISTORY_RECORD
struct msg_war_get_history_record
{
	DWORD actorID;          // 角色ID
	msg_war_get_history_record()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_WAR_SEND_HISTORY_RECORD
struct msg_war_send_history_record
{
	WORD        wWarType;                               // 战场类型
	bool        bIsFull;                                // 是否满人
	int         nHeroID;                                // 英雄ID
	WORD        wDieCount;                              // 死亡次数
	WORD        wAssistCount;                           // 助攻次数
	WORD        wKillPersonNum;                         // 击杀人物数量
	msg_war_send_history_record()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()
#endif	// __WAR_STATICMESSAGE_DEF_H__