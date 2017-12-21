/*******************************************************************
** 文件名:	NPCHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/1/18
** 版  本:	1.0
** 描  述:	NPC数据帮助函数
********************************************************************/

#include "stdafx.h"
#include "NPCHelper.h"
#include "EntityHelper.h"
#include "OssLogDef.h"
#include "TaskStorageDef.h"
#include "IResourceManager.h"
#include "WarHelper.h"

/**
@name		:填充NPC下发给客户端的消息头
@ob			:发送缓冲区
@nMsgID		:消息ID
*/
void fillNPCHead( obuf& ob, UID uidMaster, int nMsgID )
{
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule	= MSG_MODULEID_ENTITY;
	head.byKeyAction	= nMsgID;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = uidMaster;
	entityHead.byPartID = PART_COMMON;

	ob << head << entityHead;
}


/**
@name			 : 打开商店
@param uidActor  : 玩家UID
@brief dwNPC	 : NPC
@return			 : 是否设置成功
*/
MY_LUA_API bool openWarShop(PDBID pdbid, UID uidNPC)
{
	EmphasisLn("OpenShop success.pdbid=" << pdbid << "uidNPC=" << uidNPC);
	msg_entity_npc_open_shop msg;
	msg.uidNPC = uidNPC;

	ActorHelper helper(pdbid, ACTOR_DBID);
	IActorService* pActorService = helper.m_ptr;
	if( pActorService == NULL )
		return false;
	
	obuf obufData;
	fillNPCHead(obufData, pActorService->getUID(), SC_MSG_ENTITY_NPC_OPEN_SHOP);

	obufData.push_back(&msg, sizeof(msg));

	pActorService->send2Client(PACKAGE_PTR(new std::string(obufData.data(), obufData.size())));

	return true;
}

/**
@name			: 触发NPC后
@param uidActor	: 玩家UID
@param dwNPC	: NPC
@param strRes	: 返回给客户端的数据
*/
MY_LUA_API void responseNPC(PDBID pdbid, UID uidNPC, const char* strRes)
{
    if(strRes == nullptr)
        return;

    string strContext;
    IResourceManager * pResourceManger = gServerGlobal->getResourceManager();
    if(pResourceManger && pResourceManger->isLoadLuaFromPack())
    {
        strContext = string(strRes);
    }
    else
    {
        strContext = string(a2utf8(strRes)); 
    }

	msg_entity_npc_response msg;
	msg.uidNPC = uidNPC;
	msg.nLen = strContext.size() + 1;

	ActorHelper helper(pdbid, ACTOR_DBID);

	IActorService* pActorService = helper.m_ptr;
	if(pActorService == NULL )
		return;

	obuf obufData;
	fillNPCHead(obufData, pActorService->getUID(), SC_MSG_ENTITY_NPC_RESPONSE);

	obufData.push_back(&msg, sizeof(msg));
	obufData.push_back(strContext.c_str(), msg.nLen);

	pActorService->send2Client(PACKAGE_PTR(new std::string(obufData.data(), obufData.size())));
}

/**
@name			: 校验NPC和玩家的距离是否合法
@param uidActor	: 玩家UID
@param uidNPC	：NPC的uid
@param nDist	: 合法距离范围
@return			: 距离是否合法
*/
MY_LUA_API bool verifyLocation(PDBID pdbid, UID uidNPC, float radii)
{
	ActorHelper actorHelper(pdbid, ACTOR_DBID);
	IActorService* pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
		return false;

	Vector3 actorLocation =pActorService->getLocation();

	MonsterHelper monsterHelper(uidNPC);
	INPCService* pNPCService = monsterHelper.m_ptr;
	if(pNPCService == NULL)
		return false;

	Vector3 monsterLocation =pNPCService->getLocation(uidNPC);

	// 验证玩家和NPC距离是否合法
	return actorLocation.checkDistance(monsterLocation, radii);
}

/**
@name			: 获取天赋点
@param pdbdID	: 玩家UID
*/
MY_LUA_API int getTelentPoint(PDBID pdbid)
{
	ActorHelper actorHelper(pdbid, ACTOR_DBID);
	IActorService* pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
		return 0;

	return pActorService->getProperty_Integer(PROPERTY_TALENT_POINT);
}

