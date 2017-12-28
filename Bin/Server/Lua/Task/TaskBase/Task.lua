-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/29/2016
-- 版  本:	1.0
-- 描  述:	定义任务基本属性及方法
-------------------------------------------------------------------

require "Class"
require "TaskData"
require "Condition"
require "Action"

Task = Object:new
{
	id			= 0  ,               -- 任务ID
	name		= "未知任务",        -- 任务名称
	load_name	= "",				 -- 任务加载脚本名字
	type		= 1,				 -- 任务类型
	trace_show	= 1,				 -- 任务追踪显示位置
	sort_level  = 0,				 -- 优先级排序
	description = "",				 -- 任务描述
	prompt_type = 0,				 -- 提示类型
	save        = true,				 -- 任务下线保存
	allow_delete = true,			 -- 是否允许删除
	loop_type	= 0,				 -- 循环类型
	flush_time  = 0,				 -- 刷新时间
	day_count	= nil,				 -- 日次数
	week_count  = nil,				 -- 周次数
	clear_type	= 0,				 -- 任务数据清除条件（0不清除1每日清除2每周清除）
	tAcceptCondition = {},			 -- 接受条件
	tFinishCondition = {},			 -- 完成条件
	prize_type = 0,					 -- 奖励显示类型
	tAcceptAction	 = {},			 -- 接受动作
	tFinishAction	 = {},			 -- 完成动作
	tDeleteAction    = {},			 -- 删除动作
	tFinishConditionKey	 = {},		 -- 关注的完成条件key
	tGameEventFunList = {},			 -- 事件注册函数

	holder_id       = nil,			 -- 任务的角色ID(DBID)
	state			= nil,	 		 -- 任务状态
	tProgressData   = nil,			 -- 进度数据
	accept_progress = 1,			 -- 该任务在哪一步开始接受任务
	max_progress    = 2,			 -- 该任务总共有几次对话
	tProcessCondition = {},			 -- 任务进行条件
	hit = "",					     -- 任务点击提示
	
	contact_high32id= 0,			 -- 交互ID高32位
	contact_low32id = 0, 			 -- 交互ID低32位
	publisher_id	= 0,			 -- 交互任务发布者ID
	
	sub_type		= 0,			 -- 任务子类型
	sub_type_name	= "",			 -- 任务子类型名称
	specific_type	= 0,			 -- 任务子类型下具体类型
	specific_type_name	= "",		 --	任务子类型下具体类型名称
	publish_description = "",		 -- 发布说明
	emotion_words		= "",		 -- 心情话语
	task_level		= 0,			 -- 任务星级
}

-------------------------------------------------------------------
-- 构造函数
-------------------------------------------------------------------
function Task:construct()

	if self.holder_id ~= nil then
		return
	end

	if self.id == 0 then
		return
	end

	-- 接任务条件:等级
	if self.accept_condition_level ~= nil then
		InsertUni(self.tAcceptCondition, ConditionLevel:new{min_level=self.accept_condition_level[1], max_level=self.accept_condition_level[2]})
	end
	
	-- 接任务条件:前置任务
	if self.pre_id ~= nil then
		InsertUni(self.tAcceptCondition, ConditionCheckPreTaskFinish:new{pre_taskid=self.pre_id})
	end
	
	-- 每日循环次数控制
	if self.day_count ~= nil and self.day_count > 0 then

		if self.week_count == nil or self.week_count <= 0 then
			InsertUni(self.tAcceptCondition, ConditionCheckTaskDayCount:new{max_count=self.day_count})
			InsertUni(self.tAcceptAction, ActionAddTaskDayCount:new{})
		end

		if self.week_count ~= nil and self.week_count > 0 then
			InsertUni(self.tAcceptCondition, ConditionCheckTaskWeekCount:new{max_count=self.week_count})
			InsertUni(self.tAcceptAction, ActionAddTaskWeekCount:new{})

			InsertUni(self.tAcceptCondition, ConditionCheckTaskDayCount:new{max_count=self.day_count})
			InsertUni(self.tAcceptAction, ActionAddTaskDayCount:new{})
		end
	end

	-- 完成动作:奖励绑定金钱
	if self.prize_binding_gold ~= nil then
		InsertUni(self.tFinishAction, ActionBinding:new{binding_gold=self.prize_binding_gold})
	end

	-- 完成动作:奖励角色经验
	if self.prize_hero_exp ~= nil then
		InsertUni(self.tFinishAction, ActionHeroExp:new{hero_exp=self.prize_hero_exp})
	end

	-- 完成动作:奖励物品
	if self.prize_good ~= nil then
		for index,prize_id in pairs(self.prize_good) do
			InsertUni(self.tFinishAction, ActionAddPrize:new{prize_id=prize_id, prize_count=1})
		end
	end
