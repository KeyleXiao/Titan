-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李有红
-- 日  期:	2017/1/18
-- 版  本:	1.0
-- 描  述:	NPC处理器
-------------------------------------------------------------------

require("NPC")
require("NPCDialog")
require("NPCFuncRegister")

-- NPC触发器ID
g_NPCTriggerID = nil

-- 初始化NPC处理器
function InitNPCHandler()
	
	local scene_id = GetLuaSceneID()
    g_NPCTriggerID = CreateNPCTrigger(scene_id, "NPCTriggerHandler")
    if g_NPCTriggerID == 0 then
		error("InitNPCHandler CreateNPCTrigger fail")
		return false
    end
	
	-- 订阅场景卸载事件
	g_luaEventPublisher:Subscribe(LUA_EVENT_SCENE_UNLOAD, ReleaseNPCHandler)
	
	
	trace("InitNPCHandler success")
	
	return true
end

-- 场景卸载处理
function ReleaseNPCHandler()
	if DeleteTrigger(g_NPCTriggerID) == false then
		error("Delete NPCTrigger failed, nTriggerID="..g_NPCTriggerID)
	end
	
end

-------------------------------------------------------------------
-- 处理NPC点击事件
-- @param npc    	: NPC对象
-- @param npc_id    : NPC逻辑ID
-- @param actor  	: 点击的人
-------------------------------------------------------------------
function NPCTriggerHandler(npc, npc_id, actor)
	NPCTriggerHandler_Kernel(npc, npc_id, actor)
end


-------------------------------------------------------------------
-- 处理NPC点击事件(最底层函数)
-- @param npc    	: NPC对象
-- @param npc_id    : NPC逻辑ID
-- @param actor  	: 点击的人
-------------------------------------------------------------------
function NPCTriggerHandler_Kernel(npc, npc_id, actor)
	
	-- 校验NPC位置
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE)==false then
		return
	end
	
	local NPC_info = NPC_TABLE[npc_id]
	if NPC_info==nil then
	     return
	end
	
	-- 弹出功能创建
	local hide_function_entry = nil
	
	-- 话题列表,跟同一个npc可以选择说多个话题
	-- 格式: { {"对话条目",响应函数,参数列表,排序优先级,对话颜色,图标,功能分类(1：功能，2：任务，3：默认对白)} } 
	local topic_list = {}
	
	-- 输出NPC对应的功能列表
	if hide_function_entry ~= true then
		local func_table = {}
		if NPC_info["function"] ~= nil then
			if type(NPC_info["function"]) == 'table' then
				func_table = NPC_info["function"]
			else
				func_table[1] = NPC_info["function"]
			end
		end
	
		-- 一个NPC可以有多个功能
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
	
	-- 输出对话，获取主题列表的条数
	local topic_num = #topic_list

	-- 单个话题直接弹出
	if topic_num == 1 and NPC_info["show_default_dialog"] == nil then
		for index, params in pairs(topic_list) do
			_G[params[2]](table.unpack(params[3]))
			return
		end
	end
	
	-- 输出默认对白
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
	
	-- 无话可说
	if topic_num == 0 then
		return
	end
	
	-- 单个话题
	if topic_num == 1 and NPC_info["show_default_dialog"] == nil then
		for index, params in pairs(topic_list) do
			error("param2="..params[2]..",param3="..params[3])
			_G[params[2]](table.unpack(params[3]))
			return
		end
	end
	
	-- 多个话题
	MultiTopicDialog(actor, topic_list, npc)
end

-------------------------------------------------------------------
-- 构建翅膀NPC界面
-- @param actor : 点击npc的人
-- @param npc   : npc对象
--------------------------------------------------------------------
function BuildBuyWings(actor, npc, npc_id, topic_list)

	-- 校验NPC位置
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE)==false then
		return
	end
	
	table.insert(topic_list,{ "购买翅膀", "RequestBuyWings", {npc, actor, npc_id}, 0, '#FFFFFF', nil, 1})
end


-------------------------------------------------------------------
-- 构建装备NPC界面
-- @param actor : 点击npc的人
-- @param npc   : npc对象
--------------------------------------------------------------------
function BuildBuyShopGoods(actor, npc, npc_id, topic_list)

	-- 校验NPC位置
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
-- 构建复活NPC界面
-- @param actor : 点击npc的人
-- @param npc   : npc对象
--------------------------------------------------------------------
function BuildWarCureMember(actor, npc, npc_id, topic_list)

	-- 校验NPC位置
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE)==false then
		return
	end
	
	table.insert(topic_list,{ "复活队友", "RequestWarCureMember", {npc, actor, npc_id}, 0, '#FFFFFF', nil, 1})
end