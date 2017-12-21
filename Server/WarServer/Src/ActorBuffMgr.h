/*******************************************************************
** 文件名: ActorBuffMgr.h 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 6/22/2017
** 版  本: 1.0
** 描  述: 人物增加BUFF收集管理

********************************************************************/

#pragma once

#include <map>
using namespace std;

struct __IWarPart;

// 人物增加BUFF收集管理
class CActorBuffMgr : public IEventExecuteSink, public TimerHandler
{
    enum
    {
        NOTIFY_FLUSH_BUFFLIST_TIMER,        // 通知战场刷新BUFF列表
        TIMER_INTERVAL = 500,               // 500ms刷新一下
    };
public:
    /** 构造函数
    */
    CActorBuffMgr(void);

    /** 析构函数
    */
    ~CActorBuffMgr(void);

    /** 创建
    */
    bool onLoad( __IWarPart* pWarPart);

    /** 重置
    */
    void restore();

    /** 释放
    */
    void release();

    ///////////////////////////////// IEventExecuteSink ////////////////////////////
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
    virtual void        OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ///////////////////////////////// TimerHandler ////////////////////////////
    /**
    @purpose          : 定时器触发后回调,你可以在这里编写处理代码
    @param   dwTimerID: 定时器ID,用于区分是哪个定时器
    @return           : empty
    */
    virtual void        OnTimer( unsigned long dwTimerID );

private:
    __IEntity *		    getHost();

    /** 角色增加BUFF事件
    */
    void                onEventActorAddBuff(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    /** 通知战场刷新BUFF列表
    */
    void                onTimerFlushBuffList(void);

private:
    __IWarPart *      m_pWarPart;

    map<UID, DWORD>      m_mapBuffList;      // 给自己增加BUFF列表
};