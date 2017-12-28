-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李有红
-- 日  期:	2017/1/18
-- 版  本:	1.0
-- 描  述:	NPC初始化
-------------------------------------------------------------------

-- NPC功能ID定义 
require("NPCHandler")
require("NPCFuncHandler")


if (InitNPCHandler() == false) then
	error("Init NPCHandler failed.")
end

if (InitNPCFuncHandler() == false) then
	error("Init NPCFuncHandler failed.")
end