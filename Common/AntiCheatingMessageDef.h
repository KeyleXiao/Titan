/*******************************************************************
** 文件名:	AntiCheatingMessageDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008-2010 All Rights Reserved
** 创建人:	陈开明
** 日  期:	2010-9-29
** 版  本:	1.0
** 描  述:	反外挂系统公共数据和通信协议定义

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#pragma pack(1)


#define ANTICHEATING_ENCRYPT_LEN		128		// 公钥长度
#define MAX_RECORD_COUNT				512		// 检测记录的最大数量
#define MAX_DETECT_NAME_LEN				128		// 检测名（进程名或模块名或SHA1值或特征代码等）长度

// 加密算法定义
enum EAntiCheatingEncrypt
{
	ANTICHEATING_ENCRYPT_NO               =  0,   // 不加密
	ANTICHEATING_ENCRYPT_RSA              =  1,   // RSA加密
	ANTICHEATING_ENCRYPT_RC4              =  2,   // RC4加密
	ANTICHEATING_ENCRYPT_CUSTOM           =  3,   // 自定义的加密方式
	ANTICHEATING_ENCRYPT_MAX              =  4,   
};

// 开关状态
enum EAntiCheatingSwitchState
{
	SWITCH_STATE_OFF		= 0,	// 关闭
	SWITCH_STATE_ON			= 1		// 开启
};

// 处理方式
enum EAntiCheatingDisposalMode
{
	DISPOSAL_MDOE_NOT			= 0,	// 不处理
	DISPOSAL_MDOE_DISCONNECT	= 1,	// 断线
	DISPOSAL_MDOE_BAN_USERID	= 2,	// 封号
	DISPOSAL_MDOE_ATTENUATION	= 3,	// 自动进入衰减模式
	DISPOSAL_MDOE_IMPRISONMENT	= 4,	// 自动抓监牢
	DISPOSAL_MDOE_VERIFY_CODE	= 5,	// 自动出验证码
	DISPOSAL_MDOE_KILL_PROCESS	= 6,	// 终结外挂进程
	DISPOSAL_MDOE_EXIT_GAME		= 7,	// 退出游戏
	DISPOSAL_MDOE_KILLSELF		= 8,	// 杀掉自己
	DISPOSAL_MDOE_MAX_COUNT				// 处理方式个数
};

// 检测方式
enum EAntiCheatingDetectlMode
{
	DETECT_MDOE_PROCESS_NAME	= 0,	// 检测进程名
	DETECT_MDOE_MODULE_NAME		= 1,	// 检测模块名
	DETECT_MDOE_WINDOW_TITLE	= 2,	// 检测窗口标题
	DETECT_MDOE_SHA1_VALUE		= 3,	// 检测SHA1值
	DETECT_MDOE_FEATURE_CODE	= 4,	// 检测特征代码
	//DETECT_MDOE_REGISTRY		= 5,	// 检测注册表
	DETECT_MDOE_ANTIDEBUG		= 5,	// 检测注册表
	DETECT_MDOE_MAX_COUNT				// 检测方式个数
};

enum ESuspectClientType
{
	SUSPECT_DLL,					// 可疑的DLL
	SUSPECT_ACTIVE,					// 非活动窗口收到鼠标消息
	SUSPECT_THREAD,					// 非法的线程执行
};

// 检测的外挂特征库记录（与后台配置的外挂特征库的记录对应）
struct SDetectRecord
{
	UINT uiRecordID;							// 记录ID
	EAntiCheatingDetectlMode eDetectMode;		// 检测方式
	char szParam1[MAX_DETECT_NAME_LEN];			// 检测参数1（进程名或模块名或窗口标题或SHA1值或特征代码，和检测方式对应）
	DWORD dwParam2;								// 检测参数2（检测对象在模块文件中的偏移地址或模块文件的字节大小）
	UINT uiParam3;								// 检测参数3（检测对象在模块文件中的字节数）
	EAntiCheatingDisposalMode eDisposalMode;	// 处理方式

	SDetectRecord()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
enum EMMSGANTICHEATMOUDLE
{
	EMMSG_ANTICHEATING_SEND_CONFIG=1,			// 反外挂服务器端的配置模块主动发送给客户端的反外挂配置信息
	EMMSG_ANTICHEATING_SEND_DETECT_RECORDS,		// 反外挂服务器端的配置模块主动发送给客户端的外挂特征库配置信息
	EMMSG_ANTICHEATING_DISPOSAL_USER,			// 客户端反外挂模块处理可疑玩家的请求和应答报文
	EMMSG_ANTICHEATING_SAVE_DISPOSAL_RECORD,	// 验证码服务器向场景服发送的保存到数据库处理记录
	EMMSG_ANTICHEATING_NOTIFY_DETECT,			// 通知客户端反外挂进行检测的请求和应答
	EMMSG_CLOSE_NEW_ANTICHEATING_CONFIG,		// 关闭新反外挂系统

	EMMSG_REPORT_ROBOT_CLIENT=28,				// 上报机器人客户端
	EMMSG_REPORT_SUSPECT_CLIENT,				// 上报可疑客户端
};
// 反外挂服务器端的配置模块主动发送给客户端的反外挂配置信息
#define MSG_ANTICHEATING_SEND_CONFIG				EMMSG_ANTICHEATING_SEND_CONFIG
struct SMsgAntiCheatingConfig_SC 
{
	EAntiCheatingSwitchState eSwitchState;			// 开关状态
	int nDetectInterval;							// 定时检测时间间隔（单位：分钟）
	int nReportInterval;							// 定时上报检测结果时间间隔（单位：秒）
	EAntiCheatingEncrypt   eEncryptMethod;			// 加密方法
	char szKeyData[ANTICHEATING_ENCRYPT_LEN];		// 公钥内容

	SMsgAntiCheatingConfig_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 反外挂服务器端的配置模块主动发送给客户端的外挂特征库配置信息
#define MSG_ANTICHEATING_SEND_DETECT_RECORDS		EMMSG_ANTICHEATING_SEND_DETECT_RECORDS
struct SMsgAntiCheatingDetectRecord_SC
{
	unsigned int uiRecordCount;							// 检测记录的条数
	SDetectRecord sDetectRecords[MAX_RECORD_COUNT];

	SMsgAntiCheatingDetectRecord_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 客户端反外挂模块处理可疑玩家的请求和应答报文
#define MSG_ANTICHEATING_DISPOSAL_USER		EMMSG_ANTICHEATING_DISPOSAL_USER
// 客户端反外挂模块请求处理可疑玩家时上报给反外挂服务器端的处理模块的报文
struct SMsgAntiCheatingDisposalPlayer_CS
{
	UINT uiRecordID;							// 记录ID
	EAntiCheatingDetectlMode eDetectMode;		// 检测方式
	char szProcessName[MAX_DETECT_NAME_LEN];	// 检测到的可疑进程名
	char szModuleName[MAX_DETECT_NAME_LEN];		// 检测到的可疑模块名
	char szModulePath[MAX_PATH];				// 检测到的可疑模块全名
	char szParam1[MAX_DETECT_NAME_LEN];			// 检测参数1（进程名或模块名或窗口标题或SHA1值或特征代码，和检测方式对应）
	DWORD dwParam2;								// 检测参数2（检测对象在模块文件中的偏移地址）
	UINT uiParam3;								// 检测参数3（检测对象在模块文件中的字节数）
	EAntiCheatingDisposalMode eDisposalMode;	// 处理方式

	SMsgAntiCheatingDisposalPlayer_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 验证码服务器向场景服发送的保存到数据库处理记录
#define MSG_ANTICHEATING_SAVE_DISPOSAL_RECORD		EMMSG_ANTICHEATING_SAVE_DISPOSAL_RECORD
// 处理记录
struct SMsgAntiCheatingDisposalRecord_YC
{
	ulong ulClientID;
	DWORD dwPDBID;
	SMsgAntiCheatingDisposalPlayer_CS sDisposalPlayer;
	bool bIsDisposed;

	SMsgAntiCheatingDisposalRecord_YC()
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 通知客户端反外挂进行检测的请求和应答
#define MSG_ANTICHEATING_NOTIFY_DETECT			EMMSG_ANTICHEATING_NOTIFY_DETECT

// 关闭新反外挂系统
#define MSG_CLOSE_NEW_ANTICHEATING_CONFIG		EMMSG_CLOSE_NEW_ANTICHEATING_CONFIG

// 上报机器人客户端
#define MSG_REPORT_ROBOT_CLIENT					EMMSG_REPORT_ROBOT_CLIENT
// 上报可疑客户端
#define MSG_REPORT_SUSPECT_CLIENT				EMMSG_REPORT_SUSPECT_CLIENT
struct SMsgReportSuspectClient
{
	int type;							// 类型
	int len;							// 数据长度
	SMsgReportSuspectClient()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()