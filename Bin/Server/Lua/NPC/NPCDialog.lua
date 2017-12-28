-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���к�
-- ��  ��:	2017/1/18
-- ��  ��:	1.0
-- ��  ��:	NPC�Ի���� 
-------------------------------------------------------------------

require("NPC")

-------------------------------------------------------------------
-- ���NPCĬ�϶԰� 
-- @param npc    : NPC����
-- @param npc_id : NPC�߼�ID 
-- @param actor  : �������
-- @param topic_list: �����б� 
-------------------------------------------------------------------
function DefaultNPCDialogHandler(npc,npc_id,actor,topic_list)
	table.insert(topic_list,{"��̸","DefaultNPCDialog",{npc_id,npc,actor},0,'#FFFFFF',nil,3,false})
end

-------------------------------------------------------------------
-- ���NPCĬ�϶԰�
-- @param npc_id : NPC�߼�ID 
-- @param actor  : �������
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
-- ���������⹩���ѡ��
-- @param actor: ���ID
-- @param topic_list: �����б�
-- @param npc: �Ի���npc
----------------------------------------------------------------------------------------
function MultiTopicDialog(actor, topic_list, npc)
	local entry_list = {{}, {}, {}}
	
	-- ���ݹ��ܷ�����з���
	for index, params in pairs(topic_list) do
		if params[7] == nil then
			params[7] = 1
		end
		
		table.insert(entry_list[params[7]], params)
	end
	
	
	local dialog_content = nil
	local npc_name = nil
	local npc_view_type = nil
	-- ���Ĭ�϶԰�
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

	-- ���������
	for i = #entry_list, 1, -1 do
		local entrys = entry_list[i]
		if #entrys > 0 then
			for index, params in pairs(entrys) do
				-- �û����ǹ���
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
-- �Ѳ����б�ת��Ϊ�ֺŷָ����ַ���
-- @param params: �����б�
----------------------------------------------------------------------------------------
function BuildParamsString(params)
	local str = ""
	for index, param in pairs(params) do
		
		-- �������÷ֺŷָ�
		if index ~= 1 then
			str = str..";"
		end
	
		-- ��һ������Ϊ�������������Ϊ�ú����Ĳ���
		-- ���в�������Ϊ�ַ�������"**"Ϊǰ׺��Ϊ������û��ǰ׺
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