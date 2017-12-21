//==========================================================================
/**
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/28/2016
** 版  本:	1.0
** 描  述:	师徒消息定义
*/
//==========================================================================

#ifndef __MENTORSHIPMSGDEF_H__
#define __MENTORSHIPMSGDEF_H__

#include "GatewayServiceDef.h"
#include "NetMessageDef.h"
#include "IGameDatabaseDef.h"
#include "NameLenDef.h"
#include "MentorshipDef.h"


using namespace Gateway;

#pragma once
#pragma pack(1)

/******************************************************************/
// 上层的消息码必定是MSG_MODULEID_MENTORSHIP
/******************************************************************/
///////////////////////////////////////////////////////////////////


/***************************************************************/
//////////////////////// 服务器层的全局定义 /////////////////////
/***************************************************************/
enum EMMentorshipMsgType
{
    emMsgMentorship_Module = 1,							// 模块消息码
};

// 消息ID定义
//////////////////////////////////////////////////////////////////////////
enum MENTORSHIP_NET_MESSAGE_KEYACTION
{
	MSG_MENTORSHIP_PERSONDATA,					// 师徒个人详细数据

	MSG_MENTORSHIP_UPDATE_BASE_PERSONDATA,		// 更新个人基础数据

	MSG_MENTORSHIP_UPDATE_MENTORSHIP,			// 更新师徒关系数据

	MSG_MENTORSHIP_ADD_MENTORSHIP,				// 添加师徒关系数据

	MSG_MENTORSHIP_APPLY,						// 师徒关系申请

	MSG_MENTORSHIP_APPLY_ANSWER,				// 师徒关系申请应答

	MSG_MENTORSHIP_PROMOTE,						// 提升见习学徒

    MSG_MENTORSHIP_DISMISS,						// 解散师徒关系

	MSG_MENTORSHIP_ISSUE,						// 拜师收徒信息发布

	MSG_MENTORSHIP_ISSUE_LIST,					// 拜师收徒发布信息列表

	MSG_MENTORSHIP_ISSUE_AUTOSET,				// 拜师收徒信息发布自动设置

	MSG_MENTORSHIP_MASTER_PRENTICELIST,			// 导师的学徒列表

	MSG_MENTORSHIP_MEMBER_LOGIN_LOGOUT,			// 师徒成员上下线

	MSG_MENTORSHIP_DISMISS_OS,					// 社会服向场景服解散师徒关系

	MSG_MENTORSHIP_UPDATE_MENTORSHIP_OS,		// 社会服向场景服更新师徒关系数据

	MSG_MENTORSHIP_ADD_MEMBER_SHIPVAL_SO,		// 场景服向社会添加师徒成员师徒值

	MSG_MENTORSHIP_GIVE_FIRSTEWIN_OS,			// 社会服向场景给予首胜奖励

	MSG_MENTORSHIP_REMOVEALLSHIP_OS,			// 社会服向场景移除所有师徒关系

	MSG_MENTORSHIP_REQUEST_FIRST_AWARD,			// 请求领取首胜奖励

	MSG_MENTORSHIP_MAXID,
};

struct SMsgMentorshipSubMsg
{
	DWORD dwMsgCode;						// 子消息代码  EMMentorshipMsgType

