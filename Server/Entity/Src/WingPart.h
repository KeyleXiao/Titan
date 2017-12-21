/*******************************************************************
** 文件名:	WingPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	翅膀部件实现
********************************************************************/
#pragma once

#include "IWingPart.h"
#include "IEntity.h"

class CWingPart:public __IWingPart, public IEventExecuteSink, public TimerHandler
{
    // 飞行定时器
    #define FLY_CHECK_TIMER    0
    // 飞行定时器间隔
    #define FLY_CHECK_INTERVAL     1000
public:
	/////////////////////////////////////////ISerializable/////////////////////////////////////////
	virtual bool onSerialize(rkt::ibuffer &in, int flag);

	virtual bool deSerialize(rkt::obuf &out, int flag);

	/////////////////////////////////////////__IEntityPart/////////////////////////////////////////
	/** 取得部件ID
	*/
	virtual short getPartID(void);

	/** 载入部件
	*/
	virtual bool onLoad(__IEntity *pEntity, int nLoginMode);

	/** 取得部件实体
	*/
	virtual __IEntity *getHost(void);

	/** 部件消息
	*/
	virtual bool onMessage(void *pEntityHead, int nMsgID, void *pData, size_t nLen);

	/** 部件释放
	*/
	virtual void release(void);

    // 切换英雄
    virtual void  onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual void  restore();

    /////////////////////////////////////////__IWingPart/////////////////////////////////////////
    // 设置翅膀ID
    // @param nWingID : 翅膀ID
    virtual bool SetWingID(int nWingID);

    // 取得翅膀ID
    virtual int GetWingID(void);

    // 设置翅膀状态
    // @param nWingID : 翅膀状态
    virtual void SetWingState(int nWingState);

    // 取得翅膀状态
    virtual int GetWingState(void);

    // 设置翅膀飞行时间
    // @param nTime : 飞行时间
    virtual void SetFlyTime(int nTime);

    // 增加翅膀飞行时间
    // @param nTime : 增加飞行时间
    virtual void AddFlyTime(int nTime);

    // 取得翅膀飞行时间
    virtual int GetFlyTime(void);

    // 检测改变翅膀状态
    // @param nStateType : 翅膀状态
    virtual bool CheckChangeWingState(int nStateType);

    // 设置翅膀属性
    // @param nType  : 翅膀属性
    // @param fValue : 翅膀数值
    virtual void SetWingProperty(int nType, float fValue);

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
    /** 
    @param   wEventID ：事件ID
    @param   bSrcType ：发送源类型
    @param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
    @param   pszContext : 上下文
    @param   nLen : 上下文长度
    @return  
    @note     
    @warning 
    @retval buffer 
    */
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    /////////////////////////////////////////TimerHandler/////////////////////////////////////////
    /**
    @purpose          : 定时器触发后回调,你可以在这里编写处理代码
    @param	 dwTimerID: 定时器ID,用于区分是哪个定时器
    @return		      : empty
    */
    virtual void OnTimer( unsigned long dwTimerID );

	/////////////////////////////////////////CWingPart/////////////////////////////////////////
	/** 构造函数
	*/
	CWingPart(void);

	/** 析构函数
	*/
	~CWingPart(void);

private:
    // 客户端消息设置翅膀状态
    void OnClientMessageSetWingState(void *pData, size_t nLen);

    // 改变翅膀状态
    void OnChangeWingState(int nWingState);

    // 设置翅膀信息
    void SetWingInfo();

    // 更新翅膀信息
    void UpdateWingInfo(void);

    // 发出状态改变事件
    void fireFlyStateChangeEvent(bool bFlying);

    // 清除飞行信息
    void ClearFlyInfo(void);

    // 更新飞行时间
    void UpdateFlyTime(void);

    // 检查体力
    void checkFlyStamina();

    // 检查飞行时间
    void checkFlyTime();

    // 检查遭遇敌人
    void checkFlyEncounter();

    // 设置自己对全图可见
    void exposeSelf(bool bExpose);

    // 戴上翅膀
    void onLoadWing();

    // 卸装翅膀
    void onUnloadWing();

private:
	// 技能部件实体
	__IEntity			*m_pEntity;

    // 翅膀信息
    SWing               m_Wing;

    // 翅膀状态
    int                 m_nWingState;

    // 飞行时间
    int                 m_nFlyTime;

    // 最后飞行时间
    DWORD               m_dwLastFlyTime;

    // 是否改变翅膀信息
    bool                m_bChangeWing;
};