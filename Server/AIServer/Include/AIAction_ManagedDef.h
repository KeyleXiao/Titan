#pragma once
#include "ManagedCode.h"
#include "AI_ManagedDef.h"
#include "DamageDef_ManagedDef.h"

DESC("巡逻类型")
ENUM PATROL_TYPE
{
    DESC("按路径(按照靠近敌方基地的方向自动选择巡逻方向)")
    EPATROL_ALONG_PATH_FWD_ENEMY_HOME,
    DESC("随机")
    EPATROL_RANDOM,
	DESC("按路径(严格按照路径巡逻)")
	EPATROL_ALONG_PATH_NORMAL,
};

DESC("巡逻行为")
STRUCT AIActionServer_Patrol
{
    DESC("行为ID")
    int nID;

    DESC("巡逻类型")
    PATROL_TYPE ePatrolType;

    DESC("值(按路径=保留;随机=半径)")
    int nValue;

    DESC("保留字段")
    int nReserved;

    STRUCT_INIT(AIActionServer_Patrol)
};

DESC("回归行为")
STRUCT AIActionServer_Retrive
{
    DESC("行为ID")
    int nID;
    DESC("是否需要回血回魔")
    MANAGED_BOOL eFullHpMp;
    DESC("检测间隔(检测回归结束)")
    int nCheckInterval;

    STRUCT_INIT(AIActionServer_Retrive) 
};

DESC("使用技能行为")
STRUCT AIActionServer_CastSpell
{
    DESC("行为ID")
    int nID;
    DESC("目标类型")
    AI_TARGET_TYPE eBestTargetType;
    DESC("备选目标类型")
    AI_TARGET_TYPE eAltTargetType;
    DESC("是否持续攻击")
    MANAGED_BOOL eKeepAttack;
    DESC("攻击间隔ms/僵直时间(B回城不需要再配空闲行为)")
    int nAttackInterval;
    DESC("是否指定技能")
    MANAGED_BOOL eSpecifySpell;
    DESC("技能槽索引")
    int nSlotIndex;
    DESC("是否靠近目标")
    MANAGED_BOOL eKeepClose;
    DESC("攻击过程中是否检测回归")
    MANAGED_BOOL eCheckRetrieve;
    DESC("贴近距离")
    float fCloseDis;
    DESC("终止距离,与目标距离太大则终止")
    float fStopDis;
    DESC("是否锁定目标")
    MANAGED_BOOL eLockTarget;
    DESC("贴近距离增减范围米(float)")
    float fCloseRandRange;
    DESC("多久没攻击到放弃ms(0表示无限制)")
    int nQuitInterval;

    STRUCT_INIT(AIActionServer_CastSpell)
};

DESC("靠近目标行为")
STRUCT AIActionServer_ClostTo
{
    DESC("行为ID")
    int nID;
    DESC("靠近目标类型")
    AI_TARGET_TYPE eTargetType;
    DESC("是否跟随")
    MANAGED_BOOL eKeepFollow;
    DESC("靠近过程中是否检测回归")
    MANAGED_BOOL eCheckRetrieve;
    DESC("时间间隔ms")
    int nCloseInterval;
    DESC("期望贴近距离")
    float fCloseDis;
    DESC("允许的最大位移(米)") 
    float fDisFromStartPointLimit;

    STRUCT_INIT(AIActionServer_ClostTo)
};

DESC("起跳")
STRUCT AIActionServer_Jump
{
    DESC("行为ID")
    int nID;
    DESC("朝向的目标")
    AI_TARGET_TYPE eTargetType;
    DESC("是否自动检测着地")
    MANAGED_BOOL eCheckGround;
    DESC("地面海拔高度(不自动检测着地时用)")
    float fGroundY;

    STRUCT_INIT(AIActionServer_Jump)
};

