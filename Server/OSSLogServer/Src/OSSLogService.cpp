/*******************************************************************
** 文件名:	OSSLogService.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-7-19
** 版  本:	1.0
** 描  述:	日记服务

********************************************************************/

#include "stdafx.h"
#include "OSSLogService.h"
#include "OSSLogServer.h"


////////////////// OSSLogService ////////////////////////////////////////////////////////
// 构造函数
COSSLogService::COSSLogService()
{

}

// 析构函数
COSSLogService::~COSSLogService()
{

}

// 创建
bool COSSLogService::create()
{
    return true;
}

// 释放
void COSSLogService::release()
{

}

////////////////// IOSSLogService ////////////////////////////////////////////////////////
/** 记录文字型日志
@param   nNoteTrackType: OSS_NOTE_TRACK_TYPE
@param   dwIndex ：索引，各个类型不同，索引也不一样：参考OSS_NOTE_TRACK_TYPE对应数值
@param   pszNote ：描述
@return  
*/
void COSSLogService::addGameNoteLog(int nGameWorldID, int nNoteTrackType, DWORD dwIndex, LPCSTR pszNote)
{
    g_pOssLogServer ? g_pOssLogServer->addGameNoteLog(nGameWorldID, nNoteTrackType, dwIndex, pszNote) : 0;
}

/** 商城购买消耗
@ name   : 点券日志
@ param pActorTicketLog: 日志的详细信息 DBREQ_OSS_MONITOR_ACTORTICKET_ITEM
*/
void COSSLogService::addPlazaTicketLog(int nGameWorldID, void* pActorTicketLog, size_t nLogSize)
{
    g_pOssLogServer ? g_pOssLogServer->addPlazaTicketLog(nGameWorldID, pActorTicketLog, nLogSize) : 0;
}

/**
@ name   : 点券产生/消耗
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 非绑定点券数目（产生为正数，消耗为负数）
@ param byApproach : 非绑定点券产生/消耗的途径
@ param nTime      : 非绑定点券产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService::addTicketLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    g_pOssLogServer ? g_pOssLogServer->addTicketLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, szRemark, dwReserve) : 0;
}

/**
@ name   : 点券转进/转出
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 非绑定点券数目（产生为正数，消耗为负数）
@ param byApproach : 非绑定点券产生/消耗的途径
@ param nTime      : 非绑定点券产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService::transTickLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    g_pOssLogServer ? g_pOssLogServer->transTickLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, szRemark, dwReserve) : 0;
}

/**
@ name   : 金钱产生/消耗
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService::addMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    g_pOssLogServer ? g_pOssLogServer->addMoneyLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, szRemark, dwReserve) : 0;
}

/**
@ name   : 金钱转进/转出
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService::transMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    g_pOssLogServer ? g_pOssLogServer->transMoneyLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, szRemark, dwReserve) : 0;
}

/**
@ name   : 绑金产生/消耗
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService::addBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    g_pOssLogServer ? g_pOssLogServer->addBindMoneyLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, szRemark, dwReserve) : 0;
}

/**
@ name   : 绑金转进/转出
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService::transBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    g_pOssLogServer ? g_pOssLogServer->transBindMoneyLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, szRemark, dwReserve) : 0;
}


/**	记录经验日志
*/
void COSSLogService::addExpLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    g_pOssLogServer ? g_pOssLogServer->addExpLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, szRemark, dwReserve) : 0;
}

