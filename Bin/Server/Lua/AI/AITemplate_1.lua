-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李建伟
-- 日  期:	11/1/2017
-- 版  本:	1.0
-- 描  述:	AITemplate_1模板实现
-------------------------------------------------------------------

require "AITemplateBase"

AITemplate_1 = AITemplate:new
{
	template_id = 1,
} 

-------------------------------------------------------------------
-- 注册事件 初始化配置
-------------------------------------------------------------------
function AITemplate_1:Init(pAITaskMgr)
	if pAITaskMgr == nil then 
		return
	end
	
	-- 注册出生事件
	pAITaskMgr:luaRegEvent(AI_EVENT_BORN)
	-- 注册死亡事件
	pAITaskMgr:luaRegEvent(AI_EVENT_DIE)
	--注册复活事件
	pAITaskMgr:luaRegEvent(AI_EVENT_REBORN)
	--注册雷达事件
	pAITaskMgr:luaRegEvent(AI_EVENT_RADAR_ALARM)
	--注册收到攻击信号事件
	pAITaskMgr:luaRegEvent(AI_EVENT_ATTACK_SIGNAL)
	--注册队友被攻击事件
	pAITaskMgr:luaRegEvent(AI_EVENT_FRIEND_ENTITY_BEHIT)
	--注册boss被攻击事件
	pAITaskMgr:luaRegEvent(AI_EVENT_BOSS_BEHIT)
	--注册自己被攻击事件
	pAITaskMgr:luaRegEvent(AI_EVENT_BEHIT)
	--注册推塔小龙出现事件
	pAITaskMgr:luaRegEvent(AI_EVENT_SUMMON_SMALL_DRAGON_BORN)
	--注册中团事件
	pAITaskMgr:luaRegEvent(AI_EVENT_ENTER_ALL_MID_STAGE)
	--注册进入敌方视野事件
	pAITaskMgr:luaRegEvent(AI_EVENT_ENTER_ENEMY_SIGHT)
	--注册think事件
	pAITaskMgr:luaRegEvent(AI_EVENT_THINK)
	
	-- 初始化配置
	-- 走真人路径
	local schema = TaskSchema()
	schema.nTaskType = ATT_SIM 
	schema.nActiveType = AI_TASK_ACTIVE_TYPE_EVENT
	schema.nPriority = 2
	schema.nWeight = 100
	schema.nMinHp = 0
	schema.nMaxHp = 999999
	schema.nMinLevel = 0
	schema.nMaxLevel = 99
	schema.nMaxDistance = 10000
	schema.nTimeOut = 120 --s
	schema.nUpdateInterval = 5000 --ms
	schema.nAttackableTarget = HERO_NORMAL_TARGET
	schema.bAllowConfrontState = false
	schema.bThink = false
	pAITaskMgr:set_ATT_SIM_Schema(schema)
	--推线
	schema.nTaskType = ATT_FORWARD_LINE
	schema.nActiveType = AI_TASK_ACTIVE_TYPE_RANDOM
	schema.nPriority = 1
	schema.nWeight = 70
	schema.nMinHp = 0
	schema.nMaxHp = 999999
	schema.nMinLevel = 0
	schema.nMaxLevel = 99
	schema.nMaxDistance = 10000
	schema.nTimeOut = 180 --s
	schema.nUpdateInterval = 3000 --ms
	schema.nAttackableTarget = HERO_NORMAL_TARGET 
	schema.bAllowConfrontState = true
	schema.bThink = true
	pAITaskMgr:set_ATT_FORWARD_LINE_Schema(schema)
	--支援其他线
	schema.nTaskType = ATT_HELP_OTHER_LINE
	schema.nActiveType = AI_TASK_ACTIVE_TYPE_EVENT
	schema.nPriority = 9
	schema.nWeight = 100
	schema.nMinHp = 500
	schema.nMaxHp = 999999
	schema.nMinLevel = 6
	schema.nMaxLevel = 99
	schema.nMaxDistance = 10000
	schema.nTimeOut = 60 --s
	schema.nUpdateInterval = 3000 --ms
	schema.nAttackableTarget = HERO_NORMAL_TARGET
	schema.bAllowConfrontState = true
	schema.bThink = false
	pAITaskMgr:set_ATT_HELP_OTHER_LINE_Schema(schema)
	--跟随
	schema.nTaskType = ATT_FOLLOW
	schema.nActiveType = AI_TASK_ACTIVE_TYPE_EVENT
	schema.nPriority = 10
	schema.nWeight = 100
	schema.nMinHp = 200
	schema.nMaxHp = 999999
	schema.nMinLevel = 0
	schema.nMaxLevel = 99
	schema.nMaxDistance = 10000
	schema.nTimeOut = 8 --s
	schema.nUpdateInterval = 1000 --ms
	schema.nAttackableTarget = HERO_NORMAL_TARGET 
	schema.bAllowConfrontState = false
	schema.bThink = false
	pAITaskMgr:set_ATT_FOLLOW_Schema(schema)
	--回城
	schema.nTaskType = ATT_BACK
	schema.nActiveType = AI_TASK_ACTIVE_TYPE_EVENT
	schema.nPriority = 1
	schema.nWeight = 5
	schema.nMinHp = 0
	schema.nMaxHp = 999999
	schema.nMinLevel = 0
	schema.nMaxLevel = 99
	schema.nMaxDistance = 10000
	schema.nTimeOut = 180 --s
	schema.nUpdateInterval = 3000 --ms
	schema.nAttackableTarget = HERO_NORMAL_TARGET 
	schema.bAllowConfrontState = false
	schema.bThink = false
	pAITaskMgr:set_ATT_BACK_Schema(schema)
	--打野
	schema.nTaskType = ATT_HUNTING
	schema.nActiveType = AI_TASK_ACTIVE_TYPE_RANDOM
	schema.nPriority = 1
	schema.nWeight = 30
	schema.nMinHp = 600
	schema.nMaxHp = 999999
	schema.nMinLevel = 4
	schema.nMaxLevel = 99
	schema.nMaxDistance = 64
	schema.nTimeOut = 180 --s
	schema.nUpdateInterval = 3000 --ms
	schema.nAttackableTarget = HERO_HUNTING_TARGET
	schema.bAllowConfrontState = false
	schema.bThink = true
	pAITaskMgr:set_ATT_HUNTING_Schema(schema)
end