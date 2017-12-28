/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\MobaGoods.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 秦其勇
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场管理信息
** 应  用: 
**************************** 修改记录 ******************************/
#pragma once

#include "stdafx.h"
#include "MobaGoodsSchemeDef.h"
#include "MobaGoodsProp.h"
#include "IEntity.h"
#include "IFreezeServer.h"
#include "SpellDef_ManagedDef.h"
#include "EntityHelper.h"
#include "ISpell.h"
#include "ITimerService.h"
#include "MultiCDGoods.h"

using namespace SPELL;
class CMobaGoods: public IMobaGoods
{
public:
	CMobaGoods();

	virtual ~CMobaGoods();

	virtual void				Initial(__IEntity * pPerson, int nSlotIndex, int nGoodID);

	virtual void				AddGoodsEffect();

	virtual void				RecomputeGoodsEffect();

	virtual void				RemoveGoodsEffect();

	virtual bool				CanAddOverlayCount();

	virtual bool				AddOverlayCount();

	virtual int					GetOverLayCount();

	virtual void				SetOverLayCount(int nOverLayCount);

    virtual void                addCreateMonsterInheritEffect(UID uid);

	/** 是否是充能物品
	@param
	@param
	@return
	*/
	virtual bool 				bIsMultipleCDGoods();

    /** 设置更新槽位数据
	@param   
	@param   
	@return 
	*/
    virtual void				UpdateGoodSlotInfo();

	/** 获取物品技能ID
	@param   
	@param   
	@return 
	*/
	virtual int 				GetSkillID();

	/** 移除
	@param   
	@param   
	*/
	virtual bool				Remove();


	/** 获得物品ID
	@param   
	@param   
	@return  
	*/
	virtual int					GetGoodID();

	/** 获得物品槽位
	@param   
	@param   
	@return  
	*/
	virtual int					GetSlotIndex();

	/** 获得物品能使用数量
	@param   
	@param   
	@return  
	*/
	virtual int					GetCanUseCount();

	/** 能否使用
	@param   
	@param   
	@return  
	*/
	virtual bool				CanUse();

	/** 能否使用
	@param   
	@param   
	@return  
	*/
	virtual bool				Use();


	/** 取得配置信息
	@param   
	@param   
	@return  
	*/
	virtual SSchemeMobaGoods*	GetSchemeInfo();

	/** 取得所属人物
	@param   
	@param   
	@return  
	*/
	virtual __IEntity *			GetMaster();

	/** 消耗数量
	@param   
	@param   
	@return  
	*/
	virtual void				ConsumedOverlayCount();

	/** 消耗使用次数
	@param   
	@param   
	@return  
	*/
	virtual void				ConsumedUseCount();

	/** 获取能使用的次数 
	@param   
	@param   
	@return  能叠加的物品返回累积的数量 消耗次数物品的返回能使用的次数
	*/
	virtual int					GetCanUseTimes();

	/** 添加冷却
	@param   
	@param   
	@return  
	*/
	virtual void				AddFreezeTime(EFREEZE_CLASS FreezeClass, int nFreezeID);

	/** 获取物品是否是新添加
	@param   
	@param   
	@return 
	*/
	virtual bool 				GetIsNewAddGoods() ;

	/** 设置物品是否是新添加
	@param   
	@param   
	@return 
	*/
	virtual void 				SetIsNewAddGoods(bool bState);

	/** 获取物品类型
	@param   
	@param   
	@return  能叠加的物品返回累积的数量 消耗次数物品的返回能使用的次数
	*/
	virtual emMobaGoodType		GetGoodsType();

	/** 重置使用可以使用次数
	@param   
	@param   
	@return  重置消耗次数物品能使用的次数
	*/
	virtual bool				ResetCanUseCount();
  
    /** 设置可以使用的次数
	@param   
	@param   
	@return 
	*/
	virtual void				SetCanUseCount(int nCanUseCount);
    
	/** 设置多重冷却初始化多重冷却所需数据
	@param   
	@param   
	@return  
	*/
	virtual void                SetMultipleCoolDown(int nInterval,int OverlayCountLimit,bool bResetUseCount, int bInitalUseCount);
	

    /** 释放
	@param   
	@param   
	@return  
	*/
	virtual void				Release();

    /** 序列化相关
	@param   
	@param   
	*/
    virtual bool			    onSerialize( rkt::ibuffer & in,int flag );

	virtual bool			    deSerialize( rkt::obuf & out,int flag );

    virtual bool			    onLoad( __IEntity* pEntity, int nLoginMode);

    void                        recordeMultipleCDInfo();


private:

	__IEntity *				m_pMaster;
	bool					m_bIsNewAdd;
	int						m_nGoodID;
	int						m_nSlotIndex;
	int						m_nCurOverlayCount; //	当前叠加数量
	int						m_nCanUseCount;		//	能使用次数
	emMobaGoodType			m_GoodsType;
    bool                    m_bMultipleCDGoods;
	
	//效果ID 效果状态结构
	typedef map<int,SMobaGoodEffectInfo>	T_MAP_EFFECT;
	T_MAP_EFFECT			m_mapEffect;

    T_MAP_EFFECT            m_mapInheritEffect;         //召唤怪物继承属性

    CMultiCDGoods*          m_pMultiCDGoods;
};