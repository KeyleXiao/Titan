/*******************************************************************
** 文件名:	ILoginServerProtocol.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	03/08/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_LOGIN_SERVER_PROTOCOL_H__
#define __INTERFACE_LOGIN_SERVER_PROTOCOL_H__

#include "IGameDatabaseDef.h"
#include "Vector3.h"
#include "GatewayServiceDef.h"

// 创建角色分步骤模式
#define USE_CREATE_STEP_MODE

#pragma pack(1)

// 登录模式
enum EMClientLoginType
{
    LoginType_None = 0,						// 老式登录模式
    LoginType_AutoLogin,					// 自动登录
};

namespace rkt
{
#ifndef USERNAME_MAXSIZE
#	define  USERNAME_MAXSIZE      DBDEF_USERNAME_MAXSIZE
#endif
#ifndef PASSWORD_MAXSIZE
#	define  PASSWORD_MAXSIZE      DBDEF_PASSWORD_MAXSIZE
#endif
#ifndef MACADDR_MAXSIZE
#	define  MACADDR_MAXSIZE       DBDEF_MAC_ADDRESS_MAXSIZE
#endif
#ifndef ACTORNAME_MAXSIZE
#	define  ACTORNAME_MAXSIZE     DBDEF_ACTORNAME_MAXSIZE
#endif

#	define  APPERANCE_DATA_LEN    82
#	define  ENCRYPT_KEY_LEN	      64
#	define  GAMEWORLD_NAME_LEN	  32
#	define  MAX_GESTURE_BUFF_LEN  32
#	define  MAX_SUPER_PASS_LEN	  41
#	define  MAX_PHONE_NUMBER_LEN  32
#	define  MAX_PHONE_INFO_LEN	  128
#	define	MAX_DBWORDID_COUNT	  12
#	define	MAX_VERIFYCODE_LEN    32 
#	define  MAX_PWD_MD5_LEN		  60
#   define  MAX_SUMMONER_MAX      2
//#	define	CREATEACOTR_ENTERZONE
//#	define	SELECTACOTR_TRYENTERZONE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
@name : 登陆服务器协议定义
@brief: 登陆服务器功能:
		1.帐号管理: 注册/验证/删除 (注册和删除可以在WEB上做)
		2.角色管理: 创建/选择/删除
		3.登陆排队
		4.软键盘/安全提交密码

		安全登陆方案:
		1.玩家在多服务器之间切换时的身份认证问题 
		(玩家迁移消息只能在服务器之间发,因为客户端连接保持在网关上不会断开,所以不会产生没有什么问题,
		另外数据库需要对角色记录进行加解锁,防止多服务器对角色数据进行存取时出现回档现象)

		2.密码安全传输-防止网络层监听
		(防止监听有几种办法,例如客户端使用 RSA Public-key加密密码,服务器使用private-key解开.
		但Public-key使用过于频繁(不是一次一密),可能会影响加密算法的健壮性
		另外的方案就是使用Diffie Hellman安全交换密码,可以做到一次一密
		但这个方案容易受到中间人攻击,既A-B进行通讯,C伪装成A跟B通讯,同时又伪装成B跟A通讯,从而套取密码,
		我们的做法是通讯层统一进行流加密,流加密时使用Diffie Hellman做到一次一密,另外登陆服务器在接收登陆消息时使用RSA私钥解密密码字符串)

		3.密码安全传输-防止客户端输入窗口被监听
		这个可以采用软键盘机制,但是要注意软键盘的图象和布局必须由服务器发过来,客户端永远不出现明文字符串

		4.由于使用软键盘机制是绝对安全的,所以删除角色,高等级的装备转移强制要求使用软键盘输入2级密码则可以将盗号的危害减到很小
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////////
	/**************************** 登陆服务器和客户端之间的通讯协议 **********************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// 错误提示定义
	enum
	{
		NoError     = 0 , // 没有错误,纯粹的提示信息
		LoginError      , // 登陆错误
		CreateActorError, // 创建角色错误
		DeleteActorError, // 删除角色错误
		SelectActorError, // 选择角色错误
		AccountLockError, // 帐号被锁通知
		AccountUnlocking, // 帐号解锁通知
		GestureError,	  // 手势验证失败
		PartnerCheckError, // 联运帐号验证出错
		ActivateCheckError, // 激活验证出错
	};

    // 登陆服返回结果定义
    enum LOGIN_RESULT_CODE
    {
        CREATE_RESULT_SUCCESS,  // 创建角色成功
    };

	// 用户登陆状态定义
	enum GATEWAY_CLIENT_STATE
	{
		CLIENT_CONN_STATE_LOGIN =  1,   // 登陆状态(等待登陆,包括普通登陆和特殊方式的登陆,例如动态密码,矩阵卡等可能的登陆方式)
        CLIENT_CONN_STATE_ACTOR,        // 选择静态角色状态
        CLIENT_CONN_STATE_QUEUE,        // 排队状态(等待进入静态地图)
		CLIENT_CONN_STATE_GAME,         // 游戏状态(正在游戏中)
	};

	// 加密算法定义
	enum
	{
		LOGIN_ENCRYPT_NO               =  0,   // 不加密
		LOGIN_ENCRYPT_RSA              =  1,   // RSA加密
		LOGIN_ENCRYPT_RC4              =  2,   // RC4加密
		LOGIN_ENCRYPT_CUSTOM           =  3,   // 公司内部自定义的加密方式
		LOGIN_ENCRYPT_MAX              =  4,   
	};

    // Web Api定义
    enum WEB_API_TYPE
    {
        WEB_API_TYPE_UNKNOWN,             // 没有定义的
        WEB_API_TYPE_RECOMMEND,           // 推荐人
        WEB_API_TYPE_MAX,
    };

	// 手势密码启用状态
	enum
	{
		tGesture_State_NoUse = 0,				// 未启用手势需提醒
		tGesture_State_NeedCheck,				// 启用手势验证
		tGesture_State_NoCheck,					// 未启用手势不需提醒
		tGesture_State_NeedModify,				// 要求修改手势密码
	};

	// 联运验证信息类型
	enum
	{
		LOGIN_PARTNER_CHECK_SUCESS = 0,			// 成功
		LOGIN_PARTNER_CHECK_ERROR,				// 错误
	};

    enum
    {
        LOGIN_UPDATE_REASON_LOGIN,              // 登录账号
        LOGIN_UPDATE_REASON_CREATE,             // 创建角色
        LOGIN_UPDATE_REASON_DELETE,             // 删除角色
    };

    enum
    {
        CS_MSG_USER_ENTER_GAME,
        CS_MSG_USER_CREATE_PLAYER,              // 请求创建角色
        SC_MSG_USER_ENTER_GAME,                 // 进入游戏
        SC_MSG_UPDATE_SELECT_ACTOR,             // 更新选中的角色信息
        CS_MSG_USER_EXITGAME,                   // 退出
        CS_MSG_USER_MIGRATE_ACHIEVE,            // 请求完成迁移
        MSG_LOGINSERVER_MESSAGE,                // 登陆提示
        MSG_LOGINSERVER_HANDSHAKE,              // 登陆握手
        MSG_LOGINSERVER_LOGIN,                  // 请求登陆
        MSG_LOGINSERVER_CLIENT_LYCHECKINFO,     // 联运发给客户端的key
        MSG_LOGINSERVER_UPDATE_ACTOR,           // 更新角色信息
        MSG_LOGINSERVER_CREATE_ACTOR,           // 创建角色
        MSG_LOGINSERVER_CREATE_ACTOR_EXTEND,    // 创建角色附加消息
        MSG_LOGINSERVER_CREATE_RESULT,          // 创建角色结果S->C
        MSG_LOGINSERVER_DELETE_ACTOR,           // 删除角色
        MSG_LOGINSERVER_SELECT_ACTOR,           // 选择角色
        MSG_LOGINSERVER_SERVERCHECKREQ,         // 服务器检测
        MSG_LOGINSERVER_SERVERCHECKRES,
        MSG_LOGINSERVER_SWITCH_STATE,           // 切换登陆状态
        MSG_LOGINSERVER_QUEUE_ORDER,            // 服务器通知客户端排队的序号
        MSG_LOGINSERVER_ENTER_GAMEWORLD,
        MSG_LOGINSERVER_RECONNECT,
        MSG_LOGINSERVER_KICKOUT,
        MSG_LOGINSERVER_REKICKOUT,              // 场景服务器踢人后通知登陆服
        MSG_LOGINSERVER_REMOVEANTIADDICT,       // 登录服同步解除防沉迷信息
        MSG_LOGINSERVER_ANNOUNCE,               // 排队状态发通告
        MSG_LOGINSERVER_SILENCEREFRESH_PROGRESS,// 角色沉默了，开始恢复
        MSG_LOGINSERVER_SILENCEREFRESHLEAVE,
        MSG_LOGINSERVER_ACTOR_SECUREINFO,       // 客户端向服务器请求状态信息
        MSG_LOGINSERVER_PASSPORT_LYCHECKINFO,   // 帐号验证服回过来的验证信息
        MSG_LOGINSERVER_CLIENT_VERIFYCODE_ERROR,// 验证码错误消息
        MSG_LOGINSERVER_CLIENT_REFRESH_VERIFYCODE,  // 客户端请求刷新验证码
        MSG_LOGINSERVER_REGISTERACCOUNT,        // 客户端申请注册账号
        MSG_LOGINSERVER_REGISTERACCOUNT_RESULT, // 服务器返回申请注册账号结果
        MSG_LOGINSERVER_REGISTERACCOUNT_REQUESTVERTIFYCODE, // 申请验证码
        MSG_LOGINSERVER_TRY_RETURNLOGIN,        // 请求返回到登录状态
        MSG_LOGINSERVER_SEND_GAMEAREA_INFO,     // 服务器向客户端发送游戏世界信息
        MSG_LOGINSERVER_REQUEST_ACTOR_COUNTS,   // 请求用帐号各区角色数
        MSG_LOGINSERVER_REQUEST_ACTOR_COUNTS_RESULT,        // 请求用帐号各区角色数结果

        MSG_LOGINSERVER_SEARCH_ACTORNAME,               // 获取角色随机名称
        MSG_LOGINSERVER_SEARCH_ACTORNAME_RESULT,      // 获取角色随机名称结果

        MSG_LOGINSERVER_REQ_QUERY_ACTOR_CONTINUE_PLAY_INFO,      // 请求查询角色续完信息
        MSG_LOGINSERVER_RET_QUERY_ACTOR_CONTINUE_PALY_INFO,

        MSG_LOGINSERVER_CLIENT_TREK_ACK,

        MSG_LOGINSERVER_SERVER_MSG_INFO,       // 登陆服到客户端提示信息
    };

    //-----------------------------登录相关---------------------------------
    // 服务端-客户端 登录返回消息
    // SC_MSG_USER_LOGIN
    struct SC_LoginMsg
    {
        DWORD   dwLoginCode;                        // 登录返回码,0表示成功
        char    szMsg[256];                         // 登录提示信息
        DWORD   dwGameWorldID;                      // 区ID
        UDBID   udbid;                              // 唯一的账号ID
        int     playercnt;                          // 角色数
        // SPlayerInfo player[playercnt];
    };

    //-----------------------------进入游戏---------------------------------
    // SC_MSG_USER_ENTER_GAME 服务器返回进入游戏结果
    struct SC_EnterGameMsg
    {
        int nMapID;             // 地图ID
        int nErrorCode;			// 错误码
    };

    // CS_MSG_USER_CREATE_PLAYER
    struct CS_CreatePlayer
    {
        PDBID pdbid;
        int nMapID;
    };

    //-----------------------------退出游戏---------------------------------
    //客户端-服务器 退出消息
    // CS_MSG_USER_EXITGAME
    struct CS_ExitGameMsg
    {
		PDBID idActor;
        UID uid;
    };

    // 客户端-登录服 请求完成迁移
    // CS_MSG_USER_MIGRATE_ACHIEVE
    struct CS_MigrateAchieve
    {
        UDBID   udbid;
        PDBID   pdbid;
        char    skey[MIGRATE_SKEYLEN];
    };

    // MSG_LOGINSERVER_MESSAGE
	struct SMsgLoginServer_Message
	{
		DWORD  dwErrorCode;                     // 错误码
		WORD   wMsgLen;                         // 提示信息长度
		char   szMessage[];                     // 提示信息内容
	};

    // MSG_LOGINSERVER_HANDSHAKE
	struct SMsgLoginServer_Handshake
	{
		BYTE   byEncryptMethod;                 // 加密方法,参考前面的加密方法定义
		bool   bPublicGameWorld;			    // 是否为公共区
		int    wEncryptKey;                     // 加密密钥
		char   szKeyData[ENCRYPT_KEY_LEN];      // 公钥内容
		int    nThisGameWorldID;			    // 当前所属游戏世界ID
		int    nPublicGameWorldID;			    // 当前公共区游戏世界ID
		char   szGameWorldName[GAMEWORLD_NAME_LEN];// 所属游戏世界名称

		DWORD   dwDBWorld[GAMEWORLD_NAME_LEN];  // 所属游戏世界名称
		DWORD	dwFlag;						    // 标识符

		SMsgLoginServer_Handshake()
		{
			memset(this, 0, sizeof(*this));
		}
	};

    // MSG_LOGINSERVER_LOGIN
	struct SMsgLoginServer_Login
	{
		DWORD  dwClientVer;                     // 客户端版本号
        DWORD  dwGameWorldID;                   // 游戏区ID
		char   szUserName[USERNAME_MAXSIZE];    // 用户帐号
		char   szPassword[PASSWORD_MAXSIZE];    // 用户密码
		char   szMacAddress[MACADDR_MAXSIZE];   // 网卡地址
		char   szVerifyCode[MAX_VERIFYCODE_LEN];// 验证码
		BYTE   nLoginType;						// 登录模式 EMClientLoginType
        DWORD  dwSerialNO;                      // 增加客户端硬盘序列号信息
        DWORD  dwPartnerID;                     // 合作伙伴ID
        bool   bForceLogin;                     // 是否强制登录

		SMsgLoginServer_Login()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 扩展的登陆信息包
	struct SMsgLoginServer_LoginEx : public SMsgLoginServer_Login
	{
		//DWORD  dwPartnerID;                     // 合作伙伴ID
		//DWORD dwSerialNO;                       // add by lgf 10.10.12 增加客户端硬盘序列号信息
		DWORD dwSize;						    // 后续扩展数据长度 联运需求
	};

    // MSG_LOGINSERVER_UPDATE_ACTOR
	struct SMsgLoginServer_UpdateActor
	{
        struct ActorInfo
        {
            DWORD dwWorldID;                                      // 当前角色所属的游戏世界ID
            DWORD dwActorID;                                      // 角色ID
            char  szActorName[DBDEF_ACTORNAME_MAXSIZE];           // 角色名
            WORD  nHeroID;                                        // 静态英雄ID
            WORD  nSkinID;                                        // 静态皮肤ID
            WORD  nLevel;                                         // 静态角色等级
            bool  bSex;                                           // 性别
        };

        DWORD       dwLoginCode;                    // 登录返回码,0表示成功
        int         nReason;                        // 更新的原因LOGIN_UPDATE_REASON_XXX
        DWORD       dwGameWorldID;                  // 区ID
        UDBID       udbid;                          // 唯一的账号ID
        WORD        nPlayerCount;                   // 角色数
        ActorInfo   players[MAX_ACTOR_NUM];         // 角色数据
        char        szUserName[USERNAME_MAXSIZE];   // 账号名称
        WORD        wMsgLen;                        // 登录提示信息长度
        // wMsgLen * char                           // 登录提示信息

		SMsgLoginServer_UpdateActor(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


    // MSG_LOGINSERVER_CREATE_ACTOR
	struct SMsgLoginServer_CreateActor
	{
		char  szActorName[ACTORNAME_MAXSIZE];       // 角色名
		bool  bSex;                                 // 性别
        WORD  nHeroID;                              // 英雄ID
		WORD  nSkinID;                              // 皮肤ID

        SMsgLoginServer_CreateActor(void)
        {
            memset(this, 0, sizeof(*this));
        }
	};
    // MSG_LOGINSERVER_CREATE_ACTOR_EXTEND
    struct SMsgLoginServer_CreateActor_Extend
    {
        WORD  nHeroID;                              // 英雄ID
        WORD  nSkinID;                              // 皮肤ID

        SMsgLoginServer_CreateActor_Extend(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // MSG_LOGINSERVER_CREATE_RESULT
    struct SMsgLoginServer_CreateActor_Result
    {
        BYTE  nReason;                              // 参考 LOGIN_RESULT_CODE
        int   nFlag;                                // 标识位，备用

        SMsgLoginServer_CreateActor_Result(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // MSG_LOGINSERVER_DELETE_ACTOR
	struct SMsgLoginServer_DeleteActor
	{
		char szActorName[ACTORNAME_MAXSIZE];	    // 要删除的角色名
		char szEstatePassword[PASSWORD_MAXSIZE];    // 财产保护密码,用ActorInfo做密钥使用RC4加密再发过来
		char szSupperPassword[MAX_SUPER_PASS_LEN];  // 超级密码长度,用ActorInfo做密钥使用RC4加密再发过来
	};

    
    // MSG_LOGINSERVER_SELECT_ACTOR
	struct SMsgLoginServer_SelectActor
	{
        char szActorName[ACTORNAME_MAXSIZE];  // 要删除的角色名
	};


    // MSG_LOGINSERVER_SWITCH_STATE
	struct SMsgLoginServer_SwitchState
	{
		unsigned long  state;                // 要切换到哪个状态(见前面的登陆状态定义CLIENT_CONN_STATE_LOGIN)
	};


    //  MSG_LOGINSERVER_QUEUE_ORDER
	struct SMsgLoginServer_QueueOrder
	{
		unsigned long RemainTime;
		unsigned long order;                // 在队列中的位置,=0表示移出了队列
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	/**************************** 登陆服务器和场景服务器之间的通讯协议 ******************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////

    // MSG_LOGINSERVER_ENTER_GAMEWORLD
	// 用户帐号信息,此信息由登陆服传递给场景服务器
	struct UserAccountInfo
	{
		bool				bIsAdult;				  // 是否成年,0:未成年，1;已成年
		DWORD				dwIdCard;				  // 身份证号（由数据库生成的身份证替代号）
		DWORD				dwPassportFlags;		  // 通行证相关标识 2009.09.05,标志位定义见IGameDatabaseDef.h
		DWORD				dwAppealFlags;		      // 申诉相关标识 2009.09.05,标志位定义见IGameDatabaseDef.h
		DWORD				dwServiceFlags;			  // 服务标识: 0x1被推荐用户;0x2推荐用户
		int					nPassProtectTime;		  // 密保绑定时间
	};

	// YY联运会员数据
	struct YYVipAccountInfo
	{
		int					nVipCode;				// vip会员码 0 :用户是vip; 1 : 非vip; 4 :认证失败;5 :其他错误
		DWORD				dwVipExpireTime;		// vip到期时间
		int					nVipGrade;				// vip会员等级grade会员等级code=0时该值有效，否则无效:grade >= 1 and grade <= 7（以实际返回值为准）
		int					nVipType;				// vip会员类型 0月费 1年费 2季费
		bool				bCheckedData;			// 是否已经请求数据过

		YYVipAccountInfo()
		{
			nVipCode = 1;
			dwVipExpireTime = 0;
			nVipGrade = 0;
			nVipType = 0;
			bCheckedData = false;
		}
	};

	// 场景服务器传递给登陆服务器的信息
	struct SMsgLoginServer_EnterGameWorld : public UserAccountInfo
	{
		DWORD    dwActorDBID;               // 角色数据库ID
		DWORD    dwClientID;                // 客户端ID(网关)
		DWORD    dwUDBID;                   // 账号数据库ID      
		char     szMacAddress[24];          // 网卡地址
		char     szIP[32];				    // IP地址
		DWORD	 nEnterMapID;			    // 准备进入地图
		Vector3	 vEnterTile;			    // 准备进入位置

		LONGLONG stid;					    // 跨区时的STID
		int      nPartnerID;			    // 游戏合作商ID，直通 add by lgf 11.04.01
		int		 nBirthday;				    // 玩家出生日期 格式：20110622，其中0表示玩家没有实名认证，没有出生日期 
        char	 szUserName[ACTORNAME_MAXSIZE];	// 用户名
		///// 附加信息
		int		 nTotalPay;						// 总充值金

		SMsgLoginServer_EnterGameWorld()
		{
			memset(this, 0, sizeof(*this));
		}

	};

    // define MSG_LOGINSERVER_RECONNECT
	struct SMsgLoginServer_Reconnect
	{
		DWORD    dwClientID;              // 客户端ID(网关)
		DWORD    dwServerID;              // 服务器ID
	};

    // MSG_LOGINSERVER_KICKOUT
	struct SMsgLoginServer_Kickout
	{
		DWORD  dwUDBID;                   // 用户ID
		WORD   wMsgLen;                   // 踢人提示信息长度
		char   szMessage[];               // 踢人提示信息内容
	};


    // MSG_LOGINSERVER_REKICKOUT
	struct SMsgLoginServer_ReKickout
	{
		DWORD  dwUDBID;                   // 用户ID
		int	   nReturn;					  // 0代表踢人成功，other代表踢人失败
	};


	// MSG_LOGINSERVER_REMOVEANTIADDICT
	struct SMsgLoginServer_RemoveAntiaddict
	{
		DWORD dwClientID;
	};

    // MSG_LOGINSERVER_SERVERCHECKREQ
	struct SMsgLoginServer_ServerCheck
	{
		int nSerial;		// 序号
		int nGameWorldID;	// 游戏世界ID
		int nBeginTick;		// 检测开始的TICK数
		int nEndTick;		// 检测结数的TICK数
		WORD wServerType;	// 服务器类型
		WORD wServerNo;		// 服务器编号（同类型时）
		int nCheckTime;		// 开始检测采集的时间点
		int nServerID;		// 服务器ID
		SMsgLoginServer_ServerCheck()
		{
			memset(this, 0, sizeof(*this));
		}
	};

    // MSG_LOGINSERVER_ANNOUNCE
	struct SMsgLoginServer_Announce 
	{
		char szAnnounce[512];

		SMsgLoginServer_Announce()
		{
			memset(this,0,sizeof(*this));
		}
	};

    // MSG_LOGINSERVER_SILENCEREFRESH_PROGRESS
	struct  SMsgLoginServer_SilenceRefresh_Progress
	{
		WORD   wNeedTime;                 // 预计需要时间
		char   szMessage[128];            // 信息提示
		SMsgLoginServer_SilenceRefresh_Progress()
		{
			memset(this,0,sizeof(*this));
		}
	};

    // MSG_LOGINSERVER_ACTOR_SECUREINFO
	struct SMsgLoginServer_Actor_SecureInfo
	{
		DWORD  dwPassportFlags;						
		BYTE   byGestrueFlags;
		BYTE   byEncryptMethod;						// 加密方法,参考前面的加密方法定义
		char   szKeyData[ENCRYPT_KEY_LEN];			// 公钥内容
		ulong  nCharKey;
		BYTE   nTryPlayFlag;						// 试玩标识(0正常，1试玩)
	};

    // MSG_LOGINSERVER_PASSPORT_LYCHECKINFO
	struct SMsgLoginServer_Passport_CheckInfo
	{
		int	nPartnerID;							 // 合作商ID
		ulong	nClientID;							 // 客户端ID
		BYTE	nMsgType;							 // 信息类型
		DWORD   dwSize;								 // 数据长度
		SMsgLoginServer_Passport_CheckInfo(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


    // MSG_LOGINSERVER_REGISTERACCOUNT
	struct SMsgLoginServer_RegisterAccount
	{
		DWORD	dwPartnerID;
		char			szAccount[USERNAME_MAXSIZE];	// 账号
		char			szPassword[PASSWORD_MAXSIZE];// 密码
		char			szRealyName[USERNAME_MAXSIZE];// 真实姓名
		char			szID[USERNAME_MAXSIZE];// 身份证号码
		char			szVertifyCode[8];// 验证码
		char			szMacAddress[MACADDR_MAXSIZE];		// MAC地址
		char			szRadID[32];									//广告ID，默认为客户端
		char			szRSID[32];										//客户端子站点  

		SMsgLoginServer_RegisterAccount(void)
		{
			memset(this,0,sizeof(*this));
		}
	};

    // MSG_LOGINSERVER_REGISTERACCOUNT_RESULT
	struct SMsgLoginServer_RegisterAccountResult
	{
		DWORD   dwUserID;       // 账号ID
		char    szContext[256]; // 描述

		SMsgLoginServer_RegisterAccountResult(void)
		{
			memset(this,0,sizeof(*this));
		}
	};

    // MSG_LOGINSERVER_SEND_GAMEAREA_INFO
	struct SMsgLoginServer_GameAreaInfo
	{
		DWORD dwLaunchTime;		    // 开区时间time(NULL)
		int   nCurLvCivilGrade;	    // 世界文明等级（人物）
		BYTE  bMobileNo;		    // 是否有开区订阅
		SMsgLoginServer_GameAreaInfo(void)
		{
			memset(this,0,sizeof(*this));
		}
	};

    // MSG_LOGINSERVER_REQUEST_ACTOR_COUNTS
    struct SMsgLoginServer_RequestActorCountsResultNode
    {
        DWORD	 dwWorldID;									// 游戏世界ID
        DWORD	 dwCounts;									// 角色数
    };

    // MSG_LOGINSERVER_REQUEST_ACTOR_COUNTS_RESULT
    struct SMsgLoginServer_RequestActorCountsResult_LC
    {
        DWORD dwCounts;
        // dwCounts * SMsgLoginServer_RequestActorCountsResultNode

        SMsgLoginServer_RequestActorCountsResult_LC(void)
        {
            memset(this,0,sizeof(*this));
        }
    };

    // 存放名称的结构
    struct SMsgLoginServer_SearchActorNameInfo 
    {
        char szName[ACTORNAME_MAXSIZE];
    };
    // MSG_LOGINSERVER_SEARCH_ACTORNAMEEX   获取角色名称
	struct SMsgLoginServer_SearchActorName
	{
		BYTE	nSex;								// 性别
		char	szOldName[ACTORNAME_MAXSIZE];		// 上次获取角色名称
		BYTE	nOldSex;							// 上次获取角色名称对应性别
		BYTE	nFlag;								// 0.获取新的角色名称（角色名不为空时先需返还）；1.返还后不需要再获取

		SMsgLoginServer_SearchActorName(void)
		{
			memset(this,0,sizeof(*this));
		}
	};

    // MSG_LOGINSERVER_SEARCH_ACTORNAMEEX_RESULT		角色名称关键词搜索结果
	struct SMsgLoginServer_SearchActorNameResult
	{
		BYTE	nSex;								// 性别
		char	szName[ACTORNAME_MAXSIZE];			// 名称

		SMsgLoginServer_SearchActorNameResult(void)
		{
			memset(this,0,sizeof(*this));
		}
	};

    // 请求查询角色续完信息 MSG_LOGINSERVER_REQ_QUERY_ACTOR_CONTINUE_PLAY_INFO,
    struct SMsgLoginServer_QueryActorContinuePlayInfo
    {
        Gateway::ClientID client;
        DWORD dwActorID;

        SMsgLoginServer_QueryActorContinuePlayInfo()
        {
            memset(this, 0, sizeof(*this));
        }
    };


    // 请求查询角色续完信息返回 MSG_LOGINSERVER_RET_QUERY_ACTOR_CONTINUE_PALY_INFO,
    struct SMsgLoginServer_RetQueryActorContinuePlayInfo
    {
        Gateway::ClientID client;
        DWORD dwActorID;        // 角色ID
        bool bInWar;
        DWORD dwGameWorldID;    // 游戏世界ID
        DWORD dwZoneServerID;  // 续完场景服节点
        int  nSceneID;         // 续完场景ID
        int  nSceneSession;

        SMsgLoginServer_RetQueryActorContinuePlayInfo()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // MSG_LOGINSERVER_SERVER_MSG_INFO
    struct SMsgLoginServer_ServerMsgInfo
    {
        char	szMessage[GAME_LONGSTR_MAXSIZE];			// 文字信息

        SMsgLoginServer_ServerMsgInfo()
        {
            memset(this, 0, sizeof(*this));
        }
    };

} // rkt namespace

#pragma pack()

#endif//__INTERFACE_LOGIN_SERVER_PROTOCOL_H__