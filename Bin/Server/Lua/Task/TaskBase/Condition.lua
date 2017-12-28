-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/29/2016
-- ��  ��:	1.0
-- ��  ��:	����ģ��
-------------------------------------------------------------------

require "Class"
require "TaskDef"

--[[
  ���ǽ������е��������Ƴ����һ��������������ʵ��
  ��Щ��������������ɵķ�������ϵͳ�Ľ��������б�,�ƽ������б�,��������б���
  ʾ��:
  T1_FirstWin = Task:new{
    ...
  	tAcceptCondition={ConditionLevel:new{min_level=0,max_level=120}},   -- ��������������ȼ���Χ
    ...
  }
  
  ÿ������֧���ĸ�����:
  1.Test(actor,task) ���������в���
  2.Hint(actor,task) ����������������׷����Ϣ 
  3.GetConditionKey(actor,task) ��ȡ������ע��key
  4.GetRegisterGameEvent()		��ȡע����Ϸ�¼�

  ��ϵͳ֧�ֵ�������: 
  ConditionCheckTaskDayCount            : ���ÿ��ѭ������ [���������������ʵ��]
  ConditionCheckTaskWeekCount			: ���ÿ��ѭ������ [���������������ʵ��]
  ConditionCheckPreTaskFinish			: ���ǰ�������Ƿ���� [���������������ʵ��]
  ConditionCheckHaveClan				: ����Ƿ������� [���������������ʵ��]
  ConditionLevel						: �����ҵȼ� [���������������ʵ��]
  ConditionWinCount						: ������ʤ������ [���������������ʵ��]
  ConditionFinishCount					: ��������ɴ��� [���������������ʵ��]
  ConditionHaveHeroCount				: ������Ӣ��ӵ�и��� [���������������ʵ��]
  ConditionHaveSkinCount				: ������Ƥ��ӵ�и��� [���������������ʵ��]
  ConditionMatchCount					: �����ұ������� [���������������ʵ��]
  ConditionOpenChestCount				: �����Ҵ򿪱������ [���������������ʵ��]
  ConditionInlayCrownGem				: ��������Ƕ��ʯ���� [���������������ʵ��]
  ConditionHeroUpGrade					: ������Ӣ������ [���������������ʵ��]
  ConditionUseXp						: ������ʹ��xp [���������������ʵ��]
  ConditionKillMonsterCount				: �����һ�ɱĳ�������� [���������������ʵ��]
  ConditionUseWarWring					: ������ս��ʹ�ó�� [���������������ʵ��]
  ConditionMasterCount					: ������ʦ������ [���������������ʵ��]
  ConditionPrenticeCount				: ������ѧͽ���� [���������������ʵ��]
  ConditionMentorshipCount				: ������ʦ����ѧͽ���� [���������������ʵ��]
  ConditionClanDonateCount				: ���������˾��״��� [���������������ʵ��]
  ConditionCheckNewPlayerGuideStep		: ������������������������ [���������������ʵ��]
  
  ConditionApplyFriendCount				: �����ҽ������뽻�Ѵ��� [���������������ʵ��]
  ConditionCheckIsMaster				: �����ҽ�����ʦ������ [���������������ʵ��]
  ConditionContactReplyCount			: �����ҽ����ظ����� [���������������ʵ��]
  ConditionContactPKCount				: �����ҽ���PK���� [���������������ʵ��]
  
]]--

-------------------------------------------------------------------
-- ���� : ���ÿ��ѭ������
-- @param max_count : ���ѭ������
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
-- ���� : ���ÿ��ѭ������
-- @param max_count : ���ѭ������
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
-- ���� : ���ǰ�������Ƿ����
-- @param max_count : ���ѭ������
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
-- ���� : ����Ƿ�������
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
-- ���� : �жϵȼ�
-- @param min_level : ��͵ȼ�
-- @param max_level : ��ߵȼ�
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
-- ���� : ʤ������
-- @param count : ʤ������
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

	-- ȡ��ʤ�������������ݼ�ֵ
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

	-- ȡ��ʤ�������������ݼ�ֵ
	local win_count_key = GetWinCountTaskKey(holder_id, task.id)
	local win_count_value = GetTaskIntData(holder_id, win_count_key)
	if win_count_value == TASK_DATA_INVALID then
		win_count_value = 0
	end

	return "("..win_count_value.."/"..self.count..")"
end

-------------------------------------------------------------------
-- ���� : ��ɴ���
-- @param count : ��ɴ���
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

	-- ȡ����ɴ����������ݼ�ֵ
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

	-- ȡ��ʤ�������������ݼ�ֵ
	local finish_count_key = GetFinishCountTaskKey(holder_id, task.id)
	local finish_count_value = GetTaskIntData(holder_id, finish_count_key)
	if finish_count_value == TASK_DATA_INVALID then
		finish_count_value = 0
	end

	return "("..finish_count_value.."/"..self.count..")"
end

-------------------------------------------------------------------
-- ���� : ���Ӣ��ӵ�и���
-- @param count : ӵ�и���
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
-- ���� : ���Ƥ��ӵ�и���
-- @param count : ӵ�и���
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
-- ���� : ��ұ�������
-- @param count : ��������
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
-- ���� : ��Ҵ򿪱������
-- @param count : ���������
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
-- ���� : �����Ƕ��ʯ
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
-- ���� : ���Ӣ������
-- @param count : �ȼ�
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
-- ���� : ���ʹ��xp
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
-- ���� : ��һ�ɱĳ��������(1���� 2 С�� 3 ������С��)
-- @param count : ������
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
-- ���� : ���ս��ʹ�ó��
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
-- ���� : ���ʦ������
-- @param count : ʦ������
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
-- ���� : ���ѧͽ����
-- @param count : ѧͽ����
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
-- ���� : ���ʦͽ����
-- @param count : ʦͽ����
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
-- ���� : ������˾��״���
-- @param count : ���״���
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
-- ���� : ������������������������
-- @param count : ����
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
-- ���� : �����ҽ������뽻�Ѵ��� 
-- @param count : PK����
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
-- ���� : �Ҹ�ʦ��
-- @param count : ʦͽ����
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
-- ���� : �����ҽ����ظ����� 
-- @param count : �ظ�����
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
-- ���� : �����ҽ���PK���� 
-- @param count : PK����
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




