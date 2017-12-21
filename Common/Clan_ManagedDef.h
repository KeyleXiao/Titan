/*******************************************************************
** 文件名:	Clan_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	侯森
** 日  期:	5/26/2016
** 版  本:	1.0
** 描  述:	帮会相关托管公共定义
********************************************************************/

#ifndef __COMMON_CLAN_MANAGED_DEF_H__
#define __COMMON_CLAN_MANAGED_DEF_H__

#include "GameDef.h"
#include "ManagedCode.h"

namespace Clan_ManagerDef
{
    ENUM EMCLAN_LIST
    {
        // 帮会列表一页的大小
        CLAN_LIST_PAGE_SIZE = 6,
    };


    // 帮会列表状态定义
    ENUM EMClanListClanStatus
    {
	    CLAN_LIST_STATUS_NORMAL,		// 普通
	    CLAN_LIST_STATUS_REQUESTING,	// 申请中
	    CLAN_LIST_STATUS_FULL,			// 已满
		CLAN_LIST_STATUS_DISMISSING,	// 解散中
	    CLAN_LIST_STATUS_MAX			// 最大状态数
    };

    // 帮会职位
    ENUM EMClanIdentity
    {
	    EMCLANIDENTITY_INVALID = -1,		// 无效
	    EMCLANIDENTITY_MEMBER = 0,			// 成员
	    EMCLANIDENTITY_SHAIKH,				// 指挥官
	    EMCLANIDENTITY_ELDER,				// 长老
    };

    ENUM EMClanOrderMax
    {
        EMCLANORDERMAX      = 3,             // 帮会排行最大数量
    };

    // 帮会状态枚举
    ENUM EMClanState
    {
        emClanState_Invaild  = -1,			// 非法状态
        emClanState_Normal = 0,				// 通常状态
        emClanState_Dismissing,				// 解散中
        emClanState_Num,					// 状态数量
    };

	// 盟主状态枚举
	ENUM EMClanShaikhState
	{
		ClanShaikhState_Unknow = 0,			    // 未知
		ClanShaikhState_Reigning,			    // 在位中
		ClanShaikhState_Offline,				// 下线中
		ClanShaikhState_Outgoing,			    // 卸任中
	};
}

#endif // __COMMON_CLAN_MANAGED_DEF_H__