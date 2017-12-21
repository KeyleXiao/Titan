/*******************************************************************
** 文件名:	VoiceDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音系统相关定义,主要是语音客户端,语音服务器等主关语音的系统的定义,不要包括游戏特定指定游戏应用,有的话定在VoiceModule中
** 应  用:  从VoiceModule分出,这里的定义不特定指定游戏应用,可所有端共用
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "Voice_ManagedDef.h"
#include "buffer.h"


// 打印调试信息,并设为测试模式
//#define VOICE_PRINT_DEBUG_INF

// 设为压力测试模式
//#define VOICE_BIG_TEST

//#define VOICE_USE_UDP_SEND	// 用UPD发缓冲机制不一样,不用缓冲太长

// 用Base库的UDP网络收发
#define VOICE_USE_BASEUDP

// 打开语音模块
#define VOICE_MODUEL_OPEN
#define VC_OS_WINDOWS

#ifndef SafeRelease
#	define SafeRelease(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif



#pragma pack(1)

/////// 公共宏定义 ///////////////////////////////////////////////////////////////////

// tolua_begin

#define  VOICE_INVALID_ID						0xFFFFFFFF		//DWORD 的最大数,0xFFFFFFFF
#define  VOICE_MAX_WORD_NUM						0xFFFF			//WORD 的最大数,0xFFFF

#define  VOICE_NAME_MAXSIZE						32				//名称最大长度,32
#define  VOICE_MSGSTR_MAXSIZE					64				//信息文字最大长度,64
#define  VOICE_TITLE_MAXSIZE					128				//标题字串的长度,128
#define  VOICE_LONGSTR_MAXSIZE					256				//长字串的长度,256
#define  VOICE_TEXTSTR_MAXSIZE					512				//大字串的长度,512

/// 重连周期10秒
#define  VOICE_RECONNECT_INTERVAL				(10*1000)


#define  VOICE_BROADCASTALL_NUM					0xFFFF			//WORD 的最大数,0xFFFF


#define  VOICE_KEEPALIVE_INTERVAL				2000			// 两次KeepAlive探测间的时间间隔
#define  VOICE_KEEPALIVE_TIMES					5000			// 开始首次KeepAlive探测前的TCP空闭时间
#define  VOICE_KEEPALIVE_TIMEOUT				3000			// 无响应时间
#define  VOICE_KEEPALIVE_MAXTIMEOUT				120000			// 最大无响应时间


#define  VOICE_MAX_GATEWAYID					64				// 因为语音ClientID中只用了8位表示网关ID,所以网关ID最大只能255位
#define  VOICE_MAX_CLIENT_COUNT					8192			// 一台网关服务器最多承载的人数 最多 8192 
#define  VOICE_MAX_ROOM_USERCOUNT				100000			// 房间最大人数

#define  VOICE_RC4_KEY_SIZE						64				// RC4密钥长度
#define  VOICE_RC4_MAX_SIZE						64				// RC4密钥最大长度

#define  VOICE_SERVICE_UPDATE_INFO_INTERVAL		1200000			// 更新全部游戏用户所有信息到语音服务器，20分钟同步一次
#define  VOICE_CHANNEL_OBSERVER_TIMEOUT			120000			// 频道观察者超时时间120秒
#define	 VOICE_ROOM_MIN_BUFF_SIZE				16				// 房间用户列表最小缓冲内存个数
#define	 VOICE_ROOM_MAX_MICORDER				16				// 麦序最大人数
#define  VOICE_ACTIVE_CHANNEL_TIME				30000			// 活跃频道时间
#define  VOICE_CALL_FANS_COOL_TIME				3600			// 召集粉丝冷却时间（秒） 一个小时
#define  VOICE_AD_COOL_TIME						3600			// 语音召集令冷却时间（秒） 一个小时

#define  VOICE_COOL_SAVE_TIMES					600000			// 数据保存冷却时间,毫秒(默认10分钟)
#define  VOICE_COOL_SAVE_COUNTS					100				// 请求保存数据冷却最大请求次数,大于此数就会保存

#define  VOICE_SAVE_DB_INTERVAL					600000			// 数据保存间隔时间,毫秒(默认10分钟)

#define  VOICE_MAX_DIAMOND_COUNT				500000			// 最大蓝钻数

#define  VOICE_CLIENT_AUDIONET_MAX				40				// 客户端语音视频流专用网络通道最大缓存包数,多于这个将丢弃新来的发包请求(默认40,大概20秒语音数据)
#define  VOICE_GATEWAY_AUDIONET_MAX				80				// 语音网关语音视频流专用网络通道最大缓存包数,多于这个将丢弃新来的发包请求(默认80,大概40秒语音数据)


// tolua_end


// 语音客户端ID信息
struct VClientIDInfo
{
	DWORD dwGatewayID : 8;  // 语音网关ID
	DWORD dwConnIndex :24;  // 语音连接序号

	VClientIDInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 频道
struct SVoiceChannelID
{
	DWORD	dwRoomID:8;		// 房间ID
	DWORD	dwType:8;		// 频道类型
	DWORD	dwWorldID:16;	// 世界ID
	DWORD	dwID:32;		// 频道ID
};

// 语音压缩参数结构
struct SVoiceCodecInfo
{
	// speex类型结构信息
	struct SpeexInfo
	{
		bool bVBREnabled;	// 是否是动态比特率
		BYTE nBand;			// (SPEEX)声音质量控制，0 - 8KHZ 最差，1 - 16KHZ 中等，2 - 32KHZ 最好
		BYTE nQuality;		// (SPEEX)声音质量控制，0 ~ 10， 0 最差，10 最好
	};
	// celt类型
	struct CELTInfo
	{
		bool bVBREnabled;	// 是否是动态比特率	
		UINT nSampleRate;	// (CELT压缩动)声音质量控制 32000 ~ 96000，从低到高 默认48000
		UINT nVBRTargetBitrate;//(CELT压缩动),输出的比特率,如128000 默认32000
	};

	union
	{
		SpeexInfo speex;
		CELTInfo celt;
	} Info;


	BYTE nType;			// 语音编码类型 EMVoiceCodecType
	BYTE nChannelCount;	// 支持的音频通道数,立体声为2,单声道为1
};


// tolua_begin

/////// 公共枚举定义 ///////////////////////////////////////////////////////////////////

// 服务器跨线程执行操作代码定义
enum EMVoiceServerControlCode
{
	VS_ControlCode_None = 0,		// 无操作
	VS_ControlCode_StartService,	// 启动服务
	VS_ControlCode_StopService,		// 停止服务
	VS_ControlCode_ShutDown,		// 关闭程序,停止服务后要关闭进程
	// 最大
	VS_ControlCode_Max
};

// 服务器列表更新类型定义
enum EMVoiceServerListUpdateType
{
	VS_SvrListUpdateType_All = 0,		// 全部
	VS_SvrListUpdateType_Add,			// 新增
	VS_SvrListUpdateType_Edit,			// 修改
	VS_SvrListUpdateType_Del,			// 删除
	// 最大
	VS_SvrListUpdateType_Max
};

// 控制命令
enum EMVoiceCommand
{
	VoiceCommand_ExportUserList = 1,	// 导出用户列表
	VoiceCommand_ChangeMaxConnection,	// 修改最大连接数
	VoiceCommand_ExportChannelList,		// 导出频道列表
	VoiceCommand_NetSpeedMax,			// 设置最大网络带宽(Mbps)

	VoiceCommand_Max
};

// 数据转发时使用的通道类型
enum EMVoiceSendChannelType
{
	VoiceSendChannel_Control = 1,		// 控制数据通道
	VoiceSendChannel_Voice,				// 语音数据通道
};


// 发送语音召集的广播类型
enum EMVoiceADBroadcastType
{
	VoiceBroadcast_None			= 0x0,		// 无效
	VoiceBroadcast_SelfWorld	= 0x1,		// 广播自己区
	VoiceBroadcast_AllWorld		= 0x2,		// 广播所有区
	VoiceBroadcast_ExceptSelfWorld	= 0x4,	// 广播除自己区
};

// 发送语音视频流包的位标识,8位
enum EMVoiceStreamDataFlag
{
	VoiceStreamData_None			= 0x0,		// 无效
	VoiceStreamData_Audio			= 0x1,		// 有后续语音包,否则是结束包
	VoiceStreamData_Video			= 0x2,		// 包含视频数据,要用不同的组包方式和拆包方式,以在没有视频时不带视频信息以节省点流量
	VoiceStreamData_KeyVideo		= 0x4,		// 表示当前帧是关键帧
	//VoiceStreamData_NoUse			= 0x8,		// 
};

// 所有权限
#define VOICE_USER_RIGHT_ALL		0xFFFFFFFF
// OW，频道拥有者权限
#define VOICE_USER_RIGHT_OW			(VoiceRight_Owner|VoiceRight_Ban|VoiceRight_Kick|VoiceRight_Move|VoiceRight_Broadcast|VoiceRight_AD|VoiceRight_Mic|VoiceRight_Lock|VoiceRight_Speak|VoiceRight_Vest)
// admin，管理员权限
#define VOICE_USER_RIGHT_ADMIN		(VoiceRight_Kick|VoiceRight_Move|VoiceRight_AD|VoiceRight_Mic|VoiceRight_Lock|VoiceRight_Speak|VoiceRight_Vest|VoiceRight_Ban)
// Host，主持人权限
#define VOICE_USER_RIGHT_HOST		(VoiceRight_Kick|VoiceRight_Move|VoiceRight_AD|VoiceRight_Mic|VoiceRight_Speak|VoiceRight_Ban)	
// VIP，嘉宾权限
#define VOICE_USER_RIGHT_VIP		(VoiceRight_Speak)

// 频道运行状态标记
enum EMVoiceChannelRunFlag
{
    VoiceChannelRunFlag_Saving			= 0x1,	///  频道销毁时数据正在保存数据库
    //VoiceChannelRunFlag_Unuse		= 0x2,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x4,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x8,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x10,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x20,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x40,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x80,	/// 
    //VoiceChannelRunFlag_Unuse		= 0x100, ///
    //VoiceChannelRunFlag_Unuse		= 0x200, ///
    //VoiceChannelRunFlag_Unuse		= 0x400, ///
    //VoiceChannelRunFlag_Unuse		= 0x800, /// 
    //VoiceChannelRunFlag_Unuse  		= 0x1000, /// 
    //VoiceChannelRunFlag_Unuse		= 0x2000, /// 
};


// 频道应用类型
enum EMVoiceChannelUseType
{
	VoiceChannelUseType_None = 0,	// 默认


	VoiceChannelUseType_Max,
};



// 房间基本信息 网络传输用
enum EMVoiceRoomBaseInfo
{
	VoiceRoomBaseInfo_Lock0			= 0x1,	/// 0号房间加锁
	VoiceRoomBaseInfo_Lock1			= 0x2,	/// 1号房间加锁
	VoiceRoomBaseInfo_Lock2			= 0x4,	/// 2号房间加锁
	VoiceRoomBaseInfo_Lock3			= 0x8,	/// 3号房间加锁
	VoiceRoomBaseInfo_Lock4			= 0x10,	/// 4号房间加锁
	VoiceRoomBaseInfo_Lock5			= 0x20,	/// 5号房间加锁
	VoiceRoomBaseInfo_Lock6			= 0x40,	/// 6号房间加锁
	VoiceRoomBaseInfo_Lock7			= 0x80,	/// 7号房间加锁 VoiceRoomBaseInfo_Lock数量必须要和VoiceRoomType_Max保持一致
	//VoiceRoomState_Unuse			= 0x100, ///
	//VoiceRoomState_Unuse			= 0x200, ///
	//VoiceRoomState_Unuse			= 0x400, ///
	//VoiceRoomState_Unuse			= 0x800, /// 
	//VoiceRoomState_Unuse  		= 0x1000, /// 
	//VoiceRoomState_Unuse			= 0x2000, /// 
};

// 排行榜状态
enum EMVoicePersonalTopState
{
	VoicePersonalTopState_Voice			= 0x1,	/// 正在语音
	VoicePersonalTopState_Video			= 0x2,	/// 正在视频
	//VoicePersonalTopState_Unuse			= 0x4,	/// 
	//VoicePersonalTopState_Unuse			= 0x8,	/// 
	//VoicePersonalTopState_Unuse			= 0x10,	/// 
	//VoicePersonalTopState_Unuse			= 0x20,	/// 
	//VoicePersonalTopState_Unuse			= 0x40,	/// 
	//VoicePersonalTopState_Unuse			= 0x80,	/// 
	//VoicePersonalTopState_Unuse			= 0x100, ///
	//VoicePersonalTopState_Unuse			= 0x200, ///
	//VoicePersonalTopState_Unuse			= 0x400, ///
	//VoicePersonalTopState_Unuse			= 0x800, /// 
	//VoicePersonalTopState_Unuse  		= 0x1000, /// 
	//VoicePersonalTopState_Unuse			= 0x2000, ///
};

// 语音模块列表更新类型
enum EMVoiceUpdateType
{
	VoiceUpdateType_All = 0,		// 同步全部
	VoiceUpdateType_Add,			// 新增 
	VoiceUpdateType_Edit,			// 修改
	VoiceUpdateType_Delete,			// 删除
};

// 语音服务器更新用户信息到客户端
enum EMVoiceUpdateUserType
{
	VoiceUpdateUserType_All = 0,		// 全部
	VoiceUpdateUserType_Base,			// 更新基础信息
	VoiceUpdateUserType_Type,			// 更新指定类的频道,权限
	VoiceUpdateUserType_AllChannel,		// 全部频道信息
	VoiceUpdateUserType_AllRight,		// 更新权限
	VoiceUpdateUserType_AllType,		// 更新类型
};

// 语音服务器更新房间信息到客户端
enum EMVoiceUpdateRoomType
{
	VoiceUpdateRoomType_All = 0,		// 全部
	VoiceUpdateRoomType_Base,			// 基础信息
	VoiceUpdateRoomType_MicUser,		// 麦上的人信息
	VoiceUpdateRoomType_Remark,			// 更新公告
};

// 语音服务器更新频道信息到客户端
enum EMVoiceUpdateChannelType
{
	VoiceUpdateChannelType_All = 0,		// 频道全部信息，以后扩展用
	VoiceUpdateChannelType_Base,		// 频道基础信息
	VoiceUpdateChannelType_Audio,		// 频道里房间是否有人语音信息
	VoiceUpdateChannelType_Name,		// 频道名称
	VoiceUpdateChannelType_RoomName,	// 频道房间名称
};

// 语音服务器更新频道信息到客户端
enum EMVoiceUpdateAllChannelType
{
	VoiceUpdateAllChannelType_All = 0,			// 所有频道的全部信息，以后扩展用
	VoiceUpdateAllChannelType_Audio,			// 所有频道是否有人语音信息
};


// 更新一个成员的部分信息到客户端
enum EMVoiceUpdateUserPartInfoType
{
	VoiceUpdateUserPartInfoType_StateFlag = 0,	// 用户状态标记
	VoiceUpdateUserPartInfoType_FansCount,		// 粉丝数量
};


// 用户踢出原因
enum EMVoiceKickReason
{
	VoiceKickReason_Default = 0,		// 默认
	VoiceKickReason_Full,				// 人数满了
	VoiceKickReason_LoginTimeout,		// 登录超时
	VoiceKickReason_LoginError,			// 登录失败
	VoiceKickReason_UserExist,			// 用户存在

	VoiceKickReason_Max,
};

// 移动麦序操作
enum EMVoiceMoveMicOrderType
{
	VoiceMoveMicOrder_Up,		// 上移麦序
	VoiceMoveMicOrder_Down,		// 下移麦序
	VoiceMoveMicOrder_Jump2,	// 跳到2号麦序
};


// 私聊设置
enum EMVoiceChatConfig
{
	VoiceChatConfig_Defualt,	// 默认设置，接受任何人的私聊
	VoiceChatConfig_Ban,		// 不接受任何私聊
	VoiceChatConfig_BanStranger,// 不接受陌生人私聊
};

// 私聊用户类型
enum EMVoiceChatUserType
{
	VoiceChatUserType_None,		// 无效类型
	VoiceChatUserType_Inviter,	// 邀请者
	VoiceChatUserType_Acceptor,	// 接受者
	VoiceChatUserType_Visitor,	// 旁听者，主要是指GM

	VoiceChatUserType_Max,
};


/// 系统信息（含聊天信息）位置定义，借用聊天消息定义，为了不冲突，复制一份
enum EMVoiceInfoPos
{
	VoiceInfoPos_ChatWindow		= 0x1,	/// 聊天框
	VoiceInfoPos_ScreenTopAD	= 0x2,	/// 置顶广告栏
	VoiceInfoPos_ScreenTopLarge	= 0x4,	/// 置顶大字信息
	VoiceInfoPos_ActorAbove		= 0x8,	/// 角色上方
	VoiceInfoPos_ActorUnder		= 0x10,	/// 角色下方
	VoiceInfoPos_ActorRight		= 0x20,	/// 角色右方
	VoiceInfoPos_ScreenRight	= 0x40,	/// 屏幕右方
	VoiceInfoPos_MouseInfo		= 0x80,	/// 鼠标跟踪信息
	VoiceInfoPos_WarTips		= 0x100, ///战斗提示信息
	VoiceInfoPos_ScreenCenter	= 0x200, ///屏幕中上方活动条提示信息
	VoiceInfoPos_Rosponse		= 0x400, ///即时提示信息
	VoiceInfoPos_ActorUnderLarge= 0x800, /// 角色下方大字信息
	VoiceInfoPos_ScreenAnim1  	= 0x1000, /// 屏幕动画字体信息
	VoiceInfoPos_ScreenAnim2	= 0x2000, /// 屏幕动画字体信息

	VoiceMaxInfoPosCount		= 0x3FFF,	/// 最大位置数
};


/// 系统消息，借用聊天消息定义，为了不冲突，复制一份
enum EMVoiceTipType
{
	VoiceTipType_Nothing = 0,		/// 无定义0
	VoiceTipType_System,			/// 系统提示1
	VoiceTipType_Operate,			/// 操作提示2
	VoiceTipType_War,				/// 战斗、PK提示3
	VoiceTipType_Task,				/// 任务提示4
	VoiceTipType_Error,				/// 错误提示5
	VoiceTipType_Warning,			/// 警告提示6
	VoiceTipType_Boss,				/// boss信息提示7
	VoiceTipType_SystemTips,		/// 系统、官方公告8
	VoiceTipType_Equip,				/// 装备相关9
	VoiceTipType_Conscribe,			/// 战士招募10
	VoiceTipType_Weal,				/// 官员福利11
	VoiceTipType_Auction,			/// 拍卖信息12

	VoiceMaxTipTypeCount,			///最大种类数
};


/////// 客户端公共枚举定义/////////////////////////////////////////////



// 语音编码类型
enum EMVoiceCodecType
{
	VoiceCodeType_Speex = 0,	// speex类型
	VoiceCodeType_Celt,			// Celt类型
	VoiceCodeType_Max,			// 最大数量
};


/////// 服务器公共枚举定义/////////////////////////////////////////////

// 语音服务器用户状态类型
enum EMVoiceUserState
{
	VoiceUserState_None = 0,	// 无效
	VoiceUserState_Offline,		// 不在线
	VoiceUserState_Online,		// 在线未登录
	VoiceUserState_Login,		// 在线已登录

	VoiceUserState_Max,
};

///// 公用数据结构定义 //////////////////////////////////////////////

// 通过跨区语音服务器中转消息必须加入此消息体
struct SMsgVoiceHead
{
	DWORD dwWorldID;		// 消息来源游戏世界ID
	DWORD dwServerID;		// 消息来源服务器ID
};

// 游戏服务器信息数据结构
struct SVoiceGameServerInfo
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID;			// 服务器所在游戏世界ID
	WORD	wServerType;		// 服务器类型
	WORD	wSubID;				// 第n号服务器
	DWORD	dwStateFlag;		// 服务器状态
	void*	pExtPart;			// 连接器扩展部件接口

	SVoiceGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 自由麦结构体
struct SVoiceCountryFreeMic
{
	char	szNickname[VOICE_NAME_MAXSIZE];		// 昵称
	DWORD	dwPDBID;
	BYTE	nCountryID;			// 国家ID

	SVoiceCountryFreeMic(void)
	{
		memset( this, 0, sizeof(SVoiceCountryFreeMic) );
	}
};

// 频道配置结构
struct SVoiceChannelConfig
{
	DWORD	dwChannelType;			// 频道类型   EMVoiceChannelType
	bool	bOpen;					// 是否开放
	bool	bUserList;				// 是否支持成员列表
	bool	bEnterFree;				// 是否支持可任意进入此类型的频道，如个人频道
	bool	bMicOrder;				// 是否支持麦序模式
	bool	bMicFree;				// 是否支持自由模式
	WORD	wCallMinLevel;			// 召集需要的最小等级，0为不限制
	DWORD	dwChatCoolDown;			// 文字聊天冷却时间，秒
	DWORD	dwVestCount;			// 临时管理个数
	bool	bPush;					// 频道推送
	DWORD	dwMaxCount;				// 最大人数
	DWORD	dwVoiceQuality;			// 语音品质 EMVoiceQualityType
	DWORD	dwADCoolTime;			// 语音召集令冷却时间
};

// 房间配置结构
struct SVoiceRoomConfig
{
	DWORD	dwMicState;				// 默认麦序模式
	DWORD	dwMicTimeLen;			// 麦序模式下，时长秒
	DWORD	dwMicInvalidTime;		// 麦序模式下，多久不说话，自动下麦
	BYTE	nPush;					// 房间推送
};



// 语音服务器上客户端登录信息
struct SVoiceClientLoginInfo
{
	DWORD	dwConnectTime;	// 连入时间
	DWORD	dwRemoteIP;		// 该客户端IP

	SVoiceClientLoginInfo(void)
	{
		memset( this, 0, sizeof(SVoiceClientLoginInfo) );
	}
};

// 语音频道基本信息
struct SVoiceChannelBaseInfo
{
	DWORD dwStateFlag;		// 状态标记 EMVoiceChannelStateFlag
	DWORD dwChatCoolDown;	// 文字聊天冷却时间，秒
	DWORD dwOwnerUDBID;		// 频道拥有者UDBID
	DWORD dwQuality;		// 语音品质  EMVoiceQualityType
	DWORD dwDefautRoomID;	// 默认进入的房间ID
	DWORD dwUseType;		// 应用类型
	DWORD dwSpare1;			// 备用1
	DWORD dwSpare2;			// 备用2
	DWORD dwSpare3;			// 备用3
	DWORD dwOwnerType;		// 频道拥有者帐号类型

	SVoiceChannelBaseInfo(void)
	{
		memset( this, 0, sizeof(SVoiceChannelBaseInfo) );
	}
};

// 语音房间基本信息
struct SVoiceRoomBaseInfo
{
	BYTE	nMicState;			// 麦类型 EMVoiceMicState
	DWORD	dwStateFlag;		// 房间状态标记 EMVoiceRoomStateFlag
	DWORD	dwUserCount;		// 房间人数，参考值
	DWORD	dwChatCoolDown;		// 文字聊天冷却时间
	DWORD	dwMicOrderTime;		// 默认麦序时间

	SVoiceRoomBaseInfo(void)
	{
		memset( this, 0, sizeof(SVoiceRoomBaseInfo) );
	}
};

// 语音私聊信息
struct SVoiceChatInfo
{
	bool bMicOpen;		// 是否开麦
	DWORD dwChatID;		// 私聊对象ID
	DWORD dwUserType;	// EMVoiceChatUserType
	DWORD dwLastAcceptTime;	// 上次接受私聊邀请的时间

	SVoiceChatInfo(void)
	{
		memset( this, 0, sizeof(SVoiceChatInfo) );
	}
};


///// 客户端公用数据结构定义 //////////////////////////////////////////////


// 房间麦序结点
struct SVoiceRoomMicData
{
	DWORD		dwClientID;		/// 客户端ID
	DWORD		dwIndex;		/// 麦序

	SVoiceRoomMicData(void)
	{
		memset( this, 0, sizeof(SVoiceRoomMicData) );
	}
};

// 房间麦说话结点
struct SVoiceRoomVoiceData
{
	DWORD		dwClientID;		/// 客户端ID
	DWORD		dwTime;			/// 开始说话时间

	SVoiceRoomVoiceData(void)
	{
		memset( this, 0, sizeof(SVoiceRoomVoiceData) );
	}
};

// 房间麦说话结点结果
struct SVoiceRoomVoiceDataResult
{
	DWORD		dwClientID;		/// 客户端ID
	bool		bTalking;		/// 是否在说话

	SVoiceRoomVoiceDataResult(void)
	{
		memset( this, 0, sizeof(SVoiceRoomVoiceDataResult) );
	}
};

// 频道里临时管理
struct SVoiceVestData
{
	DWORD dwUDBID;						// dwUDBID
	BYTE nSex;
	BYTE nNewUserType;					// 用户类型	
	char szName[VOICE_NAME_MAXSIZE];	// 名称
};

// 当前在麦序上的人
struct SVoiceCurMicUserInfo
{
	DWORD	dwMicClientID;	// 当前在麦上的人
	DWORD	dwMicEndTime;	// 麦结束时间
	DWORD	dwFansCount;	// 粉丝数量
	bool	bMicFans;		// 是麦上用户的粉丝
	DWORD	dwWorldID;		// 麦上用户所在世界ID

	SVoiceCurMicUserInfo(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};

// 世界数据
struct SVoiceWorldData
{
	DWORD dwWorldID;		// 世界ID
	char szWorldName[VOICE_NAME_MAXSIZE];	// 世界名字

	SVoiceWorldData(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};

// 个人频道排行榜数据
struct SVoicePersonalTopData
{
	DWORD dwWorldID;		// 世界ID
	DWORD dwCountryID;		// 国家ID
	DWORD dwUDBID;			// 通行证ID
	char szOwnerName[VOICE_NAME_MAXSIZE];	// 名称
	DWORD dwUserCount;		// 在线人数
	DWORD dwMaxMemberCount;	// 历史最高在线人数
	BYTE nState;			// 状态
	DWORD dwOwnerType;		// 频道拥有者帐号类型

	SVoicePersonalTopData(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};


// 频道在线排序数据
struct SVoiceChannelSortInfo
{
	LONGLONG nChannelID;	// 频道ID
	DWORD dwUserCount;		// 在线人数
	DWORD dwUserData;		// 用户数据
};

// 收到粉丝召集的信息
struct SVoiceCallFansData
{
	DWORD dwTime;				// 邀请时间
	DWORD dwClientID;			// 客户端ID
	DWORD dwChannelType;
	DWORD dwRoomID;
	DWORD dwWorldID;
	DWORD dwID;
	BYTE nSex;					// 性别
	BYTE nCountryID;			// 国家ID
	DWORD dwFromWorldID;			// 世界ID
	DWORD dwUDBID;				// UDBID
	char szNickname[VOICE_NAME_MAXSIZE];		// 昵称
	char szMsg[VOICE_TEXTSTR_MAXSIZE];	// 文本信息
	BYTE nParam1;			// 备用数据1
	BYTE nParam2;			// 备用数据2

};

// 请求保存数据冷却信息数据结构
struct SVoiceSaveCoolInfo
{
	int nCounts;						// 冷却时的保存个数
	DWORD dwLastTimes;					// 最后保存时间
	bool bOnTimer;						// 是否是定时调用,是就不作为新请求，并把有排队的请求放行

	SVoiceSaveCoolInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 语音粉丝排行榜数据
struct SVoiceFansTopListData
{
	DWORD dwUDBID;									// 通行证UDBID
	DWORD dwUserType;								// 通行证帐号类型
	DWORD dwRank;									// 名次
	DWORD dwFansCount;								// 粉丝总数
	DWORD dwPDBID;									// 最近登录的角色ID
	DWORD dwFromWorldID;							// 最近登录的角色来源世界ID
	DWORD dwCountryID;								// 最近登录的角色所属ID(国家，大陆。。)
	char szActorName[VOICE_NAME_MAXSIZE];			// 最近登录的角色角色名

	SVoiceFansTopListData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 蓝钻信息
struct SVoiceDiamondInfo
{
	DWORD dwUDBID;				// 通行证UDBID,唯一
	DWORD dwBlueDiamond;		// 蓝钻总数，一直累加
	DWORD dwCurBlueDiamond;		// 当前蓝钻数，只有这个数值才可结算成红钻
	DWORD dwBlueDiamondOld;		// 当月之前的总数
	DWORD dwBlueDiamondToday;	// 今日获得蓝钻数，第二天自动把今天的这个值添加到dwCurBlueCount中，然后清零

	DWORD dwRedDiamond;			// 红钻总数，一直累加，这部分可结算成金币/RMB

	SVoiceDiamondInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


///////////////////////////////////////////////////////////////////
// 事件码枚举定义: ENUM_EVENT_VOICE + 消息码名
enum EMEventVoice
{
	ENUM_EVENT_VOICE_INVALID = 0,
	ENUM_EVENT_VOICE_HANDSHAKE,					// 与语音服务器握手成功事件
	ENUM_EVENT_VOICE_SERVERLIST_ALL,			// 全部服务器加入事件
	ENUM_EVENT_VOICE_SERVERLIST_ADD,			// 新增服务器事件
	ENUM_EVENT_VOICE_SERVERLIST_DEL,			// 删除服务器事件
	ENUM_EVENT_VOICE_WORLDLIST_ALL,				// 全部游戏世界加入事件
	ENUM_EVENT_VOICE_WORLDLIST_ADD,				// 新增游戏世界事件
	ENUM_EVENT_VOICE_WORLDLIST_EDIT,			// 修改游戏世界事件

	// 客户端使用
	ENUM_EVENT_VOICE_CLIENT_INITSPEEX,			// 异步操作压缩器 采样率线程
	ENUM_EVENT_VOICE_CLIENT_CONNECTSERVER,		// 请求连服务器
	ENUM_EVENT_VOICE_CLIENT_DISCONNECT,			// 与服务器断开连接
	ENUM_EVENT_VOICE_CLIENT_CONNECTED,			// 已连接服务器
	ENUM_EVENT_VOICE_CLIENT_RECONNECT,			// 重新连接服务器
	ENUM_EVENT_VOICE_CLIENT_CHATMSG,			// 聊天

	ENUM_EVENT_VOICE_CLIENT_USERINFOUPDATE,		// 用户信息更新，包含用户的频道基本信息，拥有权限
	ENUM_EVENT_VOICE_CLIENT_ROOMINFOUPDATE,		// 房间信息更新
	ENUM_EVENT_VOICE_CLIENT_ISSPEAKING,	        // 语音用户是否在说话
	ENUM_EVENT_VOICE_CLIENT_ENTERROOMRESULT,	// 进入房间结果
	ENUM_EVENT_VOICE_CLIENT_CHANNELINFOUPDATE,	// 频道信息更新
	ENUM_EVENT_VOICE_CLIENT_SELFSPEAKING,		// 自己说话的更新

	ENUM_EVENT_VOICE_CLIENT_NEEDNEWIP,			// 需要重选IP
	ENUM_EVENT_VOICE_CLIENT_TEXTCHATMESSAGE,	// 文本聊天

	ENUM_EVENT_VOICE_CLIENT_VOICELIST,			// 语音列表
	ENUM_EVENT_VOICE_CLIENT_CHATLIST,			// 私聊列表

	ENUM_EVENT_VOICE_CLIENT_ALLCHANNELINFOUPDATE,	// 全部频道信息更新
	ENUM_EVENT_VOICE_CLIENT_FIRSTSPEAKING,		// 第一次频道有人说话事件
	ENUM_EVENT_VOICE_CLIENT_VESTLIST,			// 马甲（临时管理）列表更新
	ENUM_EVENT_VOICE_CLIENT_PERSONALTOPLIST,	// 个人频道排行榜
	ENUM_EVENT_VOICE_CHAT,						// 私聊
	ENUM_EVENT_VOICE_CLIENT_CHANNELAUDIO,		// 频道语音推送
	ENUM_EVENT_VOICE_CLIENT_CALLFANS,			// 召集粉丝
	ENUM_EVENT_VOICE_CLIENT_GIFTSENDINFO,		// 送礼信息
	ENUM_EVENT_VOICE_CLIENT_DIAMONDINFO,		// 蓝钻信息
	ENUM_EVENT_VOICE_CLIENT_FANSTOPLIST,		// 粉丝排行榜
	ENUM_EVENT_VOICE_CLIENT_ADMINENTERINFO,		// 管理员进入信息
	ENUM_EVENT_VOICE_CLIENT_AD,					// 语音召集令
	ENUM_EVENT_VOICE_CLIENT_VIDEOFRAME,			// 视频数据
	ENUM_EVENT_VOICE_CLIENT_VIDEOSHOW,			// 显示视频事件,如有新视频源,等待视频中,视频中断
	ENUM_EVENT_VOICE_CLIENT_VIDEO,				// 视频事件

	ENUM_EVENT_VOICE_MAXID,						// 最大事件码
};

// 最大事件码
#define EVENT_VOICE_MAXID						ENUM_EVENT_VOICE_MAXID




//////////////////////////////////////////////////////////////////////////

// 异步操作压缩器 采样率线程
enum EMSubEventInitSpeex
{
	EventInitSpeex_Nothing	= 0,	// 无作为
	EventInitSpeex_Init,			// 创建解压缩器 采样率
	EventInitSpeex_UnInit,			// 释放解压缩器 采样率
};

// 通道事件枚举
enum EMVoiceAudioEvent
{
	VoiceAudioEvent_NetConnect = 0,		// 连接网络
	VoiceAudioEvent_NetClose,			// 断开网络
	VoiceAudioEvent_AddVoiceUser,		// 添加语音用户
	VoiceAudioEvent_AddToBlackList,		// 添加到黑名单
	VoiceAudioEvent_FireProviderEvent,	// 抛出表现层事件

	VoiceAudioEvent_OnNetConnected = 0,		// 连接网络
	VoiceAudioEvent_OnNetClose,				// 断开网络
};


// 语音列表子事件
enum EMSubEventVoiceList
{
	EventVoiceList_Clear,		// 清空正在语音人员列表
	EventVoiceList_Add,			// 添加正在语音人员
	EventVoiceList_Flash,		// 刷新列表
};

// 语音私聊列表
enum EMSubEventChatList
{
	EventChatList_Clear,		// 清空私聊列表
	EventChatList_Add,			// 添加私聊
	EventChatList_Flash,		// 刷新列表
};

// 召集粉丝事件
enum EMSubEventCallFans
{
	EventCallFans_Call,				// 收到粉丝召集
	EventCallFans_OnlineCount,		// 粉丝在线数回应
	EventCallFans_Success,			// 召集粉丝操作成功
	EventCallFans_Fail,				// 召集粉丝操作失败
	EventCallFans_CallResult,		// 请求召集粉丝结果
	EventCallFans_Flash,			// 请求刷新界面
	EventCallFans_UpdateTime,		// 更新上次邀请粉丝时间
};

// 私聊子事件
enum EMSubEventVoiceChat
{	
	EventVoiceChat_Invite,			// 接收到私聊邀请
	EventVoiceChat_InviteSuccess,	// 发送私聊邀请成功
	EventVoiceChat_RunSuccess,		// 私聊成功进行
	EventVoiceChat_End,				// 私聊结束
	EventVoiceChat_OpenMic,			// 私聊开麦
};

// 显示视频事件,如有新视频源,等待视频中,视频中断 子事件 dwData1
enum EMSubEventVoiceVideoShow
{	
	EventVoiceVideoShow_NewVideo,		// 有新的视频	,dwData2:通道Id,dwData3:dwClientID
	EventVoiceVideoShow_EndVideo,		// 视频信号中断	,dwData2:通道Id,dwData3:dwClientID
	EventVoiceVideoShow_KeyFrame,		// 视频第一次收到关键帧	,dwData2:通道Id,dwData3:dwClientID
};

// 视频事件
enum EMSubEventVoiceVideo
{	
	EventVoiceVideo_Device,			// 视频引擎设备相关
	EventVoiceVideo_Camera,			// 采集视频摄像头输入,dwData2: EMVideoCameraControlCode
};




// tolua_end










// 与语音服务器握手成功事件
#define EVENT_VOICE_HANDSHAKE				ENUM_EVENT_VOICE_HANDSHAKE
// 事件数据结构
struct SEventVoiceHandshake
{
	DWORD	dwServerID;				// 服务器ID

	SEventVoiceHandshake(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 全部服务器加入事件
#define EVENT_VOICE_SERVERLIST_ALL				ENUM_EVENT_VOICE_SERVERLIST_ALL
// 事件数据结构
struct SEventVoiceServerListAll
{
	DWORD	dwServerCounts;				// 收到服务器个数

	SEventVoiceServerListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 新增服务器事件
#define EVENT_VOICE_SERVERLIST_ADD				ENUM_EVENT_VOICE_SERVERLIST_ADD
// 事件数据结构
struct SEventVoiceServerListAdd
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID;			// 服务器所在游戏世界ID
	WORD	wServerType;		// 服务器类型
	WORD	wSubID;				// 第n号服务器
	DWORD	dwStateFlag;		// 服务器状态

	SEventVoiceServerListAdd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 删除服务器事件
#define EVENT_VOICE_SERVERLIST_DEL				ENUM_EVENT_VOICE_SERVERLIST_DEL
// 事件数据结构
struct SEventVoiceServerListDel
{
	DWORD dwServerID;				// 服务器ID

	SEventVoiceServerListDel(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 全部游戏世界加入事件
#define EVENT_VOICE_WORLDLIST_ALL				ENUM_EVENT_VOICE_WORLDLIST_ALL
// 事件数据结构
struct SEventVoiceWorldListAll
{
	DWORD	dwWorldCounts;				// 收到游戏世界个数

	SEventVoiceWorldListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 新增游戏世界事件
#define EVENT_VOICE_WORLDLIST_ADD				ENUM_EVENT_VOICE_WORLDLIST_ADD
// 事件数据结构
struct SEventVoiceWorldListAdd
{
	DWORD	dwWorldID;			// 游戏世界ID

	SEventVoiceWorldListAdd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 修改游戏世界事件
#define EVENT_VOICE_WORLDLIST_EDIT				ENUM_EVENT_VOICE_WORLDLIST_EDIT
// 事件数据结构
struct SEventVoiceWorldListEdit
{
	DWORD dwWorldID;				// 游戏世界ID

	SEventVoiceWorldListEdit(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 事件基本头
struct SEventVoiceBaseHead
{
	DWORD dwData1;		// 数据1
	DWORD dwData2;		// 数据2
	DWORD dwData3;		// 数据3
};


#define		EVENT_VOICE_CLIENT_CONNECTSERVER	ENUM_EVENT_VOICE_CLIENT_CONNECTSERVER		// 请求连服务器
#define		EVENT_VOICE_CLIENT_DISCONNECT		ENUM_EVENT_VOICE_CLIENT_DISCONNECT			// 连接断开
#define		EVENT_VOICE_CLIENT_CONNECTED		ENUM_EVENT_VOICE_CLIENT_CONNECTED			// 已连接
#define		EVENT_VOICE_CLIENT_RECONNECT		ENUM_EVENT_VOICE_CLIENT_RECONNECT			// 重新连接

#define		EVENT_VOICE_CLIENT_CHATMSG			ENUM_EVENT_VOICE_CLIENT_CHATMSG				// 聊天  语音服务器发来的聊天消息，直接转发数据
// SVoiceSubMsgSendChatMsg_VC

#define		EVENT_VOICE_CLIENT_USERINFOUPDATE	ENUM_EVENT_VOICE_CLIENT_USERINFOUPDATE		// 用户信息更新，包含用户的频道基本信息，拥有权限
#define		EVENT_VOICE_CLIENT_ROOMINFOUPDATE	ENUM_EVENT_VOICE_CLIENT_ROOMINFOUPDATE		// 房间信息更新
#define		EVENT_VOICE_CLIENT_ISSPEAKING       ENUM_EVENT_VOICE_CLIENT_ISSPEAKING		    // 语音用户是否在说话
#define		EVENT_VOICE_CLIENT_ENTERROOMRESULT	ENUM_EVENT_VOICE_CLIENT_ENTERROOMRESULT		// 进入房间结果
#define		EVENT_VOICE_CLIENT_CHANNELINFOUPDATE	ENUM_EVENT_VOICE_CLIENT_CHANNELINFOUPDATE	// 频道信息更新
#define		EVENT_VOICE_CLIENT_ALLCHANNELINFOUPDATE	ENUM_EVENT_VOICE_CLIENT_ALLCHANNELINFOUPDATE	// 全部频道信息更新
#define		EVENT_VOICE_CLIENT_SELFSPEAKING	ENUM_EVENT_VOICE_CLIENT_SELFSPEAKING			// 频道信息更新

#define		EVENT_VOICE_CLIENT_NEEDNEWIP		ENUM_EVENT_VOICE_CLIENT_NEEDNEWIP			// 需要重选IP
#define		EVENT_VOICE_CLIENT_TEXTCHATMESSAGE		ENUM_EVENT_VOICE_CLIENT_TEXTCHATMESSAGE	// 文本聊天

#define		EVENT_VOICE_CLIENT_VOICELIST			ENUM_EVENT_VOICE_CLIENT_VOICELIST		// 正在语音人员列表
#define		EVENT_VOICE_CLIENT_CHATLIST				ENUM_EVENT_VOICE_CLIENT_CHATLIST		// 私聊列表

#define		EVENT_VOICE_CLIENT_FIRSTSPEAKING		ENUM_EVENT_VOICE_CLIENT_FIRSTSPEAKING	// 房间第一次有人说话是事件
#define		EVENT_VOICE_CLIENT_VESTLIST				ENUM_EVENT_VOICE_CLIENT_VESTLIST		// 马甲（临时管理）列表更新
#define		EVENT_VOICE_CLIENT_PERSONALTOPLIST		ENUM_EVENT_VOICE_CLIENT_PERSONALTOPLIST	// 个人频道排行榜
#define		EVENT_VOICE_CHAT						ENUM_EVENT_VOICE_CHAT					// 私聊
#define		EVENT_VOICE_CLIENT_CHANNELAUDIO			ENUM_EVENT_VOICE_CLIENT_CHANNELAUDIO	// 频道语音推送
#define		EVENT_VOICE_CLIENT_CALLFANS				ENUM_EVENT_VOICE_CLIENT_CALLFANS		// 召集粉丝
#define		EVENT_VOICE_CLIENT_GIFTSENDINFO			ENUM_EVENT_VOICE_CLIENT_GIFTSENDINFO	// 送礼信息
#define		EVENT_VOICE_CLIENT_DIAMONDINFO			ENUM_EVENT_VOICE_CLIENT_DIAMONDINFO		// 蓝钻信息
#define		EVENT_VOICE_CLIENT_FANSTOPLIST			ENUM_EVENT_VOICE_CLIENT_FANSTOPLIST		// 粉丝排行榜
#define		EVENT_VOICE_CLIENT_ADMINENTERINFO		ENUM_EVENT_VOICE_CLIENT_ADMINENTERINFO	// 管理员进入信息
#define		EVENT_VOICE_CLIENT_AD					ENUM_EVENT_VOICE_CLIENT_AD				// 语音召集令
#define		EVENT_VOICE_CLIENT_VIDEOFRAME			ENUM_EVENT_VOICE_CLIENT_VIDEOFRAME		// 视频数据
#define		EVENT_VOICE_CLIENT_VIDEOSHOW			ENUM_EVENT_VOICE_CLIENT_VIDEOSHOW		// 显示视频事件,如有新视频源,等待视频中,视频中断
#define		EVENT_VOICE_CLIENT_INITSPEEX			ENUM_EVENT_VOICE_CLIENT_INITSPEEX		// 异步操作压缩器 采样率线程
#define		EVENT_VOICE_CLIENT_VIDEO				ENUM_EVENT_VOICE_CLIENT_VIDEO			// 视频事件








//////////////////////////////////////////////////////////////////////////
// 消息码与数据结构

/***************************************************************/
///////////////////// 语音服务器模块 的消息码/////////////////
// 上层的消息码必定是 MSG_MODULEID_VOICE
/***************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EMMsgVoice
{
	ENUM_MSG_VOICE_INVALID = 0,

	// 公共消息
	ENUM_MSG_VOICE_HANDSHAKE,						// 握手消息
	ENUM_MSG_VOICE_HANDSHAKE_RESPONSE,				// 回应握手消息
	ENUM_MSG_VOICE_KEEPALIVE,						// 心跳检查
	ENUM_MSG_VOICE_ANSWER_KEEPALIVE,				// 回应心跳检查
	ENUM_MSG_VOICE_SENDDATA,						// 请求语音服务器网关服务发送数据给各软件服务器
	ENUM_MSG_VOICE_BROADCASTDATA,					// 请求语音服务器网关服务广播数据给各软件服务器
	ENUM_MSG_VOICE_BROADCASTDATATOCLIENT,			// 请求语音服务器广播数据给客户端
	ENUM_MSG_VOICE_WORLDLIST_UPDATE,				// 游戏世界列表更新
	ENUM_MSG_VOICE_SERVERLIST_UPDATE,				// 服务器列表更新
	ENUM_MSG_VOICE_DATA_SUBMSG,						// 语音子消息处理
	ENUM_MSG_VOICE_VOICEDATA,						// 语音数据
	ENUM_MSG_VOICE_SENDCHATMSG,						// 聊天消息
	ENUM_MSG_VOICE_PERFORMANCE,						// 性能检测消息包
	ENUM_MSG_VOICE_VOICELIST,						// 说话列表
	ENUM_MSG_VOICE_CHATLIST,						// 私聊列表
	ENUM_MSG_VOICE_WORLDLIST,						// 世界列表
	ENUM_MSG_VOICE_AD,								// 频道拉人
	ENUM_MSG_VOICE_VESTLIST,						// 马甲（临时管理）列表
	ENUM_MSG_VOICE_PERSONALTOPLIST,					// 个人排行榜
	ENUM_MSG_VOICE_CHATINVITE,						// 私聊邀请
	ENUM_MSG_VOICE_ADDVOICEBLUEDIAMOND,				// 添加蓝钻

	ENUM_MSG_VOICE_BRIDGEDATA,						// 发送消息到其他服务器，可跨区
	ENUM_MSG_VOICE_CHECK,							// 检测消息，用来跨服检测网络连接状态
	ENUM_MSG_VOICE_CHECKBACK,						// 检测消息回复，用来跨服检测网络连接状态

	// 语音网关消息
	ENUM_MSG_VOICE_LOGIN,							// 请求登录语音服务器
	ENUM_MSG_VOICE_CLIENTLIST_UPDATE,				// 语音网关同步客户端信息到语音服务器

	// 语音服务器
	ENUM_MSG_VOICE_USERINFO_UPDATE,					// 更新语音用户信息
	ENUM_MSG_VOICE_CHANNELUSERLIST_UPDATE,			// 更新频道成员列表
	ENUM_MSG_VOICE_ROOMINFO_UPDATE,					// 更新房间信息
	ENUM_MSG_VOICE_USERPARTINFO_UPDATE,				// 更新语音用户部分信息
	ENUM_MSG_VOICE_CHANNELINFO_UPDATE,				// 更新频道信
	ENUM_MSG_VOICE_TEXTCHATMESSAGE,					// 文字聊天消息
	ENUM_MSG_VOICE_ALLCHANNELINFO_UPDATE,			// 更新所有频道信息，通用的基本信息
	ENUM_MSG_VOICE_CHATUSERINFO_UPDATE,				// 私聊用户信息更新
	ENUM_MSG_VOICE_CALLFANS,						// 召集粉丝
	ENUM_MSG_VOICE_GIFTSENDINFO,					// 送礼信息
	ENUM_MSG_VOICE_DIAMONDINFO,						// 蓝钻红钻信息
	ENUM_MSG_VOICE_FANSTOPLIST,						// 粉丝排行榜
	ENUM_MSG_VOICE_ADMINENTERINFO,					// 管理员进入信息

	// 请求房间人数
	ENUM_MSG_VOICE_REQ_ROOM_ACTOR_LIST,				// 请求房间人员列表
	ENUM_MSG_VOICE_SEND_ROOM_ACTOR_LIST,			// 请求房间人员列表返回
	ENUM_MSG_VOICE_UB_SET_TEMP_RIGHT,               // 统战开始请求临时权限
	ENUM_MSG_VOICE_UBINFO_UPDATE,					// 社会服向语音服更新玩家统战信息

	ENUM_MSG_VOICE_SOCAIL_NOTIFY_USER_LOGOUT,		// 社会服通知玩家下线
	
	
	ENUM_MSG_VOICE_ALL_WORLD_BROADCAST,				// 请求语音服帮助广播消息到所有区
	ENUM_MSG_VOICE_MAXID,							// 最大消息码
};

// 最大消息码
#define MSG_VOICE_MAXID						ENUM_MSG_VOICE_MAXID



//////////////////////////////////////////////////////////////////////////
// 握手消息
#define MSG_VOICE_HANDSHAKE				ENUM_MSG_VOICE_HANDSHAKE
// 语音服务器连接器与语音服务器握手消息数据结构
struct SMsgVoiceHandshake_SB
{
	DWORD	dwWorldID;						// 服务器所在游戏世界ID
	WORD	nServerType;					// 服务器类型
	WORD	wSubID;							// 第n号服务器
	bool	bIsPublic;						// 是否是公共区的
	char	szName[VOICE_NAME_MAXSIZE];	// 服务器名
	char	szWorldName[VOICE_NAME_MAXSIZE];	// 游戏世界名

	SMsgVoiceHandshake_SB(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 客户端与语音网关握手
struct SMsgVoiceHandshake_CG
{
	char	szNickname[VOICE_NAME_MAXSIZE];		// 昵称
	DWORD	dwPDBID;							// PDBID
	char	szMacAddress[VOICE_NAME_MAXSIZE];	// 网卡地址

	SMsgVoiceHandshake_CG(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 语音数据通道与网关握手
struct SMsgVoiceDataHandShake_CG
{
	DWORD dwClientID;		// 客户端ID
	BYTE	nKeyLen;
	//char szKeyData[nKeyLen]				// 动态密码

	SMsgVoiceDataHandShake_CG(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 语音网关与语音服务器握手
struct SMsgVoiceHandshake_GV
{
	DWORD	dwGatewayID;			// 语音网关
	DWORD	dwMaxCounts;			// 最多人数

	SMsgVoiceHandshake_GV(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 回应握手消息
#define MSG_VOICE_HANDSHAKE_RESPONSE		ENUM_MSG_VOICE_HANDSHAKE_RESPONSE

// 语音服务器回应握手消息数据结构
struct SMsgVoiceHandshakeResponse	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间

	SMsgVoiceHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 语音网关回应握手消息数据结构
struct SMsgVoiceHandshakeResponse_GC	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间
	DWORD	dwAudioPort;		// 语音通道端口
	BYTE	nKeyLen;
	//char szKeyData[nKeyLen]	// 动态密码

	SMsgVoiceHandshakeResponse_GC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 语音服务器回应语音网关握手消息数据结构
struct SMsgVoiceHandshakeResponse_VG	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间

	SMsgVoiceHandshakeResponse_VG(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 语音服务器回应语音客户端握手消息数据结构
struct SMsgVoiceHandshakeResponse_VC	
{
	DWORD	dwClientID;				// 客户端ID
	DWORD	dwServerTime;			// 服务器时间
	BYTE	dwVoiceQuality;			// 频道默认语音品质类型 EMVoiceQualityType
	SVoiceCodecInfo	codecInfo;		// 压缩参数
	BYTE	nKeyLen;
	//char szKeyData[nKeyLen]					// 动态密码

	SMsgVoiceHandshakeResponse_VC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 心跳检查
#define MSG_VOICE_KEEPALIVE				ENUM_MSG_VOICE_KEEPALIVE


//////////////////////////////////////////////////////////////////////////
// 回应心跳检查
#define MSG_VOICE_ANSWER_KEEPALIVE			ENUM_MSG_VOICE_ANSWER_KEEPALIVE

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 请求语音服务器网关服务发送数据给各软件服务器消息
#define MSG_VOICE_SENDDATA					ENUM_MSG_VOICE_SENDDATA

// 请求语音服务器网关服务发送数据给各软件服务器数据结构
struct SMsgVoiceSendData
{
	DWORD	nSendChennel:8;		// 发送数据通道类型 EMVoiceSendChannelType
	DWORD	dwServerID:24;		// 软件服务器ID
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据
};


//////////////////////////////////////////////////////////////////////////
// 请求语音服务器网关服务广播数据给各软件服务器
#define MSG_VOICE_BROADCASTDATA					ENUM_MSG_VOICE_BROADCASTDATA

// 请求语音服务器网关服务广播数据给各软件服务器数据结构
struct SMsgVoiceBroadcastData
{
	BYTE	nSendChennel;		// 发送数据通道类型 EMVoiceSendChannelType
	DWORD	dwServerCounts;		// 软件服务器ID列表个数
	DWORD	nDataLens;			// 数据长度
	//DWORD	dwServerID[dwServerCounts];			// 软件服务器ID列表
	//BYTE	pData[nDataLens];					// 数据
};


//////////////////////////////////////////////////////////////////////////
// 请求语音服务器广播数据给客户端
#define MSG_VOICE_BROADCASTDATATOCLIENT ENUM_MSG_VOICE_BROADCASTDATATOCLIENT
struct SMsgVoiceBroadcastDataToClient
{
	BYTE nType;				// 广播类型		EMVoiceChannelType
	DWORD dwPDBID1;			// 角色ID1
	DWORD dwPDBID2;			// 角色ID2

	WORD nWorldID1;			// 世界ID1
	WORD nWorldID2;			// 世界ID2

	DWORD dwValueID1;		// ID值1
	DWORD dwValueID2;		// ID值2

	bool bRoom;				// 是否广播用户dwPDBID1所在房间

	DWORD dwDataLen;		// 数据长度
	// char pData[nDataLen];
};

//////////////////////////////////////////////////////////////////////////
// 游戏世界列表更新
#define MSG_VOICE_WORLDLIST_UPDATE					ENUM_MSG_VOICE_WORLDLIST_UPDATE

// 消息用 游戏世界数据结构
struct SMsgVoiceGameWorldNode
{
	DWORD	dwWorldID;			// 游戏世界ID
	BYTE	nNameStrlens;		// 游戏世界名称字串长度

	SMsgVoiceGameWorldNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 游戏世界列表更新数据结构
struct SMsgVoiceWorldListUpdate
{
	BYTE	nUpdateType;		// 更新类型,EMVoiceServerListUpdateType
	DWORD	dwWorldCounts;		// 游戏世界个数
	//dwWorldCounts* SMsgVoiceGameWorldNode;	// 软件服务器信息列表
	// dwWorldCounts* char szWorldName[nNameStrlens];	// 游戏世界名称列表

	SMsgVoiceWorldListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 服务器列表更新
#define MSG_VOICE_SERVERLIST_UPDATE					ENUM_MSG_VOICE_SERVERLIST_UPDATE

// 消息用 游戏服务器信息数据结构
struct SMsgVoiceGameServerNode
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID:16;		// 服务器所在游戏世界ID
	WORD	wServerType:8;		// 服务器类型
	WORD	wSubID:8;			// 第n号服务器
	DWORD	dwStateFlag:8;		// 服务器状态

	SMsgVoiceGameServerNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 服务器列表更新数据结构
struct SMsgVoiceServerListUpdate
{
	BYTE	nUpdateType;		// 更新类型,EMVoiceServerListUpdateType
	DWORD	dwServerCounts;		// 软件服务器个数
	//dwServerCounts* SMsgVoiceGameServerNode;	// 软件服务器信息列表

	SMsgVoiceServerListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 语音网关同步客户端信息到语音服务器
#define		MSG_VOICE_CLIENTLIST_UPDATE		ENUM_MSG_VOICE_CLIENTLIST_UPDATE
// 消息用 客户端信息数据结构
struct SMsgVoiceGatewayClientNode
{
	DWORD	dwClientID;			// 客户端ID
	DWORD	dwRemoteIP;			// 客户端IP
	//DWORD	dwPDBID;			// PDBID
};

// 客户端列表更新数据结构
struct SMsgVoiceGatewayClientListUpdate
{
	BYTE	nUpdateType;		// 更新类型
	DWORD	dwCounts;		// 游戏世界个数
	// dwCounts * SMsgVoiceGatewayClientNode;	// 客户端信息列表
};


//////////////////////////////////////////////////////////////////////////
// 语音服务器信息子消息
#define MSG_VOICE_DATA_SUBMSG					ENUM_MSG_VOICE_DATA_SUBMSG

// 语音服务器信息子消息数据结构
struct SMsgVoiceDataSubMsg
{
	DWORD dwMsgCode;						// 子消息代码  EMVoiceDataSubMsgCode
	DWORD dwUserData1;						// 用户数据1
	DWORD dwUserData2;						// 用户数据2
	DWORD dwUserData3;						// 用户数据3
	DWORD dwUserData4;
	DWORD dwUserData5;
	// 附加数据

	SMsgVoiceDataSubMsg(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 语音服务器信息消息子消息代码定义
enum EMVoiceDataSubMsgCode
{
	////////////////////	语音服务器   //////////////////////////////////////////////////////
	VoiceSubMsg_SendChatMsg = 0,					// 聊天消息
	VoiceSubMsg_LoginResponse,						// 登录回应，收到该消息就表示登录已经成功
	VoiceSubMsg_KickUser,							// 踢出用户
	VoiceSubMsg_AddVoiceTime,						// 增加语音时间
	VoiceSubMsg_VoiceCheck,							// 检测消息，用来跨服检测网络连接状态
	VoiceSubMsg_VoiceCheckBack,						// 检测消息回复，用来跨服检测网络连接状态
	VoiceSubMsg_UpdateNumValue,						// 更新数值到游戏服务器

	////////////////////	语音逻辑客户端   //////////////////////////////////////////////////////
	VoiceSubMsg_RequestLogin,					// 请求语音社会服登录
	VoiceSubMsg_OpenMic,						// 话筒操作	0 关闭 1 打开
	VoiceSubMsg_OpenHeadset,					// 听筒操作	0 关闭 1 打开
	VoiceSubMsg_RequestVisitor,					// 请求观察
	VoiceSubMsg_EnterRoom,						// 进入房间
	VoiceSubMsg_EnterRoomResult,				// 进入房间结果
	VoiceSubMsg_ChangeMicState,					// 设置房间麦序模式
	VoiceSubMsg_SetRoomStateFlag,				// 设置房间状态标记
	VoiceSubMsg_SetRoomLock,					// 设置房间锁
	VoiceSubMsg_AddMicOrder,					// 抢/下麦
	VoiceSubMsg_AddMicOrderTime,				// 增加/减少麦上人说话时间
	VoiceSubMsg_SetMicTime,						// 设置麦序时间
	VoiceSubMsg_AddClientToMicOrder,			// 把一个用户添加到麦序，或从麦序上移除
	VoiceSubMsg_MoveMicOrder,					// 移动麦序
	VoiceSubMsg_Move,							// 成员调度
	VoiceSubMsg_Broadcast,						// 频道广播
	VoiceSubMsg_AD,								// 频道拉人
	VoiceSubMsg_Ban,							// 封PDBID 禁言
	VoiceSubMsg_Kick,							// 踢人
	VoiceSubMsg_SetRemark,						// 设置频道备注
	VoiceSubMsg_SetTitleName,					// 设置称号
	VoiceSubMsg_GMEnterRoom,					// GM进入指定房间
	VoiceSubMsg_GMEnterRoomByPDBID,				// GM进入玩家所在房间
	VoiceSubMsg_ClearAllMicOrder,				// 清空麦序
	VoiceSubMsg_OpenUserList,					// 开启用户列表
	VoiceSubMsg_OpenChannel,					// 开启频道
	VoiceSubMsg_Chat,							// 文字聊天
	VoiceSubMsg_VoiceList,						// 正在语音列表
	VoiceSubMsg_ChatList,						// 私聊列表
	VoiceSubMsg_GetLastAudioInfo,				// 获取所有频道是否有人语音信息
	VoiceSubMsg_GetVoiceTimeLen,				// 获取语音在线时长
	VoiceSubMsg_EnterRoomByPDBID,				// 进入他所在的频道
	VoiceSubMsg_AddVest,						// 添加马甲（临时管理）
	VoiceSubMsg_GetVestList,					// 获取马甲（临时管理）列表
	VoiceSubMsg_SetPersonalChannelName,			// 修改个人频道名称
	VoiceSubMsg_SetPersonalChannelPublic,		// 公开个人频道，参加个人排行榜
	VoiceSubMsg_SendPersonalConfig,				// 发送个人频道配置
	VoiceSubMsg_RequestPersonalTopList,			// 获取个人频道排行榜
	VoiceSubMsg_ChatInvite,						// 私聊邀请
	VoiceSubMsg_ChatInviteResponse,				// 私聊邀请回应
	VoiceSubMsg_ChatInviteSuccess,				// 发送私聊邀请成功
	VoiceSubMsg_ChatRunSuccess,					// 私聊成功进行
	VoiceSubMsg_ChatClose,						// 私聊关闭
	VoiceSubMsg_ChatEnd,						// 私聊结束
	VoiceSubMsg_OpenChatMic,					// 私聊开麦
	VoiceSubMsg_SetChatConfig,					// 私聊设置
	VoiceSubMsg_GMEnterChat,					// GM旁听私聊
	VoiceSubMsg_ShareMic,						// 连麦
	VoiceSubMsg_ChannelAudio,					// 频道在语音
	VoiceSubMsg_MicFans,						// 是麦上用户的粉丝
	VoiceSubMsg_AddAtten,						// 添加/取消关注
	VoiceSubMsg_CallFans,						// 召集粉丝
	VoiceSubMsg_RequestFansOnlineCount,			// 请求在线粉丝数量
	VoiceSubMsg_AcceptFansCall,					// 接受粉丝召集
	VoiceSubMsg_RequestFansCallResult,			// 请求召集粉丝结果
	VoiceSubMsg_SetRoomName,					// 设置房间名称
	VoiceSubMsg_SetChannelMaxCount,				// 设置频道人数上限
	VoiceSubMsg_SetRoomChatCoolDown,			// 设置房间文字聊天冷却时间
	VoiceSubMsg_CloseRoomChat,					// 关闭房间文字聊天
	VoiceSubMsg_SetChannelQuality,				// 设置频道语音品质
	VoiceSubMsg_SendGift,						// 赠送礼物
	VoiceSubMsg_AcceptGift,						// 接收礼物
	VoiceSubMsg_AddBlueDiamond,					// 添加蓝钻
	VoiceSubMsg_GiftSendInfo,					// 送礼信息
	VoiceSubMsg_RequestDiamondInfo,				// 请求蓝钻等信息
	VoiceSubMsg_RequestFansTopList,				// 获取粉丝排行榜
	VoiceSubMsg_OpenVideo,						// 视频操作	0 关闭 1 打开

	// 最大
	VoiceSubMsg_Max
};

// VoiceSubMsg_RequestLogin

// 聊天消息 VoiceSubMsg_SendChatMsg
struct SVoiceSubMsgSendChatMsg_VC
{
	bool bTrace;		// 是否trace到LOG
	WORD nTipsType;		// 提示信息类型
	WORD nPos;			// 系统信息显示位置
	WORD dwTextLen;
	//char text[dwTextLen]
};

// VoiceSubMsg_EnterRoom,						// 进入房间
struct SVoiceSubMsgEnterRoom_CV
{
	LONGLONG nRoomID;						// 房间ID						
	char szPassword[VOICE_NAME_MAXSIZE];	// 密码 如果非加锁房间，无视

	SVoiceSubMsgEnterRoom_CV(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};


// VoiceSubMsg_SendGift				// 赠送礼物
struct SVoiceSubMsgSendGift_CS
{
	DWORD dwWorldID;	// 赠送人世界ID
	DWORD dwPDBID;		// 接收人
	DWORD dwGiftID;		// 礼物ID
	DWORD dwFlag;		// 位标识	EMVoiceSendGiftFlag
	DWORD dwData1;		// 备用自定义1
	DWORD dwData2;		// 备用自定义2
	DWORD dwData3;		// 备用自定义3
	WORD nMessageLen;	// 赠言长度
	// char szMessage[nMessageLen];
};


//////////////////////////////////////////////////////////////////////////
// 检测消息，用来跨服检测网络连接状态
// VoiceSubMsg_VoiceCheck,							
// VoiceSubMsg_VoiceCheckBack
// CHECK执行消息码
enum EMVoiceCheckActionID
{
	VoiceCheckActionID_SendGift = 0,	// 送礼

	VoiceCheckActionID_Max,
};

struct SVoiceSubMsgCheck
{
	BYTE nSrcPoint;		// 来源端
	WORD wSrcWorldID;	// 来源世界ID
	WORD wDestWorldID;	// 目的世界ID
	WORD wActionId;		// 执行消息码		EMVoiceCheckActionID
	DWORD dwPDBIDSend;	// 发送方PDBID
	DWORD dwPDBID;		// 接收方PDBID
	DWORD dwSpare1;		// 备用1
	DWORD dwSpare2;		// 备用2
	DWORD dwSpare3;		// 备用3
	DWORD dwDataLen;	// 额外数据长度
};

// VoiceSubMsg_AcceptGift 接收礼物
struct SVoiceSubMsgAcceptGift_ZZ
{
	DWORD dwPDBIDSend;	// 赠送人
	WORD nWorldID;		// 赠送人世界ID
	BYTE nCountry;		// 赠送人国家ID
	DWORD dwPDBID;		// 接收人
	WORD nGiftID;		// 礼物ID
	DWORD dwFlag;		// 位标识	EMVoiceSendGiftFlag
	DWORD dwData1;		// 备用自定义1
	DWORD dwData2;		// 备用自定义2
	DWORD dwData3;		// 备用自定义3
	BYTE nNameLen;		// 名字长度
	WORD nMessageLen;	// 赠言长度
	// char szName[nNameLen];
	// char szMessage[nMessageLen];
};

// VoiceSubMsg_AddBlueDiamond
struct SVoiceSubMsgAddBlueDiamond_ZV
{
	DWORD dwUDBID;			// 增加的通行证
	DWORD dwPDBID;			// 得到蓝钻的通行证下对应的角色ID，用来记录日志用
	DWORD dwCount;			// 增加的蓝钻数，一次最大不超过500000，超过的当作无效并报警
	DWORD dwPDBIDSend;		// 赠送礼物的人PDBID，用来记录日志用
	DWORD dwSpare1;			// 备用1，存放接收的礼物ID
	DWORD dwSpare2;			// 备用2
	DWORD dwSpare3;			// 备用2
	WORD nReasonLen;		// 备注长度
	// char szReason[nReasonLen];
};

// VoiceSubMsg_UpdateNumValue
enum EMVoiceUpdateNumValueID
{
	VoiceUpdateNumValueID_UserType = 0,		// 身份标记
};




//////////////////////////////////////////////////////////////////////////
// 请求性能检测消息包
#define	 MSG_VOICE_PERFORMANCE			ENUM_MSG_VOICE_PERFORMANCE

// 语音性能数据流通端定义
enum EMVoicePerformanceType
{
	VoicePerformanceType_RecordAudio = 0,	// 客户端 录音数据
	VoicePerformanceType_EncodeFrame,		// 客户端 压缩语音
	VoicePerformanceType_DecodeFrame,		// 客户端 解缩语音
	VoicePerformanceType_PlayAudio,			// 客户端 播放语音
	VoicePerformanceType_ClientPack,		// 客户端 语音组包
	VoicePerformanceType_Client,			// 客户端
	VoicePerformanceType_Gateway,			// 语音网关
	VoicePerformanceType_Server,			// 语音服
	VoicePerformanceType_Max				// 最大
};

// 性能数据
struct SMsgVoicePerformanceNode
{
	LONGLONG nSendTicks;		// 发包时间
	LONGLONG nRecvTicks;		// 收到时间
	LONGLONG nFrequency;		// CPU频率
	WORD	nRecvCounts;		// 待处理收包个数
	WORD	nSendCounts;		// 待处理发包个数

	SMsgVoicePerformanceNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 性能检测消息包
struct SMsgVoicePerformanceData
{
	SMsgVoicePerformanceNode data[VoicePerformanceType_Max];		// 性能数据 EMVoicePerformanceType
};

// 语音->网关，性能检测消息包
struct SMsgVoicePerformance_S
{
	DWORD  dwClientID;     // 客户端ID
	SMsgVoicePerformanceData perData;	// 性能检测消息包
};

//////////////////////////////////////////////////////////////////////////


// 请求登录语音服务器
#define		MSG_VOICE_LOGIN			ENUM_MSG_VOICE_LOGIN

struct SVoiceChannel
{
    LONGLONG channleID;
    BYTE nUserType;

    SVoiceChannel()
    {
        Clear();
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 语音用户信息
struct SVoiceUserInfo
{
    DWORD dwFromWorldID;
    DWORD dwClientID;		// 客户端ID
    DWORD dwPDBID;			// PDBID
    DWORD dwUDBID;			// 帐号ID
    char  szName[VOICE_NAME_MAXSIZE];   
    int   nSex;             // 性别
    int   nRankLevel;      // 段位
    char  szKinName[32];   // 家族名称
    SVoiceChannel channelList[VoiceChannel_Max];  // 自己默认频道列表
    SVoiceChannel exChannel;  // 扩展频道 例如其他人(主播)的个人频道

    // 数据库数据
    DWORD		dwDBUserType;	// DB用户类型
    DWORD		dwFansCount;	// 所有粉丝数
    DWORD       dwYesterdayFansCount; // 昨日粉丝数
    
    // 当前的一些状态
    BYTE		nVestUserType;        // 当前频道马甲的用户类型，为频道拥有者授权的管理员类型
    DWORD       dwStateFlag;          // 状态标记
    int         nMicIndex;            // 麦序索引 0表示无麦序

    SVoiceUserInfo()
    {
        Clear();
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 语音用户信息字段定义
enum SVoiceUserInfoFieldID
{
    VOICE_FIELD_FROMWORLDID,
    VOICE_FIELD_CLIENTID,
    VOICE_FIELD_PDBID,
    VOICE_FIELD_UDBID,
    VOICE_FIELD_NAME,
    VOICE_FIELD_SEX,
    VOICE_FIELD_RANK_LEVEL,
    VOICE_FIELD_KINNAME,
    
    // 频道列表 必须对应EMVoiceChannelType
    VOICE_FIELD_CHANNEL_None,
    VOICE_FIELD_CHANNEL_Gm,
    VOICE_FIELD_CHANNEL_World,
    VOICE_FIELD_CHANNEL_Country,
    VOICE_FIELD_CHANNEL_Clan,
    VOICE_FIELD_CHANNEL_Kin,
    VOICE_FIELD_CHANNEL_Group,
    VOICE_FIELD_CHANNEL_Team,
    VOICE_FIELD_CHANNEL_War,
    VOICE_FIELD_CHANNEL_Personal,

    VOICE_FIELD_EX_CHANNEL,

    VOICE_FIELD_DBUSER_TYPE,
    VOICE_FIELD_FANSCOUNT,
    VOICE_FIELD_FANSCOUNT_YESTERDAY,

    VOICE_FIELD_VESTTYPE,
    VOICE_FIELD_STATE_FLAG,
    VOICE_FIELD_MIC_INDEX,

    VOICE_FIELD_MAX,  // 不成超过 BYTE上限
};

// 包装语音用户信息 方便更新字段
struct SVoiceUserInfo_Table
{
    struct FieldInfo
    {
        int nOffset;
        int nLen;

        FieldInfo()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    SVoiceUserInfo context;
    FieldInfo fields[VOICE_FIELD_MAX];

    SVoiceUserInfo_Table()
    {
#define CreateField(id, member) \
    fields[id].nOffset = (int)(&((SVoiceUserInfo*)0)->member); \
    fields[id].nLen = sizeof(context.member);

        CreateField(VOICE_FIELD_FROMWORLDID, dwFromWorldID);
        CreateField(VOICE_FIELD_CLIENTID, dwClientID);
        CreateField(VOICE_FIELD_PDBID, dwPDBID);
        CreateField(VOICE_FIELD_UDBID, dwUDBID);
        CreateField(VOICE_FIELD_NAME, szName);
        CreateField(VOICE_FIELD_SEX, nSex);
        CreateField(VOICE_FIELD_RANK_LEVEL, nRankLevel);
        CreateField(VOICE_FIELD_KINNAME, szKinName);

        for(int i = VOICE_FIELD_CHANNEL_Gm; i <= VOICE_FIELD_CHANNEL_Personal; ++i)
        {
            CreateField(i, channelList[i - VOICE_FIELD_CHANNEL_None]);
        }

        CreateField(VOICE_FIELD_EX_CHANNEL, exChannel);
       
        CreateField(VOICE_FIELD_DBUSER_TYPE, dwDBUserType);
        CreateField(VOICE_FIELD_FANSCOUNT, dwFansCount);
        CreateField(VOICE_FIELD_FANSCOUNT_YESTERDAY, dwYesterdayFansCount);

        CreateField(VOICE_FIELD_VESTTYPE, nVestUserType);
        CreateField(VOICE_FIELD_STATE_FLAG, dwStateFlag);
        CreateField(VOICE_FIELD_MIC_INDEX, nMicIndex);
    }

    void getFields(rkt::obuf& ob, bool bMask[VOICE_FIELD_MAX])
    {
        int nFieldNumOffset = ob.size();
        
        // 字段数量
        ob << (BYTE)0;

        BYTE byUpdateNum = 0;
        for(int i = 0; i < VOICE_FIELD_MAX; ++i)
        {
            if(bMask[i])
            {
                ob << (BYTE)i;
                ob.push_back((char*)&context + fields[i].nOffset, fields[i].nLen);
                ++byUpdateNum;
            }
        }

        *(BYTE*)(ob.data() + nFieldNumOffset) = byUpdateNum;
    }

    bool updateFields(rkt::ibuffer& in, bool bMask[VOICE_FIELD_MAX] = NULL)
    {
        BYTE byFieldsNum = 0;
        in >> byFieldsNum;
        BYTE byFieldID = 0;
        for(int i = 0; i < byFieldsNum; ++i)
        {
            in >> byFieldID;
            if(in.fail())
            {
                return false;
            }

            if(byFieldID < 0 || byFieldID >= VOICE_FIELD_MAX)
            {
                return false;
            }

            const FieldInfo& field = fields[byFieldID];
            if(in.space() < field.nLen)
            {
                return false;
            }
            memcpy(((char*)&context) + field.nOffset, in.current(), field.nLen);
            in.offset(field.nLen);

            if(bMask) bMask[byFieldID] = true;
        }

        return true;
    }
};


// 场景服请求社会服，语音登录
struct SMsgVoiceZoneLogin_ZS
{
	SVoiceUserInfo userInfo;
    char   key[VOICE_RC4_KEY_SIZE];

    SMsgVoiceZoneLogin_ZS()
    {
        memset(this, 0, sizeof(*this));
    }
};


//////////////////////////////////////////////////////////////////////////
// 语音数据
#define		MSG_VOICE_VOICEDATA			ENUM_MSG_VOICE_VOICEDATA

// 毎帧语音数信息
struct SMsgVoiceAudioFrame
{
	WORD	nSize;		// 数据大小
};
// 毎帧语音视频数信息
struct SMsgVoiceVideoFrame
{
	WORD	nAudioSize;		// 语音数据大小
	WORD	nVideoSize;		// 视频数据大小
};

// 语音包结构
struct SMsgVoiceAudioDataHead
{
	DWORD dwClientID;	// 客户端ID
	BYTE nCounts;		// 个数
	BYTE nParam;		// 参数 声音来源用户设置的流畅度系数 用来设置自己的语音流畅机制

	// SMsgVoiceAudioFrame * nCounts;	// 每帧信息
	// char Frame[nSize] * nCounts;
};


//////////////////////////////////////////////////////////////////////////
// 语音服务器更新用户到客户端
#define		MSG_VOICE_USERINFO_UPDATE			ENUM_MSG_VOICE_USERINFO_UPDATE


//////////////////////////////////////////////////////////////////////////
// 频道成员列表更新
#define		MSG_VOICE_CHANNELUSERLIST_UPDATE		ENUM_MSG_VOICE_CHANNELUSERLIST_UPDATE


//////////////////////////////////////////////////////////////////////////
// 更新房间信息
#define		MSG_VOICE_ROOMINFO_UPDATE		ENUM_MSG_VOICE_ROOMINFO_UPDATE
// 更新头
struct SMsgVoiceUpdateRoomHead_VC
{
	BYTE nUpdateType;	// 更新类型  EMVoiceUpdateRoomType
	BYTE nRoomID;		// 房间ID
	// 不同类不同数据
};

// VoiceUpdateRoomType_All  全部
// SMsgVoiceUpdateRoomBase_VC baseInfo;		// 房间基本信息
// SMsgVoiceUpdateRoomMicUser_VC micUser;	// 麦上的人信息 如果是麦序模式，才发送


// VoiceUpdateRoomType_Base 基础信息
struct SMsgVoiceUpdateRoomBase_VC
{
	BYTE	nMicState;			// 麦类型 EMVoiceMicState
	DWORD	dwStateFlag;		// 房间状态标记 EMVoiceRoomStateFlag
	DWORD	dwUserCount;		// 房间人数，参考值
	WORD	dwChatCoolDown;		// 文字聊天冷却时间
	WORD	dwMicOrderTime;		// 默认麦序时间
};

// VoiceUpdateRoomType_MicUser 麦上的人信息
struct SMsgVoiceUpdateRoomMicUser_VC
{
	DWORD	dwClientID;				// 当前在麦上的人，ClientID
	DWORD	dwMicEndTime;			// 麦结束时间
	DWORD	dwFansCount;			// 粉丝数
	WORD	dwWorldID;				// 麦上的人所在世界ID
};

// VoiceUpdateRoomType_Remark 公告信息
struct SMsgVoiceUpdateRoomRemark_VC
{
	WORD nRemarkLen;					// 备注信息长度
	//char szRemarkLen[nRemarkLen]		// 备注信息
};


//////////////////////////////////////////////////////////////////////////
// 更新频道信
#define		MSG_VOICE_CHANNELINFO_UPDATE	ENUM_MSG_VOICE_CHANNELINFO_UPDATE
// 更新头
struct SMsgVoiceUpdateChannelInfoHead_VC
{
	BYTE nUpdateType;		// 更新类型
};

// VoiceUpdateChannelType_All   所有信息
struct SMsgVoiceUpdateChannelAll_VC
{
	BYTE nOwnerNameLen;						// 频道所属名字长度
	SVoiceChannelBaseInfo baseInfo;			// 频道基本信息
	WORD nRoomLockInfo;						// 房间加锁信息
	WORD nAudioInfo;						// 房间最后语音信息
	BYTE nRoomNameLen[VoiceRoomType_Max];	// 房间名
	// char szOwnerName[nOwnerNameLen];
	// char szRoomName[VoiceRoomType_Max][VOICE_NAME_MAXSIZE]
};

// VoiceUpdateChannelType_Base  频道基础信息
struct SMsgVoiceUpdateChannelBase_VC
{
	SVoiceChannelBaseInfo baseInfo;			// 频道基本信息
	BYTE nRoomLockInfo;						// 房间加锁信息
	WORD nAudioInfo;						// 房间最后语音信息
};

// VoiceUpdateChannelType_Audio
struct SMsgVoiceUpdateChannelAudio_VC
{
	WORD nAudioInfo;		// 房间最后语音信息
};

// VoiceUpdateChannelType_Name 
struct SMsgVoiceUpdateChannelName_VC
{
	char szChannelName[VOICE_NAME_MAXSIZE];		// 频道名称
};

// VoiceUpdateChannelType_RoomName
struct SMsgVoiceUpdateRoomName_VC
{
	BYTE nRoomID;		// 房间ID
	char szRoomName[VOICE_NAME_MAXSIZE];		// 房间名称
};

//////////////////////////////////////////////////////////////////////////
// 文字聊天消息
#define		MSG_VOICE_TEXTCHATMESSAGE	ENUM_MSG_VOICE_TEXTCHATMESSAGE
struct SMsgVoiceTextChatMessage_VC
{
	BYTE nChannel;				// 文字频道
	DWORD dwWorldID;			// 发送者世界ID
	BYTE nCountry;				// 国家ID
	DWORD dwPDBID;				// 发送者PDBID
	DWORD dwTarget;				// 接收者PDBID
	BYTE nNameLen;				// 发送者名字长度
	BYTE nTextLen;				// 文字聊天
	// char szName[nNameLen]
	// char szText[nTextLen]
};


//////////////////////////////////////////////////////////////////////////
// 说话列表
#define		MSG_VOICE_VOICELIST		ENUM_MSG_VOICE_VOICELIST
struct SMsgVoiceVoiceList_VC
{
	DWORD	dwCount;		// 说话人的个数
};

struct SMsgVoiceVoiceNode_VC
{
	DWORD		dwPDBID;			// 说话人的PDBID
	BYTE		nChannelType;		// 频道类型
	DWORD		dwWorldID;			// 世界ID
	DWORD		dwID;				// ID值
	BYTE		nRoomID;			// 子房间ID
	DWORD		dwCount;			// 人数
	WORD		nTime;				// 多少秒前说话
	BYTE		nNameLen;			// 说话人名称
	// char szName[nNameLen]
};

#define		MSG_VOICE_CHATLIST		ENUM_MSG_VOICE_CHATLIST						// 私聊列表
struct SMsgVoiceChatList_VC
{
	DWORD dwCount;		// 私聊个数
};

struct SMsgVoiceChatNode_VC
{
	DWORD dwChatID;				// 私聊ID
	DWORD dwWorldIDInviter;		// 邀请者世界ID
	DWORD dwWorldIDAcceptor;	// 接受者世界ID
	BYTE nNameLenInviter;	
	BYTE nNameLenAcceptor;
	DWORD dwInviterUDBID;		/// 邀请者用户帐号ID
	DWORD dwAcceptorUDBID;		/// 接受者用户帐号ID
	// char szNameI[nNameLenInviter];
	// char szNameA[nNameLenAcceptor];
};


//////////////////////////////////////////////////////////////////////////
// 世界列表
#define		MSG_VOICE_WORLDLIST		ENUM_MSG_VOICE_WORLDLIST
struct SMsgVoiceWorldList_VC
{
	DWORD	dwCount;		// 游戏世界个数
};

struct SMsgVoiceWorldNode_VC
{
	DWORD	dwID;			// 游戏世界ID
	BYTE	nNameLen;		// 游戏世界名字长度
	// char szName[nNameLen];
};


//////////////////////////////////////////////////////////////////////////
// 更新所有频道信息，通用的基本信息
#define		MSG_VOICE_ALLCHANNELINFO_UPDATE		ENUM_MSG_VOICE_ALLCHANNELINFO_UPDATE	

// 更新头
struct SMsgVoiceUpdateAllChannelInfoHead_VC
{
	BYTE nUpdateType;		// 更新类型
};

// VoiceUpdateAllChannelType_Audio,	 所有频道是否有人语音信息
struct SMsgVoiceUpdateAllChannelAudio_VC
{
	WORD nChannelAudioInfo;		// 语音时间信息，位保存
};


//////////////////////////////////////////////////////////////////////////
// 频道拉人
#define		MSG_VOICE_AD		ENUM_MSG_VOICE_AD
struct SMsgVoiceAD_SC
{
	DWORD dwTime;		// 召集时间
	DWORD dwPDBID;		// 发送人
	DWORD dwUDBID;		// UDBID
	BYTE nSex;			// 性别
	BYTE nChannelType;	// 频道类型
	BYTE nRoomID;		// 子房间ID
	DWORD dwValueID;	// 值ID
	BYTE nADType;		// 附加拉人效果 弹DIDA的位置
	WORD nTextLen;		// 附加文本长度
	// char szInviterName[nNameLen];
	// char szText[dwTextLen]	// 附加文本
};

struct SMsgVoiceAD_SV
{
	DWORD dwPDBID;			// 发送人
	BYTE nBroadcastType;	// EMVoiceADBroadcastType
	BYTE nADType;			// 附加拉人效果 弹DIDA的位置
	WORD nTextLen;			// 附加文本长度
	// char szText[nTextLen]	// 附加文本
};

//////////////////////////////////////////////////////////////////////////
// 马甲（临时管理）列表
#define		MSG_VOICE_VESTLIST		ENUM_MSG_VOICE_VESTLIST

// 更新头
struct SVoiceVestList_VC
{
	BYTE nUpdateType;		// 更新类型	EMVoiceUpdateType
	BYTE nCount;			// 更新个数
};

// 更新结点
struct SVoiceVestNode_VC
{
	DWORD dwUDBID;			// dwUDBID
	BYTE nSex;
	BYTE nNewUserType;		// 用户类型	
	BYTE nNameLen;			// 名称长度
	// char szName[nNameLen];
};

//////////////////////////////////////////////////////////////////////////
// 个人排行榜
#define MSG_VOICE_PERSONALTOPLIST	ENUM_MSG_VOICE_PERSONALTOPLIST
struct SVoicePersonalTopList_VC
{
	DWORD dwUpdateTime;
	BYTE nCount;	// 个数
};

struct SVoicePersonalTopData_VC
{
	DWORD dwWorldID;
	BYTE nCountryID;
	DWORD dwUDBID;			// 通行证ID
	DWORD dwUserCount;		// 在线人数
	DWORD dwMaxMemberCount;	// 历史最高在线人数
	BYTE nState;			// 状态
	BYTE nOwnerType;		// 拥有者帐号类型
	BYTE nNameLen;			// 名称长度
	//char szName[nNameLen];	// 名称
};


//////////////////////////////////////////////////////////////////////////
// 私聊邀请
#define MSG_VOICE_CHATINVITE	ENUM_MSG_VOICE_CHATINVITE
struct SVoiceChatInvite_SV
{
	DWORD dwInviter;		// 邀请者PDBID
	DWORD dwAcceptor;		// 接受者PDBID
	bool bStranger;			// 是否是陌生人
};


//////////////////////////////////////////////////////////////////////////
// 私聊用户信息更新
#define MSG_VOICE_CHATUSERINFO_UPDATE ENUM_MSG_VOICE_CHATUSERINFO_UPDATE
struct SVoiceChatUserInfo_VC
{
	BYTE nUserType;			// 用户类型，0 邀请者 1 接受者
	SVoiceUserInfo userInfo;	// 用户信息
};


//////////////////////////////////////////////////////////////////////////
// 召集粉丝
#define MSG_VOICE_CALLFANS ENUM_MSG_VOICE_CALLFANS
struct SVoiceCallFans_VC
{
	DWORD dwTime;				// 邀请时间
	DWORD dwClientID;			// 客户端ID
	LONGLONG nID;				// 召集房间ID
	BYTE nSex;
	BYTE nCountryID;			// 国家ID
	DWORD dwFromWorldID;			// 世界ID
	DWORD dwUDBID;				// UDBID
	char szNickname[VOICE_NAME_MAXSIZE];		// 昵称
	WORD nMsgLen;
	BYTE nParam1;			// 备用数据1
	BYTE nParam2;			// 备用数据2
	// char szMsg[nMsgLen];
};


//////////////////////////////////////////////////////////////////////////
// 发送消息到其他服务器，可跨区
#define MSG_VOICE_BRIDGEDATA ENUM_MSG_VOICE_BRIDGEDATA
struct SMsgVoiceBridgeData
{
	BYTE nSrcEndPoint;		// 目的地
	WORD wDestWorldID;		// 目的世界ID
	DWORD dwPDBID;			// 目的角色
	DWORD dwDataLen;		// 数据长度
};


//////////////////////////////////////////////////////////////////////////
// 添加蓝钻
#define MSG_VOICE_ADDVOICEBLUEDIAMOND ENUM_MSG_VOICE_ADDVOICEBLUEDIAMOND
struct SMsgVoiceAddVoiceBlueDiamond_VV
{
	DWORD dwUDBID;		// 添加者UDBID
	DWORD dwPDBID;		// 添加者PDBID
	DWORD dwCount;		// 添加蓝钻数量
	DWORD dwPDBIDSend;	// 赠送人PDBID
	DWORD dwSpare1;
	DWORD dwSpare2;
	DWORD dwSpare3;		// 备用
	WORD nReasonLen;	// 备注长度
	// char szReason[nReasonLen];
};


//////////////////////////////////////////////////////////////////////////
// 送礼信息
#define MSG_VOICE_GIFTSENDINFO ENUM_MSG_VOICE_GIFTSENDINFO
struct SMsgVoiceGiftSendInfo
{
	DWORD dwTime;			// 赠送时间
	DWORD dwPDBIDSend;		// 赠送人
	WORD nWorldIDSend;		// 赠送人世界ID
	BYTE nCountrySend;		// 赠送人国家ID

	DWORD dwPDBID;		// 接收人
	WORD nWorldID;		// 接收人世界ID
	BYTE nCountry;		// 接收人国家ID

	WORD nGiftID;		// 礼物ID
	BYTE nGiftLevel;		// 礼物等级
	BYTE nBroadcast;	// 礼物广播范围

	DWORD dwFlag;		// 位标识	EMVoiceSendGiftFlag
	DWORD dwData1;		// 备用自定义1
	DWORD dwData2;		// 备用自定义2
	DWORD dwData3;		// 备用自定义3

	BYTE nNameLenSend;	// 赠送人名字长度
	BYTE nNameLen;		// 接收人名字长度
	WORD nMessageLen;	// 赠言长度
	//char szNameSender[nNameSendLen];
	//char szName[nNameLen];
	//char szMessage[nMessageLen];
};


//////////////////////////////////////////////////////////////////////////
// 蓝钻红钻信息
#define MSG_VOICE_DIAMONDINFO ENUM_MSG_VOICE_DIAMONDINFO
struct SMsgVoiceDiamondInfo_VC
{
	DWORD dwUDBID;				// 通行证UDBID,唯一
	DWORD dwBlueDiamond;		// 蓝钻总数，一直累加
	DWORD dwCurBlueDiamond;		// 当前蓝钻数，只有这个数值才可结算成红钻
	DWORD dwBlueDiamondOld;		// 当月之前的总数
	DWORD dwBlueDiamondToday;	// 今日获得蓝钻数，第二天自动把今天的这个值添加到dwCurBlueCount中，然后清零

	DWORD dwRedDiamond;			// 红钻总数，一直累加，这部分可结算成金币/RMB
};


//////////////////////////////////////////////////////////////////////////
// 粉丝排行榜
#define MSG_VOICE_FANSTOPLIST ENUM_MSG_VOICE_FANSTOPLIST
struct SMsgVoiceFansTopList_VC
{
	DWORD dwUpdateTime;		// 排行榜生成时间
	BYTE nCount;			// 个数
};

struct SMsgVoiceFansTopNode_VC
{
	DWORD dwUDBID;						// 通行证UDBID
	BYTE nUserType;						// 通行证帐号类型
	BYTE nRank;							// 名次
	DWORD dwFansCount;					// 粉丝总数
	WORD dwFromWorldID;					// 最近登录的角色来源世界ID
	BYTE nCountryID;					// 最近登录的角色所属ID(国家，大陆。。)
	BYTE nNameLen;
	//char szActorName[nNameLen];		// 最近登录的角色角色名
};


//////////////////////////////////////////////////////////////////////////
// 管理员进入信息
#define MSG_VOICE_ADMINENTERINFO ENUM_MSG_VOICE_ADMINENTERINFO
struct SMsgVoiceAdminEnterInfo_VC
{
	bool bEnter;		// 是否进入 true进，false 出
	DWORD dwClientID;	// 客户端ID
	WORD nWorldID;		// 世界ID
	DWORD dwPDBID;
	BYTE nRoomID;		// 进出的房间ID
	BYTE nNameLen;		// 角色名称
	// char szActorName[nNameLen];
};

///////////////////////////////////////////////////////////////////////
// 请求房间人员列表
#define MSG_VOICE_REQ_ROOM_ACTOR_LIST ENUM_MSG_VOICE_REQ_ROOM_ACTOR_LIST				
struct SMsgVoiceReqRoomActorList_OV
{
	DWORD dwDBID;	// 请求人物DBID
	DWORD dwWorldID; 
	int	nNation;	// 国家ID
	SVoiceChannelID roomInfo;  // 房间信息  注:开始统战时这个字段忽略
	BYTE byFlag;		// 结束统战请求：0，开始统战请求：1

	SMsgVoiceReqRoomActorList_OV()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 发送房间人员列表
#define MSG_VOICE_SEND_ROOM_ACTOR_LIST ENUM_MSG_VOICE_SEND_ROOM_ACTOR_LIST			
struct SMsgVoiceSendRoomActorList_VO
{
	DWORD dwDBID; // 请求角色ID
	DWORD dwWorldID; 
	int	nNation;	// 国家ID
	SVoiceChannelID roomInfo;  // 房间信息
	DWORD		dwStatsMaxMemberNum; // 统计的房间人数峰值
	DWORD		dwActorCount;
	BYTE		byFlag;		// 结束统战请求：0，开始统战请求：1
	//房间人物信息
	//DWORD arrActorDBIDList[];

	SMsgVoiceSendRoomActorList_VO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 请求设置统战临时权限
#define MSG_VOICE_UB_SET_TEMP_RIGHT ENUM_MSG_VOICE_UB_SET_TEMP_RIGHT
struct SMsgVoiceUBSetTempRight_OV
{
	DWORD dwDBID;
	SVoiceChannelID roomInfo;  // 房间信息
	bool bOn; // true -- 开启权限 false -- 收回权限
	int nStatsFlag; // 0--关闭峰值统计 1--开始峰值统计 2--维持峰值统计 
};

// 社会服向语音服更新统战信息
#define MSG_VOICE_UBINFO_UPDATE ENUM_MSG_VOICE_UBINFO_UPDATE 
struct SMsgVoiceUBUpdateActorInfo_OV
{
	DWORD dwDBID;
	DWORD dwTotalCredit; // 累积声望
	bool bIsUBCommander; // 是否是统战
};

// 社会服通知玩家下线
#define MSG_VOICE_SOCIAL_NOTIFY_USER_LOGOUT ENUM_MSG_VOICE_SOCAIL_NOTIFY_USER_LOGOUT
struct SMsgVoiceSocialNotifyActorLogout_OV
{
	DWORD dwPDBID;  // 玩家DBID
};

#pragma pack()