/**
@name			: 设置天赋点数
@param uidActor	: 玩家UID
@param nTelnetPoint : 天赋点数
@return			: 是否成功
*/
MY_LUA_API bool setTelentPoint(PDBID pdbid, int nTelnetPoint)
{
	ActorHelper actorHelper(pdbid, ACTOR_DBID);
	IActorService* pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
		return false;

	return pActorService->setProperty_Integer(PROPERTY_TALENT_POINT, nTelnetPoint);
}


/**
@name					: 给玩家添加buff
@param uidActor			: 玩家UID
@param dwBufferID		: buff ID
@param dwBufferLevel	: buff 等级
@param uidAdd			: 添加者
@param dwTime			: buff持续时间，填0表示取配置时间
@param pContext			: buff上下文
@param nContexLen		: buff上下文大小
@return					: 是否成功
*/
MY_LUA_API bool addBuff(PDBID pdbid, DWORD dwBufferID, DWORD dwBufferLevel, UID uidAdd, DWORD dwTime, void* pContext, int nContexLen)
{
	ActorHelper actorHelper(pdbid, ACTOR_DBID);
	IActorService* pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
		return false;

	return pActorService->AddBuff(dwBufferID, dwBufferLevel, uidAdd, dwTime, pContext, nContexLen);
}

/**
@name					: 关闭客户端的NPC对话框
*/
MY_LUA_API void closeNPCDialogBox(PDBID pdbid)
{
	ActorHelper actorHelper(pdbid, ACTOR_DBID);
	IActorService* pActorService = actorHelper.m_ptr;
	if(pActorService == NULL )
		return ;

	obuf obufData;
	fillNPCHead(obufData, pActorService->getUID(), SC_MSG_ENTITY_NPC_CLOSE_DIALOGBOX);

	pActorService->send2Client(PACKAGE_PTR(new std::string(obufData.data(), obufData.size())));
}

/**
@name					: 设置战场使用翅膀标识
@param uidActor			: 玩家UID
*/
MY_LUA_API void setActorWarUseWringFlag(PDBID pdbid)
{
	ActorHelper actorHelper(pdbid, ACTOR_DBID);
	IActorService* pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
		return;

	int nUseFlag = pActorService->getTaskDataInt(TASKKEY_ACTOR_WAR_USEWRING);

	if (nUseFlag == TASKDATA_DEFAULT_VALUE)
	{
		pActorService->addTaskDataInt(TASKKEY_ACTOR_WAR_USEWRING, 1);
	}
}

MY_LUA_API void cureMember(PDBID pdbid)
{
	ActorHelper actorHelper(pdbid, ACTOR_DBID);
	IActorService* pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
		return;

	int nWarID = pActorService->getWarID();

	if (nWarID == INVALID_WAR_ID)
		return;

	WarHelper warHelper(nWarID);
	IWarService* pWarService = warHelper.m_ptr;
	if (pWarService == NULL)
		return;

	pWarService->cureMember(pActorService->getUID());
}

MY_LUA_API bool luaBuyGoods(PDBID pdbid, int nGoodID, int nCost)
{
    ActorHelper actorHelper(pdbid, ACTOR_DBID);
    IActorService* pActorService = actorHelper.m_ptr;
    if (pActorService == NULL)
        return false;

    
    return pActorService->luaBuyGoods(nGoodID, nCost);
}

MY_LUA_API void NPCGoodsBuySuccess(PDBID pdbid, int uidNPC, int npcID, int goodsID, int buyTime)
{
    EmphasisLn("NPCGoodsBuySuccess.pdbid=" << pdbid << "uidNPC=" << uidNPC << "npcID=" << npcID << "goodsID=" << goodsID);

    SNPCGoodsBuySuccess buyGoods;
    buyGoods.uidNPC = uidNPC;
    buyGoods.pdbid = pdbid;
    buyGoods.goodsID = goodsID;
    buyGoods.npcID = npcID;
    buyGoods.buyTime = buyTime;

    ActorHelper helper(pdbid, ACTOR_DBID);
    IActorService* pActorService = helper.m_ptr;
    if (pActorService == NULL)
        return;

    obuf obufData;
    fillNPCHead(obufData, pActorService->getUID(), SC_MSG_ENTITY_NPC_BUY_SUCCESS);

    obufData.push_back(&buyGoods, sizeof(buyGoods));

    pActorService->send2Client(PACKAGE_PTR(new std::string(obufData.data(), obufData.size())));
}