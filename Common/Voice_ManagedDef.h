#ifndef __VOICE_MANAGED_DEF_H__
#define __VOICE_MANAGED_DEF_H__

#include "ManagedCode.h"

#pragma warning(disable:4482)


//////////////////////////////////////////////////////////////////////////
/*						C++和C#公用数据结构								*/
//////////////////////////////////////////////////////////////////////////

// 语音频道类型
// 不要随意挪动位置，社会服计算权限，必须要以VoiceChannel_GM开头，VoiceChannel_Personal结尾，否则计算不正确
ENUM EMVoiceChannelType
{
    VoiceChannel_None = 0,		// 无效频道
    VoiceChannel_GM,			// 官方
    VoiceChannel_World,			// 世界
    VoiceChannel_Country,		// 国家
    VoiceChannel_Clan,			// 联盟
    VoiceChannel_Kin,			// 家族
    VoiceChannel_Group,			// 团队
    VoiceChannel_Team,			// 队伍
    VoiceChannel_War,           // 战场频道
    VoiceChannel_Personal,		// 个人
    VoiceChannel_Max,
};

// 语音房间类型
ENUM EMVoiceRoomType
{
    VoiceRoomType_Main = 0,		// 大厅
    VoiceRoomType_PK,			// 统战
    VoiceRoomType_KTV,			// 娱乐
    VoiceRoomType_Chat,			// 聊天
    VoiceRoomType_Hang,			// 挂机
    VoiceRoomType_Meeting,		// 会议
    VoiceRoomType_Office,		// 办公
    VoiceRoomType_Private,		// 密室
    VoiceRoomType_Max,			// 最大
};

// 房间控麦类型
ENUM EMVoiceMicState
{
    VoiceMicState_MicMaster = 0,		/// 主席模式
    VoiceMicState_MicFree,				/// 自由麦
    VoiceMicState_MicOrder,				/// 麦序

    VoiceMicState_Max,			/// 
};


// 频道状态标记
ENUM EMVoiceChannelStateFlag
{
    VoiceChannelState_Close			= 0x1,	/// 频道关闭
    VoiceChannelState_NoUserList	= 0x2,	/// 关闭用户列表
    VoiceChannelState_NoMicOrder	= 0x4,	/// 关闭麦序模式
    VoiceChannelState_NoMicFree		= 0x8,	/// 关闭自由模式
    VoiceChannelState_Public		= 0x10,	/// 频道公开参加排行榜，只对个人频道有效
    //VoiceChannelState_Unuse		= 0x20,	/// 
    //VoiceChannelState_Unuse		= 0x40,	/// 
    //VoiceChannelState_Unuse		= 0x80,	/// 
    //VoiceChannelState_Unuse		= 0x100, ///
    //VoiceChannelState_Unuse		= 0x200, ///
    //VoiceChannelState_Unuse		= 0x400, ///
    //VoiceChannelState_Unuse		= 0x800, /// 
    //VoiceChannelState_Unuse  		= 0x1000, /// 
    //VoiceChannelState_Unuse		= 0x2000, /// 
};

// 房间状态标记
ENUM EMVoiceRoomStateFlag
{
    VoiceRoomState_AddMicOrder		= 0x1,	/// 麦序模式下可抢麦
    VoiceRoomState_KeySpeak			= 0x2,	/// 自由麦下只能按键发音
    VoiceRoomState_Lock				= 0x4,	/// 有密码
    VoiceRoomState_HoldMic			= 0x8,	/// 麦序模式下控麦
    VoiceRoomState_NoChat			= 0x10,	/// 禁止文字聊天
    //VoiceRoomState_Unuse			= 0x20,	/// 
    //VoiceRoomState_Unuse		= 0x40,	/// 
    //VoiceRoomState_Unuse		= 0x80,	/// 
    //VoiceRoomState_Unuse		= 0x100, ///
    //VoiceRoomState_Unuse		= 0x200, ///
    //VoiceRoomState_Unuse		= 0x400, ///
    //VoiceRoomState_Unuse		= 0x800, /// 
    //VoiceRoomState_Unuse  	= 0x1000, /// 
    //VoiceRoomState_Unuse		= 0x2000, /// 
};

