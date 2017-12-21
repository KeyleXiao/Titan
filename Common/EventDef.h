/*******************************************************************
** 文件名:	EventDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/6/2014
** 版  本:	1.0
** 描  述:	

			游戏事件定义
********************************************************************/


#pragma once

#include "EventRefBase.h"
#include "Event_ManagedDef.h"
#include "Slot_ManagedDef.h"
#include "SpellDef_ManagedDef.h"
#include "ActorDef.h"
#include "PlayerSignal_ManagedDef.h"
#include "War_ManagedDef.h"

#ifndef MAKE_EVENT_KEY
#define MAKE_EVENT_KEY(t, e)    ( ((t) << 16) | (e) )
#endif

#pragma pack(push, 1)

// ---------------------------------------------------------------------------------------------
// 01.事件: 实体创建						EVENT_ENTITY_CREATE
struct event_entity_create : public event_base
{
	UID		uid;
	int		flag;						// 创建类型: 参考SERIALIZE_FLAG
	DWORD	dwData;						// 人物：ClientID, 如果是怪物则是保存与其关联的动态障碍信息
	int		nData;						// 人物：SERVICEID
    UID     uidCreator;                  // 创建者uid
	event_entity_create() { memset(this, 0, sizeof(*this)); }
};

//  角色重进场景 EVENT_ACTOR_REENTER_SCENE
struct event_actor_reenter_scene : public event_base
{
    UID		uid;

    event_actor_reenter_scene() { memset(this, 0, sizeof(*this)); }
};

// 02.事件: 实体销毁						EVENT_ENTITY_DESTROY
struct event_entity_destroy : public event_base
{
	UID		uid;    
	DWORD	dwData;						// 人物：ClientID,如果是怪物则是保存与其关联的动态障碍信息
	bool    bBroadAOI;                  // 是否需要AOI广播

	event_entity_destroy() { memset(this, 0, sizeof(*this)); }
};

// 事件: 移动同步（广播给客户端）  EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE
struct event_entity_move_behaviour_change  : public event_base
{
    UID		 uid;
    Move3    move;
    float    speed;
	bool     bNotifySelf;    // 是客户端发过来的不需要发回去
	bool     bNeedBroadcast;
    int      nBehaviour;
    int      nCreatureState;

    event_entity_move_behaviour_change() { uid = 0; bNotifySelf=false; }
};

// 事件： 移动事件  EVENT_ENTITY_MOVE
struct event_entity_move : public event_base
{
    UID  uid;
    Vector3 oldLoc;
    Vector3 newLoc;
    int curState; // EMoveType
    int nShiftPriority;
    bool bShiftPassive;
    int nVisibleFlag;

    event_entity_move(){ uid = 0; }
};

// 事件: 实体传送						EVENT_ENTITY_TRANSPORT
struct event_entity_transport : public event_base
{
    int		oldScene;
    Vector3	oldLoc;			// 角色老位置
    int		newScene;
    Move3	newMove;	    // 角色新位置
    int     nReason;        // 传送原因  TRANSPORT_REASON

    event_entity_transport() { oldScene = newScene = 0;  }
};

// 事件: 进入视野						EVENT_ENTITY_ENTER_VIEWPORT
struct event_entity_enter_viewport : public event_base
{
	UID   uidMe;
	UID   uidEnter;

	event_entity_enter_viewport() { memset(this, 0, sizeof(*this)); }
};

// 事件: 离开视野						EVENT_ENTITY_LEAVE_VIEWPORT
struct event_entity_leave_viewport : public event_base
{
	UID   uidMe;
	UID   uidLeave;

	event_entity_leave_viewport() { memset(this, 0, sizeof(*this)); }
};

// 事件： 撞墙           EVENT_ENTITY_HIT_WALL
struct event_entity_hit_wall : public event_base
{
    event_entity_hit_wall() { memset(this, 0, sizeof(*this)); }
};


// ---------------------------------------------------------------------------------------------
// 开始冷却,客户端用来转圈的				EVENT_FREEZE_START
struct event_freeze_start_c : public event_base
{
	DWORD			dwClassID;		// 冷却类
	DWORD			dwFreezeID;		// 冷却ID
	DWORD			dwFreezeTime;	// 冷却时长
	bool			bIsBreak;		// 是否是打断冷却

	// 界面那边需要知道哪个行为触发这次冷却，他需要做某个行为
	bool			bTrigger;		// 是否本冷却触发

	event_freeze_start_c() { memset(this, 0, sizeof(*this)); }
};


// ---------------------------------------------------------------------------------------------
// 06.事件: 实体属性变化
// ---------------------------------------------------------------------------------------------
// EVENT_ENTITY_PROPERTY_CHANGE	(EVENT_OFFSET+6)
struct event_entity_property_change : public event_base
{
	int		nPropertyID;		// 属性ID
    char    data[128];
    int     nLen;

	event_entity_property_change() { memset(this, 0, sizeof(*this)); }
};

// 实体属性广播 EVENT_ENTITY_PROPERTY_BROADCAST
struct event_entity_properties : public event_base
{
    bool    isBroadcast;        // 广播状态
    int     nPropCount;         // 要广播的属性个数
    // nPropertyCount * sizeof(int)

