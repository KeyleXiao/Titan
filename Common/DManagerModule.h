/*******************************************************************
** 文件名:	e:\Rocket\Common\DManagerModule.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/20
** 版  本:	1.0
** 描  述:	服务器管理器模块定义	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __D_ManagerModule_H__
#define __D_ManagerModule_H__


// 打印调试信息,并设为测试模式
//#define MANAGER_PRINT_DEBUG_INF

#ifndef SafeRelease
#	define SafeRelease(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif


#pragma pack(1)

/******************************************************************/
/////////////////////服务器管理器的公共结构体定义 //////////////////
/******************************************************************/
///////////////////////////////////////////////////////////////////


#define  MANAGER_MAX_DWORD			0xFFFFFFFF				//DWORD 的最大数,0xFFFFFFFF	
#define  MANAGER_MAX_INT			0x7FFFFFFF				//int 的最大数,0x7FFFFFFF
#define  MANAGER_BROADCASTALL_NUM	0xFFFF					//WORD 的最大数,0xFFFF

/// 重连周期10秒
#define	MG_RECONNECT_INTERVAL			10000

#define  MANAGER_NAME_MAXSIZE		32		//名称最大长度,32
#define  MANAGER_MSGSTR_MAXSIZE		64		//信息文字最大长度,64
#define  MANAGER_TITLE_MAXSIZE		128		//标题字串的长度,128
#define  MANAGER_LONGSTR_MAXSIZE	256		//长字串的长度,256
#define  MANAGER_TEXTSTR_MAXSIZE	512		//大字串的长度,512

#define  MAX_MAINMANAGERID			MANAGER_MAX_INT					// 最大主服务器ID

#define  MANAGER_ROOT_DBID			1		// 服务器管理器的数据库ID

#define  MANAGER_RC4_KEY_SIZE		64		// RC4密钥长度
#define  MANAGER_RC4_MAX_SIZE		64		// RC4密钥最大长度

#define  MANAGER_ENABLE_TEMPID		true	// 打开临时ID处理
#define  MANAGER_GAME_TEMPID		MANAGER_MAX_INT		// 临时游戏ID
#define  MANAGER_AREA_TEMPID		MANAGER_MAX_INT		// 临时游戏区域ID

#define  MANAGER_LOGOUTCLOSE		false	// 用户注销登录时关闭客户端

#define  MANAGER_LISTWORLD_MAX		1024	// 最多能列出游戏世界个数

#define  MANAGER_KEEPALIVE_INTERVAL		2000	// 两次KeepAlive探测间的时间间隔
#define  MANAGER_KEEPALIVE_TIMES		5000	// 开始首次KeepAlive探测前的TCP空闭时间
#define  MANAGER_KEEPALIVE_TIMEOUT		3000	// 无响应时间
#define  MANAGER_KEEPALIVE_MAXTIMEOUT	60000	// 最大无响应时间

#define  MANAGER_LKEEPALIVE_INTERVAL	60000	// 公网长时类,两次KeepAlive探测间的时间间隔
#define  MANAGER_LKEEPALIVE_TIMEOUT		80000	// 公网长时类,无响应时间
#define  MANAGER_LKEEPALIVE_MAXTIMEOUT	180000	// 公网长时类,最大无响应时间

#define MANAGER_CMDOUTPUTLENS			102400				// CMD输出Buffer长度
#define MANAGER_UPLOADFILE_MAXSIZE		1048576000			// 上传的最大文件大小 20MB = (20*1024*1024=20971520)
#define MANAGER_VERIFYFILE_MAXSIZE		1048576000			// 上传的最大文件大小 20MB = (20*1024*1024=20971520)
#define MANAGER_NETBIGSEND_PACKSIZE		16320				// 分开发送的每个包的大小
#define MANAGER_NETSEND_BIGDATAFLAG		32768				// 指定数据包接最大最大值时分开发送(这个标志仅供内部使用)


#define MANAGER_SOFTCONSOLE_TIMEOUT		120		// 软件服务器控制台过期时间 120 秒
#define MANAGER_TRACELIST_MAXCOUNT		1000	// 接收Trace列表最大条数
#define MANAGER_CHECKTRACE_INTERVAL		1000	// 检查Trace时间间隔 1000 ms

#define MANAGER_REALTIMEUSER_TIMEOUT	120		// 软件服务器实时数据监听用户过期时间 120 秒
#define MANAGER_CHECKRTDATA_INTERVAL	1000	// 软件服务器要求更新实时数据时间间隔 1000 ms

#define MANAGER_SOFTNOANSWER_TIMEOUT	120		// 软件服务器无响应过期时间 120 秒

#define MANAGER_PUBLICWORLD_FLAG		10		// 公共区标识
#define MANAGER_RTBUFDATA_MAXSIZE		16320	// 状态监听数据最大数据块大小


#define  SCRIPTDATA_BUF_MAXSIZE		    (512*1024*1024)		// 缓存最大长度 512MB
#define  SCRIPTDATA_DELBUF_SIZE		    (32*1024*1024)		// 超出时释放缓存长度 32MB
//////////////////////////////////////////////////////////////////////////

/*管理器消息码层次
|－服务器消息码
|－－模块消息码
|－－－行为消息码 //*/


// 管理器所有消息的消息头
struct SManagerMsgHead
{
	// 源端点码
	BYTE		SrcEndPoint;

	// 目标端点码
	BYTE		DestEndPoint;	

	// 目标模块消息码
	BYTE		byKeyModule;

	// 行为消息码
	WORD		wKeyAction;
};


// 主服务器消息服务器标识
struct SMainManagerMsgHead
{
	// 服务器标识ID
	DWORD dwServerID;
};

// 管理器大数据包组包消息的消息头
struct SManagerBigDataMsgHead
{
	// 大数据包组包标识
	BYTE		nBigDataFlag;

	// 数据包序号,头包为0
	DWORD		dwDataIndex;

	// 数据包长度,头包和尾包为总长度
	DWORD		dwDataLens;
};

/***************************************************************/
//////////////////////// 管理器服务器层的全局定义 /////////////////////
/***************************************************************/
enum EMManagerEndpoint
{
    MSG_MG_ENDPOINT_UNKNOW = 0,         // 未定义
    MSG_MG_ENDPOINT_SCENE,              // 场景服务器    ［简称：S］
    MSG_MG_ENDPOINT_GATEWAY,            // 网关服务器    ［简称：G］
    MSG_MG_ENDPOINT_LOGIN,              // 登录服务器    ［简称：L］
    MSG_MG_ENDPOINT_WORLD,              // 世界服务器    ［简称：W］
    MSG_MG_ENDPOINT_CLIENT,             // 客户端        ［简称：C］
    MSG_MG_ENDPOINT_CENTER,             // 中心服务器    ［简称：E］
    MSG_MG_ENDPOINT_VERIFY,             // 验证服务器    ［简称：Y］
    MSG_MG_ENDPOINT_SOCIAL,             // 社会服务器    ［简称：O］
    MSG_MG_ENDPOINT_GUARD,              // 数据监护服务器［简称：G］
    MSG_MG_ENDPOINT_BRIDGE,             // 跨区桥服务器  ［简称：B］
    MSG_MG_ENDPOINT_DBHTTP,             // DB应用服务器  ［简称：D］
    MSG_MG_ENDPOINT_PASSPORT,           // 通行证服务器  ［简称：P］
    MSG_MG_ENDPOINT_VOICE,              // 语音服务器    ［简称：V］
    MSG_MG_ENDPOINT_VOICEGATE,          // 语音网关      ［简称：VG］
    MSG_MG_ENDPOINT_SNS,                // 社交服务器     [简称：N]
    MSG_MG_ENDPOINT_CONTROL,            // 中控服务器     [简称：R]
    MSG_MG_ENDPOINT_TIME,               // 时间同步服务器 [简称：T]
    MSG_MG_SERVERID_MAXID,              // 最大

    MSG_MG_ENDPOINT_ROOT = 100,			// 根服务器管理器［简称：R］
    MSG_MG_ENDPOINT_MAIN,				// 主服务器管理器［简称：M］
    MSG_MG_ENDPOINT_MANAGER,			// 子服务器管理器［简称：Z］
    MSG_MG_ENDPOINT_MCLIENT,			// 服务器管理器客户端［简称：C］

    MSG_MG_SERVERID_MGMAXID,			// 最大


    MSG_MG_ENDPOINT_BIGDATAFLAG_BEGIN = 200,	// 当数据包超过最大值时合并发送开始标识(这个标志仅供内部使用)
    MSG_MG_ENDPOINT_BIGDATAFLAG_DATA,			// 当数据包超过最大值时合并发送数据包标识(这个标志仅供内部使用)
    MSG_MG_ENDPOINT_BIGDATAFLAG_END,			// 当数据包超过最大值时合并发送结束标识(这个标志仅供内部使用)
};


/***************************************************************/
///////////////////// 管理器目标模块层的全局定义 ////////////////
/***************************************************************/
// 模块ID定义
enum EMManagerMoudleID
{
	// 未定义
	MSG_MG_MODULEID_UNKNOW = 0,
	MSG_MG_MODULEID_MANAGER,			// 服务器管理器 模块
	// 最大
	MSG_MG_MODULEID_MAXID
};


// 管理员帐号权限类型
enum EMManagerUserType
{
	// 未定义
	MANAGER_TYPE_NORMAL = 0,		// 普通管理员,没有开关机权限
	MANAGER_TYPE_MANAGER,			// 管理员,有开关机等维护权限
	MANAGER_TYPE_ADMIN,				// 超级管理员,有所有权限
	// 最大
	MANAGER_TYPE_MAXID
};


// 踢出用户方式类型
enum EMKickoutUserType
{
	// 未定义
	MANAGER_KICKOUTUSER_UNKNOW = 0,			// 未知原因
	MANAGER_KICKOUTUSER_OTHERLOGIN,			// 帐号在其它地方登录
	// 最大
	MANAGER_KICKOUTUSER_MAXID
};

// 服务器节点类型
enum EMTreeServerNodeType
{
	// 未定义
	TREE_SERVERTYPE_SOFT = 0,		// 软件服务器
	TREE_SERVERTYPE_HARD,			// 硬件服务器
	// 最大
	TREE_SERVERTYPE_MAXID
};

// 踢出用户方式类型
enum EMManagerDataUpdateType
{
	// 未定义
	MANAGER_UPDATETYPE_ADD = 0,			// 新加
	MANAGER_UPDATETYPE_EDIT,			// 修改
	MANAGER_UPDATETYPE_DEL,				// 删除
	// 最大
	MANAGER_UPDATETYPE_MAXID
};

// 软件服务器启动标识 32位
enum EMManagerStartServerFlag
{
	MANAGER_STARTFLAG_NONE			= 0x0,			// 0	:无
	MANAGER_STARTFLAG_GETSCRIPT		= 0x1,			// 1	:取得启动脚本
	MANAGER_STARTFLAG_DBSERVER		= 0x2,			// 2	:取得DBSERVER脚本
	MANAGER_STARTFLAG_DLL	        = 0x4,			// 4	:取得DLL
	//MANAGER_STARTFLAG_NoUse			= 0x8,			// 8	:未用
	//MANAGER_STARTFLAG_NoUse			= 0x10,			// 16	:未用
	//MANAGER_STARTFLAG_NoUse			= 0x20,			// 32	:未用
	//MANAGER_STARTFLAG_NoUse			= 0x40,			// 64	:未用
	//MANAGER_STARTFLAG_NoUse			= 0x80,			// 128	:未用
	//MANAGER_STARTFLAG_NoUse			= 0x100,		// 256	:未用
};
#define MANAGER_STARTFILAG_ALL_RESOURCE     MANAGER_STARTFLAG_GETSCRIPT|MANAGER_STARTFLAG_DBSERVER|MANAGER_STARTFLAG_DLL

// 强杀软件服务器标识 32位
enum EMManagerKillServerFlag
{
	MANAGER_KILLFLAG_NONE			= 0x0,			// 0	:无
	MANAGER_KILLFLAG_NOLOG			= 0x1,			// 1	:不存日志
	MANAGER_KILLFLAG_DIRECT_KILL	= 0x2,			// 2	:直接杀,不选关服
	MANAGER_KILLFLAG_KILL_ALL		= 0x4,			// 4	:杀掉所有游戏服
	//MANAGER_KILLFLAG_NoUse			= 0x8,			// 8	:未用
	//MANAGER_KILLFLAG_NoUse			= 0x10,			// 16	:未用
	//MANAGER_KILLFLAG_NoUse			= 0x20,			// 32	:未用
};

// 请求启动结果代码标识
enum EMRequestStartResultCode
{
	// 未定义
	MANAGER_STARTCODE_OK = 0,		// OK通过
	MANAGER_STARTCODE_NOTFIND,		// 没找到服务器信息
	MANAGER_STARTCODE_UNVERIFY,		// 没通过取得脚本验证
	MANAGER_STARTCODE_NOSCRIPT,		// 脚本数据不存在
	MANAGER_STARTCODE_LOADSCRIPT,	// 正在加载脚本数据,要脚本的可定时重试请求启动,直到取得
	MANAGER_STARTCODE_FAILSCRIPT,	// 发脚本数据失败
	MANAGER_STARTCODE_SAMERUN,		// 同类型和同号服务器已启动
	MANAGER_STARTCODE_WRONGPC,		// 不在此硬件服务器上启动
	// 最大
	MANAGER_STARTCODE_MAXID
};

// 服务器脚本状态
enum EMServerScriptStateFlag
{
	// 未定义
	MANAGER_SCRIPTFLAG_NO = 0,		// 没有脚本
	MANAGER_SCRIPTFLAG_NOTLOAD,		// 没加载脚本
	MANAGER_SCRIPTFLAG_LOADING,		// 正在加载
	MANAGER_SCRIPTFLAG_OK,			// 加载完成
	// 最大
	MANAGER_SCRIPTFLAG_MAXID
};

