/*******************************************************************
** 文件名:	IAITask.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	
** 版  本:	1.0
** 描  述:  AI任务接口
** 应  用:  	
********************************************************************/

#pragma once
#include "Vector3.h"

// AI任务状态
enum AITaskState
{
	ATS_NONE,         // 无
	ATS_RUNNING,	  // 正在执行
	ATS_SUSPEND,      // 暂停
	ATS_FINISH,		  // 完成
};

// AI任务类型
enum AITaskType
{
	ATT_SIM,					// 模拟真人玩家 出门
	ATT_FORWARD_LINE,			// 推线
	ATT_HELP_OTHER_LINE,		// 支援其他线任务
	ATT_FOLLOW,					// 跟随
	ATT_BACK,					// 回城
	ATT_HUNTING,				// 打野
	ATT_MAX,
};

#define TASK_PRIORITY_HIGHEST 100

// AI任务接口
struct IAITask
{
	// 执行
	virtual bool run(AITaskState& oriState) = 0;

	// 暂停
	virtual bool suspend(AITaskState& oriState) = 0;

	// 完成
	virtual void finish() = 0;

	// 当前状态
	virtual AITaskState state() = 0;

	// 获取任务地点
	virtual Vector3 getLoc() = 0;

	// 获取任务类型
	virtual AITaskType getType() = 0;

	// 获取优先级
	virtual int getPriority() = 0;

	// 更新攻击掩码
	virtual void updateAttackMask(int nAttackMask) = 0;

	// 更新优先级
	virtual void updatePriority(int nPriority) = 0;

	// 更新超时时间
	virtual void updateTimeOut(int nTimeOut) = 0;

    // 更新context
    virtual void updateContext(void* context) = 0;

    virtual std::string& log(const char* szLog) = 0;

	// 释放
	virtual void release() = 0;
};