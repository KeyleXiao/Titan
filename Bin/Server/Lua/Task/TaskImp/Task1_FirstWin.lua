-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/30/2016
-- 版  本:	1.0
-- 描  述:	首胜任务
-- 首胜任务完成有两种状态（（1/2）（完成普通首胜）（2/2）（完成联盟首胜））
-------------------------------------------------------------------

require "TaskFramePlayDotaGame"

-------------------------------------------------------------------
-- 首胜
-------------------------------------------------------------------
Task1_FirstWin = TaskFramePlayDotaGame:new
{
}

----------------------------------------------------------------------------
-- 玩DOTA游戏执行

-- param1 : 队友列表
-- param2 : 比赛结果统计
-- param2解析：1.英雄类型2.比赛类型
-- 			   3.是否联盟组队4.是否师徒组队5.是否胜利6.是否超神7.是否MVP8.是否输出最高9.是否拆塔最高10.是否助攻最高11.是否金钱最高
-- 			   12.最大连杀数量13.最大不死击杀数量14.击杀数量15.助攻数量16.战绩评分17.获得金钱数量18.决胜控制数量19.惊险救治数量20.拆塔数量21.击杀小龙数量22.击杀大龙数量23.死亡数量
----------------------------------------------------------------------------
function Task1_FirstWin:OnEventPlayDotaGame(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if tGameData == nil then
		return
	end

	if tGameData.nParam2 == nil then
		return
	end
	
	local tWarResultData = split_string(tGameData.nParam2, ";", tonumber)
	if tWarResultData == nil or table_count(tWarResultData) < 23 then
		return
	end
	
	-- 判断是否胜利
	if tWarResultData[5] ~= 1 then
		return
	end
	
	-- 取得玩家胜利次数任务数据(普通完成算1次，联盟完成算2次)
	local task_key = GetWinCountTaskKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- 完成1态，再次普通完成过滤
	if tWarResultData[3] ~= 1 and task_value == 1 then
		return
	end
	
	-- 设置完成状态数据
	self:SetState(TaskState.FinishState)
	
	if tWarResultData[3] ~= 1 then	-- 完成1态
		SetTaskIntData(self.holder_id, task_key, 1)	
	else							-- 完成2态
		SetTaskIntData(self.holder_id, task_key, 2)
		-- 注销玩DOTA游戏事件
		self:UnregisterPlayDotaGame()
	end
	
	-- 触发事件
	OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
end

-- 任务提示 
function Task1_FirstWin:GetHit()

	if self.holder_id == nil or self.holder_id <= 0 then
		return ""
	end

	if self.id == nil or self.id <= 0 then
		return ""
	end
	
	local szHit = ""
	
	if self:IsEnd() == true then
	
		local day_time_key = GetDayTimeTaskKey(self.holder_id, self.id)
		local day_time_value = GetTaskIntData(self.holder_id, day_time_key)

		if day_time_value ~= TASK_DATA_INVALID then
			local nTime = os.time()
			local nEndTime = day_time_value + self.flush_time*3600
			if nTime < nEndTime then
				local nRemainTime = nEndTime - nTime
				szHit = szHit..nRemainTime
			end
		end
	else
		
		local win_count_key = GetWinCountTaskKey(self.holder_id, self.id)
		local win_count_value = GetTaskIntData(self.holder_id, win_count_key)
		if win_count_value == TASK_DATA_INVALID then
			win_count_value = 0
		end
		szHit = szHit.."("..win_count_value.."/2)"
	end

	return szHit
end

-- 角色登陆
function Task1_FirstWin:OnLogin()

	-- 接受任务
	Task.OnLogin(self)

	-- 接受状态和完成1态 订阅事件
	local win_count_key = GetWinCountTaskKey(self.holder_id, self.id)
	local win_count_value = GetTaskIntData(self.holder_id, win_count_key)
	if self.state < TaskState.FinishState or (self.state == TaskState.FinishState and win_count_value == 1) then
		-- 注册玩DOTA游戏事件
		self:RegisterPlayDotaGame()
	end
end

-- 结束任务
function Task1_FirstWin:End()

	local task_key = GetWinCountTaskKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value ~= 1 and task_value ~= 2 then
		return
	end
	
	self:SetState(TaskState.EndState)
	
	if self.prize_good == nil or table_count(self.prize_good) < 1 then
		return
	end
	
	-- 设置数据
	local tFinishAction = {}
	local strPrizeGood = ""
	if task_value == 1 then
	
		local nPrizeID = self.prize_good[1]
		InsertUni(tFinishAction, ActionAddPrize:new{prize_id=nPrizeID, prize_count=1})
		strPrizeGood = strPrizeGood..nPrizeID..";"
	elseif task_value == 2 then
	
		for index,prize_id in pairs(self.prize_good) do
			InsertUni(tFinishAction, ActionAddPrize:new{prize_id=prize_id, prize_count=1})
			strPrizeGood = strPrizeGood..prize_id..";"
		end
	end
	
	-- 发放奖励
	for index,action in pairs(tFinishAction) do
		action:run(self.holder_id, self)
	end
	
	-- 发送客户端
	local strPrize = "<id>"..self.id.."</id><name>"..self.name.."</name><description>"..self.description.."</description><binding_gold>0</binding_gold><exp>0</exp><prize>"..strPrizeGood.."</prize>"
	SendTaskPrize(self.holder_id, strPrize)
	OssMonitorTask(self.id, 0, 1, 0)
	
	-- 删除数据
	RemoveTaskData(self.holder_id, task_key)
	
	-- 注销玩DOTA游戏事件
	self:UnregisterPlayDotaGame()
end