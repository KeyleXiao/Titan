/*******************************************************************
** 文件名: IWarBase.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/1
** 版  本: 1.0
** 描  述: 战场基类-实现基本的功能函数
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "EntityDef.h"
#include "IWarService.h"
#include "SchemeWarDef.h"
#include "WarMessageDef.h"
#include "IMonsterExec.h"
#include "ICamp.h"
#include "ITimeSyncService.h"
#include "IServerGlobal.h"
#include "WarDef.h"
#include "IWar.h"
#include "WarDef_Internal.h"

class IWarBase : public IWar
{
public:
	IWarBase();
	virtual ~IWarBase();

	/////////////////////////////////IWar Start/////////////////////////////////////////
	// 是否结束
	virtual bool isEnd();

	// 获得胜利条件
	virtual EWarEndReason getWarEndReason();

	virtual void setWarEndReason(EWarEndReason eReason);

	//// 获取战场所有人物UID
	//virtual int getAllPerson(UID *PersonArray, int nArraySize) { return 0; };
	// 获取场景ID
	virtual int getWarSceneID();
	// 获得战场类型ID
	virtual int getWarTypeId();
	//// 初始化
	virtual bool Init();

	//// 反初始化
	virtual void UnInit();

	virtual void Start();

	// 获得战场持续的时间
	virtual DWORD getWarContinueTime();

	// 广播玩家信息
	virtual bool broadCastDataToAllPlayer(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp = CAMP_MAX);

	// 广播给其他阵营玩家 除了 nCamp 阵营 其他全部广播
	virtual bool broadCastDataToAllOtherCamp(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp = CAMP_MAX);

	// 广播给有视野的玩家信息
	virtual bool broadCastDataToHaveSightPlayer(Vector3 vec, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

	virtual const SchemeWarPersonConfigInfo * getPersonConfigInfo();

	virtual int getStaticID();

	virtual int getSceneID();

	// 获得战场的创建序列号 创建怪物使用
	virtual int getMapID();

	virtual int getWarID();

	virtual DWORD  getMatchTypeID();

	// 获得战场DBKey
	virtual LONGLONG getWarDBKey();

	//// 战场游戏时间（正式开始后计时）
	virtual DWORD getWarPlayTime();

    // 战场正常结束标示
	virtual void setNormalEnd(bool bNormalEnd);

	//// 某个阵营是否有真人玩家
	virtual bool isHaveRealPlayer(int nCamp);

	// 根据uid获得玩家当前的信息
	virtual SWPerson* getPlayerInfo(UID uid);

	/////////////////////////////////IWar End/////////////////////////////////////////
protected:
	// 获取显示添加经验金币配置
	bool                        getWarShowAddEXPAndMoney() { return m_bIsWarShowAddEXPAndMoney; };

private:
	// 生成战场唯一id
	LONGLONG createDBWarId(int nSceneId);

protected:

	volatile LONG           m_lMutex;
	LONGLONG                m_dwDBWarID;                                        // 战场数据库唯一id
	IWarService*            m_pWarService;
	SWarCreateContext       m_context;                                          // 战场创建上下文
	int                     m_nSceneID;                                         // 战场场景ID
	int                     m_nStartTimeTick;                                   // 战场正式开始时间（从第一个玩家进入战场开始计算）

	EWarPhase               m_WarPhase;                                         // 战场状态
	bool                    m_bIsStart;                                         // 战场是否已开启
	DWORD                   m_BeginTime;                                        // 战场创建时间(单位为秒)
    DWORD                   m_continueTime;                                     // 总的比赛时间(单位为秒)
	int                     m_nPersonNum;                                       // 进入的玩家个数
	WarCampMap              m_mapWarCamp;                                       // 战场阵营信息
    bool                    m_bIsFinish;                                        // 比赛是否结束

    EWarEndReason           m_EndWarReason;

    bool                    m_bNormalEnd;										// 是否正常结束

	map<PDBID, SPendingCreateActor>             m_mapPendingCreateActor;        // 准备进入战场的角色


    std::map<PDBID, WarPersonVector::iterator> m_mapPDBID2IterList;             // 角色PDBID对应人物列表迭代器列表，用于提高查找人性能
    std::map<UID, WarPersonVector::iterator> m_mapUID2IterList;                 // 角色UID对应人物列表迭代器列表，用于提高查找人性能

	bool                    m_bIsWarShowAddEXPAndMoney;                         // 战场经验天赋打印开关

	map<int, SCampTeamRecordInfo>               m_mapCampTeamRecord;			// 战后不同阵营的团队信息
};
