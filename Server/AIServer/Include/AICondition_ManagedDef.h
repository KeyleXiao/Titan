#pragma once
#include "ManagedCode.h"
#include "AI_ManagedDef.h"

// 注:有的条件里有是否设置目标，是因为有的目标不确定，
//    需要搜索出符合条件的目标，假如在条件里就设置了目标，行为里目标就直接配置为
//    目标类型就可以了，不用重新搜索。

DESC("比较")
ENUM COMPARE
{
    DESC("大于等于")
    GREATER_THAN,
    DESC("小于")
    LESS_THAN,
};

DESC("血量/蓝量")
ENUM HP_MP_TYPE
{
    DESC("血量")
    HP_MP_TYPE_HP,
    DESC("蓝量")
    HP_MP_TYPE_MP,
};

DESC("百分比/值")
ENUM VALUE_TYPE
{
    DESC("百分比")
    VALUE_TYPE_RATE,
    DESC("值")
    VALUE_TYPE_VALUE,
};

DESC("实体类型判断")
STRUCT AIConditionServer_EntityType
{
    DESC("条件ID")
    int nID;
    DESC("目标类型")
    AI_TARGET_TYPE eTargetType;
    DESC("保留字段")
    float fReserved;
    DESC("是否是该类型")
    AI_ENTITY_TYPE eEntityType;

    STRUCT_INIT(AIConditionServer_EntityType)
};

DESC("实体阵营关系判断")
STRUCT AIConditionServer_CampRelationship
{
    DESC("条件ID")
    int nID;
    DESC("目标类型")
    AI_TARGET_TYPE eTargetType;
    DESC("阵营关系")
    AI_CAMP_RELATIONSHIP eCampRelation;
    DESC("是否设置目标")
    MANAGED_BOOL eSetTarget;

    STRUCT_INIT(AIConditionServer_CampRelationship)
};

DESC("与目标距离判断")
STRUCT AIConditionServer_Distance2Target
{
    DESC("条件ID")
    int nID;
    DESC("目标类型")
    AI_TARGET_TYPE eTargetType;
    DESC("保留字段")
    float fReserved;
    DESC("比较操作")
    COMPARE eCompare;
    DESC("距离")
    float fDistance;
    DESC("是否设置目标")
    MANAGED_BOOL eSetTarget;

    STRUCT_INIT(AIConditionServer_Distance2Target)
};

DESC("与事件地点距离判断")
STRUCT AIConditionServer_Distance2EventLoc
{
    DESC("条件ID")
    int nID;
    DESC("与事件地点距离大于/小于")
    COMPARE eCompare;
    DESC("距离")
    float fDistance;

    STRUCT_INIT(AIConditionServer_Distance2EventLoc)
};

DESC("与出生点的距离判断")
STRUCT AIConditionServer_Distance2BornLoc
{
    DESC("条件ID")
    int nID;
    DESC("比较操作")
    COMPARE eCompare;
    DESC("距离")
    float fDistance;

    STRUCT_INIT(AIConditionServer_Distance2BornLoc)
};

DESC("自己附近的实体是否存在")
STRUCT AIConditionServer_NearbyEntityExist
{
    DESC("条件ID")
    int nID;
    DESC("实体阵营")
    AI_CAMP_RELATIONSHIP eCamp;
    DESC("实体类型")
    AI_ENTITY_TYPE eEntityType;
    DESC("存在/不存在")
    MANAGED_BOOL eExist;

    STRUCT_INIT(AIConditionServer_NearbyEntityExist)
};

DESC("目标周围实体数量判断")
STRUCT  AIConditionServer_NearbyEntityNum
{
    DESC("条件ID")
    int nID;
    DESC("目标")
    AI_TARGET_TYPE eTargetType;
    DESC("保留字段")
    float fReserved;
    DESC("附近范围")
    float fNearbyRaddi;
    DESC("实体与自己的阵营关系")
    AI_CAMP_RELATIONSHIP eCamp;
    DESC("实体类型")
    AI_ENTITY_TYPE eEntityType;
    DESC("比较操作")
    COMPARE eCompare;
    DESC("个数")
    int nValue;
    DESC("是否设置目标")
    MANAGED_BOOL eSetTarget;

    STRUCT_INIT( AIConditionServer_NearbyEntityNum)
};

