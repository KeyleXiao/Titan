-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	12/3/2016
-- 版  本:	1.0
-- 描  述:	任务列表维护
-------------------------------------------------------------------

require "TaskList"
require "TaskFactory"
require "TaskData"
require "TaskDef"

-- 任务礼包更新定时器
g_nTaskListUpdateTimer = 0

-- 任务状态变化后延迟同步客户端任务列表
TASK_LIST_UPDATE_CACHE = {}

-- 任务绑金奖励图标
TASK_BINDINGGOLD_PRIZEID = 1518
-- 任务经验奖励图标
TASK_EXP_PRIZEID = 1519

-- 任务刷新操作更新定时器
g_nTaskReFlushOptTimer = 0
g_bInitReflushTimer = true

-------------------------------------------------------------------
-- 初始化任务列表管理器
-- @note  : 该模块服务器把任务列表同步发给客户端
-------------------------------------------------------------------
function InitTaskListManager()

	-- 订阅登陆事件
	g_luaEventPublisher:Subscribe(LUA_EVENT_ACTOR_LOGIN, TaskListManager_LoginEventHandler)

	-- 订阅任务变化事件
    RegisterTaskListChangeEvent(0, 0, TaskListManager_TaskEventHandler, 0)

	local nSceneID = GetLuaSceneID()
	-- 任务列表不能频繁发给客户端，需要缓冲一下，所以设个定时器
    g_nTaskListUpdateTimer = CreateTimerTrigger(nSceneID, "TaskListManager_TimerHandler", 1000, 0)
	
	-- 订阅场景卸载事件
	g_luaEventPublisher:Subscribe(LUA_EVENT_SCENE_UNLOAD, TaskListManager_SceneUnloadEventHandler)
	
	-- 订阅任务发布、获取功能更新定时器
	SetActorOptReflushTimer()
end

-------------------------------------------------------------------
-- 场景卸载事件处理
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
-- 任务数据发生了变化
-- 添加到缓冲列表中，等会再发
-------------------------------------------------------------------
function TaskListManager_TaskEventHandler(holder_id,task_id,action)

	-- 优化控制标识，true=开启优化,false=关闭优化
	if g_UserTaskOptimizeOpen then
		-- 一些事件需要改变备份任务
		if action == 'accept' then

			-- 如果这个玩家数据为nil
			if g_tUserHaveTaskIDList[holder_id] == nil then
				g_tUserHaveTaskIDList[holder_id] = {}
			end
			
			-- 玩家无此任务
			if not g_tUserHaveTaskIDList[holder_id][task_id] then
				-- 设置玩家拥有此任务
				g_tUserHaveTaskIDList[holder_id][task_id] = true
				-- 下线需广播标志
				g_tUserHaveTaskIDListChange[holder_id] = true
			end
		elseif action == 'delete' or action == 'complete' then

			-- 如果这个玩家数据为nil
			if g_tUserHaveTaskIDList[holder_id] == nil then
				g_tUserHaveTaskIDList[holder_id] = {}
			end

			-- 玩家拥有任务
			if g_tUserHaveTaskIDList[holder_id][task_id] then
				-- 设置玩家无此任务
				g_tUserHaveTaskIDList[holder_id][task_id] = nil
				-- 下线需广播标志
				g_tUserHaveTaskIDListChange[holder_id] = true
			end
		end
	end

	-- 登出事件和更新事件不关心
	if action == 'logout' or action == 'delete' then
		return
	end

	if TASK_LIST_UPDATE_CACHE[holder_id] == nil then
		TASK_LIST_UPDATE_CACHE[holder_id] = {}
	end

	if action == 'update' then							   -- 更新任务
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 0
	elseif action == 'accept' then						   -- 接受任务
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 1
	elseif action == 'accomplish' then					   -- 任务完成
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 2
	elseif action == 'complete' then					   -- 任务结束
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 3
	elseif action == 'abort' then						   -- 放弃任务
		TASK_LIST_UPDATE_CACHE[holder_id][task_id] = 4
	end
