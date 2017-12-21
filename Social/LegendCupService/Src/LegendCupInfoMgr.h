//==========================================================================
/**
* @file	  : LegendCupInfoMgr.h
* @author : 秦其勇
* created : 2010-4-29   
* purpose : 举办的杯赛管理
*/
//==========================================================================

#ifndef __LEGENDCUP_INFO_H__
#define __LEGENDCUP_INFO_H__

#include <vector>
#include <hash_map>
#include "ISocialGlobal.h"
#include "IDBEngineService.h"
#include "LegendCup.h"
#include "ExternalFacade.h"
class CLegendCup;
class CLegendCupInfoMgr
{	
public:
	CLegendCupInfoMgr();
	virtual ~CLegendCupInfoMgr();
	
	void                    release();

    void                    inital(ELegendCupType byCreateLegendCupType);

    // 添加杯赛信息到管理类
    bool                    addLegendCup(CLegendCup* pLegendCup);

    // 添加比赛很名单
    void                    addToBlackList(SMsgAddToBlackList* pMsgInfo);

    // 移除比赛很名单
    void                    removeBlackList(SMsgRemoveBlackList* pMsgInfo);

    // 发布比赛
    void                    publicCompetition(SMsgPublicCompetition* pMsgInfo);

    // 退出比赛申请
    void                    exitRegisterCup(SMsgExitCup* pMsgInfo);

    // 申请加入杯赛检查
    void                    registerCupCheck(SMsgJoinCup* pMsgInfo);

    // 加入杯赛申请列表
    void                    addRegisterCup(SMsgJoinCup* pMsgInfo);

    // 线下结束报名
    void                    offlineEndRegister(SMsgOfflineEndRegister* pMsgInfo);

    // 添加战队对应的LegendCupID(防止重复申请)
    void                    addCupAllKinMapedLegendID(CLegendCup *pLegendCup);

    // 查找比赛
    CLegendCup*             findCup(LONGLONG llLegendCupID);

    // 返回杯赛基础信息buf 和 数量
    CLegendCup*             findKinJoinedLegendCup( DWORD dwKinID);

    // 返回杯赛基础信息buf 和 数量
    int                     getAllCupBaseInfo(obuf &out, DWORD dwRequestKinID);

    // 移除杯赛信息
    void                    endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID);


    // 移除杯赛所有相关信息
    void                    removeServiceCupInfo(LONGLONG llLegendCupID);

    // 能否创建杯赛
    bool                    canCreateLegendCup(SMsgCreateCup* pCreateCup);

    // 获取创建杯赛信息
    SCreaterLegendCupInfo    findCreateIDLegendCupInfo(DWORD dwCreateID);

    // 玩家登出
    void                    onActorLogOut(DWORD dwActorID);


private:
    // 能否申请加入比赛
    bool    canRequestJoinCup(DWORD dwKinID);
    
    // 移除战队映射
    void    removeKinMapedLegendID(DWORD dwKinID);

    // 添加战队映射比赛ID
    void    addKinMapedLegendID(DWORD dwKinID, LONGLONG llLegendCupID);

    // 移除杯赛对应的所有战队映射
    void    removeLegendCupIDAllKinMap(LONGLONG llLegendCupID);

    // 移除杯赛对应的创建者映射
    void    removeLegendCupIDCreaterMap(LONGLONG llLegendCupID);

    // 移除释放杯赛实例
    void    releaseLegendCup(LONGLONG llLegendCupID);

private:
    ELegendCupType m_byCreateLegendCupType;

    //战队ID  to  杯赛ID
    map<DWORD, LONGLONG> m_mapKin2LegendID;

    // 杯赛ID 对应的杯赛相关信息
    map<LONGLONG, CLegendCup*> m_mapLegendID2Legend;

    // 创建者ID（系统赛: 不记录 个人赛: ActorID 联盟赛: clanID）  to  杯赛ID
    map<DWORD, LONGLONG> m_mapCreateIDHaveLegendCup;

};
#endif