DESC("附近实体数量比较")
STRUCT AIConditionServer_NearbyEntityNumCompare
{
    DESC("条件ID")
    int nID;
    DESC("x米范围内")
    float fSearchRaddi;
    DESC("实体类型")
    AI_ENTITY_TYPE eEntityType;
    DESC("大于等于/小于敌方")
    COMPARE eCompare;

    STRUCT_INIT(AIConditionServer_NearbyEntityNumCompare)
};

DESC("血量/蓝量判断")
STRUCT AIConditionServer_HpMp
{
    DESC("条件ID")
    int nID;
    DESC("目标类型")
    AI_TARGET_TYPE eTargetType;
    DESC("保留字段")
    float fReserved;
    DESC("血量/蓝量")
    HP_MP_TYPE eHpMp;
    DESC("比较操作")
    COMPARE eCompare;
    DESC("百分比/血量值")
    VALUE_TYPE eValueType;
    DESC("值")
    float fValue;
    DESC("是否设置目标")
    MANAGED_BOOL eSetTarget;

    STRUCT_INIT(AIConditionServer_HpMp)
};

DESC("自己技能是否在CD")
STRUCT AIConditionServer_SpellCD
{
    DESC("条件ID")
    int nID;
    DESC("技能槽索引")
    int nSlotIndex;
    DESC("是否在CD")
    MANAGED_BOOL eIsCD;

    STRUCT_INIT(AIConditionServer_SpellCD)
};

DESC("是否可以使用物品")
STRUCT AIConditionServer_CanUseItem
{
    DESC("条件ID")
    int nID;
    DESC("物品ID")
    int nItemID;
    DESC("冷却时间(ms)")
    int nCD;

    STRUCT_INIT(AIConditionServer_CanUseItem)
};

DESC("到自己路线的最短距离判断")
STRUCT AIConditionServer_MinDistance2SelfPath
{
    DESC("条件ID")
    int nID;
    DESC("与自己路径最短距离大于等于/小于")
    COMPARE eCompare;
    DESC("距离")
    int nDistance;

    STRUCT_INIT(AIConditionServer_MinDistance2SelfPath)
};

DESC("判断buff是否存在")
STRUCT AIConditionServer_BuffExist
{
    DESC("条件ID")
    int nID;
    DESC("待判断目标")
    AI_TARGET_TYPE eTarget;
    DESC("buffID")
    int nBuffID;
    DESC("是否存在")
    MANAGED_BOOL eExist;
    DESC("buff等级，0表示任意等级")
    int nBuffLevel;

    STRUCT_INIT(AIConditionServer_BuffExist)
};

DESC("判断主人施法技能槽位")
STRUCT AIConditionServer_MasterCastSpellSlot
{
    DESC("条件ID")
    int nID;
    DESC("槽位")
    int nSlotIndex;

    STRUCT_INIT(AIConditionServer_MasterCastSpellSlot)
};

DESC("高度(Y坐标)判断")
STRUCT AIConditionServer_Height
{
    DESC("条件ID")
    int nID;
    DESC("待判断目标")
    AI_TARGET_TYPE eTarget;
    DESC("Y大于等于/Y小于")
    COMPARE eCompare;
    DESC("米")
    float fHeight;

    STRUCT_INIT(AIConditionServer_Height)
};

DESC("某路")
ENUM PATH_DESC
{
    DESC("任意路")
    PATH_ANY,
    DESC("上路")
    PATH_UP,
    DESC("中路")
    PATH_MID,
    DESC("下路")
    PATH_DOWN,
};

DESC("某方某路某塔是否已破")
STRUCT AIConditionServer_TowerLost
{
    DESC("条件ID")
    int nID;
    DESC("某方")
    AI_CAMP_RELATIONSHIP eCamp;
    DESC("某路")
    PATH_DESC ePath;
    DESC("某塔[序号从1开始]")
    int nSN;

    STRUCT_INIT(AIConditionServer_TowerLost)
};

DESC("自己等级条件")
STRUCT AIConditionServer_Level
{
    DESC("条件ID")
    int nID;
    DESC("大于等于/小于")
    COMPARE eCompare;
    DESC("级")
    int nLevel;

    STRUCT_INIT(AIConditionServer_Level)
};
