/*******************************************************************
** 文件名:	PlayerKillPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/04/2016
** 版  本:	1.0
** 描  述:	PK部件实现
********************************************************************/
#pragma once

#include "IPlayerKillPart.h"
#include "IEntity.h"
#include "PlayerKillMessageDef.h"

class CPlayerKillPart:public __IPlayerKillPart, public TimerHandler, public IEventExecuteSink, public IEventVoteSink
{
    // PK逻辑定时器
    #define PK_LOGIC_TIMER    0
    #define PK_LOGIC_TIME     1000
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

    virtual void destroy();

	/** 部件释放
	*/
	virtual void release(void);

    /** 切换英雄
	*/
    virtual void onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore);

    virtual void  restore();

    virtual void  forceEndPK();
    /////////////////////////////////////////TimerHandler/////////////////////////////////////////
    virtual void OnTimer( unsigned long dwTimerID );

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
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    /////////////////////////////////////////IEventVoteSink/////////////////////////////////////////
    /** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  如果返回false : 则中断执行，否则继续向下执行
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    /////////////////////////////////////////__IPlayerKillPart/////////////////////////////////////////
    // 选择目标
    virtual bool chooseTarget(int nTargetFilter, UID uidTarget);

    // 设置PK类型
    virtual void setPKType(int nType);

    // 获取PK类型
    virtual int getPKType(void);

    // 设置决斗目标
    virtual void setFightTarget(UID uidTarget);

    // 检测决斗
    virtual bool checkPK(UID uidTarget);

    // 获取PK状态
    virtual int getPKState(void);

	// 设置PK状态
	virtual void setPKState(int nPKState);

	// 获取自己的阵营
	virtual int getPKCamp();

	/////////////////////////////////////////CPlayerKillPart/////////////////////////////////////////
	/** 构造函数
	*/
	CPlayerKillPart(void);

	/** 析构函数
	*/
	~CPlayerKillPart(void);

private:
    // 客户端消息选择目标
    void onClientMessageChooseTarget(void *pData, size_t nLen);

    // 请求决斗
    void onMessageRequestPK(void *pData, size_t nLen);

    // 客户端消息操作信息
    void onClientMessageHandleInfo(void *pData, size_t nLen);

    // 开始PK
    void onMessageStartPK(void *pData, size_t nLen);

    // 结束PK
    void onMessageEndPK(void *pData, size_t nLen);

    // 取消PK
    void onMessageCancelPK(void *pData, size_t nLen);

    // 创建PK怪物
    bool createPKMonster(Vector3 &vNewLoc);

    // 设置PK信息
    void setPKInfo(UID uidTarget, DWORD dwPKStartTime);

    // 清除PK信息
    void clearPKInfo(void);

    // 检测状态改变
    void checkStateChange(void);

    // 检测战斗范围
    void checkFightDistance(void);

    // 检测战斗时间
    void checkFightTime(void);

    // 检测战斗死亡
    void checkFightDie(void);

    // 检测战斗登出
    void checkFightLogout(void);

    // 设置结束PK
    void setEndPK(UID uidWin, UID uidLost, int nResult, char *szReason);

    // 设置PK结果
    void setPKResult(int nResult, bool bForceEnd = false);

    // 发送请求PK
    void sendRequestPK(const SRequestPKInfo &Info);

    // 增加PKBUFF
    void addPKBuff(bool bStart);

    // 技能是否能作用于怪物上
    bool checkCanCastSpellOnMonster(UID uidTarget);

private:
	// 部件实体
	__IEntity			*m_pEntity;

    // PK类型
    int                 m_nPKType;

    // 决斗目标
    UID                 m_uidTarget;

    // 上次请求时间
    DWORD               m_dwLastRequestTime;

    // 请求PK信息容器
    typedef vector<SRequestPKInfo>  REQUEST_PK_VECTOR;
    REQUEST_PK_VECTOR   m_RequestPKVector;

    // PK状态
    int                 m_nPKState;
    // PK开始时间
    DWORD               m_dwPKStartTime;
    // PK创建怪物(标杆等)
    UID                 m_uidPKMonster;

    // 因为设置阵营会对客户端造成各种困扰，只能自己记录
    // PK阵营记录
    typedef map<UID, int> CAMP_MAP;
    CAMP_MAP            m_CampMap;
};