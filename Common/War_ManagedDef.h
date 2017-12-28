/*******************************************************************
** 文件名:	War_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/3/2015
** 版  本:	1.0
** 描  述:	战场相关托管公共定义

			
********************************************************************/

#ifndef __COMMON_WAR_MANAGED_DEF_H__
#define __COMMON_WAR_MANAGED_DEF_H__


#include "GameDef.h"
#include "ManagedCode.h"

namespace  War
{

		//////////////////////////////////////////////////////////////////////////
		/*						C++和C#公用数据结构								*/
		//////////////////////////////////////////////////////////////////////////

		// 战场宏定义
		#define GENIC_MAX_SIZE 3
		ENUM EWarDefine
		{
			EGENIC_MAX_SIZE = 3,			// 收集基因最大数
		};

        // 战场勋章 场景服和客户端都会用到的数据
        ENUM EWarMedal
        {
            EWM_NONE            = 0x0000,
            EWM_OUTPUT          = 0x0001,     // 最高总输出（对英雄的）
            EWM_KILLCOUNT       = 0x0002,     // 杀人数最高
            EWM_ASSCOUNT        = 0x0004,     // 助攻数最多
            EWM_DAMAGE          = 0x0008,     // 承受的伤害最高

            EWM_DESTORYTOWER    = 0X0010,     // 推塔数最多
            EWM_MONEY           = 0X0020,     // 总的金币最多
            EWM_KILLMONSTER     = 0X0040,     // 击杀怪物数最多
			EWM_CURE			= 0X0080,	  // 治疗

			EWM_THRILLSAVE		= 0X0100,	  // 惊险救治
			EWM_DEADLYCONTROL	= 0X0200,	  // 夺命控制
			EWM_TEAMCONTROL		= 0X0400,	  // 团控
            EWM_SCORE           = 0x0800,     // 综合分数最高即mvp

            EWM_KILLDRAGON		= 0x1000,     // 杀龙最多
            EWM_JOINBATTLE		= 0x2000,     // 参战率最高
			EWM_MVPOFLOSER		= 0x4000,	  // 失败方mvp
        };

		// 战场地图信息累类型
		ENUM EWarMiniMapTypeInfo
		{
			EWMMI_NONE			= 0,	
			EWMMI_ATTACK,					// 进攻
			EWMMI_RETREAT,					// 撤退
			EWMMI_DISAPPEAR,				// 敌人消失
			EWMMI_SUPPORT,					// 正在支援
		};


		// 战场地图信息累类型
		ENUM EPLAYER_INDEX
		{
			PLAYER_INDEX_MAX			= 9,
		};

        // 战场结束后战绩表显示类型
        ENUM ERecordTable_Type
        {
            ERTT_HERO = 1,          // 英雄总览
            ERTT_GRIDDING,          // 网格浏览
            ERTT_GRAPH,             // 图标浏览

            ERTT_MAX,
        };


        // 所有类型的数据
		// 注意！！！！！！！！！ERecord_Data_Type这个有添加 这里必须要改
		//////////////////////////////////////////////////////////////////////////
        #define ERDT_MAXR			        30	// ERDT_MAX
		ENUM ERecord_Data_Type
		{
				// 战斗统计
				ERDT_KILLNUM = 0,               // 击杀的数量
				ERDT_DEADNUM,                   // 死亡的数量
				ERDT_ASSIST,                    // 助攻
				ERDT_SCORE,						// 综合评分
				ERDT_GPM,						// 每分钟金钱量，金钱除以战斗时长
				ERDT_FIRSTBLOOD,				// 是否第一滴血
				ERDT_DEADLYCONTROL,             // 决胜控制

				// 团队贡献
				ERDT_MONEYRATE,					// 金钱比（百分比，数值乘100）
				ERDT_OUTPUTRATE,                // 输出率（百分比，数值乘100）
				ERDT_JOINBATTLERATE,            // 参战率（百分比，数值乘100）
				ERDT_OUTPUT,                    // 总输出伤害
				ERDT_OUTDAMAGEPHYSICS,          // 物理输出伤害
				ERDT_OUTDAMAGEMAGIC,            // 魔法输出伤害
				ERDT_OUTDAMAGETRUE,             // 真实输出伤害
				ERDT_BEARDAMAGE,                // 承受总伤害
				ERDT_BEARPHYSIC,                // 承受的物理伤害
				ERDT_BEARMAGIC,                 // 承受的魔法伤害
				ERDT_BEARTRUE,                  // 承受的真实伤害
				ERDT_MONEY,                     // 金钱量
				ERDT_CURE,                      // 治疗
				ERDT_RECOVERYHP,                // 恢复的生命值

				// 精彩表现
				ERDT_TIMEKILLMAX,               // 最高连杀
				ERDT_ALIVECONTKILLMAX,          // 最高多杀
				ERDT_ALIVECOUTASSISTMAX,        // 最高连续助攻
				ERDT_THRILLSAVE,                // 惊险救治
				ERDT_CRILDAMAGEMAX,             // 最高暴击伤害
				ERDT_TOWER,						// 破塔
				ERDT_DRAGEN,					// 击杀小龙

				ERDT_MAX1,						// 以上为战后统计界面统一显示的数据

				// 其他数据
				ERDT_KILLMONSTER,				// 击杀怪物数

				ERDT_MAX
		};
        //////////////////////////////修改结构体必须要改 ERDT_MAXR////////////////////////////

