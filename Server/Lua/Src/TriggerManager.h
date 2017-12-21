/*******************************************************************
** 文件名:	TriggerManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	触发器管理类
********************************************************************/

#ifndef __TRIGGER_MANAGER_H__
#define __TRIGGER_MANAGER_H__

#include "ITrigger.h"
#include "Singleton.h"
#include <map>

/**
@name : 管理所有的Trigger的创建和销毁
@brief: 
*/
class TriggerManager : public Singleton<TriggerManager>
{
public:
    TriggerManager();
    ~TriggerManager();

public:
    /**
	@name            : 添加Trigger 
	@brief           : 
	@param pTrigger  : 触发器指针 
	@return          : 返回触发器ID
	*/
	TRIGGER_ID addTrigger(ITrigger *pTrigger);

	/**
	@name            : 删除触发器 
	@brief           : 
	@param   id      : 触发器ID 
	@return          : 如果成功删除返回true
	*/
	bool deleteTrigger(TRIGGER_ID id);

    /**
	@name            : 查找触发器 
	@brief           : 
	@param   id      : 触发器ID 
	@return          : 如果成功返回触发器指针
	*/
    ITrigger *findTrigger(TRIGGER_ID id);

    /** 
    @name            : 输出触发器信息
	*/
	void outputStatTriggerInfo(void);

	/**
	@name            : 创建定时触发器
	@brief           : 
    @param nSceneID  : 场景ID
	@param szCallFunc: 回调函数名
	@param nInterval : 定时事件
	@param nTimes    : 触发次数
	@return          : 返回新创建的触发器ID
	*/
	int createTimerTrigger(int nSceneID, const char *szCallFunc, int nInterval, int nTimes);

    /**
    @name            : 创建日期触发器
    @brief           : 到某年某月某日某时星期几触发某个事件
    @param nSceneID  : 场景ID
    @param szCallFunc: 回调函数名
    @param year      : 某年，如果传入-1表示每年
    @param month     : 某月，如果传入-1表示每月
    @param day       : 某日，如果传入-1表示每日
    @param hour      : 某时，如果传入-1表示每小时
    @param minute    : 某分，如果传入-1表示不判断
    @param wday      : 星期几，如果传入-1表示不判断
    @return          : 返回新创建的触发器ID,如果触发后不需要再触发了请调用deleteTrigger删除触发器
    */
    int createDateTrigger(int nSceneID, const char *szCallFunc, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nWDay);

	/**
	@name            : 创建登陆触发器 
	@brief           : 
    @param nSceneID  : 场景ID
	@param szCallFunc: 回调函数名
	@return          : 返回新创建的触发器ID
	*/
	int createLoginTrigger(int nSceneID, const char *szCallFunc);

    /**
	@name            : 创建战场人物任务数据触发器 
	@brief           : 
    @param nSceneID  : 场景ID
	@param szCallFunc: 回调函数名
	@return          : 返回新创建的触发器ID
	*/
	int createWarPersonTaskTrigger(int nSceneID, const char *szCallFunc);

	/**
	@name            : 创建NPC触发器 
	@brief           : 
    @param nSceneID  : 场景ID
	@param szCallFunc: 回调函数名
	@return          : 返回新创建的触发器ID
	*/
	int createNPCTrigger(int nSceneID, const char *szCallFunc);

	/**
	@name            : 创建NPC功能触发器 
	@brief           : 
    @param nSceneID  : 场景ID
	@param szCallFunc: 回调函数名
	@return          : 返回新创建的触发器ID
	*/
	int createNPCFuncTrigger(int nSceneID, const char *szCallFunc);

	 /**
	@name            : 创建角色任务数据触发器 
	@brief           : 
    @param nSceneID  : 场景ID
	@param szCallFunc: 回调函数名
	@return          : 返回新创建的触发器ID
	*/
	int createActorTaskTrigger(int nSceneID, const char *szCallFunc);

private:
	typedef std::map<TRIGGER_ID,ITrigger *>     TRIGGER_MAP;
	TRIGGER_MAP                                 m_triggerMap;

    volatile LONG           m_mutex;
};

#endif//__TRIGGER_MANAGER_H__