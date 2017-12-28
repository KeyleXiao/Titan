-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/29/2016
-- ��  ��:	1.0
-- ��  ��:	���񴴽�����
-------------------------------------------------------------------

require "Hepler"
require "Task"

-------------------------------------------------------------------
-- ���񹤳�
-- note : ���� : ����һ������ID, ͨ�����񹤳������Դ���һ���������
-------------------------------------------------------------------
TaskFactory = Object:new
{
	tTaskList = {},
	tTaskTypeList = {}
}

-------------------------------------------------------------------
-- ��ȡ�����б� 
-- note : �ڴ˴�ע���������� 
-------------------------------------------------------------------
function TaskFactory:ReadTaskList()

	local strFileName = "Task.csv"

    local tTask = ReadCsvFile(strFileName)
	if tTask == nil then
		error("ReadTaskList read error strFileName="..strFileName)
		return
	end
	
	local shceme_match = {}
	shceme_match[1--[['���']]] = {'id','set'}
	shceme_match[2--[['����']]] = {'name','set'}
	shceme_match[3--[['����ű�']]] = {'load_name','load'}
	shceme_match[4--[['��������']]] = {'type','set'}				-- �������� TaskType:1��ʤ����2�������3�ճ�ѭ������4����5����
	shceme_match[5--[['����׷����ʾλ��']]] = {'trace_show','set'}	-- 1����ϵͳ2����������
	shceme_match[6--[['���ȼ�����']]] = {'sort_level','set'}		-- 0Ϊ����������Խ��д��Խǰ
	shceme_match[7--[['��������']]] = {'description','set'}
	shceme_match[8--[['��ʾ����']]] = {'prompt_type','set'}			-- 0.���ܲ���ʾ 1.������ʾ 2.�����ʾ
	shceme_match[9--[['�������߱���']]] = {'save','set'}			-- 0.���� 1.������
	shceme_match[10--[['��ɾ��']]]   = {'allow_delete','set'}		-- 0.����ɾ�� 1.��ɾ��
	shceme_match[11--[['ѭ������']]] = {'loop_type','set'}			-- 1.��Сʱˢ�� 2.ÿ�չ̶�ʱ��ˢ�� 3.ÿ��ˢ��
	shceme_match[12--[['ˢ��ʱ��']]] = {'flush_time','set'}			-- Сʱˢ�¼��
	shceme_match[13--[['�մ���']]] = {'day_count','set'}			-- ÿ���ظ�����
	shceme_match[14--[['�ܴ���']]] = {'week_count','set'}			-- ÿ���ظ�����
	shceme_match[15--[['���������������']]] = {'clear_type','set'}	-- 0.����� 1.ÿ����� 2.ÿ�����
	shceme_match[16--[['ǰ������']]] = {'pre_id','set'}
	shceme_match[17--[['��������']]] = {'rear_id','set'}
	shceme_match[18--[['��������:�����ߵȼ�']]] = {'accept_condition_level','accept_condition_level'}
	shceme_match[19--[['�������:�������']]] = {'finish_condition_team_type','set'}
	shceme_match[20--[['�������:ʤ������']]] = {'finish_condition_win_count','set'}
	shceme_match[21--[['�������:��ɴ���']]] = {'finish_condition_finish_count','set'}
	shceme_match[22--[['�������:ʹ��Ӣ������']]] = {'finish_condition_hero_type','finish_condition_hero_type'}
	shceme_match[23--[['�������:ӵ�ж��ٸ�Ӣ��']]] = {'finish_condition_hero_count','set'}
	shceme_match[24--[['�������:ӵ�ж��ٸ�Ӣ��Ƥ��']]] = {'finish_condition_heroskin_count','set'}
	shceme_match[25--[['�������:��ɫ�ȼ�']]] = {'finish_condition_level','set'}	-- ��ɫ�ȼ��ﵽX��
	shceme_match[26--[['�������:��ɫ�����ܳ���']]] = {'finish_condition_match_count','set'}
	shceme_match[27--[['�������:�򿪱������']]] = {'finish_condition_open_chest_count','set'}
	shceme_match[28--[['�������:���˾�����']]] = {'finish_condition_clan_donate_count','set'}
	shceme_match[29--[['������ʾ����']]] = {'prize_type','set'}				-- 0.��� 1.��ɫ���� 2.��һ������ID
	shceme_match[30--[['�����󶨽�Ǯ']]] = {'prize_binding_gold','set'}
	shceme_match[31--[['������ɫ����']]] = {'prize_hero_exp','set'}
	shceme_match[32--[['������Ʒ']]] = {'prize_good','prize_good'}
	shceme_match[33--[['�ͻ�����������']]] = {'client_link','set'}	-- 1������ʤ2�������3�������4������5��ʯ6����7ʦͽ8Ӣ�۽���
	
	shceme_match[34--[['����������']]] = {'sub_type', 'set'}		-- 1��ҫ��2������3���ѡ�4���顢5һ����
	shceme_match[35--[['��������������']]] = {'sub_type_name', 'set'}
	shceme_match[36--[['�����������']]] = {'specific_type', 'set'}	
	shceme_match[37--[['���������������']]] = {'specific_type_name', 'set'}
	shceme_match[38--[['����˵��']]] = {'publish_description', 'set'}
	shceme_match[39--[['���黰��']]] = {'emotion_words', 'set'}	
	shceme_match[40--[['�����Ǽ�']]] = {'task_level','set'}

	local scheme_field_num = table_count(shceme_match)
	for index,task_info in pairs(tTask) do
		if scheme_field_num ~= table_count(task_info) then
			error("ReadTaskList error no match")
			return
		end
		
		local task_id     = task_info[1]
		local task_name   = task_info[2]
		local script_file = task_info[3]
		local task_type	  = task_info[4]
		local couple_value = nil
		
		-- ���ؽű�
		if script_file == nil or script_file == '' or script_file== ' ' then
			error("ReadTaskList error load script_file id="..task_id)
			return
		end

		local propertys = { id=task_id, name=task_name, type=task_type }
		
		for field=5, scheme_field_num, 1 do
			local key = shceme_match[field][1]
			local action = shceme_match[field][2]
			local value = task_info[field]
			
			-- ��ֵ���� 
			if value ~= nil and value ~= -1 and value ~= '' and value ~= ' ' then
				-- ����ֵ 
				if action == 'set' then
					if type(value)=="string" then
						value = string.gsub(value,'"','')    -- ȥ������
						value = string.gsub(value,'\n','')   -- ȥ���س�
						value = string.gsub(value,'\r','')   -- ȥ������
					end
					
					propertys[key] = value
				end

				-- ���������ȼ� 
				if action == 'accept_condition_level' then

					if type(value)=="string" then
						value = string.gsub(value,'"','')    -- ȥ������
						value = string.gsub(value,'\n','')   -- ȥ���س�
						value = string.gsub(value,'\r','')   -- ȥ������
					end

					if value ~= '-1' then
						-- �ֽ���������ȼ�
						local tLevel = split_string(value, ";", tonumber)
						if tLevel == nil or table_count(tLevel) ~= 2 then
							error("ReadTaskList error index="..index..",accept_condition_level")
						end

						propertys[key] = tLevel
					end
				end

				-- �������:Ӣ������ 
				if action == 'finish_condition_hero_type' then

					if type(value)=="string" then
						value = string.gsub(value,'"','')    -- ȥ������
						value = string.gsub(value,'\n','')   -- ȥ���س�
						value = string.gsub(value,'\r','')   -- ȥ������
					end

					if value ~= '-1' then
						-- �ֽ��������:Ӣ������ 
						propertys[key] = split_string(value, ";", tonumber)
					end
				end

				-- ������Ʒ 
				if action == 'prize_good' then

					if type(value)=="string" then
						value = string.gsub(value,'"','')    -- ȥ������
						value = string.gsub(value,'\n','')   -- ȥ���س�
						value = string.gsub(value,'\r','')   -- ȥ������
					end

					if value ~= '-1' then
						-- �ֽ⽱����Ʒ
						propertys[key] = split_string(value, ";", tonumber)
					end
				end
			end
		end
		
		local strScript = ""..script_file
		-- װ�ؽű�
		require(strScript)

		local task = _G[script_file]
		if task == nil then
			error("ReadTaskList error script_file="..script_file.."not find id="..task_id..",task_name="..task_name)
			return
		end

		self.tTaskList[task_id] = new(task, propertys)
		
		if self.tTaskTypeList[task_type] == nil then
			self.tTaskTypeList[task_type] = {}
		end
		
		table.insert(self.tTaskTypeList[task_type], task_id)
	end
		
	trace("----------------------- TaskFactory load finish --------------------------")
