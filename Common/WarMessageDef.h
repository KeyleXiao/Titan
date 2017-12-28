/*******************************************************************
** 文件名:	WarMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/23/2015
** 版  本:	1.0
** 描  述:	War消息定义
********************************************************************/

#ifndef __WAR_MESSAGE_DEF_H__
#define __WAR_MESSAGE_DEF_H__

#include "NetMessageDef.h"
#include "War_ManagedDef.h"
#include "WarDef.h"

#pragma pack(1)
using namespace War;

/////////////////////////// War模块消息  ////////////////////////////
// 消息码定义键值
enum SC_WAR_NET_MESSAGE
{
    SC_NET_MSG_WAR = SC_MSG_WAR_START,		// 战场消息

    // 战场部件 WarPart
    SC_MSG_WAR_PERSONINFO_INITAL,				// 请求初始化
    SC_MSG_WAR_PERSONINFO,						// 玩家信息同步
    SC_MSG_WAR_MONSTER_TIME,					// 怪物时间
    SC_MSG_WAR_IMPORTANT_ENTITYDIE,				// 重要实体死亡
    SC_MSG_WAR_IMPORTANT_ENTITYRELIVE,			// 重要实体复活
    SC_MSG_WAR_IMPORTANT_MONSTERRELIVE,			// 重要怪物实体复活
    SC_MSG_WAR_IMPORTANT_MONSTERTEXTDIE,		// 重要怪物死亡文字广播
    SC_MSG_WAR_KILL_COUNT,						// 连杀广播
    SC_MSG_WAR_DIE_COUNT,						// 连死广播
    SC_MSG_WAR_END_INFO,						// 战斗结束信息
    SC_MSG_WAR_MINI_MAP_INFO,					// 小地图信息  场景服-客户端
    CS_MSG_WAR_MINI_MAP_INFO,					// 小地图信息   客户端-场景服	
    SC_MSG_WAR_MINI_MAP_ICON,					// 怪物小地图图标
    SC_MSG_WAR_CLEAR_ALL_MINI_MAP_ICON,			// 删除所有小地图图标信息
    SC_MSG_WAR_ENTITY_FIGHT_MINI_MAP_INFO,		// 战场实体战斗小地图信息
    SC_MSG_WAR_ENTITY_ALL_PLAYER_CREATED,   	// 战场全部玩家实体创建完毕下发
    CS_MSG_WAR_REQ_WAR_BASE_INFO,				// 请求战场基本信息
    SC_MSG_WAR_NOTIFY_WAR_INFO,					// 通知战场信息
    SC_MSG_WAR_IMPORTANT_MONSTERRELIVE_EARLY,	// 提早提醒玩家水晶等重要实体复活 
    CS_MSG_WAR_PLAYER_TABLE_REQUEST,			// 请求玩家战绩开关
	CS_MSG_WAR_ENTITY_SPECIAL_SELECT,			// 特殊实体选择 目前有祭坛
	CS_MSG_WAR_ENTITY_SPECIAL_SEARCH,			// 特殊实体范围搜索实体 目前有瞭望台
	SC_MSG_WAR_ENTITY_SPECIAL_SEARCH,   		// 特殊实体范围搜索状态改变通知客户端 目前有瞭望台
    //战场物品部件	MobaGoodsPart
    CS_MSG_WAR_BUY_GOOD,				// 请求购买物品
    CS_MSG_WAR_SELL_GOOD,				// 请求出售物品
    CS_MSG_WAR_EXCHANGE_GOOD,			// 请求交换物品
    CS_MSG_WAR_USE_GOOD,				// 请求交换物品

    SC_MSG_WAR_GOODS_LIST_TOSELF,		// 下发给自己的物品列表
    SC_MSG_WAR_GOODS_LIST,				// 下发装备列表
    CS_MSG_WAR_REVOCATION_OPERATION,	// 撤销操作
    SC_MSG_WAR_CAN_BUY_OR_SELL,			// 能否购买物品标示
    SC_MSG_WAR_CAN_CANCEL_OPREATION,	// 能否购买物品标示

    SC_MSG_WAR_EXIT_WAR,				// 退出战场

    SC_MSG_WAR_BROADCAST_SKILL_STATE,	// 广播技能状态

    SC_MSG_WAR_COMBA_KILL,				// 显示连杀特效