DESC("升级技能行为")
STRUCT AIActionServer_UpgradeSpell
{
    DESC("行为ID")
    int nID;
    DESC("1级升技能槽索引")
    int n1SlotIndex;
    DESC("2级升技能槽索引")
    int n2SlotIndex;
    DESC("3级升技能槽索引")
    int n3SlotIndex;
    DESC("4级升技能槽索引")
    int n4SlotIndex;
    DESC("5级升技能槽索引")
    int n5SlotIndex;
    DESC("6级升技能槽索引")
    int n6SlotIndex;
    DESC("7级升技能槽索引")
    int n7SlotIndex;
    DESC("8级升技能槽索引")
    int n8SlotIndex;
    DESC("9级升技能槽索引")
    int n9SlotIndex;
    DESC("10级升技能槽索引")
    int n10SlotIndex;
    DESC("11级升技能槽索引")
    int n11SlotIndex;
    DESC("12级升技能槽索引")
    int n12SlotIndex;
    DESC("13级升技能槽索引")
    int n13SlotIndex;
    DESC("14级升技能槽索引")
    int n14SlotIndex;
    DESC("15级升技能槽索引")
    int n15SlotIndex;
    DESC("16级升技能槽索引")
    int n16SlotIndex;
    DESC("17级升技能槽索引")
    int n17SlotIndex;
    DESC("18级升技能槽索引")
    int n18SlotIndex;
    DESC("19级升技能槽索引")
    int n19SlotIndex;
    DESC("20级升技能槽索引")
    int n20SlotIndex;

    STRUCT_INIT(AIActionServer_UpgradeSpell) 
};