        ENUM EWarCampType
        {
            DESC("己方阵营")
            EWCT_SELFCAMP  = 0x0001,
            DESC("敌方阵营")
            EWCT_ENIMICAMP = 0x0002,
            DESC("全部阵营")
            EWCT_ALL       = 0x0003,
        };

        // 怪物处理方式
        ENUM EWarMonsterExec
        {
            DESC("无(取英雄)")
            EWME_None       = 0x0000,		// 无(取英雄)
            DESC("基地")
            EWME_Base       = 0x0001,		// 基地
            DESC("水晶")
            EWME_Crystal    = 0x0002,		// 水晶
            DESC("塔")
            EWME_Tower      = 0x0004,		// 塔
            DESC("士兵")
            EWME_Soldier    = 0x0008,		// 士兵
            DESC("野怪")
            EWME_Monster    = 0x0010,		// 野怪
            DESC("策略野怪")
            EWME_BOSS       = 0x0020,		// 策略野怪
            EWME_MAX        = 0x00FF,
		};

        // 怪物处理方式
        ENUM EWarMonsterRefreshType
        {
            EWMRT_None = 0,		// 不存在的处理方式
            EWMRT_Normal,		// 正常刷兵
            EWMRT_Random,		// 随机方式
            EWMRT_Interval,		// 时段方式
            EWMRT_RandomInterval,// 时段随机方式
            EWMRT_PosAndMonsterRandom,    // 位置随机+ 怪物随机
            EWMRT_MAX,
        };

	    //最大槽位
	    #define MAX_GOODS_SLOT_COUNT		7

		// 最大可激活天赋
		#define MAX_ACTIVE_TALENT_COUNT		6

	    //特殊槽位（只能放特殊槽位物品）
	    #define SPECIAL_SLOT_INDEX			MAX_GOODS_SLOT_COUNT - 1

        // 自动使用物品的槽位
        #define GOODS_AUTOUSE_SLOT		-1

        // 脚本槽位定义相关
        ENUM SLOT_DEFINE
        {
            D_MAX_GOODS_SLOT_COUNT    = MAX_GOODS_SLOT_COUNT,
            D_SPECIAL_SLOT_INDEX      = SPECIAL_SLOT_INDEX,
            D_GOODS_AUTOUSE_SLOT      = GOODS_AUTOUSE_SLOT,
        };

	    // 最大宝箱ID
	    #define PRIZECHEST_TYPE_MAX		100

		// 宝石宝箱ID
		#define PRIZECHEST_TYPE_GEMSTONE 7
	    // 单次奖励最大数量
	    #define PRIZEGROUP_RESULT_MAX	10