    SC_MSG_WAR_BACKROUND_MUSIC,			// 播放背景音乐
	
    SC_MSG_WAR_PROCESS_BROADCAST,		// 流程广播控制

	SC_MSG_WAR_XP_RANDOM_PLAYER,		// 随机冷却中的xp技能到客户端

    SC_MSG_WAR_UPDATE_PERSONTAB,		// 玩家TAB信息同步

	SC_MSG_WAR_SPECIALCONTRIBUTION,		// 玩家特殊贡献信息同步(惊现救治 夺命控制 团控)

	SC_MSG_WAR_FILTER_SUPPORT_PERSON,	// 筛选出到点赞平台条件的人
	CS_MSG_WAR_ADD_LABEL,				// 客户端添加标签操作
	SC_MSG_WAR_ADD_LABEL,				// 广播客户端添加标签操作

    CS_MSG_WAR_PICK_UP_ITEM,            // 捡东西（血包）

	// 装备管理
	CS_MSG_WAR_ADD_EQUIPSCHEME,			// 增加装备方案
	CS_MSG_WAR_DEL_EQUIPSCHEME,			// 删除装备方案
	CS_MSG_WAR_MODIFY_EQUIPSCHEME,		// 修改装备方案
	CS_MSG_WAR_READ_EQUIPSCHEME,		// 查询装备方案
	SC_MSG_WAR_EQUIPSCHEME_LIST,		// 装备方案列表

	// 天赋
	SC_MSG_WAR_ACTIVE_TALENT_LIST,		// 激活天赋操作

	// 挂机
	SC_MSG_WAR_BOTTING_NOTIFY,			// 通知客户端处于挂机状态
	CS_MSG_WAR_BOTTING_CANCEL,			// 挂机取消
	CS_MSG_WAR_BOTTING_STATE,			// 客户端上报客户端在挂机

    // 杯赛
	SC_MSG_WAR_END_LEGENDCUP_INFO,		// 战场结束杯赛信息

	// 大逃杀战场
	SC_MSG_WAR_SAFETYZONE_INFO,			// 安全区信息
	SC_MSG_WAR_SAFETYZONE_SPREAD_CD,	// 安全区扩散倒计时

    SC_MSG_WAR_PLAYER_RECONNECTION,     // 玩家掉线重连

	SC_MSG_WAR_SERVER_TOVIEW_MSG,		// 服务器直接发消息到C#层

	CS_MSG_WAR_REQ_EXIT_WAR,			// 玩家请求退出战场
};


struct msg_war_end_legendCup_info
{
    BYTE    eRoundType;                   // 本轮类型 淘汰 or 小组赛 ERoundCompetitionType
    bool    bRoundLastNode;               // 是否是本轮最后一轮
    bool    bIsOnlineCup;
    bool    bIsLastRound;
    bool    bNodeEnd;
    bool    bWin;
    DWORD   dwNextStartInterval;
    DWORD   dwNextRoundStartTime;

