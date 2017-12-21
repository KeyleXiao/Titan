/*******************************************************************
** 文件名:	LogRecord.h 
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


#ifndef __OSS_SRC_LOGRECORD_H__
#define __OSS_SRC_LOGRECORD_H__


#include <map>
#include <list>
#include "OssLogDef.h"

#include <strstream>

// 数值警报信息结构
struct NUMERICLOG_REPORT_INFO  
{
	DWORD dwMaster;            //玩家DBID
	char szNumericName[64];    //数值名 ： 绑金数值 金币数值 经验数值 翅膀经验数值 金钱数值 绑定金钱数值 声望数值
	int nNum;                  //本次操作数值数
	LONGLONG nTotalNum;        //多次合计数值数
	LONGLONG NumericMaxNum;    //数值阀值
	WORD wApproach;            //操作数值途径编号
	int subClass;              //用于存声望子类类型
	char szApproachDesc[128];  //操作数值途径

	NUMERICLOG_REPORT_INFO(void)
	{
		memset(this,0,sizeof(*this));
	}
};


// 数值告警统计关键字
struct SNumAlarmStatisticsKey
{
	BYTE	byType;				// 数值类型EMNumericAlarm_Type_Max
	BYTE	bySubType;			// 子类型(目前仅当byType为声望时有效)
	BYTE	byOprType;			// 操作类型
	WORD	wApproach;			// 途径
	DWORD	dwReserve;			// 保留字段
	DWORD	dwMaster;			// 角色ID

	bool operator< (const SNumAlarmStatisticsKey &key) const
	{
		if (byType != key.byType)
		{
			return byType < key.byType;
		}
		if (bySubType != key.bySubType)
		{
			return bySubType < key.bySubType;
		}
		if (byOprType != key.byOprType)
		{
			return byOprType < key.byOprType;
		}
		if (wApproach != key.wApproach)
		{
			return wApproach < key.wApproach;
		}
		if (dwReserve != key.dwReserve)
		{
			return dwReserve < key.dwReserve;
		}
		if (dwMaster != key.dwMaster)
		{
			return dwMaster < key.dwMaster;
		}
		return false;
	}

	SNumAlarmStatisticsKey(BYTE type = 0, BYTE subtype = 0, BYTE oprtype = 0, WORD approach = 0, DWORD reserve = 0, DWORD master = 0)
		:byType(type)
		,bySubType(subtype)
		,byOprType(oprtype)
		,wApproach(approach)
		,dwReserve(reserve)
		,dwMaster(master)
	{
	}
};

// 数值告警统计信息
struct SNumAlarmStatisticsValue
{
	LONGLONG	lValue;			// 具体数值
	DWORD		dwRecordTime;	// 日志记录时间

	SNumAlarmStatisticsValue()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 数值告警统计关键字-统计信息映射表
typedef map<SNumAlarmStatisticsKey, SNumAlarmStatisticsValue> TMap_NumAlarmStatistics;


class CLogRecord
{
    // 玩家点券日志列表
    typedef list<DBREQ_OSS_MONITOR_ACTORTICKET_ITEM> OSS_TICKET_LIST;

    // 数值
    typedef DBREQ_OSS_MONITOR_ACTORVALUE        NUMERIC_VALUE;
    // 单条数值记录
    typedef list<NUMERIC_VALUE>                 NUMERIC_VALUE_LIST;
    // 单项数据存储
    typedef map<DWORD, NUMERIC_VALUE_LIST>      NUMERIC_VALUE_MAP;

    // 玩家物品日记
    typedef list<DBREQ_OSS_ACTOR_GOODS_LOG>             TLIST_ACTOR_GOODS_LOG;

    // 玩家升级日记
    typedef list<DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG>     TLIST_ACTOR_UPGRADE_LOG;

	// 玩家打造日记
	typedef list<DBREQ_OSS_ACTOR_GOOD_FROGE_LOG>     TLIST_ACTOR_FROGE_LOG;

public:
    /** 记录点券详细日志（购买日记）
	*/
	void RecordPlazaTicket(DBREQ_OSS_MONITOR_ACTORTICKET_ITEM* pActorTicketLog);

	/**	记录点券日志
	*/
	void RecordTicket(DWORD dwMaster, int nTarget, int nNum, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve);

	/**	记录金钱日志
	*/
	void RecordMoney(DWORD dwMaster, int nTarget, int nNum, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve);

	/**	记录绑定金钱日志
	*/
	void RecordBindMoney(DWORD dwMaster, int nTarget, int nNum, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve);

    /**	记录经验日志
    */
    void RecordExp(DWORD dwMaster, int nTarget, int nNum, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve);

	/**	记录玩家升级日记
	*/
	void RecordActorUpgrade(DWORD dwUserID, DWORD dwActorID, BYTE nUpgradeType, DWORD dwValue, int nTime, DWORD dwReserve);

	/**	记录玩家打造日记
	*/
	void RecordActorForge( DWORD dwUserID, DWORD dwActorID, BYTE nOperateType, DWORD dwGoodID, const char * szGoodName, int nTime, const char * szForgeDesc, DWORD dwReserve );

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
	void TrackAddNumeric(BYTE byObjType, int nObjectID, int nTarget, int nNum, BYTE byNumType, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve);

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
	void TrackAddGoods(BYTE byFromType, long lFromObjID, BYTE byToType, long lToObjID, BYTE nGoodsType, BYTE byOpType, WORD wApproach, int nTypeID, int nNum, int nTime, const char * szRemark=0, DWORD dwReserveID1=0, DWORD dwReserveID2=0);



    /** 保存所有日志
    */
    void SaveAllLog();

	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@return		      : empty
	*/
	void OnTimer( );

