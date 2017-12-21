/*******************************************************************
** 文件名:	OSSLogServer.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-7-4
** 版  本:	1.0
** 描  述:  (Operating Support System)运营支持系统日志
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "ITimerService.h"
#include "IMiscManager.h"
#include "LogRecord.h"
#include "OSSLogServer.h"
#include "SharePropertyDef.h"
#include "IShareServer.h"


#define GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwPDBID, szFunction)					    \
    pLogRecord = getLogRecord(nGameWorldID, __FILE__, __LINE__);			                    \
    if (pLogRecord == NULL)																        \
    {																					        \
        ErrorLn((szFunction) << "没有日志记录对象 dwPDBID=" << dwPDBID);                        \
        debugBreak;																		        \
        return;																			        \
    }		
//////////////////////////////////////////////////////////////////////////

COSSLogServer	*		g_pOssLogServer = NULL;

COSSLogServer::COSSLogServer()
    : m_pService( 0 )
{
    g_pOssLogServer = this;
}

COSSLogServer::~COSSLogServer()
{

}


// 创建
bool COSSLogServer::create(void)
{
    // 主动创建Service
    createService("OSSLogService");

    // 数据库接口注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if(pDBEngineService)
    {
        pDBEngineService->registerHandler(this, __FUNCTION__);
    }

    ITimerService *  pTimerService = gSocialGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->SetTimer(OSS_SAVE_TIMERID, OSS_REPORT_INTERVAL, this, INFINITY_CALL, __FUNCTION__);
    } 

    // 发布停机事件    
    // 发服务器关闭事件，主要是通知场景服模块做善后事件
    IEventEngine * pEventEngine = gSocialGlobal->getEventEngine();
    if(pEventEngine != NULL)
    {
        // 订阅
        pEventEngine->Subscibe(this, EVENT_STOP_SERVER_SERVICE_SAVE_DATA, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
    }

    return true;
}

void COSSLogServer::onStop()
{
    IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
    if(pEventEngine)
    {
        // 取消订阅 
        pEventEngine->UnSubscibe(this, EVENT_STOP_SERVER_SERVICE_SAVE_DATA, SOURCE_TYPE_SYSTEM, 0);
    }

    // 数据库接口反注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if(pDBEngineService)
    {
        pDBEngineService->unregisterHandler(this);
    }    

    ITimerService *  pTimerService = gSocialGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->KillTimer(OSS_SAVE_TIMERID,this);
    }

    // 主动释放Service
    if(m_pService)
    {
        m_pService->stop();
    }
    m_pService = SERVICE_PTR(0);
}

//////////////////////////////////////////////////////////////////////////
// 释放
void COSSLogServer::release(void)
{
    delete this;
}


/** 执行一个存储过程
@param   dwCmdID		：cmdid
@param   dwQueueIndex	: 异步队列索引号
@param   pszInData		：输入数据
@param   nInDataLen		：输入数据长度
@param   pDBRetSink		：结果返回接口	
@param   nGameWorldID	: 向何个游戏世界数据库执行，若为公共区,必填游戏世界id
@return  		
@retval buffer 
*/
bool COSSLogServer::executeSP(DWORD dwCmdID, LPCSTR pszInData, int nInDataLen, int nGameWorldID)
{
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return false;
    }

    if(nGameWorldID == 0) nGameWorldID = getThisGameWorldID();

    if ( !pDBEngine->executeSP(dwCmdID, dwCmdID, pszInData, nInDataLen, static_cast<IDBRetSink *>(this), nGameWorldID) )
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK failed");
        return false;
    }
    return true;
}

