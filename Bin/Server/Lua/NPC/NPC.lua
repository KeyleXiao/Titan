-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李有红
-- 日  期:	2017/1/18
-- 版  本:	1.0
-- 描  述:	定义NPC属性
-------------------------------------------------------------------

-- NPC功能ID定义 
require("NPCFunctionDef")

--------------------------------------------------------------------------------
-- NPC配置方法 (游戏中的NPC需要在NPC_TABLE中进行定义,以下是属性名及说明)
-- 1.name          : npc名字
-- 2.id            : npc id
-- 4.location      : 创建位置 { 地图,x坐标,y坐标,不要多国复制(可选) }
-- 5.mirror(可选)  : 镜像,有些NPC需要在多个位置创建 { {地图1,x坐标1,y坐标1},{地图2,x坐标2,y坐标2} }
-- 6.dialog        :  默认对白,在这个对话表里可以填写多句对话(以逗号分隔),系统会随机选一句话说
-- 7.dialog_handler: 对白处理函数,默认为DefaultNPCDialogHandler
-- 8.function(可选): NPC功能列表,上面有功能列表定义,如果只承载一项功能,可以直接填,如果有多项功能则填成一个表
--------------------------------------------------------------------------------

NPC_TABLE = {

	[10001] = { 
				name = ("战场商店"),										 -- 名称
				id = 10001,                      			 				 -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				--["mirror"] = {{0;0;0},{0;0;0}}
				["dialog"] = { ("仙丹灵药,日用百货,无奇不有,包你满意")},		 -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_WAR_SHOP,                             	 -- 该NPC的功能
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                         -- NPC 窗口类型，客户端显示什么样的界面
			  },
	[10002] = { 
				name = ("飞行使者"),											 -- 名称
				id = 10002,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("想飞行吗？每次只要100金币哦！")},			 -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_WINGS,									 -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_DEFAULT                          -- NPC 窗口类型，客户端显示什么样的界面
			  },

	[2178] = { 
				name = ("超时空商店"),											 -- 名称
				id = 2178,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			  },
	[2179] = { 
				name = ("超时空商店"),											 -- 名称
				id = 2179,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2180] = { 
				name = ("复活祭坛"),											 -- 名称
				id = 2180,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("复活队友！")},			 -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_CURE_MEMBER,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_DEFAULT                         -- NPC 窗口类型，客户端显示什么样的界面
			  },
	[2181] = { 
				name = ("超时空商店"),											 -- 名称
				id = 2181,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2182] = { 
				name = ("超时空商店"),											 -- 名称
				id = 2182,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2183] = { 
				name = ("超时空商店"),											 -- 名称
				id = 2183,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2184] = { 
				name = ("超时空商店"),											 -- 名称
				id = 2184,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
			 
	[2185] = { 
				name = ("超时空商店"),											 -- 名称
				id = 2185,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },

	[2186] = { 
				name = ("装备"),											 -- 名称
				id = 2186,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2187] = { 
				name = ("装备"),											 -- 名称
				id = 2187,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2188] = { 
				name = ("装备"),											 -- 名称
				id = 2188,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2189] = { 
				name = ("装备"),											 -- 名称
				id = 2189,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2190] = { 
				name = ("装备"),											 -- 名称
				id = 2190,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2191] = { 
				name = ("装备"),											 -- 名称
				id = 2187,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2192] = { 
				name = ("装备"),											 -- 名称
				id = 2192,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2193] = { 
				name = ("装备"),											 -- 名称
				id = 2193,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2194] = { 
				name = ("装备"),											 -- 名称
				id = 2194,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2195] = { 
				name = ("装备"),											 -- 名称
				id = 2195,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2205] = { 
				name = ("装备"),											 -- 名称
				id = 2205,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2206] = { 
				name = ("装备"),											 -- 名称
				id = 2206,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2207] = { 
				name = ("装备"),											 -- 名称
				id = 2207,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2208] = { 
				name = ("装备"),											 -- 名称
				id = 2208,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
	[2209] = { 
				name = ("装备"),											 -- 名称
				id = 2209,							                         -- id
				["location"] = {0,0,0},                             	 	 -- 默认位置
				["rotation"] = {0,0,0},                             	 	 -- 旋转坐标
				["dialog"] = { ("快来我这购买装备吧！")},			         -- 默认对白
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- 对话处理函数
				["function"] = NPC_GOODS_SHOP,							     -- 该NPC的功能
				show_default_dialog = true,									 -- 显示默认对话
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC 窗口类型，客户端显示什么样的界面
			 },
--------------------------------------------------------------------------------

}    ---------此括号对应起始括号