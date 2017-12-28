-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���� (Carl Chen)
-- ��  ��:	08/07/2008
-- ��  ��:	1.0
-- ��  ��:  lua�е��¼��������Ļ��ƣ�����ԭ��ο����ģʽ-�۲���
-------------------------------------------------------------------

require "Class"

-------------------------------------------------------------------
-- �¼�������
-- �ṩ�¼��ķ�����ȡ������
-------------------------------------------------------------------
Publisher = Object:new
{
	global_event_id = 0 ,
    subscriber_list = {},
}

-------------------------------------------------------------------
-- �����¼�
-- @param event_id : �¼�ID
-- @param subscriber:�����ߣ������������󼴿�
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
-- �˶��¼�
-- @param event_id : �¼�ID
-- @param subscriber:�����ߣ������������󼴿�
-------------------------------------------------------------------
function Publisher:Unsubscribe( event_id,subscriber )
	if self.subscriber_list[event_id]==nil then
       return
	end

	self.subscriber_list[event_id][subscriber] = nil
end

-------------------------------------------------------------------
-- �����¼�
-- param1~4��������ɲ���
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
-- �����¼�����
-------------------------------------------------------------------
-- ȫ���¼�����
g_luaEventPublisher = new(Publisher,{})

LUA_EVENT_ACTOR_LOGIN 			= 1  -- ��ҵ�½
LUA_EVENT_ACTOR_LOGOUT			= 2  -- ��ҵǳ�
LUA_EVENT_SCENE_UNLOAD			= 3  -- ��������