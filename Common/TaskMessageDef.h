/*******************************************************************
** 文件名:	TaskMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	任务消息定义
********************************************************************/

#ifndef __TASK_MESSAGE_DEF_H__
#define __TASK_MESSAGE_DEF_H__

/////////////////////////// 任务模块消息  ////////////////////////////
// 消息码定义键值
enum TASK_NET_MESSAGE
{
    TASK_MSG_SHOW_TASK_LIST = 0,		        // 显示任务列表
    TASK_MSG_DELETE_TASK_LIST,                  // 删除任务列表
    TASK_MSG_UPDATE_TASK_LIST,                  // 更新任务
    TASK_MSG_SEND_TASK_PRIZE,                   // 发送任务奖励
    TASK_MSG_FINISH_TASK,                       // 完成任务
	TASK_MSG_FRESH_TASK,                        // 刷新任务

	TASK_MSG_VIEW_CONTACT_BASE_C0,				// 查看交互任务基础
	TASK_MSG_VIEW_CONTACT_BASE_OC,				// 查看交互任务基础
	TASK_MSG_VIEW_CONTACT_REPLY_C0,				// 查看交互任务回复
	TASK_MSG_VIEW_CONTACT_REPLY_OC,				// 查看交互任务回复	

	TASK_MSG_ADD_CONTACT_TASK_CS,				// 请求新增交互任务
	TASK_MSG_ADD_CONTACT_TASK_SO,				// 请求新增交互任务
	TASK_MSG_ADD_CONTACT_TASK_SUCCESS_OS,		// 成功新增交互任务
	TASK_MSG_ADD_CONTACT_TASK_SUCCESS_SC,		// 成功新增交互任务

	TASK_MSG_PRE_CATCH_TASK_CS,					// 预获取任务
	TASK_MSG_PRE_CATCH_TASK_SO,					// 预获取任务
	TASK_MSG_PRE_CATCH_TASK_OS,					// 预获取任务
	TASK_MSG_PRE_CATCH_TASK_SC,					// 预获取任务

	TASK_MSG_CATCH_TASK_CS,						// 获取任务
	TASK_MSG_CATCH_TASK_SO,						// 获取任务
	TASK_MSG_CATCH_TASK_SC,						// 获取任务

	TASK_MSG_ADD_CONTACT_REPLY_CO,				// 新增任务回复
	TASK_MSG_ADD_CONTACT_REPLY_OS,				// 新增任务回复

	TASK_MSG_REFLUSH_OPT_CS,					// 执行刷新操作
	TASK_MSG_REFLUSH_OPT_SC,					// 刷新操作信息

	TASK_MSG_ACTOR_PUBLISH_CONTACT_OC,			// 记录的玩家发布任务

	TASK_MSG_CONTACT_APPLY_FRIEND_OS,			// 交互任务申请加好友

	TASK_MSG_VIEW_CONTACT_DELETE_OC,			// 交互任务已删除

	TASK_MSG_UPDATE_RANK_TASK_OS,				// 更新排位任务
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(1)

// 删除任务 客户端-服务器
struct STaskDeleteList_CS
{
    int     nTaskID;            // 删除任务ID

    STaskDeleteList_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 完成任务 客户端-服务器
struct STaskFinishList_CS
{
    int     nTaskID;            // 完成任务ID

    STaskFinishList_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 刷新任务 客户端-服务器
struct STaskFresh_CS
{
	int     nTaskType;            // 刷新任务类型

	STaskFresh_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 查看交互任务基础
struct SContactViewBase_CO 
{
	LONGLONG	llContactID;

	SContactViewBase_CO()
	{
		memset(this, 0, sizeof(*this));
	}
};

//TASK_MSG_VIEW_CONTACT_DELETE_OC
struct SContactBaseDelete_OC 
{
	LONGLONG	llContactID;

	SContactBaseDelete_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 查看交互任务回复
struct SContactViewReply_CO 
{
	int			nTaskCount;
	LONGLONG	arrContactId[CONTACT_TASK_MAXSIZE + PUBLISH_CONTACT_MAXSIZE];
	int			arrReplyCount[CONTACT_TASK_MAXSIZE + PUBLISH_CONTACT_MAXSIZE];

