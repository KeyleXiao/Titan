-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李有红
-- 日  期:	2017/2/14
-- 版  本:	1.0
-- 描  述:	NPC功能处理器
-------------------------------------------------------------------

require("NPC")
require("NPCDialog")
require("NPCFuncRegister")

-- NPC功能触发器ID
g_NPCFuncTriggerID = nil

-- 初始化NPC功能处理器
function InitNPCFuncHandler()
	
	local scene_id = GetLuaSceneID()
    g_NPCFuncTriggerID = CreateNPCFuncTrigger(scene_id, "NPCFuncTriggerHandler")
    if g_NPCFuncTriggerID == 0 then
		error("InitNPCFuncHandler CreateNPCFuncTrigger fail")
		return false
    end
	
	-- 订阅场景卸载事件
	g_luaEventPublisher:Subscribe(LUA_EVENT_SCENE_UNLOAD, ReleaseNPCFuncHandler)
	
	trace("InitNPCFuncHandler success")
	
	return true
end

-- 卸载NPC功能处理器
function ReleaseNPCFuncHandler()
	if DeleteTrigger(g_NPCFuncTriggerID) == false then
		error("Delete NPCFuncTrigger failed, nTriggerID="..g_NPCFuncTriggerID)
	end
	
end

-------------------------------------------------------------------
-- 处理NPC点击事件
-- @param npc    	: NPC对象
-- @param npc_id    : NPC逻辑ID
-- @param actor  	: 点击的人
-- @param html_req	: html请求字符串
-------------------------------------------------------------------
function NPCFuncTriggerHandler(npc, npc_id, actor, html_req)
	-- 提取处理函数和参数
	local pos = string.find(html_req, '?')
	local func_str = string.sub(html_req, 1, pos-1)
	local params_str = string.sub(html_req, pos+1, -1)
	
	CheckSceneServerRegister(actor, func_str, params_str)
end

------------------------------------------------------------------
-- 检查场景服注册函数
-- @param actor  	: 玩家
-- @param func_str	: 函数名字
-- @param params_str: 参数字符串
------------------------------------------------------------------
function CheckSceneServerRegister(actor, func_str, params_str)
	if func_str == "HTMLRequestHandler" then
		return HTMLRequestHandler(actor, func_str, params_str)
	end
	
	local prefix = string.sub(func_str, 1, 7)
	if prefix ~= "Request" then
		warning("客户端试图请求非法的lua函数")
		return
	end
	
	local func = _G[func_str]
	if func == nil then
		return
	end
	
	local now = GetTickCountEx()
	
	-- 执行函数
	func()
	
	local ticks = GetTickCountEx() - now
	if ticks > 100 then
		error("HTMLRequestHandler:func_str="..func_str..",ticks="..ticks..",pdbid="..actor)
	end
	
end

------------------------------------------------------------------
-- HTML请求处理器
-- @param actor  	: 玩家
-- @param func_str	: 函数名字
-- @param params_str: 参数字符串
------------------------------------------------------------------
function HTMLRequestHandler(actor, func_str, params_str)	
	local params_list = ParseHTMLParams(params_str)
	
	if #params_list == 0 then
		return
	end
	
	-- 第一个参数为函数名称
	local func_str = params_list[1]
	table.remove(params_list, 1)
	
	if g_RequestFuncList[func_str] ~= 1 then
		warning("This function isn't register.func_str="..func_str)
		return
	end
	
	local func = _G[func_str]

	if func == nil then
		return
	end
	
	local now = GetTickCountEx()
	
	-- 调用函数
	func(table.unpack(params_list))
	
	local ticks = GetTickCountEx() - now
	if ticks > 100 then
		error("HTMLRequestHandler:func_str="..func_str..",ticks="..ticks..",pdbid="..actor)
	end
end

------------------------------------------------------------------
-- 解析html链接的参数，返回参数列表
-- 格式例子：1=RequestBuyWings&2=**lyh&3=123
-- params_str	: html链接的参数
------------------------------------------------------------------
function ParseHTMLParams(params_str)
	local str_list1 = split_string(params_str, '&')
	
	local params_list = {}
	for index, str in pairs(str_list1) do
		local str_list2 = split_string(str, '=')
		
		-- 判断是否为空值
		local param = str_list2[2]
		if param ~= "" then
			-- 第一个参数为函数名称
			if index == 1 then
				table.insert(params_list, param)
			-- "**"开头的为字符串类型
			elseif string.byte(param,1) == 42 and string.byte(param,2) == 42 then
				table.insert(params_list, string.sub(param, 3))
			else
				table.insert(params_list, tonumber(param))
			end
			
		end

	end
	return params_list
end