end

-------------------------------------------------------------------
-- [角色登录] => 读取任务列表
-------------------------------------------------------------------
function TaskListManager_LoginEventHandler(holder_id, flag)

	-- 检测是否刷新操作次数
	systemReflushOptCount(holder_id)

	-- 首胜类任务自动接收
	RequestCanAutoAcceptFirstWinTypeTask(holder_id)
	
	-- 自动接受新手任务
	RequestAutoAcceptTask(holder_id)
	
	-- 检测玩家排位任务
	CheckActorRankTask(holder_id)

	-- 请求任务列表
    RequestTaskList(holder_id)
end


function TaskListManager_TimerHandler()

	local tick = GetTickCountEx()
	for holder_id,tTask in pairs(TASK_LIST_UPDATE_CACHE) do

		-- 向客户端同步任务列表
		if TASK_LIST[holder_id]~=nil then

			local task_info = {}
			local task_finish = {}
			local task_delete = {}
			for task_id,flag in pairs(tTask) do

				if flag == 0 then						-- 更新

					local task = TASK_LIST[holder_id][task_id]
					if task~=nil then
						task_info[task_id] = GetTaskBaseStateInfo(holder_id, task)
					end
				elseif flag == 1 then					-- 接受

					local task = TASK_LIST[holder_id][task_id]
					if task~=nil then
						task_info[task_id] = GetTaskBaseStateInfo(holder_id, task)
					end
				elseif flag == 2 then					-- 完成
				
					local task = TASK_LIST[holder_id][task_id]
					if task~=nil then
						task_info[task_id] = GetTaskBaseStateInfo(holder_id, task)
					end
				elseif flag == 3 then					-- 交付

					table.insert(task_finish, task_id)
				elseif flag == 4 then					-- 放弃

					table.insert(task_delete, task_id)
				end
			end

			-- 完成玩家任务
			local strTaskDeleteInfo = ""
			local bTaskFinish = false
			strTaskDeleteInfo = strTaskDeleteInfo.."<taskfinish>"
			for index,task_id in pairs(task_finish) do
				strTaskDeleteInfo = strTaskDeleteInfo..task_id..";"
				bTaskFinish = true
			end
			strTaskDeleteInfo = strTaskDeleteInfo.."</taskfinish>"
			-- 删除玩家任务
			local bTaskDelete = false
			strTaskDeleteInfo = strTaskDeleteInfo.."<taskdelete>"
			for index,task_id in pairs(task_delete) do
				strTaskDeleteInfo = strTaskDeleteInfo..task_id..";"
				bTaskDelete = true
			end
			strTaskDeleteInfo = strTaskDeleteInfo.."</taskdelete>"
			-- 更新任务列表
			if bTaskFinish or bTaskDelete then
				DeleteTaskList(holder_id, strTaskDeleteInfo)
			end

			-- 更新玩家任务信息
			local bTaskUpdate = false
			local strTaskInfo = ""
			for task_id,info in pairs(task_info) do
				if info~=nil then
					strTaskInfo = strTaskInfo..info
					bTaskUpdate = true
				end
			end
			-- 更新任务列表
			if bTaskUpdate then
				UpdateTaskList(holder_id, strTaskInfo)
			end

			-- 有完成任务
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
-- 请求当前任务列表
-- 将玩家所有已接的任务发给客户端,格式为{任务id,任务名称，任务等级，任务价值，对话npcid,任务类型，任务是否可完成}
-------------------------------------------------------------------
function RequestTaskList(holder_id)

    if holder_id == nil or TASK_LIST[holder_id] == nil then
  		return
    end

	local strTaskInfo = ""
	-- 刷新信息
	local optInfo = GetActorReflushOptInfo(holder_id)
	if optInfo ~= nil then
		strTaskInfo = strTaskInfo..optInfo
	end
	-- 任务详情
	for task_id,task in pairs(TASK_LIST[holder_id]) do
		local info = GetTaskBaseStateInfo(holder_id, task)
		--trace("task_id="..task_id..",info="..info)
		if info~=nil then
			strTaskInfo = strTaskInfo..info
		end
	end

	-- 发送任务列表
	SendTaskList(holder_id, strTaskInfo)
