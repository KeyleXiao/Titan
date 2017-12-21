#pragma once


//////////////////////// 怪物归属策略  ///////////////////////////
enum
{
	EMonster_HoldStrategy_Simple = 1,				// 简单策略
	EMonster_HoldStrategy_LastAttack,				// 最后一次攻击权
	EMonster_HoldStrategy_DurativeAttack,			// 持续攻击权
	EMonster_HoldStrategy_Born,						// 出生归属
	EMonster_HoldStrategy_System,					// 系统归属
	EMonster_HoldStrategy_Hurt,						// 伤害（仇恨）归属
	EMonster_HoldStrategy_HurtEx1,					// 伤害（仇恨）归属(记录1个次归属)
	EMonster_HoldStrategy_HurtEx2,					// 伤害（仇恨）归属(记录2个次归属)

	EMonster_HoldStrategy_ExDamage = 10000,			// 伤害分配
	EMonster_HoldStrategy_ExSimple,					// 伤害分配+简单策略
	EMonster_HoldStrategy_ExLastAttack,				// 伤害分配+最后一次攻击权
	EMonster_HoldStrategy_ExDurativeAttack,			// 伤害分配+持续攻击权
	EMonster_HoldStrategy_ExBorn,					// 伤害分配+出生归属
	EMonster_HoldStrategy_ExSystem,					// 伤害分配+系统归属
	EMonster_HoldStrategy_ExHurt,					// 伤害分配+伤害（仇恨）归属
};

//////////////////////// 怪物搜敌策略 //////////////////////////////
enum
{	
	// 本怪物搜到人物后，主动加入仇恨列表
	EMonster_CreepPerson		= 1<<0,			// 搜人
	// 本怪物搜到怪物后，主动加入仇恨列表 
	EMonster_CreepMonster		= 1<<1,			// 搜怪
	// 主动搜索中立方实体
	EMonster_CreepNeutral		= 1<<2,			// 搜索中立方
	// undo it
	EMonster_CreepShen			= 1<<3,			// 周围无人也搜
	// 怪物在战斗状态时，不停止搜敌
	EMonster_CreepInBattle		= 1<<4,			// 战斗状态也搜敌
	// 怪物是否拥有搜索草丛能力
	EMonster_CreepInGrass		= 1<<5,			// 搜索草丛能力
	
};

//////////////////////// 怪物表现策略 //////////////////////////////
enum
{
	EMonster_BehaveStrategy_Hide = 0x0001,			// 隐藏，客户端看不到，但小地图上依然会存在
	EMonster_BehaveStrategy_NoSelectable = 0x0002,	// 客户端无法选取
	EMonster_BehaveStrategy_EmptyBlock = 0x0004,	// 无占位
	EMonster_BehaveStrategy_HideName = 0x0008,		// 隐藏头顶名字
	EMonster_BehaveStrategy_HideHP = 0x0010,		// 隐藏头顶血条
	EMonster_BehaveStrategy_NoDamage = 0x0020,		// 不受伤害，就用群攻都无法伤害
	EMonster_BehaveStrategy_Fly = 0x0040,			// 忽略占位块
	EMonster_BehaveStrategy_HideShadow = 0x0080,	// 隐藏阴影，小地图不显示
	EMonster_BehaveStrategy_HideMinimap = 0x0100,	// 小地图不显示
	EMonster_BehaveStrategy_HideSelect =  0x0200,	// 不显示选中光圈
};
