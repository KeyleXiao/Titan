/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\MobaGoodsSchemeDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/18/2015	16:36
** 版  本:	1.0
** 描  述:	
            Moba战场物品结构定义
********************************************************************/

#pragma once
#include <string>
#include "MobaGoodsDef.h"

// 物品配置
#define MOBA_GOODS_SCHEME_FILENAME "Scp\\MobaGoods.csv"

// 推荐装备配置
#define RECOMMEND_GOOD_SCHEME_FILENAME "Scp\\MobaRecommendGood.csv"

// 效果ID对应效果组分类配置
#define GOOD_EFFECT_GROUPTYPE_SCHEME_FILENAME "Scp\\MobaGoodEffectGroupType.csv"


struct SGoodEffectInfo
{
	int nEffectID;
	emMobaGoodEffectType nEffectType;

	SGoodEffectInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SSchemeMobaGoods
{
	  
	//装备ID
	int	nGoodsID;

	// 装备名称
	char szName[GAME_NAME_MAXSIZE];

	// 物品类型
	emMobaGoodType	nTypeID;  //1、装备 2、物品

	// 装备图标ID
	int nIconID;

	//装备标签组
	int nLabelGourp;

	// 装备子标签
	int nLabel;

	// 英雄专属
	bool bIsHeroLimit;

	// 英雄ID
	int nHeroID;
	
	//职业限制 0：无限制 1、远程 2、近战
	int nVocationTypeLimit;

	// 合成需要装备
	int NeedComposeList[COMPOSE_MAX_COUNT];

	// 能合装备
	int CanComposeList[CAN_COMPOSE_MAX_COUNT];

	// 绝对价格
	int nGoodCost;

	// 出售价格
	int nSoldOutCost;

	// 物品装备属性配置
	SGoodEffectInfo GoodEffectInfo[MAX_EFFECT_COUNT];

	// 装备属性数量 记录数据 
	int nEffectCount;

	// 物品装备移除属性配置
	int GoodRemoveEffectInfo[MAX_EFFECT_COUNT];

    // 继承物品装备属性配置
    SGoodEffectInfo InheritGoodEffectInfo[MAX_EFFECT_COUNT];

    // 继承装备属性数量 记录数据 
    int nInheritEffectCount;

    // 继承物品装备移除属性配置
    int InheritGoodRemoveEffectInfo[MAX_EFFECT_COUNT];

	// 战场限制
	int WarLimit[WAR_LIMIT_MAX];

	// 自动使用(格子不够自动使用)
	emGoodAutoUseType eAutoUseType;

	// 是否是特殊槽位物品
	bool bIsSpecialSlotGood;

	// 能否主动使用
	bool bCanUse;

	//使用是否移除	
	bool bUsedRmove; 

	//是否能叠加
	bool bCanOverlay;

	//最大叠加数量
	int nOverlayCount;

	//最大叠加数量
	int nMaxCount;

	//使用BuffID
	int nUseBuffID;

	//使用Buff技能冷却ID
	int nUseBuffFreezeID;
	
	// 使用技能ID
	int nUseSkillID;

	// 可使用次数
	int nCountCanUse;

	// 可使用次数对应图标组
	int nCanUseIconList[MAX_USE_ICON_LIST_COUNT];

	// 装备描述
	char szDescrption[GAME_NAME_MAXSIZE];

	// 贵重度
	int nPreciousDegree;

    // 标识 MOBA_GOODS_FLAG
    int nFlag;

	SSchemeMobaGoods()
	{
		memset(this, 0, sizeof(*this));
	}
};

//效果分类列表
struct SSchemeGoodEffectGroupType
{
	// 效果种类的名称
	char szName[GAME_NAME_MAXSIZE];
	// 效果ID
	int nEffectID;
	// 效果所属于ID组
	int nEffectGroupID;

	SSchemeGoodEffectGroupType()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 推荐装备配置
struct SSchemeRecommendGood
{
	//英雄ID 
	int nHeroID;
	// 比赛类型组
	int nMatchTypeList[MatchType_MaxID];
	//出门装 
	int EerlyStage[RECOMMEND_GOODS_MAX_COUNT];
	//前期装
	int MiddleStage[RECOMMEND_GOODS_MAX_COUNT];
	//核心装
	int DownWindStage[RECOMMEND_GOODS_MAX_COUNT];
	//常用装 
	int UpWindStage[RECOMMEND_GOODS_MAX_COUNT];
	//消耗品
	int ConsumeGoods[RECOMMEND_GOODS_MAX_COUNT];

	SSchemeRecommendGood()
	{
		memset(this, 0, sizeof(*this));
	}
};