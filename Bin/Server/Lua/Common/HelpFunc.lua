-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/25/2016
-- ��  ��:	1.0
-- ��  ��:	���ø�������
-------------------------------------------------------------------

-------------------------------------------------------------------
-- �ָ��ַ���
-- brief : �ָ��ַ���������һ���ַ�������
-- @param str : Ҫ�ָ���ַ���
-- @param separator : �ָ��,������һ������1-n���ַ���
-- @param filter    : ������,Ԥ������,����Ϊnil,������Դ���tonumber,������ȫ��ת��������
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
	
	-- Ԥ����һ��
	if filter~=nil then
		do_visit( ret,filter )
	end
	
	return ret
end

-------------------------------------------------------------------
-- ��һ�����е�����Ԫ����һ��Ԥ���� 
-- @param inlist : Ҫ����ı� 
-- @param func   : ������
-- @ʾ��         :
-- local str  = "1,2,3,4,5"
-- local x = split_string(str,",")
-- do_visit( x,tonumber ) -- ��x�е�����Ԫ��ת��������      
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
-- ��һ�����в���ĳ��Ԫ��
-- @param itable : Ҫ���ҵı�
-- @param value  : Ҫ�ҵ�ֵ
-- @return       : ����ҵ��򷵻����ֵ��Ӧ�ļ�ֵ,���򷵻�nil
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
-- ����һ�����е�Ԫ�ظ���
-- @param t   : Ҫ���ҵı�
-- @return    : ����ҵ��򷵻����ֵ��Ӧ�ļ�ֵ,���򷵻�nil
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
-- ��һ����˳���ŷŵı��˳����� 
-- @param t   : Ҫ���ҵı�  (ע�⣺�ú������޸ĵ�ԭʼ�������,��Ϊlua����Ǵ����õ�) 
-- @return    : �Ѿ����ҵı� 
-- @ps        : ���΢��յ������ķ����㷨 
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
-- ��һ����˳���ŷŵı��˳����� 
-- @param t   : Ҫ���ҵı�  (ע�⣺�ú������޸ĵ�ԭʼ�������,��Ϊlua����Ǵ����õ�) 
-- @return    : �������t,�Ѿ����ҵı� 
-- @add 2013-11-12 : ��չ��table_rand����������table�����ϸ�Ҫ���±�����1��ʼ���������ġ�
--                   �˺������table�������±��ǡ�number����Ԫ�ؽ�����������У�ϴ�ƣ�������Ԫ�ز��䡣
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
		if (nFlag ~= nRand) then                -- Ԫ�ؽ���			
			t[tTableFlag[nFlag]], t[tTableFlag[nRand]] = t[tTableFlag[nRand]], t[tTableFlag[nFlag]]			
		end
	end
end

-------------------------------------------------------------------
-- ����������
-- @param b : ԭʼ��
-- @param a : �±�
-------------------------------------------------------------------
-- lua�ı�Ĭ�������õģ�����a={1,2} b=a b[1]=2 ��ʱa[1]Ҳ�����2
-- �������֧�ֽ��������Ŀ���һ�ݣ����Կ��Զ����޸�
-- ͬʱ֧��Ƕ�׵ı���,����a={1,2,{3,4,5}} copy_table(b,a)
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

-- �ѱ�תΪ�ַ�����ʽ����ת��C++��ʵ�֣��Ա��������
-- �˺��������������֡��ǹؼ����ַ���Ϊ���������֡��ַ���������ֵ�ͱ�Ϊֵ�ı�
-- ��������stringToTableת�ر�ʱ���ܻᱨ�������ԭ��һ��
function TableToString(tTable)
	if type(tTable) ~= "table" then
		-- �Ǳ��򷵻�nil�����������ô�����������ڱ���������򷵻�""���Ա��������
		return nil
	end
	
	local strResult = ""
	-- ���ּ�����
	local nNumberKeyIndex = 1
	-- ��һ�����ּ�
	local nLastNumberKey = 0
	-- ���ּ��Ƿ�����
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
				-- ���Ӽ�ֵ
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
-- ת�����ַ���
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
-- ��һ���ַ���ת����һ����
-- ����: "{1,2,3}" => {1,2,3}
--       "{a=1,b=2,c=3}" => {a=1,b=2,c=3}
-- ��ע: TableToString�ķ������
-------------------------------------------------------------------
function StringToTable( str )
	if type(str) ~= "string" then
		return nil
	end

	local t = string.stringToTable(str)

	return t
