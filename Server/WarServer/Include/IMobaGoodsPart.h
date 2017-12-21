/*******************************************************************
** 文件名:	IMobaGoodsPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	03/06/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IEntity.h"
#include "MobaGoodsDef.h"
#include "SlotDef.h"
struct IMobaGoods;
// 实体状态部件接口
struct __IMobaGoodsPart : public __IEntityPart
{
	
	//获取是否有相同ID装备
	virtual bool HaveOtherSameGoods(int nGoodID, int nSlotIndex) = 0;

	//重算未启用的效果
	virtual void RecomputeAllGoodsEffect() = 0;

	//使用物品检查
	virtual bool CanUseSkill(SSlotSkillInfo *SlotSkillInfo) = 0;

	//使用物品技能
	virtual bool UseSkill(SSlotSkillInfo *SlotSkillInfo) = 0;

	// 添加属性到属性管理器
	virtual bool AddEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType) = 0;

	// 移除属性管理器属性
	virtual bool RemoveEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType) = 0;

    // 添加分身怪物继承属性
    virtual bool AddMonsterInheritEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType) = 0;

    // 移除分身怪物继承属性
    virtual bool RemoveMonsterInheritEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType) = 0;

	// 设置能否购买或者出售物品
	virtual void SetCanBuyOrSell(bool isCanBuyOrSell) = 0;

	// 重置能使用的次数
	virtual void ResetCanUseCount() = 0;
	// 获得物品对象
	virtual IMobaGoods* getMobaGoods(SLOT_TYPE  nSlotType,int nSlotIndex) = 0 ;

    // 更新单个槽位数据到客户端
    virtual void UpdateGoodSlotInfo(SLOT_TYPE  nSlotType,int nSlotIndex) = 0;

    // 效果外部接口 添加物品
    virtual void effectAddGood(int nGoodID,int nCount) = 0;

    // 效果外部接口 移除物品
    virtual void effectRemoveGood(int nGoodID, int nCount) = 0;

    // 添加分身怪物继承属性
    virtual bool AddCreateMonsterInheritEffect(UID uid, int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType) = 0;

    // 添加继承怪物
    virtual void AddInheritMonster(UID uid) = 0;

    // 移除继承怪物
    virtual void RemoveInheritMonster(UID uid) = 0;

    // 获取记录的多重冷却效果数据
    virtual SEffectCDOverlay getCDOverlayInfo(int nFreezeID) = 0;

    // 获取记录的多重冷却效果数据
    virtual void setCDOverlayInfo(int nFreezeID, SEffectCDOverlay data) = 0;

    // 移除多重冷却效果数据
    virtual void removeCDOverlayInfo(int nFreezeID) = 0;

    // 外部调用接口 原价出售物品
    virtual void sellGoodsOriginalPrice() = 0;

    // lua外部接口 购买物品
    virtual bool luaBuyGoods(int nGoodID, int nCost) = 0;

    // lua外部接口 出售物品
    virtual void luaSellGoods(int nSlotIndex, int nGoodID) = 0;
};


////////////////////////////物品对像////////////////////////////////////
struct IMobaGoods
{
	/** 获取物品是否是新添加
	@param   
	@param   
	@return 
	*/
	virtual bool 						GetIsNewAddGoods() = NULL;

	/** 设置物品是否是新添加
	@param   
	@param   
	@return 
	*/
	virtual void 						SetIsNewAddGoods(bool bState) = NULL;

	/** 获取物品技能ID
	@param   
	@param   
	@return 
	*/
	virtual int 						GetSkillID() = NULL;

	/** 获取物品类型
	@param   
	@param   
	@return  能叠加的物品返回累积的数量 消耗次数物品的返回能使用的次数
	*/
	virtual emMobaGoodType				GetGoodsType() = NULL;

	/** 获取能使用的次数 
	@param   
	@param   
	@return  能叠加的物品返回累积的数量 消耗次数物品的返回能使用的次数
	*/
	virtual int				GetCanUseTimes() = NULL;
	/** 获得物品ID
	@param   
	@param   
	*/
	virtual int					GetGoodID()= NULL;

	/** 获得物品槽位
	@param   
	@param   
	@return  
	*/
	virtual int					GetSlotIndex() = NULL;

	/** 获得物品能使用次数
	@param   
	@param   
	@return  
	*/
	virtual int					GetCanUseCount() = NULL;

    /** 设置能使用的物品次数
	@param   
	@param   
	@return  
	*/
	virtual void				SetCanUseCount(int nCanUseCount) = NULL;

    /** 重置能使用的次数
	@param   
	@param   
	*/
	virtual bool				ResetCanUseCount() = NULL;
    

	/** 能否使用
	@param   
	@param   
	@return  
	*/
	virtual bool				CanUse() = NULL;

	/** 能否使用
	@param   
	@param   
	@return  
	*/
	virtual bool				Use() = NULL;

	/** 取得配置信息
	@param   
	@param   
	@return  
	*/
	virtual SSchemeMobaGoods*	GetSchemeInfo() = NULL;


	/** 物品初始化
	@param   
	@param   
	*/
	virtual void				Initial(__IEntity * pPerson, int SlotIndex,int nGoodID) = NULL;

	/** 添加物品属性效果
	@param   
	@param   
	*/
	virtual void				AddGoodsEffect() = NULL;

	/** 移除物品属性效果
	@param   
	@param   
	*/
	virtual void				RemoveGoodsEffect()= NULL;

	/** 是否能累加数量
	@param   
	@param   
	*/
	virtual bool				CanAddOverlayCount() = NULL;

	/** 累加当前使用数量
	@param   
	@param   
	*/
	virtual bool				AddOverlayCount() = NULL;

	/** 获取当前累加数量
	@param   
	@param   
	*/
	virtual int					GetOverLayCount() = NULL;

	/** 设置当前累加数量
	@param   
	@param   
	*/
	virtual void				SetOverLayCount(int nOverLayCount) = NULL;

	/** 重算未启用效果
	@param   
	@param   
	*/
	virtual void				RecomputeGoodsEffect() = NULL;

	/** 移除
	@param   
	@param   
	*/
	virtual bool				Remove() = NULL;

	/** 释放
	@param   
	@param   
	*/
	virtual void				Release() = NULL;

	

    /** 设置多重冷却
	@param   
	@param   
	*/
	virtual void                SetMultipleCoolDown(int nInterval,int OverlayCountLimit,bool bResetUseCount,int bInitalUseCount) = NULL;

    // 添加新创建分身怪继承属性
    virtual void                addCreateMonsterInheritEffect(UID uid) = 0;

    /** 序列化相关
	@param   
	@param   
	*/
    virtual bool			    onSerialize( rkt::ibuffer & in,int flag ) = NULL;
	virtual bool			    deSerialize( rkt::obuf & out,int flag ) = NULL;
    virtual bool			    onLoad( __IEntity* pEntity, int nLoginMode) = NULL;
    
};


////////////////////////////物品对像属性////////////////////////////////////
struct IMobaGoodsProp
{

	/** 添加效果
	@param   
	@param   
	@return  
	*/
	virtual bool				AddEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID) = NULL;

	/** 移除效果
	@param   
	@param   
	@return  
	*/
	virtual bool				RemoveEffect(int nSlotID, int nEffectID) = NULL;

	/** 移除所有效果
	@param   
	@param   
	@return  
	*/
	virtual void				RemoveAllEffect() = NULL;

	/** 释放
	@param   
	@param   
	*/
	virtual void				Release() = NULL;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IMobaGoodsPart * CreateMobaGoodsPart();
#	define	CreateMobaGoodsPartProc	CreateMobaGoodsPart
#else													/// 动态库版本
typedef __IMobaGoodsPart * (RKT_CDECL * procCreateMobaGoodsPart)();
#	define	CreateMobaGoodsPartProc	DllApi<procCreateMobaGoodsPart>::load(MAKE_DLL_NAME(WarServer), "CreateMobaGoodsPart")
#endif