    event_entity_properties() { memset(this, 0, sizeof(*this)); }
};

// 立即刷新属性到客户端 EVENT_FLUSH_PROPERTY_2_ClIENT_IMMEDIATELY
struct event_flush_property_2_client_immediately
{
    int nPropertyID; 
};

// 实体状态发生变化						EVENT_ENTITY_CHANGE_STATE
struct event_entity_change_state : public event_base
{
	UID		uid;				// 实体uid
    int     nOldState;
	int		nState;				// 状态 ENTITY_STATE

	event_entity_change_state() { memset(this, 0, sizeof(*this)); }
};

// 实体飞行状态改变  EVENT_ENTITY_FLY_STATE_CHANGE
struct event_entity_fly_state_change : public event_base
{
    UID     uid;
    bool    bFly;

    event_entity_fly_state_change(){ memset(this, 0, sizeof(*this)); }
};


// 实体增加buff							EVENT_ENTITY_ADDBUFF
struct event_entity_add_buff : public event_base
{
	DWORD			dwBuffID;		// ID
	DWORD			dwBuffLevel;	// 等级
	UID				uidEffect;		// 本buff的作用对像
	UID				uidAdd;			// 本buff的添加者
	DWORD			dwBuffFlag;		// buff标志
	int				nErrorID;		// 不能添加BUFF的原因

	event_entity_add_buff() { memset(this, 0, sizeof(*this)); }
};


// 实体移除buff							EVENT_ENTITY_REMOVEBUFF
struct event_entity_remove_buff : public event_base
{
	DWORD			dwBuffID;		// buff id
	DWORD			dwBuffLevel;	// buff等级
	UID				uidEffect;		// 本buff的作用对像
	UID				uidAdd;			// 本buff的添加者
	DWORD			dwBuffFlag;		// buff标志
	UID				uidRemove;		// 移除者

	event_entity_remove_buff() { memset(this, 0, sizeof(*this)); }
};

// buff客户端类型
enum BUFF_CLIENT_TYPE
{
	BUFF_CLIENT_TYPE_ADD = 1,		// 增加Buff
	BUFF_CLIENT_TYPE_REMOVE,		// 移除Buff
};

// 客户端
struct event_entity_buff_client : public event_base
{
	int				nType;			// buff操作类型
	DWORD			dwIndex;		// buff索引
	int				nIconID;		// buff图标ID
	DWORD			dwTime;			// buff剩余时间
	char			szName[32];		// buff名字
	char			szTips[256];	// buff描述
	int             nLevel;         // buff等级
	bool            nIsShow;        // buff显示
	event_entity_buff_client() { memset(this, 0, sizeof(*this)); }
};


// 攻击别人						EVENT_ENTITY_ATTACK
struct event_entity_attack : public event_base
{
	ulong		uidOperator;		// 使用技能的操作者
	ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID
    int         nSlotIndex;         // 槽位
	int			nAttackResultType;	// 攻击结果类型(EAttackResultType)
	int			nSpellEffectType;	// 技能效果类型(SPELL_EFFECT_TYPE)
    int			nSpellFlag;			// 技能标识字段
    float       fPromoteFactor;     // 提升系数

	event_entity_attack()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 攻击别人                 EVENT_ENTITY_ATTACK_EX
struct event_entity_attack_ex  : public event_base
{
	ulong		uidOperator;		// 使用技能的操作者
	ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	int         nSpellID;			// 技能ID
	int         nSpellEffectType;	// 技能效果类型
	int         nDamageType;		// 伤害类型
	DWORD       dwAttackSerialID;	// 攻击序列号
	int		    nAttackCount;		// 攻击目标数量，第几个目标(0表示首个目标)
    float	    fFocoEnergiaPCT;	// 蓄力百分比
    int		    nCRD;				// 暴击伤害

	event_entity_attack_ex() { memset(this, 0, sizeof(*this)); }
};


// 受到伤害						EVENT_ENTITY_DAMAGE
struct event_entity_damage : public event_base
{
	int			nID;				// 技能ID
	ulong		uidOperator;		// 使用技能的操作者
    ulong       uidRealOperator;    // 真实的操作者（因为uidOperator字段可能被改动过，比如召唤物被改为了主人）
	ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	int			nDamageHP;			// 伤害血量
	int			nDamageType;		// 伤害类型
	int			nSpellEffectType;	// 技能效果类型
	bool		bDead;				// 是否死亡
	bool		bFakeDead;			// 是否假死
    DeadEffectType nDeadType;
	int			nInitialDamageHP;	// 初始伤害,未经过效果改变的伤害值
    int         nAttackResultType;  // 攻击结果
    bool		bRebound;			// 是否反弹伤害

	event_entity_damage() { memset(this, 0, sizeof(*this)); }
};

//EVENT_ENTITY_WAR_REC_DAMAGE                战场实体需要记录的伤害
typedef event_entity_damage event_entity_warrec_damage;

// 死亡事件						EVENT_ENTITY_DIE
struct event_entity_die : public event_base
{
    int         nSpellID;           // 技能ID
	UID			uidOperator;		// 操作者
	UID			uidTarget;			// 目标者(死亡者)
	int			nDeadType;			// 死亡类型
	Vector3		vTargetEndLoc;		// 目标结束点
	int			nSpellEffectType;	// 技能效果类型
    Vector3     vTurnTargetTile;    // 转向目标点

