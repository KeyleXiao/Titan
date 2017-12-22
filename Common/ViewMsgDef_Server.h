/*******************************************************************
** 文件名:	ViewMsgDef_Server.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/29
** 版  本:	1.0
** 描  述:	观战系统消息结构体定义（服务器与服务器之间的消息）
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once
#include "ViewMsgDef.h"
#include "IByteRecord.h"
#include "IMsg.h"
#include "ViewDef_.h"
#include <vector>
#include "ISerializableData.h"

#pragma pack(1)

/*****************************************************************/
///////////////////////// 观战模块 的消息结构体////////////////////
// 上层的消息码必定是 MSG_MODULEID_VIEW
/*****************************************************************/
// 消息格式为 SMsgView_ + 起止节点 + 消息名
// 起止节点定义如下：
// GM	网关——管理服
// GV	网关——观战服
// VG	观战服——网关
// VM	观战服——管理服
// MG	管理服——网关
// MV	管理服——观战服
// SM	游戏服——管理服
// MS	管理服——游戏服

///////////////////////////////////////////////////////////////////
// 握手消息 ENUM_MSG_VIEW_HANDSHAKE
// 观战服与管理服握手
struct SMsgView_VM_Handshake : public IFixMsg<SMsgView_VM_Handshake>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	DWORD	dwServerID;		// ServerID
	DWORD	dwMaxCount;		// 能承载的最大人数
	DWORD	dwCurrCount;	// 当前人数
};
struct SMsgView_GM_Handshake : public IFixMsg<SMsgView_GM_Handshake>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	//DWORD	dwServerID;		// ServerID
	WORD	wClientPort;	// 客户端监听端口
	WORD	wViewPort;		// 该Gate监听ViewServer的端口
	DWORD	dwMaxCount;		// 能承载的最大人数
	DWORD	dwCurrCount;	// 当前人数
};
struct SMsgView_VG_Handshake : public IFixMsg<SMsgView_VG_Handshake>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	DWORD	dwServerID;	// ServerID
	DWORD	dwID;		// Mng为我生成的动态ID
};
struct SMsgView_SM_Handshake : public IFixMsg<SMsgView_SM_Handshake>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_UNKNOW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	// 空消息
};


///////////////////////////////////////////////////////////////////
// 回应握手消息 ENUM_MSG_VIEW_HANDSHAKE_RESPONSE
// 管理服回复观战服
struct SMsgView_MV_HandshakeResponse : public IFixMsg<SMsgView_MV_HandshakeResponse>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	DWORD	dwID;	// Mng为你生成的动态ID
};
struct SMsgView_MG_HandshakeResponse : public IFixMsg<SMsgView_MG_HandshakeResponse>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	DWORD	dwID;	// Mng为你生成的动态ID
};
struct SMsgView_GV_HandshakeResponse : public IFixMsg<SMsgView_GV_HandshakeResponse>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	DWORD	dwID;	// Mng为Gate生成的动态ID
};
struct SMsgView_MS_HandshakeResponse : public IFixMsg<SMsgView_MS_HandshakeResponse>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_UNKNOW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	DWORD	dwID;	// Mng为你生成的动态ID
};


///////////////////////////////////////////////////////////////////
// 心跳检查	ENUM_MSG_VIEW_KEEPALIVE
struct SMsgView_VM_KeepAlive : public IFixMsg<SMsgView_VM_KeepAlive>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }

	// 空消息
};
struct SMsgView_GM_KeepAlive : public IFixMsg<SMsgView_GM_KeepAlive>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }

	// 空消息
};
struct SMsgView_VG_KeepAlive : public IFixMsg<SMsgView_VG_KeepAlive>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }

	// 空消息
};
FixMsgStruct(SMsgView_SM_KeepAlive)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_UNKNOW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }

	// 空消息
};


