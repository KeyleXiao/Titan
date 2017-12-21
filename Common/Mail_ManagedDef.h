/*******************************************************************
** 文件名:	Mail_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:  易耀辉 
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	邮件公共定义
********************************************************************/

#ifndef __COMMON_MAIL_MANAGED_DEF_H__
#define __COMMON_MAIL_MANAGED_DEF_H__

#include "GameDef.h"
#include "ManagedCode.h"

namespace Mail_ManagerDef
{
	// 邮件类型枚举
	ENUM EMMailType
	{
		emMailType_Invalid = -1,			// 无效的
		emMailType_System = 0,				// 系统邮件
		emMailType_Player,					// 玩家邮件
		emMailType_Notify,					// 通知邮件
	};

	// 邮件资源来源类型
	ENUM EMMailSourceType
	{
		emMailSourceType_Normal = 0,				// 普通
		emMailSourceType_System,					// 系统
		emMailSourceType_PlayerGift,				// 玩家赠送
		emMailSourceType_PlayerTrade,				// 玩家交易
	};


    // 邮件配置脚本标题内容填充ID
    ENUM EMMailFillID
    {
	    emMailFill_Begin=0,				        // 启始
	    emMailFill_DragonBall,			        // 七龙珠
												// 2~10000策划自己维护相关
												
        emMailFill_LegendCup_Begin = 10000,		// 各个系统邮件 程序定义 
        emMailFill_LegendCupBonus,              // 杯赛奖金
        emMailFill_LegendCupPrize,              // 杯赛物品奖励
        emMailFill_LegendCupEveryValidWarPrize, // 杯赛有效比赛奖励
		emMailFill_ClanDismiss,                 // 联盟盟主手动解散
		emMailFill_ClanAutoDismiss,             // 联盟活跃度过低后台自动解散
		emMailFill_KinDismiss,                  // 战队队长手动解散
		emMailFill_KinAutoDismiss,              // 战队活跃度过低后台自动解散

        emMailFill_LegendCupRetRegisterCup,          // 回退参赛费用
        emMailFill_LegendCupCancelCreateRet,         // 回退创建杯赛创建费用
        emMailFill_LegendCupCancelContriBonusRet,    // 回退创建杯赛出资费用
        emMailFill_LegendCupCancelRegisterRet,      // 回退创建杯赛失败报名费用
        emMailFill_LegendCup_End = 10099,

		emMailFill_Rank_Begin = 10100,			// 排位开始
		emMailFill_Rank_TaskPrize,				// 排位任务奖励
		emMailFill_Rank_SeasonPrize,			// 排位赛季奖励
		emMailFill_Rank_End = 10199,			// 排位结束

	    emMailFill_Max = 50000,					// 最大
    };
}

#endif // __COMMON_MAIL_MANAGED_DEF_H__