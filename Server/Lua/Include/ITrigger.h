/*******************************************************************
** 文件名:	ITrigger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	触发器接口
********************************************************************/

#ifndef __INTERFACE_TRIGGER_H__
#define __INTERFACE_TRIGGER_H__

/// 触发器ID
typedef int TRIGGER_ID;

/// 无效触发器ID
#define INVALID_TRIGGER_ID    0

/**
@name : 触发器接口
@brief: 触发器是为游戏核心引擎与lua脚本之间进行事件传递而设计的一种对象
        这类对象订阅游戏引擎中的各类事件,当事件触发并满足指定的条件后回调lua的脚本
		以建立脚本系统的事件机制.
		In our games,The Trigger is designed for events notification from core game engine to peripheral LUA script.
		Such objects usually subscribe various events in game system.
		While event is firing and appointed conditions is matched,it will call functions in LUA scripts.  
		Then,The event system for script is set up.
*/
enum
{
    ETriggerType_Timer = 0,								// 定时触发器:TimerTrigger
    ETriggerType_Date,									// 日期触发器:DateTrigger
	ETriggerType_Login,								    // 登陆触发器:LoginTrigger
	ETriggerType_WarPersonTask,							// 战场人物任务数据触发器:WarPersonTaskTrigger
	ETriggerType_NPC,									// NPC触发器:NPCTrigger
	ETriggerType_NPCFunc,								// NPC触发器:NPCTriggerFuncTrigger
	ETriggerType_ActorTask,								// 角色数据任务触发器:ActorTaskTrigger
	
	ETriggerType_Max,
};

struct ITrigger
{
	/**
	@name         : 安装触发器
	@brief        : 触发器需要的参数例如回调函数名,触发条件等通过构造函数传递
	@return       : 
	*/
	virtual bool Setup() = 0;

	/**
	@name         : 设置触发器ID
	@param   id   : 
	*/
	virtual void SetID(int nID) = 0;

	/**
	@name         : 取得触发器ID
	*/
	virtual int GetID() const = 0;

	/**
	@name         : 销毁触发器
	@brief        : 
	@return       : 
	*/
	virtual void Release() = 0;

	/** 获得触发器类型，方便统计
	@param   
	@param   
	@return  
	*/
	virtual int GetTriggerType(void) = 0;

	/** 获得触发器回调接口，方便统计
	@param   
	@param   
	@return  
	*/
	virtual std::string GetTriggerCallBack(void) = 0;

    /**
    @name         : 取得触发器场景ID
    @return       : 取得触发器场景ID
	*/
	virtual int GetSceneID() = 0;
};
#endif//__INTERFACE_TRIGGER_H__