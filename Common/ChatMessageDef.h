/*******************************************************************
** 文件名:	ChatMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	侯森
** 日  期:	3/22/2016
** 版  本:	1.0
** 描  述:	Chat消息定义
********************************************************************/

#ifndef __CHAT_MESSAGE_DEF_H__
#define __CHAT_MESSAGE_DEF_H__

#include "NetMessageDef.h"
#include "ChatDef.h"

// 聊天系统消息码定义键值
enum CHAT_MESSAGE
{
	SC_NET_MSG_CHAT = SC_MSG_CHAT_START,			// 聊天系统消息

	// 服务端->客户端
	SC_MSG_CHAT_INFO_BROADCAST,						// 广播聊天信息
	SC_MSG_CHAT_PRIVATE_INFO_BROADCAST,				// 广播私聊信息
	SC_MSG_CHAT_ACTORTIPINFO_BROADCAST,				// 广播伪装角色聊天信息
	SC_MSG_CHAT_SYSTEM_INFO_BROADCAST,				// 广播系统信息
	SC_MSG_CHAT_LOUDSPEAKER_BROADCAST,				// 广播大喇叭
	SC_MSG_CHAT_SEND_CACHE_MESSAGE,					// 发送聊天记录
	SC_MSG_CHAT_GMCOMMAND_RESPONSE,					// -处理GM请求
	// 客户端->服务器
	CS_MSG_CHAT_INFO_SEND,							// 聊天信息发送消息
	CS_MSG_CHAT_PRIVATE_INFO_SEND,					// 私聊信息发送消息
	CS_MSG_CHAT_SET_CHANNEL_MASK,					// 设置屏蔽频道
	CS_MSG_CHAT_GMCOMMAND,							// -GM请求
	CS_MSG_CHAT_GMCOMMAND_RESULT,					// -GM请求处理结果
	// 服务器->服务器
	SO_MSG_CHAT_INFO_SEND,							// 转发聊天信息
	SO_MSG_CHAT_PRIVATE_INFO_SEND,					// 转发私聊信息
	SS_MSG_CHAT_INFO_BROADCAST,						// 转交聊天广播
	SS_MSG_CHAT_SYSTEM_INFO_BROADCAST,				// 广播系统信息
	OS_MSG_CHAT_CHECKCHATCONSUME,					// -处理聊天消耗
	SO_MSG_CHAT_DONECHATCONSUME,					// -处理聊天消耗结束
	SO_MSG_CHAT_SYSTEM_INFO_BROADCAST,				// 转交广播系统信息
	SO_MSG_CHAT_LOUDSPEAKER_BROADCAST,				// 转交喇叭广播
	OS_MSG_CHAT_SYSTEM_CMD,							// 系统命令

