/*******************************************************************
** 文件名:	AOI_Define.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/13/2014
** 版  本:	1.0
** 描  述:	视野管理相关的定义

			
********************************************************************/

#pragma once

#include "Game_ManagedDef.h"

// 广播类型定义
enum BROADCAST_TYPE
{
	BROADCAST_CREATE,							// 广播类型: 实体创建
	BROADCAST_DESTROY,							// 广播类型: 实体销毁
	BROADCAST_IN_SIGHT,							// 广播类型: 进入视野
	BROADCAST_OUT_OF_SIGHT,						// 广播类型: 离开视野
	BROADCAST_MOVE,								// 广播类型: 角色移动
	BROADCAST_DO_ACTION,						// 广播类型: 做一个动作
	BROADCAST_CHANGE_PROPERTY,					// 广播类型: 属性改变
	BROADCAST_PROPERTY_SET,						// 广播类型: 属性集
	BROADCAST_CHANGE_STATE,						// 广播类型: 状态改变
	BROADCAST_PLAY_EFFECT,						// 广播类型: 播放效果
	BROADCAST_BUFF,								// 广播类型: Buff相关
	BROADCAST_SPELL,							// 广播类型: 技能相关
	BROADCAST_SCENE,							// 广播类型: 场景相关
    BROADCAST_PATH,                             // 广播类型: 寻路动态障碍相关
	BROADCAST_TYPE_COUNT,
};

// 广播ID,相同ID的广播数据包会只发一个，减少冗余
struct BROADCAST_ID
{
	unsigned short key:5;
	unsigned short type:5;
	unsigned short padding:6;

	BROADCAST_ID(unsigned short t, unsigned short k = 0)
		: type(t)
		, key(k)
	{
		padding = 0;
	}

	operator unsigned short()
	{
		return *(unsigned short*)this;
	}

	BROADCAST_ID()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SCreateSceneContext
{
    int mapID;
    bool isDynamic;
    EGameMode gameMode;
    int nCampCount;
    SCreateSceneContext()
    {
        mapID       = 0;
        isDynamic   = false;
        gameMode    = EGM_RPG;
        nCampCount  = 0;
    }
};

// 视野管理分层定义
enum
{
	LAYER_WATCHER  = 1,		 // 有视野的对象，能看见别人，主要包括玩家，哨兵怪，灯笼等
	LAYER_BLIND    = 1<<1,   // 无视野对象，主要是普通怪，无主动视野的实体，但能被别人看见
	LAYER_ALL     = 255,     // 所有对象

	LAYER_COUNT   = 2,		 // 总共只有2层
};

#define  LAYER_INDEX(mask) ( mask&LAYER_WATCHER? 0 : 1)

// 阵营掩码定义
#define CAMP_MASK(camp) (0x01<<camp)
#define CAMP_EXCEPT_MASK(camp) (~CAMP_MASK(camp)) & CAMP_MASK_ALL
#define CAMP_COUNT_MAX 16
#define CAMP_MASK_ALL  65535

// 最多只关注2048个对象
#define   MAX_INTEREST_OBJECT	2048

// 圆整
#define	  ROUND_UP(x, align) (((int) (x) + (align - 1)) & ~(align - 1))

#define AOI_GRID_DIM    2		// 二维格子
#define AOI_GRID_SIZE   4       // 每个单元格4个大小
#define AOI_LOC_COUNT   4		// 多点广播最大数量


/************************************************************************/
/* 视野相关配置参数                                                        */
/************************************************************************/
#define	AOI_DEFAULT_SIGHT		(32)


#define  AOI_INPUTPARM_ARRAY_OPT
#ifdef AOI_INPUTPARM_ARRAY_OPT
#undef  MAX_INTEREST_OBJECT
#define MAX_INTEREST_OBJECT 128
#endif