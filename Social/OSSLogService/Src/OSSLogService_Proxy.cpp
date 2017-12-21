/*******************************************************************
** 文件名:	OSSLogService_Proxy.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-7-19
** 版  本:	1.0
** 描  述:	日记服务接口


********************************************************************/

#include "stdafx.h"
#include "OSSLogService_Proxy.h"


//////////////////////////////////////////////////////////////////////////
COSSLogService_Proxy::COSSLogService_Proxy()
{

}

COSSLogService_Proxy::~COSSLogService_Proxy()
{

}

//////////////////////////////////////////////////////////////////////////
/** 记录文字型日志
@param   nNoteTrackType: OSS_NOTE_TRACK_TYPE
@param   dwIndex ：索引，各个类型不同，索引也不一样：参考OSS_NOTE_TRACK_TYPE对应数值
@param   pszNote ：描述
@return  
*/
void COSSLogService_Proxy::addGameNoteLog(int nGameWorldID, int nNoteTrackType, DWORD dwIndex, LPCSTR pszNote)
{
    if(pszNote == NULL) return;

    size_t nNoteLen = strlen(pszNote) + 1;
    size_t nLen = sizeof(int) + sizeof(int) + sizeof(DWORD) + sizeof(size_t)+ nNoteLen;

    BUILD_MSG_BUFFER_LEN(IOSSLogService::addGameNoteLog, nLen);
    t_data << nGameWorldID << nNoteTrackType << dwIndex << nNoteLen;
    t_data.push_back(pszNote, nNoteLen);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/** 商城购买消耗
@ name   : 点券日志
@ param pActorTicketLog: 日志的详细信息 DBREQ_OSS_MONITOR_ACTORTICKET_ITEM
*/
void COSSLogService_Proxy::addPlazaTicketLog(int nGameWorldID, void* pActorTicketLog, size_t nLogSize)
{
    if(pActorTicketLog == NULL || nLogSize == 0) return;

    obuf256 t_data;
    t_data << nGameWorldID << nLogSize;
    t_data.push_back(pActorTicketLog, nLogSize);

	BUILD_MSG_BUFFER( IOSSLogService::addPlazaTicketLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/**
@ name   : 点券产生/消耗
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nSource	   : 与玩家进行交互的对象ID
@ param nNum	   : 非绑定点券数目（产生为正数，消耗为负数）
@ param byApproach : 非绑定点券产生/消耗的途径
@ param nTime      : 非绑定点券产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService_Proxy::addTicketLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << dwMaster << nTarget << nNum << wApproach << nTime << dwReserve << strRemark;

	BUILD_MSG_BUFFER( IOSSLogService::addTicketLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/**
@ name   : 点券转进/转出
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nSource	   : 与玩家进行交互的对象ID
@ param nNum	   : 非绑定点券数目（产生为正数，消耗为负数）
@ param byApproach : 非绑定点券产生/消耗的途径
@ param nTime      : 非绑定点券产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService_Proxy::transTickLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << dwMaster << nTarget << nNum << wApproach << nTime << dwReserve << strRemark;

	BUILD_MSG_BUFFER( IOSSLogService::transTickLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/**
@ name   : 金钱产生/消耗
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nSource	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService_Proxy::addMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << dwMaster << nTarget << nNum << wApproach << nTime << dwReserve << strRemark;

	BUILD_MSG_BUFFER( IOSSLogService::addMoneyLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/**
@ name   : 金钱转进/转出
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nSource	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService_Proxy::transMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << dwMaster << nTarget << nNum << wApproach << nTime << dwReserve << strRemark;

	BUILD_MSG_BUFFER( IOSSLogService::transMoneyLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/**
@ name   : 绑金产生/消耗
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nSource	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService_Proxy::addBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << dwMaster << nTarget << nNum << wApproach << nTime << dwReserve << strRemark;

	BUILD_MSG_BUFFER( IOSSLogService::addBindMoneyLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/**
@ name   : 绑金转进/转出
@ param nGameWorldID: 游戏世界ID
@ param dwMaster   : 玩家ID
@ param nSource	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogService_Proxy::transBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << dwMaster << nTarget << nNum << wApproach << nTime << dwReserve << strRemark;

	BUILD_MSG_BUFFER( IOSSLogService::transBindMoneyLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}


/**	记录经验日志
*/
void COSSLogService_Proxy::addExpLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << dwMaster << nTarget << nNum << wApproach << nTime << dwReserve << strRemark;

	BUILD_MSG_BUFFER( IOSSLogService::addExpLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
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
void COSSLogService_Proxy::addNumericLog(int nGameWorldID, BYTE byObjType, int nObjectID, int nTarget, int nNum, BYTE byNumType, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << byObjType << nObjectID << nTarget << nNum << byNumType << byOpType << wApproach << nTime << dwReserve << strRemark;
	BUILD_MSG_BUFFER( IOSSLogService::addNumericLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
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
void COSSLogService_Proxy::addActorGoodsLog(int nGameWorldID, BYTE byFromType, long lFromObjID, BYTE byToType, long lToObjID, BYTE nGoodsType, BYTE byOpType, WORD wApproach,
    int nTypeID, int nNum, int nTime, const char * szRemark, DWORD dwReserveID1, DWORD dwReserveID2)
{
    string strRemark = (szRemark == NULL ? "" : szRemark);
    obuf256 t_data;
    t_data << nGameWorldID << byFromType << lFromObjID << byToType << lToObjID << nGoodsType << byOpType << wApproach << nTypeID << nNum << nTime << dwReserveID1 << dwReserveID2 << strRemark;
    BUILD_MSG_BUFFER( IOSSLogService::addActorGoodsLog,t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
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
void COSSLogService_Proxy::addActorUpgradeLog(int nGameWorldID, DWORD dwUserID, DWORD dwActorID, BYTE nUpgradeType, DWORD dwValue, int nTime, DWORD dwReserve)
{
    BUILD_MSG_CONTEXT_7( IOSSLogService::addActorUpgradeLog,int,nGameWorldID, DWORD, dwUserID, DWORD, dwActorID, BYTE, nUpgradeType, DWORD, dwValue, int, nTime, DWORD, dwReserve);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}