-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	李建伟
-- 日  期:	11/1/2017
-- 版  本:	1.0
-- 描  述:	AI模板基类
-------------------------------------------------------------------

AITemplate = Object:new
{
	template_id = -1,
	tEventHandler = {},
}

-------------------------------------------------------------------
-- 构造函数
-------------------------------------------------------------------
function AITemplate:construct()
	self.tEventHandler[AI_EVENT_BORN] = self.OnEventBorn 
	self.tEventHandler[AI_EVENT_DIE] = self.OnEventDie
	self.tEventHandler[AI_EVENT_REBORN] = self.OnEventReborn
	self.tEventHandler[AI_EVENT_RADAR_ALARM] = self.OnEventRadarAlarm
	self.tEventHandler[AI_EVENT_ATTACK_SIGNAL] = self.OnEventAttackSignal
	self.tEventHandler[AI_EVENT_FRIEND_ENTITY_BEHIT] = self.OnEventFriendBeHit
	self.tEventHandler[AI_EVENT_BOSS_BEHIT] = self.OnEventBossBeHit
	self.tEventHandler[AI_EVENT_BEHIT] = self.OnEventbeHit
	self.tEventHandler[AI_EVENT_SUMMON_SMALL_DRAGON_BORN] = self.OnEventSummonSmallDragonBorn
	self.tEventHandler[AI_EVENT_ENTER_ALL_MID_STAGE] = self.OnEventMid
	self.tEventHandler[AI_EVENT_ENTER_ENEMY_SIGHT] = self.OnEventEnterEnemySight	
	self.tEventHandler[AI_EVENT_THINK] = self.OnEventThink
end

-------------------------------------------------------------------
-- 事件处理
-------------------------------------------------------------------
function AITemplate:OnEvent(pAITaskMgr, pAIContext, loc)
	local eventHandler = self.tEventHandler[pAIContext.nEventID]
	
	if eventHandler == nil then 
		error("ai template = " .. template_id .. " onEvent event handler not register, eventid=" .. pAIContext.nEventID)   
		return
	end
	
	eventHandler(self, pAITaskMgr, pAIContext, loc)
end

-----------------------------具体模板选择重写下面的函数------------

-------------------------------------------------------------------
-- 初始化:注册事件 初始化配置
-------------------------------------------------------------------
function AITemplate:Init(pAITaskMgr) 
	warning("template should implement self's SubscribeEvent function, template_id=" .. template_id)
end

-------------------------------------------------------------------
-- 出生事件处理
-------------------------------------------------------------------
function AITemplate:OnEventBorn(pAITaskMgr, pAIContext, loc)
	local pAIProxy = pAIContext.pAIProxy
	if pAIProxy == nil then 
		error("OnEventBorn pAIProxy is nil")  
		return
	end
	
	if pAIProxy:hasSimData() then 
		pAITaskMgr:addTask(ATT_SIM, 0, -1, 0, -1)
	else
		pAITaskMgr:execTaskStack()
	end
end  

-------------------------------------------------------------------
-- 处理死亡事件
-------------------------------------------------------------------
function AITemplate:OnEventDie(pAITaskMgr, pAIContext, loc)
	pAITaskMgr:suspend()
end

-------------------------------------------------------------------
-- 处理复活事件
-------------------------------------------------------------------
function AITemplate:OnEventReborn(pAITaskMgr, pAIContext, loc)
	pAITaskMgr:execTaskStack()
end

-------------------------------------------------------------------
-- 处理报警事件
-------------------------------------------------------------------
function AITemplate:OnEventRadarAlarm(pAITaskMgr, pAIContext, loc)
	local pAIProxy = pAIContext.pAIProxy
	if pAIProxy == nil then 
		error("OnEventRadarAlarm pAIProxy is nil")  
		return
	end
	
	local pSelf = pAIProxy:getSelfSnapshot()
	if pSelf == nil then 
		error("OnEventRadarAlarm pSelf is nil")
	end
	
	local function getLocDistance(loc1, loc2)
		local deltaX = loc1.x - loc2.x
		local deltaZ = loc1.z - loc2.z
		
		return math.sqrt(deltaX * deltaX + deltaZ * deltaZ)
	end
	
	if pAIProxy:ackRadar(pAIContext.uidOperator, pAIContext.nParam1, pAIContext.nParam2 == 1, pAIContext.nParam3) then
		-- 自己在高地外 直接回城
		if getLocDistance(pSelf.m_Loc, pAIProxy.m_ptBornLoc) > 128 and getLocDistance(loc, pAIProxy.m_ptBornLoc) < 96 then 
			pAITaskMgr:addTask(ATT_BACK, eBackReason_Defend, -1, 0, 9) -- 优先级为支援其他线的优先级
			return
		end
		
		-- 支援其他线
		local pTarget = pAIProxy:getTargetSnapshot(pAIContext.uidOperator)
		if pTarget == nil then 
			return
		end
		
		pAITaskMgr:addTask(ATT_HELP_OTHER_LINE, pTarget.m_PathID, pAIContext.nParam3 / 1000, 0, -1)
	end
