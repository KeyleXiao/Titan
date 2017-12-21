/*******************************************************************
** 文件名:	Buff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/13  11:29
** 版  本:	1.0
** 描  述:	BUFF
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <list>
using namespace std;
#include "IEffect.h"
#include "BuffDef.h"
#include "BuffPart.h"
#include "HasCheck.h"
#include "NoHasCheck.h"
#include "EndConditionAttack.h"
#include "EndConditionChangeLoc.h"
#include "EndConditionDie.h"
#include "EndConditionInjured.h"
#include "EndConditionStartMove.h"
#include "EndConditionAttackReady.h"
#include "EndConditionInjuredByActor.h"
#include "EndConditionClientRemoveByActor.h"

class CBuff : public TimerHandler
{
	typedef std::list<IEffect *> TLIST_EFFECT;
	typedef list<DWORD>			TLIST_DWORD;	
	enum
	{
		ETimerEventID_Life = 0,	// buff生命周期
	};
public:
	//////////////////////////ITimeAxis///////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void			OnTimer(unsigned long dwTimerID);

	///////////////////////////////////////////////////////////////////
	/** 释放,会释放内存
	@param   
	@param   
	@return  
	*/
	void					Release(void);

	/** 还原,不释放内存，只将BUFF还原到创建时BUFF
	@param   未来继续使用
	@param   
	@return  
	*/
	void					Restore(bool bCreateEndArose);

	/** 创建
	@param   
	@param   
	@return  
	*/
	bool					Create(DWORD dwIndex, CBuffPart *pBuffPart, UID uidAdd, DWORD dwBuffID, DWORD dwBuffLevel, DWORD dwTime = 0, EFFECT_CONTEXT *pContext=NULL);

	/** 启用
	@param   
	@param   
	@return  
	*/
	bool					Start();

	/** BUFF ID
	@param   
	@param   
	@return  
	*/
	DWORD					GetBuffID(void);

	/** 等级
	@param   
	@param   
	@return  
	*/
	DWORD					GetBuffLevel(void);

	/** 标志
	@param   
	@param   
	@return  
	*/
	DWORD					GetFlag(void);

	/** 添加者
	@param   
	@param   
	@return  
	*/
	UID						GetAdd(void);

	/** 
	@param   
	@param   
	@return  
	*/
	DWORD					GetIndex(void);

	/** 总时间
	@param   
	@param   
	@return  
	*/
	DWORD					GetTotalTime(void);

	/** 剩余时间
	@param   
	@param   
	@return  －1：表示时间已越过，不用通知客户端创建。0：表示永久时间。正数表示剩余时间
	*/
	int						GetLeftTime(void);

	/** 
	@param   
	@param   
	@return  
	*/
	bool					GetBuildEffectFlag(void);

	/** 启用
	@param   
	@param   
	@return  
	*/
	bool					StartEffect();

	/** 是否有随机效果
	@param   
	@param   
	@return  
	*/
	bool					IsRandEffect(void);

	/** 本BUFF是否有效
	@param   
	@param   
	@return  
	*/
	bool					IsValid(void);

	/** 判断某个效果是否存在
	@param   dwEffectID:效果ID
	@param  lIgnoreUid:忽略的添加者 
	@return  
	*/
	bool					IsEffectIDExist(DWORD dwEffectID, UID lIgnoreUid);

	/** 获取使用技能时鼠标点击的位置
	@param
	@return
	*/
	Vector3				    GetEffectTargetTile();

	/** 获取技能ID
	@param
	@return
	*/
	int						GetSpellID();

	/** 只重置持续时间
	@param
	@return
	*/
	bool					AddBuffOnlyReplaceTime(DWORD dwBuffLevel, DWORD dwTime);

	/**
	@param   
	@param   
	@return  
	*/
	CBuff(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CBuff(void);

private:
	/** 构建属性表
	@param   
	@param   
	@return  
	*/
	bool					BuildEffectList();

private:
	////////////////////////////////////
	// buff id
	DWORD					m_dwBuffID;

	// 等级
	WORD					m_dwBuffLevel;

	// 标志
	DWORD					m_dwSchemeFlag;

	// 效果表
	TLIST_DWORD				m_listSchemeEffect;

	// 随机效果概率
	DWORD					m_dwSchemeRandPer;

	// 随机效果
	TLIST_DWORD				m_listSchemeRandEffect;

	// 后续触发概率
	DWORD					m_dwSchemeEndArosePer;

	// 后续触发BUFF：id, level,id, level, id, level
	TLIST_DWORD				m_listSchemeEndAroseBuff;

	////////////////////////////////////
	// 时间
	DWORD					m_dwTime;

	// 启动时间
	DWORD					m_dwStartTick;

	// 效果表
	TLIST_EFFECT			m_listEffect;

	// 序号
	DWORD					m_dwIndex;

	// 添加者
	UID						m_uidAdd;

	// 被作用者部件
	CBuffPart *				m_pBuffPart;

	// 效果
	EFFECT_CONTEXT			m_effectContext;

	// 是否已经创建效果
	bool					m_bBuildEffectFlag;

	// BUFF是否已经启动，防止重复启动
	bool					m_bStartBuffFlag;

	// BUFF条件
	CHasCheck				m_HasCheck;

	// 非BUFF条件
	CNoHasCheck				m_NoHasCheck;

	// 是否需要check才能启动
	bool					m_bIsCheckStart;

	// 是否有随机效果
	bool					m_bRandEffect;

	// 本BUFF是否依然有效
	bool					m_bThisValidFlag;

	////////////////////////////////////
	// 主动攻击后取消
	CEndConditionAttack		m_EndConditionAttack;

	// 改变位置后取消
	CEndConditionChangeLoc	m_EndConditionChangeLoc;

	// 死亡后取消
	CEndConditionDie		m_EndConditionDie;

	// 被击后取消
	CEndConditionInjured	m_EndConditionInjured;

	// 开始移动取消
	CEndConditionStartMove	m_EndConditionStartMove;

	// 有蓄气攻击，蓄气时取消
	CEndConditionAttackReady m_EndConditionAttackReady;

    // 被英雄攻击取消
    CEndConditionInjuredByActor m_EndConditionInjuredByActor;

    // 客户端取消
    CEndConditionClientRemoveByActor m_EndConditionClientRemoveByActor;

	////////////////////////////////////
	// 是否回收利用的，调试用
	bool					m_bRecycle;

    bool                    m_bEffectStartFlag;
};