//==========================================================================
/**
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016.6.28
** 版  本:	1.0
** 描  述:	组队消息定义
*/
//==========================================================================

#ifndef __TEAMMSGDEF_H__
// __TEAMMSGDEF_H__

#include "DMirrorServer.h"
#include "TeamDef.h"
#include "GatewayServiceDef.h"
#include "TeamDef.h"
#include "NetMessageDef.h"
#include "IGameDatabaseDef.h"

using namespace Gateway;

#pragma once
#pragma pack(1)

template<typename TMsg>
bool getTeamMsg(TMsg*& pMsg, void*& pData, size_t& stLen)
{
    if (nullptr == pData)
    {
        WarningLn("getMsg nullptr == pData");
        return false;
    }
    if ( stLen < sizeof(TMsg) )
    {
        WarningLn("getMsg stLen < sizeof(TMsg) stLen=" << stLen << " sizeof(TMsg)=" << sizeof(TMsg));
        return false;
    }

    pMsg	= (TMsg *)pData;
    pData	= (void*)(pMsg+1);
    stLen	-= sizeof(TMsg);

    return true;
}


static SNetMsgHead*	getTeamMsgHead( PACKAGE_PTR msg, void*& pData, size_t& stLen )
{
    pData	= (void*)msg->c_str();
    stLen	= msg->size();

    SNetMsgHead* pMsg;
    if ( !getTeamMsg( pMsg, pData, stLen ) )
    {
        WarningLn("getMsgHead failed! !getMsg");
        return nullptr;
    }
    return pMsg;
}

// 消息ID定义
//////////////////////////////////////////////////////////////////////////
enum TEAM_NET_MESSAGE_KEYACTION
{
    MSG_TEAM_REQUESTJOIN,
    MSG_TEAM_INVITEDJOIN,			
    MSG_TEAM_BUILDFLOW,				
    MSG_TEAM_CREATETEAM,		
    MSG_TEAM_JOINMEMBER,			
    MSG_TEAM_LEAVEMEMBER,			
    MSG_TEAM_UPDATEMEMBER,			
    MSG_TEAM_CHANGECAPTAIN,			
    MSG_TEAM_PERSONLINESTATE,			
    MSG_TEAM_DISBANDTEAM,			
    MSG_TEAM_APPOINTCAPTAIN,			
    MSG_TEAM_KICKOUTMEMBER,
    MSG_TEAM_APPLYPICKMODE,
    MSG_TEAM_SYNCMEMBER_TOALLTEAM,
    MSG_TEAM_DATARESTORE,
    MSG_TEAM_APPLYDICEMODE,
    MSG_TEAM_CONSULTDICEMODE,
    MSG_TEAM_POSTPLACE,
    MSG_TEAM_FORCESYNCPLACE,
    MSG_TEAM_MEMBERDIESTATE,
    MSG_TEAM_MEMBERLOGIN,
    MSG_TEAM_MEMBERLOGOUT,
    MSG_TEAM_MEMBERCHANGEZONE,
    MSG_TEAM_GROUP_EVENT,
    MSG_TEAM_GROUP_CLIENT_EVENT,
    MSG_TEAM_GROUP_SERVER_EVENT,
    MSG_TEAM_GROUP_SOCIAL_EVENT,
    MSG_TEAM_SEND_TOTAL_DATA,
    MSG_TEAM_REQUEST_GROUP_MAPLOC,
    MSG_TEAM_SEND_GROUP_MAPLOC,
    MSG_TEAM_REQUEST_JOIN_TEAM,
    MSG_TEAM_REQUEST_JOIN_TEAM_RESULT,
    MSG_TEAM_APPLYVOTEKICK,
    MSG_TEAM_CONSULTKICKMEMBER,
    MSG_TEAM_CHANGEFLAG,
    TEAMSUBMSG_BUILDFLOW_INVITED,
    TEAMSUBMSG_BUILDFLOW_GETMIRROR,
    TEAMSUBMSG_BUILDFLOW_REQUEST,
    MSG_TEAM_CLAN_INVITEJOIN_TEAM,
};



/******************************************************************/
///////////////////////// 组队的消息码定义 /////////////////////////
// 上层的消息码必定是MSG_MODULEID_TEAM
/******************************************************************/
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// 申请加入
//	MSG_TEAM_REQUESTJOIN									1
// 消息体
struct SMsgTeamRequestJoin_CS
{
    DWORD		dwTargetPDBID;					// 目标PDBID
};

