-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	12/3/2016
-- ��  ��:	1.0
-- ��  ��:	�����б�ά��
-------------------------------------------------------------------

require "TaskList"
require "TaskFactory"
require "TaskData"
require "TaskDef"

-- ����������¶�ʱ��
g_nTaskListUpdateTimer = 0

-- ����״̬�仯���ӳ�ͬ���ͻ��������б�
TASK_LIST_UPDATE_CACHE = {}

-- ��������ͼ��
TASK_BINDINGGOLD_PRIZEID = 1518
-- �����齱��ͼ��
TASK_EXP_PRIZEID = 1519

-- ����ˢ�²������¶�ʱ��
g_nTaskReFlushOptTimer = 0
g_bInitReflushTimer = true

-------------------------------------------------------------------
-- ��ʼ�������б������
-- @note  : ��ģ��������������б�ͬ�������ͻ���
-------------------------------------------------------------------
function InitTaskListManager()

	-- ���ĵ�½�¼�
	g_luaEventPublisher:Subscribe(LUA_EVENT_ACTOR_LOGIN, TaskListManager_LoginEventHandler)

	-- ��������仯�¼�
    RegisterTaskListChangeEvent(0, 0, TaskListManager_TaskEventHandler, 0)

	local nSceneID = GetLuaSceneID()
	-- �����б���Ƶ�������ͻ��ˣ���Ҫ����һ�£����������ʱ��
    g_nTaskListUpdateTimer = CreateTimerTrigger(nSceneID, "TaskListManager_TimerHandler", 1000, 0)
	
	-- ���ĳ���ж���¼�
	g_luaEventPublisher:Subscribe(LUA_EVENT_SCENE_UNLOAD, TaskListManager_SceneUnloadEventHandler)
	
	-- �������񷢲�����ȡ���ܸ��¶�ʱ��
	SetActorOptReflushTimer()
end

-------------------------------------------------------------------
-- ����ж���¼�����
-------------------------------------------------------------------
function TaskListManager_SceneUnloadEventHandler()

	if g_nTaskListUpdateTimer ~= 0 and DeleteTrigger(g_nTaskListUpdateTimer) == false then
		error("Delete TaskTimerTrigger faild TriggerID="..g_nTaskListUpdateTimer)
	end
	
	if g_LoginTriggerID ~= 0 and DeleteTrigger(g_LoginTriggerID) == false then
		error("Delete LoginTrigger faild TriggerID="..g_LoginTriggerID)
	end
	
	if g_WarPersonTaskTrigger ~= 0 and DeleteTrigger(g_WarPersonTaskTrigger) == false then
		error("Delete WarPersonTaskTrigger faild TriggerID="..g_WarPersonTaskTrigger)
	end
	
	if g_ActorDataTaskTrigger ~= 0 and DeleteTrigger(g_ActorDataTaskTrigger) == false then
		error("Delete ActorDataTaskTrigger faild TriggerID="..g_ActorDataTaskTrigger)
	end
	
	if g_nTaskReFlushOptTimer ~= 0 and DeleteTrigger(g_nTaskReFlushOptTimer) == false then
		error("Delete g_nTaskReFlushOptTimer faild TriggerID="..g_nTaskReFlushOptTimer)
	end
	
end

-------------------------------------------------------------------
-- �������ݷ����˱仯
-- ��ӵ������б��У��Ȼ��ٷ�
-------------------------------------------------------------------
function TaskListManager_TaskEventHandler(holder_id,task_id,action)

	-- �Ż����Ʊ�ʶ��true=�����Ż�,false=�ر��Ż�
	if g_UserTaskOptimizeOpen then
		-- һЩ�¼���Ҫ�ı䱸������
		if action == 'accept' then

			-- �������������Ϊnil
			if g_tUserHaveTaskIDList[holder_id] == nil then
				g_tUserHaveTaskIDList[holder_id] = {}
			end
			
			-- ����޴�����
			if not g_tUserHaveTaskIDList[holder_id][task_id] then
				-- �������ӵ�д�����
				g_tUserHaveTaskIDList[holder_id][task_id] = true
				-- ������㲥��־
				g_tUserHaveTaskIDListChange[holder_id] = true
			end
		elseif action == 'delete' or action == 'complete' then

			-- �������������Ϊnil
			if g_tUserHaveTaskIDList[holder_id] == nil then
				g_tUserHaveTaskIDList[holder_id] = {}
			end

			-- ���ӵ������
			if g_tUserHaveTaskIDList[holder_id][task_id] then
				-- ��������޴�����
				g_tUserHaveTaskIDList[holder_id][task_id] = nil
				-- ������㲥��־
				g_tUserHaveTaskIDListChange[holder_id] = true
			end
		end
	end

	-- �ǳ��¼��͸����¼�������
	if action == 'logout' or action == 'delete' then
		return
	end

	if TASK_LIST_UPDATE_CACHE[holder_id] == nil then
		TASK_LIST_UPDATE_CACHE[holder_id] = {}
	end

	if action == 'update' then							   -- ��������
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 0
	elseif action == 'accept' then						   -- ��������
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 1
	elseif action == 'accomplish' then					   -- �������
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 2
	elseif action == 'complete' then					   -- �������
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 3
	elseif action == 'abort' then						   -- ��������
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 4
	end
end

-------------------------------------------------------------------
-- [��ɫ��¼] => ��ȡ�����б�
-------------------------------------------------------------------
function TaskListManager_LoginEventHandler(holder_id, flag)

	-- ����Ƿ�ˢ�²�������
	systemReflushOptCount(holder_id)

	-- ��ʤ�������Զ�����
	RequestCanAutoAcceptFirstWinTypeTask(holder_id)
	
	-- �Զ�������������
	RequestAutoAcceptTask(holder_id)
	
	-- ��������λ����
	CheckActorRankTask(holder_id)

	-- ���������б�
    RequestTaskList(holder_id)
end


