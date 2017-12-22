/*******************************************************************
** 文件名:	SpellMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/10/2015
** 版  本:	1.0
** 描  述:	技能消息定义
********************************************************************/

#ifndef __SPELL_MESSAGE_DEF_H__
#define __SPELL_MESSAGE_DEF_H__

#include "NetMessageDef.h"
#include "SpellDef.h"
#include "EffectCommon_ManagedDef.h"
using namespace SPELL;

#pragma pack(1)

/////////////////////////// 技能模块消息  ////////////////////////////
// 消息码定义键值
enum SPELL_NET_MESSAGE
{
	SPELL_MSG_PREPARE = 0,		        // 预施法
	SPELL_MSG_CAST,                     // 技能施法
	SPELL_MSG_HIT,						// 击中消息
	SPELL_MSG_TALENT_CHANGE,			// 天赋数据改变
	SPELL_MSG_BREAK,                    // 技能被打断
	SPELL_MSG_ADD_SPELL,				// 增加技能
	SPELL_MSG_FINISH,					// 技能结束
	SPELL_MSG_SPELLFLAG_UPDATE,			// 技能标志位更新
	SPELL_MSG_CONTEXT_UPDATE,			// 技能现场数据更新
    //SPELL_MSG_DUMMY,                  // 天赋数据更新给客户端
    SPELL_MSG_ACTIVE_TALENT,            // 激活天赋
	SPELL_MSG_FOCO_ENERGIA,				// 蓄力消息
	SPELL_MSG_DAMAGE,					// 伤害消息
	SPELL_MSG_CANCEL_CAST,				// 取消施法
	SPELL_MSG_UPGRADE_SPELL,			// 升级技能
	SPELL_MSG_SPELL_POINT,				// 技能点数
    SPELL_MSG_RANGE_SELECTION,			// 技能范围选择
    SPELL_MSG_CHANNEL,                  // 技能吟唱
    SPELL_MSG_PERSPECTIVE_SHOW,         // 透视显示
    SPELL_MSG_SPELL_OVERLAY,            // 技能累积
    SPELL_MSG_MAP_FLY,                  // 地图飞檐走壁
    SPELL_MSG_PERPARE_FAIL,                // 施法失败(清僵直时间)
	SPELL_MSG_PREPARE_RECALC_TARGETINFO,   // 预施法后重计算目标点

    SPELL_MSG_TARGET_TOO_FAR_INFO,

    SPELL_MSG_KILL_ENTITY,              // 杀死了一个单位
};


// 技能现场对象实现
struct SSpellContextImp
{
	UID			uidOperator;		// 使用技能的操作者
	UID			uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID
	int			nViewID;			// 技能视图ID
	DWORD		dwSerialID;			// 序列号ID
	int			nSlotIndex;			// 槽索引
	SLOT_TYPE	nSlotType;			// 槽位类型	(1、装备 2、技能)
#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
    DWORD       dwViewOptTick;      // 显示层触发时间
    DWORD       dwOptTick;          // 逻辑层收到包时间
#endif

	SSpellContextImp()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 客户端-场景服 施法技能消息
struct SSpellContextImp_CS
{
	UID			uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
    DWORD       dwOPTick;           // 施法时的tick数
    Move3       moveState;          // 施法时的移动状态(包括位置、速度、朝向)
	UID			uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID
	int			nSlotIndex;			// 槽索引
	SLOT_TYPE	nSlotType;			// 槽位类型	(1、装备 2、技能)
#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
    DWORD       dwViewTick;         // 显示层触发时间
    DWORD       dwCmdQueueTick;     // 命令队列收包时间
    DWORD       dwMsgSendTick;      // 消息发送时间
#endif
	SSpellContextImp_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 施法技能消息
struct SSpellContextImp_SC
{
    int         nKey;               // 技能唯一标识
	ulong		uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
    DWORD       dwOPTick;           // 操作时的tick数
    Move3       moveState;          // 角色的移动状态(包括位置、速度、朝向)
	ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
    Vector3     ptStartTile;        // 技能起点位置
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID
#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
    DWORD       dwCViewTick;        // 显示层触发时间
    DWORD       dwCCmdQueueTick;    // 命令队列收包时间
    DWORD       dwCMsgSendTick;     // 客户端消息发送时间
	DWORD		dwGSTickCS;			// 到达Gateway时间ms Client->Gateway
	DWORD		dwGCTickCS;			// 到达GatewayConnector时间ms Gateway->Connector
	DWORD		dwSVTickCS;			// 到达实体时间ms Connector->Player
	DWORD		dwSendTick;			// 给客户端发送消息时间 ms
    DWORD       dwCRecvTick;        // 客户端收到消息时间
#endif

