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

// 通用效果现场
struct EFFECT_CONTEXT
{
	IEntity	   *pEntity;
	UID		   uidOperator;		// 操作者(比如接到其他玩家使用技能消息，需不需要读条，其他玩家技能结束，自己操作不操作等)
	UID        uidTarget;
    Vector3	   ptStartTile;		// 技能使用起点
	Vector3	   ptTargetTile;	// 使用技能时鼠标点击的位置
    Vector3    ptTargetTileOri;
	int		   nID;				// 技能ID，buffID等
	DWORD	   dwTime;			// 时间
	float	   fDistance;		// 技能距离
	float	   fAttackRange;	// 技能范围
	int		   nPASD;			// 攻击速度
    int		   nAttackCount;	// 攻击目标数量，第几个目标(0表示首个目标)
    bool       bOnAppear;         // 这个效果早就开启了只是进视野才知道(表示一种状态,如虫子的钻地状态)

	EFFECT_CONTEXT()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 效果接口
struct IEffect
{
	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext ) = 0;

	// 效果停止
	virtual void			Stop() = 0;

	// 克隆一个新效果
	virtual IEffect *       Clone() = 0;

	// 释放
	virtual void			Release() = 0;

	// 取得效果ID
	virtual int				GetEffectID() = 0;
};

// 效果管理接口
struct IEffectFactory
{
	// 加载效果工厂
	virtual bool            Load( const char * szFileName )= 0;

	/**
	@name				: 创建效果对象
	@param				: 效果ID
	@return
	*/
	virtual IEffect*		BuildEffect(int nEffectID) = 0;

	/**
	@name				: 执行一个内置效果(比如技能系统注册的内置效果)
	@param	strType  	: 效果类型名
	@param  pData       : 效果数据
	@param  nLen        : 效果数据长度
	@return
	*/
	virtual bool            DoInnerEffect(const char * strType,void * pData,int nLen,EFFECT_CONTEXT *pContext ) = 0;

	// 释放
	virtual void			release() = 0;
	// 某个效果ID是否存在
	virtual bool			IsExistEffectID(int nEffectID) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"
#if defined(_LIB) || defined(EFFECTCLIENT_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(EffectClient))
	extern "C" IEffectFactory * CreateEffectFactory();
#	define	CreateEffectFactoryProc	CreateEffectFactory
#else													/// 动态库版本
	typedef IEffectFactory * (RKT_CDECL * procCreateEffectFactory)();
#	define	CreateEffectFactoryProc	DllApi<procCreateEffectFactory>::load(MAKE_DLL_NAME(EffectClient), "CreateEffectFactory")
#endif
