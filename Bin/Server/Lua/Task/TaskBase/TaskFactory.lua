-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/29/2016
-- 版  本:	1.0
-- 描  述:	任务创建工厂
-------------------------------------------------------------------

require "Hepler"
require "Task"

-------------------------------------------------------------------
-- 任务工厂
-- note : 作用 : 传入一个任务ID, 通过任务工厂，可以创建一个任务对象
-------------------------------------------------------------------
TaskFactory = Object:new
{
	tTaskList = {},
	tTaskTypeList = {}
}

-------------------------------------------------------------------
-- 读取任务列表 
-- note : 在此处注册具体的任务 
-------------------------------------------------------------------
function TaskFactory:ReadTaskList()

	local strFileName = "Task.csv"

    local tTask = ReadCsvFile(strFileName)
	if tTask == nil then
		error("ReadTaskList read error strFileName="..strFileName)
		return
	end
	
	local shceme_match = {}
	shceme_match[1--[['编号']]] = {'id','set'}
	shceme_match[2--[['名称']]] = {'name','set'}
	shceme_match[3--[['任务脚本']]] = {'load_name','load'}
	shceme_match[4--[['任务类型']]] = {'type','set'}				-- 任务类型 TaskType:1首胜任务2帮会任务3日常循环任务4主线5交互
	shceme_match[5--[['任务追踪显示位置']]] = {'trace_show','set'}	-- 1任务系统2帮会任务界面
	shceme_match[6--[['优先级排序']]] = {'sort_level','set'}		-- 0为不处理，数字越大写的越前
	shceme_match[7--[['任务描述']]] = {'description','set'}
	shceme_match[8--[['提示类型']]] = {'prompt_type','set'}			-- 0.接受不提示 1.接受提示 2.完成提示
	shceme_match[9--[['任务下线保存']]] = {'save','set'}			-- 0.保存 1.不保存
	shceme_match[10--[['可删除']]]   = {'allow_delete','set'}		-- 0.不可删除 1.可删除
	shceme_match[11--[['循环类型']]] = {'loop_type','set'}			-- 1.按小时刷新 2.每日固定时间刷新 3.每周刷新
	shceme_match[12--[['刷新时间']]] = {'flush_time','set'}			-- 小时刷新间隔
	shceme_match[13--[['日次数']]] = {'day_count','set'}			-- 每日重复次数
	shceme_match[14--[['周次数']]] = {'week_count','set'}			-- 每周重复次数
	shceme_match[15--[['任务数据清除条件']]] = {'clear_type','set'}	-- 0.不清除 1.每日清除 2.每周清除
	shceme_match[16--[['前置任务']]] = {'pre_id','set'}
	shceme_match[17--[['后置任务']]] = {'rear_id','set'}
	shceme_match[18--[['接受条件:最低最高等级']]] = {'accept_condition_level','accept_condition_level'}
	shceme_match[19--[['完成条件:组队类型']]] = {'finish_condition_team_type','set'}
	shceme_match[20--[['完成条件:胜利次数']]] = {'finish_condition_win_count','set'}
	shceme_match[21--[['完成条件:完成次数']]] = {'finish_condition_finish_count','set'}
	shceme_match[22--[['完成条件:使用英雄类型']]] = {'finish_condition_hero_type','finish_condition_hero_type'}
	shceme_match[23--[['完成条件:拥有多少个英雄']]] = {'finish_condition_hero_count','set'}
	shceme_match[24--[['完成条件:拥有多少个英雄皮肤']]] = {'finish_condition_heroskin_count','set'}
	shceme_match[25--[['完成条件:角色等级']]] = {'finish_condition_level','set'}	-- 角色等级达到X级
	shceme_match[26--[['完成条件:角色比赛总场次']]] = {'finish_condition_match_count','set'}
	shceme_match[27--[['完成条件:打开宝箱个数']]] = {'finish_condition_open_chest_count','set'}
	shceme_match[28--[['完成条件:联盟捐献数']]] = {'finish_condition_clan_donate_count','set'}
	shceme_match[29--[['奖励显示类型']]] = {'prize_type','set'}				-- 0.绑金 1.角色经验 2.第一个奖励ID
	shceme_match[30--[['奖励绑定金钱']]] = {'prize_binding_gold','set'}
	shceme_match[31--[['奖励角色经验']]] = {'prize_hero_exp','set'}
	shceme_match[32--[['奖励物品']]] = {'prize_good','prize_good'}
	shceme_match[33--[['客户端引导类型']]] = {'client_link','set'}	-- 1联盟首胜2联盟组队3参与比赛4开宝箱5宝石6联盟7师徒8英雄晋升
	
	shceme_match[34--[['任务子类型']]] = {'sub_type', 'set'}		-- 1炫耀、2互助、3交友、4心情、5一起玩
	shceme_match[35--[['任务子类型名称']]] = {'sub_type_name', 'set'}
	shceme_match[36--[['任务具体类型']]] = {'specific_type', 'set'}	
	shceme_match[37--[['任务具体类型名称']]] = {'specific_type_name', 'set'}
	shceme_match[38--[['发布说明']]] = {'publish_description', 'set'}
	shceme_match[39--[['心情话语']]] = {'emotion_words', 'set'}	
	shceme_match[40--[['任务星级']]] = {'task_level','set'}

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
		
		-- 加载脚本
		if script_file == nil or script_file == '' or script_file== ' ' then
			error("ReadTaskList error load script_file id="..task_id)
			return
		end

		local propertys = { id=task_id, name=task_name, type=task_type }
		
		for field=5, scheme_field_num, 1 do
			local key = shceme_match[field][1]
			local action = shceme_match[field][2]
			local value = task_info[field]
			
			-- 空值忽略 
			if value ~= nil and value ~= -1 and value ~= '' and value ~= ' ' then
				-- 设置值 
				if action == 'set' then
					if type(value)=="string" then
						value = string.gsub(value,'"','')    -- 去掉引号
						value = string.gsub(value,'\n','')   -- 去掉回车
						value = string.gsub(value,'\r','')   -- 去掉换行
					end
					
					propertys[key] = value
				end

				-- 接受条件等级 
				if action == 'accept_condition_level' then

					if type(value)=="string" then
						value = string.gsub(value,'"','')    -- 去掉引号
						value = string.gsub(value,'\n','')   -- 去掉回车
						value = string.gsub(value,'\r','')   -- 去掉换行
					end

					if value ~= '-1' then
						-- 分解接受条件等级
						local tLevel = split_string(value, ";", tonumber)
						if tLevel == nil or table_count(tLevel) ~= 2 then
							error("ReadTaskList error index="..index..",accept_condition_level")
						end

						propertys[key] = tLevel
					end
				end

				-- 完成条件:英雄类型 
				if action == 'finish_condition_hero_type' then

					if type(value)=="string" then
						value = string.gsub(value,'"','')    -- 去掉引号
						value = string.gsub(value,'\n','')   -- 去掉回车
						value = string.gsub(value,'\r','')   -- 去掉换行
					end

					if value ~= '-1' then
						-- 分解完成条件:英雄类型 
						propertys[key] = split_string(value, ";", tonumber)
					end
				end

				-- 奖励物品 
				if action == 'prize_good' then

					if type(value)=="string" then
						value = string.gsub(value,'"','')    -- 去掉引号
						value = string.gsub(value,'\n','')   -- 去掉回车
						value = string.gsub(value,'\r','')   -- 去掉换行
					end

					if value ~= '-1' then
						-- 分解奖励物品
						propertys[key] = split_string(value, ";", tonumber)
					end
				end
			end
		end
		
		local strScript = ""..script_file
		-- 装载脚本
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
-- 注册任务
-- note : 注册一个任务到任务工厂
-- @param Task : 任务对象
-------------------------------------------------------------------
function RegisterTask(Task)

    if Task.id == 0 then
    	error("RegisterTask id=0,name="..Task.name)
    	return
    end

    TaskFactory.tTaskList[Task.id] = Task
end

-------------------------------------------------------------------
-- 创建任务
-- @param holder_id   : 创建任务的玩家
-- @param task_id	  : 要创建的任务ID
-- @warning           : 人物退出时请调用DeleteTask销毁任务 
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
-- 销毁任务
-- @param Task : 任务对象
-------------------------------------------------------------------
function TaskFactory:DeleteTask(Task)

	if Task == nil then
		return
	end

	delete(Task)
	Task = nil
end

----------------------------------------------------------------------- 交互任务 ----------------------------------------------------------------------------

-------------------------------------------------------------------
-- 创建交互任务
-- @param holder_id			: 创建任务的玩家
-- @param sub_type			: 创建交互任务子类型
-- @param specific_type		: 创建交互任务子类型下具体类型
-- @param contact_high32id	: 随机交互ID高32位
-- @param contact_low32id	: 随机交互ID低32位
-- @param publisher_id		: 发布者ID
-- @warning					: 人物退出时请调用DeleteTask销毁任务
-- @addition				: 仅创建的时候调用，上线load时还调用CreateTask
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
	
	-- 找一个空的任务填充
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
