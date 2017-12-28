/*******************************************************************
** 文件名:	Entity_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	3/3/2015
** 版  本:	1.0
** 描  述:	实体相关托管公共定义

			
********************************************************************/

#ifndef __COMMON_ENTITY_MANAGED_DEF_H__
#define __COMMON_ENTITY_MANAGED_DEF_H__


#include "GameDef.h"
#include "VOCATION_ManagedDef.h"

#pragma warning(disable:4482)


//////////////////////////////////////////////////////////////////////////
/*						C++和C#公用数据结构								*/
//////////////////////////////////////////////////////////////////////////
// 实体部件ID
ENUM ENTITY_PART
{
	PART_PROPERTY = 0,			// 实体属性部件
	PART_BANK,					// 实体数据仓库部件
	PART_SPELL,					// 实体技能部件
	PART_AOI,					// 实体视野部件
	PART_MOVE,			        // 实体同步部件

	PART_FREEZE,				// 冷却部件
	PART_DAMAGE,				// 伤害部件
	PART_DIE,					// 死亡部件
	PART_BUFF,					// BUFF部件
	PART_SLOTLOGIC,				// 槽位逻辑部件
	PART_PERSON_TANK,			// 人物载具承载部件
	PART_TANK_BOX,				// 载具承载部件
	PART_ENMITY,				// 仇恨部件
	PART_PROGRESS_BAR,          // 进度条部件
	PART_DRAGONBALL,			// 七龙珠部件
	PART_WAR,					// 战场部件
	PART_MOBAGOODS,				// moba物品部件
	PART_CHAT,					// 聊天部件
    PART_ORDER,                 // 排行榜部件
    PART_WING,                  // 翅膀部件
    PART_TITLE,                 // 称号部件
    PART_LOGOUT,                // 登出、存盘部件
    PART_PK,                    // PK部件
    PART_TEST,					// 测试部件
	PART_LEGENDCUP,             // 杯赛部件
	PART_MAIL,					// 邮件部件
	PART_COMMON,				// 通用部件
    PART_AI,					// AI部件
	ENTITY_PART_MAX,			// 最大部件ID
};

///////////////////////人物性别////////////////////////////////////
ENUM PERSON_SEX 
{
	SEX_MALE = 0,					// 帅哥
	SEX_FEMALE,						// 美女
	SEX_MAX,						// 最大性别
};

//////////////////////实体状态定义///////////////////////////
ENUM ENTITY_STATE
{
	ENTITY_STATE_UNKNOW = 0,		// 未知的
	ENTITY_STATE_STAND,				// 站立
	ENTITY_STATE_DIE,				// 死亡
	ENTITY_STATE_FIGHT,				// 战斗

	ENTITY_STATE_MAXID				// 最大
};


