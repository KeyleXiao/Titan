//==========================================================================
/**
* @file	  : DKinEvent.h
* @author : xiaobao
* created : 2009-3-7
* purpose : 家族消息码和结构定义
*/
//==========================================================================
#ifndef __DKinEvent_H__
#define __DKinEvent_H__

#include "KinDef.h"
#include "EventRefBase.h"

/***************************************************************/
///////////////////// 家族事件码 SOURCE_TYPE_KIN////////////////
/***************************************************************/
#pragma pack(1)

// =========================================单纯的消息回应============================================


// 家族从数据库数据读取完毕
//EVENT_KIN_READ_DB_FINISHED						   

// 更新家族ID事件
//EVENT_KIN_UPDATEID
// 消息体
struct SEventKin_UpdateKinID: public event_base
{
    DWORD dwPDBID;										// 玩家ID
    DWORD dwKinID;                                      // 家族ID
};

//EVENT_KIN_QUIT
struct SEventKin_Quit: public event_base
{
    DWORD dwKinID;
    DWORD dwActorID;
};

//EVENT_KIN_DELMEMBER
struct SEventKin_DelMember : public event_base
{
    DWORD dwKinID;										// 家族ID
    DWORD dwActorID;										// 玩家ID
};

// 申请加入家族
//EVENT_KIN_ADDAPPLY								    
struct SEventKin_AddApply_OC
{
    DWORD dwPDBID;										// 玩家ID
    char  szName[KIN_NAME_SIZE];						// 家族名字
    char  nLevel;						                // 等级
    DWORD dwPower;                                      // 战力
    int   nVocation;                                   // 职业
    int   nFaceID;                                      // 头像ID
};


// 拒绝申请加入家族
//EVENT_KIN_REMOVEAPPLY								
struct SEventKin_RemoveApply_OC
{
   	DWORD dwKinID;										// 现在的家族ID
	DWORD dwPDBID;										// 玩家ID
	bool  bAutoAccpet;									// 是否自动收人
	BYTE  nLevelLimit;									// 招人等级限制
	DWORD dwVocationLimit;								// 招人职业限制
};

// 家族列表项刷新
//EVENT_KIN_BASEDATAUPDATE
struct SEventKin_BaseDataUpdate_C
{
	int nPlace;											// 家族排名
};	

// 成员信息返回
//EVENT_KIN_LOOKUPMEMBER
struct SEventKin_LookUpMember_C
{
	DWORD dwPDBID;										// 玩家ID
	char szName[32];									// 名字
	int nLevel;											// 等级
	int nVocation;										// 职业
	int nGrade;											// 族内等级
	char szRemark[32];									// 备注
};

// 家族成员联系信息列表项刷新
//EVENT_CONTACT_BASEDATAUPDATE
struct SEventContact_BaseDataUpdate_C
{
	int dwKinID;										// 家族ID
};	

// 添加成员
//EVENT_KIN_ADD_MEMBER				    
struct SEventKin_AddMember_OC
{
    DWORD   dwUDBID;                            // 账号ID
    DWORD	dwPDBID;							// 玩家ID
    BYTE	nLevel;								// 玩家等级
    BYTE	nIdentity;							// 族内职位
    bool	bIsOnline;							// 玩家是否在线
    int		nLogoutTime;						// 玩家最后下线时间
    BYTE	nVocation;							// 玩家职业
    BYTE    nFaceID;                            // 头像ID
    bool	bIsPublicOnline;					// 玩家是否在公共区在线
    DWORD   dwBattleVal;						// 战力值
    DWORD   dwSrcBattleVal;						// 上一次战力值
    BYTE	dwBVOrder;							// 战力值排行
    BYTE	dwLastOrder;						// 上一次战力值排行
    char	szName[ACTORNAME_MAXSIZE];			// 玩家名字
    char	szKinName[KIN_NAME_SIZE];			// 家族名字
    DWORD   dwState;                            // 玩家状态
    BYTE    bySex;								// 性别 0男 1女 2不区分

    // 以下是最大段位信息（nGrade）
    BYTE    nMatchType;
    DWORD   nRankScore;
    DWORD   nGrade;


};

//EVENT_KIN_REMOVEMEMBER
struct SEventKin_RemoveMember_OC
{
    DWORD	dwPDBID;							// 玩家ID

};


// 创建家族成功事件
//EVENT_KIN_CREATE_SUCCESS
struct SEventKin_CreateSuccess_OC
{
    bool bSuccess;
};

// =========================================主角家族数据============================================
// 成员信息返回
//EVENT_KIN_UPDATEHEROMEMBER

// 退出家族
//EVENT_KIN_QUIT
struct SEventKin_Quit_C : public event_base
{
	DWORD dwPDBID;										// 玩家ID
};

// =========================================家族数据相关============================================
// 成员加入
//EVENT_KIN_ADDMEMBER
struct SEventKin_AddMember_C
{
	DWORD dwKinID;										// 家族ID
	int	  nKinState;									// 家族状态
	DWORD dwPDBID;										// 玩家ID
	char  szName[ACTORNAME_MAXSIZE];					// 玩家名字
	