function TaskListManager_TimerHandler()

	local tick = GetTickCountEx()
	for holder_id,tTask in pairs(TASK_LIST_UPDATE_CACHE) do

		-- ��ͻ���ͬ�������б�
		if TASK_LIST[holder_id]~=nil then

			local task_info = {}
			local task_finish = {}
			local task_delete = {}
			for task_id,flag in pairs(tTask) do

				if flag == 0 then						-- ����

					local task = TASK_LIST[holder_id][task_id]
					if task~=nil then
						task_info[task_id] = GetTaskBaseStateInfo(holder_id, task)
					end
				elseif flag == 1 then					-- ����

					local task = TASK_LIST[holder_id][task_id]
					if task~=nil then
						task_info[task_id] = GetTaskBaseStateInfo(holder_id, task)
					end
				elseif flag == 2 then					-- ���
				
					local task = TASK_LIST[holder_id][task_id]
					if task~=nil then
						task_info[task_id] = GetTaskBaseStateInfo(holder_id, task)
					end
				elseif flag == 3 then					-- ����

					table.insert(task_finish, task_id)
				elseif flag == 4 then					-- ����

					table.insert(task_delete, task_id)
				end
			end

			-- ����������
			local strTaskDeleteInfo = ""
			local bTaskFinish = false
			strTaskDeleteInfo = strTaskDeleteInfo.."<taskfinish>"
			for index,task_id in pairs(task_finish) do
				strTaskDeleteInfo = strTaskDeleteInfo..task_id..";"
				bTaskFinish = true
			end
			strTaskDeleteInfo = strTaskDeleteInfo.."</taskfinish>"
			-- ɾ���������
			local bTaskDelete = false
			strTaskDeleteInfo = strTaskDeleteInfo.."<taskdelete>"
			for index,task_id in pairs(task_delete) do
				strTaskDeleteInfo = strTaskDeleteInfo..task_id..";"
				bTaskDelete = true
			end
			strTaskDeleteInfo = strTaskDeleteInfo.."</taskdelete>"
			-- ���������б�
			if bTaskFinish or bTaskDelete then
				DeleteTaskList(holder_id, strTaskDeleteInfo)
			end

			-- �������������Ϣ
			local bTaskUpdate = false
			local strTaskInfo = ""
			for task_id,info in pairs(task_info) do
				if info~=nil then
					strTaskInfo = strTaskInfo..info
					bTaskUpdate = true
				end
			end
			-- ���������б�
			if bTaskUpdate then
				UpdateTaskList(holder_id, strTaskInfo)
			end

			-- ���������
			if bFinishTask then
				
			end
		end

		TASK_LIST_UPDATE_CACHE[holder_id] = nil

		if GetTickCountEx() - tick > 1000 then
			return
		end
	end
end

-------------------------------------------------------------------
-- ����ǰ�����б�
-- ����������ѽӵ����񷢸��ͻ���,��ʽΪ{����id,�������ƣ�����ȼ��������ֵ���Ի�npcid,�������ͣ������Ƿ�����}
-------------------------------------------------------------------
function RequestTaskList(holder_id)

    if holder_id == nil or TASK_LIST[holder_id] == nil then
  		return
    end

	local strTaskInfo = ""
	-- ˢ����Ϣ
	local optInfo = GetActorReflushOptInfo(holder_id)
	if optInfo ~= nil then
		strTaskInfo = strTaskInfo..optInfo
	end
	-- ��������
	for task_id,task in pairs(TASK_LIST[holder_id]) do
		local info = GetTaskBaseStateInfo(holder_id, task)
		--trace("task_id="..task_id..",info="..info)
		if info~=nil then
			strTaskInfo = strTaskInfo..info
		end
	end

	-- ���������б�
	SendTaskList(holder_id, strTaskInfo)
end

-------------------------------------------------------------------
-- ȡ�õ�ǰ����Ļ�����Ϣ�������ͻ���
-------------------------------------------------------------------
function GetTaskBaseStateInfo(holder_id, task)

	if holder_id == nil or holder_id <= 0 or task == nil then
		return nil
	end

	local strPrize = ""
	
	if task.prize_binding_gold ~= nil and task.prize_binding_gold > 0 then
		strPrize = strPrize..TASK_BINDINGGOLD_PRIZEID..";"..task.prize_binding_gold..";"
	end

	if task.prize_hero_exp ~= nil and task.prize_hero_exp > 0 then
		strPrize = strPrize..TASK_EXP_PRIZEID..";"..task.prize_hero_exp..";"
	end

	if task.prize_good ~= nil then
		for index,prize_id in pairs(task.prize_good) do
			strPrize = strPrize..prize_id..";1;"
		end
	end
	
	local nClientLink = 0
	if task.client_link ~= nil and task.client_link ~= -1 then
		nClientLink = task.client_link
	end
	
	-- ��ʤ������Ϣ���⴦��
	local strDesc = task.description
	if task.type == TaskType.FirstWinType then
		strDesc = to_utf8("Ӯ��һ�������ɻ�ñ��䣬�����˳�Ա��ӻ�ʤ�ɻ��ȫ������")
		local win_count_key = GetWinCountTaskKey(holder_id, task.id)
		local win_count_value = GetTaskIntData(holder_id, win_count_key)
		-- �����ͨ��ʤ����
		if win_count_value == 1 then
			strDesc = to_utf8("Ӯ��һ�������ɻ�ñ��䣬<color='#a8232e'>�����˳�Ա��ӻ�ʤ�ɻ��ȫ������</color>")
		end
	end
	
	local strResult = ""
	strResult = strResult.."<id>"..task.id.."</id><type>"..task.type.."</type><name>"..task.name.."</name>"
	strResult = strResult.."<state>"..task:GetState().."</state><description>"..strDesc.."</description><prize>"..strPrize.."</prize>"
	strResult = strResult.."<hit>"..task:GetHit().."</hit><clientlink>"..nClientLink.."</clientlink><subtype>"..task.sub_type.."</subtype>"
	strResult = strResult.."<specifictype>"..task.specific_type.."</specifictype><contacthigh32id>"..task.contact_high32id.."</contacthigh32id><contactlow32id>"..task.contact_low32id.."</contactlow32id>"
	
	return strResult
end