end

-------------------------------------------------------------------
-- ��һ��Ԫ�ز��뵽һ�����У�����ñ��Ѵ������Ԫ���򲻲���
-- ���table��Ƕ����table,���IsTableEqual�ж��������Ƿ����
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
-- ����table�Ƿ����
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
-- ���ؽ�Ǯ�ַ���
-------------------------------------------------------------------
function HFMoneyString(money)
	local gold = math.floor(money/10000)
	local silver = math.floor((money - math.floor(money/10000)*10000)/100)
	local copper = math.floor(money - math.floor(money/100)*100)

	local retString = ""
	if gold >= 1 then
		retString = retString..(string.format("%d��",gold))
	end
	if silver >= 1 then
		retString = retString..(string.format("%d��",silver))
	end

	if (gold < 1 and silver < 1) or copper >= 1 then
		retString = retString..(string.format("%d��",copper))
	end

	return retString
end

-------------------------------------------------------------------------
-- ��2ά���е�����ת�浽csv�ļ�
-- ��table�е�Ԫ��ֻ����string,number,boolean��������
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
	
	-- ��2ά���е���ֵ���δ洢���ļ�
	for y,record in pairs(table) do
		local str   = ""
		local title = ""
		local desc  = ""

		for x,field in pairs(record) do
			local space = ""
			if str~="" then
				space = ","
			end

			-- ���ɱ���
			if output_title==false then
				local type_index = { boolean='bool',number='int',string='string' }
				title = title..space..type_index[type(field)]
				desc  = desc..space.."�ֶ�"..x
			end

			-- �����Ͳ���ֱ�Ӵ�
			if field==true then
				field = 1
			end
			if field==false then
				field = 0
			end

			str = str..space..field
		end

		-- �������
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
-- ʱ�任��ͨ�ú���
-- @param time 			: �˲���Ϊʱ���(local today = os.date("*t"))������today
-- @param control_param : �˲���Ϊ���ؿ��Ʋ���(1Ϊ����Сʱ��,2Ϊ���ط�����,3Ϊ����������)
-- @note        		: �˺��������ж�ʱ���Ա������ݸ�׼ȷ
-- @note				: timeΪ������ʱ���,��Ȼ����Ĭ��ʱ����� 
--------------------------------------------------------------------------------
function TimeComputationPublic(time,control_param)
	local a ={year=2008,month=1,day=1,hour=0,min=0,sec=0}
	local mid_num = os.time(a) 
    local num = os.time(time) - mid_num
	-- �ɲ�������������Сʱ,���ӻ�����
	if  control_param == 1 then
		local total_hour_num = math.floor(num / 3600)   -- Сʱ 
		return total_hour_num
	end
	if  control_param == 2 then
		local total_minute_num = math.floor(num / 60)	-- ���� 
	    return total_minute_num
	end
	if  control_param == 3 then
		local total_second_num = num   -- ���� 
	    return total_second_num
	end
end

-------------------------------------------------------------------
-- ����ʱ���ַ���,�������ܼ�
-- @return   : ��ʽΪ18:00
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
-- ����ʱ���ַ���,�����ܼ�
-- @return   : ��ʽΪ����18:00
-------------------------------------------------------------------
function GetTimeStringEx(week,hour, min)
	local week_str = {[0] = "����", "��һ", "�ܶ�", "����", "����", "����", "����"}
	if week_str[week] == nil then
	    return GetTimeString(hour,min)
	else
	    return week_str[week]..GetTimeString(hour,min)
	end
	
	return ""
end