/** 其它数值数据记录
@ param byObjType  : 类型（OSS_OPT_TYPE）
@ param nObjectID  : 类型ID（角色ID、帮会ID）
@ param nTarget	   : 与主ID相关的保留数据
@ param nNum	   : 数值大小
@ param byNumType  : 数值类型 OSS_NUMERIC_TYPE
@ param byOpType   : 操作类型 OSS_RESOURCE_OPT_TYPE
@ param byApproach : 途径
@ param nTime      : 记录的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService::addNumericLog(int nGameWorldID, BYTE byObjType, int nObjectID, int nTarget, int nNum, BYTE byNumType, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    g_pOssLogServer ? g_pOssLogServer->addNumericLog(nGameWorldID, byObjType, nObjectID, nTarget, nNum, byNumType, byOpType, wApproach, nTime, szRemark, dwReserve) : 0;
}


/** 玩家物品日记
@ param nGameWorldID: 玩家游戏世界ID
@ param byFromType : 物品原主类型（OSS_OPT_TYPE）
@ param lFromObjID : 物品原主Id  或者怪物的ID,任务Npc,Id
@ param byToObjType: 物品新主类型（OSS_OPT_TYPE）
@ param lToObjID   : 物品新主Id（角色ID、帮会ID...）
@ param nGoodsType : 类型 OSS_GOODS_TYPE
@ param byOpType   : 操作类型 OSS_RESOURCE_OPT_TYPE
@ param byApproach : 途径 OSS_RESOURCE_OPT_TYPE各项对应细项
@ param nTypeID    : 类型id
@ param nNum       : 数量
@ param nTime      : 日记产生时间
@ param szRemark   : 描述字符串
@ param dwReserveID1: 保留ID1
@ param dwReserveID2: 保留ID2
*/
void COSSLogService::addActorGoodsLog(int nGameWorldID, BYTE byFromType, long lFromObjID, BYTE byToType, long lToObjID, BYTE nGoodsType, BYTE byOpType, WORD wApproach,
    int nTypeID, int nNum, int nTime, const char * szRemark, DWORD dwReserveID1, DWORD dwReserveID2)
{
    g_pOssLogServer ? g_pOssLogServer->addActorGoodsLog(nGameWorldID, byFromType, lFromObjID, byToType, lToObjID, nGoodsType, byOpType, wApproach, nTypeID, nNum, nTime, szRemark, dwReserveID1, dwReserveID2) : 0;
}

/** 玩家升级日记
@ param nGameWorldID: 游戏世界ID
@ param dwUserID   : 玩家账号ID
@ param dwActorID  : 玩家角色ID
@ param nUpgradeType: 升级类型（1=等级，2=段位，3=英雄评级）
@ param dwValue	   : 数值
@ param nTime      : 发生时间
@ param dwReserve  : 保留数据（英雄评级时为英雄ID）
*/
void COSSLogService::addActorUpgradeLog(int nGameWorldID, DWORD dwUserID, DWORD dwActorID, BYTE nUpgradeType, DWORD dwValue, int nTime, DWORD dwReserve)
{
	g_pOssLogServer ? g_pOssLogServer->addActorUpgradeLog(nGameWorldID, dwUserID, dwActorID, nUpgradeType, dwValue, nTime, dwReserve) : 0;
}

/** 玩家打造日记
@ param nGameWorldID: 游戏世界ID
@ param dwUserID  ` : 玩家账号ID
@ param dwActorID  `: 玩家角色ID
@ param nOperateType: 操作类别（OSS_GOOD_FORGE_TYPE）
@ param nGoodID	   `: 物品id
@ param szGoodName	: 物品描述
@ param nTime	    : 发生时间
@ param dwReserve   : 保留数据
*/
void COSSLogService::addActorForgeLog(int nGameWorldID, DWORD dwUserID, DWORD dwActorID, BYTE nOperateType, DWORD dwGoodID,const char * szGoodName ,int nTime,const char * szForgeDesc, DWORD dwReserve )
{
	g_pOssLogServer ? g_pOssLogServer->addActorForgeLog(nGameWorldID,  dwUserID,  dwActorID,  nOperateType,  dwGoodID, szGoodName , nTime, szForgeDesc,  dwReserve) : 0;
}

/** 任务事件触发后填写信息以便写日志
@ param nTaskID	   `: 任务id
@ param nAcceptNum	: 接受次数
@ param nFinishNum	: 完成次数
@ param nGiveUpNum  : 放弃次数
*/
void COSSLogService::addTaskInfo(int nTaskID, int nAcceptNum, int nFinishNum, int nGiveUpNum)
{
	g_pOssLogServer ? g_pOssLogServer->addTaskInfo(nTaskID, nAcceptNum, nFinishNum, nGiveUpNum) : 0;
}