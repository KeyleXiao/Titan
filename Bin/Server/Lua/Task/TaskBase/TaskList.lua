-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	12/1/2016
-- ��  ��:	1.0
-- ��  ��:	�����б�
-------------------------------------------------------------------

require "TaskFactory"
require "TaskFramePlayDotaGame"
require "TaskFrameActorData"

-------------------------------------------------------------------
-- ��������б�
TASK_LIST = {}         -- �洢��ʽ [���ID][{����ID=�������,����ID=�������,...}]

-- ����б�
ACTOR_LIST = {}			-- �洢��ʽ [���ID]={�������}

-- ��½������
g_LoginTriggerID = 0

-------------------------------------------------------------------


-------------------------------------------------------------------
-- 2013-3-5�����Ż�����
-- �Ż����Ʊ�ʶ��true=�����Ż�,false=�ر��Ż�
g_UserTaskOptimizeOpen = false

-- ��¼��ҵ�ǰӵ������ID����ÿ�����������ݣ������Ż���ҵ�½�����PDBIDΪ��ֵ
g_tUserHaveTaskIDList = {}

-- ��������иı䣬������㲥�����������ݣ����PDBIDΪ��ֵ
g_tUserHaveTaskIDListChange ={}
-------------------------------------------------------------------


-------------------------------------------------------------------
-- �����б����
-------------------------------------------------------------------
function InitTaskList()

	local scene_id = GetLuaSceneID()
    g_LoginTriggerID = CreateLoginTrigger(scene_id, "ActorLoginHandler")
    if g_LoginTriggerID == 0 then
		error("InitTaskList CreateLoginTrigger fail")
		return false
    end
	
	g_WarPersonTaskTrigger = CreateWarPersonTaskTrigger(scene_id, "SetPlayDotaGameEvent")
	if g_WarPersonTaskTrigger == 0 then
		error("InitTaskList CreateWarPersonTaskTrigger fail")
		return false
	end
	
	g_ActorDataTaskTrigger = CreateActorTaskTrigger(scene_id, "SetActorDataGameEvent")
	if g_ActorDataTaskTrigger == 0 then
		error("InitTaskList CreateActorTaskTrigger fail")
		return false
	end

	return true
end

-------------------------------------------------------------------
-- �û���½ʱ������������
-- @param holder_id : PDBID
-- @param flag		: 1�������û�����
-------------------------------------------------------------------
function ActorLoginHandler(holder_id, flag, holder_name)

	ACTOR_LIST[holder_id] = holder_name

	if TASK_LIST[holder_id] == nil then
		TASK_LIST[holder_id] = {}
	else
	    return
	end

	-- �Ƿ�㲥
	local bBroadcast = false

	-- ���µ�½���¶�ȡ����
	if flag == 1 or not g_UserTaskOptimizeOpen or g_tUserHaveTaskIDList[holder_id] == nil then

		-- �Ż����Ʊ�ʶ��true=�����Ż�,false=�ر��Ż�
		if g_UserTaskOptimizeOpen then
			-- ��ʼ����
			g_tUserHaveTaskIDList[holder_id] = {}
		end

		for task_id,task_data in pairs(TaskFactory.tTaskList) do

			local task_key = GetTaskIDKey(holder_id, task_id)
			local task_state = GetTaskIntData(holder_id, task_key)

			if task_state ~= TASK_DATA_INVALID and task_state <= TaskState.FinishState then

				local status,result = pcall(TryLoadActorTask, holder_id, task_id)
				if not status then
					RemoveTaskData(holder_id, task_key)
					if result~=nil then
						error("ActorLoginHandler load fail holder_id="..holder_id..",task_id="..task_id..",result="..result)
					end
				else
					-- �Ż����Ʊ�ʶ��true=�����Ż�,false=�ر��Ż�
					if g_UserTaskOptimizeOpen then
						-- ���������IDΪ��ǰӵ������
						g_tUserHaveTaskIDList[holder_id][task_id] = true

						-- ���ù㲥ͬ����־
						bBroadcast = true
					end
				end
			end
		end

		-- ��½����Ҫ�㲥����ʱ���㲥������������
		if bBroadcast then

			local strMsg = nil
			if g_tUserHaveTaskIDList[holder_id] == nil then
				strMsg = nil
			else

				local tUserHaveTaskIDList = {}
				for task_id, nValue in g_tUserHaveTaskIDList[holder_id] do
					table.insert(tUserHaveTaskIDList, task_id)
				end
				strMsg = tableToString(tUserHaveTaskIDList)
			end
			-- ����û������ҲҪͬ������
			--ZoneServerBroadcast("TaskList_SetUserHaveTaskIDList", nil, holder_id, strMsg)
		end
	else
		for task_id, value in pairs(g_tUserHaveTaskIDList[holder_id]) do
			local task_data = TaskFactory.tTaskList[task_id]
			if task_data ~= nil then

				local task_key = GetTaskIDKey(holder_id, task_id)
				local task_state = GetTaskIntData(holder_id, task_key)
				if task_state ~= TASK_DATA_INVALID and task_state <= TaskState.FinishState then

					local status,result = pcall(TryLoadActorTask, holder_id, task_id)
					if not status then

						RemoveTaskData(holder_id, task_key)
						if result~=nil then
							error("ActorLoginHandler have load fail holder_id="..holder_id..",task_id="..task_id..",result="..result)
						end
					end
				end
			end
		end
	end

    -- �����¼�
    g_luaEventPublisher:FireEvent(LUA_EVENT_ACTOR_LOGIN, holder_id, flag)
