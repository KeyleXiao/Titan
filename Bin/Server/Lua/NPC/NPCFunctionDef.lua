-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李有红
-- 日  期:	2017/1/18
-- 版  本:	1.0
-- 描  述:	维护NPC点击时的事务处理
-------------------------------------------------------------------

------------------------------------------------------------------- 
-- NPC功能定义的方法:
-- 1.在这里定义一个NPC功能ID,注意不要和其他人的功能ID重复
-- 2.在后面的g_NPCFunctionHandler里定义这个功能对应的处理函数
-- 3.在NPC.LUA中给指定的NPC挂上这个功能
-- 4.当玩家点击该NPC时就会自动调用到g_NPCFunctionHandler里定义的函数 
------------------------------------------------------------------- 
NPC_WAR_SHOP		= 1				-- 战场商店NPC
NPC_WINGS			= 2				-- 翅膀NPC
NPC_GOODS_SHOP      = 3             -- 购买装备
NPC_CURE_MEMBER     = 4             -- 复活队友

NPC_VIEW_WAR_DEFAULT      = 1             -- 翅膀界面
NPC_VIEW_WAR_GOODS_SHOP   = 2             -- 野外NPC商店界面

------------------------------------------------------------------
-- NPC功能处理表
-- @param note : 根据NPC的功能ID调用指定的响应函数 
-- 格式: [功能ID] = { "对话条目","响应函数",排序优先级, "对话颜色", 是否自己生成对话条目}
------------------------------------------------------------------- 
g_NPCFunctionHandler = {}

g_NPCFunctionHandler[NPC_WAR_SHOP]		= {"购买物品", "RequestOpenWarShop", 10, "#FFDD00"}
g_NPCFunctionHandler[NPC_WINGS]			= {"购买翅膀", "BuildBuyWings", 10, "#FFDD00", true}
g_NPCFunctionHandler[NPC_GOODS_SHOP]	= {"购买装备", "BuildBuyShopGoods", 10, "#FFDD00", true}
g_NPCFunctionHandler[NPC_CURE_MEMBER]	= {"复活队友", "BuildWarCureMember", 10, "#FFDD00", true}