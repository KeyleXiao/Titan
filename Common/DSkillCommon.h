#pragma once

#include "IEntity.h"
#include "NetMessageDef.h"

// 技能条件文件名
#define SKILL_SCHEME_FILENAME					"Scp\\Skill.csv"
#define SKILLCONDITION_SCHEME_FILENAME			"Scp\\SkillCondition.csv"
#define SKILLINFLUENCE_SCHEME_FILENAME			"Scp\\SkillInfluence.csv"
#define SKILLACTION_SCHEME_FILENAME				"Scp\\SkillAction.csv"

#define MAX_SKILL_OBJECT_LEFT		120*1000	// 技能对象生命期

// 技能条件最大数
#define SKILL_CONDITION_MAX	8
// 技能条件配置
struct SSkillConditionScheme
{
	int		nSkillValue;
	int		nType;
	int		nData[SKILL_CONDITION_MAX];

	SSkillConditionScheme()
	{
		memset(this, 0, sizeof(SSkillConditionScheme));
	}
};

// 技能影响最大数
#define SKILL_INFLUENCE_MAX	8
// 技能影响配置
struct SSkillInfluenceScheme
{
	int		nSkillValue;
	int		nType;
	int		nDetailType;
	int		nData[SKILL_INFLUENCE_MAX];

	SSkillInfluenceScheme()
	{
		memset(this, 0, sizeof(SSkillInfluenceScheme));
	}
};

// 技能动作最大数
#define SKILL_ACTION_MAX	8
// 技能动作配置
struct SSkillActionScheme
{
	int		nSkillValue;
	int		nStartTime;			// 起始时间
	int		nDurationTime;		// 持续时间
	int		nActionID;			// 动作ID
	int		nData[SKILL_ACTION_MAX];

	SSkillActionScheme()
	{
		memset(this, 0, sizeof(SSkillActionScheme));
	}
};

enum
{
	MAX_SKILL_COUNT = 1000,			// 最大大类技能数
	MAX_SUBSKILL_COUNT = 1000,		// 大类技能下最大子技能数
	MAX_ACTOR_SKILL_COUNT = 64,		// 角色最大技能数
};

typedef XlsValue						SkillProperty;
typedef XlsRecord						SkillPropList;
typedef RecordSet<MAX_SUBSKILL_COUNT>	SkillScheme;

// 技能执行/学习结果
enum EMSkillResult
{
	SkillResult_Success,			// 成功
	SkillResult_InvalidArgs,		// 无效参数
	SkillResult_Unknow,				// 未知错误

	// 逻辑错误
	SkillResult_ScantyHP,			// HP不够
	SkillResult_NoTarget,			// 没有目标
	SkillResult_Colding,			// 冷却中
};
#define SkillSuccess(ret)	((ret) == SkillResult_Success)
#define SkillFailed(ret)	(!SkillSuccess(ret))
#define MAKE_SKILLVALUE(mainId,subId) ((mainId)*MAX_SKILL_COUNT+(subId))

// 技能行为
enum EMSkillAction
{
	SkillAction_None,				//
	SkillAction_Start,				// 开始
	SkillAction_Process,			// 过程中
	SkillAction_End,				// 结束
	SkillAction_Max,
};

enum EMSkillState
{
	enSkillState_Normal,				// 正常状态(非施放技能状态)
	enSkillState_RapidAttack,			// 瞬发型技能施法状态

	enSkillState_Max,
};

// 技能伤害类型
enum EMSkillDamageType
{
	enSkillDamage_None,					// 无类型
	enSkillDamage_Attack = 0x01,		// 攻击型
	enSkillDamage_Defence = 0x02,		// 防守型
	enSkillDamage_All = 0x03,			// 攻防一体
};

// 技能脚本字段定义
enum EMSkillProperty
{
	// 大类技能对象属性
	SkillProperty_ObjType,			// 对象类型(1-技能大类,2-子技能)(ulong)
	SkillProperty_Name,				// 名字(string)
	SkillProperty_Desc,				// 技能简单描述(string)
	SkillProperty_Id,				// 技能Id(大类Id或子技能Id)(ushort)
	SkillProperty_MaxLevel,			// 最大等级(ulong)
	SkillProperty_IconId,			// 正常,按下,失效图标Id(IntArray)
	SkillProperty_ContinueHitTime,	// 连击时间间隔

	// 子技能相关属性
	SkillProperty_SkillType,		// 技能类型，参考SkillType的枚举定义
	SkillProperty_ViewId,			// 外观Id，仅客户端用，依赖外观脚本(ulong)
	SkillProperty_ColdTimeId,		// 冷却时间Id
	SkillProperty_TotalTime,		// 时间跨度

	// 伤害属性
	SkillProperty_DamageValue,		// 附加伤害

	MaxSkillPropertyCount,
};

// 技能类型
enum SkillType
{
	// 技能操作方式(选择类型，单选)
	SkillType_NoSelect		= 0x01,			// 不需要选择(主要对自己，如加血)
	SkillType_SelectEntity	= 0x02,			// 选择实体(服务器选择)
	SkillType_SelectTile	= 0x04,			// 选择TILE(客户端选择)
	SkillType_SelectMask	= 0x07,

	// 效果中心点(中心点类型，单选)
	SkillType_Myself		= 0x10,			// 以自己为中心点
	SkillType_Target		= 0x20,			// 以目标(点)为中心点
	SkillType_CenterMask	= 0x30,

	// 对象筛选方式之阵营(单选)
	SkillType_NoCamp		= 0x100,		// 无阵营
	SkillType_Enmity		= 0x200,		// 敌方
	SkillType_Amity			= 0x400,		// 友方
	SkillType_FilterMask	= 0x800,
};

#pragma pack(push)
#pragma pack(1)


/// 技能ID
struct SkillId
{
	ushort mainId;					// 技能大类ID
	ushort subId;					// 子技能Id，用于角色时指等级

	SkillId()
	{
		memset(this, 0, sizeof(SkillId));
	}
	SkillId(ushort maidId, ushort subId)
	{
		this->mainId = maidId;
		this->subId = subId;
	}
};

// 技能现场对象实现
struct SkillContextImp
{
	UID			uidOperator;		// 使用技能的操作者
	UID			uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	SkillId		skillId;
	int			viewid;				// 技能视图ID
	DWORD		dwSerialID;			// 序列号ID

	SkillContextImp()
	{
		memset(this,0,sizeof(SkillContextImp));
	}
};

/// 用于网络传输用的，其他模块不要访问
struct SkillContextImp_CS
{
	UID			uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
	UID			uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	SkillId		skillId;			// 技能ID

	SkillContextImp_CS()
	{
		memset(this,0,sizeof(SkillContextImp_CS));
	}
};

// 用于网络传输用的，其他模块不要访问
struct SkillContextImp_SC
{
	ulong		uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
	ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	SkillId		skillId;			// 技能ID
	int			viewid;				// 技能视图ID
	DWORD		dwSerialID;			// 序列号ID

	SkillContextImp_SC()
	{
		memset(this,0,sizeof(SkillContextImp_SC));
	}
};
#pragma pack(pop)

////////////////////////////////////////////////////////////////////
#define		MISC_PERCENT_BASE				10000

// Skill、buff、effect、Freeze公共数据
#define		MAX_BUILDCONTEXT_COUNT			8
struct SMiscBuildContext
{
	int		nValue1[MAX_BUILDCONTEXT_COUNT];
	int		nValue2[MAX_BUILDCONTEXT_COUNT];
	int		nPlusValue;											// 附加数值

	SMiscBuildContext(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