	BYTE  nLevel;										// 玩家等级
	BYTE  nIdentity;									// 族内职位
	bool  bIsOnline;									// 玩家是否在线
	int   nLogoutTime;									// 玩家最后下线时间
	BYTE  nVocation;									// 玩家职业
	DWORD dwBattleVal;									// 战力值
	DWORD dwSrcBattleVal;								// 上一次战力值
	DWORD dwBVOrder;									// 战力值排名
	DWORD dwLastOrder;									// 上一次战力值排行
};

struct SEventKin_AddMember_O
{
	DWORD dwKinID;										// 家族ID
	int	  nKinState;									// 家族状态
	DWORD dwPDBID;										// 玩家ID
};

// 删除成员
//EVENT_KIN_DELMEMBER
struct SEventKin_DelMember_C : public event_base
{
	DWORD dwKinID;										// 家族ID
	int	  nKinState;									// 家族状态
	DWORD dwPDBID;										// 玩家ID
	bool  bIsOnline;									// 玩家是否在线
};



// 更新成员
//EVENT_KIN_UPDATEMEMBER
struct SEventKin_UpdateMember_C
{
    DWORD dwUDBID;                                      // 账号ID
	DWORD dwPDBID;										// 玩家ID
	char  szName[ACTORNAME_MAXSIZE];					// 玩家名字
	BYTE  nLevel;										// 玩家等级
	BYTE  nIdentity;									// 族内职位
	bool  bIsOnline;									// 玩家是否在线
	int   nLogoutTime;									// 玩家最后下线时间
	BYTE  nVocation;									// 玩家职业
    BYTE  nFaceID;                                      // 头像ID
	DWORD dwBattleVal;									// 战力值
	DWORD dwSrcBattleVal;								// 上一次战力值
	DWORD dwBVOrder;									// 战力值排名
	DWORD dwLastOrder;									// 上一次战力值排行
    DWORD dwState;                                      // 玩家状态
    BYTE  bySex;                                        // 性别

    // 以下是最大段位信息（nGrade）
    BYTE    nMatchType;
    DWORD   nRankScore;
    DWORD   nGrade;
};

// 家族整型属性改变
//EVENT_KIN_UPDATE_NUMPROP
struct SEventKin_UpdateNumProp_C
{
	int nPropID;
	int nValue;
};

// 家族字符型属性改变
//EVENT_KIN_UPDATE_STRINGPROP
struct SEventKin_UpdateStringProp_C
{
	int nPropID;
	char szValue[KIN_MAX_PROPLEN];
};

// 家族创建
//EVENT_KIN_CREATE
struct SEventKin_Create
{
	DWORD dwKinID;
};

// 家族成为正式家族
//EVENT_KIN_TOFORMAL
struct SEventKin_ToFormal
{
	DWORD dwKinID;
};

// 家族解散
//EVENT_KIN_DISMISS
struct SEventKin_Dismiss : public event_base
{
	DWORD dwKinID;
	DWORD dwShaikhID;
};



// 刷新Boss信息
//EVENT_KIN_UPDATE_BOSSINFO
struct SEventKin_UpdateBossInfo_C
{
	DWORD dwBossID;									// 正在培养的BossID
	short nBossGrowPercent;							// 正在培养的Boss的成长度
	short nSatiation;								// 正在培养的Boss的温饱度
};

// 成员登陆
//EVENT_KIN_LOGIN
struct SEventKin_MemberLogin: public event_base
{
	DWORD dwPDBID;									// 成员角色ID
	DWORD dwKinID;									// 家族ID
};

// 成员下线
//EVENT_KIN_LOGOUT
struct SEventKin_MemberLogout: public event_base
{
	DWORD dwPDBID;									// 成员角色ID
	DWORD dwKinID;									// 家族ID
};

// 家族族长更换事件
//EVENT_KIN_CHANGE_SHAIKH
struct SEventKin_ChangeShaikh : public event_base
{
	DWORD dwKinID;									// 家族ID
	DWORD dwOldShaikh;								// 原来的家族族长
	DWORD dwNewShaikh;								// 新家族族长
};

// 编辑自动接收
//EVENT_KIN_EDITAUTOACCEPT
struct SEventKin_EditAutoAccept
{
    bool  bAutoAccept;								// 是否自动接收成员
	short nAcceptLevel;								// 自动收人等级(2字节)
	DWORD dwAcceptFlag;								// 自动收人设置(4字节)
};

// 请求自动接收设置
// EVENT_KIN_AUTOACCEPTREQUEST
struct SEventKin_AutoAcceptRequest
{
	bool  bAutoAccept;								// 是否自动接收成员
	short nAcceptLevel;								// 自动收人等级(2字节)
	DWORD dwAcceptFlag;								// 自动收人设置(4字节)

	// 留言
};

// 请求自动接收设置
//EVENT_KIN_PERSONJOINEDKIN
struct SEventKin_PersonJoinedKin
{
    bool  bInSuccess;								        // 加入成功

};


// 更新家族军饷信息
//EVENT_KIN_ONSOLDIERPAYUPDATE
struct SEventKin_OnSoldierPayUpdate: public event_base
{
	int nSysTime;
};

// 家族副族长更换事件
//EVENT_KIN_CHANGE_ELDER
struct SEventKin_ChangeElder: public event_base
{
	bool	bAdd;			// ture是添加		
	DWORD	dwPDBID;		// 副族长PDBID
};

#pragma pack()

#endif