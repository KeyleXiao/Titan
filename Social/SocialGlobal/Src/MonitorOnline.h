/*******************************************************************
** 文件名:	MonitorOnline.h  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-11-22
** 版  本:	1.0
** 描  述:	在线人数监控
            
********************************************************************/

#pragma once;

#include "IDBEngineService.h"


// 在线人数监控
class CMonitorOnline : public IDBRetSink, public TimerHandler
{

    enum
    {
        ETIMER_SAVE_ONLINE,     		        // 保存在线处理定时器
    };
    enum
    {
        SAVE_ONLINE_INTERVAL        = 60000,     // 每分钟保存在线人数
    };


    // 在线数据信息
    struct SOnlineUserCountsInfo
    {
        DWORD dwTime;				// 时间
        DWORD dwUserCounts;			// 本区在线
        DWORD dwOtherCounts;		// 跨服在线角色个数

        SOnlineUserCountsInfo(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 在线数据列表
    typedef list<SOnlineUserCountsInfo>	TLIST_OnlineUserCountsInfo;

public:

    // 构造函数
    CMonitorOnline(void);

    // 析构函数
    ~CMonitorOnline(void);

    // 创建
    bool create(void);

    // 释放
    void release(void);

	///////////////////////////////// IDBRetSink ////////////////////////////
	/** 数据库请求返回回调方法
	@param   pDBRetSink		：结果返回接口	
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

    ////////////////// TimerHandler ////////////////////////////////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void        OnTimer( unsigned long dwTimerID );



private:
    // 定时保存在线人数
    void onTimerSaveOnline(void);

private:
    DWORD						    m_dwLastSaveOnlineTimes;	// 最后保存在线时间
    TLIST_OnlineUserCountsInfo      m_OnlineUserInfoList;		// 在线数据列表
};