//////////////////////////////////////////////////////////////////////////
// 实体属性
ENUM ENTITY_PROPERTY
{
	// 玩家基础属性,需要保存到数据库的
	PROPERTY_ID = 0,		        // ID 
	PROPERTY_NAME,			        // 名字
	PROPERTY_SCENEID,		        // 场景ID
	PROPERTY_LOCATION,		        // 位置
	PROPERTY_ANGLE,			        // 面向角度
	PROPERTY_VOCATION,		        // 职业ID(怪物子类型)
	PROPERTY_SEX,			        // 性别(怪物类型)
	PROPERTY_SKIN,			        // 皮肤
	PROPERTY_LEVEL,			        // 等级
	PROPERTY_EXP,			        // 经验
	PROPERTY_HP,			        // 血量值
	PROPERTY_MAX_HP,		        // 最大血量值
	PROPERTY_MP,			        // 法术值
	PROPERTY_MAX_MP,		        // 最大法术值
	PROPERTY_TICKET,		        // 点券
	PROPERTY_CAMP,                  // 阵营
    PROPERTY_KIN,			        // 战队ID
    PROPERTY_CLAN,                  // 帮会ID
	PROPERTY_FIGHTCAPACITY,         // 玩家战力
    PROPERTY_BINDGOLD,              // 玩家绑定金币
    PROPERTY_GOLD,                  // 玩家金币
    PROPERTY_FACEID,                // 头像ID
    PROPERTY_TEAMID,                // 队伍ID

    PROPERTY_MATCHTYPE,             // 排位段位信息 类型nMatchTypeID = 8  
    PROPERTY_RANKSCORE,
    PROPERTY_RANKGRADE,
    PROPERTY_OBSERVER,              // 观察者属性
    PROPERTY_VISIBLE_FLAG,          // 实体对各个阵营的可见标志掩码
    PROPERTY_KINGRANK_ORDER,        // 王者排位排名

	// 临时属性,通过计算出来的
	PROPERTY_UID,			        // 角色UID
    PROPERTY_MASTER_UID,            // 主人UID
	PROPERTY_ATTACK,		        // 攻击力
	PROPERTY_DEFENCE,		        // 防御力
	PROPERTY_SHIELD,                // 护盾值（额外生命）
	PROPERTY_STAMINA,               // 体力值
	PROPERTY_MAX_STAMINA,	        // 最大体力值
	PROPERTY_MAGIC_TYPE,            // 魔法类型
	PROPERTY_ENERGY,		        // 能量
	PROPERTY_MAX_ENERGY,	        // 最大能量

	PROPERTY_TALENT_POINT,          // 天赋点数
	PROPERTY_MASK,			        // 标志位	ENTITY_MASKFLAG
    PROPERTY_OBSFLAG,               // 可否穿越动态障碍标志位
    PROPERTY_FORMTYPE,              // 立足点类型 FORM_TYPE

	PROPERTY_PA,			        // 物理攻击(放大BASE_IN_MULTIPLE倍)
	PROPERTY_MA,			        // 法术攻击(放大BASE_IN_MULTIPLE倍)
	PROPERTY_PD,			        // 物理防御(放大BASE_IN_MULTIPLE倍)
	PROPERTY_MD,			        // 法术防御(放大BASE_IN_MULTIPLE倍)
	PROPERTY_SD,			        // 移动速度(放大BASE_IN_MULTIPLE倍)
	PROPERTY_PASD,			        // 攻击速度(放大ZOOM_IN_MULTIPLE倍)
    PROPERTY_AF,			        // 普攻系数(放大ZOOM_IN_MULTIPLE倍)
	PROPERTY_CRC,			        // 暴击概率(放大ZOOM_IN_MULTIPLE倍)
	PROPERTY_HP_REPLY,		        // 血量回复(放大BASE_IN_MULTIPLE倍)
	PROPERTY_MP_REPLY,		        // 魔法回复(放大BASE_IN_MULTIPLE倍)
    PROPERTY_ENERGY_REPLY,          // 能量回复(放大BASE_IN_MULTIPLE倍)

	PROPERTY_CRD,					// 暴击伤害

	PROPERTY_PDP,					// 物理防御穿透
	PROPERTY_PCT_PDP,				// 百分比物理防御穿透

	PROPERTY_MDP,					// 法术防御穿透
	PROPERTY_PCT_MDP,				// 百分比法术防御穿透

	PROPERTY_PCT_PAIMM,				// 百分比物理伤害减免
	PROPERTY_PCT_MAIMM,				// 百分比法术伤害减免

	PROPERTY_PCT_COOL_REDUCE,		// 百分比冷却缩减

	PROPERTY_PCT_PDRAINS,			// 百分比物理吸血
	PROPERTY_PCT_MDRAINS,			// 百分比法术吸血
	PROPERTY_TENACITY,				// 韧性

    PROPERTY_AIM_DISTANCE,			// 瞄准距离
    PROPERTY_AI_SCHEME_ID,          // AI配置ID

	PROPERTY_PCT_AF,				// 百分比普攻系数

	PROPERTY_CLIENT_MAX,	// *注：如果要给C#使用，请把属性放置到此前面
	//////////////////////////////////////////////////////////////////////////

	// 累加公式：当前生命＝（基础生命+附加生命之和）*（1+附加生命百分比之和）
    // *基础血量
	PROPERTY_BASE_HP = PROPERTY_CLIENT_MAX,				
	PROPERTY_APPEND_HP,				// 附加血量
	PROPERTY_APPEND_PCT_HP,			// 附加百分比血量

	PROPERTY_BASE_MP,				// 基础魔法
	PROPERTY_APPEND_MP,				// 附加魔法
	PROPERTY_APPEND_PCT_MP,			// 附加百分比魔法

    PROPERTY_BASE_ENERGY,		    // 基础能量
    PROPERTY_APPEND_ENERGY,		    // 附加能量
    PROPERTY_APPEND_PCT_ENERGY,		// 附加百分比能量

	PROPERTY_BASE_PA,				// 基础物理攻击
	PROPERTY_APPEND_PA,				// 附加物理攻击
	PROPERTY_APPEND_PCT_PA,			// 附加百分比物理攻击

	PROPERTY_BASE_MA,				// 基础法术攻击
	PROPERTY_APPEND_MA,				// 附加法术攻击
	PROPERTY_APPEND_PCT_MA,			// 附加百分比法术攻击

	PROPERTY_BASE_PD,				// 基础物理防御
	PROPERTY_APPEND_PD,				// 附加物理防御
	PROPERTY_APPEND_PCT_PD,			// 附加百分比物理防御

	PROPERTY_BASE_MD,				// 基础法术防御
	PROPERTY_APPEND_MD,				// 附加法术防御
	PROPERTY_APPEND_PCT_MD,			// 附加百分比法术防御

	PROPERTY_BASE_SD,				// 基础移动速度
	PROPERTY_APPEND_SD,				// 附加移动速度
	PROPERTY_APPEND_PCT_SD,			// 附加百分比移动速度

	PROPERTY_BASE_PASD,				// 基础攻击速度
	PROPERTY_APPEND_PASD,			// 附加攻击速度
	PROPERTY_APPEND_PCT_PASD,		// 附加百分比攻击速度

    PROPERTY_BASE_AF,				// 基础普攻系数
    PROPERTY_APPEND_AF,			    // 附加普攻系数
    PROPERTY_APPEND_PCT_AF,		    // 附加百分比普攻系数

	PROPERTY_BASE_CRC,				// 基础暴击概率
	PROPERTY_APPEND_CRC,			// 附加暴击概率
	PROPERTY_APPEND_PCT_CRC,		// 附加百分比暴击概率

	PROPERTY_BASE_HP_REPLY,			// 基础血量回复
	PROPERTY_APPEND_HP_REPLY,		// 附加血量回复
	PROPERTY_APPEND_PCT_HP_REPLY,	// 附加百分比血量回复

	PROPERTY_BASE_MP_REPLY,			// 基础魔法回复
	PROPERTY_APPEND_MP_REPLY,		// 附加魔法回复
	PROPERTY_APPEND_PCT_MP_REPLY,	// 附加百分比魔法回复

    PROPERTY_BASE_ENERGY_REPLY,     // 基础能量回复
    PROPERTY_APPEND_ENERGY_REPLY,   // 附加能量回复
    PROPERTY_APPEND_PCT_ENERGY_REPLY,// 附加百分比能量回复

    PROPERTY_APPEND_PCT_PDP,        // 附加额外百分比护甲穿透
    PROPERTY_APPEND_PCT_MDP,        // 附加额外百分比魔抗穿透
	/*
    因为直接设置PROPERTY_PCT_COOL_REDUCE,达到上限后回减会出问题，
	目前增加一个属性用来效果设置
	*/
	PROPERTY_BASE_PCT_COOL_REDUCE,	// 基础百分比冷却缩减

	PROPERTY_BASE_ENACITY,			// 基础韧性

	PROPERTY_MONNEY_COEFFICIENT,    // 额外金币系数
	PROPERTY_EXP_COEFFICIENT,       // 额外经验系数

    PROPERTY_BASE_DAMAGE_BONUS,     // 基础伤害加成(固定值)
    PROPERTY_BASE_DAMAGE_BONUS_PCT, // 基础伤害加成百分比
    PROPERTY_DAMAGE_BONUS,          // 伤害加成(固定值)
    PROPERTY_DAMAGE_BONUS_PCT,      // 伤害加成百分比

    PROPERTY_CONTROL_IMMUNE,        // 控制免疫(减少控制时间)

    PROPERTY_PROMOTE_SHIELD,        // 提升护盾

    PROPERTY_VELOCITY,              // 瞬时速度 只作存储用

	PROPERTY_BULLET_COUNT,			// 子弹数量



	PROPERTY_MAX,					// 最大属性
};

#define BASE_IN_MULTIPLE	100.0f	// 基础值放大倍数
#define ZOOM_IN_MULTIPLE	10000.0f	// 放大倍数(百分比扩大的倍数)