end

-------------------------------------------------------------------
-- 取得当前任务的基础信息，发给客户端
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
	
	-- 首胜任务信息特殊处理
	local strDesc = task.description
	if task.type == TaskType.FirstWinType then
		strDesc = to_utf8("赢得一场比赛可获得宝箱，与联盟成员组队获胜可获得全部奖励")
		local win_count_key = GetWinCountTaskKey(holder_id, task.id)
		local win_count_value = GetTaskIntData(holder_id, win_count_key)
		-- 完成普通首胜任务
		if win_count_value == 1 then
			strDesc = to_utf8("赢得一场比赛可获得宝箱，<color='#a8232e'>与联盟成员组队获胜可获得全部奖励</color>")
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
-- 请求删除任务
-- @param holder_id : 角色DBID
-- @param task_id	: 任务ID
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

	-- 不允许删除
	if oTask.allow_delete == false then
		ShowSystemMessage(holder_id, 18001)
		return false
	end

	if oTask:DeleteTask()==false then
		return false
	end

	--local szText = to_utf8("角色ID:")..holder_id..to_utf8(",角色名字:")..ACTOR_LIST[holder_id]..to_utf8(",放弃任务ID:")..task_id..to_utf8(",任务名字:")..oTask.name
	local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Delete Task ID:"..task_id..",Task Name:"..oTask.name
	AddTaskGameNoteLog(holder_id, szText)

	--放弃了任务,触发事件
    OnActorTaskListChanged(holder_id, task_id, "abort")
    
	TaskFactory:DeleteTask(oTask)
	TASK_LIST[holder_id][task_id] = nil

	-- 真正删除了任务 
    OnActorTaskListChanged(holder_id, task_id, "delete")

	return true
end

-------------------------------------------------------------------
-- 请求可自动接受首胜类型任务
-- @param holder_id : 角色DBID
-- @addition 		: 两种情况检测，1、登陆 2、客户端定时器请求刷新
-- @addition		：符合条件，刷新时间内小于接收次数
-------------------------------------------------------------------
function RequestCanAutoAcceptFirstWinTypeTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return
	end
	
	-- 类型为1表示可接受时自动接任务类型
	if TaskFactory.tTaskTypeList == nil or TaskFactory.tTaskTypeList[TaskType.FirstWinType] == nil then
		error("RequestCanAutoAcceptFirstWinTypeTask not this TaskType"..TaskType.FirstWinType)
		return
	end
	
	for _,task_id in pairs(TaskFactory.tTaskTypeList[TaskType.FirstWinType]) do
	
		local task = TaskFactory.tTaskList[task_id]
		
		if task ~= nil then
		
			task.holder_id = holder_id
			if task:CanAccept(true) == true then
				-- 已有数据直接返回，否则接受
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

					-- 接受任务
					oTask:Accept()

					TASK_LIST[holder_id][task_id] = oTask

					--接受了任务,触发事件
					OnActorTaskListChanged(holder_id, task_id, "accept")

					--local szText = to_utf8("角色ID:")..holder_id..to_utf8(",角色名字:")..ACTOR_LIST[holder_id]..to_utf8(",接受任务ID:")..task_id..to_utf8(",任务名字:")..oTask.name
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
						
						-- 先把任务加入列表
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
-- 请求刷新任务
-- @param holder_id : 角色DBID
-- @param task_id	: 任务类型
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
-- 请求完成任务
-- @param holder_id : 角色DBID
-- @param task_id	: 任务ID
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

	--local szText = to_utf8("角色ID:")..holder_id..to_utf8(",角色名字:")..ACTOR_LIST[holder_id]..to_utf8(",完成任务ID:")..task_id..to_utf8(",任务名字:")..oTask.name
	local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Finish Task ID:"..task_id..",Task Name:"..oTask.name
	AddTaskGameNoteLog(holder_id, szText)
	
	-- 结束任务
	oTask:End()
	
	-- 排位任务:
	-- 1、链式任务获取下一个任务节点（位置不能变,利用End()完成状态接受后置任务，然后DeleteTask()删除掉）
	-- 2、链式任务最后一个不删除
	if oTask.type == TaskType.RankType then
		if oTask.rear_id ~= nil and oTask.rear_id > 0 then
			DealAcceptNextRankTask(holder_id, oTask.rear_id)
		else
			OnActorTaskListChanged(holder_id, task_id,"update")
			return
		end
	end
	
	-- 首胜任务：触发一个更新
	if oTask.type == TaskType.FirstWinType then
		OnActorTaskListChanged(holder_id, task_id,"update")
		return
	end
	
	-- 删除任务
	oTask:DeleteTask()
	
	-- 释放任务内存
	TaskFactory:DeleteTask(oTask)

	-- 置空玩家任务
	TASK_LIST[holder_id][task_id] = nil

	-- 触发一个完成
	OnActorTaskListChanged(holder_id, task_id,"complete")