end

-------------------------------------------------------------------
-- ע������
-- note : ע��һ���������񹤳�
-- @param Task : �������
-------------------------------------------------------------------
function RegisterTask(Task)

    if Task.id == 0 then
    	error("RegisterTask id=0,name="..Task.name)
    	return
    end

    TaskFactory.tTaskList[Task.id] = Task
end

-------------------------------------------------------------------
-- ��������
-- @param holder_id   : ������������
-- @param task_id	  : Ҫ����������ID
-- @warning           : �����˳�ʱ�����DeleteTask�������� 
-------------------------------------------------------------------
function TaskFactory:CreateTask(holder_id, task_id)

	if task_id == nil then
		warning("CreateTask Failed!task_id is nil")
		return nil
	end
	
	local Task = self.tTaskList[task_id]
	if Task ~= nil then
		return new(Task,{holder_id=holder_id,state=TaskState.BeginState})
	end

	warning("CreateTask error task_id="..task_id)
	return nil
end

-------------------------------------------------------------------
-- ��������
-- @param Task : �������
-------------------------------------------------------------------
function TaskFactory:DeleteTask(Task)

	if Task == nil then
		return
	end

	delete(Task)
	Task = nil
end

----------------------------------------------------------------------- �������� ----------------------------------------------------------------------------

