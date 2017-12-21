/*******************************************************************
** 文件名:	BuffDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/13/2014
** 版  本:	1.0
** 描  述:	BUFF公用定义

			
********************************************************************/


#ifndef __COMMON_BUFF_DEF_H__
#define __COMMON_BUFF_DEF_H__

#pragma pack(1)

#define  BUFFDATA_LEN_MAX			168

// 是否可添加某BUFF处理结果
enum BUFF_ADD_RESULT
{
	EBuffAdd_Success,			// 添加BUFF成功
	EBuffAdd_Failure,			// 添加BUFF失败（原因为其它）

	// 下面的值应与抗性类型匹配
	EBuffAdd_KxXuanYun,			// 被眩晕抗性抵抗
	EBuffAdd_KxJianSu,			// 被减速抗性抵抗
	EBuffAdd_KxKongJu,			// 被恐惧抗性抵抗
	EBuffAdd_KxFengMo,			// 被封魔抗性抵抗
	EBuffAdd_KxDingShen,		// 被定身抗性抵抗
};


// BUFFFlag类型定义
enum BUFF_FLAG_TYPE
{
	tBuff_TypeFlag_Client			    = 0x00000001,	// 客户端创建
	tBuff_TypeFlag_Bcast			    = 0x00000002,	// 需要广播
	tBuff_TypeFlag_Save				    = 0x00000004,	// 保存数据库
	tBuff_TypeFlag_OffTime			    = 0x00000008,	// 下线计时

	tBuff_TypeFlag_System			    = 0x00000010,	// 系统BUFF
	tBuff_TypeFlag_Magic			    = 0x00000020,	// 魔法BUFF
	tBuff_TypeFlag_Toxic			    = 0x00000040,	// 中毒BUFF
	tBuff_TypeFlag_SpeedDown		    = 0x00000080,	// 减速BUFF

	tBuff_TypeFlag_Morbid			    = 0x00000100,	// 疾病BUFF
	tBuff_TypeFlag_HealDispel		    = 0x00000200,	// 有益驱散被技能驱散
	tBuff_TypeFlag_HurtDispel		    = 0x00000400,	// 有害驱散被技能驱散
	tBuff_TypeFlag_HurtDispelWd		    = 0x00000800,	// 有害驱散被无敌驱散

	tBuff_TypeFlag_Faint			    = 0x00001000,	// 眩晕BUFF
	tBuff_TypeFlag_Affright			    = 0x00002000,	// 惊吓BUFF
	tBuff_TypeFlag_Durance			    = 0x00004000,	// 禁锢BUFF
	tBuff_TypeFlag_Freeze			    = 0x00008000,	// 冻结BUFF

	tBuff_TypeFlag_Apotheosize		    = 0x00010000,	// 封魔BUFF
	tBuff_TypeFlag_Back				    = 0x00020000,	// 击退BUFF
	tBuff_TypeFlag_Fly				    = 0x00040000,	// 击飞BUFF
	tBuff_TypeFlag_IsShow			    = 0x00080000,	// 显示等级

    tBuff_TypeFlag_DieRemove            = 0x00100000,	// 死亡关闭光效
    tBuff_TypeFlag_ImmuneControl	    = 0x00200000,	// 免疫控制
    tBuff_TypeFlag_Useful	            = 0x00400000,	// 有益BUFF
    tBuff_TypeFlag_DieRemoveReliveAdd   = 0x00800000,	// 死亡关闭复活重加光效

	tBuff_TypeFlag_QueryAccelerate		= 0x01000000,	// 查询加速(跨线程查询是否存在时不需要锁线程)
	tBuff_TypeFlag_Invisible			= 0x02000000,	// 隐身buff
	tBuff_TypeFlag_BcastOnlyMaster		= 0x04000000,	// 只广播给主人
	tBuff_TypeFlag_AddBuffReplaceTime	= 0x08000000,	// 添加相同Buff时候，只替换buff持续时间

};

