/*******************************************************************
** 文件名:	API_Wrapper.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			场景服API封装
********************************************************************/

#include "stdafx.h"
 
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge/LuaBridge.h"
#include "ILuaEngine.h"

#include "CommonHelper.h"
#include "TriggerHelper.h"
#include "TaskHelper.h"
#include "NPCHelper.h"
#include "SceneServerAPI.h"

using namespace  luabridge;

extern "C" __declspec(dllexport) bool BindLuaAPI(ILuaEngine * pLua)
{
	lua_State * L = pLua->GetLuaState();

    getGlobalNamespace (L)
        .beginNamespace ("commonHepler")
        .addCFunction ("readCsvFile", &readCsvFile)
        .addFunction ("getTickCountEx", &getTickCountEx)
        .addFunction("getSyncTick", &getSyncTick)
    .endNamespace ();

    getGlobalNamespace (L)
        .beginNamespace ("triggerHepler")
        .addFunction ("deleteTrigger", &deleteTrigger)
        .addFunction ("createTimerTrigger", &createTimerTrigger)
        .addFunction ("createDateTrigger", &createDateTrigger)
        .addFunction ("createLoginTrigger", &createLoginTrigger)
        .addFunction ("createWarPersonTaskTrigger", &createWarPersonTaskTrigger)
        .addFunction ("createNPCTrigger", &createNPCTrigger)
        .addFunction ("createNPCFuncTrigger", &createNPCFuncTrigger)
		.addFunction("createActorTaskTrigger", &createActorTaskTrigger)
        .endNamespace ();

    getGlobalNamespace (L)
		.beginNamespace ("taskHepler")
		.addFunction ("setTaskIntData", &setTaskIntData)
		.addFunction ("getTaskIntData", &getTaskIntData)
		.addFunction ("removeTaskData", &removeTaskData)
		.addFunction ("declareTaskData", &declareTaskData)
    .endNamespace ();

	getGlobalNamespace (L)
	.beginNamespace ("gameHepler")
		.addFunction ("createNPC", &createNPC)
        .addFunction("destroyNPC", &destroyNPC)
        .addFunction ("sendTaskList", &sendTaskList)
        .addFunction ("updateTaskList", &updateTaskList)
        .addFunction ("deleteTaskList", &deleteTaskList)
        .addFunction ("sendTaskPrize", &sendTaskPrize)
        .addFunction ("addExp", &addExp)
        .addFunction ("setActorBaseExp", &setActorBaseExp)
        .addFunction ("canAddMoney", &canAddMoney)
        .addFunction ("addMoney", &addMoney)
        .addFunction ("canPrizeActor", &canPrizeActor)
        .addFunction ("prizeActor", &prizeActor)
        .addFunction ("showSystemMessage", &showSystemMessage)
        .addFunction ("addTaskGameNoteLog", &addTaskGameNoteLog)
		.addFunction("getPlayerLevel", &getPlayerLevel)
		.addFunction("getPlayerClanID", &getPlayerClanID)
		.addFunction("getPlayerMatchCount", &getPlayerMatchCount)
		.addFunction("getPlayerHeroCount", &getPlayerHeroCount)
		.addFunction("getPlayerSkinCount", &getPlayerSkinCount)
		.addFunction("getPlayerMasterCount", &getPlayerMasterCount)
		.addFunction("getPlayerPrenticeCount", &getPlayerPrenticeCount)
		.addFunction("getPlayerHeroCountByLevel", &getPlayerHeroCountByLevel)
		.addFunction("getPlayerKillBigDragoCount", &getPlayerKillBigDragoCount)
		.addFunction("getPlayerKillSmallDragoCount", &getPlayerKillSmallDragoCount)
		.addFunction("getPlayerWarUseWringCount", &getPlayerWarUseWringCount)
		.addFunction("getPlayerWarUseXPCount", &getPlayerWarUseXPCount)
        .addFunction("getPlayerGuideStep", &getPlayerGuideStep)
		.addFunction("OssMonitorTask", &OssMonitorTask)
		.addFunction("onCheckAddContactTask", &onCheckAddContactTask)
		.addFunction("sendAddTaskSuccess", &sendAddTaskSuccess)
		.addFunction("sendPreCatchTaskResult", &sendPreCatchTaskResult)
		.addFunction("sendCatchTaskResult", &sendCatchTaskResult)
		.addFunction("sendReflushOptInfo", &sendReflushOptInfo)
		.addFunction("checkAddMoneyOfReflushOpt", &checkAddMoneyOfReflushOpt)
		.addFunction("checkIsPlayerPrentice", &checkIsPlayerPrentice)
		.addFunction("sendPlayerMail", &sendPlayerMail)
	.endNamespace ();

	getGlobalNamespace (L)
	.beginNamespace("npcHelper")
		.addFunction("openWarShop", &openWarShop)
		.addFunction("responseNPC", &responseNPC)
		.addFunction("verifyLocation", &verifyLocation)
		.addFunction("getTelentPoint", &getTelentPoint)
		.addFunction("setTelentPoint", &setTelentPoint)
		.addFunction("addBuff", &addBuff)
		.addFunction("closeNPCDialogBox", &closeNPCDialogBox)
		.addFunction("setActorWarUseWringFlag", &setActorWarUseWringFlag)
	.endNamespace();

	getGlobalNamespace(L)
		.beginNamespace("warHelper")
		.addFunction("cureMember", &cureMember)
        .addFunction("luaBuyGoods", &luaBuyGoods)
        .addFunction("NPCGoodsBuySuccess", &NPCGoodsBuySuccess)
		.endNamespace();

	return true;
}
