-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���к�
-- ��  ��:	2017/1/18
-- ��  ��:	1.0
-- ��  ��:	NPC������
-------------------------------------------------------------------

require("NPC")
require("NPCDialog")
require("NPCFuncRegister")

-- NPC������ID
g_NPCTriggerID = nil

-- ��ʼ��NPC������
function InitNPCHandler()
	
	local scene_id = GetLuaSceneID()
    g_NPCTriggerID = CreateNPCTrigger(scene_id, "NPCTriggerHandler")
    if g_NPCTriggerID == 0 then
		error("InitNPCHandler CreateNPCTrigger fail")
		return false
    end
	
	-- ���ĳ���ж���¼�
	g_luaEventPublisher:Subscribe(LUA_EVENT_SCENE_UNLOAD, ReleaseNPCHandler)
	
	
	trace("InitNPCHandler success")
	
	return true
end

-- ����ж�ش���
function ReleaseNPCHandler()
	if DeleteTrigger(g_NPCTriggerID) == false then
		error("Delete NPCTrigger failed, nTriggerID="..g_NPCTriggerID)
	end
	
end

-------------------------------------------------------------------
-- ����NPC����¼�
-- @param npc    	: NPC����
-- @param npc_id    : NPC�߼�ID
-- @param actor  	: �������
-------------------------------------------------------------------
function NPCTriggerHandler(npc, npc_id, actor)
	NPCTriggerHandler_Kernel(npc, npc_id, actor)
end


-------------------------------------------------------------------
-- ����NPC����¼�(��ײ㺯��)
-- @param npc    	: NPC����
-- @param npc_id    : NPC�߼�ID
-- @param actor  	: �������
-------------------------------------------------------------------
function NPCTriggerHandler_Kernel(npc, npc_id, actor)
	
	-- У��NPCλ��
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE)==false then
		return
	end
	
	local NPC_info = NPC_TABLE[npc_id]
	if NPC_info==nil then
	     return
	end
	
	-- �������ܴ���
	local hide_function_entry = nil
	
	-- �����б�,��ͬһ��npc����ѡ��˵�������
	-- ��ʽ: { {"�Ի���Ŀ",��Ӧ����,�����б�,�������ȼ�,�Ի���ɫ,ͼ��,���ܷ���(1�����ܣ�2������3��Ĭ�϶԰�)} } 
	local topic_list = {}
	
	-- ���NPC��Ӧ�Ĺ����б�
	if hide_function_entry ~= true then
		local func_table = {}
		if NPC_info["function"] ~= nil then
			if type(NPC_info["function"]) == 'table' then
				func_table = NPC_info["function"]
			else
				func_table[1] = NPC_info["function"]
			end
		end
	
		-- һ��NPC�����ж������
		for index, func_name in pairs(func_table) do
			if g_NPCFunctionHandler[func_name] ~= nil then
				local func_info = g_NPCFunctionHandler[func_name]
				local build_entry = func_info[5]
				local handler_func = func_info[2]
				if build_entry == true then
					_G[handler_func](actor, npc, npc_id, topic_list)
				else
					table.insert(topic_list, {func_info[1], handler_func, {actor, npc, npc_id}, func_info[3], func_info[4]})
				end
			end
		end
	end
	
	-- ����Ի�����ȡ�����б������
	local topic_num = #topic_list

	-- ��������ֱ�ӵ���
	if topic_num == 1 and NPC_info["show_default_dialog"] == nil then
		for index, params in pairs(topic_list) do
			_G[params[2]](table.unpack(params[3]))
			return
		end
	end
	
	-- ���Ĭ�϶԰�
	if NPC_info["dialog"] ~= nil then
		if type(NPC_info["dialog_handler"]) == 'function' then
			NPC_info["dialog_handler"](npc, npc_id, actor, topic_list)
			topic_num = topic_num + 1
		else
			local NPC_function = _G[NPC_info["dialog_handler"]]
			NPC_function(npc, npc_id, actor, topic_list)
			topic_num = topic_num + 1
		end
	end
	
	-- �޻���˵
	if topic_num == 0 then
		return
	end
	
	-- ��������
	if topic_num == 1 and NPC_info["show_default_dialog"] == nil then
		for index, params in pairs(topic_list) do
			error("param2="..params[2]..",param3="..params[3])
			_G[params[2]](table.unpack(params[3]))
			return
		end
	end
	
	-- �������
	MultiTopicDialog(actor, topic_list, npc)