// 创建服务
bool COSSLogServer::createService(const char * pszServiceSchemeName)
{
    m_pService = gSocialGlobal->getMiscManager()->create_service(pszServiceSchemeName);

	return true;
}
//////////////////////////////////////////////////////////////////////////
/** 取日志记录管理器
@param   
@param   
@return  
*/
CLogRecord* COSSLogServer::getLogRecord(int nGameWorldID, const char* file, int line)
{
    if (nGameWorldID==0)
    {
        nGameWorldID = getThisGameWorldID();
        WarningLn(__FUNCTION__": worldid is zero, system set this worldid to world value. worldid="<<nGameWorldID <<", file="<<file <<", line="<<line);
        debugBreak;
    }


    TMAP_LOGRECORD::iterator Iter = m_logRecordMap.find(nGameWorldID);
    if (Iter != m_logRecordMap.end())
    {
        return Iter->second;
    }

    CLogRecord* pLogRecord = new CLogRecord();
    if(pLogRecord == NULL || !pLogRecord->Create(nGameWorldID))
    {
        SAFE_RELEASE(pLogRecord);
        return NULL;
    }

    m_logRecordMap[nGameWorldID] = pLogRecord;
    return pLogRecord;
}


/** 取操作类型 OSS_RESOURCE_OPT_TYPE
*/
BYTE COSSLogServer::getOpType(WORD wApproach)
{
    if (wApproach>=OSSRES_ADD_MIN && wApproach<OSSRES_ADD_MAX)                    // 资源添加
    {
        return OSS_RESOURCE_OPT_ADD;
    }
    else if (wApproach>=OSSRES_DEL_MIN && wApproach<OSSRES_DEL_MAX)               // 资源消耗
    {
        return OSS_RESOURCE_OPT_DEL;
    }
    else if (wApproach>=OSSRES_TRANIN_MIN && wApproach<OSSRES_TRANIN_MAX)         // 资源转进
    {
        return OSS_RESOURCE_OPT_TRANSIN;
    }
    else if (wApproach>=OSSRES_TRANOUT_MIN && wApproach<OSSRES_TRANOUT_MAX)       // 资源转出
    {
        return OSS_RESOURCE_OPT_TRANSOUT;
    }
    else
    {
        return OSS_RESOURCE_OPT_MAX;
    }
}