struct SMsgTeamRequestJoin_SO
{
    DWORD		dwTargetPDBID;					// 目标PDBID
    // STeamSyncPersonContext				// 自已的映像数据
};

///////////////////////////////////////////////////////////////////
// 邀请加入
// MSG_TEAM_INVITEDJOIN									2
// 消息体
struct SMsgTeamInvitedJoin_CS
{
    DWORD		dwTargetPDBID;					// 目标PDBID
};

struct SMsgTeamInvitedJoin_SO
{
    DWORD		dwTargetPDBID;					// 目标PDBID
    // STeamSyncPersonContext				// 自已的映像数据
};

///////////////////////////////////////////////////////////////////
// 组建队伍流程
// MSG_TEAM_BUILDFLOW										3
struct SMsgBuildFlowHead
{	
    DWORD		dwBuildingSN;				// 组建流水号
    DWORD		dwClientPDBID;				// 客户端PDBID

    DWORD		dwSubMsgID;					// 子消息码
};

////////////////////////////////////////////////////////////////
// 邀请加入
// TEAMSUBMSG_BUILDFLOW_INVITED	1
struct SSubMsgBuildFlowInvited_OC
{
    DWORD		dwCaptain;				// 队长PDBID

    DWORD		dwInviter;				// 邀请者

    int			nOffCount;				// 离线邀请人个数
    int			nOnCount;				// 在线邀请人个数

    // nOffCount * STeamMemberInfo		// 离线邀请人信息		
    // nOnCount * STeamSyncPersonContext// 在线邀请人信息
};

struct SSubMsgBuildFlowInvited_CS
{
    int			nResult;				// 响应结果
};

struct SSubMsgBuildFlowInvited_SO
{		
    int			nResult;				// 响应结果
    // STeamSyncPersonContext			// 自已的映像数据	
};

////////////////////////////////////////////////////////////////
// 获取队友映像数据
// TEAMSUBMSG_BUILDFLOW_GETMIRROR	2
struct SSubMsgBuildFlowGetMirror_OS
{
    DWORD		Member[TEAM_MEMBER_MAXCOUNT];
};

struct SSubMsgBuildFlowGetMirror_SO
{	
    // STeamSyncPersonContext			// 在线邀请人信息
};

////////////////////////////////////////////////////////////////
// 申请加入
// TEAMSUBMSG_BUILDFLOW_REQUEST	3
struct SSubMsgBuildFlowRequest_OC
{
    // 用来作界面的提示的，如果在队伍里：提示为“申请加入你的队伍”
    // 否则提示为“申请与您组队，同意后您将成为新队伍队长”
    bool		bSelfInTeamFlag;		// 自己是否在队伍里
    // STeamSyncPersonContext			// 申请人信息
};

struct SSubMsgBuildFlowRequest_CS
{
    int			nResult;				// 响应结果		
};

struct SSubMsgBuildFlowRequest_SO
{
    int			nResult;				// 响应结果		
    // STeamSyncPersonContext			// 自已的映像数据	
};

///////////////////////////////////////////////////////////////////
// 创建队伍
// MSG_TEAM_CREATETEAM										4
// 消息体
struct SMsgTeamCreateTeam_OS
{
    int			nTeamID;					// 队伍ID
    int			nPickMode;					// 拾取模式
    SDiceModeInfo diceinfo;					// 团队拾取设
};

///////////////////////////////////////////////////////////////////
// 队员加入
// MSG_TEAM_JOINMEMBER										5
// 消息体
struct SMsgTeamJoinMember_OS
{
    int			nTeamID;					// 队伍ID
    STeamMemberInfo member;					// 队友信息
};

struct SMsgTeamJoinMember_SO
{
    int			nTeamID;					// 队伍ID
    DWORD		dwPDBID;					// 玩家ID
    bool		bIsCaptain;					// 是否为队长
    STeamSyncPersonContext person;			// 映像数据
};


///////////////////////////////////////////////////////////////////
// 队员离开
// MSG_TEAM_LEAVEMEMBER									6
// 消息体
struct SMsgTeamLeaveMember_CS
{

};

struct SMsgTeamLeaveMember_SO
{
    int			nTeamID;					// 队伍ID
    DWORD		dwLeavePDBID;				// 离开PDBID
};

struct SMsgTeamLeaveMember_OS
{
    int			nTeamID;					// 队伍ID
    DWORD		dwLeavePDBID;				// 离开PDBID
    char		szName[GAME_NAME_MAXSIZE];	// 离开者的名字

    int			nLeaveReason;				// 离开原因	
};