// BUFF结束类型
enum BUFF_END_TYPE
{
    BUFF_END_DIE = 1,                   // 死亡结束
    BUFF_END_INJURED,                   // 受伤结束
    BUFF_END_MOVE,                      // 移动结束
    BUFF_END_ATTACK,                    // 攻击结束
    BUFF_END_ATTACK_READY,              // 蓄气时取消
    BUFF_END_INJURED_BY_ACTOR,          // 被英雄攻击取消
    BUFF_END_CLIENT_REMOVE,             // 客户端取消
};

// 韧性免疫标志位
#define TENACITY_IMMUNE_FLAG (tBuff_TypeFlag_SpeedDown|tBuff_TypeFlag_Faint|tBuff_TypeFlag_Affright|tBuff_TypeFlag_Durance|tBuff_TypeFlag_Freeze|tBuff_TypeFlag_Apotheosize)
// 控制免疫标志位
#define CONTROL_IMMUNE_FLAG (tBuff_TypeFlag_SpeedDown|tBuff_TypeFlag_Faint|tBuff_TypeFlag_Affright|tBuff_TypeFlag_Durance|tBuff_TypeFlag_Freeze|tBuff_TypeFlag_Apotheosize)


struct SBuffSchemeInfo						// buff配置信息
{	
	DWORD				dwBuffID;			// buffID
	char				szName[64];			// buff名字
	DWORD				dwReplaceRule;		// 叠加规则
	DWORD				dwFlag;				// 标志
	list<DWORD>			listEndFlag;		// 结束条件
	int					nIconID;			// 图标id
	int					nPileQty;			// 叠加数量

	struct SLevelInfo
	{
		DWORD			dwBuffLevel;		// 等级
		char			szDesc[256];		// 描述

		DWORD			dwFlashID;			// 光效ID
		DWORD			dwFlashIDEx;		// 光效ID 扩展

		DWORD			dwTimeLong;			// 持续时间
		DWORD			dwTimeLongEx;		// 持续时间 扩展

		list<DWORD>		listEffect;			// 效果表

		list<DWORD>		listHasBuff;		// BUFF条件
		DWORD			dwHasCheck;			// BUFF条件 检查方式

		list<DWORD>		listNoBuff;			// 非BUFF条件
		DWORD			dwNoCheck;			// 非BUFF条件 检查方式

		list<DWORD>		listDispel;			// 驱散BUFF
		DWORD			dwDispelPer;		// 驱散概率

		list<DWORD>		listRandEffect;		// 随机效果
		DWORD			dwRandPer;			// 随机效果概率
		DWORD			dwRandFlashID;		// 随机效果光效
		//char			szRandDesc[256];	// 随机效果描述

		list<DWORD>		listEndAroseBuff;	// 后续触发BUFF：id, level,id, level, id, level
		DWORD			dwEndArosePer;		// 后续触发概率

		int				nAccQueryIndex;		// 快速查询索引
	};

	list<SLevelInfo>	listLevelInfo;		// buff各等级配置
};
typedef map<DWORD, SBuffSchemeInfo>			TMAP_BUFFSCHEMEINFO;


// 与效果相关的BUFF信息
struct SEffectFromBuffInfo
{
	// 此效果是属于谁的ＢＵＦＦ
	LONGLONG uidMaster;

	// 此效果所属的ＢＵＦＦ的序号是多少
	DWORD dwBuffIndex;

	SEffectFromBuffInfo()
	{
		memset(this, 0, sizeof(*this));
	};
};

// BUFF节点
struct SBuffNode
{
	DWORD			dwID;			// buffID
	DWORD			dwLevel;		// 等级

	SBuffNode()
	{
		memset(this, 0, sizeof(*this));
	};
};

// BUFF数据
struct SBuffContext
{
    int        nID;             // 技能ID
	int		   nAttackCount;	// 攻击目标数量，第几个目标(0表示首个目标)
	Vector3	   ptTargetTile;	// 使用技能时鼠标点击的位置
    float	   fFocoEnergiaPCT;	// 蓄力百分比

	SBuffContext()
	{
		memset(this, 0, sizeof(*this));
	};
};

#pragma pack()
#endif	// __COMMON_BUFF_DEF_H__