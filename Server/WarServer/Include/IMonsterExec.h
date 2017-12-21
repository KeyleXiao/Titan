/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 陈俊涛
** 日  期: 2015/3/12
** 版  本: 1.0
** 描  述: 战场 怪物处理的接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "StdAfx.h"
#include "SchemeWarDef.h"
#include "ISchemeCenter.h"
#include "EntityHelper.h"
#include "WarMessageDef.h"
#include "Event_ManagedDef.h"
#include "IAIService.h"
#include <vector>
#include <list>
using namespace std;

typedef map<UID, int> CreateMonUIDList;

typedef map<UID, int> RefreshCumulantList;

struct SMonsterRefreshInfo
{
	// 刷新的信息
	SWarInfoScheme RefreshWarInfo;

	// 是否刷新
	bool bIsOnRefresh;

	// 刷新时间
	int nRefreshTime;

	// 最近刷新时间
	DWORD nLatestRefreshTime;

	// 具体的怪物信息
    map<int,WarPointVect> mapWarPointInfo;

    // 当前刷新的怪物列表第几个位置
    int nRefreshIndex;

    // 当前刷新的第几个位置
    int nPosIndex;

	// 创建的怪物列表
	CreateMonUIDList creatMonUIDList;

	// 是否刷新
	bool bEventHaveChanged;

	int nMurderCamp;

	bool bIsFirstRefresh;

	bool bIsFirstTableRefresh;

	// 野怪生存情况已广播阵营
	bool bArrayBroadcastCamp[SCHAR_MAX];

	// 广播怪UID
	UID uBroadcastMonsterUid;

	SMonsterRefreshInfo()
	{
		bIsOnRefresh = false;
		nRefreshTime = 0;
		nLatestRefreshTime = 0;
		bEventHaveChanged = false;
		nMurderCamp = 0;
		bIsFirstRefresh = true;
		bIsFirstTableRefresh = true;
		uBroadcastMonsterUid = 0;
        nRefreshIndex = -1;
	}
};

typedef vector<SMonsterRefreshInfo> MonsterRefreshVect;

class IWar;

// 怪物处理接口，必须实现 
// 继承事件接口，用于订阅 怪物死亡事件
class IMonsterExec
{
public:
	// 初始化
	virtual bool Init(IWar *pCWar, EWarMonsterExec _ExexType) = 0;

	// 时间处理
	virtual void timeExec() = 0;

	

	// 怪物死亡事件处理
	virtual void execMonsterDead(sEntityDieInfo entityDieInfo) = 0;

	// 外部调用内部影响事件处理
	virtual void getWarMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize, bool bBorn = true) = 0;

	// 查找配置索引是否存在
	virtual bool findIndex(int nIndex) = 0;

	// 释放资源
	virtual void destroyAllRes() = 0;

    // 释放
    virtual void release() = 0;

    // 发送小地图信息
    virtual void sendMiniMapInfo(ClientID clientID) = 0;

    //获取战场指定类型UID
    virtual void serchCampUIDList(BYTE bySerchCamp,BYTE byGetCampType, CreateMonUIDList& retMap) = 0;

    //获取怪物刷新信息
    virtual MonsterRefreshVect* getMonsterRefresh() = 0;

    //获取类型信息
    virtual EWarMonsterExec getMonsterType() = 0;

    // 停止AI
    virtual void stopAI() = 0;
};