///////////////////////////////////////////////////////////////////
// 回应心跳检查	ENUM_MSG_VIEW_ANSWER_KEEPALIVE
struct SMsgView_MV_Answer_KeepAlive : public IFixMsg<SMsgView_MV_Answer_KeepAlive>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// 空消息
};
struct SMsgView_MG_Answer_KeepAlive : public IFixMsg<SMsgView_MG_Answer_KeepAlive>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// 空消息
};
struct SMsgView_GV_Answer_KeepAlive : public IFixMsg<SMsgView_GV_Answer_KeepAlive>
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// 空消息
};
FixMsgStruct(SMsgView_MS_Answer_KeepAlive)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_UNKNOW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// 空消息
};


///////////////////////////////////////////////////////////////////
// 所有网关的信息 ENUM_MSG_VIEW_GATES_INFO
struct SGateInfo
{
	std::string	strIp;
	WORD		wPort;
	WORD		wID;
};
typedef	std::vector<SGateInfo*> VecGateInfo;
struct SMsgView_MV_GatesInfo : public ISerializableData
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_GATES_INFO; }

	VecGateInfo		vecGates;

	virtual ~SMsgView_MV_GatesInfo()
	{
		auto count = vecGates.size();
		for (int i = 0; i < count; ++i)
		{
			SGateInfo*& pInfo = vecGates.at(i);
			safeDelete(pInfo);
		}
		vecGates.clear();
	}

	// 反序列化
	virtual void fromBytes(const char* p, size_t len) override
	{
		ibuffer ibufData(p, len);
		WORD count;
		ibufData >> count;

		for (int i = 0; i < count; ++i)
		{
			SGateInfo* pInfo = new SGateInfo;
			ibufData >> pInfo->strIp >> pInfo->wPort >> pInfo->wID;
			vecGates.push_back(pInfo);
		}
	}

	// 序列化
	virtual void toBytes(obuf& obufData) const override
	{
		WORD count = (WORD)vecGates.size();
		obufData << count;

		for (int i = 0; i < count; ++i)
		{
			SGateInfo* pInfo = vecGates.at(i);
			obufData << pInfo->strIp << pInfo->wPort << pInfo->wID;
		}
	}
};


///////////////////////////////////////////////////////////////////
// 请求观看某场战报	ENUM_MSG_VIEW_REQUEST_REPLAY
// 回复给Game服的结果枚举
FixMsgStruct(SMsgView_SM_RequestReplay)
{
	BYTE GetSrcEndPoint() { return MSG_MG_ENDPOINT_UNKNOW; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_REPLAY; }

	PlayerID	dwPlayerID;	// 标识观战客户端的唯一ID
	ReplayID	rID;		// 战报ID
	WORD		wRightID;	// 权限ID
};


///////////////////////////////////////////////////////////////////
// 请求观看某场战报 的回复——失败	ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL
enum EnumResultRequestReplay
{
	E_RESULT_RR_SUCCESS = 0,			// 成功
	E_RESULT_RR_ERR_NO_VIEW_SERVER,		// 没有ViewServer在线
	E_RESULT_RR_ERR_VIEW_IS_FULL,		// 所有ViewServer满载
	E_RESULT_RR_ERR_GATE_IS_FULL,		// 所有Gate满载
	E_RESULT_RR_ERR_REQUEST_REPEATED,	// 重复申请

	E_RESULT_RR_MAX,
};
FixMsgStruct(SMsgView_MS_RequestReplay_Fail)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_UNKNOW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL; }

	EnumResultRequestReplay		eResult;	// 结果枚举，只可能是各种错误码
	SMsgView_SM_RequestReplay	reqMsg;		// 之前Game服发过来的信息
};


///////////////////////////////////////////////////////////////////
// 请求观看某场战报 的回复——成功	ENUM_MSG_VIEW_REQUEST_REPLAY_SUCCESS
struct SMsgView_MS_RequestReplay_Success : public ISerializableData
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_UNKNOW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_REPLAY_SUCCESS; }

	SMsgView_SM_RequestReplay	reqMsg;		// 之前Game服发过来的信息
	BYTE	szKeyData[VIEW_SECRET_KEY_SIZE];// 秘钥
	std::string	strIp;	// Gate的IP
	WORD		wPort;	// Gate的端口

	virtual void fromBytes(const char* p, size_t len) override
	{
		ibuffer ibufData(p, len);
		ibufData >> reqMsg;
		ibufData.pop(szKeyData, VIEW_SECRET_KEY_SIZE);
		ibufData >> strIp >> wPort;
	}


	virtual void toBytes(obuf& obufData) const override
	{
		obufData << reqMsg;
		obufData.push_back(szKeyData, VIEW_SECRET_KEY_SIZE);
		obufData << strIp << wPort;
	}

};