// ！！！
// 注意：c#的enum会自动排序，所以下面的需要从小到大排，不然描述会对不上正确的数值
// 实体属性效果更改索引,对应效果可改变的实体属性
ENUM ENTITY_PROPERTY_
{
	DESC("更改皮肤(模型)")
	CHANGE_SKIN = ENTITY_PROPERTY::PROPERTY_SKIN,	
	DESC("附加血量")
	APPEND_HP = ENTITY_PROPERTY::PROPERTY_APPEND_HP,
	DESC("附加百分比血量")
	APPEND_PCT_HP = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_HP,
	DESC("附加魔法")
	APPEND_MP = ENTITY_PROPERTY::PROPERTY_APPEND_MP,
	DESC("附加百分比魔法")
	APPEND_PCT_MP = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MP,
    DESC("附加能量")
    APPEND_ENERGY = ENTITY_PROPERTY::PROPERTY_APPEND_ENERGY,
    DESC("附加百分比能量")
    APPEND_PCT_ENERGY = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_ENERGY,
	DESC("附加物理攻击")
	APPEND_PA = ENTITY_PROPERTY::PROPERTY_APPEND_PA,	
	DESC("附加百分比物理攻击")
	APPEND_PCT_PA = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_PA,	
	DESC("附加法术攻击")
	APPEND_MA = ENTITY_PROPERTY::PROPERTY_APPEND_MA,	
	DESC("附加百分比法术攻击")
	APPEND_PCT_MA = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MA,	
	DESC("附加物理防御")
	APPEND_PD = ENTITY_PROPERTY::PROPERTY_APPEND_PD,	
	DESC("附加百分比物理防御")
	APPEND_PCT_PD = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_PD,	
	DESC("附加法术防御")
	APPEND_MD = ENTITY_PROPERTY::PROPERTY_APPEND_MD,	
	DESC("附加百分比法术防御")
	APPEND_PCT_MD = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MD,	
	DESC("附加移动速度")
	APPEND_SD = ENTITY_PROPERTY::PROPERTY_APPEND_SD,		
	DESC("附加百分比移动速度")
	APPEND_PCT_SD = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_SD,	
	DESC("附加攻击速度")
	APPEND_PASD = ENTITY_PROPERTY::PROPERTY_APPEND_PASD,	
	DESC("附加百分比攻击速度")
	APPEND_PCT_PASD = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_PASD,
    DESC("附加普攻系数")
    APPEND_AF = ENTITY_PROPERTY::PROPERTY_APPEND_AF,	
    DESC("附加百分比普攻系数")
    APPEND_PCT_AF = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_AF,
	DESC("附加暴击概率")
	APPEND_CRC = ENTITY_PROPERTY::PROPERTY_APPEND_CRC,			
	DESC("附加百分比暴击概率")
	APPEND_PCT_CRC = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_CRC,
	DESC("附加血量回复")
	APPEND_HP_REPLY = ENTITY_PROPERTY::PROPERTY_APPEND_HP_REPLY,			
	DESC("附加百分比血量回复")
	APPEND_PCT_HP_REPLY = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_HP_REPLY,
	DESC("附加魔法回复")
	APPEND_MP_REPLY = ENTITY_PROPERTY::PROPERTY_APPEND_MP_REPLY,			
	DESC("附加百分比魔法回复")
	APPEND_PCT_MP_REPLY = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MP_REPLY,
    DESC("附加能量回复")
    APPEND_ENERGY_REPLY = ENTITY_PROPERTY::PROPERTY_APPEND_ENERGY_REPLY,			
    DESC("附加百分比能量回复")
    APPEND_PCT_ENERGY_REPLY = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_ENERGY_REPLY,
	DESC("暴击伤害")
	__CRD = ENTITY_PROPERTY::PROPERTY_CRD,	
	DESC("物理防御穿透") 
	__PDP = ENTITY_PROPERTY::PROPERTY_PDP,							
	DESC("百分比物理防御穿透") 
	__PCT_PDP = ENTITY_PROPERTY::PROPERTY_PCT_PDP,					
	DESC("法术防御穿透")
	__MDP = ENTITY_PROPERTY::PROPERTY_MDP,					
	DESC("百分比法术防御穿透")
	__PCT_MDP = ENTITY_PROPERTY::PROPERTY_PCT_MDP,	
	DESC("物理伤害减免百分比")
	__PCT_PAIMM = ENTITY_PROPERTY::PROPERTY_PCT_PAIMM,
	DESC("法术伤害减免百分比")
	__PCT_MAIMM = ENTITY_PROPERTY::PROPERTY_PCT_MAIMM,
	DESC("基础百分比冷却缩减")
	_BASE_PCT_COOL_REDUCE = ENTITY_PROPERTY::PROPERTY_BASE_PCT_COOL_REDUCE,
	DESC("体力")
	_STAMINA = ENTITY_PROPERTY::PROPERTY_STAMINA,
	DESC("最大体力")
	_MAX_STAMINA = ENTITY_PROPERTY::PROPERTY_MAX_STAMINA,
	DESC("能量")
	_ENERGY = ENTITY_PROPERTY::PROPERTY_ENERGY,
	DESC("最大能量")
	_MAX_ENERGY = ENTITY_PROPERTY::PROPERTY_MAX_ENERGY,
	DESC("百分比物理吸血")
	_PCT_PDRAINS = ENTITY_PROPERTY::PROPERTY_PCT_PDRAINS,
	DESC("百分比法术吸血")
	_PCT_MDRAINS = ENTITY_PROPERTY::PROPERTY_PCT_MDRAINS,
	DESC("基础韧性")
	_BASE_ENACITY = ENTITY_PROPERTY::PROPERTY_BASE_ENACITY,
    DESC("基础伤害加成(固定值)")
    _BASE_DAMAGE_BONUS = ENTITY_PROPERTY::PROPERTY_BASE_DAMAGE_BONUS,
    DESC("基础伤害加成百分比")
    _BASE_DAMAGE_BONUS_PCT = ENTITY_PROPERTY::PROPERTY_BASE_DAMAGE_BONUS_PCT,
    DESC("控制免疫(减少控制时间)")
    _CONTROL_IMMUNE = ENTITY_PROPERTY::PROPERTY_CONTROL_IMMUNE,
    DESC("瞄准距离")
    _AIM_DISTANCE = ENTITY_PROPERTY::PROPERTY_AIM_DISTANCE,
    DESC("提升护盾")
    _PROMOTE_SHIELD = ENTITY_PROPERTY::PROPERTY_PROMOTE_SHIELD,
    DESC("附加额外百分比护甲穿透")
    _APPEND_PCT_PDP = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_PDP,
    DESC("附加额外百分比魔抗穿透")
    _APPEND_PCT_MDP = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MDP,
};

