//==========================================================================
/**
* @file	  : IDateTriggerManager.h
* @author : 秦其勇
* created : 2016/9/2
* purpose : 日期型触发器（精确度 1分钟）
*/
//==========================================================================

#ifndef __IDateTriggerManager_H__
#define __IDateTriggerManager_H__

// 日期型时刻触发器回调接口
struct IDateHander
{
	/// purpose: 日期达到
	virtual void DateCome(int nTriggerID) = NULL;
};

// 日期型触发器接口
struct IDateTriggerManager
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
	virtual void RegisterDateTrigger(int nTriggerID, IDateHander* pDateHander, int year, int month, int day, int hour, int min, int wday) = 0;

	/// purpose: 消耗一个触发时刻点
	/// param  : nTriggerID   触发器ID
	/// param  : pDateHander   回调接口
	virtual void UnRegisterDateTrigger(int nTriggerID, IDateHander* pDateHander) = 0;

	/// purpose: 清除日期定时器半小时冷却
	virtual void ClearAllCool() = 0;

    /// purpose: 加载
    virtual bool load() = 0;

    /// purpose: 释放
    virtual void release() = 0;
};

#endif // __IDateTriggerManager_H__