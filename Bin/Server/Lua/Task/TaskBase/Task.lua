-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/29/2016
-- ��  ��:	1.0
-- ��  ��:	��������������Լ�����
-------------------------------------------------------------------

require "Class"
require "TaskData"
require "Condition"
require "Action"

Task = Object:new
{
	id			= 0  ,               -- ����ID
	name		= "δ֪����",        -- ��������
	load_name	= "",				 -- ������ؽű�����
	type		= 1,				 -- ��������
	trace_show	= 1,				 -- ����׷����ʾλ��
	sort_level  = 0,				 -- ���ȼ�����
	description = "",				 -- ��������
	prompt_type = 0,				 -- ��ʾ����
	save        = true,				 -- �������߱���
	allow_delete = true,			 -- �Ƿ�����ɾ��
	loop_type	= 0,				 -- ѭ������
	flush_time  = 0,				 -- ˢ��ʱ��
	day_count	= nil,				 -- �մ���
	week_count  = nil,				 -- �ܴ���
	clear_type	= 0,				 -- �����������������0�����1ÿ�����2ÿ�������
	tAcceptCondition = {},			 -- ��������
	tFinishCondition = {},			 -- �������
	prize_type = 0,					 -- ������ʾ����
	tAcceptAction	 = {},			 -- ���ܶ���
	tFinishAction	 = {},			 -- ��ɶ���
	tDeleteAction    = {},			 -- ɾ������
	tFinishConditionKey	 = {},		 -- ��ע���������key
	tGameEventFunList = {},			 -- �¼�ע�ắ��

	holder_id       = nil,			 -- ����Ľ�ɫID(DBID)
	state			= nil,	 		 -- ����״̬
	tProgressData   = nil,			 -- ��������
	accept_progress = 1,			 -- ����������һ����ʼ��������
	max_progress    = 2,			 -- �������ܹ��м��ζԻ�
	tProcessCondition = {},			 -- �����������
	hit = "",					     -- ��������ʾ
	
	contact_high32id= 0,			 -- ����ID��32λ
	contact_low32id = 0, 			 -- ����ID��32λ
	publisher_id	= 0,			 -- �������񷢲���ID
	
	sub_type		= 0,			 -- ����������
	sub_type_name	= "",			 -- ��������������
	specific_type	= 0,			 -- �����������¾�������
	specific_type_name	= "",		 --	�����������¾�����������
	publish_description = "",		 -- ����˵��
	emotion_words		= "",		 -- ���黰��
	task_level		= 0,			 -- �����Ǽ�
}

-------------------------------------------------------------------
-- ���캯��
-------------------------------------------------------------------
function Task:construct()

	if self.holder_id ~= nil then
		return
	end

	if self.id == 0 then
		return
	end

	-- ����������:�ȼ�
	if self.accept_condition_level ~= nil then
		InsertUni(self.tAcceptCondition, ConditionLevel:new{min_level=self.accept_condition_level[1], max_level=self.accept_condition_level[2]})
	end
	
	-- ����������:ǰ������
	if self.pre_id ~= nil then
		InsertUni(self.tAcceptCondition, ConditionCheckPreTaskFinish:new{pre_taskid=self.pre_id})
	end
	
	-- ÿ��ѭ����������
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

	-- ��ɶ���:�����󶨽�Ǯ
	if self.prize_binding_gold ~= nil then
		InsertUni(self.tFinishAction, ActionBinding:new{binding_gold=self.prize_binding_gold})
	end

	-- ��ɶ���:������ɫ����
	if self.prize_hero_exp ~= nil then
		InsertUni(self.tFinishAction, ActionHeroExp:new{hero_exp=self.prize_hero_exp})
	end

	-- ��ɶ���:������Ʒ
	if self.prize_good ~= nil then
		for index,prize_id in pairs(self.prize_good) do
			InsertUni(self.tFinishAction, ActionAddPrize:new{prize_id=prize_id, prize_count=1})
		end
	end
end

-- ��ȡ����״̬
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

-- ��������״̬
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

-- ȡ���������
function Task:GetProgress()

	if self.tProgressData == nil then
		return 1
	end

	return self.tProgressData
