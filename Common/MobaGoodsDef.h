/*******************************************************************
** 文件名:	MobaGoodsDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇敢
** 日  期:	5/26/2015
** 版  本:	1.0
** 描  述:	

			战场物品定义相关文件
********************************************************************/

#pragma once


#include "IEntity.h"
#include "ISchemeCenter.h"
#include "WarDef.h"
#include "IEffect.h"


//装备合成最大数量
#define COMPOSE_MAX_COUNT			5

#define CAN_COMPOSE_MAX_COUNT		20

#define MAX_EFFECT_COUNT			10

#define RECOMMEND_GOODS_MAX_COUNT	10

#define INVALID_MOBA_GOOD_ID		0

#define MAX_USE_ICON_LIST_COUNT		5

#define WAR_LIMIT_MAX				5

enum emGoodAutoUseType
{
    GAT_NoneType= 0,
    GAT_SlotNotEnoughAutoUse,
    GAT_ForceAutoUse,
    GAT_MAX,
};

enum emMobaGoodType
{
	MGT_NoneType= 0,
	MGT_EquipmentType,
	MGT_LeechdomType,
    MGT_MAX,
};

enum enMobaGoodBuyType
{
    MGBT_BUY = 0,               // 购买类型
    MGBT_SELL,                  // 出售类型
    MGBT_REVOCATION_BUY,        // 撤销购买物品操作
    MGBT_REVOCATION_SELL,       // 撤销卖出物品操作
};

enum emMobaGoodEffectType
{
	MGET_NoneType = 0,
	MGET_BaseEffect,            // 基础效果
	MGET_IDUniqueEffect,        // ID唯一
	MGET_GroupUniqueEffect,     // 组唯一
};

enum emMobaGoodPreciousDegree
{
	MGPD_HIGH = 0,				// 大件物品
	MGPD_LOW = 1,				// 小件物品
};

struct SMobaGoodEffectInfo
{
	int nEffectID;
	emMobaGoodEffectType nEffectType;
	bool bIsOpen;

	SMobaGoodEffectInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SPropInfo
{
    UID TargetUID;              // 目标UID
	int nSlot;                  // 槽位ID
	int nEffectID;              // 效果ID
	int nEntityRecEffectIndex;    // 实体记录的效果ID

	SPropInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SSlotRevocationInfo
{
	int nSlotIndex;
	int nGoodID;
	int nCount;
	emMobaGoodType GoodType;

    SSlotRevocationInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SExchange
{
    int srcIndex;
    int destIndex;

    SExchange()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SRevocationInfo
{

	EWarRevocationType RevocationType;
	bool bisAddOverlayCount;
	int nTalentPointChange;
	list<SSlotRevocationInfo> removeGoodsList;
	list<SSlotRevocationInfo> addGoodsList;
    SExchange ExchangeInfo;

	SRevocationInfo()
	{
        RevocationType = EWRT_None;
        bisAddOverlayCount = false;
        nTalentPointChange = 0;
	}
//     ~SRevocationInfo()
//     {
//         if(pListRemoveGoodsList) { delete pListRemoveGoodsList; pListRemoveGoodsList = NULL;}
//         if(pListAddGoodsList) { delete pListAddGoodsList; pListAddGoodsList = NULL;}
//     }
};

struct SEffectCDOverlay
{
    int nLeftCanUseCount;
    int nLeftCDTime;

    SEffectCDOverlay()
    {
        memset(this, 0, sizeof(*this));
    }
};