-------------------------------------------------------------------
-- ����ɾ������
-- @param holder_id : ��ɫDBID
-- @param task_id	: ����ID
-------------------------------------------------------------------
function RequestDeleteTask(holder_id, task_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return false
	end

	if task_id == nil or task_id <= 0 then
		return false
	end
	
    local oTask = TASK_LIST[holder_id][task_id]
	if oTask == nil then
		return false
	end

	-- ������ɾ��
	if oTask.allow_delete == false then
		ShowSystemMessage(holder_id, 18001)
		return false
	end

	if oTask:DeleteTask()==false then
		return false
	end

	--local szText = to_utf8("��ɫID:")..holder_id..to_utf8(",��ɫ����:")..ACTOR_LIST[holder_id]..to_utf8(",��������ID:")..task_id..to_utf8(",��������:")..oTask.name
	local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Delete Task ID:"..task_id..",Task Name:"..oTask.name
	AddTaskGameNoteLog(holder_id, szText)

	--����������,�����¼�
    OnActorTaskListChanged(holder_id, task_id, "abort")
    
	TaskFactory:DeleteTask(oTask)
	TASK_LIST[holder_id][task_id] = nil

	-- ����ɾ�������� 
    OnActorTaskListChanged(holder_id, task_id, "delete")

	return true
end

-------------------------------------------------------------------
-- ������Զ�������ʤ��������
-- @param holder_id : ��ɫDBID
-- @addition 		: ���������⣬1����½ 2���ͻ��˶�ʱ������ˢ��
-- @addition		������������ˢ��ʱ����С�ڽ��մ���
-------------------------------------------------------------------
function RequestCanAutoAcceptFirstWinTypeTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return
	end
	
	-- ����Ϊ1��ʾ�ɽ���ʱ�Զ�����������
	if TaskFactory.tTaskTypeList == nil or TaskFactory.tTaskTypeList[TaskType.FirstWinType] == nil then
		error("RequestCanAutoAcceptFirstWinTypeTask not this TaskType"..TaskType.FirstWinType)
		return
	end
	
	for _,task_id in pairs(TaskFactory.tTaskTypeList[TaskType.FirstWinType]) do
	
		local task = TaskFactory.tTaskList[task_id]
		
		if task ~= nil then
		
			task.holder_id = holder_id
			if task:CanAccept(true) == true then
				-- ��������ֱ�ӷ��أ��������
				local oTask = TASK_LIST[holder_id][task_id]
				if oTask == nil then
					oTask = TaskFactory:CreateTask(holder_id, task_id)
				else
					local task_key = GetWinCountTaskKey(oTask.holder_id, oTask.id)
					local task_value = GetTaskIntData(oTask.holder_id, task_key)
					if task_value == 1 or task_value == 2 then
						return
					end
				end
				
				if oTask ~= nil then

					-- ��������
					oTask:Accept()

					TASK_LIST[holder_id][task_id] = oTask

					--����������,�����¼�
					OnActorTaskListChanged(holder_id, task_id, "accept")

					--local szText = to_utf8("��ɫID:")..holder_id..to_utf8(",��ɫ����:")..ACTOR_LIST[holder_id]..to_utf8(",��������ID:")..task_id..to_utf8(",��������:")..oTask.name
					local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Accept Task ID:"..task_id..",Task Name:"..oTask.name
					AddTaskGameNoteLog(holder_id, szText)
				end
			else
				
				local pre_task_state = TaskState.EndState
				if task.pre_id ~= nil then
					local pre_task_key = GetTaskIDKey(holder_id, task.pre_id)
					local pre_task_value = GetTaskIntData(holder_id, pre_task_key)
					if pre_task_value == TASK_DATA_INVALID then
						pre_task_value = TaskState.BeginState
					end
					pre_task_state = pre_task_value
				end		
				
				if pre_task_state >= TaskState.EndState and TASK_LIST[holder_id][task_id] == nil then

					local oTask = TaskFactory:CreateTask(holder_id, task_id)
					if oTask ~= nil then
						
						-- �Ȱ���������б�
						TASK_LIST[holder_id][task_id] = oTask
						
						oTask:OnLogin()
					end
				end
			end
			task.holder_id = nil
		end
	end
end

-------------------------------------------------------------------
-- ����ˢ������
-- @param holder_id : ��ɫDBID
-- @param task_id	: ��������
-------------------------------------------------------------------
function RequestFreshTask(holder_id, task_type)

	if holder_id == nil or TASK_LIST[holder_id] == nil or task_type == nil then
		return
	end
	
	if task_type == TaskType.FirstWinType then
		RequestCanAutoAcceptFirstWinTypeTask(holder_id)
	end
	
end

-------------------------------------------------------------------
-- �����������
-- @param holder_id : ��ɫDBID
-- @param task_id	: ����ID
-------------------------------------------------------------------
function RequestFinishTask(holder_id, task_id)
	
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

	if oTask:IsFinished() == false then
		return
	end

	--local szText = to_utf8("��ɫID:")..holder_id..to_utf8(",��ɫ����:")..ACTOR_LIST[holder_id]..to_utf8(",�������ID:")..task_id..to_utf8(",��������:")..oTask.name
	local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Finish Task ID:"..task_id..",Task Name:"..oTask.name
	AddTaskGameNoteLog(holder_id, szText)
	
	-- ��������
	oTask:End()
	
	-- ��λ����:
	-- 1����ʽ�����ȡ��һ������ڵ㣨λ�ò��ܱ�,����End()���״̬���ܺ�������Ȼ��DeleteTask()ɾ������
	-- 2����ʽ�������һ����ɾ��
	if oTask.type == TaskType.RankType then
		if oTask.rear_id ~= nil and oTask.rear_id > 0 then
			DealAcceptNextRankTask(holder_id, oTask.rear_id)
		else
			OnActorTaskListChanged(holder_id, task_id,"update")
			return
		end
	end
	
	-- ��ʤ���񣺴���һ������
	if oTask.type == TaskType.FirstWinType then
		OnActorTaskListChanged(holder_id, task_id,"update")
		return
	end
	
	-- ɾ������
	oTask:DeleteTask()
	
	-- �ͷ������ڴ�
	TaskFactory:DeleteTask(oTask)

	-- �ÿ��������
	TASK_LIST[holder_id][task_id] = nil

	-- ����һ�����
	OnActorTaskListChanged(holder_id, task_id,"complete")

end

------------------------------------------------------------------------- �������� --------------------------------------------------------------------------

-------------------------------------------------------------------------
-- ������ӽ�������
-- @param holder_id 	: ��ɫDBID
-- @param sub_type		: ��������������
-- @param specific_type	: ���������������¾�������
-- @param tag_high32id	: ��ǩ��32λID
-- @param tag_low32id	: ��ǩ��32λID
-- @param emotion_words : ���黰��
-------------------------------------------------------------------------
function RequestAddContactTask(holder_id, sub_type, specific_type, tag_high32id, tag_low32id, emotion_words)

	if holder_id == nil or sub_type == nil or specific_type == nil then
		return
	end
	-- ��鷢������
	local Publish_Count_Key = GetContactPublishCountKey(holder_id)
	local Publish_Count_Value = GetTaskIntData(holder_id, Publish_Count_Key)
	
	if Publish_Count_Value > 0 then
		-- ֪ͨ�������
		OnCheckAddContactTask(holder_id, sub_type, specific_type, tag_high32id, tag_low32id, emotion_words, 1)
	else
		-- ֪ͨ������� 
		OnCheckAddContactTask(holder_id, sub_type, specific_type, tag_high32id, tag_low32id, emotion_words, 0)
	end
end

-------------------------------------------------------------------------
-- �ɹ���ӽ�������
-- @param holder_id : ��ɫDBID
-- @param contact_high32id	: �������ID��32λ
-- @param contact_low32id	: �������ID��32λ
-------------------------------------------------------------------------
function onSucceedAddContactTask(holder_id, contact_high32id, contact_low32id)

	if holder_id == nil then
		return
	end
	
	-- ���·�������
	local Publish_Count_Key = GetContactPublishCountKey(holder_id)
	local Publish_Count_Value = GetTaskIntData(holder_id, Publish_Count_Key)
	Publish_Count_Value = Publish_Count_Value - 1
	SetTaskIntData(holder_id, Publish_Count_Key, Publish_Count_Value);
	
	-- ����ʣ�෢������
	local strTaskInfo = ""
	strTaskInfo = strTaskInfo.."<publishcount>"..Publish_Count_Value.."</publishcount>".."<contacthigh32id>"..contact_high32id.."</contacthigh32id>".."<contactlow32id>"..contact_low32id.."</contactlow32id>"
	SendAddTaskSuccess(holder_id, strTaskInfo)
	
end

-------------------------------------------------------------------------
-- Ԥ���󲶻�����
-- @param holder_id 		: ��ɫDBID
-- @param contact_high32id	: �������ID��32λ
-- @param contact_low32id	: �������ID��32λ
-- @param sub_type			: �����������������
-- @param specific_type		: ������������������¾�������
-- @param publisher_id		: ������DBID
-------------------------------------------------------------------------
function RequestPreCatchTask(holder_id, contact_high32id, contact_low32id, sub_type, specific_type, publisher_id)

	if holder_id == nil then
		return
	end

	local strTaskInfo = ""
		
	-- �����������
	local Catch_Count_Key = GetTaskCatchCountKey(holder_id)
	local Catch_Count_Value = GetTaskIntData(holder_id, Catch_Count_Key)

	if Catch_Count_Value <= 0 then
		-- ����Ԥ������
		strTaskInfo = strTaskInfo.."<success>".."0".."</success>".."<msgid>".."18014".."</msgid>"
		SendPreCatchTaskResult(holder_id, strTaskInfo)
		return
	end

	-- ������������
	local tActorTask = TASK_LIST[holder_id]
	local nActorTaskCount = table_count(tActorTask)

	if nActorTaskCount >= TASK_ACTOR_MAX_COUNT then
		-- ����Ԥ������
		strTaskInfo = strTaskInfo.."<success>".."0".."</success>".."<msgid>".."18015".."</msgid>"
		SendPreCatchTaskResult(holder_id, strTaskInfo)
		return
	end

	-- ���ɲ�������
	local tAcceptType = {}
	
	local bContactTaskValid = false
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.ContacType]) do
		local oTask = TaskFactory.tTaskList[task_id]
		if oTask ~= nil and oTask.sub_type == sub_type and oTask.specific_type == specific_type and TASK_LIST[holder_id][task_id] == nil then
			oTask.holder_id = holder_id
			if oTask:CanAccept(true) == true then
				oTask.holder_id = nil
				bContactTaskValid = true
				break
			end
			oTask.holder_id = nil
		end
	end
	if (contact_high32id ~= 0 or contact_low32id ~= 0) and GetContactTaskCount(holder_id) < TASK_CONTACT_ID_MAX_COUNT and bContactTaskValid then
		table.insert(tAcceptType, TaskType.ContacType)
	end
	
	local nRandMainTypeID = RandMainTypeTask(holder_id)
	if nRandMainTypeID > 0 then
		table.insert(tAcceptType, TaskType.MainType)
	end
	
	local nRandCycleTypeID = RandCycleTypeTask(holder_id)
	if nRandCycleTypeID > 0 then
		table.insert(tAcceptType, TaskType.CycleType)
	end
	
	local nRandDailyTypeID = RandDailyTypeTask(holder_id)
	if nRandDailyTypeID > 0 then
		table.insert(tAcceptType, TaskType.DailyType)
	end
	
	local nTypeCount = table_count(tAcceptType)

	if nTypeCount <= 0 then
		
		-- ����Ԥ������

		if GetContactTaskCount(holder_id) >= TASK_CONTACT_ID_MAX_COUNT then
			strTaskInfo = strTaskInfo.."<success>".."0".."</success>".."<msgid>".."18015".."</msgid>"
		else
			strTaskInfo = strTaskInfo.."<success>".."0".."</success>".."<msgid>".."18016".."</msgid>"
		end
		SendPreCatchTaskResult(holder_id, strTaskInfo)
		return
		
	end
	
	-- 1.������������
	Catch_Count_Value = Catch_Count_Value - 1
	SetTaskIntData(holder_id, Catch_Count_Key, Catch_Count_Value)
	
	-- 2.�����������(3��)
	strTaskInfo = strTaskInfo.."<success>".."1".."</success>".."<catchcount>"..Catch_Count_Value.."</catchcount>"
	for	i = 1, 3 do
		
		local tRandType = table_rand(tAcceptType)
		
		if tRandType[1] == TaskType.ContacType then
			strTaskInfo = strTaskInfo.."<tasktype>"..TaskType.ContacType.."</tasktype>".."<contacthigh32id>"..contact_high32id.."</contacthigh32id>"
			strTaskInfo = strTaskInfo.."<contactlow32id>"..contact_low32id.."</contactlow32id>".."<subtype>"..sub_type.."</subtype>"
			strTaskInfo = strTaskInfo.."<specifictype>"..specific_type.."</specifictype>".."<publisherid>"..publisher_id.."</publisherid>"
		elseif tRandType[1] == TaskType.MainType then
			strTaskInfo = strTaskInfo.."<tasktype>"..TaskType.MainType.."</tasktype>".."<taskid>"..nRandMainTypeID.."</taskid>"
		elseif tRandType[1] == TaskType.CycleType then
			strTaskInfo = strTaskInfo.."<tasktype>"..TaskType.CycleType.."</tasktype>".."<taskid>"..nRandCycleTypeID.."</taskid>"
		elseif tRandType[1] == TaskType.DailyType then
			strTaskInfo = strTaskInfo.."<tasktype>"..TaskType.DailyType.."</tasktype>".."<taskid>"..nRandDailyTypeID.."</taskid>"
		end
		
	end
	
	-- 3.����Ԥ������
	SendPreCatchTaskResult(holder_id, strTaskInfo)
	
