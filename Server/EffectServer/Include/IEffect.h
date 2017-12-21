/*******************************************************************
** 文件名:	IEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/23
** 版  本:	1.0
** 描  述:	效果管理器相关接口
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEntity.h"

struct CONDITION_CONTEXT;

// 通用效果现场
struct EFFECT_CONTEXT
{
	__IEntity* pEntity;
	UID        uidOperator;		// 操作者(一些事件与pEntity不是同一个对象)
	UID        uidTarget;		// 目标
    Vector3	   ptStartTile;		// 技能使用起点
	Vector3	   ptTargetTile;	// 使用技能时鼠标点击的位置
    Vector3	   ptTargetTileOri;	// 使用技能时鼠标点击的位置(击中事件的上下文中ptTargetTile已经被替换为击中点 这里记录一下初始的)
	int		   nID;				// 技能ID
	DWORD	   dwTime;			// 时间
	DWORD	   dwAttackSerialID;	// 攻击对象序号
	DWORD	   dwIndex;				// buffIndex等
	int		   nAttackCount;		// 攻击目标数量，第几个目标(0表示首个目标)
	int		   nSlotIndex;			// 槽索引
	int        nSlotType;			// 槽位类型	(1、装备 2、技能)
	DWORD	   dwFlag;				// buff标识位
    float	   fFocoEnergiaPCT;	    // 蓄力百分比
    Vector3	   vSelfLoc;	        // 施法时自己位置
    DWORD      dwStartTick;         // 技能开始时间
	int		   nReverse1;			// 保留字段1

	EFFECT_CONTEXT()
	{
		memset(this, 0, sizeof(*this));

        ptTargetTileOri = INVALID_LOC;
	}
};

ENUM EFFECT_TYPE
{
    DESC("扩展效果")
    EFFECT_EX       = 0,
    DESC("改变值效果")
    EFFECT_CHANGE_VALUE,
};

// 效果接口
struct IEffect
{
    // 获取效果ID
    virtual int             GetID() = 0;

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext=NULL ) = 0;

	// 效果停止
	virtual void			Stop() = 0;

	// 克隆一个新效果
	virtual IEffect *       Clone() = 0;

	// 释放
	virtual void			Release() = 0;

    // 取得效果类型
    virtual int				GetEffectType() = 0;

	// 检查配置数据的合法性。因为存放配置数据的是struct，不方便加虚函数，所以放在这里检测。同时，为了保持此类为接口类，故设为纯虚函数。
	virtual bool			Check() = 0;

    // 技能调时 是否开始后马上停止
    virtual bool            StopImmAsSpellEffect() = 0;
};

struct IEffectEx : public IEffect
{
    // 取得效果类型
    virtual int				GetEffectType() { return EFFECT_EX; }
	// 检查配置数据的合法性，默认为合格
	virtual bool			Check() { return true;}
    // 技能调用时 是否马上停止
    virtual bool            StopImmAsSpellEffect() { return true; }
};

typedef std::list<double> EFFECT_VALUE_LIST;
typedef std::list<string> EFFECT_STRING_LIST;
struct IEffectChangeValue : public IEffectEx
{
    // 取得效果类型
    virtual int				GetEffectType() { return EFFECT_CHANGE_VALUE; }

    // 设置数值
    virtual void			SetValue(EFFECT_VALUE_LIST *pListValue) {};

    // 设置数值
    virtual void			SetValue(EFFECT_STRING_LIST *pListValue) {};

	// 获取数值类型
	virtual int				GetValueType() { return -1; }

	// 检查配置数据的合法性，默认为合格
	virtual bool			Check() { return true; }
};

// 效果管理接口
struct IEffectFactory
{
	// 加载效果工厂
	virtual bool            Load( const char * szFileName )= 0;

	/**
	@name				: 执行一个内置效果(比如技能系统注册的内置效果)
	@param	strType  	: 效果类型名
	@param  pData       : 效果数据
	@param  nLen        : 效果数据长度
	@return
	*/
	virtual bool            DoInnerEffect(const char * strType,void * pData,int nLen,EFFECT_CONTEXT *pContext ) = 0;

	/**
	@name				: 创建效果对象
	@param				: 效果ID
	@return
	*/
	virtual IEffect*		BuildEffect(int nEffectID) = 0;

	/** 执行Service的on_stop
	@return  
	*/
    virtual void            onStop(void) = 0;

	// 释放
	virtual void			release() = 0;

	// 某个效果ID是否存在
	virtual bool			IsExistEffectID(int nEffectID) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"
#if defined(_LIB) || defined(EFFECTSERVER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(EffectServer))
	extern "C" IEffectFactory * CreateEffectFactory();
#	define	CreateEffectFactoryProc	CreateEffectFactory
#else													/// 动态库版本
	typedef IEffectFactory * (RKT_CDECL * procCreateEffectFactory)();
#	define	CreateEffectFactoryProc	DllApi<procCreateEffectFactory>::load(MAKE_DLL_NAME(EffectServer), "CreateEffectFactory")
#endif
