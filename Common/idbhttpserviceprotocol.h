/*******************************************************************
** 文件名:	IDBFacilityProtocol.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2010 - All Rights Reserved
** 创建人:	吕国锋（Richmond）
** 日  期:	05/18/2010
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#pragma pack(1)

namespace DBHttp
{
#define MAX_PARAM_LEN	32

	//////////////////////////////////////////////////////////////////////////
	/*
	@name:与数据库交互的协议
	@brief:DBFacilityInteract功能：
			1。侦听特定端口，接受数据库发过来的命令
			2。解析命令，转发或执行功能
	*/
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////////
	/**************************** 登陆服务器和数据库之间的通讯协议 **********************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// 加密算法定义
	enum
	{
		DBHTTP_ENCRYPT_NO               =  0,   // 不加密
		DBHTTP_ENCRYPT_RSA              =  1,   // RSA加密
		DBHTTP_ENCRYPT_RC4              =  2,   // RC4加密
		DBHTTP_ENCRYPT_CUSTOM       =  3,   // 公司内部自定义的加密方式
		DBHTTP_ENCRYPT_MAX              =  4,   
	};

	//服务器类型
	enum eServer
	{
		Server_UnKnow		= 0,
		Server_SCENE			= 1,
		Server_SOCIAL	,
		Server_LOGIN	,
		Server_DBHttp,
		Server_MAX,
	};

	//信息类型
	enum eValuetype
	{
		Valuetype_INT			= 1,
		Valuetype_STRING	,
		Valuetype_MAX,
	};

	//返回值定义
	enum eRet
	{
		Ret_NO			= 1,
		Ret_YES,
		Ret_MAX,
	};
	
	/***************************************************************/
	//////////////////////// 具体消息Action的全局定义 /////////////////////
	/***************************************************************/
	enum eAction
	{
		DBHttpAction_Notity = 1,
		DBHttpAction_SystemMessage,
		DBHttpAction_CallLuaFunction,
		DBHttpAction_NUM,
	};

	enum eSubAction
	{
		//DBHttpSubAction_Notity_DIDA= 1,                   // 弹出dida(server=2社会服)
		//DBHttpSubAction_Notity_SCROLL,                    // 掉卷轴(server=2社会服)
		DBHttpSubAction_Notity_KICKOUT              = 3,    // 踢人(server=2社会服)
		//DBHttpSubAction_Notity_ANNOUNCE,                  // 发公告(server=2社会服)
		//DBHttpSubAction_Notity_FENGHAO,                   // 封号(server=3登陆服)
		//DBHttpSubAction_Notity_NOTFENG,                   // 解封(server=3登陆服)
		DBHttpSubAction_Notity_GETEMAIL             = 7,    // 通知读取邮件(server=2社会服)
		//DBHttpSubAction_Notity_ChatMessage,               // 聊天(server=2社会服)
		DBHttpSubAction_Notity_ALLGetMail           = 9,    // 通知所有玩家读取邮件(server=2社会服)
		//DBHttpSubAction_Notity_Pay,                       // 通知该账号充值成功了
		//DBHttpSubAction_Notity_AddedService,              // 增值服务通告修改玩家属性成功类型(server=2社会服)

		DBHttpSubAction_Notity_GETUSERLIST          = 100,  // 取得玩家角色ID列表(server=2社会服)
		DBHttpSubAction_Notity_NUM,
	};

	/***************************************************************/
	//////////////////////// 消息头中"行为”的全局定义 /////////////////////
	/***************************************************************/
	//
#define MSG_DBHTTP2ZONE_REQUEST		1
	struct SMsgDBHttp_Request
	{
		DWORD action;				//行为
		DWORD  sub;
		DWORD  valuetype;			//类型
		DWORD num;					//玩家个数
	};

#define MSG_SOCIAL2DBHTTP_USERLIST 2

#define MSG_DBHTTP_USERLOGIN				3		//Social -> DBhttp
	struct SMsgDBHttp_UserLogin 
	{
		DWORD pdbid;
		LONGLONG	uid;
		DWORD clientid;
		DWORD svrid;
		DWORD worldid;
		SMsgDBHttp_UserLogin()
		{
			memset(this, 0, sizeof(*this));
		}
		SMsgDBHttp_UserLogin(DWORD pdbid_,DWORD svrid_)
		{
			memset(this, 0, sizeof(*this));
			pdbid = pdbid_;
			svrid = svrid_;
		}
	};

#define MSG_DBHTTP_USERLOGOUT			4	//Social -> DBhttp
	typedef SMsgDBHttp_UserLogin SMsgDBHttp_UserLogout;

//踢人
#define MSG_DBHTTP_KICKOUT			5	//Social -> Zone
#define  MAX_REASONBUFFER_SIZE	128
	struct SMsgDBHttp_Kickout
	{
		DWORD pdbid;
		char		szReason[MAX_REASONBUFFER_SIZE];
		SMsgDBHttp_Kickout()
		{
			memset(this, 0, sizeof(*this));
		}
	};

//数据库后台请求玩家读取邮件
#define MSG_DBHTTP_NOTITYGETEMAIL			6	//Social -> Zone
	struct SMsgDBHttp_NotityGetEmail
	{
		DWORD pdbid;
		SMsgDBHttp_NotityGetEmail()
		{
			memset(this, 0, sizeof(*this));
		}
	};

//数据库后台请求某国家所有玩家读取邮件
#define MSG_DBHTTP_NOTITYMULTIPLAYER_GETEMAIL			7	//Social -> Zone
#define	MAX_PLYARE_COUNT				100
	struct SMsgDBHttp_NotityMultiPlayersGetEmail
	{
        int nHttpRetValue;
        int nReserve;           // 保留数据

		SMsgDBHttp_NotityMultiPlayersGetEmail()
		{
			memset(this, 0, sizeof(*this));
		}
	};
// 
 //聊天
 #define MSG_DBHTTP_ChatMessageFromPlayer2OffLineGM			7	//Social -> DBhttp

// 社会服转发公告信息到场景服
#define MSG_DBHTTP_NOTIFYTRAN			8				//Social -> Zone
struct SMsgDBHttp_NotifyTrans
{
	DWORD	dwPDBID;				// 玩家角色ID
	DWORD	dwContentSize;			// 公告长度
	DWORD	dwParamSize;			// 参数长度
	// char szBuf[7000];	
	SMsgDBHttp_NotifyTrans()
	{
		memset(this, 0, sizeof(*this));
	}
};
	
	// add by lgf 12.03.31 转发其他服务器请求后台的url地址（可满足各种应用）
	#define MSG_DBHTTP_REQUESTURL								9		//Social -> DBhttp
	struct SMsgDBHttp_RequestUrl
	{
		DWORD	dwServerID;				// 当前服务器ID
		DWORD	dwSessionID;			// 会话ID

		//url ...
		SMsgDBHttp_RequestUrl()
		{
			memset(this, 0, sizeof(*this));
		}
	};
	struct SMsgDBHttp_ResponseUrl
	{
		DWORD	dwSessionID;			// 会话ID

		SMsgDBHttp_ResponseUrl()
		{
			memset(this, 0, sizeof(*this));
		}
	};
};

#pragma pack()