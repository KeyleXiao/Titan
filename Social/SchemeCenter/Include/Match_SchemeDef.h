/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\Match_SchemeDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/18/2015	16:36
** 版  本:	1.0
** 描  述:	
            比赛 文件数据定义
********************************************************************/

#pragma once

#include <string>
#include "MatchSchemeDef.h"

// 
// // 比赛大厅房间的游戏地图配置
// #define D_MatchRoomMap_SCHEME_FILENAME "Scp\\MatchRoomMap.xml"
// struct S_SchemeMatchRoomMap
// {
// 	// 地图
// 	T_MDMapID	m_MapID;
// 	// 地图名称
// 	char		m_szName[GAME_NAME_MAXSIZE];	
// 	// 地图描述
// 	char		m_szDesc[GAME_MSGSTR_MAXSIZE];	
// 	// 创建时的地图图片显示路径
// 	char		m_szPicPath[GAME_MSGSTR_MAXSIZE];	
// 	// 地图阵营数
// 	BYTE		m_u8CampCount;
// 	// 支持的模式 
// 	bool	m_Modes[MatchRoomModes];
// 
// 	// 阵营的出生的坐标点
// 	vector<vector<Vector3> > m_CampBornPos;
// };
// class I_SchemeMatchRoomMap
// {
// public:
// 	// 取得配置信息
// 	virtual S_SchemeMatchRoomMap * GetSchemeInfo(T_MDMapID m_MapID) = 0;
// };
// 
// 
// // 比赛大厅房间的游戏模式配置
// #define D_MatchRoomMode_SCHEME_FILENAME "Scp\\MatchRoomMode.xml"
// struct S_SchemeMatchRoomMode
// {
// 	// 模式ID
// 	T_MDModeID	m_ModeID;
// 	// 每个阵营最大人数
// 	BYTE		m_CampRoleMax;
// 	// 选人倒计时秒
// 	BYTE		m_SelectHeroTime;	
// 	// 选人确定后缩短倒计时到几秒
// 	BYTE		m_ConfirmHeroTime;	
// 
// 	// 那些状态(E_RoomState位或)可以换阵营
// 	bool		m_RoomStateChangeCamp[ERSs];
// 	// 那些状态(E_RoomState位或)可以加入房间
// 	bool		m_RoomStateEnter[ERSs];
// 	// 同一英雄在一个阵营中最多能有几个
// 	BYTE		m_CampSameHeroCountMax;
// 	// 同一英雄在所有阵营中最多能有几个
// 	BYTE		m_SameHeroCountMax;
// };
// class I_SchemeMatchRoomMode
// {
// public:
// 	// 取得配置信息
// 	virtual S_SchemeMatchRoomMode* GetSchemeInfo(T_MDModeID ModeID) = 0;
// };