-------------------------------------------------------------------
-- ����ʱ�䳤�� 
-- @return   : ��ʽ x��xСʱx����
-------------------------------------------------------------------
function GetIntervalString(day, hour, minute,second)
    if second == nil then
        second = 0
    end
    
    if day > 0 then
		if hour > 0 then
			return string.format("%d��%dСʱ", day, hour)
		else
			return string.format("%d��", day)
		end
	else
		if hour > 0 then
		    if minute > 0 then
		        return string.format("%dСʱ%d��", hour, minute)
		    else
		        return string.format("%dСʱ", hour, minute)
		    end
		else
		    if minute > 0 then
		        if second > 0 then
		            return string.format("%d��%d��", minute, second)
		        else
		            return string.format("%d��", minute)
		        end
		    else
		        if second > 0 then
		            return string.format("%d��", second)
		        else
		            return "0"
		        end
		    end
		end
	end

	return "0"
end


-------------------------------------------------------------------
-- ת����ʱ���ַ���
-------------------------------------------------------------------
function GetRemainSecTimeStr(nRemainSeconds)
	local strRet = ""
	-- ʣ��ʱ�����
	local bNums = 0
	local sNums = 0
	-- С�ڵ���60��
	if nRemainSeconds<60 then
		strRet = string.format("%d��",nRemainSeconds)
	elseif nRemainSeconds<3600 then		--С��1Сʱ
		bNums = math.floor(nRemainSeconds/60)
		local RemainSeconds_1, RemainSeconds_2 = math.modf(nRemainSeconds,60)
		sNums = RemainSeconds_1
		if sNums==0 then
			strRet = string.format("%d����",bNums)
		else
			strRet = string.format("%d����%d��",bNums,sNums)
		end
	elseif nRemainSeconds<86400 then
		bNums = math.floor(nRemainSeconds/3600)
		local RemainSeconds_1, RemainSeconds_2 = math.modf(nRemainSeconds,3600)
		sNums = math.floor(RemainSeconds_1/60)
		if sNums==0 then
			strRet = string.format("%dСʱ",bNums)
		else
			strRet = string.format("%dСʱ%d����",bNums,sNums)
		end
	else
		bNums = math.floor(nRemainSeconds/86400)
		local RemainSeconds_1, RemainSeconds_2 = math.modf(nRemainSeconds,86400)
		sNums = math.floor(RemainSeconds_1/3600)
		if sNums==0 then
			strRet = string.format("%d��",bNums)
		else
			strRet = string.format("%d��%dСʱ",bNums,sNums)
		end
	end

	return strRet
end

-------------------------------------------------------------------
-- ��һ������ת�����ַ�����ÿǧλ��һ��","����9,999,999
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
-- ���ַ������油��ո������չ�������
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
-- ��Ϸ�����������������ÿ��������Ĵ���
-- ͨ����Щ���������ÿ��12����0,���������һ�ȵ�12��һ��������
-- Ϊ�˷������,���е�������������22:00��0,Ϊ��ʵ�����Ŀ���ֲ��Ķ�����ʵ��
-- ���ǽ���Ϸ�е��߼�ʱ������ƫ��2��Сʱ,���Լ����������Ĵ�����,����os.date�ĵط���ػ���GetGameDate,ȡ��Ϸ�߼�ʱ��
--------------------------------------------------------------------------------
function GetGameDate(reset_time)
	if reset_time==nil then
		reset_time = 24
	end
	
	return os.date("*t",os.time()+(24-reset_time)*3600)
end

-------------------------------------------------------------------
-- ����2������֮�������
-- @param time1 : ʱ��1����λ��
-- @param time1 : ʱ��2����λ��
-- @return    : ����2��ʱ�����������
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
-- ����2������֮�������
-- @param time1 : ʱ��1����λ��
-- @param time1 : ʱ��2����λ��
-- @return    : ����2��ʱ�����������
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
-- ����2������֮�������
-- @param time1 : ʱ��1����λ��
-- @param time1 : ʱ��2����λ��
-- @return    : ����2��ʱ�����������
-------------------------------------------------------------------
function DiffMonth(time1, time2)

	local date1 = os.date("*t", time1)
	local date2 = os.date("*t", time2)
	
	local diffYear = date2.year - date1.year 
	local diffMonth = date2.month - date1.month
	
	return diffYear * 12 + diffMonth
end

-------------------------------------------------------------------
-- ����2������֮���Сʱ��
-- @param time1 : ʱ��1����λ��
-- @param time1 : ʱ��2����λ��
-- @return    : ����2��ʱ�������Сʱ
-------------------------------------------------------------------
function DiffHour(time1, time2)
	if time1==nil or time2 == nil then
		return 0
	end
	local nHourSec = 3600
	return math.floor((time2 - time1) / nHourSec)
