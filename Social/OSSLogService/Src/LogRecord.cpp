/*******************************************************************
** 文件名:	LogRecord.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华 (Carl Chen)
** 日  期:	2016-7-4
** 版  本:	1.0
** 描  述:  记录日志
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "LogRecord.h"
#include "OssLogDef.h"
#include "time.h"
#include "IDBEngine.h"
#include <strstream>
#include "OSSLogServer.h"


CLogRecord::CLogRecord()
{
    m_nGameWorldID = 0;
    // 上次玩家物品存盘时间
    m_nActorGoodsLogLastSaveTime = 0;
    // 上次玩家升级日记存盘时间
    m_nActorUpgradeLogLastSaveTime = 0;
}

bool CLogRecord::Create(int nGameWorldID)
{
    m_nGameWorldID = nGameWorldID;

    time_t	long_time; 
    time(&long_time);

    return true;
}

void CLogRecord::Release()
{

}

////////////////////////////////////////////////////////////////////////////////////////
/** 记录点券详细日志（购买日记）
*/
void CLogRecord::RecordPlazaTicket(DBREQ_OSS_MONITOR_ACTORTICKET_ITEM* pActorTicketLog)
{
    if (pActorTicketLog == NULL)
    {
        return;
    }

    // 由于外面进来的都是UTF8字符串，这个里面是批量保存的，所以我们将在这里强制转下码
    string str(utf82a(pActorTicketLog->szGoodsName));
    memset(pActorTicketLog->szGoodsName, 0, sizeof(pActorTicketLog->szGoodsName));
    sstrcpyn( pActorTicketLog->szGoodsName, str.c_str(), sizeof(pActorTicketLog->szGoodsName) );

    m_listPlazaTicket.push_back(*pActorTicketLog);
    if (m_listPlazaTicket.size() >= OSS_TICKET_SAVE_MAXCOUNT)
    {
        SavePlazaTicketLog();
    }
}

/** 保存点券详细日志
*/
bool CLogRecord::SavePlazaTicketLog()
{
    if(m_listPlazaTicket.empty())
    {
        return true;
    }

    obuf obLog;
    DBREQ_OSS_MONITOR_ACTORTICKET log;

    obuf obData;
    for (OSS_TICKET_LIST::iterator it = m_listPlazaTicket.begin(); it != m_listPlazaTicket.end(); ++ it)
    {
        if (log.Num >= OSS_TICKET_SAVE_MAXCOUNT)
        {
            int nLogLength = log.Num * sizeof(DBREQ_OSS_MONITOR_ACTORTICKET_ITEM);

            obLog.push_back(&log, sizeof(log));            
            obLog.push_back(&nLogLength, sizeof(nLogLength));   // BUFF长度
            obLog.push_back(obData.data(), obData.size());

            SaveToVan(GAMEDB_REQUEST_MONITOR_ACTORTICKET, obLog.data(), obLog.size());

            obLog.clear();
            obData.clear();
            log.Num = 0;

            continue;
        }

        DBREQ_OSS_MONITOR_ACTORTICKET_ITEM & item = (*it);
        obData.push_back(&item, sizeof(item));

        ++log.Num;
    }

    if (log.Num>0)
    {
        int nLogLength = log.Num * sizeof(DBREQ_OSS_MONITOR_ACTORTICKET_ITEM);

        obLog.push_back(&log, sizeof(log));            
        obLog.push_back(&nLogLength, sizeof(nLogLength));   // BUFF长度
        obLog.push_back(obData.data(), obData.size());

        SaveToVan(GAMEDB_REQUEST_MONITOR_ACTORTICKET, obLog.data(), obLog.size());
    }
    m_listPlazaTicket.clear();

    return true;
}

