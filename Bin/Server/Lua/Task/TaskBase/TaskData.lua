-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	11/29/2016
-- ��  ��:	1.0
-- ��  ��:	�������ݶ���
-------------------------------------------------------------------

-- ��Ч��������ֵ(0X7FFFFFFF)
TASK_DATA_INVALID = 2147483647

TASK_ID_MAX_COUNT = 2000			 		-- ����ID�������(������2000,��������,�������10000)
TASK_ID_KEY_OFFSET = 100000			 		-- ����ID����ƫ��ֵ
TASK_DAY_COUNT_OFFSET = 110000       		-- ÿ�մ���ƫ��ֵ
TASK_DAY_TIME_OFFSET = 120000        		-- ѭ������ʱ��ƫ��ֵ
TASK_WEEK_COUNT_OFFSET = 130000      		-- ÿ�ܴ���ƫ��ֵ
TASK_WEEK_TIME_OFFSET = 140000       		-- ÿ�ܴ���ʱ��ƫ��ֵ
TASK_WIN_COUNT_OFFSET = 150000       		-- ʤ������ƫ��ֵ
TASK_FINISH_COUNT_OFFSET = 160000    		-- ��ɴ���ƫ��ֵ
TASK_OPEN_CHEST_COUNT_OFFSET = 170000		-- ���������ƫ��ֵ
TASK_CLAN_DONATE_COUNT_OFFSET = 180000		-- ���˾��״���ƫ��ֵ
TASK_INLAYCROWNGEM_COUNT_OFFSET = 190000	-- ��Ƕ��ʯ����ƫ��ֵ


-- ��������
TASK_CONTACT_HIGH32_ID_KEY_OFFSET = 200000		-- ���������32λID����ƫ��ֵ
TASK_CONTACT_LOW32_ID_KEY_OFFSET  = 210000		-- ���������32λID����ƫ��ֵ
TASK_PUBLISHER_ID_KEY_OFFSET   	  = 220000		-- ������ID����ƫ��ֵ
TASK_CONTACT_APPLYFRIEND_COUNT_OFFSET = 230000	-- ���������Ѵ���
TASK_CONTACT_REPLY_COUNT_OFFSET   = 240000		-- ��������ظ�����
TASK_CONTACT_PK_COUNT_OFFSET	  = 250000		-- ��������PK����

TASK_ACTOR_MAX_COUNT			  = 15			-- ��ҿɽ�������������
TASK_CONTACT_ID_MAX_COUNT 		  = 2			-- ��������ɽ����������
TASK_CYCLE_MAX_COUNT			  = 11			-- ѭ������ɽ����������
TASK_DAILY_MAX_COUNT			  = 1			-- �ճ�����ÿ��ɽ����������

TASK_CONTACT_PUBLISH_MAX_COUNT	  = 2			-- ˢ��ʱ���ڽ�������ɷ����������
TASK_CATCH_MAX_COUNT	  		  = 6			-- ˢ��ʱ��������ɲ����������
TASK_REFLUSH_TIME_INTERVAL  	  = 24			-- ����ˢ��ʱ����
TASK_MANUAL_REFLUSH_MAX_COUNT	  = 2			-- �ֶ�ˢ��������(24Сʱˢ��һ��)	

-- ����ID�������ֵ
TASK_ID_KEY_MAX = TASK_ID_KEY_OFFSET + TASK_ID_MAX_COUNT

-- ������¼��ע��ģ������ظ�ע��
TASK_DATA = {}

-------------------------------------------------------------------
-- ����ʹ������ 1-9999
-------------------------------------------------------------------
TASK_CONTACT_PUBLISH_COUNT_OFFSET = 1		-- ��ǰˢ��ʱ�����ڽ�������ɷ�������ƫ��ֵ
TASK_CATCH_COUNT_OFFSET	  		  = 2		-- ��ǰˢ��ʱ����������ɲ�������ƫ��ֵ
TASK_REFLUSH_TIME_OFFSET  		  = 3		-- �����ϴ�ˢ��ʱ��ƫ��ֵ
TASK_MANUAL_REFLUSH_COUNT_OFFSET  = 4		-- �ֶ�ˢ��ʣ�����


TASK_RANK_REFLUSH_TIME_OFFSET	  = 11		-- ��λ�����ϴ�ˢ��ʱ��ƫ��ֵ

-------------------------------------------------------------------
-- ����ע����������
-- @param holder_id : ��ɫPDBID
-- @param task_key  : ��������
-- @param bIsInt	: �Ƿ�intֵ
-- @param bSave		: �Ƿ�����ݿ�
-------------------------------------------------------------------
function AddDeclareTaskData(holder_id, task_key, bIsInt, bSave)

	if TASK_DATA[holder_id] == nil then
		TASK_DATA[holder_id] = {}
	end
	
	if TASK_DATA[holder_id][task_key] == nil then
        TASK_DATA[holder_id][task_key] = {task_key, bIsInt, bSave}
        DeclareTaskData(holder_id, task_key, bIsInt, bSave)
	end
end

-------------------------------------------------------------------
-- ȡ��ĳ���������������ID��ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetTaskIDKey(holder_id, task_id)

	if holder_id == nil or task_id == nil then
		error("GetTaskIDKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetTaskIDKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_ID_KEY_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�����������ÿ�մ�����ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetDayCountTaskKey(holder_id, task_id)

	if holder_id == nil or task_id == nil then
		error("GetDayCountTaskKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetDayCountTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_DAY_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�����������ѭ������ʱ���ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetDayTimeTaskKey(holder_id, task_id)

	if holder_id == nil or task_id == nil then
		 error("GetDayTimeTaskKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetDayTimeTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_DAY_TIME_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�����������ÿ�ܴ�����ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetWeekCountTaskKey(holder_id, task_id)

	if holder_id == nil or task_id == nil then
		error("GetWeekCountTaskKey error holder_id == nil or task_id == nil")
		return 0
	end
	
	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetWeekCountTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_WEEK_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�����������ÿ�ܴ���ʱ���ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetWeekTimeTaskKey(holder_id, task_id)

	if holder_id == nil or task_id == nil then
		error("GetWeekTimeTaskKey error holder_id == nil or task_id == nil")
		return 0
	end
	
	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetWeekTimeTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_WEEK_TIME_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�����������ʤ��������ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetWinCountTaskKey(holder_id, task_id)

	if holder_id == nil or task_id == nil then
		error("GetWinCountTaskKey error holder_id == nil or task_id == nil")
		return 0
	end
	
	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetWinCountTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_WIN_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�������������ɴ�����ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetFinishCountTaskKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetFinishCountTaskKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetFinishCountTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_FINISH_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ����������Ŀ����������ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetOpenChestCountTaskKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetOpenChestCountTaskKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetOpenChestCountTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_OPEN_CHEST_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ��������������˾��״�����ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetClanDonateCountTaskKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetClanDonateCountTaskKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetClanDonateCountTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_CLAN_DONATE_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�������������Ƕ��ʯ������ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetInlayCrownGemCountTaskKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetInlayCrownGemCountTaskKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetInlayCrownGemCountTaskKey error task_id="..task_id)
		return 0
	end

	local task_key = task_id + TASK_INLAYCROWNGEM_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

------------------------------------------------------------------ �������� -------------------------------------------------------------------------------------

-------------------------------------------------------------------
-- ȡ��ĳ����������Ľ���ID��32λ��ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetContactHigh32IDKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetContactHigh32IDKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetContactHigh32IDKey error task_id = "..task_id)
		return 0
	end

	local task_key = task_id + TASK_CONTACT_HIGH32_ID_KEY_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ����������Ľ���ID��32λ��ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetContactLow32IDKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetContactLow32IDKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetContactLow32IDKey error task_id = "..task_id)
		return 0
	end

	local task_key = task_id + TASK_CONTACT_LOW32_ID_KEY_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ����������ķ�����ID��ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetContactPublisherIDKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetContactPublisherIDKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetContactPublisherIDKey error task_id = "..task_id)
		return 0
	end

	local task_key = task_id + TASK_PUBLISHER_ID_KEY_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ������������������Ѵ�����ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetContactApplyFriendCountKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetContactApplyFriendCountKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetContactApplyFriendCountKey error task_id = "..task_id)
		return 0
	end

	local task_key = task_id + TASK_CONTACT_APPLYFRIEND_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�����������������ظ�������ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetContactReplyCountKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetContactReplyCountKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetContactReplyCountKey error task_id = "..task_id)
		return 0
	end

	local task_key = task_id + TASK_CONTACT_REPLY_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- ȡ��ĳ�����������������PK������ֵ
-- @param task_id : ����ID
-------------------------------------------------------------------
function GetContactPKCountKey(holder_id, task_id)
	
	if holder_id == nil or task_id == nil then
		error("GetContactPKCountKey error holder_id == nil or task_id == nil")
		return 0
	end

	if task_id <= 0 or task_id >= TASK_ID_MAX_COUNT then
		error("GetContactPKCountKey error task_id = "..task_id)
		return 0
	end

	local task_key = task_id + TASK_CONTACT_PK_COUNT_OFFSET
	-- ����ע����������
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

--------------------------------------------------------------------
-- ȡ�õ�ǰˢ��ʱ�����ڽ�������ɷ��������ļ�ֵ
--------------------------------------------------------------------
function GetContactPublishCountKey(holder_id)
	
	if holder_id == nil then
		error("GetContactPublishCountKey error holder_id == nil")
		return 0
	end
 
	-- ����ע����������
	AddDeclareTaskData(holder_id, TASK_CONTACT_PUBLISH_COUNT_OFFSET, true, true)
 
	return TASK_CONTACT_PUBLISH_COUNT_OFFSET
 
end

---------------------------------------------------------------------
-- ȡ�õ�ǰˢ��ʱ����������ɲ��������ļ�ֵ
---------------------------------------------------------------------
function GetTaskCatchCountKey(holder_id)

	if holder_id == nil then
		error("GetTaskCatchCountKey error holder_id == nil")
		return 0
	end

	-- ����ע����������
	AddDeclareTaskData(holder_id, TASK_CATCH_COUNT_OFFSET, true, true)
	
	return TASK_CATCH_COUNT_OFFSET
	
end

---------------------------------------------------------------------
-- ȡ���ϴ�ˢ��ʱ��ļ�ֵ
---------------------------------------------------------------------
function GetReflushTimeKey(holder_id)

	if holder_id == nil then
		error("GetReflushTimeKey error holder_id == nil")
		return 0
	end

	-- ����ע����������
	AddDeclareTaskData(holder_id, TASK_REFLUSH_TIME_OFFSET, true, true)

	return TASK_REFLUSH_TIME_OFFSET
	
end

---------------------------------------------------------------------
-- �ֶ�ˢ��ʣ������ļ�ֵ
---------------------------------------------------------------------
function GetManualReflushCountKey(holder_id)

	if holder_id == nil then
		error("GetManualReflushCountKey error holder_id == nil")
		return 0
	end

	-- ����ע����������
	AddDeclareTaskData(holder_id, TASK_MANUAL_REFLUSH_COUNT_OFFSET, true, true)

	return TASK_MANUAL_REFLUSH_COUNT_OFFSET
	
end

---------------------------------------------------------------------
-- ��λ�����ϴ�ˢ��ʱ��ļ�ֵ
---------------------------------------------------------------------
function GetRankReflushTimeKey(holder_id)

	if holder_id == nil then
		error("GetRankReflushTimeKey error holder_id == nil")
		return 0
	end

	-- ����ע����������
	AddDeclareTaskData(holder_id, TASK_RANK_REFLUSH_TIME_OFFSET, true, true)

	return TASK_RANK_REFLUSH_TIME_OFFSET
	
end
