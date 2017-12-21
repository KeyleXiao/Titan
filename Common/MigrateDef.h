/*******************************************************************
** 文件名:	MigrateDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	7/15/2015
** 版  本:	1.0
** 描  述:	

modify:ljw
			数据迁移相关公共头
********************************************************************/

#pragma once

#include "GameDef.h"
#include "GatewayServiceDef.h"
#include "ActorDef.h"
#include "ILoginServerProtocol.h"
#include "GatewayServiceDef.h"

using Gateway::ClientID;
using Gateway::SessionID;

#define __SWITCH_SCENE_PRINT__

#define MAX_SCENE_PLAYER_COUNT 200


#pragma pack(push, 1)

// 消息定义
enum
{
    MSG_REQ_SWITCH_SCENE_XS,            // 请求切换场景

    MSG_REQ_ENTER_SCENE_XS,             // 请求进场景
    MSG_RET_ENTER_SCENE_SX,             // 请求进场景返回
    MSG_NOTIFY_OUT_COMPLETE_SS,         // 通知迁入端迁出完成

    MSG_SWITCH_SCENE_SC,                // 通知客户端切换地图
    MSG_SCENE_LOADING_PROGRESS_CS,      // 客户端通知场景加载进度
    MSG_SCENE_LOADING_PROGRESS_SC,      // 场景服广播角色加载场景进度

    MSG_REQ_GAVE_LOGIN_AUTHORITY_XL,    // 请求登录服授权登录(跨区切场景)
    MSG_REQ_GAVE_LOGIN_AUTHORITY_LX,    // 请求登录服授权登录返回(跨区切场景)
    MSG_NOTIFY_CLIENT_REDIRECT_XC,      // 通知客户端重定向

    MSG_NOTIFY_CLIENT_SWITCH_STATE,     // 让客户端切状态

    MSG_NOTIFY_CLIENT_RELOGIN,          // 通知客户端重新登录

    MSG_ACTOR_LOGIN_OK,                 // 通知社会服角色上线

	MSG_REQ_SWITCH_SCENE_CS,			// 客户端请求切换场景

	MSG_REQ_PRE_ENTER_SCENE_XS,			// 请求预进场景
};

struct MIGRATE_ID_INFO
{
    unsigned long long nFrom: 8;
    unsigned long long nWorldID : 16;
    unsigned long long nZoneServerID : 8;
    unsigned long long nSerialNo : 32;

    MIGRATE_ID_INFO()
    {
        memset(this, 0, sizeof(*this));
    }
};
typedef unsigned long long MIGRATE_ID;

// 场景节点
struct SSceneNode
{
    int nWorldID;
    int nZoneServerID;
    int nSceneID;
    int nSceneSN;

    SSceneNode()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景节点
struct SSceneNodeEx : public SSceneNode
{
	int nPlayerCount;
	int nPreEnterPlayerCount;

    SSceneNodeEx()
    {
        memset(this, 0, sizeof(*this));
    }
};

enum ESwitchScene_Reason
{
    eswitchscene_normal,            // 进出静态场景
    eswitchscene_enter_battlefield, // 进战场
    eswitchscene_out_battlefield,   // 出战场
};


enum EMapType
{
	emaptype_main,		// 主城地图
	emaptype_dynamic,	// 动态地图
};

// 请求切场景 MSG_REQ_SWITCH_SCENE_XS
struct SMsgReqSwitchScene_XS
{
    PDBID idActor;  
    SSceneNode destScene; //目标场景 
    Vector3 locBorn;      // 出生点
    float fBornAngle;   // 出生朝向
    ESwitchScene_Reason eReason;

    SMsgReqSwitchScene_XS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求预进场景上下文
struct SPreEnterSceneContext
{
	SSceneNode destNode;
	int nPlayerCount;
	// nPlayerCount * SPreEnterScenePlayerInfo 

	SPreEnterSceneContext()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SPreEnterScenePlayerInfo
{
	PDBID idActor;
	ClientID client;
	int nDstSceneID;

	SPreEnterScenePlayerInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};
// 请求预进场景 MSG_REQ_PRE_ENTER_SCENE_XS
struct SMsgReqPreEnterScene_XS
{
	int nCount;			// 预进场景的玩家个数
	int nDstSceneID;	// 目标场景
	// nCount * SPreEnterScenePlayerInfo

