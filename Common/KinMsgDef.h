//==========================================================================
/**
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	17：45
** 版  本:	1.0
** 描  述:	战队消息定义
*/
//==========================================================================

#ifndef __KINMSGDEF_H__
#define __KINMSGDEF_H__

#include "GatewayServiceDef.h"
#include "MatchDef.h"
#include "NetMessageDef.h"
#include "IGameDatabaseDef.h"
#include "NameLenDef.h"
#include "KinDef.h"
#include "War_ManagedDef.h"

using namespace Gateway;

#pragma once
#pragma pack(1)

/******************************************************************/
// 上层的消息码必定是MSG_MODULEID_KIN
/******************************************************************/
///////////////////////////////////////////////////////////////////


/***************************************************************/
//////////////////////// 服务器层的全局定义 /////////////////////
/***************************************************************/
enum EMKinMsgType
{
    emMsgKin_Module = 1,								// 模块消息码
    emMsgKin_Kin,										// 战队消息码

};

struct SKinMsgHead
{
    DWORD dwSrcPointID;		// 来源ID（客户端是客户端ID，服务器是场景服的ID）
    BYTE  nSrcEndPoint;		// 源端点码（服务器还是客户端）
};


struct SMsgKinDataSubMsg
{
    DWORD dwMsgCode;						// 子消息代码  EMKinMsgType

    // 附加数据
    DWORD	dwKinID;

