/*******************************************************************
** 文件名:	Match_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/3/2015
** 版  本:	1.0
** 描  述:	比赛通用结构定义

			
********************************************************************/

#ifndef __COMMON_MATCH_MANAGED_DEF_H__
#define __COMMON_MATCH_MANAGED_DEF_H__


#include "ManagedCode.h"

namespace  Match_ManagerDef
{
	//////////////////////////////////////////////////////////////////////////
	/*						C++和C#公用数据结构								*/
	//////////////////////////////////////////////////////////////////////////
    // 比赛类型
    ENUM EMMatchType
    {
        MatchType_None      = 0, 
        MatchType_NotUse1,		        // 不用
        MatchType_MachineMatch,		    // 人机
        MatchType_NewPlayerGuide2,      // 新手引导地图2
        MatchType_PressureTest,			// 压力测试类型	
        MatchType_Match,                // 匹配
        MatchType_DoNotRefreshMonster,  // 不刷兵
        MatchType_First5Match,          // 前5场匹配           
        MatchType_Rank,        			// 排位
        MatchType_NotUse4,               
        MatchType_NewPlayerGuide1,      // 新手引导地图1                   
        MatchType_NotUse6,                
        MatchType_NotUse7,                
        MatchType_NotUse8,                
        MatchType_MultiCampMatch,       // 多阵营匹配             
        // 最大
        MatchType_MaxID
    };

	// 房间类型
	ENUM EMHallRoomType
	{
		HallRoomType_Unknown	= 0,		// 无效
		HallRoomType_Custom,				// 自定义房间
		HallRoomType_Macth,					// 匹配房间
		HallRoomType_MacthMachine,			// 人机房间
		HallRoomType_LegendCup,				// 杯赛房间
	};

    // 惩罚类型枚举
    ENUM EPunishmentType
    {
        emPunishmentType_Invalid = 0,			    // 无效的
        emPunishmentType_Plug,				        // 外挂惩罚：使用外挂惩罚
        emPunishmentType_Match,					    // 撮合惩罚：撮合退出相关
        emPunishmentType_War,					    // 战场惩罚：挂机或者逃跑
        emPunishmentType_Accuse,                    // 举报惩罚：玩家举报
        emPunishmentType_Max,
    };

}
#endif	// __COMMON_MATCH_MANAGED_DEF_H__