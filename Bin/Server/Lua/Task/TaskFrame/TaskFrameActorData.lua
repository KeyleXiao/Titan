-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	��ҫ��
-- ��  ��:	3/20/2017
-- ��  ��:	1.0
-- ��  ��:	��ɫ����
-------------------------------------------------------------------

require "Task"


-- ��ɫ���ݴ�����
g_ActorDataTaskTrigger = 0
-- ��ɫ�����¼�����ע��
g_tActorGameEvent = {}

-------------------------------------------------------------------
-- ��ɫ����
-------------------------------------------------------------------
TaskFrameActorData = Task:new
{
	
}

-------------------------------------------------------------------
-- ���캯��
-------------------------------------------------------------------
function TaskFrameActorData:construct()

	-- ע���¼�������
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
	
	-- �������:ӵ�ж��ٸ�Ӣ��
	if self.finish_condition_hero_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionHaveHeroCount:new{count=self.finish_condition_hero_count})
	end

	-- �������:ӵ�ж��ٸ�Ӣ��Ƥ��
	if self.finish_condition_heroskin_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionHaveSkinCount:new{count=self.finish_condition_heroskin_count})
	end
	
	-- �������:��ɫ�ȼ�
	if self.finish_condition_level ~= nil then
		InsertUni(self.tFinishCondition, ConditionLevel:new{min_level=self.finish_condition_level, max_level=self.finish_condition_level})
	end
	
	-- �������:�򿪱������
	if self.finish_condition_open_chest_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionOpenChestCount:new{count=self.finish_condition_open_chest_count})
	end
	
	-- �������:���˾�����
	if self.finish_condition_clan_donate_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionClanDonateCount:new{count=self.finish_condition_clan_donate_count})
	end
end

-- ��ɫ��½
function TaskFrameActorData:OnLogin()

	-- ��������
	Task.OnLogin(self)

	if self:GetState() < TaskState.FinishState then
		-- ע���ɫ�����¼�
		self:RegisterActorGameEvent()
	end
end

-- ��ɫ�˳�
function TaskFrameActorData:OnLogout()
	local actor = self.holder_id
	if actor == nil or actor <= 0 then
		return
	end

	Task.OnLogout()

	-- ��ս�ɫ�ڴ�����
	if g_tActorGameEvent[actor] ~= nil then
		g_tActorGameEvent[actor] = nil
	end
end


-- ��������
function TaskFrameActorData:Accept()

	-- ��������
	Task.Accept(self)

	if self:GetState() < TaskState.FinishState then
		-- ע���ɫ�����¼�
		self:RegisterActorGameEvent()
	end
end

-- �������
function TaskFrameActorData:Finish()

	-- �������
	Task.Finish(self)

	-- ע����ɫ�����¼�
	self:UnregisterActorGameEvent()
end

-- ��������
function TaskFrameActorData:End()

	-- ��������
	Task.End(self)

	-- ע����ɫ�����¼�
	self:UnregisterActorGameEvent()
end

-- ɾ������
function TaskFrameActorData:DeleteTask()

	-- ɾ������
	Task.DeleteTask(self)

	-- ע����ɫ�����¼�
	self:UnregisterActorGameEvent()
end

-- ע���ɫ�����¼�
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

-- ע����ɫ�����¼�
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

-- ���һ����Ӧ����
function TaskFrameActorData:OnEventNormalProcess(tGameData)
	
	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	-- �ж��Ƿ����
	if self:CanFinish() == true then
		self:Finish()
		-- �����¼�
		OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
	else
		self:Update()
	end
end

----------------------------------------------------------------------------
-- ��Ƕ��ʯ
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
	-- ������Ƕ��ʯ
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	-- �ж��Ƿ����
	if self:CanFinish() == true then
		
		self:Finish()
		-- �����¼�
		OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
	else
		self:Update()
	end
end

----------------------------------------------------------------------------
-- ������
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
	-- ���ÿ��������
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	-- �ж��Ƿ����
	if self:CanFinish() == true then
		
		self:Finish()
		-- �����¼�
		OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
	else
		self:Update()
	end
end

----------------------------------------------------------------------------
-- ���˾���
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
	-- �������˾�������
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	-- �ж��Ƿ����
	if self:CanFinish() == true then
		
		self:Finish()
		-- �����¼�
		OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
	else
		self:Update()
	end
end

-- ��Ӧ��������¼�
function TaskFrameActorData:OnActorGameEvent(nEvent, tGameData)

	if self.tGameEventFunList[nEvent] == nil then
		error("OnGameEvent faild" .. nEvent)
		return
	end
	
	-- �����¼���Ӧ����
	self.tGameEventFunList[nEvent](self, tGameData)
end

-- �������������Ϸ�¼�
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

	-- bool����c++�����ת����int
	local tGameData = {}
	tGameData.nParam1 = nParam1
	tGameData.nParam2 = nParam2
	tGameData.nParam3 = nParam3

	for id,task in pairs(tDataInfo) do
		task:OnActorGameEvent(nEventID, tGameData)
	end
end
