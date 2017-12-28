-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李有红
-- 日  期:	2017/1/18
-- 版  本:	1.0
-- 描  述:	NPC其他信息配置表
-------------------------------------------------------------------

-- 触发NPC的有效距离
NPC_TRIGGER_DISTANCE = 10

-- 翅膀NPC配置
WINGS_CONFIG = {
	["cost"] = 100, 			-- 消耗天赋点数
	["buff_id"] = 2377, 		-- 翅膀buff ID
	["no_enough_gold"] = ("你的金币不足100不能飞行"), 
}

	-- id:装备id；cool_time:下次多久以后才可以购买；refresh_time:多久补充一次数量，为0即没有冷却时间不会补充数量;
	-- can_buy_count:当可购买的数量消耗完毕以后，装备图标变成灰色
	-- after_buy_disappear：购买完成后NPC消失
	-- name:物品名称
	-- goods_cost：物品价格
	-- min_value：最小随机值
	-- max_value：最大随机值
GOODSSHOP_CONFIG = {
	[2178] = {
		min_random = 1, max_random = 10000, random_count = 6, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 277, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $灵能飞刃", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 286, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $强袭大剑", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 291, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $伊贺飓风", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 285, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $暴走左轮", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 257, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $死神的凝视", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 258, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $雷霆法典", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 

	[2179] = {
		min_random = 1, max_random = 10000, random_count = 9, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 286, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $强袭大剑", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 285, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $暴走左轮", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 258, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $雷霆法典", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 259, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $圣光裁决", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 278, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $风行利刃", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 292, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $血饮狂刀", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
			
	[2181] = {
		min_random = 1, max_random = 10000, random_count = 18, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 280, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $行刑黑棺", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 279, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $暴君", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 261, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $至尊魔戒", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 277, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $灵能飞刃", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 291, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $伊贺飓风", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 257, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $死神的凝视", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
			
	[2182] = {
		min_random = 1, max_random = 10000, random_count = 7, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 254, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $支配者面具", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 266, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $诅咒之眼", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 289, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $御法之剑", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 286, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $强袭大剑", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 285, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $暴走左轮", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 258, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $雷霆法典", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
	
	[2183] = {
		min_random = 1, max_random = 10000, random_count = 7, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 277, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $灵能飞刃", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 291, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $伊贺飓风", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 257, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $死神的凝视", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 282, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $斩裂剑", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 288, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $雷切太刀", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 293, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $超合金巨钻", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
	
	[2184] = {
		min_random = 1, max_random = 10000, random_count = 6, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 280, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $行刑黑棺", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 279, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $暴君", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 261, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $至尊魔戒", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 259, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $圣光裁决", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 278, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $风行利刃", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 292, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $血饮狂刀", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	}, 
	
	[2185] = {
		min_random = 1, max_random = 10000, random_count = 6, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 254, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $支配者面具", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[2] = {id = 266, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $诅咒之眼", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[3] = {id = 289, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $御法之剑", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[4] = {id = 282, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $斩裂剑", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[5] = {id = 288, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $雷切太刀", goods_cost =-1, min_value =1, max_value = 10000,}, 
			[6] = {id = 293, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $超合金巨钻", goods_cost =-1, min_value =1, max_value = 10000,}, 
		}
	},
	
    [2186] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 245, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "风行利刃(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2187] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 246, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "羽化之靴（主动）", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2188] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 247, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "斩裂剑(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
    [2189] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 248, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "巨龙狩猎者(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2190] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		[1] = {id = 249, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "超合金巨钻(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
	}, 
	
	[2191] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 250, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "女神恩赐(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2192] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 251, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "巫师帽子(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2193] = {
		min_random = 1, max_random = 10000, random_count = 4, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 252, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "凛冬法杖(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2194] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 253, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "至尊魔戒(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	[2195] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = false, 
		goods_table = {
			[1] = {id = 254, cool_time = 300, refresh_time = 300, can_buy_count = 1, name = "支配者面具(主动)", goods_cost = 0, min_value = 1, max_value = 10000}
		}
	}, 
	
	--掉稀有大件
	[2205] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 245, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $风行利刃(主动)", goods_cost =0, min_value =1, max_value = 1000,}, 
			[2] = {id = 246, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $羽化之靴（主动）", goods_cost =0, min_value =1001, max_value = 2000,}, 
			[3] = {id = 247, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $斩裂剑(主动)", goods_cost =0, min_value =2001, max_value = 3000,}, 
			[4] = {id = 248, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $巨龙狩猎者(主动)", goods_cost =0, min_value =3001, max_value = 4000,}, 
			[5] = {id = 249, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $超合金巨钻(主动)", goods_cost =0, min_value =4001, max_value = 5000,}, 
			[6] = {id = 280, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $行刑黑棺", goods_cost =0, min_value =5001, max_value = 6000,}, 
			[7] = {id = 279, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $暴君", goods_cost =0, min_value =6001, max_value = 7000,}, 
			[8] = {id = 252, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $凛冬法杖(主动)", goods_cost =0, min_value =7001, max_value = 8000,}, 
			[9] = {id = 253, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $至尊魔戒(主动)", goods_cost =0, min_value =8001, max_value = 9000,}, 
			[10] = {id = 254, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $支配者面具", goods_cost =0, min_value =9001, max_value = 10000,}, 
		}
	}, 
	
	--掉普通物理大件
	[2206] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 277, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $灵能飞刃", goods_cost =0, min_value =1, max_value = 1000,}, 
			[2] = {id = 278, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $风行利刃", goods_cost =0, min_value =1001, max_value = 2000,}, 
			[3] = {id = 289, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $御法之剑", goods_cost =0, min_value =2001, max_value = 3000,}, 
			[4] = {id = 286, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $强袭大剑", goods_cost =0, min_value =3001, max_value = 4000,}, 
			[5] = {id = 282, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $斩裂剑", goods_cost =0, min_value =4001, max_value = 5000,}, 
			[6] = {id = 291, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $伊贺飓风", goods_cost =0, min_value =5001, max_value = 6000,}, 
			[7] = {id = 285, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $暴走左轮", goods_cost =0, min_value =6001, max_value = 7000,}, 
			[8] = {id = 288, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $雷切太刀", goods_cost =0, min_value =7001, max_value = 8000,}, 
			[9] = {id = 293, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $超合金巨钻", goods_cost =0, min_value =8001, max_value = 9000,}, 
			[10] = {id = 292, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $血饮狂刀", goods_cost =0, min_value =9001, max_value = 10000,}, 
		}
	}, 
	--掉中物理小件
	[2207] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 306, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $恶魔拳套", goods_cost =0, min_value =1, max_value = 1000,}, 
			[2] = {id = 308, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $速射短弓", goods_cost =0, min_value =1001, max_value = 2000,}, 
			[3] = {id = 304, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $追风", goods_cost =0, min_value =2001, max_value = 3000,}, 
			[4] = {id = 299, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $骑士长剑", goods_cost =0, min_value =3001, max_value = 4000,}, 
			[5] = {id = 309, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $锯齿匕首", goods_cost =0, min_value =4001, max_value = 5000,}, 
			[6] = {id = 310, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $铁锄", goods_cost =0, min_value =5001, max_value = 6000,}, 
			[7] = {id = 305, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $水晶剑", goods_cost =0, min_value =6001, max_value = 7000,}, 
			[8] = {id = 302, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $寒冰连枷", goods_cost =0, min_value =7001, max_value = 8000,}, 
			[9] = {id = 300, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $精钢锤", goods_cost =0, min_value =8001, max_value = 9000,}, 
			[10] = {id = 301, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $刺客飞刀", goods_cost =0, min_value =9001, max_value = 10000,}, 
		}
	}, 
	--掉中法术小件
	[2208] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 257, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $死神的凝视", goods_cost =0, min_value =1, max_value = 2000,}, 
			[2] = {id = 258, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $雷霆法典", goods_cost =0, min_value =2001, max_value = 4000,}, 
			[3] = {id = 259, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $圣光裁决", goods_cost =0, min_value =4001, max_value = 6000,}, 
			[4] = {id = 266, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $诅咒之眼", goods_cost =0, min_value =6001, max_value = 8000,}, 
			[5] = {id = 261, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $至尊魔戒", goods_cost =0, min_value =8001, max_value = 10000,}, 
		}
	},
	--掉中法术小件
	[2209] = {
		min_random = 1, max_random = 10000, random_count = 1, after_buy_disappear = true, 
		goods_table = {
			[1] = {id = 270, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $圣徒信仰", goods_cost =0, min_value =1, max_value = 2000,}, 
			[2] = {id = 274, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $亡灵之魄", goods_cost =0, min_value =2001, max_value = 4000,}, 
			[3] = {id = 269, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $紫水晶", goods_cost =0, min_value =4001, max_value = 6000,}, 
			[4] = {id = 275, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $魔典", goods_cost =0, min_value =6001, max_value = 8000,}, 
			[5] = {id = 268, cool_time = 300, refresh_time = 600, can_buy_count = 1, name =" $智者典籍", goods_cost =0, min_value =8001, max_value = 10000,}, 
		}
	},
}


