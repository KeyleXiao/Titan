-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/30/2016
-- ��  ��:	1.0
-- ��  ��:	��ʤ����
-- ��ʤ�������������״̬����1/2���������ͨ��ʤ����2/2�������������ʤ����
-------------------------------------------------------------------

require "TaskFramePlayDotaGame"

-------------------------------------------------------------------
-- ��ʤ
-------------------------------------------------------------------
Task1_FirstWin = TaskFramePlayDotaGame:new
{
}

----------------------------------------------------------------------------
-- ��DOTA��Ϸִ��

-- param1 : �����б�
-- param2 : �������ͳ��
-- param2������1.Ӣ������2.��������
-- 			   3.�Ƿ��������4.�Ƿ�ʦͽ���5.�Ƿ�ʤ��6.�Ƿ���7.�Ƿ�MVP8.�Ƿ�������9.�Ƿ�������10.�Ƿ��������11.�Ƿ��Ǯ���
-- 			   12.�����ɱ����13.�������ɱ����14.��ɱ����15.��������16.ս������17.��ý�Ǯ����18.��ʤ��������19.���վ�������20.��������21.��ɱС������22.��ɱ��������23.��������
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
	
	-- �ж��Ƿ�ʤ��
	if tWarResultData[5] ~= 1 then
		return
	end
	
	-- ȡ�����ʤ��������������(��ͨ�����1�Σ����������2��)
	local task_key = GetWinCountTaskKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- ���1̬���ٴ���ͨ��ɹ���
	if tWarResultData[3] ~= 1 and task_value == 1 then
		return
	end
	
	-- �������״̬����
	self:SetState(TaskState.FinishState)
	
	if tWarResultData[3] ~= 1 then	-- ���1̬
		SetTaskIntData(self.holder_id, task_key, 1)	
	else							-- ���2̬
		SetTaskIntData(self.holder_id, task_key, 2)
		-- ע����DOTA��Ϸ�¼�
		self:UnregisterPlayDotaGame()
	end
	
	-- �����¼�
	OnActorTaskListChanged(self.holder_id, self.id, "accomplish")
end

-- ������ʾ 
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

-- ��ɫ��½
function Task1_FirstWin:OnLogin()

	-- ��������
	Task.OnLogin(self)

	-- ����״̬�����1̬ �����¼�
	local win_count_key = GetWinCountTaskKey(self.holder_id, self.id)
	local win_count_value = GetTaskIntData(self.holder_id, win_count_key)
	if self.state < TaskState.FinishState or (self.state == TaskState.FinishState and win_count_value == 1) then
		-- ע����DOTA��Ϸ�¼�
		self:RegisterPlayDotaGame()
	end
end

-- ��������
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
	
	-- ��������
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
	
	-- ���Ž���
	for index,action in pairs(tFinishAction) do
		action:run(self.holder_id, self)
	end
	
	-- ���Ϳͻ���
	local strPrize = "<id>"..self.id.."</id><name>"..self.name.."</name><description>"..self.description.."</description><binding_gold>0</binding_gold><exp>0</exp><prize>"..strPrizeGood.."</prize>"
	SendTaskPrize(self.holder_id, strPrize)
	OssMonitorTask(self.id, 0, 1, 0)
	
	-- ɾ������
	RemoveTaskData(self.holder_id, task_key)
	
	-- ע����DOTA��Ϸ�¼�
	self:UnregisterPlayDotaGame()
end