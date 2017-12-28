-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	陈涛 (Carl Chen)
-- 日  期:	08/07/2008
-- 版  本:	1.0
-- 描  述:  lua中的事件发布订阅机制，基本原理参考设计模式-观察者
-------------------------------------------------------------------

require "Class"

-------------------------------------------------------------------
-- 事件发布者
-- 提供事件的发布和取消方法
-------------------------------------------------------------------
Publisher = Object:new
{
	global_event_id = 0 ,
    subscriber_list = {},
}

-------------------------------------------------------------------
-- 订阅事件
-- @param event_id : 事件ID
-- @param subscriber:订阅者，传处理函数对象即可
-------------------------------------------------------------------
function Publisher:Subscribe( event_id,subscriber,param )
	if self.subscriber_list[event_id]==nil then
       self.subscriber_list[event_id] = {}
	end
	
	if  param ~= nil then
        self.subscriber_list[event_id][subscriber] = param
	else
        self.subscriber_list[event_id][subscriber] = 1
	end

end

-------------------------------------------------------------------
-- 退订事件
-- @param event_id : 事件ID
-- @param subscriber:订阅者，传处理函数对象即可
-------------------------------------------------------------------
function Publisher:Unsubscribe( event_id,subscriber )
	if self.subscriber_list[event_id]==nil then
       return
	end

	self.subscriber_list[event_id][subscriber] = nil
end

-------------------------------------------------------------------
-- 触发事件
-- param1~4参数可填可不填
-------------------------------------------------------------------
function Publisher:FireEvent( event_id,... )
	if self.subscriber_list[event_id]==nil then
       return
	end
	
	arg = { ... }

	local param_list = {0}
	
	local i=0
	for i,value in pairs(arg) do
		table.insert(param_list,arg[i])
	end
	
	table.insert(param_list,1)
	i = table_count(param_list)
		
	for subscriber,value in pairs(self.subscriber_list[event_id]) do
	    param_list[1] = subscriber
	    param_list[i] = value
        SafeCall( table.unpack(param_list) )
	end
end

-------------------------------------------------------------------
-- 具体事件定义
-------------------------------------------------------------------
-- 全局事件中心
g_luaEventPublisher = new(Publisher,{})

LUA_EVENT_ACTOR_LOGIN 			= 1  -- 玩家登陆
LUA_EVENT_ACTOR_LOGOUT			= 2  -- 玩家登出
LUA_EVENT_SCENE_UNLOAD			= 3  -- 场景销毁