end

-- 获取任务状态
function Task:GetState()
	
	if self.holder_id == nil or self.holder_id <= 0 then
		return TaskState.BeginState
	end
	
	local task_key = GetTaskIDKey(self.holder_id, self.id)
	local task_state = GetTaskIntData(self.holder_id, task_key)
	if task_state == TASK_DATA_INVALID then
		task_state = TaskState.BeginState
	end
	
	return task_state
end

-- 设置任务状态
function Task:SetState(nState, bUpdateFlag)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if self:GetState() == nState then
		return
	end
	
	self.state = nState

    local task_key = GetTaskIDKey(self.holder_id, self.id)
	SetTaskIntData(self.holder_id, task_key, nState)
	
	if bUpdateFlag == true then
		self:Update()
	end	
end

-- 取得任务进度
function Task:GetProgress()

	if self.tProgressData == nil then
		return 1
	end

	return self.tProgressData
end

-- 设置任务进度
function Task:SetProgress(progress)

	if progress == nil then
		return
	end

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if self:GetProgress() == progress then
		return
	end
	
	self.tProgressData = progress

    self:Update()
	self:UpdateProgress()
end

-- 任务进度更新
function Task:UpdateProgress()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if TASK_LIST[self.holder_id] == nil or TASK_LIST[self.holder_id][self.id] == nil then
		return
	end

	OnActorTaskListChanged(self.holder_id, self.id, "updateprogress")
end

-- 能否接受任务
-- @param hard : 是否只检查刚性条件 hard condition
--             : 比如包裹是否满,这个就不是一个刚性条件,因为玩家是可以修改的,但等级要求就是刚性条件
function Task:CanAccept(hard)

	if self.holder_id == nil or self.holder_id <= 0 then
		return false
	end

	for index,condition in pairs(self.tAcceptCondition) do
		if hard~=true or condition.is_hard~=false then
			if condition.parent.Test(condition, self.holder_id, self, hard)~=true then
				return false,condition
			end
		end
	end

	return true
end

-- 能否完成任务
function Task:CanFinish(return_reason)

	if self.holder_id == nil or self.holder_id <= 0 then
		return false
	end

	if self:GetState() ~= TaskState.AcceptState then
		return false
	end

	for index,condition in pairs(self.tFinishCondition) do
	
		local task_key = condition:GetConditionKey(self.holder_id, self)
		if task_key ~= 0 and self.tFinishConditionKey[task_key] == nil then
			self.tFinishConditionKey[task_key] = true
		end
		
		if condition:Test(self.holder_id, self)~=true then
			-- 尽量返回原因
			if return_reason==true and condition.hint~=nil then
				return false,condition:Hint(self.holder_id, self)
			end
			return false
		end
	end

	return true
end