private:
    /** 数值改变增加到系统监控
    */
    void AddValueToMonitor(DWORD dwMaster, BYTE byNumType, WORD wApproach, int nNum);

	/*
	@name  数值日志异常发出警报
	@param  buffer  情况描述
	*/
	void NumericLogSendReportToGM(NUMERICLOG_REPORT_INFO& stInfo);

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
	void RecordNumericForAlarm(BYTE byType, BYTE bySubType, BYTE byOprType, WORD wApproach, DWORD dwReserve, DWORD dwMaster, int nAddValue);

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
	bool GetNumbericAlarmThsld(BYTE ucType, WORD wApproach, DWORD dwMaster, int &nSingleThsld, LONGLONG &lTotalThsld, string &strNumeric, string &strApproach);

	/** 保存点券
	*/
	bool SavePlazaTicketLog();

    /** 保存某人的日志
	*/
	void SavePersonLog(DWORD dwMaster);

	/** 保存特殊数值日志
	*/
	void SaveNumericLog(DWORD dwObjectID);

    /** 保存日志
    */
    bool SaveToVan(int nDbCmdID, void* log, size_t nLen);

    /** 批量保存玩家升级日记
    */
    void BatchSaveActorUpgradeLog();

    /** 批量保存玩家物品日记
    */
    void BatchSaveActorGoodsLog();
	
	  /** 批量保存玩家打造日记
    */
	void BatchSaveActorFrogeLog();

public:

    bool Create(int nGameWorldID);

    void Release();

    CLogRecord();
private:

    // 游戏世界ID
	int                 m_nGameWorldID;

    // 玩家购买点券详细日志列表
    OSS_TICKET_LIST     m_listPlazaTicket;

    // 人物细项数值日记
	NUMERIC_VALUE_MAP   m_mapPlayerValue;

    // 其它数值日志
    NUMERIC_VALUE_MAP   m_mapOtherValue;


    // 玩家物品日记
    TLIST_ACTOR_GOODS_LOG       m_lstActorGoodsLog;
    // 上次玩家物品存盘时间
    int						    m_nActorGoodsLogLastSaveTime;

    // 玩家升级日记
    TLIST_ACTOR_UPGRADE_LOG		m_lstActorUpgradeLog;

	// 玩家打造日记
	TLIST_ACTOR_FROGE_LOG		m_lstActorFrogeLog;

    // 上次玩家升级日记存盘时间
    int						    m_nActorUpgradeLogLastSaveTime;
};

#endif// __LOG_RECORD_H__