DESC("购买物品行为（作废）")
STRUCT AIActionServer_BuyItem
{
    DESC("行为ID")
    int nID;
    DESC("血瓶ID")
    short nHpBottleID;
    DESC("血瓶数量")
    short nHpBottleNum;
    DESC("魔瓶ID")
    short nMpBottleID;
    DESC("魔瓶数量")
    short nMpBottleNum;

    DESC("购买第1件前需要卖出")
    short nItem1Sell;
    DESC("第1件买入ID")
    short nItem1Buy;
    DESC("第1件花费")
    short nItem1Cost;
    
    DESC("购买第2件前需要卖出")
    short nItem2Sell;
    DESC("第2件买入ID")
    short nItem2Buy;
    DESC("第2件花费")
    short nItem2Cost;

    DESC("购买第3件前需要卖出")
    short nItem3Sell;
    DESC("第3件买入ID")
    short nItem3Buy;
    DESC("第3件花费")
    short nItem3Cost;

    DESC("购买第4件前需要卖出")
    short nItem4Sell;
    DESC("第4件买入ID")
    short nItem4Buy;
    DESC("第4件花费")
    short nItem4Cost;

    DESC("购买第5件前需要卖出")
    short nItem5Sell;
    DESC("第5件买入ID")
    short nItem5Buy;
    DESC("第5件花费")
    short nItem5Cost;

    DESC("购买第6件前需要卖出")
    short nItem6Sell;
    DESC("第6件买入ID")
    short nItem6Buy;
    DESC("第6件花费")
    short nItem6Cost;

    DESC("购买第7件前需要卖出")
    short nItem7Sell;
    DESC("第7件买入ID")
    short nItem7Buy;
    DESC("第7件花费")
    short nItem7Cost;

    DESC("购买第8件前需要卖出")
    short nItem8Sell;
    DESC("第8件买入ID")
    short nItem8Buy;
    DESC("第8件花费")
    short nItem8Cost;

    DESC("购买第9件前需要卖出")
    short nItem9Sell;
    DESC("第9件买入ID")
    short nItem9Buy;
    DESC("第9件花费")
    short nItem9Cost;

    DESC("购买第10件前需要卖出")
    short nItem10Sell;
    DESC("第10件买入ID")
    short nItem10Buy;
    DESC("第10件花费")
    short nItem10Cost;

    DESC("购买第11件前需要卖出")
    short nItem11Sell;
    DESC("第11件买入ID")
    short nItem11Buy;
    DESC("第11件花费")
    short nItem11Cost;

    DESC("购买第12件前需要卖出")
    short nItem12Sell;
    DESC("第12件买入ID")
    short nItem12Buy;
    DESC("第12件花费")
    short nItem12Cost;

    DESC("购买第13件前需要卖出")
    short nItem13Sell;
    DESC("第13件买入ID")
    short nItem13Buy;
    DESC("第13件花费")
    short nItem13Cost;

    DESC("购买第14件前需要卖出")
    short nItem14Sell;
    DESC("第14件买入ID")
    short nItem14Buy;
    DESC("第14件花费")
    short nItem14Cost;

    DESC("购买第15件前需要卖出")
    short nItem15Sell;
    DESC("第15件买入ID")
    short nItem15Buy;
    DESC("第15件花费")
    short nItem15Cost;

    DESC("购买第16件前需要卖出")
    short nItem16Sell;
    DESC("第16件买入ID")
    short nItem16Buy;
    DESC("第16件花费")
    short nItem16Cost;

    DESC("购买第17件前需要卖出")
    short nItem17Sell;
    DESC("第17件买入ID")
    short nItem17Buy;
    DESC("第17件花费")
    short nItem17Cost;

    DESC("购买第18件前需要卖出")
    short nItem18Sell;
    DESC("第18件买入ID")
    short nItem18Buy;
    DESC("第18件花费")
    short nItem18Cost;

    DESC("购买第19件前需要卖出")
    short nItem19Sell;
    DESC("第19件买入ID")
    short nItem19Buy;
    DESC("第19件花费")
    short nItem19Cost;

    DESC("购买第20件前需要卖出")
    short nItem20Sell;
    DESC("第20件买入ID")
    short nItem20Buy;
    DESC("第20件花费")
    short nItem20Cost;

    DESC("购买第21件前需要卖出")
    short nItem21Sell;
    DESC("第21件买入ID")
    short nItem21Buy;
    DESC("第21件花费")
    short nItem21Cost;

    DESC("购买第22件前需要卖出")
    short nItem22Sell;
    DESC("第22件买入ID")
    short nItem22Buy;
    DESC("第22件花费")
    short nItem22Cost;

    DESC("购买第23件前需要卖出")
    short nItem23Sell;
    DESC("第23件买入ID")
    short nItem23Buy;
    DESC("第23件花费")
    short nItem23Cost;

    DESC("购买第24件前需要卖出")
    short nItem24Sell;
    DESC("第24件买入ID")
    short nItem24Buy;
    DESC("第24件花费")
    short nItem24Cost;

    DESC("购买第25件前需要卖出")
    short nItem25Sell;
    DESC("第25件买入ID")
    short nItem25Buy;
    DESC("第25件花费")
    short nItem25Cost;

    DESC("购买第26件前需要卖出")
    short nItem26Sell;
    DESC("第26件买入ID")
    short nItem26Buy;
    DESC("第26件花费")
    short nItem26Cost;

    DESC("购买第27件前需要卖出")
    short nItem27Sell;
    DESC("第27件买入ID")
    short nItem27Buy;
    DESC("第27件花费")
    short nItem27Cost;

    DESC("购买第28件前需要卖出")
    short nItem28Sell;
    DESC("第28件买入ID")
    short nItem28Buy;
    DESC("第28件花费")
    short nItem28Cost;

    DESC("购买第29件前需要卖出")
    short nItem29Sell;
    DESC("第29件买入ID")
    short nItem29Buy;
    DESC("第29件花费")
    short nItem29Cost;

    DESC("购买第30件前需要卖出")
    short nItem30Sell;
    DESC("第30件买入ID")
    short nItem30Buy;
    DESC("第30件花费")
    short nItem30Cost;

    DESC("购买第31件前需要卖出")
    short nItem31Sell;
    DESC("第31件买入ID")
    short nItem31Buy;
    DESC("第31件花费")
    short nItem31Cost;

    DESC("购买第32件前需要卖出")
    short nItem32Sell;
    DESC("第32件买入ID")
    short nItem32Buy;
    DESC("第32件花费")
    short nItem32Cost;

    STRUCT_INIT(AIActionServer_BuyItem)
};