-- 是否可以进行任务
function Task:CanProcess(progress, return_reason)

	if self.holder_id == nil or self.holder_id <= 0 then
		return false
	end

	if progress == nil then
		progress = self:GetProgress()
	end
	if progress == self.max_progress then
		return self:CanFinish(return_reason)
	end

	if self.tProcessCondition[progress] == nil then
		return true
	end

	for index,condition in pairs(self.tProcessCondition[progress]) do
		if type(condition) ~= 'table' then
			error("tProcessCondition[progress] condition type ~= table progress=",progress,"id=",self.id)
		else
			if condition.test == nil then
				error("tProcessCondition[progress] condition:test==nil progress=",progress,"id=",self.id)
			elseif condition:Test(self.holder_id, self)~=true then
				-- 尽量返回原因 
				if return_reason==true and condition.hint~=nil then 
					return false,condition:Hint(self.holder_id, self)
				end
				return false	
			end
		end
	end
	
	return true
end

-- 移除完成条件数据
function Task:RemoveFinishConditionTaskData()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	for task_key,_ in pairs(self.tFinishConditionKey) do
		RemoveTaskData(self.holder_id, task_key)
	end
	
	self.tFinishConditionKey = {}
end

-- 接受任务
function Task:Accept()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	for index,action in pairs(self.tAcceptAction) do
		action:run(self.holder_id, self)
	end

	-- 设置成接受进度
	self:SetProgress(self.accept_progress+1)
	
	self:SetState(TaskState.AcceptState)
	
	-- 交互任务，记录交互ID
	if self.type == TaskType.ContacType then
		
		local contact_high32id_key = GetContactHigh32IDKey(self.holder_id, self.id)
		SetTaskIntData(self.holder_id, contact_high32id_key, self.contact_high32id)
		
		local contact_low32id_key = GetContactLow32IDKey(self.holder_id, self.id)
		SetTaskIntData(self.holder_id, contact_low32id_key, self.contact_low32id)
		
		local contact_publisherid_key = GetContactPublisherIDKey(self.holder_id, self.id)
		SetTaskIntData(self.holder_id, contact_publisherid_key, self.publisher_id)
		
	end
	
	OssMonitorTask(self.id, 1, 0, 0)
	
end

-- 完成任务
function Task:Finish()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	self:SetState(TaskState.FinishState)
	
	self:RemoveFinishConditionTaskData()
	
	--if self.type == TaskType.ContacType then
	--	OnFinishContactTask(self.holder_id, self.contact_id)	
	--end
	
end

-- 结束任务
function Task:End()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	self:SetState(TaskState.EndState)
	
	for index,action in pairs(self.tFinishAction) do
		action:run(self.holder_id, self)
	end

	-- 完成动作:奖励绑定金钱
	local strPrizeBindingGold = ""
	if self.prize_binding_gold ~= nil then
		strPrizeBindingGold = self.prize_binding_gold
	end

	-- 完成动作:奖励角色经验
	local strPrizeHeroExp = ""
	if self.prize_hero_exp ~= nil then
		strPrizeHeroExp = self.prize_hero_exp
	end

	-- 完成动作:奖励物品
	local strPrizeGood = ""
	if self.prize_good ~= nil then
		for index,prize_id in pairs(self.prize_good) do
			strPrizeGood = strPrizeGood..prize_id..";"
		end
	end
	local strPrize = "<id>"..self.id.."</id><name>"..self.name.."</name><description>"..self.description.."</description><binding_gold>"..strPrizeBindingGold.."</binding_gold><exp>"..strPrizeHeroExp.."</exp><prize>"..strPrizeGood.."</prize>"
	SendTaskPrize(self.holder_id, strPrize)
	
	OssMonitorTask(self.id, 0, 1, 0)
	
end

-- 删除任务
function Task:DeleteTask()
	
	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	for index,action in pairs(self.tDeleteAction) do
		action:run(self.holder_id, self)
	end
	
	if self:GetState() ~= TaskState.EndState then
		OssMonitorTask(self.id, 0, 0, 1)
	end
	
	-- 清除任务数据，这样可以重复接
	-- 已结束主线任务不清理
	-- 首胜任务不清理
	if (self.type == TaskType.MainType and self.state == TaskState.EndState) or self.type == TaskType.FirstWinType then
		return
	end
	local task_key = GetTaskIDKey(self.holder_id, self.id)
	RemoveTaskData(self.holder_id, task_key)
	
	-- 交互任务，清除ID数据，可以重复接
	if self.type == TaskType.ContacType then

		local contact_high32id_key = GetContactHigh32IDKey(self.holder_id, self.id)
		RemoveTaskData(self.holder_id, contact_high32id_key)
		local contact_low32id_key = GetContactLow32IDKey(self.holder_id, self.id)
		RemoveTaskData(self.holder_id, contact_low32id_key)
		local contact_publisherid_key = GetContactPublisherIDKey(self.holder_id, self.id)
		RemoveTaskData(self.holder_id, contact_publisherid_key)

	end