		// 奖励类型定义
		ENUM EMActorPrizeType
		{
			ACTOR_PRIZE_ACTOREXP = 1,		// 召唤师经验
			ACTOR_PRIZE_GOLD,				// 金币
			ACTOR_PRIZE_HERO,				// 英雄
			ACTOR_PRIZE_RUNE,				// 符文
			ACTOR_PRIZE_HEROCARD,			// 英雄卡牌
			ACTOR_PRIZE_HEROCARD_RANDOM,	// 随机英雄卡牌
			ACTOR_PRIZE_TALENT,				// 天赋
			ACTOR_PRIZE_CHEST,				// 宝箱
			ACTOR_PRIZE_SPECAIL,			// 特殊奖励 例如改名卡,刷新卡牌界面
			ACTOR_PRIZE_BENEFITCARD,		// 增益卡	
			ACTOR_PRIZE_HEROXPSKILL,		// 英雄xp技能 	
			ACTOR_PRIZE_HEROSKIN,			// 英雄皮肤 
			ACTOR_PRIZE_GEMSTONE,			// 宝石
			ACTOR_PRIZE_GEMSTONEPAGE,		// 水晶模组
			ACTOR_PRIZE_TASKSEARCH,			// 任务搜索次数
			ACTOR_PRIZE_EMOJI,				// 表情包
		};
		// 士兵气势（三路）类型
		ENUM EWarSoliderPathType
		{
			EWSPT_INVALID = 0,						    // 错误士兵路径类型
			EWSPT_TOP = 1,								// 上路士兵路径类型
			EWSPT_MID = 2,								// 上路士兵路径类型
			EWSPT_BOT = 4,								// 上路士兵路径类型

			EWSPT_MAX = EWSPT_TOP|EWSPT_BOT | EWSPT_MID , // 全部路士兵路径类型
		};

        // 战场结束类型
        ENUM EWarEndReason
        {
                CAMP_WIN_INVALID = -1,
                // 阵营1胜利
                CAMP_WIN_BLUE = 1,
                // 阵营2胜利
                CAMP_WIN_RED,
                // 阵营1投降
                CAMP_SURRENDER_BLUE,
                // 阵营2投降
                CAMP_SURRENDER_RED,
                // 无人自动结束
                CAMP_NOPERSON_END,
                // 非正常关闭
                CAMP_NOT_NORMAL_END,

                CAMP_WIN_MAX
        };

        // 快速入口模式
        ENUM EWarModeDef
        {
                // 快速游戏
                MODE_SpeedVS = 1,
                // 对战电脑
                MODE_MachineVS,
                // 排位比赛
                MODE_RankVS,
                // 自定比赛
                MODE_CustomerVS,
                // 生存模式
                MODE_SurviveVS,
                MODE_MAX
        };

        ENUM EWarCheckBuyGoodErrorCode
        {
            DESC("没有错误")
            EBEC_CheckNoneError= 0,
            DESC("实体错误")
            EBEC_CheckEntityEerror,
            DESC("配置错误")
            EBEC_CheckConfigError,
            DESC("最大持有")
            EBEC_CheckHoldMaxCount,
            DESC("战场不支持")
            EBEC_CheckThisWarCantBuy,
            DESC("无法购买状态")
            EBEC_CheckCantBuyState,
            DESC("无法出售状态")
            EBEC_CheckCantSellState,
            DESC("不够天赋点")
            EBEC_CheckNotEnoughMoney,
            DESC("格子已满")
            EBEC_CheckIsSlotFull,
            DESC("物品限制购买")
            EBEC_CheckGoodBuyLimit,
            DESC("购买冷却中")
            EBEC_CheckGoodBuyFreeze,
        };

        ENUM EGetSingleWarType
        {
            EGSWT_PlayerRecord = 1,         // 玩家战绩
            EGSWT_KinRecord,                // 战队战绩
            EGSWT_Max,
        };

		// 装备方案ID
		ENUM EWEquipSchemeType
		{
			EWEST_None = 0,		// 空 
			EWEST_OftenUse,		// 常用方案
			EWEST_Diy1,			// 自定义方案一
			EWEST_Diy2,			// 自定义方案二
			EWEST_Diy3,			// 自定义方案三
			EWEST_Diy4,			// 自定义方案四

			EWEST_Max
		};

		// 挂机通知类型
		ENUM EWBottingNotifyType
		{
			EWBNT_Normal,		// 普通
			EWBNT_Punish,		// 已经受到惩罚
		};

		// 玩家生命状态
		ENUM ELifeState
		{
			ELS_Alive,		// 存活
			ELS_Dead,		// 死亡
			ELS_FakeDead,   // 假死
		};

        // 装备标识
        ENUM MOBA_GOODS_FLAG
        {
            GOODS_FLAG_NPC_PURCHASE = 1 << 0,     // NPC处购买
        };

