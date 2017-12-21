/*******************************************************************
** 文件名:	NetMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/21/2014
** 版  本:	1.0
** 描  述:	

			定义游戏客户端和服务器通讯的协议
********************************************************************/

#include "Vector3.h"
#include "GameDef.h"
#include "PackagePtr.h"

#pragma once
#pragma pack(1)
///////////////////////// 所有消息的消息头 ////////////////////////
struct SNetMsgHead
{	
	BYTE		bySrcEndPoint;					// 源端点码	
	BYTE		byDestEndPoint;					// 目标端点码	
	BYTE		byKeyModule;					// 目标模块消息码	
	BYTE		byKeyAction;					// 行为消息码

	SNetMsgHead(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



/***************************************************************/
//////////////////////// 服务器层的全局定义 /////////////////////
/***************************************************************/
enum GAME_ENDPOINT
{ 	
	MSG_ENDPOINT_UNKNOW = 0,			// 未定义	
	MSG_ENDPOINT_SCENE,					// 场景服务器	    ［简称：S］
	MSG_ENDPOINT_GATEWAY,				// 网关服务器	    ［简称：G］
	MSG_ENDPOINT_LOGIN,					// 登录服务器	    ［简称：L］
	MSG_ENDPOINT_WORLD,					// 世界服务器	    ［简称：W］
	MSG_ENDPOINT_CLIENT,				// 客户端			［简称：C］
	MSG_ENDPOINT_CENTER,				// 中心服务器	    ［简称：E］
    MSG_ENDPOINT_VERIFY,				// 验证服务器	    ［简称：Y］
    MSG_ENDPOINT_SOCIAL,				// 社会服务器	    ［简称：O］
    MSG_ENDPOINT_GUARD,					// 数据监护服务器	［简称：G］
    MSG_ENDPOINT_BRIDGE,				// 跨区桥服务器		［简称：B］
    MSG_ENDPOINT_DBHTTP,				// DB应用服务器		［简称：D］
    MSG_ENDPOINT_PASSPORT,				// 通行证服务器		［简称：P］
    MSG_ENDPOINT_VOICE,					// 语音服务器	    ［简称：V］	
    MSG_ENDPOINT_VOICEGATE,				// 语音网关		    ［简称：VG］
    MSG_ENDPOINT_SNS,                   // 社交服务器       ［简称：T]
    MSG_ENDPOINT_CONTROL,               // 中控服务器       ［简称：R]
	MSG_ENDPOINT_VIEWMNG,				// 观战管理服		［简称：VWM]
	MSG_ENDPOINT_VIEW,					// 观战服			［简称：VW]
	MSG_ENDPOINT_VIEWGATE,				// 观战网关			［简称：VWG]

	MSG_SERVERID_MAXID					// 最大
};

/***************************************************************/
///////////////////// 目标模块层的全局定义 //////////////////////
/***************************************************************/
enum
{
	MSG_MODULEID_UNKNOW = 0,					// 未定义	
	MSG_MODULEID_GATEWAY,						// (S)网关模块	
	MSG_MODULEID_CENTERSERVER,					// (S)中心服务器模块	
	MSG_MODULEID_WORLDSERVER,					// (W)世界服务器模块
    MSG_MODULEID_ANTIADDICT,					// (W)防沉迷模块	
	MSG_MODULEID_LOGIN,							// (S)登录模块	
    MSG_MODULEID_PASSPORT,						// 通行证服务器模块
    MSG_MODULEID_VERIFY,						// 验证码模块	
    MSG_MODULEID_TEST,                          // 测试模块
    MSG_MODULEID_DBHTTP,						// DBHTTP模块
	MSG_MODULEID_GAME,							// 游戏模块
	MSG_MODULEID_ENTITY,                        // 实体
	MSG_MODULEID_CHAT,                          // 聊天
	MSG_MODULEID_COMMON,						// 公共模块
	MSG_MODULEID_ANTICHEATING,					// 反外挂模块
	MSG_MODULEID_EFFECT,						// 效果模块
	MSG_MODULEID_TANK,							// 载具
	MSG_MODULEID_MATCHSCENE,					// 比赛大厅场景服
	MSG_MODULEID_MATCHSERVER,					// 比赛大厅服 // 临时的，以后单独服务器
	MSG_MODULEID_WAR,							// 战场
	MSG_MODULEID_WAR_GOOD,						// 战场物品
	MSG_MODULEID_SLOT,							// 槽位模块
	MSG_MODULEID_BRIDGE,						// 跨区桥服务器模块
	MSG_MODULEID_GUARD,							// 数据监护模块
	MSG_MODULEID_SHARE,							// 共享模块
	MSG_MODULEID_VOICE,							// 语音服务器模块

	MSG_MODULEID_DRAGON,                        // 龙珠
    MSG_MODULEID_MIGRATE,                       // 迁移模块
	MSG_MODULEID_MATCH,							// 比赛大厅场景服

    MSG_MODULEID_ORDER,                         // 排行榜模块

    MSG_MODULEID_PATH,                          // 寻路动态障碍模块

    MSG_MODULEID_KIN,                           // 战队模块

	MSG_MODULEID_CLAN,                          // 帮会模块

    MSG_MODULEID_TASKSTORAGE,					// 集体任务数据	

    MSG_MODULEID_TEAM,                          // 组队模块
    
    MSG_MODULEID_LEGENDCUP,                     // 杯赛

    MSG_MODULEID_TASK,                          // 任务模块

	MSG_MODULEID_MENTORSHIP,					// 师徒模块

	MSG_MODULEID_SNS,							// 社交系统

    MSG_MODULEID_CONTROL,                       // 中控制模块
	
	MSG_MODULEID_STATICWAR,					    // 战场数据模块

	MSG_MODULEID_MAIL,							// 邮件模块

	MSG_MODULEID_VIEW,							// 观战模块

	MSG_MODULEID_MAXID,							// 最大
};

// 消息码定义键值
enum NET_MESSAGE_KEYACTION
{
	SC_MSG_FREEZE_START = (NETMSG_OFFSET+1),	// 冷却模块开始消息码
	SC_MSG_BUFF_START,							// BUFF模块开始消息码
	SC_MSG_SPELL_START,							// 技能模块开始消息码
	SC_MSG_DAMAGE_START,						// 伤害模块开始消息码
	SC_MSG_TANK_START,							// 载具模块开始消息码

	SC_MSG_ACTION_PLAYEFFECT,					// 播 光效或音效
	SC_MSG_WAR_START,							// 战场消息
	SC_MSG_SLOT_START,							// 槽位消息
	SC_MSG_CHAT_START,							// 聊天系统消息
    SC_MSG_MATCH_START,							// 撮合房间系统消息

    SC_MSG_KIN_START,							// 战队系统消息
    SC_MSG_CLAN_START,							// 帮会系统消息

    SC_MSG_TITLE_START,							// 称号系统消息

    SC_MSG_WARSTATIC_START,							// 战场消息

    SS_MSG_SYSTEM_START,                        // 服务器系统消息
};

// 系统消息码定义键值
enum GAME_SYSTEM_MESSAGE
{
    SS_MSG_SYSTEM_NEWDAY = SS_MSG_SYSTEM_START, // 新的一天
};

// 播 光效或音效	MSG_ACTION_PLAYEFFECT
struct msg_entity_action_play_effect
{
	DWORD		dwFlashID;		// 出生光效
	DWORD		dwSoundID;		// 出生音效

	msg_entity_action_play_effect(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


#pragma pack()