// 用户游戏世界操作权限分类 DWORD 32位
enum EMManagerUserWorldRight
{
	// 未定义
	MG_RIGHT_NONE				= 0x0,			// 0	:无
	MG_RIGHT_SHOW				= 0x1,			// 1	:查看
	MG_RIGHT_SENDAD				= 0x2,			// 2	:发公告
	MG_RIGHT_STARTSERVER		= 0x4,			// 4	:启动软件服务器
	MG_RIGHT_CLOSESERVER		= 0x8,			// 8	:关闭软件服务器
	MG_RIGHT_KILLSERVER			= 0x10,			// 16	:杀软件服务器进程
	MG_RIGHT_VIEWSERVER			= 0x20,			// 32	:监控软件服务器
	MG_RIGHT_RESTARTHARD		= 0x40,			// 64	:重起硬件服务器
	MG_RIGHT_VIEWHARD			= 0x80,			// 128	:监控硬件服务器
	MG_RIGHT_FILE				= 0x100,		// 256	:上传文件
	MG_RIGHT_SCRIPT				= 0x200,		// 512	:刷脚本
	MG_RIGHT_DATABASE			= 0x400,		// 1024	:数据库
	MG_RIGHT_CMD				= 0x800,		// 2048	:CMD控制台
	MG_RIGHT_SETUP				= 0x1000,		// 4096	:服务器安装更新
	MG_RIGHT_REPORTGM			= 0x2000,		// 8192	:玩家举报信息给GM管理
	MG_RIGHT_SERVERALERT		= 0x4000,		// 16384:服务器报警
	//MG_RIGHT_NOUSE5			= 0x8000,		// 32768:未用
	//MG_RIGHT_NOUSE6			= 0x10000,		// 65536:未用
	// 最大
	MG_RIGHT_MAXID
};


// 日志类型定义
enum EMManagerLogType
{
	// 未定义
	MG_LOGTYPE_UNKNOW = 0,		// 未知
	MG_LOGTYPE_LOGIN,			// 登录
	MG_LOGTYPE_LOGOUT,			// 注销登录
	MG_LOGTYPE_LOGKICKOUT,		// 登录踢出
	MG_LOGTYPE_CHANGEPWD,		// 修改密码
	MG_LOGTYPE_SENDAD,			// 发公告
	MG_LOGTYPE_STARTSERVER,		// 启动软件服务器
	MG_LOGTYPE_CLOSESERVER,		// 关闭软件服务器
	MG_LOGTYPE_KILLSERVER,		// 杀软件服务器进程
	MG_LOGTYPE_VIEWSERVER,		// 监控软件服务器
	MG_LOGTYPE_RESTARTHARD,		// 重起硬件服务器
	MG_LOGTYPE_VIEWHARD,		// 监控硬件服务器
	MG_LOGTYPE_FILE,			// 上传文件
	MG_LOGTYPE_SCRIPT,			// 刷脚本
	MG_LOGTYPE_DATABASE,		// 数据库
	MG_LOGTYPE_CONTROL,			// 操控
	MG_LOGTYPE_GAMEREPORT,		// 游戏中举报信息
	MG_LOGTYPE_SERVERALERT,		// 游戏服务器报警信息
	// 最大
	MG_LOGTYPE_MAXID
};


// 服务器状态类型定义
enum EMManagerServerStateFlag
{
	// 未定义
	MG_SERVERSTATE_UNKNOW = 0,		// 未知
	MG_SERVERSTATE_UNSTART,			// 未启动
	MG_SERVERSTATE_STARTING,		// 正在启动
	MG_SERVERSTATE_STARTFAIL,		// 启动失败
	MG_SERVERSTATE_RUNING,			// 已启动，正在远行
	MG_SERVERSTATE_NOANSWER,		// 已启动，无响应
	MG_SERVERSTATE_STOPING,			// 正在关闭
	MG_SERVERSTATE_STOPED,			// 已关闭
	// 最大
	MG_SERVERSTATE_MAXID
};

// 服务器跨线程执行操作代码定义
enum EMManagerControlCode
{
	// 未定义
	MG_ControlCode_None = 0,		// 无操作
	MG_ControlCode_StartService,	// 启动服务
	MG_ControlCode_StopService,		// 停止服务
	MG_ControlCode_ShutDown,		// 关闭程序,停止服务后要关闭进程
	// 最大
	MG_ControlCode_Max
};

// 游戏世界操作代码定义
enum EMManagerWorldControlCode
{
	// 未定义
	MG_WorldControl_None = 0,		// 无操作
	MG_WorldControl_LoadScript,		// 从数据库加载启动脚本
	MG_WorldControl_LoadInfo,		// 从数据库加载信息
	MG_WorldControl_SendInfo,		// 刷新信息和启动脚本
	// 最大
	MG_WorldControl_Max
};


// 操作主服务器代码定义
enum EMManagerMainControlCode
{
	// 未定义
	MG_MainControl_None = 0,			// 无操作
	MG_MainControl_UpdateScriptState,	// 更新启动脚本状态
	MG_MainControl_RequestSvnUpload,	// 请求上传
	MG_MainControl_RequestSvnFinish,	// 请求上传
	MG_MainControl_RequestSvnFile,		// 请求Svn上传
	MG_MainControl_RequestSvnManage,	// 请求管理Svn
	MG_MainControl_RequestSvnRight,		// 请求临时授权上传
	// 最大
	MG_MainControl_Max
};

// 主服务器输出数据类型定义
enum EMManagerMainDataType
{
	// 未定义
	MG_MainData_None = 0,				// 无操作
	MG_MainData_UpdateScriptState,		// 更新启动脚本状态结果
	MG_MainData_ShowSvnCmdDlg,			// 打开SvnCmd控制台
	MG_MainData_RunSvnCmdResult,		// 运行SvnCmd命令结果
	MG_MainData_UpdateSvnBaseInfo,		// 更新信息到客户端
	MG_MainData_UpdateSvnUserInfo,		// 更新用户信息到客户端
	// 最大
	MG_MainData_Max
};


// 子服务器操作代码定义
enum EMManagerHardControlCode
{
	// 未定义
	MG_HardControl_None = 0,		// 无操作
	MG_HardControl_UseCmd,			// 使用Cmd控制台
	MG_HardControl_RunCmd,			// 运行Cmd命令
	MG_HardControl_UserLogin,		// 用户登录
	MG_HardControl_GetTime,			// 取得服务器时间
	MG_HardControl_ListenRTData,	// 监听实时数据
	// 最大
	MG_HardControl_Max
};

// 子服务器输出数据类型定义
enum EMManagerHardDataType
{
	// 未定义
	MG_HardData_None = 0,		// 无操作
	MG_HardData_OpenCmdResult,	// 打开Cmd控制台结果
	MG_HardData_CloseCmdResult,	// 关闭Cmd控制台结果
	MG_HardData_RunCmdResult,	// 运行Cmd命令结果
	MG_HardData_GetTimeResult,	// 取得服务器时间结果
	MG_HardData_ListenRTDataResult,	// 请求监听实时数据结果
	MG_HardData_GameServerAlert,	// 报警信息
	// 最大
	MG_HardData_Max
};


// 软件服务器操作代码定义
enum EMManagerSoftControlCode
{
	// 未定义
	MG_SoftControl_None = 0,		// 无操作
	MG_SoftControl_UseCmd,			// 使用Cmd控制台
	MG_SoftControl_RunCmd,			// 运行Cmd命令
	MG_SoftControl_UserLogin,		// 用户登录
	MG_SoftControl_TraceLevel,		// 修改Trace级别
	MG_SoftControl_CmdTraceLevel,	// 修改控制台Trace级别
	MG_SoftControl_SetAlertLevel,	// 修改报警级别
	MG_SoftControl_GetAlertLevel,	// 取得报警级别
	MG_SoftControl_SetServerUID,	// 设定软件服务器的UID
	MG_SoftControl_RunGMCommand,	// 运行GM命令
	MG_SoftControl_GMReplyReport,	// GM回复举报信息给玩家
	MG_SoftControl_UpdateScpFile,	// 请求服务器刷新脚本
	MG_SoftControl_ListenRTData,	// 监听实时数据
	MG_SoftControl_ListenRTVisitor,	// 软件服务器状态监听
	// 最大
	MG_SoftControl_Max
};

// 软件服务器输出数据类型定义
enum EMManagerSoftDataType
{
	// 未定义
	MG_SoftData_None = 0,			// 无操作
	MG_SoftData_OpenCmdResult,		// 打开Cmd控制台结果
	MG_SoftData_CloseCmdResult,		// 关闭Cmd控制台结果
	MG_SoftData_RunCmdResult,		// 运行Cmd命令结果
	MG_SoftData_UpdateProperty,		// 更新服务器属性
	MG_SoftData_GetAlertLevelResult,// 取得服务器报警级别结果
	MG_SoftData_GameServerAlert,	// 游戏世界软件服务器报警信息
	MG_SoftData_GMCommandResult,	// 运行GM命令结果
	MG_SoftData_ReportToGM,			// 玩家举报信息给GM
	MG_SoftData_SameReport,			// 被服务器挡住的玩家举报信息,用来计数
	MG_SoftData_UpdateScpFileResult,// 请求服务器刷新脚本结果
	MG_SoftData_ListenRTDataResult,	// 请求监听实时数据结果
	// 最大
	MG_SoftData_Max
};


// 所有脚本类型ID定义
enum MGCSchemeTypeID
{
	MGCSchemeType_None = 0,				// 无效
	MGCSchemeType_StartServer,			// 开服脚本
	MGCSchemeType_CloseServer,			// 关服假本
	MGCSchemeType_AutoTask,				// 自动任务脚本

	MGCSchemeType_MaxID,				// 最大数
};


// 文件管理消息类型定义
enum EMManagerFileMsgType
{
	// 未定义
	MG_FileMsgType_None = 0,			// 无操作
	MG_FileMsgType_UploadRequest,		// 请求上传文件
	MG_FileMsgType_UploadResult,		// 请求上传文件结果
	MG_FileMsgType_FileData,			// 文件数据
	MG_FileMsgType_FileDataResult,		// 文件数据结果
	MG_FileMsgType_DownloadRequest,		// 请求下载文件
	MG_FileMsgType_DownloadResult,		// 请求下载文件结果
	MG_FileMsgType_DirListRequest,		// 请求列出目录信息
	MG_FileMsgType_DirListResult,		// 目录信息结果
	MG_FileMsgType_BroadcastFile,		// 请求主服务器广播文件到子服务器
	MG_FileMsgType_BroadcastFileResult,	// 请求主服务器广播文件到子服务器结果
	MG_FileMsgType_BroadcastUploadResult,// 请求广播上传文件结果
	MG_FileMsgType_BroadcastFileDataResult,	// 广播文件数据结果
	MG_FileMsgType_UnPackFile,			// 请求子服务器解更新包
	MG_FileMsgType_UnPackFileResult,	// 请求子服务器解更新包结果
	MG_FileMsgType_VerifyPackFile,		// 请求子服务器校验更新包
	MG_FileMsgType_VerifyPackFileResult,// 请求子服务器校验更新包结果
	MG_FileMsgType_UpdateServer,		// 请求服务器更新
	MG_FileMsgType_UpdateServerResult,	// 请求服务器更新结果
	MG_FileMsgType_RemoveFile,			// 请求子服务器删除文件
	MG_FileMsgType_RemoveFileResult,	// 请求子服务器删除文件结果
	MG_FileMsgType_RemoveLog,			// 请求子服务器清除Log文件
	MG_FileMsgType_RemoveLogResult,		// 请求子服务器清除Log文件结果
	MG_FileMsgType_RunAutoBat,			// 请求子服务器运行自动批处理
	MG_FileMsgType_RunAutoBatResult,	// 请求子服务器运行自动批处理结果
	// 最大
	MG_FileMsgType_Max
};

// 上传文件状态定义
enum EMManagerFileUploadState
{
	// 未定义
	MG_FileUploadState_None = 0,			// 未知
	MG_FileUploadState_UnLoadFile,			// 未加载文件
	MG_FileUploadState_LoadFileOK,			// 已加载文件
	MG_FileUploadState_LoadFileFail,		// 加载文件失败
	MG_FileUploadState_Request,				// 请求上传文件
	MG_FileUploadState_RequestFail,			// 请求上传文件失败
	MG_FileUploadState_FileData,			// 传送文件数据
	MG_FileUploadState_FileDataOK,			// 传送文件数据成功
	MG_FileUploadState_FileDataFail,		// 传送文件数据失败
	MG_FileUploadState_UnPackOK,			// 解包成功
	MG_FileUploadState_VerifyPackOK,		// 校验成功
	MG_FileUploadState_VerifyPackFail,		// 校验失败
	// 最大
	MG_FileUploadState_Max
};
// 下载文件状态定义
enum EMManagerFileDownloadState
{
	// 未定义
	MG_FileDownloadState_None = 0,			// 未知
	MG_FileDownloadState_WaitFileData,		// 等待文件数据
	MG_FileDownloadState_FileDataOK,		// 收到数据成功
	MG_FileDownloadState_FileDataFail,		// 收到数据失败
	MG_FileDownloadState_SaveFileOK,		// 保存文件成功
	MG_FileDownloadState_SaveFileFail,		// 保存文件失败
	// 最大
	MG_FileDownloadState_Max
};

//  子服务器安装更新进行状态
enum EMWorldSetupUpdateStatus
{
	MG_WorldSetupUpdateStatus_None = 0,		// 未开始
	MG_WorldSetupUpdateStatus_GetVer,		// 正在取得版本信息
	MG_WorldSetupUpdateStatus_BackUp,		// 正在备份
	MG_WorldSetupUpdateStatus_ReName,		// 正在重命名
	MG_WorldSetupUpdateStatus_Setup,		// 正在安装更新
	MG_WorldSetupUpdateStatus_Verify,		// 正在校验
	MG_WorldSetupUpdateStatus_OK,			// 完成
	// 最大
	MG_WorldSetupUpdateStatus_MaxID
};

// 刷新脚本状态定义
enum EMManagerFileUpdateScpState
{
	// 未定义
	MG_FileUpdateScpState_None = 0,			// 未知
	MG_FileUpdateScpState_Request,			// 请求刷新
	MG_FileUpdateScpState_UpdateOK,			// 刷新成功
	MG_FileUpdateScpState_UpdateFail,		// 刷新失败
	// 最大
	MG_FileUpdateScpState_Max
};


//////////////////////////////////////////////////////////////////////////
// 各服务器的实时数据ID定义

#define MANAGER_REALTIMEDATA_MAXID		32		// 实时数据最大ID,以下各服最大ID不能超过此ID
#define MANAGER_REALTIMEDATA_CPU_MAX	10000	// CPU占用百分比数

// 中心服器 实时数据ID定义
enum EMManagerRealTimeDataCenter
{
	MG_CenterRTData_CPUCurrentUsedValue = 0,		// 当前进程的CPU占用百分比
	MG_CenterRTData_PhysicalMemoryUsedSize,			// 当前进程的物理内存已使用大小
	MG_CenterRTData_VirtualMemoryUsedSize,			// 当前进程的虚拟内存已使用大小
	MG_CenterRTData_SendSpeed,						// 每秒发包速度
	MG_CenterRTData_RecvSpeed,						// 每秒收包速度
	// 最大
	MG_CenterRTData_Max
};

