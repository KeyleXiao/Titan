/*******************************************************************
** 文件名:	ViewMsgDef.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/29
** 版  本:	1.0
** 描  述:	观战系统消息号定义
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "buffer.h"
#include "Common.h"
#include "NetMessageDef.h"
#include "DManagerModule.h"

#pragma pack(1)
/*****************************************************************/
///////////////////// 观战服务器模块 的消息码//////////////////////
// 1.上层的消息码必定是 MSG_MODULEID_VIEW
// 2.某些消息号是通用的。比如HANDSHAKE，既作为客户端到网关的握手消息号，
// 也作为服务器之间握手的消息号
/*****************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM_MSG_VIEW_ + 消息码名
enum EMMsgView
{
	ENUM_MSG_VIEW_INVALID = 0,

	// 公共消息
	ENUM_MSG_VIEW_HANDSHAKE,				// 握手消息
	ENUM_MSG_VIEW_HANDSHAKE_RESPONSE,		// 回应握手消息
	ENUM_MSG_VIEW_KEEPALIVE,				// 心跳检查
	ENUM_MSG_VIEW_ANSWER_KEEPALIVE,			// 回应心跳检查

	// GATE——VIEW
	ENUM_MSG_VIEW_C_REQUEST_REPLAY,			//通知View，某Client要观看某战报，以某种权限

	// MNG——VIEW
	ENUM_MSG_VIEW_GATES_INFO,				// 所有网关的信息
	ENUM_MSG_VIEW_START_VIEW,				// 请开始加载某场战报

	// MNG——GATE
	ENUM_MSG_VIEW_REQUEST_ADD_PLAYER,		// ID为XX的玩家将要来看某局比赛，在某台View上，请你接待，这是秘钥...。超时时间为T
	// GATE——MNG
	ENUM_MSG_VIEW_REQUEST_ADD_PLAYER_REPLY,	// Gate回复Mng，添加玩家结果
	// MNG——GATE、GATE——MNG
	ENUM_MSG_VIEW_AUTH_DEL,					// 某玩家的验证信息删除
	// GATE——VIEW、GATE——MNG
	ENUM_MSG_VIEW_PLAYER_DEL,				// 删除玩家，即玩家下线

	// GAME——MNG
	ENUM_MSG_VIEW_REQUEST_REPLAY,			// 请求观看某场战报
	// MNG——GAME
	ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL,		// 请求观看某场战报 的回复——失败
	ENUM_MSG_VIEW_REQUEST_REPLAY_SUCCESS,	// 请求观看某场战报 的回复——成功


	// GATE——CLIENT
	ENUM_MSG_VIEW_HANDSHAKE_FAIL,			// Gate回复Client，登陆失败

	// VIEW——CLIENT
	ENUM_MSG_VIEW_BATTLE_PREP,				// 战报预备信息
	ENUM_MSG_VIEW_BATTLE_INFO,				// 战报信息

	//ENUM_MSG_VIEW_SENDDATA,						// 请求网关服务发送数据给各软件服务器
	//ENUM_MSG_VIEW_SERVERLIST_UPDATE,			// 服务器列表更新
	//ENUM_MSG_VIEW_DATA_SUBMSG,					// 语音子消息处理
	//ENUM_MSG_VIEW_SENDCHATMSG,					// 聊天消息
	//ENUM_MSG_VIEW_PERFORMANCE,					// 性能检测消息包

	//ENUM_MSG_VIEW_CHECK,						// 检测消息，用来跨服检测网络连接状态
	//ENUM_MSG_VIEW_CHECKBACK,					// 检测消息回复，用来跨服检测网络连接状态

	//// 语音网关消息

	//// 语音服务器
	//ENUM_MSG_VIEW_USERPARTINFO_UPDATE,			// 更新语音用户部分信息
	//ENUM_MSG_VIEW_ALL_WORLD_BROADCAST,			// 请求语音服帮助广播消息到所有区

	ENUM_MSG_VIEW_MAXID,	// 最大消息码
};

#pragma pack()