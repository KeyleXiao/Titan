/*******************************************************************
** 文件名:	MonitorOnline.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-11-22
** 版  本:	1.0
** 描  述:	在线人数监控
            
********************************************************************/

#include "stdafx.h"
#include "ITimerService.h"
#include "ISocialGlobal.h"
#include "IShareServer.h"
#include "IGameDatabaseDef.h"
#include "MonitorOnline.h"

//////////////////////////////////////////////////////////////////////////

CMonitorOnline::CMonitorOnline(void)
    : m_dwLastSaveOnlineTimes(0)
{

}

CMonitorOnline::~CMonitorOnline(void)
{

}


// 创建
bool CMonitorOnline::create()
{
    ITimerService*  pTimerService = gSocialGlobal->getTimerService();
    if(pTimerService)
    {
        // 保存在线处理定时器
        pTimerService->SetTimer(ETIMER_SAVE_ONLINE, SAVE_ONLINE_INTERVAL, CAST_TYPE(ITimerHandler*, this));
    }

    // 数据库接口注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if(pDBEngineService)
    {
        pDBEngineService->registerHandler(this, __FUNCTION__);
    }   

    return true;
}

// 释放
void CMonitorOnline::release()
{
    // 数据库接口反注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if(pDBEngineService)
    {
        pDBEngineService->unregisterHandler(this);
    }    

    // 杀掉定时器
    ITimerService*  pTimerService = gSocialGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->KillTimer(ETIMER_SAVE_ONLINE, CAST_TYPE(ITimerHandler*, this));
    }

}

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
void CMonitorOnline::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    switch(nCmdID)
    {
    case GAMEDB_REQUEST_ONLINENUM_LOG:
        {
            if(nDBRetCode != DBRET_CODE_SUCCEED)
            {
                if(pszDBRetDesc != NULL)
                {
                    ErrorLn("Write Online Player Number To DataBase Execute Failed! Reason:"<< pszDBRetDesc);
                }
                else
                {
                    ErrorLn("Write Online Player Number To DataBase Execute Failed Reason!" );;
                }
            }
        }
        break;

    default: break;
    }
}

////////////////// TimerHandler ////////////////////////////////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CMonitorOnline::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case ETIMER_SAVE_ONLINE:        // 保存在线处理定时器
        {
            onTimerSaveOnline();
        }
        break;

    default: break;
    }
}


