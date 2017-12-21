
/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\MobaGoods.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 秦其勇
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场多重冷却相关
** 应  用: 
**************************** 修改记录 ******************************/
#pragma once
#include "stdafx.h"
#include "IServerGlobal.h"
#include "TimerHandler.h"
#include "ITimerService.h"
#include "MobaGoodsSchemeDef.h"
/*********************************************************
物品多重冷却管理类
    1、设置可以多重物品可以使用的次数
    2、更新物品的使用次数
    3、更新物品的冷却时间
    4、注意：充能物品一般有两种冷却（使用冷却 和充能冷却）
**********************************************************/

enum MultiCDState
{
    State_Charge,	            // 充能状态
    State_UseInterval,          // 使用间隔状态
    State_Full,                 // 充满状态
};

class CMultiCDGoods :public TimerHandler
{
    enum
    {
        ETimerEventID_GoodsMultipleChargedFreeze    = 0,	    // 多重充能冷却定时器
        ETimerEventID_GoodsMultipleIntervalFreeze   = 1,        // 多重使用间隔冷却定时器
    };

    
public:
    CMultiCDGoods(void);
    ~CMultiCDGoods(void);

    ////////////////////////////////TimerHandler//////////////////////////////////////////
    /** 定时器
	@param   
	@param   
	@return  
	*/
	virtual void                OnTimer( unsigned long dwTimerID );

    /** 多重冷却充能逻辑定时
	@param   
	@param   
	@return  
	*/
	virtual void                OnGoodsMultipleCDTimeTimer();

	/** 多重冷却间隔逻辑定时
	@param   
	@param   
	@return  
	*/
	virtual void                OnGoodsMultipleIntervalTimeTimer();

    /** 多重冷却间隔逻辑定时
	@param   
	@param   
	@return  
	*/
    bool                        inital( __IEntity* pEntity, IMobaGoods* pMobaGood,int nInterval,int OverlayCountLimit,bool bResetUseCount, int bInitalUseCount);

    /** 释放
	@param   
	@param   
	@return  
	*/
    void                        release();

    /** 充能物品能否使用
	@param   
	@param   
	@return  
	*/
    bool                        canUse();

    /** 添加冷却
	@param   
	@param   
	@return  
	*/
    void                        AddFreezeTime(EFREEZE_CLASS ecFreezeClass, int nFreezeID);

    /** 添加冷却
	@param   
	@param   
	@return  
	*/
    bool                        ResetCanUseCount();

    /** 序列化相关
	@param   
	@param   
	*/
    virtual bool			    onSerialize( rkt::ibuffer & in,int flag );

	virtual bool			    deSerialize( rkt::obuf & out,int flag );

    virtual bool			    onLoad( __IEntity* pEntity, IMobaGoods *pMobaGoods);
    
private:

    /** 获得多重冷却间隔时间
	@param   
	@param   
	@return  间隔时间
	*/
    int     getMultipleFreezeIntervalTime();


private:
    __IEntity *				m_pMaster;
    IMobaGoods*             m_pMobaGoods;
    // 多重冷却
    MultiCDState            m_eCDState;                             //  冷却状态
    bool                    m_bIsExistLeftChargedTime;              //  剩余冷却充能时间是否存在     
    int                     m_nLeftChargeFreezeTime;                //  多重冷却充能剩余冷却时间
    int                     m_nUseInterval;                         //  多重冷却使用间隔
    int                     m_MultipleOverlayCountLimit;            //  多重冷却充能次数
    DWORD                   m_dwChargeFreezeTick;                   //  最近一次充能冷却时间 （和使用间隔冷却分开）       
    bool                    m_bMultipleResetUseCount;               //  多重冷却的物品
    int                     m_nNeedSubFreezeTime;                  //  使用间隔大于充能冷却的时候需要补充的冷却时间
};

