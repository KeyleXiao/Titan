/*******************************************************************
** 文件名:	Common\IVoiceDataBaseDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2013-03-20
** 版  本:	1.0
** 描  述:	语音服务器 定义访问游戏数据库时需要用到的各种数据结构
** 应  用:  语音服务器数据库接口独立与游戏的接口,使用独立的 DBVoiceServer,DBVoiceProcedure.dbs	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __I_VOICE_DATABASE_DEFINE_H__
#define __I_VOICE_DATABASE_DEFINE_H__


#pragma pack(1)


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*********************************** 常量定义 **********************************************************/
	#define  DBDEF_VOICE_NAME_MAXSIZE		32		//名称最大长度
	#define  DBDEF_VOICE_MSGSTR_MAXSIZE		64		//信息文字最大长度,64
	#define  DBDEF_VOICE_TITLE_MAXSIZE		128		//标题字串的长度
	#define  DBDEF_VOICE_LONGSTR_MAXSIZE	256		//长字串的长度
	#define  DBDEF_VOICE_TEXTSTR_MAXSIZE	512		//大字串的长度

	#define  VOICE_SERVER_DBID			1		// 语音服务器的数据库ID

	// 定义访问游戏数据库时需要用到的存储过程及存储过程参数结构
	// 语音服务器数据库接口独立与游戏的接口,使用独立的 DBVoiceServer,DBVoiceProcedure.dbs

	enum EMVoiceDBRequestCmdID
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////// 语音服务器 ///////////////////////////////////////////////////
		VOICEDB_REQUEST_VOICE_INVALID = 0,
		// 操作表名: VoiceUser VoiceActor
		VOICEDB_REQUEST_VOICEUSER_START,			// 语音服务器开启了,要清理一下上线状态等初始化操作
		VOICEDB_REQUEST_VOICEUSER_LOGIN,			// 语音角色用户登录
		VOICEDB_REQUEST_VOICEUSER_LOGOUT,			// 语音角色用户登出
		VOICEDB_REQUEST_VOICEUSER_ENTER,			// 语音角色用户进入频道
		VOICEDB_REQUEST_VOICEUSER_UPDATE,			// 语音角色用户更新数据
		// 操作表名: VoiceChannel
		VOICEDB_REQUEST_VOICECHANNEL_READ,			// 读取语音频道信息
		VOICEDB_REQUEST_VOICECHANNEL_SAVE,			// 保存语音频道信息
		// 操作表名: VoiceRoom
		VOICEDB_REQUEST_VOICEROOM_READ,				// 读取语音频道房间信息
		VOICEDB_REQUEST_VOICEROOM_SAVE,				// 保存语音频道房间信息
		// 操作表名: VoiceVestInfo
		VOICEDB_REQUEST_VOICEVEST_READ,				// 读取马甲权限用户信息
		VOICEDB_REQUEST_VOICEVEST_ADD,				// 新增马甲权限用户信息
		VOICEDB_REQUEST_VOICEVEST_DEL,				// 删除马甲权限用户信息
		// 操作表名: VoiceFansUser
		VOICEDB_REQUEST_VOICEFANS_READ,				// 读取粉丝关系信息
		VOICEDB_REQUEST_VOICEFANS_ADD,				// 新增粉丝关系
		VOICEDB_REQUEST_VOICEFANS_DEL,				// 删除粉丝关系
		VOICEDB_REQUEST_VOICEFANS_FIND,				// 查找粉丝
		VOICEDB_REQUEST_VOICEFANS_TOPLIST,			// 粉丝排行榜
		
		// 操作表名: VoiceNumValue
		VOICEDB_REQUEST_VOICENUMVALUE_READ,			// 读取语音数据
		VOICEDB_REQUEST_VOICENUMVALUE_ADDBLUECOUNT,	// 增加蓝钻数

	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*************************************** 语音服务器相关 表定义****************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 语音通行证用户信息表 数据库表: 表名 VoiceUser
	// 所有角色共用的信息
	struct DB_Table_VoiceUser
	{
		DWORD dwUDBID;			// 通行证UDBID,唯一
		DWORD dwPoint;			// 积分,增量修改,可多处修改
		DWORD dwFlower;			// 鲜花数量,增量修改,可多处修改
		DWORD dwExp;			// 经验,可换算成等级,增量修改,可多处修改
		DWORD dwInviteTime;		// 最后邀请粉丝时间，用来冷却
		DWORD dwFansCount;		// 所有粉丝数，在加粉丝时+1,删除时-1,这样可提高登录的性能
		DWORD dwOldFansCount;	// 昨日粉丝数
		DWORD dwFlag;			// 状态标识
		DWORD dwUserType;		// 用户类型
		DWORD dwSpare1;			// 备用1
		DWORD dwSpare2;			// 备用2
		DWORD dwSpare3;			// 备用3
		//DWORD dwUpdateFansTime;		// 真实数粉丝数时间，用来冷却，这个数据操作内部用，程序不管

		DB_Table_VoiceUser(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 语音角色信息表 数据库表: 表名 VoiceActor
	struct DB_Table_VoiceActor
	{
		DWORD dwPDBID;									// 角色的PDBID,唯一
		DWORD dwUDBID;									// 通行证UDBID
		DWORD dwSvrWorldID;								// 语音服所属游戏世界ID
		DWORD dwFromWorldID;							// 角色登陆所属游戏世界ID
		BYTE nBelongID;									// 角色登陆所属ID(国家，大陆。。)
		char szActorName[DBDEF_VOICE_NAME_MAXSIZE];		// 角色登陆角色名
		BYTE nOnline;									// 在线状态,0:不在线,1:在线
		DWORD dwLoginTime;								// 上线时间
		DWORD dwLogoutTime;								// 下线时间
		LONGLONG nChannelID;							// 所在频道ID
		DWORD dwChannelTime;							// 进入频道时间
		DWORD dwFlag;									// 状态标识
		DWORD dwSpare1;									// 备用1
		DWORD dwSpare2;									// 备用2
		DWORD dwSpare3;									// 备用3

		DB_Table_VoiceActor(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 语音通行证用户的粉丝关系表 数据库表: 表名 VoiceFansUser
	struct DB_Table_VoiceFansUser
	{
		DWORD dwUDBID;			// 关注的用户通行证UDBID
		DWORD dwFansUDBID;		// 粉丝的通行证UDBID
		DWORD dwAddTime;		// 加入时间，加记录时自动加上当前时间

		DB_Table_VoiceFansUser(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 语音粉丝排行榜结点
	struct DB_Table_VoiceFansData
	{
		DWORD dwUDBID;									// 通行证UDBID
		DWORD dwUserType;								// 通行证帐号类型
		DWORD nRank;									// 名次
		DWORD dwFansCount;								// 粉丝总数
		DWORD dwPDBID;									// 最近登录的角色ID
		DWORD dwFromWorldID;							// 最近登录的角色来源世界ID
		BYTE nBelongID;									// 最近登录的角色所属ID(国家，大陆。。)
		char szActorName[DBDEF_VOICE_NAME_MAXSIZE];		// 最近登录的角色角色名
		
		DB_Table_VoiceFansData(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 语音频道信息表 数据库表: 表名 VoiceChannel
	struct DB_Table_VoiceChannel
	{
		LONGLONG nChannelID;	// 频道ID

		DWORD dwType;			// 频道类型
		DWORD dwWorldID;		// 世界ID
		DWORD dwDataID;			// 频道数值ID

		DWORD dwOwnerUDBID;		// 主人通行证UDBID
		DWORD dwMaxOnline;		// 最高在线
		DWORD dwQuality;		// 语音品质
		DWORD dwRoomID;			// 默认进入的房间ID
		DWORD dwFlag;			// 状态标识
		DWORD dwUseType;		// 应用类型

		char szName[DBDEF_VOICE_NAME_MAXSIZE];	// 语音频道名称
		DWORD dwSpare1;			// 备用1
		DWORD dwSpare2;			// 备用2
		DWORD dwSpare3;			// 备用3

		DWORD dwOwnerType;		// 主人通行证类型 DB_Table_VoiceUser的dwUserType字段

		DB_Table_VoiceChannel(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 语音频道房间信息表 数据库表: 表名 VoiceRoom
	struct DB_Table_VoiceRoom
	{
		LONGLONG nChannelID;	// 频道ID
		DWORD dwRoomID;			// 房间ID
		DWORD dwFlag;			// 状态标识
		DWORD dwMicTimes;		// 麦时间
		DWORD dwMicState;		// 默认麦序模式
		char szName[DBDEF_VOICE_NAME_MAXSIZE];	// 房间名称
		char szPassword[DBDEF_VOICE_NAME_MAXSIZE];	// 房间密码
		char szTitle[DBDEF_VOICE_TEXTSTR_MAXSIZE];	// 房间公告
		DWORD dwSpare1;			// 备用1
		DWORD dwSpare2;			// 备用2
		DWORD dwSpare3;			// 备用3

		DB_Table_VoiceRoom(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 语音频道马甲权限用户信息表 数据库表: 表名 VoiceVestInfo
	struct DB_Table_VoiceVestInfo
	{
		LONGLONG nChannelID;	// 频道ID
		DWORD dwPDBID;			// 用户角色ID
		BYTE nUserType;			// 用户类型
		BYTE nSex;				// 性别
		DWORD dwFlag;			// 状态标识
		char szName[DBDEF_VOICE_NAME_MAXSIZE];
		DWORD dwSpare1;			// 备用1
		DWORD dwSpare2;			// 备用2
		DWORD dwSpare3;			// 备用3
		DWORD dwAddTime;		// 加入时间，加记录时自动加上

		DB_Table_VoiceVestInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 语音通行证数值表 数据库表: 表名 VoiceNumValue
	// 所有角色共用的信息
	struct DB_Table_VoiceNumValue
	{
		DWORD dwUDBID;			// 通行证UDBID,唯一
		DWORD dwBlueDiamond;	// 蓝钻总数，一直累加
		DWORD dwCurBlueDiamond;	// 当前蓝钻数，只有这个数值才可结算成红钻
		DWORD dwBlueDiamondOld;	// 当月之前的总数
		DWORD dwBlueDiamondToday;	// 今日获得蓝钻数，第二天自动把今天的这个值添加到dwCurBlueCount中，然后清零

		DWORD dwRedDiamond;		// 红钻数，这部分可结算成金币/RMB

		DWORD dwSpare1;			// 备用1
		DWORD dwSpare2;			// 备用2
		DWORD dwSpare3;			// 备用3

		DB_Table_VoiceNumValue(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/************************* 语音服务器相关所有数据库操作接口定义 ******************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: VoiceUser VoiceActor

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEUSER_START 语音服务器开启了,要清理一下上线状态等初始化操作 : 传入参数结构
	struct DB_Param_VoiceUser_Start
	{
		// 启动:所属游戏世界ID的角色用户在线nOnline=1的清为0
		// 关服:所属游戏世界ID的角色用户在线nOnline=1的清为0，dwLoginTime=当前时间
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID
		bool  bIsStart;			// 是否启动，true为启动，false为关服

		DB_Param_VoiceUser_Start(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEUSER_LOGIN 语音角色用户登录 : 传入参数结构
	struct DB_Param_VoiceUser_Login
	{
		// 没有找到角色的PDBID,新加一条，没找到通行证UDBID,新加一条,有就更新 nOnline=1,dwLoginTime=当前时间,dwSvrWorldID,dwFromWorldID,nCountry,szName
		DWORD dwPDBID;									// 角色的PDBID
		DWORD dwUDBID;									// 通行证UDBID
		DWORD dwSvrWorldID;								// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		DWORD dwFromWorldID;							// 角色登陆所属游戏世界ID
		BYTE nBelongID;									// 角色登陆所属ID(国家，大陆。。)
		char szActorName[DBDEF_VOICE_NAME_MAXSIZE];		// 角色登陆角色名

		DB_Param_VoiceUser_Login(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	/// 语音角色用户登录: 返回结果 
	struct DB_Result_VoiceUser_Login
	{
		int nRecordCount;					// 返回记录的个数,没找到返回0

		// 结果集：nRecordCount * DB_Table_VoiceUser

		DB_Result_VoiceUser_Login(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEUSER_LOGOUT 语音角色用户登出 : 传入参数结构
	struct DB_Param_VoiceUser_Logout
	{
		// 没有找到角色为PDBID,就不处理,有就更新 nOnline=0,dwLogoutTime=当前时间
		DWORD dwPDBID;			// 角色的PDBID
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		DB_Param_VoiceUser_Logout(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果


	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEUSER_ENTER 语音角色用户进入频道 : 传入参数结构
	struct DB_Param_VoiceUser_Enter
	{
		// 没有找到角色为PDBID,就不处理,有就更新 dwChannelTime=当前时间,nChannelID
		DWORD dwPDBID;			// 角色的PDBID
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		LONGLONG nChannelID;	// 所在频道ID

		DB_Param_VoiceUser_Enter(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEUSER_UPDATE 语音角色用户更新数据 : 传入参数结构
	struct DB_Param_VoiceUser_Update
	{
		// 没有找到通行证UDBID,就不处理,有就更新 dwPoint,dwFlower,dwExp
		DWORD dwPDBID;			// 角色的PDBID
		DWORD dwUDBID;			// 通行证UDBID
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		DWORD dwFromWorldID;	// 角色登陆所属游戏世界ID

		int nChangePoint;		// 变化积分,增量修改,可多处修改
		int nChangeFlower;		// 变化鲜花数量,增量修改,可多处修改
		int nChangeExp;			// 变化经验,可换算成等级,增量修改,可多处修改

		DB_Param_VoiceUser_Update(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: VoiceChannel

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICECHANNEL_READ 读取语音频道信息 : 传入参数结构
	struct DB_Param_VoiceChannel_Read
	{
		// 条件参数,找到nChannelID的记录返回
		LONGLONG nChannelID;	// 频道ID
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		DB_Param_VoiceChannel_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 读取语音频道信息: 返回结果
	struct DB_Result_VoiceChannel_Read
	{
		int nRecordCount;					// 返回记录的个数,没找到返回0

		// 结果集：nRecordCount * DB_Table_VoiceChannel 

		DB_Result_VoiceChannel_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICECHANNEL_SAVE 保存语音频道信息 : 传入参数结构
	struct DB_Param_VoiceChannel_Save
	{
		// 条件参数,找到nChannelID的记录来修改，否则不修改
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		bool bIsNew;			// 是否是新增,如是新增，如已有记录就失败不操作，以免覆盖,如是修改，没有记录时可加一条

		// 要保存的数据
		LONGLONG nChannelID;	// 频道ID
		DWORD dwType;			// 频道类型
		DWORD dwWorldID;		// 世界ID
		DWORD dwDataID;			// 频道数值ID

		DWORD dwOwnerUDBID;		// 主人通行证UDBID
		DWORD dwMaxOnline;		// 最高在线
		DWORD dwQuality;		// 语音品质
		DWORD dwRoomID;			// 默认进入的房间ID
		DWORD dwFlag;			// 状态标识
		DWORD dwUseType;		// 应用类型

		char szName[DBDEF_VOICE_NAME_MAXSIZE];	// 语音频道名称
		DWORD dwSpare1;			// 备用1
		DWORD dwSpare2;			// 备用2
		DWORD dwSpare3;			// 备用3


		DB_Param_VoiceChannel_Save(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: VoiceRoom

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEROOM_READ 读取语音频道房间信息 : 传入参数结构
	struct DB_Param_VoiceRoom_Read
	{
		// 条件参数,找到nChannelID的所有房间记录,并返回
		LONGLONG nChannelID;	// 频道ID
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		DB_Param_VoiceRoom_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 读取语音频道房间信息: 返回结果
	struct DB_Result_VoiceRoom_Read
	{
		int nRecordCount;					// 返回记录的个数

		// 结果集：nRecordCount * DB_Table_VoiceRoom

		DB_Result_VoiceRoom_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEROOM_SAVE 保存语音频道房间信息 : 传入参数结构
	struct DB_Param_VoiceRoom_Save
	{
		// 条件参数,找到nChannelID的记录来修改，否则不修改
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		bool bIsNew;			// 是否是新增,如是新增，如已有记录就失败不操作，以免覆盖,如是修改，没有记录时可加一条

		// 要保存的数据
		LONGLONG nChannelID;	// 频道ID
		DWORD dwRoomID;			// 房间ID
		DWORD dwFlag;			// 状态标识
		DWORD dwMicTimes;		// 麦时间
		DWORD dwMicState;		// 默认麦序模式
		char szName[DBDEF_VOICE_NAME_MAXSIZE];	// 房间名称
		char szPassword[DBDEF_VOICE_NAME_MAXSIZE];	// 房间密码
		char szTitle[DBDEF_VOICE_TEXTSTR_MAXSIZE];	// 房间公告
		DWORD dwSpare1;			// 备用1
		DWORD dwSpare2;			// 备用2
		DWORD dwSpare3;			// 备用3


		DB_Param_VoiceRoom_Save(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: VoiceVestInfo

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEVEST_READ 读取马甲权限用户信息 : 传入参数结构
	struct DB_Param_VoiceVestInfo_Read
	{
		// 条件参数,找到nChannelID的所有房间记录,并返回
		LONGLONG nChannelID;	// 频道ID
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		DB_Param_VoiceVestInfo_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 读取马甲权限用户信息: 返回结果
	struct DB_Result_VoiceVestInfo_Read
	{
		int nRecordCount;					// 返回记录的个数

		// 结果集：nRecordCount * DB_Table_VoiceVestInfo

		DB_Result_VoiceVestInfo_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEVEST_ADD 新增马甲权限用户信息 : 传入参数结构
	struct DB_Param_VoiceVestInfo_Add
	{
		// 如找到为nChannelID,dwPDBID的记录就修改，否则新增
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		// 要保存的数据
		LONGLONG nChannelID;	// 频道ID
		DWORD dwPDBID;			// 用户角色ID
		BYTE nUserType;			// 用户类型
		BYTE nSex;				// 性别
		DWORD dwFlag;			// 状态标识
		char szName[DBDEF_VOICE_NAME_MAXSIZE];
		DWORD dwSpare1;			// 备用1
		DWORD dwSpare2;			// 备用2
		DWORD dwSpare3;			// 备用3


		DB_Param_VoiceVestInfo_Add(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果


	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEVEST_DEL 删除马甲权限用户信息 : 传入参数结构
	struct DB_Param_VoiceVestInfo_Del
	{
		// 如找到为nChannelID,dwPDBID的记录就删除
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		// 条件参数
		LONGLONG nChannelID;	// 频道ID
		DWORD dwPDBID;			// 用户角色ID

		DB_Param_VoiceVestInfo_Del(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: VoiceFansUser

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEFANS_READ 读取粉丝关系信息 : 传入参数结构
	struct DB_Param_VoiceFansUser_Read
	{
		// 条件参数,找到dwFansUDBID的所有粉丝记录,并返回些用户关注的用户通行证UDBID列表,最多1000个
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		DWORD dwFansUDBID;		// 粉丝的通行证UDBID

		DB_Param_VoiceFansUser_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 读取粉丝关系信息: 返回结果
	struct DB_Result_VoiceFansUser_Read
	{
		int nRecordCount;					// 返回记录的个数

		// 结果集：nRecordCount * DWORD dwUDBID

		DB_Result_VoiceFansUser_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEFANS_ADD 新增粉丝关系 : 传入参数结构
	struct DB_Param_VoiceFansUser_Add
	{
		// 如果存在为 dwUDBID,dwFansUDBID的记录，就不处理，无就新增，并且VoiceUser.dwFansCount 在加粉丝时+1,这样可提高登录的性能
		// 当前时间比VoiceUser.dwUpdateFansTime大1周（这个DB自己可定义时长，主要是来提高登录的性能的）,就查dwUDBID的所有粉丝数并更新到VoiceUser.dwFansCount中
		// 并修改VoiceUser.dwUpdateFansTime 为当前时间
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		// 要保存的数据
		DWORD dwUDBID;			// 关注的用户通行证UDBID
		DWORD dwFansUDBID;		// 粉丝的通行证UDBID

		DB_Param_VoiceFansUser_Add(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 新增粉丝关系: 返回结果
	struct DB_Result_VoiceFansUser_Add
	{
		int nFansCount;					// 返回粉丝数总数
		int OldFansCount;				// 昨天粉丝总数

		DB_Result_VoiceFansUser_Add(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEFANS_DEL 删除粉丝关系 : 传入参数结构
	struct DB_Param_VoiceFansUser_Del
	{
		// 删除 dwUDBID,dwFansUDBID的记录,并且VoiceUser.dwFansCount 删除粉丝时-1,这样可提高登录的性能，没有就不处理，
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		// 条件参数
		DWORD dwUDBID;			// 关注的用户通行证UDBID
		DWORD dwFansUDBID;		// 粉丝的通行证UDBID

		DB_Param_VoiceFansUser_Del(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 删除粉丝关系: 返回结果
	struct DB_Result_VoiceFansUser_Del
	{
		int nFansCount;					// 返回粉丝数总数
		int OldFansCount;				// 昨天粉丝总数

		DB_Result_VoiceFansUser_Del(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICEFANS_FIND 查找粉丝 : 传入参数结构
	// 注：有些主播可能粉丝上几万或更多，这个功能是用来用户邀请在线粉丝用的，要用到在线用户表来查询
	struct DB_Param_VoiceFansUser_Find
	{
		// 找到VoiceFansUser.dwUDBID==dwUDBID,并且VoiceFansUser.dwFansUDBID在VoiceActor.dwUDBID中在线的所有VoiceActor.dwPDBID结果集
		// 并且要判断 当前时间now - VoiceUser.dwInviteTime>dwCoolTime 才可找记录,否则返回0条
		// 最多返回10000条

		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		DWORD dwUDBID;			// 关注的用户通行证UDBID
		DWORD dwCoolTime;		// 要冷却的时长秒数<60就默认3600

		DB_Param_VoiceFansUser_Find(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 读取粉丝关系信息: 返回结果
	struct DB_Result_VoiceFansUser_Find
	{
		int nResult;						// 处理结果标识: 1.表示正常,0.表示正在冷却
		DWORD dwInviteTime;					// 最后邀请粉丝时间，用来显示冷却明间
		int nRecordCount;					// 返回记录的个数

		// 结果集：nRecordCount * DWORD dwPDBID

		DB_Result_VoiceFansUser_Find(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// VOICEDB_REQUEST_VOICEFANS_TOPLIST			// 粉丝排行榜
	// 注：如果VoiceActor表中的szName长度为0，则不进入排行榜
	// 查询VoiceActor.dwSvrWorldID = dwSvrWorldID 且VoiceActor.szName的长度大于0，且VoiceUser.dwFansCount大于0 集合中前50名
	struct DB_Param_VoiceFansUser_TopList
	{
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID

		DB_Param_VoiceFansUser_TopList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 粉丝排行榜 返回结果
	struct DB_Result_VoiceFansUser_TopList
	{
		int nRecordCount;					// 返回记录的个数

		// 结果集：nRecordCount * DB_Table_VoiceFansData
		
		DB_Result_VoiceFansUser_TopList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	//////////////////////////////////////////////////////////////////////////
	// 操作表名: VoiceNumValue

	///////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICENUMVALUE_READ 读取语音数据 : 传入参数结构
	struct DB_Param_VoiceNumValue_Read
	{
		// 条件参数,找到dwUDBID的数值记录,并返回
		DWORD dwUDBID;			// 通行证帐号ID
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		DB_Param_VoiceNumValue_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 读取语音数据: 返回结果
	struct DB_Result_VoiceNumValue_Read
	{
		int nRecordCount;					// 返回记录的个数

		// 结果集：nRecordCount * DB_Table_VoiceNumValue

		DB_Result_VoiceNumValue_Read(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//////////////////////////////////////////////////////////////////////////
	/// VOICEDB_REQUEST_VOICENUMVALUE_ADDBLUECOUNT 增加蓝钻数 : 传入参数结构
	struct DB_Param_VoiceNumValue_AddBlueDiamond
	{
		// 如果，dwUDBID 对应的 VoiceNumValue 不存在就新增一条记录，然后在 VoiceNumValue.dwBlueDiamondToday 上添加 dwCount
		DWORD dwSvrWorldID;		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用

		// 要保存的数据
		DWORD dwUDBID;			// 增加的通行证
		DWORD dwPDBID;			// 得到蓝钻的通行证下对应的角色ID，用来记录日志用
		DWORD dwPDBIDSend;		// 赠送礼物的人PDBID，用来记录日志用
		DWORD dwDiamondCount;	// 增加的蓝钻数，一次最大不超过500000，超过的当作无效并报警
		DWORD dwGiftID;			// 接收的礼物ID

		DB_Param_VoiceNumValue_AddBlueDiamond(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 不用处理返回结果


#pragma pack()

#endif//__INETERFACE_VOICE_DATABASE_DEFINE_H__