-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	周坤
-- 日  期:	8/4/2017
-- 版  本:	1.0
-- 描  述:	奇妙的缘份
-------------------------------------------------------------------

require "TaskFrameActorData"

-------------------------------------------------------------------
-- 奇妙的缘份
-------------------------------------------------------------------
Task_306_Friend = TaskFrameActorData:new
{
	tFinishCondition = { ConditionApplyFriendCount:new{count=1} },			 -- 完成条件
}

----------------------------------------------------------------------------
-- 奇妙的缘份
-- param1 : 好友PDBID
----------------------------------------------------------------------------
function Task_306_Friend:OnEventNormalProcess(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if tGameData == nil then
		return
	end

	-- 判断交友对象
	if tGameData.nParam1 == nil or tGameData.nParam1 ~= self.publisher_id then
		return
	end
	
	local task_key = GetContactApplyFriendCountKey(self.holder_id, self.id)
	local task_value = GetTaskIntData(self.holder_id, task_key)
	if task_value == TASK_DATA_INVALID then
		task_value = 0
	end
	-- 设置交友申请次数
	SetTaskIntData(self.holder_id, task_key, task_value+1)

	TaskFrameActorData.OnEventNormalProcess(self, tGameData)
end