//////////////////////////////////////////////////////////////////////////
// 定时保存在线人数
void CMonitorOnline::onTimerSaveOnline(void)
{
    // 当前时间
    DWORD dwNowTime =(DWORD)time(NULL);

    DWORD dwUserCounts = 0;

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver != NULL)
    {
        dwUserCounts = pShareReceiver->GetPersonCount();
    }

    // TODO 跨服在线角色个数
    DWORD dwOtherWorldUserCounts = 0;


    // 在线数据信息
    SOnlineUserCountsInfo addNode;
    addNode.dwTime			= dwNowTime;				// 时间
    addNode.dwUserCounts	= dwUserCounts;				// 本区在线
    addNode.dwOtherCounts	= dwOtherWorldUserCounts;	// 跨服在线角色个数

    m_OnlineUserInfoList.push_back(addNode);	// 加入一个节点数据

    bool bSaveOnline = false;	// 是否要保存
    // 最后保存在线时间为0就存一次
    if (m_dwLastSaveOnlineTimes==0)
    {
        bSaveOnline = true;
    }
    else
    {
        // 现在的时间
        time_t	long_NowTime = (time_t) dwNowTime; 
        tm local_NowTime;
        localtime_s(&local_NowTime,&long_NowTime);

        // 上次的时间
        time_t	long_LastTime = (time_t) m_dwLastSaveOnlineTimes; 
        tm local_LastTime;
        localtime_s(&local_LastTime,&long_LastTime);

        // 分钟不一样就存一下,一分钟存一次
        if (local_NowTime.tm_min!=local_LastTime.tm_min)
        {
            bSaveOnline = true;
        }
    }

    // 要保存
    if (bSaveOnline)
    {
        // 保存的数据 每一分钟用户在线人数统计
        GAMEDB_PARAM_ONLINENUM_LOG dbParam;
        dbParam.nWorldID			= getThisGameWorldID();			// 游戏世界ID
        dbParam.nMonitorTime		= dwNowTime;					// 写入时间

        // 初始化
        dbParam.nOnlineMaxNum		= 0;					// 本区一分钟内最高在线人数
        dbParam.nOnlineAvgNum		= 0;					// 本区一分钟内平均在线人数
        dbParam.nOnlineMinNum		= INT_MAX;		        // 本区一分钟内最小在线人数
        dbParam.nOnlineAreaMaxNum	= 0;					// 跨区一分钟内最高在线人数 
        dbParam.nOnlineAreaAvgNum	= 0;					// 跨区一分钟内平均在线人数 
        dbParam.nOnlineAreaMinNum	= INT_MAX;		        // 跨区一分钟内最小在线人数

        // 计算数据
        DWORD dwRecCounts			= 0;			        // 记录数
        double fTotalOnline			= 0.0;			        // 本区在线人数汇总
        double fTotalAreaOnline		= 0.0;			        // 跨区在线人数汇总
        for (TLIST_OnlineUserCountsInfo::iterator it=m_OnlineUserInfoList.begin();it!=m_OnlineUserInfoList.end();it++)
        {
            dwRecCounts++;
            SOnlineUserCountsInfo &node = (*it);
            fTotalOnline		+= (double)node.dwUserCounts;
            fTotalAreaOnline	+= (double)node.dwOtherCounts;
            if (node.dwUserCounts > (DWORD)dbParam.nOnlineMaxNum)
            {
                dbParam.nOnlineMaxNum = (int)node.dwUserCounts;
            }
            if (node.dwUserCounts < (DWORD)dbParam.nOnlineMinNum)
            {
                dbParam.nOnlineMinNum = (int)node.dwUserCounts;
            }

            if (node.dwOtherCounts > (DWORD)dbParam.nOnlineAreaMaxNum)
            {
                dbParam.nOnlineAreaMaxNum = (int)node.dwOtherCounts;
            }
            if (node.dwOtherCounts < (DWORD)dbParam.nOnlineAreaMinNum)
            {
                dbParam.nOnlineAreaMinNum = (int)node.dwOtherCounts;
            }
        }

        // 计算平均
        if (dwRecCounts>0)
        {
            dbParam.nOnlineAvgNum		= (int)(fTotalOnline/(double)dwRecCounts);					// 本区一分钟内平均在线人数
            dbParam.nOnlineAreaAvgNum	= (int)(fTotalAreaOnline/(double)dwRecCounts);				// 跨区一分钟内平均在线人数 
        }

        // 调整一下,最小无效情况
        if (dbParam.nOnlineMinNum>dbParam.nOnlineMaxNum)
        {
            dbParam.nOnlineMinNum = dbParam.nOnlineMaxNum;
        }
        if (dbParam.nOnlineAreaMinNum>dbParam.nOnlineAreaMaxNum)
        {
            dbParam.nOnlineAreaMinNum = dbParam.nOnlineAreaMaxNum;
        }


        IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
        if (pDBEngine)
        {
            if (pDBEngine->executeSP(GAMEDB_REQUEST_ONLINENUM_LOG,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this) ,dbParam.nWorldID) == false)
            {
                ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_ONLINENUM_LOG failed!");
            }
        }


        static DWORD dwLastTraceTimes = 0;	// 最后显示时间
        // 10分钟显示一次
        if (dwLastTraceTimes>=10)
        {
            dwLastTraceTimes = 0;
        }
        if (dwLastTraceTimes==0)
        {
            EmphasisLn("Online Num Save DB, thisArea=:"<<dwUserCounts<<",other Area=:"<<dwOtherWorldUserCounts<<" (1 minute to save time, 10 minutes tip again! )");
            EmphasisLn("thisArea[MAX="<<dbParam.nOnlineMaxNum<<",MIN="<<dbParam.nOnlineMinNum<<",AVG="<<dbParam.nOnlineAvgNum<<"] OtherArea[MAX="<<dbParam.nOnlineAreaMaxNum<<",MIN="<<dbParam.nOnlineAreaMinNum<<",AVG="<<dbParam.nOnlineAreaAvgNum<<"]");
        }
        dwLastTraceTimes++;

        // 清空数据
        m_OnlineUserInfoList.clear();
        m_dwLastSaveOnlineTimes = dwNowTime;	// 最后保存在线时间
    }
}