end

------------------------------------------------------------------------- 交互任务 --------------------------------------------------------------------------

-------------------------------------------------------------------------
-- 请求添加交互任务
-- @param holder_id 	: 角色DBID
-- @param sub_type		: 交互任务子类型
-- @param specific_type	: 交互任务子类型下具体类型
-- @param tag_high32id	: 标签高32位ID
-- @param tag_low32id	: 标签低32位ID
-- @param emotion_words : 心情话语
-------------------------------------------------------------------------
function RequestAddContactTask(holder_id, sub_type, specific_type, tag_high32id, tag_low32id, emotion_words)

	if holder_id == nil or sub_type == nil or specific_type == nil then
		return
	end
	-- 检查发布次数
	local Publish_Count_Key = GetContactPublishCountKey(holder_id)
	local Publish_Count_Value = GetTaskIntData(holder_id, Publish_Count_Key)
	
	if Publish_Count_Value > 0 then
		-- 通知可以添加
		OnCheckAddContactTask(holder_id, sub_type, specific_type, tag_high32id, tag_low32id, emotion_words, 1)
	else
		-- 通知不能添加 
		OnCheckAddContactTask(holder_id, sub_type, specific_type, tag_high32id, tag_low32id, emotion_words, 0)
	end
end

-------------------------------------------------------------------------
-- 成功添加交互任务
-- @param holder_id : 角色DBID
-- @param contact_high32id	: 随机交互ID高32位
-- @param contact_low32id	: 随机交互ID低32位
-------------------------------------------------------------------------
function onSucceedAddContactTask(holder_id, contact_high32id, contact_low32id)

	if holder_id == nil then
		return
	end
	
	-- 更新发布次数
	local Publish_Count_Key = GetContactPublishCountKey(holder_id)
	local Publish_Count_Value = GetTaskIntData(holder_id, Publish_Count_Key)
	Publish_Count_Value = Publish_Count_Value - 1
	SetTaskIntData(holder_id, Publish_Count_Key, Publish_Count_Value);
	
	-- 发送剩余发布次数
	local strTaskInfo = ""
	strTaskInfo = strTaskInfo.."<publishcount>"..Publish_Count_Value.."</publishcount>".."<contacthigh32id>"..contact_high32id.."</contacthigh32id>".."<contactlow32id>"..contact_low32id.."</contactlow32id>"
	SendAddTaskSuccess(holder_id, strTaskInfo)
	
end