	// 以下为待整理消息码,需要整理时再整理
	/*
 	/// 修改聊天设置，如退订某频道消息 上下文：(const ChatConfig& cfg)
 	ChatMsg_Config,
 
 	///统计的FPS和Delay数据，暂时放在这里了 上下文：(ulong id, ulong totalFPSTimes, ulong lowFPSTimes, ulong averageFPS, ulong highDelayTimes, ulong averageDelay)
 	ChatMsg_FPSDELAY,
 
 	///	用户的MAC,硬盘序列，C盘序列，上下文：(ulong mac, ulong disk, ulong cDisk)
 	ChatMsg_MuteInfo,
 
 	//////////////////////////////////////////////////////////////////////////
 	// 聊天服务器或场景服务器 -> 客户端(注：经网关服务器中转)
 	//////////////////////////////////////////////////////////////////////////
 
 	/// 禁言			上下文：(ulong minutes, ulong mute_rule)
 	ChatMsg_Mute,
 
 	/// 用户权限更新	上下文：(ulong right)
 	ChatMsg_UpdateRight,
 
 
 	/// 非文明值信息传递 
 	/// 上下文：(int System,int World,int Nation,int Tribe,int Kin,int Team,int Group,int Province,int City,int Current,int Zone,int Union,int Private,int Business,int Voice,ulong clearTime)
 	ChatMsg_RudeFine,
 
 
 	//////////////////////////////////////////////////////////////////////////
 	// 场景服务器 -> 聊天服务器(注：经中心服务器中转)
 	//////////////////////////////////////////////////////////////////////////
 
 	///	用户登录	上下文：(UseraInfo* info)
 	ChatMsg_UserLogin,
 
 	///	用户登出	上下文：(dbid user)
 	ChatMsg_UserLogoff,
 
 	/// 用户信息改变	上下文：(UseraInfo* info, ulong updateOption) 主要包括：变更国家，变更帮会，加入脱离退伍，用户升级，权限变更，场景切换，其中id为必填项
 	ChatMsg_InfoChanged,
 
 	/// 系统向指定的用户列表广播聊天信息	上下文：(dbid sender, ulong channel, const char* text, const char* datetime)
 	ChatMsg_Broadcast,
 
 	/// 系统向指定的用户列表广播系统提示信息，如队友捡到物品信息	上下文：(TipsType type, dbid sender, ulong channel, ulong pos, const char* text )
 	ChatMsg_BroadcastSystemTips,
 
 	/// 同上，但传入的第一个参数是ID
 	ChatMsg_BroadcastSystemTipsByID,
 
 	/// 转发来自于客户端的消息
 	ChatMsg_SendEx,
 
 
 
 
 	//////////////////////////////////////////////////////////////////////////
 	// 聊天服务器 -> 场景服务器
 	//////////////////////////////////////////////////////////////////////////
 
 	/// 9宫格广播消息（适合普通频道）	上下文：(dbid sender, const char* text, const char* datetime, eMuteRule)
 	ChatMsg_Broadcast_9,
 
 	/// 9宫格广播系统提示消息（适合普通频道）	上下文：(TipType type, dbid sender, ulong pos, const char* text)
 	ChatMsg_Broadcast_9_SystemTips,
 
 	/// 广播本地图消息（适合区域频道）	上下文：(dbid sender, const char* text, const char* datetime, eMuteRule)
 	ChatMsg_Broadcast_Zone,
 
 	/// 请求所有在线用户列表（如果聊天服务器后启动或重启需要） 上下文：无
 	ChatMsg_OnlineUserInfo,
 
 	/// 邮寄系统命令	上下文：(dbid sender, const char* cmd)
 	ChatMsg_SystemCommand,
 
 	/// 扣除点数（有的聊天需要收费，所以。。。）
 	/// TODO 
 
 	// 客户端2命令
 	/// 扫描用户信息 上下文：(dbid sender, const char* cmd)
 	ChatMsg_ScanInfo,
 
 	// 客户端命令
 	/// 扫描用户到达 上下文：(const char* str)
 	ChatMsg_ScanInfoArrived,
 
 	// 社会服命令
 	/// 处理扫描信息 上下文：(dbid sender, const char* str)
 	ChatMsg_HandleScanInfo,
 
 	/// 转发社会服禁言禁言结束 上下文：(dbid mutePlayer)
 	ChatMsg_MuteStatus,
 	
 	/// （喇叭类型，当前服务器的喇叭个数，喇叭光效，喇叭内容）
 	/// 发送喇叭信息  上下文：(LoudspeakerType loudType, ulong lastlout, unsigned int effectType, const char* text )
 	LoudspeakerMsg_Send,
 	
 	/// (发送者dbid， 喇叭类型， 发送者名字， 发送的文本内容， 光效ID, 国家ID，服务器当前还剩多少个排队喇叭)
 	/// 喇叭信息到达    上下文： (dbid sender, ulong loudType, const char* name, const char* text, unsigned int effectType, ulong nationId, ulong leaveLSCount)
 	LoudspeakerMsg_Arrived,
 
 	/// 转发来自于客户端的喇叭消息
 	LoudspeakerMsg_SendEx,
 
 
 	// 客户端
 	/// 统计用户信息 上下文：(dbid sender, const char* cmd)
 	ChatMsg_StatInfo,
 
 	// 社会服命令
 	/// 处理扫描信息 上下文：(dbid sender, const char* str)
 	ChatMsg_HandleStatInfo,
 
 	/// 免登入打开官方的某些链接 上下文：(ushort type)
 	ChatMsg_PostFreeOpenURL,
 
 	/// (指定动作("open\print"), 指定要打开的文件或程序, 给要打开的程序指定参数, 缺省目录, 打开选项)
 	/// 打开URL 上下文：(const char* operation, const char* fileName, const char* parameters, const char* directory, ulong integer)
 	ChatMsg_OpenURL,
 
 	/// 杀掉本进程
 	ChatMsg_KillServerCluster,
 
 	// 发送举报信息( 消息结构 ulong type, BOOL isVIP, ulong dwReasonFlag, ulong dwCheaterID, const char* strCheaterName, const char* strTitle, const char* strMsg )
 	ChatMsg_Report,
 
 	// 禁言自己， 上下文：(const char* reason)
 	ChatMsg_MuteSelf,
 
 	// 跨区踢人
 	ChatMsg_KickoutToPublicWorld,
 
 	// 跨区传送
 	ChatMsg_HangAround,
 
 	// 跨区聊天
 	ChatMsg_KuaQuChat,
 
 	/// 聊天信息到达	上下文：(dbid sender, UID senderUID, ulong channel, ulong worldId, const char* name, [const char* honor,] [uchar city,] [const char* targetName,] const char* text, const char* datetime)
 	ChatMsg_Arrived_KuaQu,
 
 	//滴答
 	ChatMsg_Dida,
 
 	//场景服来的GM命令
 	ChatMsg_ServerCommand,
 
 	// chat message from backend
 	ChatMsg_MessageFromBackend,
 
 	// 设置安全时间
 	ChatMsg_SetSecurityTime,
 
 	///统计网络Delay数据，暂时放在这里了 上下文：(ulong id, ulong highDelayTimes, ulong averageDelay)
 	ChatMsg_DyncNetDelay,
 
 	/// 跨普通区传送
 	ChatMsg_HangExAround,
	*/
};


