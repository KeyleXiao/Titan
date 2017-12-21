#pragma once
#include "ManagedCode.h"
#include "GameDef.h"

DESC("是否")
ENUM MANAGED_BOOL
{
    DESC("否")
    ENUM_NO,
    DESC("是")
    ENUM_YES,
};

DESC("AI消息")
ENUM AI_MESSAGE
{
    DESC("移动无法启动消息")
    AI_MESSAGE_MOVE_START_FAIL,
    DESC("停止移动消息")
    AI_MESSAGE_MOVE_STOPED,
};

DESC("AI事件")
ENUM AI_EVENT
{
    DESC("帧事件")
    AI_EVENT_FRAME_UPDATE,
    DESC("出生事件")
    AI_EVENT_BORN,
    DESC("死亡事件")
    AI_EVENT_DIE,
    DESC("复活事件")
    AI_EVENT_REBORN,
    DESC("自己被攻击事件")
    AI_EVENT_BEHIT,  
    DESC("友方单位被攻击(防御塔被攻击全图广播给假人/假人和野怪被攻击都是附近广播)")
    AI_EVENT_FRIEND_ENTITY_BEHIT,
    DESC("敌方单位被攻击")
    AI_EVENT_ENEMY_ENTITY_BEHIT,
    DESC("自己升级事件")
    AI_EVENT_LEVEL_UPGRADE,
    DESC("检测回归事件")
    AI_EVENT_CHECK_RETRIEVE,
    DESC("回归结束事件")
    AI_EVENT_RETRIEVE_END,
    DESC("被地形卡住事件")
    AI_EVENT_GOT_STUCK,
    DESC("空闲结束事件")
    AI_EVENT_IDLE_END,
    DESC("警戒报警事件")
    AI_EVENT_WATCHOUT_ALARM_FIRE,
    DESC("警戒报警解除事件")
    AI_EVENT_WATCHOUT_ALARM_CANCEL,
    DESC("收到主人标记攻击事件")
    AI_EVENT_RECV_MASTER_MARK_ATTACK_SIGNAL,
    DESC("被捡起事件")
    AI_EVENT_PICKED_UP,
    DESC("建筑物被摧毁事件")
    AI_EVENT_BUILDING_DESTROIED,
    DESC("收到进攻信号事件")
    AI_EVENT_ATTACK_SIGNAL,
    DESC("主人攻击事件")
    AI_EVENT_MASTER_ATTACK,
    DESC("收到请求跟随事件")
    AI_EVENT_RECV_REQ_FOLLOW_OTHER,
    DESC("收到取消跟随事件")
    AI_EVENT_RECV_BREAK_FOLLOW_OTHER,
    DESC("自己进入敌方视野")
    AI_EVENT_ENTER_ENEMY_SIGHT,
    DESC("自己离开敌方视野")
    AI_EVENT_LEAVE_ENEMY_SIGHT,
    DESC("空闲事件")
    AI_EVENT_IDLE,
    DESC("扫描装置报警事件")
    AI_EVENT_RADAR_ALARM,
    DESC("主人死亡事件")
    AI_EVENT_MASTER_DIE,

	////////////新增英雄AI事件///////////////
	DESC("开火事件")
	AI_EVENT_FIRE,
	DESC("已回城事件")
	AI_EVENT_BACK_HOME,
	DESC("(大龙小龙)被攻击")
	AI_EVENT_BOSS_BEHIT,
	DESC("(英雄)脱离战斗")
	AI_EVENT_EXIT_FIGHT,

    ////////////////////////////////////////
    DESC("推塔小龙出现")
    AI_EVENT_SUMMON_SMALL_DRAGON_BORN,

    DESC("进入中团阶段")
    AI_EVENT_ENTER_ALL_MID_STAGE,

    DESC("think事件")
    AI_EVENT_THINK,
};


DESC("策略类型")
ENUM AI_DECISION_TYPE
{
    DESC("无类型")
    AI_DECISION_TYPE_NONE,
    DESC("移动策略")
    AI_DECISION_TYPE_MOVE,
    DESC("施法策略")
    AI_DECISION_TYPE_CAST_SPELL,
    DESC("自定义类型1")
    AI_DECISION_TYPE_CUSTOM_1,
    DESC("自定义类型2")
    AI_DECISION_TYPE_CUSTOM_2,
    DESC("策略类型最大值")
    AI_DECISION_TYPE_MAX,
};

