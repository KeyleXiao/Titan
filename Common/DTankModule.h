/*******************************************************************
** 文件名:	e:\Rocket\Common\DTankModule.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/4/13
** 版  本:	1.0
** 描  述:	载具模块定义	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

//////////////////////////////////////////////////////////////////////////
// BUFF等级节点
struct SBuffLevelNode
{
	DWORD	dwBuffID;			// BUFFID
	WORD	wLevel;				// BUFF等级

	SBuffLevelNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


#ifndef __D_TANKMODULE_H__
#define __D_TANKMODULE_H__


// 打印调试信息,并设为测试模式
//#define TANK_PRINT_DEBUG_INF
//#define TANKCARRY_PRINT_DEBUG_INF

#define TANK_SCHEME_FILENAME 				"Scp\\Tank.csv"					// 载具配置名字

enum TANK_SCHEME_COLUMN
{
	TANK_COL_ID = 0,							// 怪物ID
	TANK_COL_NAME,							// 怪物名
	TANK_COL_PREFABPATH,						// 预制体路径
	TANK_COL_HP,								// 生命值
	TANK_COL_SPEED,							// 移动速度
	TANK_COL_ATTACK,							// 攻击力
	TANK_COL_EYESHOT,						// 视野
	TANK_COL_CAMP,							// 阵营
};


struct __IPlayerRole;

#pragma pack(1)

/******************************************************************/
/////////////////////载具的公共结构体定义 ////////////////////////
/******************************************************************/
///////////////////////////////////////////////////////////////////


// tolua_begin


// 公用定义 ( 打头: TANK )

#define TANK_MAX_ID					10000000			// 最大载具ID
#define TANKPATROL_MAX_ID			10000				// 最大载具巡逻ID
#define TANK_MAX_PERSON				100					// 最大容量数
#define TANK_MAX_SKILL_INDEX		5					// 最大技能序号
#define TANK_MAX_PK_SKILL_INDEX		35					// 最大技能序号
#define TANK_MAX_SEARCH_STEPCOUNT	128					// 随机巡逻最大搜路点数
#define TANK_MAX_STEPCOUNT			300					// 固定线路最大可走路点数
#define TANK_MAX_STEPCOUNT0			4					// 到达时最大可走路点数
#define TANK_REACH_DISTANCE			5					// 到达目地多少距离就算到达
#define TANK_AI_INTERVAL			1000				// AI定时器 1000 毫秒
#define TANK_ONSTOP_INTERVAL		2000				// 超时超过算停留 2000 毫秒
#define TANK_MAX_MAPTILE			1500				// 地图最大title数
#define TANK_MAX_STOPTIME			3600				// 停留最长时间 60 秒
#define TANK_STR_BONELEN			512					// 坐位骨骼名列表字串长度
#define TANK_MAX_REACH_TILE			50					// 最大能进入距离
#define TANK_STAT_DELAYTIME			60000				// 开车时间 60000 毫秒
#define TANK_SKILL_DELAYTIME		500					// 技能冷却最小 500 毫秒
//////////////////////////////////////////////////////////////////////////

#define TANK_MAX_SKILLRIGHT			8					// 技能权限区间定义最大个数
#define TANK_EXPORTBUFFER_MAXSIZE	32768				// 载具传送导出数据Buffer最大size
#define PK_TANK_SKILL_MAX_NUM		40					// pk载具快捷栏数据最大个数
#define TANK_INITBUFF_MAXCOUNT		5					// 载具出生BUFF最大数量

// 载具伤害计算公式
enum EMTANKDAMAGEFORMULA
{
	TANKDAMAGEFORMULA_1 = 1,		// 第一套伤害公式：伤害=（攻击*攻击）/（攻击/10+3*防御）
	TANKDAMAGEFORMULA_2,			// 第二套伤害公式：伤害=（攻击*攻击）/（攻击/10+3*防御）*（1-（攻击-标准攻击）/攻击*削减系数）
	TANKDAMAGEFORMULA_MAXID,		// 最大方式ID
};


