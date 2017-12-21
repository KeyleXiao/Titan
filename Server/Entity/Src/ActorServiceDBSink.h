/*******************************************************************
** 文件名: ActorServiceDBSink.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 4/27/2017
** 版  本: 1.0
** 描  述: ActorService数据库处理

********************************************************************/

#pragma once

#include "IDBEngineService.h"

class ActorService;

// 人物上下线日记
class CActorServiceDBSink : public IDBRetSink
{
public:
    /** 构造函数
    */
    CActorServiceDBSink(void);

    /** 析构函数
    */
    ~CActorServiceDBSink(void);

    /** 创建
    */
    bool on_start( ActorService* pActorService);

    /** 释放
    */
    void on_stop();

    /** 从数据中加载角色数据
    */
    bool loadActorDataFromDataBase(DWORD dwUser, PDBID dbid);

    /** 玩家登陆设置锁标识
    */
    bool setActorLoginLockFlag(DWORD dwUser, PDBID dbid);

    ///////////////////////////////// IDBRetSink ////////////////////////////
    /** 数据库请求返回回调方法
    @param   pDBRetSink     ：结果返回接口 
    @param   nCmdID ：命令ＩＤ
    @param   nDBRetCode：数据库请求返回值，参考上面定义
    @param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
    @param   nQueueIndex：队列定义
    @param   pOutData：输出数据
    @param   nOutLen：输出数据长度
    @return  
    @note     
    @warning 此对像千万不能在数据库返回前释放，否则会非法！
    @retval buffer 
    */  
    virtual void        OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:

    void onReturnSetLockFlag( int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
private:
    ActorService *      m_pActorService;

};