-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	12/14/2016
-- 版  本:	1.0
-- 描  述:	动作模板
-------------------------------------------------------------------

require "Class"

--[[
  我们将任务中的动作控制抽象成一个个的动作对象实现
  这些动作对象可以自由的放在任务系统的接受动作列表,推进动作列表,完成动作列表中
  示例:
  T1_FirstWin = Task:new{
    ...
  	tFinishAction={ActionAddPrize:new{prize_id=1,prize_count=1}},
    ...
  }
  
  每个动作支持一个方法:
  1.run(actor,task) 对动作进行执行

  本系统支持的动作有: 
  ActionAddTaskDayCount					: 增加每日任务数 [参数见后面的动作实现]
  ActionAddTaskWeekCount				: 增加每周任务数 [参数见后面的动作实现]
  ActionBinding							: 奖励绑金 [参数见后面的动作实现]
  ActionHeroExp							: 奖励角色经验 [参数见后面的动作实现]
  ActionAddPrize						: 奖励物品 [参数见后面的动作实现]
]]--

-------------------------------------------------------------------
-- 增加每日任务数
-------------------------------------------------------------------
ActionAddTaskDayCount = Object:new
{
}

function ActionAddTaskDayCount:run(holder_id, task)

	if holder_id == nil or task == nil then
		return
	end

 	task:SetTaskDayCount(task:GetTaskDayCount() + 1)
end

-------------------------------------------------------------------
-- 增加每周任务数
-------------------------------------------------------------------
ActionAddTaskWeekCount = Object:new
{
}

function ActionAddTaskWeekCount:run(holder_id, task)

	if holder_id == nil or task == nil then
		return
	end

 	task:SetTaskWeekCount(task:GetTaskWeekCount() + 1)
end

-------------------------------------------------------------------
-- 动作 : 奖励绑金
-- @param binding_gold	  : 奖励绑金
-------------------------------------------------------------------
ActionBinding = Object:new
{
	binding_gold = nil,
}

function ActionBinding:run(holder_id, task)
	
	if self.binding_gold == nil then
		return
	end

	if holder_id == nil or task == nil then
		return
	end

	local nTicket = 0;
	local nGlod   = self.binding_gold;
	if CanAddMoney(holder_id, nTicket, nGlod) == false then
		return
	end

	-- 增加奖励绑金
	AddMoney(holder_id, nTicket, nGlod, task.name)
end

-------------------------------------------------------------------
-- 动作 : 奖励角色经验
-- @param actor_exp	  : 奖励角色经验
-------------------------------------------------------------------
ActionHeroExp = Object:new
{
	hero_exp = nil,
}

function ActionHeroExp:run(holder_id, task)
	
	if self.hero_exp == nil then
		return
	end

	if holder_id == nil or task == nil then
		return
	end

	-- 增加奖励角色经验
	SetActorBaseExp(holder_id, self.hero_exp, task.name)
end

-------------------------------------------------------------------
-- 动作 : 奖励物品
-- @param prize_id	  : 奖励物品ID
-- @param prize_count : 奖励物品数量
-------------------------------------------------------------------
ActionAddPrize = Object:new
{
	prize_id = nil,
	prize_count = nil,
}

function ActionAddPrize:run(holder_id, task)
	
	if self.prize_id == nil or self.prize_id <= 0 then
		return
	end

	if self.prize_count == nil or self.prize_count <= 0 then
		return
	end

	if holder_id == nil or task == nil then
		return
	end

	if CanPrizeActor(holder_id, self.prize_id, self.prize_count) == false then
		return
	end

	-- 增加奖励
	PrizeActor(holder_id, self.prize_id, self.prize_count, holder_id, task.name)
end