DESC("阵营关系")
ENUM AI_CAMP_RELATIONSHIP
{
    DESC("任意阵营")
    AI_ANY_CAMP,
    DESC("己方")
    AI_SELF_CAMP,
    DESC("敌方")
    AI_ENEMY_CAMP,
    DESC("阵营关系最大值(不可选)")
    AI_CAMP_MAX,
};

DESC("实体类型")
ENUM AI_ENTITY_TYPE
{
	DESC("任意类型(小兵、建筑、英雄、小龙)")
	AI_ENTITY_TYPE_ANY,
	DESC("英雄")
	AI_ENTITY_TYPE_HERO,
	DESC("小兵")
	AI_ENTITY_TYPE_SOLDIER,
	DESC("防御塔")
	AI_ENTITY_TYPE_DEFENSIVE_TOWER,
	DESC("基地")
	AI_ENTITY_TYPE_BASE,
	DESC("水晶")
	AI_ENTITY_TYPE_CRYSTAL_TOWER,
	DESC("野生怪")
	AI_ENTITY_TYPE_WILD_MONSTER,
	DESC("眼石")
	AI_ENTITY_TYPE_EYE_STONE,
	DESC("血包")
	AI_ENTITY_TYPE_HP_SUPPLIES,
	DESC("小龙")
	AI_ENTITY_TYPE_SMALL_DRAGON,
	DESC("大龙")
	AI_ENTITY_TYPE_HUGE_DRAGON,
	DESC("泉水")
	AI_ENTITY_TYPE_SPRING,
    DESC("推塔小龙")
    AI_ENTITY_TYPE_SUMMON_SMALL_DRAGON,
	DESC("未知类型")
	AI_ENTITY_TYPE_UNKNOWN,
	DESC("实体类型最大值(不可选)")
	AI_ENTITY_TYPE_MAX,
};

DESC("实体类型")
ENUM AI_ENTITY_TYPE_MASK
{
	DESC("未知类型")
	AI_ENTITY_TYPE_UNKNOWN_M = 0,
	DESC("泉水")
	AI_ENTITY_TYPE_SPRING_M = 0x1,
	DESC("基地")
	AI_ENTITY_TYPE_BASE_M = 0x2,
	DESC("防御塔")
	AI_ENTITY_TYPE_DEFENSIVE_TOWER_M = 0x4,
	DESC("水晶")
	AI_ENTITY_TYPE_CRYSTAL_TOWER_M = 0x8,
    DESC("英雄")
    AI_ENTITY_TYPE_HERO_M = 0x10,
    DESC("小兵")
    AI_ENTITY_TYPE_SOLDIER_M = 0x20,
	DESC("召唤小龙")
	AI_ENTITY_TYPE_SUMMON_SMALL_DRAGON_M = 0x40,
    DESC("野生怪")
    AI_ENTITY_TYPE_WILD_MONSTER_M = 0x80,
	DESC("小龙")
	AI_ENTITY_TYPE_SMALL_DRAGON_M = 0x100,
	DESC("大龙")
	AI_ENTITY_TYPE_HUGE_DRAGON_M = 0x200,
    DESC("眼石")
    AI_ENTITY_TYPE_EYE_STONE_M = 0x400,
    DESC("血包")
    AI_ENTITY_TYPE_HP_SUPPLIES_M = 0x800,
    DESC("实体类型最大值(不可选)")
    AI_ENTITY_TYPE_MAX_M,
};

