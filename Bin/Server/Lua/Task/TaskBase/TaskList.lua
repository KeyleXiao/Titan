-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	12/1/2016
-- 版  本:	1.0
-- 描  述:	任务列表
-------------------------------------------------------------------

require "TaskFactory"
require "TaskFramePlayDotaGame"
require "TaskFrameActorData"

-------------------------------------------------------------------
-- 玩家任务列表
TASK_LIST = {}         -- 存储格式 [玩家ID][{任务ID=任务对象,任务ID=任务对象,...}]

-- 玩家列表
ACTOR_LIST = {}			-- 存储格式 [玩家ID]={玩家名字}

-- 登陆触发器
g_LoginTriggerID = 0

-------------------------------------------------------------------


-------------------------------------------------------------------
-- 2013-3-5任务优化处理
-- 优化控制标识，true=开启优化,false=关闭优化
g_UserTaskOptimizeOpen = false

-- 记录玩家当前拥有任务ID，并每个场景服备份，用于优化玩家登陆，玩家PDBID为键值
g_tUserHaveTaskIDList = {}

-- 玩家任务有改变，下线需广播其他场景备份，玩家PDBID为键值
g_tUserHaveTaskIDListChange ={}
-------------------------------------------------------------------


-------------------------------------------------------------------
-- 任务列表管理
-------------------------------------------------------------------
function InitTaskList()

	local scene_id = GetLuaSceneID()
    g_LoginTriggerID = CreateLoginTrigger(scene_id, "ActorLoginHandler")
    if g_LoginTriggerID == 0 then
		error("InitTaskList CreateLoginTrigger fail")
		return false
    end
	
	g_WarPersonTaskTrigger = CreateWarPersonTaskTrigger(scene_id, "SetPlayDotaGameEvent")
	if g_WarPersonTaskTrigger == 0 then
		error("InitTaskList CreateWarPersonTaskTrigger fail")
		return false
	end
	
	g_ActorDataTaskTrigger = CreateActorTaskTrigger(scene_id, "SetActorDataGameEvent")
	if g_ActorDataTaskTrigger == 0 then
		error("InitTaskList CreateActorTaskTrigger fail")
		return false
	end

	return true
end

-------------------------------------------------------------------
-- 用户登陆时载入任务数据
-- @param holder_id : PDBID
-- @param flag		: 1真正的用户上线
-------------------------------------------------------------------
function ActorLoginHandler(holder_id, flag, holder_name)

	ACTOR_LIST[holder_id] = holder_name

	if TASK_LIST[holder_id] == nil then
		TASK_LIST[holder_id] = {}
	else
	    return
	end

	-- 是否广播
	local bBroadcast = false

	-- 重新登陆重新读取任务
	if flag == 1 or not g_UserTaskOptimizeOpen or g_tUserHaveTaskIDList[holder_id] == nil then

		-- 优化控制标识，true=开启优化,false=关闭优化
		if g_UserTaskOptimizeOpen then
			-- 初始化表
			g_tUserHaveTaskIDList[holder_id] = {}
		end

		for task_id,task_data in pairs(TaskFactory.tTaskList) do

			local task_key = GetTaskIDKey(holder_id, task_id)
			local task_state = GetTaskIntData(holder_id, task_key)

			if task_state ~= TASK_DATA_INVALID and task_state <= TaskState.FinishState then

				local status,result = pcall(TryLoadActorTask, holder_id, task_id)
				if not status then
					RemoveTaskData(holder_id, task_key)
					if result~=nil then
						error("ActorLoginHandler load fail holder_id="..holder_id..",task_id="..task_id..",result="..result)
					end
				else
					-- 优化控制标识，true=开启优化,false=关闭优化
					if g_UserTaskOptimizeOpen then
						-- 该玩家任务ID为当前拥有任务
						g_tUserHaveTaskIDList[holder_id][task_id] = true

						-- 设置广播同步标志
						bBroadcast = true
					end
				end
			end
		end

		-- 登陆有需要广播任务时将广播到其他场景服
		if bBroadcast then

			local strMsg = nil
			if g_tUserHaveTaskIDList[holder_id] == nil then
				strMsg = nil
			else

				local tUserHaveTaskIDList = {}
				for task_id, nValue in g_tUserHaveTaskIDList[holder_id] do
					table.insert(tUserHaveTaskIDList, task_id)
				end
				strMsg = tableToString(tUserHaveTaskIDList)
			end
			-- 就算没有任务也要同步数据
			--ZoneServerBroadcast("TaskList_SetUserHaveTaskIDList", nil, holder_id, strMsg)
		end
	else
		for task_id, value in pairs(g_tUserHaveTaskIDList[holder_id]) do
			local task_data = TaskFactory.tTaskList[task_id]
			if task_data ~= nil then

				local task_key = GetTaskIDKey(holder_id, task_id)
				local task_state = GetTaskIntData(holder_id, task_key)
				if task_state ~= TASK_DATA_INVALID and task_state <= TaskState.FinishState then

					local status,result = pcall(TryLoadActorTask, holder_id, task_id)
					if not status then

						RemoveTaskData(holder_id, task_key)
						if result~=nil then
							error("ActorLoginHandler have load fail holder_id="..holder_id..",task_id="..task_id..",result="..result)
						end
					end
				end
			end
		end
	end

    -- 发出事件
    g_luaEventPublisher:FireEvent(LUA_EVENT_ACTOR_LOGIN, holder_id, flag)