end

-------------------------------------------------------------------
-- �û��˳�ʱ������������
-- @param holder_id : PDBID
-- @param flag		: 1�������û�����
-------------------------------------------------------------------
function ActorLogoutHandler(holder_id, flag)

    if TASK_LIST[holder_id] == nil then
  		return
    end

	 -- �����¼�
    g_luaEventPublisher:FireEvent(LUA_EVENT_ACTOR_LOGOUT, holder_id, flag)

	for task_id,task in pairs(TASK_LIST[holder_id]) do

		task:OnLogout(holder_id)

		-- ���������������Ҫɾ��
		if task.save==false then
			task:DeleteTask()
		end

		-- �����¼�
        OnActorTaskListChanged(holder_id, task_id, "logout")
		TaskFactory:DeleteTask(task)
	end

	-- �Ż����Ʊ�ʶ��true=�����Ż�,false=�ر��Ż�,ͬ�����е�ͼ������
	if g_UserTaskOptimizeOpen then
		-- ��Ҫ�㲥ʱ����������ʱ
		if g_tUserHaveTaskIDListChange[holder_id] == true  or flag == 1 then
			local strMsg = nil
			-- �����������ʱ��������г�����������񱸷�
			if g_tUserHaveTaskIDList[holder_id] == nil or flag == 1 then
				strMsg = nil
			else
				local tUserHaveTaskIDList = {}
				for task_id, nValue in pairs(g_tUserHaveTaskIDList[holder_id]) do
					table.insert(tUserHaveTaskIDList, task_id)
				end
				strMsg = tableToString(tUserHaveTaskIDList)
			end
			-- �ÿջ�������
			g_tUserHaveTaskIDList[holder_id] = nil
			-- ����û������ҲҪͬ������
			--ZoneServerBroadcast("TaskList_SetUserHaveTaskIDList", nil, holder_id, strMsg)
		end

		-- �ÿ����߹㲥��־
		g_tUserHaveTaskIDListChange[holder_id] = nil
	end

    TASK_LIST[holder_id]=nil

	ACTOR_LIST[holder_id] = nil

    -- ��������б�������ʹ��Щģ��©�����˶��¼��Ĵ���Ҳ�������
    TASK_LIST_EVENT_HANDLER[holder_id]=nil

	-- ��������������ע��
	TASK_DATA[holder_id] = nil
end

-------------------------------------------------------------------
-- ���������������
-- @param holder_id : PDBID
-- @param task_id	: ����ID
-------------------------------------------------------------------
function TryLoadActorTask(holder_id, task_id)

	local task = TaskFactory:CreateTask(holder_id, task_id)
    if task == nil then
		local task_key = GetTaskIDKey(holder_id, task_id)
    	RemoveTaskData(holder_id, task_key)
		warning("TryLoadActorTask error task_id="..task_id)
		return
	end

	-- �����½
	task:OnLogin()
	
	-- ����Ƿ����
	if task:CanFinish() == true then
		task:Finish()
	end

	if task:GetState() <= TaskState.FinishState then
		TASK_LIST[holder_id][task_id]=task
	else
		warning("TryLoadActorTask error finish task_id="..task_id)
	end

	-- �����¼�
	OnActorTaskListChanged(holder_id, task_id, "login")
end