	SSpellContextImp_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 天赋效果改变类型
enum TALENT_EFFECT_CHANGE_TYPE
{
    TALENT_EFFECT_ADD,			        // 天赋效果增加
    TALENT_EFFECT_REMOVE,		        // 天赋效果移除
};

// 场景服-客户端 技能的天赋效果改变
struct SSpellTalentChange_SC
{
    TALENT_EFFECT_CHANGE_TYPE nChangeType;      // 天赋效果改变类型
    int                       nSpellID;         // 技能ID
    int                       nTalentID;        // 天赋ID

	SSpellTalentChange_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-场景服 击中消息
struct SSpellHit_SS
{
    DWORD       dwTargetCount;      // 目标数量
	Vector3		ptTargetTile;		// 击中的位置
    Vector3     ptSpellTargetTile;  // 使用技能时鼠标点击的位置
    UID         uidTarget;          // 暂时只有飞行炸弹有用到--第一个击中的目标
	int			nID;				// 技能ID
	float		fFocoEnergiaPCT;	// 蓄力百分比
	DWORD		dwAttackSerialID;	// 攻击对象序号
	int			nAttackCount;		// 攻击目标数量，第几个目标(0表示首个目标)
    Vector3	    vSelfLoc;	        // 施法时自己位置

	SSpellHit_SS()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 击中消息
struct SSpellHit_SC
{
	ulong		uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID
    DWORD       dwTargetCount;      // 目标数量
    int			nAttackCount;		// 攻击目标数量，第几个目标(0表示首个目标)
	DWORD		dwAttackSerialID;	// 攻击对象序号

	SSpellHit_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 技能被打断
struct SSpellBreak_SC
{
	ulong		uidOperator;		// 使用技能的操作者
	int			nID;				// 技能ID

	SSpellBreak_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-场景服 技能被打断
struct SSpellBreak_SS
{
    UID		    uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
    UID		    uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
    Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
    int			nID;				// 技能ID

    SSpellBreak_SS()
    {
        memset(this, 0, sizeof(*this));
    }
};


// 场景服-客户端 施法失败
struct SSpellPrepareFail_SC
{
    int        nID;

    SSpellPrepareFail_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-客户端 技能增加
struct SSpellAdd_SC
{
	int			nID;				// 技能ID

	SSpellAdd_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 技能结束(类似吟唱技能结束需要让客户端恢复到站立状态)
struct SSpellFinish_SC
{
	ulong		uidOperator;		// 使用技能的操作者
	int			nID;				// 技能ID

	SSpellFinish_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 技能标志位更新
struct SSpellFlagUpdate_SC
{
	int			nID;						// 技能ID
	int			nFlag[SPELL_FLAG_MAX];		// 技能标志位

	SSpellFlagUpdate_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 客户端-场景服(以及场景服广播给客户端) 技能现场数据更新
struct SSpellContextUpdate
{
	int			nID;			// 技能ID
	ulong		uidTarget;		// 目标ID
	Vector3		ptTargetTile;	// 目标点

	SSpellContextUpdate()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 客户端-场景服 施法技能目标数据消息
struct SSpellContextImpTargetInfo_CS
{
    int         nSpellID;
    int         nSpellKey;
    Vector3		ptSelf;		    // 自身位置
    Vector3		ptTarget;		// 目标位置

    SSpellContextImpTargetInfo_CS()
    {
        nSpellKey = 0;
        nSpellKey = 0;
    }
};


// ----------------------------天赋------------------------------------
// 天赋效果类型
enum TalentEffectType
{
    Talent_Effect_AddSpell,                               // 增加技能
    Talent_Effect_InfluenceSpellField,                    // 影响技能字段
    Talent_Effect_InfluenceSpellCondEff,                  // 影响技能条件效果
    Talent_Effect_StartEffects,                           // 直接开启一批效果
    Talent_Effect_ReplaceSpell,                           // 替换技能
    Talent_Effect_AddBuff,                                // 增加BUFF
};

// 天赋效果
struct STalentEffectBase
{
    int     nID;               // 效果ID
    int     nEffectType;       // 效果类型 TalentEffectType