-------------------------------------------------------------------
-- ������������
-- @param holder_id			: ������������
-- @param sub_type			: ������������������
-- @param specific_type		: �������������������¾�������
-- @param contact_high32id	: �������ID��32λ
-- @param contact_low32id	: �������ID��32λ
-- @param publisher_id		: ������ID
-- @warning					: �����˳�ʱ�����DeleteTask��������
-- @addition				: ��������ʱ����ã�����loadʱ������CreateTask
-------------------------------------------------------------------
function TaskFactory:CreateContactTask(holder_id, sub_type, specific_type, contact_high32id, contact_low32id, publisher_id)

	if holder_id == nil or sub_type == nil or specific_type == nil or publisher_id == nil then
		error("CreateContactTask Failed! some params = nil")
		return nil
	end

	if self.tTaskTypeList == nil or self.tTaskTypeList[TaskType.ContacType] == nil then
		error("CreateContactTask failed! self.tTaskTypeList[TaskType.ContacType] == nil")
		return nil
	end
	
	-- ��һ���յ��������
	local nTaskID = nil
	for _,task_id in pairs(self.tTaskTypeList[TaskType.ContacType]) do
		local oTask = self.tTaskList[task_id]
		local task_key = GetTaskIDKey(holder_id, task_id)
		local task_state = GetTaskIntData(holder_id, task_key)
		if oTask.sub_type == sub_type and oTask.specific_type == specific_type and task_state == TASK_DATA_INVALID then
			nTaskID = task_id
			break
		end
	end
	if nTaskID == nil then
		error("CreateContactTask failed! all this type task is used sub_type = "..sub_type..", specific_type = "..specific_type)
		return nil
	end
	
	local Task = self.tTaskList[nTaskID]
	if Task ~= nil then
		return new(Task, {holder_id = holder_id, state = TaskState.BeginState, contact_high32id = contact_high32id, contact_low32id = contact_low32id, publisher_id = publisher_id})
	end

	error("CreateContactTask Failed! holder_id ="..holder_id..", task_id = "..nTaskID)
	return nil
end
