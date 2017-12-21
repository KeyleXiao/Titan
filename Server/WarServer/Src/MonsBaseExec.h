/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 陈俊涛
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场信息
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "IMonsterExec.h"
#include "WarMonsterEventMgr.h"

class IWar;

// 处理基地的操作。
class CMonsBaseExec : public IMonsterExec, public IEventExecuteSink
{
public:
    //////////////////////////////////CMonsBaseExec////////////////////////////////////////
    virtual ~CMonsBaseExec(){}

    // 静态创建
    static CMonsBaseExec * execCreateMonster(IWar *pCWar, EWarMonsterExec _ExexType);

    /////////////////////////////////IMonsterExec/////////////////////////////////////////
	// 初始化
	// nWarID: 战场ID
	// eType: 怪物处理类型
	//virtual bool Init(CWar *pCWar);
	virtual bool Init(IWar *pCWar, EWarMonsterExec _ExexType);

	// 时间处理
	virtual void timeExec();

	// 怪物死亡事件处理
	virtual void execMonsterDead(sEntityDieInfo entityDieInfo);

	// 外部调用内部影响事件处理
	virtual void getWarMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize, bool bBorn);

    // 获取怪物刷新信息
    virtual MonsterRefreshVect* getMonsterRefresh();

    virtual EWarMonsterExec getMonsterType();

    // 查找配置索引是否存在
    virtual bool findIndex(int nIndex);

    // 释放资源
    virtual void destroyAllRes();

    virtual void release();


    virtual void sendMiniMapInfo(ClientID clientID);

    /** 获取战场指定类型UID
	@param byGetCampType		: 阵营类型 EWarCampType
    @param bySerchType		    : 查找类型 EWarMonsterExec
	*/
    virtual void serchCampUIDList(BYTE bySerchCamp, BYTE byGetCampType, CreateMonUIDList& retMap);

    // 停止AI
    virtual void stopAI();

    //////////////////////////////////IEventExecuteSink////////////////////////////////////////
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

private:

	// 战斗受到的伤害
	void onGetDamage(event_entity_damage damage);

private:
	// 怪物信息
	MonsterRefreshVect      m_vecMonsterRefresh;
	// 索引怪物累积刷新次数
	RefreshCumulantList     m_listRefreshCumulant;
	// 战场信息
	IWar*                   m_pWar;
	// 类型
	EWarMonsterExec         m_eExecType;
	// 记录最后小地图战斗信息时间
	map<UID,DWORD>          m_mapUidMiniMapInfoRecord;

    CWarMonsterEventMgr*     m_pMonsterEventMgr;
};