//////////////////////////////////////////////////////////////////////////
/**	记录点券日志
*/
void CLogRecord::RecordTicket(DWORD dwMaster, int nTarget, int nNum, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if (nNum == 0 || (byOpType<OSS_RESOURCE_OPT_ADD && byOpType>=OSS_RESOURCE_OPT_MAX))
    {
        return;
    }
    //if (byOpType==OSS_RESOURCE_OPT_ADD || byOpType==OSS_RESOURCE_OPT_DEL)
    //{
    //    AddValueToMonitor(dwMaster, OSS_NUMERIC_TICKET, wApproach, nNum);
    //}

    // 记录金币数值, 用于告警检测
    // RecordNumericForAlarm(EMNumericAlarm_Type_Ticket, 0, byOpType, wApproach, 0, dwMaster, nNum);

    // 增加数值日记
    TrackAddNumeric(OSS_OPT_TYPE_PERSON, dwMaster, nTarget,nNum, OSS_NUMERIC_TICKET, byOpType, wApproach, nTime, szRemark, dwReserve);
}

/**	记录金钱日志
*/
void CLogRecord::RecordMoney(DWORD dwMaster, int nTarget, int nNum, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve )
{
    if (nNum == 0 || (byOpType<OSS_RESOURCE_OPT_ADD && byOpType>=OSS_RESOURCE_OPT_MAX))
    {
        return;
    }

    // 记录银两数值, 用于告警检测
    // RecordNumericForAlarm(EMNumericAlarm_Type_Money, 0, byOpType, wApproach, dwReserve, dwMaster, nNum);

    //if (byOpType==OSS_RESOURCE_OPT_ADD || byOpType==OSS_RESOURCE_OPT_DEL)
    //{
    //    AddValueToMonitor(dwMaster, OSS_NUMERIC_MONEY, wApproach, nNum);
    //}

    //if (abs(nNum) < OSS_BINDMONEY_RECORDLIMIT)
    //{
    //    return;
    //}

    // 增加数值日记
    TrackAddNumeric(OSS_OPT_TYPE_PERSON, dwMaster, nTarget,nNum, OSS_NUMERIC_MONEY, byOpType, wApproach, nTime, szRemark, dwReserve);
}


/**	记录绑定金钱日志
*/
void CLogRecord::RecordBindMoney(DWORD dwMaster, int nTarget, int nNum, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve )
{
    if (nNum == 0 || (byOpType<OSS_RESOURCE_OPT_ADD && byOpType>=OSS_RESOURCE_OPT_MAX))
    {
        return;
    }

    // 记录绑银数值, 用于告警检测
    //RecordNumericForAlarm(EMNumericAlarm_Type_BindMoney, 0, byOpType, wApproach, dwReserve, dwMaster, nNum);

    //if (byOpType==OSS_RESOURCE_OPT_ADD || byOpType==OSS_RESOURCE_OPT_DEL)
    //{
    //    AddValueToMonitor(dwMaster, OSS_NUMERIC_BINDMONEY, wApproach, nNum);
    //}

    // 增加数值日记
    TrackAddNumeric(OSS_OPT_TYPE_PERSON, dwMaster, nTarget,nNum, OSS_NUMERIC_BINDMONEY, byOpType, wApproach, nTime, szRemark, dwReserve);
}

/**	记录经验日志
*/
void CLogRecord::RecordExp(DWORD dwMaster, int nTarget, int nNum, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve )
{
    if (nNum == 0 || (byOpType<OSS_RESOURCE_OPT_ADD && byOpType>=OSS_RESOURCE_OPT_MAX))
    {
        return;
    }

    // 记录操作的经验数值, 用于告警检测
    //RecordNumericForAlarm(NUMERIC_ALARM_TYPE_EXP, 0, byOpType, wApproach, dwReserve, dwMaster, nNum);

    //if (byOpType==OSS_RESOURCE_OPT_ADD || byOpType==OSS_RESOURCE_OPT_DEL)
    //{
    //    AddValueToMonitor(dwMaster, OSS_NUMERIC_EXP, wApproach, nNum);
    //}

    // 增加数值日记
    TrackAddNumeric(OSS_OPT_TYPE_PERSON, dwMaster, nTarget,nNum, OSS_NUMERIC_EXP, byOpType, wApproach, nTime, szRemark, dwReserve);
}


