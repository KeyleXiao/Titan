/*******************************************************************
** 文件名:	ActorDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2016 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	2/25/2016
** 版  本:	1.0
** 描  述:	角色创建结构体定义

********************************************************************/

#pragma once

#include "GameDef.h"
#include "GatewayServiceDef.h"

#pragma pack(push, 1)

using Gateway::ClientID;

// 登进模式
enum ELoginMode
{
    elogin_unknown,
    elogin_online,                  // 上线进
    elogin_in_process,              // 进程内登进
    elogin_cross_process,           // 跨进程登进
    elogin_cross_gameworld,         // 跨区登进
    elogin_reconnect_center,        // 重连中心服
};

// 登出模式
enum ELogoutMode
{
    elogout_unknown,
    elogout_offline,                // 下线登出
    elogout_in_process,             // 进程内切地图登出
    elogout_cross_process,          // 跨进程切地图登出
    elogout_cross_gameworld,        // 跨区登出
    elogout_disconnect,             // 掉线登出
    elogout_disconnect_center,		// 与中心服断线登出
};

enum ACTOR_CREATE_CONTEXT_FLAG
{
    CONTEXT_FLAG_NONE           = 0x0,             /* 无 */
    CONTEXT_FLAG_ATTACH_THREAD       = 0x01,       /* 附到场景LOGIC线程 */          
};

enum EDateSrcType
{
    eds_type_cache,    // 内存数据
    eds_type_db        // 数据库
};

// 角色创建上下文
struct SActorCreateContext
{
    ClientID            client;
    BYTE                nFlag;                  // 附加标识ACTOR_CREATE_CONTEXT_FLAG  
    UDBID               udbid;
    DWORD               dwIdCard;               // 身份证ID 只登陆创建角色有用
    bool                bIsAdult;               // 是否成年人 只登陆创建角色有用
    DWORD	            dwPassportFlags;		// 通行证相关标识 PassportFlags
    DWORD               dwServiceFlags;         // 服务器标识 ServiceFlags
    char                szPwd[GAME_PWD_MAXSIZE];  // 登录密码

    PDBID               pdbid;  
    int                 nSceneID;               // 创建后在哪个场景
    int                 nSceneSerialNo;          // 约束nSceneID（创建时nSceneID可能已经复用)
    Vector3             locBorn;                // 出生点
    float               fAngle;                 // 朝向
    EDateSrcType        eDStype;                // 角色数据来源

    //  以下是内存数据 ...
    int                 nCreateDataLen;         // 创建数据长度
    int                 nCreateDataFlag;        // TO_
    int                 nUpdateDataLen;         // 更新数据长度
    int                 nUpdateDataFlag;        // TO_
    // 创建数据...      更新数据...


    SActorCreateContext()
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack(pop)