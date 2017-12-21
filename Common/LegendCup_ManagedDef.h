/*******************************************************************
** 文件名:	Team_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/3/2015
** 版  本:	1.0
** 描  述:	战场相关托管公共定义

			
********************************************************************/

#ifndef __COMMON_LEDENDCUP_MANAGED_DEF_H__
#define __COMMON_LEDENDCUP_MANAGED_DEF_H__


#include "GameDef.h"
#include "ManagedCode.h"

namespace  LegendCup_ManagedDef
{
    ENUM ECupState
    {
        SATE_NONE,                  //  无状态
        SATE_SYSTEMCANCEL,          //  系统自动取消
        SATE_REGISTER,              //  报名
        SATE_CONFIRMENTRIES,        //  确定报名列表
        SATE_GROUPING,              //  分组
        SATE_32TO16,                //  32进16
        SATE_16TO8,                 //  16进8
        SATE_8TO4,                  //  8进4
        SATE_4TO2,                  //  4进2 
        SATE_FINAL,                 //  决赛
        SATE_END,                   //  结束
        SATE_MAX,                   //  最大
    };

    ENUM ETicketAllocateMode
    {
        ETAM_SystemSet = 1,         // 系统设置
        ETAM_CustomerSet,           // 用户设置
    };


    ENUM EJudgeNodeWar
    {
        JUDGE_NONE,                 // 未判定
        JUDGE_Cancle,               // 撤销比赛 (移除本场比赛 不计分)
        JUDGE_AllKnockOut,          // 两队弃权（本比赛节点不再进行其他比赛）
        JUDGE_SelectWinner,         // 裁判选择胜利战队
        JUDGE_ConfirmNode,          // 确定比赛（维持本场比赛结果）
        JUDGE_Reset,                // 重置比赛
    };
    
    ENUM ECompetitionNodeState
    {
        emNodeState_WaitStart,       // 等待开始
        emNodeState_CanEnter,        // 等待进入
        emNodeState_Competition,     // 进行中
        emNodeState_WaitJudge,       // 等待裁定
        emNodeState_End,	         // 结束
    };

	ENUM ELegendCupType
	{
		emCupType_None = 0,
		emCupType_System,	    // 系统比赛
		emCupType_Personal,     // 个人比赛
        emCupType_Clan,         // 联盟比赛
		emCupType_Max,			// 最大
	};

	ENUM ERoundCompetitionType
	{
		emType_None,
		emType_KnockoutCompetition,	// 淘汰赛类型
		emType_GroupCompetition,    // 小组赛类型
		emType_Max,			// 最大
	};
    
}
#endif	// __COMMON_TEAM_MANAGED_DEF_H__