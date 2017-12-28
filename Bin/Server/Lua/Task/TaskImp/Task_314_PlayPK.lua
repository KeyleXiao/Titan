-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	周坤
-- 日  期:	8/4/2017
-- 版  本:	1.0
-- 描  述:	独孤求败
-------------------------------------------------------------------

require "TaskFrameActorData"

-------------------------------------------------------------------
-- 独孤求败
-------------------------------------------------------------------
Task_314_PlayPK = TaskFrameActorData:new
{
	tFinishCondition = { ConditionPKCount:new{count=2} },			 -- 完成条件
}

----------------------------------------------------------------------------
-- 独孤求败
----------------------------------------------------------------------------
function Task_314_PlayPK:OnEventNormalProcess(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	if tGameData == nil then
		return
	end
	
	if tGameData.nParam1 == nil or tGameData.nParam1 ~= self.publisher_id then
		return
	end
	
	local task_key = GetContactPKCountKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- 设置交互PK次数
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	TaskFrameActorData.OnEventNormalProcess(self, tGameData)
end