struct SMsgTeamLeaveMember_SC
{
    int			nTeamID;					// 队伍ID
    DWORD		dwLeavePDBID;				// 离开PDBID
    char		szName[GAME_NAME_MAXSIZE];	// 离开者的名字

    int			nLeaveReason;				// 离开原因
};

///////////////////////////////////////////////////////////////////
// 队伍信息更新
// MSG_TEAM_UPDATEMEMBER									7
// 消息体
struct SMsgTeamUpdateMember_OS
{
    int			nTeamID;					// 队伍ID

    int			nMemberCount;				// 队友数
    // nMemberCount * STeamMemberInfo		// 队友信息
};

struct SMsgTeamUpdateMember_SC
{
    int			nTeamID;					// 队伍ID
    int			nPickMode;					// 拾取模式

    SDiceModeInfo	diceinfo;				// 团队拾取设置

    int			nMemberCount;				// 队友数
    // nMemberCount * STeamMemberInfo		// 队友信息
};

///////////////////////////////////////////////////////////////////
// 更换队长
// MSG_TEAM_CHANGECAPTAIN									8
// 消息体
struct SMsgTeamChangeCaptain_OS
{
    int			nTeamID;					// 队伍ID
    DWORD		dwNewCaptain;				// 新队长

    int			nChangeReason;				// 更换原因
};

struct SMsgTeamChangeCaptain_SC
{
    int			nTeamID;					// 队伍ID
    DWORD		dwNewCaptain;				// 新队长

    int			nChangeReason;				// 更换原因
};

///////////////////////////////////////////////////////////////////
// 人物进入或退出
// MSG_TEAM_PERSONLINESTATE								9
// 消息体
struct SMsgTeamPersonLineState_SO
{
    DWORD		dwPDBID;					// 谁
    DWORD		clientID;					// clientid
    bool		bLoginFlag;					// 上线/下线
    BYTE		byMode;						// 模式
    bool		bActiveExit;				// 主动下线
};

///////////////////////////////////////////////////////////////////
// 队伍解散
// MSG_TEAM_DISBANDTEAM									10
// 消息体
struct SMsgTeamDisbandTeam_SO
{
	int			nTeamID;					// 队伍ID
	DWORD		dwCaptainPDBID;				// 队长PDBID
};

struct SMsgTeamDisbandTeam_OS
{
    int			nTeamID;					// 队伍ID
};

///////////////////////////////////////////////////////////////////
// 任命队长
// MSG_TEAM_APPOINTCAPTAIN									11
// 消息体
struct SMsgTeamAppointCaptain_CS
{
    DWORD		dwNewCaptain;				// 新队长
};

struct SMsgTeamAppointCaptain_SO
{
    int			nTeamID;					// 队伍ID
    DWORD		dwCaptain;					// 队长
    DWORD		dwNewCaptain;				// 新队长
};

///////////////////////////////////////////////////////////////////
// 开除队友
// MSG_TEAM_KICKOUTMEMBER									12
// 消息体
struct SMsgTeamKickoutMember_CS
{
    DWORD		dwKickPDBID;				// 队员
};

struct SMsgTeamKickoutMember_SO
{
    int			nTeamID;					// 队伍ID
    DWORD		dwCaptain;					// 队长
    DWORD		dwKickPDBID;				// 队员
};

struct SMsgTeamClanInviteJoinTeam_SO
{
    DWORD		dwPDBID;				    // 发出邀请玩家
    char		szName[GAME_NAME_MAXSIZE];	// 发出邀请者的名字
};

///////////////////////////////////////////////////////////////////
// 申请修改拾取模式
// MSG_TEAM_APPLYPICKMODE									13
// 消息体
struct SMsgTeamApplyPickMode_CS
{
    int			nNewPickMode;				// 新拾取模式
};

struct SMsgTeamApplyPickMode_SO
{
    int			nTeamID;					// 队伍ID
    DWORD		dwCaptain;					// 队长
    int			nNewPickMode;				// 新拾取模式
};

///////////////////////////////////////////////////////////////////
// 征询修改拾取模式
// MSG_TEAM_CONSULTPICKMODE								14
// 消息体
struct SMsgTeamConsultPickMode_OC
{
    int			nTeamID;					// 队伍ID	
    DWORD		dwVotingSN;					// 投票序号
    int			nNewPickMode;				// 新拾取模式
};

struct SMsgTeamConsultPickMode_CS
{
    int			nTeamID;					// 队伍ID
    DWORD		dwVotingSN;					// 投票序号
    bool		bAgreeFlag;					// 是否同意
};