	SContactViewReply_CO()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SContactMsgHead_OC 
{
	bool bIsEnd;
	int nBufLen;

	SContactMsgHead_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SContactTaskNode_OC
{
	LONGLONG	llContactID;
	DWORD		dwPublishPDBID;
	DWORD		dwPublishUserID;
	int			nPublishHeadID;
	int			nPublishSex;
	LONGLONG	llTagID;
	DWORD		dwServerTime;
	DWORD		dwPublishTime;
	char		szPublishName[ACTORNAME_MAXSIZE];
	char		szEmotionWords[GAME_LONGSTR_MAXSIZE];

	SContactTaskNode_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SContactTaskReplyNode_OC
{
	LONGLONG	llContactID;
	DWORD		dwReplyPDBID;
	DWORD		dwReplyUserID;
	int			nRelpyHeadID;
	DWORD		dwReplyTime;
	BYTE		byReplySex;
	char		szReplyName[ACTORNAME_MAXSIZE];
	char		szReplyContent[GAME_LONGSTR_MAXSIZE];

	SContactTaskReplyNode_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};
			
// 新增交互任务
struct SContactTaskAdd_CS
{
	BYTE		bySubType;								// 子类型
	BYTE		bySpecificType;							// 具体类型（子类型下类型）
	char		szEmotionWords[GAME_LONGSTR_MAXSIZE];	// 心情话语
	LONGLONG	llTagID;								// 标签ID

	SContactTaskAdd_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SContactTaskAdd_SO 
{
	DWORD		dwPDBID;								// 发布者ID
	BYTE		bySubType;								// 子类型
	BYTE		bySpecificType;							// 具体类型（子类型下类型）
	char		szEmotionWords[GAME_LONGSTR_MAXSIZE];	// 心情话语
	LONGLONG	llTagID;								// 标签ID

	SContactTaskAdd_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SContactTaskAddResult_OS 
{
	DWORD		dwPDBID;						// 发布者ID
	LONGLONG	llContactID;					// 已发布交互ID
	bool		bSuccess;						// 添加结果

	SContactTaskAddResult_OS()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 预获取任务
struct STaskPreCatch_CS
{
	int			nCount;								// 已拥有瓶子数量
	LONGLONG	arrContactId[CONTACT_TASK_MAXSIZE];	// 已拥有瓶子

	STaskPreCatch_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct STaskPreCatch_SO
{
	DWORD		dwPDBID;							// 请求者ID
	int			nCount;								// 已拥有瓶子数量
	LONGLONG	arrContactId[CONTACT_TASK_MAXSIZE];	// 已拥有瓶子

	STaskPreCatch_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct STaskPreCatch_OS
{
	DWORD		dwPDBID;						// 请求者ID
	LONGLONG	llContactID;					// 交互ID
	BYTE		bySubType;						// 子类型
	BYTE		bySpecificType;					// 具体类型（子类型下类型）
	DWORD		dwPublisherPDBID;				// 发布者ID

	STaskPreCatch_OS()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 获取任务
struct STaskCatch_CS 
{
	int			nTaskID;						// 任务ID
	LONGLONG	llContactID;					// 交互ID
	int			nTaskType;						// 任务类型
	int			nTaskSubType;					// 任务子类型
	int			nTaskSpecificType;				// 任务具体类型
	int			nPublisherDBID;					// 发布者ID

	STaskCatch_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct STaskCatch_SO
{
	DWORD		dwPDBID;						// 人物ID
	LONGLONG	llContactID;					// 交互ID

	STaskCatch_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 新增任务回复  TASK_MSG_ADD_CONTACT_REPLY_CO
struct SContactReplyAdd_CO
{
	LONGLONG	llContactID;					// 交互ID
	int			nCurReplyCount;					// 现有回复数量
	char		szContent[GAME_LONGSTR_MAXSIZE];// 回复者发言内容

	SContactReplyAdd_CO()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SContactReplyAdd_OS
{
	DWORD		dwPDBID;						// 回复者PDBID
	LONGLONG	llContactID;					// 交互ID

	SContactReplyAdd_OS()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 记录的玩家发布任务 TASK_MSG_ACTOR_PUBLISH_CONTACT_OC			
struct SActorPublishContact_OC
{
	DWORD		dwActorID;
	int			nCount;
	//			nCount * SContactTaskNode_OC

	SActorPublishContact_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 交互任务申请加好友 TASK_MSG_CONTACT_APPLY_FRIEND_OC,
struct SContactApplyFriend_OC
{
	DWORD		dwActorID;
	DWORD		dwTargetID;
	bool		bIsDifSex;

	SContactApplyFriend_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()

#endif // __TASK_MESSAGE_DEF_H__