-------------------------------------------------------------------------
-- 预请求捕获任务
-- @param holder_id 		: 角色DBID
-- @param contact_high32id	: 随机交互ID高32位
-- @param contact_low32id	: 随机交互ID低32位
-- @param sub_type			: 随机交互任务子类型
-- @param specific_type		: 随机交互任务子类型下具体类型
-- @param publisher_id		: 发布者DBID
-------------------------------------------------------------------------
function RequestPreCatchTask(holder_id, contact_high32id, contact_low32id, sub_type, specific_type, publisher_id)

	if holder_id == nil then
		return
	end

	local strTaskInfo = ""
		
	-- 检查搜索次数
	local Catch_Count_Key = GetTaskCatchCountKey(holder_id)
	local Catch_Count_Value = GetTaskIntData(holder_id, Catch_Count_Key)

	if Catch_Count_Value <= 0 then
		-- 发送预随机结果
		strTaskInfo = strTaskInfo.."<success>".."0".."</success>".."<msgid>".."18014".."</msgid>"
		SendPreCatchTaskResult(holder_id, strTaskInfo)
		return
	end

	-- 检查总任务个数
	local tActorTask = TASK_LIST[holder_id]
	local nActorTaskCount = table_count(tActorTask)

	if nActorTaskCount >= TASK_ACTOR_MAX_COUNT then
		-- 发送预随机结果
		strTaskInfo = strTaskInfo.."<success>".."0".."</success>".."<msgid>".."18015".."</msgid>"
		SendPreCatchTaskResult(holder_id, strTaskInfo)
		return
	end

	-- 检查可捕获类型
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
		
		-- 发送预随机结果

		if GetContactTaskCount(holder_id) >= TASK_CONTACT_ID_MAX_COUNT then
			strTaskInfo = strTaskInfo.."<success>".."0".."</success>".."<msgid>".."18015".."</msgid>"
		else
			strTaskInfo = strTaskInfo.."<success>".."0".."</success>".."<msgid>".."18016".."</msgid>"
		end
		SendPreCatchTaskResult(holder_id, strTaskInfo)
		return
		
	end
	
	-- 1.更新搜索次数
	Catch_Count_Value = Catch_Count_Value - 1
	SetTaskIntData(holder_id, Catch_Count_Key, Catch_Count_Value)
	
	-- 2.随机捕获任务(3个)
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
	
	-- 3.发送预随机结果
	SendPreCatchTaskResult(holder_id, strTaskInfo)
	
end

-------------------------------------------------------------------------
-- 请求捕获任务
-- @param holder_id 		: 角色DBID
-- @param task_type			: 任务类型
-- @param task_id			: 任务ID
-- @param contact_high32id	: 随机交互ID高32位
-- @param contact_low32id	: 随机交互ID低32位
-- @param sub_type			: 交互任务子类型
-- @param specific_type		: 交互任务子类型下具体类型
-- @param publisher_id		: 发布者ID
-------------------------------------------------------------------------
function RequestCatchTask(holder_id, task_type, task_id, contact_high32id, contact_low32id, sub_type, specific_type, publisher_id)

	if holder_id == nil then
		return
	end

	-- 创建任务
	local oTask = nil
	if task_type == TaskType.ContacType then
		oTask = TaskFactory:CreateContactTask(holder_id, sub_type, specific_type, contact_high32id, contact_low32id, publisher_id)
	else
		oTask = TaskFactory:CreateTask(holder_id, task_id)
	end
	
	if oTask ~= nil then
			
		-- 接受任务
		oTask:Accept()
		TASK_LIST[holder_id][oTask.id] = oTask
		-- 接受任务，触发事件
		OnActorTaskListChanged(holder_id, oTask.id, "accept")
		-- 记录日志
		--local szText = to_utf8("角色ID:")..holder_id..to_utf8(", 角色名字:")..ACTOR_LIST[holder_id]..to_utf8(", 接受任务ID:")..oTask.id..to_utf8(", 任务名字:")..oTask.name
		local szText = "Actor ID:"..holder_id..", Actor Name:"..ACTOR_LIST[holder_id]..", Accept Task ID:"..oTask.id..", Task Name:"..oTask.name
		AddTaskGameNoteLog(holder_id, szText)
		
		-- 主线任务，检查是否已完成
		if oTask.type == TaskType.MainType and oTask:CanFinish() == true then						
			oTask:Finish()
			-- 触发事件
			OnActorTaskListChanged(holder_id, oTask.id, "accomplish")
		end
			
	end
	
	-- 发送任务数据
	local strTaskInfo = ""
	
	local info = GetTaskBaseStateInfo(holder_id, oTask)
	if info~=nil then
		strTaskInfo = strTaskInfo..info
	end
	
	SendCatchTaskResult(holder_id, strTaskInfo)
