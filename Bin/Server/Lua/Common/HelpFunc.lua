-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/25/2016
-- 版  本:	1.0
-- 描  述:	常用辅助函数
-------------------------------------------------------------------

-------------------------------------------------------------------
-- 分割字符串
-- brief : 分割字符串，返回一个字符串数组
-- @param str : 要分割的字符串
-- @param separator : 分割符,可以是一个长度1-n的字符串
-- @param filter    : 过滤器,预处理函数,可以为nil,比如可以传入tonumber,这样就全部转成数字了
-------------------------------------------------------------------
function split_string(str,separator,filter)
	if str==nil then
		return nil
	end
	
	local ret = {}
	local sp_len = string.len(separator)
	local pos = 1- sp_len
	while true do
		local pre = pos+sp_len
		pos = string.find(str,separator,pre,true)
		if pos==nil then
			table.insert(ret,string.sub(str,pre))
			break
		else
			table.insert(ret,string.sub(str,pre,pos-1))
		end
	end
	
	-- 预处理一下
	if filter~=nil then
		do_visit( ret,filter )
	end
	
	return ret
end

-------------------------------------------------------------------
-- 对一个表中的所有元素做一个预处理 
-- @param inlist : 要处理的表 
-- @param func   : 处理函数
-- @示例         :
-- local str  = "1,2,3,4,5"
-- local x = split_string(str,",")
-- do_visit( x,tonumber ) -- 将x中的所有元素转换成整形      
-------------------------------------------------------------------
function do_visit( inlist,func )
	if inlist==nil then
		return nil
	end
	
	for k,v in pairs(inlist) do
		inlist[k] = func( v )
	end
	
	return inlist
end 

-------------------------------------------------------------------
-- 在一个表中查找某个元素
-- @param itable : 要查找的表
-- @param value  : 要找的值
-- @return       : 如果找到则返回这个值对应的键值,否则返回nil
------------------------------------------------------------------- 
function table_find( itable,value )
	for k,v in pairs(itable) do
		if v==value then
			return k
		end
	end
	
	return nil
end

-------------------------------------------------------------------
-- 返回一个表中的元素个数
-- @param t   : 要查找的表
-- @return    : 如果找到则返回这个值对应的键值,否则返回nil
-------------------------------------------------------------------
function table_count( t )

	if t == nil then
		return 0
	end

	local n = 0
	for k,v in pairs(t) do
	    n = n + 1
	end
	
	return n
end

-------------------------------------------------------------------
-- 将一个按顺序排放的表的顺序打乱 
-- @param t   : 要打乱的表  (注意：该函数会修改掉原始表的数据,因为lua里表是传引用的) 
-- @return    : 已经打乱的表 
-- @ps        : 借鉴微软空当接龙的发牌算法 
-------------------------------------------------------------------
function table_rand( t )
   local total = table_count( t )
   local ret   = {}
   
   while total>0 do
	   math.randomseed(tostring(os.time()):reverse():sub(1, 6))
   	   local i = math.random(1,total)
   	   table.insert( ret, t[i] )
   	   t[i] = t[total]
   	   total = total - 1
   end
   
   return ret
end

-------------------------------------------------------------------
-- 将一个按顺序排放的表的顺序打乱 
-- @param t   : 要打乱的表  (注意：该函数会修改掉原始表的数据,因为lua里表是传引用的) 
-- @return    : 引用输出t,已经打乱的表 
-- @add 2013-11-12 : 扩展了table_rand函数，输入table不必严格要求：下标必须从1开始且是连续的。
--                   此函数会对table里所有下标是“number”的元素进行随机重排列（洗牌），其它元素不变。
-------------------------------------------------------------------
function table_randEx( t )
	if (type(t) ~= "table") then
		return
	end
	
	local tTableFlag = {}	
	for index, val in pairs(t) do
		if (type(index)  == "number") then
			table.insert(tTableFlag, index)		
		end
	end	

	local nSum = table.getn(tTableFlag);
	for  nFlag = 1, nSum do
		math.randomseed(tostring(os.time()):reverse():sub(1, 6))
		local nRand = math.random(nFlag, nSum)
		if (nFlag ~= nRand) then                -- 元素交换			
			t[tTableFlag[nFlag]], t[tTableFlag[nRand]] = t[tTableFlag[nRand]], t[tTableFlag[nFlag]]			
		end
	end