end

-------------------------------------------------------------------
-- 用户退出时保存任务数据
-- @param holder_id : PDBID
-- @param flag		: 1真正的用户下线
-------------------------------------------------------------------
function ActorLogoutHandler(holder_id, flag)

    if TASK_LIST[holder_id] == nil then
  		return
    end

	 -- 发出事件
    g_luaEventPublisher:FireEvent(LUA_EVENT_ACTOR_LOGOUT, holder_id, flag)

	for task_id,task in pairs(TASK_LIST[holder_id]) do

		task:OnLogout(holder_id)

		-- 不保存的任务下线要删除
		if task.save==false then
			task:DeleteTask()
		end

		-- 触发事件
        OnActorTaskListChanged(holder_id, task_id, "logout")
		TaskFactory:DeleteTask(task)
	end

	-- 优化控制标识，true=开启优化,false=关闭优化,同场景切地图不发送
	if g_UserTaskOptimizeOpen then
		-- 需要广播时和真正下线时
		if g_tUserHaveTaskIDListChange[holder_id] == true  or flag == 1 then
			local strMsg = nil
			-- 玩家真正下线时，清空所有场景该玩家任务备份
			if g_tUserHaveTaskIDList[holder_id] == nil or flag == 1 then
				strMsg = nil
			else
				local tUserHaveTaskIDList = {}
				for task_id, nValue in pairs(g_tUserHaveTaskIDList[holder_id]) do
					table.insert(tUserHaveTaskIDList, task_id)
				end
				strMsg = tableToString(tUserHaveTaskIDList)
			end
			-- 置空缓存任务
			g_tUserHaveTaskIDList[holder_id] = nil
			-- 就算没有任务也要同步数据
			--ZoneServerBroadcast("TaskList_SetUserHaveTaskIDList", nil, holder_id, strMsg)
		end

		-- 置空下线广播标志
		g_tUserHaveTaskIDListChange[holder_id] = nil
	end

    TASK_LIST[holder_id]=nil

	ACTOR_LIST[holder_id] = nil

    -- 清掉订阅列表，这样即使有些模块漏掉了退订事件的代码也不会出错
    TASK_LIST_EVENT_HANDLER[holder_id]=nil

	-- 清空玩家任务数据注册
	TASK_DATA[holder_id] = nil
end

-------------------------------------------------------------------
-- 尝试载入玩家任务
-- @param holder_id : PDBID
-- @param task_id	: 任务ID
-------------------------------------------------------------------
function TryLoadActorTask(holder_id, task_id)

	local task = TaskFactory:CreateTask(holder_id, task_id)
    if task == nil then
		local task_key = GetTaskIDKey(holder_id, task_id)
    	RemoveTaskData(holder_id, task_key)
		warning("TryLoadActorTask error task_id="..task_id)
		return
	end

	-- 任务登陆
	task:OnLogin()
	
	-- 检查是否完成
	if task:CanFinish() == true then
		task:Finish()
	end

	if task:GetState() <= TaskState.FinishState then
		TASK_LIST[holder_id][task_id]=task
	else
		warning("TryLoadActorTask error finish task_id="..task_id)
	end

	-- 触发事件
	OnActorTaskListChanged(holder_id, task_id, "login")
end

