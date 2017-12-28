-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/30/2016
-- 版  本:	1.0
-- 描  述:	首胜任务
-------------------------------------------------------------------

require "Task"

-- 战场人物任务数据触发器
g_WarPersonTaskTrigger = 0
-- 玩DOTA类游戏触发器
g_tPlayDotaGame = {}

-------------------------------------------------------------------
-- 首胜
-------------------------------------------------------------------
TaskFramePlayDotaGame = Task:new
{

}

-------------------------------------------------------------------
-- 构造函数
-------------------------------------------------------------------
function TaskFramePlayDotaGame:construct()

	-- 注册事件处理函数
	self.tGameEventFunList[TaskGameEventType.EventType_FINISHMATCH] = self.OnEventPlayDotaGame
	
	-- 完成条件:胜利次数
	if self.finish_condition_win_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionWinCount:new{count=self.finish_condition_win_count})
	end

	-- 完成条件:完成次数
	if self.finish_condition_finish_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionFinishCount:new{count=self.finish_condition_finish_count})
	end
	
	-- 完成条件:角色比赛总场次
	if self.finish_condition_match_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionMatchCount:new{count=self.finish_condition_match_count})
	end
	
end

-- 角色登陆
function TaskFramePlayDotaGame:OnLogin()

	-- 接受任务
	Task.OnLogin(self)

	if self:GetState() < TaskState.FinishState then
		-- 注册玩DOTA游戏事件
		self:RegisterPlayDotaGame()
	end
end

-- 角色退出
function TaskFramePlayDotaGame:OnLogout()
	local actor = self.holder_id
	if actor == nil or actor <= 0 then
		return
	end

	Task.OnLogout()

	-- 清空角色内存数据
	if g_tPlayDotaGame[actor] ~= nil then
		g_tPlayDotaGame[actor] = nil
	end
end


-- 接受任务
function TaskFramePlayDotaGame:Accept()

	-- 接受任务
	Task.Accept(self)

	if self:GetState() < TaskState.FinishState then
		-- 注册玩DOTA游戏事件
		self:RegisterPlayDotaGame()
	end
end

-- 完成任务
function TaskFramePlayDotaGame:Finish()

	-- 完成任务
	Task.Finish(self)

	-- 注销玩DOTA游戏事件
	self:UnregisterPlayDotaGame()
end

-- 结束任务
function TaskFramePlayDotaGame:End()

	-- 结束任务
	Task.End(self)

	-- 注销玩DOTA游戏事件
	self:UnregisterPlayDotaGame()
end

-- 删除任务
function TaskFramePlayDotaGame:DeleteTask()

	-- 删除任务
	Task.DeleteTask(self)

	-- 注销玩DOTA游戏事件
	self:UnregisterPlayDotaGame()
end

-- 注册玩DOTA游戏事件
function TaskFramePlayDotaGame:RegisterPlayDotaGame()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if self.id == nil or self.id <= 0 then
		return
	end
	
    if g_tPlayDotaGame[self.holder_id] == nil then
        g_tPlayDotaGame[self.holder_id] = {}
    end
	
	for index,condition in pairs(self.tFinishCondition) do
		local game_event = condition:GetRegisterGameEvent()
		if game_event ~= nil then
			if g_tPlayDotaGame[self.holder_id][game_event] == nil then
				g_tPlayDotaGame[self.holder_id][game_event] = {}
			end
			
			if g_tPlayDotaGame[self.holder_id][game_event][self.id] == nil then
				g_tPlayDotaGame[self.holder_id][game_event][self.id] = self
			end			
		end
	end
end

-- 注销玩DOTA游戏事件
function TaskFramePlayDotaGame:UnregisterPlayDotaGame()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if self.id == nil or self.id <= 0 then
		return
	end
	
    if g_tPlayDotaGame[self.holder_id] == nil then
        return
    end
	
	for index,condition in pairs(self.tFinishCondition) do
		local game_event = condition:GetRegisterGameEvent()
		if game_event ~= nil then
			if g_tPlayDotaGame[self.holder_id][game_event] ~= nil then
				g_tPlayDotaGame[self.holder_id][game_event][self.id] = nil
			end
		end
	end
end

-- 玩家一般响应处理
function TaskFramePlayDotaGame:OnEventNormalProcess(tGameData)

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
-- 玩DOTA游戏执行

-- param1 : 队友列表
-- param2 : 比赛结果统计
-- param2解析：1.英雄类型2.比赛类型
-- 			   3.是否联盟组队4.是否师徒组队5.是否胜利6.是否超神7.是否MVP8.是否输出最高9.是否拆塔最高10.是否助攻最高11.是否金钱最高
-- 			   12.最大连杀数量13.最大不死击杀数量14.击杀数量15.助攻数量16.战绩评分17.获得金钱数量18.决胜控制数量19.惊险救治数量20.拆塔数量21.击杀小龙数量22.击杀大龙数量
----------------------------------------------------------------------------
function TaskFramePlayDotaGame:OnEventPlayDotaGame(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	-- 胜利次数
	if self.finish_condition_win_count ~= nil then
		-- 取得玩家胜利次数任务数据
		local task_key = GetWinCountTaskKey(self.holder_id, self.id)
		local task_value = GetTaskIntData(self.holder_id, task_key)
		if task_value == TASK_DATA_INVALID then
			task_value = 0
		end
		-- 设置玩家完成次数
		SetTaskIntData(self.holder_id, task_key, task_value+1)
	end

	-- 完成次数
	if self.finish_condition_finish_count ~= nil then
		-- 取得玩家完成次数任务数据
		local task_key = GetFinishCountTaskKey(self.holder_id, self.id)
		local task_value = GetTaskIntData(self.holder_id, task_key)
		if task_value == TASK_DATA_INVALID then
			task_value = 0
		end
		-- 设置玩家完成次数
		SetTaskIntData(self.holder_id, task_key, task_value+1)
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

-- 响应游戏事件
function TaskFramePlayDotaGame:OnGameEvent(nEventID, tGameData)
	if self.tGameEventFunList[nEventID] == nil then
		error("OnGameEvent faild" .. nEventID)
		return
	end
	
	-- 处理事件响应函数
	self.tGameEventFunList[nEventID](self, tGameData)
end

-- 设置玩DOTA游戏事件
function SetPlayDotaGameEvent(nEventID, dwPDBID, nParam1, nParam2)

	if dwPDBID == nil or dwPDBID <= 0 then
		return
	end

	if g_tPlayDotaGame == nil or g_tPlayDotaGame[dwPDBID] == nil then
		return
	end
	local tDataInfo = g_tPlayDotaGame[dwPDBID][nEventID]
	if tDataInfo == nil then
		return
	end

	-- bool类型c++传入会转换成int
	local tGameData = {}
	tGameData.nParam1 = nParam1
	tGameData.nParam2 = nParam2

	for id,task in pairs(tDataInfo) do
		task:OnGameEvent(nEventID, tGameData)
	end
end