end

-------------------------------------------------------------------------
-- ���󲶻�����
-- @param holder_id 		: ��ɫDBID
-- @param task_type			: ��������
-- @param task_id			: ����ID
-- @param contact_high32id	: �������ID��32λ
-- @param contact_low32id	: �������ID��32λ
-- @param sub_type			: ��������������
-- @param specific_type		: ���������������¾�������
-- @param publisher_id		: ������ID
-------------------------------------------------------------------------
function RequestCatchTask(holder_id, task_type, task_id, contact_high32id, contact_low32id, sub_type, specific_type, publisher_id)

	if holder_id == nil then
		return
	end

	-- ��������
	local oTask = nil
	if task_type == TaskType.ContacType then
		oTask = TaskFactory:CreateContactTask(holder_id, sub_type, specific_type, contact_high32id, contact_low32id, publisher_id)
	else
		oTask = TaskFactory:CreateTask(holder_id, task_id)
	end
	
	if oTask ~= nil then
			
		-- ��������
		oTask:Accept()
		TASK_LIST[holder_id][oTask.id] = oTask
		-- �������񣬴����¼�
		OnActorTaskListChanged(holder_id, oTask.id, "accept")
		-- ��¼��־
		--local szText = to_utf8("��ɫID:")..holder_id..to_utf8(", ��ɫ����:")..ACTOR_LIST[holder_id]..to_utf8(", ��������ID:")..oTask.id..to_utf8(", ��������:")..oTask.name
		local szText = "Actor ID:"..holder_id..", Actor Name:"..ACTOR_LIST[holder_id]..", Accept Task ID:"..oTask.id..", Task Name:"..oTask.name
		AddTaskGameNoteLog(holder_id, szText)
		
		-- �������񣬼���Ƿ������
		if oTask.type == TaskType.MainType and oTask:CanFinish() == true then						
			oTask:Finish()
			-- �����¼�
			OnActorTaskListChanged(holder_id, oTask.id, "accomplish")
		end
			
	end
	
	-- ������������
	local strTaskInfo = ""
	
	local info = GetTaskBaseStateInfo(holder_id, oTask)
	if info~=nil then
		strTaskInfo = strTaskInfo..info
	end
	
	SendCatchTaskResult(holder_id, strTaskInfo)