end

-- 角色登陆
function Task:OnLogin()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if self.id == nil or self.id <= 0 then
		return
	end

	local task_key = GetTaskIDKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value~=TASK_DATA_INVALID then
		  self.state = task_value
	end
	
	-- 交互任务，读取交互ID
	if self.type == TaskType.ContacType then
	
		local contact_high32id_key = GetContactHigh32IDKey(self.holder_id, self.id)
		local contact_high32id_value = GetTaskIntData(self.holder_id, contact_high32id_key)
		if contact_high32id_value ~= TASK_DATA_INVALID then
			self.contact_high32id = contact_high32id_value
		end
		
		local contact_low32id_key = GetContactLow32IDKey(self.holder_id, self.id)
		local contact_low32id_value = GetTaskIntData(self.holder_id, contact_low32id_key)
		if contact_low32id_value ~= TASK_DATA_INVALID then
			self.contact_low32id = contact_low32id_value
		end
		
		local contact_publisherid_key = GetContactPublisherIDKey(self.holder_id, self.id)
		local contact_publisherid_value = GetTaskIntData(self.holder_id, contact_publisherid_key)
		if contact_publisherid_value ~= TASK_DATA_INVALID then
			self.publisher_id = contact_publisherid_value
		end
	end

	self:Update()
end

-- 角色登出
function Task:OnLogout()
end

-- 任务数据更新
function Task:Update()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if TASK_LIST[self.holder_id] == nil or TASK_LIST[self.holder_id][self.id] == nil then
		return
	end

	OnActorTaskListChanged(self.holder_id, self.id, "update")
end

-- 是否已完成
function Task:IsFinished()

	if self.holder_id == nil or self.holder_id <= 0 then
		return false
	end
	
	return self:GetState() == TaskState.FinishState
end

-- 是否已领取
function Task:IsEnd()

	if self.holder_id == nil or self.holder_id <= 0 then
		return false
	end
	
	return self:GetState() == TaskState.EndState
end

-- 取得任务今日次数
function Task:GetTaskDayCount()

	if self.holder_id == nil or self.holder_id <= 0 then
		error("GetDailyRepeat error self.holder_id,id="..self.id)
		return 0
	end

    local day_time_key = GetDayTimeTaskKey(self.holder_id, self.id)
    local day_time_value = GetTaskIntData(self.holder_id, day_time_key)

	-- 不是今天的数据要清除
 	if day_time_value ~= TASK_DATA_INVALID then

        local day_count_key = GetDayCountTaskKey(self.holder_id, self.id)
		
		-- 小时循环
		if self.loop_type == 1 then

			local nTime = os.time()
			-- 只有时间大于才数据存储时间才操作(防止场景服不同步刷次数)
			if nTime > day_time_value then
				if nTime >= day_time_value + self.flush_time*3600 then
					RemoveTaskData(self.holder_id, day_time_key)
					RemoveTaskData(self.holder_id, day_count_key)
					return 0
				end
			end
		else
			-- 后面继续根据loop_type分类型操作
			local today = GetGameDate(self.reset_time)
			local nTime = os.time(today)
			-- 只有时间大于才数据存储时间才操作(防止场景服不同步刷次数)
			if nTime > day_time_value then
				-- 不是同一天
				if DiffDay(day_time_value, nTime) ~= 0 then
					RemoveTaskData(self.holder_id, day_time_key)
					RemoveTaskData(self.holder_id, day_count_key)
					return 0
				end
			end
		end

		local day_count = GetTaskIntData(self.holder_id, day_count_key)
		if day_count == TASK_DATA_INVALID then
			day_count = 0
		end
		return day_count
	end

	return 0