/**	记录玩家升级日记
*/
void CLogRecord::RecordActorUpgrade(DWORD dwUserID, DWORD dwActorID, BYTE nUpgradeType, DWORD dwValue, int nTime, DWORD dwReserve)
{
    DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG node;
	node.dwWorldID = m_nGameWorldID;	// 世界ID
    node.dwUserID = dwUserID;       // 账号ID
    node.dwActorID = dwActorID;     // 角色ID
    node.nUpgradeType = nUpgradeType;  // 升级类型（1=等级，2=段位，3=英雄评级）
    node.dwValue = dwValue;         // 数值
    node.dwOptTime = nTime;         // 发生时间
    node.dwReserveID = dwReserve;	// 保留（英雄评级时为英雄ID）

    //加入本条日志
    m_lstActorUpgradeLog.push_back(node);

    int nLogCount = m_lstActorUpgradeLog.size();
    size_t nSize = (nLogCount + 2)*sizeof(DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG) + sizeof(DBREQ_PARAM_OSS_ACTOR_UPGRADE_LEVEL_LOGS);
    if (nSize >= 7*1024)
    {
        BatchSaveActorUpgradeLog();
    }
}


/** 批量保存玩家升级日记
*/
void CLogRecord::BatchSaveActorUpgradeLog()
{
    if (m_lstActorUpgradeLog.empty())
    {
        return;
    }
        
    time_t	long_time; time(&long_time);int nCurTime = (int)long_time;

    obuf obLog;
    DBREQ_PARAM_OSS_ACTOR_UPGRADE_LEVEL_LOGS log;

    size_t nSize = 0;
    obuf obData;

    // 保存单条记录
    for(TLIST_ACTOR_UPGRADE_LOG::iterator iter = m_lstActorUpgradeLog.begin(); iter != m_lstActorUpgradeLog.end(); ++iter)
    {
        if (nSize >= 7*1024)
        {
            int nLogLength = log.Num * sizeof(DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG);

            obLog.push_back(&log, sizeof(log));            
            obLog.push_back(&nLogLength, sizeof(nLogLength));      // BUFF长度
            obLog.push_back(obData.data(), obData.size());

            SaveToVan(GAMEDB_REQUEST_ACTOR_UPGRADE_LEVEL_LOG, obLog.data(), obLog.size());

            obLog.clear();
            obData.clear();
            log.Num = 0;
            nSize = 0;

            continue;
        }

        DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG & item = (*iter);
        if(item.dwOptTime == 0) item.dwOptTime = nCurTime;
        obData.push_back(&item, sizeof(item));

        nSize += sizeof(item);
        ++log.Num;
    }
    if(log.Num > 0)
    {
        int nLogLength = log.Num * sizeof(DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG);

        obLog.push_back(&log, sizeof(log));            
        obLog.push_back(&nLogLength, sizeof(nLogLength));
        obLog.push_back(obData.data(), obData.size());

        SaveToVan(GAMEDB_REQUEST_ACTOR_UPGRADE_LEVEL_LOG, obLog.data(), obLog.size());

        obLog.clear();
        obData.clear();

        log.Num = 0;
        nSize = 0;
    }

    // 清空当前对象数值数据
    m_lstActorUpgradeLog.clear();
}

