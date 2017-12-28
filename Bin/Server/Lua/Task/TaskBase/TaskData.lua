-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/29/2016
-- 版  本:	1.0
-- 描  述:	任务数据定义
-------------------------------------------------------------------

-- 无效任务数据值(0X7FFFFFFF)
TASK_DATA_INVALID = 2147483647

TASK_ID_MAX_COUNT = 2000			 		-- 任务ID最大数量(先设置2000,不够再扩,最大扩到10000)
TASK_ID_KEY_OFFSET = 100000			 		-- 任务ID数据偏移值
TASK_DAY_COUNT_OFFSET = 110000       		-- 每日次数偏移值
TASK_DAY_TIME_OFFSET = 120000        		-- 循环次数时间偏移值
TASK_WEEK_COUNT_OFFSET = 130000      		-- 每周次数偏移值
TASK_WEEK_TIME_OFFSET = 140000       		-- 每周次数时间偏移值
TASK_WIN_COUNT_OFFSET = 150000       		-- 胜利次数偏移值
TASK_FINISH_COUNT_OFFSET = 160000    		-- 完成次数偏移值
TASK_OPEN_CHEST_COUNT_OFFSET = 170000		-- 开宝箱个数偏移值
TASK_CLAN_DONATE_COUNT_OFFSET = 180000		-- 联盟捐献次数偏移值
TASK_INLAYCROWNGEM_COUNT_OFFSET = 190000	-- 镶嵌宝石次数偏移值


-- 交互任务
TASK_CONTACT_HIGH32_ID_KEY_OFFSET = 200000		-- 交互任务高32位ID数据偏移值
TASK_CONTACT_LOW32_ID_KEY_OFFSET  = 210000		-- 交互任务低32位ID数据偏移值
TASK_PUBLISHER_ID_KEY_OFFSET   	  = 220000		-- 发布者ID数据偏移值
TASK_CONTACT_APPLYFRIEND_COUNT_OFFSET = 230000	-- 交互任务交友次数
TASK_CONTACT_REPLY_COUNT_OFFSET   = 240000		-- 交互任务回复次数
TASK_CONTACT_PK_COUNT_OFFSET	  = 250000		-- 交互任务PK次数

TASK_ACTOR_MAX_COUNT			  = 15			-- 玩家可接受任务总数量
TASK_CONTACT_ID_MAX_COUNT 		  = 2			-- 交互任务可接受最大数量
TASK_CYCLE_MAX_COUNT			  = 11			-- 循环任务可接受最大数量
TASK_DAILY_MAX_COUNT			  = 1			-- 日常任务每天可接受最大数量

TASK_CONTACT_PUBLISH_MAX_COUNT	  = 2			-- 刷新时间内交互任务可发布最大数量
TASK_CATCH_MAX_COUNT	  		  = 6			-- 刷新时间内任务可捕获最大数量
TASK_REFLUSH_TIME_INTERVAL  	  = 24			-- 任务刷新时间间隔
TASK_MANUAL_REFLUSH_MAX_COUNT	  = 2			-- 手动刷新最大次数(24小时刷新一次)	

-- 任务ID数据最大值
TASK_ID_KEY_MAX = TASK_ID_KEY_OFFSET + TASK_ID_MAX_COUNT

-- 用来记录已注册的，避免重复注册
TASK_DATA = {}

-------------------------------------------------------------------
-- 任务使用数据 1-9999
-------------------------------------------------------------------
TASK_CONTACT_PUBLISH_COUNT_OFFSET = 1		-- 当前刷新时间间隔内交互任务可发布数量偏移值
TASK_CATCH_COUNT_OFFSET	  		  = 2		-- 当前刷新时间间隔内任务可捕获数量偏移值
TASK_REFLUSH_TIME_OFFSET  		  = 3		-- 任务上次刷新时间偏移值
TASK_MANUAL_REFLUSH_COUNT_OFFSET  = 4		-- 手动刷新剩余次数


TASK_RANK_REFLUSH_TIME_OFFSET	  = 11		-- 排位任务上次刷新时间偏移值

-------------------------------------------------------------------
-- 增加注册任务数据
-- @param holder_id : 角色PDBID
-- @param task_key  : 任务数据
-- @param bIsInt	: 是否int值
-- @param bSave		: 是否存数据库
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
-- 取得某个任务关联的任务ID键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的每日次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的循环次数时间键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的每周次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的每周次数时间键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的胜利次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的完成次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的开宝箱个数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的联盟捐献次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的镶嵌宝石次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

------------------------------------------------------------------ 交互任务 -------------------------------------------------------------------------------------

-------------------------------------------------------------------
-- 取得某个任务关联的交互ID高32位键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的交互ID低32位键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联的发布者ID键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联交互任务交友次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联交互任务回复次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

-------------------------------------------------------------------
-- 取得某个任务关联交互任务PK次数键值
-- @param task_id : 任务ID
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
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, task_key, true, true)

	return task_key
end

--------------------------------------------------------------------
-- 取得当前刷新时间间隔内交互任务可发布数量的键值
--------------------------------------------------------------------
function GetContactPublishCountKey(holder_id)
	
	if holder_id == nil then
		error("GetContactPublishCountKey error holder_id == nil")
		return 0
	end
 
	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, TASK_CONTACT_PUBLISH_COUNT_OFFSET, true, true)
 
	return TASK_CONTACT_PUBLISH_COUNT_OFFSET
 
end

---------------------------------------------------------------------
-- 取得当前刷新时间间隔内任务可捕获数量的键值
---------------------------------------------------------------------
function GetTaskCatchCountKey(holder_id)

	if holder_id == nil then
		error("GetTaskCatchCountKey error holder_id == nil")
		return 0
	end

	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, TASK_CATCH_COUNT_OFFSET, true, true)
	
	return TASK_CATCH_COUNT_OFFSET
	
end

---------------------------------------------------------------------
-- 取得上次刷新时间的键值
---------------------------------------------------------------------
function GetReflushTimeKey(holder_id)

	if holder_id == nil then
		error("GetReflushTimeKey error holder_id == nil")
		return 0
	end

	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, TASK_REFLUSH_TIME_OFFSET, true, true)

	return TASK_REFLUSH_TIME_OFFSET
	
end

---------------------------------------------------------------------
-- 手动刷新剩余次数的键值
---------------------------------------------------------------------
function GetManualReflushCountKey(holder_id)

	if holder_id == nil then
		error("GetManualReflushCountKey error holder_id == nil")
		return 0
	end

	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, TASK_MANUAL_REFLUSH_COUNT_OFFSET, true, true)

	return TASK_MANUAL_REFLUSH_COUNT_OFFSET
	
end

---------------------------------------------------------------------
-- 排位任务上次刷新时间的键值
---------------------------------------------------------------------
function GetRankReflushTimeKey(holder_id)

	if holder_id == nil then
		error("GetRankReflushTimeKey error holder_id == nil")
		return 0
	end

	-- 增加注册任务数据
	AddDeclareTaskData(holder_id, TASK_RANK_REFLUSH_TIME_OFFSET, true, true)

	return TASK_RANK_REFLUSH_TIME_OFFSET
	
end
