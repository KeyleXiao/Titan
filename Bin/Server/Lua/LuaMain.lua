-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/25/2016
-- 版  本:	1.0
-- 描  述:	主函数入口
-------------------------------------------------------------------

---------------------------------------------------
-- LUA入口函数，用于加载LUA文件
---------------------------------------------------
function lua_main()
	--error("execute lua_main().......")
	-- 加载 Hepler.lua 文件，游戏帮助函数
	require ("Hepler")
	-- 加载 Global.lua 文件，全局函数
	require ("Global")
	--  加载 HelpFunc.lua 文件，公共帮助函数
	require ("HelpFunc")
	--  加载 LuaEvent.lua 文件， lua事件定义
	require ("LuaEvent")
	--  加载 Class.lua 文件，类继承
	require ("Class")
	--  加载 SupperStone.lua 文件，七龙珠操作相关
	require ("SupperStone")

	--  加载 NPCHelper.lua 文件，NPC相关
	require ("NPCHelper")
	--  加载 NPCInit.lua 文件
	require ("NPCInit")

	--  加载 InitTask.lua 文件，初始化任务
	require ("InitTask")
end

--[[
-- csv 读取用法
local task_table = commonHepler.readCsvFile("NPC.csv")
error("task_table="..TableToString(task_table))
if task_table ~= nil then
	for index,task_info in pairs(task_table) do
		error("index="..index.."task_info="..TableToString(task_info))
	end
end
]]

--[[
-- 定时器触发器用法
function Test(nTriggerID)
	local nSceneID = GetLuaSceneID()
	error("Test 8888888888888 nTriggerID="..nTriggerID..",nSceneID="..nSceneID)
end
local nSceneID = GetLuaSceneID()
triggerHepler.createTimerTrigger(nSceneID, "Test", 1000, 3)
--]]