	event_entity_die()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 发送人物载具事件到客户端信息数据
struct event_tankpart_action : public event_base
{
	LONGLONG uidTank;		// 载具UID
	BYTE	nEventType;		// 载具事件类型 参考 enum EMTankPartEventType
	UID		uidSpare;		// 备用数字
	// void * pData;		// 符加数据

	event_tankpart_action() { memset(this, 0, sizeof(*this)); }
};


// 施法技能					EVENT_ENTITY_CAST_SPELL
struct event_cast_spell : public event_base
{
	int			nID;				// 技能ID
	UID			uidOperator;		// 操作者
	UID         uidTarget;			// 目标
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nSpellEffectType;	// 技能效果类型
	int			nCoolID;			// 冷却ID
	int		    nSlotIndex;			// 槽索引
	int         nSlotType;			// 槽位类型	(1、装备 2、技能)
	int			nSpellFlag;			// 技能标识字段
	int			nCastType;			// 技能使用类型
	int			nFailResult;		// 技能释放失败结果
    int         nDamageType;        // 伤害类型

	event_cast_spell(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 回复技能属性查询 EVENT_ENTITY_RESPONSE_SPELL_ATTR_QUERY
struct event_response_spell_attr_query : public event_base
{
    int     nID;

    event_response_spell_attr_query(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


// 场景创建成功					EVENT_SCENE_CREATED
struct event_scene_created : public event_base
{
	int			nMapID;			// 地图ID
	int			nSceneID;		// 场景ID

	event_scene_created() { memset(this, 0, sizeof(*this)); }
};
// 场景销毁					EVENT_SCENE_DESTROY
struct event_scene_destroy : public event_base
{
	int			nMapID;			// 地图ID
	int			nSceneID;		// 场景ID

	event_scene_destroy() { memset(this, 0, sizeof(*this)); }
};

// 场景创建实体					EVENT_SCENE_ENTITY_CREATE
struct event_scene_entity_create : public event_base
{
	UID			uid;			// 生物UID
	BYTE		byType;			// 生物类型
	int			nSceneID;		// 场景ID
	int			nData;			// 附加数据，人物时为ClientID

	event_scene_entity_create() { memset(this, 0, sizeof(*this)); }
};

// 场景移除实体					EVENT_SCENE_ENTITY_REMOVE
struct event_scene_entity_remove : public event_base
{
	UID			uid;			// 生物UID
	BYTE		byType;			// 生物类型
	int			nSceneID;		// 场景ID

	event_scene_entity_remove() { memset(this, 0, sizeof(*this)); }
};

// 实体播放动作					EVENT_ENTITY_PLAY_ANIMATION
struct event_entity_play_animation : public event_base
{
	UID			uid;			// 生物UID
	char		szName[ANIMATION_NAME_LEN];		// 动作名字

	event_entity_play_animation() { memset(this, 0, sizeof(*this)); }
};

// ---------------------------------------------------------------------------------------------
// 06.事件: 等级改变
// ---------------------------------------------------------------------------------------------
// EVENT_ENTITY_UPGRADE_LEVEL
struct event_entity_upgrade_level : public event_base
{
	UID		uChangedUID;		// 人物UID
	int		nLevel;

	event_entity_upgrade_level() { memset(this, 0, sizeof(*this)); }
};

// 玩家角色等级改变(数据库)       EVENT_ENTITY_ACTOR_CHANGE_LEVEL
struct event_actor_change_level : public event_base
{
    UID		uChangedUID;		// 角色UID
    int		nLevel;

    event_actor_change_level() { memset(this, 0, sizeof(*this)); }
};

// 技能改变事件       EVENT_ENTITY_SPELL_CHANGE
struct event_spell_change : public event_base
{
	int     nSpellID;           // 技能ID
	bool    bAdd;               // 是否增加(true-增加 false-移除)
    bool    bIsMaster;          // 是否主角(true.主角,false.非主角）

	event_spell_change() { memset(this, 0, sizeof(*this)); }
};

// 取消施法					EVENT_ENTITY_CANCEL_CAST
struct event_cancel_cast : public event_base
{
	int			nID;				// 技能ID

	event_cancel_cast() { memset(this, 0, sizeof(*this)); }
};

// 击杀增加天赋点数					EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT
struct event_entity_kill_add_talent_point : public event_base
{
	UID			uidOperator;		// 操作者(凶手)
	UID         uidTarget;			// 目标(死亡者)
	int			nValue;				// 增加值

	event_entity_kill_add_talent_point() { memset(this, 0, sizeof(*this)); }
};

// 天赋点数更改               EVENT_ENTITY_TALENT_CHANGE
struct event_entity_talent_change:public event_base
{
    UID         uId;                // 玩家uid
    int         nTalentValue;       // 改变后的天赋点
    int         nChangePoint;       // 改变的天赋点
    BYTE        byAddType;          // 1、系统 2、普通
    BYTE        byChangeType;       // 改变的类型 ENUM_TALENT_CHANGE_TYPE 
    event_entity_talent_change(){memset(this, 0, sizeof(*this));}
};

// 槽位索引更新                       EVENT_SLOTINDEX_UPDATE
struct event_slotindex_update : public event_base
{
	bool		bAdd;				// 增加,移除
	int			nSlotIndex;			// 槽位索引
	int			nSlotLevel;			// 槽位等级
	SLOT_TYPE	nSlotType;			// 槽位类型 
	int			nID;				// 技能ID,物品ID

	event_slotindex_update() { memset(this, 0, sizeof(*this)); }
};

// 皮肤改变							EVENT_ENTITY_SKIN_CHANGED
struct event_entity_skin_changed : public event_base
{
	UID		uChangedUID;		// 人物UID

	event_entity_skin_changed() { memset(this, 0, sizeof(*this)); }
};


// 某台服务器信息更新	EVENT_SYSTEM_SERVERINFOUPDATED
struct event_systemserverinfo_updated_s : public event_base
{
	DWORD			ServerID;		// 服务器ID	CSID类型
	int				nState;			// 服务器状态	SERVER_STATE类型
	LPCSTR			pInfo;			// 服务器数据	ServerData类型
};

// 默认区变化了,新服务器IP,端口可从config.ini中取 EVENT_SYSTEM_DEFAULTSERVER_CHANGED
struct  event_system_default_server_changed : public event_base
{
    // 新区服务器信息
    int nID;			// 服务器索引ID
    int nGroup;			// 区组序号
    int nDistrict;		// 版本分类序号
    int nServer;		// 游戏世界区序号
    // 原区服务器信息
    int nID0;			// 服务器索引ID
    int nGroup0;		// 区组序号
    int nDistrict0;		// 版本分类序号
    int nServer0;		// 游戏世界区序号
    bool bIsVerChange;	// 换区是否变化版本了,有就要退出游戏并更新
};

// 学习技能						EVENT_ENTITY_LEARN_SPELL
struct event_learn_spell : public event_base
{
	UID			uidOperator;		// 操作者
	int			nID;				// 技能ID
	int			nCoolID;			// 冷却ID
    int         nSlotIndex;
	SLOT_TYPE   nSlotType;			// 槽位类型	(1、装备 2、技能)
	int			nSpellFlag;			// 技能标识字段

	event_learn_spell(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 冷却改变						EVENT_ENTITY_FREEZE_CHANGED
struct event_freeze_changed : public event_base
{
	DWORD			dwClassID;			// 技能ID
	DWORD			dwFreezeID;			// 冷却ID
	UID				PersonUID;				// 人物UID

	event_freeze_changed(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 准备改变属性						EVENT_ENTITY_PREPARE_CHANGE_PROPERTY
struct event_prepare_change_property : public event_base
{
	int			nID;				// 改变属性ID
	DWORD		dwFlag;				// 技能标志位
	int			nValue;				// 改变属性值

	event_prepare_change_property(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 助攻增加事件						EVENT_GAME_ACTOR_ASSIST_CHANGE
struct event_assist_add:public event_base
{
	UID			uUid;				// 人物id
	UID			uKillId;			// 击杀这id
	UID			uDeadId;			// 死亡者id
	int			nAssistCount;		// 助攻次数
	event_assist_add(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};

// 死亡归属事件						EVENT_GAME_ACTOR_KILL_BELONGS
struct event_kill_belongs:public event_base
{
    UID			uMurderId;			// 击杀者id
    UID			uDeadId;			// 死亡者id

    event_kill_belongs(void)
    {
        memset( this, 0, sizeof(*this) );
    }
};

// 助攻增加事件						EVENT_GAME_ACTOR_KILL_MONSTER
struct event_kill_monster:public event_base
{
	UID			uMurderUid;			// 人物id
	UID			uDeadId;			// 击杀这id
	int			nEXP;				// 增加经验值
	int			nTalentValue;		// 增加的天赋点
	event_kill_monster(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};

// 击退事件						EVENT_ENTITY_BEAT_BACK
struct event_beat_back:public event_base
{
	UID			uidOperator;		// 操作者UID
	UID			uidTarget;			// 目标UID

	event_beat_back(void)
	{
		memset( this, 0, sizeof(*this) );
	}
};

// 向服务器发送施法技能事件						EVENT_ENTITY_SEND_CAST_SPELL
struct event_entity_send_cast_spell : public event_base
{
    int			nID;				// 技能ID

    event_entity_send_cast_spell() { memset(this, 0, sizeof(*this)); }
};

// 改变翅膀事件					EVENT_ENTITY_CHANGE_WING
struct event_entity_change_wing : public event_base
{
    int			nOldWingID;				// 旧翅膀ID
    int			nNewWingID;				// 新翅膀ID

    event_entity_change_wing() { memset(this, 0, sizeof(*this)); }
};

// 角色登进场景服   EVENT_GAME_ACTOR_LOGIN
struct event_actor_login : public event_base
{
    unsigned long   client;
    PDBID           dwDBID;
    UID             uid;
    int             nSceneID; // 登进的场景ID
    ELoginMode      eMode;  

    event_actor_login() { memset(this, 0, sizeof(*this)); } 
};

struct event_entity_continue_play : public event_base
{

    PDBID           dwDBID;
    ClientID        clientID;
    event_entity_continue_play() { memset(this, 0, sizeof(*this)); } 
};


// 角色登出场景服  EVENT_GAME_ACTOR_LOGOUT
struct event_actor_logout : public event_base
{
    unsigned long   client;
    PDBID           dwDBID;
    UID             uid;
    int             nSceneID;  // 登出时所在的场景ID
    ELogoutMode     eMode;

    event_actor_logout() { memset(this, 0, sizeof(*this)); } 
};

// 切换游戏状态   EVENT_GAME_ACTOR_CHANGE_GAMESTATE
struct event_actor_change_gamestate : public event_base
{
    UID             uid;
    PDBID           dwDBID;
    DWORD           dwState;

    event_actor_change_gamestate() { memset(this, 0, sizeof(*this)); }
};

// 完成读取任务数据    EVENT_ACTOR_READTASKDATA,EVENT_GAME_ACTOR_READTASKDATA     
struct event_actor_read_taskdata_s : public event_base
{
    DWORD			dwHolder;		// 拥有者

    event_actor_read_taskdata_s() { memset(this, 0, sizeof(*this)); }
};

// 草丛事件事件					EVENT_ENTITY_FOREST
struct event_entity_forest : public event_base
{
    int			nType;				// 0离开草丛,1进入草丛

    event_entity_forest() { memset(this, 0, sizeof(*this)); }
};

// 升级技能					EVENT_UPDATE_SPELL
struct event_entity_update_spell : public event_base
{
    int			nSlotIndex;			// 槽位索引
    int			nSlotLevel;			// 槽位等级
    SLOT_TYPE	nSlotType;			// 槽位类型
    int         nSpellID;           // 技能ID

    event_entity_update_spell() { memset(this, 0, sizeof(*this)); }
};

// 移除技能   EVENT_REMOVE_SPELL
struct event_entity_remove_spell : public event_base
{
    int nSpellID;

    event_entity_remove_spell(){ memset(this, 0, sizeof(*this)); }
};

// 购买物品 EVENT_BUY_ITEM
struct event_buy_item : public event_base
{
    int nItemID;
    int nItemType;

    event_buy_item(){ memset(this, 0, sizeof(*this)); }
};

// 物品槽里的物品改变事件 EVENT_ITEM_IN_SLOT_CHANGE
struct event_item_in_slot_change : public event_base
{
    int nSlotIndex;
    int nItemID;
    int nItemCount;
    
    event_item_in_slot_change(){ memset(this, 0, sizeof(*this)); }
};

// 使用物品事件 EVENT_USE_ITEM
struct event_use_item : public event_base
{
    int nItemID;

    event_use_item(){ memset(this, 0, sizeof(*this)); }
};

// 执行属性同步              EVENT_GAME_ACTOR_SYNC_PROPERTY
struct event_entity_sync_share_property : public event_base
{
    UID     uidMaster;          // 所属者UID
    DWORD   dwDBID;             // dbid
    int		nPropertyID;		// 属性ID
    int     nValue;             // 属性值

    event_entity_sync_share_property() { memset(this, 0, sizeof(*this)); }
};

// EVENT_GOT_STUCK
struct event_got_stuck : public event_base
{
    int nDummy;

    event_got_stuck() { memset(this, 0, sizeof(*this)); }
};

// EVENT_ENTITY_CHANGE_OPERATION
struct event_entity_change_operation : public event_base
{
    int nType;                  // 操作类型

    event_entity_change_operation() { memset(this, 0, sizeof(*this)); }
};

//EVENT_ENTITY_CHANGE_HERO
struct event_entity_change_hero : public event_base
{
    int nOldHeroID;             // 当前英雄
    int nNewHeroID;             // 新英雄

    event_entity_change_hero() { memset(this, 0, sizeof(*this)); }
};

// EVENT_CLAN_CREATE
struct event_clan_create : public event_base
{
	DWORD dwClanID;

	event_clan_create()
	{
		memset(this, 0, sizeof(*this));
	}
};

//EVENT_CLAN_UPDATE_ORDERINFO
struct event_clan_update_orderinfo : public event_base
{
    DWORD   dwClanID;			                        // 战队ID
    char    szClanName[KIN_NAME_SIZE];			        // 战队名称
    DWORD   dwTotalFightCapacity;                       // 战力总和
    DWORD   dwShaikhID;							        // 族长ID
    char    szShaikhName[ACTORNAME_MAXSIZE];	        // 族长名字
    DWORD   dwPlayerCount;                              // 帮会人数

    event_clan_update_orderinfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

//EVENT_KIN_READ_DB_FINISHED
struct event_kin_read_finished_head : public event_base
{
    DWORD   nCount;			                        // 战队ID
    
    //nCount*sizeof(int)
    event_kin_read_finished_head()
    {
        memset(this, 0, sizeof(*this));
    }
};

// EVENT_CLAN_DISMISS
struct event_clan_dismiss : public event_base
{
	DWORD dwClanID;

	event_clan_dismiss()
	{
		memset(this, 0, sizeof(*this));
	}
};

// EVENT_CLAN_MEMBERLOGIN
struct event_clan_memberlogin : public event_base
{
	DWORD dwPDBID;									// 成员角色ID
	DWORD dwClanID;									// 帮会ID

	event_clan_memberlogin()
	{
		memset(this, 0, sizeof(*this));
	}
};

// EVENT_CLAN_SHAIKH_CHANGE
struct event_clan_shaikh_change : public event_base
{
	DWORD dwClanID;									// 帮会ID
	DWORD dwCountryID;								// 帮会国家
	DWORD dwOldShaikh;								// 旧帮主
	DWORD dwNewShaikh;								// 新帮主
	char szNewName[ACTORNAME_MAXSIZE];				// 新帮主名字

	event_clan_shaikh_change()
	{
		memset(this, 0, sizeof(*this));
	}
};

// EVENT_CLAN_UPDATEID
struct event_clan_update_id : public event_base
{
	DWORD dwKinID;                                      // 战队ID
	DWORD dwClanID;                                     // 帮会ID

	event_clan_update_id()
	{
		memset(this, 0, sizeof(*this));
	}
};

// EVENT_GAME_ACTOR_BUY_HERO
struct event_entity_buy_hero : public event_base
{
    PDBID   idActor;        // 玩家pdbid
    UID     uID;            // 玩家uid
    int     nHeroID;        // 英雄id
    BYTE    nHeroStar;      // 英雄星级
    DWORD   dwFlags;        // 英雄标识HERO_FLAG

    event_entity_buy_hero() { memset(this, 0, sizeof(*this)); }
};

// EVENT_PLAYER_SIGNAL_RECVD,                  
struct event_player_signal_recvd : public event_base
{
    UID uidOperator;                     // 信号发起者
    UID uidMarkTarget;                   // 标记的目标
    Vector3 ptSignLoc;                   // 信号地点
    GameLogic::EMinimapClickType eSignal;     // 信号类型

    event_player_signal_recvd() { memset(this, 0, sizeof(*this)); }
};

// EVENT_GAME_ACTOR_UPDATE_MATCHRANK
struct event_entity_update_matchrank : public event_base
{
    PDBID   idActor;      // 玩家pdbid
    BYTE    nMatchType;   
    DWORD   nRankScore;
	DWORD	dwMatchNum;
	DWORD	dwWinNum;
    DWORD   nHideRankScore;

    event_entity_update_matchrank() { memset(this, 0, sizeof(*this)); }
};

// 更新战队ID事件
//EVENT_KIN_UPDATE_ORDERINFO
// 消息体
struct event_kin_update_orderinfo: public event_base
{
    DWORD   dwKinID;			                        // 战队ID
    char    szKinName[KIN_NAME_SIZE];			        // 战队名称
    DWORD   dwTotalFightCapacity;                       // 战力总和
    DWORD   dwShaikhID;							        // 族长ID
    char    szShaikhName[ACTORNAME_MAXSIZE];	        // 族长名字
    DWORD   dwPlayerCount;                              // 战队人数
};

// 准备施法技能					EVENT_ENTITY_PREPARE_CAST_SPELL
struct event_prepare_cast_spell : public event_base
{
    int			nID;				// 技能ID
    UID			uidOperator;		// 操作者
    UID         uidTarget;			// 目标
    Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
    int			nFailResult;		// 技能释放失败结果
    int			nSlotIndex;			// 槽索引
    SLOT_TYPE	nSlotType;			// 槽位类型	(1、装备 2、技能)
    int         nEffecType;         // 技能效果类型 SPELL_EFFECT_TYPE
    int         nAttackType;        // 攻击类型 ATTACK_TYPE

    event_prepare_cast_spell(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 技能准备事件
struct event_spell_prepare : public event_base
{
    int nID;
    int nCastType;

    event_spell_prepare()
    {
        memset(this, 0, sizeof(*this));
    }
};

//EVENT_ENTITY_WAR_GOOD_LIST
struct event_entity_war_good_list : public event_base
{
    PDBID			ActorID;				                    // 玩家PDBID
    int             nCamp;                                      // 阵营
    int             nGoodList[MAX_GOODS_SLOT_COUNT];	        // 物品列表

    event_entity_war_good_list(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 键盘事件						EVENT_ENTITY_INPUT_MANAGER
struct event_input_manager : public event_base
{
    int             nKeyCode;	            //键码EViewKeyCode
    int             nKeyState;	            //按键状态EViewKeyState

    event_input_manager() { memset(this, 0, sizeof(*this)); }
};

//EVENT_ENTITY_CAN_BUY_GOOD
struct event_entity_can_buy_good : public event_base
{
    int nGoodID;
    BYTE ErrorCode;	            // 键码EWarCheckBuyGoodErrorCode

    event_entity_can_buy_good() { memset(this, 0, sizeof(*this)); }
};

// 清除连杀					EVENT_ENTITY_CLEAR_MULTI_KILL
struct event_entity_clear_multi_kill : public event_base
{
    UID			uidOperator;				// 操作者
    UID           uidTarget;                  // 目标

    event_entity_clear_multi_kill() { memset(this, 0, sizeof(*this)); }
};

struct event_game_thrillingcure : public event_base
{
    UID			uidOperator;				// 操作者
    UID         uidTarget;                  // 目标
    DWORD       dwTime;                     // 惊险救治时间

    event_game_thrillingcure() { memset(this, 0, sizeof(*this)); }
};

struct event_game_killingcontrol : public event_base
{
    UID			uidOperator;				// 操作者
    UID         uidTarget;                  // 目标

    event_game_killingcontrol() { memset(this, 0, sizeof(*this)); }
};

struct event_game_groupcontrol : public event_base
{
    UID			uidGroupControl;				// 操作者
	int			nGroupControlNum;				// 团控人数
    event_game_groupcontrol() { memset(this, 0, sizeof(*this)); }
};

// 玩家角色名字改变       EVENT_ENTITY_ACTOR_CHANGE_NAME、EVENT_GAME_ACTOR_CHANGE_NAME
struct event_actor_change_name : public event_base
{
	UID		uActorUID;		// 角色UID
    PDBID   nPDBID;         // 角色ID
    char    szName[ACTORNAME_MAXSIZE];  // 修改后的角色

	event_actor_change_name() { memset(this, 0, sizeof(*this)); }
};

// 玩家修改活动度       EVENT_GAME_ACTOR_SET_ACTIVITY
struct event_actor_set_activity : public event_base
{
    UID		uActorUID;		// 角色UID
    PDBID   nPDBID;         // 角色ID
    int     nDayValue;      // 天活跃度
    int     nWeekValue;     // 周活跃度
    int     nFlag;          // 活跃度标识 PLAYER_ACTIVITY_EVENT_FLAG

    event_actor_set_activity() { memset(this, 0, sizeof(*this)); }
};


// EVENT_GAME_WAR_PERSON_TASK
struct event_game_war_person_task : public event_base
{
	DWORD dwPDBID;
	char szTeamMemberID[GAME_MSGSTR_MAXSIZE];
	char szWarResult[GAME_LONGSTR_MAXSIZE];
	//1.英雄类型2.比赛类型
	//3.是否联盟组队4.是否师徒组队5.是否胜利6.是否超神7.是否MVP8.是否输出最高9.是否拆塔最高10.是否助攻最高11.是否金钱最高
	//12.最大连杀数量13.最大不死击杀数量14.击杀数量15.助攻数量16.战绩评分17.获得金钱数量18.决胜控制数量19.惊险救治数量20.拆塔数量21.击杀小龙数量22.击杀大龙数量23.死亡数量

	event_game_war_person_task() { memset(this, 0, sizeof(*this)); }
};

// 客户端移除buff			EVENT_ENTITY_CLIENT_REMOVE_BUFF
struct event_entity_client_remove_buff : public event_base
{
    UID uidOperator;        // 操作者

    event_entity_client_remove_buff() { memset(this, 0, sizeof(*this)); }
};

// 玩家数据任务事件 EVENT_GAME_ACTOR_DATA_TASK
struct event_game_actor_data_task : public event_base
{
	DWORD dwPDBID;
	WORD  nTaskLuaEvent;
	int   nParam1;
	int   nParam2;
	int   nParam3;

	event_game_actor_data_task() { memset(this, 0, sizeof(*this)); }
};


// 续玩客户端连入事件 EVENT_CONTINUE_PLAY_CLIENT_INCOMMING
struct event_continue_play_client_incomming : public event_base
{
    DWORD dwDBID;
    ClientID client;

    event_continue_play_client_incomming() { memset(this, 0, sizeof(*this));}
};

// 角色金钱变化            EVENT_ACTOR_CHANGE_MONEY
struct event_actor_change_money : public event_base
{
    DWORD	dwUserID;				// 帐号ID
    DWORD	dwActorID;				// 角色ID        
    int		nTotalTicket;			// 当前总点券
    int		nTotalBindTicket;		// 当前总金币
	int		nTotalTicketBefore;		// 變化前总点券
	int		nTotalBindTicketBefore;	// 變化前总金币
    bool    bResult;                // 改变结果,true.成功 false.失败
	char	szParam[128];			// 附加文本

    event_actor_change_money() { memset(this, 0, sizeof(*this)); }
};

// 玩家复活事件            EVENT_ACTOR_RELIVE
struct event_actor_relive : public event_base
{
    event_actor_relive()
    { 
        memset(this, 0, sizeof(*this));
    }
};

// 玩家激活天赋            EVENT_ENTITY_ACTIVE_TALENT
struct event_entity_active_talent : public event_base
{
    DWORD   nPDBID;                 // 角色PDBID
    int     nTalentID;              // 天赋ID


    event_entity_active_talent()
    { 
        memset(this, 0, sizeof(*this));
    }
};

// 触发NPC			EVENT_PERSON_MEETNPC
struct event_entity_npc_trigger : public event_base
{
	DWORD uidNPC;
	PDBID pdbid;

	event_entity_npc_trigger ()
	{ 
		memset(this, 0, sizeof(*this));
	}
};

// 点击NPC功能			
struct event_entity_npc_func : public event_base
{
	UID		uidNPC;								// NPC UID
	PDBID  pdbid;
	DWORD	dwParamLen;							// 参数长度

	event_entity_npc_func ()
	{ 
		memset(this, 0, sizeof(*this));
	}
};

// 切换游戏状态   EVENT_ACTOR_CHANGE_GAMESTATE
struct event_entity_actor_change_gamestate : public event_base
{
	DWORD           dwState;

	event_entity_actor_change_gamestate() { memset(this, 0, sizeof(*this)); }
};

// pk目标改变 EVENT_ACTOR_PK_TARGET_CHANGE
struct event_actor_pk_target_change : public event_base
{
    UID     uidTarget;

    event_actor_pk_target_change():uidTarget(0xFFFFFFFF){}
};

// 主角冲锋状态改变 EVENT_SELF_SHIFT_STATE_CHANGE
struct event_self_shift_state_change : public event_base
{
    bool   bShift;

    event_self_shift_state_change():bShift(false){}
};

// 阻挡类结界创建 EVENT_GAME_BLOCK_BARRIER_CREATE
struct event_block_barrier_create : public event_base
{
    int                 nBarrierShape; 
    int                 nBlockType;
    UID                 uidCreater;
    DWORD               dwSerial; 
    int                 nTargetFilter;

    // 圆环定义  // c++里结构体不能放入union
    struct circle_def
    {
        Vector3     locCenter;
        float       fInnerRadius;
        float       fOutterRadius;
    } circle;

    // 直线(有厚度)定义
    struct line_def
    {
        Vector3     locStart;
        Vector3     locEnd;
        float       fWidth;   // 直线宽度
        float       fHeight;  // 直线高度
    } line;

    // 圆柱体定义
    struct cylinder_def
    {
        Vector3 locCenter;
        float fRadius;
        float fHeight;
    } cylinder;

    event_block_barrier_create()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 阻挡类结界销毁   EVENT_GAME_BLOCK_BARRIER_DESTROY, 
struct event_block_barrier_destroy : public event_base
{
    UID                 uidCreater;
    DWORD               dwSerial; 
    int                 nBlockType;

    event_block_barrier_destroy()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 准备替换技能
struct event_prereplace_spell : public event_base
{
    int nSlotType;
    int nSlotIndex;

    event_prereplace_spell()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 附属怪物通知事件  EVENT_ENTITY_BELONG_MONSTER_NOTIFY
struct event_entity_belong_monster_notify : public event_base
{
    UID  uidMaster;
    UID  uidMonster;
    int  nFlag;         // 标识 MONSTER_BELONG_EVENT_NOTIFY_FLAG
    int  nValue;        // (生命周期时 1.创建,0.销毁)
    
    event_entity_belong_monster_notify(){ memset(this, 0, sizeof(*this)); }
};

// 增益助攻Buff EVENT_GAME_ADD_ASSIST_BUFF
struct event_game_add_assist_buff : public event_base
{
	UID    uid;
	int    nCount;  // SActor_Add_Buff_Node记录数量
	
	event_game_add_assist_buff()
	{
		memset(this, 0, sizeof(*this)); 
	}

};

// 玩家预进入场景  EVENT_SCENE_PRE_ENTER_SCENE
struct event_scene_pre_enter_scene : public event_base
{
	int nSceneID;

	event_scene_pre_enter_scene()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 加好友事件
struct event_sns_add_buddy : public event_base
{
	PDBID dwOptPdbid;	// 操作者pdbid

	PDBID dwPdbid;		// 被加者pdbid
	BYTE  bySex;		// 被加者性别

	event_sns_add_buddy()
	{
		memset(this, 0, sizeof(*this));
	}
};

//  角色跨场景预导出数据事件   EVENT_ACTOR_PRE_EXPORT_DATA
struct event_actor_pre_export_data : public event_base
{
	int nFlag;
	event_actor_pre_export_data()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 战队长改变事件 EVENT_KIN_SHAIKHID_CHANGED
struct event_kin_shaikhid_changed : public event_base
{
    int nOldID;
    int nNewID;

    event_kin_shaikhid_changed()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 盟主改变事件 EVENT_CLAN_SHAIKHID_CHANGED
struct event_clan_shaikhid_changed : public event_base
{
    int nOldID;
    int nNewID;

    event_clan_shaikhid_changed()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 玩家充值点券成功   EVENT_GAME_ACTOR_TOPUP_SUCCESS
struct event_player_topup_success : public event_base
{
	DWORD   dwPDBID;            // 角色ID
	int		nTicket;			// 提取点券

	event_player_topup_success() { memset(this, 0, sizeof(*this)); }
};

// 大逃杀战场-安全区信息
struct event_war_safetyzone_info : public event_base
{
	int		nRound;					// 当前轮数
	Vector3 centerOfNoxZone;		// 毒气圈圆心
	float	radiusOfNoxZone;		// 毒气圈半径
	Vector3 centerOfSafetyZone;		// 安全圈圆心
	float	radiusOfSafetyZone;		// 安全圈半径
	DWORD	spreadBeginTick;		// 扩散开始时间
	DWORD	spreadTime;				// 扩散时间

	event_war_safetyzone_info()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 排位赛季开始
struct event_match_season_begin : public event_base
{
	int		nBeginSeasonIndex;			// 开始赛季索引

	event_match_season_begin()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 排位赛季结束
struct event_match_season_end : public event_base
{
	int		nEndSeasonIndex;			// 结束赛季索引

	event_match_season_end()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack(pop)