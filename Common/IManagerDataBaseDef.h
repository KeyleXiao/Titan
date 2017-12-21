/*******************************************************************
** 文件名:	e:\Rocket\Common\IManagerDataBaseDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/20 10:05
** 版  本:	1.0
** 描  述:	服务器管理器 定义访问游戏数据库时需要用到的各种数据结构
** 应  用:  服务器管理器数据库接口独立与游戏的接口,使用独立的 DBManagerServer,DBManagerProcedure.dbs	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __I_MANAGER_DATABASE_DEFINE_H__
#define __I_MANAGER_DATABASE_DEFINE_H__


#pragma pack(1)


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*********************************** 常量定义 **********************************************************/
	#define  DBDEF_MANAGER_NAME_MAXSIZE		32		//名称最大长度
	#define  DBDEF_MANAGER_MSGSTR_MAXSIZE	64		//信息文字最大长度,64
	#define  DBDEF_MANAGER_TITLE_MAXSIZE	128		//标题字串的长度
	#define  DBDEF_MANAGER_LONGSTR_MAXSIZE	256		//长字串的长度
	#define  DBDEF_MANAGER_TEXTSTR_MAXSIZE	512		//大字串的长度
    #define  DBDEF_ASYNQUEUE_MAXINDEX           10      //

	// 定义访问游戏数据库时需要用到的存储过程及存储过程参数结构
	// 服务器管理器数据库接口独立与游戏的接口,使用独立的 DBManagerServer,DBManagerProcedure.dbs	

	enum
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////// 服务器管理器 ///////////////////////////////////////////////////
		GAMEDB_REQUEST_MANAGER_INVALID = 0,
		// 操作表名: ManagerGameList 游戏列表
		GAMEDB_REQUEST_MANAGERGAMELIST_READ,						// 查询游戏列表
		// 操作表名: ManagerAreaList 游戏区域列表
		GAMEDB_REQUEST_MANAGERAREALIST_READ,						// 查询游戏区域列表
		// 操作表名: ManagerWorldList 游戏世界列表
		GAMEDB_REQUEST_MANAGERWORLDLIST_READ,						// 查询游戏世界列表
		// 操作表名: ManagerComputerList 硬件服务器列表
		GAMEDB_REQUEST_MANAGERCOMPUTERLIST_READ,					// 查询硬件服务器列表
		// 操作表名: ManagerServerList 软件服务器列表
		GAMEDB_REQUEST_MANAGERSERVERLIST_READ,						// 查询软件服务器列表
		// 操作表名: ManagerUser 服务器管理员帐号信息
		GAMEDB_REQUEST_MANAGERUSER_LOGIN,							// 查询服务器管理员帐号信息,用户登录接口
		GAMEDB_REQUEST_MANAGERUSER_CHANGEPWD,						// 修改服务器管理员帐号信息,修改密码接口
		// 操作表名: ManagerLog 日志信息
		GAMEDB_REQUEST_MANAGERLOG_ADD,								// 添加服务器日志信息
		// 操作表名: ManagerWorldRight 管理员游戏世界权限
		GAMEDB_REQUEST_MANAGERWORLDRIGHT_READ,						// 查询管理员游戏世界权限
		// 操作表名: ManagerComputerInfo 服务器和登录机器信息
		GAMEDB_REQUEST_MANAGERCOMPUTERINFO_READ,					// 查询机器信息
		GAMEDB_REQUEST_MANAGERCOMPUTERINFO_ADD,						// 添加机器信息
		GAMEDB_REQUEST_MANAGERCOMPUTERINFO_DEL,						// 删除机器信息 
		// 操作表名: 启动服务器脚本文件数据表
		GAMEDB_REQUEST_MANAGERSCRIPTINFO_READ,						// 读取指定游戏世界启动脚本信息
		GAMEDB_REQUEST_MANAGERSCRIPT_READ,							// 读取启动服务器脚本文件数据
		GAMEDB_REQUEST_MANAGERDBSERVER_READ,						// 读取服务器DBServer脚本配置数据
	};


	// 数据库ID定义
	enum GLOBAL_DATABASEID
	{	
		GLOBAL_DATABASEID_Passport  = 5,	// 防沉迷（通行证）			
		GLOBAL_DATABASEID_SNS   	= 98,	// 好友
		GLOBAL_DATABASEID_Voice 	= 100,	// 语音
	};
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*************************************** 服务器管理器相关 表定义****************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 游戏列表 数据库表: 表名 ManagerGameList
	struct DB_Table_ManagerGameList
	{
		DWORD dwID;									// 游戏ID, 0为无效ID,数据库设为唯一 
		DWORD dwIndex;								// 游戏序号,用来排序用
		DWORD dwFlag;								// 游戏标识,0:无效,1:有效游戏,2:有效服务器群 
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];	// 游戏名称
		char szTitle[DBDEF_MANAGER_TITLE_MAXSIZE];	// 游戏说明
		DWORD dwSpare1;								// 备用1
		DWORD dwSpare2;								// 备用2
		DWORD dwSpare3;								// 备用3


		DB_Table_ManagerGameList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 游戏区域列表 数据库表: 表名 ManagerAreaList
	struct DB_Table_ManagerAreaList
	{
		DWORD dwID;									// 游戏区域ID, 0为无效ID,数据库设为自增唯一
		DWORD dwGameID;								// 所属游戏ID 取 ManagerGameList.dwID
		DWORD dwIndex;								// 游戏区域序号,用来排序用
		DWORD dwFlag;								// 游戏区域标识,0:无效,1:有效
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];	// 游戏区域名称
		char szTitle[DBDEF_MANAGER_TITLE_MAXSIZE];	// 游戏区域说明
		DWORD dwSpare1;								// 备用1
		DWORD dwSpare2;								// 备用2
		DWORD dwSpare3;								// 备用3


		DB_Table_ManagerAreaList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 游戏世界列表 数据库表: 表名 ManagerWorldList
	struct DB_Table_ManagerWorldList
	{
		DWORD dwID;									// 游戏世界ID, 0为无效ID,数据库设为自增唯一
		DWORD dwAreaID;								// 所属游戏区域ID 取 ManagerAreaList.dwID
		DWORD dwIndex;								// 游戏世界序号,用来排序用
		DWORD dwFlag;								// 游戏世界标识,0:无效,1:有效,2:推荐,3:新 
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];	// 游戏世界名称
		char szTitle[DBDEF_MANAGER_TITLE_MAXSIZE];	// 游戏世界说明
		char szAdUrl1[DBDEF_MANAGER_TITLE_MAXSIZE];	// 游戏世界广告Url连接1
		char szAdUrl2[DBDEF_MANAGER_TITLE_MAXSIZE];	// 游戏世界广告Url连接2
		DWORD dwSpare1;								// 备用1
		DWORD dwSpare2;								// 备用2
		DWORD dwSpare3;								// 备用3


		DB_Table_ManagerWorldList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 硬件服务器列表 数据库表: 表名 ManagerComputerList
	struct DB_Table_ManagerComputerList
	{
		DWORD dwID;									// 硬件服务器ID, 0为无效ID,数据库设为自增唯一
		DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
		DWORD dwIndex;								// 硬件服务器序号,用来排序用
		DWORD dwFlag;								// 硬件服务器标识,0:无效,1:有效
		DWORD dwComputerInfoID;						// 对应的机器ID  取 ManagerComputerInfo.dwID 
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];	// 硬件服务器名称
		char szTitle[DBDEF_MANAGER_TITLE_MAXSIZE];	// 硬件服务器说明
		char szMACAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 硬件服务器 MAC地址
		char szDiskSN[DBDEF_MANAGER_NAME_MAXSIZE];	// 硬件服务器硬盘序号 Disk Serial Number
		char szRegSN[DBDEF_MANAGER_TITLE_MAXSIZE];	// 硬件服务器授权注册码
		DWORD dwSpare1;								// 备用1
		DWORD dwSpare2;								// 备用2
		DWORD dwSpare3;								// 备用3


		DB_Table_ManagerComputerList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 软件服务器列表 数据库表: 表名 ManagerServerList
	struct DB_Table_ManagerServerList
	{
		DWORD dwID;									// 软件服务器ID, 0为无效ID,数据库设为自增唯一
		DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
		DWORD dwComputerID;							// 所属硬件服务器ID 取 ManagerComputerList.dwID
		DWORD dwIndex;								// 软件服务器序号,用来排序用
		DWORD dwType;								// 软件服务器类型,参考服务器类型定义
		DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
		DWORD dwFlag;								// 软件服务器标识,0:无效,1:有效
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];	// 软件服务器名称
		char szTitle[DBDEF_MANAGER_TITLE_MAXSIZE];	// 软件服务器说明
		DWORD dwSpare1;								// 备用1
		DWORD dwSpare2;								// 备用2
		DWORD dwSpare3;								// 备用3


		DB_Table_ManagerServerList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 服务器服管理员帐号信息 数据库表: 表名 ManagerUser 
	struct DB_Table_ManagerUser
	{
		DWORD dwID;										// 管理员帐号ID, 0为无效ID,数据库设为自增唯一
		DWORD dwType;									// 管理员帐号权限类型,参考权限类型定义
		DWORD dwFlag;									// 管理员帐号标识,0:无效,1:有效
		DWORD dwCheckPCFlag;							// 是否起用登录机器限制,0:不起用,1:起用
		DWORD dwLoginCount;								// 登录次数
		DWORD dwLastTime;								// 最近登录时间
		char szUserName[DBDEF_MANAGER_NAME_MAXSIZE];	// 管理员登录帐号
		char szPassword[DBDEF_MANAGER_MSGSTR_MAXSIZE];	// 管理员登录帐号密码
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];		// 管理员姓名
		char szDept[DBDEF_MANAGER_NAME_MAXSIZE];		// 管理员部门
		char szTitle[DBDEF_MANAGER_TITLE_MAXSIZE];		// 管理员说明
		char szMACAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 最近登录的网卡MAC地址
		char szIPAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 最近登录的IP
		DWORD dwSpare1;									// 备用1
		DWORD dwSpare2;									// 备用2
		DWORD dwSpare3;									// 备用3


		DB_Table_ManagerUser(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 服务器日志信息 数据库表: 表名 ManagerLog 
	struct DB_Table_ManagerLog
	{
		DWORD dwID;										// 日志ID, 0为无效ID,数据库设为自增唯一
		DWORD dwType;									// 日志类型,参考日志类型定义
		DWORD dwTime;									// 时间
		DWORD dwUserID;									// 操作管理员帐号ID,服务器的日志时为0
		DWORD dwWorldID;								// 游戏世界ID 
		DWORD dwComputerID;								// 硬件服务器ID 
		DWORD dwServerID;								// 软件服务器ID
		char szUserName[DBDEF_MANAGER_NAME_MAXSIZE];	// 管理员登录帐号
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];		// 管理员姓名
		char szLogTxt[DBDEF_MANAGER_TEXTSTR_MAXSIZE];	// 日志内容
		DWORD dwSpare1;									// 备用1
		DWORD dwSpare2;									// 备用2
		DWORD dwSpare3;									// 备用3


		DB_Table_ManagerLog(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 管理员游戏世界权限 数据库表: 表名 ManagerWorldRight
	struct DB_Table_ManagerWorldRight
	{
		DWORD dwID;									// 权限记录ID, 0为无效ID,数据库设为自增唯一
		DWORD dwUserID;								// 操作管理员帐号ID
		DWORD dwWorldID;							// 游戏世界ID
		DWORD dwRightValue;							// 权限数值,参考权限定义

		DB_Table_ManagerWorldRight(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 服务器和登录机器资料信息 数据库表: 表名 ManagerComputerInfo
	struct DB_Table_ManagerComputerInfo
	{
		DWORD dwID;										// 机器ID, 0为无效ID,数据库设为自增唯一
		DWORD dwWorldID;								// 所属游戏世界ID 取 ManagerWorldList.dwID,为0表示是客户端或无效
		DWORD dwType;									// 机器类型,参考机器类型定义 
		DWORD dwTime;									// 加入时间
		DWORD dwLastTime;								// 最近活动时间
		DWORD dwFlag;									// 机器标识,0:无效,1:有效,2:新未登记的
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];		// 机器名称
		char szPcName[DBDEF_MANAGER_NAME_MAXSIZE];		// 取到的电脑名称
		char szMACAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 网卡MAC地址
		char szDiskSN[DBDEF_MANAGER_NAME_MAXSIZE];		// 硬盘序号 Disk Serial Number 
		char szIPAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// IP地址
		char szTitle[DBDEF_MANAGER_TEXTSTR_MAXSIZE];	// 机器备注 
		DWORD dwSpare1;									// 备用1
		DWORD dwSpare2;									// 备用2
		DWORD dwSpare3;									// 备用3


		DB_Table_ManagerComputerInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 管理员可登录机器列表 数据库表: 表名 ManagerLoginPcList
	struct DB_Table_ManagerLoginPcList
	{
		DWORD dwID;									// 记录ID, 0为无效ID,数据库设为自增唯一
		DWORD dwUserID;								// 管理员帐号ID
		DWORD dwComputerID;							// 可登录机器ID
		DWORD dwFlag;								// 标识,0:无效,1:有效
		DWORD dwSpare1;								// 备用1
		DWORD dwSpare2;								// 备用2
		DWORD dwSpare3;								// 备用3 

		DB_Table_ManagerLoginPcList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/************************* 服务器管理器相关所有数据库操作接口定义 ******************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerGameList 游戏列表
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERGAMELIST_READ 查询游戏列表 : 传入参数结构
	struct DB_Param_Read_ManagerGameList
	{
		// 排序以dwIndex,dwID顺序
		DWORD dwID;							// 游戏ID，0为忽略

		DB_Param_Read_ManagerGameList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果
	struct DB_Result_Read_ManagerGameList
	{
		int nRecordCount;					// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Table_ManagerGameList

		DB_Result_Read_ManagerGameList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerAreaList 游戏区域列表
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERAREALIST_READ 查询游戏区域列表 : 传入参数结构
	struct DB_Param_Read_ManagerAreaList
	{
		// 条件参数,所有为and关系,排序以dwIndex,dwID顺序
		DWORD dwID;							// 游戏区域ID，0为忽略
		DWORD dwGameID;						// 所属游戏ID，0为忽略

		DB_Param_Read_ManagerAreaList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果
	struct DB_Result_Read_ManagerAreaList
	{
		int nRecordCount;					// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Table_ManagerAreaList

		DB_Result_Read_ManagerAreaList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerWorldList 游戏世界列表
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERWORLDLIST_READ 查询游戏世界列表 : 传入参数结构
	struct DB_Param_Read_ManagerWorldList
	{
		// 条件参数,所有为and关系,排序以dwIndex,dwID顺序
		DWORD dwID;							// 游戏世界ID，0为忽略
		DWORD dwAreaID;						// 所属游戏区域ID，0为忽略 

		DB_Param_Read_ManagerWorldList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果
	struct DB_Result_Read_ManagerWorldList
	{
		int nRecordCount;					// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Table_ManagerWorldList

		DB_Result_Read_ManagerWorldList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerComputerList 硬件服务器列表
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERCOMPUTERLIST_READ 查询硬件服务器列表 : 传入参数结构
	struct DB_Param_Read_ManagerComputerList
	{
		// 条件参数,所有为and关系,排序以dwIndex,dwID顺序
		DWORD dwID;							// 硬件服务器ID，0为忽略
		DWORD dwWorldID;					// 所属游戏世界ID，0为忽略 

		DB_Param_Read_ManagerComputerList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果
	struct DB_Result_Read_ManagerComputerList
	{
		int nRecordCount;					// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Table_ManagerComputerList

		DB_Result_Read_ManagerComputerList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerServerList 软件服务器列表
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERSERVERLIST_READ 查询软件服务器列表 : 传入参数结构
	struct DB_Param_Read_ManagerServerList
	{
		// 条件参数,所有为and关系,排序以dwIndex,dwID顺序
		DWORD dwID;							// 软件服务器ID，0为忽略
		DWORD dwWorldID;					// 所属游戏世界ID，0为忽略 

		DB_Param_Read_ManagerServerList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	// 返回结果
	struct DB_Result_Read_ManagerServerList
	{
		int nRecordCount;					// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Table_ManagerServerList

		DB_Result_Read_ManagerServerList(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerUser 服务器管理员帐号信息
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERUSER_LOGIN 查询服务器管理员帐号信息,用户登录接口 : 传入参数结构
	struct DB_Param_ManagerUser_Login
	{
		// 此接口为用户登录接口,请注意处理逻辑要严谨,清晰,处理代码不能有漏洞
		// 先验证用户名和密码，如果通过,如起用登录机器限制,就在可登录机器列表中检查网卡MAC地址和硬盘序号,
		// 管理员可登录机器列表ManagerLoginPcList中dwUserID为用户ID,dwFlag==1的记录
		// 找到就通过,没有就失败,成功时,更新最近登录时间,最近登录的网卡MAC地址,最近登录的IP信息,登录次数+1
		// 条件参数 
		char szUserName[DBDEF_MANAGER_NAME_MAXSIZE];	// 管理员登录帐号
		char szPassword[DBDEF_MANAGER_MSGSTR_MAXSIZE];	// 管理员登录帐号密码,已加密
		char szMACAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 登录机器的网卡MAC地址
		char szDiskSN[DBDEF_MANAGER_NAME_MAXSIZE];		// 登录机器硬盘序号 Disk Serial Number 
		char szIPAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 登录机器的IP

		DB_Param_ManagerUser_Login(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	// 返回结果
	struct DB_Result_ManagerUser_Login
	{
		BYTE nResult;				// 返回验证登录标识,0:验证失败,1:验证成功
		BYTE nFailCode;				// 验证失败的类型代码, 0,无, 1:用户被禁用,2:用户名不存在,3:密码不正确,4:网卡MAC地址/硬盘序号不正确 
		int nRecordCount;			// 返回管理员有权限的游戏世界记录的个数
		char szResultMsg[DBDEF_MANAGER_LONGSTR_MAXSIZE];	// 验证结果信息
		DB_Table_ManagerUser userInfo;		// 返回用户信息
		
		// 验证失败不返回
		// 有权限的游戏世界结果集：nRecordCount * DB_Result_ManagerWorldRight_Record

		DB_Result_ManagerUser_Login(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果 
	struct DB_Result_ManagerWorldRight_Record
	{
		DWORD dwUserID;								// 操作管理员帐号ID
		DWORD dwWorldID;							// 游戏世界ID
		DWORD dwRightValue;							// 权限数值,参考权限定义

		DB_Result_ManagerWorldRight_Record(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERUSER_CHANGEPWD 修改服务器管理员帐号信息,修改密码接口 : 传入参数结构
	struct DB_Param_ManagerUser_ChangePassword
	{
		// 此修改密码接口,用传入参数作和登录一样的验证,如通过就改为新密码
		// 条件参数
		char szUserName[DBDEF_MANAGER_NAME_MAXSIZE];	// 管理员登录帐号
		char szPassword[DBDEF_MANAGER_MSGSTR_MAXSIZE];	// 管理员登录帐号密码,已加密
		char szMACAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 登录机器的网卡MAC地址
		char szDiskSN[DBDEF_MANAGER_NAME_MAXSIZE];		// 登录机器硬盘序号 Disk Serial Number 

		// 存入的数据
		char szNewPassword[DBDEF_MANAGER_MSGSTR_MAXSIZE];	// 新的管理员登录帐号密码,已加密

		DB_Param_ManagerUser_ChangePassword(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果
	struct DB_Result_ManagerUser_ChangePassword
	{
		BYTE nResult;				// 返回修改密码成功标识,0:修改失败,1:修改成功
		BYTE nFailCode;				// 修改失败的类型代码, 0,无, 1:用户被禁用,2:用户名不存在,3:密码不正确,4:网卡MAC地址/硬盘序号不正确,6:无效新密码
		char szResultMsg[DBDEF_MANAGER_LONGSTR_MAXSIZE];	// 验证结果信息

		DB_Result_ManagerUser_ChangePassword(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerWorldRight 管理员游戏世界权限
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERWORLDRIGHT_READ 查询管理员游戏世界权限 : 传入参数结构
	struct DB_Param_Read_ManagerWorldRight
	{
		// 条件参数,排序以dwID顺序
		DWORD dwUserID;						// 操作管理员帐号ID,0为无效

		DB_Param_Read_ManagerWorldRight(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	// 返回结果
	struct DB_Result_Read_ManagerWorldRight
	{
		int nRecordCount;					// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Result_ManagerWorldRight_Record

		DB_Result_Read_ManagerWorldRight(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerLog 日志信息
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERLOG_ADD 添加服务器日志信息 : 传入参数结构
	struct DB_Param_Add_ManagerLog
	{
		// 存入的数据
		DWORD dwType;									// 日志类型,参考日志类型定义
		DWORD dwTime;									// 时间
		DWORD dwUserID;									// 操作管理员帐号ID,服务器的日志时为0
		DWORD dwWorldID;								// 游戏世界ID 
		DWORD dwComputerID;								// 硬件服务器ID 
		DWORD dwServerID;								// 软件服务器ID
		char szUserName[DBDEF_MANAGER_NAME_MAXSIZE];	// 管理员登录帐号
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];		// 管理员姓名
		char szLogTxt[DBDEF_MANAGER_TEXTSTR_MAXSIZE];	// 日志内容
		DWORD dwSpare1;									// 备用1
		DWORD dwSpare2;									// 备用2
		DWORD dwSpare3;									// 备用3

		DB_Param_Add_ManagerLog(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果 
	struct DB_Result_Add_ManagerLog
	{

		DB_Result_Add_ManagerLog(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ManagerComputerInfo 服务器和登录机器信息
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERCOMPUTERINFO_READ 查询机器信息 : 传入参数结构
	struct DB_Param_Read_ManagerComputerInfo
	{
		// 条件参数,排序以dwID顺序
		DWORD dwID;										// 机器ID，为0时以网卡MAC地址和硬盘序号查 
		char szMACAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 网卡MAC地址
		char szDiskSN[DBDEF_MANAGER_NAME_MAXSIZE];		// 硬盘序号 Disk Serial Number 

		DB_Param_Read_ManagerComputerInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	// 返回结果
	struct DB_Result_Read_ManagerComputerInfo
	{
		int nRecordCount;					// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Table_ManagerComputerInfo

		DB_Result_Read_ManagerComputerInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERCOMPUTERINFO_ADD 添加机器信息 : 传入参数结构
	struct DB_Param_Add_ManagerComputerInfo
	{
		// 以 网卡MAC地址和硬盘序号查询,如果存在就不添加,而是更新 最近活动时间,取到的电脑名称,IP地址,机器备注
		// 存入的数据
		DWORD dwID;										// 机器ID, 为0时新增，否则更新
		DWORD dwWorldID;								// 所属游戏世界ID 取 ManagerWorldList.dwID,为0表示是客户端或无效
		DWORD dwType;									// 机器类型,参考机器类型定义 
		DWORD dwTime;									// 加入时间
		DWORD dwLastTime;								// 最近活动时间
		DWORD dwFlag;									// 机器标识,0:无效,1:有效,2:新未登记的
		char szName[DBDEF_MANAGER_NAME_MAXSIZE];		// 机器名称
		char szPcName[DBDEF_MANAGER_NAME_MAXSIZE];		// 取到的电脑名称
		char szMACAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 网卡MAC地址
		char szDiskSN[DBDEF_MANAGER_NAME_MAXSIZE];		// 硬盘序号 Disk Serial Number 
		char szIPAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// IP地址
		char szTitle[DBDEF_MANAGER_TEXTSTR_MAXSIZE];	// 机器备注 
		DWORD dwSpare1;									// 备用1
		DWORD dwSpare2;									// 备用2
		DWORD dwSpare3;									// 备用3

		DB_Param_Add_ManagerComputerInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果 
	struct DB_Result_Add_ManagerComputerInfo
	{
		DWORD	dwID;							// 机器ID, 0为无效ID 
		BYTE	nResult;						// 结果 0:出错,没有新增或修改; 1:新增成功,2:修改成功

		DB_Result_Add_ManagerComputerInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERCOMPUTERINFO_DEL 删除机器信息  : 传入参数结构
	struct DB_Param_Del_ManagerComputerInfo
	{
		// 条件参数
		DWORD dwID;										// 机器ID，为0时以网卡MAC地址和硬盘序号查
		char szMACAddress[DBDEF_MANAGER_NAME_MAXSIZE];	// 网卡MAC地址 
		char szDiskSN[DBDEF_MANAGER_NAME_MAXSIZE];		// 硬盘序号 Disk Serial Number 

		DB_Param_Del_ManagerComputerInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 返回结果
	struct DB_Result_Del_ManagerComputerInfo
	{

		DB_Result_Del_ManagerComputerInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: 启动服务器脚本文件数据表
	//////////////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERSCRIPTINFO_READ  读取指定游戏世界启动脚本信息 : 传入参数结构

	struct DB_Param_Read_ManagerScriptInfo
	{
		// 条件参数
		DWORD dwWorldID;						// 游戏世界ID
		BYTE nServerType;						// 服务器类型

		DB_Param_Read_ManagerScriptInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 返回结果
	struct DB_Result_Read_ManagerScriptInfo
	{
		DWORD dwWorldID;					// 游戏世界ID
		BYTE nServerType;					// 服务器类型
		DWORD dwScriptGroupID;				// 脚本文件组ID

		DB_Result_Read_ManagerScriptInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_MANAGERSCRIPT_READ 读取启动服务器脚本文件数据 : 传入参数结构
	struct DB_Param_Read_ManagerScriptData
	{
		// 条件参数
		DWORD dwScriptGroupID;				// 脚本文件组ID 
		BYTE nSchemeIndex;					// 配置号（从一号脚本开始读取）

		DB_Param_Read_ManagerScriptData(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// 返回结果
	struct DB_Result_Read_ManagerScriptData
	{
		DWORD dwScriptGroupID;				// 脚本文件组ID
	    BYTE nScriptIndex;					// 脚本序号（0：未找到指定脚本,1：读的一号脚本，2：读的二号脚本,...）

		BYTE nLastEndData;					// 是否是最后一个数据包 ( 0：表示未读完,1：表示已读完) 
		DWORD dwDataLens;					// 配置数据长度
		DWORD dwDataLens0;					// 配置数据长度
		
		// 返回结果集：.配置数据

		DB_Result_Read_ManagerScriptData(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//GAMEDB_REQUEST_MANAGERDBSERVER_READ  读取服务器DBServer脚本配置数据 : 传入参数结构
	struct DB_Param_Read_ManagerDBServerData
	{
		// 条件参数
		BYTE  byType;				// 查询类型 0正常 2中心组
		DWORD dwWorldID;			// 游戏世界ID

		DB_Param_Read_ManagerDBServerData(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结果
	struct DB_Result_Read_ManagerDBServerData
	{
		DWORD dwWorldID;				// 游戏世界ID
		int nRecordCount;				// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Table_ManagerDBServerDataInfo

		DB_Result_Read_ManagerDBServerData(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 每个节点信息
	struct DB_Table_ManagerDBServerDataInfo
	{
		BYTE        nDataBaseServerType;						// 数据库服务器类型 0 普通区 1公共区 2中心组
		DWORD		dwWorldID;									// 游戏世界ID（中心库WorldID = 0）
		WORD		wDataBaseID;								// 数据库ＩＤ，上层程序不需要指定数据库了。
		BYTE		byDataBaseSystemType;						// 数据库类型 0 SQL Server 1 MySQL
		char		szServerName[32];							// 数据库服务器名
		char		szEmergentServerName[32];					// 紧急数据库服务器名
		char		szDataBaseName[32];							// 数据库名
		char		szDataBaseNickName[32];						// 数据库昵称，只是起提示作用的，比如说trace里，就用这个。
		char		szUserName[32];								// 用户名
		char		szPassword[256];							// 密码
		BYTE		byAsynQueueNum[DBDEF_ASYNQUEUE_MAXINDEX];	// 数据库队列数
		BYTE		byRetryLevel;								// 重试等级（０最小 -> …………）,如果小于等于此等级，则作抛包处理，大于则重试
		DWORD		dwRetryInternal;							// 重试间隔,
		WORD		wRetryMaxNumber;							// 重试最多次数，如果超过，则作抛包处理
		DWORD		dwRetryQueueMaxSize;						// 重试队列最大长度，如果超过，则不管等级，将作抛包处理
		BYTE		byThrowLevel;								// 抛包等级（０最小 -> …………）,如果小于等于此等级，则作抛包处理
		DWORD		dwThrowQueueSize;							// 触发抛包的队列长度，如果超过，无法压力队列，直接返回失败

		// 返回结果集：.配置数据
		DB_Table_ManagerDBServerDataInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif//__INETERFACE_MANAGER_DATABASE_DEFINE_H__