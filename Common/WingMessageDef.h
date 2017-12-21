/*******************************************************************
** 文件名:	WingMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	翅膀消息定义
********************************************************************/

#ifndef __WING_MESSAGE_DEF_H__
#define __WING_MESSAGE_DEF_H__

#include "NetMessageDef.h"

#pragma pack(1)

/////////////////////////// 翅膀模块消息  ////////////////////////////
// 消息码定义键值
enum WING_NET_MESSAGE
{
	WING_MSG_SET_WING_STATE = 0,		        // 设置翅膀状态
    WING_MSG_SET_WING_INFO,		                // 设置翅膀信息
    WING_MSG_UPDATE_WING_INFO,		            // 更新翅膀信息
    WING_MSG_UPDATE_FLY_TIME,                   // 更新飞行时间
};

// 客户端-场景服 设置翅膀状态   WING_MSG_SET_WING_STATE
struct SSetWingState_CS
{
    int    nStateType;       // 翅膀状态
    bool   bEncounterFlag;   // 是否遭遇敌人

    SSetWingState_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-客户端 设置翅膀信息   WING_MSG_SET_WING_INFO
struct SSetWingInfo_SC
{
    BYTE  byWingID;
    BYTE  byStateType;       // 翅膀状态

    SSetWingInfo_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-客户端 更新翅膀信息   WING_MSG_UPDATE_WING_INFO
struct SUpdateWingInfo_SC
{
    int    nStateType;       // 翅膀状态

    SUpdateWingInfo_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-客户端 更新飞行时间   WING_MSG_UPDATE_FLY_TIME
struct SUpdateFlyTime_SC
{
    int    nStateType;       // 翅膀状态
    int    nTime;            // 飞行时间

    SUpdateFlyTime_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack()
#endif	// __WING_MESSAGE_DEF_H__