end

-------------------------------------------------------------------------
-- 随机一个可接收的循环任务
-------------------------------------------------------------------------
function RandCycleTypeTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return 0
	end
	
	-- 循环任务任务上限判定
	local nTaskCount = 0
	for	_, task in pairs(TASK_LIST[holder_id]) do
		if task.type == TaskType.CycleType then
			nTaskCount = nTaskCount + 1
		end
	end
	if nTaskCount >= TASK_CYCLE_MAX_COUNT then
		return 0
	end
	
	-- 随机一个循环任务
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
-- 随机一个可接收的主线任务
-------------------------------------------------------------------------
function RandMainTypeTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return 0
	end	
	
	-- 随机一个主线任务
	if TaskFactory.tTaskTypeList == nil	or TaskFactory.tTaskTypeList[TaskType.MainType] == nil then
		error("RandMainTypeTask error. TaskFactory.tTaskTypeList[TaskType.MainType] == nil")
		return 0
	end
	
	local tCanAcceptMainTask = {}
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.MainType]) do
		local task = TaskFactory.tTaskList[task_id]
		if task ~= nil and TASK_LIST[holder_id][task_id] == nil then
			task.holder_id = holder_id
			-- 主线任务完成后不可再次接收
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
-- 随机一个可接收的日常任务
-------------------------------------------------------------------------
function RandDailyTypeTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return 0
	end	
	
	-- 日常任务每天只可接受一次
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
	
	
	-- 随机一个日常任务
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
-- 取得当前用户的刷新信息
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
	
	-- 剩余刷新时间
	local reflush_time_key = GetReflushTimeKey(holder_id)
	local reflush_time_value = GetTaskIntData(holder_id, reflush_time_key)
	local left_reflush_time =  reflush_time_value + (3600*TASK_REFLUSH_TIME_INTERVAL) - os.time()
	strOptInfo = strOptInfo.."<reflushtime>"..left_reflush_time.."</reflushtime>"
	
	return strOptInfo

end

-------------------------------------------------------------------------
-- 设置刷新玩家发布、获取任务定时器
-- @addition 	: TASK_REFLUSH_TIME_INTERVAL的整数倍时间点刷一次，
-- @addition 	: 起服时开启一个剩余时间定时器，后续开启TASK_REFLUSH_TIME_INTERVAL时间定时器
-------------------------------------------------------------------------
function SetActorOptReflushTimer()

	local nFlushSec = 3600 * TASK_REFLUSH_TIME_INTERVAL
	local nDate = os.date("*t", os.time())
	local nTime = nFlushSec - (nDate.hour * 3600 + nDate.min * 60 + nDate.sec) % nFlushSec
	local nSceneID = GetLuaSceneID()
	g_nTaskReFlushOptTimer = CreateTimerTrigger(nSceneID, "TasklistManager_FlushTimerHandler", nTime*1000, 0)
		
end

