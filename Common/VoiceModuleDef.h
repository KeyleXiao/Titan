/*******************************************************************
** 文件名:	VoiceModuleDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音游戏模块定义
** 应  用:  礼物系统
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __D_VoiceModule_H__
#define __D_VoiceModule_H__

#include "VoiceDef.h"


#pragma pack(1)

/////// 公共宏定义 ///////////////////////////////////////////////////////////////////


// 语音配置
#define VOICE_CONFIG_FILENAME		"Scp\\Voice.xml"

// 语音礼物配置
#define VOICE_GIFT_CONFIG_FILENAME	"Scp\\VoiceGift.csv"


#define VOICE_GM_NICKNAME	GM_NICKNAME




// tolua_begin


#define	 VOICE_MAX_GIFT_LEVEL		8		// 礼物最高等级


#define  VOICE_MAX_GIFT_REBATE_COUNT 10		// 打折信息最大条数



/////// 公共枚举定义 ///////////////////////////////////////////////////////////////////


// 语音礼物配置广播类型
enum EMVoiceGiftBroadcastType
{
	VoiceGiftBroadcast_TextRoom = 0,		// 仅文字广播语音房间
	VoiceGiftBroadcast_TextRoomEffectBoth,	// 文字广播语音房间，光效、喇叭广播双方
	VoiceGiftBroadcast_Room,		// 文字、光效、喇叭广播房间
	VoiceGiftBroadcast_Country,		// 文字、光效、喇叭广播双方国家
	VoiceGiftBroadcast_World,		// 文字、光效、喇叭广播双方世界
	VoiceGiftBroadcast_All,			// 文字、光效、喇叭广播全游戏区

	VoiceGiftSchemeBroadcast_Max,
};



// 语音配置文件数值索引ID
enum EMVoiceConfigNumID
{
	VoiceConfigNumID_VoiceGiftOpen = 0,		// 语音送礼是否开放
	VoiceConfigNumID_Data2,					// 备用2
	VoiceConfigNumID_Data3,					// 备用3
	VoiceConfigNumID_Data4,					// 备用4
	VoiceConfigNumID_Data5,					// 备用5
	VoiceConfigNumID_Data6,					// 备用6
	VoiceConfigNumID_Data7,					// 备用7
	VoiceConfigNumID_Data8,					// 备用8
	VoiceConfigNumID_Data9,					// 备用9
	VoiceConfigNumID_Data10,				// 备用10

	VoiceConfigNumID_Max,
};


// 赠送礼物位标记
enum EMVoiceSendGiftFlag
{
	VoiceSendGiftFlag_Low			= 0x1,	/// 低调送礼,只房间看到
	VoiceSendGiftFlag_Private		= 0x2,	/// 私密送礼,只有2人能看到
	//VoiceSendGiftFlag_Unuse		= 0x4,	/// 
	//VoiceSendGiftFlag_Unuse		= 0x8,	///
	//VoiceSendGiftFlag_Unuse		= 0x10,	///
	//VoiceSendGiftFlag_Unuse		= 0x20,	/// 
};


/////// 客户端公共枚举定义/////////////////////////////////////////////


/////// 服务器公共枚举定义/////////////////////////////////////////////


///// 公用数据结构定义 //////////////////////////////////////////////


// 语音配置数据
struct SVoiceRebateData
{
	BYTE nOpen;		// 是否开启
	BYTE nMonth;	// 打折月
	BYTE nDay;		// 打折日
		
	char szTips[VOICE_TITLE_MAXSIZE];

	SVoiceRebateData(void)
	{
		memset( this, 0, sizeof(SVoiceRebateData) );
	}
};

// 语音配置结构
struct SVoiceSchemeInfo
{
	int nVoiceConfigNum[VoiceConfigNumID_Max];		// 数值配置
	SVoiceRebateData VoiceRebateData[VOICE_MAX_GIFT_REBATE_COUNT];	// 打折信息

	SVoiceSchemeInfo(void)
	{
		memset( this, 0, sizeof(SVoiceSchemeInfo) );
	}
};

// 语音礼物配置结构
struct SVoiceGiftSchemeInfo
{
	WORD nID;		// ID
	BYTE nLevel;	// 等级
	DWORD dwPrice;	// 单价
	BYTE nBroadcast;// 广播范围
	BYTE nRebate;	// 折扣

	DWORD dwSendPoint;		// 赠送方获得积分数
	DWORD dwSendGoodsID;	// 赠送方获得物品ID
	DWORD dwMaleSendTitleID;	// 赠送方获得称号ID 男
	DWORD dwFemaleSendTitleID;	// 赠送方获得称号ID 女
	DWORD dwSendBuffID;		// 赠送方获得BUFFID
	DWORD dwSendFlower;		// 赠送方获得鲜花数

	DWORD dwRecvBlueDiamond;	// 接收方获得蓝钻数
	DWORD dwRecvGoodsID;	// 接收方获得物品ID
	DWORD dwMaleRecvTitleID;	// 接收方获得称号ID 男
	DWORD dwFemaleRecvTitleID;	// 接收方获得称号ID 女
	DWORD dwRecvBuffID;		// 接收方获得BUFFID
	DWORD dwRecvFlower;		// 接收方获得鲜花数

	BYTE nEnable;							// 是否显示在界面上 1 显示 0 不显示
	char szName[VOICE_NAME_MAXSIZE];		// 名称

	BYTE nCoolTime;			// 冷却时间

	SVoiceGiftSchemeInfo(void)
	{
		memset( this, 0, sizeof(SVoiceGiftSchemeInfo) );
	}
};

// 语音礼物配置结构扩展
struct SVoiceGiftSchemeInfoEx : public SVoiceGiftSchemeInfo
{
	WORD nIconPos;							// 动画位置
	WORD nShowPos;							// 显示位置
	DWORD dwEffectID;						// 场景层光效ID
	DWORD dwUIEffectID;						// UI层光效ID
	char szTooltips[VOICE_LONGSTR_MAXSIZE];	// tooltips描述

	char szWords1[VOICE_TITLE_MAXSIZE];		// 赠言1
	char szWords2[VOICE_TITLE_MAXSIZE];		// 赠言2
	char szWords3[VOICE_TITLE_MAXSIZE];		// 赠言3
};


///// 客户端公用数据结构定义 //////////////////////////////////////////////





// tolua_end


#pragma pack()


#endif	// __D_VoiceModule_H__