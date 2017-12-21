/*******************************************************************
** 文件名:	E:\speed\Server\MatchSceneServer\Include\MatchSceneDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	15:46
** 版  本:	1.0
** 描  述:	
            比赛大厅场景服 相关的定义
********************************************************************/

#pragma once

#define  MatchSceneDebug 

/*
// 消息ID定义
// MSS  MATCH_SCENE_SERVER
// C	Client
// MS	MATCH_SERVER

// Client -> MatchSceneServer
enum C_2_MATCH_SCENE_SERVER_NET_MESSAGE_KEYACTION
{
	// 客户端请求进入大厅
	C2MSS_MSG_ENTER_HALL = (NETMSG_OFFSET+1),
	// 创建房间
	C2MSS_MSG_CREATE_ROOM,

	C_2_MATCH_SCENE_SERVER_NET_MESSAGE_KEYACTIONs
};

// MatchSceneServer -> Client
enum MATCH_SCENE_SERVER_2_C_NET_MESSAGE_KEYACTION
{
	// 进入大厅结果
	MSS2C_MSG_ENTER_HALL = (NETMSG_OFFSET+1),		
	// 现有房间数据
	MSS2C_MSG_ROOM,
	// 创建房间
	MSS2C_MSG_CREATE_ROOM,

	MATCH_SCENE_SERVER_2_C_NET_MESSAGE_KEYACTIONs
};



// C2MSS_MSG_ENTER_HALL
struct SMsg_C2MSS_EnterHall
{
	// 选的英雄ID
	UINT8	m_HeroID;
};

// MSS2C_MSG_ENTER_HALL
struct SMsg_MSS2C_EnterHall
{
	// 是否成功进入
	bool	m_bOK;
};

// MSS2C_MSG_ROOM
struct SMsg_MSS2C_Room
{
	UINT16	m_RoomID;
	UINT8	m_MapID;
	UINT8	m_ModeID;
	UINT8	m_State;
	UINT8	m_RoleCount;
};

// C2MSS_MSG_CREATE_ROOM
struct SMsg_C2MSS_CreateRoom
{
	UINT8	m_uMapID;
	UINT8	m_uModeID;
};

// MSS2C_MSG_CREATE_ROOM
struct SMsg_MSS2C_CreateRoom
{
	UINT16	m_RoomID;
	UINT8	m_MapID;
	UINT8	m_ModeID;
};

		




*/