-------------------------------------------------------------------
-- 设置玩家当前拥有任务ID
-- @param holder_id : PDBID
-- @param strUserHaveTaskIDList	: 拥有任务ID列表
-------------------------------------------------------------------
function TaskList_SetUserHaveTaskIDList(holder_id, strUserHaveTaskIDList)

	-- 检测参数
	if holder_id == nil or holder_id <= 0 then
		return
	end

	if strUserHaveTaskIDList == nil or strUserHaveTaskIDList == 0 then
		-- 置空
		g_tUserHaveTaskIDList[holder_id] = nil
		return
	end

	-- 转化为table
	local tUserHaveTaskIDList = stringToTable(strUserHaveTaskIDList)
	if tUserHaveTaskIDList == nil then
		return
	end

	-- 清空table表
	g_tUserHaveTaskIDList[holder_id] = {}

	for index,task_id in pairs(tUserHaveTaskIDList) do
		g_tUserHaveTaskIDList[holder_id][task_id] = true
	end
end

-------------------------------------------------------------------
-- 实现任务列表变化事件
-- 订阅玩家holder_id
-- 订阅任务的ID task_id
-- 执行的事件 handler
-- 订阅接受事件或完成或放弃事件 accept不为true即订阅销毁事件
-------------------------------------------------------------------
TASK_LIST_EVENT_HANDLER =
{
}


-------------------------------------------------------------------
-- 注册任务列表变化事件
-------------------------------------------------------------------
function RegisterTaskListChangeEvent(holder_id, task_id, handler, action)

	if TASK_LIST_EVENT_HANDLER[holder_id]==nil then
		TASK_LIST_EVENT_HANDLER[holder_id] = {}
	end

	if TASK_LIST_EVENT_HANDLER[holder_id][task_id] == nil then
	    TASK_LIST_EVENT_HANDLER[holder_id][task_id] = {}
	end

	if TASK_LIST_EVENT_HANDLER[holder_id][task_id][action] == nil then
	    TASK_LIST_EVENT_HANDLER[holder_id][task_id][action] = {}
	end

	TASK_LIST_EVENT_HANDLER[holder_id][task_id][action][handler] = handler
end

-------------------------------------------------------------------
-- 取消任务列表变化某任务的所有订阅
-- 订阅玩家holder_id
-------------------------------------------------------------------
function UnregisterTaskAllEvent(holder_id)

	if TASK_LIST_EVENT_HANDLER[holder_id] ~= nil then
		TASK_LIST_EVENT_HANDLER[holder_id] = nil
	end
end

-------------------------------------------------------------------
-- 取消任务列表变化事件订阅
-- 订阅玩家holder_id
-- 订阅任务的ID task_id
-- 取消订阅接受事件或完成或放弃事件 accept不为true即订阅销毁事件
-------------------------------------------------------------------
function UnregisterTaskListChangeEvent(holder_id, task_id, action, handler)

	if TASK_LIST_EVENT_HANDLER[holder_id] ~= nil then
		if TASK_LIST_EVENT_HANDLER[holder_id][task_id] ~= nil then
			if TASK_LIST_EVENT_HANDLER[holder_id][task_id][action] ~= nil then
				TASK_LIST_EVENT_HANDLER[holder_id][task_id][action][handler] = nil
			end
		end
	end
end

-- 事件订阅访问路径
TASK_LIST_EVENT_TRAVEL = {}

-------------------------------------------------------------------
-- 当玩家任务列表产生变化,使用已订阅的事件
-- 订阅玩家holder_id
-- 订阅任务的ID task_id
-------------------------------------------------------------------
function OnActorTaskListChanged(holder_id, task_id, action)

     -- 遍历列表
     TASK_LIST_EVENT_TRAVEL =
     {
		{holder_id,task_id,action},
		{0,0,0},
		{0,0,action},
		{0,task_id,0},
		{holder_id,0,0},
		{holder_id,task_id,0},
		{holder_id,0,action},
		{0,task_id,action},
     }

     for i,key in pairs(TASK_LIST_EVENT_TRAVEL) do
		if TASK_LIST_EVENT_HANDLER[ key[1] ]~=nil and TASK_LIST_EVENT_HANDLER[ key[1] ][ key[2] ]~=nil and TASK_LIST_EVENT_HANDLER[ key[1] ][ key[2] ][ key[3] ]~=nil then
			for k,v in pairs(TASK_LIST_EVENT_HANDLER[ key[1] ][ key[2] ][ key[3] ]) do
				SafeCall(v, holder_id, task_id, action)
			end
		end
     end
end