end

-------------------------------------------------------------------
-- 拷贝表数据
-- @param b : 原始表
-- @param a : 新表
-------------------------------------------------------------------
-- lua的表默认是引用的，例如a={1,2} b=a b[1]=2 此时a[1]也会等于2
-- 这个函数支持将表真正的拷贝一份，各自可以独立修改
-- 同时支持嵌套的表拷贝,例如a={1,2,{3,4,5}} copy_table(b,a)
-------------------------------------------------------------------
function copy_table(a,b)
	for key,value in pairs(b) do
		if type(value)=='table' and value["parent"]==nil then
			a[key] = {}
			copy_table(a[key],value)
		else
			a[key]=value
		end
	end
end

-- 把表转为字符串形式，已转到C++中实现，以便提高性能
-- 此函数适用于以数字、非关键字字符串为键且以数字、字符串、布尔值和表为值的表，
-- 否则再用stringToTable转回表时可能会报错或结果与原表不一致
function TableToString(tTable)
	if type(tTable) ~= "table" then
		-- 非表则返回nil，尽量在引用处报错而不是在本函数报错或返回""，以便跟踪问题
		return nil
	end
	
	local strResult = ""
	-- 数字键索引
	local nNumberKeyIndex = 1
	-- 上一个数字键
	local nLastNumberKey = 0
	-- 数字键是否连续
	local bNmberKeySeries = true
	
	for vKey, vValue in pairs(tTable) do
		if strResult ~= "" then
			strResult = strResult .. ","
		end

		if type(vKey) == "number" then
			if vKey ~= nLastNumberKey + 1 then
				bNmberKeySeries = false
			end
			
			if vKey == nNumberKeyIndex and bNmberKeySeries then
				-- 不加键值
				strResult = strResult .. ToStringEx(vValue)
			else
				strResult = strResult .. "[" .. vKey .. "]=" .. ToStringEx(vValue)
			end
			
			nLastNumberKey = vKey
			nNumberKeyIndex = nNumberKeyIndex + 1
		else
			strResult = strResult .. vKey .. "=" .. ToStringEx(vValue)
		end
	end

 	strResult = "{" .. strResult .. "}"
	
 	return strResult
end

-------------------------------------------------------------------
-- 转换成字符串
-------------------------------------------------------------------
function ToStringEx(value)
	if type(value)=='table' then
	   return TableToString(value)
	elseif type(value)=='string' then
		return "\""..value.."\""
	else
	   return tostring(value)
	end
end

-------------------------------------------------------------------
-- 把一个字符串转换成一个表
-- 例如: "{1,2,3}" => {1,2,3}
--       "{a=1,b=2,c=3}" => {a=1,b=2,c=3}
-- 备注: TableToString的反向过程
-------------------------------------------------------------------
function StringToTable( str )
	if type(str) ~= "string" then
		return nil
	end

	local t = string.stringToTable(str)

	return t
end

-------------------------------------------------------------------
-- 将一个元素插入到一个表中，如果该表已存在这个元素则不插入
-- 如果table中嵌套了table,则调IsTableEqual判断两个表是否相等
-------------------------------------------------------------------
function InsertUni(t,item)
	for key,value in pairs(t) do
		if type(item)=='table' then
			if IsTableEqual(value,item) then
				return
			end
		else
			if value==item then
				return
			end
		end
	end
	
	table.insert(t,item)
end

-------------------------------------------------------------------
-- 两个table是否相等
-------------------------------------------------------------------
function IsTableEqual(t1,t2)

	if type(t1)~=type(t2) then
		return false
	end	
	if t1==t2 then
		return true
	end	
	local nCountT1 = 0
	for key,value in pairs(t2) do
		nCountT1 = nCountT1 + 1
	end	
	local nCountT2 = 0
	for key,value in pairs(t1) do
		nCountT2 = nCountT2 + 1		
		if type(value)=='table' then
			if IsTableEqual(t2[key],value)~=true then
				return false
			end
		elseif t2[key]~=value then
			return false
		end
	end	
	if nCountT1 ~= nCountT2 then
		return false
	end
	
	return true
end