#pragma pack(1)
struct msg_chat_info_transmit_head
{
	PDBID pdbid;
	UID uid;

	msg_chat_info_transmit_head()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_CHAT_INFO_SEND
// 聊天消息
struct msg_chat_info_send
{
	int channel;
	int nTipID;		// EMChatTipID
	int nLen;

	msg_chat_info_send()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_CHAT_PRIVATE_INFO_SEND
// 私聊消息
struct msg_chat_private_info_send
{
	int channel;
	PDBID targetID;
	char targetName[ACTORNAME_MAXSIZE];
	int nLen;

	msg_chat_private_info_send()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_CHAT_SET_CHANNEL_MASK
// 设置屏蔽频道
struct msg_chat_set_channel_mask
{
	int channelMask;

	msg_chat_set_channel_mask()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SS_MSG_CHAT_INFO_BROADCAST
// SC_MSG_CHAT_INFO_BROADCAST
// 聊天广播消息
struct msg_chat_info_broadcast
{
	int channel;
	UID senderUID;
    PDBID senderPdbid;
	UDBID senderUdbid; 
	int senderClanID;
	int senderKinID;
	char senderName[ACTORNAME_MAXSIZE];
	int nLen;

	msg_chat_info_broadcast()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SO_MSG_CHAT_PRIVATE_INFO_SEND
// SC_MSG_CHAT_PRIVATE_INFO_BROADCAST
// 私聊广播消息
struct msg_chat_private_info_broadcast
{
	int channel;
	PDBID senderPdbid;
	UDBID senderUdbid;
	int senderKinID;
	char senderName[ACTORNAME_MAXSIZE];
	PDBID targetPdbid;
	char targetName[ACTORNAME_MAXSIZE];
	int nLen;

	msg_chat_private_info_broadcast()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_CHAT_ACTORTIPINFO_BROADCAST
// 特殊聊天广播消息
struct msg_chat_actortipinfo_broadcast
{
	int channel;
	PDBID senderPdbid;
	UDBID senderUdbid;
	int senderKinID;
	char senderName[ACTORNAME_MAXSIZE];
	int nTipID;
	int nLen;

	msg_chat_actortipinfo_broadcast()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_CHAT_SYSTEM_INFO_BROADCAST
// 系统广播消息
struct msg_chat_system_info_broadcast_sc
{
	int tipID;		// EMChatTipID
    WORD nReserveData;
	WORD nLen;

	msg_chat_system_info_broadcast_sc()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_CHAT_SEND_CACHE_MESSAGE
// 发送聊天记录
struct msg_chat_send_cache_message_sc
{
	int nCount;

	msg_chat_send_cache_message_sc()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct msg_chat_send_cache_message_sc_item
{
	PDBID nActorID;
	UDBID nUserID;
	char szName[ACTORNAME_MAXSIZE];
	int nTipID;
	WORD nChannelID;
	WORD nLen;

	msg_chat_send_cache_message_sc_item()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SS_MSG_CHAT_SYSTEM_INFO_BROADCAST
// 系统广播消息(服务器间传递)
struct msg_chat_system_info_broadcast_ss
{
	EMChatBroadcastType broadType;
	int broadTargetID;
	EMChatTipID tipID;
	int nLen;

	msg_chat_system_info_broadcast_ss()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SO_MSG_CHAT_SYSTEM_INFO_BROADCAST
// 系统广播消息(场景服到社会服间传递)
struct msg_chat_system_info_broadcast_so
{
	EMChatBroadcastType broadType;
	int broadTargetID;
	EMChatTipID tipID;
	int nLen;

	msg_chat_system_info_broadcast_so()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SO_MSG_CHAT_LOUDSPEAKER_BROADCAST
// 喇叭消息(场景服到社会服间传递)
struct msg_chat_loudspeaker_broadcast_so
{
	EMChatBroadcastType broadType;
	PDBID senderID;							// 喇叭发送者PDBID
	char senderName[ACTORNAME_MAXSIZE];     // 喇叭发送者名字
	int broadTargetID;
	EMChatTipID tipID;
	DWORD dwSpeakerID;						// 喇叭序号
	DWORD dwSendTime;						// 发送时间，场景服为准
	int nLen;

	msg_chat_loudspeaker_broadcast_so()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_CHAT_LOUDSPEAKER_BROADCAST
// 喇叭消息（服务端到客户端）
struct msg_chat_loudspeaker_broadcast_sc
{
	EMChatTipID tipID;
	int nLen;

	msg_chat_loudspeaker_broadcast_sc()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_CHAT_LOUDSPEAKER_BROADCAST
// 喇叭消息（客户端到服务端）
struct msg_chat_loudspeaker_broadcast_cs
{
	int nSpeakerType;
	int nEffectType;
	int nOrderSet;
	int nThemeID;

	msg_chat_loudspeaker_broadcast_cs()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()

#endif // __CHAT_MESSAGE_DEF_H__