// 技能的使用权限参数信息
struct STankSkillRight
{
	BYTE nStart;	// 开始
	BYTE nEnd;		// 结束

	STankSkillRight(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 自定义载具MAP Key
struct STankCustomKey
{
	long			lTankID;						// 怪物ID
	long			lLevel;							// 等级

public:
	STankCustomKey(const STankCustomKey& data)
	{
		this->lTankID = data.lTankID;
		this->lLevel = data.lLevel;
	};

	STankCustomKey(long lTankID, long lLevel)
	{
		this->lTankID = lTankID;
		this->lLevel = lLevel;
	};

	bool operator < (const STankCustomKey& data) const
	{
		if (this->GetKeyValue()<data.GetKeyValue())
		{
			return true;
		}
		return false;
	};

	LONGLONG GetKeyValue() const
	{
		LONGLONG keyValue = this->lTankID;
		keyValue = keyValue << 32;
		keyValue = keyValue & 0xffffffff00000000;
		keyValue = keyValue + this->lLevel;
		
		return keyValue;
	};
};



// Tank.csv
struct STankSchemeInfo
{
	int	nTankID;								// 载具ID
	int nMaxHP;									// 生命值
	int nMoveSpeed;								// 移动速度
	int nAttackValue;							// 攻击力
	int fEyeshot;								// 视野
	uint8 byCamp;								// 阵营
	char szName[GAME_NAME_MAXSIZE];				// 怪物名
	char szPrefabPath[GAME_NAME_MAXSIZE];		// 预设体路径

	STankSchemeInfo(void)
	{
		memset(this, 0, sizeof(*this));
	};

	void Clan(void)
	{
		memset(this, 0, sizeof(*this));
	};
};


//PKTankSkill.csv
struct SPKTankSkillSchemeInfo
{
	long				lTankID;				//载具id
	DWORD				dwSkill_1_8[8];			//快捷键1~8
	DWORD				dwSkill_s1_s0[10];		//快捷键s1~s0
	DWORD				dwSkill_f1_f8[8];		//快捷键f1~f8
	DWORD				dwSkill_r1_r8[8];		//右边快捷键1~8

	SPKTankSkillSchemeInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 承载记录信息数据
struct STankNodeData
{
	DWORD	dwIndex;		// 序号
	DWORD	dwPlayerID;		// 玩家ID
	LONGLONG uidPlayer;		// 玩家UID
	__IPlayerRole* pPlayer;		// 人物
	DWORD	dwTime;			// 进入时间
	POINT	ptStart;		// 进入位置
	bool	bForceExit;		// 是否强退,true:强退
	BYTE	nSkillRight;	// 技能权限

	STankNodeData(void)
	{
		memset(this, 0, sizeof(*this));
	};

	void clear(void)
	{
		uidPlayer	= 0;
		dwPlayerID	= 0;		// 玩家ID
		pPlayer		= NULL;		// 人物
		dwTime		= 0;		// 进入时间
		ptStart.x	= 0;
		ptStart.y	= 0;
		bForceExit	= false;
	};

};


// 客户端承载记录信息数据
struct STankClientNodeData
{
	DWORD	dwIndex;		// 序号
	DWORD	dwPlayerID;		// 玩家ID
	DWORD	dwTime;			// 进入时间
	BYTE	nSkillRight;	// 技能权限
	char szPlayerName[32];	// 人物名

	STankClientNodeData(void)
	{
		memset(this, 0, sizeof(*this));
	};

	void clear(void)
	{
		dwPlayerID	= 0;		// 玩家ID
		dwTime		= 0;		// 进入时间
		memset(szPlayerName, 0, sizeof(szPlayerName));
	};

};


// 放出乘客方式
enum EMTankPutOutMemberType
{
	// 放出乘客方式ID定义规则（TANK_OUTTYPE + 功能涵义）
	TANK_OUTTYPE_INVALID = 0,		// 无效方式ID
	TANK_OUTTYPE_ALL,				// 放出所有
	TANK_OUTTYPE_DRIVER,			// 放出所有除去司机
	TANK_OUTTYPE_DIE,				// 载具死亡放出所有
	TANK_OUTTYPE_ENABLE,			// 启用载具放出功能
	TANK_OUTTYPE_DISABLE,			// 禁用载具放出功能
	TANK_OUTTYPE_MAXID,				// 最大方式ID
};

// 乘客载具事件类型
enum EMTankPartEventType
{
	// 乘客载具事件类型ID定义规则（TANKPART_EVENT + 功能涵义）
	TANKPART_EVENT_INVALID = 0,		// 无效方式ID
	TANKPART_EVENT_UPDATE,			// 更新载具数据
	TANKPART_EVENT_ENTER,			// 进入载具
	TANKPART_EVENT_EXIT,			// 载具出来
	TANKPART_EVENT_DRIVER,			// 驾驶事件
	TANKPART_EVENT_INDEXCHANGE,		// 位置改变
	TANKPART_EVENT_ENABLED,			// 是否有效
	TANKPART_EVENT_CONFIRM,			// 要求确认
	TANKPART_EVENT_FLAG,			// 状态标识
	TANKPART_EVENT_PK_ACTION_UPDATE,// pk载具快捷键更新

	TANKPART_EVENT_MAXID			// 最大方式ID
};


// 载具客户端事件类型
enum EMTankPartClientEventType
{
	// 乘客客户端载具事件类型ID定义规则（TANKPART_CLIENT + 功能涵义）
	TANKPART_CLIENT_INVALID = 0,	// 无效方式ID
	TANKPART_CLIENT_ENTER,			// 请求进入指定载具
	TANKPART_CLIENT_EXIT,			// 请求退出当前载具
	TANKPART_CLIENT_ENTER1,			// 邀请进入指定载具
	TANKPART_CLIENT_EXIT1,			// 邀请退出当前载具
	TANKPART_CLIENT_CON_ENTER,		// 司机确认请求进入
	TANKPART_CLIENT_CON_EXIT,		// 司机确认请求退出
	TANKPART_CLIENT_CON_ENTER1,		// 司机确认邀请进入
	TANKPART_CLIENT_CON_EXIT1,		// 司机确认邀请退出
	TANKPART_CLIENT_CON_ENTER2,		// 乘客确认邀请进入
	TANKPART_CLIENT_CON_EXIT2,		// 乘客确认邀请退出
	TANKPART_CLIENT_EXINDEX,		// 请求交换位置
	TANKPART_CLIENT_EXINDEX1,		// 司机确认交换位置
	TANKPART_CLIENT_FIRE,			// 使用技能
	TANKPART_CLIENT_GETINF,			// 请求指定载具详细信息
	TANKPART_CLIENT_EDITTITLE,		// 修改自定义称号
	TANKPART_CLIENT_CHANGEDRIVER,	// 更换司机
	TANKPART_CLIENT_PUTOUT,			// 放出所有乘客
	TANKPART_CLIENT_SETFLAG,		// 开关载具状态
	TANKPART_CLIENT_CONTROLMODE,	// 切换载具控制模式
	TANKPART_CLIENT_REQUESTDATA,	// 请求更新数据
	TANKPART_CLIENT_MAXID,			// 最大方式ID
};


// 载具标识定义,可扩展,共32个标识
// 所有标识用 或位运算得到标识

/// 载具标识定义
enum EMTankBoxFlag
{
	// 载具标识类型ID定义规则（TANK_FLAG + 功能涵义）
	TANK_FLAG_NONE		= 0x0,			// 0	:无
	TANK_FLAG_LOCK		= 0x1,			// 1	:是否上锁,上锁会拒绝上下请求
	TANK_FLAG_FIRE		= 0x2,			// 2	:是否可使用技能
	TANK_FLAG_MOVE		= 0x4,			// 4	:是否能移动
	TANK_FLAG_DRIVER	= 0x8,			// 8	:是否禁用司机
	TANK_FLAG_CONFIRM	= 0x10,			// 16	:请求需司机确认
	TANK_FLAG_FORCE		= 0x20,			// 32	:邀请不需乘客确认
	TANK_FLAG_INVITE	= 0x40,			// 64	:乘客有邀请进出功能
	TANK_FLAG_TOOLS		= 0x80,			// 128	:是否有工具条
	TANK_FLAG_FLY		= 0x100,		// 256	:是否可飞行
	TANK_FLAG_AUTOKILL	= 0x200,		// 512	:没有乘客时是否自动销毁
	TANK_FLAG_DELETE	= 0x400,		// 1024	:是否有解散功能
	TANK_FLAG_KILLSELF	= 0x800,		// 2048	:是否有自爆功能
	TANK_FLAG_NOSETFLAG	= 0x1000,		// 4096	:是否禁用司机的设定功能
	//TANK_FLAG_NOUSE3		= 0x2000,		// 8192	:未用
	//TANK_FLAG_NOUSE4		= 0x4000,		// 16384:未用
	//TANK_FLAG_NOUSE5		= 0x8000,		// 32768:未用
	//TANK_FLAG_NOUSE6		= 0x10000,		// 65536:未用
};

/// 载具控制模式
enum EMTankControlMode
{
	TANK_CONTROLMODE_NONE = 0,			// 无控制
	TANK_CONTROLMODE_DRIVER,			// 驾驶员控制
	TANK_CONTROLMODE_AUTO,				// 自动驾驶控制
	TANK_CONTROLMODE_AI,				// AI控制
	TANK_CONTROLMODE_MAXID,				// 最大ID
};

/// 载具主人类型
enum EMTankOwnerType
{
	TANK_OWNERTYPE_PERSON = 0,			// 人物PDBID
	TANK_OWNERTYPE_TEAM,				// 队伍ID
	TANK_OWNERTYPE_CLAN,				// 帮会ID
	TANK_OWNERTYPE_KIN,					// 战队ID
	TANK_OWNERTYPE_NATION,				// 国家ID
};

/// 载具子类型
enum ETankSubType
{
	TANK_SUBTYPE_NONE = 0,				// 无类型
	TANK_SUBTYPE_KIN,					// 战队战车
	TANK_SUBTYPE_SPECIAL_BMW,			// 特殊双人坐骑，根据性别动态设置坐骑动作
};

/// 载具判断标志
enum ETankOpinionFlag
{
	TANK_OPINION_NONE			= 0x0,			// 0	:空标志
	TANK_OPINION_NOFOLLOW		= 0x1,			// 1	:不能接龙跟随
	TANK_OPINION_NOEXINDEX		= 0x2,			// 2	:不能交换位置
	TANK_OPINION_NOEDITTITLE	= 0x4,			// 4	:不能修改称号
	TANK_OPINION_HORSE          = 0x8,          // 8	:坐骑,选中载具时默认选中玩家
	TANK_OPINION_PERSONTITLE	= 0x10,			// 16	:显示为司机人物头顶所有称号
	TANK_OPINION_ENABLETRANS	= 0x20,			// 32	:可响应传送
	TANK_OPINION_NO_CHANGEDRIVER= 0x40,			// 64	:是否禁止更改司机
	TANK_OPINION_JUMPOUT		= 0x80,			// 128	:是否可以随时跳出
	TANK_OPINION_FORBIDTIP		= 0x100,		// 256	:是否禁用提示
	TANK_OPINION_BREAKPROGBAR	= 0x200,		// 512	:载具移动时是否打断角色读条
	TANK_OPINION_SELECTBYFACE	= 0x400,		// 1024	:通过玩家头像直接选中玩家所在载具
};

// 锁定怪物归属信息
struct SLockMonsterHoldInfo
{
	LONGLONG	uidMonster;				// 怪物UID
	DWORD		dwMonsterID;			// 怪物ID
	LONGLONG	uidHolder;				// 拥有者
	int			nTeamID;				// 队伍ID
	int			nGroupID;				// 团ID

	SLockMonsterHoldInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// tolua_end


//  巡逻点信息
struct STankPatrolPoint				// 巡逻点信息
{
	POINT			ptTile;			// 巡逻点
	int				nStopTime;		// 停留时间

	STankPatrolPoint(void)
	{
		memset(this, 0, sizeof(*this));
	};

	void clear(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

typedef list<STankPatrolPoint> TLIST_TankPatrolPoint;


// TankPatrol.csv
// 载具巡逻信息
struct STankPatrolSchemeInfo
{
	int					nID;			// 巡逻路线ID
	char				szName[32];		// 路线名称
	int					nMapID;			// 巡逻地图ID
	int					nTankID;		// 巡逻载具ID
	int					nAngle;			// 出生角度
	POINT				ptBornTile;		// 出生位置
	int					nPatrolType;	// 巡逻路线形状，（0：环形，1：表示来回形）
	int					nPatrolRadii;	// 巡逻半径，如果非0，则巡逻坐标就无效，表示限定的圆圈内随机移动
	int					nRun;			// 起用巡逻标识(0:不起用，1:起用)
	int					nStopBuffID;		// 停车状态
	int					nStopBuffLevel;		// 停车状态Buff等级
	int					nRunBuffID;			// 车行走状态
	int					nRunBuffLevel;		// 车行走状态Buff等级
	int					nTankCount;			// 	载具个数,线路上的载具数
	int					nInterval;			// 	时间间隔,每多少秒发一趟车,直到发完为止,为0时,系统自动根据载具数量和总线路时间算出平均发车时间
	int					nPathType;			// 	路线类型,0:地上路线, 1:飞行路线
	TLIST_TankPatrolPoint listPatrol;	// 巡逻点

	STankPatrolSchemeInfo(void)
	{
		clear();
		//memset(this, 0, sizeof(*this)-sizeof(TLIST_TankPatrolPoint));
	};
	void clear(void)
	{
		nID = 0;
		nMapID = 0;
		nTankID = 0;
		nAngle = 0;
		ptBornTile.x = 0;
		ptBornTile.y = 0;
		nPatrolType = 0;
		nPatrolRadii = 0;
		nRun = 0;
		nStopBuffID = 0;
		nStopBuffLevel = 0;
		nRunBuffID = 0;
		nRunBuffLevel = 0;
		nTankCount = 0;
		nInterval = 0;
		nPathType = 0;
		memset(szName, 0, sizeof(szName));
		//memset(this, 0, sizeof(*this)-sizeof(TLIST_TankPatrolPoint));
		listPatrol.clear();
	};
};

// 人物载具信息数据
struct SMsgPersonTankData
{
	//LONGLONG uidTank;		// 载具uid
	//DWORD	dwIndex;		// 序号
	bool	bEnabled;		// 是否有效
	bool	bOnDriver;		// 是否驾驶
	bool	bCreate;		// 是否是创建,是时客户端会拉入载具中
	DWORD	dwTankFlag;		// 载具标识，位运算得到标识,参考 EMTankBoxFlag

	SMsgPersonTankData(void)
	{
		memset(this, 0, sizeof(*this));
	};
};



// TANKPART_CLIENT_ENTER 进入载具
// 进入载具信息数据
struct SMsgTankClientEnter
{
	UID uidTank;		// 载具uid
	UID	uidPlayer;		// 玩家ID
	DWORD	dwIndex;		// 序号

	SMsgTankClientEnter(void)
	{
		memset(this, 0, sizeof(*this));
	};
};



// 确认信息数据
struct SMsgTankClientConfirmInfo
{
	BYTE	nType;			// 类型
	LONGLONG uidTank;		// 载具uid
	UID		uidPlayerID;		// 操作人物
	UID		uidSpare;		// 数据
	BYTE	nStrLens;		// 提示字串长度
	//char	szTitleMsg[128];	// 提示字串

	SMsgTankClientConfirmInfo(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 确认结果信息数据
struct SMsgTankClientConfirmResult
{
	BYTE	nType;			// 类型
	UID		uidTank;		// 载具uid
	UID		uidPlayerID;		// 操作人物
	UID		uidSpare;		// 数据
	BYTE	nResult;		// 结果	 0:取消,1:确定

	SMsgTankClientConfirmResult(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 修改自定义称号消息体
struct SMsgTankChangeCustomTitle
{
	char		szCustomTitle[32];	// 自定义称号

	SMsgTankChangeCustomTitle(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 设置标识状态,自己必需为司机
struct SMsgTankChangeFlag
{
	DWORD	dwFlagType;		// 标识类型参考 EMTankBoxFlag
	bool	bFlag;			// 要设置标识状态,true：是，false:否

	SMsgTankChangeFlag(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 读条进入载具相关数据
struct SEventProgressEnterTankData
{
	LONGLONG uidTank;		// 载具uid
	DWORD	dwPlayerID;		// 玩家ID
	DWORD	dwIndex;		// 序号

	SEventProgressEnterTankData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 传送载具相关保存数据
struct STankExportSaveUserData
{
	bool	bOnDriver;				// 是否司机,只有司机才会带上载具导出数据
	DWORD	dwDriverPlayerID;		// 司机角色ID,为0 表示没有传送
	DWORD	dwSeatIndex;			// 坐位号

	STankExportSaveUserData(void)
	{
		clear();
	};
	void clear(void)
	{
		memset(this, 0, sizeof(*this));
	};
};



// 换服务器时承载记录信息数据
struct STankExportRenewBoxData
{
	DWORD			dwOwnerID;			// 主人角色ID
	BYTE			nOwnerType;			// 主人类型
	DWORD			dwLifeTime;			// 载具存活时间(秒),为0时无限
	DWORD			dwFlag;				// 载具标识，位运算得到标识,参考 EMTankBoxFlag
	DWORD			dwControlMode;		// 载具控制模式，参考 EMTankControlMode
	DWORD			dwCounts;			// 成员数据个数
	DWORD			dwEnterListCounts;	// 能够进入载具的人物列表个数
	DWORD			m_dwWorldID;		// 游戏世界ID
	DWORD			m_dwCountryID;		// 国家ID
	// STankExportRenewNodeData  member[dwCounts];		// 成员数据
	// DWORD		dwEnterPDBID[nEnterListCounts];		// 能够进入载具的人物列表

	STankExportRenewBoxData(void)
	{
		memset(this, 0, sizeof(*this));
	};
};


// 换服务器时承载记录信息数据
struct STankExportRenewNodeData
{
	DWORD	dwIndex;		// 序号
	DWORD	dwPlayerID;		// 玩家ID

	STankExportRenewNodeData(void)
	{
		memset(this, 0, sizeof(*this));
	};
};


// 发送人物载具事件到客户端信息数据
struct SMsgTankBuildClientSelf_SC
{
	LONGLONG uidTank;		// 载具uid
	BYTE	dwIndex;		// 序号
	WORD	dwTankFlag;		// 载具标识，位运算得到标识,参考 EMTankBoxFlag
	bool	bEnabled:4;		// 是否有效
	bool	bOnDriver:4;	// 是否驾驶

	SMsgTankBuildClientSelf_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//载具快捷栏数据
struct STankSkillData
{
	DWORD		dwOwnerID;				//角色的pbdID
	DWORD		dwTankID;				//载具ID
	DWORD		dwTankSkills[PK_TANK_SKILL_MAX_NUM];		//技能列表

	STankSkillData(void)
	{
		memset(this,0,sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 载具锁定怪控制标识
enum EMLockMonsterFlag
{
	LockMonsterFlag_None				= 0x0,	// 0	:无
	LockMonsterFlag_NoDriverUnLock		= 0x1,	// 1	:没有司机时释放怪物
	LockMonsterFlag_ProtectHPUnLock		= 0x2,	// 2	:不高于保护血量时释放怪物
	//LockMonsterFlag_NoUse		= 0x4,			// 4	:未用
	//LockMonsterFlag_NoUse		= 0x8,			// 8	:未用
	//LockMonsterFlag_NoUse		= 0x10,			// 16	:未用
	//LockMonsterFlag_NoUse		= 0x20,			// 32	:未用
	//LockMonsterFlag_NoUse		= 0x40,			// 64	:未用
	//LockMonsterFlag_NoUse		= 0x80,			// 128	:未用
};

/// 被锁怪伤血类型
enum EMONSTERDAMAGEMPTYPE
{
	MONSTERDAMAGEMPTYPE_MAXHP = 0,				// 最大血量的百分比掉血
	MONSTERDAMAGEMPTYPE_LOCK_CURHP,				// 按被锁时当前血量的百分比掉血
	MONSTERDAMAGEMPTYPE_CALC_CURHP,				// 按计算时当前血量的百分比掉血
	MONSTERDAMAGEMPTYPE_MAXID,					// 最大
};
// TankLockMonster.csv
// 载具锁定怪配置表
struct STankLockMonsterSchemeInfo
{
	long	lTankID;							// 载具ID
	long	lMonsterID;							// 被锁怪物ID
	BYTE	byMonsterMaxCount;					// 此怪物的可被锁最大数量
	BYTE	byDisLockMinHPPer;					// 可锁怪最低血量百分比，用于保护怪不被锁定
	DWORD	dwLockTimeLen;						// 锁定持续时长 秒
	DWORD	dwDamageTimeLen;					// 伤害持续时长 秒
	WORD	wDamageInterval;					// 伤害定时间隔 秒
	BYTE	byDamageType;						// 被锁怪伤血类型 EMONSTERDAMAGEMPTYPE
	WORD	wDamageHPPer;						// 定时伤血百分比
	WORD	wNotDamageHPPer;					// 保护最低血量百分比
	WORD	wNoDriverUnLockTimeLen;				// 无司机丢怪延迟（秒），无司机时怪物不掉血
	BYTE	byFlag;								// 控制标识

	STankLockMonsterSchemeInfo(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 载具定时扩展策略
enum EMTankTimerDecision
{
	emTankTimerDecision_None = 0,				// 无效
	emTankTimerDecision_MyCountry,				// 本国玩家
	emTankTimerDecision_NotMyCountry,			// 非本国玩家
	emTankTimerDecision_My2UnionCountry,		// 本国和盟国玩家
	emTankTimerDecision_NotMy2UnionCountry,		// 非本国和盟国玩家
	emTankTimerDecision_MyCamp,					// 本阵营玩家
	emTankTimerDecision_NotMyCamp,				// 非本阵营玩家
	emTankTimerDecision_Max,					// 最大
};
// 载具定时扩展配置表 TankTimerExtend.csv
struct STankTimerExtendSchemeInfo
{
	DWORD	dwTankID;							// 载具ID
	DWORD	dwDecision;							// 载具定时扩展策略 EMTankTimerDecision
	DWORD	dwBuffID;							// 状态ID
	DWORD	dwLevel;							// 等级
	int		nDistance;							// 有效距离Tile
	DWORD	dwInterval;							// BUFF频率 秒
	int		nRunMaxCount;						// 可运行最大次数 -1不限制次数

	STankTimerExtendSchemeInfo(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#pragma pack()


#endif	// __D_TANKMODULE_H__