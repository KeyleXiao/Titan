-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李有红
-- 日  期:	2017/1/18
-- 版  本:	1.0
-- 描  述:	NPC对话输出 
-------------------------------------------------------------------

require("NPC")

-------------------------------------------------------------------
-- 输出NPC默认对白 
-- @param npc    : NPC对象
-- @param npc_id : NPC逻辑ID 
-- @param actor  : 点击的人
-- @param topic_list: 主题列表 
-------------------------------------------------------------------
function DefaultNPCDialogHandler(npc,npc_id,actor,topic_list)
	table.insert(topic_list,{"闲谈","DefaultNPCDialog",{npc_id,npc,actor},0,'#FFFFFF',nil,3,false})
end

-------------------------------------------------------------------
-- 输出NPC默认对白
-- @param npc_id : NPC逻辑ID 
-- @param actor  : 点击的人
-------------------------------------------------------------------
function DefaultNPCDialog(npc,npc_id,actor)
	local NPC_info = NPC_TABLE[npc_id]
	if NPC_info==nil then
		return
	end
	
	local dialog_list = NPC_info["dialog"]
	if dialog_list==nil then
		return
	end
	
	local dialog_content = dialog_list[math.random(#dialog_list)]
	
	local strRes = "<npc name=\""..NPC_info.name.."\" dialog=\""..dialog_content.."\"></npc>"
	
	ResponseNPC(actor, npc, strRes)
end

----------------------------------------------------------------------------------------
-- 输出多个话题供玩家选择
-- @param actor: 玩家ID
-- @param topic_list: 话题列表
-- @param npc: 对话的npc
----------------------------------------------------------------------------------------
function MultiTopicDialog(actor, topic_list, npc)
	local entry_list = {{}, {}, {}}
	
	-- 根据功能分类进行分区
	for index, params in pairs(topic_list) do
		if params[7] == nil then
			params[7] = 1
		end
		
		table.insert(entry_list[params[7]], params)
	end
	
	
	local dialog_content = nil
	local npc_name = nil
	local npc_view_type = nil
	-- 输出默认对白
	if entry_list[3][1] ~= nil then
		local npc_id = entry_list[3][1][3][1]
		local NPC_info = NPC_TABLE[npc_id]

		npc_name = NPC_info.name
		npc_view_type = NPC_info.npc_view_type
		local dialog_list = NPC_info["dialog"]
		if dialog_list ~= nil then
			local dialog_num = #dialog_list
			if dialog_num >= 1 then
				dialog_content = dialog_list[math.random(1, dialog_num)]
			end
		end
		entry_list[3] = nil
	end
	local strRes = "<npc uid=\""..npc.."\" npc_view_type=\""..npc_view_type.."\" name=\""..npc_name.."\" dialog=\""..dialog_content.."\">"

	-- 输出功能区
	for i = #entry_list, 1, -1 do
		local entrys = entry_list[i]
		if #entrys > 0 then
			for index, params in pairs(entrys) do
				-- 该话题是功能
				table.insert(params[3],1,params[2])
				if params[7] == 1 then
					strRes = strRes.."<topic function=\"HTMLRequestHandler\"".." params=\""..BuildParamsString(params[3]).."\">"..params[1].."</topic>"
				end
			end
		end
	end
	strRes = strRes.."</npc>"
	
	ResponseNPC(actor, npc, strRes)
end

----------------------------------------------------------------------------------------
-- 把参数列表转换为分号分隔的字符串
-- @param params: 参数列表
----------------------------------------------------------------------------------------
function BuildParamsString(params)
	local str = ""
	for index, param in pairs(params) do
		
		-- 参数间用分号分隔
		if index ~= 1 then
			str = str..";"
		end
	
		-- 第一个参数为函数名，后面的为该函数的参数
		-- 其中参数类型为字符串则以"**"为前缀；为数字则没有前缀
		if index == 1 then             
			str = str..param
		elseif type(param) == 'string' then
			str = str.."**"..param
		elseif type(param) == 'number' then
			str = str..param
		else
			warning("HTML link can only accept string or numeral params.");
		end
	end
	
	return str
end