	SMsgReqPreEnterScene_XS()
	{

		memset(this, 0, sizeof(*this));
	}
};

// 请求进入场景 MSG_REQ_ENTER_SCENE_XS
struct SMsgReqEnterScene_XS
{
    DWORD dwUDBID;
    DWORD dwIdCard;         // 身份证ID (只登录时有用)
    bool  bIsAdult;         // 是否是成年人(只登录时有用)
    DWORD dwPassportFlags;	// 通行证相关标识 PassportFlags(只登录时有用)
    DWORD dwServiceFlags;   // 服务器标识 ServiceFlags(只登录时有用)
    char  szPwd[GAME_PWD_MAXSIZE];  // 登录密码
         
    PDBID idActor;
    ESwitchScene_Reason eReason;
    ClientID client;
    SessionID session;
    MIGRATE_ID mid;
    SSceneNode srcScene;    // 源场景
    SSceneNode destScene;   // 目标场景
    Vector3 locBorn;        // 出生点
    float fBornAngle;       // 出生朝向
    bool bContinuePlay;     // 是否续玩
    WORD wSkinID;           // 静态皮肤ID，用于在加载场景时将皮肤预先加载提升体验
    WORD wHeroID;           // 静态英雄ID

    int nCreateDataLen;
    int nCreateDataFlag;
    int nUpdateDataLen;
    int nUpdateDataFlag;
    // 创建数据...  更新数据...

    SMsgReqEnterScene_XS()
    {
        memset(this, 0, sizeof(*this));
        client = INVALID_CLIENT;
    }
};

// 请求进入场景返回 MSG_RET_ENTER_SCENE_SX
struct SMsgRetEnterScene_SX
{
    int nRetCode;
    PDBID idActor;
    MIGRATE_ID mid;

    SMsgRetEnterScene_SX()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 通知迁入端迁出完成 MSG_NOTIFY_OUT_COMPLETE_SS
struct SMsgNotifyOutComplete_SS
{
    PDBID idActor;
    MIGRATE_ID mid;

    SMsgNotifyOutComplete_SS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 通知客户端切换场景 MSG_SWITCH_SCENE_SC
struct SMsgSwitchScene_SC
{
    PDBID idActor;
    WORD  wSkinID;      // 皮肤ID
    int  nSceneID;
    int  nMapID;
    Vector3 vecBornPos; 
    bool isContinuePlay;

    SMsgSwitchScene_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 客户端通知场景服加载场景进度 MSG_SCENE_LOADING_PROGRESS_CS
struct SMsgSceneLoadingProgress_CS
{
    PDBID idActor; // 校验
    int nProgress;
    unsigned long lPing;

    SMsgSceneLoadingProgress_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服广播客户端加载进度 MSG_SCENE_LOADING_PROGRESS_SC
struct SMsgSceneLoadingProgress_SC
{
    PDBID idActor;
    int nProgress;
    unsigned long lPing;

    SMsgSceneLoadingProgress_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求登录服授权登录(跨区) MSG_REQ_GAVE_LOGIN_AUTHORITY_XL
struct SMsgReqGaveLoginAuthority_XL
{
    PDBID idActor;
    MIGRATE_ID mid;

    SMsgReqGaveLoginAuthority_XL()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求登录服授权登录返回(跨区) MSG_RET_GAVE_LOGIN_AUTHORITY_LX
struct SMsgRetGaveLoginAuthority_LX
{
    PDBID idActor;
    MIGRATE_ID mid;
    int nRetCode;
    int nAuthorityCode;

    SMsgRetGaveLoginAuthority_LX()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 通知客户端重定向 MSG_NOTIFY_CLIENT_REDIRECT_XC
struct SMsgNotifyClientRedirect_XC
{
    int nIP; 
    int nPort;
    int nAuthorityCode;

    SMsgNotifyClientRedirect_XC()
    {
        memset(this, 0, sizeof(SMsgNotifyClientRedirect_XC));
    }
};

// 通知客户端切状态 MSG_NOTIFY_CLIENT_SWITCH_STATE
struct SMsgNotifySwitchState_SC
{
    int nNextState;

    SMsgNotifySwitchState_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 通知客户端重新登录 MSG_NOTIFY_CLIENT_RELOGIN
struct SMsgNotifyReLogin_SC
{
    int nReason;

    SMsgNotifyReLogin_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgActorLogin_OK
{
    DWORD dwDBID;
    ClientID client;

    SMsgActorLogin_OK() : dwDBID(0), client(INVALID_CLIENT)
    {
    }
};

// 客户端请求切换场景 MSG_REQ_SWITCH_SCENE_CS
struct SMsgReqSwitchScene_CS
{
	DWORD dwGameWorldID;
	DWORD dwServerID;
	DWORD dwSceneID;

	SMsgReqSwitchScene_CS()
	{
        memset(this, 0, sizeof(*this));
	}
};

#pragma pack(pop)

