-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/25/2016
-- ��  ��:	1.0
-- ��  ��:	ȫ�ֺ���
-------------------------------------------------------------------

-------------------------------------------------------------------
-- �ӳ�ִ��һ�δ��� 
-- ��Щ���벻���ڵ�ǰջ������ִ��,��Ҫ������ʱ���ӳ�ִ��һ�� 
-- @param func  : ����ָ��
-- @param ...   : �����б� 
-- @example     : DelayExecute( AddMonsterFriend,tmp_uid,actor,100 ) => AddMonsterFriend( tmp_uid,actor,100 )
-------------------------------------------------------------------
DELAY_EXECUTE_LIST = {}

-------------------------------------------------------------------
-- �ӳ�1��ִ��һ������
-------------------------------------------------------------------
function DelayExecute(func,...)

	arg = { ... }
	arg["func"] = func
	
	local nTriggerID = createTimerTrigger("DelayExecuteTimerHandler",1000,1)
 	DELAY_EXECUTE_LIST[nTriggerID] = arg
end

-------------------------------------------------------------------
-- �ӳٶ��ٺ���ִ��һ������
-------------------------------------------------------------------
function DelayExecuteEx(time, func,...)

	arg = { ... }
	arg["func"] = func

	local nTriggerID = createTimerTrigger("DelayExecuteTimerHandler",time,1)
 	DELAY_EXECUTE_LIST[nTriggerID] = arg
end

-------------------------------------------------------------------
-- �ӳ�ִ�к���
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
-- ʹ�ð�ȫ�ķ�ʽ����һ������
-- ��ν�İ�ȫ��ʽ����,������������ڲ�ִ�г���,��ôlua����Ҳ����������,����Ӱ�����Ĵ���
-- ʾ��: SafeCall( GetActorName,actorid )   �����ᰲȫ�ĵ���GetActorName����
--       ���GetActorName�������ó�����SafeCall�᷵��ʧ��,��Ҳ��������Ҫ�Ĵ�����
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
---purpose: �ж�һ�����Ƿ�Ϊ��
--@param:
--@return :
---------------------------------------------------------------------
function TableIsEmpty(t)
    return _G.next( t ) == nil
end


-- ��������ϸ����
gTriggerData = {}

-- ͳ�ƹ��ܿ���
gTriggerCountOpen = true

-------------------------------------------------------------------
---purpose	: ����һ������������
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
---purpose	: ɾ��һ������������
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
---purpose	: ��ӡ������ͳ������
--@param	: strFuncBack: nil->��ʾȫ��ӡ������->��ʾѡ���ӡ���ص������а���strFuncBack�����ݣ�
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
			-- ȫ�ִ�ӡʱ�� ֻ��ʾ �ص�����>=nShowThresh ������
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