    SMsgKinDataSubMsg(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// 消息ID定义
//////////////////////////////////////////////////////////////////////////
enum KIN_NET_MESSAGE_KEYACTION
{
    MSG_KIN_CREATE,             // 建立战队

    MSG_KIN_DISMISS,            // 解散战队

    MSG_KIN_CANEL_DISMISS,      // 取消解散

    MSG_KIN_INVITE,             // 邀请成员

    MSG_KIN_ANSWER_INVITE,      // 回应邀请成员

    MSG_KIN_APPLY,              // 申请加入
   
    MSG_KIN_ADMIT,	            // 接收申请者

    MSG_KIN_KICK,               // 剔除成员

    MSG_KIN_QUIT,               // 退出战队

    MSG_KIN_LOGIN,              // 场景服玩家登陆通知

    MSG_KIN_LIST,               // 获取战队列表

    MSG_KIN_UPATEMEMBER,        // 更新成员信息
   
    MSG_KIN_UPATEKINPORP,        // 更新战队属性
    
    MSG_KIN_KINBASEDATA,        // 申请战队基本数据

    MSG_KIN_MOTIFY,		        // todo 修改宣言，公告							
    
    MSG_KIN_UPDATEID,		    // 更改战队ID						

    MSG_KIN_LOOKUPMEMBER,		// todo查看成员的详细信息				
   
    MSG_KIN_CLIENTEVENT,	    // 客户端事件						

    MSG_KIN_UPGRADE,	        // todo 战队升级								

    MSG_KIN_REMOVEKIN_SCENE,	// 社会服向场景服删除战队						
   
    MSG_KIN_UPDATEKIN_SCENE,	// 社会服向场景服更新战队（全部）

    MSG_KIN_UPDATEKINPROP_SCENE, // 社会服向场景服更新战队属性

    MSG_KIN_CLEAR_SCENE,         // 社会服向场景服清除战队数据	

    MSG_KIN_KINMEMBERDATA,      // 战队成员数据 到客户端

    MSG_KIN_REMOVEMEMBER_SCENE,  // 社会服向场景服删除成员

    MSG_KIN_UPDATEMEMBER_SCENE,  // 社会服向场景服更新成员数据
    
    MSG_KIN_SETPROP_SO,         // 场景服向社会服提交设置属性请求
 
    MSG_KIN_UPDATECLIENT,       // 更新到战队基础信息到客户端

    MSG_KIN_SCENEPOSTMSG,        // 场景服消息转发
    
    MSG_KIN_RENAME,             // 战队改名

    MSG_KIN_MERGE_RESP,         // 合并战队回应

    MSG_KIN_LISTUPDATE,         // 战队列表更新

    MSG_KIN_ACTOR_CHANGESEX,    // 改变性别

    MSG_KIN_ACTOR_RENAME,       // 改变名字

    MSG_KIN_AUTO_ACCEPT_EDIT,   // 自动接收设置编辑

    MSG_KIN_AUTO_ACCEPT_REQUEST,   // 自动接收设置请求

    MSG_KIN_ENTERTOBSERVE,		// 进入观察战队数据				

    MSG_KIN_EXITOBSERVE,        // 取消观察战队数据							

    MSG_KIN_APPOINT,            // todo 任命职位

    MSG_KIN_NAMETITLE,          // todo 添加/删除战队名称号

    MSG_KIN_SETAUTOACCEPT,      // todo 设置是否自动接收成员

    MSG_KIN_EDITSLOGAN,         // todo 设置宣传口号和自动收人条件

    MSG_KIN_SENDSLOGAN,         // todo 发送宣传口号

    MSG_KIN_SLOGANREQUEST,      // todo 请求口号数据

    MSG_KIN_WORDREQUEST,        // todo 请求留言

    MSG_KIN_REQUEST_MERGE,      // todo 申请战队合并

    MSG_KIN_ACCEPT_MERGE,       // todo 接受战队合并请求

    MSG_KIN_MEMBER_LOGIN,       // 战队人员登录

    MSG_KIN_LIST_EXITOBSERVE,		// 战队列表观察退出	

    MSG_KIN_REQUEST_KINEVENT_LIST,   // 请求战队事件列表

    MSG_KIN_KINBASEDATA_UPDATE,     // 战队基础信息更新

	MSG_KIN_REQUEST_KINGLORYDATA,	// 请求战队荣誉数据		

	MSG_KIN_REQUEST_WARRECORD_TOTAL,	// 请求战队历史战绩

	MSG_KIN_LEGENDCUP_KIN_INFO,			// 查看杯赛战队信息

	MSG_KIN_LEGENDCUP_KIN_GLORY,		// 查看杯赛战队荣誉

	MSG_KIN_UPDATE_LIST,				// 更新杯赛列表节点
};


//MSG_KIN_CREATE									
// 消息体
struct SMsgKin_Create_CO
{
    char szName[KIN_NAME_SIZE];							// 战队名
	char szLabel[KIN_LABEL_SIZE];						// 战队标签
};

struct SMsgKin_Create_OS
{
    char szName[KIN_NAME_SIZE];							// 战队名
	char szLabel[KIN_LABEL_SIZE];						// 战队标签
    DWORD dwPDBID;										// 创建者PDBID
};

struct SMsgKin_Create_SO
{
    DWORD dwUDBID;                                      // 账号ID
    DWORD dwPDBID;										// 创建者PDBID
    char  szShaikhName[ACTORNAME_MAXSIZE];				// 角色名字
    DWORD dwKinID;										// 临时战队ID
    char  szName[KIN_NAME_SIZE];						// 战队名
    int   nEstablishTime;								// 创建时间
    bool  bCrtAsFormal;									// 直接创建为正式战队
	char szLabel[KIN_LABEL_SIZE];						// 战队标签
};


//	MSG_KIN_DISMISS									

//	MSG_KIN_CANEL_DISMISS							


//	MSG_KIN_INVITE									
// 消息体
struct SMsgKin_Invite_CO
{
    DWORD dwAnswerPDBID;								// 被邀请者ID
};

struct SMsgKin_Invite_OC
{
    DWORD dwInviterPDBID;								// 邀请者ID
    char szInviteName[ACTORNAME_MAXSIZE];
    char szKinName[KIN_NAME_SIZE];
};

// 回应邀请（同意/拒绝）
//	MSG_KIN_ANSWER_INVITE							
// 消息体
struct SMsgKin_Answer_CO
{
    DWORD dwInviterPDBID;								// 邀请者ID
    bool bIsAgree;										// 是否同意
};

// 申请加入
// MSG_KIN_APPLY									
struct SMsgKin_Apply_CO
{
    char szWord[128];									// 留言
};

// 接收申请者
//	MSG_KIN_ADMIT									
// 消息体
struct SMsgKin_Admit_CO
{
    DWORD dwApplyPDBID;									// 申请者ID
    bool  bAgree;										// 同意/拒绝
};

// 剔除成员
//	MSG_KIN_KICK								
// 消息体
struct SMsgKin_Kick_CO
{
    DWORD dwMemberPDBID;								// 成员ID
};

// 退出战队
//	MSG_KIN_QUIT									

// todo 进入观察战队数据
//	MSG_KIN_ENTERTOBSERVE							

// todo 取消观察战队数据
// MSG_KIN_EXITOBSERVE								

// todo 任命职位
// MSG_KIN_APPOINT									
// 消息体
struct SMsgKin_Appoint_CO
{
    DWORD dwMemberPDBID;								// 成员ID
    int nIdentity;										// 职位
};



// todo 添加/删除战队名称号
//	MSG_KIN_NAMETITLE								
// 消息体
struct SMsgKin_NameTitle_OS
{
    DWORD dwPDBID;										// 玩家ID
    char  szKinName[KIN_NAME_SIZE];						// 战队名字
    int   nIdentity;									// 战队职位
    bool  bRemove;										// true表示添加， false表示删除
    int   nLevel;										// 战队等级
};


// 场景服玩家登陆通知
//	MSG_KIN_LOGIN									
// 消息体
struct SMsgKin_Login_SO
{
    DWORD dwPDBID;										// 玩家ID
    bool  bOffLine;										// 是否下线后登陆（fasle表示是跨进程切换地图）
};


// 获取战队列表
// MSG_KIN_LIST									
// 消息体
struct SMsgKin_KinList_CO
{
    int nPageNum;										// 页码
    int nState;											// 战队类型 TKinState
    char szKinName[KIN_NAME_SIZE];						// 战队名
    int  nCurIndex;										// 当前下标
};

// 更新成员信息
// MSG_KIN_UPATEMEMBER								
// 消息体
struct SMsgKin_UpdateMember_OC
{
    // SKinMember
};

// 更新战队属性
// MSG_KIN_UPATEKINPORP							


// 申请战队基本数据
//	MSG_KIN_KINBASEDATA								
// 消息体
struct SMsgKin_KinBaseData_OC
{
    char  szKinName[KIN_NAME_SIZE];						// 战队名
    DWORD dwShaikhID;									// 族长
    char  szShaikhName[ACTORNAME_MAXSIZE];				// 战队名
    BYTE  nKinLevel;									// 战队等级
    char  szNotice[KIN_NOTICE_LEN];						// 战队的每日公告

    int   nActivity;									// 活跃度
	int   nWeekActivity;								// 周活跃度
    int	  nMoney;								        // 资金
    short nPlace;										// 战队荣誉值排名
    int   nSysTime;										// 系统时间

    int   nMemberCount;									// 成员个数
    int   nOnlineCount;                                 // 在线成员数量
    int   nKinMaxMember;                                // 允许最大总人数
    BYTE  nApplyMemberNum;								// 申请中成员的个数
    BYTE  nEventCount;									// 事件的个数
    short nEventDataLen;								// 事件数据长度
    int   nTotalFight;									// 总战力
    int   nTotalFightOrder;								// 总战力排名
	char	szKinLabel[KIN_LABEL_SIZE];				// 战队标签
	int		nLegendWinCount;						// 杯赛胜场
	int		nLegendTotaLCount;						// 杯赛总场数
	int		nGoldNum;								// 金奖次数
	int		nSilverNum;								// 银奖次数

    // 成员列表信息	nMemberCount * SKinListMember
    // 申请成员 nApplyMemberNum * SClientKinMember

    SMsgKin_KinBaseData_OC()
    {
        memset(this, 0, sizeof(SMsgKin_KinBaseData_OC));
    }
};

// todo 修改宣言，公告
//MSG_KIN_MOTIFY									
struct SMsgKin_Motify
{
    int nPropID;										// 属性ID
    char szText[KIN_MANIFESTO_LEN];						// 属性内容
};


// 更改ID
// MSG_KIN_UPDATEID								
struct SMsgKin_UdateID
{
    DWORD dwPDBID;										// 玩家ID
    DWORD dwKinID;										// 战队ID
};


// 查看成员的详细信息
//MSG_KIN_LOOKUPMEMBER							
struct SMsgKin_LookUpMember_CO
{
    DWORD dwPDBID;										// 玩家ID
};
// todo 修改宣言，公告
//MSG_KIN_MOTIFY									
// todo更改ID
//MSG_KIN_UPDATEID								

// todo查看成员的详细信息
// MSG_KIN_LOOKUPMEMBER							

// 客户端事件
// MSG_KIN_CLIENTEVENT								
struct SMsgKin_EventHead_OC
{
    WORD wEventID;										// 事件ID
    BYTE bSrcType;										// 事件类型
    DWORD dwSrcID;										// 事件源
};


// 社会服向场景服删除战队
// MSG_KIN_REMOVEKIN_SCENE							
struct SMsgKin_RemoveKinScene_OS
{
    DWORD dwKinID;										// 战队ID
};


// 社会服向场景服更新战队（全部）
// MSG_KIN_UPDATEKIN_SCENE							
struct SMsgKin_UpdateKinScene_OS
{
    DWORD dwID;											// 战队ID
    char  szName[KIN_NAME_SIZE];							// 战队名
    int   nState;										// 战队的状态：临时，正式，解散中
    int   nLevel;										// 战队等级
    int   nActivity;									// 活跃度
	int   nWeekActivity;                                // 周活跃度
    int   nPlace;										// 排名
    int   nShaikhID;									// 战队族长PDBID
    char  szShaikhName[ACTORNAME_MAXSIZE];				// 战队族长名
    int   nWorldID;									// 游戏世界ID
    int	  nMoney;								    // 资金
	char  szKinLabel[KIN_MANIFESTO_SIZE];			// 战队标签


    SMsgKin_UpdateKinScene_OS()
    {
        memset(this, 0, sizeof(SMsgKin_UpdateKinScene_OS));
    }
};

// 社会服向场景服更新战队属性
// MSG_KIN_UPDATEKINPROP_SCENE						
// 消息体
struct SMsgKin_UpdatePropScene_OS
{
    DWORD dwKinID;										// 战队ID
    BYTE nPropID;
    int nValue;
};


// 社会服向场景服清除战队数据
// MSG_KIN_CLEAR_SCENE							

// 社会服向场景服删除成员
// MSG_KIN_REMOVEMEMBER_SCENE						
struct SMsgKin_RemoveMember_OS
{
    DWORD dwPDBID;										// 玩家ID
    DWORD dwKinID;										// 战队ID
};

// 社会服向场景服更新成员数据
// MSG_KIN_UPDATEMEMBER_SCENE						
// 消息体
//...SSceneKinMember


// 场景服向社会服提交设置属性请求
// MSG_KIN_SETPROP_SO								
struct SMsgKin_SetProp_SO
{
    BYTE nPropID;
    int nValue;
};

// 社会服向场景服删除成员
// MSG_KIN_REMOVEMEMBER_SCENE						

// 社会服向场景服更新成员数据
// MSG_KIN_UPDATEMEMBER_SCENE						

// 场景服向社会服提交设置属性请求
// MSG_KIN_SETPROP_SO								

// 
//	MSG_KIN_UPDATECLIENT							
// 消息体
struct SMsgKin_UpdateClient_OC
{
    DWORD dwID;											// 战队ID
    BYTE  nState;										// 战队状态
    char  szName[KIN_NAME_SIZE];						// 战队名字
    DWORD dwShaikhID;									// 族长ID
    char  szShaikhName[ACTORNAME_MAXSIZE];				// 族长名字
    int   nKinLevel;                                    // 战队等级
    int   nMemberCount;                                 // 成员数量
    int   nOnlineCount;                                 // 在线成员数量
    int   nKinMaxMember;                                // 允许最大成员数量
    int   nApplyCount;                                 //  申请人数
	char  szKinLabel[KIN_MANIFESTO_SIZE];				// 战队标签
};


// 
//	MSG_KIN_MEMBER_LOGIN							
// 消息体
typedef SMsgKin_UpdateClient_OC SMsgKin_MemberLogin_OC;

// 场景服消息转发
// MSG_KIN_SCENEPOSTMSG								
struct SMsgKin_ScenePostMsg_OS
{
    DWORD dwPDBID;										// 玩家ID
    DWORD dwMsgID;										// 真正消息码ID
    //...后面跟消息的数据
};


// todo 设置是否自动接收成员
// MSG_KIN_SETAUTOACCEPT							
struct SMsgKin_SetAutoAccept
{
    bool bAuto;										// 是否自动接收成员
};


// todo 设置宣传口号和自动收人条件
// MSG_KIN_EDITSLOGAN								
struct SMsgKin_EditAutoAccept
{
    bool  bAutoAccept;                              // 自动收人设置开关
    short nAcceptLevel;								// 自动收人等级(2字节)
    DWORD dwAcceptFlag;								// 自动收人设置(4字节)
};



// 战队改名
// MSG_KIN_RENAME								
struct SMsKin_Rename_CO
{
    char	szNewName[KIN_NAME_SIZE];							// 新战队名
};


struct SMsKin_Rename_OS
{
    char	szNewName[KIN_NAME_SIZE];						// 新战队名
    DWORD	dwPDBID;										// 修改者PDBID
    DWORD	dwKinID;										// 修改者战队ID
};

struct SMsKin_Rename_SO
{
    char	szName[KIN_NAME_SIZE];							// 新战队名
    DWORD	dwPDBID;										// 
    int		nRenameTime;
    DWORD	dwKinID;
};


// 申请战队合并
// MSG_KIN_REQUEST_MERGE								
struct SMsKin_Merge_CO
{
    DWORD	dwApplyKinID;								// 申请并入战队ID
    char	szAcceptKinName[KIN_NAME_SIZE];				// 接受申请的战队名
    DWORD	dwAccpetKinID;								// 接受并入战队ID
};



//  todo 接受战队合并请求
// MSG_KIN_ACCEPT_MERGE								
struct SMsKin_Merge_OC
{
    DWORD	dwPDBID;									// 申请者PDBID
    char	szName[ACTORNAME_MAXSIZE];					// 申请者名称
    char	szKinName[KIN_NAME_SIZE];					// 申请者战队名称
};



// 合并战队回应
// MSG_KIN_MERGE_RESP									
struct SMsKin_MergeResp_CO
{
    bool	bAccept;									// 是否接受合并
    DWORD	dwApplyPDBID;								// 申请者PDBID
    char	szKinName[KIN_NAME_SIZE];					// 申请者战队名称
};


// MSG_KIN_LISTUPDATE								
struct SMsgKinListNode_OC
{
    DWORD dwID;											// 战队ID
    char  szName[KIN_NAME_SIZE];							// 战队名
    char  szShaikhName[KIN_NAME_SIZE];					// 战队族长名字
    short nMemberCount;									// 战队人数
    BYTE  nLevel;										// 战队等级
    BYTE  nState;										// 战队状态
    int   nActivity;									// 活跃度
    bool  bIsApply;										// 是否已经申请
    bool  bAutoAccpet;									// 是否自动收人
    BYTE  nLevelLimit;									// 招人等级限制
    DWORD dwAcceptFlag;								    // 接收限制
	int   nWeekActivity;                                // 周活跃度
                             
};

struct SMsgKinRequestKinList_OC
{
    int nKinCount;										// 战队个数                

    //SMsgKinListNode_OC * nKinCount;			// 战队列表信息
};

// MSG_KIN_ACTOR_CHANGESEX								
struct SMsgKinActorChangeSex_SO
{
    DWORD dwPDBID;
    int nSex;
};

// MSG_KIN_ACTOR_RENAME							    
struct SMsgKinActorRename_SO
{
    DWORD dwPDBID;
    char  szName[ACTORNAME_MAXSIZE];				// 角色名字
};


// MSG_KIN_REQUEST_KINGLORYDATA 	请求战队荣誉数据
struct SMsgKinGloryHead_OC
{
	char szKinName[ACTORNAME_MAXSIZE];				// 战队名称
	int nWinNum;									// 杯赛胜场
	int nLoseNum;									// 杯赛负场
	int nGodNum;									// 团队超神次数
	int n5KillNum;									// 团队5杀次数
	int n4KillNum;									// 团队4杀次数
	int n3KillNum;									// 团队3杀次数
	int nDeathNum;									// 团队死亡次数
	int nKillNum;									// 团队击杀次数
	int nAssistNum;									// 团队助攻次数
	int nCount;										// 比赛获奖次数
	// nCount * SMsgKinGloryNode_OC
};

struct SMsgKinGloryNode_OC
{
	LONGLONG	llLegengCupID;							// 杯赛ID
	BYTE		byCupRank;								// 杯赛名次
	char		szCupName[ACTORNAME_MAXSIZE];			// 杯赛名称
	int			nCupConfigID;							// 杯赛配置
	DWORD		dwCupEndTime;							// 杯赛结束时间
};

// 请求战队历史战绩  MSG_KIN_REQUEST_WARRECORD_TOTAL
struct SMsgKinTotalWarNode_OC 
{
	LONGLONG	nWarID;								// 战场ID
	char		szKin1Name[ACTORNAME_MAXSIZE];		// 战队1名称
	char		szKin2Name[ACTORNAME_MAXSIZE];		// 战队2名称
	DWORD		nKin1ID;							// 战队1ID
	DWORD		nKin2ID;							// 战队2ID
	DWORD		nWinKin;							// 胜利战队ID
	DWORD		nBeginTime;							// 战场开始时间
};

#pragma pack()

#endif // __KINMSGDEF_H__

