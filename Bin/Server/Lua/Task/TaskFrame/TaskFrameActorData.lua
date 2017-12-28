-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	易耀辉
-- 日  期:	3/20/2017
-- 版  本:	1.0
-- 描  述:	角色数据
-------------------------------------------------------------------

require "Task"


-- 角色数据触发器
g_ActorDataTaskTrigger = 0
-- 角色数据事件任务注册
g_tActorGameEvent = {}

-------------------------------------------------------------------
-- 角色数据
-------------------------------------------------------------------
TaskFrameActorData = Task:new
{
	
}

-------------------------------------------------------------------
-- 构造函数
-------------------------------------------------------------------
function TaskFrameActorData:construct()

	-- 注册事件处理函数
	self.tGameEventFunList[TaskGameEventType.EventType_LEVEL] 		= self.OnEventNormalProcess
	self.tGameEventFunList[TaskGameEventType.EventType_ADDHERO] 	= self.OnEventNormalProcess
	self.tGameEventFunList[TaskGameEventType.EventType_ADDSKIN] 	= self.OnEventNormalProcess
	self.tGameEventFunList[TaskGameEventType.EventType_HEROUPGRADE] = self.OnEventNormalProcess
	self.tGameEventFunList[TaskGameEventType.EventType_BUIDMENTORSHIP] = self.OnEventNormalProcess
	
	self.tGameEventFunList[TaskGameEventType.EventType_APPLYFRIEND] = self.OnEventNormalProcess
	self.tGameEventFunList[TaskGameEventType.EventType_CONTACTREPLY]= self.OnEventNormalProcess
	self.tGameEventFunList[TaskGameEventType.EventType_PK] 			= self.OnEventNormalProcess
	self.tGameEventFunList[TaskGameEventType.EventType_NovideGuide] = self.OnEventNormalProcess
	
	self.tGameEventFunList[TaskGameEventType.EventType_INLAYGEM] 	= self.OnEventInlayGem
	self.tGameEventFunList[TaskGameEventType.EventType_OPENCHEST] 	= self.OnEventOpenChest
	self.tGameEventFunList[TaskGameEventType.EventType_CLANDONATE] 	= self.OnEventClanDonate
	
	-- 完成条件:拥有多少个英雄
	if self.finish_condition_hero_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionHaveHeroCount:new{count=self.finish_condition_hero_count})
	end

	-- 完成条件:拥有多少个英雄皮肤
	if self.finish_condition_heroskin_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionHaveSkinCount:new{count=self.finish_condition_heroskin_count})
	end
	
	-- 完成条件:角色等级
	if self.finish_condition_level ~= nil then
		InsertUni(self.tFinishCondition, ConditionLevel:new{min_level=self.finish_condition_level, max_level=self.finish_condition_level})
	end
	
	-- 完成条件:打开宝箱个数
	if self.finish_condition_open_chest_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionOpenChestCount:new{count=self.finish_condition_open_chest_count})
	end
	
	-- 完成条件:联盟捐献数
	if self.finish_condition_clan_donate_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionClanDonateCount:new{count=self.finish_condition_clan_donate_count})
	end
end

-- 角色登陆
function TaskFrameActorData:OnLogin()

	-- 接受任务
	Task.OnLogin(self)

	if self:GetState() < TaskState.FinishState then
		-- 注册角色数据事件
		self:RegisterActorGameEvent()
	end
end

-- 角色退出
function TaskFrameActorData:OnLogout()
	local actor = self.holder_id
	if actor == nil or actor <= 0 then
		return
	end

	Task.OnLogout()

	-- 清空角色内存数据
	if g_tActorGameEvent[actor] ~= nil then
		g_tActorGameEvent[actor] = nil
	end
end


-- 接受任务
function TaskFrameActorData:Accept()

	-- 接受任务
	Task.Accept(self)

	if self:GetState() < TaskState.FinishState then
		-- 注册角色数据事件
		self:RegisterActorGameEvent()
	end
end

-- 完成任务
function TaskFrameActorData:Finish()

	-- 完成任务
	Task.Finish(self)

	-- 注销角色数据事件
	self:UnregisterActorGameEvent()
end

-- 结束任务
function TaskFrameActorData:End()

	-- 结束任务
	Task.End(self)

	-- 注销角色数据事件
	self:UnregisterActorGameEvent()
