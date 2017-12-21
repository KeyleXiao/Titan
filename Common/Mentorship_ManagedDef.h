/*******************************************************************
** 文件名:	Mentorship_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:  易耀辉 
** 日  期:	11/29/2016
** 版  本:	1.0
** 描  述:	师徒公共定义
********************************************************************/

#ifndef __COMMON_MENTORSHIP_MANAGED_DEF_H__
#define __COMMON_MENTORSHIP_MANAGED_DEF_H__

#include "GameDef.h"
#include "ManagedCode.h"

namespace Mentorship_ManagerDef
{
	// 师徒职务枚举
	ENUM EMMentorshipPost
	{
		emMentorshipPost_Normal = 0,				// 普通
		emMentorshipPost_Master,					// 师傅
		emMentorshipPost_Prentice,					// 徒弟
	};

	// 师徒关系类型枚举
	ENUM EMMentorshipType
	{
		emMentorshipType_Invalid = -1,				// 无效的
		emMentorshipType_Formal = 0,				// 正式的
		emMentorshipType_Intern,					// 见习的
	};

	// 师徒关系状态枚举
	ENUM EMMentorshipState
	{
		emMentorshipState_Normal = 0,				// 正常的
		emMentorshipState_Dismissing,				// 解散中
	};

	// 师徒发布类型枚举
	ENUM EMMentorshipIssueType
	{
		emMentorshipIssueType_Master = 0,			// 师傅发布
		emMentorshipIssueType_Prentice,				// 徒弟发布
	};
}

#endif // __COMMON_MENTORSHIP_MANAGED_DEF_H__