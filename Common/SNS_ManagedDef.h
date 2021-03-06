﻿/*******************************************************************
** 文件名:	SNS_ManagedDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2016/12/8
** 版  本:	1.0
** 描  述:	SNS系统逻辑层和显示层共用

			
********************************************************************/


#ifndef __SNS_MANAGED_DEF_H__
#define __SNS_MANAGED_DEF_H__

#pragma pack(1)
#include "ManagedCode.h"

//////////////////////////////////////////////////////////////////////////
/*						C++和C#公用数据结构								*/
//////////////////////////////////////////////////////////////////////////

// 好友关系状态
// 其中A是DB_Table_SNSRelationship中的dwUserID1
// 其中B是DB_Table_SNSRelationship中的dwUserID2
ENUM EMSNS_BUDDY_RELATIONSHIP
{
	EMSNS_BUDDY_RELATIONSHIP_NONE = 0,			// A和B无关系
	EMSNS_BUDDY_RELATIONSHIP_BUDDY,				// A和B是好友
	EMSNS_BUDDY_RELATIONSHIP_BLACKLIST,			// A把B加入了黑名单
	EMSNS_BUDDY_RELATIONSHIP_BLACKLIST_BY_OTHER,// A被B加入了黑名单
	EMSNS_BUDDY_RELATIONSHIP_DELETE_OTHER,		// A删除了B
	EMSNS_BUDDY_RELATIONSHIP_DELETED_BY_OTHER,	// A被B删除    
	EMSNS_BUDDY_RELATIONSHIP_RECV_INVITE,		// A收到了好友请求
	EMSNS_BUDDY_RELATIONSHIP_SEND_INVITE,		// A提交了好友请求
	EMSNS_BUDDY_RELATIONSHIP_REJECT_OTHER,		// A拒绝了B的好友请求
	EMSNS_BUDDY_RELATIONSHIP_REJECTED_BY_OTHER,	// A被B拒绝了好友请求
	EMSNS_BUDDY_RELATIONSHIP_NEARBY,			// 附近的人
};

// 好友分组状态
ENUM EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP
{
	EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_NORMAL = 0,	// 新增分组
	EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_DEL,		// 删除的分组
	EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_TEMP,		// 临时的分组
};

//// 好友状态
//ENUM EMSNS_BUDDY_STATUS
//{
//	EMSNS_BUDDY_STATUS_ONLINE,				// 在线
//	EMSNS_BUDDY_STATUS_MATCH,				// 撮合中
//	EMSNS_BUDDY_STATUS_INROOM,				// 房间中
//	EMSNS_BUDDY_STATUS_WAR,					// 游戏中
//	EMSNS_BUDDY_STATUS_LEAVE,				// 离开
//	EMSNS_BUDDY_STATUS_OFFLINE,				// 离线
//};

// 好友分组操作类型
ENUM EMSNS_BUDDYGROUP_UPDATE_TYPE
{
	EMSNS_BUDDYGROUP_UPDATE_TYPE_ADD = 0,		// 添加好友分组
	EMSNS_BUDDYGROUP_UPDATE_TYPE_DEL,			// 删除好友分组
	EMSNS_BUDDYGROUP_UPDATE_TYPE_UPDATE_NAME,	// 修改好友分组名字
};

// 好友操作类型
ENUM EMSNS_BUDDY_UPDATE_TYPE
{
	EMSNS_BUDDY_UPDATE_TYPE_ADD = 0,		// 添加好友
	EMSNS_BUDDY_UPDATE_TYPE_DEL,			// 删除好友
	EMSNS_BUDDY_UPDATE_TYPE_ADD_BLACKLIST,	// 加入黑名单
};

// 回应添加好友请求
ENUM EMSNS_RSP_ADDBUDDY_TYPE
{
	EMSNS_RSP_ADDBUDDY_TYPEE_AGGRE = 0,		// 同意加好友
	EMSNS_RSP_ADDBUDDY_TYPEE_REFUSE,		// 拒绝加好友
};

// 好友分组ID，好友分组总数固定
ENUM EMSNS_BUDDYGROUP_ID
{
	EMSNS_BUDDYGROUP_ID_MY_BUDDY = 0,	// 我的好友
	EMSNS_BUDDYGROUP_ID_BLACKLIST,		// 黑名单
	EMSNS_BUDDYGROUP_ID_NEARBY,			// 附近的人
	EMSNS_BUDDYGROUP_ID_LastPlaying,	// 最近游戏
	EMSNS_BUDDYGROUP_ID_RETAIN3,
	EMSNS_BUDDYGROUP_ID_RETAIN4,
	EMSNS_BUDDYGROUP_ID_RETAIN5,
	EMSNS_BUDDYGROUP_ID_DIY1,			// 自定义1
	EMSNS_BUDDYGROUP_ID_DIY2,
	EMSNS_BUDDYGROUP_ID_DIY3,
	EMSNS_BUDDYGROUP_ID_DIY4,
	EMSNS_BUDDYGROUP_ID_DIY6,
	EMSNS_BUDDYGROUP_ID_DIY7,
	EMSNS_BUDDYGROUP_ID_DIY8,
	EMSNS_BUDDYGROUP_ID_DIY9,
	EMSNS_BUDDYGROUP_ID_DIYMAX,
};

// 权限设置
ENUM EMSNS_PERMISSION_ID
{
	EMSNS_PERMISSION_ID_FILTER_ALL_REQ = 0,		// 屏蔽好友请求
	EMSNS_PERMISSION_ID_FILTER_ALL_MSG,			// 屏蔽好友消息
	EMSNS_PERMISSION_ID_FILTER_TEMP_SESSION,	// 屏蔽临时会话
	EMSNS_PERMISSION_ID_DONT_SHOW_OFFLINE_BUDDY,// 不显示离线好友
	EMSNS_PERMISSION_ID_HIDE_NEARBY_GROUP,		// 隐藏附近的人分组
	EMSNS_PERMISSION_ID_DONT_RECOMMOND,			// 不允许推荐给其他好友
};

// 好友信息更新类型
ENUM EMSNS_BUDDYINFO_UPDATE_TYPE
{
	EMSNS_BUDDYINFO_UPDATE_TYPE_MOOD = 0,				// 好友更新了心情
	EMSNS_BUDDYINFO_UPDATE_TYPE_BUDDYRELATION,			// 更新好友关系
};

////////////////////////////////////////C#显示层发送到社会服消息ID/////////////////////////////////////////
#define ESNSView2SocialMsg_Start 0
ENUM ESNSView2SocialMsg
{
	SNSView2Social_JoinBuddyKin = ESNSView2SocialMsg_Start,	// 加入好友的战队
};

////////////////////////////////////////C#显示层发送到社会服消息结构体/////////////////////////////////////////

#pragma pack()
#endif	// __SNS_MANAGED_DEF_H__