DESC("目标类型")
ENUM AI_TARGET_TYPE
{
    DESC("没有目标")
    AI_TARGET_NO,
    DESC("自己")
    AI_TARGET_SELF,
    DESC("操作者")
    AI_TARGET_OPERATOR,
    DESC("目标")
    AI_TARGET_TARGET,
    DESC("(阵营视野范围内)最近的友方英雄")
    AI_TARGET_NEAREST_FRIEND_HERO,
    DESC("(阵营视野范围内)最近的友方防御塔")
    AI_TARGET_NEAREST_FRIEND_DEFENSIVE_TOWER,
    DESC("(自己视野范围内)最残血的友方英雄")
    AI_TARGET_LOWEST_HP_FRIEND_HERO,
    DESC("(阵营视野范围内)最近的敌方英雄")
    AI_TARGET_NEAREST_ENEMY_HERO,
    DESC("(自己视野范围内)最近的敌方小兵")
    AI_TARGET_NEAREST_ENEMY_SOLDIER,
    DESC("(自己视野范围内)最近的敌方单位")
    AI_TARGET_NEAREST_ENEMY_ENTITY,
    DESC("(自己视野范围内)最残血的敌方英雄")
    AI_TARGET_LOWEST_HP_ENEMY_HERO,
    DESC("(自己视野范围内)最残血的敌方小兵")
    AI_TARGET_LOWEST_HP_ENEMY_SOLDIER,
    DESC("(阵营视野范围内)最近的敌方防御塔")
    AI_TARGET_NEAREST_ENEMY_DEFENSIVE_TOWER,
    DESC("自己视野范围内任意友方单位")
    AI_TARGET_ANY_FRIEND_ENTITY,
    DESC("前线友方单位")
    AI_TARGET_FRONT_FRIEND_ENTITY,
    DESC("主人")
    AI_TARGET_MASTER,
    DESC("需要支援的目标")
    AI_TARGET_NEED_HELP,
    DESC("主人标记的目标")
    AI_TARGETT_MARKED_BY_MASTER,
    DESC("己方基地")
    AI_TARGET_SELF_BASE,
    DESC("敌方基地")
    AI_TARGET_ENEMY_BASE,
    DESC("(阵营视野范围内)最近的敌方水晶")
    AI_TARGET_NEAREST_ENEMY_CRYSTAL_TOWER,
    DESC("(自己视野范围内)最近的血包")
    AI_TARGET_NEAREST_HP_SUPPLIES,
    DESC("(阵营视野范围内)最近的敌方小龙")
    AI_TARGET_NEAREST_ENEMY_SMALL_DRAGON,
    DESC("(自己视野范围内)最近的敌方小兵或小龙--防御塔首选目标") // 暂时
    AI_TARGET_NEAREST_ENEMY_SOLDIER_OR_SAMLLDRAGON,
    DESC("前线敌方单位")
    AI_TARGET_FRONT_ENEMY_ENTITY,
    DESC("中立小龙")
    AI_TARGET_NEUTRAL_SMALL_DRAGON,
    DESC("中立大龙")
    AI_TARGET_NEUTRAL_HUGE_DRAGON,
    DESC("(全图)最近的野怪")
    AI_TARGET_NEAREST_WILD_MONSTER_GLOBAL,
    DESC("(自己视野内)最残血的野怪")
    AI_TARGET_LOWEST_HP_WILD_MONSTER,
	DESC("推荐最佳攻击目标")
	AI_BEST_ATTACK_TARGET,
	DESC("推荐攻击目标_A")
	AI_ATTACK_TARGET_A,
	DESC("推荐攻击目标_Q")
	AI_ATTACK_TARGET_Q,
	DESC("推荐攻击目标_E")
	AI_ATTACK_TARGET_E,
	DESC("推荐攻击目标R")
	AI_ATTACK_TARGET_R,
	DESC("AI英雄的当前最需要保护队友")
	AI_HERO_BEST_PROTECT_HERO,
	DESC("距离AI英雄最近的袭击者")
	AI_HERO_NEAREST_ATTACKER,
	DESC("最近的敌方NPC")
	AI_TARGET_NEAREST_ENEMY_NPC = 100,
    DESC("目标类型最大值")
    AI_TARGET_MAX = 256,
    DESC("任务地点")
    AI_TARGET_TASK_LOC,
};

struct CAIProxy; 
class AIDecision;
// AI条件、行为上下文
STRUCT AI_CONTEXT
{
    int        nEventID;
    CAIProxy*  pAIProxy; 
    AIDecision* pDecision;
    UID        uidOperator;
    UID        uidTarget;
    int        nSchemeID;
    int        nDecisionSN;
    sp_float_array(loc, 3);
    int        nParam1;
    int        nParam2;
    int        nParam3;

    STRUCT_INIT(AI_CONTEXT)
};