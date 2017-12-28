-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���к�
-- ��  ��:	2017/2/14
-- ��  ��:	1.0
-- ��  ��:	NPC���ܴ�����
-------------------------------------------------------------------

require("NPC")
require("NPCDialog")
require("NPCFuncRegister")

-- NPC���ܴ�����ID
g_NPCFuncTriggerID = nil

-- ��ʼ��NPC���ܴ�����
function InitNPCFuncHandler()
	
	local scene_id = GetLuaSceneID()
    g_NPCFuncTriggerID = CreateNPCFuncTrigger(scene_id, "NPCFuncTriggerHandler")
    if g_NPCFuncTriggerID == 0 then
		error("InitNPCFuncHandler CreateNPCFuncTrigger fail")
		return false
    end
	
	-- ���ĳ���ж���¼�
	g_luaEventPublisher:Subscribe(LUA_EVENT_SCENE_UNLOAD, ReleaseNPCFuncHandler)
	
	trace("InitNPCFuncHandler success")
	
	return true
end

-- ж��NPC���ܴ�����
function ReleaseNPCFuncHandler()
	if DeleteTrigger(g_NPCFuncTriggerID) == false then
		error("Delete NPCFuncTrigger failed, nTriggerID="..g_NPCFuncTriggerID)
	end
	
end

-------------------------------------------------------------------
-- ����NPC����¼�
-- @param npc    	: NPC����
-- @param npc_id    : NPC�߼�ID
-- @param actor  	: �������
-- @param html_req	: html�����ַ���
-------------------------------------------------------------------
function NPCFuncTriggerHandler(npc, npc_id, actor, html_req)
	-- ��ȡ�������Ͳ���
	local pos = string.find(html_req, '?')
	local func_str = string.sub(html_req, 1, pos-1)
	local params_str = string.sub(html_req, pos+1, -1)
	
	CheckSceneServerRegister(actor, func_str, params_str)
end

------------------------------------------------------------------
-- ��鳡����ע�ắ��
-- @param actor  	: ���
-- @param func_str	: ��������
-- @param params_str: �����ַ���
------------------------------------------------------------------
function CheckSceneServerRegister(actor, func_str, params_str)
	if func_str == "HTMLRequestHandler" then
		return HTMLRequestHandler(actor, func_str, params_str)
	end
	
	local prefix = string.sub(func_str, 1, 7)
	if prefix ~= "Request" then
		warning("�ͻ�����ͼ����Ƿ���lua����")
		return
	end
	
	local func = _G[func_str]
	if func == nil then
		return
	end
	
	local now = GetTickCountEx()
	
	-- ִ�к���
	func()
	
	local ticks = GetTickCountEx() - now
	if ticks > 100 then
		error("HTMLRequestHandler:func_str="..func_str..",ticks="..ticks..",pdbid="..actor)
	end
	
end

------------------------------------------------------------------
-- HTML��������
-- @param actor  	: ���
-- @param func_str	: ��������
-- @param params_str: �����ַ���
------------------------------------------------------------------
function HTMLRequestHandler(actor, func_str, params_str)	
	local params_list = ParseHTMLParams(params_str)
	
	if #params_list == 0 then
		return
	end
	
	-- ��һ������Ϊ��������
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
	
	-- ���ú���
	func(table.unpack(params_list))
	
	local ticks = GetTickCountEx() - now
	if ticks > 100 then
		error("HTMLRequestHandler:func_str="..func_str..",ticks="..ticks..",pdbid="..actor)
	end
end

------------------------------------------------------------------
-- ����html���ӵĲ��������ز����б�
-- ��ʽ���ӣ�1=RequestBuyWings&2=**lyh&3=123
-- params_str	: html���ӵĲ���
------------------------------------------------------------------
function ParseHTMLParams(params_str)
	local str_list1 = split_string(params_str, '&')
	
	local params_list = {}
	for index, str in pairs(str_list1) do
		local str_list2 = split_string(str, '=')
		
		-- �ж��Ƿ�Ϊ��ֵ
		local param = str_list2[2]
		if param ~= "" then
			-- ��һ������Ϊ��������
			if index == 1 then
				table.insert(params_list, param)
			-- "**"��ͷ��Ϊ�ַ�������
			elseif string.byte(param,1) == 42 and string.byte(param,2) == 42 then
				table.insert(params_list, string.sub(param, 3))
			else
				table.insert(params_list, tonumber(param))
			end
			
		end

	end
	return params_list
end