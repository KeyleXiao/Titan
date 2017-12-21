/*******************************************************************
** 文件名:	Team_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/3/2015
** 版  本:	1.0
** 描  述:	战场相关托管公共定义

			
********************************************************************/

#ifndef __COMMON_TEAM_MANAGED_DEF_H__
#define __COMMON_TEAM_MANAGED_DEF_H__


#include "GameDef.h"
#include "ManagedCode.h"

namespace  Team_ManagedDef
{

		//////////////////////////////////////////////////////////////////////////
		/*						C++和C#公用数据结构								*/
		//////////////////////////////////////////////////////////////////////////

        ////////////////////////// 组队分配模式 //////////////////////////
        ENUM ETeamAllotMode
        {
            
                ETeamPrize_AllotMode_Turn = 0,					// 轮流拾取
                ETeamPrize_AllotMode_Free,						// 自由拾取
                ETeamPrize_AllotMode_Captain,					// 队长拾取
                ETeamPrize_AllotMode_Dice,						// 掷骰子拾取
                ETeamPrize_AllotMode_GroupAuction,				// 金团竞拍
                ETeamPrize_Allot_Max,							// .....

        };

        ENUM EBuildFlowResult
        {
            // 组队请求结果：同意组队
            EBuildFlowResult_Agree = 0,

            // 组队请求结果：主动拒绝
            EBuildFlowResult_Disagree,

            // 组队请求结果：因设置拒绝组队而拒绝
            EBuildFlowResult_Disagree_Set,
        };

		

}
#endif	// __COMMON_TEAM_MANAGED_DEF_H__