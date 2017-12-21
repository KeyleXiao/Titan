/*******************************************************************
** 文件名:	SharePropertyDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-5-16
** 版  本:	1.0
** 描  述:	共享属性公共定义

			
********************************************************************/

#ifndef __COMMON_SHAREPROPERTY_DEF_H__
#define __COMMON_SHAREPROPERTY_DEF_H__

#pragma pack(1)


// 人物共享属性
struct SSharePersonProperty
{
    char		szName[GAME_NAME_MAXSIZE];	// 角色名称
    DWORD		dwWorldID;					// 世界ID
    DWORD		dwUDBID;					// 帐号ID
    DWORD		dwPDBID;					// 角色ID
    DWORD		dwIdCard;					// 身份证号（由数据库生成的身份证替代号）
    int         nFaceID;                    // 头像ID
    int			nSkinID;					// 皮肤
    int			nVocation;					// 职业
    WORD		wLevel;						// 等级
    BYTE		bySex;						// 性别
    BYTE		byState;					// 当前角色自身状态
    DWORD       dwGameState;                // 角色游戏状态 ACTOR_GAME_STATE

    DWORD       dwTeamID;                   // 组队ID
    DWORD		dwKinID;					// 家族ID
    DWORD		dwClanID;					// 帮会ID
    int         nFight;                     // 战力
    int         nTicket;                    // 点券
    int         nGlod;                      // 金币
    int         nBindGlod;                  // 绑定金币
    int         nDayActivity;               // 天活跃度
    int         nWeekActivity;              // 周活跃度

    DWORD		dwSceneID;					// 场景ID
    DWORD		dwMapID;					// 地图ID

    DWORD		clientID;					// 网关分配序号
    DWORD		serverID;					// 服务器ID
    BYTE		nServerSerialID;			// 场景服序号

    BYTE        byMatchType;                 // 排位段位信息 类型nMatchTypeID = 8  
    int         nRankScore;
    int         nRankGrade;
    int         nPrivilege;                  // 权限
    int         nKingRankOrder;              // 王者排位排名

    SSharePersonProperty(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景共享属性
struct SShareSceneProperty
{
    DWORD		dwSceneID;
    DWORD		dwMapID;
    bool		bEnable;
    DWORD		dwServerId;
    DWORD		dwSerialID;

    SShareSceneProperty(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


#pragma pack()

#endif	// __COMMON_SHAREPROPERTY_DEF_H__