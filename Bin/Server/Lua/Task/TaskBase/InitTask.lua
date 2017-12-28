-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	12/1/2016
-- ��  ��:	1.0
-- ��  ��:	��ʼ������ϵͳ
-------------------------------------------------------------------

require "TaskList"
require "TaskListManage"
require "TaskFactory"
require "TaskFramePlayDotaGame"

-- ��ʼ������ϵͳ
function InitTaskSystem()
	
	-- ��ʼ�������б�
	if InitTaskList() == false then
 		error("InitTaskSystem InitTaskList Fail!!!")
		return false
	end

	-- ��ʼ�������б����
	InitTaskListManager()

	-- ��ȡ�����б� 
	TaskFactory:ReadTaskList()
	
	return true
end


-- ��ʼ������ϵͳ
if InitTaskSystem()==false then
	error("InitTaskSystem Fail!!!")
end