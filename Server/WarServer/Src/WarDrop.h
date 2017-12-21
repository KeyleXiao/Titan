/*******************************************************************
** 文件名: WarDrop.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/10/31
** 版  本: 1.0
** 描  述: 战场掉落相关
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "GameDef.h"
#include "WarDef.h"
#include "SchemeDef.h"
#include "WarMessageDef.h"
#include "SchemeWarDef.h"
#include "WarDef_Internal.h"
#include "IWarDropMgr.h"
#include <map>

class IWar;
struct SWPerson;
class IAIExpCtrlMgr;
class IAIEcmCtrlMgr;
class ILogRecordMgr;
class IEcmCtrlMgr;
class ILvExpCtrlMgr;
class IExpCtrlMgr;
class IWarMiscManager;

class CWarDrop : public IWarDropMgr
{
public:
	CWarDrop(IWar* pWar);
	~CWarDrop();

public:
	////////////////////////IWarDropMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

    // 增加天赋点
    virtual void addPersonTalentValue(UID realMurderID, UID uMurderID,int nMurderCamp, EWarTalentType TalentType,Vector3 vLoc,int nDistans, UID nMonsterID,int nEXP=0,int nAdd = 0, float fDecayFactor = 0.0f, ETalentDropType talentDropType = ETALENTDROPTYPE_SOLIDER);

    // 增加经验值
	virtual bool addHeroEXP(EWarAddEXPType AddExpType,UID MurderID,int nMurderCamp, Vector3 vLoc, int nDis, int nAddValue = 0, EExpDropType expDropType = EEXPDROPTYPE_SOLIDER );

    // 添加buff
	virtual bool addHeroBuffValue(EWarBuffType AddBuffType,UID MurderID,int nMurderCamp, Vector3 vLoc, int nDis, int nBuffValue = 0, int nLevel =0);

	// 处理玩家死亡
	virtual void dealPersonDie(sEntityDieInfo entityDieInfo, bool bBcastTips = true);

	// 设置整场战斗的最高不死连杀和时间连杀
	virtual void setKillMaxInfo(SWPerson* pPerson);

	////////////////////////IWarDropMgr end/////////////////////////
private:
	//////////////////////////////////////人物死亡相关功经验天赋点处理//////////////////////////////////////////////////////////
	// 处理助攻相关
	void dealWithAssistsInfo(SWPerson *EntiyDie, SWPerson *EntiyMurder, UID uMurder, vector<msg_war_entity_assist_info>& assistInfoList);

	int calculatePersonAddExpAndTalentPoint(EWarGetPersonDieEXPType nType, UID DeadPerson, UID AttacktPerson, int PersonCount, EWarDropType WarDropType, int KillOrDeadCount, bool isFirstKill, int nAroundCount = 1);

	// 处理助攻经验天赋相关
	void dealWithAssistsEXPAndTalentPoint(SWPerson *EntiyDie, UID uMurder, int nAssistPersonCount, EWarDropType WarDropType, int KillOrDeadCount, bool isFirstKill);

	// 处理被击杀周围人物相关
	void dealWithAroundEXPAndTalentPoint(SWPerson *EntiyDie, UID uMurder, int nAroundPersonCount, EWarDropType WarDropType, int KillOrDeadCount, bool isFirstKill);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////基础类添加天赋点处理////////////////////////////////////////////////////////////////
	// 检查玩家属性标志位是否可加钱
	bool checkPersonCanAddTalentInPropertyMask(int nPropertyMask, int nDropType);

	// 发出击击杀怪物事件
	void sendPersonKillMonsterEvent(UID uMurderId, UID uDeadId, int nEXP, int nTalentValue);

	// 击杀者可以是玩家和怪物 周围最近的一个人加钱
	void addPersonAloneTalentValue(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, ETalentDropType talentDropType);

	// 范围添加天赋点
	void addPersonAroundTalentValue(UID realMurderID, UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, ETalentDropType talentDropType);

	// 阵营加钱
	void addPersonCampTalentValue(UID uMurderID, int nMurderCamp, int nAdd, ETalentDropType talentDropType);

	// 击杀者只能是玩家 给击杀者加钱
	void addPersonMurderTalentValue(UID uMurderID, int nMurderCamp, int nAdd, ETalentDropType talentDropType);

	// 击杀者可以是玩家和怪物 击杀者和周围人加钱是互斥的
	void addPersonMurderEXTalentValue(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, ETalentDropType talentDropType, float fDecayFactor = 0.0f);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////基础类添加添加buff处理//////////////////////////////////////////////////////////////
	void addPersonAloneBuffValue(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, int nLevel);

	void addPersonAroundBuffValue(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, int nLevel);

	void addPersonCampBuffValue(UID uMurderID, int nMurderCamp, int nAdd, int nLevel);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////基础类添加经验值方式///////////////////////////////////////////////////////////////
	void addPersonAloneEXP(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, EExpDropType expDropType);

	void addPersonAroundEXP(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, EExpDropType expDropType);

	void addPersonCampEXP(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, EExpDropType expDropType);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
	// 发出击杀归属事件
	void sendKillBelongsEvent(UID uMurderId, UID uDeadId);

	// 发出助攻事件
	void sendAssistEvent(UID uId, UID uDeadId, UID uKillId, int nAssistCount);

private:
	// 给客户端发送消息把连杀图片显示出来
	void sendToClientShowKillIcon(UID uMurderId, int nKillCount);

private:
	// 战场
	IWar*	m_pWar;

	WarCampMap*	m_pMapWarCamp;

	// AI经济控制
	IAIEcmCtrlMgr* m_pAIEcmCtrl;
	// AI经验控制
	IAIExpCtrlMgr* m_pAIExpCtrl;
	// 战场日志
	ILogRecordMgr* m_pWarLogRecord;
	// 真人经济控制
	IEcmCtrlMgr* m_pEcmCtrl;
	// 等级经验控制
	ILvExpCtrlMgr* m_pLvExpCtrl;
	// 战场杂项管理
	IWarMiscManager* m_pMiscMgr;

	bool m_bFirstBlood;
};