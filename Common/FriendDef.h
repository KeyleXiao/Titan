//==========================================================================
/**
* @file	  : FriendDef.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-6-16   10:48
* purpose : 好友系统相关定义
*/
//==========================================================================

#ifndef __FRIENDDEF_H__
#define __FRIENDDEF_H__

#include "UserInfoDef.h"
#include "NameLenDef.h"

// tolua_begin

/// 好友系统配置
enum FriendSysConfig
{
	SysConfig_OnlyOnline	= 0x1,		/// 只显示在线用户
	SysConfig_RefuseAdded	= 0x2,		/// 拒绝加好友
	SysConfig_Hide			= 0x4,		/// 在线隐身
	SysConfig_DisableNotify	= 0x8,		/// 禁止上线通知
	SysConfig_RefuseStrangerMessage = 0x10,	//拒接接收陌生人信息
	SysConfig_ChatRecord    = 0x20,     /// 保存好友聊天记录
	SysConfig_AllowAnybody	= 0x40,		/// 允许任何人加好友
	SysConfig_AllowByCheck	= 0x80,		/// 需要在线验证好友身份
};

/// 好友关系
enum FriendRelation
{
	FriendRelation_None,				/// 没有关系
	FriendRelation_Confidant,			/// 密友
	FriendRelation_Buddy,				/// 好友
	FriendRelation_Stranger,			/// 陌生人
	FriendRelation_BlackList,			/// 黑名单
	FriendRelation_Enemy,				/// 仇敌
	FriendRelation_Group,				/// 群组
	FriendRelation_GM,					/// GM for VIP
	MaxFriendRelationCount,
};

/// 好友广播类型
enum FriendBroadcastType
{
	FriendBroadcast_Confidant	= 0x1,		/// 广播给密友
	FriendBroadcast_Buddy		= 0x10,		/// 广播给好友
	FriendBroadcast_Stranger	= 0x100,	/// 广播给陌生人
	FriendBroadcast_BlackList	= 0x1000,	/// 广播给黑名单
	FriendBroadcast_Enemy		= 0x10000,	/// 广播给仇敌
};

/// 给指定好友发送消息类型
enum FriendPrivateType
{
	FriendPrivateType_default	= 0x1,		/// 在线玩家,不在线则发送离线消息

};

/// 添加好友进行状态类型
enum FriendAddType
{
	FriendAddType_None = 0,					// 无状态
	FriendAddType_OffLine,					// 离线状态
	FriendAddType_Apply,					// 申请好友状态
	FriendAddType_AgreeApply,				// 同意申请状态
	FriendAddType_RefuseApply,				// 拒绝申请状态
	FriendAddType_BlackForbit,				// 黑名单禁止提升好友
	FriendAddType_PirateForbit,				// 海盗禁止加好友
};

enum 
{
	ErrorType_ResponseTips = 0x1,	   /// 聊天信息框即时提示信息
	ErrorType_Error		   = 0x10,     /// 错误提示
};
typedef ulong ErrorType;

/// 群类型
enum GroupType
{
	GroupType_Team,		/// 队伍群
	GroupType_Tribe,	/// 帮会群
	GroupType_Kin,		/// 家族群
	GroupType_UnionTribe,	/// 盟帮群
	GroupType_Count,
};

enum { MaxFriendCount = 120 };			/// 最大用户的好友数

#pragma pack(push)
#pragma pack(1)
#pragma warning(push) // warning C4200: 使用了非标准扩展 : 结构/联合中的零大小数组
#pragma warning(disable : 4200)

/// 好友的基本用户信息
struct FriendUserInfo
{
	ulong	id;						/// 数据库ID
	char	name[MAX_ACTORNAME_LEN];				/// 用户名
	int		level;					/// 等级
	uchar	profession;				/// 职业
	uchar	sex;					/// 性别
	uchar	nation;					/// 国家
	char	tribe[CLAN_NAME_LEN];	/// 帮会
	char	kin[KIN_NAME_LEN];		/// 家族
	bool	online;					/// 是否在线
	bool	bOnPublicline;			/// 是否在公共区
	int     map;                    /// 地图
	int	    nLogoutTime;			/// 最近下线时间

};