end

-------------------------------------------------------------------------
-- ���һ���ɽ��յ�ѭ������
-------------------------------------------------------------------------
function RandCycleTypeTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return 0
	end
	
	-- ѭ���������������ж�
	local nTaskCount = 0
	for	_, task in pairs(TASK_LIST[holder_id]) do
		if task.type == TaskType.CycleType then
			nTaskCount = nTaskCount + 1
		end
	end
	if nTaskCount >= TASK_CYCLE_MAX_COUNT then
		return 0
	end
	
	-- ���һ��ѭ������
	if TaskFactory.tTaskTypeList == nil	or TaskFactory.tTaskTypeList[TaskType.CycleType] == nil then
		error("RandCycleTypeTask error. TaskFactory.tTaskTypeList[TaskType.CycleType] == nil")
		return 0
	end
	
	local tCanAcceptCycleTask = {}
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.CycleType]) do
		local oTask = TaskFactory.tTaskList[task_id]
		if oTask ~= nil and TASK_LIST[holder_id][task_id] == nil then
			oTask.holder_id = holder_id
			if oTask:CanAccept(true) == true then
				table.insert(tCanAcceptCycleTask, task_id)
			end
			oTask.holder_id = nil
		end
	end
	
	local nCanAcceptCount = table_count(tCanAcceptCycleTask)
	if nCanAcceptCount <= 0 then
		return 0
	end
	
	local tRandCanAcceptCycleTask = table_rand(tCanAcceptCycleTask)
	return tRandCanAcceptCycleTask[1]

end

-------------------------------------------------------------------------
-- ���һ���ɽ��յ���������
-------------------------------------------------------------------------
function RandMainTypeTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return 0
	end	
	
	-- ���һ����������
	if TaskFactory.tTaskTypeList == nil	or TaskFactory.tTaskTypeList[TaskType.MainType] == nil then
		error("RandMainTypeTask error. TaskFactory.tTaskTypeList[TaskType.MainType] == nil")
		return 0
	end
	
	local tCanAcceptMainTask = {}
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.MainType]) do
		local task = TaskFactory.tTaskList[task_id]
		if task ~= nil and TASK_LIST[holder_id][task_id] == nil then
			task.holder_id = holder_id
			-- ����������ɺ󲻿��ٴν���
			local task_type = GetTaskIDKey(holder_id, task_id)
			local task_value = GetTaskIntData(holder_id, task_type)
			if task_value == TASK_DATA_INVALID and task:CanAccept(true) == true then
				table.insert(tCanAcceptMainTask, task_id)
			end
			task.holder_id = nil
		end
	end
	
	local nCanAcceptCount = table_count(tCanAcceptMainTask)
	
	if nCanAcceptCount <= 0 then
		return 0
	end
	
	local tRandCanAcceptMainTask = table_rand(tCanAcceptMainTask)
	return tRandCanAcceptMainTask[1]

end

-------------------------------------------------------------------------
-- ���һ���ɽ��յ��ճ�����
-------------------------------------------------------------------------
function RandDailyTypeTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return 0
	end	
	
	-- �ճ�����ÿ��ֻ�ɽ���һ��
	local nDailyTypeCount = 0
	for _, taskid in pairs(TaskFactory.tTaskTypeList[TaskType.DailyType]) do
		local task = TASK_LIST[holder_id][taskid]
		if task ~= nil then
			nDailyTypeCount = nDailyTypeCount + task:GetTaskDayCount()
		end
	end
	if nDailyTypeCount >= TASK_DAILY_MAX_COUNT then
		return 0
	end
	
	
	-- ���һ���ճ�����
	if TaskFactory.tTaskTypeList == nil	or TaskFactory.tTaskTypeList[TaskType.DailyType] == nil then
		error("RandDailyTypeTask error. TaskFactory.tTaskTypeList[TaskType.DailyType] == nil")
		return 0
	end
	
	local tCanAcceptDailyTask = {}
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.DailyType]) do
		local oTask = TaskFactory.tTaskList[task_id]
		if oTask ~= nil and TASK_LIST[holder_id][task_id] == nil then
			oTask.holder_id = holder_id
			if oTask:CanAccept(true) == true then
				table.insert(tCanAcceptDailyTask, task_id)
			end
			oTask.holder_id = nil
		end
	end
	
	local nCanAcceptCount = table_count(tCanAcceptDailyTask)
	
	if nCanAcceptCount <= 0 then
		return 0
	end
	
	local tRandCanAcceptDailyTask = table_rand(tCanAcceptDailyTask)
	return tRandCanAcceptDailyTask[1]