// 可持续改变的实体属性效果更改索引,对应效果：可改变的实体属性
ENUM ENTITY_PROPERTY_CONTINUED
{
	DESC("附加血量")
	APPEND_HP_ = ENTITY_PROPERTY::PROPERTY_APPEND_HP,
	DESC("附加百分比血量")
	APPEND_PCT_HP_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_HP,
	DESC("附加魔法")
	APPEND_MP_ = ENTITY_PROPERTY::PROPERTY_APPEND_MP,
	DESC("附加百分比魔法")
	APPEND_PCT_MP_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MP,
	DESC("附加能量")
	APPEND_ENERGY_ = ENTITY_PROPERTY::PROPERTY_APPEND_ENERGY,
	DESC("附加百分比能量")
	APPEND_PCT_ENERGY_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_ENERGY,
	DESC("附加物理攻击")
	APPEND_PA_ = ENTITY_PROPERTY::PROPERTY_APPEND_PA,
	DESC("附加百分比物理攻击")
	APPEND_PCT_PA_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_PA,
	DESC("附加法术攻击")
	APPEND_MA_ = ENTITY_PROPERTY::PROPERTY_APPEND_MA,
	DESC("附加百分比法术攻击")
	APPEND_PCT_MA_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MA,
	DESC("附加物理防御")
	APPEND_PD_ = ENTITY_PROPERTY::PROPERTY_APPEND_PD,
	DESC("附加百分比物理防御")
	APPEND_PCT_PD_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_PD,
	DESC("附加法术防御")
	APPEND_MD_ = ENTITY_PROPERTY::PROPERTY_APPEND_MD,
	DESC("附加百分比法术防御")
	APPEND_PCT_MD_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MD,
	DESC("附加移动速度")
	APPEND_SD_ = ENTITY_PROPERTY::PROPERTY_APPEND_SD,
	DESC("附加百分比移动速度")
	APPEND_PCT_SD_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_SD,
	DESC("附加攻击速度")
	APPEND_PASD_ = ENTITY_PROPERTY::PROPERTY_APPEND_PASD,
	DESC("附加百分比攻击速度")
	APPEND_PCT_PASD_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_PASD,
	DESC("附加普攻系数")
	APPEND_AF_ = ENTITY_PROPERTY::PROPERTY_APPEND_AF,
	DESC("附加百分比普攻系数")
	APPEND_PCT_AF_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_AF,
	DESC("附加暴击概率")
	APPEND_CRC_ = ENTITY_PROPERTY::PROPERTY_APPEND_CRC,
	DESC("附加百分比暴击概率")
	APPEND_PCT_CRC_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_CRC,
	DESC("附加血量回复")
	APPEND_HP_REPLY_ = ENTITY_PROPERTY::PROPERTY_APPEND_HP_REPLY,
	DESC("附加百分比血量回复")
	APPEND_PCT_HP_REPLY_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_HP_REPLY,
	DESC("附加魔法回复")
	APPEND_MP_REPLY_ = ENTITY_PROPERTY::PROPERTY_APPEND_MP_REPLY,
	DESC("附加百分比魔法回复")
	APPEND_PCT_MP_REPLY_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MP_REPLY,
	DESC("附加能量回复")
	APPEND_ENERGY_REPLY_ = ENTITY_PROPERTY::PROPERTY_APPEND_ENERGY_REPLY,
	DESC("附加百分比能量回复")
	APPEND_PCT_ENERGY_REPLY_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_ENERGY_REPLY,
	DESC("暴击伤害")
	__CRD_ = ENTITY_PROPERTY::PROPERTY_CRD,
	DESC("物理防御穿透")
	__PDP_ = ENTITY_PROPERTY::PROPERTY_PDP,
	DESC("百分比物理防御穿透")
	__PCT_PDP_ = ENTITY_PROPERTY::PROPERTY_PCT_PDP,
	DESC("法术防御穿透")
	__MDP_ = ENTITY_PROPERTY::PROPERTY_MDP,
	DESC("百分比法术防御穿透")
	__PCT_MDP_ = ENTITY_PROPERTY::PROPERTY_PCT_MDP,
	DESC("物理伤害减免百分比")
	__PCT_PAIMM_ = ENTITY_PROPERTY::PROPERTY_PCT_PAIMM,
	DESC("法术伤害减免百分比")
	__PCT_MAIMM_ = ENTITY_PROPERTY::PROPERTY_PCT_MAIMM,
	DESC("基础百分比冷却缩减")
	_STAMINA_ = ENTITY_PROPERTY::PROPERTY_STAMINA,
	DESC("最大体力")
	_MAX_STAMINA_ = ENTITY_PROPERTY::PROPERTY_MAX_STAMINA,
	DESC("能量")
	_ENERGY_ = ENTITY_PROPERTY::PROPERTY_ENERGY,
	DESC("最大能量")
	_MAX_ENERGY_ = ENTITY_PROPERTY::PROPERTY_MAX_ENERGY,
	DESC("百分比物理吸血")
	_PCT_PDRAINS_ = ENTITY_PROPERTY::PROPERTY_PCT_PDRAINS,
	DESC("百分比法术吸血")
	_PCT_MDRAINS_ = ENTITY_PROPERTY::PROPERTY_PCT_MDRAINS,
	DESC("基础韧性")
	_BASE_ENACITY_ = ENTITY_PROPERTY::PROPERTY_BASE_ENACITY,
	DESC("基础伤害加成(固定值)")
	_BASE_DAMAGE_BONUS_ = ENTITY_PROPERTY::PROPERTY_BASE_DAMAGE_BONUS,
	DESC("基础伤害加成百分比")
	_CONTROL_IMMUNE_ = ENTITY_PROPERTY::PROPERTY_CONTROL_IMMUNE,
	DESC("瞄准距离")
	_PROMOTE_SHIELD_ = ENTITY_PROPERTY::PROPERTY_PROMOTE_SHIELD,
	DESC("附加额外百分比护甲穿透")
	_APPEND_PCT_PDP_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_PDP,
	DESC("附加额外百分比魔抗穿透")
	_APPEND_PCT_MDP_ = ENTITY_PROPERTY::PROPERTY_APPEND_PCT_MDP,
};

DESC("使用对象")
ENUM MA_TAGET_TYPE
{
	DESC("自己")
	MA_TAGET_TO_SELF = 0,
	DESC("目标")
	MA_TAGET_TO_TARGET,
	DESC("自己和目标")
	MA_TAGET_TO_BOTH,
	DESC("所有")
	MA_TAGET_TO_ALL,
	DESC("操作者")
	MA_TAGET_TO_OPERATOR,
    DESC("无目标(不需锁定)")
    MA_TAGET_NO,
};

DESC("阵营")
ENUM MA_CAMP_RELATIONSHIP
{
    DESC("友方")
    MA_CAMP_RS_FRIEND,
    DESC("敌方")
    MA_CAMP_RS_ENEMY,
    DESC("所有")
    MA_CAMP_RS_ALL,
};

DESC("更改类型")
ENUM MA_CHANGE_TYPE
{
	DESC("增减")
	MA_CHANGE_ADD = 0,
	DESC("重设")
	MA_CHANGE_SET,
};

DESC("数值类型")
ENUM MA_VALUE_TYPE
{
	DESC("固定值")
	MA_VALUE_NUM = 0,
	DESC("百分比")
	MA_VALUE_PER,
	DESC("损失百分比")
	MA_VALUE_LOSE_PER,
	DESC("当前百分比")
	MA_VALUE_CUR_PER,
};

DESC("BOOL类型")
ENUM MA_BOOL
{		
	DESC("否")
	MA_FALSE = 0,
	DESC("是")
	MA_TRUE = 1,
};

DESC("属性类型")
ENUM MA_PROPERTY_TYPE
{		
	DESC("血量")
	MA_PROPERTY_HP = ENTITY_PROPERTY::PROPERTY_HP,
	DESC("魔法")
	MA_PROPERTY_MP = ENTITY_PROPERTY::PROPERTY_MP,
	DESC("体力")
	MA_PROPERTY_STAMINA = ENTITY_PROPERTY::PROPERTY_STAMINA,
	DESC("能量")
	MA_PROPERTY_ENERGY = ENTITY_PROPERTY::PROPERTY_ENERGY,
    DESC("等级")
    MA_PROPERTY_LEVEL = ENTITY_PROPERTY::PROPERTY_LEVEL,
    DESC("皮肤")
    MA_PROPERTY_SKIN  = ENTITY_PROPERTY::PROPERTY_SKIN,
};	

DESC("实体类型")
ENUM ENTITY_TYPE
{
	TYPE_ENTITY = 0,
	TYPE_PLAYER_ROLE,
	TYPE_MONSTER,
	TYPE_TANK,
	TYPE_MAX,
};


