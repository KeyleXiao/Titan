/*******************************************************************
** 文件名:	OssLogDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2016-7-4
** 版  本:	1.0
** 描  述:	日志结构体定义
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __COMMON_OSSLOGDEF_H__
#define __COMMON_OSSLOGDEF_H__

#include "IGameDatabaseDef.h"

#include <map>
using namespace std;

#pragma pack(1)

// 一次性人物数值最大存储多少条
#define OSS_PLAYER_NUMERIC_ITEM_SAVE_MAXCOUNT           20//120 TODO暂时修改存储上限，后续优化其他机制
// 一次性其他非人物数值最大存储多少条
#define OSS_OTHER_NUMERIC_ITEM_SAVE_MAXCOUNT            20//140
// 一次性点券最大存储多少条
#define OSS_TICKET_SAVE_MAXCOUNT                        20

// 绑定金钱达到多少时记录
#define OSS_BINDMONEY_RECORDLIMIT                       10000
// 数值最大值
#define OSS_NUMERIC_LIMIT_MAX                           100000000


// 玩家升级日记定时存盘间隔 s
#define  OSS_SAVE_ACTOR_UPGRADE_INTERVAL                5*60
// 玩家物品日记定时存盘间隔 s
#define  OSS_SAVE_ACTOR_GOODS_INTERVAL                  2*60
// 监控 任务定时存盘间隔 s
#define  OSS_SAVE_MONITOR_TASK_INTERVAL					5*60


// 数值类型
enum OSS_NUMERIC_TYPE
{
	///////////////////////////////////////////////////////////////////
	// 常用数值
    OSS_NUMERIC_BINDTICKET=1,	    // 金币
	OSS_NUMERIC_TICKET,				// 点券
    OSS_NUMERIC_EXP,				// 经验
    OSS_NUMERIC_HEROEXP,            // 英雄经验
    OSS_NUMERIC_MENTORSHIP_VALUE,   // 师徒值
	OSS_NUMERIC_SUPPORTPOINT,		// 点赞值
	OSS_NUMERIC_HERO_SCORE,	        // 英雄战绩分
	OSS_NUMERIC_MAP_ORDER,	        // 地图排位分

    //////////////////////////////////////////////////////////////////////////
    // TODO 这个区域中的项目正规化后要移除掉
	OSS_NUMERIC_MONEY,			    // 金钱
    OSS_NUMERIC_BINDMONEY,			// 绑定金钱
    OSS_NUMERIC_HEROLV,             // 英雄等级
	OSS_NUMERIC_HEROCARD,           // 英雄卡牌
	OSS_NUMERIC_HERO,				// 英雄
	OSS_NUMERIC_TALENT,				// 天赋
	OSS_NUMERIC_RUNE,				// 符文
	OSS_NUMERIC_CHEST,				// 宝箱
	OSS_NUMERIC_BENEFITCARD,		// 增益卡
	OSS_NUMERIC_HEROXPSKILL,		// 英雄xp技能
	OSS_NUMERIC_HEROSKIN,			// 英雄皮肤
    //////////////////////////////////////////////////////////////////////////
	OSS_NUMERIC_GEMSTONE,			// 宝石

	OSS_NUMERIC_MAX,
};

// 物品类型
enum OSS_GOODS_TYPE
{
    OSS_GOODS_UNKNOW,                   // 无效=0
    OSS_GOODS_CHEST,                    // 宝箱
    OSS_GOODS_TALENT,                   // 天赋
    OSS_GOODS_HEROXPSKILL,              // 英雄xp技能 必杀
    OSS_GOODS_HERO,                     // 英雄
    OSS_GOODS_HEROCARD,                 // 勋章
    OSS_GOODS_SKIN,                     // 皮肤
    OSS_GOODS_GEMSTONE,                 // 宝石
};

// 资源操作类型
enum OSS_RESOURCE_OPT_TYPE
{
	// 资源添加
	OSS_RESOURCE_OPT_ADD = 0,
	// 资源消耗
	OSS_RESOURCE_OPT_DEL,
	// 资源转进
	OSS_RESOURCE_OPT_TRANSIN,
    // 资源转出
    OSS_RESOURCE_OPT_TRANSOUT,
	// 最大值(无意义)
	OSS_RESOURCE_OPT_MAX,
};


// 资源添加途径 OSS_RESOURCE_OPT_ADD
enum OSS_RESOURCE_ADD_RANGE
{
	OSSRES_ADD_MIN = 1,

    OSSRES_ADD_WAR_PRIZE,               // 战场奖励
    OSSRES_ADD_GETEXPUPGRADE,           // 英雄获得经验升级
	OSSRES_ADD_PRIZECHEST,				// 开宝箱
	OSSRES_ADD_CLANDONATE,				// 帮会捐赠
	OSSRES_ADD_DRAGONBALL,				// 七龙珠
	OSSRES_ADD_POINTSHOP,				// 商城购买
    OSSRES_ADD_ACTORUPGRADE,            // 角色升级
    OSSRES_ADD_SEILSTONE,               // 出售宝石
    OSSRES_ADD_TASK,                    // 任务
    OSSRES_ADD_PRIZECHEST_TICK,         // 宝箱奖励重复换金币
    OSSRES_ADD_UPGRADE_RANK,            // 段位升级
    OSSRES_ADD_EVALUATE_GEMSTONE,       // 鉴定宝石
    OSSRES_ADD_UNLOCK_LEVEL,            // 角色等级解锁
    OSSRES_ADD_DBTICK_GOODS,            // 金币加倍道具产出
    OSSRES_ADD_DBEXP_GOODS,             // 经验加倍道具产出
    OSSRES_ADD_MENTORSHIP_INC_VAL,      // 师徒打比赛增加师徒值
    OSSRES_ADD_MENTORSHIP_TASK,         // 师徒任务
    OSSRES_ADD_HONOR_INC_VAL,           // 点赞增加点赞值
    OSSRES_ADD_COMPETITION_AWARD,       // 比赛系统获得奖励
	OSSRES_ADD_MAILPLUS,                // 邮件附件
	OSSRES_ADD_GLOD_FROM_TICKET,        // 点券兑换金币
	OSSRES_ADD_LIFEHERO,                // 本命英雄奖励
    OSSRES_ADD_RECOMMEND_PRIZE,         // 推广员获取奖励

	OSSRES_ADD_MAX = 6999,
};

// 资源消耗途径 OSS_RESOURCE_OPT_DEL
enum OSS_RESOURCE_DEL_RANGE
{
	OSSRES_DEL_MIN = 7000,

	OSSRES_DEL_POINTSHOP,		        // 商城购买消费
    OSSRES_DEL_HERO_UPGRADE_LEVEL,      // 英雄晋级消耗
	OSSRES_DEL_PRIZECHEST,		        // 开宝箱
	OSSRES_DEL_DRAGONBALL,		        // 七龙珠
    OSSRES_DEL_BUY_STONE_PAGE,          // 购买宝石页数
	OSSRES_DEL_OPENWINFSTONE,           // 宝石鉴定消耗
    OSSRES_DEL_LEGENDCUP_CREATE,		// 创建杯赛
    OSSRES_DEL_LEGENDCUP_CREATE_AWARD,	// 创建杯赛 玩家出资奖励
    OSSRES_DEL_LEGENDCUP_REQ_JOINCUP,	// 扣除申请比赛奖励
    OSSRES_DEL_REGISTE_COMPETITION,     // 比赛系统报名消耗
    OSSRES_DEL_TASK,	                // 任务
    OSSRES_DEL_CREATE_LEAGUES,          // 联盟创建消耗
    OSSRES_DEL_CREATE_FIGHT_TEAM,       // 战队创建消耗
    OSSRES_DEL_VOICE_GIFT,              // 语音送礼消费
    OSSRES_DEL_EXPIRE_HERO,             // 过期英雄
    OSSRES_DEL_EXPIRE_SKIN,             // 过期皮肤
    OSSRES_DEL_SALE_STONE,              // 宝石出售消耗
    OSSRES_DEL_SHOW_PRIZECHEST_STONE,   // 宝石开窗消费
    OSSRES_DEL_ACTOR_UPGRADE_LEVEL,     // 角色升级消耗
    OSSRES_DEL_HERO_UPGRADE_SKILL,      // 英雄熟练等级升级消耗
    OSSRES_DEL_KIN_RENAME,              // 战队重命名消耗

    OSSRES_DEL_TICKET_TO_GLOD,          // 消耗点券兑换成金币
    OSSRES_DEL_GLOD_EXCHAGE,            // 点券兑换成金币后的消耗金币

	OSSRES_DEL_ACTOR_RENAME,            // 玩家改名
	OSSRES_DEL_GEMSTONE_APPRAISAL,      // 宝石抛光
	OSSRES_DEL_MYSTERYSHOP_REFLUSH,     // 神秘商店刷新

	OSSRES_DEL_LIFEHERO_GOLD,		    // 替换本命英雄消耗
	OSSRES_DEL_LIFEHERO_CARD,		    // 替换本命英雄消耗

    //////////////////////////////////////////////////////////////////////////
    // TODO 这个区域中的项目正规化后要移除掉
    OSSRES_DEL_BUY_HEAO,                // 购买英雄消耗金钱
    OSSRES_DEL_UP_STAR_GOLD,            // 升星消耗金钱
    OSSRES_DEL_UP_STAR_CARD,            // 升星消耗卡牌
	OSSRES_DEL_CLANDONATE,		        // 帮会捐赠
    //////////////////////////////////////////////////////////////////////////
	OSSRES_DEL_MAX = 13999,
};

// 资源转进途径 OSS_RESOURCE_OPT_TRANSIN
enum OSS_RESOURCE_TRAN_IN_RANGE
{
	OSSRES_TRANIN_MIN = 14000,
    OSSRES_TRANIN_CLANDONATE,		        // 帮会捐赠

	OSSRES_TRANIN_MAX = 20999,
};


// 资源转出途径 OSS_RESOURCE_OPT_TRANSOUT
enum OSS_RESOURCE_TRAN_OUT_RANGE
{
    OSSRES_TRANOUT_MIN = 21000,


    OSSRES_TRANOUT_MAX = 28000,
};

// 文字型日志类型
enum OSS_NOTE_TRACK_TYPE
{
    OSS_NOTE_TRACK_NONE = 0,                // 无效ID
    OSS_NOTE_TRACK_PERSON = 1,			    // 人物，参数为PDBID
    OSS_NOTE_TRACK_CLAN,                    // 帮会，参数为ClanID
    OSS_NOTE_TRACK_KIN,                     // 战队，参数为KinID
    OSS_NOTE_TRACK_LEGENDCUP,               // 杯赛，参数为杯赛ID
	OSS_NOTE_TRACK_SYSTEM,				    // 全局，参数为0
	OSS_NOTE_TRACK_MENTORSHIP,				// 师徒，参数为学徒PDBID
    OSS_NOTE_TASK,				            // 任务, 参数为PDBID
	OSS_NOTE_TRACK_WAR,				        // 战场，参数为PDBID
	OSS_NOTE_TRACK_MAIL,					// 邮件，参数为0
	OSS_NOTE_TRACK_GM,						// GM，参数为PDBID
    OSS_NOTE_TRACK_RANK,                    // 排位分数, 参数位PDBID

    OSS_NOTE_TRACK_MAX,                     // 最大
};



// 操作者类型
enum OSS_OPT_TYPE
{
    OSS_OPT_TYPE_SYSTEM = 0,            	// 系统 + 0
    OSS_OPT_TYPE_PERSON,                    // 人物 + PDBID
    OSS_OPT_TYPE_MONSTER,                   // 怪物 + 怪物ID
    OSS_OPT_TYPE_TASK,                      // 任务 + 任务ID

    OSS_OPT_TYPE_MAX,
};


// 打造操作类别
enum OSS_GOOD_FORGE_TYPE
{
	OSS_GOOD_FORGE_NONE = 0,                // 无效ID
	OSS_GOOD_FORGE_GEMAPPRAISAL = 1,        // 宝石鉴定

	OSS_GOOD_FORGE_MAX,                     // 最大
};

// 数值告警类型
enum NUMERIC_ALARM_TYPE
{
    NUMERIC_ALARM_TYPE_NONE,
    NUMERIC_ALARM_TYPE_TICKET,		// 金币
    NUMERIC_ALARM_TYPE_MONEY,		// 银两
    NUMERIC_ALARM_TYPE_BINDMONEY,	// 绑银
    NUMERIC_ALARM_TYPE_EXP,	        // 经验

    NUMERIC_ALARM_TYPE__MAX,	    // 
};
//////////////////////////////////////////////////////////////////////////

#pragma pack()


#endif // __COMMON_OSSLOGDEF_H__