struct SMsgTeamConsultPickMode_SO
{
    int			nTeamID;					// 队伍ID
    DWORD		dwVotingSN;					// 投票序号
    DWORD		dwMember;					// 成员PDBID
    bool		bAgreeFlag;					// 是否同意
};

///////////////////////////////////////////////////////////////////
// 修改拾取模式
// MSG_TEAM_CHANGEPICKMODE									15
// 消息体
struct SMsgTeamChangePickMode_OS
{
    int			nTeamID;					// 队伍
    int			nNewPickMode;				// 新拾取模式
};

struct SMsgTeamChangePickMode_SC

{
    int			nTeamID;					// 队伍
    int			nNewPickMode;				// 新拾取模式		
};

///////////////////////////////////////////////////////////////////
// 更新队友数据（血量、位置）
// MSG_TEAM_SYNCMEMBER_TOALLTEAM							16
// 消息体
struct SMsgTeamSyncMemberToAllTeam_SC
{
    int			nTeamID;					// 队伍ID
    DWORD		dwPDBID;					// PDBID

    WORD		nFaceID;					// 头像ID
    BYTE		nLevel;						// 当前等级

    BYTE		nVocation;					// 职业

    int			nCurHP;						// 当前HP
    int			nMaxHP;						// 最大HP

    DWORD		nZoneID;					// 场景ID
    int			nMapID;						// 地图ID

    POINTS		ptTile;						// 位置
    POINTS		ptMoveTarget;				// 移动目标点

    int			nBattle;					// 战力值

    bool		bDieState;					// 死亡状态

    bool		bSpeaking;					// 是否正在说话

    SMsgTeamSyncMemberToAllTeam_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 恢复数据
// MSG_TEAM_DATARESTORE									17
// 消息体
struct SMsgTeamDataRestore_SO
{
    int				nTeamID;				// 队伍ID
    int				nPickMode;				// 拾取模式
    SDiceModeInfo	diceinfo;				// dice信息

    int			nMemberCount;				// 队友数
    // nMemberCount * STeamMemberInfo		// 队友信息

