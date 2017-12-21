/*******************************************************************
** 文件名:	E:\speed\Common\MailDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	邮件相关定义
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "NameLenDef.h"
#include "NetMessageDef.h"
#include "GameDef.h"
#include "IGameDatabaseDef.h"

//输出DEBUG信息
#define MAIL_DEBUG_OUTPUT
#ifdef MAIL_DEBUG_OUTPUT
#	define	MAIL_TRACELN(x)			TraceLn(x)
#	define	MAIL_WARNINGLN(x)		WarningLn(x)
#	define	MAIL_ERRORLN(x)			ErrorLn(x)
#	define	MAIL_EMPHASISLN(x)		EmphasisLn(x)
#else
#	define	MAIL_TRACELN(x)
#	define	MAIL_WARNINGLN(x)
#	define	MAIL_ERRORLN(x)
#	define	MAIL_EMPHASISLN(x)
#endif

#pragma pack(1)

//============================= 基本系统 ====================================
// 邮件自增索引最大值(5位)
#define MAIL_INDEX_MAX	99999
// 邮件生命周期最大值
#define MAIL_LIFE_TIME_LEN	24

// 收钱邮件附件CD时长
#define MAIL_RECV_PLUS_CD_LEN	1

// 邮件数据脚本
#define MAIL_CONFIG_FILENAME			"Scp\\MailConfig.csv"



// 邮件配置数据
struct SMailConfig
{
	int nMailFillID;										// 填充ID
	int nMailLifeTime;										// 有效时间（天）
	char  szTitle[MAIL_TITLE_MAXSIZE];						// 标题
	char  szContext[MAIL_CONTENT_MAXSIZE];					// 内容
	char  szSenderName[ACTORNAME_MAXSIZE];					// 发件者姓名	系统邮件填 系统
	int nPrizeIDCount;										// 个数
	int arrayPrizeID[MAIL_GOODS_MAX_NUM];				    // 显示奖励列表

	SMailConfig()
	{
		memset(this, 0, sizeof(SMailConfig));
	}
};

// 邮件基础数据
struct SMailBaseData
{
	LONGLONG llMailID;							// 邮件id
	BYTE  nType;								// 邮件类型
	BYTE  nSourceType;							// 资源来源类型
	char  szTitle[MAIL_TITLE_MAXSIZE];			// 标题			
	char  szSenderName[ACTORNAME_MAXSIZE];		// 发件者姓名
	DWORD dwSenderPDBID;						// 发送者PDBID
	bool  bRead;							    // 是否读取
	bool  bIsPlusData;							// 是否有附件
	DWORD dwRecvTime;							// 发件时间
	DWORD dwLifeTime;							// 生命时长 (单位:秒)

	SMailBaseData()
	{
		memset(this, 0, sizeof(SMailBaseData));
	}
};

typedef struct DBREQ_PARAM_MAILGOODS SMailGoodsInfo;

// 邮件详细数据
struct SMailDetailData
{
	LONGLONG llMailID;								// 邮件id
	char  szContext[MAIL_CONTENT_MAXSIZE];			// 内容
	int  nPlusMoney;								// 附送金钱
	int  nPlusExp;									// 附送经验
	BYTE  nPlusGoodsNum;							// 附件物品的个数
	SMailGoodsInfo sGoodsList[MAIL_GOODS_MAX_NUM];	// 附加物品id

	SMailDetailData()
	{
		memset(this, 0, sizeof(SMailDetailData));
	}
};

// 邮件发送数据
struct	SMailSendData
{
	BYTE  nType;											// 邮件类型		EMMailType
	BYTE  nSourceType;										// 资源来源类型	EMMailSourceType
	char  szTitle[MAIL_TITLE_MAXSIZE];						// 标题
	char  szContext[MAIL_CONTENT_MAXSIZE];					// 内容
	char  szSenderName[ACTORNAME_MAXSIZE];					// 发件者姓名	系统邮件填 系统
	DWORD dwSenderPDBID;									// 发送者PDBID  系统邮件填 0
	DWORD dwMasterPDBID;									// 主人PDBID
	bool  bIsPlusData;										// 是否有附件	发送邮件时不用填写
	DWORD dwRecvTime;										// 发件时间		发送邮件时不用填写
	DWORD dwLifeTime;										// 生命时长 (单位:小时)  发送邮件时不用填写
	int  nPlusMoney;										// 附送金钱
	int  nPlusExp;											// 附送经验
	BYTE  nPlusGoodsNum;									// 附件物品的个数
	SMailGoodsInfo sGoodsList[MAIL_GOODS_MAX_NUM];			// 附加物品id

	SMailSendData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 邮件节点缓存数据
struct SMailDBNodeData
{
	int  nQueueIndex;							// 节点的序号
	LONGLONG llMailID;							// 邮件id
	SMailSendData sendData;						// 邮件数据

	SMailDBNodeData()
	{
		memset(this, 0, sizeof(SMailDBNodeData));
	}
};

#pragma pack()