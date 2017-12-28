-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李建伟
-- 日  期:	11/1/2017
-- 版  本:	1.0
-- 描  述:	AImain
-------------------------------------------------------------------
require "HelpFunc"
require "Class"
require "AITemplateFactory"

-- 初始化AI
function AI_Init(pAITaskMgr, nTemplateID)
	
	local template = AITemplateFactory:GetAITemplate(nTemplateID)
	if template == nil then 
		return
	end
	
	template:Init(pAITaskMgr)
end

-- AI事件处理
function AI_OnEvent(pAITaskMgr, nTemplateID, pAIContext, loc)
	if pAITaskMgr == nil or pAIContext == nil or loc == nil then 
		error("AI_OnEvent param error")
		return
	end
	
	local template = AITemplateFactory:GetAITemplate(nTemplateID)
	if template == nil then 
		return
	end
	
	template:OnEvent(pAITaskMgr, pAIContext, loc)
end