//////////////////////////////////////////////////////////////////////////
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
@ param bSingle    : 是否单条记录
*/
void CLogRecord::TrackAddNumeric(BYTE byObjType, int nObjectID, int nTarget, int nNum, BYTE byNumType, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve)
{
    if (nNum == 0 || (byOpType!=OSS_RESOURCE_OPT_ADD && byOpType!=OSS_RESOURCE_OPT_DEL && byOpType!=OSS_RESOURCE_OPT_TRANSIN && byOpType!=OSS_RESOURCE_OPT_TRANSOUT))
    {
        return;
    }

    if (nTime==0)
    {
        time_t	long_time; time(&long_time); nTime=(int)long_time;
    }
    if (byOpType==OSS_RESOURCE_OPT_ADD || byOpType==OSS_RESOURCE_OPT_TRANSIN)
    {
        nNum = abs(nNum);
    }
    else if (byOpType==OSS_RESOURCE_OPT_DEL || byOpType==OSS_RESOURCE_OPT_TRANSOUT)
    {
        nNum = -abs(nNum);
    }

    NUMERIC_VALUE_MAP * pMap = ( byObjType == OSS_OPT_TYPE_PERSON ? &m_mapPlayerValue : &m_mapOtherValue);
    NUMERIC_VALUE_LIST& singleList= (*pMap)[nObjectID];

    NUMERIC_VALUE actorValue;
    actorValue.dwObjectID   = nObjectID;        // 类型ID（角色ID、帮会ID）
    actorValue.nObjType     = byObjType;        // 类型（OSS_OPT_TYPE）
    actorValue.dwReserveID  = nTarget;          // 保留ID(任务的奖励的时候填写任务ID，交易的时候 填写对方玩家的ID等等)
    actorValue.nValueType   = byNumType;        // 数值类型 OSS_NUMERIC_TYPE(1金钱、2绑金、3点卷、4经验) 
    actorValue.nOpType      = byOpType;	        // 操作类型 OSS_RESOURCE_OPT_TYPE(消耗、产生、转进、转出)
    actorValue.wApproach    = wApproach;	    // 途径 OSS_RESOURCE_ADD_RANGE、OSS_RESOURCE_DEL_RANGE、OSS_RESOURCE_TRAN_IN_RANGE、OSS_RESOURCE_TRAN_OUT_RANGE
    actorValue.dwReserveTime= nTime;            // 保留时间
    actorValue.nNum         = nNum;             // 数值数量
    actorValue.dwReserveID2 = dwReserve;        // 保留数值2

    if (szRemark!=NULL)
    {
        // 由于外面进来的都是UTF8字符串，这个里面是批量保存的，所以我们将在这里强制转下码
        sstrcpyn(actorValue.szDesc, utf82a(szRemark), sizeof(actorValue.szDesc));
    }

    singleList.push_back(actorValue);

    // 数量达到上限就保存一次
    if(byObjType == OSS_OPT_TYPE_PERSON)
    {
        if(singleList.size() >= OSS_PLAYER_NUMERIC_ITEM_SAVE_MAXCOUNT)
        {
            SavePersonLog(nObjectID);
        }
    }
    else
    {
        if(singleList.size() >= OSS_OTHER_NUMERIC_ITEM_SAVE_MAXCOUNT)
        {
            SaveNumericLog(nObjectID);
        }
    }
}


/** 保存所有日志
*/
void CLogRecord::SaveAllLog()
{
    // 保存所有人物记录
    for (NUMERIC_VALUE_MAP::iterator iter = m_mapPlayerValue.begin(); iter != m_mapPlayerValue.end(); ++iter)
    {
        SavePersonLog(iter->first);
    }

    // 保存所有非人物记录
    for (NUMERIC_VALUE_MAP::iterator iter = m_mapOtherValue.begin(); iter != m_mapOtherValue.end(); ++iter)
    {
        SaveNumericLog(iter->first);
    }

    // 保存点券日记
    SavePlazaTicketLog();

    // 批量保存玩家升级日记
    BatchSaveActorUpgradeLog();

    // 批量保存玩家物品日记
    BatchSaveActorGoodsLog();
}

