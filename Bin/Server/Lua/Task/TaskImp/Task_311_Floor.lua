-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	����
-- ��  ��:	8/4/2017
-- ��  ��:	1.0
-- ��  ��:	׼����¥
-------------------------------------------------------------------

require "TaskFrameActorData"

-------------------------------------------------------------------
-- ׼����¥
-------------------------------------------------------------------
Task_311_Floor = TaskFrameActorData:new
{
	tFinishCondition = { ConditionContactReplyCount:new{count=10} },			 -- �������
}

----------------------------------------------------------------------------
-- ׼����¥
-- param1 : �������ID��32λ
-- param2 : �������ID��32λ
----------------------------------------------------------------------------
function Task_311_Floor:OnEventNormalProcess(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	-- �жϽ���ID
	if tGameData.nParam1 == nil or tGameData.nParam1 ~= self.contact_high32id then
		return
	end
	if tGameData.nParam2 == nil or tGameData.nParam2 ~= self.contact_low32id then
		return
	end
	
	local task_key = GetContactReplyCountKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- ���ý����ظ�����
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	TaskFrameActorData.OnEventNormalProcess(self, tGameData)
end
