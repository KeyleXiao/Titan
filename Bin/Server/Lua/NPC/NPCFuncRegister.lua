-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李有红
-- 日  期:	2017/2/26
-- 版  本:	1.0
-- 描  述:  函数注册列表
-------------------------------------------------------------------

require("NPCConfig")

-- 请求函数注册列表
g_RequestFuncList=
{
	["RequestBuyWings"] = 1,
	["RequestWarCureMember"] = 1,
	["RequestBuyShopGoods"] = 1,
}

g_NPCGoodsManager =
{

}

-------------------------------------------------------------------
-- 打开商店界面
-- @warning     : 在这里还需要进行必要的校验，防止客户端作弊
-- @param actor : 点击npc的人
-- @param npc   : npc对象
-------------------------------------------------------------------
function RequestOpenWarShop(actor,npc)
	-- 校验NPC位置
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE)==false then
		return
	end
	
	OpenWarShop(actor, npc)
end

-------------------------------------------------------------------
-- 请求购买翅膀
-- @param actor : 点击npc的人
-- @param npc   : npc对象
--------------------------------------------------------------------
function RequestBuyWings(npc, actor, npc_id)
	local NPC_info = NPC_TABLE[npc_id]
	if NPC_info == nil then
		return
	end
	
	-- 校验NPC位置
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE) == false then
		return
	end
	
	-- 购买翅膀，
	local cost = WINGS_CONFIG.cost
	local totalTP = GetTelentPoint(actor)
	
	-- 天赋点不够
	if totalTP < cost then
		local strRes = "<npc uid=\""..npc.."\" npc_view_type=\""..NPC_info.npc_view_type.."\" name=\""..NPC_info.name.."\" dialog=\""..WINGS_CONFIG.no_enough_gold.."\"></npc>" 
		ResponseNPC(actor, npc, strRes)
		return
	end
	
	local buff_id = WINGS_CONFIG.buff_id  -- 翅膀buff ID
	if AddBuff(actor, buff_id, 1, actor, 0, nil, 0) == true then
		-- 减去天赋点
		SetTelentPoint(actor, totalTP - cost)
		-- 关闭客户端NPC对话框
		CloseNPCDialogBox(actor)
		-- 设置标识
		SetActorWarUseWringFlag(actor)
	else
		warning("Add buff failed.actor="..actor..",buff_id="..buff_id)
	end
end



-------------------------------------------------------------------
-- 请求购买装备
-- @param actor : 点击npc的人
-- @param npc   : npc对象
--------------------------------------------------------------------
function RequestBuyShopGoods(npc, actor, npc_id, goods_id, goods_cost, buytime, cool_time)
	-- 校验NPC位置
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE) == false then
		return
	end

	local npc_table = GOODSSHOP_CONFIG[npc_id]
	if npc_table == nil then
		error("RequestBuyShopGoods:not find npc_table="..npc_id)
		return
	end
	
	if (g_NPCGoodsManager[npc] == nil) then
		error("not find g_NPCGoodsManager,npc="..npc.."npc_id="..npc_id)
		return
	end
	
	local goods_table = g_NPCGoodsManager[npc]['filter_goods_table']
	if (goods_table == nil) then
		error("not find goods_table="..npc_id)
		return
	end
	
	if g_NPCGoodsManager[npc][goods_id] == nil then
		g_NPCGoodsManager[npc][goods_id] = {}
		g_NPCGoodsManager[npc][goods_id]['already_buy_count'] = 0
	end
	
	local goods;
	for key, value in pairs(goods_table) do
		if value['id'] == goods_id then
			goods = value;
			break;
		end
	end
	
	if (goods == nil) then
		error("not find goods_id="..goods_id)
		return
	end
	
	local is_success = false
	local now = GetSyncTick()
	local last = g_NPCGoodsManager[npc][goods_id]['ticks']

	if last == nil then
		is_success = luaBuyGoods(actor, goods_id, goods['goods_cost'])
	else
		local cool_time = 0
		if (g_NPCGoodsManager[npc][goods_id]['already_buy_count'] >= goods_table[goods_id]['can_buy_count']) then
			if (npc_table['after_buy_disappear']) then
				error("can not buy more goods.="..goods_id..",npcID="..npc_id..",npc="..npc)
				return
			else
				cool_time = goods['refresh_time']
				g_NPCGoodsManager[npc][goods_id]['already_buy_count'] = 0
			end
		else
			cool_time = goods['cool_time']
		end
				
		local ticks = now - last;
		if ticks >= cool_time * 1000 then
			is_success = luaBuyGoods(actor, goods_id, goods['goods_cost'])
		else
			ShowSystemMessage(actor, 4035, math.ceil(goods['cool_time']- ticks / 1000))
			return
		end
	end

	if is_success then
		g_NPCGoodsManager[npc][goods_id]['ticks'] = now
		g_NPCGoodsManager[npc][goods_id]['already_buy_count'] = g_NPCGoodsManager[npc][goods_id]['already_buy_count'] + 1
		g_NPCGoodsManager[npc]['total_already_buy_count'] = g_NPCGoodsManager[npc]['total_already_buy_count'] + 1
		if (g_NPCGoodsManager[npc]['total_already_buy_count'] >= g_NPCGoodsManager[npc]['total_can_buy_count'] and npc_table['after_buy_disappear']) then
			local scene_id = GetLuaSceneID()
			LuaDestroyNPC(scene_id, npc)
			g_NPCGoodsManager[npc] = nil
			CloseNPCDialogBox(actor);
		else
			NPCGoodsBuySuccess(actor, npc, npc_id, goods_id, now)
		end
	end
end





-------------------------------------------------------------------
-- 请求复活队友
-- @param actor : 点击npc的人
-- @param npc   : npc对象
--------------------------------------------------------------------
function RequestWarCureMember(npc, actor, npc_id)
	local NPC_info = NPC_TABLE[npc_id]
	if NPC_info == nil then
		return
	end
	
	-- 校验NPC位置
	if VerifyLocation(actor, npc, NPC_TRIGGER_DISTANCE) == false then
		return
	end
	
	cureMember(actor);
end