// 怪物类型
DESC("怪物类型")
ENUM MONSTER_TYPE
{
	MONSTER_TYPE_NULL		= 0,		// 怪物类型无效值
	MONSTER_TYPE_COMMON,				// 普通的
	MONSTER_TYPE_TOWER,					// 堡垒
	MONSTER_TYPE_WILD_MONITOR,			// 野怪监视器
    MONSTER_TYPE_DYNAMIC_OBSTACLE,      // 动态障碍怪
	MONSTER_TYPE_OBSTACLE,              // 障碍怪
	MONSTER_TYPE_MAX,					// 怪物类型最大值
};


// 怪物子类型
DESC("怪物子类型")
ENUM MONSTER_SUB_TYPE
{
    MONSTER_SUB_TYPE_NULL = 0,              // 无效
    MONSTER_SUB_TYPE_WILD_MONSTER,          // 野生怪
    MONSTER_SUB_TYPE_SOLDIER,               // 小兵
    MONSTER_SUB_TYPE_EYE_STONE,             // 眼石
    MONSTER_SUB_TYPE_DEFENSIVE_TOWER,       // 防御塔
    MONSTER_SUB_TYPE_BASE,                  // 基地
    MONSTER_SUB_TYPE_CRYSTAL_TOWER,         // 水晶
    MONSTER_SUB_TYPE_SPRING,                // 泉水
    MONSTER_SUB_TYPE_BARRIER,               // 路障
    MOSNTER_SUB_TYPE_MONITOR,               // 监视怪
	MONSTER_SUB_TYPE_NPC,				    // NPC
    MONSTER_SUB_TYPE_HP_SUPPLIES,           // 血包
    MONSTER_SUB_TYPE_SMALL_DRAGON,          // 小龙
    MONSTER_SUB_TYPE_HUGE_DRAGON,           // 大龙
    MONSTER_SUB_TYPE_SUMMON_SMALL_DRAGON,   // 推塔小龙
    MONSTER_SUB_TYPE_SUMMON_ENTITY,        // 召唤物
};


DESC("实体标记")
ENUM ENTITY_MASKFLAG 
{
	MASKFLAG_NORMAL			                = 0x0,		// 缺省不处理
	MASKFLAG_HIDE_BLOOD		                = 1<<0,		// 隐藏血条
	MASKFLAG_HIDE_NAME		                = 1<<1,		// 隐藏名称
	MASKFLAG_NOT_SELECT		                = 1<<2,		// 禁止被选中
	MASKFLAG_NOT_TURN		                = 1<<3,		// 禁止转向
	MASKFLAG_HIDE_FRIEND	                = 1<<4,		// 对友方隐藏模型
	MASKFLAG_HIDE_ENEMY		                = 1<<5,		// 对非友方隐藏模型
	MASKFLAG_SHOW_EFFECT	                = 1<<6,		// 显示攻击光效
	MASKFLAG_SHOW_BIGBLOOD	                = 1<<7,		// 是否显示大血条(BOSS血条)
	MASKFLAG_HAVE_COLLIDER	                = 1<<8,		// 是否拥有碰撞(不能让主角穿过去)
	MASKFLAG_FRIEND_CROSS	                = 1<<9,		// 队友可穿过
	MASKFLAG_NOT_MOVE		                = 1<<10,	// 禁止移动
	MASKFLAG_NOT_FLY		                = 1<<11,	// 不可击飞(上下方向)
	MASKFLAG_NOT_BACK		                = 1<<12,	// 不可击退(平面方向)
	MASKFLAG_BY_SECKILL		                = 1<<13,	// 被秒杀标识
	MASKFLAG_AS_WARD		                = 1<<14,	// 拥有结界效果，友方可穿过敌方不可穿过
    MASKFLAG_HIDE_IN_SELF_MINIMAP           = 1<<15,    // 在自己的小地图上不显示
    MASKFLAG_HIDE_IN_FRIEND_MINIMAP         = 1<<16,    // 在友方的小地图上不显示
    MASKFLAG_HIDE_IN_ENEMY_MIMIMAP          = 1<<17,    // 在敌方的小地图上不显示
    MASKFLAG_KILL_MONSTER_NOT_ADD_GOLD		= 1<<18,	// 击杀小兵不加金币
    MASKFLAG_KILL_CREEP_NOT_ADD_GOLD		= 1<<19,	// 击杀野怪不加金币
    MASKFLAG_KILL_ROLE_NOT_ADD_GOLD		    = 1<<20,	// 击杀英雄不加金币
    MASKFLAG_SELECT_SPECIAL				    = 1<<21,	// 特殊快捷键和特殊的方式选取 一般不能选
	MASKFLAG_SEARCH_SPECIAL			        = 1<<22,	// 特殊怪搜寻实体 一般怪没有
    MASKFLAG_MASTER_DAMAGE		            = 1<<23,	// 主人伤害标识
    MASKFLAG_DIE_NO_COMMONLIGHT   		    = 1<<24,	// 怪物死亡不播放公共的死亡光效(现在是金币)
    MASKFLAG_NOT_ATTACK   		            = 1<<25,	// 不能攻击    
    MASKFLAG_PROPCHANGE_HIDE_SAME_CAMP      = 1<<26,	// 属性变化不广播给同阵营英雄
    MASKFLAG_FORBID_CHANGE                  = 1<<27,	// 禁止切换（人物为切英雄）
    MASKFLAG_DISABLE_MINIMAP_SIGHT_CHECK    = 1<<28,    // 关闭小地图视野检测
};

DESC("翅膀状态类型")
ENUM WING_STATE_TYPE
{
    DESC("无效状态")
    WING_STATE_INVALID = 0,
    DESC("起飞")
    WING_STATE_START_FLY,
    DESC("正常飞行")
    WING_STATE_NORMAL_FLIGHT,
    DESC("加速")
    WING_STATE_ACCELERATION,
    DESC("落地")
    WING_STATE_FLOOR,
    DESC("翅膀最大状态")
    WING_STATE_MAX,
};

// 魔法类型
ENUM MAGIC_TYPE
{
	MAGIC_TYPE_NONE = 0,				// 不显示
	MAGIC_TYPE_MP,						// 显示魔法
	MAGIC_TYPE_ENERGY,					// 显示能量(包括怒气共用能量字段，可控制自动增长和不增长等)
	MAGIC_TYPE_MAX,
};

#define MAGIC_NAME_LEN	16

// 魔法类型值(扩大此倍数来区分要显示的魔法类型)
#define MAGIC_TYPE_VALUE		1000000

// 技能升级槽位数量
#define SPELL_UPGRADE_SLOT_COUNT			4

/// 动作ID
ENUM ActID
{
    StandIdle,  // 站着不动
    Forward,    // 前进
    Back,       // 后退
    Left,       // 左转
    Right,      // 又转
    Jump,       // 跳跃
    Fall,       // 下落
    Attack,     // 攻击
    Kick,       // 脚踢
    Die,        // 死亡  
    SwimmingIdle,//游泳不动
    SwimmingForward,//游泳前进
    SwimmingBack,//游泳后退
    SwimmingLeft,//左边游泳
    SwimmingRight,//右边游泳

    /// ACT数目，请在这个前面添加
    ACT_COUNT,
};

