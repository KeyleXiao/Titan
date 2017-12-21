/*******************************************************************
** 文件名:	IOSSLogService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-7-19
** 版  本:	1.0
** 描  述:	日记服务接口


********************************************************************/


#ifndef __OSSLOGSERVER_INCLUDE_I_OSSLOGSERVICE_H__
#define __OSSLOGSERVER_INCLUDE_I_OSSLOGSERVICE_H__



// 接口
struct IOSSLogService
{

	/** 记录文字型日志
	@param   nNoteTrackType: OSS_NOTE_TRACK_TYPE
	@param   dwIndex ：索引，各个类型不同，索引也不一样：参考OSS_NOTE_TRACK_TYPE对应数值
	@param   pszNote ：描述
	@return  
	*/
	virtual void addGameNoteLog(int nGameWorldID, int nNoteTrackType, DWORD dwIndex, LPCSTR pszNote) = 0;

	/** 商城购买消耗
	@ name   : 点券日志
	@ param pActorTicketLog: 日志的详细信息 DBREQ_OSS_MONITOR_ACTORTICKET_ITEM
	*/
	virtual void addPlazaTicketLog(int nGameWorldID, void* pActorTicketLog, size_t nLogSize) = 0;

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
	virtual void addTicketLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0) = 0;

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
    virtual void transTickLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0) = 0;

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
	virtual void addMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0 ) = 0;

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
	virtual void transMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0 ) = 0;

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
	virtual void addBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0 ) = 0;

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
	virtual void transBindMoneyLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0 ) = 0;

    /**	记录经验日志
    */
    virtual void addExpLog(int nGameWorldID, DWORD dwMaster, int nTarget, int nNum, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0) = 0;

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
	virtual void addNumericLog(int nGameWorldID, BYTE byObjType, int nObjectID, int nTarget, int nNum, BYTE byNumType, BYTE byOpType, WORD wApproach, int nTime, const char * szRemark, DWORD dwReserve = 0) = 0;


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
	virtual void addActorGoodsLog(int nGameWorldID, BYTE byFromType, long lFromObjID, BYTE byToType, long lToObjID, BYTE nGoodsType, BYTE byOpType, WORD wApproach, int nTypeID, int nNum, int nTime, const char * szRemark=0, DWORD dwReserveID1=0, DWORD dwReserveID2=0) = 0;

    /** 玩家升级日记
	@ param nGameWorldID: 游戏世界ID
	@ param dwUserID   : 玩家账号ID
	@ param dwActorID  : 玩家角色ID
	@ param nUpgradeType: 升级类型（1=等级，2=段位，3=英雄评级）
	@ param dwValue	   : 数值
	@ param nTime      : 发生时间
	@ param dwReserve  : 保留数据（英雄评级时为英雄ID）
	*/
	virtual void addActorUpgradeLog(int nGameWorldID, DWORD dwUserID, DWORD dwActorID, BYTE nUpgradeType, DWORD dwValue, int nTime, DWORD dwReserve = 0) = 0;
};


struct IServiceStub;
extern "C" __declspec(dllexport) IServiceStub * CreateOSSLogService();

#endif  // __OSSLOGSERVER_INCLUDE_I_OSSLOGSERVICE_H__