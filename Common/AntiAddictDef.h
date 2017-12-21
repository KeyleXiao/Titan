/*******************************************************************
** 文件名:	AntiAddictDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/7/2016
** 版  本:	1.0
** 描  述:	防沉迷

********************************************************************/

#ifndef __ANTI_ADDICT_DEF_H__
#define __ANTI_ADDICT_DEF_H__

#ifdef VERSEION_DEBUG
#   define ANTI_ADDICT_DEBUG_OUTPUT
#endif
#ifdef ADDICT_DEBUG_OUTPUT
#	define ADDICT_TRACELN(x)			TraceLn(x)
#	define ADDICT_EMPHASISLN(x)			EmphasisLn(x)
#	define ADDICT_ERRORLN(x)			ErrorLn(x)
#	define ADDICT_WARNINGLN(x)			WarningLn(x)
#else
#	define ADDICT_TRACELN(x)
#	define ADDICT_EMPHASISLN(x)
#	define ADDICT_ERRORLN(x)
#	define ADDICT_WARNINGLN(x)
#endif


// 1个小时
#define ADDICT_ONEHOUR		    (3600)

// 2个小时
#define ADDICT_TWOHOUR		    (3600 * 2)

// 收益减半时间（疲劳时间）
#define ADDICT_TIRE_TIME	    (3600 * 3)

// 无收益时间（不健康时间）
#define ADDICT_ILL_TIME		    (3600 * 5)

// 5分钟
#define ADDICT_FIVE_MIN		    (1000 * 60 * 5)

// 5小时
#define ADDICT_FIVE_HOUR	    (3600 * 5)

// 踢人时间
#define ADDICT_KICK_TIME        (5000)


enum EAddictState
{
	emAddictState_Health = 0,		// 健康时间
	emAddictState_Tire,				// 疲劳时间
	emAddictState_Ill,				// 不健康时间
};


///////////////////////// Message /////////////////////////////////////////////////
#pragma pack(1)

// 防沉迷信息结构
struct SUserAddictInfo
{
	DWORD dwUserID;					// 玩家帐号ID
	DWORD dwIdCard;					// 玩家身份证标识
	int	  nPDBID;					// 玩家的角色ID号
	int   nLoginTime;				// 玩家身份证上线时间
	int   nLogoutTime;				// 玩家身份证下线时间
	int   nPreOnlineTime;			// 玩家历史累计在线时间
	int   nPreOffLineTime;			// 玩家历史累计不在线时间
	int   nOnlineCount;				// 在线角色个数
	bool  bDBRead;					// 是否经过数据库读出来的数据

	SUserAddictInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

/******************************************************************/
// 上层的消息码必定是 MSG_MODULEID_ANTIADDICT
/******************************************************************/
// 给世界服发送上线消息
#define MSG_ADDICT_LOGIN								1
// 消息体
struct SMsgAddictLogin_SW
{
	DWORD dwPDBID;			// 玩家的角色ID号
	DWORD dwUserID;			// 玩家帐号ID
	DWORD dwIdCard;			// 玩家身份证ID
};

// 给世界服发送下线消息
#define MSG_ADDICT_LOGOUT								2
struct SMsgAddictLogout_SW
{
	DWORD dwPDBID;			// 玩家的角色ID号
	DWORD dwUserID;			// 玩家帐号ID
	DWORD dwIdCard;			// 玩家身份证ID
};

// 世界服返回防沉迷信息给场景服
#define MSG_ADDICT_REFRESH								3
struct SMsgAddictRefresh_WS
{
	SUserAddictInfo info;
	bool bKickout;			// 是否踢人
	bool bLoginInfo;		// 区别上下线消息
};

// 游戏世界实名认证
#define MSG_ADDICT_AUTH						            4
struct SMsgAddictAuth_CS
{
	char entityname[32];
	char card[32];
};

// 游戏服务器返回结果给客户端
#define  MSG_ADDICT_GAME_Z2C						    5
struct SMsgAddictGame_Z2C 
{
	DWORD ret;
	char szRetDes[128];
};

//游戏服务器到世界服务器，通知其他游戏服务器解除防沉迷
#define  MSG_ADDICT_ALLZONE							    6
struct SMsgAddictAllGame_Z2W
{
	//DWORD dwUserID;
	DWORD dwIdCard;
};

// 世界服务器通知其他场景服务器，解除防沉迷
#define	MSG_REMOVEADDICT_W2Z					        7
struct SMsgRemoveAddict_W2Z
{
	//DWORD dwUserID;
	DWORD dwIdCard;
};

//客户端->服务器 请求激活卡
#define MSG_NPCACTIVECARD_C2Z					        8
struct SMsgNPCActiveCard_C2Z
{
	DWORD dwUserID;
	char card[32];
	DWORD dwClientIP;
};
// 服务器->客户端 请求激活卡结果
#define  MSG_NPCACTIVECARD_Z2C						    9
struct SMsgNPCActiveCard_Z2C
{
	DWORD ret;
	BYTE nRegKeyFlag;				// 0未激活新手类型卡 1已激活新手类型卡
	char szRetDes[128];
};

// 服务器->客户端 防沉迷信息提醒
#define  MSG_ADDICT_TIP_S2C								10
struct SMsgAddictTip_S2C
{
	char szRetDes[512];
};

// 服务器->世界服 身份证信息更新
#define MSG_UPDATE_IDCARD_Z2W							11
struct SMsgUpdateIdCard_Z2W 
{
	DWORD dwOldIdCard;
	DWORD dwNewIdCard;
};

// 世界服->服务器 身份证信息更新
#define MSG_UPDATE_IDCARD_W2Z							12
struct SMsgUpdateIdCard_W2Z
{
	DWORD dwOldIdCard;
	DWORD dwNewIdCard;
};

// 服务器->世界服 实名认证信息更新
#define MSG_UPDATE_REALNAME_Z2W							13
struct SMsgUpdateRealNameAuther_Z2W 
{
	DWORD dwIdCard;
};

// 世界服->服务器 实名认证信息更新
#define MSG_UPDATE_REALNAME_W2Z							14
struct SMsgUpdateRealNameAuther_W2Z  
{
	DWORD dwIdCard;
};

// 世界服批量更新防沉迷信息给场景服
#define MSG_ADDICT_BATCH_REFRESH						15

//end add
#pragma pack()

#endif // __ANTI_ADDICT_DEF_H__