// 每个英雄最大皮肤数量
#define PERSONSKIN_SKIN_COUNT  4
// 人物皮肤模型项
STRUCT PersonSkinModelItem
{
    int     nHeroID;								    // 英雄职业
    BYTE    nType;                                      // 英雄类型 VOCATION_TYPE
    BYTE    nPlayMode;                                  // 操作模式 PLAY_MODE
    BYTE    nTraits;                                    // 英雄特征 HERO_TRAITS
    BYTE    bySex;									    // 性别
    int     nDefaultSkinID;							    // 默认皮肤ID
	sp_char_array(szHeroName, GAME_NAME_MAXSIZE);	    // 英雄名
	sp_char_array(szHeroTitle, GAME_NAME_MAXSIZE);	    // 英雄称号
    int     nMinimapIconID;                             // 小地图图标ID

    sp_int_array(nSightArray, MAP_MAXCOUNT)               // 视野距离

    BYTE    nOnOff;                                     // 英雄开关
    sp_int_array(nSkinIDList, PERSONSKIN_SKIN_COUNT);   // 皮肤列表
	int		nUnlockLevel;							    // 英雄解锁等级

	STRUCT_INIT(PersonSkinModelItem)
};

#define SKININFO_SCHEME_FILENAME				"Scp\\SkinInfo.csv"
// 皮肤模型配置 服务器
STRUCT SkinInfoScheme_S
{
	int		nSkinID;								    // 皮肤ID
	int     nSkinType;								    // 皮肤类型，是怪物，还是人物，还是载具
	float	fSkinScale;								    // 皮肤缩放
	int	    nPreLoad;								    // 是否预加载
	int     nSkinLevel;								    // 皮肤等级
	int		nSkinIconID;							    // 皮肤图片id
    int     nSelectEffectID;                            // 选人光效ID  

	STRUCT_INIT(SkinInfoScheme_S)
};
// 皮肤模型配置
STRUCT SkinInfoScheme
{
    int		nSkinID;								    // 皮肤ID
    int     nSkinType;								    // 皮肤类型，是怪物，还是人物，还是载具
    float	fSkinScale;								    // 皮肤缩放
    int	    nPreLoad;								    // 是否预加载
    int     nSkinLevel;								    // 皮肤等级
    int		nSkinIconID;							    // 皮肤图片id
    int     nSelectEffectID;                            // 选人光效ID
	int		nSoundID;									// 选人音效ID

    sp_char_array(szSkinName,GAME_NAME_MAXSIZE);	    // 名字
    sp_char_array(szSkinPatch,GAME_TITLE_MAXSIZE);		// 路径使用大字符串长度，现在有的模型路径已经超过原来的GAME_NAME_MAXSIZE了
	sp_char_array(szSkinTip,GAME_TITLE_MAXSIZE);

    STRUCT_INIT(SkinInfoScheme)
};

// 同步到显示层的实体数据
STRUCT EntityViewItem
{
	int EntityID;			// 实体ID
	int nSkinID;			// 皮肤ID
	short nHeroID;			// 英雄ID
	BYTE bySexType;			// 性别
	BYTE EntityType;		// 实体类型
	short nLevel;			// 等级
	BYTE byCamp;			// 阵营
	BYTE byCampFlag;		// 阵营标识
	BYTE byIsHero;			// 是否为主角,1=主角
	int nMaskFlag;			// 实体标记 ENTITY_MASKFLAG 
	int nBossBloodShowDistance;	//boss血条显示距离
	int nBossBloodPriority;		//boss血条的显示优先级
	int nBloodStyle;            //血条样式
	float fSkinScale;			//实体缩放
    int nMinimapIcon;       // 小地图图标ID
    int nSight;             // 视野距离
	sp_char_array(szModelPath, GAME_TEXTSTR_MAXSIZE);	// 模型路径
	sp_char_array(szName, GAME_NAME_MAXSIZE);		    // 名字	

	int ndata1;				// 
	STRUCT_INIT(EntityViewItem)
};

// 血量更新
STRUCT entity_update_hp
{
public:
	int CurHp;				// 当前血量
	int MaxHp;				// 最大血量
	int TempHp;				// 临时血量
	int nCamp;				// 阵营

	STRUCT_INIT(entity_update_hp)
};

STRUCT entity_update_Numproperty
{
public :
	int nPropID;			// 属性ID
	int nValue;				// 属性数值
	int nOtherData;			// 状态数据
	int uID;	            // 显示层的实体ID
	int isIntproperty;         // 是否数值属性
	int isRaiseIprEvent;       //是否需要实时响应数值属性更新事件
	int nPBUID;                // PBUID
	STRUCT_INIT(entity_update_Numproperty)
};

// 实体位置
STRUCT entity_location
{
public:
	float x;
	float y;
	float z;

	STRUCT_INIT(entity_location)
};

// 星级配置
STRUCT SHeroStarSchemeInfo
{
	int     nHeroID;
	int     nStarLV;
	int     nPropertyID;
	int     nLimitLV;
	int     nExpType;
	int     nExclusiveTalentMaxCount;
	sp_char_array(chExclusiveTalent, GAME_NAME_MAXSIZE);
	int		dwNeedCardID;
	int     nNeedCount;
	int     nExpendGold;
	int     nStarIconID;
	sp_char_array(chStarDesc, GAME_NAME_MAXSIZE);
	int     nStarFrameID;
	int     nStarEffectID;
	int		nCardDiscount;
	int		nGoldDiscount;

	STRUCT_INIT(SHeroStarSchemeInfo)
};

// 游戏导航栏项目
STRUCT SGameNavItem
{
    BYTE nID;
    BYTE nStrLen;
    // char[]

    STRUCT_INIT(SGameNavItem)
};

// 成就类型
ENUM ENACHIEVE
{
    ENACHIEVE_WINNUM = 0,                           // 所有比赛胜利的场次
    ENACHIEVE_KILLNUM,                              // 总的击杀次数
    ENACHIEVE_MVPNUM,                               // 获得的mvp次数
    ENACHIEVE_MATCHNUM,                             // 总的比赛场次
    ENACHIEVE_ONCESKILLCOUNT,                       // 杀人数最高的次数
    ENACHIEVE_5KILLNUM,                             // 5杀的次数
    ENACHIEVE_4KILLNUM,                             // 4杀的次数
    ENACHIEVE_3KILLNUM,                             // 3杀的次数
    ENACHIEVE_SUPERGODNUM,                          // 超神的次数
    ENACHIEVE_KILLBIGDRAGONNUM,                     // 击杀大龙次数
    ENACHIEVE_KILLSMALLDRAGONNUM,                   // 击杀小龙次数
    ENACHIEVE_FIRSTKILLTOWERNUM,                    // 推塔第一次数
    ENACHIEVE_FIRSTDIENUM,                          // 死亡第一的次数
    ENACHIEVE_ONCEASSISTCOUNT,                      // 助攻数最高的次数
    ENACHIEVE_ADDVALUE,
    // 以上是累加数据

    // 以下是最高数据
    ENACHIEVE_CONTINUEWINMAX,                       // 最高连胜次数
    ENACHIEVE_SCOREMAX,                             // 最高评分
    ENACHIEVE_KILLMAX,                              // 最高击杀
    ENACHIEVE_ASSISTMAX,                            // 最高助攻
    ENACHIEVE_CONTINUEKILLMAX,                      // 最高连杀
    ENACHIEVE_OUTMAX,                               // 最高输出
    ENACHIEVE_DAMAGEMAX,                            // 最高承受伤害
    ENACHIEVE_MONEYMAX,                             // 最佳经济
    ENACHIEVE_KILLMONSTERMAX,                       // 补兵最多
    ENACHIEVE_KILLTOWETMAX,                         // 最多推塔
    ENACHIEVE_TIMEMAX,                              // 最久比赛时间
    ENACHIEVE_TIMEMIN,                              // 最短比赛事件

    ENACHIEVE_MAX
};