end

-------------------------------------------------------------------
-- ȡ�õ�ǰ�û���ˢ����Ϣ
-------------------------------------------------------------------
function GetActorReflushOptInfo(holder_id)
	
	local strOptInfo = ""
	
	local publish_count_key = GetContactPublishCountKey(holder_id)
	local publish_count_value = GetTaskIntData(holder_id, publish_count_key)
	strOptInfo = strOptInfo.."<publishcount>"..publish_count_value.."</publishcount>"
	
	local catch_count_key = GetTaskCatchCountKey(holder_id)
	local catch_count_value = GetTaskIntData(holder_id, catch_count_key)
	strOptInfo = strOptInfo.."<catchcount>"..catch_count_value.."</catchcount>"
	
	local reflush_count_key = GetManualReflushCountKey(holder_id)
	local reflush_count_value = GetTaskIntData(holder_id, reflush_count_key)
	strOptInfo = strOptInfo.."<reflushcount>"..reflush_count_value.."</reflushcount>"
	
	-- ʣ��ˢ��ʱ��
	local reflush_time_key = GetReflushTimeKey(holder_id)
	local reflush_time_value = GetTaskIntData(holder_id, reflush_time_key)
	local left_reflush_time =  reflush_time_value + (3600*TASK_REFLUSH_TIME_INTERVAL) - os.time()
	strOptInfo = strOptInfo.."<reflushtime>"..left_reflush_time.."</reflushtime>"
	
	return strOptInfo

end

-------------------------------------------------------------------------
-- ����ˢ����ҷ�������ȡ����ʱ��
-- @addition 	: TASK_REFLUSH_TIME_INTERVAL��������ʱ���ˢһ�Σ�
-- @addition 	: ���ʱ����һ��ʣ��ʱ�䶨ʱ������������TASK_REFLUSH_TIME_INTERVALʱ�䶨ʱ��
-------------------------------------------------------------------------
function SetActorOptReflushTimer()

	local nFlushSec = 3600 * TASK_REFLUSH_TIME_INTERVAL
	local nDate = os.date("*t", os.time())
	local nTime = nFlushSec - (nDate.hour * 3600 + nDate.min * 60 + nDate.sec) % nFlushSec
	local nSceneID = GetLuaSceneID()
	g_nTaskReFlushOptTimer = CreateTimerTrigger(nSceneID, "TasklistManager_FlushTimerHandler", nTime*1000, 0)
		
end

function TasklistManager_FlushTimerHandler()
	
	-- ˢ��������ҷ������������������ˢ�´���������ʱ��
	if TASK_LIST ~= nil	then
		
		local nTime = os.time()
		for holder_id,_ in pairs(TASK_LIST) do
		
			local publish_count_key = GetContactPublishCountKey(holder_id)
			SetTaskIntData(holder_id, publish_count_key, TASK_CONTACT_PUBLISH_MAX_COUNT)
			
			local catch_count_key = GetTaskCatchCountKey(holder_id)
			local catch_count_value = GetTaskIntData(holder_id, catch_count_key)
			if catch_count_value == TASK_DATA_INVALID then
				catch_count_value = 0
			end
			if TASK_CATCH_MAX_COUNT > catch_count_value then
				catch_count_value = TASK_CATCH_MAX_COUNT
			end
			SetTaskIntData(holder_id, catch_count_key, catch_count_value)
			
			local reflush_count_key = GetManualReflushCountKey(holder_id)
			SetTaskIntData(holder_id, reflush_count_key, TASK_MANUAL_REFLUSH_MAX_COUNT)
		
			local reflush_time_key = GetReflushTimeKey(holder_id)
			SetTaskIntData(holder_id, reflush_time_key, nTime)
			
			-- ���¿ͻ���
			local strOptInfo = GetActorReflushOptInfo(holder_id)
			if strOptInfo ~= nil then
				SendReflushOptInfo(holder_id, strOptInfo)
			end
			
		end
		
	end
	
	-- ����ִ��TASK_REFLUSH_TIME_INTERVALСʱ��ʱ��
	if g_bInitReflushTimer then
	
		if g_nTaskReFlushOptTimer ~= 0 and DeleteTrigger(g_nTaskReFlushOptTimer) == false then
			error("Delete g_nTaskReFlushOptTimer faild TriggerID="..g_nTaskReFlushOptTimer)
		end
		local nSceneID = GetLuaSceneID()
		g_nTaskReFlushOptTimer = CreateTimerTrigger(nSceneID, "TasklistManager_FlushTimerHandler", TASK_REFLUSH_TIME_INTERVAL*3600*1000, 0)
		
		g_bInitReflushTimer = false
	
	end
	
end