    SMsgTeamDataRestore_SO(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgTeamDataRestore_OS
{
    int				nTeamID;				// 队伍ID
    int				nPickMode;				// 拾取模式
    SDiceModeInfo	diceinfo;				// dice信息

    int				nMemberCount;			// 队友数
    // nMemberCount * STeamMemberInfo		// 队友信息

    SMsgTeamDataRestore_OS(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 申请修改Dice模式
// MSG_TEAM_APPLYDICEMODE									18
// 消息体
struct SMsgTeamApplyDiceMode_CS
{
    SDiceModeInfo diceinfo;					// dice信息
};

struct SMsgTeamApplyDiceMode_SO
{
    int			nTeamID;					// 队伍ID
    DWORD		dwMaster;					// PDBID

    SDiceModeInfo diceinfo;					// dice信息
};

///////////////////////////////////////////////////////////////////
// 修改Dice模式
// MSG_TEAM_CHANGEDICEMODE									19
// 消息体
struct SMsgTeamChangeDiceMode_OS
{
    int			nTeamID;					// 队伍
    SDiceModeInfo diceinfo;					// roll信息
};

struct SMsgTeamChangeDiceMode_SC
{
    int			nTeamID;					// 队伍ID	

    SDiceModeInfo diceinfo;					// dice信息
};

///////////////////////////////////////////////////////////////////
// 征询修改拾取模式
// MSG_TEAM_CONSULTDICEMODE								20
// 消息体
struct SMsgTeamConsultDiceMode_OC
{
    int			nTeamID;					// 队伍ID	
    DWORD		dwVotingSN;					// 投票序号
    SDiceModeInfo diceinfo;					// dice信息
};

///////////////////////////////////////////////////////////////////
// 同步队友位置
// MSG_TEAM_POSTPLACE										21
// 消息体
struct SMsgTeamPostPlace_SC
{
    int			nTeamID;					// 队伍ID

    struct SMemberPlaceInfo
    {
        DWORD	dwPDBID;					// PDBID
        DWORD	dwZoneID;					// 场景ID
        int		nMapID;						// 地图ID
        POINT	ptTile;						// tile

        POINT	ptMoveTarget;				// 移动的目标点

        SMemberPlaceInfo(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // N * SMemberPlaceInfo					// 数据
};

///////////////////////////////////////////////////////////////////
// 通知所有场景服，强制同步位置
// MSG_TEAM_FORCESYNCPLACE									22
// 消息体
struct SMsgTeamForceSyncPlace_ZZ
{
    int			nTeamID;					// 队伍ID

    SMsgTeamForceSyncPlace_ZZ(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 同步队友死亡状态
// MSG_TEAM_MEMBERDIESTATE									23
// 消息体
struct SMsgTeamMemberDieState_SC
{
    int			nTeamID;					// 队伍ID
    DWORD		dwPDBID;					// PDBID
    bool		bDieState;					// 死亡状态
    int			nReliveCountDown;			// 复活倒计时长

    SMsgTeamMemberDieState_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 队友上线
// MSG_TEAM_MEMBERLOGIN									24
// 消息体
struct SMsgTeamMemberLogin_SC
{
    int			nTeamID;					// 队伍ID
    DWORD		dwPDBID;					// PDBID
    bool		bOffline;					// 离线/跨场景

    DWORD		dwZoneID;					// 场景ID
    int			nMapID;						// 地图ID
    POINT		ptTile;						// tile

    SMsgTeamMemberLogin_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 队友下线
// MSG_TEAM_MEMBERLOGOUT									25
// 消息体
struct SMsgTeamMemberLogout_SC
{
    int			nTeamID;					// 队伍ID
    DWORD		dwPDBID;					// PDBID
    bool		bOffline;					// 离线/跨场景

    DWORD		dwZoneID;					// 场景ID
    int			nMapID;						// 地图ID
    POINT		ptTile;						// tile

    SMsgTeamMemberLogout_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 同进程跨地图
// MSG_TEAM_MEMBERCHANGEZONE								26
// 消息体
struct SMsgTeamMemberChangeZone_SC
{
    int			nTeamID;					// 队伍ID
    DWORD		dwPDBID;					// PDBID

    DWORD		dwZoneID;					// 场景ID
    int			nMapID;						// 地图ID
    POINT		ptTile;						// tile

    SMsgTeamMemberChangeZone_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 发送团事件到客户端
// MSG_TEAM_GROUP_EVENT									30

// 发送事件到客户端信息数据
struct SMsgTeamSendGroupEvent_CS
{
    BYTE	nEventType;		// 事件类型 参考 enum EMGroupEventType
    DWORD	dwSpare;		// 备用数字
    // void * pData;		// 符加数据

    SMsgTeamSendGroupEvent_CS(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


///////////////////////////////////////////////////////////////////
// 客户端发送团事件到服务器
// MSG_TEAM_GROUP_CLIENT_EVENT								31

// 消息体
struct SMsgTeamSendGroupClientEvent_CS
{
    BYTE	nEventType;		// 事件类型 参考 enum EMGroupClientEventType
    DWORD	dwSpare;		// 备用数字
    // void * pData;		// 符加数据

    SMsgTeamSendGroupClientEvent_CS(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 场景服发送团事件到社会服
// MSG_TEAM_GROUP_SERVER_EVENT									32

// 场景服发送团事件到社会服信息数据
struct SMsgTeamSendGroupServerEvent_SO
{
    BYTE	nEventType;		// 事件类型 参考 enum EMGroupEventType
    DWORD	dwSpare;		// 备用数字
    // void * pData;		// 符加数据

    SMsgTeamSendGroupServerEvent_SO(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 社会服发送团事件到场景服
// MSG_TEAM_GROUP_SOCIAL_EVENT									33

// 场景服发送团事件到社会服信息数据
struct SMsgTeamSendGroupSocialEvent_OS
{
    BYTE	nEventType;		// 事件类型 参考 enum EMGroupSvrEventType
    DWORD	dwSpare;		// 备用数字
    // void * pData;		// 符加数据

    SMsgTeamSendGroupSocialEvent_OS(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


///////////////////////////////////////////////////////////////////
// 社会服发送统计事件到场景服
// MSG_TEAM_SEND_TOTAL_DATA									34

// 场景服发送统计事件到社会服信息数据
struct SMsgTeamSendTotalData_SO
{
    BYTE	nType;			// 类型,EMGroupSendTotalType
    DWORD	dwMurderer;		// 凶手,为0表示不是人物
    DWORD	dwMaster;		// 死亡者,为0表示不是人物,DPS统计时为DPS数
    DWORD	dwData;			// 附加数

    SMsgTeamSendTotalData_SO(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 发送跟随数据给跟随者
// MSG_TEAM_SEND_FOLLOW_INFO									35
// 发送跟随数据给跟随者信息数据
struct SMsgTeamSendFollowInfo_SC
{
    DWORD	dwMapID;			// 所在地图ID
    DWORD	dwZoneID;			// 所在场景ID
    POINT	ptTarget;			// 位置
    bool	bForceFollow;		// 强制接收跟随

    SMsgTeamSendFollowInfo_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 更新跟随数据
// MSG_TEAM_UPDATE_FOLLOW_INFO									36
// 更新跟随数据信息
struct SMsgTeamUpdateFollowInfo_SC
{
    LONGLONG uidTarget;			// 目标UID
    BYTE	nFollowType;		// 跟随类型 0:普通跟随 1:接龙跟随

    SMsgTeamUpdateFollowInfo_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 请求服务器团成员位置信息
// MSG_TEAM_REQUEST_GROUP_MAPLOC								37
// 请求服务器团成员位置信息
struct SMsgTeamRequestGroupMapLoc_SC
{
    DWORD dwDataType;			// 数据标识 EMGroupRequestMapLocType

    SMsgTeamRequestGroupMapLoc_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

///////////////////////////////////////////////////////////////////
// 回复请求服务器团成员位置信息
// MSG_TEAM_SEND_GROUP_MAPLOC									38
// 回复请求服务器团成员位置信息
struct SMsgTeamSendGroupMapLocInfo_SC
{
    int nCounts;						// 成员数据条数

    struct SMsgGroupMemberPlaceNode
    {
        DWORD	dwPDBID;			// PDBID
        BYTE	nCountryID:4;		// 国家ID
        BYTE	nInGroup:2;			// 是否在团中 0:不在,1:在
        BYTE	nIsCaption:2;		// 是否队长 0:不是,1:是
        WORD	nMapID;				// 地图ID
        WORD	nTileX;				// tile x
        WORD	nTileY;				// tile y
        WORD	nMoveX;				// 移动的目标点 x
        WORD	nMoveY;				// 移动的目标点 y

        SMsgGroupMemberPlaceNode(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // nCounts * SMsgGroupMemberPlaceNode

    SMsgTeamSendGroupMapLocInfo_SC(void)
    {
        memset(this, 0, sizeof(*this));
    }
};



///////////////////////////////////////////////////////////////////
// 请求加入队伍,会通过场景服取得信息后加入队伍
// MSG_TEAM_REQUEST_JOIN_TEAM								39

struct SMsgTeamRequestJoinTeam_OS
{
    DWORD dwPDBID;			// PDBID
    int nTeamID;			// 队伍ID
    bool bIsCaptain;		// 是否队长

    SMsgTeamRequestJoinTeam_OS(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


///////////////////////////////////////////////////////////////////
// 请求加入队伍结果
// MSG_TEAM_REQUEST_JOIN_TEAM_RESULT						40

struct SMsgTeamRequestJoinTeamResult_SO
{
    DWORD		dwPDBID;					// PDBID
    int			nTeamID;					// 队伍ID
    bool		bCaptainFlag;				// 是否队长

    STeamSyncPersonContext personData;		// 人物数据

    SMsgTeamRequestJoinTeamResult_SO(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 申请修改拾取模式
// MSG_TEAM_APPLYVOTEKICK									41
// 消息体
struct SMsgTeamApplyVoteKick_C0
{
    // 要踢的玩家
    DWORD dwKickMemberID;
};

// 征询投票踢人
// MSG_TEAM_CONSULTKICKMEMBER								42
// 消息体
struct SMsgTeamConsultKickMember_OC
{
    int			nTeamID;										// 队伍ID	
    DWORD		dwVotingSN;										// 投票序号
    DWORD		dwKickMemberID;
    char		szKickMemberName[GAME_NAME_MAXSIZE];			// 被踢者ID
    char		szApplyMemberName[GAME_NAME_MAXSIZE];		// 申请者名字
};

struct SMsgTeamConsultKickMember_C0
{
    int			nTeamID;					// 队伍ID
    DWORD		dwVotingSN;					// 投票序号
    bool		bAgree;						// 是否同意
};

///////////////////////////////////////////////////////////////////
// 修改队伍状态
// MSG_TEAM_CHANGEFLAG									43
// 消息体
struct SMsgTeamChangeFlag_SO
{
    int			nTeamID;				// 队伍ID	
    bool		bIsAdd;					// true：添加，false：清除
    WORD		wStatusValue;			// 修改的值

    SMsgTeamChangeFlag_SO(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack()
#endif // __TEAMMSGDEF_H__