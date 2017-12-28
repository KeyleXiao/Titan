-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	彭政林
-- 日  期:	11/30/2016
-- 版  本:	1.0
-- 描  述:	游戏帮助函数
-------------------------------------------------------------------

-------------------------------------------------------------------
-- 公共帮助函数
-- note : commonHepler 
-------------------------------------------------------------------
function ReadCsvFile(strFileName)
	return commonHepler.readCsvFile(strFileName)
end

function GetTickCountEx()
	return commonHepler.getTickCountEx()
end

function GetSyncTick()
	return commonHepler.getSyncTick()
end

-------------------------------------------------------------------
-- 任务帮助函数
-- note : taskHepler 
-------------------------------------------------------------------
function SetTaskIntData(dwDBID, nKey, nValue)

	-- 默认注册
	AddDeclareTaskData(dwDBID, nKey, true, true)

	return taskHepler.setTaskIntData(dwDBID, nKey, nValue)
end

function GetTaskIntData(dwDBID, nKey)
	
	-- 默认注册
	AddDeclareTaskData(dwDBID, nKey, true, true)

	return taskHepler.getTaskIntData(dwDBID, nKey)
end

function RemoveTaskData(dwDBID, nKey)
	return taskHepler.removeTaskData(dwDBID, nKey)
end

function DeclareTaskData(dwDBID, nKey, bIsInt, bSave)
	return taskHepler.declareTaskData(dwDBID, nKey, bIsInt, bSave)
end

-------------------------------------------------------------------
-- 触发器帮助函数
-- note : triggerHepler 
-------------------------------------------------------------------
function DeleteTrigger(nTriggerID)
	return triggerHepler.deleteTrigger(nTriggerID)
end

function CreateTimerTrigger(nSceneID, szCallFunc, nInterval, nTimes)
	return triggerHepler.createTimerTrigger(nSceneID, szCallFunc, nInterval, nTimes)
end

function CreateDateTrigger(nSceneID, szCallFunc, nYear, nMonth, nDay, nHour, nMinute, nWDay)
	return triggerHepler.createDateTrigger(nSceneID, szCallFunc, nYear, nMonth, nDay, nHour, nMinute, nWDay)
end

function CreateLoginTrigger(nSceneID, szCallFunc)
	return triggerHepler.createLoginTrigger(nSceneID, szCallFunc)
end

function CreateWarPersonTaskTrigger(nSceneID, szCallFunc)
	return triggerHepler.createWarPersonTaskTrigger(nSceneID, szCallFunc)
end

function CreateNPCTrigger(nSceneID, szCallFunc)
	return triggerHepler.createNPCTrigger(nSceneID, szCallFunc)
end

function CreateNPCFuncTrigger(nSceneID, szCallFunc)
	return triggerHepler.createNPCFuncTrigger(nSceneID, szCallFunc)
end

function CreateActorTaskTrigger(nSceneID, szCallFunc)
	return triggerHepler.createActorTaskTrigger(nSceneID, szCallFunc)
end

-------------------------------------------------------------------
-- 游戏帮助函数
-- note : gameHepler 
-------------------------------------------------------------------
function SendTaskList(dwDBID, strTask)
	gameHepler.sendTaskList(dwDBID, strTask)
end

function UpdateTaskList(dwDBID, strTask)
	gameHepler.updateTaskList(dwDBID, strTask)
end

function DeleteTaskList(dwDBID, strTask)
	gameHepler.deleteTaskList(dwDBID, strTask)
end

function SendTaskPrize(dwDBID, strTask)
	gameHepler.sendTaskPrize(dwDBID, strTask)
end

function AddExp(holder_id, nExp, pszRemark)
	gameHepler.addExp(holder_id, nExp, pszRemark)
end

function SetActorBaseExp(holder_id, lgValue, szLogMark)
	gameHepler.setActorBaseExp(holder_id, lgValue, szLogMark)
end

function CanAddMoney(holder_id, nTicket, nGlod)
	return gameHepler.canAddMoney(holder_id, nTicket, nGlod)
end

function AddMoney(holder_id, nTicket, nGlod, pszRemark)
	gameHepler.addMoney(holder_id, nTicket, nGlod, pszRemark)
end

function CanPrizeActor(holder_id, nPrizeID, nNum)
	return gameHepler.canPrizeActor(holder_id, nPrizeID, nNum)
end

function PrizeActor(holder_id, nPrizeID, nNum, dwTargetID, szLogDesc)
	return gameHepler.prizeActor(holder_id, nPrizeID, nNum, dwTargetID, szLogDesc)
end

function ShowSystemMessage(holder_id, nTipID, szParam)
	return gameHepler.showSystemMessage(holder_id, nTipID, szParam)
end

function AddTaskGameNoteLog(holder_id, szParam)
	return gameHepler.addTaskGameNoteLog(holder_id, szParam)
end

