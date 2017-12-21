/*******************************************************************
** 文件名:	E:\speed\Common\KinDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/25/2016	10：50
** 版  本:	1.0
** 描  述:	
            师徒相关定义
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "NameLenDef.h"
#include "NetMessageDef.h"
#include "GameDef.h"

//输出DEBUG信息
#define MENTORSHIP_DEBUG_OUTPUT
#ifdef MENTORSHIP_DEBUG_OUTPUT
#	define	MENTORSHIP_TRACELN(x)			TraceLn(x)
#	define	MENTORSHIP_WARNINGLN(x)			WarningLn(x)
#	define	MENTORSHIP_ERRORLN(x)			ErrorLn(x)
#	define	MENTORSHIP_EMPHASISLN(x)		EmphasisLn(x)
#else
#	define	MENTORSHIP_TRACELN(x)
#	define	MENTORSHIP_WARNINGLN(x)
#	define	MENTORSHIP_ERRORLN(x)
#	define	MENTORSHIP_EMPHASISLN(x)
#endif

#pragma pack(1)
// 师徒数据脚本
#define MENTORSHIP_CONFIG_FILENAME			"Scp\\MentorshipConfig.csv"

//============================= 基本系统 ====================================

// 师徒发布信息列表一页的大小
#define MENTORSHIP_ISSUE_PAGE_SIZE	8

// 师徒系统int的最大整数
#define MAX_MENTORSHIP_INT_VALUE	2147483647

// 自动发布师徒信息标识
enum TMentorshipAutoIssueFlag
{
	emMentorshipAutoIssueFlag_Close = 0,		// 关闭
	emMentorshipAutoIssueFlag_Open,				// 开启
};

// 师徒个人整型属性
enum TMentorshipPersonProp
{
	em_MshipPersonProp_Post = 0,				// 师徒职位
	em_MshipPersonProp_AutoIssueMFlag,			// 自动发布师傅信息标识
	em_MshipPersonProp_AutoIssuePFlag,			// 自动发布学徒信息标识

	em_MshipPersonProp_MSumShipVal,				// 导师关系总值
	em_MshipPersonProp_MWeekShipVal,			// 导师本周关系值
	em_MshipPersonProp_MLastWeekShipVal,		// 导师上周关系值
	em_MshipPersonProp_MLastSVOrder,			// 导师上周关系排名
	em_MshipPersonProp_MRecordTime,				// 导师记录时间

	em_MshipPersonProp_PSumShipVal,				// 学徒关系总值
	em_MshipPersonProp_PWeekShipVal,			// 学徒本周关系值
	em_MshipPersonProp_PLastWeekShipVal,		// 学徒上周关系值
	em_MshipPersonProp_PLastSVOrder,			// 学徒上周关系排名
	em_MshipPersonProp_PRecordTime,				// 学徒记录时间

	em_MshipPersonProp_MFWinAwardPreCount,		// 导师首胜奖励可领取次数
	em_MshipPersonProp_MFWinAwardCount,			// 导师首胜奖励次数
	em_MshipPersonProp_MFWinAwardTime,			// 导师首胜奖励时间

	em_MshipPersonProp_PFWinAwardPreCount,		// 学徒首胜奖励可领取次数
	em_MshipPersonProp_PFWinAwardCount,			// 学徒首胜奖励次数
	em_MshipPersonProp_PFWinAwardTime,			// 学徒首胜奖励时间

	emMshipPersonProp_NumPropCount,				// 属性个数,以上是整型的属性
};

// 师徒关系数据
struct SMentorshipInfo
{
	DWORD dwMentorshipID;						// 关系id
	DWORD dwMasterPDBID;						// 师傅玩家ID
	char  szMasterName[ACTORNAME_MAXSIZE];		// 师傅玩家名字
	DWORD dwPrenticePDBID;						// 学徒玩家ID
	char  szPrenticeName[ACTORNAME_MAXSIZE];	// 学徒玩家名字
	short nFaceID;								// 头像ID
	BYTE  nRelationType;						// 关系类型
	BYTE  nState;								// 关系状态
	bool  bPromoteFlag;							// 提升标识
	DWORD dwBuildTime;							// 建立时间
	DWORD dwSeverPDBID;							// 解除者玩家ID
	DWORD dwSeverTime;							// 解除时间

	SMentorshipInfo()
	{
		memset(this, 0, sizeof(SMentorshipInfo));
	}
};

// 个人师徒数据
struct SMentorshipPerson
{
	DWORD dwPDBID;								// 玩家ID
	BYTE  nAutoIssueMFlag;						// 自动发布师傅信息标识
	BYTE  nAutoIssuePFlag;						// 自动发布学徒信息标识
	int	  nMSumShipVal;							// 导师关系总值
	int	  nMWeekShipVal;						// 导师本周关系值
	int   nMLastWeekShipVal;					// 导师上周关系值
	int   nMLastSVOrder;						// 导师上周关系排名
	int   nMRecordTime;							// 导师记录时间
	int	  nPSumShipVal;							// 学徒关系总值
	int	  nPWeekShipVal;						// 学徒本周关系值
	int   nPLastWeekShipVal;					// 学徒上周关系值
	int   nPLastSVOrder;						// 学徒上周关系排名
	int   nPRecordTime;							// 学徒记录时间
	BYTE  nMFWinAwardPreCount;					// 导师首胜奖励可领取次数
	BYTE  nMFWinAwardCount;						// 导师首胜奖励次数
	int   nMFWinAwardTime;						// 导师首胜奖励时间
	BYTE  nPFWinAwardPreCount;					// 学徒首胜奖励可领取次数
	BYTE  nPFWinAwardCount;						// 学徒首胜奖励次数
	int   nPFWinAwardTime;						// 学徒首胜奖励时间

    SMentorshipPerson()
    {
        memset(this, 0, sizeof(SMentorshipPerson));
    }
};

// 师徒发布数据
struct SMentorshipIssue
{
	DWORD dwPDBID;								// 玩家ID
	BYTE  nIssueType;							// 发布类型
	char  szIssueInfo[MENTORSHIP_ISSUE_LEN];	// 发布内容
};

// 客户端师徒信息结构
struct SClientMentorShip
{
	DWORD dwMentorshipID;						// 关系ID
	DWORD dwPDBID;								// 玩家ID
	char  szName[ACTORNAME_MAXSIZE];			// 玩家名字
	short nFaceID;								// 头像ID
	bool  bIsOnline;							// 玩家是否在线
	BYTE  nPost;								// 关系中的职位
	BYTE  nRelationType;						// 关系类型
	BYTE  nState;								// 关系状态
	DWORD dwBuildTime;							// 建立时间
	DWORD dwSeverPDBID;							// 解除者玩家ID
	DWORD dwSeverTime;							// 解除时间

	SClientMentorShip()
	{
		memset(this, 0, sizeof(SClientMentorShip));
	}
};

// 师徒常量信息
struct SMentorshipConfig
{
	int  nMasterNeedLevel;						// 师傅要求等级
	int  nPrenticeNeedLevel;					// 学徒要求等级
	int  nFormalMasterCount;					// 正式师傅个数
	int  nInternMasterCount;					// 见习师傅个数
	int  nFormalPrenticeCount;					// 正式徒弟个数
	int  nInternPrenticeCount;					// 见习徒弟个数
	int  nPFWinAwardCount;						// 学徒首胜奖励次数
	int  nPFWinAwardTimeLen;					// 学徒首胜奖励拜师时间限制
	int  nMFWinAwardCount;						// 导师首胜奖励次数
	int  nMFWinAwardTimeLen;					// 导师首胜奖励拜师时间限制
	int  nServerTimeLen;						// 解散需要时长
	int  nWinShipVal;							// 胜利获取师徒值
	int  nFailShipVal;							// 失败获取师徒值
	int  nWeekShipVal;							// 周获取师徒最大值
	int	 nExpPercent;							// 经验加成百分比
	int  nHeroExpPercent;						// 经验加成百分比
	int  nGoldPercent;							// 金币加成百分比
	int  nMFWinPrizeID;							// 导师首胜奖励ID
	int  nPFWinPrizeID;							// 学徒首胜奖励ID

    SMentorshipConfig()
    {
        memset(this, 0, sizeof(SMentorshipConfig));
    }
};

#pragma pack()