function TasklistManager_FlushTimerHandler()
	
	-- 刷新在线玩家发布次数、捕获次数、刷新次数、发布时间
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
			
			-- 更新客户端
			local strOptInfo = GetActorReflushOptInfo(holder_id)
			if strOptInfo ~= nil then
				SendReflushOptInfo(holder_id, strOptInfo)
			end
			
		end
		
	end
	
	-- 后续执行TASK_REFLUSH_TIME_INTERVAL小时定时器
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
-- 玩家登陆，系统检测是否刷新操作次数
-------------------------------------------------------------------------
function systemReflushOptCount(holder_id)
	
	local reflush_time_key = GetReflushTimeKey(holder_id)
	local reflush_time_value = GetTaskIntData(holder_id, reflush_time_key)
	local nTime = os.time()
	
	-- 检测是否刷新
	local bFlush = false
	if reflush_time_value == TASK_DATA_INVALID then
		bFlush = true
	else 
		if nTime - reflush_time_value >= 3600*TASK_REFLUSH_TIME_INTERVAL then
			bFlush = true
		end
	end
	
	-- 执行刷新操作
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
	
		-- 实际刷新时间记为上次整点时间
		local nDate = os.date("*t", nTime)
		local nCorrectTime = nTime - (nDate.hour * 3600 + nDate.min * 60 + nDate.sec) % (3600*TASK_REFLUSH_TIME_INTERVAL)
		SetTaskIntData(holder_id, reflush_time_key, nCorrectTime)
	
	end

end

-------------------------------------------------------------------------
-- 手动刷新操作次数
-------------------------------------------------------------------------
function RequestReFlushOptCount(holder_id)
	
	-- 判次数
	local reflush_count_key = GetManualReflushCountKey(holder_id)
	local reflush_count_value = GetTaskIntData(holder_id, reflush_count_key)
	if reflush_count_value <= 0 then
		ShowSystemMessage(holder_id, 18011)
		return
	end
	
	-- 扣钱
	local bResult = CheckAddMoneyOfReflushOpt(holder_id)
	if not bResult then
		ShowSystemMessage(holder_id, 18012)
		return
	end
	
	-- 减次数
	reflush_count_value = reflush_count_value - 1
	SetTaskIntData(holder_id, reflush_count_key, reflush_count_value)
	
	-- 刷新
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
	
	-- 下发 
	local strOptInfo = GetActorReflushOptInfo(holder_id)
	if strOptInfo ~= nil then
		SendReflushOptInfo(holder_id, strOptInfo)
	end

end

-------------------------------------------------------------------------
-- 已拥有交互任务数量
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
-- 自动接受新手任务
-- @param holder_id : 角色DBID
-------------------------------------------------------------------
function RequestAutoAcceptTask(holder_id)

	if holder_id == nil or TASK_LIST[holder_id] == nil then
		return
	end
	
	-- 可自动接受任务类型(有补充可自己添加)
	local tAutoAcceptID = {10,31}

	for _,task_id in pairs(tAutoAcceptID) do
	
		local task = TaskFactory.tTaskList[task_id]
		if task ~= nil and TASK_LIST[holder_id][task_id] == nil then
		
			task.holder_id = holder_id
			-- 主线任务完成后不可再次接收
			local task_type = GetTaskIDKey(holder_id, task_id)
			local task_value = GetTaskIntData(holder_id, task_type)
			if task_value == TASK_DATA_INVALID and task:CanAccept(true) == true then
			
				local oTask = TaskFactory:CreateTask(holder_id, task_id)
				if oTask ~= nil then
					-- 接受任务
					oTask:Accept()

					TASK_LIST[holder_id][task_id] = oTask

					--接受了任务,触发事件
					OnActorTaskListChanged(holder_id, task_id, "accept")

					--local szText = to_utf8("角色ID:")..holder_id..to_utf8(",角色名字:")..ACTOR_LIST[holder_id]..to_utf8(",接受任务ID:")..task_id..to_utf8(",任务名字:")..oTask.name
					local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Accept Task ID:"..task_id..",Task Name:"..oTask.name
					AddTaskGameNoteLog(holder_id, szText)
					
				end
			end
			task.holder_id = nil
		end
	end
end

-------------------------------------------------------------------
-- 奖励任务获取次数
-- @param holder_id : 角色DBID
-- @param prize_num : 奖励次数
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
	
	-- 下发 
	local strOptInfo = GetActorReflushOptInfo(holder_id)
	if strOptInfo ~= nil then
		SendReflushOptInfo(holder_id, strOptInfo)
	end