end

-- �����������
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

-- ������ȸ���
function Task:UpdateProgress()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if TASK_LIST[self.holder_id] == nil or TASK_LIST[self.holder_id][self.id] == nil then
		return
	end

	OnActorTaskListChanged(self.holder_id, self.id, "updateprogress")
end

-- �ܷ��������
-- @param hard : �Ƿ�ֻ���������� hard condition
--             : ��������Ƿ���,����Ͳ���һ����������,��Ϊ����ǿ����޸ĵ�,���ȼ�Ҫ����Ǹ�������
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

-- �ܷ��������
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
			-- ��������ԭ��
			if return_reason==true and condition.hint~=nil then
				return false,condition:Hint(self.holder_id, self)
			end
			return false
		end
	end

	return true
end

-- �Ƿ���Խ�������
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
				-- ��������ԭ�� 
				if return_reason==true and condition.hint~=nil then 
					return false,condition:Hint(self.holder_id, self)
				end
				return false	
			end
		end
	end
	
	return true
end

-- �Ƴ������������
function Task:RemoveFinishConditionTaskData()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	for task_key,_ in pairs(self.tFinishConditionKey) do
		RemoveTaskData(self.holder_id, task_key)
	end
	
	self.tFinishConditionKey = {}
end

-- ��������
function Task:Accept()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	for index,action in pairs(self.tAcceptAction) do
		action:run(self.holder_id, self)
	end

	-- ���óɽ��ܽ���
	self:SetProgress(self.accept_progress+1)
	
	self:SetState(TaskState.AcceptState)
	
	-- �������񣬼�¼����ID
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

-- �������
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

-- ��������
function Task:End()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	self:SetState(TaskState.EndState)
	
	for index,action in pairs(self.tFinishAction) do
		action:run(self.holder_id, self)
	end

	-- ��ɶ���:�����󶨽�Ǯ
	local strPrizeBindingGold = ""
	if self.prize_binding_gold ~= nil then
		strPrizeBindingGold = self.prize_binding_gold
	end

	-- ��ɶ���:������ɫ����
	local strPrizeHeroExp = ""
	if self.prize_hero_exp ~= nil then
		strPrizeHeroExp = self.prize_hero_exp
	end

	-- ��ɶ���:������Ʒ
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

-- ɾ������
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
	
	-- ����������ݣ����������ظ���
	-- �ѽ���������������
	-- ��ʤ��������
	if (self.type == TaskType.MainType and self.state == TaskState.EndState) or self.type == TaskType.FirstWinType then
		return
	end
	local task_key = GetTaskIDKey(self.holder_id, self.id)
	RemoveTaskData(self.holder_id, task_key)
	
	-- �����������ID���ݣ������ظ���
	if self.type == TaskType.ContacType then

		local contact_high32id_key = GetContactHigh32IDKey(self.holder_id, self.id)
		RemoveTaskData(self.holder_id, contact_high32id_key)
		local contact_low32id_key = GetContactLow32IDKey(self.holder_id, self.id)
		RemoveTaskData(self.holder_id, contact_low32id_key)
		local contact_publisherid_key = GetContactPublisherIDKey(self.holder_id, self.id)
		RemoveTaskData(self.holder_id, contact_publisherid_key)

	end
end

-- ��ɫ��½
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
	
	-- �������񣬶�ȡ����ID
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

-- ��ɫ�ǳ�
function Task:OnLogout()
end

-- �������ݸ���
function Task:Update()

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if TASK_LIST[self.holder_id] == nil or TASK_LIST[self.holder_id][self.id] == nil then
		return
	end

	OnActorTaskListChanged(self.holder_id, self.id, "update")
end

-- �Ƿ������
function Task:IsFinished()

	if self.holder_id == nil or self.holder_id <= 0 then
		return false
	end
	
	return self:GetState() == TaskState.FinishState
end

-- �Ƿ�����ȡ
function Task:IsEnd()

	if self.holder_id == nil or self.holder_id <= 0 then
		return false
	end
	
	return self:GetState() == TaskState.EndState
end

