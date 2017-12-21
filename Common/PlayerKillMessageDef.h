/*******************************************************************
** 文件名:	PlayerKillMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/04/2016
** 版  本:	1.0
** 描  述:	PK消息定义
********************************************************************/

#ifndef __PLAYER_KILL_MESSAGE_DEF_H__
#define __PLAYER_KILL_MESSAGE_DEF_H__

#include "NetMessageDef.h"
#include "GameDef.h"

#pragma pack(1)

enum PK_TYPE
{
    PK_INVALID = 0, // 无效PK模式
    PK_CAMP,        // 阵营模式
    PK_FIGHT,       // 决斗模式
    PK_MAX,         // 最大类型
};

enum PK_STATE
{
    PK_STATE_NOMAL = 0,   // 正常状态
    PK_STATE_PREPARE,     // 准备状态
    PK_STATE_FIGHT,       // 决斗状态
    PK_STATE_MAX,         // 最大状态类型
};

enum PK_RESULT
{
    PK_RESULT_LOST = 0,   // 决斗-失败
    PK_RESULT_WIN,        // 决斗-胜利
    PK_RESULT_FLAT,       // 决斗-平
};

/////////////////////////// PK模块消息  ////////////////////////////
// 消息码定义键值
enum PLAYER_KILL_NET_MESSAGE
{
    PK_MSG_CHOOSE_TARGET = 0,		        // 选择目标
    PK_MSG_REQUEST_PK,		                // 请求决斗
    PK_MSG_HANDLE_INFO,		                // 操作信息
    PK_MSG_START_PK,		                // 开始PK
    PK_MSG_END_PK,                          // 结束PK
    PK_MSG_CANCEL_PK,                       // 取消PK
    PK_MSG_SHOW_MESSAGE_BOX,                // 显示提示框
};

// 客户端-场景服 选择目标   PK_MSG_CHOOSE_TARGET
struct SChooseTarget_CS
{
    UID     uidTarget;                      // 目标

    SChooseTarget_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-场景服 请求PK   PK_MSG_REQUEST_PK
struct SRequestPK_SS
{
    UID     uidOperator;                    // 请求决斗者

    SRequestPK_SS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-客户端 请求PK   PK_MSG_REQUEST_PK
struct SRequestPK_SC
{
    int     nRequestCount;                  // 请求决斗数量nRequestCount*SRequestPKInfo

    SRequestPK_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SRequestPKInfo
{
    UID     uidOperator;                    // 请求决斗者
    DWORD   dwTime;                         // 请求决斗时间
    char    szInfo[256];                    // 决斗提示信息

    SRequestPKInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 客户端-场景服 操作信息   PK_MSG_HANDLE_INFO
struct SHandInfo_CS
{
    UID     uidOperator;                    // 决斗者
    bool    bAgree;                         // 是否同意

    SHandInfo_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-场景服 开始PK   PK_MSG_START_PK
struct SStartPK_SS
{
    UID       uidTarget;                     // PK目标
    DWORD     nPKStartTime;                  // PK开始时间
    Vector3   vNewLoc;                       // 创建怪物位置

    SStartPK_SS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-客户端 开始PK   PK_MSG_START_PK
struct SStartPK_SC
{
    UID       uidTarget;                     // PK目标
    Vector3   vLoc;                          // 中心点

    SStartPK_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-场景服 PK结束   PK_MSG_END_PK
struct SEndPK_SS
{
    UID     uidWin;                    // 决斗胜利者
    UID     uidLost;                   // 决斗失败者
    int     nResult;                   // 决斗结果
    char    szReason[64];              // 结束原因

    SEndPK_SS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-客户端 结束PK   PK_MSG_END_PK
struct SEndPK_SC
{
    UID       uidTarget;                     // PK目标
    int       nResult;                       // 比赛结果
    bool      bForceEnd;                     // 强制结束

    SEndPK_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 客户端-场景服 取消PK   PK_MSG_CANCEL_PK
struct SCancelPK_CS
{
    bool      bCancel;                     // 取消PK

    SCancelPK_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-客户端 显示提示框   PK_MSG_SHOW_MESSAGE_BOX
struct SShowMessageBox_SC
{
    int        nType;                                               // 消息框类型
    char       szCaption[COMMON_MESSAGE_BOX_CAPTION_LEN];			// 消息框标题
    char       szText[COMMON_MESSAGE_BOX_TEXT_LEN];			        // 消息框文本

    SShowMessageBox_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack()
#endif	// __PLAYER_KILL_MESSAGE_DEF_H__