// 登录服务器 实时数据ID定义
enum EMManagerRealTimeDataLogin
{
	MG_LoginRTData_CPUCurrentUsedValue = 0,		// 当前进程的CPU占用百分比
	MG_LoginRTData_PhysicalMemoryUsedSize,		// 当前进程的物理内存已使用大小
	MG_LoginRTData_VirtualMemoryUsedSize,		// 当前进程的虚拟内存已使用大小
	MG_LoginRTData_OnLineCounts,				// 在线数
	//MG_LoginRTData_MaxZoneLatency,			// 最大场景服网络延时
	//MG_LoginRTData_MaxPubZoneLatency,			// 最大公共区场景服网络延时
	//MG_LoginRTData_SocialLatency,				// 社会服网络延时
	//MG_LoginRTData_PubSocialLatency,			// 公共区社会服网络延时
	//MG_LoginRTData_MaxLatencyZoneID,			// 最大网络延时场景服ID
	// 最大
	MG_LoginRTData_Max
};

// 场景服务器 实时数据ID定义
enum EMManagerRealTimeDataZone
{
	MG_ZoneRTData_CPUCurrentUsedValue = 0,		// 当前进程的CPU占用百分比
	MG_ZoneRTData_PhysicalMemoryUsedSize,		// 当前进程的物理内存已使用大小
	MG_ZoneRTData_VirtualMemoryUsedSize,		// 当前进程的虚拟内存已使用大小
	MG_ZoneRTData_OnLineCounts,					// 在线数
	// 最大
	MG_ZoneRTData_Max
};

// 社会服务器 实时数据ID定义
enum EMManagerRealTimeDataSocial
{
	MG_SocialRTData_CPUCurrentUsedValue = 0,	// 当前进程的CPU占用百分比
	MG_SocialRTData_PhysicalMemoryUsedSize,		// 当前进程的物理内存已使用大小
	MG_SocialRTData_VirtualMemoryUsedSize,		// 当前进程的虚拟内存已使用大小
	MG_SocialRTData_OnLineCounts,				// 在线数
	MG_SocialRTData_OtherOnLineCounts,			// 跨服在线数
	// 最大
	MG_SocialRTData_Max
};

// 网关服务器 实时数据ID定义
enum EMManagerRealTimeDataGateway
{
	MG_GatewayRTData_CPUCurrentUsedValue = 0,	// 当前进程的CPU占用百分比
	MG_GatewayRTData_PhysicalMemoryUsedSize,	// 当前进程的物理内存已使用大小
	MG_GatewayRTData_VirtualMemoryUsedSize,		// 当前进程的虚拟内存已使用大小
	MG_GatewayRTData_OnLineCounts,				// 在线数
	MG_GatewayRTData_AverageLatency,			// 平均网络延时
	MG_GatewayRTData_Pinglatency,				// 网关到大型网站的PING值
	MG_GatewayRTData_DelayPercent,				// 延迟率高的玩家占的百分比
	MG_GatewayRTData_ClientSendSpeed,			// 客户端每秒发包速度
	MG_GatewayRTData_ClientRecvSpeed,			// 客户端每秒收包速度
	MG_GatewayRTData_ServerSendSpeed,			// 服务器每秒发包速度
	MG_GatewayRTData_ServerRecvSpeed,			// 服务器每秒收包速度
	// 最大
	MG_GatewayRTData_Max
};

// 子服务器管理器 实时数据ID定义
enum EMManagerRealTimeDataComputer
{
	MG_ComputerRTData_PhysicalMemorySize = 0,		// 本台机器的物理内存大小
	MG_ComputerRTData_PhysicalMemoryUsedSize,		// 本台机器的物理内存已使用大小
	MG_ComputerRTData_VirtualMemorySize,			// 本台机器的虚拟内存大小
	MG_ComputerRTData_VirtualMemoryUsedSize,		// 本台机器的虚拟内存已使用大小
	MG_ComputerRTData_CPUCurrentUsedValue,			// 本台机器的CPU占用百分比
	MG_ComputerRTData_ProcessCPUCurrentUsedValue,	// 当前进程的CPU占用百分比
	// 最大
	MG_ComputerRTData_Max
};


// 语音服务器 实时数据ID定义
enum EMManagerRealTimeDataVoice
{
	MG_VoiceRTData_CPUCurrentUsedValue = 0,	// 当前进程的CPU占用百分比
	MG_VoiceRTData_PhysicalMemoryUsedSize,	// 当前进程的物理内存已使用大小
	MG_VoiceRTData_VirtualMemoryUsedSize,		// 当前进程的虚拟内存已使用大小
	MG_VoiceRTData_OnLineCounts,				// 在线数
	MG_VoiceRTData_ClientSendSpeed,			// 客户端每秒发包速度
	MG_VoiceRTData_ClientRecvSpeed,			// 客户端每秒收包速度
	MG_VoiceRTData_ServerSendSpeed,			// 服务器每秒发包速度
	MG_VoiceRTData_ServerRecvSpeed,			// 服务器每秒收包速度
	// 最大
	MG_VoiceRTData_Max
};

// 语音网关 实时数据ID定义
enum EMManagerRealTimeDataVoiceGateway
{
	MG_VoiceGatewayRTData_CPUCurrentUsedValue = 0,	// 当前进程的CPU占用百分比
	MG_VoiceGatewayRTData_PhysicalMemoryUsedSize,	// 当前进程的物理内存已使用大小
	MG_VoiceGatewayRTData_VirtualMemoryUsedSize,		// 当前进程的虚拟内存已使用大小
	MG_VoiceGatewayRTData_OnLineCounts,				// 在线数
	MG_VoiceGatewayRTData_ClientSendSpeed,			// 客户端每秒发包速度
	MG_VoiceGatewayRTData_ClientRecvSpeed,			// 客户端每秒收包速度
	MG_VoiceGatewayRTData_ServerSendSpeed,			// 服务器每秒发包速度
	MG_VoiceGatewayRTData_ServerRecvSpeed,			// 服务器每秒收包速度
	// 最大
	MG_VoiceGatewayRTData_Max
};

//////////////////////////////////////////////////////////////////////////

//  版本整理管理服务器状态
enum EMManagerSvnStatus
{
	MG_SvnStatus_None = 0,		// 未开始
	MG_SvnStatus_Init,			// 正在初始化
	MG_SvnStatus_Wait,			// 正在等待
	MG_SvnStatus_Svn1,			// 星期一,下午16点 开发版整理
	MG_SvnStatus_Svn2,			// 星期三,下午16点 开发版整理
	MG_SvnStatus_Make1,			// 星期四 12:00编译,结界
	MG_SvnStatus_Release1,		// 星期五 发布到压测
	MG_SvnStatus_Make2,			// 下周星期一 12:00编译,结界
	MG_SvnStatus_Release2,		// 下周星期二 发布到内测
	MG_SvnStatus_End,			// 结束整版本
	// 最大
	MG_SvnStatus_MaxID
};

//  版本整理进行状态
enum EMManagerSvnRunStatus
{
	MG_SvnRunStatus_None = 0,		// 未开始
	MG_SvnRunStatus_Wait,			// 等待
	MG_SvnRunStatus_Start,			// 开始
	MG_SvnRunStatus_Confirm,		// 确认要提交
	MG_SvnRunStatus_WaitNext,		// 等人抢号
	MG_SvnRunStatus_Upload,			// 等人上传
	MG_SvnRunStatus_UploadOK,		// 上传成功
	MG_SvnRunStatus_Svn,			// Svn上传
	MG_SvnRunStatus_SvnOK,			// Svn上传成功
	MG_SvnRunStatus_EndSvn,			// 结束Svn上传
	MG_SvnRunStatus_Build,			// 编译处理
	MG_SvnRunStatus_BuildFail,		// 编译失败
	MG_SvnRunStatus_BuildOK,		// 编译完成
	MG_SvnRunStatus_CheckBug,		// 处理Bug提交
	MG_SvnRunStatus_End,			// 结束
	// 最大
	MG_SvnRunStatus_MaxID
};


//  软件服务器状态监听类型
enum EMManagerRealTimeVisitorType
{
	MG_RTVisitorType_IPCity = 0,	// 在线IP所有城市坐标信息
	// 最大
	MG_RTVisitorType_MaxID
};

//////////////////////////////////////////////////////////////////////////
// 公共数据结构
//////////////////////////////////////////////////////////////////////////
struct SServerTypeSubIDInfo
{
	WORD wServerSubID;				// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	WORD wServerType;				// 软件服务器类型,参考服务器类型定义

	SServerTypeSubIDInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SServerWorldTypeSubIDInfo
{
	WORD wServerSubID;				// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	WORD wServerType;				// 软件服务器类型,参考服务器类型定义
	DWORD dwWorldID;				// 游戏世界ID

	SServerWorldTypeSubIDInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SWorldServerUIDInfo
{
	DWORD dwUID;				// 服务器UID
	DWORD dwWorld;				// 游戏世界ID

	SWorldServerUIDInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SGameWorldConnectorInfo
{
	DWORD dwGameID;			// 游戏ID
	DWORD dwAreaID;			// 游戏区域ID
	DWORD dwWorldID;		// 游戏世界ID
	bool bIsTemp;			// 是否临时游戏世界
	bool bIsPublic;			// 是公共区
	char szGameName[MANAGER_NAME_MAXSIZE];		// 游戏名称
	char szAreaName[MANAGER_NAME_MAXSIZE];		// 游戏区域名称
	char szWorldName[MANAGER_NAME_MAXSIZE];		// 游戏世界名称

	SGameWorldConnectorInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
/// RootManagerServer  根服务器管理器 相关定义
//////////////////////////////////////////////////////////////////////////

// 游戏信息数据结构
struct SGameInfo
{
	DWORD dwID;									// 游戏ID, 0为无效ID,数据库设为唯一 
	DWORD dwIndex;								// 游戏序号,用来排序用
	DWORD dwFlag;								// 游戏标识,0:无效,1:有效游戏,2:有效服务器群 
	char szName[MANAGER_NAME_MAXSIZE];			// 游戏名称
	char szTitle[MANAGER_TITLE_MAXSIZE];		// 游戏说明
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3

	SGameInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 游戏区域信息数据结构
struct SGameAreaInfo
{
	DWORD dwID;									// 游戏区域ID, 0为无效ID,数据库设为自增唯一
	DWORD dwGameID;								// 所属游戏ID 取 ManagerGameList.dwID
	DWORD dwIndex;								// 游戏区域序号,用来排序用
	DWORD dwFlag;								// 游戏区域标识,0:无效,1:有效
	char szName[MANAGER_NAME_MAXSIZE];			// 游戏区域名称
	char szTitle[MANAGER_TITLE_MAXSIZE];		// 游戏区域说明
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3

	SGameAreaInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 游戏世界信息数据结构
struct SGameWorldInfo
{
	DWORD dwID;									// 游戏世界ID, 0为无效ID,数据库设为自增唯一
	DWORD dwAreaID;								// 所属游戏区域ID 取 ManagerAreaList.dwID
	DWORD dwIndex;								// 游戏世界序号,用来排序用
	DWORD dwFlag;								// 游戏世界标识,0:无效,1:有效,2:推荐,3:新 
	char szName[MANAGER_NAME_MAXSIZE];			// 游戏世界名称
	char szTitle[MANAGER_TITLE_MAXSIZE];		// 游戏世界说明
	char szAdUrl1[MANAGER_TITLE_MAXSIZE];		// 游戏世界广告Url连接1
	char szAdUrl2[MANAGER_TITLE_MAXSIZE];		// 游戏世界广告Url连接2
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3

	SGameWorldInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 硬件服务器信息数据结构
struct SGameComputerInfo
{
	DWORD dwID;									// 硬件服务器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
	DWORD dwIndex;								// 硬件服务器序号,用来排序用
	DWORD dwFlag;								// 硬件服务器标识,0:无效,1:有效
	DWORD dwComputerInfoID;						// 对应的机器ID  取 ManagerComputerInfo.dwID 
	char szName[MANAGER_NAME_MAXSIZE];			// 硬件服务器名称
	char szTitle[MANAGER_TITLE_MAXSIZE];		// 硬件服务器说明
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 硬件服务器 MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 硬件服务器硬盘序号 Disk Serial Number
	char szRegSN[MANAGER_TITLE_MAXSIZE];		// 硬件服务器授权注册码
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3

	SGameComputerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 软件服务器信息数据结构
struct SGameServerInfo
{
	DWORD dwID;									// 软件服务器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
	DWORD dwComputerID;							// 所属硬件服务器ID 取 ManagerComputerList.dwID
	DWORD dwIndex;								// 软件服务器序号,用来排序用
	DWORD dwType;								// 软件服务器类型,参考服务器类型定义
	DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	DWORD dwFlag;								// 软件服务器标识,0:无效,1:有效
	char szName[MANAGER_NAME_MAXSIZE];			// 软件服务器名称
	char szTitle[MANAGER_TITLE_MAXSIZE];		// 软件服务器说明
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3

	SGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 服务器服管理员帐号信息数据结构
struct SManagerUserInfo
{
	DWORD dwID;										// 管理员帐号ID, 0为无效ID,数据库设为自增唯一
	DWORD dwType;									// 管理员帐号权限类型,参考权限类型定义
	DWORD dwFlag;									// 管理员帐号标识,0:无效,1:有效
	DWORD dwCheckPCFlag;							// 是否起用登录机器限制,0:不起用,1:起用
	DWORD dwLoginCount;								// 登录次数
	DWORD dwLastTime;								// 最近登录时间
	char szUserName[MANAGER_NAME_MAXSIZE];			// 管理员登录帐号
	char szPassword[MANAGER_MSGSTR_MAXSIZE];		// 管理员登录帐号密码
	char szName[MANAGER_NAME_MAXSIZE];				// 管理员姓名
	char szDept[MANAGER_NAME_MAXSIZE];				// 管理员部门
	char szTitle[MANAGER_TITLE_MAXSIZE];			// 管理员说明
	char szMACAddress[MANAGER_NAME_MAXSIZE];		// 最近登录的网卡MAC地址
	char szIPAddress[MANAGER_NAME_MAXSIZE];			// 最近登录的IP
	DWORD dwSpare1;									// 备用1,用作游戏中GM命令权限
	DWORD dwSpare2;									// 备用2
	DWORD dwSpare3;									// 备用3

	SManagerUserInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 管理员游戏世界权限信息数据结构
struct SManagerWorldRight
{
	DWORD dwUserID;								// 操作管理员帐号ID
	DWORD dwWorldID;							// 游戏世界ID
	DWORD dwRightValue;							// 权限数值,参考权限定义

	SManagerWorldRight(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 服务器和登录机器信息数据结构
struct SManagerPCInfo
{
	DWORD dwID;										// 机器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;								// 所属游戏世界ID 取 ManagerWorldList.dwID,为0表示是客户端或无效
	DWORD dwType;									// 机器类型,参考机器类型定义 
	DWORD dwTime;									// 加入时间
	DWORD dwLastTime;								// 最近活动时间
	DWORD dwFlag;									// 机器标识,0:无效,1:有效,2:新未登记的
	char szName[MANAGER_NAME_MAXSIZE];				// 机器名称
	char szPcName[MANAGER_NAME_MAXSIZE];			// 取到的电脑名称
	char szMACAddress[MANAGER_NAME_MAXSIZE];		// 网卡MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];			// 硬盘序号 Disk Serial Number 
	char szIPAddress[MANAGER_NAME_MAXSIZE];			// IP地址
	char szTitle[MANAGER_TEXTSTR_MAXSIZE];			// 机器备注 
	DWORD dwSpare1;									// 备用1
	DWORD dwSpare2;									// 备用2
	DWORD dwSpare3;									// 备用3

	SManagerPCInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 用户登录数据
struct SManagerUserLoginData
{
	char szUserName[MANAGER_NAME_MAXSIZE];		// 管理员登录帐号
	char szPassword[MANAGER_MSGSTR_MAXSIZE];	// 管理员登录帐号密码,已加密
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 登录机器的网卡MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 登录机器硬盘序号 Disk Serial Number 
	char szIPAddress[MANAGER_NAME_MAXSIZE];		// 登录机器的IP

	SManagerUserLoginData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 发送服务器启动脚本文件数据给指定游戏世界结构
struct SSendWorldScriptInfo
{
	DWORD dwWorldID;					// 游戏世界ID
	BYTE nServerType;					// 服务器类型
	DWORD dwScriptGroupID;				// 脚本文件组ID
    BYTE  nEncry;                       // 加密类型
    BYTE  nResourceType;                // 资源类型
	BYTE nMaxSchemeIndex;				// 最大配置号
	BYTE nSchemeIndex;					// 配置号（从一号脚本开始读取）
	DWORD dwDataLens;					// 配置数据长度
	BYTE * pData;						// 配置数据

	SSendWorldScriptInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
/// MainManagerServer  主服务器管理器 相关定义
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
/// ManagerServer  子服务器管理器 相关定义
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
/// ManagerClient  服务器管理器客户端 相关定义
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
/// ManagerConnector  软件服务器 管理器连接器 相关定义
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
/// MainManagerConnector  主服务器连接器 相关定义
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
// 事件码枚举定义: ENUM_EVENT_MG + 消息码名
enum EMEventManager
{
	ENUM_EVENT_MG_INVALID = 0,
	ENUM_EVENT_MG_REAL_TIME_DATA,				// 要求更新实时数据事件
	ENUM_EVENT_MG_REAL_TIME_VISITOR,			// 要求更新状态监听事件

	ENUM_EVENT_MG_MAXID,						// 最大事件码
};

// 最大事件码
#define EVENT_MG_MAXID						ENUM_EVENT_MG_MAXID

//////////////////////////////////////////////////////////////////////////
// 要求更新实时数据事件
#define EVENT_MG_REAL_TIME_DATA		ENUM_EVENT_MG_REAL_TIME_DATA

// 要求更新实时数据事件数据结构
struct SEventManagerRealTimeData
{
	DWORD dwWorldID;			// 游戏世界ID
	WORD wServerType;			// 服务器类型
	WORD wSubID;				// 服务器序号

	SEventManagerRealTimeData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 要求更新状态监听事件
#define EVENT_MG_REAL_TIME_VISITOR		ENUM_EVENT_MG_REAL_TIME_VISITOR

// 要求更新状态监听事件数据结构
struct SEventManagerRealTimeVisitor
{
	DWORD dwRTVisitorType;		// 状态监听类型
	DWORD dwWorldID;			// 游戏世界ID
	WORD wServerType;			// 服务器类型
	WORD wSubID;				// 服务器序号

	SEventManagerRealTimeVisitor(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



//////////////////////////////////////////////////////////////////////////
// 消息码与数据结构

/***************************************************************/
///////////////////// 服务器管理器 模块 的消息码/////////////////
// 上层的消息码必定是 MSG_MG_MODULEID_MANAGER
/***************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EMMsgManager
{
	ENUM_MSG_MG_INVALID = 0,
	ENUM_MSG_MG_HANDSHAKE,						// 握手消息
	ENUM_MSG_MG_HANDSHAKE_RESPONSE,				// 回应握手消息
	ENUM_MSG_MG_DISCONNECTED,					// 断线消息
	ENUM_MSG_MG_SENDROOTDATA,					// 请求主服务器网关服务发送数据给根服务器
	ENUM_MSG_MG_SENDCLIENTDATA,					// 请求主服务器网关服务发送数据给客户端
	ENUM_MSG_MG_SENDHARDDATA,					// 请求主服务器网关服务发送数据给各硬件服务器
	ENUM_MSG_MG_SENDSOFTDATA,					// 请求主服务器网关服务发送数据给各软件服务器
	ENUM_MSG_MG_SENDSUBDATA,					// 请求子服务器网关服务发送数据给各软件服务器
	ENUM_MSG_MG_SENDMAINDATA,					// 请求子服务器网关服务发送数据给主服务器
	ENUM_MSG_MG_USERLOGIN,						// 用户登录消息
	ENUM_MSG_MG_USERLOGIN_RESULT,				// 用户登录结果消息
	ENUM_MSG_MG_USERLOGOUT,						// 用户注销登录消息
	ENUM_MSG_MG_USERLOGOUT_RESULT,				// 用户注销登录结果消息
	ENUM_MSG_MG_CHANGEPWD,						// 修改密码消息
	ENUM_MSG_MG_CHANGEPWD_RESULT,				// 修改密码结果消息
	ENUM_MSG_MG_SENDCLIENT_GAMETREE,			// 发送游戏结构树信息给客户端消息
	ENUM_MSG_MG_SENDMAIN_WORLDINFO,				// 发送游戏世界服务器相关信息给主服务器消息
	ENUM_MSG_MG_SENDMAIN_LOGININFO,				// 发送用户登录信息给主服务器
	ENUM_MSG_MG_FEEDBACK_LOGININFO,				// 回馈用户登录给根服务器
	ENUM_MSG_MG_SENDCLIENT_MAINLOGIN,			// 通知客户端登录主服务器
	ENUM_MSG_MG_USER_MAINLOGIN,					// 用户登录主服务器
	ENUM_MSG_MG_USER_MAINLOGIN_RESULT,			// 用户登录主服务器结果消息
	ENUM_MSG_MG_UPDATE_WORLDSTATE,				// 根服务器动态更新各游戏世界状态给客户端
	ENUM_MSG_MG_KICKOUTUSER,					// 踢出用户消息
	ENUM_MSG_MG_UPDATE_SOFTINFO,				// 更新软件服务器到主服务器信息消息
	ENUM_MSG_MG_UPDATEWORLD_CLIENT,				// 发送游戏世界数据给客户端消息
	ENUM_MSG_MG_UPDATE_SERVERINFO,				// 主服务器动态更新各软件服务器状态给客户端
	ENUM_MSG_MG_UPDATE_COMPUTERINFO,			// 主服务器动态更新各硬件服务器状态给客户端
	ENUM_MSG_MG_SENDSCRIPTDATA,					// 发送服务器启动脚本文件数据给指定游戏世界
	ENUM_MSG_MG_GETSCRIPTDATA,					// 向根服务器取服务器启动脚本文件数据
	ENUM_MSG_MG_REQUESTSTART,					// 各软件服务器向主服务器请求启动
	ENUM_MSG_MG_REQUESTSTART_RESULT,			// 各软件服务器向主服务器请求启动结果消息
	ENUM_MSG_MG_STARTSERVER,					// 要求子服务器启动软件服务器
	ENUM_MSG_MG_CLOSESERVER,					// 要求软件服务器关闭
	ENUM_MSG_MG_KILLSERVER,						// 要求子服务器强杀软件服务器进程
	ENUM_MSG_MG_SERVERSENDAD,					// 要求软件服务器发布公告
	ENUM_MSG_MG_RESTARTCOMPUTER,				// 要求子服务器重起硬件服务器
	ENUM_MSG_MG_REQUESTCLOSE,					// 各软件服务器向主服务器请求关闭
	ENUM_MSG_MG_REQUESTRESTART,					// 硬件服务器向主服务器请求重起
	ENUM_MSG_MG_ADDLOG,							// 向根服务器请求写入日志
	ENUM_MSG_MG_SHOWWINDOW,						// 通知服务器显示程序窗口
	ENUM_MSG_MG_SETSERVERSTATE,					// 设定服务器状态
	ENUM_MSG_MG_CONTROLWORLD,					// 操作游戏世界
	ENUM_MSG_MG_CONTROLHARDSERVER,				// 操作硬件服务器
	ENUM_MSG_MG_HARDSERVERDATA,					// 硬件服务器输出数据
	ENUM_MSG_MG_CONTROLSOFTSERVER,				// 操作软件服务器
	ENUM_MSG_MG_SOFTSERVERDATA,					// 软件服务器输出数据
	ENUM_MSG_MG_TRACEDATA,						// Trace输出数据
	ENUM_MSG_MG_KEEPALIVE,						// 心跳检查
	ENUM_MSG_MG_ANSWER_KEEPALIVE,				// 回应心跳检查
	ENUM_MSG_MG_MAIN_ADDNEWPC,					// 新加硬件服务器
	ENUM_MSG_MG_CONTROLMAINSERVER,				// 操作主服务器
	ENUM_MSG_MG_MAINSERVERDATA,					// 主服务器输出数据
	ENUM_MSG_MG_FILEMSG,						// 文件管理消息
	ENUM_MSG_MG_UPDATEVERSION,					// 子服务器更新版本消息
	ENUM_MSG_MG_REALTIMEDATA,					// 实时数据输出
	ENUM_MSG_MG_PCREALTIMEDATA,					// 硬件服务器实时数据输出
	ENUM_MSG_MG_REALTIMEVISITOR,				// 服务器实时状态监听输出

    ENUM_MSG_MG_SENDLOCALRESOURCE,              // 发送用本地资源启服


	ENUM_MSG_MG_MAXID,							// 最大消息码
};

// 最大消息码
#define MSG_MG_MAXID						ENUM_MSG_MG_MAXID



//////////////////////////////////////////////////////////////////////////
// 握手消息
#define MSG_MG_HANDSHAKE				ENUM_MSG_MG_HANDSHAKE

// 握手消息数据结构
struct SMsgManagerHandshake
{
	WORD	nServerType;			// 服务器类型

	SMsgManagerHandshake(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 主服务器与根服务器握手消息数据结构
struct SMsgManagerHandshake_MR
{
	WORD	nServerType;			// 服务器类型
	DWORD	dwWorldID;				// 游戏世界ID
	WORD	nClientPort;			// 客户端监听端口
	WORD	nFtpPort;				// Ftp监听端口
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 机器的网卡MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 机器硬盘序号 Disk Serial Number 

	SMsgManagerHandshake_MR(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 服务器连接器与子服务器握手消息数据结构
struct SMsgManagerHandshake_SZ
{
	DWORD	dwProcessId;			// 进程ID
	WORD	nServerType;			// 服务器类型
	WORD	wSubID;					// 第n号服务器
	char szName[MANAGER_NAME_MAXSIZE];	// 服务器名

	SMsgManagerHandshake_SZ(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 子服务器与主服务器握手消息数据结构
struct SMsgManagerHandshake_ZM
{
	WORD nServerType;			// 服务器类型
	DWORD dwPackVer;			// 服务器更新包版本号
	DWORD dwPackType;			// 服务器更新包类型
	DWORD dwPackVer0;			// 现用服务器更新包版本号
	DWORD dwPackType0;			// 现用服务器更新包类型
	char szHostName[MANAGER_NAME_MAXSIZE];		// 机器名称
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 机器的网卡MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 机器硬盘序号 Disk Serial Number 

	SMsgManagerHandshake_ZM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// Ftp客户端与主服务器握手消息数据结构
struct SMsgManagerFtpHandshake
{
	WORD nServerType;			// 服务器类型
	DWORD dwClientID;			// 对应的连接ID

	SMsgManagerFtpHandshake(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 回应握手消息
#define MSG_MG_HANDSHAKE_RESPONSE		ENUM_MSG_MG_HANDSHAKE_RESPONSE

// 回应握手消息数据结构
struct SMsgManagerHandshakeResponse	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间

	SMsgManagerHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 根服务器回应客户端握手消息数据结构
struct SMsgManagerHandshakeResponse_RC	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间
	BYTE	nKeyLen;			// 登陆服务器提供的密钥长度
	//char	pKeyData[nKeyLen];	// 登陆服务器提供的密钥内容

	SMsgManagerHandshakeResponse_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 主服务器回应客户端握手消息数据结构
struct SMsgManagerHandshakeResponse_MC	
{
	DWORD	dwID;				// 连接ID
	BYTE	nKeyLen;			// 登陆服务器提供的密钥长度
	//char	pKeyData[nKeyLen];	// 登陆服务器提供的密钥内容

	SMsgManagerHandshakeResponse_MC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 根服务器回应主服务器握手消息数据结构
struct SMsgManagerHandshakeResponse_RM
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间
	BYTE	nKeyLen;			// 登陆服务器提供的密钥长度
	//char	pKeyData[nKeyLen];	// 登陆服务器提供的密钥内容

	SMsgManagerHandshakeResponse_RM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 回应握手消息数据结构
struct SMsgManagerHandshakeResponse_MZ	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwUID;				// UID
	DWORD	dwServerTime;		// 服务器时间
	DWORD	dwWorldID;			// 游戏世界ID
	char	szWorldName[MANAGER_NAME_MAXSIZE];	// 游戏世界名称

	SMsgManagerHandshakeResponse_MZ(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 子服务器回应软件服务器握手消息数据结构
struct SMsgManagerHandshakeResponse_ZS	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间
	char	szFtpRootPath[MANAGER_LONGSTR_MAXSIZE];	// 游戏世界名称

	SMsgManagerHandshakeResponse_ZS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 断线消息
#define MSG_MG_DISCONNECTED		ENUM_MSG_MG_DISCONNECTED

// 断线消息数据结构
struct SMsgManagerDisconnected
{
	DWORD	dwReason;			// 原因

	SMsgManagerDisconnected(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 心跳检查
#define MSG_MG_KEEPALIVE		ENUM_MSG_MG_KEEPALIVE


//////////////////////////////////////////////////////////////////////////
// 回应心跳检查
#define MSG_MG_ANSWER_KEEPALIVE		ENUM_MSG_MG_ANSWER_KEEPALIVE

// 心跳检查消息数据结构
struct SMsgManagerAnswerKeepAlive
{
	DWORD	dwUID;			// 软件服务器UID

	SMsgManagerAnswerKeepAlive(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 请求主服务器网关服务发送数据给根服务器消息
#define MSG_MG_SENDROOTDATA		ENUM_MSG_MG_SENDROOTDATA

// 请求主服务器网关服务发送数据给根服务器消息数据结构
struct SMsgSendRootData_MR
{
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据

	SMsgSendRootData_MR(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 请求主服务器网关服务发送数据给客户端消息
#define MSG_MG_SENDCLIENTDATA		ENUM_MSG_MG_SENDCLIENTDATA

// 请求主服务器网关服务发送数据给客户端消息数据结构
struct SMsgSendClientData_MC
{
	DWORD	dwClientID;			// 客户端ID
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据

	SMsgSendClientData_MC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 请求主服务器网关服务发送数据给各硬件服务器消息
#define MSG_MG_SENDHARDDATA		ENUM_MSG_MG_SENDHARDDATA

// 请求主服务器网关服务发送数据给各硬件服务器数据结构
struct SMsgSendHardData_MZ
{
	DWORD	dwComputerUID;		// 硬件服务器UID
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据

	SMsgSendHardData_MZ(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 请求主服务器网关服务发送数据给各软件服务器消息
#define MSG_MG_SENDSOFTDATA		ENUM_MSG_MG_SENDSOFTDATA

// 请求主服务器网关服务发送数据给各软件服务器数据结构
struct SMsgSendSoftData_M
{
	DWORD	dwServerUID;		// 软件服务器UID
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据

	SMsgSendSoftData_M(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 请求子服务器网关服务发送数据给各软件服务器消息
#define MSG_MG_SENDSUBDATA		ENUM_MSG_MG_SENDSUBDATA

// 请求子服务器网关服务发送数据给各软件服务器数据结构
struct SMsgSendSubData_MZ
{
	DWORD	dwServerID;			// 软件服务器在子服务器ID
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据

	SMsgSendSubData_MZ(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 请求子服务器网关服务发送数据给主服务器消息
#define MSG_MG_SENDMAINDATA		ENUM_MSG_MG_SENDMAINDATA

// 请求子服务器网关服务发送数据给主服务器数据结构
struct SMsgSendSubData_ZM
{
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据

	SMsgSendSubData_ZM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



//////////////////////////////////////////////////////////////////////////
// 用户登录消息
#define MSG_MG_USERLOGIN		ENUM_MSG_MG_USERLOGIN

// 用户登录消息数据结构
struct SMsgManagerUserLogin_CR
{
	DWORD  dwReserve;							// 保留字段,暂时不用,起干扰作用
	char szUserName[MANAGER_NAME_MAXSIZE];		// 管理员登录帐号
	char szPassword[MANAGER_NAME_MAXSIZE];		// 管理员登录帐号密码,已加密
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 登录机器的网卡MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 登录机器硬盘序号 Disk Serial Number 
};

//////////////////////////////////////////////////////////////////////////
// 用户登录结果消息
#define MSG_MG_USERLOGIN_RESULT		ENUM_MSG_MG_USERLOGIN_RESULT

// 用户登录结果消息数据结构
struct SMsgManagerUserLoginResult_RC
{
	BYTE nResult;							// 返回验证登录标识,0:验证失败,1:验证成功
	BYTE nFailCode;							// 验证失败的类型代码, 0,无, 1:用户被禁用,2:用户名不存在,3:密码不正确,4:网卡MAC地址不正确,5:硬盘序号不正确 
	int nResultMsgLen;						// 验证结果信息长度
	int nUserTitleLen;						// 管理员说明长度
	int nRecordCount;						// 返回管理员有权限的游戏世界记录的个数
	// char* szResultMsg;					// 验证结果信息 nResultMsgLen

	// 验证失败不返回
	// SMsgManagerLoginUserInfo				// 用户信息
	// char* szTitle;						// 管理员说明 nUserTitleLen
	// 有权限的游戏世界结果集：nRecordCount * SMsgManagerUserWorldRightRecord

	SMsgManagerUserLoginResult_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 用户信息
struct SMsgManagerLoginUserInfo
{
	DWORD dwID;										// 管理员帐号ID, 0为无效ID
	DWORD dwType;									// 管理员帐号权限类型,参考权限类型定义 EMManagerUserType
	DWORD dwFlag;									// 管理员帐号标识,0:无效,1:有效
	DWORD dwCheckPCFlag;							// 是否起用登录机器限制,0:不起用,1:起用
	DWORD dwLoginCount;								// 登录次数
	DWORD dwLastTime;								// 最近登录时间
	char szUserName[MANAGER_NAME_MAXSIZE];			// 管理员登录帐号
	char szName[MANAGER_NAME_MAXSIZE];				// 管理员姓名
	char szDept[MANAGER_NAME_MAXSIZE];				// 管理员部门
	char szMACAddress[MANAGER_NAME_MAXSIZE];		// 最近登录的网卡MAC地址
	char szIPAddress[MANAGER_NAME_MAXSIZE];			// 最近登录的IP
	DWORD dwSpare1;									// 备用1
	DWORD dwSpare2;									// 备用2
	DWORD dwSpare3;									// 备用3
	DWORD dwKeyData[3];								// 令牌数据

	SMsgManagerLoginUserInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
// 有权限的游戏世界和权限信息
struct SMsgManagerUserWorldRightRecord
{
	DWORD dwWorldID;							// 游戏世界ID
	DWORD dwRightValue;							// 权限数值,参考权限定义

	SMsgManagerUserWorldRightRecord(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 用户注销登录消息
#define MSG_MG_USERLOGOUT		ENUM_MSG_MG_USERLOGOUT

// 用户注销登录消息数据结构
struct SMsgManagerUserLogout_CR
{
	DWORD dwUserID;							// 管理员帐号ID, 0为无效ID

	SMsgManagerUserLogout_CR(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////////////
// 用户注销登录结果消息
#define MSG_MG_USERLOGOUT_RESULT		ENUM_MSG_MG_USERLOGOUT_RESULT

// 用户注销登录结果消息数据结构
struct SMsgManagerUserLogoutResult_RC
{
	DWORD dwUserID;							// 管理员帐号ID, 0为无效ID
	BYTE nResult;							// 返回验证登录标识,0:验证失败,1:验证成功
	BYTE nFailCode;							// 验证失败的类型代码, 0,无....
	int nResultMsgLen;						// 验证结果信息长度
	// char* szResultMsg;					// 验证结果信息 nResultMsgLen

	SMsgManagerUserLogoutResult_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



//////////////////////////////////////////////////////////////////////////
// 修改密码消息
#define MSG_MG_CHANGEPWD		ENUM_MSG_MG_CHANGEPWD

// 修改密码消息数据结构
struct SMsgManagerChangePassword_CR
{
	DWORD  dwReserve;							// 保留字段,暂时不用,起干扰作用
	char szUserName[MANAGER_NAME_MAXSIZE];		// 管理员登录帐号
	char szPassword[MANAGER_NAME_MAXSIZE];		// 管理员登录帐号密码,已加密
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 登录机器的网卡MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 登录机器硬盘序号 Disk Serial Number 
	char szNewPassword[MANAGER_NAME_MAXSIZE];	// 新的管理员登录帐号密码,已加密

	SMsgManagerChangePassword_CR(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 修改密码结果消息
#define MSG_MG_CHANGEPWD_RESULT		ENUM_MSG_MG_CHANGEPWD_RESULT

// 修改密码结果消息数据结构
struct SMsgManagerChangePasswordResult_RC
{
	BYTE nResult;				// 返回修改密码成功标识,0:修改失败,1:修改成功
	BYTE nFailCode;				// 修改失败的类型代码, 0,无, 1:用户被禁用,2:用户名不存在,3:密码不正确,4:网卡MAC地址不正确,5:硬盘序号不正确,6:无效新密码
	int nResultMsgLen;			// 验证结果信息长度
	// char* szResultMsg;		// 验证结果信息 nResultMsgLen

	SMsgManagerChangePasswordResult_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 发送游戏结构树信息给客户端消息
#define MSG_MG_SENDCLIENT_GAMETREE		ENUM_MSG_MG_SENDCLIENT_GAMETREE

// 发送游戏结构树信息给客户端消息数据结构
struct SMsgManagerSendClientGameTree_RC
{
	int nGameCounts;				// 游戏个数
	int nGameAreaCounts;			// 游戏区域个数
	int nGameWorldCounts;			// 游戏世界个数

	// nGameCounts * SMsgGameInfo			// 所有游戏列表
	// nGameAreaCounts * SMsgGameAreaInfo	// 所有游戏区域列表
	// nGameWorldCounts * SMsgGameWorldInfo	// 所有游戏世界列表
	// nGameWorldCounts 个 char IP+szName[nNameStrLens];	// 游戏世界名称,放在所有列表后面

	SMsgManagerSendClientGameTree_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 游戏信息数据结构
struct SMsgGameInfo
{
	DWORD dwID;									// 游戏ID, 0为无效ID,数据库设为唯一 
	DWORD dwIndex;								// 游戏序号,用来排序用
	DWORD dwFlag;								// 游戏标识,0:无效,1:有效游戏,2:有效服务器群 
	char szName[MANAGER_NAME_MAXSIZE];			// 游戏名称
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3

	SMsgGameInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 游戏区域信息数据结构
struct SMsgGameAreaInfo
{
	DWORD dwID;									// 游戏区域ID, 0为无效ID,数据库设为自增唯一
	DWORD dwGameID;								// 所属游戏ID 取 ManagerGameList.dwID
	DWORD dwIndex;								// 游戏区域序号,用来排序用
	DWORD dwFlag;								// 游戏区域标识,0:无效,1:有效
	char szName[MANAGER_NAME_MAXSIZE];			// 游戏区域名称
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3

	SMsgGameAreaInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 游戏世界信息数据结构
struct SMsgGameWorldInfo
{
	DWORD dwID;									// 游戏世界ID, 0为无效ID,数据库设为自增唯一
	DWORD dwAreaID;								// 所属游戏区域ID 取 ManagerAreaList.dwID
	DWORD dwIndex;								// 游戏世界序号,用来排序用
	DWORD dwFlag;								// 游戏世界标识,0:无效,1:有效,2:推荐,3:新 
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3
	bool bIsTemp;								// 是否临时游戏世界
	DWORD dwServerID;							// 主服务器ID
	bool bOnline;								// 是否开启
	DWORD dwAddTime;							// 加入时间
	WORD nClientPort;							// 客户端监听端口
	BYTE nIPStrLens;							// 主服务器IP长度
	BYTE nNameStrLens;							// 游戏世界名称长度
	//char * szIP;								// 主服务器IP,放在所有列表后面
	//char * szName;							// 游戏世界名称,放在所有列表后面

	SMsgGameWorldInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 发送游戏世界服务器相关信息给主服务器消息
#define MSG_MG_SENDMAIN_WORLDINFO		ENUM_MSG_MG_SENDMAIN_WORLDINFO


// 游戏世界信息数据结构
struct SMsgMainGameWorldInfo
{
	DWORD dwID;									// 游戏世界ID, 0为无效ID,数据库设为自增唯一
	DWORD dwGameID;								// 所属游戏ID 取 ManagerGameList.dwID
	DWORD dwAreaID;								// 所属游戏区域ID 取 ManagerAreaList.dwID
	DWORD dwIndex;								// 游戏世界序号,用来排序用
	DWORD dwFlag;								// 游戏世界标识,0:无效,1:有效,2:推荐,3:新 
	char szName[MANAGER_NAME_MAXSIZE];			// 游戏世界名称
	char szAreaName[MANAGER_NAME_MAXSIZE];		// 游戏区域名称
	char szGameName[MANAGER_NAME_MAXSIZE];		// 游戏名称
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3
	bool bIsTemp;								// 是否临时游戏世界

	SMsgMainGameWorldInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 发送游戏世界服务器相关信息给主服务器消息数据结构
struct SMsgManagerSendMainWorldInfo_RM : public SMsgMainGameWorldInfo
{
	int nGameComputerCounts;		// 硬件服务器个数
	int nGameServerCounts;			// 软件服务器个数

	// nGameServerCounts * SMsgGameServerInfo			// 所有软件服务器列表
	// nGameComputerCounts * SMsgGameComputerInfo		// 所有硬件服务器列表
	// nGameComputerCounts 个 char szRegSN[nRegSNStrLens];	// 硬件服务器授权注册码,放在列表后面

	SMsgManagerSendMainWorldInfo_RM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



// 硬件服务器信息数据结构
struct SMsgGameComputerInfo
{
	DWORD dwID;									// 硬件服务器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
	DWORD dwIndex;								// 硬件服务器序号,用来排序用
	DWORD dwFlag;								// 硬件服务器标识,0:无效,1:有效
	DWORD dwComputerInfoID;						// 对应的机器ID  取 ManagerComputerInfo.dwID 
	char szName[MANAGER_NAME_MAXSIZE];			// 硬件服务器名称
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 硬件服务器 MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 硬件服务器硬盘序号 Disk Serial Number
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3
	BYTE nRegSNStrLens;							// 硬件服务器授权注册码长度

	// char* szRegSN;	// 硬件服务器授权注册码,放在列表后面

	SMsgGameComputerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 软件服务器信息数据结构
struct SMsgGameServerInfo
{
	DWORD dwID;									// 软件服务器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
	DWORD dwComputerID;							// 所属硬件服务器ID 取 ManagerComputerList.dwID
	DWORD dwIndex;								// 软件服务器序号,用来排序用
	DWORD dwType;								// 软件服务器类型,参考服务器类型定义
	DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	DWORD dwFlag;								// 软件服务器标识,0:无效,1:有效
	char szName[MANAGER_NAME_MAXSIZE];			// 软件服务器名称
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3

	SMsgGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



//////////////////////////////////////////////////////////////////////////
// 发送用户登录信息给主服务器消息
#define MSG_MG_SENDMAIN_LOGININFO		ENUM_MSG_MG_SENDMAIN_LOGININFO

// 发送用户登录信息给主服务器消息数据结构
struct SMsgManagerSendMainLoginInfo_RM
{
	DWORD dwWorldID;		// 游戏世界ID
	DWORD dwUserID;			// 用户ID
	DWORD dwType;			// 管理员帐号权限类型,参考权限类型定义
	DWORD dwFlag;			// 管理员帐号标识,0:无效,1:有效
	bool bLogin;			// 是否登录
	DWORD dwRightValue;		// 用户权限
	DWORD dwKeyData[3];		// 令牌数据
	DWORD dwRootClientID;	// 根服务器上的客户端ID
	char szName[MANAGER_NAME_MAXSIZE]; // 管理员姓名

	SMsgManagerSendMainLoginInfo_RM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 回馈用户登录给根服务器消息
#define MSG_MG_FEEDBACK_LOGININFO		ENUM_MSG_MG_FEEDBACK_LOGININFO

// 回馈用户登录给根服务器消息数据结构
struct SMsgManagerFeedbackLoginInfo_MR
{
	DWORD dwWorldID;		// 游戏世界ID
	DWORD dwUserID;			// 用户ID
	bool bLogin;			// 是否登录

	SMsgManagerFeedbackLoginInfo_MR(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  通知客户端登录主服务器
#define MSG_MG_SENDCLIENT_MAINLOGIN		ENUM_MSG_MG_SENDCLIENT_MAINLOGIN

//  通知客户端登录主服务器数据结构
struct SMsgManagerSendClientMainLogin_RC
{
	DWORD dwWorldID;		// 游戏世界ID
	DWORD dwServerID;		// 主服务器ID
	DWORD dwUserID;			// 用户ID
	bool bLogin;			// 是否登录
	WORD nClientPort;		// 客户端监听端口
	WORD nFtpPort;			// Ftp监听端口
	BYTE nIPStrLens;		// 主服务器IP长度
	//char * szIP;			// 主服务器IP

	SMsgManagerSendClientMainLogin_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  用户登录主服务器
#define MSG_MG_USER_MAINLOGIN		ENUM_MSG_MG_USER_MAINLOGIN

//  用户登录主服务器数据结构
struct SMsgManagerUserMainLogin_CM
{
	DWORD dwWorldID;		// 游戏世界ID
	DWORD dwUserID;			// 用户ID
	DWORD dwRootClientID;	// 根服务器上的客户端ID
	DWORD dwKeyData[3];		// 令牌数据

	SMsgManagerUserMainLogin_CM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



//////////////////////////////////////////////////////////////////////////
//  用户登录主服务器结果消息
#define MSG_MG_USER_MAINLOGIN_RESULT		ENUM_MSG_MG_USER_MAINLOGIN_RESULT

//  用户登录主服务器结果消息数据结构
struct SMsgManagerUserMainLoginResult_MC
{
	int nGameComputerCounts;		// 硬件服务器个数
	int nGameServerCounts;			// 软件服务器个数

	// nGameComputerCounts * SMsgMainGameComputerInfo		// 所有硬件服务器列表
	// nGameServerCounts * SMsgMainGameServerInfo			// 所有软件服务器列表

	SMsgManagerUserMainLoginResult_MC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 主服务器上的硬件服务器信息数据结构
struct SMsgMainGameComputerInfo
{
	DWORD dwUID;								// 硬件服务器UID,主服务器上的ID
	DWORD dwSID;								// 硬件服务器连接ID,主服务器上的连接ID
	DWORD dwID;									// 硬件服务器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
	DWORD dwIndex;								// 硬件服务器序号,用来排序用
	DWORD dwFlag;								// 硬件服务器标识,0:无效,1:有效
	bool bOnline;								// 是否开启
	DWORD dwAddTime;							// 加入时间
	DWORD dwPackVer;							// 服务器更新包版本号
	DWORD dwPackType;							// 服务器更新包类型
	DWORD dwPackVer0;							// 现用服务器更新包版本号
	DWORD dwPackType0;							// 现用服务器更新包类型
	char szName[MANAGER_NAME_MAXSIZE];			// 硬件服务器名称
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 硬件服务器 MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 硬件服务器硬盘序号 Disk Serial Number
	char szHostName[MANAGER_NAME_MAXSIZE];		// 硬件服务器电脑名称

	SMsgMainGameComputerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 主服务器上的软件服务器信息数据结构
struct SMsgMainGameServerInfo
{
	DWORD dwUID;								// 软件服务器UID,主服务器上的ID
	DWORD dwID;									// 软件服务器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
	DWORD dwComputerID;							// 所属硬件服务器ID 取 ManagerComputerList.dwID
	DWORD dwIndex;								// 软件服务器序号,用来排序用
	DWORD dwType;								// 软件服务器类型,参考服务器类型定义
	DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	DWORD dwFlag;								// 软件服务器标识,0:无效,1:有效
	bool bOnline;								// 是否开启
	DWORD dwStateFlag;							// 服务器状态
	DWORD dwAddTime;							// 加入时间
	DWORD dwComputerUID;						// 所在硬件服务器UID
	DWORD dwSubServerID;						// 所在子服务器上的ID
	char szName[MANAGER_NAME_MAXSIZE];			// 软件服务器名称
	char szRegName[MANAGER_NAME_MAXSIZE];		// 注册的软件服务器名称

	SMsgMainGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  根服务器更新各游戏世界状态给客户端
#define MSG_MG_UPDATE_WORLDSTATE		ENUM_MSG_MG_UPDATE_WORLDSTATE

//  根服务器更新各游戏世界状态给客户端数据结构
struct SMsgRootUpdateWorldState_RC
{
	DWORD dwWorldID;		// 游戏世界ID
	bool bOnline;			// 是否开启

	SMsgRootUpdateWorldState_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  踢出用户消息
#define MSG_MG_KICKOUTUSER		ENUM_MSG_MG_KICKOUTUSER

//  踢出用户消息数据结构
struct SMsgRootKickoutUserInfo_RC
{
	DWORD dwUserID;			// 用户ID
	BYTE nKickoutType;		// 踢出类型
	DWORD dwSpare;			// 备用数字
	char szMsg[MANAGER_TITLE_MAXSIZE];			// 信息字串

	SMsgRootKickoutUserInfo_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  更新软件服务器信息消息
#define MSG_MG_UPDATE_SOFTINFO		ENUM_MSG_MG_UPDATE_SOFTINFO

//  更新软件服务器信息消息数据结构
struct SMsgManagerUpdateSoftServerInfo_ZM
{
	DWORD				dwServerID;			// 服务器ID
	WORD				wServerType;		// 服务器类型,会根据参见消息结构头第一个字段转发消息给不同的服务器
	WORD				wSubID;				// 第n号服务器
	bool				bOnline;			// 在线
	DWORD				dwStateFlag;		// 服务器状态
	char				szName[MANAGER_NAME_MAXSIZE];	// 服务器名

	SMsgManagerUpdateSoftServerInfo_ZM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 发送游戏世界数据给客户端消息
#define MSG_MG_UPDATEWORLD_CLIENT		ENUM_MSG_MG_UPDATEWORLD_CLIENT

//  发送游戏世界数据给客户端消息数据结构
struct SMsgManagerUpdateWorldInfo_RC
{
	BYTE nUpdateType;							// 0:新增，1:修该,2:删除
	DWORD dwID;									// 游戏世界ID, 0为无效ID,数据库设为自增唯一
	DWORD dwAreaID;								// 所属游戏区域ID 取 ManagerAreaList.dwID
	DWORD dwIndex;								// 游戏世界序号,用来排序用
	DWORD dwFlag;								// 游戏世界标识,0:无效,1:有效,2:推荐,3:新 
	DWORD dwSpare1;								// 备用1
	DWORD dwSpare2;								// 备用2
	DWORD dwSpare3;								// 备用3
	bool bIsTemp;								// 是否临时游戏世界
	DWORD dwServerID;							// 主服务器ID
	bool bOnline;								// 是否开启
	DWORD dwAddTime;							// 加入时间
	WORD nClientPort;							// 客户端监听端口
	char szIP[MANAGER_NAME_MAXSIZE];			// 主服务器IP,放在所有列表后面
	char szName[MANAGER_NAME_MAXSIZE];			// 游戏世界名称,放在所有列表后面

	SMsgManagerUpdateWorldInfo_RC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  主服务器动态更新各软件服务器状态给客户端
#define MSG_MG_UPDATE_SERVERINFO		ENUM_MSG_MG_UPDATE_SERVERINFO

//  主服务器动态更新各软件服务器状态给客户端数据结构
struct SMsgMainUpdateServerInfo_MC
{
	BYTE nUpdateType;							// 0:新增，1:修该,2:删除
	DWORD dwUID;								// 软件服务器UID,主服务器上的ID
	DWORD dwID;									// 软件服务器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
	DWORD dwComputerID;							// 所属硬件服务器ID 取 ManagerComputerList.dwID
	DWORD dwIndex;								// 软件服务器序号,用来排序用
	DWORD dwType;								// 软件服务器类型,参考服务器类型定义
	DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	DWORD dwFlag;								// 软件服务器标识,0:无效,1:有效
	bool bOnline;								// 是否开启
	DWORD dwStateFlag;							// 服务器状态
	DWORD dwAddTime;							// 加入时间
	DWORD dwComputerUID;						// 所在硬件服务器UID
	DWORD dwSubServerID;						// 所在子服务器上的ID
	BYTE nNameStrlens;							// 软件服务器名称长度
	BYTE nRegNameStrlens;						// 注册的软件服务器名称
	//char szName[nNameStrlens];				// 软件服务器名称
	//char szRegName[nRegNameStrlens];			// 注册的软件服务器名称

	SMsgMainUpdateServerInfo_MC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  主服务器动态更新各硬件服务器状态给客户端
#define MSG_MG_UPDATE_COMPUTERINFO		ENUM_MSG_MG_UPDATE_COMPUTERINFO

//  主服务器动态更新各硬件服务器状态给客户端数据结构
struct SMsgMainUpdateComputerInfo_MC
{
	BYTE nUpdateType;							// 0:新增，1:修该,2:删除
	DWORD dwUID;								// 硬件服务器UID,主服务器上的ID
	DWORD dwSID;								// 硬件服务器连接ID,主服务器上的连接ID
	DWORD dwID;									// 硬件服务器ID, 0为无效ID,数据库设为自增唯一
	DWORD dwWorldID;							// 所属游戏世界ID 取 ManagerWorldList.dwID
	DWORD dwIndex;								// 硬件服务器序号,用来排序用
	DWORD dwFlag;								// 硬件服务器标识,0:无效,1:有效
	bool bOnline;								// 是否开启
	DWORD dwAddTime;							// 加入时间
	DWORD dwPackVer;							// 服务器更新包版本号
	DWORD dwPackType;							// 服务器更新包类型
	DWORD dwPackVer0;							// 现用服务器更新包版本号
	DWORD dwPackType0;							// 现用服务器更新包类型
	BYTE nNameStrlens;							// 硬件服务器名称长度
	BYTE nMACStrlens;							// 硬件服务器 MAC地址长度
	BYTE nDiskSNStrlens;						// 硬件服务器名称长度
	BYTE nHostStrlens;							// 硬件服务器电脑名称长度
	//char szName[nNameStrlens];				// 硬件服务器名称
	//char szMACAddress[nMACStrlens];			// 硬件服务器 MAC地址
	//char szDiskSN[nDiskSNStrlens];			// 硬件服务器硬盘序号 Disk Serial Number
	//char szHostName[nDiskSNStrlens];			// 硬件服务器电脑名称

	SMsgMainUpdateComputerInfo_MC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  发送服务器启动脚本文件数据给指定游戏世界
#define MSG_MG_SENDSCRIPTDATA		ENUM_MSG_MG_SENDSCRIPTDATA

//  更新软件服务器信息消息数据结构
struct SMsgManagerSendScriptData_RM
{
	DWORD dwWorldID;					// 游戏世界ID
	BYTE nServerType;					// 服务器类型
	DWORD dwScriptGroupID;				// 脚本文件组ID
    BYTE  nEncry;                       // 文件加密方式
    BYTE  nResourceType;                // 资源类型
	BYTE nMaxSchemeIndex;				// 最大配置号
	BYTE nSchemeIndex;					// 配置号（从一号脚本开始读取）
	DWORD dwDataLens;					// 本配置号数据长度
	// BYTE * pData;					// 配置数据

	SMsgManagerSendScriptData_RM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  向根服务器取服务器启动脚本文件数据
#define MSG_MG_GETSCRIPTDATA		ENUM_MSG_MG_GETSCRIPTDATA

//  向根服务器取服务器启动脚本文件数据消息数据结构
struct SMsgManagerGetScriptData_MR
{
	DWORD dwWorldID;					// 游戏世界ID

	SMsgManagerGetScriptData_MR(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



//////////////////////////////////////////////////////////////////////////
//  各软件服务器向主服务器请求启动
#define MSG_MG_REQUESTSTART		ENUM_MSG_MG_REQUESTSTART

//  各软件服务器向主服务器请求启动数据结构
struct SMsgManagerRequestStartServer_Z
{
	DWORD dwStartFlag;							// 启动标识 EMManagerStartServerFlag
	DWORD dwUserData;							// 用户数据
	DWORD dwHardServerID;						// 硬件服务器ID
	DWORD dwServerOnHardID;						// 所在硬件服务器上ID
	char szMACAddress[MANAGER_NAME_MAXSIZE];	// 硬件服务器 MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];		// 硬件服务器硬盘序号 Disk Serial Number

	SMsgManagerRequestStartServer_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  各软件服务器向主服务器请求启动结果消息
#define MSG_MG_REQUESTSTART_RESULT		ENUM_MSG_MG_REQUESTSTART_RESULT

//  各软件服务器向主服务器请求启动结果消息数据结构
struct SMsgManagerRequestStartResult_M
{
	bool bResult;								// 能否启动
	DWORD dwResultFlag;							// 结果代码标识 EMRequestStartResultCode
	DWORD dwStartFlag;							// 发过来的启动标识
	DWORD dwUserData;							// 用户数据
	char szResultMsg[MANAGER_LONGSTR_MAXSIZE];	// 结果信息

	SMsgManagerRequestStartResult_M(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  要求子服务器启动软件服务器消息
#define MSG_MG_STARTSERVER		ENUM_MSG_MG_STARTSERVER

//  要求子服务器启动软件服务器消息数据结构
struct SMsgManagerStartServer_Z
{
	DWORD dwUserID;								// 操作启动用户ID,0为系统
	DWORD dwServerUID;							// 未启动的软件服务器UID
	DWORD dwServerType;							// 服务器类型
	DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	DWORD dwStartFlag;							// 启动标识
	DWORD dwDelays;								// 延迟启动毫秒数

	SMsgManagerStartServer_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  要求软件服务器关闭消息
#define MSG_MG_CLOSESERVER		ENUM_MSG_MG_CLOSESERVER

//  要求软件服务器关闭消息数据结构
struct SMsgManagerCloseServer_Z
{
	DWORD dwUserID;								// 操作关闭用户ID,0为系统
	DWORD dwServerUID;							// 软件服务器UID
	DWORD dwServerType;							// 服务器类型
	DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	DWORD dwCloseFlag;							// 关闭标识
	DWORD dwDelays;								// 延迟关闭毫秒数

	SMsgManagerCloseServer_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  要求子服务器强杀软件服务器进程消息
#define MSG_MG_KILLSERVER		ENUM_MSG_MG_KILLSERVER

//  要求子服务器强杀软件服务器进程消息数据结构
struct SMsgManagerKillServer_Z
{
	DWORD dwUserID;								// 操作强杀用户ID,0为系统
	DWORD dwServerUID;							// 软件服务器UID
	DWORD dwServerOnHardID;						// 所在硬件服务器上ID
	DWORD dwServerType;							// 服务器类型
	DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	DWORD dwKillFlag;							// 强杀标识
	DWORD dwDelays;								// 延迟强杀毫秒数

	SMsgManagerKillServer_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  要求软件服务器发布公告消息
#define MSG_MG_SERVERSENDAD		ENUM_MSG_MG_SERVERSENDAD

//  要求软件服务器发布公告消息数据结构
struct SMsgManagerServerSendAd_Z
{
	DWORD dwUserID;								// 操作发布用户ID,0为系统
	DWORD dwServerUID;							// 软件服务器UID
	DWORD dwServerType;							// 服务器类型
	DWORD dwSubID;								// 软件服务器第n号服务器,只有一个服务器的为0,其它的从1~n 
	DWORD dwSendFlag;							// 发布标识
	DWORD dwDelays;								// 延迟发布毫秒数
	WORD nSendMsgLens;							// 公告长度
	//char szSendMsg[nSendMsgLens];				// 公告信息

	SMsgManagerServerSendAd_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  各软件服务器向主服务器请求关闭
#define MSG_MG_REQUESTCLOSE		ENUM_MSG_MG_REQUESTCLOSE

//  各软件服务器向主服务器请求关闭数据结构
struct SMsgManagerRequestCloseServer_Z
{
	DWORD dwCloseFlag;							// 关闭标识 EMManagerCloseServerFlag
	DWORD dwUserData;							// 用户数据
	DWORD dwHardServerID;						// 硬件服务器ID
	DWORD dwServerOnHardID;						// 所在硬件服务器上ID

	SMsgManagerRequestCloseServer_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  要求子服务器重起硬件服务器
#define MSG_MG_RESTARTCOMPUTER		ENUM_MSG_MG_RESTARTCOMPUTER

//  要求子服务器重起硬件服务器数据结构
struct SMsgManagerRestartComputer_Z
{
	DWORD dwUserID;								// 操作关闭用户ID,0为系统
	DWORD dwRestartFlag;						// 重起标识 EMManagerRestartComputerFlag
	DWORD dwUserData;							// 用户数据
	DWORD dwHardServerID;						// 硬件服务器ID
	DWORD dwDelays;								// 延迟重起毫秒数

	SMsgManagerRestartComputer_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  硬件服务器向主服务器请求重起
#define MSG_MG_REQUESTRESTART		ENUM_MSG_MG_REQUESTRESTART

//  硬件服务器向主服务器请求重起数据结构
struct SMsgManagerRequestRestartComputer_Z
{
	DWORD dwCloseFlag;							// 关闭标识 EMManagerCloseServerFlag
	DWORD dwUserData;							// 用户数据

	SMsgManagerRequestRestartComputer_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 子服务器更新版本消息
#define MSG_MG_UPDATEVERSION		ENUM_MSG_MG_UPDATEVERSION
//  子服务器更新版本消息数据结构
struct SMsgManagerRequestUpdateVersion_Z
{
	DWORD dwPackVer;				// 服务器更新包版本号
	DWORD dwPackType;				// 服务器更新包类型
	DWORD dwPackVer0;				// 现用服务器更新包版本号
	DWORD dwPackType0;				// 现用服务器更新包类型

	SMsgManagerRequestUpdateVersion_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  要求根服务器加入日志
#define MSG_MG_ADDLOG		ENUM_MSG_MG_ADDLOG

//  要求根服务器加入日志消息数据结构
struct SMsgManagerSendAddLogData_R
{
	DWORD dwType;									// 日志类型,参考日志类型定义
	DWORD dwUserID;									// 操作管理员帐号ID,服务器的日志时为0
	DWORD dwWorldID;								// 游戏世界ID 
	DWORD dwComputerID;								// 硬件服务器ID 
	DWORD dwServerID;								// 软件服务器ID
	DWORD dwSpare1;									// 备用1
	DWORD dwSpare2;									// 备用2
	DWORD dwSpare3;									// 备用3
	WORD nNameLens;									// 管理员姓名长度
	WORD nUserNameLens;								// 管理员登录帐号长度
	WORD nLogTxtLens;								// 日志内容长度
	//char szName[nNameLens];						// 管理员姓名
	//char szUserName[nUserNameLens];				// 管理员登录帐号
	//char szLogTxt[nLogTxtLens];					// 日志内容

	SMsgManagerSendAddLogData_R(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



//////////////////////////////////////////////////////////////////////////
//  通知服务器显示程序窗口
#define MSG_MG_SHOWWINDOW		ENUM_MSG_MG_SHOWWINDOW

//  通知服务器显示程序窗口数据结构
struct SMsgManagerShowServerWindow_Z
{
	DWORD dwFlag;							// 标识

	SMsgManagerShowServerWindow_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  设定服务器状态
#define MSG_MG_SETSERVERSTATE		ENUM_MSG_MG_SETSERVERSTATE

//  设定服务器状态数据结构
struct SMsgManagerSetServerState_Z
{
	DWORD dwFlag;							// 标识

	SMsgManagerSetServerState_Z(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  操作游戏世界
#define MSG_MG_CONTROLWORLD		ENUM_MSG_MG_CONTROLWORLD

//  操作游戏世界数据结构
struct SMsgManagerControlWorld_CR
{
	DWORD dwWorldID;						// 游戏世界ID 
	DWORD dwControlCode;					// 操作标识  EMManagerWorldControlCode
	DWORD dwUserData;						// 用户数据

	SMsgManagerControlWorld_CR(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  操作硬件服务器消息
#define MSG_MG_CONTROLHARDSERVER		ENUM_MSG_MG_CONTROLHARDSERVER

//  操作硬件服务器消息数据结构
struct SMsgManagerControlHardServer_CZ
{
	DWORD dwUserID;							// 操作用户ID
	DWORD dwClientID;						// 客户端ID
	DWORD dwControlCode;					// 操作标识  EMManagerHardControlCode
	DWORD dwUserData;						// 用户数据
	// 附加数据

	SMsgManagerControlHardServer_CZ(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
/// 各操作数据说明
// MG_HardControl_UseCmd 使用Cmd控制台
// 用户数据 1:打开Cmd ,2:关闭Cmd

// MG_HardControl_RunCmd  运行Cmd命令
struct SMsgManagerRunCmdData
{
	DWORD dwStrLens;						// Cmd命令长度
	//char szCmdLine[dwStrLens];			// Cmd命令

	SMsgManagerRunCmdData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  硬件服务器输出数据消息
#define MSG_MG_HARDSERVERDATA		ENUM_MSG_MG_HARDSERVERDATA

//  硬件服务器输出数据结构
struct SMsgManagerHardServerData_ZC
{
	DWORD dwHardServerUID;					// 服务器UID
	DWORD dwDataType;						// 输出数据类型  EMManagerHardDataType
	DWORD dwUserData;						// 用户数据
	// 附加数据

	SMsgManagerHardServerData_ZC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

/// 各数据类型说明
// MG_HardData_OpenCmdResult,MG_HardData_CloseCmdResult  使用Cmd控制台结果
// 用户数据 0:不成功,1:成功 

// MG_HardData_RunCmdResult  运行Cmd命令结果
// 用户数据 0:不成功,1:成功 2:Cmd运行返回
struct SMsgManagerRunCmdResultData
{
	DWORD dwStrLens;						// Cmd命令结果长度
	//char szCmdResult[dwStrLens];			// Cmd命令结果

	SMsgManagerRunCmdResultData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  操作软件服务器消息
#define MSG_MG_CONTROLSOFTSERVER		ENUM_MSG_MG_CONTROLSOFTSERVER

//  操作硬件服务器消息数据结构
struct SMsgManagerControlSoftServer_C
{
	DWORD dwUserID;							// 操作用户ID
	DWORD dwClientID;						// 客户端ID
	DWORD dwControlCode;					// 操作标识  EMManagerSoftControlCode
	DWORD dwUserData1;						// 用户数据1
	DWORD dwUserData2;						// 用户数据2
	DWORD dwUserData3;						// 用户数据3
	// 附加数据

	SMsgManagerControlSoftServer_C(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
/// 各操作数据说明
// MG_SoftControl_UseCmd 使用Cmd控制台
// 用户数据 1:打开Cmd ,2:关闭Cmd

// MG_SoftControl_RunCmd  运行Cmd命令
// SMsgManagerRunCmdData

// MG_SoftControl_RunGMCommand  运行GM命令

struct SMsgManagerRunGMCommandData
{
	DWORD dwRightData;						// GM权限数据
	BYTE nGMNameLens;						// GM名称长度
	DWORD dwStrLens;						// Cmd命令长度
	//char szGMName[nGMNameLens];			// GM名称
	//char szCmdLine[dwStrLens];			// Cmd命令

	SMsgManagerRunGMCommandData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MG_SoftControl_GMReplyReport  GM回复举报信息给玩家

// GM回复举报信息类型定义
enum EMManagerGMReplyReportMsgType
{
	// 未定义
	MG_GMReplyReportType_None = 0,			// 未知
	MG_GMReplyReportType_Chat,				// 聊天回复玩家举报信息
	MG_GMReplyReportType_Close,				// 关闭与玩家聊天
	// 最大
	MG_GMReplyReportType_Max
};

// GM回复举报信息给玩家数据结构
struct SMsgManagerGMReplyReportData
{
	DWORD dwMsgType;						// 消息类型
	DWORD dwPlayerID;						// 玩家角色ID
	DWORD dwMsgID;							// 举报信息ID
	BYTE nGMNameLens;						// GM名字长度
	WORD nMsgLens;							// GM回复内容字串长度
	// 附加数据
	// char szName[nGMNameLens];			// GM名字
	// char szMsg[nMsgLens];				// 回复内容

	SMsgManagerGMReplyReportData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// MG_SoftControl_UpdateScpFile,		// 请求服务器刷新脚本
struct SMsgManagerFileUpdateScpRequest
{
	DWORD dwUpdateScpID;					// ID,用来回传找对应数据对象
	WORD nFileNameLens;						// 服务器上脚本文件名长度,包括路径
	//char szFileName[nFileNameLens];		// 源文件名
};
// MG_SoftData_UpdateScpFileResult,		// 请求服务器刷新脚本结果
struct SMsgManagerFileUpdateScpResult
{
	DWORD dwUpdateScpID;					// ID,用来回传找对应数据对象
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息

	SMsgManagerFileUpdateScpResult(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  软件服务器输出数据消息
#define MSG_MG_SOFTSERVERDATA		ENUM_MSG_MG_SOFTSERVERDATA

//  硬件服务器输出数据结构
struct SMsgManagerSoftServerData_C
{
	DWORD dwSoftServerUID;					// 软件服务器UID
	DWORD dwDataType;						// 输出数据类型  EMManagerSoftDataType
	DWORD dwUserData1;						// 用户数据1
	DWORD dwUserData2;						// 用户数据2
	DWORD dwUserData3;						// 用户数据3
	// 附加数据

	SMsgManagerSoftServerData_C(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

/// 各数据类型说明
// MG_SoftData_OpenCmdResult,MG_SoftData_CloseCmdResult  使用Cmd控制台结果
// 用户数据 0:不成功,1:成功 


// MG_SoftData_GameServerAlert	游戏世界软件服务器报警信息
// dwUserData1 软件服务器类型,dwUserData2 软件服务器第n号服务器,dwUserData3 报警等级
struct SMsgManagerSoftData_ServerAlert
{
	DWORD dwAlertType;						// 报警类型  EMGameServerAlertSrcType
	WORD nMsgLens;							// 报警内容长度
	//char szMsg[nMsgLens];					// 报警内容
};


// MG_SoftData_GMCommandResult	运行GM命令结果
struct SMsgManagerSoftData_GMCommandResult
{
	BYTE nType;								// 结果类型,0.立即结果,1.异步结果
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
};

//	MG_SoftData_ReportToGM,			// 玩家举报信息给GM

// 玩家举报信息给GM类型定义
enum EMManagerReportToGMMsgType
{
	// 未定义
	MG_ReportToGMType_None = 0,			// 未知
	MG_ReportToGMType_Report,			// 玩家举报信息
	MG_ReportToGMType_ChatReport,		// 玩家与回复Gm聊天 dwUserData1 回传举报信息ID,聊天信息放在用户说明szMsg中
	MG_ReportToGMType_ReplyFail,		// GM回复失败 dwUserData1 回传举报信息ID
	// 最大
	MG_ReportToGMType_Max
};


//  玩家举报信息给GM消息数据结构
struct SMsgManagerSoftData_ReportToGM
{
	DWORD dwMsgType;						// 消息类型 EMManagerReportToGMMsgType
	DWORD dwReasonFlag;						// 原因标识 EMManagerReportToGMReasonFlag
	DWORD dwPlayerID;						// 玩家角色ID
	DWORD dwCountryID;						// 玩家角色国家ID
	DWORD dwToPlayerID;						// 举报对象玩家角色ID
	BYTE nNameLens;							// 角色名长度
	BYTE nToNameLens;						// 举报对象角色名长度
	WORD nTitleLens;						// 附加文字符串长度
	WORD nMsgLens;							// 用户说明字串长度
	// 附加数据
	// char szName[nNameLens];				// 角色名
	// char szToName[nToNameLens];			// 举报对象角色名
	// char szTitle[nTitleLens];			// 附加文字
	// char szMsg[nMsgLens];				// 用户说明

	SMsgManagerSoftData_ReportToGM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MG_SoftData_SameReport,			// 被服务器挡住的玩家举报信息,用来计数
// dwUserData1 举报人PDBID,dwUserData2 被举报人PDBID,dwUserData3 备用

//////////////////////////////////////////////////////////////////////////
//  Trace输出数据
#define MSG_MG_TRACEDATA		ENUM_MSG_MG_TRACEDATA

struct SMsgManagerTraceDataNode
{
	BYTE nTraceLevel;						// 消息级别
	DWORD dwStrLens;						// 数据长度
	//char szMsg[dwStrLens];				// Trace输出数据,放在后面
};

//  Trace输出数据结构
struct SMsgManagerTraceData_C
{
	DWORD dwSoftServerUID;					// 软件服务器UID
	DWORD dwLogCounts;						// Trace条数
	
	// dwLogCounts * SMsgManagerTraceDataNode
	// dwLogCounts * char szMsg[dwStrLens]

	SMsgManagerTraceData_C(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  实时数据输出
#define MSG_MG_REALTIMEDATA		ENUM_MSG_MG_REALTIMEDATA

//  实时数据输出结构
struct SMsgManagerRealTimeData_C
{
	DWORD dwSoftServerUID;					// 软件服务器UID
	DWORD dwRTDataFlag;						// 有数据的位置标识,位标识
	BYTE nDataCounts;						// 数据个数
	// nDataCounts * int ;					// 实时数据

	SMsgManagerRealTimeData_C(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  硬件服务器实时数据输出
#define MSG_MG_PCREALTIMEDATA		ENUM_MSG_MG_PCREALTIMEDATA

//  硬件服务器实时数据输出结构
struct SMsgManagerPcRealTimeData_C
{
	DWORD dwHardServerUID;					// 硬件服务器UID
	DWORD dwRTDataFlag;						// 有数据的位置标识,位标识
	BYTE nDataCounts;						// 数据个数
	// nDataCounts * int ;					// 实时数据

	SMsgManagerPcRealTimeData_C(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  服务器实时状态监听输出
#define MSG_MG_REALTIMEVISITOR		ENUM_MSG_MG_REALTIMEVISITOR

//  服务器实时状态监听输出结构
struct SMsgManagerRealTimeVisitor_C
{
	DWORD dwSoftServerUID;					// 软件服务器UID
	DWORD dwRTVisitorType;					// 软件服务器状态监听类型 EMManagerRealTimeVisitorType
	// 附加数据

	SMsgManagerRealTimeVisitor_C(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MG_RTVisitorType_IPCity 在线IP所有城市坐标信息

// 省市经纬度信息数据结构
struct SMsgManagerRTVisitorIPCityNode
{
	//DWORD dwTimes;		// (先只要实时数据,不发时间了) 时间,同一批数据一样,并且不同游戏区时间统一处理成一样的数字
	//DWORD dwWorldID;		// (消息中有了) 所在游戏世界ID
	WORD dwCityID;			// 所在省份+城市的ID,字典表ID 参考:CityLocation.csv
	WORD dwOnlines;			// 在线数
};
// 在线IP所有城市坐标信息 消息头
struct SMsgManagerRTVisitorIPCity_C
{
	WORD nDataCounts;						// 数据个数
	// nDataCounts * SMsgManagerRTVisitorIPCityNode;	// 在线IP所有城市实时在线数据
};



//////////////////////////////////////////////////////////////////////////
//  新加硬件服务器数据消息
#define MSG_MG_MAIN_ADDNEWPC		ENUM_MSG_MG_MAIN_ADDNEWPC

//  新加硬件服务器数据消息数据结构
struct SMsgManagerMainAddNewPcInfo_MZ
{
	// 以 网卡MAC地址和硬盘序号查询,如果存在就不添加,而是更新 最近活动时间,取到的电脑名称,IP地址,机器备注
	// 存入的数据
	DWORD dwID;										// 机器ID, 为0时新增，否则更新
	DWORD dwWorldID;								// 所属游戏世界ID 取 ManagerWorldList.dwID,为0表示是客户端或无效
	DWORD dwType;									// 机器类型,参考机器类型定义 
	DWORD dwTime;									// 加入时间
	DWORD dwLastTime;								// 最近活动时间
	DWORD dwFlag;									// 机器标识,0:无效,1:有效,2:新未登记的
	char szName[MANAGER_NAME_MAXSIZE];				// 机器名称
	char szPcName[MANAGER_NAME_MAXSIZE];			// 取到的电脑名称
	char szMACAddress[MANAGER_NAME_MAXSIZE];		// 网卡MAC地址
	char szDiskSN[MANAGER_NAME_MAXSIZE];			// 硬盘序号 Disk Serial Number 
	char szIPAddress[MANAGER_NAME_MAXSIZE];			// IP地址
	char szTitle[MANAGER_TEXTSTR_MAXSIZE];			// 机器备注 
	DWORD dwSpare1;									// 备用1
	DWORD dwSpare2;									// 备用2
	DWORD dwSpare3;									// 备用3

	SMsgManagerMainAddNewPcInfo_MZ(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
//  操作主服务器消息
#define MSG_MG_CONTROLMAINSERVER		ENUM_MSG_MG_CONTROLMAINSERVER

//  操作主服务器消息数据结构
struct SMsgManagerControlMainServer_CM
{
	DWORD dwUserID;							// 操作用户ID
	DWORD dwClientID;						// 客户端ID
	DWORD dwControlCode;					// 操作标识  EMManagerMainControlCode
	DWORD dwUserData;						// 用户数据
	// 附加数据

	SMsgManagerControlMainServer_CM(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
//  主服务器输出数据消息
#define MSG_MG_MAINSERVERDATA		ENUM_MSG_MG_MAINSERVERDATA

//  主服务器输出数据结构
struct SMsgManagerMainServerData_MC
{
	DWORD dwDataType;						// 输出数据类型  EMManagerMainDataType
	DWORD dwUserData;						// 用户数据
	DWORD dwUserData1;						// 用户数据1
	// 附加数据

	SMsgManagerMainServerData_MC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 发送用本地资源启服
#define MSG_MG_SENDLOCALRESOURCE   ENUM_MSG_MG_SENDLOCALRESOURCE            
struct SMsgManagerLocalResource_RM
{
    DWORD dwWorldID;                // 游戏世界ID

    SMsgManagerLocalResource_RM(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


//////////////////////////////////////////////////////////////////////////
//  文件管理消息
#define MSG_MG_FILEMSG				ENUM_MSG_MG_FILEMSG

//  文件管理消息数据结构
struct SMsgManagerFileMsg
{
	DWORD dwControlCode;			// 操作标识  EMManagerFileMsgType
	BYTE nServerType;				// 服务器类型
	DWORD dwServerData1;			// 服务器标识数据1
	DWORD dwServerData2;			// 服务器标识数据2
	DWORD dwServerData3;			// 服务器标识数据3
	// 附加数据

	SMsgManagerFileMsg(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MG_FileMsgType_UploadRequest			// 请求上传文件
struct SMsgManagerFileUploadRequest
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	DWORD dwFileDataLens;					// 文件内容长度
	WORD nFileNameLens;						// 文件名长度,包括路径
	//char szFileName[nFileNameLens];		// 文件名
};
// MG_FileMsgType_UploadResult			// 请求上传文件结果
struct SMsgManagerFileUploadResult
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	DWORD dwDownloadID;						// 下载ID,用来回传找对应数据对象
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
};
// MG_FileMsgType_FileData				// 文件数据
struct SMsgManagerSendFileData
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	DWORD dwDownloadID;						// 下载ID,用来回传找对应数据对象
	unsigned char szVerifyData[20];			// 校验数据
	DWORD dwFileDataLens;					// 文件内容长度
	DWORD dwFileEditTime;					// 文件修改时间
	//BYTE pData[dwFileDataLens];			// 文件内容
};
// MG_FileMsgType_FileDataResult		// 文件数据结果
struct SMsgManagerFileDataResult
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	DWORD dwDownloadID;						// 下载ID,用来回传找对应数据对象
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
};
// MG_FileMsgType_DownloadRequest		// 请求下载文件
struct SMsgManagerFileDownloadRequest
{
	DWORD dwDownloadID;						// 下载ID,用来回传找对应数据对象
	WORD nFileNameLens;						// 文件名长度,包括路径
	//char szFileName[nFileNameLens];		// 文件名
};
// MG_FileMsgType_DownloadResult		// 请求下载文件结果
struct SMsgManagerFileDownloadResult
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	DWORD dwDownloadID;						// 下载ID,用来回传找对应数据对象
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
};
// MG_FileMsgType_DirListRequest		// 请求列出目录信息
struct SMsgManagerFileDirListRequest
{
	WORD nPathNameLens;						// 目录名长度
	//char szPathName[nPathNameLens];		// 目录名
};
// MG_FileMsgType_DirListResult			// 目录信息结果
struct SMsgManagerFileDirListNode
{
	BYTE		nNodeType;					// 节点类型,0.文件,1.目录
	DWORD		dwSize;						// 文件大小
	DWORD		dwEditTime;					// 修改时间
	WORD		nFileNameLens;				// 文件名长度
	//char szFileName[nStrLens];			// 文件名

	SMsgManagerFileDirListNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgManagerFileDirListResult
{
	int	nCount;			// 个数
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
	// nCount * SMsgManagerFileDirListNode
	// nCount * char * szName
};

// MG_FileMsgType_BroadcastFile				// 请求主服务器广播文件到子服务器
struct SMsgManagerFileBroadcastRequest
{
	DWORD dwHardUID;						// 子服务器UID
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	WORD nSrcFileNameLens;					// 主服务器上源文件名长度,包括路径
	WORD nDestFileNameLens;					// 子服务器上目标文件名长度,包括路径
	//char szSrcFileName[nSrcFileNameLens];		// 源文件名
	//char szDestFileName[nDestFileNameLens];	// 目标文件名
};
// MG_FileMsgType_BroadcastFileResult		// 请求主服务器广播文件到子服务器结果
struct SMsgManagerFileBroadcastResult
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	DWORD dwDownloadID;						// 下载ID,用来回传找对应数据对象
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
};

// MG_FileMsgType_UnPackFile				// 请求子服务器解更新包
struct SMsgManagerFileUnPackRequest
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	WORD nPackFileNameLens;					// 主服务器上包源文件名长度,包括路径
	WORD nDestPathNameLens;					// 子服务器上目标路经名长度
	//char szPackFileName[nPackFileNameLens];	// 源文件名
	//char szDestPathName[nDestPathNameLens];	// 目标路经
};
// MG_FileMsgType_UnPackFileResult			// 请求子服务器解更新包结果
struct SMsgManagerFileUnPackResult
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
};

// MG_FileMsgType_VerifyPackFile,			// 请求子服务器校验更新包
struct SMsgManagerFileVerifyPackRequest
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	WORD nVerifyFileNameLens;				// 主服务器上包源文件名长度,包括路径
	WORD nDestPathNameLens;					// 子服务器上目标路经名长度
	//char szVerifyFileName[nVerifyFileNameLens];	// 源文件名
	//char szDestPathName[nDestPathNameLens];		// 目标路经
};
// MG_FileMsgType_VerifyPackFileResult,		// 请求子服务器校验更新包结果
struct SMsgManagerFileVerifyPackResult
{
	DWORD dwUploadID;						// 上传ID,用来回传找对应数据对象
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
};

// MG_FileMsgType_UpdateServer				// 请求服务器更新
struct SMsgManagerFileUpdateServerRequest
{
	DWORD dwSetupStatus;					// 安装更新步骤 EMWorldSetupUpdateStatus
	WORD nVerifyFileNameLens;				// 主服务器上包源文件名长度,包括路径
	WORD nDestPathNameLens;					// 子服务器上目标路经名长度
	//char szVerifyFileName[nVerifyFileNameLens];	// 源文件名
	//char szDestPathName[nDestPathNameLens];		// 目标路经
};
// MG_FileMsgType_UpdateServerResult		// 请求服务器更新结果
struct SMsgManagerFileUpdateServerResult
{
	DWORD dwSetupStatus;					// 安装更新步骤 EMWorldSetupUpdateStatus
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	DWORD dwUserData[5];					// 用户数据
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息
	
	SMsgManagerFileUpdateServerResult(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MG_FileMsgType_RemoveFile			// 请求子服务器删除文件
struct SMsgManagerFileRemoveRequest
{
	WORD nFileNameLens;						// 服务器上脚本文件名长度,包括路径
	//char szFileName[nFileNameLens];		// 源文件名
};

// MG_FileMsgType_RemoveFileResult		// 请求子服务器删除文件结果
struct SMsgManagerFileRemoveResult
{
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	WORD nMsgLens;							// 结果信息长度
	//char szMsg[nMsgLens];					// 结果信息

	SMsgManagerFileRemoveResult(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// MG_FileMsgType_RemoveLog,			// 请求子服务器清除Log文件
struct SMsgManagerFileRemoveLogRequest
{
	DWORD dwPreTimes;					// 清除多少秒以前的Log文件
};

// MG_FileMsgType_RemoveLogResult,		// 请求子服务器清除Log文件结果
struct SMsgManagerFileRemoveLogResult
{
	BYTE nFlag;								// 结果标识,0.失败，1.成功
	ULONGLONG dwTotalFileSize;				// 总文件内容数据长度
	ULONGLONG dwRemoveFileSize;				// 删除总文件内容数据长度
	DWORD dwRemoveOKCounts;					// 删除成功个数
	DWORD dwRemoveFailCounts;				// 删除失败个数
	DWORD dwRetPreTimes;					// 处理在些时间前的

	SMsgManagerFileRemoveLogResult(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//MG_MainData_UpdateSvnBaseInfo,		// 更新信息到客户端
struct SMsgManagerMainDataUpdateSvnBaseInfo
{
	bool bOpenSvn;				// 是否打开Svn
	bool bOpenUpload;			// 是否打开上传
	DWORD dwSuperUserID;		// 当前操作用户ID
	DWORD dwState;				// 版本整理管理服务器状态
	DWORD dwOldState;			// 版本整理管理服务器状态
	DWORD dwRunStatus;			// 版本整理进行状态
	DWORD dwSvnUserID;			// 当前可操作的用户
	int nWaitSvnCounts;			// 在等待时可上传总次数
	DWORD dwAdminUserID[8];		// 可授权操作的用户ID
	char szState[128];			// 当前状态文字
	char szTitle[256];			// 当前状态说明
	char szVerIDName[64];		// 公测版本号名称

	SMsgManagerMainDataUpdateSvnBaseInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//MG_MainData_UpdateSvnUserInfo,		// 更新用户信息到客户端
struct SMsgManagerMainDataUpdateSvnUserInfo
{
	DWORD dwUserID;				// 当前操作用户ID
	bool bHaveSvn;				// 有要上传的
	DWORD dwSvnCounts;			// 已提交了多少次
	bool bCanSvn;				// 可以上传了
	DWORD dwCanSvnCounts;		// 可用多少次

	SMsgManagerMainDataUpdateSvnUserInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


#pragma pack()


#endif	// __D_ManagerModule_H__