    msg_war_end_legendCup_info(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_war_update_persontab
{
    // 数据流的方式进行传输
    PDBID uPDBID;	// 人物PDBID
    UID uID;		// 玩家ID
    int nKillNum;	// 玩家击杀数
    int nDeadNum;	// 死亡数
    int nLevel;		// 等级
    int nContKill;	// 连杀
    int nAssists;	// 助攻
    int	nKillMonsterNum; //怪物击杀数量
    bool bIsOnGame; // 是否在游戏中
    int nDestroyTower; // 推塔数

    msg_war_update_persontab(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_war_person_info
{
    // 数据流的方式进行传输
    PDBID uPDBID;	// 人物PDBID
    int nCamp;		// 阵营信息
    int nIndex;		// 位置
    UID uID;		// 玩家ID
    int nKillNum;	// 玩家击杀数
    int nDeadNum;	// 死亡数
    int nHeroID;	// 英雄ID
	int nSkinID;	// 英雄皮肤ID
    int nLevel;		// 等级
    int nEXP;		// 经验
    int nContKill;	// 连杀
    int nAssists;	// 助攻
    int	nKillMonsterNum; //怪物击杀数量
    bool bIsOnGame; // 是否在游戏中
    char gcPlayerName[GAME_NAME_MAXSIZE];//玩家名字
	int  nSummerID; // 召唤师技能id 
	bool bIsAI;		// 是否AI
	int nAliveContAssist;// 不死连助攻
	int bySex;	// 性别
	int byPlayerType;// 玩家类型（EPlayerType）

    msg_war_person_info(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_PERSONINFO 玩家信息同步
struct msg_war_person_info_list
{
    // 玩家个数
    int nNum;
    // nNum * msg_war_person_info
    msg_war_person_info_list(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_MONSTER_TIME 怪物时间
struct msg_war_monster_time
{
    // 怪物小地图索引
    int nIconIndex;

    // 怪物类型
    int nType;
    // 倒计时
    int nTime;
    // 战绩表位置
    int nTableIndex;

    msg_war_monster_time(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_war_important_entiy_dieinfo
{
    bool	isMonsterKill;
    bool	bIsFirstBlood;	                    //	首杀
    bool	bIsCampSkill;	                    //	是否是阵营击杀	
    int		nCamp;			                    //  被哪方击杀
    int		MasterEntityType;	                //  死亡实体类型
    int		MasterHeroID;
    char	MasterName[GAME_NAME_MAXSIZE];      //  被击者名称
    int     MasterDieKillCount;                 //  被杀者之前不死连杀次数
	UID		uidMaster;							//	被杀者uid					

    int		MurdererEntityType;	                //  击杀实体类型
    int		MurdererHeroID;
    char	MurdererName[GAME_NAME_MAXSIZE];    //  击杀者名称
    int		nKillCount;	                        //	死亡连杀数量
    int		nBroadCastIndex;	                // 广播索引
    bool    isAce;				                // 是否团灭
    bool	bIsOver;			                // 是否被终结
	int		nAddTalentPoint;					// 终结和第一滴血有这个数据
	UID		uidMurderer;						//	击杀者uid		

	int		nAssistCount;						// 助攻者数目
	
    msg_war_important_entiy_dieinfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_war_important_monster_reliveinfo
{
    int		nCamp;				//  复活实体阵营
    int		MasterEntityType;	//  复活实体类型
    int		MasterHeroID;
    int		nBroadCastIndex;	// 广播索引

    msg_war_important_monster_reliveinfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_war_important_monster_dietxtinfo
{
    int		nCamp;				//  死亡实体阵营
    int		MasterEntityType;	//  死亡实体类型
    int		MasterHeroID;
    int		nBroadCastIndex;	// 广播索引

    msg_war_important_monster_dietxtinfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 助攻者信息
struct msg_war_entity_assist_info
{
	int nEntityType;		// 助攻者实体类型
	UID uidAssist;			// 助攻者id
	int nHeroID;			// 英雄ID
	char szName[GAME_NAME_MAXSIZE];	// 助攻者名字

	msg_war_entity_assist_info(void)
	{
        memset(this, 0, sizeof(*this));
	}
};

struct msg_war_entity_kill_countinfo
{
    bool	isMonsterKill;

    int		MasterEntityType;	            //  死亡实体类型
    int		MasterHeroID;
    char	MasterName[GAME_NAME_MAXSIZE];  //  被杀者名称
    int     MasterDieKillCount;             // 被杀者之前的连杀次数
	UID		uidMaster;						//	被杀者uid		

    int		MurdererEntityType;	            //  死亡实体类型
    int		MurdererHeroID;
    int		MurdererCamp;
    char	MurdererName[GAME_NAME_MAXSIZE];//  击杀者名称
	UID		uidMurderer;					//	击杀者uid		
    int		nTimeKillCount;                 // 战斗中连续击杀
    bool    bIsAce;			                // 是否团灭

	int		nAssistCount;					// 助攻者数量

    msg_war_entity_kill_countinfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_END_INFO
struct msg_war_entity_end_warinfo
{
    // 玩家pdbid
    PDBID pDbid;
    // 玩家所在阵营
    int nCamp;

    // 玩家序号位
    int nIndex;

    // 玩家ID
    UID uID;

    // 英雄ID
    int nHeroID;

    // 玩家名称
    char PlayerName[GAME_NAME_MAXSIZE];//  玩家名称

    // 玩家等级
    int nLevel;

    // 经验
    int nExperien;


    // 战斗勋章
    WORD  WarMedal;

    // 天赋技能1
    int nTalentSkill1;
    // 天赋技能2
    int nTalentSkill2;

    // 所有数据信息
    int gzAllData[ERDT_MAX];

    // mvp分数
    int nScore;

    // 所有称号
    int nTitle;

    // 一场比赛的天梯分数
    int nMatchTypeScore;

    // 玩家的天梯分数
    int nPlayerMatchScore;

	// 以前的段位分
	int nPreMatchTypeScore;

	// 以前的段位等级
	int nPreMatchTypeGrade;

    // 天梯名称
    char szPlayerMatchName[GAME_NAME_MAXSIZE];

    // 比赛类型
    int nMatchType;
	
	// 玩家类型
	int nPlayerType;
	
    msg_war_entity_end_warinfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_war_entity_end_warinfo_list
{
	// DBWarID
    LONGLONG dwDBWarID;
	// 失败阵营
    int nWinCamp;
    // 使用时间
    unsigned long uUseTime; 
	// 失败方基地坐标位置(该值为扩大了ACTOR_LOCATION_SCALE倍的数值)
	int xBasePos, yBasePos, zBasePos;
    // 玩家数量
    int nPlayerCount;

    // nPlayerCount*sizeof(msg_war_entity_end_warinfo)

    msg_war_entity_end_warinfo_list(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_war_entity_minimap_info
{
    // 玩家名称
    UID uPlayerUID;//  玩家名称

    // 小地图消息类型
    int nType;

    // 消息坐标点
    float vec[MAX_WAR_MINIMAP_DEFINE];

	// 消息目标ID
	int nEntityID;

    msg_war_entity_minimap_info(void)
    {
        memset(this, 0, sizeof(*this));
    }
};
struct msg_war_entity_update_table_request
{

    UID uPlayerUID;
    bool bIsOpen;


    msg_war_entity_update_table_request(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// SC_MSG_WAR_MINI_MAP_ICON 怪物时间
struct msg_war_entity_minimap_icon
{
    bool bIsShow;
    // 怪物小地图索引
    int nIconIndex;
    // 地图ID
    int nMapID;

    msg_war_entity_minimap_icon(void)
    {
        memset(this, 0, sizeof(*this));
    }
};



//SC_MSG_WAR_IMPORTANT_ENTITYRELIVE
struct msg_war_important_entiy_reliveinfo
{
    DWORD	dwRevliveTime;		//  复活时间:毫秒
    UID		uReliveUID;			//	复活人物UID
	PDBID   uRelivePDBID;		//  复活人物PUBID
    msg_war_important_entiy_reliveinfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct msg_dragon_show_position
{
    Vector3	Position;		//  显示位置
    UID		uUID;			//	显示人物UID

    msg_dragon_show_position(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//SC_MSG_WAR_CLEAR_ALL_MINI_MAP_ICON
struct msg_clear_all_mini_map_icon
{
    bool isClearAll;

    msg_clear_all_mini_map_icon(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//SC_MSG_WAR_ENTITY_FIGHT_MINI_MAP_INFO
struct msg_entity_fight_mini_map_info
{
    int nInfoType;
    DWORD dwSrcID;

    msg_entity_fight_mini_map_info(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//CS_MSG_WAR_BUY_GOOD,				// 请求购买物品
struct msg_entity_fight_buy_good
{
    int nGoodID;

    msg_entity_fight_buy_good(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


//CS_MSG_WAR_SELL_GOOD,				// 请求出售物品
struct msg_entity_fight_sell_good
{
    int nGoodID;
    int nSlotIndex;

    msg_entity_fight_sell_good(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


//CS_MSG_WAR_EXCHANGE_GOOD,			// 请求交换物品位置
struct msg_entity_fight_exchange_good
{
    int nSrcSlot;
    int nTargetSlot;

    msg_entity_fight_exchange_good(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//CS_MSG_WAR_USE_GOOD,			// 使用物品
struct msg_entity_fight_use_good
{
    int nSlotIndex;

    msg_entity_fight_use_good(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


//SC_MSG_WAR_GOODS_LIST,			// 下发装备列表
struct msg_entity_fight_good_list
{
    int nCount;
    PDBID idActor;
    //nCount*msg_entity_fight_good
    msg_entity_fight_good_list(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_BROADCAST_SKILL_STATE
struct msg_player_skill_state
{
    int		nID;					// 技能ID
    UID		uOperator;				// 操作者
    int		nCoolTime;				// 冷却时间
    msg_player_skill_state()
    {
        memset( this, 0, sizeof(*this) );
    }
};

struct msg_entity_fight_good
{
    UID	uUID;
    int nSlotIndex;
    SLOT_TYPE nSlotType;    // SLOT_TYPE
    int nGoodID;
    bool bCanUse;
    int nCanUseTimes;
    int nFreezeID;
    int nSkillID;
    int nBuffID;
    bool bIsNewAdd;

    msg_entity_fight_good(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//CS_MSG_WAR_REVOCATION_OPERATION
struct msg_entity_revocation_operation
{
    UID	uUID;

    msg_entity_revocation_operation(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//SC_MSG_WAR_CAN_BUY_OR_SELL
struct msg_entity_can_buy_or_sell
{
    bool bCanOrSell;

    msg_entity_can_buy_or_sell(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//SC_MSG_WAR_CAN_CANCEL_OPREATION
struct msg_entity_can_cancel_opration
{
    bool bCanCancelOpration;

    msg_entity_can_cancel_opration(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_BACKROUND_MUSIC 玩家信息同步
struct msg_war_backround_music
{
    // 音效ID
    int nSoundID;

    msg_war_backround_music(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_ENTITY_ALL_PLAYER_CREATED 所有玩家全部进入战场后uid和pbuid下发
struct msg_war_all_players_created
{

    UID	uUID;        
    UID	uPDBID;
    int nCamp;
    msg_war_all_players_created(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_PROCESS_BROADCAST 流程广播
struct msg_war_process_broadcast
{
    // 广播ID
    int nBroadCastID;

    msg_war_process_broadcast(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// CS_MSG_WAR_ENTITY_SPECIAL_SELECT 特殊实体选取 目前有玩家选取祭坛
struct msg_war_select_special_entity
{
	// 玩家名称
	UID uidOperator;	 //  操作者
	UID uidTarget;		 //  被选者
	int nFunctionType;   //  作用类型 ESpecial_Select_Entity_Function_Type

	msg_war_select_special_entity(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_WAR_ENTITY_SPECIAL_SEARCH 特殊实体范围搜寻 目前有瞭望台搜寻玩家
struct msg_war_special_entity_range_search
{
	UID uidOperator;	 //  操作者
	UID uidTarget;		 //  被选者
	int nFunctionType;   //  作用类型
	bool bSearchOrBreak; //  开始搜索到目标或者而打断目标 ESpecial_Entity_Range_Search_Function_Type

	msg_war_special_entity_range_search(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//SC_MSG_WAR_ENTITY_SPECIAL_SEARCH 特殊实体范围搜索状态改变通知客户端 目前有瞭望台
struct msg_war_special_entity_search_state_change
{
	UID uidOperator;	 //  操作者
	bool bSearch;        //  开始或者停止搜索

	msg_war_special_entity_search_state_change(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_WAR_SPECIALCONTRIBUTION 玩家特殊贡献信息同步(惊现救治 夺命控制 团控)
struct msg_war_special_contribution
{
	ESpecialContributionType eType;
	int		nOperatorHeroID;
	int		nParam1;
	char	szOperatorName[GAME_NAME_MAXSIZE];
	msg_war_special_contribution(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_WAR_FILTER_All_SUPPORT_PERSON 筛选出到点赞平台条件的人
struct msg_war_filter_support_person
{
	PDBID uPDBIU;
	int nVoteID;		// 点赞评选类型
	msg_war_filter_support_person(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 添加标签
struct msg_war_add_label
{
	PDBID targetPdbid;
	int nLabelID;
	char szOperatorName[ACTORNAME_MAXSIZE];
	char szTargetName[ACTORNAME_MAXSIZE];

	msg_war_add_label(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct msg_pick_up_item
{
    int itemID; 

    msg_pick_up_item(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_EQUIPSCHEME_LIST		某个英雄的装备方案列表
struct msg_war_equipscheme_list
{
	PDBID pdbid;		// 玩家ID
	int nHeroID;		// 英雄ID
	int nSchemeCount;	// 装备方案列表

    msg_war_equipscheme_list(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// CS_MSG_WAR_ADD_EQUIPSCHEME		// 添加装备方案
struct msg_war_add_equipscheme
{
	int nHeroID;										// 英雄名字
	char szSchemeName[GAME_NAME_MAXSIZE];				// 装备方案名字
	char szAuthorName[ACTORNAME_MAXSIZE];				// 装备方案作者名字
	char szSchemeTitle[GAME_NAME_MAXSIZE];				// 装备方案称号
	int nEquipIDList[WARQUIPSCHEME_EQUIP_MAXSIZE];		// 装备权重信息

	msg_war_add_equipscheme(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// CS_MSG_WAR_MODIFY_EQUIPSCHEME	// 修改装备方案
struct msg_war_modify_equipscheme
{
	int nHeroID;										// 英雄名字
	BYTE bySchemeID;									// 装备方案ID
	char szSchemeName[GAME_NAME_MAXSIZE];				// 装备方案名字
	char szAuthorName[ACTORNAME_MAXSIZE];				// 装备方案作者名字
	char szSchemeTitle[GAME_NAME_MAXSIZE];				// 装备方案称号
	int nEquipIDList[WARQUIPSCHEME_EQUIP_MAXSIZE];		// 装备权重信息

	msg_war_modify_equipscheme(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// CS_MSG_WAR_DEL_EQUIPSCHEME		// 删除装备方案
struct msg_war_del_equipscheme
{
	int nHeroID;										// 英雄名字
	BYTE bySchemeID;									// 装备方案ID

	msg_war_del_equipscheme(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// CS_MSG_WAR_READ_EQUIPSCHEME		// 读取装备方案
struct msg_war_read_equipscheme
{
	int nHeroID;										// 英雄名字

	msg_war_read_equipscheme(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// SC_MSG_WAR_ACTIVE_TALENT_LIST
struct msg_war_active_talent_list
{
	int pdbid;
	int nCurTalent;		// 当前激活的天赋
	int nTalentList[MAX_ACTIVE_TALENT_COUNT];

	msg_war_active_talent_list(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_WAR_BOTTING_NOTIFY		// 挂机提醒
struct msg_war_botting_notify
{
	int byType;			// 挂机类型（EWBottingNotifyType）

	msg_war_botting_notify(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_WAR_RSP_WAR_INFO
struct msg_war_rsp_war_info
{
	int nWarStartTick;			// 战场开始时刻（ms)
	LONGLONG lDBWarID;			// 战场数据库id
	int nConfigWarID;			// 战场配置表id
	bool isFull;				// 是否满人
	int nMatchType;				// 匹配类型

	msg_war_rsp_war_info(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// SC_MSG_WAR_SAFETYZONE_INFO			// 安全区信息
struct msg_war_safetyzone_info
{
	int nCenterOfNoxZoneX;		// 当前毒气圈的圆心坐标X
	int nCenterOfNoxZoneY;		// 当前毒气圈的圆心坐标Y
	int nCenterOfNoxZoneZ;		// 当前毒气圈的圆心坐标Z
	int nRadiusOfNoxZone;		// 当前毒气圈的半径
	int nCenterOfSafetyZoneX;	// 当前安全圈圆心坐标
	int nCenterOfSafetyZoneY;	// 当前安全圈圆心坐标
	int nCenterOfSafetyZoneZ;	// 当前安全圈圆心坐标
	int nRadiusOfSafetyZone;	// 当前安全圈的半径
	int nSpreadBeginTick;		// 扩散开始的时间（时间服务器Tick数）
	int nSpreadTime;			// 扩散总时间

	msg_war_safetyzone_info(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_WAR_PLAYER_RECONNECTION
struct msg_war_player_reconnection
{
	int actorID;
	int heroID;
	char actorName[GAME_NAME_MAXSIZE];

	msg_war_player_reconnection(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_WAR_SAFETYZONE_SPREAD_CD    // 扩散倒计时开始
struct msg_war_safetyzone_spread_cd
{
	int nSpreadCDTick;			// 扩散倒计时开始时间（时间服务器Tick数）
	int nSpreadCDTime;			// 扩散倒计时总时间

	msg_war_safetyzone_spread_cd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_WAR_SERVER_TOVIEW_MSG  服务器直接发消息到C#层
struct msg_war_server_toview_msg
{
	int nKeyAction;

	msg_war_server_toview_msg(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
#pragma pack()
#endif	// __BUFF_MESSAGE_DEF_H__