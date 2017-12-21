/*******************************************************************
** 文件名:	IDateTriggerService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:  秦其勇
** 日  期:	18/10/2014
** 版  本:	1.0
** 描  述:	日期触发器服务接口

			
********************************************************************/

#pragma once


using namespace rkt;
/**
	将时间轴服务再包装一边，以使得在各个线程都能安全调用
*/

struct IDateHandler;
struct SDateTriggerInfo;

struct IDateTriggerService
{
        /// purpose: 注册一个触发时刻点
        /// param  : nTriggerID   触发器ID
        /// param  : pDateHander   回调接口
        /// param  : year  年，为-1表示每年都触发
        /// param  : month 月，为-1表示每月都触发
        /// param  : day   日，为-1表示每天都触发
        /// param  : hour  小时
        /// param  : min   分钟
        /// param  : wday  星期几，为-1表示不判断
        /// @note  :如果触发后不需要再触发了请调用UnRegisterDateTrigger删除触发器
        virtual bool RegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander, SDateTriggerInfo sTriggerInfo, const char * debugInfo=0 ) = 0;

        /// purpose: 消耗一个触发时刻点
        /// param  : nTriggerID   触发器ID
        /// param  : pDateHander   回调接口
        virtual bool UnRegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander) = 0;


		/**
		@purpose          : 检查定时器是否需要触发
		@param timeout    : 单次执行时间片
		*/
		virtual void CheckTimer(ulong timeout = 0) = 0;

};