function GetPlayerLevel(holder_id)
	return gameHepler.getPlayerLevel(holder_id)
end

function GetPlayerClanID(holder_id)
	return gameHepler.getPlayerClanID(holder_id)
end

function GetPlayerMatchCount(holder_id)
	return gameHepler.getPlayerMatchCount(holder_id)
end

function GetPlayerHeroCount(holder_id)
	return gameHepler.getPlayerHeroCount(holder_id)
end

function GetPlayerSkinCount(holder_id)
	return gameHepler.getPlayerSkinCount(holder_id)
end

function GetPlayerMasterCount(holder_id)
	return gameHepler.getPlayerMasterCount(holder_id)
end

function GetPlayerPrenticeCount(holder_id)
	return gameHepler.getPlayerPrenticeCount(holder_id)
end

function GetPlayerHeroCountByLevel(holder_id, nLevel)
	return gameHepler.getPlayerHeroCountByLevel(holder_id, nLevel)
end

function GetPlayerKillBigDragoCount(holder_id)
	return gameHepler.getPlayerKillBigDragoCount(holder_id)
end

function GetPlayerKillSmallDragoCount(holder_id)
	return gameHepler.getPlayerKillSmallDragoCount(holder_id)
end

function GetPlayerWarUseWringCount(holder_id)
	return gameHepler.getPlayerWarUseWringCount(holder_id)
end

function GetPlayerWarUseXPCount(holder_id)
	return gameHepler.getPlayerWarUseXPCount(holder_id)
end

function GetPlayerGuideStep(holder_id)
	return gameHepler.getPlayerGuideStep(holder_id)
end

function OssMonitorTask(task_id, nAccecptNum, nFinishNum, nGiveUpNum)
	return gameHepler.OssMonitorTask(task_id, nAccecptNum, nFinishNum, nGiveUpNum)
end

function OnCheckAddContactTask(holder_id, sub_type, specific_type, tagid_high32, tagid_low32, emotion_words, nResult)
	return gameHepler.onCheckAddContactTask(holder_id, sub_type, specific_type, tagid_high32, tagid_low32, emotion_words, nResult)
end

function SendAddTaskSuccess(holder_id, strTask)
	return gameHepler.sendAddTaskSuccess(holder_id, strTask)
end

function SendPreCatchTaskResult(holder_id, strTask)
	return gameHepler.sendPreCatchTaskResult(holder_id, strTask)
end

function SendCatchTaskResult(holder_id, strTask)
	return gameHepler.sendCatchTaskResult(holder_id, strTask)
end

function SendReflushOptInfo(holder_id, strTask)
	return gameHepler.sendReflushOptInfo(holder_id, strTask)
end

function CheckAddMoneyOfReflushOpt(holder_id)
	return gameHepler.checkAddMoneyOfReflushOpt(holder_id)
end

function CheckIsPlayerPrentice(holder_id, target_id)
	return gameHepler.checkIsPlayerPrentice(holder_id, target_id)
end

function SendPlayerMail(holder_id, nGold, nExp, nPrizeID1, nPrizeID2)
	return gameHepler.sendPlayerMail(holder_id, nGold, nExp, nPrizeID1, nPrizeID2)
end

-------------------------------------------------------------------
-- npc帮助函数
-- note : npcHelper
-------------------------------------------------------------------
function OpenWarShop(actor, uidNPC)
	return npcHelper.openWarShop(actor, uidNPC)
end

function ResponseNPC(actor, uidNPC, strRes)
	return npcHelper.responseNPC(actor, uidNPC, strRes)
end

function VerifyLocation(actor, uidNPC, radii)
	return npcHelper.verifyLocation(actor, uidNPC, radii)
end

function GetTelentPoint(actor)
	return npcHelper.getTelentPoint(actor)
end

function SetTelentPoint(actor, nTelnetPoint)
	return npcHelper.setTelentPoint(actor, nTelnetPoint)
end

function AddBuff(actor, buff_id, buff_lv, add_id, buff_time, context, context_len)
	return npcHelper.addBuff(actor, buff_id, buff_lv, add_id, buff_time, context, context_len)
end

function CloseNPCDialogBox(actor)
	return npcHelper.closeNPCDialogBox(actor)
end

function SetActorWarUseWringFlag(actor)
	return npcHelper.setActorWarUseWringFlag(actor)
end

-------------------------------------------------------------------
-- war帮助函数
-- note : warHelper
-------------------------------------------------------------------
function cureMember(actor)
	return warHelper.cureMember(actor)
end

function luaBuyGoods(pdbid, goods_id, goods_cost)
	return warHelper.luaBuyGoods(pdbid, goods_id, goods_cost)
end

function NPCGoodsBuySuccess(pdbid, npc, npc_id, goods_id, now)
	return warHelper.NPCGoodsBuySuccess(pdbid, npc, npc_id, goods_id, now)
end