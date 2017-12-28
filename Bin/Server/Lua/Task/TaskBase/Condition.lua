-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/29/2016
-- 版  本:	1.0
-- 描  述:	条件模板
-------------------------------------------------------------------

require "Class"
require "TaskDef"

--[[
  我们将任务中的条件控制抽象成一个个的条件对象实现
  这些条件对象可以自由的放在任务系统的接受条件列表,推进条件列表,完成条件列表中
  示例:
  T1_FirstWin = Task:new{
    ...
  	tAcceptCondition={ConditionLevel:new{min_level=0,max_level=120}},   -- 任务接受条件，等级范围
    ...
  }
  
  每个条件支持四个方法:
  1.Test(actor,task) 对条件进行测试
  2.Hint(actor,task) 根据条件生成任务追踪信息 
  3.GetConditionKey(actor,task) 获取条件关注的key
  4.GetRegisterGameEvent()		获取注册游戏事件

  本系统支持的条件有: 
  ConditionCheckTaskDayCount            : 检查每日循环次数 [参数见后面的条件实现]
  ConditionCheckTaskWeekCount			: 检查每周循环次数 [参数见后面的条件实现]
  ConditionCheckPreTaskFinish			: 检查前置任务是否完成 [参数见后面的条件实现]
  ConditionCheckHaveClan				: 检查是否有联盟 [参数见后面的条件实现]
  ConditionLevel						: 检查玩家等级 [参数见后面的条件实现]
  ConditionWinCount						: 检查玩家胜利次数 [参数见后面的条件实现]
  ConditionFinishCount					: 检查玩家完成次数 [参数见后面的条件实现]
  ConditionHaveHeroCount				: 检查玩家英雄拥有个数 [参数见后面的条件实现]
  ConditionHaveSkinCount				: 检查玩家皮肤拥有个数 [参数见后面的条件实现]
  ConditionMatchCount					: 检查玩家比赛场数 [参数见后面的条件实现]
  ConditionOpenChestCount				: 检查玩家打开宝箱个数 [参数见后面的条件实现]
  ConditionInlayCrownGem				: 检查玩家镶嵌宝石个数 [参数见后面的条件实现]
  ConditionHeroUpGrade					: 检查玩家英雄升级 [参数见后面的条件实现]
  ConditionUseXp						: 检查玩家使用xp [参数见后面的条件实现]
  ConditionKillMonsterCount				: 检查玩家击杀某个怪物数 [参数见后面的条件实现]
  ConditionUseWarWring					: 检查玩家战场使用翅膀 [参数见后面的条件实现]
  ConditionMasterCount					: 检查玩家师傅个数 [参数见后面的条件实现]
  ConditionPrenticeCount				: 检查玩家学徒个数 [参数见后面的条件实现]
  ConditionMentorshipCount				: 检查玩家师傅和学徒个数 [参数见后面的条件实现]
  ConditionClanDonateCount				: 检查玩家联盟捐献次数 [参数见后面的条件实现]
  ConditionCheckNewPlayerGuideStep		: 检查玩家已做过新手引导步骤 [参数见后面的条件实现]
  
  ConditionApplyFriendCount				: 检查玩家交互申请交友次数 [参数见后面的条件实现]
  ConditionCheckIsMaster				: 检查玩家交互求师傅次数 [参数见后面的条件实现]
  ConditionContactReplyCount			: 检查玩家交互回复次数 [参数见后面的条件实现]
  ConditionContactPKCount				: 检查玩家交互PK次数 [参数见后面的条件实现]
  
]]--

-------------------------------------------------------------------
-- 条件 : 检查每日循环次数
-- @param max_count : 最大循环次数
-------------------------------------------------------------------
ConditionCheckTaskDayCount = Object:new
{
	max_count = 1,
}

function ConditionCheckTaskDayCount:GetRegisterGameEvent()
	return nil
end

function ConditionCheckTaskDayCount:GetConditionKey(holder_id, task)
	return 0
end

function ConditionCheckTaskDayCount:Test(holder_id, task)

	if holder_id == nil or task == nil then
		return false
	end

 	return task:GetTaskDayCount() < self.max_count
end

function ConditionCheckTaskDayCount:Hint(holder_id, task)
	return ""
end

-------------------------------------------------------------------
-- 条件 : 检查每周循环次数
-- @param max_count : 最大循环次数
-------------------------------------------------------------------
ConditionCheckTaskWeekCount = Object:new
{
	max_count = 1,
}

