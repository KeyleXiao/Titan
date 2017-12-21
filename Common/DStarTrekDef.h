/*******************************************************************
** 文件名:	f:\Rocket20100601\Common\DStarTrekDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭智锋 （9671690@qq.com）
** 日  期:	2010/6/29 17:24
** 版  本:	1.0
** 描  述:	跨区数据宏定义头文件
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#pragma pack(1)
// tolua_begin

#define ST_LUAFUNCTION_MAXLEN				64

enum
{
	tStarTrek_Type_GoEctype = 0,			// 跨区下副本
	tStarTrek_Type_GoNormal = 0,			// 返回本区
	tStarTrek_Type_ToGeneral = 1,			// 跨普通区
};

enum
{
	tStarTrek_Flag_RunOnDie = 0x0001,		// 即使死亡也传	
	tStarTrek_Flag_RunOnEctype = 0x0002,	// 即使副本也传
	tStarTrek_Flag_RunOnTank = 0x0004,		// 即使载具也传
	tStarTrek_Flag_CannotVeto= 0x0008L,		// 不能否决的
};

// session key 的长度
#define DStarTrek_SessionKey_Len 256

// 客户端连接网关最大个数
#define DStarTrek_Gateway_Num 5

enum
{
	tStarTrek_Result_Succeed = 1,			// 成功
	tStarTrek_Result_ParameterError,		// 参数错误
	tStarTrek_Result_NoSupport,				// 不支持跨区
	tStarTrek_Result_DisconBridge,			// 与桥服务器断开
	tStarTrek_Result_StateError,			// 人物控制状态错误
	tStarTrek_Result_InEctype,				// 在副本里
	tStarTrek_Result_GameWorldError,		// 迁入游戏世界错误
	tStarTrek_Result_ZoneNoExist,			// 迁入场景不存在
	tStarTrek_Result_ModuleVeto,			// 其他模块否决
	tStarTrek_Result_ServerStopping,		// 服务器正在关机
	tStarTrek_Result_OutGameWorldError,		// 迁出游戏世界错误
	tStarTrek_Result_MysticError,			// 诡异的错误
	tStarTrek_Result_OverPersonMaxCount,	// 溢过在线上限
	tStarTrek_Result_EstopEnterZone,		// 不允许进入迁入场景
	tStarTrek_Result_RunLuaFunctionFail,	// 调用Lua函数失败
	tStarTrek_Result_InGameWorldUserLock,	// 迁入游戏世界已有角色
	tStarTrek_Result_ExecuteSPFail,			// 调用存储过程失败
	tStarTrek_Result_CloseStarTreak,		// 关闭了跨区传送
	tStarTrek_Result_PersonNoExist,			// 回迁时，角色不存在
	tStarTrek_Result_OverMemory,			// 内存已达上限
	tStarTrek_Result_NoMakeSTID,			// 无法生成STID
	tStarTrek_Result_STing,					// 正在跨区状态
	tStarTrek_Result_CannotForceExitTank,	// 无法拉下载具
	tStarTrek_Result_PublicDBPersonExist,	// 公共区游戏库内存在此角色
};

enum
{
	tStarTrek_MsgDir_In = 1,				// 流向迁入服务器
	tStarTrek_MsgDir_Out,					// 流向迁出服务器
};

enum
{
	tReleaseTreak_Reason_Succeed = 1,		// 成功
	tReleaseTreak_Reason_SwitchState,		// 切换状态
	tReleaseTreak_Reason_ConsultOverTime,	// 征询对方超时
	tReleaseTreak_Reason_ContextMisjudge,	// 现场对不上
	tReleaseTreak_Reason_ConsultRefuse,		// 征询回应为:拒绝
	tReleaseTreak_Reason_RunLuaFunctionFail,// 调用Lua函数失败
	tReleaseTreak_Reason_CanOutLuaVeto,		// 迁出场景服传送前的否决Lua函数
	tReleaseTreak_Reason_MysticError,		// 诡异的错误
	tReleaseTreak_Reason_ExportContextFail,	// 导出数据失败
	tReleaseTreak_Reason_NoInTheTrekState,	// 不在Trek状态
	tReleaseTreak_Reason_TrekStateMakeError,// TrekState出错	
	tReleaseTreak_Reason_ClientDisconnect,	// 客户端已掉线
	tReleaseTreak_Reason_DisconBridge,		// 与桥服务器断开
	tReleaseTreak_Reason_PostDataFail,		// postdata失败
	tReleaseTreak_Reason_LoginOverTime,		// 等待客户端登陆超时
	tReleaseTreak_Reason_SessionKeyError,	// SessionKey错误
	tReleaseTreak_Reason_ClientKickout,		// 客户端被踢掉
	tReleaseTreak_Reason_LeaveTrekLogin,	// 离开ECONNECT_STATE_TREKLOGIN
	tReleaseTreak_Reason_SessionKeyOverTime,// 等待SessionKey超时
	tReleaseTreak_Reason_ExportDataError,	// 导出数据超时
};

////////////////////////// 传送序号 ///////////////////////////////
// 游戏世界ID(12位)+场景服ID(8位)+时间(20)+流水号(24位)
typedef LONGLONG	STID;

///////////////////////////////////////////////////////////////////
// 跨区传送的上下文
struct SStarTrekContext
{	
	STID		stid;						// STID

	DWORD		PDBID;						// PDBID
	DWORD		UDBID;						// UDBID
	
	LONGLONG	uidPerson;					// uid
	DWORD		clientID;					// clientid

	int			nFromGameWorldID;			// 何个游戏世界角色

	int			nOutGameWorldID;			// 迁出世界ID
	DWORD		dwOutZoneID;				// 迁出场景ID
	int			nOutMapID;					// 迁出地图ID

	int			nInGameWorldID;				// 迁入世界ID
	DWORD		dwInBridgeServerID;			// 迁入服务器ID
	DWORD		dwInZoneID;					// 迁入场景ID
	POINT		ptInLoc;					// 迁入地图位置

	int			nStarTrekType;				// 传送类型
	DWORD		dwStarTrekFlag;				// 传送标志

	char		szIP[32];					// 传送者IP
	DWORD		dwNetworkType;				//add by lgf 12.02.27 网络类型

	// 迁出场景服传送前的否决Lua函数
	// 输入参数:
	//  1: pdbid
	//  2: InGameWorldID
	//  3: InZoneID
	//  4: nStarTrekType
	//  5: dwStarTrekFlag
	// 输出参数:
	//  1: 结果代码
	char		szCanOutLuaFunction[ST_LUAFUNCTION_MAXLEN];

	// 迁出场景服传送前的执行Lua函数
	// 输入参数:
	//  1: pdbid
	//  2: InGameWorldID
	//  3: InZoneID
	//  4: nStarTrekType
	//  5: dwStarTrekFlag
	char		szPreOutLuaFunction[ST_LUAFUNCTION_MAXLEN];

	// 迁出场景服传送失败的执行Lua函数
	// 输入参数:
	//  1: pdbid
	//  2: InGameWorldID
	//  3: InZoneID
	//  4: nStarTrekType
	//  5: dwStarTrekFlag
	//  6: 失败原因
	//  7: 扩展原因
	char		szFailOutLuaFunction[ST_LUAFUNCTION_MAXLEN];

	// 迁入场景服能否接收的否决Lua函数
	// 输入参数:
	//	1: pdbid
	//  2: 迁出世界ID
	//  3: 迁出地图ID
	//	4: 迁入场景ID
	//  5: 传送类型
	//  6: 传送标志
	// 输出参数:
	//  1: 结果代码
	char		szCanInLuaFunction[ST_LUAFUNCTION_MAXLEN];

	// 迁入场景服成功传送后的执行Lua函数
	// 输入参数:
	//	1: pdbid
	//  2: 迁出世界ID
	//  3: 迁出地图ID
	//	4: 迁入场景ID
	//  5: 传送类型
	//  6: 传送标志
	char		szDoneInLuaFunction[ST_LUAFUNCTION_MAXLEN];

	SStarTrekContext(void)
	{
		memset(this, 0, sizeof(*this));
	}

	void Clear(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 网关数据
struct SGatewayInfo 
{
	DWORD ip;
	WORD port;
};

// 登陆服务器暂存数据
struct SLoginServerContext
{	
	STID		stid;						// STID

	DWORD		PDBID;						// PDBID
	DWORD		UDBID;						// UDBID

	LONGLONG	uidPerson;					// uid
	DWORD		clientID;					// clientid
	DWORD		serverID;					// 服务器ID
	DWORD		gameWorldID;				// 玩家所属游戏世界ID

	int			nOutGameWorldID;			// 迁出世界ID
	DWORD		dwOutZoneID;				// 迁出场景ID
	int			nOutMapID;					// 迁出地图ID

	int			nInGameWorldID;				// 迁入世界ID
	DWORD		dwInBridgeServerID;			// 迁入服务器ID
	DWORD		dwInZoneID;					// 迁入场景ID
	POINT		ptInLoc;					// 迁入地图位置

	int			nStarTrekType;				// 传送类型
	DWORD		dwStarTrekFlag;				// 传送标志

	char		szIP[32];					// 传送者IP
	DWORD		dwNetworkType;				//add by lgf 12.02.27 网络类型
};

// tolua_end
#pragma pack()