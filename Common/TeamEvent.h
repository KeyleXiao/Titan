//==========================================================================
/**
* @file	  : TeamEvent.h
* @author : 秦其勇
* created : 2016-5-16
* purpose : 组队事件定义
*/
//==========================================================================
#ifndef __DTeamEvent_H__
#define __DTeamEvent_H__

#include "TeamDef.h"
#include "EventRefBase.h"

/***************************************************************/
///////////////////// 组队事件码 SOURCE_TYPE_TEAM////////////////
/***************************************************************/
#pragma pack(1)


// 创建队伍
//EVENT_TEAM_CREATETEAM	
struct SEventTeamCreateTeam_O: public event_base
{
	int			nTeamID;
};

struct SEventTeamCreateTeam_S: public event_base
{	
	int			nTeamID;	
};

struct SEventTeamCreateTeam_C: public event_base
{
	int			nOldTeamID;
};

// 队员加入
//EVENT_TEAM_JOINTEAM
struct SEventTeamJoinTeam_O: public event_base
{
	DWORD		dwJoinPDBID;
};

struct SEventTeamJoinTeam_S : public event_base
{
	DWORD		dwJoinPDBID;
};

// EVENT_ENTITY_JOINTEAM
// 场景服（execute）
struct SEventPersonJoinTeam_S : public event_base
{
    int			nTeamID;			// 队伍ID
};

struct SEventPersonLeaveTeam_S : public event_base
{
    int			nTeamID;			// 队伍ID
};

struct SEventTeamJoinTeam_C: public event_base
{
	DWORD		dwJoinPDBID;
};

// 队员离开
//EVENT_TEAM_LEAVETEAM									
struct SEventTeamLeaveTeam_O: public event_base
{
	DWORD		dwLeavePDBID;
};

struct SEventTeamLeaveTeam_S: public event_base
{
	DWORD		dwLeavePDBID;
};

struct SEventTeamLeaveTeam_C: public event_base
{
	DWORD		dwLeavePDBID;
};

// 更新队长
//EVENT_TEAM_CAPTAIN										
struct SEventTeamCaptain_O: public event_base
{
	DWORD		dwOldCaptain;
	DWORD		dwNewCaptain;
};

struct SEventTeamCaptain_S: public event_base
{
	DWORD		dwOldCaptain;
};


struct SEventTeamCaptain_C: public event_base
{
	DWORD		dwOldCaptain;
};

// 更新拾取模式
//EVENT_TEAM_PICKMODE										
struct SEventTeamPickMode_C: public event_base
{
	int			nOldPickMode;
};

// 更新成员
//EVENT_TEAM_MEMBERCHANGE									
struct SEventTeamMemberChange_C: public event_base
{
	DWORD		dwPDBID;
};

struct SEventTeamMemberChange_O: public event_base
{
	DWORD		dwPDBID;
};

// 退出队伍
//EVENT_TEAM_QUITTEAM										
struct SEventTeamQuitTeam_C: public event_base
{

};

// 队伍解散
//EVENT_TEAM_DISBAND		
struct SEventTeamDisband_O: public event_base
{
	int			nTeamID;
};

struct SEventTeamDisband_S: public event_base
{
	int			nTeamID;
};

// 更新了队友位置
//EVENT_TEAM_POSTPLACE
struct SEventTeamPostPlace_C: public event_base
{

};

// 队友更新了模拟位置
//EVENT_TEAM_SIMULATETRACK
struct SEventTeamSimulateTrack_C: public event_base
{
	DWORD		dwPDBID;
	POINT		ptSimulateTile;	// 当前模拟位置
};


// 更新团长
//EVENT_GROUP_CAPTAIN
struct SEventGroupCaptain_O: public event_base
{
	DWORD		dwOldCaptain;
	DWORD		dwNewCaptain;
};

#pragma pack()

#endif