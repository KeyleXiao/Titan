/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 陈俊涛
** 日  期: 2015/3/13
** 版  本: 1.0
** 描  述: 战场解析文件数据定义
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "WarDef.h"
#include <string>

// SchemeWarManager
#define WARMANAGER_SCHEME_FILENAME "Scp\\WarManager.csv"
struct SWarScheme
{
	// 战场ID
	int nStaticID;
	// 战场名称
	char szName[GAME_NAME_MAXSIZE];			// 预设体路径

	// 游戏类型
	int nGameType;
	// 玩法类型
	int nPlayType;
	// 开始时间
	UINT nStartTime;
	// 倒计时,提示时间
	UINT nPromptTime;
	// 结束滞留时间
	UINT nCleanTime;

	SWarScheme()
	{
		memset(this,0,sizeof(*this));
	};
};


// SchemeWarInfo
#define WARINFO_SCHEME_FILENAME "Scp\\WarInfo.csv"

// 建筑死亡复活事件
enum EWarInfoEvent
{
	EWIE_NONE = 0,
	EWIE_Protect,			// 保护的事件, 保护的塔显露出来
	EWIE_OpenRefresh,		// 设置开启刷新
	EWIE_ProtectBase,		// 保护基地
	EWIE_MutexProtectBase,	//保护基地互斥事件
};

// 怪物处理方式
enum EWarMonsterExec
{
	EWME_None = 0,		// 不存在的处理方式
	EWME_Base,			// 基地的处理方式
	EWME_Tower,			// 建筑的处理方式
	EWME_Soldier,		// 士兵的处理方式
	EWME_Monster,		// 野怪的处理方式
};

// 经验增加类型类型
enum EWarAddEXPType
{
	EWEXP_None = 0,		// 不存在的处理方式
	EWEXP_Alone,		// 单独
	EWEXP_Around,		// 周围
	EWEXP_CampAll,		// 己方全部
};

// 掉落天赋类型
enum EWarTalentType
{
	EWTT_None = 0,		// 不存在的处理方式
	EWTT_Alone,			// 单独
	EWTT_Around,		// 周围
	EWTT_CampAll,		// 己方全部
	EWTT_MurderAdd,		// 补兵添加
};

// 掉落天赋类型
enum EWarBuffType
{
	EWBT_None = 0,		// 不存在的处理方式
	EWBT_Alone,			// 单独
	EWBT_Around,		// 周围
	EWBT_CampAll,		// 己方全部
};

// 掉落天赋类型
enum EWarBroadCastType
{
	EWBCT_None		= 0x0,		// 不存在的处理方式
	EWBCT_DieIcon	= 0x01,		// 死亡图标广播
	EWBCT_Relive	= 0x02,		// 重生广播
	EWBCT_DieText	= 0x04,		// 死亡文字广播
	EWBCT_All		= 0x07,		// 以上全部
};

//击杀英雄获得经验方式
enum EWarGetPersonDieEXPType
{
	EWGP_Kill,
	EWGP_Assist,
};


// 小地图信息事件
enum EMiniMapInfoType
{
	EMMIT_None = 0,
	EMMIT_Protect,			// 保护
	EMMIT_Attack,			// 攻击
};


#define  MAX_WAR_EVENT_DEFINE 5

struct SWarEventDefine
{
	EWarInfoEvent eWarEvent;
	int nContIndex;
	int nParm;

	SWarEventDefine()
	{
		memset(this, 0, sizeof(*this));
	};
};

// 解析文件数据
struct SWarInfoScheme
{
	// 序列号
	int nIndex;
	// 名称
	char szName[GAME_NAME_MAXSIZE];	
	// 是否直接刷出来
	bool bIsUse;
	// 阵营
	int nCamp;
	// 事件类型
	EWarMonsterExec eMonsterType;
	// 是否共享图标
	bool bIsShare;

	// 图标类型
	int nIconIndex;

	// 战场ID
	int nWarID;
	// 怪物表
	int nMonsterIndex;
	// 起始时间
	int nStartTime;
	// 刷新时间
	int nRefreshTime;

	// 同步到哪一行刷新时间
	int nSynRefreshTimeIndex;

	//基础等级	
	int nBaseLevel;

	//升级时间
	int nUpgradeTime;

	//最大等级
	int nMaxLevel;

	// 怪物刷新显示的序列
	int nTimeIndex;

	// 寻路类型
	int nPathID;

	// 出身影响事件定义
	SWarEventDefine WarBornEventDefine[MAX_WAR_EVENT_DEFINE];

	// 死亡影响事件定义
	SWarEventDefine WarDeadEventDefine[MAX_WAR_EVENT_DEFINE];

	SWarInfoScheme()
	{
		memset(this, 0, sizeof(*this));
	};
};




// SchemeWarPoint
#define WARPOINT_SCHEME_FILENAME "Scp\\WarPoint.csv"
struct SWarPointScheme
{
	// 序列号
	int nIndex;
	// 怪物ID
	int nMonsterID;

	// 朝向
	float fAX;
	float fAY;
	float fAZ;

	// 坐标
	float fX;
	float fY;
	float fZ;
	// 广播类型 EWarBroadCastType
	int nBroadCastType;

	// 位置索引
	int nLocalIndex;

	// 死亡增加经验方式 EWarAddEXPType
	int nAddEXPType;
	// 死亡的经验
	int nExp;

	// 掉落天赋类型 EWarTalentType
	int nTalentType;
	// 天赋值
	int nTalentValue;

	// 掉落天赋类型 EWarBuffType
	int nBuffType;
	// buff值
	int nBuffValue;
	// buff等级
	int nBuffLevel;

	// 范围
	int nDistanceValue;

	// 怪物累积次数多少次数刷新
	int nRefreshCumulant;

	// 同步到哪个索引累积刷新次数
	int nSynRefreshCumulantIndex;

	// 开启小地图信息防守进攻标志
	int nMiniMapAttackProtectFlag;

	SWarPointScheme()
	{
		memset(this, 0, sizeof(*this));
	};
};
// 怪物个数
typedef vector<SWarPointScheme> WarPointVect;