    STalentEffectBase() : nID(0), nEffectType(0)
    {
    }

    ~STalentEffectBase(){
        nID = 0;
    }

};

// 效果:增加技能
struct STalentEffectAddSpell : public STalentEffectBase
{
    int nSpellID;               // 新增技能ID
    int nSlotType;              // 槽位类型
    int nSlotIndex;             // 槽位索引
    int nSlotLevel;             // 槽位等级

    STalentEffectAddSpell() : nSpellID(0),nSlotType(0),nSlotIndex(0),nSlotLevel(0)
    {
    }
};

// 效果:影响技能字段
struct STalentEffectInfluenceSpellField : public STalentEffectBase
{
    int		nSpellID;				// 影响技能ID
    int		nSpellIndex;			// 影响技能字段
    int     nType;
    int		nValue;					// 天赋数值
    char    strValue[256];  

    STalentEffectInfluenceSpellField() : nSpellID(0), nSpellIndex(0), nValue(0)
    {
        memset(strValue, 0, sizeof(strValue));
    }
};

enum TalentEffectCondEffType
{
    TE_CondEff_Type_AddCond,         // 增加技能条件
    TE_CondEff_Type_IgnoreCond,      // 减少技能条件
    TE_CondEff_Type_AddSpellEff,     // 增加技能效果
    TE_CondEff_type_IgnoreSpellEff,  // 减少技能效果
};

// 效果:影响技能条件/影响技能效果
struct STalentEffectInfluenceSpellCondEff : public STalentEffectBase
{
    int		nSpellID;                       // 技能ID
    int     nEventID;                       // 事件ID
    int     nSubType;                       // 子类型 TalentEffectCondEffType
    int     nCondOrEffID;                   // 影响的条件或者技能效果

    STalentEffectInfluenceSpellCondEff() : nSpellID(0), nEventID(0), nSubType(0), nCondOrEffID(0) 
    {

    }
};

// 效果:直接开启效果
struct STalentEffectStartEffects : public STalentEffectBase
{
    std::set<int>   setEffects;             // 效果列表

    STalentEffectStartEffects()
    {
        setEffects.clear();
    }
};

// 效果:替换技能
struct STalentEffectReplaceSpell : public STalentEffectBase
{
    int     nOldSpellID;        // 旧技能ID
    int     nNewSpellID;        // 新技能ID

    STalentEffectReplaceSpell() : nOldSpellID(0), nNewSpellID(0) 
    {
        
    }
};

// 效果:增加Buff
struct STalentEffectAddBuff : public STalentEffectBase
{
    int nBuffID;                // BuffID
    int nBuffLevel;             // Buff等级

    STalentEffectAddBuff() : nBuffID(0),nBuffLevel(0)
    {
    }
};

// 激活类型
enum TalentActiveMode
{
    Talent_Active_Auto,         // 自动激活
    Talent_Active_Manual,       // 手动激活
};

// 天赋
struct STalent
{
    int nID;                                  // 天赋ID
    char szName[GAME_NAME_MAXSIZE];           // 天赋名字
    std::vector<int> vecMatchType;            // 比赛类型容器
    std::vector<int> vecMapID;                // 地图ID容器
    int nIconID;                              // 天赋图标ID
    int nVocation;                            // 职业
    int nActiveMode;                          // 激活方式  TalentActiveMode
    int nCondLevel;                           // 激活该天赋所需等级
    int nCondPreTalent;                       // 激活该天赋所需前置天赋
    int nCostPoint;                           // 激活该天赋需要消耗的点数
    std::vector<int> vecEffect;               // 天赋的效果列表

    char szDesc[GAME_NAME_MAXSIZE];           // 天赋描述，策划反馈现在不用，只为策划参考用途

    STalent()     
    {
		nID=nIconID=nVocation=nActiveMode=nActiveMode=nCondLevel=nCondLevel=nCondPreTalent=nCostPoint = 0;
		memset(szName, 0, sizeof(szName));
		memset(szDesc, 0, sizeof(szDesc));
        vecMatchType.clear();
        vecMapID.clear();
        vecMapID.clear();
    }
};

// 客户端请求激活天赋
struct SActiveTalent_CS
{
    int        nTalentID;       // 请求激活天赋ID