end

-------------------------------------------------------------------
-- ����2������֮��ķ��Ӳ�
-- @param time1 : ʱ��1����λ��
-- @param time1 : ʱ��2����λ��
-- @return    : ����2��ʱ������ķ��Ӳ�
-------------------------------------------------------------------
function DiffMin(time1, time2)
	if time1==nil or time2 == nil then
		return 0
	end

	local nMinSec = 60
	return  math.floor((time2 - time1) / nMinSec)

end

-------------------------------------------------------------------
-- �ж�һ��ʱ����Ƿ��ڱ���
-- @param time1 : ʱ�䣬��λ��
-------------------------------------------------------------------
function IsCurMonth(time)
    local today = os.date("*t")
    
    local time_date = os.date("*t", time)
    
    return (today.year == time_date.year and today.month == time_date.month)
end

-------------------------------------------------------------------
-- �Ƿ���ͬһ��
-- @param time1 : ʱ��1����λ��
-- @param time1 : ʱ��2����λ��
-- @return    : ����2��ʱ�����������
-------------------------------------------------------------------
function IsSameDay(time1, time2)
    local time_date1 = os.date("*t", time1)
    local time_date2 = os.date("*t", time2)
    
    return (time_date1.year == time_date2.year and time_date1.month == time_date2.month and time_date1.day == time_date2.day)
end

-------------------------------------------------------------------
-- ��������һ����
-- @param fNumber		: Ҫ�����������
-- @param nDecimalDigit	: ��ȷ��С������λ����Ϊ�Ǹ�����Ĭ��Ϊ0
-- @return				: �ɹ�����������������ʧ�ܣ�ԭ��
-- ������RoundNumber(3.1415926, 0) = 3, RoundNumber(3.1415926, 1) = 3.1
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
-- ��ȡһ����������ָ����Χλ��ֵ
-- @param nNumber	: ԭ������
-- @param nBeginBit	: �ӵ�λ����Ŀ�ʼλ
-- @param nBeginBit	: �ӵ�λ����Ľ���λ
-- @return			: �ɹ���ָ����Χλ��ֵ��ʧ�ܣ�ԭ��
-- ���� GetRangeBitValue(88, 4, 5) = 3��GetRangeBitValue(888, 1, 8) = 120
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
	
	-- ��������λȫΪ1������
	local nMask = c_xor(2 ^ (nBeginBit - 1) - 1, 2 ^ nEndBit - 1)
	-- ����������������������λ��Ӧ��λֵ
	local nBitValue = c_and(nNumber, nMask)
	-- �Ƶ����λ����ȡ����ֵ
	nBitValue = c_rshift(nBitValue, nBeginBit - 1)
	
	return nBitValue
end

-------------------------------------------------------------------
-- ����һ����������ָ����Χλ��ֵ
-- @param nNumber	: ԭ������
-- @param nBeginBit	: �ӵ�λ����Ŀ�ʼλ
-- @param nBeginBit	: �ӵ�λ����Ľ���λ
-- @param nValue	: ���õ�ֵ
-- @return			: �ɹ������ú������ֵ��ʧ�ܣ�ԭ��
-- ���� SetRangeBitValue(88, 4, 5, 1) = 72��SetRangeBitValue(888, 1, 8, 8) = 776
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
	
	-- ��������λȫΪ1������
	local nMask = c_xor(2 ^ (nBeginBit - 1) - 1, 2 ^ nEndBit - 1)
	-- ��Ҫ���õ�ֵ�Ƶ�Ҫ���õ�λ��
	nValue = c_lshift(nValue, nBeginBit - 1)
	-- ���õ�ֵ���ܳ�������
	if nValue > nMask then
		return nNumber
	end
	
	-- �Ȱ�Ҫ���õ�λ��0
	nNumber = c_and(nNumber, c_not(nMask))
	nNumber = c_or(nNumber, nValue)
	
	return nNumber
end

-------------------------------------------------------------------
-- ������������������
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
	--ѭ������
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