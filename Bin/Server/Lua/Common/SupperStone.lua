-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	12/22/2016
-- 版  本:	1.0
-- 描  述:	七龙珠帮助函数
-------------------------------------------------------------------

-------------------------------------------------------------------
-- 七龙珠接受任务
-- @param holder_id   : 玩家ID
-- @param task_id	  : 任务ID
-------------------------------------------------------------------
function SupperStone_AcceptTask(holder_id, task_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return
	end

	if task_id == nil or task_id <= 0 then
		return
	end
	
    local task = TASK_LIST[holder_id][task_id]
	if task ~= nil then
		return
	end

	local oTask = TaskFactory:CreateTask(holder_id, task_id)
	if oTask ~= nil then

		-- 接受任务
		oTask:Accept()

		TASK_LIST[holder_id][task_id] = oTask

		--接受了任务,触发事件
		OnActorTaskListChanged(holder_id, task_id, "accept")
	end
end

-------------------------------------------------------------------
-- 七龙珠完成任务
-- @param holder_id   : 玩家ID
-- @param task_id	  : 任务ID
-------------------------------------------------------------------
function SupperStone_FinishTask(holder_id, task_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return
	end

	if task_id == nil or task_id <= 0 then
		return
	end
	
    local oTask = TASK_LIST[holder_id][task_id]
	if oTask == nil then
		return
	end
	
	if oTask.type == TaskType.FirstWinType then
	
		-- 设置完成状态数据
		oTask:SetState(TaskState.FinishState)
		-- 完成2态
		local task_key = GetWinCountTaskKey(holder_id, task_id)
		SetTaskIntData(holder_id, task_key, 2)
		-- 注销玩DOTA游戏事件
		oTask:UnregisterPlayDotaGame()
	else
	
		oTask:Finish()
	end

	-- 触发事件
	OnActorTaskListChanged(holder_id, task_id, "accomplish")
end

-------------------------------------------------------------------
-- 七龙珠删除任务
-- @param holder_id   : 玩家ID
-- @param task_id	  : 任务ID
-------------------------------------------------------------------
function SupperStone_DeleteTask(holder_id, task_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return
	end
	
	if task_id == nil then
		return
	end
	
	if task_id == 0 then
		SupperStone_DeleteAllTask(holder_id)
	else
		SupperStone_DeleteSingleTask(holder_id, task_id)
	end
end

-- 删除单个任务
function SupperStone_DeleteSingleTask(holder_id, task_id)

    local task = TASK_LIST[holder_id][task_id]
	if task ~= nil then

		if task:DeleteTask() == false then
			return
		end

		--放弃了任务,触发事件
		OnActorTaskListChanged(holder_id, task_id, "abort")
		
		TaskFactory:DeleteTask(task)
		TASK_LIST[holder_id][task_id] = nil

		-- 真正删除了任务 
		OnActorTaskListChanged(holder_id, task_id, "delete")
	end
	
	local task_key = GetTaskIDKey(holder_id, task_id)
	RemoveTaskData(holder_id, task_key)
end

-- 删除所有任务
function SupperStone_DeleteAllTask(holder_id)
	
	for task_id,task in pairs(TaskFactory.tTaskList) do
		SupperStone_DeleteSingleTask(holder_id, task_id)
	end
end

-------------------------------------------------------------------
-- 七龙珠移除任务日,周次数
-- @param holder_id   : 玩家ID
-- @param task_id	  : 任务ID
-------------------------------------------------------------------
function SupperStone_RemoveTaskDayAndWeekCount(holder_id, task_id)

	if holder_id == nil then
		return
	end

	if task_id == nil or task_id < 0 then
		return
	end

	if task_id == 0 then
		for task_id,task in pairs(TaskFactory.tTaskList) do

			-- 清除任务数据，这样可以重复接
			local task_key = GetTaskIDKey(holder_id, task_id)
			RemoveTaskData(holder_id, task_key)
			local day_time_key = GetDayTimeTaskKey(holder_id, task_id)
			RemoveTaskData(holder_id, day_time_key)
			local day_count_key = GetDayCountTaskKey(holder_id, task_id)
			RemoveTaskData(holder_id, day_count_key)
			local week_time_key = GetWeekTimeTaskKey(holder_id, task_id)
			RemoveTaskData(holder_id, week_time_key)
			local week_count_key = GetWeekCountTaskKey(holder_id, task_id)
			RemoveTaskData(holder_id, week_count_key)
		end
	else
		local task = TaskFactory.tTaskList[task_id]
		if task == nil then
			return
		end

		-- 清除任务数据，这样可以重复接
		local task_key = GetTaskIDKey(holder_id, task_id)
		RemoveTaskData(holder_id, task_key)
		local day_time_key = GetDayTimeTaskKey(holder_id, task_id)
		RemoveTaskData(holder_id, day_time_key)
		local day_count_key = GetDayCountTaskKey(holder_id, task_id)
		RemoveTaskData(holder_id, day_count_key)
		local week_time_key = GetWeekTimeTaskKey(holder_id, task_id)
		RemoveTaskData(holder_id, week_time_key)
		local week_count_key = GetWeekCountTaskKey(holder_id, task_id)
		RemoveTaskData(holder_id, week_count_key)
	end
end

-------------------------------------------------------------------
-- 七龙珠设置任务数据
-- @param holder_id   : 玩家ID
-- @param task_id	  : 任务ID
-------------------------------------------------------------------
function SupperStone_SetTaskData(holder_id, task_data_id, task_data_value)

	if holder_id == nil then
		return
	end

	SetTaskIntData(holder_id, task_data_id, task_data_value)
end

-------------------------------------------------------------------
-- 七龙珠刷新任务操作次数
-- @param holder_id			: 玩家ID
-- @param catch_count		: 搜索次数
-- @param publish_count		: 发布次数
-------------------------------------------------------------------
function SupperStone_ReflushTaskOptCount(holder_id, catch_count, publish_count)

	if holder_id == nil or catch_count == nil or publish_count == nil then
		return
	end

	-- 刷新
	local catch_count_key = GetTaskCatchCountKey(holder_id)
	local catch_count_value = GetTaskIntData(holder_id, catch_count_key)
	if catch_count_value == TASK_DATA_INVALID then
		catch_count_value = 0
	end
	catch_count_value = catch_count_value + catch_count
	SetTaskIntData(holder_id, catch_count_key, catch_count_value)
	
	
	local publish_count_key = GetContactPublishCountKey(holder_id)
	local publish_count_value = GetTaskIntData(holder_id, publish_count_key)
	if publish_count_value == TASK_DATA_INVALID then
		publish_count_value = 0
	end
	publish_count_value = publish_count_value + publish_count
	SetTaskIntData(holder_id, publish_count_key, publish_count_value)
	
	-- 下发 
	local strOptInfo = GetActorReflushOptInfo(holder_id)
	if strOptInfo ~= nil then
		SendReflushOptInfo(holder_id, strOptInfo)
	end

end