/** 保存某人的日志
*/
void CLogRecord::SavePersonLog(DWORD dwMaster)
{
    time_t	long_time; time(&long_time);int nCurTime = (int)long_time;

    obuf obLog;
    DBREQ_PARAM_OSS_MONITOR_ACTORVALUES log;

    obuf obData;
    ///////////////////////////////////////////////////////////////////
    // 保存单条记录
    NUMERIC_VALUE_LIST &singleList = m_mapPlayerValue[dwMaster];
    for(NUMERIC_VALUE_LIST::iterator iter = singleList.begin(); iter != singleList.end(); ++iter)
    {
        if(log.Num >= OSS_PLAYER_NUMERIC_ITEM_SAVE_MAXCOUNT)
        {
            int nLogLength = log.Num * sizeof(DBREQ_OSS_MONITOR_ACTORVALUE);

            obLog.push_back(&log, sizeof(log));            
            obLog.push_back(&nLogLength, sizeof(nLogLength));      // BUFF长度
            obLog.push_back(obData.data(), obData.size());

            SaveToVan(GAMEDB_REQUEST_MONITOR_ACTORVALUES, obLog.data(), obLog.size());

            obLog.clear();
            obData.clear();
            log.Num = 0;

            continue;
        }

        DBREQ_OSS_MONITOR_ACTORVALUE & item = (*iter);
        if(item.dwReserveTime == 0) item.dwReserveTime = nCurTime;
        obData.push_back(&item, sizeof(item));

        ++log.Num;
    }
    if(log.Num > 0)
    {
        int nLogLength = log.Num * sizeof(DBREQ_OSS_MONITOR_ACTORVALUE);

        obLog.push_back(&log, sizeof(log));            
        obLog.push_back(&nLogLength, sizeof(nLogLength));
        obLog.push_back(obData.data(), obData.size());

        SaveToVan(GAMEDB_REQUEST_MONITOR_ACTORVALUES, obLog.data(), obLog.size());

        obLog.clear();
        obData.clear();
        log.Num = 0;
    }
    // 清空当前人数值数据
    m_mapPlayerValue.erase(dwMaster);
}

/** 保存特殊数值日志
*/
void CLogRecord::SaveNumericLog(DWORD dwObjectID)
{
    time_t	long_time; time(&long_time);int nCurTime = (int)long_time;

    obuf obLog;
    DBREQ_PARAM_OSS_MONITOR_ACTORVALUES log;

    obuf obData;
    // 保存单条记录
    NUMERIC_VALUE_LIST &singleList = m_mapOtherValue[dwObjectID];
    for(NUMERIC_VALUE_LIST::iterator iter = singleList.begin(); iter != singleList.end(); ++iter)
    {
        if (log.Num >= OSS_OTHER_NUMERIC_ITEM_SAVE_MAXCOUNT)
        {
            int nLogLength = log.Num * sizeof(DBREQ_OSS_MONITOR_ACTORVALUE);

            obLog.push_back(&log, sizeof(log));            
            obLog.push_back(&nLogLength, sizeof(nLogLength));      // BUFF长度
            obLog.push_back(obData.data(), obData.size());

            SaveToVan(GAMEDB_REQUEST_MONITOR_ACTORVALUES, obLog.data(), obLog.size());

            obLog.clear();
            obData.clear();
            log.Num = 0;

            continue;
        }

        DBREQ_OSS_MONITOR_ACTORVALUE & item = (*iter);
        if(item.dwReserveTime == 0) item.dwReserveTime = nCurTime;        
        obData.push_back(&item, sizeof(item));

        ++log.Num;
    }
    if(log.Num > 0)
    {
        int nLogLength = log.Num * sizeof(DBREQ_OSS_MONITOR_ACTORVALUE);

        obLog.push_back(&log, sizeof(log));            
        obLog.push_back(&nLogLength, sizeof(nLogLength));
        obLog.push_back(obData.data(), obData.size());

        SaveToVan(GAMEDB_REQUEST_MONITOR_ACTORVALUES, obLog.data(), obLog.size());

        obLog.clear();
        obData.clear();
        
        log.Num = 0;
    }
    // 清空当前对象数值数据
    m_mapOtherValue.erase(dwObjectID);
}