-------------------------------------------------------------------
-- 返回金钱字符串
-------------------------------------------------------------------
function HFMoneyString(money)
	local gold = math.floor(money/10000)
	local silver = math.floor((money - math.floor(money/10000)*10000)/100)
	local copper = math.floor(money - math.floor(money/100)*100)

	local retString = ""
	if gold >= 1 then
		retString = retString..(string.format("%d锭",gold))
	end
	if silver >= 1 then
		retString = retString..(string.format("%d两",silver))
	end

	if (gold < 1 and silver < 1) or copper >= 1 then
		retString = retString..(string.format("%d文",copper))
	end

	return retString
end

-------------------------------------------------------------------------
-- 将2维表中的内容转存到csv文件
-- 该table中的元素只能是string,number,boolean三种类型
-------------------------------------------------------------------------
function SaveTableToCsv(table,filename,append)
	local file = nil
	
	if append == true then
        file = io.open(filename,"a")
	else
	    file = io.open(filename,"w+")
	end
	
	if file==nil then
		warning("save file "..filename.." failed!")
		return
	end

	local output_title = false

	if append==true then
		output_title = true
	end
	
	-- 将2维表中的数值依次存储到文件
	for y,record in pairs(table) do
		local str   = ""
		local title = ""
		local desc  = ""

		for x,field in pairs(record) do
			local space = ""
			if str~="" then
				space = ","
			end

			-- 生成标题
			if output_title==false then
				local type_index = { boolean='bool',number='int',string='string' }
				title = title..space..type_index[type(field)]
				desc  = desc..space.."字段"..x
			end

			-- 布尔型不能直接存
			if field==true then
				field = 1
			end
			if field==false then
				field = 0
			end

			str = str..space..field
		end

		-- 输出标题
		if output_title==false then
			output_title = true
			file:write(desc.."\n")
			file:write(title.."\n")
		end

		file:write(str.."\n")
	end

	file:flush()
	file:close()
end

--------------------------------------------------------------------------------
-- 时间换算通用函数
-- @param time 			: 此参数为时间表(local today = os.date("*t"))如其中today
-- @param control_param : 此参数为返回控制参数(1为返回小时制,2为返回分钟制,3为返回秒钟制)
-- @note        		: 此函数用于判断时间以便于数据更准确
-- @note				: time为完整的时间表,不然会有默认时间出现 
--------------------------------------------------------------------------------
function TimeComputationPublic(time,control_param)
	local a ={year=2008,month=1,day=1,hour=0,min=0,sec=0}
	local mid_num = os.time(a) 
    local num = os.time(time) - mid_num
	-- 由参数决定返回总小时,分钟或秒钟
	if  control_param == 1 then
		local total_hour_num = math.floor(num / 3600)   -- 小时 
		return total_hour_num
	end
	if  control_param == 2 then
		local total_minute_num = math.floor(num / 60)	-- 分钟 
	    return total_minute_num
	end
	if  control_param == 3 then
		local total_second_num = num   -- 秒钟 
	    return total_second_num
	end
end

-------------------------------------------------------------------
-- 返回时间字符串,不包括周几
-- @return   : 形式为18:00
-------------------------------------------------------------------
function GetTimeString(hour, min)
	local tmp_time = hour
	
	if hour < 10 then
		if min < 10 then
			tmp_time = "0"..hour..":".."0"..min
		else
			tmp_time = "0"..hour..":"..min
		end
	else
		if min < 10 then
			tmp_time = hour..":".."0"..min
		else
			tmp_time = hour..":"..min
		end
	end
	
	return tmp_time
end

-------------------------------------------------------------------
-- 返回时间字符串,包括周几
-- @return   : 形式为周六18:00
-------------------------------------------------------------------
function GetTimeStringEx(week,hour, min)
	local week_str = {[0] = "周日", "周一", "周二", "周三", "周四", "周五", "周六"}
	if week_str[week] == nil then
	    return GetTimeString(hour,min)
	else
	    return week_str[week]..GetTimeString(hour,min)
	end
	
	return ""
end