/// 用户最基本的好友配置信息
struct FriendConfigInfo
{
	ulong	syscfg;					/// 系统配置
	ushort	faceid;					/// 头像Id
	uchar	province;				/// 省份
	uchar	city;					/// 城市
	uchar	age;					/// 年龄
	char	remark[257];			/// 自我介绍
};
struct FriendConfigInfoProvinceCity
{
	char szProvinceName[32]; 	    //省份名
	char szCityName[32]; 			//城市名
};
/// 用户的好友信息
struct FriendInfo
{
	FriendConfigInfo	cfg;		/// 配置信息
	FriendUserInfo		user;		/// 用户信息
};

/// 用于群组的好友信息
struct GroupFriendInfo
{
	ulong	id;						/// 数据库ID
	char	name[MAX_ACTORNAME_LEN];				/// 用户名
	ushort	faceid;					/// 头像Id
	bool	online;					/// 是否在线
};

/// 好友列表中的好友记录
struct FriendRecord
{
	uchar		relation;			/// 好友关系
	FriendInfo	info;				/// 好友信息
	FriendRecord()
	{
		memset(this,0,sizeof(*this));
	}
};

/// 离线消息
struct OfflineMsg
{
	ulong sender;					/// 消息发送者
	char msg[257];					/// 消息体
	char date[20];					/// 日期时间
};


//公共区与普通区之间使用数据定义



/// 公共区陌生人第一次私聊使用好友的基本用户信息
struct FriendUserInfoPub
{
	ulong	id;						/// 数据库ID
	char	name[MAX_ACTORNAME_LEN];				/// 用户名
	int		level;					/// 等级
	uchar	profession;				/// 职业
	uchar	sex;					/// 性别
	uchar	nation;					/// 国家
	char	tribe[CLAN_NAME_LEN];	/// 帮会
	char	kin[KIN_NAME_LEN];		/// 家族
	bool	online;					/// 是否在线
	bool	bOnPublicline;			/// 是否在公共区
	int     map;                    /// 地图
};

/// 用户最基本的好友配置信息
struct FriendConfigInfoPub
{
	ulong	syscfg;					/// 系统配置
	ushort	faceid;					/// 头像Id
	uchar	province;				/// 省份
	uchar	city;					/// 城市
	uchar	age;					/// 年龄
};
/// 用户的好友信息
struct FriendInfoPub
{
	FriendConfigInfoPub		cfg;			/// 配置信息 公共区和普通区通信 去除用户签名 太大了! By yuxb 
	FriendUserInfo			user;			/// 用户信息
};

/// 公共区好有聊天结构体
struct FriendRecordPub
{
	uchar			relation;			/// 好友关系
	FriendInfoPub	info;				/// 好友信息
};
////////////////////////////////////////////////

// tolua_end

/// 好友列表
struct FriendList
{
	uchar			idx;
	ushort			count;
	FriendRecord	list[];
};

/// 后台GM发送聊天消息给玩家(dbid offLineGM_dbid, const char* offLineGM_name, dbid player_dbid, const char* message)
//FriendMsg_OffLineGMSendMessage2Player,
struct sOffLineGMSendMessage2Player
{
	dbid offLineGM_dbid;
	char offLineGM_name[MAX_ACTORNAME_LEN];
	dbid player_dbid;
	char message[512];
};
struct sOffLineGM
{
	dbid offLineGM_dbid;
	char offLineGM_name[MAX_ACTORNAME_LEN];
	bool isNormal;//这里的gm有两种，一种是默认读脚本产生的isNormal = true，一种是gm主动找玩家产生的
};
//
struct sPlayerSendMessage2OffLineGM
{
	dbid offLineGM_dbid;
	char offLineGM_name[MAX_ACTORNAME_LEN];
	dbid player_dbid;
	char player_name[MAX_ACTORNAME_LEN];
	char message[512];
};
//
#pragma warning(pop)
#pragma pack(pop)



/// 好友消息定义
enum
{
	// 客户端发给社会服务器
	/// 设置用户配置 (const FriendConfigInfo* cfg)
	FriendMsg_SetFriendInfo,

	/// 添加好友(dbid friendId, uchar relation)
	FriendMsg_AddFriend,

	/// 更新好友关系(添加或修改)(dbid friendId, uchar relation)
	FriendMsg_UpdateFriend,

	/// 删除好友 (dbid friendId)
	FriendMsg_RemoveFriend,

	/// 请求好友信息(dbid friendId, const char* nick)
	FriendMsg_RequestFriendInfo,

