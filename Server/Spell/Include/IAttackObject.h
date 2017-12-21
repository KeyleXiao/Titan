/*******************************************************************
** 文件名:	IAttackObject.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	

			攻击对象
********************************************************************/


#pragma once

struct SPELL_CONTEXT;
using namespace SPELL;

// 攻击对象定时器
#define ATTACK_LOGIC_TIMER	0

/**
技能释放后，创建攻击对象，攻击对象根据自己的轨迹和伤害方式，给命中的目标发送伤害消息
攻击对象类型:
1. 单体  【必命中】
2. 定点范围 【圆形】
3. 直线     【长方形】
4. 飞行单体 【锁定】【只需要计算命中时间】
5. 飞行直线 【圆形】【沿途的所有目标都被击中】
6. 飞行爆炸 【圆形】【沿途不管，目的地爆炸】
*/

struct ATTACK_DATA
{
	int nID;				 // 技能ID
	int	nTargetCount;		 // 最大攻击目标数量
	float fChooseRange;		 // 选择范围
	float fAttackRange;      // 攻击范围
	float fDistance;		 // 技能距离
	float fFlyingSpeed;		 // 飞行速度
	float fUseHeight;		 // 使用技能高度
	float fAttackHeight;     // 技能攻击高度
	int nTargetFilter;		 // 技能目标类型
	float fFocoEnergiaPCT;	 // 蓄力百分比
	DWORD dwAttackSerialID;	 // 攻击对象序号
	int nSpellFlag;		     // 技能标识
    int nSelectType;         // 选择类型
	int nCastType;			 // 使用类型
    int nCastFlag;           // 技能使用标识
    Vector3 vSelfLoc;        // 自己位置点
    int nDuration;           // 技能持续时间
    DWORD dwSpellStartTick;  // 技能开始时间

	ATTACK_DATA()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 伤害信息
struct SDamageInfo
{
	// 同目标伤害次数
	int		 nTargetDamageCount;
	// 上次伤害时间
	DWORD	nLastDamageTick;

	SDamageInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct IAttackObject
{
	virtual bool Run( ATTACK_DATA * pAttack,SPELL_CONTEXT * pContext ) = 0;

	// 取得攻击类别
	virtual ATTACK_TYPE GetType() = 0;

	// 取得攻击对象起点
	virtual Vector3 getStartTile(void) = 0;

	// 取得攻击对象当前点
	virtual Vector3 getTargetTile(void) = 0;

	// 取得能否回收
	virtual bool getCanRecycle(void) = 0;

    // 取得技能开始时刻
    virtual DWORD getSpellStartTick() = 0;

	// 释放
	virtual void Release() = 0;
};