///////////////////////////////////////////////////////////////////
// 请开始加载某场战报	ENUM_MSG_VIEW_START_VIEW
FixMsgStruct(SMsgView_MV_StartView)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_START_VIEW; }

	ReplayID	rID;		// 战报ID
};


///////////////////////////////////////////////////////////////////
// ID为XX的玩家将要来看某局比赛，在某台View上，请你接待，这是秘钥...。超时时间为T	ENUM_MSG_VIEW_REQUEST_ADD_PLAYER
FixMsgStruct(SMsgView_MG_RequestAddPlayer)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_ADD_PLAYER; }

	SMsgView_SM_RequestReplay	reqMsg;		// 之前Game服发过来的信息
	ViewID	wViewID;						// ViewServerID
	BYTE	szKeyData[VIEW_SECRET_KEY_SIZE];// 秘钥
	DWORD	dwTimeOut;						// 超时时间
};


///////////////////////////////////////////////////////////////////
enum EnumResultRequestAddPlayer
{
	E_RESULT_RAP_SUCCESS = 0,		// 成功
	E_RESULT_RAP_ERR_PLAYER_EXIST,	// 该ID玩家已存在于本Gate，且已登陆
	E_RESULT_RAP_ERR_NOT_REACH_VIEW,// 该ViewServer我连接不上(即使是内网，也有可能出现某几台机器之间连接不上的情况)

	E_RESULT_RAP_MAX,
};
// Gate回复Mng，添加玩家结果	ENUM_MSG_VIEW_REQUEST_ADD_PLAYER_REPLY
FixMsgStruct(SMsgView_GM_RequestAddPlayerReply)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_REQUEST_ADD_PLAYER_REPLY; }

	EnumResultRequestAddPlayer	eResult;	// 结果枚举
	PlayerID					dwPlayerID;	// 标识观战客户端的唯一ID
};


///////////////////////////////////////////////////////////////////
// 某玩家的验证信息删除	ENUM_MSG_VIEW_AUTH_DEL
enum EnumAuthDelReason
{
	// Gate通知Mng
	E_AuthDelR_HANDSHAKE,		// 握手登陆成功
	E_AuthDelR_TIME_OUT,		// 超时了
	E_AuthDelR_NOT_REACH_VIEW,	// 该ViewServer我连接不上了（虽然之前连得上）

	// Mng通知Gate
	E_AuthDelR_USER_CANCEL,		// 玩家取消
	E_AuthDelR_MNG_CANCEL,		// Mng取消(多半是由于Mng与Gate的连接断开过)

	E_AuthDelR_MAX,
};
FixMsgStruct(SMsgView_GM_AuthDel)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_AUTH_DEL; }

	PlayerID			dwPlayerID;	// 标识观战客户端的唯一ID
	EnumAuthDelReason	eReason;	// 原因
};
FixMsgStruct(SMsgView_MG_AuthDel)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWMNG; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_AUTH_DEL; }

	PlayerID			dwPlayerID;	// 标识观战客户端的唯一ID
	EnumAuthDelReason	eReason;	// 原因
};


///////////////////////////////////////////////////////////////////
//通知View，某Client要观看某战报，以某种权限	ENUM_MSG_VIEW_C_REQUEST_REPLAY
FixMsgStruct(SMsgView_GV_CRequestReplay)
{
	BYTE GetSrcEndPoint() { return MSG_ENDPOINT_VIEWGATE; };
	BYTE GetDestEndPoint() { return MSG_ENDPOINT_VIEW; };
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_C_REQUEST_REPLAY; }

	SMsgView_SM_RequestReplay	reqMsg;		// 之前Game服发过来的信息
};


#pragma pack()