DESC("购买物品行为(读csv配置)")
STRUCT AIActionServer_BuyItems
{
    DESC("行为ID")
    int nID;

    STRUCT_INIT(AIActionServer_BuyItems)
};

DESC("使用物品行为")
STRUCT AIActionServer_UseItem
{
    DESC("行为ID")
    int nID;
    DESC("物品ID")
    int nItemID;

    STRUCT_INIT(AIActionServer_UseItem)
};

DESC("创建npc行为")
STRUCT AIActionServer_CreateNpc
{
    DESC("行为ID")
    int nID;
    DESC("NPCID")
    int nNpcID;
    DESC("保留字段")
    int nSubType;
    DESC("npc等级")
    int nLevel;
    DESC("图标ID")
    int nIcon;
    DESC("朝向x")
    float fDirX;
    DESC("朝向y")
    float fDirY;
    DESC("朝向z")
    float fDirZ;

    STRUCT_INIT(AIActionServer_CreateNpc)
};

DESC("销毁自身行为(自杀)")
STRUCT AIActionServer_DestroySelf
{
    DESC("行为ID")
    int nID;
    DESC("存活时间ms")
    int nLifeTime;

    DESC("伤害类型DAMAGE_TYPE")
    int nDamageType;

    DESC("技能效果类型SPELL_EFFECT_TYPE")
    int	nSpellEffectType;

    DESC("死亡效果类型")
    DeadEffectType nDeadType;

    STRUCT_INIT(AIActionServer_DestroySelf)
};

DESC("加buff")
STRUCT AIActionServer_AddBuff
{
    DESC("行为ID")
    int nID;
    DESC("目标")
    AI_TARGET_TYPE eTargetType;
    DESC("保留字段")
    float fReserved;
    DESC("buffID")
    int nBuffID;
    DESC("buff等级")
    int nBuffLevel;
    DESC("增加/移除")
    MANAGED_BOOL eAdd;

    STRUCT_INIT(AIActionServer_AddBuff)
};

DESC("空闲行为")
STRUCT AIActionServer_Idle
{
    DESC("行为ID")
    int nID;
    DESC("空闲时间ms")
    int nIdleTime;

    STRUCT_INIT(AIActionServer_Idle)
};


DESC("警戒行为(视野内有情况持续报警无情况解除报警)")
STRUCT AIActionServer_WatchOut
{
    DESC("行为ID")
    int nID;
    DESC("监控哪个阵营的实体")
    AI_CAMP_RELATIONSHIP eCamp;
    DESC("监控哪种实体")
    AI_ENTITY_TYPE eEntityType;
    DESC("搜索间隔ms")
    int nInterval;

    STRUCT_INIT(AIActionServer_WatchOut)
};

DESC("警戒敌人行为(开始监控直到发现敌人报警)")
STRUCT AIActionServer_WatchOutEnemy
{
    DESC("行为ID")
    int nID;
    DESC("搜敌间隔ms")
    int nInterval;
    DESC("警戒距离")
    int nDistance;
	DESC("警戒AI类型掩码")
	int nMask;

    STRUCT_INIT(AIActionServer_WatchOutEnemy)
};

DESC("预警行为[范围内己方英雄数小于敌方广播预警事件]")
STRUCT AIActionServer_RadarScan
{
    DESC("行为ID")
    int nID;
    DESC("扫描间隔ms(预警可以配的间隔长一些)")
    int nInterval;
    DESC("预警扫描半径m(预警需要配的半径大一些)")
    int nScanRaddi;
    DESC("预警时这个范围内有敌方附带紧急标志")
    int nEmergentRaddi;

    STRUCT_INIT(AIActionServer_RadarScan)
};

//---------------------------------------------------------------

