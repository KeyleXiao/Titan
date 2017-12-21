/*******************************************************************
** 文件名:	Team_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/3/2015
** 版  本:	1.0
** 描  述:	战场相关托管公共定义

			
********************************************************************/

#ifndef __COMMON_ORDER_MANAGED_DEF_H__
#define __COMMON_ORDER_MANAGED_DEF_H__


#include "GameDef.h"
#include "VOCATION_ManagedDef.h"
#include "ManagedCode.h"


namespace  Order_ManagedDef
{

		//////////////////////////////////////////////////////////////////////////
		/*						C++和C#公用数据结构								*/
		//////////////////////////////////////////////////////////////////////////

        // 排行分类
        ENUM EOrderClass
        {
	        EOC_Mentorship_Order_Class = 0,                // 师徒
            EOC_Person_Order_Class,					       // 个人排行	   
            EOC_Hero_Order_Class,						   // 英雄排行
	        EOC_Clan_Order_Class,						   // 联盟排行
	        EOC_Kin_Order_Class,						   // 战队排行
            EOC_King_Of_Hero_Class,						   // 英雄王者榜
            
	        EOC_Order_Class_Max,						   // 排行
        };

        // 具体个人排行关键字
        ENUM EPlayerOrder_Key
        {
            
            Player_MatchType_Key = 0,                       // 撮合类型榜 EOC_Person_Order_Class 单独读取

            Player_CurWeek_Master_Key = 1,		            // 本周导师榜 EOC_Mentorship_Order_Class 键值属性读取
            Player_CurWeek_Prentice_Key,			        // 本周学徒榜 EOC_Mentorship_Order_Class
            Player_LastWeek_Master_Key,			            // 本周导师榜 EOC_Mentorship_Order_Class
            Player_LastWeek_Prentice_Key,		            // 本周学徒榜 EOC_Mentorship_Order_Class
            Player_Mentorship_Order_Class_Key_Max,		    //  EOC_Mentorship_Order_Class 最大值

	        Player_Kill_Key = 51,							// 角色击杀榜 EOC_Person_Order_Class 键值属性读取
	        Player_Donate_Key,							    // 捐赠榜     EOC_Person_Order_Class
	        Player_Gemstone_Key,							// 宝石榜     EOC_Person_Order_Class
	        Player_Skin_Key,							    // 皮肤榜     EOC_Person_Order_Class
	        Player_Herocount_Key,							// 英雄数量榜 EOC_Person_Order_Class
            Player_OrderClass_Key_Max,						// EOC_Person_Order_Class 最大值

            // 这里是王者排位榜的 500 + 比赛类型ID
            Player_KingRank_Key = 501,                       // 王者排位榜 EOC_Person_Order_Class 特殊处理单独读取 + 比赛类型

            ////////////////////////////////////在这之前加其他玩家的键值榜单//////////////////////////////////////
            Player_King_Of_Hero_Key = 1001,					// 英雄王者榜 EOC_King_Of_Hero_Class

#ifdef _MANAGED
            Player_King_Of_Hero_Max = Player_King_Of_Hero_Key + VOCATION::VOCATION_MAX - 1,
#else
            Player_King_Of_Hero_Max = Player_King_Of_Hero_Key + VOCATION_MAX - 1,
#endif

	        Player_Order_Key_Max,

        };

        // 英雄排行关键字
        ENUM EHeroOrder
        {
	        Order_Hero_Develop_Key = 0,
            Order_Hero_Type_Key,
	        Order_Hero_Key_Max,
        };

        // 战队排行关键字
        ENUM EKinOrder
        {
	        Order_Kin_Key = 0,
	        Order_Kin_Key_Max,
        };

        // 帮会排行关键字
        ENUM EClanOrder
        {
	        Order_Clan_Key = 0,
	        Order_Clan_Key_Max,
        };


        //////////////////////////和数据库那边约定 排行榜设置 //////////////////////////
        ///////////////////注意！！！添加个人排行榜设置对应要和数据那边约定 ////////////
        ///////////////////注意！！！添加同时要修改 GEOS_MAX 定义的值///////////////////
        ENUM EDBOrderSetInfo
        {
            EOS_MatchType_Order_Set_Class = 0,         // 撮合类型排序
            EOS_PlayerKill_Oder_Set_Class,             // 角色排行榜相关 1、多杀
            EOS_Donate_Oder_Set_Class,                 // 捐赠排行设置
            EOS_Hero_Order_Set_Class,                  // 英雄榜单 

            EOS_Max_Set_Class,                        // 最大
        };
        #define GEOS_MAX                             4  // EOS_Max_Set_Class

}
#endif	// __COMMON_TEAM_MANAGED_DEF_H__