	SActiveTalent_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 效果-击退
struct SEfffectBeatBack_SC
{
	UID			uidOperator;		// 操作者
	UID			uidTarget;			// 目标
	Vector3		ptOperatorStartTile;		// 操作者起点
	Vector3		ptOperatorTile;		// 操作者点
	Vector3		ptTargetStartTile;			// 目标者起点
	Vector3		ptTargetTile;		// 目标点
	float		fSpeed;				// 击退速度
	float		fDistance;			// 击退距离
	float		fLocationDistance;	// 离目标点距离
	float		fHeight;			// 击退高度
	MOVE_TYPE	nMoveType;		    // 移动类型
	int			nLockTarget;		// 1表示锁定目标
	int			nLockTime;			// 锁定目标时间,超过时间打断
    int         nPassive;           // 被动位移标识(0主动位移,不为0被动位移)
    float       fMaxTime;           // 最大时间

	SEfffectBeatBack_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 客户端-场景服 技能蓄力
struct SSpellFocoEnergia_CS
{
	int			nID;			// 技能ID

	SSpellFocoEnergia_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 客户端-场景服 技能取消施法
struct SSpellCancelCast_CS
{
	int			nID;			// 技能ID

	SSpellCancelCast_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 显示提示文字
struct SShowTipsText_SC
{
	int			nType;			// 类型
	char		strText[TIPS_TEXT_LEN];		// 提示文字

	SShowTipsText_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 客户端-场景服 升级技能
struct SUpgradeSpell_CS
{
	int			nSlotIndex;		// 槽位置

	SUpgradeSpell_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 技能点数
struct SSpellPoint_SC
{
	int			nSpellPoint;	// 技能点数
	int			nSlotLevel[SPELL_UPGRADE_SLOT_COUNT];		// 槽等级

	SSpellPoint_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 技能范围选择数量
#define SPELL_RANGE_SELECTION_COUNT 32

// 范围选择
struct SSpellRangeSelection
{
	int			nID;									// 技能ID
    UID         uidOperator;                            // 操作者
    Vector3     ptTargetTile;                           // 目标点
    int			nArrayCount;	                        // 选择人数数量
    UID			uidArray[SPELL_RANGE_SELECTION_COUNT];  // 选择人数数组
    Vector3	    vSelfLoc;	                            // 施法时自己位置

    SSpellRangeSelection()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 场景服-场景服 技能结束
struct SSpellFinish_SS
{
    ulong		uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
    ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
    Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
    int			nID;				// 技能ID

    SSpellFinish_SS()
    {
        memset(this, 0, sizeof(*this));
    }
};


struct STargetInfo
{
    UID         uidTarget;          // 目标UID
    Vector3     ptTargetTile;       // 目标点

    STargetInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SPerspectiveShow_SC
{
    bool        bShow;              // 是否显示
    int         nCount;             // 显示数量

    SPerspectiveShow_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SSpellOverlay_SC
{
    int				nID;		            // 技能ID
    SLOT_TYPE       nSlotType;              // 槽位类型
    int             nSlotIndex;             // 槽位索引
    int             nFlagIndex;             // 标志位索引
    int				nFlag;		            // 标志值
    int				nType;		            // 转圈类型(0不转圈，1转圈)
    int				nLeftTime;	            // 转圈剩余时间
    int				nTotalTime;	            // 转圈总时间
    int				nFreezeLeftTime;		// 冷却剩余时间

    SSpellOverlay_SC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 新目标点
struct SNewTargetInfo
{
    int			nID;				// 技能ID
    int			nSlotIndex;			// 槽索引
    SLOT_TYPE	nSlotType;			// 槽位类型	(1、装备 2、技能)

    SNewTargetInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 地图飞檐走壁
struct SSpellMapFly
{
    Vector3		vTileA;		    // 飞檐走壁点A
    Vector3		vTileB;		    // 飞檐走壁点B
    Vector3		vTileC;		    // 飞檐走壁点C
    Vector3		vTileD;		    // 飞檐走壁点D
    float       fWalkHeight;    // 飞檐走壁高度
    Vector3		vAngle;		    // 飞檐走壁角度

    SSpellMapFly()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SSpellTraceLoc
{
    int nSpellID;
    float fSpellDistance;
    float fServerDistance;
    UID uidTarget;
    Vector3 locTarget;
    Vector3 locSelf;

    SSpellTraceLoc()
    {
        memset(this, 0, sizeof(*this));
    }
};

//-----------------------------技能相关---------------------------------
#pragma pack()
#endif	// __SPELL_MESSAGE_DEF_H__