end

-- 删除任务
function TaskFrameActorData:DeleteTask()

	-- 删除任务
	Task.DeleteTask(self)

	-- 注销角色数据事件
	self:UnregisterActorGameEvent()
end

-- 注册角色数据事件
function TaskFrameActorData:RegisterActorGameEvent()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if self.id == nil or self.id <= 0 then
		return
	end
	
    if g_tActorGameEvent[self.holder_id] == nil then
        g_tActorGameEvent[self.holder_id] = {}
    end
	
	for index,condition in pairs(self.tFinishCondition) do
		local game_event = condition:GetRegisterGameEvent()
		if game_event ~= nil then
			if g_tActorGameEvent[self.holder_id][game_event] == nil then
				g_tActorGameEvent[self.holder_id][game_event] = {}
			end
			if g_tActorGameEvent[self.holder_id][game_event][self.id] == nil then
				g_tActorGameEvent[self.holder_id][game_event][self.id] = self
			end			
		end
	end
end

-- 注销角色数据事件
function TaskFrameActorData:UnregisterActorGameEvent()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if self.id == nil or self.id <= 0 then
		return
	end
	
    if g_tActorGameEvent[self.holder_id] == nil then
        return
    end
	
	for index,condition in pairs(self.tFinishCondition) do
		local game_event = condition:GetRegisterGameEvent()
		if game_event ~= nil then
			if g_tActorGameEvent[self.holder_id][game_event] ~= nil then
				g_tActorGameEvent[self.holder_id][game_event][self.id] = nil
			end
		end
	end
end

-- 玩家一般响应处理
function TaskFrameActorData:OnEventNormalProcess(tGameData)
	
	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	-- 判断是否完成
	if self:CanFinish() == true then
		self:Finish()
		-- 触发事件
		OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
	else
		self:Update()
	end
end

----------------------------------------------------------------------------
-- 镶嵌宝石
----------------------------------------------------------------------------
function TaskFrameActorData:OnEventInlayGem(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	if tGameData == nil then
		return
	end
	
	local task_key = GetInlayCrownGemCountTaskKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- 设置镶嵌宝石
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	-- 判断是否完成
	if self:CanFinish() == true then
		
		self:Finish()
		-- 触发事件
		OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
	else
		self:Update()
	end
end

----------------------------------------------------------------------------
-- 开宝箱
----------------------------------------------------------------------------
function TaskFrameActorData:OnEventOpenChest(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	if tGameData == nil then
		return
	end
	
	local task_key = GetOpenChestCountTaskKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- 设置开宝箱个数
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	-- 判断是否完成
	if self:CanFinish() == true then
		
		self:Finish()
		-- 触发事件
		OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
	else
		self:Update()
	end
end

----------------------------------------------------------------------------
-- 联盟捐赠
----------------------------------------------------------------------------
function TaskFrameActorData:OnEventClanDonate(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	if tGameData == nil then
		return
	end
	
	local task_key = GetClanDonateCountTaskKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- 设置联盟捐赠次数
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	-- 判断是否完成
	if self:CanFinish() == true then
		
		self:Finish()
		-- 触发事件
		OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
	else
		self:Update()
	end
end

-- 响应玩家数据事件
function TaskFrameActorData:OnActorGameEvent(nEvent, tGameData)

	if self.tGameEventFunList[nEvent] == nil then
		error("OnGameEvent faild" .. nEvent)
		return
	end
	
	-- 处理事件响应函数
	self.tGameEventFunList[nEvent](self, tGameData)
end

-- 设置玩家数据游戏事件
function SetActorDataGameEvent(nEventID, dwPDBID, nParam1, nParam2, nParam3)

	if dwPDBID == nil or dwPDBID <= 0 then
		return
	end

	if g_tActorGameEvent == nil or g_tActorGameEvent[dwPDBID] == nil then
		return
	end
	local tDataInfo = g_tActorGameEvent[dwPDBID][nEventID]
	if tDataInfo == nil then
		return
	end

	-- bool类型c++传入会转换成int
	local tGameData = {}
	tGameData.nParam1 = nParam1
	tGameData.nParam2 = nParam2
	tGameData.nParam3 = nParam3

	for id,task in pairs(tDataInfo) do
		task:OnActorGameEvent(nEventID, tGameData)
	end
end
