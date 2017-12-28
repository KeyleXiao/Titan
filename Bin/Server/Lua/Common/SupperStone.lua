-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	12/22/2016
-- ��  ��:	1.0
-- ��  ��:	�������������
-------------------------------------------------------------------

-------------------------------------------------------------------
-- �������������
-- @param holder_id   : ���ID
-- @param task_id	  : ����ID
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

		-- ��������
		oTask:Accept()

		TASK_LIST[holder_id][task_id] = oTask

		--����������,�����¼�
		OnActorTaskListChanged(holder_id, task_id, "accept")
	end
end

-------------------------------------------------------------------
-- �������������
-- @param holder_id   : ���ID
-- @param task_id	  : ����ID
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
	
		-- �������״̬����
		oTask:SetState(TaskState.FinishState)
		-- ���2̬
		local task_key = GetWinCountTaskKey(holder_id, task_id)
		SetTaskIntData(holder_id, task_key, 2)
		-- ע����DOTA��Ϸ�¼�
		oTask:UnregisterPlayDotaGame()
	else
	
		oTask:Finish()
	end

	-- �����¼�
	OnActorTaskListChanged(holder_id, task_id, "accomplish")
end

-------------------------------------------------------------------
-- ������ɾ������
-- @param holder_id   : ���ID
-- @param task_id	  : ����ID
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

-- ɾ����������
function SupperStone_DeleteSingleTask(holder_id, task_id)

    local task = TASK_LIST[holder_id][task_id]
	if task ~= nil then

		if task:DeleteTask() == false then
			return
		end

		--����������,�����¼�
		OnActorTaskListChanged(holder_id, task_id, "abort")
		
		TaskFactory:DeleteTask(task)
		TASK_LIST[holder_id][task_id] = nil

		-- ����ɾ�������� 
		OnActorTaskListChanged(holder_id, task_id, "delete")
	end
	
	local task_key = GetTaskIDKey(holder_id, task_id)
	RemoveTaskData(holder_id, task_key)
end

-- ɾ����������
function SupperStone_DeleteAllTask(holder_id)
	
	for task_id,task in pairs(TaskFactory.tTaskList) do
		SupperStone_DeleteSingleTask(holder_id, task_id)
	end
end

-------------------------------------------------------------------
-- �������Ƴ�������,�ܴ���
-- @param holder_id   : ���ID
-- @param task_id	  : ����ID
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

			-- ����������ݣ����������ظ���
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

		-- ����������ݣ����������ظ���
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
-- ������������������
-- @param holder_id   : ���ID
-- @param task_id	  : ����ID
-------------------------------------------------------------------
function SupperStone_SetTaskData(holder_id, task_data_id, task_data_value)

	if holder_id == nil then
		return
	end

	SetTaskIntData(holder_id, task_data_id, task_data_value)
end

-------------------------------------------------------------------
-- ������ˢ�������������
-- @param holder_id			: ���ID
-- @param catch_count		: ��������
-- @param publish_count		: ��������
-------------------------------------------------------------------
function SupperStone_ReflushTaskOptCount(holder_id, catch_count, publish_count)

	if holder_id == nil or catch_count == nil or publish_count == nil then
		return
	end

	-- ˢ��
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
	
	-- �·� 
	local strOptInfo = GetActorReflushOptInfo(holder_id)
	if strOptInfo ~= nil then
		SendReflushOptInfo(holder_id, strOptInfo)
	end

end
