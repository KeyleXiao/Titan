-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/25/2016
-- ��  ��:	1.0
-- ��  ��:	���������
-------------------------------------------------------------------

---------------------------------------------------
-- LUA��ں��������ڼ���LUA�ļ�
---------------------------------------------------
function lua_main()
	--error("execute lua_main().......")
	-- ���� Hepler.lua �ļ�����Ϸ��������
	require ("Hepler")
	-- ���� Global.lua �ļ���ȫ�ֺ���
	require ("Global")
	--  ���� HelpFunc.lua �ļ���������������
	require ("HelpFunc")
	--  ���� LuaEvent.lua �ļ��� lua�¼�����
	require ("LuaEvent")
	--  ���� Class.lua �ļ�����̳�
	require ("Class")
	--  ���� SupperStone.lua �ļ���������������
	require ("SupperStone")

	--  ���� NPCHelper.lua �ļ���NPC���
	require ("NPCHelper")
	--  ���� NPCInit.lua �ļ�
	require ("NPCInit")

	--  ���� InitTask.lua �ļ�����ʼ������
	require ("InitTask")
end

--[[
-- csv ��ȡ�÷�
local task_table = commonHepler.readCsvFile("NPC.csv")
error("task_table="..TableToString(task_table))
if task_table ~= nil then
	for index,task_info in pairs(task_table) do
		error("index="..index.."task_info="..TableToString(task_info))
	end
end
]]

--[[
-- ��ʱ���������÷�
function Test(nTriggerID)
	local nSceneID = GetLuaSceneID()
	error("Test 8888888888888 nTriggerID="..nTriggerID..",nSceneID="..nSceneID)
end
local nSceneID = GetLuaSceneID()
triggerHepler.createTimerTrigger(nSceneID, "Test", 1000, 3)
--]]