end

-------------------------------------------------------------------
-- 处理进攻信号事件
-------------------------------------------------------------------
function AITemplate:OnEventAttackSignal(pAITaskMgr, pAIContext, loc)
	local pAIProxy = pAIContext.pAIProxy
	if pAIProxy == nil then 
		error("OnEventAttackSignal pAIProxy is nil")  
		return
	end
	
	local pTarget = pAIProxy:getTargetSnapshot(pAIContext.uidTarget)
	if pTarget == nil then 
		warning("OnEventAttackSignal target is nil")
		return
	end
	
	local nAIType = toAIEntityType(pTarget.m_uid, pTarget.m_nVocation)
	
	local nCustomPriority = -1
	if nAIType == AI_ENTITY_TYPE_SOLDIER or nAIType == AI_ENTITY_TYPE_WILD_MONSTER then 
		-- 协助进攻小兵或者野怪 优先级不要超过支援防御塔
		nCustomPriority = 7
	end
	pAITaskMgr:addTask(ATT_FOLLOW, pAIContext.uidTarget, -1, aiEntityTypeMask(nAIType), nCustomPriority)
end

-------------------------------------------------------------------
-- 处理队友被攻击事件
-------------------------------------------------------------------
function AITemplate:OnEventFriendBeHit(pAITaskMgr, pAIContext, loc)
	local pAIProxy = pAIContext.pAIProxy
	if pAIProxy == nil then 
		error("OnEventFriendBeHit pAIProxy is nil")  
		return
	end
	
	if toAIEntityType(pAIContext.uidTarget, 0) ~= AI_ENTITY_TYPE_HERO then 
		return
	end
	
	local pOperator = pAIProxy:getTargetSnapshot(pAIContext.uidOperator)
	if pOperator == nil then 
		return
	end
	
	-- 操作者是防御塔 不需要去帮忙
	local nAttackerType = toAIEntityType(pOperator.m_uid, pOperator.m_nVocation)
	local nAttackerMask = aiEntityTypeMask(nAttackerType) 
	if nAttackerMask == AI_ENTITY_TYPE_DEFENSIVE_TOWER_M then 
		return
	end
	
	local nCustomPriority = -1
	
	if nAttackerType == AI_ENTITY_TYPE_SOLDIER or nAttackerType == AI_ENTITY_TYPE_WILD_MONSTER then 
		-- 协助进攻小兵或者野怪 优先级不要超过支援防御塔
		nCustomPriority = 7
	end
	
	pAITaskMgr:addTask(ATT_FOLLOW, pAIContext.uidTarget, -1, nAttackerMask, nCustomPriority)
end

-------------------------------------------------------------------
-- 处理boss被攻击事件
-------------------------------------------------------------------
function AITemplate:OnEventBossBeHit(pAITaskMgr, pAIContext, loc)
	local pAIProxy = pAIContext.pAIProxy
	if pAIProxy == nil then 
		error("OnEventBossBeHit pAIProxy is nil")  
		return
	end
	
	local pTarget = pAIProxy:getTargetSnapshot(pAIContext.uidTarget)
	if pTarget == nil then 
		return
	end
	
	pAITaskMgr:addTask(ATT_FOLLOW, pAIContext.uidTarget, 30, aiEntityTypeMask(toAIEntityType(pTarget.m_uid, pTarget.m_nVocation)), -1)
end

-------------------------------------------------------------------
-- 处理自己被攻击事件
-------------------------------------------------------------------
function AITemplate:OnEventbeHit(pAITaskMgr, pAIContext, loc)
	local pAIProxy = pAIContext.pAIProxy
	if pAIProxy == nil then 
		error("OnEventbeHit pAIProxy is nil")  
		return
	end
	
	local pSelf = pAIProxy:getSelfSnapshot()
	if pSelf == nil then 
		return
	end
	
	if pSelf.m_nCurHp < pSelf.m_nDandgerHp then 
		pAITaskMgr:addTask(ATT_BACK, eBackReason_Recover, -1, 0, TASK_PRIORITY_HIGHEST)
	elseif pSelf.m_nCurHp / pSelf.m_nMaxHp < 0.4 then
		pAITaskMgr:addTask(ATT_BACK, eBackReason_Recover, -1, 0, -1)
	end