//////////////////////////////////////////////////////////////////////////
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
void CLogRecord::TrackAddGoods(BYTE byFromType, long lFromObjID, BYTE byToType, long lToObjID, BYTE nGoodsType, BYTE byOpType, WORD wApproach, int nTypeID, int nNum, int nTime, const char * szRemark, DWORD dwReserveID1, DWORD dwReserveID2)
{
    DBREQ_OSS_ACTOR_GOODS_LOG node;
    node.lFrom      = lFromObjID;					// 物品原主Id  或者怪物的ID,任务Npc,Id
    node.lFromType  = byFromType;					// 物品原主类型（OSS_OPT_TYPE）
    node.lTo        = lToObjID;						// 物品新主Id
    node.lToType    = byToType;						// 物品新主类型（OSS_OPT_TYPE）

    node.nLogType   = nGoodsType;		            // 类型 OSS_GOODS_TYPE
    node.nOpType    = byOpType;                     // 操作类型 OSS_RESOURCE_OPT_TYPE(消耗、产生、转进、转出)    
    node.wApproach  = wApproach;                    // 途径 OSS_RESOURCE_OPT_TYPE各项对应细项

    node.nTypeID    = nTypeID;                      // 类型id
    node.nNum       = nNum;                         // 数量
    node.dwReserveID1 = dwReserveID1;               // 保留ID1
    node.dwReserveID2 = dwReserveID2;               // 保留ID2

    node.dwOptTime    = nTime;                      // 发生时间
    // 描述字符串
    if (szRemark!=NULL)
    {
        // 由于外面进来的都是UTF8字符串，这个里面是批量保存的，所以我们将在这里强制转下码
        sstrcpyn(node.szDesc, utf82a(szRemark), sizeof(node.szDesc));
    }

    //加入本条日志
    m_lstActorGoodsLog.push_back(node);

    DBREQ_PARAM_OSS_ACTOR_GOODS_LOGS saveLog;
    int nLogCount = m_lstActorGoodsLog.size();

    size_t nSize = (nLogCount + 2)*sizeof(DBREQ_OSS_ACTOR_GOODS_LOG) + sizeof(DBREQ_PARAM_OSS_ACTOR_GOODS_LOGS);
    if (nSize >= 7*1024)
    {
        BatchSaveActorGoodsLog();
    }
}

/** 批量保存玩家物品日记
*/
void CLogRecord::BatchSaveActorGoodsLog()
{
    if (m_lstActorGoodsLog.empty())
    {
        return;
    }

    time_t	long_time; time(&long_time);int nCurTime = (int)long_time;

    obuf obLog;
    DBREQ_PARAM_OSS_ACTOR_GOODS_LOGS log;

    size_t nSize = 0;
    obuf obData;

    // 保存单条记录
    for(TLIST_ACTOR_GOODS_LOG::iterator iter = m_lstActorGoodsLog.begin(); iter != m_lstActorGoodsLog.end(); ++iter)
    {
        if (nSize >= 7*1024)
        {
            int nLogLength = log.Num * sizeof(DBREQ_OSS_ACTOR_GOODS_LOG);

            obLog.push_back(&log, sizeof(log));            
            obLog.push_back(&nLogLength, sizeof(nLogLength));      // BUFF长度
            obLog.push_back(obData.data(), obData.size());

            SaveToVan(GAMEDB_REQUEST_ACTOR_GOODS_LOG, obLog.data(), obLog.size());

            obLog.clear();
            obData.clear();
            log.Num = 0;
            nSize = 0;

            continue;
        }

        DBREQ_OSS_ACTOR_GOODS_LOG & item = (*iter);
        if(item.dwOptTime == 0) item.dwOptTime = nCurTime;
        obData.push_back(&item, sizeof(item));

        nSize += sizeof(item);
        ++log.Num;
    }
    if(log.Num > 0)
    {
        int nLogLength = log.Num * sizeof(DBREQ_OSS_ACTOR_GOODS_LOG);

        obLog.push_back(&log, sizeof(log));            
        obLog.push_back(&nLogLength, sizeof(nLogLength));
        obLog.push_back(obData.data(), obData.size());

        SaveToVan(GAMEDB_REQUEST_ACTOR_GOODS_LOG, obLog.data(), obLog.size());

        obLog.clear();
        obData.clear();

        log.Num = 0;
        nSize = 0;
    }

    // 清空当前对象数值数据
    m_lstActorGoodsLog.clear();
}

