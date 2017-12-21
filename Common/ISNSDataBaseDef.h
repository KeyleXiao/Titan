/*******************************************************************
** 文件名:	ISNSDataBaseDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李有红
** 日  期:	2016/12/21
** 版  本:	1.0
** 描  述:	SNS服务器 定义访问游戏数据库时需要用到的各种数据结构
** 应  用:  SNS服务器数据库接口独立与游戏的接口,使用独立的 DBSNSServer,DBSNSProcedure.dbs	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ISNSDATABASEDEF_H__
#define __ISNSDATABASEDEF_H__

#pragma pack(1)

namespace SNS
{

    /*********************************** 常量定义 **********************************************************/
#	define DBDEF_SNS_REMARKNAME_MAXSIZE			32		// 好友备注名长度
#	define DBDEF_SNS_BUDDYGROUPNAME_MAXSIZE		32		// 好友分组名字长度
#   define  DBDEF_CLANNAME_MAXSIZE			32      // 帮会名称字节长度
#   define  DBDEF_ACTORNAME_MAXSIZE         32      // 角色名称字节长度
#	define	DBDEF_TITLENAME_MAXSIZE			32		// 称号长度
#	define	DBDEF_GAME_NAME_MAXSIZE			32		// 名称长度
#	define	DBDEF_MOOD_CONTENT_MAXSIZE		128		// 心情长度
#	define	DBDEF_WORLD_NAME_MAXSIZE		32		// 游戏世界名字长度


	enum 
	{
		GAMEDB_REQUEST_SNS_INVALID = 0,
		GAMEDB_REQUEST_SNS_ACTORINFO_UPDATE ,				// 更新用户最后一次登陆的大区角色信息（没有则添加）
		GAMEDB_REQUEST_SNS_BUDDY_LIST_READ,					// 获取某个用户的所有好友
		GAMEDB_REQUEST_SNS_BUDDYGROUP_LIST_READ,			// 获取某个用户的所有好友分组
		GAMEDB_REQUEST_SNS_BUDDYGROUP_UPDATE,				// 更新好友分组
		GAMEDB_REQUEST_SNS_BUDDYRELATION_LIST_READ,			// 获取某个用户的所有好友关系信息
		GAMEDB_REQUEST_SNS_BUDDYRELATION_UPDATE,			// 更新与某个好友的关系信息
		GAMEDB_REQUEST_SNS_BUDDYRELATION_STATUS_UPDATE,		// 更新与某个好友的关系
		GAMEDB_REQUEST_SNS_BUDDYRELATION_READ,				// 读取某个用户与某个好友的关系
		GAMEDB_REQUEST_SNS_BUDDY_INFO_READ,					// 获取某个用户的信息
		GAMEDB_REQUEST_SNS_USER_UPDATE,						// 更新某个用户的信息
		GAMEDB_REQUEST_SNS_USER_READ,						// 读取某个用户的信息
	};
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////SNS 好友系统//////////////////////////////////////////////////
	// SNS用户表
	struct DB_Table_SNSUser
	{
		DWORD	dwUserID;								// 账号ID
		char	szMood[DBDEF_MOOD_CONTENT_MAXSIZE];		// 心情
		int		nPermission;							// 权限（具体权限程序里使用bit控制）
		int		nReverse1;								// 保留
		int		nReverse2;								// 保留
		int		nReverse3;								// 保留
		DWORD	nUpdated;								// 更新时间
	};

	// SNS游戏角色数据库表：SNSActor
	// 保存玩家最后一次登录所在服的角色信息
	struct DB_Table_SNSActor
	{
		DWORD	dwUserID;								// 账号ID
		DWORD	dwGameID;								// 游戏ID，属于哪个游戏
		DWORD	dwWorldID;								// 世界ID
		DWORD	dwActorID;								// 角色ID
		WORD	wHeadID;								// 头像ID
		char	szActorName[DBDEF_ACTORNAME_MAXSIZE];	// 角色名字
		WORD	wLevel;									// 角色等级
		DWORD	dwLastLogout;							// 最后下线时间
		int		nRankIconID;							// 段位头像
		char	szRankName[DBDEF_GAME_NAME_MAXSIZE];	// 段位描述
		int		nMatchNum;								// 总场数
		int		nWinNum;								// 胜场数
		char	szClan[DBDEF_CLANNAME_MAXSIZE]; 		// 联盟名称
		char	szTitle[DBDEF_TITLENAME_MAXSIZE];		// 称号
		BYTE    nSex;									// 性别 0男 1女 2不区分
		char    szWorldName[DBDEF_GAME_NAME_MAXSIZE];	// 世界名字
		DWORD	nCreated;								// 创建时间
		DWORD	nUpdated;								// 更新时间
	};

	// SNS好友关系数据库表：SNSRelationship
	struct DB_Table_SNSRelationship
	{
		DWORD	dwUserID1;									// 用户1的账号id
		DWORD	dwUserID2;									// 用户2的账号id
		BYTE	nStatus;									// 关系状态
		DWORD	dwBuddyGroupID;								// 好友分组ID（默认为0）
		BYTE	nIntimacy;									// 亲密度（默认为0）
		char	szRemark[DBDEF_SNS_REMARKNAME_MAXSIZE];		// 用户2在用户1中的备注名
		DWORD	nCreated;									// 创建时间
		DWORD	nUpdated;									// 更新时间
	};

	// SNS好友分组数据库表：SNSBuddyGroup
	struct DB_Table_SNSBuddyGroup
	{
		DWORD	dwBuddyGroupID;								// 好友分组ID
		DWORD	dwUserID;									// 账号ID，这个分组的归属者
		char	dwName[DBDEF_SNS_BUDDYGROUPNAME_MAXSIZE];	// 分组名称
	};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_ACTORINFO_UPDATE 更新角色信息：传入参数结构
	// 更新玩家角色信息，没有则添加
	struct DB_Param_Update_SNSActorInfo
	{
		DB_Table_SNSActor actorInfo;
		DWORD	dwNow;										// 当前时间

		DB_Param_Update_SNSActorInfo(void)
		{
			memset(this, 0, sizeof(DB_Param_Update_SNSActorInfo));
		}
	};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_ACTORINFO_UPDATE 更新角色信息：返回结果结构
	//struct DB_Result_Update_SNSActorInfo
	//{
	//	DB_Result_Update_SNSActorInfo(void)
	//	{
	//		memset(this, 0, sizeof(DB_Result_Update_SNSActorInfo));
	//	}
	//};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDY_LIST_READ 查询某个用户的所有好友：传入参数结构
	// 在DB_Table_SNSRelationship表中查找nStatus为非0的所有好友
	struct DB_Param_Read_SNSAllBuddy
	{
		DWORD	dwUserID;		// 账号ID

		DB_Param_Read_SNSAllBuddy(void)
		{
			memset(this, 0, sizeof(DB_Param_Read_SNSAllBuddy));
		}
	};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDY_LIST_READ 查询某个用户的所有好友：返回结果结构
	// 在DB_Table_SNSRelationship表中查找nStatus为非0的所有好友
	// 返回所有好友的角色信息和好友关系信息
	struct DB_Result_Read_SNSAllBuddy
	{
		int nRecordCount;		// 结果集个数

		// 结果集：nRecordCount * (DB_Table_SNSActor)
		DB_Result_Read_SNSAllBuddy(void)
		{
			memset(this, 0, sizeof(DB_Result_Read_SNSAllBuddy));
		}
	};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYRELATION_LIST_READ 读取玩家的好友关系：传入参数
	// 在DB_Table_SNSRelationship表中查找nStatus为非0的记录
	struct DB_Param_Read_SNSBuddyRelationList
	{
		DWORD	dwUserID;		// 账号ID

		DB_Param_Read_SNSBuddyRelationList()
		{
			memset(this, 0, sizeof(DB_Param_Read_SNSBuddyRelationList));
		}
	};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYRELATION_LIST_READ 读取玩家的好友关系：返回结果结构
	// 在DB_Table_SNSRelationship表中查找nStatus为非0的记录
	struct DB_Result_Read_SNSBuddyRelationList
	{
		int nRecordCount;		// 结果集个数

		// 结果集：nRecordCount * DB_Table_SNSRelationship
		DB_Result_Read_SNSBuddyRelationList()
		{
			memset(this, 0, sizeof(DB_Result_Read_SNSBuddyRelationList));
		}
	};

	/////////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYRELATION_UPDATE 更新好友关系信息：传入参数
	struct DB_Param_Update_SNSBuddyRelation
	{
		DB_Table_SNSRelationship relationInfo;

		DB_Param_Update_SNSBuddyRelation()
		{
			memset(this, 0, sizeof(DB_Param_Update_SNSBuddyRelation));
		}
	};

	//////////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYRELATION_STATUS_UPDATE  更新好友关系状态：传出参数
	struct DB_Param_Update_SNSBuddyRelationStatus
	{
		BYTE	nOptType;								// 该参数按原样返回就行；表示操作类型，比如加入黑名单，删除好友等等（当等于3时候，删除该条数据）

		DWORD	dwUserID1;								// 玩家1ID
		DWORD	dwUserID2;								// 玩家2ID
		BYTE	byStatus;								// 关系状态
		DWORD	dwNow;									// 当前时间

		DB_Param_Update_SNSBuddyRelationStatus()
		{
			memset(this, 0, sizeof(DB_Param_Update_SNSBuddyRelationStatus));
		}
	};

	//////////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYRELATION_STATUS_UPDATE  更新好友关系状态：传入参数
	struct DB_Result_Update_SNSBuddyRelationStatus
	{
		BYTE	nOptType;								// 该参数按原样返回就行；表示操作类型，比如加入黑名单，删除好友等等

		DWORD	dwUserID1;								// 玩家1ID
		DWORD	dwUserID2;								// 玩家2ID
		BYTE	byStatus;								// 关系状态1

		DB_Result_Update_SNSBuddyRelationStatus()
		{
			memset(this, 0, sizeof(DB_Result_Update_SNSBuddyRelationStatus));
		}
	};
	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_RELATION_STATUS_UPDATE 更新好友关系状态：传入参数结构
	// 更新好友状态，没有记录则新增一条
	// 1.如果有记录（只是例子）
	//		UPDATE SNSRelationship SET status=nStatus1 WHERE id1=dwUserID1,id2=dwUserID2
	//		UPDATE SNSRelationship SET status=nStatus2 WHERE id1=dwUserID2,id2=dwUserID1
	// 2.如果没有记录（只是例子）
	//		UPDATE SNSRelationship SET id1=dwUserID1,id2=dwUserID2, status=nStatus1, remark=(SELECT szActorName FROM SNSActor WHERE UserID=dwUserID2)
	//		UPDATE SNSRelationship SET id1=dwUserID2,id2=dwUserID1, status=nStatus2, remark=(SELECT szActorName FROM SNSActor WHERE UserID=dwUserID1)

	//struct DB_Param_Update_SNSRelationStatus
	//{
	//	BYTE	nOptType;								// 该参数按原样返回就行；表示操作类型，比如加入黑名单，删除好友等等
	//	DWORD	dwUserID1;								// 玩家1ID
	//	BYTE	nStatus1;								// 关系状态1
	//	char	szRemark1[DBDEF_SNS_REMARKNAME_MAXSIZE];// 玩家2在玩家1中的备注名
	//	
	//	DWORD	dwUserID2;								// 玩家2ID
	//	BYTE	nStatus2;								// 关系状态2
	//	char	szRemark2[DBDEF_SNS_REMARKNAME_MAXSIZE];// 玩家1在玩家2中的备注名		
	//	DWORD	dwNow;									// 当前时间戳

	//	DB_Param_Update_SNSRelationStatus(void)
	//	{
	//		memset(this, 0, sizeof(DB_Param_Update_SNSRelationStatus));
	//	}
	//};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_RELATION_STATUS_UPDATE 更新好友关系状态：返回结果结构
	// 返回修改后的值
	// 关系一：dwUserID1 dwUserID2 nStatus1 szRemark1
	// 关系二：dwUserID2 dwUserID1 nStatus2 szRemark2
	//struct DB_Result_Update_SNSRelationStatus
	//{
	//	BYTE	nOptType;								// 该参数按原样返回就行；表示操作类型，比如加入黑名单，删除好友等等

	//	DWORD	dwUserID1;								// 玩家1ID
	//	BYTE	nStatus1;								// 关系状态1
	//	char	szRemark1[DBDEF_SNS_REMARKNAME_MAXSIZE];// 玩家2在玩家1中的备注名

	//	DWORD	dwUserID2;								// 玩家2ID
	//	BYTE	nStatus2;								// 关系状态2
	//	char	szRemark2[DBDEF_SNS_REMARKNAME_MAXSIZE];// 玩家1在玩家2中的备注名

	//	DB_Result_Update_SNSRelationStatus(void)
	//	{
	//		memset(this, 0, sizeof(DB_Result_Update_SNSRelationStatus));
	//	}
	//};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYGROUP_LIST_READ 读取玩家的好友分组：传入参数结构
	struct DB_Param_Read_SNSBuddyGroup
	{
		DWORD	dwUserID;		// 玩家ID

		DB_Param_Read_SNSBuddyGroup(void)
		{
			memset(this, 0, sizeof(DB_Param_Read_SNSBuddyGroup));
		}
	};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYGROUP_LIST_READ 读取玩家的好友分组：返回结果结构
	struct DB_Result_Read_SNSBuddyGroup
	{
		int nRecordCount;		// 结果集个数

		// 返回结果：nRecordCount * DB_Table_SNSBuddyGroup
		DB_Result_Read_SNSBuddyGroup(void)
		{
			memset(this, 0, sizeof(DB_Result_Read_SNSBuddyGroup));
		}
	};


	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_REMARK_UPDATE 更新好友备注名：传入参数结构
	//struct DB_Param_Update_SNSRemark
	//{
	//	DWORD	dwUserID1;								// 用户ID
	//	DWORD	dwUserID2;								// 被修改的好友ID
	//	char	szRemark[DBDEF_SNS_REMARKNAME_MAXSIZE];		// 新的备注名

	//	DB_Param_Update_SNSRemark(void)
	//	{
	//		memset(this, 0, sizeof(DB_Param_Update_SNSRemark));
	//	}
	//};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_REMARK_UPDATE 更新好友备注名：返回结果结构
	//struct DB_Result_Update_SNSRemark
	//{
	//	DWORD	dwUserID1;								// 用户ID
	//	DWORD	dwUserID2;								// 被修改的好友ID
	//	char	szRemark[DBDEF_SNS_REMARKNAME_MAXSIZE];		// 新的备注名

	//	DB_Result_Update_SNSRemark(void)
	//	{
	//		memset(this, 0, sizeof(DB_Result_Update_SNSRemark));
	//	}
	//};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYGROUP_UPDATE 更新好友分组信息：传入参数结构
	// 更新好友分组信息，没有则添加
	struct DB_Param_Update_SNSBuddyGroup
	{
		BYTE	nOptType;									// 该参数按原样返回就行；表示操作类型，增加分组，删除分组、修改名字（当等于1时候，删除该条数据）

		DB_Table_SNSBuddyGroup buddyGroup;
		//DWORD	dwBuddyGroupID;								// 分组ID
		//DWORD	dwUserID;									// 用户ID
		//char	szName[DBDEF_SNS_BUDDYGROUPNAME_MAXSIZE];	// 分组名字

		DB_Param_Update_SNSBuddyGroup(void)
		{
			memset(this, 0, sizeof(DB_Param_Update_SNSBuddyGroup));
		}
	};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDYGROUP_UPDATE 更新好友分组信息：返回结果结构
	//struct DB_Result_Update_SNSBuddyGroup
	//{
	//	BYTE	nOptType;										// 该参数按原样返回就行；表示操作类型，增加分组，删除分组、修改名字

	//	DWORD	dwBuddyGroupID;									// 分组ID
	//	DWORD	dwUserID;									// 用户ID
	//	char	szName[DBDEF_SNS_BUDDYGROUPNAME_MAXSIZE];		// 分组名字

	//	DB_Result_Update_SNSBuddyGroup(void)
	//	{
	//		memset(this, 0, sizeof(DB_Result_Update_SNSBuddyGroup));
	//	}
	//};

	/////////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_BUDDY_INFO_READ 查询玩家信息
	struct DB_Param_Read_BuddyInfo
	{
		//BYTE	nOptType;								// 该参数按原样返回就行；表示操作类型，通过名字查找好友、查看好友信息

		DWORD dwUserID;									// 玩家账号ID
		//char szActorName[DBDEF_ACTORNAME_MAXSIZE];		// 角色名字

		DB_Param_Read_BuddyInfo(void)
		{
			memset(this, 0, sizeof(DB_Param_Read_BuddyInfo));
		}
	};

	// GAMEDB_REQUEST_SNS_BUDDY_INFO_READ 查询玩家信息
	struct DB_Result_Read_BuddyInfo
	{
		//BYTE	nOptType;								// 该参数按原样返回就行；表示操作类型，通过名字查找好友、查看好友信息

		DB_Table_SNSActor actorInfo;					// 角色信息

		DB_Result_Read_BuddyInfo(void)
		{
			memset(this, 0, sizeof(DB_Result_Read_BuddyInfo));
		}
	};

	// GAMEDB_REQUEST_SNS_CHANGE_BUDDYGROUP 更改好友的分组：传入参数
	// 修改DB_Table_SNSRelationship表
	//struct DB_Param_Change_BuddyGroup
	//{
	//	DWORD dwUserID1;				// 用户1
	//	DWORD dwUserID2;				// 用户2
	//	DWORD dwBuddyGroupID;			// 新的好友分组ID

	//	DB_Param_Change_BuddyGroup(void)
	//	{
	//		memset(this, 0, sizeof(DB_Param_Change_BuddyGroup));
	//	}
	//};

	// GAMEDB_REQUEST_SNS_CHANGE_BUDDYGROUP 更改好友的分组：返回参数
	//struct DB_Result_Change_BuddyGroup
	//{
	//	DWORD dwUserID1;				// 用户1
	//	DWORD dwUserID2;				// 用户2
	//	DWORD dwBuddyGroupID;			// 新的好友分组ID

	//	DB_Result_Change_BuddyGroup(void)
	//	{
	//		memset(this, 0, sizeof(DB_Result_Change_BuddyGroup));
	//	}
	//};

	// GAMEDB_REQUEST_SNS_BUDDYRELATION_READ 获取好友关系状态：传入参数
	struct DB_Param_Read_BuddyRelation
	{
		DWORD dwUserID1;			// 账号ID1
		DWORD dwUserID2;			// 账号ID2

		DB_Param_Read_BuddyRelation(void)
		{
			memset(this, 0, sizeof(DB_Param_Read_BuddyRelation));
		}
	};

	// GAMEDB_REQUEST_SNS_BUDDYRELATION_READ 获取好友关系状态：返回参数
	struct DB_Result_Read_BuddyRelation
	{
		DB_Table_SNSRelationship relationInfo;		// 好友关系状态

		DB_Result_Read_BuddyRelation(void)
		{
			memset(this, 0, sizeof(DB_Result_Read_BuddyRelation));
		}
	};

	// GAMEDB_REQUEST_SNS_USER_UPDATE 更新用户信息：传入参数结构
	// 更新用户信息，没有则添加
	struct DB_Param_Update_SNSUserInfo
	{
		DB_Table_SNSUser UserInfo;

		DB_Param_Update_SNSUserInfo(void)
		{
			memset(this, 0, sizeof(DB_Param_Update_SNSUserInfo));
		}
	};
	////////////////////////////////////////////////////
	//// GAMEDB_REQUEST_SNS_USER_UPDATE 更新用户信息：传出参数结构
	//// 更新用户信息，返回结果
	//struct DB_Result_Update_SNSUserInfo
	//{
	//	DB_Result_Update_SNSUserInfo(void)
	//	{
	//		memset(this, 0, sizeof(DB_Result_Update_SNSUserInfo));
	//	}
	//};

	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_USER_READ 读取用户信息：传入参数结构
	// 读取用户信息
	struct DB_Param_Read_SNSUserInfo
	{
		DWORD dwUserID;

		DB_Param_Read_SNSUserInfo(void)
		{
			memset(this, 0, sizeof(DB_Param_Read_SNSUserInfo));
		}
	};
	//////////////////////////////////////////////////
	// GAMEDB_REQUEST_SNS_USER_READ 读取用户信息：传入参数结构
	// 读取用户信息，返回结果
	struct DB_Result_Read_SNSUserInfo
	{
		DB_Table_SNSUser UserInfo;

		DB_Result_Read_SNSUserInfo(void)
		{
			memset(this, 0, sizeof(DB_Result_Read_SNSUserInfo));
		}
	};


	//////////////////////////////////////////////////
	
}
#pragma pack()

#endif//__ISNSDATABASEDEF_H__