end

-------------------------------------------------------------------
-- 处理推塔小龙出现事件
-------------------------------------------------------------------
function AITemplate:OnEventSummonSmallDragonBorn(pAITaskMgr, pAIContext, loc)
	pAITaskMgr:addTask(ATT_FOLLOW, pAIContext.uidTarget, 1800, AI_ENTITY_TYPE_SUMMON_SMALL_DRAGON_M, TASK_PRIORITY_HIGHEST)
end

-------------------------------------------------------------------
-- 中团阶段
-------------------------------------------------------------------
function AITemplate:OnEventMid(pAITaskMgr, pAIContext, loc)
	local schema = TaskSchema()
	schema.nTaskType = ATT_FORWARD_LINE
	schema.nActiveType = AI_TASK_ACTIVE_TYPE_RANDOM
	schema.nPriority = 1
	schema.nWeight = 100 -- 修改推线权重
	schema.nMinHp = 0
	schema.nMaxHp = 999999
	schema.nMinLevel = 0
	schema.nMaxLevel = 99
	schema.nMaxDistance = 10000
	schema.nTimeOut = 180 --s
	schema.nUpdateInterval = 3000 --ms
	schema.nAttackableTarget = HERO_NORMAL_TARGET 
	schema.bAllowConfrontState = true
	pAITaskMgr:set_ATT_FORWARD_LINE_Schema(schema)
	
	schema.nTaskType = ATT_HUNTING
	schema.nActiveType = AI_TASK_ACTIVE_TYPE_RANDOM
	schema.nPriority = 1
	schema.nWeight = 1  -- 修改打野权重
	schema.nMinHp = 600
	schema.nMaxHp = 999999
	schema.nMinLevel = 4
	schema.nMaxLevel = 99
	schema.nMaxDistance = 64
	schema.nTimeOut = 180 --s
	schema.nUpdateInterval = 3000 --ms
	schema.nAttackableTarget = HERO_HUNTING_TARGET
	schema.bAllowConfrontState = false
	pAITaskMgr:set_ATT_HUNTING_Schema(schema)
end

-------------------------------------------------------------------
-- 进入敌方视野
-------------------------------------------------------------------
function AITemplate:OnEventEnterEnemySight(pAITaskMgr, pAIContext, loc)
	pAITaskMgr:activeFsController()
end


-------------------------------------------------------------------
-- think
-------------------------------------------------------------------
function AITemplate:OnEventThink(pAITaskMgr, pAIContext, loc)	
	local pAIProxy = pAIContext.pAIProxy
	if pAIProxy == nil then 
		error("OnEventbeHit pAIProxy is nil")  
		return
	end
	
	local pSelf = pAIProxy:getSelfSnapshot()
	if pSelf == nil then 
		return
	end
	
	local minLevel = 10
	if pSelf.m_nLevel < minLevel then 
		return
	end
	
	local minHp = 700
	if pSelf.m_nCurHp < minHp then 
		return
	end
	
	-- 是否打小龙
	local uidSmallDragon = CAITargetHelper.getInstance():getTarget(pAIContext, AI_TARGET_NEUTRAL_SMALL_DRAGON, false)
	local pSmallDragon = pAIProxy:getTargetSnapshot(uidSmallDragon, true)
	if pSmallDragon ~= nil then
		if pAITaskMgr:addTask(ATT_FOLLOW, uidSmallDragon, 120, aiEntityTypeMask(toAIEntityType(pSmallDragon.m_uid, pSmallDragon.m_nVocation)), -1) then 
			-- 300米内请求支援
			pAIProxy:broadcastAttackSignal(uidSmallDragon, 300)
			return
		end
	end
	
	-- 是否打大龙
	local uidHugeDragon = CAITargetHelper.getInstance():getTarget(pAIContext, AI_TARGET_NEUTRAL_HUGE_DRAGON, false) 
	local pHugeDragon = pAIProxy:getTargetSnapshot(uidHugeDragon, true) 
	if pHugeDragon ~= nil then
		if pSelf.m_nLevel >= 16 and pSelf.m_nCurHp >= 1000 then
			if pAITaskMgr:addTask(ATT_FOLLOW, uidHugeDragon, 120, aiEntityTypeMask(toAIEntityType(pHugeDragon.m_uid, pHugeDragon.m_nVocation)), -1) then 
				-- 300米内请求支援
				pAIProxy:broadcastAttackSignal(uidHugeDragon, 300)
				return
			end
		end
	end
end
