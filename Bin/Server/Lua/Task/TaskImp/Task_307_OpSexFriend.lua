-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	����
-- ��  ��:	8/4/2017
-- ��  ��:	1.0
-- ��  ��:	�����ҵ�����
-------------------------------------------------------------------

require "TaskFrameActorData"

-------------------------------------------------------------------
-- �����ҵ�����
-------------------------------------------------------------------
Task_307_OpSexFriend = TaskFrameActorData:new
{
	tFinishCondition = { ConditionApplyFriendCount:new{count=1} },			 -- �������
}

----------------------------------------------------------------------------
-- �����ҵ�����
-- param1 : ����PDBID
-- param2 : �Ƿ����� (1��0����)
----------------------------------------------------------------------------
function Task_307_OpSexFriend:OnEventNormalProcess(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if tGameData == nil then
		return
	end

	-- �жϽ��Ѷ���
	if tGameData.nParam1 == nil or tGameData.nParam1 ~= self.publisher_id then
		return
	end
	
	-- �ж��Ƿ�����
	if tGameData.nParam2 == nil or tGameData.nParam2 ~= 1 then
		return
	end
	
	local task_key = GetContactApplyFriendCountKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- ���ý����������
	SetTaskIntData(self.holder_id, task_key, task_value+1)

	TaskFrameActorData.OnEventNormalProcess(self, tGameData)
end