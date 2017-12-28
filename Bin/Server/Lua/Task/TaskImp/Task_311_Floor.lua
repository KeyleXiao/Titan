-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	周坤
-- 日  期:	8/4/2017
-- 版  本:	1.0
-- 描  述:	准备盖楼
-------------------------------------------------------------------

require "TaskFrameActorData"

-------------------------------------------------------------------
-- 准备盖楼
-------------------------------------------------------------------
Task_311_Floor = TaskFrameActorData:new
{
	tFinishCondition = { ConditionContactReplyCount:new{count=10} },			 -- 完成条件
}

----------------------------------------------------------------------------
-- 准备盖楼
-- param1 : 随机交互ID高32位
-- param2 : 随机交互ID低32位
----------------------------------------------------------------------------
function Task_311_Floor:OnEventNormalProcess(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end
	
	-- 判断交互ID
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
	-- 设置交互回复次数
	SetTaskIntData(self.holder_id, task_key, task_value+1)
	
	TaskFrameActorData.OnEventNormalProcess(self, tGameData)
end