-------------------------------------------------------------------
-- ������ҵ�ǰӵ������ID
-- @param holder_id : PDBID
-- @param strUserHaveTaskIDList	: ӵ������ID�б�
-------------------------------------------------------------------
function TaskList_SetUserHaveTaskIDList(holder_id, strUserHaveTaskIDList)

	-- ������
	if holder_id == nil or holder_id <= 0 then
		return
	end

	if strUserHaveTaskIDList == nil or strUserHaveTaskIDList == 0 then
		-- �ÿ�
		g_tUserHaveTaskIDList[holder_id] = nil
		return
	end

	-- ת��Ϊtable
	local tUserHaveTaskIDList = stringToTable(strUserHaveTaskIDList)
	if tUserHaveTaskIDList == nil then
		return
	end

	-- ���table��
	g_tUserHaveTaskIDList[holder_id] = {}

	for index,task_id in pairs(tUserHaveTaskIDList) do
		g_tUserHaveTaskIDList[holder_id][task_id] = true
	end
end

-------------------------------------------------------------------
-- ʵ�������б�仯�¼�
-- �������holder_id
-- ���������ID task_id
-- ִ�е��¼� handler
-- ���Ľ����¼�����ɻ�����¼� accept��Ϊtrue�����������¼�
-------------------------------------------------------------------
TASK_LIST_EVENT_HANDLER =
{
}


-------------------------------------------------------------------
-- ע�������б�仯�¼�
-------------------------------------------------------------------
function RegisterTaskListChangeEvent(holder_id, task_id, handler, action)

	if TASK_LIST_EVENT_HANDLER[holder_id]==nil then
		TASK_LIST_EVENT_HANDLER[holder_id] = {}
	end

	if TASK_LIST_EVENT_HANDLER[holder_id][task_id] == nil then
	    TASK_LIST_EVENT_HANDLER[holder_id][task_id] = {}
	end

	if TASK_LIST_EVENT_HANDLER[holder_id][task_id][action] == nil then
	    TASK_LIST_EVENT_HANDLER[holder_id][task_id][action] = {}
	end

	TASK_LIST_EVENT_HANDLER[holder_id][task_id][action][handler] = handler
end

-------------------------------------------------------------------
-- ȡ�������б�仯ĳ��������ж���
-- �������holder_id
-------------------------------------------------------------------
function UnregisterTaskAllEvent(holder_id)

	if TASK_LIST_EVENT_HANDLER[holder_id] ~= nil then
		TASK_LIST_EVENT_HANDLER[holder_id] = nil
	end
end

-------------------------------------------------------------------
-- ȡ�������б�仯�¼�����
-- �������holder_id
-- ���������ID task_id
-- ȡ�����Ľ����¼�����ɻ�����¼� accept��Ϊtrue�����������¼�
-------------------------------------------------------------------
function UnregisterTaskListChangeEvent(holder_id, task_id, action, handler)

	if TASK_LIST_EVENT_HANDLER[holder_id] ~= nil then
		if TASK_LIST_EVENT_HANDLER[holder_id][task_id] ~= nil then
			if TASK_LIST_EVENT_HANDLER[holder_id][task_id][action] ~= nil then
				TASK_LIST_EVENT_HANDLER[holder_id][task_id][action][handler] = nil
			end
		end
	end
end

-- �¼����ķ���·��
TASK_LIST_EVENT_TRAVEL = {}

-------------------------------------------------------------------
-- ����������б�����仯,ʹ���Ѷ��ĵ��¼�
-- �������holder_id
-- ���������ID task_id
-------------------------------------------------------------------
function OnActorTaskListChanged(holder_id, task_id, action)

     -- �����б�
     TASK_LIST_EVENT_TRAVEL =
     {
		{holder_id,task_id,action},
		{0,0,0},
		{0,0,action},
		{0,task_id,0},
		{holder_id,0,0},
		{holder_id,task_id,0},
		{holder_id,0,action},
		{0,task_id,action},
     }

     for i,key in pairs(TASK_LIST_EVENT_TRAVEL) do
		if TASK_LIST_EVENT_HANDLER[ key[1] ]~=nil and TASK_LIST_EVENT_HANDLER[ key[1] ][ key[2] ]~=nil and TASK_LIST_EVENT_HANDLER[ key[1] ][ key[2] ][ key[3] ]~=nil then
			for k,v in pairs(TASK_LIST_EVENT_HANDLER[ key[1] ][ key[2] ][ key[3] ]) do
				SafeCall(v, holder_id, task_id, action)
			end
		end
     end
end