end

------------------------------------------------------------------------- 排位任务 --------------------------------------------------------------------------

-------------------------------------------------------------------------
-- 检测玩家排位任务（没有赋值头节点，有判断是否新的一周）
-- @param holder_id 	: 角色DBID
-------------------------------------------------------------------------
function CheckActorRankTask(holder_id)

	if holder_id == nil	or TASK_LIST[holder_id] == nil then
		return
	end

	if TaskFactory.tTaskTypeList == nil	or TaskFactory.tTaskTypeList[TaskType.RankType] == nil then
		return
	end
	
	local tRankTask = {}
	-- 添加缓存任务
	for _, task_id in pairs(TaskFactory.tTaskTypeList[TaskType.RankType]) do
		local task = TASK_LIST[holder_id][task_id]
		if task ~= nil then
			table.insert(tRankTask, task_id)
		end
	end
	-- 链式任务最后一个领取状态，没有读取到内存，手动添加
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
	
	-- 排位链式任务数量不会多于1个
	if nRankTaskCount > 1 then
		error("CheckRankTaskOnLogin error. nRankTaskCount > 1, nActorID = "..holder_id)
		return
	end
	
	-- 未获取
	if nRankTaskCount == 0 then
		DealAutoAcceptRankTask(holder_id)
	-- 已获取更新
	elseif nRankTaskCount == 1 then
		DealWeekClearRankTask(holder_id, tRankTask[1])
	end
end


-------------------------------------------------------------------------
-- 处理自动接收排位任务（由于前置任务的限制，只会获取链式任务的头任务）
-- @param holder_id 	: 角色DBID
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
					-- 接受任务
					oTask:Accept()
					TASK_LIST[holder_id][task_id] = oTask

					--接受了任务,触发事件
					OnActorTaskListChanged(holder_id, task_id, "accept")
					local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Accept Task ID:"..task_id..",Task Name:"..oTask.name
					AddTaskGameNoteLog(holder_id, szText)
					
					-- 更新排位任务获取时间
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
-- 处理接收下一个排位任务
-- @param holder_id 	: 角色DBID
-- @param task_id		: 任务ID
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
				-- 接受任务
				oTask:Accept()
				TASK_LIST[holder_id][task_id] = oTask

				--接受了任务,触发事件
				OnActorTaskListChanged(holder_id, task_id, "accept")
				local szText = "Actor ID:"..holder_id..",Actor Name:"..ACTOR_LIST[holder_id]..",Accept Task ID:"..task_id..",Task Name:"..oTask.name
				AddTaskGameNoteLog(holder_id, szText)
				
				-- 更新排位任务获取时间
				local nNowTime = os.time()
				local nReflushTimeKey = GetRankReflushTimeKey(holder_id)
				SetTaskIntData(holder_id, nReflushTimeKey, nNowTime)
				
			end
		end
		task.holder_id = nil
	end
	
end

-------------------------------------------------------------------------
-- 处理排位任务的周清理
-- @param holder_id 	: 角色DBID
-- @param task_id		: 任务ID
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
	
	-- 同一周
	local nResult = DiffWeek(nReflushTimeValue, nNowTime)
	if nResult <= 0 then
		return
	end
	
	-- 1.发送奖励
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
	
	-- 2.清理工作
	RemoveTaskData(holder_id, nReflushTimeKey)
	
	oTask:DeleteTask()
	
	TaskFactory:DeleteTask(oTask)
	TASK_LIST[holder_id][task_id] = nil
	
	-- 3.重新获取链式任务头
	DealAutoAcceptRankTask(holder_id)

end

-------------------------------------------------------------------------
-- 请求排位任务周清理
-------------------------------------------------------------------------
function RequestClearRankTask()

	if ACTOR_LIST == nil then
		return
	end

	for actor_id,_ in pairs(ACTOR_LIST) do
		CheckActorRankTask(actor_id)
	end
	
end