// 比赛结果类型
ENUM ENMATCHRESULTTYPE
{
    ENMATCHRESULTTYPE_INVALID = 0,              // 无效局

	ENMATCHRESULTTYPE_HANG,                     // 挂机局
	ENMATCHRESULTTYPE_HARD,                     // 尽力局
    ENMATCHRESULTTYPE_JILTPAN,                  // 甩锅局

	ENMATCHRESULTTYPE_WELFARE,                  // 福利局
    ENMATCHRESULTTYPE_CARRY,                    // CARRY局
	ENMATCHRESULTTYPE_TILTEDPAN,                // 翻盘局
    ENMATCHRESULTTYPE_GRIND,                    // 碾压局
    ENMATCHRESULTTYPE_LIEWIN,                   // 躺赢局

    ENMATCHRESULTTYPE_MAX
};

// 购买英雄结果类型
ENUM ENBUYHEROTYPE
{
    ENBUYHEROTYPE_SUCESS = 1,               // 购买成功
    ENBUYHEROTYPE_LACKOFTICK,               // 点券不足
    ENBUYHEROTYPE_BINDGOLD,                 // 绑定金币不足
    ENBUYHEROTYPE_GOLD,                     // 金币不足

    ENBUYHEROTYPE_MAX,
};

// 角色游戏状态 ( 在线状态 )
ENUM ACTOR_GAME_STATE
{
	GAME_STATE_INVALID = -1,        // 无效
	GAME_STATE_IDLE  = 0,           // 空闲状态,默认
	GAME_STATE_MATCH,				// 撮合申请状态
    GAME_STATE_INROOM,			    // 房间状态
	GAME_STATE_WAR,				    // 战场竞技状态
    GAME_STATE_OFFLINE,             // 不在线
    GAME_STATE_LEAVE,				// 离开
    GAME_STATE_OBSERVE,             // 观战
	GAME_STATE_MAX                  // 最大值
};

// 英雄标志位
ENUM HERO_FLAG
{
	HERO_FLAG_INVALD = 0,				// 无效标志位
	HERO_FLAG_NOT_HAVE = 1<<0,			// 未拥有英雄
    HERO_FLAG_LIFEHERO = 1<<1,          // 本命英雄
};

// 传送原因 根据原因确定 镜头是否需要和朝向一致
ENUM TRANSPORT_REASON
{
    TRANSPORT_REASON_FLASH,    // 闪现
    TRANSPORT_REASON_REBORN,   // 复活
    TRANSPORT_REASON_BACKCITY, // 回城
    TRANSPORT_REASON_SET_POS,  // 直接设置位置
    TRANSPORT_REASON_APPEAR,   // 进视野
};

ENUM ENPLAYERINFOTYPE
{
	ENPLAYERINFOTYPE_WORLDID = 0,       // 所属游戏世界id
	ENPLAYERINFOTYPE_HEROID,            // 静态英雄id类型
	ENPLAYERINFOTYPE_SKIN,              // 静态皮肤id类型
	ENPLAYERINFOTYPE_LEVEL,             // 静态等级类型
	ENPLAYERINFOTYPE_SEX,               // 性别类型  1代表false  其他非零的全是true
	ENPLAYERINFOTYPE_MAPID,             // 地图ID类型
	ENPLAYERINFOTYPE_EXP,               // 经验类型
	ENPLAYERINFOTYPE_GOLD,              // 金币类型
	ENPLAYERINFOTYPE_BINDGOLD,          // 绑定金币类型
	ENPLAYERINFOTYPE_TICKET,            // 点券类型
	ENPLAYERINFOTYPE_POOLLIMIT,         // 英雄池上限类型
	ENPLAYERINFOTYPE_HEADID,            // 头像ID类型
	ENPLAYERINFOTYPE_CLANID,            // 工会ID类型
	ENPLAYERINFOTYPE_KINID,             // 战队ID类型
	ENPLAYERINFOTYPE_TEAMID,            // 战队ID
	ENPLAYERINFOTYPE_NAME,              // 名字类型
	ENPLAYERINFOTYPE_SUMMONER,          // 召唤师技能类型
	ENPLAYERINFOTYPE_GLORY,             // 个人荣耀
	ENPLAYERINFOTYPE_PKWINNUM,          // PK胜场
	ENPLAYERINFOTYPE_ADULT,				// 成年人
	ENPLAYERINFOTYPE_GEMSTONEEXP,       // 宝石经验类型
	ENPLAYERINFOTYPE_SUPPORTPOINT,      // 点赞值
	ENPLAYERINFOTYPE_PKTOTALNUM,		// PK总胜场
	ENPLAYERINFOTYPE_PASSPORTFLAG,		// 通行证标志
    ENPLAYERINFOTYPE_RANKNUM,           // rank胜率相关
	EPIT_MAX,
};

ENUM BENEFITCARDTYPE
{
	BENEFITCARDTYPE_GOLD = 1,         // 金币增益卡
	BENEFITCARDTYPE_ACTOR_EXP = 2,    // 角色经验增益卡
	BENEFITCARDTYPE_HERO_EXP = 3,     // 英雄经验增益卡
};

ENUM BENEFITCARDSUBTYPE
{
	BENEFITCARDSUBTYPE_PERIOD = 1,     // 某个时间段内使用
	BENEFITCARDSUBTYPE_USECOUNT = 2    // 固定次数使用
};

ENUM BENEFITCARDLISTTYPE
{
	BENEFITCARDLISTTYPE_REMAIN = 1,     // 玩家剩余增益卡列表
	BENEFITCARDLISTTYPE_USED = 2		// 玩家本局产生效果的增益卡列表
};

// 任务类型
ENUM ENTASK_TYPE
{
	ENTASK_TYPE_FIRSTWIN = 1,			// 1首胜任务
	ENTASK_TYPE_CLAN,					// 2帮会任务
	ENTASK_TYPE_CYCLE,					// 3日常循环任务
	ENTASK_TYPE_MAIN,					// 4主线任务
	ENTASK_TYPE_CONTACT,				// 5交互任务
	ENTASK_TYPE_DAILY,					// 6日常任务
	ENTASK_TYPE_RANK,					// 7排位任务
};

// 任务子类型
ENUM ENTASK_SUB_TYPE
{
	ENTASK_DISPLAY = 1,			// 炫耀
	ENTASK_HELP,				// 互助
	ENTASK_FRIEND,				// 交友
	ENTASK_TRANSFER,			// 心情
	ENTASK_PLAY,				// 一起玩
};