DESC("建筑物位置")
ENUM BUILDING_POS
{
    DESC("最外层的")
    OUTER_MOST,
    DESC("最内层的")
    INNER_MOST,
};

DESC("选择最近的线")
STRUCT AIActionServer_ChooseNearestPath
{
    DESC("行为ID")
    int nID;
    DESC("线路1")
    int nPath1;
    DESC("线路2")
    int nPath2;
    DESC("线路3")
    int nPath3;

    STRUCT_INIT(AIActionServer_ChooseNearestPath)
};

DESC("范围广播一下进攻信号")
STRUCT AIActionServer_BroadcastAttackSignal
{
    DESC("行为ID")
    int nID;
    DESC("攻击目标")
    AI_TARGET_TYPE eTargetType;
    DESC("广播范围(米)")
    int nBroadcastRange;

    STRUCT_INIT(AIActionServer_BroadcastAttackSignal)
};

DESC("英雄分配初始线路")
STRUCT AIActionServer_AllotPath
{
    DESC("行为ID")
    int nID;

    STRUCT_INIT(AIActionServer_AllotPath)
};

DESC("动态选线,推线策略里的并行行为，选线规则暂时写死")
STRUCT AIActionServer_SelectPath
{
    DESC("行为ID")
    int nID;

    STRUCT_INIT(AIActionServer_SelectPath)
};

DESC("与目标保持距离")
STRUCT AIActionServer_KeepDistance
{
    DESC("行为ID")
    int nID;
    DESC("目标")
    AI_TARGET_TYPE eTarget;
    DESC("最小距离,单位米")
    float fMinDis;
    DESC("最大距离,单位米")
    float fMaxDis;

    STRUCT_INIT(AIActionServer_KeepDistance)
};

DESC("激活天赋")
STRUCT AIActionServer_ActiveTalent
{
    DESC("行为ID")
    int nID;
    DESC("1级天赋")
    int nTalent_1;
    DESC("2级天赋")
    int nTalent_2;
    DESC("3级天赋")
    int nTalent_3;
    DESC("4级天赋")
    int nTalent_4;
    DESC("5级天赋")
    int nTalent_5;
    DESC("6级天赋")
    int nTalent_6;
    DESC("7级天赋")
    int nTalent_7;
    DESC("8级天赋")
    int nTalent_8;
    DESC("9级天赋")
    int nTalent_9;
    DESC("10级天赋")
    int nTalent_10;
    DESC("11级天赋")
    int nTalent_11;
    DESC("12级天赋")
    int nTalent_12;
    DESC("13级天赋")
    int nTalent_13;
    DESC("14级天赋")
    int nTalent_14;
    DESC("15级天赋")
    int nTalent_15;
    DESC("16级天赋")
    int nTalent_16;
    DESC("17级天赋")
    int nTalent_17;
    DESC("18级天赋")
    int nTalent_18;
    DESC("19级天赋")
    int nTalent_19;
    DESC("20级天赋")
    int nTalent_20;

    STRUCT_INIT(AIActionServer_ActiveTalent)
};

DESC("执行内部AI行为")
STRUCT AIActionServer_InnerAILogic
{
    DESC("行为ID")
    int nID;
    DESC("内部AISchemeID")
    int nInnerAIScheme;

    STRUCT_INIT(AIActionServer_InnerAILogic)
};

DESC("捡起(实体)行为")
STRUCT AIActionServer_PickUpItem
{
    DESC("行为ID")
    int nID;
    DESC("目标")
    AI_TARGET_TYPE eTargetType;

    STRUCT_INIT(AIActionServer_PickUpItem)
};

DESC("敌方视野检测,在敌方视野内则发进敌方视野事件给自己(警戒)")
STRUCT AIActionServer_SightCheck
{
    DESC("行为ID")
    int nID;

    STRUCT_INIT(AIActionServer_SightCheck)
};

DESC("进入中团阶段")
STRUCT AIActionServer_AllMid
{
    DESC("行为ID")
    int nID;

    STRUCT_INIT(AIActionServer_AllMid)
};
