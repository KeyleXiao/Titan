/*******************************************************************
** 文件名:	e:\Rocket\Server\EnmityServer\Src\EnmityServerDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/6/3 15:36
** 版  本:	1.0
** 描  述:	仇恨系统定义
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IEntity.h"
#include "IAIPart.h"

#pragma pack(1)

// 黙认初始仇恨
#define DEFALUT_INIT_ENMITY				100

// OT规则超过当前最高仇恨比
#define OTRULE_CHANGETOPENEMY_MULTIPLE	1.1

// 搜敌人的间隔时长
#define CREEP_TIMER_INTERVAL			2000

// 与最高敌人相差多远，算作有效
#define DISTANCE_TOPENEMY_VAILD			24

// 大于多大的视野就启用九宫搜敌
#define LOOPTILE_MAX_EYESHOT			8

// 搜战友的间隔时长
#define CREEP_COMRADE_INTERVAL			2500

// 添加仇恨值的封顶值
#define ADDENEMY_CAPSTONE				10000000
										
// 减少仇值恨的封顶值
#define SUBENEMY_CAPSTONE				-10000000

// 最大仇恨值
#define MAX_ENMITY_VALUE				2000000000 

////////////////////////////// 仇恨状态 ////////////////////////////
enum
{
	ENMITY_STATE_IDLE = 0,						// 空闲状态
	ENMITY_STATE_PATROL,						// 巡逻状态
	ENMITY_STATE_BATTLE,						// 战斗状态
	ENMITY_STATE_RETRIVE,						// 回归状态
};

///////////////////////// 仇恨对AI的命令 ///////////////////////////
enum
{
	ENMITY_COMMAND_ENTER9GRID = 1000,			// 第一个玩家进入怪物九宫
	ENMITY_COMMAND_CHANGE9GRID,					// 九宫内人数变化(或多或少)
	ENMITY_COMMAND_EMPTY9GRID,					// 怪物九宫内已无玩家存在

	ENMITY_COMMAND_CHANGETOPENEMY,				// 修改最高仇恨者了
	// 从没有变成有最高仇恨者也是同样的命令，也是从巡逻状态转成战斗状态的因素
};

///////////////////////////// 搜敌人模式 ///////////////////////////
enum
{
	CREEP_MODE_IDLE = 0,						// 空转
	CREEP_MODE_BUSY,							// 繁忙
};

////////////////////////////// 仇恨传播 ////////////////////////////
enum
{
	RADIATE_MODE_IDLE = 0,						// 空转
	RADIATE_MODE_LEAD,							// 头目传给小弟
	RADIATE_MODE_GROUP,							// Group传播
	RADIATE_MODE_CAMP,							// 阵营传播
};

//////////////////////////// 仇恨部件现场 //////////////////////////
//struct /*ICreature*/ __IEntity;
struct IGameZone;
struct ICampPart;
class  CEnmityPremier;
class  CEnmityPart;
struct SEnmityPartAmah
{	
	__IEntity *				pMaster;			// 所属生物
	IGameZone *				pGameZone;			// 所在场景

	CEnmityPart *			pEnmityPart;		// 仇恨部件

	ICampPart *				pCampPart;			// 阵营部件

	__IAIPart *				pAIPart;			// AI部件

	float					fEyeshot;			// 生物视野	

	int						nEnmityTeamID;		// 团队ID

	bool					bCampRadiate;		// 是否阵营仇恨传播
	bool					bTeamRadiate;		// 是否团队仇恨传播
	
	int						nOnMapVestNation;	// 所在地图的国家归属

//	bool					bIsPawn;			// 是否为召唤兽

	int						nCampStrategy;		// 阵营策略

	bool					bOpenEnemyList;		// 是否打开仇恨列表

	CEnmityPremier *		pPremier;			// 仇恨管理器

	SEnmityPartAmah(void)
	{
		memset(this, 0, sizeof(*this));
	}

	void Clean(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SOutputEnemy
{
	UID						uidEnemy;			// 敌人
	int						nEnmityValue;		// 仇恨值

	SOutputEnemy(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()