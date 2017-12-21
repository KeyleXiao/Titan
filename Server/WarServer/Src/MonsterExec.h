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

// 战场中保存购买装备的log 1000毫秒
#define WarBuyMonsterLog  10000
enum
{
    ETimerEventID_SaveMonsterLog,         // 保存日志定时器
};

class IWar;

// 处理野怪
class CMonsterExec : public IMonsterExec, public IEventExecuteSink,public IDBRetSink,public TimerHandler
{
public:
    ///////////////////////////////CMonsterExec///////////////////////////////////////////
    virtual ~CMonsterExec(){}

    // 静态创建
    static CMonsterExec * execCreateMonster(IWar *pCWar, EWarMonsterExec _ExexType);

    ///////////////////////////////IMonsterExec///////////////////////////////////////////
	// 初始化
	// nWarID: 战场ID
	// eType: 怪物处理类型
	//virtual bool Init(CWar *pCWar);
	virtual bool            Init(IWar *pCWar, EWarMonsterExec _ExexType);

	// 时间处理
	virtual void            timeExec();

    // 怪物死亡事件处理
    virtual void            execMonsterDead(sEntityDieInfo entityDieInfo);

    // 外部调用内部影响事件处理
    virtual void            getWarMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize, bool bBorn);

    //获取怪物刷新信息
    virtual MonsterRefreshVect* getMonsterRefresh();

    virtual EWarMonsterExec getMonsterType();

	// 查找配置索引是否存在
	virtual bool            findIndex(int nIndex);

	// 释放资源
	virtual void            destroyAllRes();

    virtual void            release();

    virtual void            sendMiniMapInfo(ClientID clientID);

    /** 获取战场指定类型UID
	@param byGetCampType		: 阵营类型 EWarCampType
    @param bySerchType		    : 查找类型 EWarMonsterExec
	*/
    virtual void            serchCampUIDList(BYTE bySerchCamp,BYTE byGetCampType, CreateMonUIDList& retMap);


    // 停止AI
    virtual void            stopAI();

    //////////////////////////////////IEventExecuteSink////////////////////////////////////////
	virtual void            OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ///////////////////////////////////IDBRetSink///////////////////////////////////////
    virtual void			OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen){};

    /////////////////////////////TimerHandler/////////////////////////////////////////////
    virtual void            OnTimer( unsigned long dwTimerID );

public:
	// 由广播怪通知指定阵营当前进入当前视野野怪重生点的野怪生死状况
	void msgToSpecCampWildInfo(UID uidMonster, int uCamp);

private:

    // 怪物死亡的日志批处理
    bool batchSaveMonsterDieLog();

    void fillMonsterDieLog(UID uMurderID, int nCamp, int nMonstId);

private:
	// 怪物信息
	MonsterRefreshVect                      m_vecMonsterRefresh;
	// 索引怪物累积刷新次数
	RefreshCumulantList                     m_listRefreshCumulant;
	// 战场信息
	IWar *                                  m_pWar;
	// 类型
	EWarMonsterExec                         m_eExecType;
    vector<DBREQ_PARAM_WAR_MONSTERDIE>      m_vecMonsterDieLog; // 怪物死亡log
    LONGLONG                                m_WarDBKey;

    CWarMonsterEventMgr*                    m_pMonsterEventMgr;
};