///////////////////////IOSSLogServer/////////////////////////////////
/** 记录文字型日志
@param   nNoteTrackType ：OSS_NOTE_TRACK_TYPE
@param   dwIndex ：索引，各个类型不同，索引也不一样：参考OSS_NOTE_TRACK_TYPE对应数值
@param   pszNote ：描述
@return  
*/
void COSSLogServer::addGameNoteLog(int nGameWorldID, int nNoteTrackType, DWORD dwIndex, LPCSTR pszNote)
{
    if (pszNote == NULL)
    {
        return;
    }

    DBREQ_PARAM_LOG_NOTE log;
    log.wNoteType = nNoteTrackType;
    log.dwIndex = dwIndex;
    if (pszNote != NULL)
    {
        sstrcpyn(log.szNote, pszNote, sizeof(log.szNote));
    }

    executeSP(GAMEDB_REQUEST_LOG_NOTE, (LPCSTR)&log, sizeof(log), nGameWorldID);
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
void COSSLogServer::addTicketLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if(nNum == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwMaster, __FUNCTION__);

    BYTE nOptType = getOpType(wApproach);
    if (nOptType == OSS_RESOURCE_OPT_ADD)
    {
        pLogRecord->RecordTicket(dwMaster, nTarget, abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else if (nOptType == OSS_RESOURCE_OPT_DEL)
    {
        pLogRecord->RecordTicket(dwMaster, nTarget, -abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else
    {
        WarningLn(__FUNCTION__":Error nOptType="<<nOptType<<"( dwMaster = " << dwMaster << ", nTarget = " << nTarget << ", nNum = " <<  nNum << ", wApproach = " << wApproach << ", nTime = " << nTime << ", szRemark = " << szRemark << ")");
        debugBreak;
    }
}

/** 商城购买消耗
@ name   : 点券日志
@ param pActorTicketLog: 日志的详细信息 DBREQ_OSS_MONITOR_ACTORTICKET_ITEM
*/
void COSSLogServer::addPlazaTicketLog(int nGameWorldID, void* pActorTicketLog, size_t nLogSize)
{
    if (pActorTicketLog==NULL || nLogSize != sizeof(DBREQ_OSS_MONITOR_ACTORTICKET_ITEM) )
    {
        return;
    }

    if(nGameWorldID == 0) nGameWorldID = getThisGameWorldID();

    DBREQ_OSS_MONITOR_ACTORTICKET_ITEM * pItem = CAST_TYPE(DBREQ_OSS_MONITOR_ACTORTICKET_ITEM*, pActorTicketLog);
    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, pItem->nActorID, __FUNCTION__);

    pLogRecord->RecordPlazaTicket(pItem);
}

/**
@ name   : 点券转进/转出
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 非绑定点券数目（产生为正数，消耗为负数）
@ param byApproach : 非绑定点券产生/消耗的途径
@ param nTime      : 非绑定点券产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogServer::transTickLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwMaster, __FUNCTION__);

    BYTE nOptType = getOpType(wApproach);
    if (nOptType == OSS_RESOURCE_OPT_TRANSIN)           // 资源转进
    {
        pLogRecord->RecordTicket(dwMaster, nTarget, abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else if (nOptType == OSS_RESOURCE_OPT_TRANSOUT)     // 资源转出
    {
        pLogRecord->RecordTicket(dwMaster, nTarget, -abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else
    {
        WarningLn(__FUNCTION__":Error nOptType="<<nOptType<<"( dwMaster = " << dwMaster << ", nTarget = " << nTarget << ", nNum = " <<  nNum << ", wApproach = " << wApproach << ", nTime = " << nTime << ", szRemark = " << szRemark << ")");
        debugBreak;
    }	
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
void COSSLogServer::addMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if(nNum == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwMaster, __FUNCTION__);

    BYTE nOptType = getOpType(wApproach);
    if (nOptType == OSS_RESOURCE_OPT_ADD)
    {
        pLogRecord->RecordMoney(dwMaster, nTarget, abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else if (nOptType == OSS_RESOURCE_OPT_DEL)
    {
        pLogRecord->RecordMoney(dwMaster, nTarget, -abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else
    {
        WarningLn(__FUNCTION__":Error nOptType="<<nOptType<<"( dwMaster = " << dwMaster << ", nTarget = " << nTarget << ", nNum = " <<  nNum << ", wApproach = " << wApproach << ", nTime = " << nTime << ", szRemark = " << szRemark << ")");
        debugBreak;
    }
}

/**
@ name   : 金钱转进/转出
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogServer::transMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if(nNum == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwMaster, __FUNCTION__);

    BYTE nOptType = getOpType(wApproach);
    if (nOptType == OSS_RESOURCE_OPT_TRANSIN)           // 资源转进
    {
        pLogRecord->RecordMoney(dwMaster, nTarget, abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else if (nOptType == OSS_RESOURCE_OPT_TRANSOUT)     // 资源转出
    {
        pLogRecord->RecordMoney(dwMaster, nTarget, -abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else
    {
        WarningLn(__FUNCTION__":Error nOptType="<<nOptType<<"( dwMaster = " << dwMaster << ", nTarget = " << nTarget << ", nNum = " <<  nNum << ", wApproach = " << wApproach << ", nTime = " << nTime << ", szRemark = " << szRemark << ")");
        debugBreak;
    }	
}
	
/**
@ name   : 绑金产生/消耗/转进/转出
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据
*/
void COSSLogServer::addBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if(nNum == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwMaster, __FUNCTION__);

    BYTE nOptType = getOpType(wApproach);
    if (nOptType == OSS_RESOURCE_OPT_ADD)
    {
        pLogRecord->RecordBindMoney(dwMaster, nTarget, abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else if (nOptType == OSS_RESOURCE_OPT_DEL)
    {
        pLogRecord->RecordBindMoney(dwMaster, nTarget, -abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else
    {
        WarningLn(__FUNCTION__":Error nOptType="<<nOptType<<"( dwMaster = " << dwMaster << ", nTarget = " << nTarget << ", nNum = " <<  nNum << ", wApproach = " << wApproach << ", nTime = " << nTime << ", szRemark = " << szRemark << ")");
        debugBreak;
    }
}

/**
@ name   : 绑金转进/转出
@ param dwMaster   : 玩家ID
@ param nTarget	   : 与玩家进行交互的对象ID
@ param nNum	   : 金钱数目（产生为正数，消耗为负数）
@ param byApproach : 金钱产生/消耗的途径
@ param nTime      : 金钱产生/消耗的时间
@ param szRemark   : 备注
@ param dwReserve  : 保留数据（用于标识一些特别的记录）
*/
void COSSLogServer::transBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if(nNum == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwMaster, __FUNCTION__);

    BYTE nOptType = getOpType(wApproach);
    if (nOptType == OSS_RESOURCE_OPT_TRANSIN)           // 资源转进
    {
        pLogRecord->RecordBindMoney(dwMaster, nTarget, abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else if (nOptType == OSS_RESOURCE_OPT_TRANSOUT)     // 资源转出
    {
        pLogRecord->RecordBindMoney(dwMaster, nTarget, -abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else
    {
        WarningLn(__FUNCTION__":Error nOptType="<<nOptType<<"( dwMaster = " << dwMaster << ", nTarget = " << nTarget << ", nNum = " <<  nNum << ", wApproach = " << wApproach << ", nTime = " << nTime << ", szRemark = " << szRemark << ")");
        debugBreak;
    }	
}


/**	记录经验日志
*/
void COSSLogServer::addExpLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if(nNum == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwMaster, __FUNCTION__);

    BYTE nOptType = getOpType(wApproach);
    if (nOptType == OSS_RESOURCE_OPT_ADD)           // 资源添加
    {
        pLogRecord->RecordExp(dwMaster, nTarget, abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else if (nOptType == OSS_RESOURCE_OPT_DEL)     // 资源消耗
    {
        pLogRecord->RecordExp(dwMaster, nTarget, -abs(nNum), nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else
    {
        WarningLn(__FUNCTION__":Error nOptType="<<nOptType<<"( dwMaster = " << dwMaster << ", nTarget = " << nTarget << ", nNum = " <<  nNum << ", wApproach = " << wApproach << ", nTime = " << nTime << ", szRemark = " << szRemark << ")");
        debugBreak;
    }
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
void COSSLogServer::addNumericLog(int nGameWorldID, BYTE byObjType, int nObjectID, int nTarget, int nNum, BYTE byNumType, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if(nNum == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    if(byObjType == OSS_OPT_TYPE_PERSON)
    {
        //ActorHelper helper(nObjectID,ACTOR_DBID);
        //if (helper.m_ptr == NULL)
        //{
            //return;
        //}
		 SSharePersonProperty shareProperty = gSocialGlobal->getShareReceiver()->GetSharePerson(nObjectID);
		 if (shareProperty.dwPDBID == INVALID_PDBID)
		 {
			 return;
		 }
    }
    pLogRecord = getLogRecord(nGameWorldID, __FUNCTION__, __LINE__);
    if (pLogRecord == NULL)	
    {
        ErrorLn(__FUNCTION__ << ": No get the record object! ObjectID=" << nObjectID);
        return;
    }	

    BYTE nOptType = getOpType(wApproach);
    if (nOptType == OSS_RESOURCE_OPT_ADD || nOptType == OSS_RESOURCE_OPT_TRANSIN)           // 资源添加
    {
        pLogRecord->TrackAddNumeric(byObjType, nObjectID, nTarget, abs(nNum), byNumType, nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else if (nOptType == OSS_RESOURCE_OPT_DEL || nOptType == OSS_RESOURCE_OPT_TRANSOUT)     // 资源消耗
    {
        pLogRecord->TrackAddNumeric(byObjType, nObjectID, nTarget, -abs(nNum), byNumType, nOptType, wApproach, nTime, szRemark, dwReserve);
    }
    else
    {
        WarningLn(__FUNCTION__":Error nOptType="<<nOptType<<"(ObjectID= "<<nObjectID<< ",ObjType="<<byObjType<<",nTarget="<<nTarget<<",nNum="<<nNum<<",wApproach="<<wApproach<<",nTime="<<nTime<<",szRemark="<<szRemark<< ")");
        debugBreak;
    }
}


/** 玩家物品日记
@ param nGameWorldID: 玩家游戏世界ID
@ param byFromType : 物品原主类型（OSS_OPT_TYPE）
@ param lFromObjID : 物品原主Id  或者怪物的ID,任务Npc,Id
@ param byToObjType: 物品新主类型（OSS_OPT_TYPE）
@ param lToObjID   : 物品新主Id（角色ID）
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
void COSSLogServer::addActorGoodsLog(int nGameWorldID, BYTE byFromType, long lFromObjID, BYTE byToType, long lToObjID, BYTE nGoodsType, BYTE byOpType, WORD wApproach, int nTypeID, int nNum, int nTime, const char * szRemark, DWORD dwReserveID1, DWORD dwReserveID2)
{
    if(nNum == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, lToObjID, __FUNCTION__);

    pLogRecord->TrackAddGoods(byFromType, lFromObjID, byToType, lToObjID, nGoodsType, byOpType, wApproach, nTypeID, nNum, nTime, szRemark, dwReserveID1, dwReserveID2);
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
void COSSLogServer::addActorUpgradeLog(int nGameWorldID, DWORD dwUserID, DWORD dwActorID, BYTE nUpgradeType, DWORD dwValue, int nTime, DWORD dwReserve)
{
    if(dwValue == 0)
    {
        return;
    }

    CLogRecord* pLogRecord = NULL;
    GETLOGRECORDBYPDBID(pLogRecord, nGameWorldID, dwActorID, __FUNCTION__);

    pLogRecord->RecordActorUpgrade(dwUserID, dwActorID, nUpgradeType, dwValue, nTime, dwReserve);
}

///////////////////////////////// IDBRetSink ////////////////////////////
/** 数据库请求返回回调方法
@param   pDBRetSink		：结果返回接口	
@param   nCmdID ：命令ＩＤ
@param   nDBRetCode：数据库请求返回值，参考上面定义
@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
@param   nQueueIndex：队列定义
@param   pOutData：输出数据
@param   nOutLen：输出数据长度
@return  
@note     
@warning 此对像千万不能在数据库返回前释放，否则会非法！
@retval buffer 
*/	
void COSSLogServer::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{

}

////////////////// IEventExecuteSink ////////////////////////////////////////////////////////
/** 
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  
@note     
@warning 
@retval buffer 
*/
void COSSLogServer::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
    case EVENT_STOP_SERVER_SERVICE_SAVE_DATA:
        {
            for (TMAP_LOGRECORD::iterator Iter=m_logRecordMap.begin(); Iter!=m_logRecordMap.end(); Iter++)
            {
                if (Iter->second == NULL)
                {
                    continue;
                }

                Iter->second->SaveAllLog();
            }
        }
        break;

    default: break;
    }
}

////////////////////////TimerHandler//////////////////////////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void COSSLogServer::OnTimer( unsigned long dwTimerID )
{
    if (dwTimerID == OSS_SAVE_TIMERID)
    {
        for (TMAP_LOGRECORD::iterator Iter=m_logRecordMap.begin(); Iter!=m_logRecordMap.end(); Iter++)
        {
            if (Iter->second == NULL)
            {
                continue;
            }

            Iter->second->OnTimer();
        }
    }

}

//////////////////////////////////////////////////////////////////////////

// 输出函数
API_EXPORT IOSSLogServer * CreateOSSLogServer(void)
{
    // 如果创建了，则直接返回
    if(g_pOssLogServer != NULL)
    {
        return g_pOssLogServer;
    }
    // 实例化
    COSSLogServer * pOssLogServer = new COSSLogServer();
    if(pOssLogServer == NULL)
    {
        return NULL;
    }

    if(!pOssLogServer->create())
    {
        if(pOssLogServer)
        {
            pOssLogServer->release();
            pOssLogServer = 0;
        }
        return NULL;
    }

    g_pOssLogServer = pOssLogServer;

    return g_pOssLogServer;
}