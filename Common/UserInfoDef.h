//==========================================================================
/**
* @file	  : UserInfoDef.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-6-11   21:48
* purpose : 社会服务器用户信息定义
*/
//==========================================================================

#ifndef __USERINFODEF_H__
#define __USERINFODEF_H__

#include "Entity_ManagedDef.h"
#include "NameLenDef.h"
#include "GameDef.h"

/// 挂接在用户管理器上面的用户数据
enum UserDataType
{
	UserData_Chat,		/// 聊天系统数据
	UserData_Friend,	/// 好友系统数据
	UserData_Team,		/// 组队系统数据
	UserData_Kin,		/// 家族系统数据
	UserData_Nation,	/// 国家系统数据
	UserData_Tribe,		/// 帮会系统数据

	MaxUserDataCount,
};



/// 用户信息更新选项
enum UserInfoUpdateOption
{
	UpdateOption_UID			= 0x1,
	UpdateOption_ServerId		= 0x2,
	UpdateOption_Level			= 0x4,
	UpdateOption_Right			= 0x8,
	UpdateOption_Profession		= 0x10,
	UpdateOption_LingHun		= 0x20,
	UpdateOption_Team			= 0x40,
	UpdateOption_Tribe			= 0x80,
	UpdateOption_Kin			= 0x100,
	UpdateOption_Group			= 0x200,
};


/// 用户信息更新选项
enum UserOnlineStateOption
{
	UserOnlineState_OffLine			= 0,
	UserOnlineState_Online			= 1,
	UserOnlineState_OtherOnline		= 2,

};

#pragma pack(push)
#pragma pack(1)

typedef ulong dbid;

/// 基本用户数据
struct UserInfo
{
	dbid		id;							/// 用户的DBID
	UID			uid;						/// 用户的UID
	ulong		clientId;					/// 客户端Id
	ulong		svrId;						/// 服务器Id
	ulong		worldId;					/// 游戏世界ID

	char		name[ACTORNAME_MAXSIZE];	/// 用户的名字
	ulong		level;						/// 用户等级
	ulong		nation;						/// 用户所在国家id
	ulong		tribe;						/// 用户所在帮会id
	ulong		kinid;						/// 用户所在家族id
	ulong		team;						/// 用户所在队伍id
	ulong		right;						/// 用户权限
	uchar		profession;					/// 用户职业
	uchar		sex;						/// 用户性别
	uchar		province;					/// 省份
	uchar		city;						/// 城市
	int         map;                        /// 用户当前的地图ID
	ulong		group;						/// 用户所在团队ID
	DWORD		dwServiceFlags;				/// 服务标识: dwServiceFlags & 0x1被推荐用户;dwServiceFlags & 0x2推荐用户;dwServiceFlags & 0x4 VIP玩家
};

#pragma pack(pop)

#endif // __USERINFODEF_H__