end

-- 设置任务今日次数
function Task:SetTaskDayCount(count)

 	if self.holder_id == nil or self.holder_id <= 0 then
		error("SetDailyRepeat error self.holder_id,id="..self.id)
		return 0
	end

    local day_time_key = GetDayTimeTaskKey(self.holder_id, self.id)
	-- 小时循环
	if self.loop_type == 1 then
		local nTime = os.time()
		SetTaskIntData(self.holder_id, day_time_key, nTime)
	else
		-- 后面继续根据loop_type分类型操作
		local today = GetGameDate(self.reset_time)
		local nTime = os.time(today)
		SetTaskIntData(self.holder_id, day_time_key, nTime)
	end

    local day_count_key = GetDayCountTaskKey(self.holder_id, self.id)
    SetTaskIntData(self.holder_id, day_count_key, count)
end


-- 取得任务本周次数
function Task:GetTaskWeekCount()

	if self.holder_id == nil or self.holder_id <= 0 then
		error("GetWeekRepeat error self.holder_id,id="..self.id)
		return 0
	end

	local week_time_key = GetWeekTimeTaskKey(self.holder_id, self.id)
	local week_time_value = GetTaskIntData(self.holder_id, week_time_key)

	-- 不是今天的数据要清除
	if week_time_value ~= TASK_DATA_INVALID then
		-- 设置周一的0点整
		local time_value = GetGameDate(self.reset_time)
		local nTime = os.time(time_value)

		local week_count_key = GetWeekCountTaskKey(self.holder_id, self.id)

		-- 只有时间大于才数据存储时间才操作(防止场景服不同步刷次数)
		if nTime > week_time_value then
			-- 不是同一周
			if DiffWeek(week_time_value, nTime) > 0 then
				
				RemoveTaskData(self.holder_id, week_time_key)
				RemoveTaskData(self.holder_id, week_count_key)
				return 0
			end
		end

		local week_count = GetTaskIntData(self.holder_id, week_count_key)
		if week_count == TASK_DATA_INVALID then
			week_count = 0
		end
		return week_count
	end

	return 0
end

-- 设置任务本周次数
function Task:SetTaskWeekCount(count)

	if self.holder_id == nil or self.holder_id <= 0 then
		error("SetTaskWeekCount error self.holder_id,id="..self.id)
		return 0
	end

	local today = GetGameDate(self.reset_time)
	local time_value = os.time(today)

	local week_date_key = GetWeekTimeTaskKey(self.holder_id, self.id)
	SetTaskIntData(self.holder_id, week_date_key, time_value)

	local week_count_key = GetWeekCountTaskKey(self.holder_id, self.id)
	SetTaskIntData(self.holder_id, week_count_key, count)
end

-- 取得任务点击
function Task:GetHit()

	if self.holder_id == nil or self.holder_id <= 0 then
		return ""
	end

	if self.id == nil or self.id <= 0 then
		return ""
	end

	-- 判断是否完成
	--if self:IsFinished() == true or self:IsEnd() == true then
	--	return ""
	--end
	--local strHit = self.hit or ""
	--for _,condition in pairs(self.tFinishCondition) do
	--	strHit = strHit..condition:Hint(self.holder_id, self)
	--end
	
	-- 判断是否完成
	local strHit = self.hit or ""
	if self:IsFinished() == true or self:IsEnd() == true then
		-- 完成状态
		for _,condition in pairs(self.tFinishCondition) do
			if condition.count ~= nil then
				strHit = strHit.."("..condition.count.."/"..condition.count..")"
			end
		end
	else
		-- 未完成状态
		for _,condition in pairs(self.tFinishCondition) do
			strHit = strHit..condition:Hint(self.holder_id, self)
		end
	end
	
	return strHit
end