-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/25/2016
-- 版  本:	1.0
-- 描  述:	全局函数
-------------------------------------------------------------------

-------------------------------------------------------------------
-- 延迟执行一段代码 
-- 有些代码不能在当前栈中立即执行,需要开个定时器延迟执行一下 
-- @param func  : 函数指针
-- @param ...   : 参数列表 
-- @example     : DelayExecute( AddMonsterFriend,tmp_uid,actor,100 ) => AddMonsterFriend( tmp_uid,actor,100 )
-------------------------------------------------------------------
DELAY_EXECUTE_LIST = {}

-------------------------------------------------------------------
-- 延迟1秒执行一个函数
-------------------------------------------------------------------
function DelayExecute(func,...)

	arg = { ... }
	arg["func"] = func
	
	local nTriggerID = createTimerTrigger("DelayExecuteTimerHandler",1000,1)
 	DELAY_EXECUTE_LIST[nTriggerID] = arg
end

-------------------------------------------------------------------
-- 延迟多少毫秒执行一个函数
-------------------------------------------------------------------
function DelayExecuteEx(time, func,...)

	arg = { ... }
	arg["func"] = func

	local nTriggerID = createTimerTrigger("DelayExecuteTimerHandler",time,1)
 	DELAY_EXECUTE_LIST[nTriggerID] = arg
end

-------------------------------------------------------------------
-- 延迟执行函数
-------------------------------------------------------------------
function DelayExecuteTimerHandler(nTriggerID)

	local args = DELAY_EXECUTE_LIST[nTriggerID]
	if args~=nil then
    	local func = args["func"]
    	args["func"]= nil
    	
    	func( table.unpack(args) )
	end
	
 	DELAY_EXECUTE_LIST[nTriggerID] = nil
end

-------------------------------------------------------------------
-- 使用安全的方式调用一个函数
-- 所谓的安全方式就是,假设这个函数内部执行出错,那么lua代码也继续往下跑,不会影响后面的代码
-- 示例: SafeCall( GetActorName,actorid )   该语句会安全的调用GetActorName函数
--       如果GetActorName函数调用出错则SafeCall会返回失败,你也可以做必要的错误处理
-------------------------------------------------------------------
function SafeCall(...)

    arg = { ... }
	local status,errstr = pcall( table.unpack(arg) )
	if not status then
		if errstr~=nil then
			error(errstr)
		end
		
		return false
	end
	
	return true,errstr
end


-------------------------------------------------------------------
---purpose: 判断一个表是否为空
--@param:
--@return :
---------------------------------------------------------------------
function TableIsEmpty(t)
    return _G.next( t ) == nil
end


-- 触发器详细数据
gTriggerData = {}

-- 统计功能开关
gTriggerCountOpen = true

-------------------------------------------------------------------
---purpose	: 增加一个触发器数据
--@param	:
--@return	:
---------------------------------------------------------------------
function LuaAddTriggerCout(CallBackStr, TriggerType, TriggerID)

	if TriggerID == nil or TriggerID == 0 or not gTriggerCountOpen then
		return
	end

	if gTriggerData[TriggerType] == nil then
		gTriggerData[TriggerType] = {}
	end

	if gTriggerData[TriggerType][CallBackStr] == nil then
		gTriggerData[TriggerType][CallBackStr] = 0
	end

	gTriggerData[TriggerType][CallBackStr] = gTriggerData[TriggerType][CallBackStr] + 1
end

-------------------------------------------------------------------
---purpose	: 删除一个触发器数据
--@param	:
--@return	:
---------------------------------------------------------------------
function LuaDelTriggerCout(CallBackStr, TriggerType, TriggerID)

	if gTriggerData[TriggerType][CallBackStr] == nil or not gTriggerCountOpen then
		return
	end

	gTriggerData[TriggerType][CallBackStr] = gTriggerData[TriggerType][CallBackStr] - 1
	if gTriggerData[TriggerType][CallBackStr] == 0 then
		gTriggerData[TriggerType][CallBackStr] = nil
	end
end

-------------------------------------------------------------------
---purpose	: 打印触发器统计数据
--@param	: strFuncBack: nil->表示全打印，其它->表示选择打印（回调函数中包含strFuncBack的数据）
--@return	:
---------------------------------------------------------------------
function LuaPrintTriggerData(strFuncBack)

	if not gTriggerCountOpen then
		return
	end

	local nCount = 0;
	local nType  = 0;
	local nShowThresh = 5;
	local tShowTrigger = {}
	for i,tTemp in pairs(gTriggerData) do
		for callFunc,Num in pairs(tTemp) do
			-- 全局打印时， 只显示 回调个数>=nShowThresh 的数据
			if strFuncBack == nil and Num >= nShowThresh then				
				table.insert(tShowTrigger, {Func = callFunc, Num = Num})
			elseif type(strFuncBack) == "string" and string.find(callFunc, strFuncBack) ~= nil then				
				table.insert(tShowTrigger, {Func = callFunc, Num = Num})
			end
			nCount = nCount + Num
			nType  = nType + 1
		end		
	end

	local nTresh = strFuncBack==nil and nShowThresh or 1
	table.sort(tShowTrigger, function(v1, v2) return v1.Num > v2.Num end)

	error("________________________________________________begin: nTresh="..nTresh)	
	for i,v in ipairs(tShowTrigger) do
		error("TriggerNum = " .. v.Num, "  CallBackFunc = " .. v.Func)
	end
	error("________________________________________________End")

	error("All Trigger type=", nType, "All Trigger Num=", nCount)
end