//////////////////////////////////////////////////////////////////////////
/** 数值改变增加到系统监控
*/
void CLogRecord::AddValueToMonitor(DWORD dwMaster, BYTE byNumType, WORD wApproach, int nNum)
{

}

/*
@name  数值日志异常发出警报
@param  buffer  情况描述
*/
void CLogRecord::NumericLogSendReportToGM(NUMERICLOG_REPORT_INFO& stInfo)
{

}

/*
@name		记录数值信息
@param		byType			告警类型
@param		bySubType		告警子类型
@param		byOprType		操作类型
@param		wApproach		途径类型
@param		dwReserve		保留字段
@param		dwMaster		角色ID
@param		nAddValue		需要增加的值
*/
void CLogRecord::RecordNumericForAlarm(BYTE byType, BYTE bySubType, BYTE byOprType, WORD wApproach, DWORD dwReserve, DWORD dwMaster, int nAddValue)
{

}

/*
@name		获取数据告警阀值
@param		ucType			告警类型
@param		wApproach		途径类型
@param		dwMaster		角色ID
@param		nSingleThsld	单条记录阀值
@param		lTotalThsld		累计记录阀值
@param		strNumeric		数值表述
@param		strApproach		途径表述
*/
bool CLogRecord::GetNumbericAlarmThsld(BYTE ucType, WORD wApproach, DWORD dwMaster, int &nSingleThsld, LONGLONG &lTotalThsld, string &strNumeric, string &strApproach)
{
    return true;
}


/** 保存所有日志
*/
bool CLogRecord::SaveToVan(int nDbCmdID, void* log, size_t nLen)
{
    // TODO 先直接存数据库，后面要调整到统一到其他服务器上存储
    //IVanConnector * pVan = gZoneServiceProvider->GetVanConnector();
    //if (pVan == NULL)
    //{
    //    return;
    //}
    //DWORD nVanID = g_ExternalFacade.GetVanSignalID();
    //if( pVan->Start(EVanRequestType_Oss, nVanID, m_nGameWorldID) == false
    //    || pVan->Request(EVanRequestType_Oss, nVanID, nDbType, nVanID, (LPCSTR)log, nLen) == false
    //    || pVan->Finish(EVanRequestType_Oss, nVanID, true) == false)
    //{
    //    ErrorLn("CLogRecord::SaveToVan 写资源日志到数据前端出错！nCmdID=" << nDbType << " m_nGameWorldID=" << m_nGameWorldID);
    //}

    return g_pOssLogServer->executeSP(nDbCmdID, (LPCSTR)log, (int)nLen);
}


////////////////////////TimerHandler//////////////////////////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@return		      : empty
*/
void CLogRecord::OnTimer( )
{
    time_t	long_time; 
    time(&long_time);
    int curTime = (int)long_time;

    // 角色升级日记
    if (curTime-m_nActorUpgradeLogLastSaveTime >= OSS_SAVE_ACTOR_UPGRADE_INTERVAL)
    {
        BatchSaveActorUpgradeLog();
        m_nActorUpgradeLogLastSaveTime = curTime;
    }

    // 角色物品日记
    if (curTime-m_nActorGoodsLogLastSaveTime >= OSS_SAVE_ACTOR_GOODS_INTERVAL)
    {
        BatchSaveActorGoodsLog();
        m_nActorGoodsLogLastSaveTime = curTime;
    }
}