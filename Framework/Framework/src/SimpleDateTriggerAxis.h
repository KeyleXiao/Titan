/*******************************************************************
** 文件名:	SimpleDateTriggerAxis.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	触发器时间轴

			
********************************************************************/

#pragma once

#include "IDateTriggerService.h"
#include "IFramework.h"
#include "ITimerService.h"

/**
    触发器时间轴
*/

// 回调接口数据
struct SDateHander
{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int wday;					
    IDateHandler* pDateHander;	// 回调接口
    int nTriggerID;				// 触发器ID
    DWORD m_dwLastCheck;		// 上一次触发的时间

    SDateHander()
    {
        memset(this, 0, sizeof(*this));
    }
};

class SimpleDateTriggerAxis : public IDateTriggerService
{
private:
    list<SDateHander> m_DateHanderList;						// 记录

    list<SDateHander*> m_NotHandleList;                      // 没有触发的数据 防止相同的时间点触发数量级过多

public:
		bool Create();

		bool Close();

		SimpleDateTriggerAxis()
        {

        }

		virtual ~SimpleDateTriggerAxis()
		{
			Close();
		}

        /// purpose: 注册一个触发时刻点
        
        /// param  : nTriggerID   触发器ID
        /// param  : pDateHander   回调接口
        //  @param  :  SDateTriggerInfo
        ///             year  年，为-1表示每年都触发
        ///             month 月，为-1表示每月都触发
        ///             day   日，为-1表示每天都触发
        ///             hour  小时
        ///             min   分钟
        ///             wday  星期几，为-1表示不判断
        /// @note  :如果触发后不需要再触发了请调用UnRegisterDateTrigger删除触发器
        virtual bool RegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander, SDateTriggerInfo sTriggerInfo, const char * debugInfo=0 );
        

        /// purpose: 消耗一个触发时刻点
        /// param  : nTriggerID   触发器ID
        /// param  : pDateHander   回调接口
        virtual bool UnRegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander);

		/**
		@purpose          : 检查定时器是否需要触发
		@param timeout    : 单次执行时间片
		*/
		virtual void CheckTimer(ulong timeout = 0);


		
};