// 用户状态位标记
ENUM EMVoiceUserStateFlag
{
    VoiceUserStateFlag_None			= 0x0,			// 0	:无
    VoiceUserStateFlag_Mic			= 0x1,			// 1	:开麦
    VoiceUserStateFlag_NoListen		= 0x2,			// 2	:关听筒
    VoiceUserStateFlag_Mute			= 0x4,			// 4	:禁言
    VoiceUserStateFlag_Broadcast	= 0x8,			// 8	:可频道广播
    VoiceUserStateFlag_Vest			= 0x10,			// 16	:马甲
    VoiceUserStateFlag_ChatMic		= 0x20,			// 32	:私聊开麦
    VoiceUserStateFlag_Chat			= 0x40,			// 64	:在私聊
    VoiceUserStateFlag_BanChat		= 0x80,			// 128	:不接受任何私聊
    VoiceUserStateFlag_BanStrangerChat = 0x100,		// 256	:不接受陌生人私聊
    VoiceUserStateFlag_ShareMic		= 0x200,		// 512	:连麦
    VoiceUserStateFlag_Video		= 0x400,		// 1024	:视频
    //VoiceUserStateFlag_NoUse		= 0x800,		// 2048	:未用
    //VoiceUserStateFlag_NoUse		= 0x1000,		// 4096	:未用
    //VoiceUserStateFlag_NoUse		= 0x2000,		// 8192	:未用
    //VoiceUserStateFlag_NoUse		= 0x4000,		// 16384:未用
    //VoiceUserStateFlag_NoUse		= 0x8000,		// 32768:未用
    //VoiceUserStateFlag_NoUse		= 0x10000,		// 65536:未用

};

// 成员类型
ENUM EMVoiceUserType
{
    VoiceUserType_None = 0,				// 普通成员
    VoiceUserType_Normal,				// 普通成员
    VoiceUserType_VIP,					// 嘉宾主持
    VoiceUserType_Host,					// 主播
    VoiceUserType_Admin,				// 频道管理员
    VoiceUserType_OW,					// 频道拥有者
    VoiceUserType_GM,					// 官方管理员
    VoiceUserType_Max,
};

// 通行证帐号类型
ENUM EMVoiceAccountType
{
    VoiceAccountType_None = 0,	// 无效 
    VoiceAccountType_Art,		// 官方艺人
    VoiceAccountType_Host,		// 官方主播
    VoiceAccountType_Preside,	// 官方主持
    VoiceAccountType_Idol,		// 官方偶像
    VoiceAccountType_Start,		// 官方明星
    VoiceAccountType_Admin = 14,// 官方管理员
    VoiceAccountType_GM,		// 官方超级管理员

    VoiceAccountType_Max,
};

// 静音类型
ENUM EMVoiceMuteType
{
    VoiceMute_None = 0,
    VoiceMute_Headset,				// 听筒
    VoiceMute_Microphone,			// 麦克风
    VoiceMute_Max,
};


// 频道语音品质类型
ENUM EMVoiceQualityType
{
    VoiceQuality_Default = 0,		// 默认语音品质
    VoiceQuality_Low,				// 低语音品质
    VoiceQuality_Normal,			// 普通语音品质
    VoiceQuality_High,				// 高语音品质
    VoiceQuality_Max,
};

// 权限标识定义
ENUM EMVoiceRightFlag
{
    VoiceRight_None			= 0x0,			// 0	:无
    VoiceRight_Supper		= 0x1,			// 1	:超级管理权限
    VoiceRight_Ban			= 0x2,			// 2	:封PDBID 禁言
    VoiceRight_Kick			= 0x4,			// 4	:踢出频道
    VoiceRight_Move			= 0x8,			// 8	:成员调度
    VoiceRight_Broadcast	= 0x10,			// 16	:频道广播
    VoiceRight_AD			= 0x20,			// 32	:频道拉人
    VoiceRight_Mic			= 0x40,			// 64	:麦序管理
    VoiceRight_Lock			= 0x80,			// 128	:房间加锁，改名
    VoiceRight_Speak		= 0x100,		// 256	:管理员说话
    VoiceRight_Vest			= 0x200,		// 512	:管理马甲
    VoiceRight_Owner		= 0x400,		// 1024	:拥有者权限：频道改名
    //VoiceRight_NoUse		= 0x800,		// 2048	:未用
    //VoiceRight_NoUse		= 0x1000,		// 4096	:未用
    //VoiceRight_NoUse		= 0x2000,		// 8192	:未用
    //VoiceRight_NoUse		= 0x4000,		// 16384:未用
    //VoiceRight_NoUse		= 0x8000,		// 32768:未用
    //VoiceRight_NoUse		= 0x10000,		// 65536:未用

};

#endif	// __VOICE_MANAGED_DEF_H__