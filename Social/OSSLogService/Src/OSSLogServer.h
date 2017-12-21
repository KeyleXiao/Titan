/*******************************************************************
** 文件名:	OSSLogServer.h 
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

#pragma once

#include "IOSSLogServer.h"
#include "IDBEngineService.h"
#include "IServiceContainer.h"
#include "TimerHandler.h"
#include "IEventEngine.h"

class CLogRecord;

// TODO 第一版先直接存数据库，后面再分布式处理这个，以分解存数据库时导致场景上的压力
// (Operating Support System)运营支持系统日志
class COSSLogServer : public IOSSLogServer, public IDBRetSink, public IEventExecuteSink, public TimerHandler
{
    enum
    {
        OSS_SAVE_TIMERID = 0,       // 存盘定时器ID
    };
	
	// 5分种触发一次
    #define  OSS_REPORT_INTERVAL        5*60*1000

public:
	///////////////////////IOSSLogServer/////////////////////////////////
	/** 执行释放Service
	@return  
	*/
    virtual void onStop();

	/** 释放
	@return  
	*/
    virtual void release();

	/** 记录文字型日志
	@param   nNoteTrackType ：OSS_NOTE_TRACK_TYPE
	@param   dwIndex ：索引，各个类型不同，索引也不一样：参考OSS_NOTE_TRACK_TYPE对应数值
	@param   pszNote ：描述
	@return  
	*/
	virtual void addGameNoteLog(int nGameWorldID, int nNoteTrackType, DWORD dwIndex, LPCSTR pszNote);

	/** 商城购买消耗
	@ name   : 点券日志
	@ param pActorTicketLog: 日志的详细信息 DBREQ_OSS_MONITOR_ACTORTICKET_ITEM
	*/
	virtual void addPlazaTicketLog(int nGameWorldID, void* pActorTicketLog, size_t nLogSize);

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
	virtual void addTicketLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0);

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
    virtual void transTickLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0);

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
	virtual void addMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0 );

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
	virtual void transMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0 );

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
	virtual void addBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0 );

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
	virtual void transBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0 );

    /**	记录经验日志
    */
    virtual void addExpLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0);

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
	virtual void addNumericLog(int nGameWorldID, BYTE byObjType, int nObjectID, int nTarget, int nNum, BYTE byNumType, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0);

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
	virtual void addActorGoodsLog(int nGameWorldID, BYTE byFromType, long lFromObjID, BYTE byToType, long lToObjID, BYTE nGoodsType, BYTE byOpType, WORD wApproach, int nTypeID, int nNum, int nTime, const char * szRemark=0, DWORD dwReserveID1=0, DWORD dwReserveID2=0);

    /** 玩家升级日记
	@ param nGameWorldID: 游戏世界ID
	@ param dwUserID   : 玩家账号ID
	@ param dwActorID  : 玩家角色ID
	@ param nUpgradeType: 升级类型（1=等级，2=段位，3=英雄评级）
	@ param dwValue	   : 数值
	@ param nTime      : 发生时间
	@ param dwReserve  : 保留数据（英雄评级时为英雄ID）
	*/
	virtual void addActorUpgradeLog(int nGameWorldID, DWORD dwUserID, DWORD dwActorID, BYTE nUpgradeType, DWORD dwValue, int nTime, DWORD dwReserve = 0);

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
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

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
	virtual void						OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////////TimerHandler//////////////////////////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void                        OnTimer( unsigned long dwTimerID );

	///////////////////////COSSLogServer/////////////////////////////////

    // 创建
    bool create(void);

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
	bool executeSP(DWORD dwCmdID, LPCSTR pszInData, int nInDataLen, int nGameWorldID=0);

    // 创建服务
    bool createService(const char * pszServiceSchemeName);

	// 构造
	COSSLogServer(void);

	// 析构
	~COSSLogServer(void);

private:
    /** 取日志记录管理器
    @param   
    @param   
    @return  
    */
    CLogRecord* getLogRecord(int nGameWorldID, const char* file, int line);

	/** 取操作类型 OSS_RESOURCE_OPT_TYPE
	*/
	BYTE getOpType(WORD wApproach);

private:
    typedef map<int, CLogRecord*> TMAP_LOGRECORD;
    TMAP_LOGRECORD                  m_logRecordMap;

    SERVICE_PTR                     m_pService;
};

extern COSSLogServer	*		g_pOssLogServer;