	/// 登录后的一次性请求好友数据(无)
	FriendMsg_RequestFriendData,

	/// 发送聊天消息(dbid target, const char* msg)
	FriendMsg_SendMsg,

	/// 发送群组消息(ulong type, const char* msg)
	FriendMsg_SendGroupMsg,

	/// VIP玩家可以直通后台GM
	FriendMsg_IsVIP,



	// 社会服务器发给客户端
	/// 初始化省份和城市信息
	FriendMsg_InitFriendInfoProvinceCity,

	/// 更新用户自己的配置和用户信息(const FriendInfo* info)
	FriendMsg_UpdateFriendInfo,

	/// 更新好友列表(const FriendList* list)
	FriendMsg_UpdateFriendList,                                                                                                                                                                              

	/// 反馈好友信息(dbid friendId, const FriendInfo* info)
	FriendMsg_ResponseFriendInfo,

	/// 反馈添加好友(const FriendRecord* fr)
	FriendMsg_ResponseAddFriend,

	/// 反馈被添加好友(const FriendInfo* fr)
	FriendMsg_ResponseBeAddFriend,

	/// 反馈好友关系的更新(dbid friendId, uchar relation)
	FriendMsg_ResponseUpdateFriend,

	/// 反馈删除好友(dbid friendId)
	FriendMsg_ResponseRemoveFriend,

	/// 转发聊天消息(dbid target, dbid sender, const char* msg, const char* time)
	FriendMsg_MsgArrived,

	/// 转发聊天消息时被拒绝
	FriendMsg_MsgRefuse,

	/// 离线消息到达(ushort count, const OfflineMsg* msgList)
	FriendMsg_OfflineMsgArrived,

	/// 转发群组消息(ulong type, dbid sender, const char* msg, const char* time)
	FriendMsg_GroupMsgArrived,

	/// 上线通知
	FriendMsg_UserLogin,

	/// 下线通知
	FriendMsg_UserLogoff,

	/// 好友信息改变(dbid friendId, const FriendInfo* fi)
	FriendMsg_FriendInfoChanged,

	/// 有人进入队伍(ulong groupType, ulong teamId, size_t count, const FriendInfo* list)
	FriendMsg_EnterGroup,

	/// 有人离开队伍(ulong groupType, ulong teamId, dbid memberId)
	FriendMsg_LeaveGroup,
	
	/// 所有的好友相关的处理错误(ulong ErrorType, dbid target, const char* reason)
	/// ErrorType:聊天显示框的即时错误信息
	/// target   :信息
	FriendMsg_Error,


	// 社会服发给场景服务器
	/// 角色更新FaceId(dbid actor, ushort faceid)
	FriendMsg_SetFaceId,

	// 场景服务器发给社会服
	/// 向用户的指定类型的好友广播消息(dbid id, ulong broadcastType, const char* message)
	FriendMsg_BroadcastFriends,


	/// 向加我为好友的用户广播消息(dbid id, ulong broadcastType, const char* message)
	FriendMsg_BroadcastUsers,

	/// 添加好友(dbid actorId, dbid friendId, uchar relation)
	FriendMsg_AddFriendEx,

	/// 删除好友 (dbid actorId, dbid friendId)
	FriendMsg_RemoveFriendEx,

	/// 向用户的指定类型的好友广播数据(dbid id, ulong broadcastType, const void* data, size_t len)
	FriendMsg_BroadcastFriendsData,

	/// 向加我为好友的用户广播数据(dbid id, ulong broadcastType, const void* data, size_t len)
	FriendMsg_BroadcastUsersData,

	/// 公共区上线通知
	FriendMsg_OnPublicUserLogin,

	/// 公共区下线通知
	FriendMsg_OnPublicUserLogoff,

	/// 后台GM发送聊天消息给玩家(dbid offLineGM_dbid, const char* offLineGM_name, dbid player_dbid, const char* message)
	FriendMsg_OffLineGMSendMessage2Player,

	/// 向用户的指定类型的好友广播消息(dbid id, dbid target,ulong type, const char* message)
	FriendMsg_BroadcastFriendsPrivate,

	/// 申请好友状态
	FriendMsg_ApplyForFriendsStatus,

	/// 反馈被添加好友，请求验证(const FriendInfo* fr)
	FriendMsg_ResponseBeAddFriendByCheck,

};


#endif // __FRIENDDEF_H__