-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/30/2016
-- ��  ��:	1.0
-- ��  ��:	��ʤ����
-------------------------------------------------------------------

require "Task"

-- ս�������������ݴ�����
g_WarPersonTaskTrigger = 0
-- ��DOTA����Ϸ������
g_tPlayDotaGame = {}

-------------------------------------------------------------------
-- ��ʤ
-------------------------------------------------------------------
TaskFramePlayDotaGame = Task:new
{

}

-------------------------------------------------------------------
-- ���캯��
-------------------------------------------------------------------
function TaskFramePlayDotaGame:construct()

	-- ע���¼�������
	self.tGameEventFunList[TaskGameEventType.EventType_FINISHMATCH] = self.OnEventPlayDotaGame
	
	-- �������:ʤ������
	if self.finish_condition_win_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionWinCount:new{count=self.finish_condition_win_count})
	end

	-- �������:��ɴ���
	if self.finish_condition_finish_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionFinishCount:new{count=self.finish_condition_finish_count})
	end
	
	-- �������:��ɫ�����ܳ���
	if self.finish_condition_match_count ~= nil then
		InsertUni(self.tFinishCondition, ConditionMatchCount:new{count=self.finish_condition_match_count})
	end
	
end

-- ��ɫ��½
function TaskFramePlayDotaGame:OnLogin()

	-- ��������
	Task.OnLogin(self)

	if self:GetState() < TaskState.FinishState then
		-- ע����DOTA��Ϸ�¼�
		self:RegisterPlayDotaGame()
	end
end

-- ��ɫ�˳�
function TaskFramePlayDotaGame:OnLogout()
	local actor = self.holder_id
	if actor == nil or actor <= 0 then
		return
	end

	Task.OnLogout()

	-- ��ս�ɫ�ڴ�����
	if g_tPlayDotaGame[actor] ~= nil then
		g_tPlayDotaGame[actor] = nil
	end
end


-- ��������
function TaskFramePlayDotaGame:Accept()

	-- ��������
	Task.Accept(self)

	if self:GetState() < TaskState.FinishState then
		-- ע����DOTA��Ϸ�¼�
		self:RegisterPlayDotaGame()
	end
end

-- �������
function TaskFramePlayDotaGame:Finish()

	-- �������
	Task.Finish(self)

	-- ע����DOTA��Ϸ�¼�
	self:UnregisterPlayDotaGame()
end

-- ��������
function TaskFramePlayDotaGame:End()

	-- ��������
	Task.End(self)

	-- ע����DOTA��Ϸ�¼�
	self:UnregisterPlayDotaGame()
end

-- ɾ������
function TaskFramePlayDotaGame:DeleteTask()

	-- ɾ������
	Task.DeleteTask(self)

	-- ע����DOTA��Ϸ�¼�
	self:UnregisterPlayDotaGame()
end

-- ע����DOTA��Ϸ�¼�
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

-- ע����DOTA��Ϸ�¼�
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

-- ���һ����Ӧ����
function TaskFramePlayDotaGame:OnEventNormalProcess(tGameData)

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
-- ��DOTA��Ϸִ��

-- param1 : �����б�
-- param2 : �������ͳ��
-- param2������1.Ӣ������2.��������
-- 			   3.�Ƿ��������4.�Ƿ�ʦͽ���5.�Ƿ�ʤ��6.�Ƿ���7.�Ƿ�MVP8.�Ƿ�������9.�Ƿ�������10.�Ƿ��������11.�Ƿ��Ǯ���
-- 			   12.�����ɱ����13.�������ɱ����14.��ɱ����15.��������16.ս������17.��ý�Ǯ����18.��ʤ��������19.���վ�������20.��������21.��ɱС������22.��ɱ��������
----------------------------------------------------------------------------
function TaskFramePlayDotaGame:OnEventPlayDotaGame(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	-- ʤ������
	if self.finish_condition_win_count ~= nil then
		-- ȡ�����ʤ��������������
		local task_key = GetWinCountTaskKey(self.holder_id, self.id)
		local task_value = GetTaskIntData(self.holder_id, task_key)
		if task_value == TASK_DATA_INVALID then
			task_value = 0
		end
		-- ���������ɴ���
		SetTaskIntData(self.holder_id, task_key, task_value+1)
	end

	-- ��ɴ���
	if self.finish_condition_finish_count ~= nil then
		-- ȡ�������ɴ�����������
		local task_key = GetFinishCountTaskKey(self.holder_id, self.id)
		local task_value = GetTaskIntData(self.holder_id, task_key)
		if task_value == TASK_DATA_INVALID then
			task_value = 0
		end
		-- ���������ɴ���
		SetTaskIntData(self.holder_id, task_key, task_value+1)
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

-- ��Ӧ��Ϸ�¼�
function TaskFramePlayDotaGame:OnGameEvent(nEventID, tGameData)
	if self.tGameEventFunList[nEventID] == nil then
		error("OnGameEvent faild" .. nEventID)
		return
	end
	
	-- �����¼���Ӧ����
	self.tGameEventFunList[nEventID](self, tGameData)
end

-- ������DOTA��Ϸ�¼�
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

	-- bool����c++�����ת����int
	local tGameData = {}
	tGameData.nParam1 = nParam1
	tGameData.nParam2 = nParam2

	for id,task in pairs(tDataInfo) do
		task:OnGameEvent(nEventID, tGameData)
	end
end