		// 收集的基因数据信息
		STRUCT SGenicInfo
		{
			int nHeroID;
			int nSkinID;

			STRUCT_INIT(SGenicInfo);
		};

		// 战场玩家数据更新类型
		ENUM EWarPersonInfoUpdateType
		{
			EWPTUT_VOCATION,			// 职业
		};

		////////////////////////////////////////逻辑层到C#显示层消息ID/////////////////////////////////////////
		#define EServerWar2ViewMsg_Start 0
		ENUM EServerWar2ViewMsg
		{
			WAR2VIEW_MSG_BATTLEWAR_LIFESTATE = EServerWar2ViewMsg_Start,	// 大逃杀战场-任务生命状态
			WAR2VIEW_MSG_BATTLEWAR_ACE,										// 大逃杀战场-阵营团灭或胜利
			WAR2VIEW_MSG_BATTLEWAR_GENIC_UPDATE,							// 大逃杀战场-收集基因数据更新
			WAR2VIEW_MSG_BATTLEWAR_DEVILKING_UPDATE,						// 大逃杀战场-魔王更新
			WAR2VIEW_MSG_WAR_PERSON_INFO_UPDATE,							// 战场-玩家信息更新
			WAR2VIEW_MSG_WAR_XP_BEGIN_CD,									// 战场-玩家获得XP技能开始CD
		};



		////////////////////////////////////////逻辑层到C#显示层消息结构体/////////////////////////////////////////
		// WAR2VIEW_MSG_BATTLEWAR_LIFESTATE   大逃杀战场任务生命状态
		STRUCT SWar2ViewMsgPersonLifeState
		{
		public:
			int pdbid;
			int nCamp;
			int nLifeSate;
			
			STRUCT_INIT(SWar2ViewMsgPersonLifeState);
		};

		// WAR2VIEW_MSG_BATTLEWAR_ACE   大逃杀战场某个阵营团灭
		STRUCT SWar2ViewMsgAce
		{
		public:
			int nCamp;
			int nRank;
			int nKillNum;
			BYTE isWin;

			STRUCT_INIT(SWar2ViewMsgAce);
		};

        // 新战场：NPC购买装备成功
        STRUCT SNPCGoodsBuySuccess
        {
        public:
            int pdbid;
            int uidNPC;
            int npcID;
            int goodsID;
            int buyTime;

            STRUCT_INIT(SNPCGoodsBuySuccess);
        };

		// WAR2VIEW_MSG_BATTLEWAR_GENIC_UPDATE    大逃杀战场基因数据更新
		STRUCT SWar2ViewMsgGenicUpdate
		{
		public:
			sp_int_array(nHeroIDList, GENIC_MAX_SIZE);
			sp_int_array(nSkinIDList, GENIC_MAX_SIZE);

			STRUCT_INIT(SWar2ViewMsgGenicUpdate);
		};

		// WAR2VIEW_MSG_WAR_PERSON_INFO_UPDATE  战场玩家信息更新
		STRUCT SWar2ViewMsgPersonInfoUpdate
		{
		public:
			int nUpdateType;	// 更新类型 EWarPersonInfoUpdateType
			int pdbid;
			// data...

			STRUCT_INIT(SWar2ViewMsgPersonInfoUpdate);
		};

		// WAR2VIEW_MSG_BATTLEWAR_DEVILKING_UPDATE 大逃杀战场-魔王更新
		STRUCT SWar2ViewMsgDevilKingUpdate
		{
		public:
			int uidDevilKing;
			BYTE isReconnect;	// 1为重连，0为正常更新数据

			STRUCT_INIT(SWar2ViewMsgDevilKingUpdate);
		};

		// WAR2VIEW_MSG_WAR_XP_BEGIN_CD 玩家获得XP技能开始CD
		STRUCT SWar2ViewMsgXPBeginCD
		{
		public:
			int pdbid;				// 玩家的pdbid
            int uID;                // 实体ID
			int nBeginCDTick;		// 开始CD的时间（时间服务器时间，毫秒）
			int nCDTime;			// CD总时间
            int nSkillID;           // 技能ID
			STRUCT_INIT(SWar2ViewMsgXPBeginCD);
		};
}
#endif	// __COMMON_WAR_MANAGED_DEF_H__