-- ȡ��������մ���
function Task:GetTaskDayCount()

	if self.holder_id == nil or self.holder_id <= 0 then
		error("GetDailyRepeat error self.holder_id,id="..self.id)
		return 0
	end

    local day_time_key = GetDayTimeTaskKey(self.holder_id, self.id)
    local day_time_value = GetTaskIntData(self.holder_id, day_time_key)

	-- ���ǽ��������Ҫ���
 	if day_time_value ~= TASK_DATA_INVALID then

        local day_count_key = GetDayCountTaskKey(self.holder_id, self.id)
		
		-- Сʱѭ��
		if self.loop_type == 1 then

			local nTime = os.time()
			-- ֻ��ʱ����ڲ����ݴ洢ʱ��Ų���(��ֹ��������ͬ��ˢ����)
			if nTime > day_time_value then
				if nTime >= day_time_value + self.flush_time*3600 then
					RemoveTaskData(self.holder_id, day_time_key)
					RemoveTaskData(self.holder_id, day_count_key)
					return 0
				end
			end
		else
			-- �����������loop_type�����Ͳ���
			local today = GetGameDate(self.reset_time)
			local nTime = os.time(today)
			-- ֻ��ʱ����ڲ����ݴ洢ʱ��Ų���(��ֹ��������ͬ��ˢ����)
			if nTime > day_time_value then
				-- ����ͬһ��
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

-- ����������մ���
function Task:SetTaskDayCount(count)

 	if self.holder_id == nil or self.holder_id <= 0 then
		error("SetDailyRepeat error self.holder_id,id="..self.id)
		return 0
	end

    local day_time_key = GetDayTimeTaskKey(self.holder_id, self.id)
	-- Сʱѭ��
	if self.loop_type == 1 then
		local nTime = os.time()
		SetTaskIntData(self.holder_id, day_time_key, nTime)
	else
		-- �����������loop_type�����Ͳ���
		local today = GetGameDate(self.reset_time)
		local nTime = os.time(today)
		SetTaskIntData(self.holder_id, day_time_key, nTime)
	end

    local day_count_key = GetDayCountTaskKey(self.holder_id, self.id)
    SetTaskIntData(self.holder_id, day_count_key, count)
end


-- ȡ�������ܴ���
function Task:GetTaskWeekCount()

	if self.holder_id == nil or self.holder_id <= 0 then
		error("GetWeekRepeat error self.holder_id,id="..self.id)
		return 0
	end

	local week_time_key = GetWeekTimeTaskKey(self.holder_id, self.id)
	local week_time_value = GetTaskIntData(self.holder_id, week_time_key)

	-- ���ǽ��������Ҫ���
	if week_time_value ~= TASK_DATA_INVALID then
		-- ������һ��0����
		local time_value = GetGameDate(self.reset_time)
		local nTime = os.time(time_value)

		local week_count_key = GetWeekCountTaskKey(self.holder_id, self.id)

		-- ֻ��ʱ����ڲ����ݴ洢ʱ��Ų���(��ֹ��������ͬ��ˢ����)
		if nTime > week_time_value then
			-- ����ͬһ��
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

-- ���������ܴ���
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

-- ȡ��������
function Task:GetHit()

	if self.holder_id == nil or self.holder_id <= 0 then
		return ""
	end

	if self.id == nil or self.id <= 0 then
		return ""
	end

	-- �ж��Ƿ����
	--if self:IsFinished() == true or self:IsEnd() == true then
	--	return ""
	--end
	--local strHit = self.hit or ""
	--for _,condition in pairs(self.tFinishCondition) do
	--	strHit = strHit..condition:Hint(self.holder_id, self)
	--end
	
	-- �ж��Ƿ����
	local strHit = self.hit or ""
	if self:IsFinished() == true or self:IsEnd() == true then
		-- ���״̬
		for _,condition in pairs(self.tFinishCondition) do
			if condition.count ~= nil then
				strHit = strHit.."("..condition.count.."/"..condition.count..")"
			end
		end
	else
		-- δ���״̬
		for _,condition in pairs(self.tFinishCondition) do
			strHit = strHit..condition:Hint(self.holder_id, self)
		end
	end
	
	return strHit
end