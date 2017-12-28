-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李建伟
-- 日  期:	11/1/2017
-- 版  本:	1.0
-- 描  述:	AI模板工厂. AI脚本中现在无状态数据所以不需要每个实体一个AI模板的实例
-------------------------------------------------------------------
require "Class"

-------------------------------------------------------------------
-- AI模板工厂
-- note : 作用 : 传入一个AIID, 返回AI模板对象
-------------------------------------------------------------------
AITemplateFactory = Object:new
{
	tAITemplateList = {},
}

-------------------------------------------------------------------
-- 获得AI模板
-- @param scheme_id	  : 模板ID
-------------------------------------------------------------------
function AITemplateFactory:GetAITemplate(scheme_id)

	if scheme_id == nil then
		warning("GetAITemplate Failed!scheme_id is nil")
		return nil
	end
	
	local template = self.tAITemplateList[scheme_id]
	if template ~= nil then
		return template
	end
	
	-- 装载脚本
	require("AITemplate_" .. scheme_id)
	
	template = _G["AITemplate_" .. scheme_id]
	self.tAITemplateList[scheme_id] = template
	
	if template ~= nil then 
		return template
	end 

	warning("GetAITemplate error scheme_id="..scheme_id)
	return nil
end


