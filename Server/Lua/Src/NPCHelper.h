/*******************************************************************
** 文件名:	NPCHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/1/18
** 版  本:	1.0
** 描  述:	NPC数据帮助函数
********************************************************************/

#include "stdafx.h"
#include "SceneServerAPI.h"

/**
@name		:填充NPC下发给客户端的消息头
@ob			:发送缓冲区
@nMsgID		:消息ID
*/
void fillNPCHead( obuf& ob, UID uidMaster, int nMsgID );

/**
@name          : 打开战场商店
@param uidActor  : 玩家UID
@brief dwNPC   : NPC
@return        : 是否成功
*/
MY_LUA_API bool openWarShop(PDBID pdbid, UID uidNPC);

/**
@name			: 触发NPC后
@param uidActor	: 玩家UID
@param dwNPC	: NPC
@param strRes	: 返回给客户端的数据
*/
MY_LUA_API void responseNPC(PDBID pdbid, UID uidNPC, const char* strRes);

/**
@name			: 验证NPC和玩家的距离是否合法
@param uidActor	: 玩家UID
@param uidNPC	：NPC
@param nDist	: 合法距离范围
@return			: 距离是否合法
*/
MY_LUA_API bool verifyLocation(PDBID pdbid, UID uidNPC, float radii);


/**
@name			: 获取天赋点
@param uidActor	: 玩家UID
@return			: 返回天赋点数
*/
MY_LUA_API int getTelentPoint(PDBID pdbid);

/**
@name					: 设置天赋点数
@param uidActor			: 玩家UID
@param nTelnetPoint		: 天赋点数
@return					: 是否成功
*/
MY_LUA_API bool setTelentPoint(PDBID pdbid, int nTelnetPoint);


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
MY_LUA_API bool addBuff(PDBID pdbid, DWORD dwBufferID, DWORD dwBufferLevel, UID uidAdd, DWORD dwTime, void* pContext, int nContexLen);

/**
@name					: 关闭客户端的NPC对话框
@param uidActor			: 玩家UID
*/
MY_LUA_API void closeNPCDialogBox(PDBID pdbid);

/**
@name					: 设置战场使用翅膀标识
@param uidActor			: 玩家UID
*/
MY_LUA_API void setActorWarUseWringFlag(PDBID pdbid);

/**
@name					: 救治队友
@param uidActor			: 玩家UID
*/
MY_LUA_API void cureMember(PDBID pdbid);

/**
@name					: 购买装备
@param uidActor			: 玩家UID
@param uidActor			: 装备ID
*/
MY_LUA_API bool luaBuyGoods(PDBID pdbid, int nGoodsID, int nCost);

/**
@name					: 购买装备成功
@param pdbid			: 玩家UID
@param npcUID			: 装备ID
*/
MY_LUA_API void NPCGoodsBuySuccess(PDBID pdbid, int npcUID, int npcID, int goodsID, int buyTime);