/*******************************************************************
** 文件名:	Pupil.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/8/2016
** 版  本:	1.0
** 描  述:	防沉迷-未成年

********************************************************************/

#pragma once

#include "TimerHandler.h"
#include "AntiAddictDef.h"
#include "lockfree/hashmap.h"

#define SHOW_ANTI_ADDICT_LOG

// 防沉迷-未成年对象
class CPupil : public TimerHandler
{
    enum
    {
        TIP_ACTOR_TIMER,        // 提醒定时器
        KICK_ACTOR_TIMER,       // 踢人定时器
    };

public:
	/** 构造函数
	*/
    CPupil(void);

	/** 析构函数
	*/
    ~CPupil(void);

	/** 创建
	*/
    virtual bool create(SUserAddictInfo* pAddictInfo);

	/** 释放
	*/
    virtual void release(void);

	/** 取得玩家的衰减比例
    @ return : 返回衰减比例（x/100）,即返回，表示衰减一半
	*/
    virtual int getReduceRate(void);

	/** 加入角色
    @ return
	*/
    bool  addActor(PDBID pdbid, UID uid);

	/** 移除角色
    @ return
	*/
    bool  removeActor(PDBID dwPDBID, UID uid);

	/** 没有角色数据
    @ return
	*/
    bool  empty();

	/** 加入被踢角色
    @ return
	*/
    void  addKickActor(PDBID pdbid);

	/** 移除被踢角色
    @ return
	*/
    void  removeKickActor(PDBID pdbid);

	/** 更新在线账号下的角色为成年人状态
    @ return
	*/
    void  setAdult();    

	/** 更新在线账号下的角色身份证号
    @ return
	*/
    void  updateIdCard(DWORD dwIdCard);   

	/** 更新
	*/
    void update(SUserAddictInfo* pAddictInfo);

	/** 获取IdCard
	*/
    DWORD getIdCard();

	/** 更新在线账号下的角色实名认证状态
    @ return
	*/
    void  setRealNameAuther();

    ////////////////////////TimerHandler//////////////////////////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void OnTimer( unsigned long dwTimerID );


private:
    // 对当前有关的帐号进行广播消息
    void broadcastUsers(const char* szMessage);

    // 将通行证认证信息通知登陆服
    void notifyLoginServerForPassport(DWORD clientID);

private:
    // 人物UID列表
    typedef list<UID>   UID_LIST;
    typedef DWORD       IDCARD;
	typedef speed::hashmap<PDBID, UID>      MAP_ACTOR;
	typedef speed::hashmap<PDBID, DWORD>    MAP_KICK;

    // 所有的用户对象
    MAP_ACTOR		m_mapActorList;
    // 被踢队列
    MAP_KICK		m_mapKickList;
    
    SUserAddictInfo     m_AddictInfo;		// 防沉迷信息
    EAddictState        m_nState;			// 当前状态

    DWORD			    m_dwIdCard;			// 玩家身份证标识
    int                 m_nOneHourTips;		// 1小时是否已经提示
};