-------------------------------------------------------------------
-- 返回时间长度 
-- @return   : 格式 x天x小时x分钟
-------------------------------------------------------------------
function GetIntervalString(day, hour, minute,second)
    if second == nil then
        second = 0
    end
    
    if day > 0 then
		if hour > 0 then
			return string.format("%d天%d小时", day, hour)
		else
			return string.format("%d天", day)
		end
	else
		if hour > 0 then
		    if minute > 0 then
		        return string.format("%d小时%d分", hour, minute)
		    else
		        return string.format("%d小时", hour, minute)
		    end
		else
		    if minute > 0 then
		        if second > 0 then
		            return string.format("%d分%d秒", minute, second)
		        else
		            return string.format("%d分", minute)
		        end
		    else
		        if second > 0 then
		            return string.format("%d秒", second)
		        else
		            return "0"
		        end
		    end
		end
	end

	return "0"
end


-------------------------------------------------------------------
-- 转换成时间字符串
-------------------------------------------------------------------
function GetRemainSecTimeStr(nRemainSeconds)
	local strRet = ""
	-- 剩余时间计数
	local bNums = 0
	local sNums = 0
	-- 小于等于60秒
	if nRemainSeconds<60 then
		strRet = string.format("%d秒",nRemainSeconds)
	elseif nRemainSeconds<3600 then		--小于1小时
		bNums = math.floor(nRemainSeconds/60)
		local RemainSeconds_1, RemainSeconds_2 = math.modf(nRemainSeconds,60)
		sNums = RemainSeconds_1
		if sNums==0 then
			strRet = string.format("%d分钟",bNums)
		else
			strRet = string.format("%d分钟%d秒",bNums,sNums)
		end
	elseif nRemainSeconds<86400 then
		bNums = math.floor(nRemainSeconds/3600)
		local RemainSeconds_1, RemainSeconds_2 = math.modf(nRemainSeconds,3600)
		sNums = math.floor(RemainSeconds_1/60)
		if sNums==0 then
			strRet = string.format("%d小时",bNums)
		else
			strRet = string.format("%d小时%d分钟",bNums,sNums)
		end
	else
		bNums = math.floor(nRemainSeconds/86400)
		local RemainSeconds_1, RemainSeconds_2 = math.modf(nRemainSeconds,86400)
		sNums = math.floor(RemainSeconds_1/3600)
		if sNums==0 then
			strRet = string.format("%d天",bNums)
		else
			strRet = string.format("%d天%d小时",bNums,sNums)
		end
	end

	return strRet
end

-------------------------------------------------------------------
-- 把一个数字转化成字符串，每千位加一个","，如9,999,999
-------------------------------------------------------------------
function HFToKiloStr(value)
	local str = tostring(value)
	local len = string.len(str)
	local retStr = ""
	local offset = 1
	
	local len_1,len_2 = math.modf(len,3)
	if len_1 ~= 0 then
		retStr = string.sub(str, 0, len_1)
		offset = offset + len_1
	end
	
	while offset < len do
		if offset > 1 then
			retStr = retStr..","
		end
		
		retStr = retStr..string.sub(str, offset, offset + 2)
		offset = offset + 3
	end
	
	return retStr	
end

-------------------------------------------------------------------
-- 往字符串后面补齐空格，让他凑够总数量
-------------------------------------------------------------------
function FillSpace(str, maxLen)
    local str_len = string.len(str)
    if str_len >= maxLen then
        return str
    end
    local diff = maxLen - str_len
    for i = 1, diff do
        str = str.."&nbsp;"
    end
    
    return str
end

--------------------------------------------------------------------------------
-- 游戏中有许多任务限制了每天可以做的次数
-- 通常这些任务计数在每晚12点清0,这样许多玩家会等到12点一起做任务
-- 为了方便玩家,所有的任务安排在晚上22:00清0,为了实现这个目标又不改动已有实现
-- 我们将游戏中的逻辑时间往后偏移2个小时,所以检查任务次数的代码中,调用os.date的地方务必换成GetGameDate,取游戏逻辑时间
--------------------------------------------------------------------------------
function GetGameDate(reset_time)
	if reset_time==nil then
		reset_time = 24
	end
	
	return os.date("*t",os.time()+(24-reset_time)*3600)
end

