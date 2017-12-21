/*******************************************************************
** 文件名:	ViewMsgDef_Client.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/29
** 版  本:	1.0
** 描  述:	观战系统消息结构体定义（客户端与服务器之间的消息）
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "IMsg.h"
#include "ViewMsgDef.h"
#include "buffer.h"
#include "ViewDef_.h"


#pragma pack(1)
/*****************************************************************/
///////////////////////// 观战模块 的消息结构体////////////////////
// 上层的消息码必定是 MSG_MODULEID_VIEW
/*****************************************************************/
// 消息格式为 SMsgView_ + 起止节点 + 消息名
// 起止节点定义如下：
// CG	Client——网关
// CM	Client——管理服
// CV	Client——观战服


///////////////////////////////////////////////////////////////////
// 握手消息 ENUM_MSG_VIEW_HANDSHAKE
// 客户端与网关握手
struct SMsgView_CG_Handshake : public IFixMsg<SMsgView_CG_Handshake>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE; }

	PlayerID	dwPlayerID;						// PlayerID
	BYTE		szKey[VIEW_SECRET_KEY_SIZE];	// 用于验证的Key
	char		szMacAddress[MAC_ADDRESS_SIZE];	// 网卡地址
};

///////////////////////////////////////////////////////////////////
// 回应握手消息 ENUM_MSG_VIEW_HANDSHAKE_RESPONSE
// 管理服回复观战服
struct SMsgView_GC_HandshakeResponse : public IFixMsg<SMsgView_GC_HandshakeResponse>
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_RESPONSE; }

	// 空消息
};

///////////////////////////////////////////////////////////////////
// Gate回复Client，登陆失败	ENUM_MSG_VIEW_HANDSHAKE_FAIL
enum EnumHandshakeFail
{
	E_HANDSHAKE_FAIL_NO_ID = 0,		// 这里没有你的ID
	E_HANDSHAKE_FAIL_NOT_REACH_VIEW,// 该ViewServer我连接不上(即使是内网，也有可能出现某几台机器之间连接不上的情况)
	E_HANDSHAKE_FAIL_ERR_KEY,		// key错误
	E_HANDSHAKE_FAIL_IS_FULL,		// 服务器满员

	E_HANDSHAKE_FAIL_MAX,
};
FixMsgStruct(SMsgView_GC_HandshakeFail)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_HANDSHAKE_FAIL; }

	// 空消息
	EnumHandshakeFail	eFailReason;
};


///////////////////////////////////////////////////////////////////
// 心跳检查	ENUM_MSG_VIEW_KEEPALIVE
FixMsgStruct(SMsgView_CG_KeepAlive)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_KEEPALIVE; }
	// 空消息
};


///////////////////////////////////////////////////////////////////
// 回应心跳检查	ENUM_MSG_VIEW_ANSWER_KEEPALIVE
FixMsgStruct(SMsgView_GC_Answer_KeepAlive)
{
	static BYTE	GetModuleId() { return MSG_MODULEID_VIEW; }
	static BYTE	GetActionId() { return ENUM_MSG_VIEW_ANSWER_KEEPALIVE; }
	// 空消息
};

#pragma pack()