-------------------------------------------------------------------------
-- ��ҵ�½��ϵͳ����Ƿ�ˢ�²�������
-------------------------------------------------------------------------
function systemReflushOptCount(holder_id)
	
	local reflush_time_key = GetReflushTimeKey(holder_id)
	local reflush_time_value = GetTaskIntData(holder_id, reflush_time_key)
	local nTime = os.time()
	
	-- ����Ƿ�ˢ��
	local bFlush = false
	if reflush_time_value == TASK_DATA_INVALID then
		bFlush = true
	else 
		if nTime - reflush_time_value >= 3600*TASK_REFLUSH_TIME_INTERVAL then
			bFlush = true
		end
	end
	
	-- ִ��ˢ�²���
	if bFlush then
	
		local publish_count_key = GetContactPublishCountKey(holder_id)
		SetTaskIntData(holder_id, publish_count_key, TASK_CONTACT_PUBLISH_MAX_COUNT)
		
		local catch_count_key = GetTaskCatchCountKey(holder_id)
		local catch_count_value = GetTaskIntData(holder_id, catch_count_key)
		if catch_count_value == TASK_DATA_INVALID then
			catch_count_value = 0
		end
		if TASK_CATCH_MAX_COUNT > catch_count_value then
			catch_count_value = TASK_CATCH_MAX_COUNT
		end
		SetTaskIntData(holder_id, catch_count_key, catch_count_value)
		
		local reflush_count_key = GetManualReflushCountKey(holder_id)
		SetTaskIntData(holder_id, reflush_count_key, TASK_MANUAL_REFLUSH_MAX_COUNT)
	
		-- ʵ��ˢ��ʱ���Ϊ�ϴ�����ʱ��
		local nDate = os.date("*t", nTime)
		local nCorrectTime = nTime - (nDate.hour * 3600 + nDate.min * 60 + nDate.sec) % (3600*TASK_REFLUSH_TIME_INTERVAL)
		SetTaskIntData(holder_id, reflush_time_key, nCorrectTime)
	
	end

end

-------------------------------------------------------------------------
-- �ֶ�ˢ�²�������
-------------------------------------------------------------------------
function RequestReFlushOptCount(holder_id)
	
	-- �д���
	local reflush_count_key = GetManualReflushCountKey(holder_id)
	local reflush_count_value = GetTaskIntData(holder_id, reflush_count_key)
	if reflush_count_value <= 0 then
		ShowSystemMessage(holder_id, 18011)
		return
	end
	
	-- ��Ǯ
	local bResult = CheckAddMoneyOfReflushOpt(holder_id)
	if not bResult then
		ShowSystemMessage(holder_id, 18012)
		return
	end
	
	-- ������
	reflush_count_value = reflush_count_value - 1
	SetTaskIntData(holder_id, reflush_count_key, reflush_count_value)
	
	-- ˢ��
	local publish_count_key = GetContactPublishCountKey(holder_id)
	SetTaskIntData(holder_id, publish_count_key, TASK_CONTACT_PUBLISH_MAX_COUNT)
	
	local catch_count_key = GetTaskCatchCountKey(holder_id)
	local catch_count_value = GetTaskIntData(holder_id, catch_count_key)
	if catch_count_value == TASK_DATA_INVALID then
		catch_count_value = 0
	end
	if TASK_CATCH_MAX_COUNT > catch_count_value then
		catch_count_value = TASK_CATCH_MAX_COUNT
	end
	SetTaskIntData(holder_id, catch_count_key, catch_count_value)
	
	-- �·� 
	local strOptInfo = GetActorReflushOptInfo(holder_id)
	if strOptInfo ~= nil then
		SendReflushOptInfo(holder_id, strOptInfo)
	end

end

-------------------------------------------------------------------------
-- ��ӵ�н�����������
-------------------------------------------------------------------------
function GetContactTaskCount(holder_id)

	if holder_id == nil	then
		return 0
	end

	if TASK_LIST == nil or TASK_LIST[holder_id] == nil then
		return 0
	end	
	
	local nCount = 0
	for _, task in pairs(TASK_LIST[holder_id]) do
		if task.type == TaskType.ContacType then
			nCount = nCount + 1
		end
	end
	
	return nCount
end

-------------------------------------------------------------------
-- �Զ�������������
-- @param holder_id : ��ɫDBID
-------------------------------------------------------------------
function RequestAutoAcceptTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return
	end
	
	-- ���Զ�������������(�в�����Լ����)
	local tAutoAcceptID = {10,31}

	for _,task_id in pairs(tAutoAcceptID) do
	
		local task = TaskFactory.tTaskList[task_id]
		if task ~= nil and TASK_LIST[holder_id][task_id] == nil then
		
			task.holder_id = holder_id
			-- ����������ɺ󲻿��ٴν���
			local task_type = GetTaskIDKey(holder_id, task_id)
			local task_value = GetTaskIntData(holder_id, task_type)
			if task_value == TASK_DATA_INVALID and task:CanAccept(true) == true then
			
				local oTask = TaskFactory:CreateTask(holder_id, task_id)
				if oTask ~= nil then
					-- ��������
					oTask:Accept()

					TASK_LIST[holder_id][task_id] = oTask

					--����������,�����¼�
					OnActorTaskListChanged(holder_id, task_id, "accept")

					--local szText = to_utf8("��ɫID:")..holder_id..to_utf8(",��ɫ����:")..ACTOR_LIST[holder_id]..to_utf8(",��������ID:")..task_id..to_utf8(",��������:")..oTask.name
					local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Accept Task ID:"..task_id..",Task Name:"..oTask.name
					AddTaskGameNoteLog(holder_id, szText)
					
				end
			end
			task.holder_id = nil
		end
	end
end

-------------------------------------------------------------------
-- ���������ȡ����
-- @param holder_id : ��ɫDBID
-- @param prize_num : ��������
-------------------------------------------------------------------
function RequestAddTaskCatchNum(holder_id, prize_num)

	if holder_id == nil or prize_num == nil then
		return
	end
	
	if prize_num <= 0 then
		return
	end
	
	local catch_count_key = GetTaskCatchCountKey(holder_id)
	local catch_count_value = GetTaskIntData(holder_id, catch_count_key)
	catch_count_value = catch_count_value + prize_num
	SetTaskIntData(holder_id, catch_count_key, catch_count_value)
	
	-- �·� 
	local strOptInfo = GetActorReflushOptInfo(holder_id)
	if strOptInfo ~= nil then
		SendReflushOptInfo(holder_id, strOptInfo)
	end

end

------------------------------------------------------------------------- ��λ���� --------------------------------------------------------------------------