-------------------------------------------------------------------
-- 计算2个日期之间的天数
-- @param time1 : 时间1，单位秒
-- @param time1 : 时间2，单位秒
-- @return    : 返回2个时间点相差的天数
-------------------------------------------------------------------
function DiffDay(time1, time2)
	local date1 = os.date("*t", time1)
	local date2 = os.date("*t", time2)
	
	
	local daySec = 3600 * 24
	time1 = time1 - (date1.hour * 3600 + date1.min * 60 + date1.sec)
	time2 = time2 - (date2.hour * 3600 + date2.min * 60 + date2.sec)

	local diffDay =  math.floor((time2 - time1) / daySec)

	return diffDay
end

-------------------------------------------------------------------
-- 计算2个日期之间的周数
-- @param time1 : 时间1，单位秒
-- @param time1 : 时间2，单位秒
-- @return    : 返回2个时间点相差的周数
-------------------------------------------------------------------
function DiffWeek(time1, time2)
	local date1 = os.date("*t", time1)
	local date2 = os.date("*t", time2)
	
	local daySec = 3600 * 24
	time1 = time1 - (date1.hour * 3600 + date1.min * 60 + date1.sec)
	time2 = time2 - (date2.hour * 3600 + date2.min * 60 + date2.sec)

	local diffDay =  math.abs(math.floor((time2 - time1) / daySec))
	local week1 = (date1.wday-1) if week1 == 0 then week1 = 7 end
	local week2 = (date2.wday-1) if week2 == 0 then week2 = 7 end
	
	if week2 < week1 then
	    return math.floor(diffDay / 7) + 1
	end
	
	return math.floor(diffDay / 7)
end

-------------------------------------------------------------------
-- 计算2个日期之间的月数
-- @param time1 : 时间1，单位秒
-- @param time1 : 时间2，单位秒
-- @return    : 返回2个时间点相差的周数
-------------------------------------------------------------------
function DiffMonth(time1, time2)

	local date1 = os.date("*t", time1)
	local date2 = os.date("*t", time2)
	
	local diffYear = date2.year - date1.year 
	local diffMonth = date2.month - date1.month
	
	return diffYear * 12 + diffMonth
end

-------------------------------------------------------------------
-- 计算2个日期之间的小时差
-- @param time1 : 时间1，单位秒
-- @param time1 : 时间2，单位秒
-- @return    : 返回2个时间点相差的小时
-------------------------------------------------------------------
function DiffHour(time1, time2)
	if time1==nil or time2 == nil then
		return 0
	end
	local nHourSec = 3600
	return math.floor((time2 - time1) / nHourSec)
end

-------------------------------------------------------------------
-- 计算2个日期之间的分钟差
-- @param time1 : 时间1，单位秒
-- @param time1 : 时间2，单位秒
-- @return    : 返回2个时间点相差的分钟差
-------------------------------------------------------------------
function DiffMin(time1, time2)
	if time1==nil or time2 == nil then
		return 0
	end

	local nMinSec = 60
	return  math.floor((time2 - time1) / nMinSec)

end

-------------------------------------------------------------------
-- 判断一个时间点是否在本月
-- @param time1 : 时间，单位秒
-------------------------------------------------------------------
function IsCurMonth(time)
    local today = os.date("*t")
    
    local time_date = os.date("*t", time)
    
    return (today.year == time_date.year and today.month == time_date.month)
end

-------------------------------------------------------------------
-- 是否是同一天
-- @param time1 : 时间1，单位秒
-- @param time1 : 时间2，单位秒
-- @return    : 返回2个时间点相差的天数
-------------------------------------------------------------------
function IsSameDay(time1, time2)
    local time_date1 = os.date("*t", time1)
    local time_date2 = os.date("*t", time2)
    
    return (time_date1.year == time_date2.year and time_date1.month == time_date2.month and time_date1.day == time_date2.day)
end