// 任务具体类型
ENUM ENTASK_SPECIFIC_TYPE
{
	ENTASK_DISPLAY_5KILL = 1,	// 5杀
	ENTASK_DISPLAY_4KILL,		// 4杀
	ENTASK_DISPLAY_3KILL,		// 3杀
	ENTASK_DISPLAY_SuperGod,	// 超神
	ENTASK_DISPLAY_MVP,			// MVP

	ENTASK_HELP_CARD,			// 卡牌 

	ENTASK_FRIEND_FRIEND,		// 交友
	ENTASK_FRIEND_SEX,			// 异性交友
	ENTASK_FRIEND_APPRENTICE,	// 拜师

	ENTASK_TRANSFER_BLESS,		// 祝福
	ENTASK_TRANSFER_TRANSFER,	// 传递
	ENTASK_TRANSFER_FLOOR,		// 盖楼

	ENTASK_PLAY_COMPUTER,		// 人机
	ENTASK_PLAY_TEAM,			// 组队
	ENTASK_PLAY_PK,				// PK
};

// 通知数据类型
ENUM NOTIFY_DATA_TYPE
{
    NOTIFY_TYPE_UNKNOWN,                // 未定义类型
    NOTIFY_TYPE_STEP,                   // 控制步骤类型
    NOTIFY_TYPE_TIP,                    // 信息提示类型

    NOTIFY_TYPE_MAX,                    // 最大类型
};

// 新手引导类型
ENUM ENNOVICEGUIDE_TYPE
{
    ENNOVICEGUIDE_TYPE_GUID_NEWPLAYER =0,		// 进入新手操作场景引导
	ENNOVICEGUIDE_TYPE_GUIDE_ONESTEP,			// 新手随机人机引导
	ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP,			// 进入获得英雄奖励引导
	ENNOVICEGUIDE_TYPE_GUIDE_WAITFORFREEMATCH,  // 等待完成自由对局
	ENNOVICEGUIDE_TYPE_GUIDE_LIFEHEROENTRY,     // 本名英雄入口引导
    ENNOVICEGUIDE_TYPE_GUIDE_LIFEHEROCULTURE,	// 本命英雄仓引导
    ENNOVICEGUIDE_TYPE_GUIDE_LISFHEROALLFIND,	// 等待收集完所有本命英雄
    ENNOVICEGUIDE_TYPE_GUIDE_RANKENTRANCE,		// 排位入口引导
    ENNOVICEGUIDE_TYPE_GUIDE_WAITFORRANK,		// 等待第一场排位比赛结束引导
    
	ENNOVICEGUIDE_TYPE_GUIDE_TEMPSTEP,			// 临时的有实际步骤时往前记得修改

    ENNOVICEGUIDE_TYPE_GUIDE_MAX,

};

// 场外引导记录Key GuideStep.csv
ENUM EMMatchGuideStep
{
    GuideStep_StartWar_FirstEnter	= 1,	   //点新手引导UI进新手一号战场
    GuideStep_RadomMachine_StartWar,           //随机电脑引导点开战UI
    GuideStep_MachineGameModel_Select,         //点新手人机对战选择战场
    GuideStep_RadomMachine_ConfrimHero,        //选择英雄并确定
	GuideStep_HeroReward,                      //新手奖励
	GuideStep_LifeHero_Start,                  //本命英雄激活引导
	GuideStep_LifeHero_Finish,                 //本名英雄收集完成  
	GuideStep_Upgrade_GuideEnd,                //等级升到引导自动结束
	GuideStep_First_Rank_Finished,             //完成排位引导结束新手引导
};

// 复活重激活类型
ENUM RELIVE_ACTIVE_TYPE
{
    ACTIVE_TYPE_UNKNOWN,                // 未定义类型
    ACTIVE_TYPE_BUFF,                   // BUFF类型
    ACTIVE_TYPE_MAX,                    // 最大
};

// 改变英雄信息类型
ENUM CHANGE_HERO_DATA_TYPE
{
    CHANGE_HERO_TYPE_HERO,                // 改变英雄
    CHANGE_HERO_TYPE_SKIN,                // 改变皮肤
    CHANGE_HERO_TYPE_MAX,                 // 最大
};

// 皮肤等级
ENUM SKIN_LEVEL_TYPE
{
	ESLT_NORMAL,							// 普通
	ESLT_EPIC,								// 史诗
	ESLT_LEGEND,							// 传说
	ESLT_ULTIMATE,							// 限量
};

// 实体形态类型
ENUM FORM_TYPE
{
    FORM_GROUND,							// 地面
    FORM_HATHPACE,							// 高台
    FORM_SKY,							    // 高空
    FORM_WATER,							    // 水中
    FORM_MAX,                               // 最大
};

#define POINTSHOP_CARDLIST_MAXCOUNT		10
#define CROWN_PAGE_INLAY_MAX			5			
#define CROWN_PAGE_PRODUCEGEMSTONEMAX	9			

// 玩家宝石系统定义
ENUM  PLAYER_CROWN_DEF
{
    CROWN_PAGE_MAX_COUNT = 10,          /* 皇冠页最大页数 */
    CROWN_PAGE_DEFAULT_COUNT = 2,       /* 默认创建皇冠页数量 */
    CROWN_GEMSTONE_MAX_COUNT = 50,      /* 宝石拥有最大数量 */
    CROWN_GAMESTONE_INVALID_ID = 0,     /* 宝石无效D */
    CROWN_PROPERTY_COUNT_MAX = 20,      /* 属性个数 */
    //CROWN_PAGE_INLAY_MAX = 5,           /*  镶嵌数最大 */
    //CROWN_PAGE_PRODUCEGEMSTONEMAX = 9,  /*  宝石宝箱产出宝石个数 */

};

// 玩家类型
ENUM EPlayerType
{
    EWPT_INVALID,                 // 无效类型
    EWPT_REALPEOPLE,              // 真人类型
    EWPT_COMPUTER,                // 机器人类型
    EWPT_VIRTUALPEOPLE,			  // 模拟真人
    EWPT_MAX,                     // 最大类型
};

ENUM ELifeHeroType
{
	LIFEHERO_MAX_COUNT = 5
};

// 英雄星级最大值
ENUM EHeroStar
{
	HERO_STAR_LEVEL_MAX = 30
};

// 推广员总局数相关数据记录
STRUCT RecommendRecordItem
{
    unsigned int   dwUserID;                           // 下家账号ID
    unsigned int	dwMatchNum;                         // 下家总场次
    unsigned int	dwWinNum;                           // 下家总胜场次

    STRUCT_INIT(RecommendRecordItem)
};

// 推广员总局数相关数据记录
STRUCT RecommendRecord
{
    int    nPrizeTaskData;                   // 推广奖励ID
    int    nCount;                           // 下家记录数
                                             // nCount * RecommendRecordItem
    
    STRUCT_INIT(RecommendRecord)
};

STRUCT ParamRecommendPrize
{
    int nTargetID;                 // 推广目标ID，RecommendPrize.csv 的第一列

    STRUCT_INIT(ParamRecommendPrize)
};

#endif	// __COMMON_ENTITY_MANAGED_DEF_H__