	SMsgMentorshipSubMsg(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 消息体 MSG_MENTORSHIP_APPLY			-- 师徒关系申请
struct SMsgMentorship_Apply
{
	DWORD dwDesPDBID;
	BYTE  nRelationType;
	BYTE  nApplyPost;

	SMsgMentorship_Apply()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体 MSG_MENTORSHIP_APPLY_ANSWER,				-- 师徒关系申请应答
struct SMsgMentorship_Apply_Anwser
{
	DWORD dwSrcPDBID;
	BYTE  nRelationType;
	BYTE  nApplyPost;
	char  szSrcName[ACTORNAME_MAXSIZE];			// 玩家名字
	bool  bAnwser;

	SMsgMentorship_Apply_Anwser()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体  MSG_MENTORSHIP_PROMOTE					-- 提升见习学徒
struct SMsgMentorship_Promote
{
	DWORD dwMentorshipID;
	DWORD dwDesPDBID;

	SMsgMentorship_Promote()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体
struct SMsgMentorship_Dismiss
{
	DWORD dwMentorshipID;

	SMsgMentorship_Dismiss()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_MENTORSHIP_ISSUE
struct SMsgMentorship_Issue 
{
	BYTE  nIssueType;
	char  szIssueInfo[MENTORSHIP_ISSUE_LEN];

	SMsgMentorship_Issue()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgMentorship_Issue_Node
{
	DWORD dwPDBID;								// 玩家ID
	char  szName[ACTORNAME_MAXSIZE];			// 玩家名字
	WORD  nLevel;								// 玩家等级
	BYTE  bySex;                                // 性别
	char  szCorpsName[KIN_NAME_SIZE];			// 战队名
	char  szClanName[CLAN_NAME_SIZE];			// 联盟名
	char  szIssueInfo[MENTORSHIP_ISSUE_LEN];	// 发布内容

	SMsgMentorship_Issue_Node()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_MENTORSHIP_ISSUE_LIST					-- 拜师收徒发布信息列表
struct SMsgMentorshipIssueList
{
	BYTE nIssueType;
	int nCount;

	// nCount * SMsgMentorship_Issue_Node
	SMsgMentorshipIssueList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体 MSG_MENTORSHIP_ISSUE_AUTOSET				-- 拜师收徒信息发布自动设置
struct SMsgMentorshipIssue_AutoSet
{
	BYTE nIssueType;
	bool bAutoSet;

	SMsgMentorshipIssue_AutoSet()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgMentorshipMaster_PrenticeNode
{
	DWORD dwMasterPDBID;						// 师徒玩家ID
	DWORD dwPDBID;								// 学徒玩家ID
	char  szName[ACTORNAME_MAXSIZE];			// 学徒玩家名字
	short nFaceID;								// 头像ID
	bool  bIsOnline;							// 学徒玩家是否在线
	BYTE  nRelationType;						// 关系类型
	BYTE  nState;								// 关系状态

	SMsgMentorshipMaster_PrenticeNode()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体 MSG_MENTORSHIP_MASTER_PRENTICELIST			-- 导师的学徒列表
struct SMsgMentorshipMaster_PrenticeList
{
	DWORD dwMasterPDBID;
	int nCount;

	// nCount * SMsgMentorshipMaster_PrenticeNode
	SMsgMentorshipMaster_PrenticeList()
	{
		memset(this, 0, sizeof(*this));
	}

};

// 消息体 MSG_MENTORSHIP_MEMBER_LOGIN_LOGOUT			-- 师徒成员上下线
struct SMsgMentorship_Member_Login_Logout
{
	DWORD dwPDBID;
	bool  bFlag;

	SMsgMentorship_Member_Login_Logout()
	{
		memset(this, 0, sizeof(*this));
	}

};

// 消息体 MSG_MENTORSHIP_REQUEST_FIRST_AWARD			-- 请求领取首胜奖励
struct SMsgMentorship_Req_FirstAward
{
	BYTE  nPost;

	SMsgMentorship_Req_FirstAward()
	{
		memset(this, 0, sizeof(*this));
	}

};

// 消息体
struct SMsgMentorship_Dismiss_OS
{
	DWORD dwMasterPDBID;
	DWORD dwPrenticePDBID;

	SMsgMentorship_Dismiss_OS()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体
struct SMsgMentorship_UpdateMentorship_OS
{
	DWORD dwMasterPDBID;						
	DWORD dwPrenticePDBID;						
	BYTE  nRelationType;
	BYTE  nState;
	bool  bPromoteFlag;
	DWORD dwBuildTime;
	DWORD dwSeverPDBID;
	DWORD dwSeverTime;

	SMsgMentorship_UpdateMentorship_OS()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体 MSG_MENTORSHIP_ADD_MEMBER_SHIPVAL_SO		-- 场景服向社会添加师徒成员师徒值
struct SMsgMentorship_Add_Member_ShipVal_SO
{
	DWORD dwPDBID;
	bool  bFightResult;
	bool  bFirstWin;
	BYTE  nPost;

	SMsgMentorship_Add_Member_ShipVal_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体 MSG_MENTORSHIP_GIVE_FIRSTEWIN_OS			-- 社会服向场景给予首胜奖励
struct SMsgMentorship_GiveFirstWin_SO
{
	DWORD dwPDBID;
	BYTE nMentorhsipPost;

	SMsgMentorship_GiveFirstWin_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_MENTORSHIP_REMOVEALLSHIP_OS
struct SMsgMentorship_RemoveAllShip_OS
{
	bool bFlag;

	SMsgMentorship_RemoveAllShip_OS()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体 MSG_MENTORSHIP_PERSONDATA
struct SMsgMentorship_PersonData_OC
{
	int nCount;

	// nCount * SClientMentorShip
	SMsgMentorship_PersonData_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 消息体
struct SMsgMentorship_UpdateMentorship_OC
{
	DWORD dwMentorshipID;
	char  szName[ACTORNAME_MAXSIZE];			// 玩家名字
	BYTE  nRelationType;
	BYTE  nState;
	DWORD dwSeverPDBID;
	DWORD dwSeverTime;

	SMsgMentorship_UpdateMentorship_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()

#endif // __MENTORSHIPMSGDEF_H__