-------------------------------------------------------------------------
-- ��������λ����û�и�ֵͷ�ڵ㣬���ж��Ƿ��µ�һ�ܣ�
-- @param holder_id 	: ��ɫDBID
-------------------------------------------------------------------------
function CheckActorRankTask(holder_id)

	if holder_id == nil	or TASK_LIST[holder_id] == nil then
		return
	end

	if TaskFactory.tTaskTypeList == nil	or TaskFactory.tTaskTypeList[TaskType.RankType] == nil then
		return
	end
	
	local tRankTask = {}
	-- ��ӻ�������
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.RankType]) do
		local task = TASK_LIST[holder_id][task_id]
		if task ~= nil then
			table.insert(tRankTask, task_id)
		end
	end
	-- ��ʽ�������һ����ȡ״̬��û�ж�ȡ���ڴ棬�ֶ����
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.RankType]) do
		local task_key = GetTaskIDKey(holder_id, task_id)
		local task_state = GetTaskIntData(holder_id, task_key)
		if task_state ~= nil and task_state == TaskState.EndState then
		
			local oTask = TaskFactory.tTaskList[task_id]
			if oTask ~= nil then
				TASK_LIST[holder_id][task_id] = new(oTask,{holder_id=holder_id,state=TaskState.EndState})
				OnActorTaskListChanged(holder_id, task_id, "update")
				
				table.insert(tRankTask, task_id)
			end
		end
	end
	local nRankTaskCount = table_count(tRankTask)
	
	-- ��λ��ʽ���������������1��
	if nRankTaskCount > 1 then
		error("CheckRankTaskOnLogin error. nRankTaskCount > 1, nActorID = "..holder_id)
		return
	end
	
	-- δ��ȡ
	if nRankTaskCount == 0 then
		DealAutoAcceptRankTask(holder_id)
	-- �ѻ�ȡ����
	elseif nRankTaskCount == 1 then
		DealWeekClearRankTask(holder_id, tRankTask[1])
	end
end


-------------------------------------------------------------------------
-- �����Զ�������λ��������ǰ����������ƣ�ֻ���ȡ��ʽ�����ͷ����
-- @param holder_id 	: ��ɫDBID
-------------------------------------------------------------------------
function DealAutoAcceptRankTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return
	end	
	
	if TaskFactory.tTaskTypeList == nil	or TaskFactory.tTaskTypeList[TaskType.RankType] == nil then
		return
	end
	
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.RankType]) do
		local task = TaskFactory.tTaskList[task_id]
		if task ~= nil and TASK_LIST[holder_id][task_id] == nil then
		
			task.holder_id = holder_id
			if task:CanAccept(true) == true then
			
				local oTask = TaskFactory:CreateTask(holder_id, task_id)
				if oTask ~= nil then
					-- ��������
					oTask:Accept()
					TASK_LIST[holder_id][task_id] = oTask

					--����������,�����¼�
					OnActorTaskListChanged(holder_id, task_id, "accept")
					local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Accept Task ID:"..task_id..",Task Name:"..oTask.name
					AddTaskGameNoteLog(holder_id, szText)
					
					-- ������λ�����ȡʱ��
					local nNowTime = os.time()
					local nReflushTimeKey = GetRankReflushTimeKey(holder_id)
					SetTaskIntData(holder_id, nReflushTimeKey, nNowTime)
					
				end
			end
			task.holder_id = nil
			
		end
	end
	
end

-------------------------------------------------------------------------
-- ���������һ����λ����
-- @param holder_id 	: ��ɫDBID
-- @param task_id		: ����ID
-------------------------------------------------------------------------
function DealAcceptNextRankTask(holder_id, task_id)

	if holder_id == nil or task_id == nil then
		return
	end	
	
	if TASK_LIST[holder_id] == nil or TaskFactory.tTaskTypeList == nil then
		return
	end
	
	local task = TaskFactory.tTaskList[task_id]
	if task ~= nil and TASK_LIST[holder_id][task_id] == nil then
	
		task.holder_id = holder_id
		if task:CanAccept(true) == true then
		
			local oTask = TaskFactory:CreateTask(holder_id, task_id)
			if oTask ~= nil then
				-- ��������
				oTask:Accept()
				TASK_LIST[holder_id][task_id] = oTask

				--����������,�����¼�
				OnActorTaskListChanged(holder_id, task_id, "accept")
				local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Accept Task ID:"..task_id..",Task Name:"..oTask.name
				AddTaskGameNoteLog(holder_id, szText)
				
				-- ������λ�����ȡʱ��
				local nNowTime = os.time()
				local nReflushTimeKey = GetRankReflushTimeKey(holder_id)
				SetTaskIntData(holder_id, nReflushTimeKey, nNowTime)
				
			end
		end
		task.holder_id = nil
	end
	
end

-------------------------------------------------------------------------
-- ������λ�����������
-- @param holder_id 	: ��ɫDBID
-- @param task_id		: ����ID
-------------------------------------------------------------------------
function DealWeekClearRankTask(holder_id, task_id)

	if holder_id == nil	or task_id == nil or TASK_LIST[holder_id] == nil then
		return
	end
	
	local oTask = TASK_LIST[holder_id][task_id]
	if oTask == nil	then
		return
	end
	
	local nReflushTimeKey = GetRankReflushTimeKey(holder_id)
	local nReflushTimeValue = GetTaskIntData(holder_id, nReflushTimeKey)
	if nReflushTimeValue == TASK_DATA_INVALID then
		return
	end
	
	local nNowTime = os.time()
	if nNowTime < nReflushTimeValue then
		return
	end
	
	-- ͬһ��
	local nResult = DiffWeek(nReflushTimeValue, nNowTime)
	if nResult <= 0 then
		return
	end
	
	-- 1.���ͽ���
	if oTask:IsFinished() == true then
	
		local nGold = 0
		if oTask.prize_binding_gold ~= nil then
			nGold = oTask.prize_binding_gold
		end
		
		local nExp = 0
		if oTask.prize_hero_exp ~= nil then
			nExp = oTask.prize_hero_exp
		end

		local nPrizeID1 = 0
		local nPrizeID2 = 0
		if oTask.prize_good ~= nil then
			local nCount = table_count(oTask.prize_good)
			if nCount >= 2 then
				nPrizeID1 = oTask.prize_good[1]
				nPrizeID1 = oTask.prize_good[2]
			elseif nCount >= 1 then
				nPrizeID1 = oTask.prize_good[1]
			end
		end
	
		SendPlayerMail(holder_id, nGold, nExp, nPrizeID1, nPrizeID2)
	end
	
	-- 2.������
	RemoveTaskData(holder_id, nReflushTimeKey)
	
	oTask:DeleteTask()
	
	TaskFactory:DeleteTask(oTask)
	TASK_LIST[holder_id][task_id] = nil
	
	-- 3.���»�ȡ��ʽ����ͷ
	DealAutoAcceptRankTask(holder_id)

end

-------------------------------------------------------------------------
-- ������λ����������
-------------------------------------------------------------------------
function RequestClearRankTask()

	if ACTOR_LIST == nil then
		return
	end

	for actor_id,_ in pairs(ACTOR_LIST) do
		CheckActorRankTask(actor_id)
	end
	
end