-------------------------------------------------------------------
-- 四舍五入一个数
-- @param fNumber		: 要四舍五入的数
-- @param nDecimalDigit	: 精确到小数点后的位数，为非负数，默认为0
-- @return				: 成功：四舍五入后的数，失败：原数
-- 举例：RoundNumber(3.1415926, 0) = 3, RoundNumber(3.1415926, 1) = 3.1
-- RoundNumber(3.1415926, 3) = 3.142, RoundNumber(-3.1415926, 4) = -3.1416
-------------------------------------------------------------------
function RoundNumber(fNumber, nDecimalDigit)
	if fNumber == nil then
		return fNumber
	end
	
	nDecimalDigit = nDecimalDigit or 0
	if nDecimalDigit < 0 then
		return fNumber
	end
	
	local fTempNumber = fNumber * 10 ^ nDecimalDigit
	local nNumber = math.floor(fTempNumber)
	if fTempNumber - nNumber >= 0.5 then
		nNumber = nNumber + 1
	end
	
	fNumber = nNumber / 10 ^ nDecimalDigit
	
	return fNumber
end

-------------------------------------------------------------------
-- 获取一个正整数在指定范围位的值
-- @param nNumber	: 原正整数
-- @param nBeginBit	: 从低位算起的开始位
-- @param nBeginBit	: 从低位算起的结束位
-- @return			: 成功：指定范围位的值，失败：原数
-- 举例 GetRangeBitValue(88, 4, 5) = 3，GetRangeBitValue(888, 1, 8) = 120
-------------------------------------------------------------------
function GetRangeBitValue(nNumber, nBeginBit, nEndBit)
	if nNumber == nil or nNumber < 0 then
		return nNumber
	end
	
	if nBeginBit == nil or nBeginBit <= 0 then
		return nNumber
	end
	
	if nEndBit == nil or nEndBit <= 0 then
		return nNumber
	end
	
	if nBeginBit > nEndBit then
		return nNumber
	end
	
	-- 所有设置位全为1的掩码
	local nMask = c_xor(2 ^ (nBeginBit - 1) - 1, 2 ^ nEndBit - 1)
	-- 与掩码相与来保留和掩码位对应的位值
	local nBitValue = c_and(nNumber, nMask)
	-- 移到最低位来获取最终值
	nBitValue = c_rshift(nBitValue, nBeginBit - 1)
	
	return nBitValue
end

-------------------------------------------------------------------
-- 设置一个正整数在指定范围位的值
-- @param nNumber	: 原正整数
-- @param nBeginBit	: 从低位算起的开始位
-- @param nBeginBit	: 从低位算起的结束位
-- @param nValue	: 设置的值
-- @return			: 成功：设置后的整体值，失败：原数
-- 举例 SetRangeBitValue(88, 4, 5, 1) = 72，SetRangeBitValue(888, 1, 8, 8) = 776
-------------------------------------------------------------------
function SetRangeBitValue(nNumber, nBeginBit, nEndBit, nValue)
	if nNumber == nil or nNumber < 0 then
		return nNumber
	end
	
	if nBeginBit == nil or nBeginBit <= 0 then
		return nNumber
	end
	
	if nEndBit == nil or nEndBit <= 0 then
		return nNumber
	end
	
	if nValue == nil or nValue < 0 then
		return nNumber
	end
	
	if nBeginBit > nEndBit then
		return nNumber
	end
	
	-- 所有设置位全为1的掩码
	local nMask = c_xor(2 ^ (nBeginBit - 1) - 1, 2 ^ nEndBit - 1)
	-- 把要设置的值移到要设置的位上
	nValue = c_lshift(nValue, nBeginBit - 1)
	-- 设置的值不能超过掩码
	if nValue > nMask then
		return nNumber
	end
	
	-- 先把要设置的位清0
	nNumber = c_and(nNumber, c_not(nMask))
	nNumber = c_or(nNumber, nValue)
	
	return nNumber
end

-------------------------------------------------------------------
-- 解析参数参数及函数
-------------------------------------------------------------------
function ParseAndRun(parmStr)
	local param_list = {}
	local func_name  = nil
	local i = nil
	local posBegin = string.find(parmStr,"%(");
	func_name= string.sub(parmStr,0,posBegin-1)
	local posEnd = string.find(parmStr,"%)")
	local parm= string.sub(parmStr,posBegin+1,posEnd-1)
	parm = parm..","
	--循环查找
	while(true) do
		local parmpos = string.find(parm,",")
		if parmpos ==nil then
			break
		end
		table.insert(param_list,string.sub(parm,0,parmpos-1))
		parm =string.sub(parm,parmpos+1)
	end
	if _G[func_name]==nil then
		return
	end
	_G[func_name](table.unpack(param_list))
end