end

-------------------------------------------------------------------
-- �������NPC����
-- @param actor : ���npc����
-- @param npc   : npc����
--------------------------------------------------------------------
function BuildBuyWings(actor, npc, npc_id, topic_list)

	-- У��NPCλ��
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE)==false then
		return
	end
	
	table.insert(topic_list,{ "������", "RequestBuyWings", {npc, actor, npc_id}, 0, '#FFFFFF', nil, 1})
end


-------------------------------------------------------------------
-- ����װ��NPC����
-- @param actor : ���npc����
-- @param npc   : npc����
--------------------------------------------------------------------
function BuildBuyShopGoods(actor, npc, npc_id, topic_list)

	-- У��NPCλ��
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE)==false then
		return
	end
	
	local npc_table = GOODSSHOP_CONFIG[npc_id]
	if npc_table == nil then
		error("BuildBuyShopGoods:not find npc_table="..npc_id)
		return
	end
	
	local goods_table = GOODSSHOP_CONFIG[npc_id]['goods_table']
	if goods_table == nil then
		error("not find goods_table="..npc_id)
		return
	end
	
	if g_NPCGoodsManager[npc] == nil then
		g_NPCGoodsManager[npc] = {}
		g_NPCGoodsManager[npc]['total_can_buy_count'] = 0
		g_NPCGoodsManager[npc]['total_already_buy_count'] = 0
		g_NPCGoodsManager[npc]['filter_goods_table'] = {}

		local randoms = {}
		math.randomseed(tostring(os.time()):reverse():sub(1, 7))
		for i = 1, npc_table['random_count'], 1 do
			local rd = math.random(npc_table['min_random'], npc_table['max_random'])
			randoms[i] = rd;
			trace('npc random = '.. rd)
		end
		

		
		for key, value in pairs(goods_table) do
			for k, v in ipairs(randoms) do
				if ((v >= value.min_value) and (v <= value.max_value)) then
					g_NPCGoodsManager[npc]['filter_goods_table'][value.id] = value;
					break;
				end
			end
		end
		
		for k, v in pairs(g_NPCGoodsManager[npc]['filter_goods_table']) do
			g_NPCGoodsManager[npc]['total_can_buy_count'] = g_NPCGoodsManager[npc]['total_can_buy_count'] + v['can_buy_count']
		end
	end

	if (g_NPCGoodsManager[npc]['total_already_buy_count'] >= g_NPCGoodsManager[npc]['total_can_buy_count'] and npc_table['after_buy_disappear']) then
		error("Out of purchase.npc_id="..npc_id..",npc="..npc..","..g_NPCGoodsManager[npc]['total_can_buy_count']..","..g_NPCGoodsManager[npc]['total_already_buy_count'])
		return
	end
	
	local filter_goods_table = g_NPCGoodsManager[npc]['filter_goods_table']
	for key, value in pairs(filter_goods_table) do
		local ticks = 0
		if g_NPCGoodsManager[npc][value.id] and g_NPCGoodsManager[npc][value.id]['ticks'] then
			ticks = g_NPCGoodsManager[npc][value.id]['ticks']
		end
		local cool_time = value['cool_time']
		if (g_NPCGoodsManager[npc][value.id] and g_NPCGoodsManager[npc][value.id]['already_buy_count'] and g_NPCGoodsManager[npc][value.id]['already_buy_count'] >= value['can_buy_count']) then
			cool_time = value['refresh_time']
		end
		table.insert(topic_list,{ value['name'], "RequestBuyShopGoods", {npc, actor, npc_id, value['id'], value['goods_cost'], ticks, cool_time}, 0, '#FFFFFF', nil, 1})
	end
end


------------------------------------------------------------------
-- ��������NPC����
-- @param actor : ���npc����
-- @param npc   : npc����
--------------------------------------------------------------------
function BuildWarCureMember(actor, npc, npc_id, topic_list)

	-- У��NPCλ��
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE)==false then
		return
	end
	
	table.insert(topic_list,{ "�������", "RequestWarCureMember", {npc, actor, npc_id}, 0, '#FFFFFF', nil, 1})
end