function ConditionCheckTaskWeekCount:GetRegisterGameEvent()
	return nil
end

function ConditionCheckTaskWeekCount:GetConditionKey(holder_id, task)
	return 0
end

function ConditionCheckTaskWeekCount:Test(holder_id, task)

	if holder_id == nil or task == nil then
		return false
	end

 	return task:GetTaskWeekCount() < self.max_count
end

function ConditionCheckTaskWeekCount:Hint(holder_id, task)
	return ""
end

-------------------------------------------------------------------
-- 条件 : 检查前置任务是否完成
-- @param max_count : 最大循环次数
-------------------------------------------------------------------
ConditionCheckPreTaskFinish = Object:new
{
	pre_taskid = nil,
}

function ConditionCheckPreTaskFinish:GetRegisterGameEvent()
	return nil
end

function ConditionCheckPreTaskFinish:GetConditionKey(holder_id, task)
	return 0
end

function ConditionCheckPreTaskFinish:Test(holder_id, task)

	if holder_id == nil or task == nil then
		return false
	end
	
	if self.pre_taskid == nil then
		return true
	end
	
	local task_key = GetTaskIDKey(holder_id, self.pre_taskid)
	local task_value = GetTaskIntData(holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = TaskState.BeginState
	end

 	return task_value >= TaskState.EndState
end

function ConditionCheckPreTaskFinish:Hint(holder_id, task)
	return ""
end
 
-------------------------------------------------------------------
-- 条件 : 检查是否有联盟
-------------------------------------------------------------------
ConditionCheckHaveClan = Object:new
{
}

function ConditionCheckHaveClan:GetRegisterGameEvent()
	return nil
end

function ConditionCheckHaveClan:GetConditionKey(holder_id, task)
	return 0
end

function ConditionCheckHaveClan:Test(holder_id, task)

	if holder_id == nil or task == nil then
		return false
	end
	
	local nClanID = GetPlayerClanID(holder_id)

 	return nClanID > 0
end

function ConditionCheckHaveClan:Hint(holder_id, task)
	return ""
end

-------------------------------------------------------------------
-- 条件 : 判断等级
-- @param min_level : 最低等级
-- @param max_level : 最高等级
-------------------------------------------------------------------
ConditionLevel = Object:new
{
	min_level = nil,
	max_level = nil,
}

function ConditionLevel:GetRegisterGameEvent()
	return TaskGameEventType.EventType_LEVEL
end

function ConditionLevel:GetConditionKey(holder_id, task)
	return 0
end

function ConditionLevel:Test(holder_id, task)
	
	if self.min_level == nil or self.max_level == nil then
		return true
	end

	local level = GetPlayerLevel(holder_id)
	
	if self.min_level == self.max_level then
	
		return level >= self.max_level
	else
		if level >= self.min_level and level <= self.max_level then
			return true
		end
	end	
	
	return false
end

function ConditionLevel:Hint(holder_id, task)

	if self.min_level == self.max_level then
		return ""
	else
		return "("..self.min_level.."/"..self.max_level..")"
	end
end

-------------------------------------------------------------------
-- 条件 : 胜利次数
-- @param count : 胜利次数
-------------------------------------------------------------------
ConditionWinCount = Object:new
{
	count = nil,
}

function ConditionWinCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_FINISHMATCH
end

function ConditionWinCount:GetConditionKey(holder_id, task)

	if holder_id == nil or task == nil then
		return 0
	end
	
	return GetWinCountTaskKey(holder_id, task.id)
end

function ConditionWinCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end

	-- 取得胜利次数任务数据键值
	local win_count_key = GetWinCountTaskKey(holder_id, task.id)
	local win_count_value = GetTaskIntData(holder_id, win_count_key)
	if win_count_value == TASK_DATA_INVALID then
		win_count_value = 0
	end

	return win_count_value >= self.count
end

function ConditionWinCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end

	-- 取得胜利次数任务数据键值
	local win_count_key = GetWinCountTaskKey(holder_id, task.id)
	local win_count_value = GetTaskIntData(holder_id, win_count_key)
	if win_count_value == TASK_DATA_INVALID then
		win_count_value = 0
	end

	return "("..win_count_value.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 完成次数
-- @param count : 完成次数
-------------------------------------------------------------------
ConditionFinishCount = Object:new
{
	count = nil,
}

function ConditionFinishCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_FINISHMATCH
end

function ConditionFinishCount:GetConditionKey(holder_id, task)

	if holder_id == nil or task == nil then
		return 0
	end
	
	return GetFinishCountTaskKey(holder_id, task.id)
end

function ConditionFinishCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end

	-- 取得完成次数任务数据键值
	local finish_count_key = GetFinishCountTaskKey(holder_id, task.id)
	local finish_count_value = GetTaskIntData(holder_id, finish_count_key)
	if finish_count_value == TASK_DATA_INVALID then
		finish_count_value = 0
	end

	return finish_count_value >= self.count
end

function ConditionFinishCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end

	-- 取得胜利次数任务数据键值
	local finish_count_key = GetFinishCountTaskKey(holder_id, task.id)
	local finish_count_value = GetTaskIntData(holder_id, finish_count_key)
	if finish_count_value == TASK_DATA_INVALID then
		finish_count_value = 0
	end

	return "("..finish_count_value.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 玩家英雄拥有个数
-- @param count : 拥有个数
-------------------------------------------------------------------
ConditionHaveHeroCount = Object:new
{
	count = nil
}

function ConditionHaveHeroCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_ADDHERO
end

function ConditionHaveHeroCount:GetConditionKey(holder_id, task)

	return 0
end

function ConditionHaveHeroCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local cur_count = GetPlayerHeroCount(holder_id)
	
	return cur_count >= self.count
end

function ConditionHaveHeroCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local cur_count = GetPlayerHeroCount(holder_id)
	
	return "("..cur_count.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 玩家皮肤拥有个数
-- @param count : 拥有个数
-------------------------------------------------------------------
ConditionHaveSkinCount = Object:new
{
	count = nil
}

function ConditionHaveSkinCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_ADDSKIN
end

function ConditionHaveSkinCount:GetConditionKey(holder_id, task)

	return 0
end

function ConditionHaveSkinCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local cur_count = GetPlayerSkinCount(holder_id)
	
	return cur_count >= self.count
end

function ConditionHaveSkinCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local cur_count = GetPlayerSkinCount(holder_id)
	
	return "("..cur_count.."/"..self.count..")"
end
-------------------------------------------------------------------
-- 条件 : 玩家比赛场数
-- @param count : 比赛场数
-------------------------------------------------------------------
ConditionMatchCount = Object:new
{
	count = nil
}

function ConditionMatchCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_FINISHMATCH
end

function ConditionMatchCount:GetConditionKey(holder_id, task)

	return 0
end

function ConditionMatchCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local cur_count = GetPlayerMatchCount(holder_id)
	
	return cur_count >= self.count
end

function ConditionMatchCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local cur_count = GetPlayerMatchCount(holder_id)
	
	return "("..cur_count.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 玩家打开宝箱个数
-- @param count : 开宝箱个数
-------------------------------------------------------------------
ConditionOpenChestCount = Object:new
{
	count = nil
}

function ConditionOpenChestCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_OPENCHEST
end

function ConditionOpenChestCount:GetConditionKey(holder_id, task)

	if holder_id == nil or task == nil then
		return 0
	end
	
	return GetOpenChestCountTaskKey(holder_id, task.id)
end

function ConditionOpenChestCount:Test(holder_id, task)

	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local openchest_count_key = GetOpenChestCountTaskKey(holder_id, task.id)
	local openchest_count_value = GetTaskIntData(holder_id, openchest_count_key)
	if openchest_count_value == TASK_DATA_INVALID then
		openchest_count_value = 0
	end

	return openchest_count_value >= self.count
end

function ConditionOpenChestCount:Hint(holder_id, task)

	if self.count == nil or self.count <= 1 then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local openchest_count_key = GetOpenChestCountTaskKey(holder_id, task.id)
	local openchest_count_value = GetTaskIntData(holder_id, openchest_count_key)
	if openchest_count_value == TASK_DATA_INVALID then
		openchest_count_value = 0
	end
	
	return "("..openchest_count_value.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 玩家镶嵌宝石
-------------------------------------------------------------------
ConditionInlayCrownGem = Object:new
{
	count = nil
}

function ConditionInlayCrownGem:GetRegisterGameEvent()

	return TaskGameEventType.EventType_INLAYGEM
end

function ConditionInlayCrownGem:GetConditionKey(holder_id, task)
	
	if holder_id == nil or task == nil then
		return 0
	end
	
	return GetInlayCrownGemCountTaskKey(holder_id, task.id)
end

function ConditionInlayCrownGem:Test(holder_id, task)

	if self.count == nil then
		return true
	end
	
	if holder_id == nil or task == nil then
		return false
	end
	
	local inlay_count_key = GetInlayCrownGemCountTaskKey(holder_id, task.id)
	local inlay_count_value = GetTaskIntData(holder_id, inlay_count_key)
	if inlay_count_value == TASK_DATA_INVALID then
		inlay_count_value = 0
	end

	return inlay_count_value >= self.count
end

function ConditionInlayCrownGem:Hint(holder_id, task)

	if holder_id == nil or task == nil then
		return ""
	end

	return ""
end
-------------------------------------------------------------------
-- 条件 : 玩家英雄升级
-- @param count : 等级
-------------------------------------------------------------------
ConditionHeroUpGrade = Object:new
{
	level = nil
}

function ConditionHeroUpGrade:GetRegisterGameEvent()

	return TaskGameEventType.EventType_HEROUPGRADE
end

function ConditionHeroUpGrade:GetConditionKey(holder_id, task)
	
	return 0
end

function ConditionHeroUpGrade:Test(holder_id, task)
	
	if self.level == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local count = GetPlayerHeroCountByLevel(holder_id, self.level)
	
	return count > 0
end

function ConditionHeroUpGrade:Hint(holder_id, task)

	if self.level == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	return ""
end
-------------------------------------------------------------------
-- 条件 : 玩家使用xp
-------------------------------------------------------------------
ConditionUseXp = Object:new 
{
}

function ConditionUseXp:GetRegisterGameEvent()

	return TaskGameEventType.EventType_FINISHMATCH
end

function ConditionUseXp:GetConditionKey(holder_id, task)
	
	return 0
end

function ConditionUseXp:Test(holder_id, task)

	if holder_id == nil or task == nil then
		return false
	end

	return GetPlayerWarUseXPCount(holder_id) > 0
end

function ConditionUseXp:Hint(holder_id, task)

	if holder_id == nil or task == nil then
		return ""
	end

	return ""
end

-------------------------------------------------------------------
-- 条件 : 玩家击杀某个怪物数(1大龙 2 小龙 3 大龙或小龙)
-- @param count : 怪物数
-------------------------------------------------------------------
ConditionKillMonsterCount = Object:new
{
	count = nil,
	mon_type = nil,
}

function ConditionKillMonsterCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_FINISHMATCH
end

function ConditionKillMonsterCount:GetConditionKey(holder_id, task)

	return 0
end

function ConditionKillMonsterCount:Test(holder_id, task)
	
	if self.count == nil or self.mon_type == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local tmp_count = 0
	if self.mon_type == 1 then
		tmp_count = GetPlayerKillBigDragoCount(holder_id)
	elseif self.mon_type == 2 then
		tmp_count = GetPlayerKillSmallDragoCount(holder_id)
	elseif self.mon_type == 3 then
		tmp_count = GetPlayerKillBigDragoCount(holder_id) + GetPlayerKillSmallDragoCount(holder_id)
	end
	
	return tmp_count >= self.count
end

function ConditionKillMonsterCount:Hint(holder_id, task)

	if self.count == nil or self.mon_type == nil or self.count <= 1 then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local tmp_count = 0
	if self.mon_type == 1 then
		tmp_count = GetPlayerKillBigDragoCount(holder_id)
	elseif self.mon_type == 2 then
		tmp_count = GetPlayerKillSmallDragoCount(holder_id)
	elseif self.mon_type == 3 then
		tmp_count = GetPlayerKillBigDragoCount(holder_id) + GetPlayerKillSmallDragoCount(holder_id)
	end

	return "("..tmp_count.."/"..self.count..")"
end
-------------------------------------------------------------------
-- 条件 : 玩家战场使用翅膀
-------------------------------------------------------------------
ConditionUseWarWring = Object:new
{
}

function ConditionUseWarWring:GetRegisterGameEvent()

	return TaskGameEventType.EventType_FINISHMATCH
end

function ConditionUseWarWring:GetConditionKey(holder_id, task)
	
	return 0
end

function ConditionUseWarWring:Test(holder_id, task)

	if holder_id == nil or task == nil then
		return false
	end
	
	return GetPlayerWarUseWringCount(holder_id) > 0
end

function ConditionUseWarWring:Hint(holder_id, task)

	if holder_id == nil or task == nil then
		return ""
	end
	
	return ""
end

-------------------------------------------------------------------
-- 条件 : 玩家师傅个数
-- @param count : 师傅个数
-------------------------------------------------------------------
ConditionMasterCount = Object:new
{
	count = nil
}

function ConditionMasterCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_BUIDMENTORSHIP
end

function ConditionMasterCount:GetConditionKey(holder_id, task)
	
	return 0
end

function ConditionMasterCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local master_count = GetPlayerMasterCount(holder_id)
	
	return master_count >= self.count
end

function ConditionMasterCount:Hint(holder_id, task)

	if self.count == nil or self.count <= 1 then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local master_count = GetPlayerMasterCount(holder_id)

	return "("..master_count.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 玩家学徒个数
-- @param count : 学徒个数
-------------------------------------------------------------------
ConditionPrenticeCount = Object:new
{
	count = nil
}

function ConditionPrenticeCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_BUIDMENTORSHIP
end

function ConditionPrenticeCount:GetConditionKey(holder_id, task)
	
	return 0
end

function ConditionPrenticeCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local prentice_count = GetPlayerPrenticeCount(holder_id)
	
	return prentice_count >= self.count
end

function ConditionPrenticeCount:Hint(holder_id, task)

	if self.count == nil or self.count <= 1 then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local prentice_count = GetPlayerPrenticeCount(holder_id)

	return "("..prentice_count.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 玩家师徒个数
-- @param count : 师徒个数
-------------------------------------------------------------------
ConditionMentorshipCount = Object:new
{
	count = nil
}

function ConditionMentorshipCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_BUIDMENTORSHIP
end

function ConditionMentorshipCount:GetConditionKey(holder_id, task)
	
	return 0
end

function ConditionMentorshipCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local mentorship_count = GetPlayerPrenticeCount(holder_id) + GetPlayerMasterCount(holder_id)
	
	return mentorship_count >= self.count
end

function ConditionMentorshipCount:Hint(holder_id, task)

	if self.count == nil or self.count <= 1 then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local mentorship_count = GetPlayerPrenticeCount(holder_id) + GetPlayerMasterCount(holder_id)

	return "("..mentorship_count.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 玩家联盟捐献次数
-- @param count : 捐献次数
-------------------------------------------------------------------
ConditionClanDonateCount = Object:new
{
	count = nil
}

function ConditionClanDonateCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_CLANDONATE
end

function ConditionClanDonateCount:GetConditionKey(holder_id, task)

	if holder_id == nil or task == nil then
		return 0
	end
	
	return GetClanDonateCountTaskKey(holder_id, task.id)
end

function ConditionClanDonateCount:Test(holder_id, task)
	
	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local clandonate_count_key = GetClanDonateCountTaskKey(holder_id, task.id)
	local clandonate_count_value = GetTaskIntData(holder_id, clandonate_count_key)
	if clandonate_count_value == TASK_DATA_INVALID then
		clandonate_count_value = 0
	end
	
	return clandonate_count_value >= self.count
end

function ConditionClanDonateCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local clandonate_count_key = GetClanDonateCountTaskKey(holder_id, task.id)
	local clandonate_count_value = GetTaskIntData(holder_id, clandonate_count_key)
	if clandonate_count_value == TASK_DATA_INVALID then
		clandonate_count_value = 0
	end
	
	return "("..clandonate_count_value.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 检查玩家已做过新手引导步骤
-- @param count : 步骤
-------------------------------------------------------------------
ConditionCheckNewPlayerGuideStep = Object:new
{
	count = nil,
}

function ConditionCheckNewPlayerGuideStep:GetRegisterGameEvent()

	return TaskGameEventType.EventType_NovideGuide
end

function ConditionCheckNewPlayerGuideStep:GetConditionKey(holder_id, task)

	return 0
end

function ConditionCheckNewPlayerGuideStep:Test(holder_id, task)

	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local nCurrentStep = GetPlayerGuideStep(holder_id)

	return nCurrentStep >= self.count
end

function ConditionCheckNewPlayerGuideStep:Hint(holder_id, task)

	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local nCurrentStep = GetPlayerGuideStep(holder_id)
	
	return "("..nCurrentStep.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 检查玩家交互申请交友次数 
-- @param count : PK次数
-------------------------------------------------------------------
ConditionApplyFriendCount = Object:new
{
	count = nil
}

function ConditionApplyFriendCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_APPLYFRIEND
end

function ConditionApplyFriendCount:GetConditionKey(holder_id, task)

	if holder_id == nil or task == nil then
		return 0
	end
	
	return GetContactApplyFriendCountKey(holder_id, task.id)
end

function ConditionApplyFriendCount:Test(holder_id, task)

	if self.count == nil then
		return true
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local applyfriend_count_key = GetContactApplyFriendCountKey(holder_id, task.id)
	local applyfriend_count_value = GetTaskIntData(holder_id, applyfriend_count_key)
	if applyfriend_count_value == TASK_DATA_INVALID then
		applyfriend_count_value = 0
	end

	return applyfriend_count_value >= self.count
end

function ConditionApplyFriendCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local applyfriend_count_key = GetContactApplyFriendCountKey(holder_id, task.id)
	local applyfriend_count_value = GetTaskIntData(holder_id, applyfriend_count_key)
	if applyfriend_count_value == TASK_DATA_INVALID then
		applyfriend_count_value = 0
	end
	
	return "("..applyfriend_count_value.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 找个师傅
-- @param count : 师徒个数
-------------------------------------------------------------------
ConditionCheckIsMaster = Object:new
{

}

function ConditionCheckIsMaster:GetRegisterGameEvent()

	return TaskGameEventType.EventType_BUIDMENTORSHIP
end

function ConditionCheckIsMaster:GetConditionKey(holder_id, task)
	
	return 0
end

function ConditionCheckIsMaster:Test(holder_id, task)

	if holder_id == nil or task == nil then
		return false
	end

	return CheckIsPlayerPrentice(holder_id, task.publisher_id) > 0
end

function ConditionCheckIsMaster:Hint(holder_id, task)

	if holder_id == nil or task == nil then
		return ""
	end

	return ""
end

-------------------------------------------------------------------
-- 条件 : 检查玩家交互回复次数 
-- @param count : 回复次数
-------------------------------------------------------------------
ConditionContactReplyCount = Object:new
{
	count = nil
}

function ConditionContactReplyCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_CONTACTREPLY
end

function ConditionContactReplyCount:GetConditionKey(holder_id, task)

	if holder_id == nil or task == nil then
		return 0
	end
	
	return GetContactReplyCountKey(holder_id, task.id)
end

function ConditionContactReplyCount:Test(holder_id, task)

	if self.count == nil then
		return false
	end
	
	if holder_id == nil or task == nil then
		return false
	end
	
	local reply_count_key = GetContactReplyCountKey(holder_id, task.id)
	local reply_count_value = GetTaskIntData(holder_id, reply_count_key)
	if reply_count_value == TASK_DATA_INVALID then
		reply_count_value = 0
	end

	return reply_count_value >= self.count
end

function ConditionContactReplyCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local reply_count_key = GetContactReplyCountKey(holder_id, task.id)
	local reply_count_value = GetTaskIntData(holder_id, reply_count_key)
	if reply_count_value == TASK_DATA_INVALID then
		reply_count_value = 0
	end
	
	return "("..reply_count_value.."/"..self.count..")"
end

-------------------------------------------------------------------
-- 条件 : 检查玩家交互PK次数 
-- @param count : PK次数
-------------------------------------------------------------------
ConditionPKCount = Object:new
{
	count = nil
}

function ConditionPKCount:GetRegisterGameEvent()

	return TaskGameEventType.EventType_PK
end

function ConditionPKCount:GetConditionKey(holder_id, task)

	if holder_id == nil or task == nil then
		return 0
	end
	
	return GetContactPKCountKey(holder_id, task.id)
end

function ConditionPKCount:Test(holder_id, task)

	if self.count == nil then
		return false
	end

	if holder_id == nil or task == nil then
		return false
	end
	
	local contactpk_count_key = GetContactPKCountKey(holder_id, task.id)
	local contactpk_count_value = GetTaskIntData(holder_id, contactpk_count_key)
	if contactpk_count_value == TASK_DATA_INVALID then
		contactpk_count_value = 0
	end

	return contactpk_count_value >= self.count
end

function ConditionPKCount:Hint(holder_id, task)

	if self.count == nil then
		return ""
	end

	if holder_id == nil or task == nil then
		return ""
	end
	
	local contactpk_count_key = GetContactPKCountKey(holder_id, task.id)
	local contactpk_count_value = GetTaskIntData(holder_id, contactpk_count_key)
	if contactpk_count_value == TASK_DATA_INVALID then
		contactpk_count_value = 0
	end
	
	return "("..contactpk_count_value.."/"..self.count..")"
end




