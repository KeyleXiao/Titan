-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	12/1/2016
-- 版  本:	1.0
-- 描  述:	初始化任务系统
-------------------------------------------------------------------

require "TaskList"
require "TaskListManage"
require "TaskFactory"
require "TaskFramePlayDotaGame"

-- 初始化任务系统
function InitTaskSystem()
	
	-- 初始化任务列表
	if InitTaskList() == false then
 		error("InitTaskSystem InitTaskList Fail!!!")
		return false
	end

	-- 初始化任务列表管理
	InitTaskListManager()

	-- 读取任务列表 
	TaskFactory:ReadTaskList()
	
	return true
end


-- 初始化任务系统
if InitTaskSystem()==false then
	error("InitTaskSystem Fail!!!")
end