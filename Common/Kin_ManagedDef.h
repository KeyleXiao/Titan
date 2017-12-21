/*******************************************************************
** 文件名:	Kin_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/3/2015
** 版  本:	1.0
** 描  述:	家族通用结构定义

			
********************************************************************/

#ifndef __COMMON_KIN_MANAGED_DEF_H__
#define __COMMON_KIN_MANAGED_DEF_H__


#include "ManagedCode.h"

namespace  Kin_ManagerDef
{
	//////////////////////////////////////////////////////////////////////////
	/*						C++和C#公用数据结构								*/
	//////////////////////////////////////////////////////////////////////////
    // 家族的权限标志
    ENUM TKinPopedom
    {
        emKinPopedom_AppointShaikh   = 0x0001,		// 任命族长
        emKinPopedom_AppointElder    = 0x0002,		// 任命副族长

        emKinPopedom_AcceptMember    = 0x0004,		// 招收成员
        emKinPopedom_KickMember      = 0x0008,		// 开除成员(不包括族长和副族长)
        emKinPopedom_Fight           = 0x0010,		// 发起家族站
        emKinPopedom_ModifyNotice	 = 0x0020,		// 修改公告
        emKinPopedom_ModifyManifesto = 0x0040,		// 修改宣言
        emKinPopedom_ResearchSkill	 = 0x0080,		// 研究技能
        emKinPopedom_ModifyRemark	 = 0x0100,		// 修改成员备注

        emKinPopedom_ShaikhPopedom	 = 0x01FF,		// 族长权限
        emKinPopedom_ElderPopedom	 = 0x01FC,		// 副族长权限
        emKinPopedom_AllPopedom	     = 0x0FFF,		// 所有
    };


    // 家族身份
    ENUM TKinIdentity
    {
        emKinIdentity_Member = 0,					// 成员
        emKinIdentity_Elder,						// 副族长
        emKinIdentity_Shaikh,						// 族长
    };

    // 家族属性
    ENUM TKinProp
    {
        emKinProp_ID = 0,							// 家族ID
        emKinProp_State,							// 状态
        emKinProp_Level,							// 家族等级
        emKinProp_EstablishTime,					// 家族创立时间
        emKinProp_DismissApplyTime,					// 发起解散的时间
        emKinProp_ShaikhID,							// 家族族长PDBID
        emKinProp_MemberLastLogoutTime,				// 成员的最后下线时间
        emKinProp_ReadFlag,							// 已经从数据库读出数据的标志
        emKinProp_Money,                            // 资金
        emKinProp_Activity,							// 活跃度
		emKinProp_WeekActivity,						// 周活跃度
        emKinProp_Place,							// 排名
        emKinProp_WorldID,							// 游戏世界ID
        emKinProp_ChangeLand,						// 家族移民状态
		emKinProp_Shaikh_State,					    // 队长状态

        emKinProp_NumPropCount,						// 属性个数,以上是整型的属性

        emKinProp_Name = 100,						// 家族名
        emKinManifesto,								// 家族的宣言
        emKinNotice,								// 家族的每日公告
        emKinShaikhName,							// 族长名字
        emKinSlogan,								// 宣传口号

    };

    // 自动接收条件
    ENUM TKinAutoFlag
    {
        emKinAuto_None      = 0x0000,               // 无
        emKinAuto_Male		= 0x0001,				// 男性
        emKinAuto_Female	= 0x0002,				// 女性
        emKinAuto_AllSex	= 0x0003,				// 所有性别
        emKinAuto_Default	= 0x00FF,				// 缺省为都选中
    };

    ENUM TKinOrderMAX
    {
        emKinOrderMAX      = 3,               // 无
    };

    ENUM TSubPackageState
    {
        emSubPackageStateBegin    = 1,               // 开始
        emSubPackageStateEnd,               // 开始
    };
	
	// 队长状态枚举
	ENUM EMKinShaikhState
	{
		KinShaikhState_Unknow = 0,			    // 未知
		KinShaikhState_Reigning,			    // 在位中
		KinShaikhState_Offline,				    // 下线中
		KinShaikhState_Outgoing,			    // 卸任中
	};
}
#endif	// __COMMON_KIN_MANAGED_DEF_H__