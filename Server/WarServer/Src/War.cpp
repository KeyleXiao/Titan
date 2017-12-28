#include "StdAfx.h"
#include "War.h"
#include "WarMessageDef.h"
#include "IEntity.h"
#include "EntityHelper.h"
#include "MatchSceneHelper.h"
#include "IWarManager.h"
#include "IPathFindService.h"
#include "MatchSchemeDef.h"
#include "WarStaticMessageDef.h"
#include "StaticWarSceneHelper.h"
#include "MentorshipHelper.h"
#include "Mentorship_ManagedDef.h"
#include "EconomyControl.h"
#include "Support.h"
#include "XPSkill.h"
#include "SoliderVigour.h"
#include "LvExpControl.h"
#include "ForceEnd.h"
#include "ClanHelper.h"
#include "KinHelper.h"
#include "WarAddBuff.h"
#include "WarDrop.h"
#include "WarCheckBadBehavior.h"
#include "ICenterConnectorService.h"
#include "LegendCupDef.h"
#include "PathFindHelper.h"
#include "ITimerService.h"
#include "IOSSLogService.h"
#include "Vector3.h"
#include "GUID.h"
#include "WarLogRecord.h"
#include "ExternalHelper.h"
#include "AIEcmCtrl.h"
#include "AIExpCtrl.h"
#include "DamageDef.h"
#include "IAIEcmCtrlMgr.h"
#include "IAIExpCtrlMgr.h"
#include "ISupportMgr.h"
#include "IAIEcmCtrlMgr.h"
#include "IAIExpCtrlMgr.h"
#include "ICheckBadBehaviorMgr.h"
#include "IEcmCtrlMgr.h"
#include "IForceEndMgr.h"
#include "ILogRecordMgr.h"
#include "ILvExpCtrlMgr.h"
#include "ISafetyZoneMgr.h"
#include "ISoliderVigourMgr.h"
#include "ISupportMgr.h"
#include "IWarAddBuffMgr.h"
#include "IWarDropMgr.h"
#include "IXPSkillMgr.h"
#include "IMonsterDiedExecMgr.h"
#include "IWarMonsterMgr.h"
#include "MonsterDiedExecMgr.h"
#include "WarMonsterMgr.h"
#include <math.h>
#include <sstream>
#include "BuffDef.h"


using namespace Mentorship_ManagerDef;

CWar::CWar()
	: m_bFirstBlood(false)
	, m_TempAddTalentTime(0)
	, m_nAutoAddTalentInterval(0)
	, m_nAutoTalentPoint(0)
	, m_ConfigAddExpTime(0)
	, m_nFirstAutoAddExpInterval(0)     // 第一次自动加经验间隔
	, m_nAutoAddExpInterval(0)          //战场自动添加天赋点时间间隔(单位秒)
	, m_nInitialExp(0)                      //战场初始化经验
	, m_nAutoAddExp(0)                      //战场自动添加经验
	, m_nFirstAutoAddTalentInterval(0)
	, m_nInitialTalentPoint(0)
	, m_nTimeCountKillInterval(0)
	, m_bHaveComputer(false)
    , m_OBViewerUID(INVALID_UID)
{
	m_mapCampTeamRecord.clear();

	m_mapRealPlayerUid.clear();
	m_ChargedMap.clear();
    m_OBPlayerBaseInfo.clear();
	memset(m_BroadCastTimeList, 0, sizeof(m_BroadCastTimeList));
	memset(&m_sLegendCupInfo, 0, sizeof(m_sLegendCupInfo));
	memset(m_CampBaseUID, 0, sizeof(m_CampBaseUID));
	memset(m_CampBanHeroList, 0, sizeof(m_CampBanHeroList));

}

CWar::~CWar()
{
}

// 初始化
bool CWar::create(IWarService* pWarService, SWarCreateContext& context)
{
	// 创建战场场景
	ISceneManager * pSceneManager = gServerGlobal->getSceneManager();
	if (!pSceneManager)
	{
		return false;
	}

	m_pWarService = pWarService;
	m_context = context;

	int nCampCount = context.byCampCount;
	// 有怪物时，增加一个怪物阵营
	++nCampCount;
	SCreateSceneContext sCreateSceneContext;
	sCreateSceneContext.mapID = context.dwMapID;
	sCreateSceneContext.isDynamic = true;
	sCreateSceneContext.gameMode = (EGameMode)context.byMode;
	sCreateSceneContext.nCampCount = nCampCount;
	m_WarInfo.nSceneID = pSceneManager->createScene(sCreateSceneContext, &m_context.nWarID, sizeof(m_context.nWarID));
	if (m_WarInfo.nSceneID == INVALID_SCENE_ID)
	{
		ErrorLn(__FUNCTION__ ": nSceneID == INVALID_SCENE_ID");
		return false;
	}

	// 本局是否满员
	m_WarInfo.isPlayerFull = isPlayerFull(m_context.byRoleCount);
    m_WarInfo.nTotalCampCount = nCampCount;
	return true;
}

// 初始化
bool CWar::Init()
{
	// 获得配置信息
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}

	ISchemeWarManager * pSchemeWarManager = pSchemeCenter->getSchemeWarManager();
	if (pSchemeWarManager == NULL)
	{
		return false;
	}

	// 战场信息
	SWarScheme tempScheme = pSchemeWarManager->GetWarScheme(m_context.nConfigWarID);
	if (tempScheme.nStaticID <= 0)
	{
		return false;
	}


	m_WarInfo.WarPhase = EWP_Ready;

	// 流程控制数量
	m_nBroadcastProcessCount = tempScheme.nBroadcastProcessCount;

	// 流程广播赋值
	for (int i = 0; i < m_nBroadcastProcessCount; i++)
	{
		m_BroadCastTimeList[i] = tempScheme.BroadCastTimeList[i];
	}

	m_WarInfo.nWinCamp = 0;

	ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		return false;
	}
	// 取得是否显示打印添加天赋经验信息
	m_WarInfo.bIsWarShowAddEXPAndMoney = pSchemeEntity->getEntityXml()->nWarShowAddEXPAndMoney == 0 ? false : true;


	m_bFirstBlood = false;

	//// 战场信息
	ISchemeMatchRoom *pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom();
	if (pSchemeMatchRoom == NULL)
	{
		ErrorLn(__FUNCTION__": failed, pWarCampInfo == NULL");
		return false;
	}
	SSchemeMatchRoom* pWarCampInfo = pSchemeMatchRoom->getSchemeMatchRoomByMatchTypeID(m_context.dwMatchTypeID);
	if (pWarCampInfo == NULL)
	{
		ErrorLn(__FUNCTION__": failed, pWarCampInfo == NULL MatchTypeID=" << m_context.dwMatchTypeID << ", mode=" << m_context.byMode << ", dwMapID=" << m_context.dwMapID << ", static=" << m_context.nConfigWarID << ",scene=" << m_WarInfo.nSceneID);
		return false;
	}

	// 自动增加天赋点相关
	m_nAutoAddTalentInterval = 0;
	m_nAutoTalentPoint = 0;
	m_TempAddTalentTime = 0;
	m_nFirstAutoAddTalentInterval = 0;

	// 自动增加经验相关
	m_ConfigAddExpTime = 0;
	m_nFirstAutoAddExpInterval = 0;
	m_nAutoAddExpInterval = 0;
	m_nInitialExp = 0;
	m_nAutoAddExp = 0;
	m_WarInfo.nPersonNum = 0;

	const SchemeWarPersonConfigInfo* pSchemeWarPersonConfigInfo = pSchemeCenter->getSchemeWarPersonConfigInfo()->GetSchemeWarPersonConfigInfo(m_context.nConfigWarID);
	if (pSchemeWarPersonConfigInfo == NULL)
	{
		ErrorLn(__FUNCTION__": -- pSchemeWarPersonConfigInfo == NULL, WarID=" << m_context.nWarID << ", SenceID=" << m_WarInfo.nSceneID << ", nConfigWarID = " << m_context.nConfigWarID);
	}
	else
	{
		m_nAutoAddTalentInterval = pSchemeWarPersonConfigInfo->nAutoAddTalentInterval;       // 战场自动添加天赋点时间间隔(单位秒)
		m_nAutoTalentPoint = pSchemeWarPersonConfigInfo->nAutoAddTalentPoint;          // 战场自动添加天赋点
		m_nFirstAutoAddTalentInterval = pSchemeWarPersonConfigInfo->nFirstAutoAddTalentInterval;  // 第一次自动加天赋点间隔(单位秒)
		m_nInitialTalentPoint = pSchemeWarPersonConfigInfo->nInitialTalentPoint;          // 战场初始化天赋点

		m_nFirstAutoAddExpInterval = pSchemeWarPersonConfigInfo->nFirstAutoAddExpInterval;     //第一次自动加经验间隔(单位秒)
		m_nAutoAddExpInterval = pSchemeWarPersonConfigInfo->nAutoAddExpInterval;          //战场自动添加天赋点时间间隔(单位秒)
		m_nInitialExp = pSchemeWarPersonConfigInfo->nInitialExp;                  //战场初始化经验
		m_nAutoAddExp = pSchemeWarPersonConfigInfo->nAutoAddExp;                  //战场自动添加经验
	}


	// 单阵营最大人数
	DWORD dwCampRoleMaxCount = (pWarCampInfo->m_CampRoleMax <= 0 ? 5 : pWarCampInfo->m_CampRoleMax);
	// 创建阵营信息
	for (int i = 0; i < pWarCampInfo->m_CampCount; ++i)
	{
		int nCampID = i + 1;
		SWarCamp campInfo;
		campInfo.nCampID = nCampID;
		// 初始化当前阵营人数最大容量
		campInfo.warPersVect.reserve(dwCampRoleMaxCount);

		SMapLandSchemeInfo* pInfo = pSchemeCenter->getSchemeMapLand()->GetMapLandSchemeInfo(m_context.dwMapID, m_context.byMode, nCampID);
		if (pInfo != NULL)
		{
			campInfo.vecRebornPos = pInfo->vecRebornPos;
		}

		//ErrorLn("nCampID ="<<nCampID <<" m_nMapID = "<<m_nMapID<<" campInfo.vLoc.x = "<<pInfo->ptSafeTile.x<<" campInfo.vLoc.y = "<<pInfo->ptSafeTile.y<<" campInfo.vLoc.z = "<<pInfo->ptSafeTile.z);
		m_mapWarCamp[nCampID] = campInfo;
	}

	// 连杀间隔时间
	ISchemeWarPersonConfigInfo* pWarPersonConfig = pSchemeCenter->getSchemeWarPersonConfigInfo();
	if (pWarPersonConfig != NULL)
	{
		const SchemeWarPersonConfigInfo* pInfo = pWarPersonConfig->GetSchemeWarPersonConfigInfo(getStaticID());

		if (pInfo != NULL)
			m_nTimeCountKillInterval = pInfo->nTimeCountKillInterval;
	}

	// 初始化子功能
	if (InitSubFunc() == false)
	{
		ErrorLn(__FUNCTION__ " Init sub function failed.");
		return false;
	}

    /*if (createOBViewerEntity() == false)
    {
        ErrorLn(__FUNCTION__ " create OB viewer failed!");
        return false;
    }*/
    

	// 通知war已经准备好了
	MatchSceneHelper helper;
	if (helper.m_ptr)
	{
		helper.m_ptr->sendCreateMobaResult(m_context.dwRoomID, m_context.nWarID, m_WarInfo.nSceneID, true);
	}

	WAR_TRACELN(__FUNCTION__": dwRoomID = " << m_context.dwRoomID << " nWarID =" << m_context.nWarID << " m_nSceneID =" << m_nSceneID);

	return true;
}

bool CWar::InitSubFunc()
{
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__ " Get ISchemeCenter failed.");
		return false;
	}

	ISchemeWarSubFunc* pWarSubFunc = pSchemeCenter->getSchemeWarSubFunc();
	if (pWarSubFunc == NULL)
	{
		ErrorLn(__FUNCTION__ " Get ISchemeWarSubFunc failed.");
		return false;
	}

	int nMatchType = getMatchTypeID();
	///////////////////////////////////以下功能不需要一定要启动成功，因为是可选
	// 强制结算
	IForceEndMgr* pForceEndMgr = new CForceEnd(this);
	if (pForceEndMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init ForceEnd failed Or closed this function.");
		pForceEndMgr->Release();
		pForceEndMgr = NULL;
	}
	m_WarMgrList[Mgr_ForceEnd] = pForceEndMgr;
	WarningLn(__FUNCTION__ " Init ForceEnd ok");

	// 战场自动添加Buff
	IWarAddBuffMgr* pWarAddBuffMgr = new CWarAddBuff(this);
	if (pWarAddBuffMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init WarAddBuff failed Or closed this function");
		pWarAddBuffMgr->Release();
		pWarAddBuffMgr = NULL;
	}
	m_WarMgrList[Mgr_AddBuff] = pWarAddBuffMgr;

	// 战场检测恶劣行为
	ICheckBadBehaviorMgr* pCheckBadBhvMgr = new CWarCheckBadBehavior(this);
	if (pCheckBadBhvMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init WarCheckBadBehavior failed Or closed this function.");
		pCheckBadBhvMgr->Release();
		pCheckBadBhvMgr = NULL;
	}
	m_WarMgrList[Mgr_CheckBadBehavior] = pCheckBadBhvMgr;
	//////////////////////////////////////////////////////////////////


	// 经济控制相关
	IEcmCtrlMgr* pEcmCtrlMgr = new CEconomyControl(this);
	if (pEcmCtrlMgr->Init(nMatchType) == false)
	{
		ErrorLn(__FUNCTION__ " Init EcmCtrl failed.");
		pEcmCtrlMgr->Release();
		pEcmCtrlMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_EcmCtrl] = pEcmCtrlMgr;

	// 士兵士气
	ISoliderVigourMgr* pSoliderVigourMgr = new CSoliderVigour(this);
	if (pSoliderVigourMgr->Init(nMatchType) == false)
	{
		ErrorLn(__FUNCTION__ " Init SoliderVigour failed.");
		pSoliderVigourMgr->Release();
		pSoliderVigourMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_SoliderVigour] = pSoliderVigourMgr;

	// 等级经验控制
	ILvExpCtrlMgr* pLvExpCtrlMgr = new CLvExpControl(this);
	if (pLvExpCtrlMgr->Init(nMatchType) == false)
	{
		ErrorLn(__FUNCTION__ " Init LvExpControl failed.");
		pLvExpCtrlMgr->Release();
		pLvExpCtrlMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_LvExpCtrl] = pLvExpCtrlMgr;

	// 点赞
	if (pWarSubFunc->isEnableSubFunc(nMatchType, WarSubFuncType_Support))
	{
		ISupportMgr* pSupportMgr = new CSupport(this);
		if (pSupportMgr->Init(nMatchType) == false)
		{
			ErrorLn(__FUNCTION__ " Init Support failed.");
			pSupportMgr->Release();
			pSupportMgr = NULL;
			return false;
		}
		m_WarMgrList[Mgr_Support] = pSupportMgr;
	}

	// XP技能
	if (pWarSubFunc->isEnableSubFunc(nMatchType, WarSubFuncType_XPSkill))
	{
		IXPSkillMgr* pXPSkillMgr = new CXPSkill(this);
		if (pXPSkillMgr->Init(nMatchType) == false)
		{
			ErrorLn(__FUNCTION__ " Init XPSkill failed.");
			pXPSkillMgr->Release();
			pXPSkillMgr = NULL;
			return false;
		}
		m_WarMgrList[Mgr_XPSKill] = pXPSkillMgr;
	}

	// 战场日志
	ILogRecordMgr* pLogRecordMgr = new CWarLogRecord(this);
	if (pLogRecordMgr->Init(nMatchType) == false)
	{
		ErrorLn(__FUNCTION__ " Init WarLogRecord failed.");
		pLogRecordMgr->Release();
		pLogRecordMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_LogRecord] = pLogRecordMgr;


	// AI经济经验控制
	if (pWarSubFunc->isEnableSubFunc(nMatchType, WarSubFuncType_AIEcmCtrl))
	{
		IAIEcmCtrlMgr* pAIEcmCtrlMgr = new CAIEcmCtrl(this);
		if (pAIEcmCtrlMgr->Init(nMatchType) == false)
		{
			ErrorLn(__FUNCTION__ " Init AIEcmExpCtrl failed.");
			pAIEcmCtrlMgr->Release();
			pAIEcmCtrlMgr = NULL;
			return false;
		}
		m_WarMgrList[Mgr_AIEcmCtrl] = pAIEcmCtrlMgr;
	}

	// AI经验控制
	if (pWarSubFunc->isEnableSubFunc(nMatchType, WarSubFuncType_AIExpCtrl))
	{
		IAIExpCtrlMgr* pAIExpCtrlMgr = new CAIExpCtrl(this);
		if (pAIExpCtrlMgr->Init(nMatchType) == false)
		{
			ErrorLn(__FUNCTION__ " Init AIExpCtrl failed.");
			pAIExpCtrlMgr->Release();
			pAIExpCtrlMgr = NULL;
			return false;
		}
		m_WarMgrList[Mgr_AIExpCtrl] = pAIExpCtrlMgr;
	}

	// 战场掉落
	IWarDropMgr* pWarDropMgr = new CWarDrop(this);
	if (pWarDropMgr->Init(nMatchType) == false)
	{
		ErrorLn(__FUNCTION__ " Init WarDrop failed.");
		pWarDropMgr->Release();
		pWarDropMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_Drop] = pWarDropMgr;

	// 怪物管理
	IWarMonsterMgr* pWarMonsterMgr = new CWarMonsterMgr(this);
	if (pWarMonsterMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init CWarMonsterMgr failed.");
		pWarMonsterMgr->Release();
		pWarMonsterMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_Monster] = pWarMonsterMgr;

	// 怪物死亡处理
	IMonsterDiedExecMgr* pMonsterDiedExecMgr = new CMonsterDiedExecMgr(this);
	if (pMonsterDiedExecMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init CMonsterDiedExecMgr failed.");
		pMonsterDiedExecMgr->Release();
		pMonsterDiedExecMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_MonsterDiedExec] = pMonsterDiedExecMgr;

	return true;
}

// 开启战场(场景创建成功后自动开启战场)
void CWar::Start()
{
	CWarBase::Start();
}

void CWar::destroy()
{
	// 销毁场景
	if (!gServerGlobal->getSceneManager()->closeScene(getWarSceneID()))
	{
		ErrorLn(__FUNCTION__ << __LINE__ << ", destroy war failed, sceneid=" << getWarSceneID());
		return;
	}

	// 销毁战场
	SERVICE_PTR pWarService = gServerGlobal->getWarManager()->GetWar(getWarID());
	if (pWarService == 0)
	{
		ErrorLn(__FUNCTION__ << __LINE__ << ", destroy war fialed.. warid=" << getWarID());
		return;
	}
	pWarService->stop();
}

// 反初始化
void CWar::UnInit()
{
	// 清除玩家信息
	m_mapWarCamp.clear();

	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr != NULL)
	{
		pWarMonsterMgr->releaseMonsterRes();
	}

	for (int i = 0; i < Mgr_Max; ++i)
	{
		__IWarMgr* pWarMgr = m_WarMgrList[i];
		if (pWarMgr != NULL)
		{
			pWarMgr->Release();
			pWarMgr = NULL;
			m_WarMgrList[i] = NULL;
		}
	}

	CWarBase::UnInit();
}

// 玩家进入战场
bool CWar::ClientPrepareEnterWar(const SActorPreaperEnterWar& sActorPreaperEnterWar)
{
	iniWarActorInfo(sActorPreaperEnterWar);
	return true;
}

void CWar::playerAddBuff(SWarEffectAddBuff sData)
{
	// 查找玩家
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		for (WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin(); itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			UID uid = INVALID_UID;
			switch (sData.byGetCampType)
			{
			case EWCT_SELFCAMP:
			{
				// 己方
				if (itCamp->first == sData.bySelfCamp)
				{
					uid = itPerson->uID;
				}
			}
			break;
			case EWCT_ENIMICAMP:
			{
				if (itCamp->first != sData.bySelfCamp)
				{
					// 添加buff
					uid = itPerson->uID;
				}
			}
			break;
			case EWCT_ALL:
			{
				// 添加buff
				uid = itPerson->uID;
			}
			break;
			default:break;
			}

			if (uid == INVALID_UID)
			{
				continue;
			}

			switch (sData.byAddType)
			{
			case BUFF_ADD:
			{
				// 给实体添加buff
				AddBuff(uid, sData.nBuffID, sData.nBuffLevel, sData.uidSrcEntity);
	
			}
			break;
			case BUFF_REMOVE:
			{
				// 给实体移除buff
				RemoveBuff(uid, sData.nBuffID, sData.uidSrcEntity, sData.uidSrcEntity);
			
			}
			break;
			case BUFF_OVERLAY:
			{
				// 叠加BUFF
				SetAccumulateBuff(uid, sData.nBuffID, sData.uidSrcEntity, true);
			}
			break;
			case BUFF_DECREASE:
			{
				// 递减BUFF
				SetAccumulateBuff(uid, sData.nBuffID, sData.uidSrcEntity, false);
			}
			break;
			default:
				break;
			}
		}
	}
}

void CWar::warAddSerchTypeBuff(SWarEffectAddBuff sData)
{
	if (sData.bySerchType == EWME_None)
	{
		playerAddBuff(sData);

	}
	else
	{
		IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)getWarMgr(Mgr_Monster);
		if (pWarMonsterMgr != NULL)
		{
			pWarMonsterMgr->monsterAddBuff(sData);
		}
	}
}

void CWar::sendToClientImportantEntityDie(const msg_war_important_entiy_dieinfo& EntiyDieInfo, const vector<msg_war_entity_assist_info> & assistList)
{
	obuf oEntityDieInfo;
	oEntityDieInfo << EntiyDieInfo;

	// 压入助攻者数据
	vector<msg_war_entity_assist_info>::const_iterator iter = assistList.begin();
	for (; iter != assistList.end(); ++iter)
	{
		oEntityDieInfo << *iter;
	}

	broadCastDataToAllPlayer(SC_MSG_WAR_IMPORTANT_ENTITYDIE, oEntityDieInfo.data(), oEntityDieInfo.size());
}

void CWar::sendToClientKillCountInfo(const msg_war_entity_kill_countinfo& EntiySkillCountInfo, const vector<msg_war_entity_assist_info> & assistList)
{
	obuf oSkillCountInfo;
	oSkillCountInfo << EntiySkillCountInfo;

	// 压入助攻者数据
	vector<msg_war_entity_assist_info>::const_iterator iter = assistList.begin();
	for (; iter != assistList.end(); ++iter)
	{
		oSkillCountInfo << *iter;
	}

	broadCastDataToAllPlayer(SC_MSG_WAR_KILL_COUNT, oSkillCountInfo.data(), oSkillCountInfo.size());
}

const SchemeWarPersonConfigInfo * CWar::getPersonConfigInfo()
{
	// 获得配置信息
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return NULL;
	}
	ISchemeWarPersonConfigInfo *pSchemeWarPersonConfigInfo = pSchemeCenter->getSchemeWarPersonConfigInfo();
	return pSchemeWarPersonConfigInfo ? pSchemeWarPersonConfigInfo->GetSchemeWarPersonConfigInfo(getStaticID()) : NULL;
}


int CWar::getCreateLevel(int nBaseLevel, int nMonsterRefreshBeginTime, int nMonsterUpgradeTime, int nMaxLevel)
{
	if (nBaseLevel < 0)
	{
		return 1;
	}

	if (nMonsterUpgradeTime <= 0)
	{
		return nBaseLevel;
	}
	else
	{
		if (getWarStarTime() <= 0)
		{
			return nBaseLevel;
		}
		//////////////////////////////////配置里面已经换算了 秒对应的毫秒////////////////////////////////////////
		int CurLevel = (int)((gServerGlobal->getTimeSyncService()->GetTick() - getWarStarTime() - nMonsterRefreshBeginTime) / (nMonsterUpgradeTime));

		if (CurLevel <= 0)
		{
			return nBaseLevel;
		}

		if (nMaxLevel > 0)
		{
			int Level = CurLevel + nBaseLevel;
			return (int)(Level > nMaxLevel ? nMaxLevel : Level);
		}
		else
		{
			return (int)(CurLevel > 0 ? CurLevel + nBaseLevel : nBaseLevel);
		}
	}
}

void CWar::sendToClientImportantMonsterRelive(msg_war_important_monster_reliveinfo MonsterReliveInfo)
{
	obuf oEntityDieInfo;
	oEntityDieInfo << MonsterReliveInfo;
	broadCastDataToAllPlayer(SC_MSG_WAR_IMPORTANT_MONSTERRELIVE, oEntityDieInfo.data(), oEntityDieInfo.size());
}

void CWar::SendToClientImportantMonsterEarlyRelive(msg_war_important_monster_reliveinfo MonsterReliveInfo)
{
	obuf oEntityDieInfo;
	oEntityDieInfo << MonsterReliveInfo;
	broadCastDataToAllPlayer(SC_MSG_WAR_IMPORTANT_MONSTERRELIVE_EARLY, oEntityDieInfo.data(), oEntityDieInfo.size());
}

void CWar::sendToClientMonsterDieBroadText(msg_war_important_monster_dietxtinfo MonsterDieTextInfo)
{
	obuf oEntityDieTextInfo;
	oEntityDieTextInfo << MonsterDieTextInfo;
	broadCastDataToAllPlayer(SC_MSG_WAR_IMPORTANT_MONSTERTEXTDIE, oEntityDieTextInfo.data(), oEntityDieTextInfo.size());
}

void CWar::deSerialzed(UID uid, rkt::obuf & out, int flag)
{
	if (flag & TO_CLIENT_SELF)
	{
		bool bCheckBadBeheavior = false;
		int nBottingCount = 0;
		int nSummerID = 0;

		SWPerson* pPerson = getPlayerInfo(uid);
		if (pPerson != NULL)
		{
			bCheckBadBeheavior = isOpenCheckBadBehavior();
			nBottingCount = pPerson->nBottingCount;
			nSummerID = pPerson->nSummerID;
		}

		out << bCheckBadBeheavior << nBottingCount << nSummerID;
	}
}

// 设置失败方水晶位置
void CWar::setFailedCampBasePos(Vector3 v)
{
	m_FailedCampBasePos = v;
}

void CWar::setCampBaseUID(int nCamp, UID uid)
{
	if (nCamp < 0 || nCamp >= CAMP_MAX)
		return;

	m_CampBaseUID[nCamp] = uid;
}

void CWar::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_GAME_ENTITY_DAMAGE:          //英雄受到伤害事件
		{
			if (nLen != sizeof(event_entity_damage))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_damage) !");
				return;
			}

			event_entity_damage *pData = (event_entity_damage *)pszContext;
			onGetDamage(*pData);

		}
		break;

	case EVENT_GAME_ACTOR_WAR_REC_DAMAGE:    //召唤怪物受到伤害事件
		{
			if (nLen != sizeof(event_entity_warrec_damage))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_warrec_damage) !");
				return;
			}

			event_entity_warrec_damage *pData = (event_entity_warrec_damage *)pszContext;
			onGetDamage(*pData);

		}
		break;

	case EVENT_GAME_ENTITY_CHANGE_STATE:    //玩家状态切换
		{
			if (nLen != sizeof(event_entity_change_state))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_change_state) !");
				return;
			}

			event_entity_change_state *pData = (event_entity_change_state *)pszContext;
			onGetChangeState(*pData);

		}
		break;

	case EVENT_GAME_ACTOR_UPGRADE_LEVEL:    // 升级事件
		{
			if (nLen != sizeof(event_entity_upgrade_level))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_upgrade_level) !");
				return;
			}

			event_entity_upgrade_level *pData = (event_entity_upgrade_level *)pszContext;
			onEntityUpgrade(*pData);

		}
		break;

	case EVENT_GAME_ACTOR_TALENT_CHANGE:    // 玩家天赋点改变
		{
			if (nLen != sizeof(event_entity_talent_change))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_talent_change) !");
				return;
			}
			event_entity_talent_change* pEvent = (event_entity_talent_change*)pszContext;
			onTalentChange(pEvent);
		}
		break;

	case EVENT_GAME_ENTITY_PREPARE_CURE:    // 治疗事件
		{
			if (nLen != sizeof(CureContext))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(CureContext) !" << ", nLen = " << nLen << ", sizeof(CureContext) = " << sizeof(CureContext));
				return;
			}
			CureContext* pEvent = (CureContext*)pszContext;
			onHaveCure(pEvent);
		}
		break;

	case EVENT_GAME_ACTOR_WAR_GOOD_LIST:    // 物品列表事件
		{

			if (nLen != sizeof(event_entity_war_good_list))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_war_good_list) !" << ", nLen = " << nLen << ", sizeof(event_entity_war_good_list) = " << sizeof(event_entity_war_good_list));
				return;
			}
			event_entity_war_good_list* pEvent = (event_entity_war_good_list*)pszContext;
			onSetGoodList(pEvent);
		}
		break;
	case EVENT_GAME_CLEAR_MULTI_KILL:    // 清除连杀
		{

			if (nLen != sizeof(event_entity_clear_multi_kill))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_clear_multi_kill) !" << ", nLen = " << nLen << ", sizeof(event_entity_clear_multi_kill) = " << sizeof(event_entity_clear_multi_kill));
				return;
			}
			event_entity_clear_multi_kill* pEvent = (event_entity_clear_multi_kill*)pszContext;
			onClearDeadContKill(pEvent);
		}
		break;
	case EVENT_GAME_THRILLINGCURE:    // 惊险救治
		{

			if (nLen != sizeof(event_game_thrillingcure))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_game_thrillingcure) !" << ", nLen = " << nLen << ", sizeof(event_game_thrillingcure) = " << sizeof(event_game_thrillingcure));
				return;
			}
			// 处理惊险救治
			event_game_thrillingcure* pEvent = (event_game_thrillingcure*)pszContext;
			onThrillingCure(pEvent);
		}
		break;
	case EVENT_GAME_KILLINGCONTROL:    // 夺命控制
		{

			if (nLen != sizeof(event_game_killingcontrol))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_game_killingcontrol) !" << ", nLen = " << nLen << ", sizeof(event_game_killingcontrol) = " << sizeof(event_game_killingcontrol));
				return;
			}
			event_game_killingcontrol* pEvent = (event_game_killingcontrol*)pszContext;
			onKillingControl(pEvent);
		}
		break;
	case EVENT_GAME_GROUPCONTROL:    // 团体控制
		{

			if (nLen != sizeof(event_game_groupcontrol))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_game_groupcontrol) !" << ", nLen = " << nLen << ", sizeof(event_game_groupcontrol) = " << sizeof(event_game_groupcontrol));
				return;
			}
			event_game_groupcontrol* pEvent = (event_game_groupcontrol*)pszContext;
			onGroupControl(pEvent);
		}
		break;
	case EVENT_GAME_ACTOR_CONTINUE_PLAY:    // 玩家续玩
		{

			if (nLen != sizeof(event_entity_continue_play))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_game_groupcontrol) !" << ", nLen = " << nLen << ", sizeof(event_game_groupcontrol) = " << sizeof(event_game_groupcontrol));
				return;
			}
			event_entity_continue_play* pEvent = (event_entity_continue_play*)pszContext;
			onPlayerContinuePlay(pEvent);
		}
		break;
	case EVENT_GAME_ACTOR_ACTIVE_TALENT:
		{
			if (nLen != sizeof(event_entity_active_talent))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_active_talent) !" << ", nLen = " << nLen << ", sizeof(event_entity_active_talent) = " << sizeof(event_entity_active_talent));
				return;
			}

			event_entity_active_talent* pEvent = (event_entity_active_talent*)pszContext;
			ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
			if (pSchemeCenter == NULL)
				return;
			ISchemeHeroTalentConfig* pHeroTalent = pSchemeCenter->getSchemeHeroTalentConfig();
			if (pHeroTalent == NULL)
				return;

			SWPerson* pPerson = getPlayerInfoByPDBIU(pEvent->nPDBID);
			if (pPerson == NULL)
				return;

			Single_Hero_Talent_InfoEx* pTalent = pHeroTalent->getHeroTalent(pPerson->nHeroID, pEvent->nTalentID);
			if (pTalent == NULL)
				return;

			onActiveTalent(pEvent->nPDBID, pEvent->nTalentID, pTalent->nLearnLevel);
		}
		break;
	case EVENT_GAME_ADD_ASSIST_BUFF:
		{
			if (nLen < sizeof(event_game_add_assist_buff))
				return;

			event_game_add_assist_buff* pEvent = (event_game_add_assist_buff*)pszContext;
			if (nLen != sizeof(event_game_add_assist_buff) + pEvent->nCount * sizeof(SActor_Add_Buff_Node))
				return;

			onAddAssistBuff(pEvent->uid, pEvent->nCount, pszContext + sizeof(event_game_add_assist_buff));
		}
		break;
	default:break;
	}
}

void CWar::onKillingControl(event_game_killingcontrol *pEvent)
{
	SWPerson* pOperaterPerson = getPlayerInfo(pEvent->uidOperator);
	if (pOperaterPerson == NULL)
	{
		return;
	}
	SWPerson* pTargetPerson = getPlayerInfo(pEvent->uidTarget);
	if (pTargetPerson == NULL)
	{
		return;
	}
	// 夺命控制队友 不科学 过滤掉
	if (pOperaterPerson->nCamp == pTargetPerson->nCamp)
	{
		return;
	}

	pOperaterPerson->nBaseRecord[EDT_DeadlyControl]++;
	BroadcastSpecialContribution(ESCT_DEADLYCONTROL, pOperaterPerson);
}

void CWar::onThrillingCure(event_game_thrillingcure *pEvent)
{
	SWPerson* pOperaterPerson = getPlayerInfo(pEvent->uidOperator);
	if (pOperaterPerson == NULL)
	{
		return;
	}
	SWPerson* pTargetPerson = getPlayerInfo(pEvent->uidTarget);
	if (pTargetPerson == NULL)
	{
		return;
	}
	// 惊现救治地方 不科学 过滤掉
	if (pOperaterPerson->nCamp != pTargetPerson->nCamp)
	{
		return;
	}

	pOperaterPerson->nBaseRecord[EDT_ThrillSave]++;
	BroadcastSpecialContribution(ESCT_THRILLSAVE, pOperaterPerson);
}

void CWar::onGroupControl(event_game_groupcontrol *pEvent)
{
	SWPerson* pOperaterPerson = getPlayerInfo(pEvent->uidGroupControl);
	if (pOperaterPerson == NULL)
	{
		return;
	}

	pOperaterPerson->teamControlList.push_back(pEvent->nGroupControlNum);
	pOperaterPerson->nBaseRecord[EDT_TeamBattleControl] = max(pOperaterPerson->nBaseRecord[EDT_TeamBattleControl], pEvent->nGroupControlNum);
	BroadcastSpecialContribution(ESCT_TEAMBATTLECONTROL, pOperaterPerson);
}

void CWar::onGetDamage(event_entity_damage damage)
{
	// 处理英雄打出的伤害
	int nEntityType = UID_2_TYPE(damage.uidOperator);
	if (nEntityType == TYPE_PLAYER_ROLE)
	{
		onGetHeroDamage(damage);

		//被攻击了判定发战斗小地图信息
		sendMiniMapFightInfo(damage);
	}
}

void CWar::onGetChangeState(event_entity_change_state changestate)
{
	if (changestate.uid == INVALID_UID)
	{
		return;
	}
	if (changestate.nState <= ENTITY_STATE_UNKNOW || changestate.nState >= ENTITY_STATE_MAXID)
	{
		return;
	}

	// 清空本人的受到伤害列表
	SWPerson* pPerson = getPlayerInfo(changestate.uid);
	if (pPerson == NULL)
		return;

	// 判断人物状态
	//if (ENTITY_STATE_STAND == changestate.nState)
	//{
		//清除记录伤害值
		//pPerson->GetDamageVect.clear();
		//pPerson->uLastAttackUID = INVALID_UID;
		//pPerson->lifeState = ELS_Alive;
	//}

	//清除记录伤害值
	//玩家进入战斗状态时清理，如果是进入站立状态时候清理，因为多线程问题，会先进入站立状态玩家后调用onEntityDieEx的问题
	if (pPerson->lifeState == ELS_Alive && ENTITY_STATE_FIGHT == changestate.nState)
	{
		pPerson->GetDamageVect.clear();
		pPerson->uLastAttackUID = INVALID_UID;
	}
}

void CWar::onTalentChange(event_entity_talent_change* pTalentChange)
{
	UID uId = pTalentChange->uId;
	int nTalent = pTalentChange->nTalentValue;
	int nChangePoint = pTalentChange->nChangePoint;
	BYTE byChangeType = pTalentChange->byChangeType;

	SWPerson* pPerson = getPlayerInfo(uId);
	if (pPerson == NULL)
		return;

	// 统计获得天赋点
	if (byChangeType != ENUM_TALENT_REVOKE && byChangeType != ENUM_TALENT_SELL)
	{
		if (pPerson->nCurselTalent < nTalent && nChangePoint > 0)
		{
			pPerson->nBaseRecord[EDT_Money] += nChangePoint;
			if (ENUM_TALENT_INIT == byChangeType)
			{
				sendAllPersonInfoToAllClient();
			}
			else
			{
				sendToOpenTabInfoClient(pPerson->uPDBID, *pPerson);
			}
		}

		pPerson->nCurselTalent = nTalent;
	}


	// 天赋点更改日志
	ILogRecordMgr* pLogRecordMgr = (ILogRecordMgr*)getWarMgr(Mgr_LogRecord);
	if (pLogRecordMgr != NULL)
		pLogRecordMgr->setAddTalentToMap(pPerson->uPDBID, nChangePoint, ChangeT2DropT((ENUM_TALENT_CHANGE_TYPE)byChangeType));
}

void CWar::onEntityUpgrade(event_entity_upgrade_level entityUpgrade)
{
	if (getWarPhase() == EWP_End)
	{
		return;
	}
	if (entityUpgrade.uChangedUID == INVALID_UID)
	{
		return;
	}

	ActorHelper actorHelper(entityUpgrade.uChangedUID);
	IActorService *pActorService = actorHelper.m_ptr;

	// 判断人物状态
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			int nOldLv = itPerson->nLevel;
			if (itPerson->uID == entityUpgrade.uChangedUID && nOldLv != 0)
			{
				itPerson->nLevel = entityUpgrade.nLevel;
				addHeroStarPro(itPerson->uID, entityUpgrade.nLevel, nOldLv);

				if (pActorService)
				{
					pActorService->setCrownEffect(ENTITY_UPGRADE);
				}
				return;
			}
		}
	}
}

void CWar::Release()
{
	m_ChargedMap.clear();
}

void CWar::onActorEnterWar(const SActorEnterWarBaseInfo& sActorEnterWarBaseInfo)
{
    // todo OB位查看
    if (sActorEnterWarBaseInfo.bIsOB)
    {
        // 存起来
        m_OBPlayerBaseInfo[sActorEnterWarBaseInfo.uidActor] = sActorEnterWarBaseInfo;
        sTabRecordInfo& recodInfo = m_mapTableRequest[sActorEnterWarBaseInfo.pdbid];
        recodInfo.bOpen = true;
        recodInfo.dwOpenTicks = gServerGlobal->getTimeSyncService()->GetTick();

        return;
    }
    
	// 进入之后人物基本信息
	SWPerson person;
	person.dwUserID = sActorEnterWarBaseInfo.dwUserID;
	person.nCamp = sActorEnterWarBaseInfo.nCamp;
	person.uID = sActorEnterWarBaseInfo.uidActor;
	person.ulClient = sActorEnterWarBaseInfo.ulClient;
	person.uPDBID = sActorEnterWarBaseInfo.pdbid;
	person.nHeroID = sActorEnterWarBaseInfo.nHeroID;
	person.nSummerID = sActorEnterWarBaseInfo.nSummerID;
	person.nHeroSkinID = sActorEnterWarBaseInfo.nHeroSkinID;

	person.bInWar = true;
	person.nIndex = 0;
	person.nSrcSceneID = 0;
	person.nSrcWorldID = 0;

	map<PDBID, SPendingCreateActor>::iterator iter = m_mapPendingCreateActor.find(sActorEnterWarBaseInfo.pdbid);
	if (iter == m_mapPendingCreateActor.end())
	{
		ErrorLn(__FUNCTION__": failed, unknown uidActor:" << sActorEnterWarBaseInfo.uidActor);
		return;
	}

	SPendingCreateActor sPendingCreateActor = iter->second;
	person.nIndex = sPendingCreateActor.nIndex;
	person.uPDBID = sPendingCreateActor.uPDBID;
	person.nSrcWorldID = sPendingCreateActor.nSrcWorldID;
	person.nSrcSceneID = sPendingCreateActor.nSrcSceneID;
	person.vPosition = sPendingCreateActor.vPosition;
	person.vAngle = sPendingCreateActor.vAngle;
	person.bIsComputer = sPendingCreateActor.bIsComputer;
	WarCampMap::iterator itCamp = m_mapWarCamp.find(sActorEnterWarBaseInfo.nCamp);
	if (itCamp == m_mapWarCamp.end())
	{
		ErrorLn(__FUNCTION__": failed, unknown camp:" << sActorEnterWarBaseInfo.nCamp);
		return;
	}

	// 记录撮合类型分数
	ActorHelper actorHelper(sActorEnterWarBaseInfo.uidActor);
	IActorService *pActorService = actorHelper.m_ptr;
	if (pActorService != NULL)
	{
		person.nPreMatchTypeScore = pActorService->getMatchTypeRank((EMMatchType)getMatchTypeID());

		person.nPreMatchTypeGrade = pActorService->getMatchTypeRankGrade((EMMatchType)getMatchTypeID());
	}

	// 统计玩家个数
	if (person.bIsComputer)
	{
		itCamp->second.nAIPlayerCount++;
	}
	else
	{
		itCamp->second.nRealPlayerCount++;
		itCamp->second.nMatchScoreRealPlayer += person.nPreMatchTypeScore;
	}
	// 统计总分
	itCamp->second.nTotalMatchScore += person.nPreMatchTypeScore;

	// 找到阵营里面的人物之后修改
	WarPersonVector::iterator iterPerson = itCamp->second.warPersVect.begin();
	if (iterPerson == itCamp->second.warPersVect.end())
	{
		if (m_nInitialTalentPoint > 0 && person.bIniTalentPointAdd == false && pActorService != NULL)
		{
			if (getWarShowAddEXPAndMoney() == true)
			{
				TraceLn(__FUNCTION__": Name" << person.szName << " AddTalentPoint = " << m_nInitialTalentPoint);
			}

			updateTalentPoint(iterPerson->uID, m_nInitialTalentPoint, ENUM_TALENT_INIT);
			person.bIniTalentPointAdd = true;
		}

		if (m_nInitialExp > 0 && person.bIniExpAdd == false && pActorService != NULL)
		{
			if (getWarShowAddEXPAndMoney() == true)
			{
				TraceLn(__FUNCTION__": Name" << person.szName << " AddExp = " << m_nInitialExp);
			}

			addPersonExp(iterPerson->uID, m_nInitialExp, EEXPDROPTYPE_INIT);

			person.bIniExpAdd = true;
		}

		itCamp->second.warPersVect.push_back(person);
		// 将当前人插入到PDBID和UID关联iterator列表中
		WarPersonVector::iterator & last_iter = --(itCamp->second.warPersVect.end());
		m_mapPDBID2IterList[person.uPDBID] = last_iter;
		m_mapUID2IterList[person.uID] = last_iter;
	}
	else
	{
		for (; iterPerson != itCamp->second.warPersVect.end(); ++iterPerson)
		{
			if (iterPerson->uPDBID == person.uPDBID)
			{
				// 实体进入战场之后更新基础数据
				iterPerson->dwUserID = person.dwUserID;
				iterPerson->nCamp = person.nCamp;
				iterPerson->uID = person.uID;
				iterPerson->ulClient = person.ulClient;
				iterPerson->nHeroID = person.nHeroID;
				iterPerson->nIndex = person.nIndex;
				iterPerson->bInWar = person.bInWar;
				//iterPerson->nSummerID   = person.nSummerID;	// 使用ClientPrepareEnterWar中设置的召唤师技能
				iterPerson->nHeroSkinID = person.nHeroSkinID;
				iterPerson->nPreMatchTypeScore = person.nPreMatchTypeScore;

				if (iterPerson->bIniTalentPointAdd == false)
				{
					if (m_nInitialTalentPoint > 0 && pActorService != NULL)
					{
						if (getWarShowAddEXPAndMoney() == true)
						{
							TraceLn(__FUNCTION__": Name" << pActorService->getName().c_str() << " AddTalentPoint = " << m_nInitialTalentPoint);
						}

						updateTalentPoint(iterPerson->uID, m_nInitialTalentPoint, ENUM_TALENT_INIT);

						iterPerson->bIniTalentPointAdd = true;
					}
				}

				iterPerson->nSrcWorldID = person.nSrcWorldID;
				iterPerson->nSrcSceneID = person.nSrcSceneID;
				iterPerson->vPosition = person.vPosition;
				iterPerson->vAngle = person.vAngle;


				// 将当前人插入到PDBID关联iterator列表中
				m_mapPDBID2IterList[person.uPDBID] = iterPerson;
				m_mapUID2IterList[person.uID] = iterPerson;

				break;
			}
		}
	}

	m_mapUidMiniMapInfoRecord[sActorEnterWarBaseInfo.uidActor] = gServerGlobal->getTimeSyncService()->GetTick();

	// 英雄属性相关
	addHeroStarPro(sActorEnterWarBaseInfo.uidActor);

    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    ISchemeWarManager * pSchemeWarManager = pSchemeCenter->getSchemeWarManager();
    if (pSchemeWarManager == NULL)
    {
        ErrorLn(__FUNCTION__": pSchemeWarManager == NULL");
        return;
    }

    SWarScheme tempScheme = pSchemeWarManager->GetWarScheme(m_context.nConfigWarID);
    if (tempScheme.nStaticID <= 0)
    {
        ErrorLn(__FUNCTION__": nStaticID <= 0 nStaticID =" << tempScheme.nStaticID);
        return;
    }

	if (m_lMutex == 0 && InterlockedCompareExchange(&m_lMutex, 1, 0) == 0)
	{
		// 有玩家进入战场 则设置为战场切换状态
		if (getWarPhase() == EWP_Ready)
		{
            // 战场正式开始时间,该时间必须这样获取,因为客户端战场的进行时间是根据这个值计算
            m_ConfigAddExpTime = m_TempAddTalentTime = m_WarInfo.nStartTimeTick = gServerGlobal->getTimeSyncService()->GetTick();

            // 开启流程定时器
            setBcastProcessTimer();

            //有人物创建并进入战场之后才设置战场开始
            setWarPhase(EWP_Start);

            // 战场正式开始
            onWarRealStart();
        }
	}
	m_WarInfo.nCurEnterWarCount++;

	if (m_WarInfo.nCurEnterWarCount == m_context.byRoleCount)
	{
		onAllActorEnter();
	}
}

//	1、英雄星级部分：
//      星级总血量＝星级血量+星级血量成长*（熟练等级-1）
//      星级总血量成长率＝星级血量成长率+星级血量成长率成长*（熟练等级-1）
//
//    2、这些属性带到战场内后
//    战场内英雄实际血量＝基础血量+星级总血量+（血量成长率+星级总血量成长率）*（场内等级-1）
void CWar::addHeroStarPro(UID uID, int nNewLv, int nOldLv)
{
	ISchemeHeroStarData* pStarData = gServerGlobal->getSchemeCenter()->getSchemeHeroStarData();
	if (pStarData == NULL)
		return;

	SWPerson* pPerson = getPlayerInfo(uID);
	if (pPerson == NULL)
	{
		return;
	}

	int nHeroStarEx = pPerson->byHeroStarLvEx;
	int nStaticLv = pPerson->byHeroStaticLv;
	SHeroGrowProInfo* pGrowProInfo = pStarData->getGrowProInfo(pPerson->nHeroID, nHeroStarEx);
	if (pGrowProInfo == NULL)
	{
		return;
	}

	for (int i = 0; i < PERSON_STAR_PROP_COUNT; ++i)
	{
		SPersonStarProp sStarProp = pGrowProInfo->sStarProp[i];
		float nSumGrowRate = (float)(sStarProp.nStarGrowRate + sStarProp.nStarRateGrow * (nStaticLv - 1));
		int nPropID = sStarProp.nPropID;
		float fSumValue = (float)((float)sStarProp.nStarBase + sStarProp.nStarGrow * (nStaticLv - 1));
		int nAddValue = fSumValue + nSumGrowRate * ((float)nNewLv - 1.0f);
		int nOldValue = 0;
		if (nOldLv != 0)
		{
			nOldValue = fSumValue + nSumGrowRate * (nOldLv - 1);
		}
		addProperty_Integer(uID, nPropID, nAddValue - nOldValue);

		//战场内英雄实际血量＝基础血量+星级总血量+（血量成长率+星级总血量成长率）*（场内等级-1）

#ifdef ENTITY_CHECK_SKIN
		//if(nPropID == PROPERTY_SKIN)
		//{
		//    TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< uID <<", value="<< nAddValue);
		//}
#endif

#ifdef PRINTF_PLANLOG
		//WarningLn("------------------------" << (ENTITY_PROPERTY)nPropID << "----------------------------------------------------");
		//WarningLn(__FUNCTION__":Planning requirements add sumGrow = " << nSumGrowRate);
		//WarningLn(__FUNCTION__":Planning requirements star add property = " << (nAddValue - nOldValue));
#endif

	}
}


void CWar::onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info)
{
	if (INVALID_UID == entityDieInfo.uDiedUID || INVALID_UID == entityDieInfo.uMurderUID)
	{
		//不知道谁杀死的
		ErrorLn(__FUNCTION__": -- uDiedUID =" << entityDieInfo.uDiedUID << " uMurderUID =" << entityDieInfo.uMurderUID);
		return;
	}
	// 玩家死亡
	if (UID_2_TYPE(entityDieInfo.uDiedUID) == TYPE_PLAYER_ROLE)
	{
		addPersonDeadInfo(entityDieInfo, info);

		setSumDieTime(info.nActiveInterval, entityDieInfo);

		// 死亡的人需要广播给其他助攻或者击杀 天赋点改变事件会广播
		if (getWarPhase() != EWP_End)
		{
			sendAllPersonInfoToAllClient();
		}
	}
	else
	{
		ErrorLn(__FUNCTION__": -- UID_2_TYPE(uidMaster) != TYPE_PLAYER_ROLE uidMaster =" << entityDieInfo.uDiedUID);
	}
}

void CWar::AddActivity()
{
	/// 获取配置中心接口
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (!pSchemeCenter)
	{
		ErrorLn(__FUNCTION__" -- pSchemeCenter == NULL");
		return;
	}
	ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeEntity == NULL");
		return;
	}

	ISchemeActivityCenter* pActivity = pSchemeCenter->getSchemeActivityCenter();
	if (!pActivity)
	{
		ErrorLn(__FUNCTION__" -- pActivity(ISchemeActivityCenter) == NULL");
		return;
	}

	int maxDayActivity = pSchemeEntity->getEntityXml()->nMaxDayActivity;
	ClanHelper clanHelper;
	KinHelper kinHelper;
	for (WarCampMap::iterator itCamp = m_mapWarCamp.begin(); itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector & personList = itCamp->second.warPersVect;
		for (WarPersonVector::iterator itPerson = personList.begin(); itPerson != personList.end(); ++itPerson)
		{
			if (!itPerson->bIsComputer)
			{
				ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
				IActorService *pActorService = actorHelper.m_ptr;
				if (pActorService == NULL)
				{
					ErrorLn(__FUNCTION__" -- pActorService == NULL itPerson->uPDBID =" << itPerson->uPDBID);
					continue;
				}

				int activity = pActorService->getDayActivity();
				if (activity >= maxDayActivity) // 判断活跃度是否达到上限值
				{
					TraceLn(__FUNCTION__" dayActivity has reached the upper limit.=" << activity);
					continue;
				}

				int addedActivity = pActivity->getActivity(EMACTIVITYTYPE_WAR, m_context.dwMatchTypeID);
				int weekActivity = pActorService->getWeekActivity();
				pActorService->setWeekActivity(weekActivity + addedActivity, false);
				pActorService->setDayActivity(activity + addedActivity);

				int clanID = pActorService->getProperty_Integer(PROPERTY_CLAN);
				if (clanID > 0)
				{
					if (clanHelper.m_ptr)
					{
						clanHelper.m_ptr->setNumProp(clanID, emClanProp_WeekActivity, addedActivity);
					}
					else
					{
						TraceLn(__FUNCTION__" not execute addWeekActivity operation.clanID=" << clanID);
					}
				}

				int kinID = pActorService->getProperty_Integer(PROPERTY_KIN);
				if (kinID > 0)
				{
					if (kinHelper.m_ptr)
					{
						kinHelper.m_ptr->setWeekActivity(kinID, addedActivity);
					}
					else
					{
						TraceLn(__FUNCTION__" not execute addWeekActivity operation.kinID=" << kinID);
					}
				}
			}
		}
	}
}

void CWar::getCampBanHero(int nCamp, WORD banList[5], size_t banNum)
{
	if (nCamp < 0 || nCamp >= CAMP_MAX)
		return;

	int i = 0;
	vector<int>::iterator iter = m_CampBanHeroList[nCamp].begin();
	for (; iter != m_CampBanHeroList[nCamp].end(); ++iter)
	{
		if (i >= banNum)
			break;

		banList[i] = *iter;

		i++;
	}
}

void CWar::getAllBanHero(WORD * banList, size_t banNum)
{
	vector<int> result;
	result.insert(result.end(), m_CampBanHeroList[CAMP_BLUE].begin(), m_CampBanHeroList[CAMP_BLUE].end());
	result.insert(result.end(), m_CampBanHeroList[CAMP_RED].begin(), m_CampBanHeroList[CAMP_RED].end());

	int i = 0;
	vector<int>::iterator iter = result.begin();
	for (; iter != result.end(); ++iter)
	{
		if (i >= banNum)
			break;

		banList[i] = *iter;

		i++;
	}
}

bool CWar::isPlayerFull(int nRoleCount)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (!pSchemeCenter)
		return false;

	ISchemeMatchRoom* pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom();
	if (!pSchemeMatchRoom)
		return false;

	SSchemeMatchRoom *pScheme = pSchemeMatchRoom->getSchemeMatchRoomByMatchTypeID(getMatchTypeID());
	if (!pScheme)
		return false;

	if (nRoleCount < pScheme->getPlayerCountMax())
		return false;

	return true;
}

const void CWar::getWarResultString(SWPerson* pPerson, char * szWarResultStr, size_t nLen, ISchemePersonModel* pPersonModel)
{
	int warResult[WRTT_Max] = { 0 };

	//STWarEndGraded grade;
	//map<PDBID, STWarEndGraded>::iterator it = m_mapWarEndGraded.find(pPerson->uPDBID);
	//if (it != m_mapWarEndGraded.end())
	//{
		//grade = it->second;
	//}

	BYTE nMentorshipPost = 0;
	warResult[WRTT_HeroType] = pPersonModel->GetHeroType(pPerson->nHeroID);
	warResult[WRTT_MatchType] = getMatchTypeID();
	warResult[WRTT_IsHaveClanMember] = isHaveSameClanMember(pPerson->uPDBID) ? 1 : 0;
	warResult[WRTT_IsHaveMentorship] = isHaveMentorship(pPerson->uPDBID, nMentorshipPost) ? 1 : 0;
	warResult[WRTT_IsWin] = (pPerson->nCamp == getWinCamp()) ? 1 : 0;
	warResult[WRTT_IsSuperGod] = getSuperGodCount(pPerson->uPDBID) > 0 ? 1 : 0;
	warResult[WRTT_IsMVP] = pPerson->nTitle & EWM_SCORE ? 1 : 0;
	warResult[WRTT_IsTopOutput] = pPerson->nTitle & EWM_OUTPUT ? 1 : 0;
	warResult[WRTT_IsTopKillTower] = pPerson->nTitle & EWM_DESTORYTOWER ? 1 : 0;
	warResult[WRTT_IsTopAssist] = pPerson->nTitle & EWM_ASSCOUNT ? 1 : 0;
	warResult[WRTT_IsTopGold] = pPerson->nTitle & EWM_MONEY ? 1 : 0;
	warResult[WRTT_TimeContKillMax] = pPerson->nTimeKill;
	warResult[WRTT_DeadContKillMax] = pPerson->nDeadKill;
	warResult[WRTT_KillNum] = pPerson->nBaseRecord[EDT_KillCount];
	warResult[WRTT_AssistNum] = pPerson->nBaseRecord[EDT_AssistCount];
	warResult[WRTT_Score] = pPerson->nBaseRecord[EDT_Score];
	warResult[WRTT_Gold] = pPerson->nBaseRecord[EDT_Money];
	warResult[WRTT_DeadlyControlNum] = pPerson->nBaseRecord[EDT_DeadlyControl];
	warResult[WRTT_ThrillSave] = pPerson->nBaseRecord[EDT_ThrillSave];
	warResult[WRTT_KillTowerNum] = pPerson->nBaseRecord[EDT_DestoryTower];
	warResult[WRTT_KillSmallBoss] = pPerson->nSmallBossMonster;
	warResult[WRTT_KillBigBoss] = pPerson->nBigBossMonster;
	warResult[WRTT_DeadNum] = pPerson->nDeadNum;


	stringstream ss;
	for (int i = 0; i < WRTT_Max; ++i)
	{
		ss << warResult[i] << ';';
	}

	string tmpStr = ss.str();
	if (nLen < tmpStr.size())
	{
		ErrorLn("The length of warResult is too long.");
	}
	else
	{
		sstrcpyn(szWarResultStr, tmpStr.c_str(), nLen);
	}
}

void CWar::End()
{
	EmphasisLn(__FUNCTION__": m_nSceneID=" << getWarSceneID() << " roomid=" << m_context.dwRoomID);
	if (isEnd())
	{
		return;
	}

	m_WarInfo.EndTime = time(NULL);

	m_WarInfo.dwContinueTime = m_WarInfo.EndTime - m_WarInfo.BeginTime;

	// 计算每个玩家的连杀、时间杀
	SceneLogicEventHelper helper(getWarSceneID());
	if (helper.m_ptr)
	{
		helper.m_ptr->FireExecute(EVENT_SCENE_WAR_END, SOURCE_TYPE_SCENE, 0, NULL, 0);
	}

	ICheckBadBehaviorMgr* pCheckBadBhvMgr = (ICheckBadBehaviorMgr*)getWarMgr(Mgr_CheckBadBehavior);
	if (pCheckBadBhvMgr != NULL)
	{
		pCheckBadBhvMgr->warEndBottingRec();
	}

	ILogRecordMgr* pLogRecordMgr = (ILogRecordMgr*)getWarMgr(Mgr_LogRecord);
	ISupportMgr* pSupportMgr = (ISupportMgr*)getWarMgr(Mgr_Support);
	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)getWarMgr(Mgr_Monster);

	// 角色战绩相关记录
	statisticsPlayerGraded();


	// 减少记录的玩家惩罚信息社会服的观察期
	warEndPunishSubObserveCount();

	// 战场结束，给玩家联盟和战队增加活跃值
	AddActivity();

	// 流程结束 但是不释放
	MatchSceneHelper().m_ptr->setWarProcessEnd(m_context.nWarID);

	// 停止怪物AI
	if (pWarMonsterMgr != NULL)
	{
		pWarMonsterMgr->stopMonsterAI();
	}

	// 停止电脑AI
	stopComputerAI();

	////释放战场怪物资源
	//if (!m_bReleaseAllMonster)
	//{
	//	releaseMonsterRes();
	//	m_bReleaseAllMonster = true;
	//}



	// 设置战场结果（计算完战绩相关数据才能设置）
	setLegendCupWarResult();

	// 设置失败阵营之后才能设置原因
	updateWarEndReason();

	// 添加天梯分数
	setMatchTypeRankScoreAndGrade();

    // 每个比赛类型胜场处理
    updatePlayerMatchTypeInfo();

	// 排位赛季数据统计
	setMatchRankSeasonInfo();

	//战绩统计 下发战场战绩数据
	calculateWarEndData();

	// 计算经济经验分布比例
	calculateEcmExpProportionList();

	// 强制将日记记录到数据库中
	if (pLogRecordMgr != NULL)
	{
		pLogRecordMgr->recordAddExpOrTalentLog(true);
	}

	// 杯赛结束信息处理
	sendLegendCupWarEndInfo();

	// 更新点赞人物信息
	if (pSupportMgr != NULL)
	{
		pSupportMgr->filterPlayersForSupportCondition(bIsCanReachAwarkCondition());
	}

	// 战场结束增加个人玩家信息
	addPlayerData();

	// 发结束消息
	msg_clear_all_mini_map_icon msgClearInfo;
	msgClearInfo.isClearAll = true;
	obuf oClearInfo;
	oClearInfo << msgClearInfo;
	broadCastDataToAllPlayer(SC_MSG_WAR_CLEAR_ALL_MINI_MAP_ICON, oClearInfo.data(), oClearInfo.size());

	setWarPhase(EWP_End);

	// 发送战绩表信息到数据库
	if (pLogRecordMgr != NULL)
	{
		judgeWarPersenResultType();
		pLogRecordMgr->fillWarActorLog(true);
		pLogRecordMgr->batchSaveWarActorLog(true);

		// 保存皮肤使用次数数据
		pLogRecordMgr->saveHeroSkinUseCount();

		// 保存本局被Ban英雄信息
		pLogRecordMgr->saveBanHeroListToDB();

		// 保存天梯分
		pLogRecordMgr->saveMatchTypeRankScoreAndGrade();

		// 记录英雄输赢数据
		pLogRecordMgr->recordHeroWinOrDefeat();
	}

	// 退出压力测试账号
	exitTester();

	// 结束子功能
	for (int i = 0; i < Mgr_Max; ++i)
	{
		__IWarMgr* pWarMgr = m_WarMgrList[i];
		if (pWarMgr != NULL)
		{
			pWarMgr->onWarEnd();
		}
	}

	m_WarInfo.bIsFinish = true;

	CWarBase::End();
}

void CWar::exitTester()
{
	MatchSceneHelper matchSceneHelper;
	IMatchSceneService *pMatchSceneService = matchSceneHelper.m_ptr;
	if (pMatchSceneService == NULL)
	{
		return;
	}

	// 直接退出战场
	for (WarCampMap::iterator itCamp = m_mapWarCamp.begin(); itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector &personList = itCamp->second.warPersVect;
		for (WarPersonVector::iterator itPerson = personList.begin(); itPerson != personList.end(); ++itPerson)
		{
			if (itPerson->bIsComputer)
				continue;

			ActorHelper helper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActor = helper.m_ptr;
			if (pActor == NULL)
			{
				ErrorLn(__FUNCTION__": -- pActor == NULL PDBID = " << itPerson->uPDBID);
				return;
			}

			if (pActor->isTester())
			{

				SNetMsgHead MsgHead;
				MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
				MsgHead.bySrcEndPoint = MSG_ENDPOINT_SCENE;
				MsgHead.byKeyModule = MSG_MODULEID_MATCH;
				MsgHead.byKeyAction = SS_MSG_WAR_END_TESTER_EXIT_WAR;
				
				pMatchSceneService->handleServerMsg(gServerGlobal->getServerID(), MsgHead, (void*)(&itPerson->uPDBID), sizeof(itPerson->uPDBID));
			}
		}
	}
}

bool CWar::isOpenCheckBadBehavior()
{
	ICheckBadBehaviorMgr* pCheckBadBhvMgr = (ICheckBadBehaviorMgr*)getWarMgr(Mgr_CheckBadBehavior);

	return pCheckBadBhvMgr != NULL;
}



void CWar::setMatchTypeRankScoreAndGrade()
{
	// 获得配置信息
	ISchemeMatchRankConfigInfo * pSchemeRankInfo = gServerGlobal->getSchemeCenter()->getSchemeMatchRankConfig();
	if (pSchemeRankInfo == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeRankInfo == NULL");
		return;
	}

	// 是否满足加分条件
	if (bIsCanReachAwarkCondition() == false)
	{
		return;
	}

    map<int, int> mapCampAvgHideRank;
    for (WarCampMap::iterator itCamp = m_mapWarCamp.begin(); itCamp != m_mapWarCamp.end(); ++itCamp)
    {
        
        WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
        int nSize = itCamp->second.warPersVect.size();
        for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
        {
            // 每个阵营的隐藏分
            mapCampAvgHideRank[itCamp->first] += itPerson->nHideRank;
        }

        if (nSize > 0)
        {
            mapCampAvgHideRank[itCamp->first] /= nSize;
        }
    }

	// 计算并设置得分
	for (WarCampMap::iterator itCamp = m_mapWarCamp.begin(); itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (IsComputerPDBID(itPerson->uPDBID))
				continue;

			ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				ErrorLn(__FUNCTION__" -- pActorService == NULL itPerson->uPDBID =" << itPerson->uPDBID);
				continue;
			}

            // 正常分数
			int nScore = 0;
            SMatchRankConfigSchemeInfo* pScheme = pSchemeRankInfo->getShemeInfoByTypeAndScore(getMatchTypeID(), pActorService->getMatchTypeRank((EMMatchType)getMatchTypeID()));
            if (pScheme == NULL)
            {
                continue;
            }
            if (getWinCamp() == itCamp->first)
            {
                nScore += pScheme->nStarSection;
            }
            else
            {
                nScore -= pScheme->nStarSection;
            }

            itPerson->nGetMatchTypeScore = nScore;
            
            // 计算B A得分 B:敌方阵营隐藏平均分 A：我方阵营隐藏平均分
            int B = 0;
            int A = 0;
            for(map<int,int>::iterator iter = mapCampAvgHideRank.begin(); iter != mapCampAvgHideRank.end(); ++iter)
            {
                if(itCamp->first == iter->first)
                {
                    // 我方
                    A = iter->second;
                }
                else
                {
                    // 敌方
                    B = iter->second;
                }
            }

            // 隐藏分数
            int nHideScore = 0;
            int nCureHideScore = pActorService->getMatchTypeHideRank((EMMatchType)getMatchTypeID());
            SMatchRankConfigSchemeInfo* pHideScheme = pSchemeRankInfo->getShemeHideInfoByTypeAndScore(getMatchTypeID(), nCureHideScore);
            if (pHideScheme != NULL)
            {
                if (getWinCamp() == itCamp->first)
                {
                    // 胜利得分= K*(1-1/(1+10^((B-A)/M)))
					nHideScore = (int)((float)(pHideScheme->nKRatio)*(1.0f - 1.0f/(1.0f+pow((float)10,((float)(B-A)/ (float)pHideScheme->nHideKRatio)))));
				}
                else
                {
                    // 失败得分= K*(0-1/(1+10^((B-A)/M)))
					nHideScore = (int)((float)(pHideScheme->nKRatio)*(0.0f - 1.0f/(1.0f+pow((float)10,((float)(B-A)/ (float)pHideScheme->nHideKRatio)))));
				}
            }

			// 添加匹配分
			pActorService->addMatchTypeRank((EMMatchType)getMatchTypeID(), nScore, nHideScore);
		}
	}
}

void CWar::setMatchRankSeasonInfo()
{	
	// 排位赛
	if (getMatchTypeID() != MatchType_Rank)
		return;

	// 休赛期不计入数据
	DWORD dwWarEndTime = m_WarInfo.EndTime;
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;
	ISchemeMatchSeasonTime * pSchemeSeasonTime = pSchemeCenter->getSchemeMatchSeasonTime();
	if (pSchemeSeasonTime == NULL)
		return;
	SSchemeMatchSeasonTime * pSchemeSeason = pSchemeSeasonTime->getSchemeMatchSeasonTimeByTime(dwWarEndTime);
	if (pSchemeSeason == NULL)
		return;

	// 设置排位赛季数据
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (IsComputerPDBID(itPerson->uPDBID))
				continue;

			ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				ErrorLn(__FUNCTION__" -- pActorService == NULL itPerson->uPDBID =" << itPerson->uPDBID);
				continue;
			}

			SRankSeasonWarEndInfo warInfo;
			warInfo.dwWarEndTime = dwWarEndTime;								// 战场结束时间
			warInfo.wMvpCount = isGetMVP(itPerson->uPDBID) ? 1 : 0;				// MVP数量
			warInfo.wSuperGodCount = getSuperGodCount(itPerson->uPDBID);		// 超神数量
			warInfo.wKill5Count = getTimeKillCountOfN(itPerson->uPDBID, 5);		// 5杀数量
			warInfo.wKill4Count = getTimeKillCountOfN(itPerson->uPDBID, 4);		// 4杀数量
			warInfo.wKill3Count = getTimeKillCountOfN(itPerson->uPDBID, 3);		// 3杀数量
			warInfo.wHeroID = itPerson->nHeroID;								// 使用英雄ID
			warInfo.bWin = m_WarInfo.nWinCamp == itPerson->nCamp;				// 是否胜利

			pActorService->setMatchRankSeasonInfo(warInfo);
		}
	}
}

bool CWar::bIsCanReachAwarkCondition()
{
	ISchemeOutputRestriction* pSchemeInfo = gServerGlobal->getSchemeCenter()->getSchemeOutputRestriction();
	if (pSchemeInfo == NULL)
	{
		ErrorLn(__FUNCTION__": can't find output restriction scheme config");
		return false;
	}
	int nMatchTypeID = getMatchTypeID();
	const SOutputRestrictionConfig* pConfig = pSchemeInfo->getOutputRestrictionConfig(nMatchTypeID);
	if (pConfig == NULL)
	{
		ErrorLn(__FUNCTION__": can't find pConfig, nMatchTypeId = " << nMatchTypeID);
		return false;
	}

	if (m_mapWarCamp.size() != pConfig->mapPersonNum.size())
	{
		return false;
	}

	std::map<int, int>::const_iterator itCondition = pConfig->mapPersonNum.begin();
	for (; itCondition != pConfig->mapPersonNum.end(); ++itCondition)
	{
		WarCampMap::iterator it = m_mapWarCamp.find(itCondition->first);
		if (it == m_mapWarCamp.end()) return false;

		if (it->second.warPersVect.size() < itCondition->second) return false;
	}

	if (m_WarInfo.dwContinueTime / 60 < pConfig->nMatchTime)
	{
		return false;
	}

	if (m_WarInfo.nPersonNum < pConfig->nPersonMin)
	{
		return false;
	}

	return true;
}


DWORD CWar::getCampMatchTypeAverageScore(int nCamp, bool bIncludeAI)
{
	WarCampMap::iterator iter = m_mapWarCamp.find(nCamp);
	if (iter == m_mapWarCamp.end())
	{
		return 0;
	}

	if (bIncludeAI)
	{
		int nPlayerCount = iter->second.nAIPlayerCount + iter->second.nRealPlayerCount;
		if (nPlayerCount == 0)
		{
			nPlayerCount = 1;
		}
		return iter->second.nTotalMatchScore / nPlayerCount;
	}
	else
	{
		// 只计算真人的数据
		if (iter->second.nRealPlayerCount <= 0)
		{
			return 0;
		}
		return iter->second.nMatchScoreRealPlayer / iter->second.nRealPlayerCount;
	}
}

void CWar::OnTimer(unsigned long dwTimerID)
{
	if (dwTimerID < ETimerEventID_Max)
	{
		// 发送信息到客户端
		if (ETimerEventID_WarProcess == dwTimerID)
		{
			switch (getWarPhase())
			{
				// 准备，读图，需要在开始前进行校验
			case EWP_Ready:
				{
					// 还未开始
					DWORD CurTime = gServerGlobal->getTimeSyncService()->GetTick();
					if (getWarStarTime() > CurTime)
					{
						//m_STime + m_StartTime*1000 > CurTime
						//if(m_STime + m_PromptTime*1000 > CurTime)
						//{
						//	// 进行倒计时提示

						//}
					}
				}
				break;

				// 开始倒计时
			case EWP_Start:
				{
					DWORD cuTickCount = gServerGlobal->getTimeSyncService()->GetTick();

					// 自动添加天赋点 
					if (m_nAutoTalentPoint > 0 && m_TempAddTalentTime > 0)
					{

						if (m_nFirstAutoAddTalentInterval > 0 && m_TempAddTalentTime + m_nFirstAutoAddTalentInterval * 1000 <= cuTickCount)
						{
							m_TempAddTalentTime = m_TempAddTalentTime + m_nFirstAutoAddTalentInterval * 1000;
							//第一次开始加天赋点 给所有人添加天赋点
							addWarTimerPersonTalentValue(m_nAutoTalentPoint);
							m_nFirstAutoAddTalentInterval = 0;
						}
						else if (m_nFirstAutoAddTalentInterval == 0 && m_TempAddTalentTime + m_nAutoAddTalentInterval * 1000 <= cuTickCount)
						{
							m_TempAddTalentTime = m_TempAddTalentTime + m_nAutoAddTalentInterval * 1000;
							//给所有人添加天赋点
							addWarTimerPersonTalentValue(m_nAutoTalentPoint);
						}
					}


					// 自动添加经验 
					if (m_nAutoAddExp > 0 && m_ConfigAddExpTime > 0)
					{
						if (m_nFirstAutoAddExpInterval > 0 && m_ConfigAddExpTime + m_nFirstAutoAddExpInterval * 1000 <= cuTickCount)
						{
							m_ConfigAddExpTime = m_ConfigAddExpTime + m_nFirstAutoAddExpInterval * 1000;
							//第一次开始加经验
							addWarTimerPersonExpValue(m_nAutoAddExp);
							m_nFirstAutoAddExpInterval = 0;
						}
						else if (m_nFirstAutoAddExpInterval == 0 && m_ConfigAddExpTime + m_nAutoAddExpInterval * 1000 <= cuTickCount)
						{
							m_ConfigAddExpTime = m_ConfigAddExpTime + m_nAutoAddExpInterval * 1000;
							//给所有人添加经验
							addWarTimerPersonExpValue(m_nAutoAddExp);
						}
					}

					// 检测xp技能
					IXPSkillMgr* pXPSkillMgr = (IXPSkillMgr*)getWarMgr(Mgr_XPSKill);
					if (pXPSkillMgr != NULL)
						pXPSkillMgr->checkPlayerXpSkillChangeInTimes();
				}
				break;

			case EWP_End:
				{
					// 只做空处理，后续，在进行扩展，是否进行倒计时提示。
				}
				break;

			default:
				break;
			}
		}
		else if (ETimerEventID_DeadKillCount == dwTimerID)
		{
			// 这里是战斗连杀清除
			WarCampMap::iterator itCamp = m_mapWarCamp.begin();
			for (; itCamp != m_mapWarCamp.end(); ++itCamp)
			{
				WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
				for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
				{
					// 战场连杀清除处理
					if (itPerson->dwTimeContKilltick > 0 && itPerson->dwTimeContKilltick + m_nTimeCountKillInterval < gServerGlobal->getTimeSyncService()->GetTick())
					{
						// 设置整场战斗的连杀最大值
						setKillMaxInfo(&(*itPerson));

						itPerson->timeContKillList.push_back(itPerson->nTimeContKill);
						itPerson->nTimeContKill = 0;
						//顺便把战斗连杀计时也清理了减少逻辑判断
						itPerson->dwTimeContKilltick = 0;
					}
				}
			}
		}
		else if (ETimerEventID_WarActorLog == dwTimerID)      // 记录一场战斗的记录信息
		{
			// 暂时注释 数据库只存储不更新 之后需要的话再加上
			//EWarPhase wWarPhase = getWarPhase();
			//if (wWarPhase == EWP_Start)
			//{
			//	// 保存战场对战信息
			//	if (m_pWarLogRecord != NULL)
			//	{
			//		m_pWarLogRecord->fillWarActorLog(false);
			//		m_pWarLogRecord->batchSaveWarActorLog(false);
			//	}
			//}
		}
		else if (ETimerEventID_EcmExpSituation == dwTimerID)
		{
			updateWarEcmExpSituation();
		}
		else if (ETimerEventID_EcmControlRatio == dwTimerID)
		{
			// 真人玩家经济控制
			IEcmCtrlMgr* pEcmCtrlMgr = (IEcmCtrlMgr*)getWarMgr(Mgr_EcmCtrl);
			if (pEcmCtrlMgr != NULL)
				pEcmCtrlMgr->calcEcmControlRatio();

			// AI玩家经济控制
			IAIEcmCtrlMgr* pAIEcmCtrlMgr = (IAIEcmCtrlMgr*)getWarMgr(Mgr_AIEcmCtrl);
			if (pAIEcmCtrlMgr != NULL)
				pAIEcmCtrlMgr->calcAIEcmCtrlRatio();

			// AI玩家经验控制
			IAIExpCtrlMgr* pAIExpCtrlMgr = (IAIExpCtrlMgr*)getWarMgr(Mgr_AIExpCtrl);
			if (pAIExpCtrlMgr != NULL)
				pAIExpCtrlMgr->calcAIExpCtrlRatio();
		}
		else if (ETimerEventID_EcmControlLine == dwTimerID)
		{

			// 真人玩家经济控制
			IEcmCtrlMgr* pEcmCtrlMgr = (IEcmCtrlMgr*)getWarMgr(Mgr_EcmCtrl);
			if (pEcmCtrlMgr != NULL)
				pEcmCtrlMgr->calcEcmControlLine();

			// AI玩家经济控制
			IAIEcmCtrlMgr* pAIEcmCtrlMgr = (IAIEcmCtrlMgr*)getWarMgr(Mgr_AIEcmCtrl);
			if (pAIEcmCtrlMgr != NULL)
				pAIEcmCtrlMgr->calcAIEcmCtrlLine();

			// AI玩家经验控制
			IAIExpCtrlMgr* pAIExpCtrlMgr = (IAIExpCtrlMgr*)getWarMgr(Mgr_AIExpCtrl);
			if (pAIExpCtrlMgr != NULL)
				pAIExpCtrlMgr->calcAIExpCtrlLine();
		}
	}
	else
	{
		int nID = dwTimerID - ETimerEventID_Max;
		if (nID >= MAX_WAR_BROADCAST_PROCESS || nID < 0)
		{
			return;
		}
		// 广播流程语音文字信息
		msg_war_process_broadcast broadCastInfo;
		broadCastInfo.nBroadCastID = m_BroadCastTimeList[nID].BroadCastID;
		if (broadCastInfo.nBroadCastID > 0)
		{
			obuf obInfo;
			obInfo << broadCastInfo;

			broadCastDataToAllPlayer(SC_MSG_WAR_PROCESS_BROADCAST, obInfo.data(), obInfo.size());
		}
	}
}

void CWar::addWarTimerPersonTalentValue(int nAdd)
{
	if (isEnd())
	{
		return;
	}

	std::map<int, int> mapMaxEcm;
	ISchemeAIEcmAdjust* pAIEcmAdjustScheme = NULL;

	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (itPerson->uPDBID == INVALID_PDBID)
			{
				ErrorLn(__FUNCTION__": PDBID == INVALID_PDBID!");
				continue;
			}
			ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				if (!itPerson->bIsComputer)
				{
					//todo 临时关闭 退出战场服务已经释放了 
					//ErrorLn(__FUNCTION__": pActorService == NULL PDBID = "<<itPerson->uPDBID<<" UID = "<<itPerson->uID);
				}
				continue;
			}

			// 经济差控制
			int nNewAdd = nAdd;
			if (itPerson->bIsComputer)
			{
				// AI玩家经济控制
				IAIEcmCtrlMgr* pAIEcmCtrlMgr = (IAIEcmCtrlMgr*)getWarMgr(Mgr_AIEcmCtrl);
				if (pAIEcmCtrlMgr != NULL)
				{
					nNewAdd = pAIEcmCtrlMgr->controlTelentPoint(itPerson->uPDBID, nAdd, ETALENTDROPTYPE_NORMAL);
				}
			}
			else
			{
				IEcmCtrlMgr* pEcmCtrlMgr = (IEcmCtrlMgr*)getWarMgr(Mgr_EcmCtrl);
				if (pEcmCtrlMgr != NULL)
				{
					nNewAdd = pEcmCtrlMgr->controlTelentPoint(itPerson->uPDBID, nAdd, ETALENTDROPTYPE_NORMAL);
				}
			}

			addPersonTalentPoint(itPerson->uID, nNewAdd, ETALENTDROPTYPE_NORMAL);
		}
	}
}

void CWar::addWarTimerPersonExpValue(int nAdd)
{
	if (isEnd())
	{
		return;
	}

	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (itPerson->uPDBID == INVALID_PDBID)
			{
				ErrorLn(__FUNCTION__": PDBID == INVALID_PDBID!");
				continue;
			}
			ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService != NULL && nAdd != 0)
			{
				if (itPerson->bIsComputer)
				{
					IAIExpCtrlMgr* pAIExpCtrlMgr = (IAIExpCtrlMgr*)getWarMgr(Mgr_AIExpCtrl);
					if (pAIExpCtrlMgr != NULL)
					{
						nAdd = pAIExpCtrlMgr->controlExp(itPerson->uPDBID, nAdd, EEXPDROPTYPE_NORMAL);
					}
				}

				addPersonExp(itPerson->uID, nAdd, EEXPDROPTYPE_NORMAL);
			}
		}
	}
}


void CWar::iniWarActorInfo(const SActorPreaperEnterWar & sActorPreaperEnterWar)
{
	SPendingCreateActor sPending;
	sPending.uPDBID = sActorPreaperEnterWar.uPDBID;
	sPending.nHeroID = sActorPreaperEnterWar.nHeroID;
	sPending.nHeroSkinID = sActorPreaperEnterWar.nHeroSkinID;
	sPending.nCamp = sActorPreaperEnterWar.nCamp;
	sPending.nIndex = sActorPreaperEnterWar.nIndex;
	sPending.nSrcWorldID = sActorPreaperEnterWar.nSrcWorldID;
	sPending.nSrcSceneID = sActorPreaperEnterWar.nSrcSceneID;
	sPending.vPosition = sActorPreaperEnterWar.vPosition;
	sPending.vAngle = sActorPreaperEnterWar.vAngle;
	sPending.bIsComputer = sActorPreaperEnterWar.isComputer;
	sPending.byHeroStarLv = sActorPreaperEnterWar.byHeroStarLv;                             // 英雄星级
	sPending.byHeroStaticLv = sActorPreaperEnterWar.byHeroStaticLv;;                            // 英雄外部静态等级
	sPending.byActorSex = sActorPreaperEnterWar.byActorSex;
	sstrcpyn(sPending.szName, sActorPreaperEnterWar.szName, sizeof(sPending.szName));
	m_mapPendingCreateActor[sActorPreaperEnterWar.uPDBID] = sPending;

	// 基础数据放入阵营数据里面
	WarCampMap::iterator itCamp = m_mapWarCamp.find(sActorPreaperEnterWar.nCamp);
	if (itCamp == m_mapWarCamp.end())
	{
		ErrorLn(__FUNCTION__": unknown camp:" << sActorPreaperEnterWar.nCamp);
		return;
	}

	SWPerson person;
	person.uID = 0;
	person.ulClient = 0;
	person.uPDBID = sActorPreaperEnterWar.uPDBID;
	person.nHeroID = sActorPreaperEnterWar.nHeroID;
	person.nHeroSkinID = sActorPreaperEnterWar.nHeroSkinID;
	person.nIndex = sActorPreaperEnterWar.nIndex;
	person.bInWar = false;
	person.nSrcSceneID = 0;
	person.nSrcWorldID = 0;
	person.bIniTalentPointAdd = false;
	person.bIsComputer = sActorPreaperEnterWar.isComputer;
	person.nSummerID = sActorPreaperEnterWar.nSummonserSkill[0];
	person.nXPSkillID = sActorPreaperEnterWar.nHeroXPSkillID;
	person.byHeroStarLv = sActorPreaperEnterWar.byHeroStarLv;
	person.byHeroStaticLv = sActorPreaperEnterWar.byHeroStaticLv;
	person.byActorSex = sActorPreaperEnterWar.byActorSex;
	person.nAIID = sActorPreaperEnterWar.nAIID;
    person.nHideRank = sActorPreaperEnterWar.nHideRank;
	sstrcpyn(person.szName, sActorPreaperEnterWar.szName, sizeof(person.szName));
	person.byPlayerType = sActorPreaperEnterWar.isComputer ? (m_context.byRoomType == HallRoomType_Macth ? EWPT_VIRTUALPEOPLE : EWPT_COMPUTER) : EWPT_REALPEOPLE;

	// 计算HeroStarLvEx
	person.byHeroStarLvEx = g_EHelper.isCanUseHeroStarLv(getMatchTypeID()) ? person.byHeroStarLv : 0;

	if (person.bIsComputer)
	{
		m_bHaveComputer = true;
	}

	itCamp->second.warPersVect.push_back(person);

	// 统计阵营人数
	//(m_mapCampRecord[sActorPreaperEnterWar.nCamp].byCampNumber)++;
	//if (sActorPreaperEnterWar.isComputer)
		//(m_mapCampRecord[sActorPreaperEnterWar.nCamp].byComputerNum)++;
	//else
		//(m_mapCampRecord[sActorPreaperEnterWar.nCamp].byRealPlayerNum)++;

	// 统计总人数
	m_WarInfo.nPersonNum++;
}

void CWar::onEntityDieBroadCast(sEntityDieInfo entityDieInfo, int nBroadCastIndex)
{
	//死亡实体信息初始化
	msg_war_important_entiy_dieinfo ImportantEntityDieInfo;
	ImportantEntityDieInfo.nBroadCastIndex = nBroadCastIndex;
	ImportantEntityDieInfo.bIsFirstBlood = false;
	ImportantEntityDieInfo.bIsCampSkill = false;
	ImportantEntityDieInfo.nCamp = entityDieInfo.nMurderCamp;
	ImportantEntityDieInfo.MasterEntityType = TYPE_MONSTER;
	ImportantEntityDieInfo.MasterHeroID = entityDieInfo.uDiedPropertyID;
	ImportantEntityDieInfo.uidMaster = entityDieInfo.uDiedUID;
	ImportantEntityDieInfo.uidMurderer = entityDieInfo.uMurderUID;
	sstrcpyn(ImportantEntityDieInfo.MasterName, entityDieInfo.szDiedName, sizeof(ImportantEntityDieInfo.MasterName));
	sstrcpyn(ImportantEntityDieInfo.MurdererName, entityDieInfo.szMurderName, sizeof(ImportantEntityDieInfo.MurdererName));

	if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_PLAYER_ROLE)
	{
		// 查找人物
		ImportantEntityDieInfo.MurdererEntityType = TYPE_PLAYER_ROLE;
		ImportantEntityDieInfo.MurdererHeroID = entityDieInfo.nMurderVocation;
	}
	else if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_MONSTER)
	{
		ImportantEntityDieInfo.MurdererEntityType = TYPE_MONSTER;
		ImportantEntityDieInfo.MurdererHeroID = entityDieInfo.uMurderPropertyID;
	}

	vector<msg_war_entity_assist_info> assistList;	// 助攻者列表
	assistList.clear();
	ImportantEntityDieInfo.nAssistCount = 0;
	sendToClientImportantEntityDie(ImportantEntityDieInfo, assistList);
}

void CWar::onEntityDie(sEntityDieInfo entityDieInfo)
{
	//怪物死亡
	if (INVALID_UID == entityDieInfo.uDiedUID || INVALID_UID == entityDieInfo.uMurderUID)
	{
		//不知道谁杀死的
		return;
	}

	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pWarMonsterMgr == NULL");
		return;
	}

	//获得怪物类型
	EWarMonsterExec nType = pWarMonsterMgr->getWarMonsterTypeByUID(entityDieInfo.uDiedUID);

	if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_PLAYER_ROLE)
	{
		// 查找人物
		SWPerson* pPerson = getPlayerInfo(entityDieInfo.uMurderUID);
		if (pPerson != NULL)
		{
			// 添加击杀怪物数量   
			switch (nType)
			{
			case EWME_Tower:
				{
					pPerson->nBaseRecord[EDT_DestoryTower]++;
				}
				break;
			case EWME_Crystal:
				{
					pPerson->nCrystalNum++;
				}
				break;
			case EWME_Soldier:
				{
					pPerson->nBaseRecord[EDT_KillMonster]++;
				}
				break;
			case EWME_Monster:
				{
					pPerson->nFieldMonster++;
					pPerson->nBaseRecord[EDT_KillMonster]++;
				}
				break;
			case EWME_BOSS:
				{
					pPerson->nBaseRecord[EDT_KillMonster]++;

					// 计算Boss类型
					MonsterHelper helper(entityDieInfo.uDiedUID);
					INPCService* pNPCService = helper.m_ptr;
					if (pNPCService == NULL)
					{
						break;
					}

					EWBossType bossType = EWBossType_NONE;
					if (entityDieInfo.nDiedVocation == MONSTER_SUB_TYPE_SMALL_DRAGON)
					{
						bossType = EWBossType_SMALLDRAGON;
					}
					else if (entityDieInfo.nDiedVocation == MONSTER_SUB_TYPE_HUGE_DRAGON)
					{
						bossType = EWBossType_BIGDRAGON;
					}

					// 计算大小龙击杀数
					calcAroundBossCount(entityDieInfo.nMurderCamp, entityDieInfo.uMurderUID, bossType);
				}
				break;
			default:
				{
					ErrorLn(" monster type not have this type nType = " << nType);
				}
			}
		}
	}

	pWarMonsterMgr->onMonsterDie(&entityDieInfo);

}

void CWar::setSumDieTime(time_t nTime, sEntityDieInfo entityDieInfo)
{
	int nCamp = entityDieInfo.nDiedCamp;
	WarCampMap::iterator itFindCamp = m_mapWarCamp.find(nCamp);
	if (itFindCamp == m_mapWarCamp.end())
	{
		return;
	}
	WarPersonVector::iterator itFindPerson = itFindCamp->second.warPersVect.begin();

	// 单位分钟
	int nMinTime = nTime / 1000;

	for (; itFindPerson != itFindCamp->second.warPersVect.end(); ++itFindPerson)
	{
		if (entityDieInfo.uDiedUID == itFindPerson->uID)
		{
			itFindPerson->nSumDieTime += nMinTime;
			break;
		}
	}
	msg_war_important_entiy_reliveinfo ReliveInfo;

	ReliveInfo.dwRevliveTime = (gServerGlobal->getTimeSyncService()->GetTick() + nTime);// info.nActiveInterval;
	ReliveInfo.uReliveUID = entityDieInfo.uDiedUID;
	ReliveInfo.uRelivePDBID = entityDieInfo.uDiedPropertyID;
	obuf oEntityReliveInfo;
	oEntityReliveInfo << ReliveInfo;
	broadCastDataToAllPlayer(SC_MSG_WAR_IMPORTANT_ENTITYRELIVE, oEntityReliveInfo.data(), oEntityReliveInfo.size());
}

// 玩家死亡信息
void CWar::addPersonDeadInfo(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info, bool bBcastTips)
{
	// 设置玩家复活信息
	setReliveInfo(entityDieInfo, info);

	// 玩家死亡处理
	IWarDropMgr* pWarDropMgr = (IWarDropMgr*)getWarMgr(Mgr_Drop);
	if (pWarDropMgr != NULL)
	{
		pWarDropMgr->dealPersonDie(entityDieInfo, bBcastTips);
	}
}

void CWar::setReliveInfo(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info)
{
	SWPerson* pDiedPerson = getPlayerInfo(entityDieInfo.uDiedUID);
	if (pDiedPerson == NULL)
	{
		ErrorLn(__FUNCTION__": pDiedPerson == NULL uDiedUID =" << entityDieInfo.uDiedUID);
		return;
	}

	// 设置复活信息
	int nDiePersonLevel = entityDieInfo.nLevel;
	int nDiePersonCamp = entityDieInfo.nDiedCamp;
	int nDiePersonIndex = pDiedPerson->nIndex;

	info.nSceneID = getWarSceneID();
	// 复活CD
	info.nActiveInterval = getHeroReliveTime(nDiePersonLevel);

	// 复活点
	WarCampMap::iterator itFindCamp = m_mapWarCamp.find(entityDieInfo.nDiedCamp);
	if (itFindCamp == m_mapWarCamp.end())
	{
		ErrorLn(__FUNCTION__": camp not found entityDieInfo.nDiedCamp =" << entityDieInfo.nDiedCamp);
		return;
	}
	if (!itFindCamp->second.vecRebornPos.empty())
	{
		int nIndex = 0;
		if (nDiePersonIndex >= 0)
		{
			nIndex = nDiePersonIndex % itFindCamp->second.vecRebornPos.size();
		}
		else
		{
			nIndex = rand() % itFindCamp->second.vecRebornPos.size();
		}
		const SPosWithAngle& pos = itFindCamp->second.vecRebornPos[nIndex];

		info.x = pos.ptPos.x;
		info.y = pos.ptPos.y;
		info.z = pos.ptPos.z;
		info.fAngleY = pos.fAngleY;
	}
}

void CWar::sendToClientShowKillIcon(UID uMurderId, int nKillCount)
{
	if (nKillCount == 0 || uMurderId == 0)
	{
		WarningLn(__FUNCTION__ ": uMurderId = " << uMurderId << ", nKillCount = " << nKillCount);
		return;
	}
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = SC_MSG_WAR_COMBA_KILL;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = uMurderId;
	entityHead.byPartID = PART_WAR;

	obuf obuffData;
	obuffData << head << entityHead;
	int nSkillNum = nKillCount;
	obuffData.push_back(&nSkillNum, sizeof(nSkillNum));
	send2Client(uMurderId, PACKAGE_PTR(new std::string(obuffData.data(), obuffData.size())));
}

void CWar::setKillMaxInfo(SWPerson* pPerson)
{
	// 记录整场战斗的最高值
	int nKill = pPerson->nDeadKill;
	// 当前的击杀值
	int nContKill = pPerson->nDeadContKill;

	pPerson->nDeadKill = nKill < nContKill ? nContKill : nKill;

	nKill = pPerson->nTimeKill;
	nContKill = pPerson->nTimeContKill;
	pPerson->nTimeKill = nKill < nContKill ? nContKill : nKill;


	pPerson->nAliveContAssistMax = max(pPerson->nAliveContAssistMax, pPerson->nAliveContAssist);
}

void CWar::sendAllTabUpdateInfoToClient(UID uId)
{
	if (getWarPhase() == EWP_End)
	{
		return;
	}

	int nPersonCount = 0;
	// 组织数据发送到客户端
	obuf oAllPersonInfo;
	obuf oAllPerson;

	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{

			msg_war_update_persontab tempInfo;
			tempInfo.uPDBID = itPerson->uPDBID;
			tempInfo.uID = itPerson->uID;
			tempInfo.nLevel = itPerson->nLevel;
			tempInfo.nKillNum = itPerson->nBaseRecord[EDT_KillCount];
			tempInfo.nKillMonsterNum = itPerson->nBaseRecord[EDT_Money];
			tempInfo.nAssists = itPerson->nBaseRecord[EDT_AssistCount];
			tempInfo.nDeadNum = itPerson->nDeadNum;
			tempInfo.nContKill = itPerson->deadContKillList.back();
			tempInfo.bIsOnGame = itPerson->bIsOnGame;
            tempInfo.nDestroyTower = itPerson->nBaseRecord[EDT_DestoryTower];

			oAllPerson << tempInfo;
			nPersonCount++;
		}
	}

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = SC_MSG_WAR_UPDATE_PERSONTAB;

	msg_war_person_info_list PersonList;
	PersonList.nNum = nPersonCount;

	oAllPersonInfo << PersonList;
	oAllPersonInfo.push_back(oAllPerson.data(), oAllPerson.size());

	obuf oSendData;
	oSendData << head;
	oSendData.push_back(oAllPersonInfo.data(), oAllPersonInfo.size());

	send2Client(uId, PACKAGE_PTR(new std::string(oSendData.data(), oSendData.size())));

}

bool CWar::judgeCampIsAce(int nCamp)
{
	WarCampMap::iterator itCamp = m_mapWarCamp.find(nCamp);
	if (itCamp == m_mapWarCamp.end())
	{
		ErrorLn(__FUNCTION__ ": m_mapWarCamp not have nCamp = " << nCamp);
		return false;
	}

	WarPersonVector::iterator itBegin = itCamp->second.warPersVect.begin();
	for (; itBegin != itCamp->second.warPersVect.end(); ++itBegin)
	{
		// 如果有一个没死就不是团灭
		if (itBegin->lifeState != ELS_Dead)
		{
			return false;
		}
	}

	itCamp->second.isAce = true;

	return true;
}

void CWar::addPersonExp(UID uid, int nAddExp, int nExpDropType)
{
	ActorHelper actorHelper(uid);
	IActorService *pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
	{
		return;
	}

	pActorService->addExp(nAddExp, 0);

	SWPerson* pPerson = getPlayerInfo(uid);
	if (pPerson == NULL)
	{
		return;
	}

	pPerson->nExperien = pPerson->nExperien + nAddExp;

	ILogRecordMgr* pLogRecord = (ILogRecordMgr*)getWarMgr(Mgr_LogRecord);
	if (pLogRecord != NULL)
	{
		pLogRecord->setAddExpToMap(pPerson->uPDBID, nAddExp, (EExpDropType)nExpDropType);
	}
}

void CWar::addPersonTalentPoint(UID uid, int nTalentPoint, int nTalentDropType)
{
	ActorHelper actorHelper(uid);
	IActorService *pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
	{
		return;
	}

	pActorService->updateTalentPoint(nTalentPoint, DropT2ChangeT((ETalentDropType)nTalentDropType));
}


void CWar::calculateWarEndData()
{
	ISchemeMatchRankConfigInfo* pSchemeInfo = gServerGlobal->getSchemeCenter()->getSchemeMatchRankConfig();
	if (pSchemeInfo == NULL)
	{
		return;
	}

	msg_war_entity_end_warinfo_list WarInfoList;
	WarInfoList.dwDBWarID = getWarDBKey();
	WarInfoList.nWinCamp = getWinCamp();
	WarInfoList.uUseTime = m_WarInfo.dwContinueTime;
	WarInfoList.xBasePos = m_FailedCampBasePos.x * ACTOR_LOCATION_SCALE;
	WarInfoList.yBasePos = m_FailedCampBasePos.y * ACTOR_LOCATION_SCALE;
	WarInfoList.zBasePos = m_FailedCampBasePos.z * ACTOR_LOCATION_SCALE;

	//广播战绩数据
	obuf PersonListHeadInfo;
	obuf PersonListWarInfo;

	//结算所有人物战场信息并且下发
	map<int, SCampTeamRecordInfo>::iterator itTeamResult;
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		itTeamResult = m_mapCampTeamRecord.find(itCamp->first);
		if (itTeamResult == m_mapCampTeamRecord.end())
		{
			continue;
		}

		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			setKillMaxInfo(&(*itPerson));

			//所有人物
			WarInfoList.nPlayerCount++;

			msg_war_entity_end_warinfo		WarInfo;

			WarInfo.pDbid = itPerson->uPDBID;

			WarInfo.nCamp = itCamp->first;

			// 玩家序号位
			WarInfo.nIndex = itPerson->nIndex;

			// 玩家ID
			WarInfo.uID = itPerson->uID;

			// 英雄ID
			WarInfo.nHeroID = itPerson->nHeroID;

			// 玩家等级
			WarInfo.nLevel = itPerson->nLevel;

			// 经验
			WarInfo.nExperien = itPerson->nExperien;

			/////////////////////////////////////////战斗统计////////////////////////////////////
			// 击杀
			WarInfo.gzAllData[ERDT_KILLNUM] = itPerson->nBaseRecord[EDT_KillCount];

			// 死亡次数
			WarInfo.gzAllData[ERDT_DEADNUM] = itPerson->nDeadNum;

			// 助攻数
			WarInfo.gzAllData[ERDT_ASSIST] = itPerson->nBaseRecord[EDT_AssistCount];

			// kda
			int nDeadNum = max(itPerson->nDeadNum, 1);
			WarInfo.gzAllData[ERDT_SCORE] = itPerson->nBaseRecord[EDT_Score];

			// 每分钟金钱量
			int nContinueMin = max(m_WarInfo.dwContinueTime / 60, 1);
			WarInfo.gzAllData[ERDT_GPM] = itPerson->nBaseRecord[EDT_Money] / nContinueMin;

			// 第一滴血
			WarInfo.gzAllData[ERDT_FIRSTBLOOD] = itPerson->bFirstBlood ? 1 : 0;

			// 决胜控制
			WarInfo.gzAllData[ERDT_DEADLYCONTROL] = itPerson->nBaseRecord[EDT_DeadlyControl];

			/////////////////////////////////////////团队贡献////////////////////////////////////
			// 金钱比
			int nCampMoney = max(itTeamResult->second.nCampMoney, 1);
			WarInfo.gzAllData[ERDT_MONEYRATE] = 100 * ((float)itPerson->nBaseRecord[EDT_Money] / nCampMoney);

			// 输出率
			int nCampOutput = max(itTeamResult->second.nCampOutput, 1);
			WarInfo.gzAllData[ERDT_OUTPUTRATE] = 100 * ((float)itPerson->nBaseRecord[EDT_OutPut] / nCampOutput);

			// 参战率
			int nCampKillNum = max(itTeamResult->second.nCampKillNum, 1);
			WarInfo.gzAllData[ERDT_JOINBATTLERATE] = 100 * ((float)itPerson->nBaseRecord[EDT_KillCount] + itPerson->nBaseRecord[EDT_AssistCount]) / nCampKillNum;

			// 输出伤害
			WarInfo.gzAllData[ERDT_OUTPUT] = itPerson->nBaseRecord[EDT_OutPut];

			// 输出物理伤害
			WarInfo.gzAllData[ERDT_OUTDAMAGEPHYSICS] = itPerson->nOutHeroPhysics;

			// 输出魔法伤害
			WarInfo.gzAllData[ERDT_OUTDAMAGEMAGIC] = itPerson->nOutHeroMagic;

			// 输出真实伤害
			WarInfo.gzAllData[ERDT_OUTDAMAGETRUE] = itPerson->nOutHeroTrue;

			// 承受伤害
			WarInfo.gzAllData[ERDT_BEARDAMAGE] = itPerson->nBaseRecord[EDT_Damage];

			// 承受的物理伤害
			WarInfo.gzAllData[ERDT_BEARPHYSIC] = itPerson->nBearPhysic;

			// 承受的魔法伤害
			WarInfo.gzAllData[ERDT_BEARMAGIC] = itPerson->nBearMagic;

			// 承受的魔法伤害
			WarInfo.gzAllData[ERDT_BEARTRUE] = itPerson->nBearTrue;

			// 金钱量
			WarInfo.gzAllData[ERDT_MONEY] = itPerson->nBaseRecord[EDT_Money];

			// 总治疗
			WarInfo.gzAllData[ERDT_CURE] = itPerson->nBaseRecord[EDT_Cure];

			// 恢复的生命值
			WarInfo.gzAllData[ERDT_RECOVERYHP] = itPerson->nRecoveryHP;

			///////////////////////////////////////////精彩表现//////////////////////////
			// 最高连杀
			WarInfo.gzAllData[ERDT_TIMEKILLMAX] = itPerson->nTimeKill;

			// 最高多杀
			WarInfo.gzAllData[ERDT_ALIVECONTKILLMAX] = itPerson->nDeadKill;

			// 最高连续助攻
			WarInfo.gzAllData[ERDT_ALIVECOUTASSISTMAX] = itPerson->nAliveContAssistMax;

			// 惊险救治
			WarInfo.gzAllData[ERDT_THRILLSAVE] = itPerson->nBaseRecord[EDT_ThrillSave];

			// 破塔
			WarInfo.gzAllData[ERDT_TOWER] = itPerson->nBaseRecord[EDT_DestoryTower];

			// 最大暴击
			WarInfo.gzAllData[ERDT_CRILDAMAGEMAX] = itPerson->nMaxCritical;

			// 控龙
			WarInfo.gzAllData[ERDT_DRAGEN] = itPerson->nBaseRecord[EDT_KillDragon];

			// 击杀怪物数
			WarInfo.gzAllData[ERDT_KILLMONSTER] = itPerson->nBaseRecord[EDT_KillDragon] + itPerson->nBaseRecord[EDT_KillMonster];

			// 一场比赛的天梯分数
			WarInfo.nMatchTypeScore = itPerson->nGetMatchTypeScore;
			// 玩家类型
			WarInfo.nPlayerType = itPerson->bIsComputer ? (m_context.byRoomType == HallRoomType_Macth ? EWPT_VIRTUALPEOPLE : EWPT_COMPUTER) : EWPT_REALPEOPLE;
			// 玩家的天梯分数
			EMMatchType emMatchType = (EMMatchType)getMatchTypeID();
			// 比赛类型
			WarInfo.nMatchType = emMatchType;
			// 当前天梯分
			WarInfo.nPlayerMatchScore = itPerson->nPreMatchTypeScore + itPerson->nGetMatchTypeScore;
			// 以前的段位分
			WarInfo.nPreMatchTypeScore = itPerson->nPreMatchTypeScore;
			// 以前的段位等级
			WarInfo.nPreMatchTypeGrade = itPerson->nPreMatchTypeGrade;
			
			SMatchRankConfigSchemeInfo* pScheme = pSchemeInfo->getShemeInfoByTypeAndScore(emMatchType, WarInfo.nPlayerMatchScore);
			if (pScheme != NULL)
				sstrcpyn(WarInfo.szPlayerMatchName, pScheme->szDes, sizeof(WarInfo.szPlayerMatchName));

			// 获得的称号和mvp分数
			//map<PDBID, STWarEndGraded>::iterator itTitle = m_mapWarEndGraded.find(itPerson->uPDBID);
			//if (itTitle != m_mapWarEndGraded.end())
			{
				WarInfo.nTitle = itPerson->nTitle;
				WarInfo.nScore = itPerson->nComprehensiveScore;
			}

			// 召唤师技能1
			WarInfo.nTalentSkill1 = itPerson->nSummerID;
			WarInfo.nTalentSkill2 = 0;
			sstrcpyn(WarInfo.PlayerName, itPerson->szName, sizeof(WarInfo.PlayerName));

			PersonListWarInfo << WarInfo;
		}
	}

	PersonListHeadInfo << WarInfoList;
	PersonListHeadInfo.push_back(PersonListWarInfo.data(), PersonListWarInfo.size());
	broadCastDataToAllPlayer(SC_MSG_WAR_END_INFO, PersonListHeadInfo.data(), PersonListHeadInfo.size());
}

void CWar::recordDamageInfo(int& nAllDamage, int& nPhysicDamage, int& nMagicDamage, int& nTrueDamage, const event_entity_damage& damage)
{
	// 记录英雄的总的输出（谨记录对英雄的伤害）
	nAllDamage += damage.nDamageHP;

	// 记录英雄的总的物理伤害或魔法伤害
	if (damage.nDamageType == DAMAGE_TYPE_PHYSICS)
	{
		nPhysicDamage += damage.nDamageHP;
	}
	else if (damage.nDamageType == DAMAGE_TYPE_MAGIC)
	{
		nMagicDamage += damage.nDamageHP;
	}
	else if (damage.nDamageType == DAMAGE_TYPE_TURE_DAMAGE)
	{
		nTrueDamage += damage.nDamageHP;
	}
}

// 记录玩家与玩家的输出和伤害 
void CWar::onGetHeroDamage(const event_entity_damage& damage)
{
	if (UID_2_TYPE(damage.uidTarget) != TYPE_PLAYER_ROLE)
	{
		return;
	}

	SWPerson* pOperaterPerson = getPlayerInfo(damage.uidOperator);
	if (pOperaterPerson == NULL)
		return;

	SWPerson* pTargetPerson = getPlayerInfo(damage.uidTarget);
	if (pTargetPerson == NULL)
		return;

	// 记录目标承受的伤害
	if (pOperaterPerson->nCamp != pTargetPerson->nCamp)
	{
		//添加记录伤害值
		pTargetPerson->GetDamageVect[damage.uidOperator] += damage.nDamageHP;
		pTargetPerson->uLastAttackUID = damage.uidOperator;
		// 记录英雄承受的总的物理伤害和魔法伤害
		recordDamageInfo(pTargetPerson->nBaseRecord[EDT_Damage], pTargetPerson->nBearPhysic, pTargetPerson->nBearMagic, pTargetPerson->nBearTrue, damage);
	}

	// 记录操作者输出的伤害
	if (pTargetPerson->nCamp != pOperaterPerson->nCamp)
	{
		recordDamageInfo(pOperaterPerson->nBaseRecord[EDT_OutPut], pOperaterPerson->nOutHeroPhysics, pOperaterPerson->nOutHeroMagic, pOperaterPerson->nOutHeroTrue, damage);
		if (damage.nAttackResultType == AttackResult_Fatal)
		{
			// 计算最大暴击值
			if (pOperaterPerson->nMaxCritical < damage.nDamageHP)
				pOperaterPerson->nMaxCritical = damage.nDamageHP;
		}
	}
}

void CWar::onHaveCure(CureContext* pCureContext)
{
	// 只记录英雄治疗
	if (UID_2_TYPE(pCureContext->uidOperator) != TYPE_PLAYER_ROLE)
	{
		return;
	}

	SWPerson* pOperator = getPlayerInfo(pCureContext->uidOperator);
	if (pOperator == NULL)
		return;

	// 计算自身恢复的生命值
	if (pCureContext->uidOperator == pCureContext->uidTarget)
	{

		pOperator->nRecoveryHP += pCureContext->nCureHP;
		return;
	}


	// 计算给队友恢复的生命值
	SWPerson* pTarget = getPlayerInfo(pCureContext->uidTarget);
	if (pTarget == NULL)
		return;

	if (pTarget->nCamp == pOperator->nCamp)
		pOperator->nBaseRecord[EDT_Cure] += pCureContext->nCureHP;
}

void CWar::onWarRealStart()
{
	EmphasisLn("War real start");

	for (int i = 0; i < Mgr_Max; ++i)
	{
		__IWarMgr* pWarMgr = m_WarMgrList[i];
		if (pWarMgr != NULL)
		{
			pWarMgr->onWarRealStart();
		}
	}
}

void CWar::addPlayerData()
{

}

bool CWar::isHaveRealPlayer(int nCamp)
{
	WarCampMap::iterator itCamp = m_mapWarCamp.find(nCamp);
	if (itCamp != m_mapWarCamp.end())
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (!itPerson->bIsComputer)
				return true;
		}
	}

	return false;
}

void CWar::statisticsPlayerGraded()
{
	//SEDTPersonData  sMaxEDTGroup[EDT_MAX];              // 各项称号 评分数值最大值

	map<int,map<int, SEDTPersonData>> sMaxEDTGroup;              // 各项称号 评分数值最大值
	
	// 计算阵营团队的战绩信息
	SetCampTeamRecordInfo();

	// 计算连杀
	calcContKill();

	WarCampMap::iterator itBegin = m_mapWarCamp.begin();
	for (; itBegin != m_mapWarCamp.end(); ++itBegin)
	{
		SWarCamp* pWarCamp = &itBegin->second;
		WarPersonVector::iterator itPerBegin = pWarCamp->warPersVect.begin();
		for (; itPerBegin != pWarCamp->warPersVect.end(); ++itPerBegin)
		{
			// 设置杀人称号
			setKillTitle(*itPerBegin, itPerBegin->nTimeKill, itPerBegin->nDeadKill);
			// 保留综合分数最高的玩家的pdbid
			int nComprehenScore = getComprehensivePerformanceScore(*itPerBegin);
			itPerBegin->nComprehensiveScore = nComprehenScore;
			//m_mapWarEndGraded[itPerBegin->uPDBID] = stWarEndGraded;
			// 获得各个评分数据值的最大值
			itPerBegin->nBaseRecord[EDT_Score] = nComprehenScore;
			getEveryEDTMaxData(sMaxEDTGroup[itPerBegin->nCamp], &(*itPerBegin));
			getEveryEDTMaxData(sMaxEDTGroup[CAMP_MAX], &(*itPerBegin));
		}
	}

	// 计算失败方mvp
	int nFailedCamp = getEnemyCamp(getWinCamp());
	calcFailedCampMvp(nFailedCamp, sMaxEDTGroup[CAMP_MAX]);

	itBegin = m_mapWarCamp.begin();
	for (; itBegin != m_mapWarCamp.end(); ++itBegin)
	{
		SWarCamp* pWarCamp = &itBegin->second;
		WarPersonVector::iterator itPerBegin = pWarCamp->warPersVect.begin();

		for (; itPerBegin != pWarCamp->warPersVect.end(); ++itPerBegin)
		{
			// 获得唯一称号的得主的pdbid
			for (int i = 0; i < EDT_MAX; ++i)
			{
				int nTitleType = EWM_NONE + 1;

				if (!canAddPersonTitle(i, *itPerBegin, sMaxEDTGroup))
				{
					continue;
				}

				nTitleType <<= i;
				itPerBegin->nTitle = itPerBegin->nTitle | nTitleType;
			}
		}
	}
}

void CWar::getEveryEDTMaxData(map<int, SEDTPersonData> &pMaxEDTPDBID, SWPerson *pPerson)
{
	for (int i = 0; i < EDT_MAX; ++i)
	{
		if (EDT_Score == i)
		{
			if (pPerson->nBaseRecord[i] > pMaxEDTPDBID[i].nBaseRecord && pPerson->nCamp == getWinCamp())
			{
				pMaxEDTPDBID[i].dwActorID = pPerson->uPDBID;
				pMaxEDTPDBID[i].nBaseRecord = pPerson->nBaseRecord[i];
			}
		}
		else
		{
			if (pPerson->nBaseRecord[i] > pMaxEDTPDBID[i].nBaseRecord)
			{
				pMaxEDTPDBID[i].dwActorID = pPerson->uPDBID;
				pMaxEDTPDBID[i].nBaseRecord = pPerson->nBaseRecord[i];
			}
		}
	}
}

bool  CWar::canAddPersonTitle(int nEDTIndex, const SWPerson& swPerson, map<int, map<int, SEDTPersonData>> & pMaxEDTGroup)
{
	if (swPerson.nBaseRecord[nEDTIndex] <= 0)
	{
		return false;
	}

	EDataType eType = (EDataType)nEDTIndex;
	if (eType == EDT_JoinBattleRate)
	{
		// 每个阵营一个玩家
		if (swPerson.nBaseRecord[nEDTIndex] < pMaxEDTGroup[swPerson.nCamp][nEDTIndex].nBaseRecord)
		{
			return false;
		}
		if (swPerson.uPDBID != pMaxEDTGroup[swPerson.nCamp][nEDTIndex].dwActorID)
		{
			return false;
		}

		return true;
	}
	else
	{
		if (swPerson.nBaseRecord[nEDTIndex] < pMaxEDTGroup[CAMP_MAX][nEDTIndex].nBaseRecord)
		{
			return false;
		}

		bool bOneOnly = true;
		if (eType == EDT_DestoryTower || eType == EDT_TeamBattleControl)
		{
			bOneOnly = false;
		}

		if (bOneOnly && swPerson.uPDBID != pMaxEDTGroup[CAMP_MAX][nEDTIndex].dwActorID)
		{
			return false;
		}

		if (eType != EDT_Cure&&eType != EDT_ThrillSave&&eType != EDT_DeadlyControl&&eType != EDT_TeamBattleControl && eType != EDT_KillDragon)
		{
			return true;
		}
	}


	// 以下几个变量是需要脚本控制的量EDT_Cure 、EDT_ThrillSave 、EDT_DeadlyControl 、EDT_TeamBattleControl
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
		return false;
	}

	ISchemeWarEvaluate* pSchemeEvaluate = pSchemeCenter->getSchemeWarEvaluate();
	if (pSchemeEvaluate == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pSchemeEvaluate failed!");
		return false;
	}

	SWarEvaluathInfo* pEvaluathInfo = pSchemeEvaluate->getEvaluathInfo(m_context.dwMatchTypeID);
	if (pEvaluathInfo == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pEvaluathInfo  failed");
		return false;
	}

	bool bReachLimit = false;
	switch (eType)
	{
	case EDT_Cure:
		{
			map<int, SCampTeamRecordInfo>::iterator itTeamResult = m_mapCampTeamRecord.find(swPerson.nCamp);
			if (itTeamResult == m_mapCampTeamRecord.end())
			{
				break;
			}

			//int nCampBearDamage = max(itTeamResult->second.nCampBearDamage, 1);
			//bReachLimit = ((float)swPerson.nBaseRecord[EDT_Cure] / nCampBearDamage) > pEvaluathInfo->fCureRateHonorT;

			bReachLimit = swPerson.nBaseRecord[EDT_Cure] > pEvaluathInfo->fCureHonorT;
		}
		break;
	case EDT_ThrillSave:
		{
			bReachLimit = swPerson.nBaseRecord[EDT_ThrillSave] > pEvaluathInfo->nThrillSaveHonorT;
		}
		break;
	case EDT_DeadlyControl:
		{
			bReachLimit = swPerson.nBaseRecord[EDT_DeadlyControl] > pEvaluathInfo->nDeadlyControlHonorT;
		}
		break;
	case EDT_TeamBattleControl:
		{
			bool bControlReachFive = swPerson.nBaseRecord[EDT_TeamBattleControl] >= 5;

			float fTeamBattleControlScore = 0;
			if (!bControlReachFive)
			{
				int nTeamBattleControlThreshold = pEvaluathInfo->nTeamBattleControlT;
				TeamControlList::const_iterator iterControlBegin = swPerson.teamControlList.begin();
				for (; iterControlBegin != swPerson.teamControlList.end(); ++iterControlBegin)
				{
					int nTeamBattleControlCount = *iterControlBegin;
					if (nTeamBattleControlCount < nTeamBattleControlThreshold)
					{
						continue;
					}

					fTeamBattleControlScore += (nTeamBattleControlCount - nTeamBattleControlThreshold)*pEvaluathInfo->fTeamBattleControlR + pEvaluathInfo->fTeamBattleControlTScore;
				}
			}

			bReachLimit = bControlReachFive || fTeamBattleControlScore > pEvaluathInfo->fTeamBattleControlHonorT;
		}
		break;
	case EDT_KillDragon:
		{
			bReachLimit = swPerson.nBaseRecord[EDT_KillDragon] > pEvaluathInfo->nKillDragonHonorT;
		}
		break;
	default:
		break;
	}

	return bReachLimit;
}

// 获得综合表现分 总分计算改变 ： 击杀破塔得分+团队贡献得分+特殊贡献得分
int CWar::getComprehensivePerformanceScore(const SWPerson& swPerson)
{
	float fKillDestroyScore = getKillAndDestroyTowerScore(swPerson);
	float fTeamContributionScore = getTeamContributionScore(swPerson);
	float fSpecialContributionScore = getSpeicalContributionScore(swPerson);

	float fComprehensive = fKillDestroyScore + fTeamContributionScore + fSpecialContributionScore;

	//TraceLn(__FUNCTION__": Comprehensive score = " << (int)fComprehensive );
	return (int)fComprehensive;
}

// 获得综合得分之一 击杀破塔得分 : KDA+连杀+多杀+破塔+控龙
float CWar::getKillAndDestroyTowerScore(const SWPerson& swPerson)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
		return -1;
	}

	ISchemeWarEvaluate* pSchemeEvaluate = pSchemeCenter->getSchemeWarEvaluate();
	if (pSchemeEvaluate == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pSchemeEvaluate failed!");
		return -1;
	}

	SWarEvaluathInfo* pEvaluathInfo = pSchemeEvaluate->getEvaluathInfo(m_context.dwMatchTypeID);
	if (pEvaluathInfo == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pEvaluathInfo  failed");
		return -1;
	}

	map<int, SCampTeamRecordInfo>::iterator itTeamResult = m_mapCampTeamRecord.find(swPerson.nCamp);
	if (itTeamResult == m_mapCampTeamRecord.end())
	{
		ErrorLn(__FUNCTION__ << ": itTeamResult == m_mapCampTeamRecord.end(),nCamp=" << swPerson.nCamp);
		return 0;
	}

	// 方式一：计算kda得分  ＫＤＡ得分＝（人头+助攻）/max（死亡，1）*ＫＤＡ系数*ＫＤＡ加权系数，取值范围（0~配置最大值）
	//float fKdaScore = ((float)(swPerson.nBaseRecord[EDT_KillCount] + swPerson.nBaseRecord[EDT_AssistCount])/nDeadNum) *pEvaluathInfo->fKDAWR;
	// 方式二：Kda得分 = （人头 + 助攻 - 死亡）/ 阵营击杀总数 * kda系数 * kda加权系数
	int nCampKillNum = max(itTeamResult->second.nCampKillNum, 1);
	int nDeadNum = swPerson.nDeadNum;
	float fKdaScore = ((float)(swPerson.nBaseRecord[EDT_KillCount] + swPerson.nBaseRecord[EDT_AssistCount] - nDeadNum) / nCampKillNum) * pEvaluathInfo->fKDAWR * pEvaluathInfo->fKDAR;

	// 计算破塔得分＝（破塔数）*破塔系数*破塔加权系数，取值范围（大于等于0）
	float fDamageTower = (float)swPerson.nBaseRecord[EDT_DestoryTower] * pEvaluathInfo->fDestoryTowerR*pEvaluathInfo->fDestoryTowerWR;

	// 不死连杀总得分＝【求和【连杀得分（连杀数-连杀阀值）*连杀系数 】】* 连杀加权系数，取值范围（0~配置最大值）
	float fAliveMultiKillScore = 0;
	int nAliveMultiKillThreshold = pEvaluathInfo->nAliveMultiKillT;
	ContinuKillList::const_iterator iterDeadBegin = swPerson.deadContKillList.begin();
	for (; iterDeadBegin != swPerson.deadContKillList.end(); ++iterDeadBegin)
	{
		int nAliveMultiKillCount = *iterDeadBegin;
		if (nAliveMultiKillCount < nAliveMultiKillThreshold)
		{
			continue;
		}

		fAliveMultiKillScore += (float)(nAliveMultiKillCount - nAliveMultiKillThreshold)*pEvaluathInfo->fAliveMultiKillR + pEvaluathInfo->fAliveMultiKillTScore;
	}

	fAliveMultiKillScore *= pEvaluathInfo->fAliveMultiKillWR;

	// 多杀总得分＝【求和【多杀得分（多杀数-多杀阀值）*多杀系数 】】* 多杀加权系数，取值范围（0~配置最大值）
	float fTimeMultiKillScore = 0;
	int nTimeKillThreshold = pEvaluathInfo->nTimeMultiKillT;
	ContinuKillList::const_iterator iterTimeBegin = swPerson.timeContKillList.begin();
	for (; iterTimeBegin != swPerson.timeContKillList.end(); ++iterTimeBegin)
	{
		int nTimedMultiKillCount = *iterTimeBegin;
		if (nTimedMultiKillCount < nTimeKillThreshold)
		{
			continue;
		}

		fTimeMultiKillScore += (float)(nTimedMultiKillCount - nTimeKillThreshold)*pEvaluathInfo->fTimeMultiKillR + pEvaluathInfo->fTimeMultiKillTScore;
	}

	fTimeMultiKillScore *= pEvaluathInfo->fTimeMultiKillWR;

	// y^1得分
	// y^1 = （y1 – g函数平移系数）/（g函数缩放系数）
	float fKillDestroyScore = fKdaScore + fDamageTower + fAliveMultiKillScore + fTimeMultiKillScore;
	float fKillScoreFunctionParam1 = pEvaluathInfo->fKillScoreFunctionParam1;
	if (equalto0(fKillScoreFunctionParam1))
		fKillScoreFunctionParam1 = 1.0f;
	float fKillDestroyScoreEx = (fKillDestroyScore - pEvaluathInfo->fKillScoreFunctionParam2) / fKillScoreFunctionParam1;
	makesure_minmax(fKillDestroyScoreEx, pEvaluathInfo->fKillDestroyMin, pEvaluathInfo->fKillDestroyMax);
	fKillDestroyScoreEx *= pEvaluathInfo->fKillDestroyWR*pEvaluathInfo->nComprehensiveScoreEnlargeR;

	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml != NULL && pEntityXml->nIsShowPlanLog > 0)
	{
		TraceLn(__FUNCTION__": nKdaScore score = " << fKdaScore);
		TraceLn(__FUNCTION__": nDamageTower score = " << fDamageTower);
		TraceLn(__FUNCTION__": nAliveContKillScore score = " << fAliveMultiKillScore);
		TraceLn(__FUNCTION__": nTimeContKillScore score = " << fTimeMultiKillScore);
		TraceLn(__FUNCTION__": fKillDestroyScore score = " << fKillDestroyScore);
		TraceLn(__FUNCTION__": fKillDestroyScoreEx score = " << fKillDestroyScoreEx);
	}

	return fKillDestroyScoreEx;
}

// 获得综合得分之二 团队贡献得分
float CWar::getTeamContributionScore(const SWPerson& swPerson)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
		return -1;
	}

	ISchemeWarEvaluate* pSchemeEvaluate = pSchemeCenter->getSchemeWarEvaluate();
	if (pSchemeEvaluate == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pSchemeEvaluate failed!");
		return -1;
	}

	SWarEvaluathInfo* pEvaluathInfo = pSchemeEvaluate->getEvaluathInfo(m_context.dwMatchTypeID);
	if (pEvaluathInfo == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pEvaluathInfo  failed");
		return -1;
	}

	map<int, SCampTeamRecordInfo>::iterator itTeamResult = m_mapCampTeamRecord.find(swPerson.nCamp);
	if (itTeamResult == m_mapCampTeamRecord.end())
	{
		return 0;
	}

	int nCampOutput = max(itTeamResult->second.nCampOutput, 1);
	float fOutputRateInTeam = (float)swPerson.nBaseRecord[EDT_OutPut] / nCampOutput;

	int nCampMoney = max(itTeamResult->second.nCampMoney, 1);
	float fMoneyRateInTeam = (float)swPerson.nBaseRecord[EDT_Money] / nCampMoney;
	fMoneyRateInTeam = max(fMoneyRateInTeam, 1);

	int nCampKillNum = max(itTeamResult->second.nCampKillNum, 1);
	float fJoinBattleRateInTeam = ((float)(swPerson.nBaseRecord[EDT_KillCount] + swPerson.nBaseRecord[EDT_AssistCount])) / nCampKillNum;

	// 团队贡献分 = 输出金钱比加权基本系数 * 输出占比
	int nWinLoseRatio = swPerson.nCamp == getWinCamp() ? 1 : 0;
	//float fTeamContributionScore = (pEvaluathInfo->fOutputDamageWithMoneyBaseWR+nWinLoseRatio*pEvaluathInfo->fOutputDamageWithMoneyWinLoseWR)*(fOutputRateInTeam/fMoneyRateInTeam)+fJoinBattleRateInTeam*pEvaluathInfo->fKDAR;
	float fTeamContributionScore = pEvaluathInfo->fOutputDamageWithMoneyBaseWR * fOutputRateInTeam;
	makesure_minmax(fTeamContributionScore, pEvaluathInfo->fTeamContributionMin, pEvaluathInfo->fTeamContributionMax);
	fTeamContributionScore *= pEvaluathInfo->fTeamContributionWR*pEvaluathInfo->nComprehensiveScoreEnlargeR;

	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml != NULL && pEntityXml->nIsShowPlanLog)
	{
		TraceLn(__FUNCTION__": fOutputRateInTeam score = " << fOutputRateInTeam);
		TraceLn(__FUNCTION__": fMoneyRateInTeam score = " << fMoneyRateInTeam);
		TraceLn(__FUNCTION__": fJoinBattleRateInTeam score = " << fJoinBattleRateInTeam);
		TraceLn(__FUNCTION__": fTeamContributionScore score = " << fTeamContributionScore);
	}
	return fTeamContributionScore;
}

// 获得综合得分之三 特殊贡献得分
float CWar::getSpeicalContributionScore(const SWPerson& swPerson)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
		return -1;
	}

	ISchemeWarEvaluate* pSchemeEvaluate = pSchemeCenter->getSchemeWarEvaluate();
	if (pSchemeEvaluate == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pSchemeEvaluate failed!");
		return -1;
	}

	SWarEvaluathInfo* pEvaluathInfo = pSchemeEvaluate->getEvaluathInfo(m_context.dwMatchTypeID);
	if (pEvaluathInfo == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pEvaluathInfo  failed");
		return -1;
	}

	map<int, SCampTeamRecordInfo>::iterator itTeamResult = m_mapCampTeamRecord.find(swPerson.nCamp);
	if (itTeamResult == m_mapCampTeamRecord.end())
	{
		return 0;
	}

	int nCampBearDamage = max(itTeamResult->second.nCampBearDamage, 1);
	float fBearDamageRateInTeam = (float)pEvaluathInfo->fBearDamageWR*swPerson.nBaseRecord[EDT_Damage] / nCampBearDamage;

	float fCureRateInTeam = pEvaluathInfo->fCureWR*(swPerson.nBaseRecord[EDT_Cure] / nCampBearDamage);

	int nThrillSaveCount = swPerson.nBaseRecord[EDT_ThrillSave];
	int nThrillSaveADDDeadNum = max(nThrillSaveCount + itTeamResult->second.nCampDeadNum, 1);
	float fThrillSaveRateInTeam = (float)pEvaluathInfo->fThrillSaveWR*nThrillSaveCount / (nThrillSaveADDDeadNum);

	int nDeadlyControlCount = swPerson.nBaseRecord[EDT_DeadlyControl];
	int nCampKillNum = max(itTeamResult->second.nCampKillNum, 1);
	float fDeadlyControlRateInTeam = (float)pEvaluathInfo->fDeadlyControlWR*nDeadlyControlCount / (nCampKillNum);

	float fTeamBattleControlScore = 0;
	int nTeamBattleControlThreshold = pEvaluathInfo->nTeamBattleControlT;
	TeamControlList::const_iterator iterControlBegin = swPerson.teamControlList.begin();
	for (; iterControlBegin != swPerson.teamControlList.end(); ++iterControlBegin)
	{
		int nTeamBattleControlCount = *iterControlBegin;
		if (nTeamBattleControlCount < nTeamBattleControlThreshold)
		{
			continue;
		}

		fTeamBattleControlScore += (float)(nTeamBattleControlCount - nTeamBattleControlThreshold)*pEvaluathInfo->fTeamBattleControlR + pEvaluathInfo->fTeamBattleControlTScore;
	}

	fTeamBattleControlScore *= pEvaluathInfo->fTeamBattleControlWR;

	// 特殊贡献得分
	float fSpeicalContributionScore = fBearDamageRateInTeam + fCureRateInTeam + fThrillSaveRateInTeam + fDeadlyControlRateInTeam + fTeamBattleControlScore;
	makesure_minmax(fSpeicalContributionScore, pEvaluathInfo->fSpecialContributionMin, pEvaluathInfo->fSpecialContributionMax);
	fSpeicalContributionScore *= pEvaluathInfo->fSpecialContributionWR*pEvaluathInfo->nComprehensiveScoreEnlargeR;

	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml != NULL && pEntityXml->nIsShowPlanLog)
	{
		TraceLn(__FUNCTION__": fBearDamageRateInTeam score = " << fBearDamageRateInTeam);
		TraceLn(__FUNCTION__": fCureRateInTeam score = " << fCureRateInTeam);
		TraceLn(__FUNCTION__": fThrillSaveRateInTeam score = " << fThrillSaveRateInTeam);
		TraceLn(__FUNCTION__": fDeadlyControlRateInTeam score = " << fDeadlyControlRateInTeam);
		TraceLn(__FUNCTION__": fTeamBattleControlScore score = " << fTeamBattleControlScore);
		TraceLn(__FUNCTION__": fSpeicalContributionScore score = " << fSpeicalContributionScore);
	}

	return fSpeicalContributionScore;
}


void CWar::setKillTitle(SWPerson& person, int nTimeKill, int nDeadKill)
{
	// 设置时间内连杀称号
	if (nTimeKill >= ETKT_TYPE_3 && nTimeKill < ETKT_TYPE_MAX)
	{
		person.nKillTimeTitle = (ETimeKillType)nTimeKill;
	}
	else if (nTimeKill >= ETKT_TYPE_MAX)
	{
		person.nKillTimeTitle = ETKT_TYPE_MAX - 1;
	}


	// 设置在不死情况下连杀称号
	if (nDeadKill >= EKNT_TYPE_3 && nDeadKill < EKNT_TYPE_MAX)
	{
		person.nKillDeadTitle = (EDeadKillNumTitile)nDeadKill;
	}
	else if (nDeadKill >= EKNT_TYPE_MAX)
	{
		person.nKillDeadTitle = EKNT_TYPE_MAX - 1;
	}
}

void CWar::SetCampTeamRecordInfo()
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
		return;
	}

	ISchemeWarEvaluate* pSchemeEvaluate = pSchemeCenter->getSchemeWarEvaluate();
	if (pSchemeEvaluate == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pSchemeEvaluate failed!");
		return;
	}
	SWarEvaluathInfo* pEvaluathInfo = pSchemeEvaluate->getEvaluathInfo(m_context.dwMatchTypeID);
	if (pEvaluathInfo == NULL)
	{
		ErrorLn(__FUNCTION__ << ": get pEvaluathInfo  failed");
		return;
	}

	WarCampMap::iterator itBegin = m_mapWarCamp.begin();

	for (; itBegin != m_mapWarCamp.end(); ++itBegin)
	{
		SWarCamp sWarCamp = itBegin->second;
		if (sWarCamp.warPersVect.empty())
		{
			continue;
		}

		SCampTeamRecordInfo stTeamInfo;
		WarPersonVector::iterator itPerBegin = sWarCamp.warPersVect.begin();
		for (; itPerBegin != sWarCamp.warPersVect.end(); ++itPerBegin)
		{
			stTeamInfo.nCampOutput += (int)(itPerBegin->nBaseRecord[EDT_OutPut]);
			stTeamInfo.nCampBearDamage += (int)(itPerBegin->nBaseRecord[EDT_Damage]);
			stTeamInfo.nCampMoney += (int)(itPerBegin->nBaseRecord[EDT_Money]);
			stTeamInfo.nCampKillNum += (int)(itPerBegin->nBaseRecord[EDT_KillCount]);
			stTeamInfo.nCampDeadNum += itPerBegin->nDeadNum;
		}

		m_mapCampTeamRecord[itBegin->first] = stTeamInfo;
	}
}

// 设置杯赛比赛结果
void CWar::setLegendCupWarResult()
{
	int nWinCamp = (getWinCamp() == CAMP_BLUE) ? CAMP_BLUE : CAMP_RED;
	int nFailedCamp = getEnemyCamp(nWinCamp);
	int nWinCampGlory[emLegendGlory_Max] = { 0 }, nFailCampGlory[emLegendGlory_Max] = { 0 };
	setCampLegendCupGlory(nWinCampGlory, emLegendGlory_Max, nWinCamp, nWinCamp);
	setCampLegendCupGlory(nFailCampGlory, emLegendGlory_Max, nFailedCamp, nWinCamp);
	SMsgSetWarResultData resultData;
	resultData.nWarID = m_context.nWarID;
	resultData.llDBWarID = getWarDBKey();
	resultData.nFailedCamp = nFailedCamp;
	resultData.nFailedKDA = getCampAvgKDAScore(nFailedCamp);
	resultData.nWinKDA = getCampAvgKDAScore(nWinCamp);
	memcpy(resultData.nWinGlory, nWinCampGlory, sizeof(resultData.nWinGlory));
	memcpy(resultData.nFailGlory, nFailCampGlory, sizeof(resultData.nFailGlory));
	resultData.nGloryLen = emLegendGlory_Max;
	resultData.dwBeginTime = m_WarInfo.BeginTime;
	MatchSceneHelper().m_ptr->setWarResult(resultData);
}

int CWar::getCampAvgKDAScore(int nPlayerCamp)
{
	int nSumScore = 0;
	int nPersonNum = 0;

	SWarCamp* pWarCamp = getWarCamp(nPlayerCamp);
	if (pWarCamp == NULL)
		return 0;

	for (int i = 0; i < pWarCamp->warPersVect.size(); ++i)
	{

		nSumScore += (pWarCamp->warPersVect[i].nComprehensiveScore);
		nPersonNum++;
	}

	if (nPersonNum == 0)
		return 0;

	return nSumScore / nPersonNum;
}

void CWar::setCampLegendCupGlory(int nGlory[], int nGloryLen, int nCamp, int nWinCamp)
{
	WarCampMap::iterator it = m_mapWarCamp.find(nCamp);
	if (it == m_mapWarCamp.end())
	{
		return;
	}
	SWarCamp* sWarCamp = &it->second;

	if (nCamp == nWinCamp)
		nGlory[emLegendGlory_WinCount] += 1;
	else
		nGlory[emLegendGlory_LoseCount] += 1;

	WarPersonVector::iterator itPerson = sWarCamp->warPersVect.begin();
	for (; itPerson != sWarCamp->warPersVect.end(); ++itPerson)
	{
		nGlory[emLegendGlory_KillCount] += itPerson->nBaseRecord[EDT_KillCount];
		nGlory[emLegendGlory_AssistCount] += itPerson->nBaseRecord[EDT_AssistCount];
		nGlory[emLegendGlory_DeathCount] += itPerson->nDeadNum;
		nGlory[emLegendGlory_GodCount] += getSuperGodCount(itPerson->uPDBID);

		ContinuKillList::iterator iter = itPerson->timeContKillList.begin();
		for (; iter != itPerson->timeContKillList.end(); ++iter)
		{
			int nTimeContKill = *iter;
			if (nTimeContKill >= 5)
			{
				nGlory[emLegendGlory_5KillCount] += 1;
			}
			if (nTimeContKill >= 4)
			{
				nGlory[emLegendGlory_4KillCount] += 1;
			}
			if (nTimeContKill >= 3)
			{
				nGlory[emLegendGlory_3KillCount] += 1;
			}
		}
	}
}

void CWar::updatePlayerMatchTypeInfo()
{
	// 获得事件中心接口
	SceneLogicEventHelper eventHelper(getWarSceneID());
	IEventEngine *pEventEngine = eventHelper.m_ptr;
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;

	ISchemePersonModel* pPersonModel = pSchemeCenter->getSchemePersonModel();
	if (pPersonModel == NULL)
		return;

	int nMatchTypeID = getMatchTypeID();
	for (map<PDBID, UID>::iterator it = m_mapRealPlayerUid.begin(); it != m_mapRealPlayerUid.end(); ++it)
	{
		const PDBID & pdbid = it->first;
		// 排除掉电脑
		SWPerson * pPerson = getPlayerInfoByPDBIU(pdbid);
		if (!pPerson || pPerson->bIsComputer)
			continue;

		int nCamp = pPerson->nCamp;
		ActorHelper actorHelper(it->second);
		IActorService * pActor = actorHelper.m_ptr;
		if (!pActor)
		{
			continue;
		}

		// 更新地图信息
		pActor->updateMapInfo(nMatchTypeID, (nCamp == getWinCamp()));

		// 玩家任务数据
		if (pEventEngine)
		{
			event_game_war_person_task eventData;
			eventData.dwPDBID = pdbid;
			getCampPdbidString(pPerson->nCamp, eventData.szTeamMemberID, sizeof(eventData.szTeamMemberID));
			getWarResultString(pPerson, eventData.szWarResult, sizeof(eventData.szWarResult), pPersonModel);

			UID uid = pPerson->uID;
			pEventEngine->FireExecute(EVENT_GAME_WAR_PERSON_TASK, UID_2_TYPE(uid), uid, (LPCSTR)&eventData, sizeof(eventData));
		}
	}
}

bool CWar::isHaveSameClanMember(PDBID idActor)
{
	ActorHelper actorHelper(idActor, ACTOR_DBID);
	IActorService* pActor = actorHelper.m_ptr;
	if (pActor == NULL)
		return false;

	int nCamp = pActor->getProperty_Integer(PROPERTY_CAMP);
	int nSelfClanID = pActor->getProperty_Integer(PROPERTY_CLAN);

	WarCampMap::iterator it = m_mapWarCamp.find(nCamp);
	if (it != m_mapWarCamp.end())
	{
		SWarCamp sWarCamp = it->second;
		WarPersonVector::iterator itBegin = sWarCamp.warPersVect.begin();
		for (; itBegin != sWarCamp.warPersVect.end(); ++itBegin)
		{
			ActorHelper actorOther(itBegin->uPDBID, ACTOR_DBID);
			IActorService* pActorOther = actorOther.m_ptr;
			if (pActorOther == NULL) continue;
			int nClanID = pActorOther->getProperty_Integer(PROPERTY_CLAN);
			if (nSelfClanID == nClanID && nClanID > 0 && idActor != itBegin->uPDBID)
			{
				return true;
			}
		}
	}

	return false;
}

// 判断同盟中是否有师徒关系  false 代表没有， true为有
bool CWar::isHaveMentorship(PDBID idActor, BYTE& nMentorshipPost)
{
	ActorHelper actorHelper(idActor, ACTOR_DBID);
	IActorService* pActor = actorHelper.m_ptr;
	if (pActor == NULL)
		return false;

	MentorshipHelper mentorshipHelper;
	IMentorshipSceneService *pMentorshipService = mentorshipHelper.m_ptr;
	if (pMentorshipService == NULL)
	{
		return false;
	}

	bool bRet = false;
	int nCamp = pActor->getProperty_Integer(PROPERTY_CAMP);

	WarCampMap::iterator it = m_mapWarCamp.find(nCamp);
	if (it != m_mapWarCamp.end())
	{
		SWarCamp sWarCamp = it->second;
		WarPersonVector::iterator itBegin = sWarCamp.warPersVect.begin();
		for (; itBegin != sWarCamp.warPersVect.end(); ++itBegin)
		{
			ActorHelper actorOther(itBegin->uPDBID, ACTOR_DBID);
			IActorService* pActorOther = actorOther.m_ptr;
			if (pActorOther == NULL) continue;
			if (idActor != itBegin->uPDBID)
			{
				nMentorshipPost = pMentorshipService->getMyMentorshipPost(idActor, itBegin->uPDBID);
				if (nMentorshipPost != emMentorshipPost_Normal)
					bRet = true;

				if (nMentorshipPost == emMentorshipPost_Master)
					return true;
			}
		}
	}

	return bRet;
}

// 判断本场是否获得MVP
bool CWar::isGetMVP(PDBID idActor)
{
	SWPerson* pPerson = getPlayerInfoByPDBIU(idActor);
	if (pPerson == NULL)
		return false;

	return (pPerson->nTitle & EWM_SCORE);
}

// 获取多杀最高次数
int	CWar::getTimeKillMaxCount(PDBID idActor)
{
	SWPerson* pPerson = getPlayerInfoByPDBIU(idActor);
	if (pPerson == NULL)
		return 0;

	int nTimeContKill = 0;
	ContinuKillList::iterator iter = pPerson->timeContKillList.begin();
	for (; iter != pPerson->timeContKillList.end(); ++iter)
	{
		nTimeContKill = max(nTimeContKill, *iter);
	}

	return nTimeContKill;
}

int CWar::getTimeKillCountOfN(PDBID idActor, int n)
{
	SWPerson* pPerson = getPlayerInfoByPDBIU(idActor);
	if (pPerson == NULL)
		return 0;

	int nNum = 0;
	for (auto iter = pPerson->timeContKillList.begin(); iter != pPerson->timeContKillList.end(); ++iter)
	{
		if (*iter >= n)
		{
			nNum++;
		}
	}

	return nNum;
}

int CWar::getSuperGodCount(PDBID idActor)
{
	SWPerson* pPerson = getPlayerInfoByPDBIU(idActor);
	if (pPerson == NULL)
		return 0;

	int nNum = 0;
	for (auto iter = pPerson->deadContKillList.begin(); iter != pPerson->deadContKillList.end(); ++iter)
	{
		if (*iter >= EKNT_TYPE_8)
		{
			nNum++;
		}
	}

	return nNum;
}

void CWar::sendMiniMapFightInfo(event_entity_damage damage)
{
	if (UID_2_TYPE(damage.uidTarget) != TYPE_PLAYER_ROLE || UID_2_TYPE(damage.uidOperator) != TYPE_PLAYER_ROLE)
	{
		return;
	}

	//被攻击了判定发战斗小地图信息
	ActorHelper actorHelper(damage.uidTarget);
	IActorService *pTarget = actorHelper.m_ptr;

	if (pTarget == NULL)
	{
		return;
	}

	map<UID, DWORD>::iterator iter = m_mapUidMiniMapInfoRecord.find(damage.uidTarget);
	DWORD curTime = gServerGlobal->getTimeSyncService()->GetTick();

	const SchemeWarPersonConfigInfo *pWarPersonConfig = getPersonConfigInfo();
	if (!pWarPersonConfig)
	{
		ErrorLn(__FUNCTION__":pWarPersonConfig == NULL")
			return;
	}

	if (iter == m_mapUidMiniMapInfoRecord.end() || iter->second + pWarPersonConfig->nPersonFightIntervalTime * 1000 > curTime)
	{
		return;
	}

	m_mapUidMiniMapInfoRecord[damage.uidTarget] = curTime;

	// 进攻
	msg_entity_fight_mini_map_info msgFightInfo;
	msgFightInfo.dwSrcID = damage.uidTarget;
	msgFightInfo.nInfoType = 4;
	obuf oFightInfo;
	oFightInfo << msgFightInfo;
	broadCastDataToAllPlayer(SC_MSG_WAR_ENTITY_FIGHT_MINI_MAP_INFO, oFightInfo.data(), oFightInfo.size());
}

void CWar::updateWarEndReason()
{
	switch (getWinCamp())
	{

	case (int)CAMP_WIN_BLUE:// 阵营1
		{
			m_WarInfo.EndWarReason = CAMP_WIN_BLUE;
		}
		break;
	case (int)CAMP_WIN_RED:// 阵营2
		{
			m_WarInfo.EndWarReason = CAMP_WIN_RED;
		}
		break;
	default:break;
	}
}



int CWar::getCampPlayerCount(int nCamp, bool bInlucdeComputer)
{
	WarCampMap::iterator iter = m_mapWarCamp.find(nCamp);
	if (iter == m_mapWarCamp.end())
		return 0;

	int nCount = 0;
	if (bInlucdeComputer)
		nCount = iter->second.nAIPlayerCount + iter->second.nRealPlayerCount;
	else
		nCount = iter->second.nRealPlayerCount;

	return nCount;
}

// 获取某个阵营的平均经济
int CWar::getAvgEmcByCamp(int nCamp, bool bInlucdeComputer)
{
	int nPersonNum = getCampPlayerCount(nCamp, bInlucdeComputer);
	int nTotalEmc = getTotalEmcByCamp(nCamp, bInlucdeComputer);

	if (nPersonNum == 0 || nTotalEmc == 0)
	{
		return 0;
	}

	return nTotalEmc / nPersonNum;
}

// 获取某个阵营的最高经济
int CWar::getEcmMaxByCamp(int nCamp)
{
	WarCampMap::iterator campIter = m_mapWarCamp.find(nCamp);
	if (campIter == m_mapWarCamp.end())
	{
		return 0;
	}

	int nMaxEcm = 0;
	WarPersonVector::iterator iterPerson = campIter->second.warPersVect.begin();
	for (; iterPerson != campIter->second.warPersVect.end(); ++iterPerson)
	{
		if (iterPerson->nBaseRecord[EDT_Money] > nMaxEcm)
		{
			nMaxEcm = iterPerson->nBaseRecord[EDT_Money];
		}
	}

	return nMaxEcm;
}

SCampTeamRecordInfo* CWar::getCampRecordInfo(int nCamp)
{
	map<int, SCampTeamRecordInfo>::iterator itTeamResult = m_mapCampTeamRecord.find(nCamp);
	if (itTeamResult == m_mapCampTeamRecord.end())
		return NULL;

	return &itTeamResult->second;
}

void CWar::onSetGoodList(event_entity_war_good_list *pEvent)
{
	WarCampMap::iterator it = m_mapWarCamp.find(pEvent->nCamp);
	if (it == m_mapWarCamp.end())
	{
		return;
	}

	WarPersonVector::iterator itBegin = it->second.warPersVect.begin();
	for (; itBegin != it->second.warPersVect.end(); ++itBegin)
	{
		if (itBegin->uPDBID != pEvent->ActorID)
		{
			continue;
		}

		for (int i = 0; i < MAX_GOODS_SLOT_COUNT; ++i)
		{
			itBegin->nGoodList[i] = pEvent->nGoodList[i];
		}
		return;
	}
}

void CWar::onClearDeadContKill(event_entity_clear_multi_kill *pEvent)
{
	WarCampMap::iterator it = m_mapWarCamp.begin();
	for (; it != m_mapWarCamp.end(); ++it)
	{
		WarPersonVector::iterator itBegin = it->second.warPersVect.begin();
		for (; itBegin != it->second.warPersVect.end(); ++itBegin)
		{
			if (itBegin->uID != pEvent->uidOperator)
			{
				continue;;
			}

			// 清空连杀
			setKillMaxInfo(&(*itBegin));

			itBegin->nDeadContKill = 0;
			itBegin->nAliveContAssist = 0;
			break;

		}
	}
}

// 玩家特殊贡献信息同步(惊现救治 夺命控制 团控)
void CWar::BroadcastSpecialContribution(ESpecialContributionType eType, const SWPerson* pSWPerson)
{
	switch (eType)
	{
	case ESCT_THRILLSAVE:
	case ESCT_DEADLYCONTROL:
		{
			SNetMsgHead head;
			head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
			head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
			head.byKeyModule = MSG_MODULEID_WAR;
			head.byKeyAction = SC_MSG_WAR_SPECIALCONTRIBUTION;

			obuf obuffData;
			obuffData << head;

			msg_war_special_contribution msg;
			msg.eType = eType;

			obuffData.push_back(&msg, sizeof(msg));
			send2Client(pSWPerson->uID, PACKAGE_PTR(new std::string(obuffData.data(), obuffData.size())));
		}
		break;
	case ESCT_TEAMBATTLECONTROL:
		{
			int nTeamBattleControlCount = pSWPerson->teamControlList.back();
			if (nTeamBattleControlCount < 3)
			{
				break;
			}

			msg_war_special_contribution msg;
			msg.eType = eType;
			msg.nOperatorHeroID = pSWPerson->nHeroID;
			msg.nParam1 = nTeamBattleControlCount;
			sstrcpyn(msg.szOperatorName, pSWPerson->szName, sizeof(msg.szOperatorName));
			broadCastDataToSameCamp(SC_MSG_WAR_SPECIALCONTRIBUTION, (LPCSTR)&msg, sizeof(msg_war_special_contribution), pSWPerson->nCamp);
		}
		break;
	default:
		break;
	}

	return;
}

void CWar::onPlayerContinuePlay(event_entity_continue_play *pEvent)
{
	// 发送小地图信息
	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr != NULL)
	{
		pWarMonsterMgr->onActorContinuePlay(pEvent->clientID);
	}

	SWPerson* pPerson = getPlayerInfoByPDBIU(pEvent->dwDBID);
	if (pPerson == NULL)
		return;

	// 发送已激活天赋信息
	msg_war_active_talent_list data;
	data.pdbid = pPerson->uPDBID;
	data.nCurTalent = 0;
	memcpy(data.nTalentList, pPerson->TalentList, sizeof(data.nTalentList));
	sendDataToClient(pPerson->uID, SC_MSG_WAR_ACTIVE_TALENT_LIST, &data, sizeof(data));
}

bool CWar::onWarClientMsg(UID uidActor, BYTE byKeyAction, const char* pData, size_t stLen)
{
	switch (byKeyAction)
	{
	case CS_MSG_WAR_MINI_MAP_INFO:	        // 客户端地图消息
		{
			onClientMsgMiniMapInfo((void*)pData, stLen);
		}
		break;
	case CS_MSG_WAR_REQ_WAR_BASE_INFO:
		{
			// 客户端请求战场信息
			onClientReqWarBaseInfo(uidActor, (void*)pData, stLen);
		}
		break;
	case CS_MSG_WAR_PLAYER_TABLE_REQUEST:   // 客户端tab请求
		{
			onClientMsgUpdateTableRequest(uidActor, (void*)pData, stLen);
		}
		break;
	case CS_MSG_WAR_ADD_LABEL:   // 客户端为其他玩家添加标签
		{
			ISupportMgr* pSupportMgr = (ISupportMgr*)getWarMgr(Mgr_Support);
			if (pSupportMgr != NULL)
			{
				pSupportMgr->onClientMsgPersonAddLabelRequest(uidActor, (void*)pData, stLen);
			}
		}
		break;
	case CS_MSG_WAR_PICK_UP_ITEM:
		{
			if (pData == NULL)
			{
				return false;
			}
			msg_pick_up_item* pMsg = (msg_pick_up_item*)pData;

			// 判断距离
			Vector3 locActor = getLocation(uidActor);
			Vector3 locItem = getLocation(pMsg->itemID);
			if (!locActor.checkDistance(locItem, 5))
			{
				TraceLn(__FUNCTION__ << ", pick up item failed, distance > 5,  item(" << locItem.x << "," << locItem.y << "," << locItem.z << "), actor(" << locActor.x << "," << locActor.y << "," << locActor.z << ")");
				return false;
			}

			// 消息头
			SNetMsgHead head;
			head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
			head.byDestEndPoint = MSG_ENDPOINT_SCENE;
			head.byKeyModule = MSG_MODULEID_ENTITY;
			head.byKeyAction = SS_MSG_AI_PICKED_UP;

			// 实体类统一用子头来用于转向部件
			SMsgEntityHead entityHead;
			entityHead.uidMaster = pMsg->itemID;
			entityHead.byPartID = PART_AI;

			obuf256 data;
			data << head << entityHead << uidActor;

			handleMessage(pMsg->itemID, data.data(), data.size(), MSG_FLAG_NO_BLOCK);
		}
		break;
	case CS_MSG_WAR_BOTTING_CANCEL:
	case CS_MSG_WAR_BOTTING_STATE:
		{
			ICheckBadBehaviorMgr* pCheckBadBhvMgr = (ICheckBadBehaviorMgr*)getWarMgr(Mgr_CheckBadBehavior);
			if (pCheckBadBhvMgr != NULL)
			{
				pCheckBadBhvMgr->onMessage(byKeyAction, uidActor, (void*)pData, stLen);
			}
		}
		break;
	default:
		{
			WarningLn(__FUNCTION__": Unkonwn byKeyAction=" << byKeyAction);
			return false;
		}
	}

	return true;
}

void CWar::onClientMsgMiniMapInfo(void* pData, size_t stLen)
{
	if (stLen < sizeof(msg_war_entity_minimap_info))
	{
		ErrorLn(__FUNCTION__": failed! message length is too short! stLen=" << stLen);
		return;
	}
	msg_war_entity_minimap_info * pPlayerMiniMapInfo = (msg_war_entity_minimap_info *)pData;

	ActorHelper actorHelper(pPlayerMiniMapInfo->uPlayerUID);
	IActorService *pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
	{
		return;
	}

	obuf oEntityMiniMapInfo;
	oEntityMiniMapInfo << *pPlayerMiniMapInfo;
	broadCastDataToAllPlayer(SC_MSG_WAR_MINI_MAP_INFO, oEntityMiniMapInfo.data(), oEntityMiniMapInfo.size(), pActorService->getProperty_Integer(PROPERTY_CAMP));

	event_player_signal_recvd event;
	event.uidOperator = pPlayerMiniMapInfo->uPlayerUID;
	event.uidMarkTarget = pPlayerMiniMapInfo->nEntityID;
	event.ptSignLoc.x = pPlayerMiniMapInfo->vec[0];
	event.ptSignLoc.y = pPlayerMiniMapInfo->vec[1];
	event.ptSignLoc.z = pPlayerMiniMapInfo->vec[2];
	event.eSignal = (GameLogic::EMinimapClickType)pPlayerMiniMapInfo->nType;
	SceneLogicEventHelper helper(getWarSceneID());
	if (helper.m_ptr)
	{
		helper.m_ptr->FireExecute(EVENT_PLAYER_SIGNAL_RECVD, SOURCE_TYPE_PERSON, pPlayerMiniMapInfo->uPlayerUID, (LPCSTR)&event, sizeof(event));
	}
}

void CWar::onClientMsgUpdateTableRequest(UID uId, void* pData, size_t stLen)
{
	if (stLen < sizeof(msg_war_entity_update_table_request))
	{
		ErrorLn(__FUNCTION__": failed! message length is too short! stLen=" << stLen);
		return;
	}
	msg_war_entity_update_table_request * pTableRequestInfo = (msg_war_entity_update_table_request *)pData;

	ActorHelper actorHelper(pTableRequestInfo->uPlayerUID);
	IActorService *pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
	{
		return;
	}

    if (pActorService->getProperty_Integer(PROPERTY_OBSERVER) > 0)
    {
        return;
    }

	PDBID pDBID = pActorService->getProperty_Integer(PROPERTY_ID);

	sTabRecordInfo& recodInfo = m_mapTableRequest[pDBID];
	recodInfo.bOpen = pTableRequestInfo->bIsOpen;
	if (recodInfo.bOpen)
	{
		// 检查是否请求太频繁
		DWORD curTick = gServerGlobal->getTimeSyncService()->GetTick();
		if (curTick - recodInfo.dwOpenTicks > MAX_TAB_REQUEST_TICK_INTERVAL)
		{
			recodInfo.dwOpenTicks = curTick;
			// 立即更新所有人的table信息
			sendAllTabUpdateInfoToClient(pTableRequestInfo->uPlayerUID);
		}
	}
}

void CWar::onAddAssistBuff(UID uid, int nBuffCount, LPCSTR pData)
{
	SWPerson* pPerson = getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	// 保存玩家的助攻buff的时间
	SActor_Add_Buff_Node* pNode = (SActor_Add_Buff_Node*)pData;
	for (int i = 0; i < nBuffCount; ++i)
	{
		pPerson->AssistBuffVect[pNode->uidAdd] = pNode->dwUpdateTick;

		pNode++;
	}
}

void CWar::onActiveTalent(PDBID pdbid, int nTalentID, int nLevel)
{
	// 根据等级获取当前激活的天赋数组的下标
	static int TalentActiveLevel[MAX_ACTIVE_TALENT_COUNT] = { 3, 6 , 9, 12, 15, 18 };
	int nIndex = 0;
	for (int i = 0; i < MAX_ACTIVE_TALENT_COUNT; ++i)
	{
		if (TalentActiveLevel[i] == nLevel)
		{
			nIndex = i;
			break;
		}
	}

	if (nIndex < 0 || nIndex >= MAX_ACTIVE_TALENT_COUNT)
		return;

	SWPerson* pPerson = getPlayerInfoByPDBIU(pdbid);
	if (pPerson == NULL)
		return;

	pPerson->TalentList[nIndex] = nTalentID;

	// 更新到所有的客户端
	msg_war_active_talent_list data;
	data.pdbid = pdbid;
	data.nCurTalent = nTalentID;
	memcpy(data.nTalentList, pPerson->TalentList, sizeof(data.nTalentList));
	broadCastDataToAllPlayer(SC_MSG_WAR_ACTIVE_TALENT_LIST, (LPCSTR)&data, sizeof(data));
}

void CWar::updateWarEcmExpSituation()
{
	ISchemeMatchCoefficient* pMatchCoeff = gServerGlobal->getSchemeCenter()->getSchemeMatchCoeff();
	if (pMatchCoeff == NULL)
		return;

	SMatchCoefficient* pCoeff = pMatchCoeff->getMatchCoeff(getMatchTypeID());
	if (pCoeff == NULL)
	{
		ErrorLn(__FUNCTION__": pCoeff == NULL nMatchType = " << getMatchTypeID());
		return;
	}

	const int nCampCount = 2;
	int nCampEcmList[nCampCount] = { 0 };
	int nCampExpList[nCampCount] = { 0 };
	int nCampListIndex = 0;
	WarCampMap::iterator iterCamp = m_mapWarCamp.begin();
	for (; iterCamp != m_mapWarCamp.end(); ++iterCamp)
	{
		if (nCampListIndex >= nCampCount)
			continue;

		SWarCamp* sWarCamp = &iterCamp->second;
		WarPersonVector::iterator itPer = sWarCamp->warPersVect.begin();
		for (; itPer != sWarCamp->warPersVect.end(); ++itPer)
		{
			nCampEcmList[nCampListIndex] += itPer->nBaseRecord[EDT_Money];
			nCampExpList[nCampListIndex] += itPer->nExperien;
		}

		nCampListIndex++;
	}

	int nEcmOffset = nCampEcmList[0] - nCampEcmList[1];
	int nExpOffset = nCampExpList[0] - nCampExpList[1];

	int nUpdateIndex = m_EcmExpSituation.ecmExpSitnList.size();
	float fVariable = (float)nUpdateIndex / 2;

	//int nFlagList = {1,0,-1};
	int nEcmFlag = -2;
	float fEcmCompare = pCoeff->nAG*(fVariable*fVariable) + pCoeff->nBG*(fVariable)+pCoeff->nCG;
	if (nEcmOffset > fEcmCompare)
		nEcmFlag = WAR_ECM_EXP_ADVANTAGE;
	else if ((-fEcmCompare) < nEcmOffset&&nEcmOffset < fEcmCompare)
		nEcmFlag = WAR_ECM_EXP_STALEMATE;
	else if (nEcmOffset < (-fEcmCompare))
		nEcmFlag = WAR_ECM_EXP_INFERIORITY;
	else
		ErrorLn(__FUNCTION__": unbelievable ecmSituation  please check EcmOffset:" << nEcmOffset << "fEcmCompare:" << fEcmCompare);

	int nExpFlag = -2;
	float fExpCompare = pCoeff->nAE*(fVariable*fVariable) + pCoeff->nBE*(fVariable)+pCoeff->nCE;
	if (nExpOffset > fExpCompare)
		nExpFlag = WAR_ECM_EXP_ADVANTAGE;
	else if ((-fExpCompare) < nExpOffset&&nExpOffset < fExpCompare)
		nExpFlag = WAR_ECM_EXP_STALEMATE;
	else if (nExpOffset < (-fExpCompare))
		nExpFlag = WAR_ECM_EXP_INFERIORITY;
	else
		ErrorLn(__FUNCTION__": unbelievable ExpSituation  please check ExpOffset:" << nExpOffset << "fExpCompare:" << fExpCompare);

	int nEcmExpFlag = -2;
	int nEcmExpFlagValue = nEcmFlag + nExpFlag;
	if (nEcmExpFlagValue > 0)
		nEcmExpFlag = WAR_ECM_EXP_ADVANTAGE;
	else if (nEcmExpFlagValue == 0)
		nEcmExpFlag = WAR_ECM_EXP_STALEMATE;
	else if (nEcmExpFlagValue < 0)
		nEcmExpFlag = WAR_ECM_EXP_INFERIORITY;

	m_EcmExpSituation.nEcmExpSitn = nEcmExpFlag;
	m_EcmExpSituation.ecmExpSitnList.push_back(nEcmExpFlag);
}

void CWar::judgeWarPersenResultType()
{
	ISchemeMatchCoefficient* pMatchCoeff = gServerGlobal->getSchemeCenter()->getSchemeMatchCoeff();
	if (pMatchCoeff == NULL)
		return;

	SMatchCoefficient* pCoeff = pMatchCoeff->getMatchCoeff(getMatchTypeID());
	if (pCoeff == NULL)
	{
		ErrorLn(__FUNCTION__": pCoeff == NULL nMatchType = " << getMatchTypeID());
		return;
	}

	// 战败方优先判断称号
	int nFailedCamp = getEnemyCamp(getWinCamp());
	WarCampMap::iterator iterCamp = m_mapWarCamp.find(nFailedCamp);
	if (iterCamp == m_mapWarCamp.end())
		return;

	WarPersonVector::iterator itPerBegin = iterCamp->second.warPersVect.begin();
	WarPersonVector::iterator itPerEnd = iterCamp->second.warPersVect.end();

	float fFailEcmExpProList[(int)EWEEST_MAX] = { 0 };
	if (nFailedCamp == CAMP_BLUE)
	{
		memcpy(fFailEcmExpProList, m_EcmExpSituation.fEcmExpProList, sizeof(fFailEcmExpProList));
	}
	else
	{
		fFailEcmExpProList[(int)EWEEST_ADVANTAGE] = m_EcmExpSituation.fEcmExpProList[(int)EWEEST_INFERIORITY];
		fFailEcmExpProList[(int)EWEEST_INFERIORITY] = m_EcmExpSituation.fEcmExpProList[(int)EWEEST_ADVANTAGE];
		fFailEcmExpProList[(int)EWEEST_STALEMATE] = m_EcmExpSituation.fEcmExpProList[(int)EWEEST_STALEMATE];
	}

	for (int i = (int)ENMATCHRESULTTYPE_HANG; i <= (int)ENMATCHRESULTTYPE_JILTPAN; ++i)
	{
		itPerBegin = iterCamp->second.warPersVect.begin();
		switch ((ENMATCHRESULTTYPE)i)
		{
		case ENMATCHRESULTTYPE_HANG:
			{
				// 挂机局判定之一 掉线 不动 暂缓
				// 挂机局判定之二 金钱比输出比

				map<int, SCampTeamRecordInfo>::iterator itTeamResult = m_mapCampTeamRecord.find(iterCamp->first);
				if (itTeamResult == m_mapCampTeamRecord.end())
				{
					break;
				}

				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType != 0)
						continue;

					// 金钱比
					int nCampMoney = max(itTeamResult->second.nCampMoney, 1);
					float fMoneyRate = ((float)itPerBegin->nBaseRecord[EDT_Money] / nCampMoney);

					// 输出率
					int nCampOutput = max(itTeamResult->second.nCampOutput, 1);
					float fOutPutRate = ((float)itPerBegin->nBaseRecord[EDT_OutPut] / nCampOutput);

					if (fMoneyRate < pCoeff->fW12&&fOutPutRate < pCoeff->fW12)
						itPerBegin->byResultType = ENMATCHRESULTTYPE_HANG;
				}
			}
			break;
		case ENMATCHRESULTTYPE_HARD:
			{
				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType != 0)
						continue;

					if (itPerBegin->nBaseRecord[EDT_Score] < pCoeff->nW13)
						continue;

					itPerBegin->byResultType = ENMATCHRESULTTYPE_HARD;
				}
			}
			break;
		case ENMATCHRESULTTYPE_JILTPAN:
			{
				bool bJiLTPan = false;
				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->nBaseRecord[EDT_Score] < pCoeff->nW14)
					{
						bJiLTPan = true;
						break;;
					}
				}

				if (!bJiLTPan)
					break;

				itPerBegin = iterCamp->second.warPersVect.begin();
				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType != 0)
						continue;

					if (itPerBegin->nBaseRecord[EDT_Score] < pCoeff->nW15)
						continue;

					itPerBegin->byResultType = ENMATCHRESULTTYPE_JILTPAN;
				}
			}
			break;
		default:
			break;
		}
	}

	int nVictoryCamp = nFailedCamp == CAMP_BLUE ? CAMP_RED : CAMP_BLUE;
	iterCamp = m_mapWarCamp.find(nVictoryCamp);
	if (iterCamp == m_mapWarCamp.end())
		return;

	itPerBegin = iterCamp->second.warPersVect.begin();
	itPerEnd = iterCamp->second.warPersVect.end();

	float fVictoryEcmExpProList[(int)EWEEST_MAX] = { 0 };
	if (nVictoryCamp == CAMP_BLUE)
	{
		memcpy(fVictoryEcmExpProList, m_EcmExpSituation.fEcmExpProList, sizeof(fVictoryEcmExpProList));
	}
	else
	{
		fVictoryEcmExpProList[(int)EWEEST_ADVANTAGE] = m_EcmExpSituation.fEcmExpProList[(int)EWEEST_INFERIORITY];
		fVictoryEcmExpProList[(int)EWEEST_INFERIORITY] = m_EcmExpSituation.fEcmExpProList[(int)EWEEST_ADVANTAGE];
		fVictoryEcmExpProList[(int)EWEEST_STALEMATE] = m_EcmExpSituation.fEcmExpProList[(int)EWEEST_STALEMATE];
	}

	for (int i = (int)ENMATCHRESULTTYPE_WELFARE; i <= (int)ENMATCHRESULTTYPE_LIEWIN; ++i)
	{
		itPerBegin = iterCamp->second.warPersVect.begin();
		switch ((ENMATCHRESULTTYPE)i)
		{
		case ENMATCHRESULTTYPE_WELFARE:
			{
				bool bHang = false;
				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType == ENMATCHRESULTTYPE_HANG)
					{
						bHang = true;
						break;
					}
				}

				if (!bHang)
					break;

				int nWelFareCount = 0;
				itPerBegin = iterCamp->second.warPersVect.begin();
				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType != 0)
						continue;

					itPerBegin->byResultType = ENMATCHRESULTTYPE_WELFARE;
					nWelFareCount++;
				}

				if (nWelFareCount == iterCamp->second.warPersVect.size())
					i = (int)ENMATCHRESULTTYPE_LIEWIN + 1;
			}
			break;
		case ENMATCHRESULTTYPE_CARRY:
			{
				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType != 0)
						continue;

					if (itPerBegin->nBaseRecord[EDT_Score] < pCoeff->nW21)
						continue;

					itPerBegin->byResultType = ENMATCHRESULTTYPE_CARRY;
				}
			}
			break;
		case ENMATCHRESULTTYPE_TILTEDPAN:
			{
				if (fVictoryEcmExpProList[(int)EWEEST_INFERIORITY] < pCoeff->fW22)
					break;

				int nPlayerCount = 0;
				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType != 0)
						continue;

					itPerBegin->byResultType = ENMATCHRESULTTYPE_TILTEDPAN;
					nPlayerCount++;
				}

				if (nPlayerCount == iterCamp->second.warPersVect.size())
					i = (int)ENMATCHRESULTTYPE_LIEWIN + 1;
			}
			break;
		case ENMATCHRESULTTYPE_GRIND:
			{
				if (fVictoryEcmExpProList[(int)EWEEST_ADVANTAGE] < pCoeff->fW23)
					break;

				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType != 0)
						continue;

					if (itPerBegin->nBaseRecord[EDT_Score] < pCoeff->nW24)
						continue;

					itPerBegin->byResultType = ENMATCHRESULTTYPE_GRIND;
				}
			}
			break;
		case ENMATCHRESULTTYPE_LIEWIN:
			{
				for (; itPerBegin != itPerEnd; ++itPerBegin)
				{
					if (itPerBegin->byResultType != 0)
						continue;

					if (itPerBegin->nBaseRecord[EDT_Score] >= pCoeff->nW25)
						continue;

					itPerBegin->byResultType = ENMATCHRESULTTYPE_LIEWIN;
				}
			}
			break;
		default:
			break;
		}
	}
}

void CWar::calculateEcmExpProportionList()
{
	int nTotalCount = m_EcmExpSituation.ecmExpSitnList.size();
	int nAdvantageCount = 0;
	int nStalemateCount = 0;
	int nInferiorityCount = 0;
	EcmExpSitnList::iterator iterBegin = m_EcmExpSituation.ecmExpSitnList.begin();
	for (; iterBegin != m_EcmExpSituation.ecmExpSitnList.end(); ++iterBegin)
	{
		int nFlag = *iterBegin;
		if (nFlag == WAR_ECM_EXP_ADVANTAGE)
			nAdvantageCount++;
		else if (nFlag == WAR_ECM_EXP_STALEMATE)
			nStalemateCount++;
		else if (nFlag == WAR_ECM_EXP_INFERIORITY)
			nInferiorityCount++;
	}

	m_EcmExpSituation.fEcmExpProList[(int)EWEEST_ADVANTAGE] = (float)nAdvantageCount / nTotalCount;
	m_EcmExpSituation.fEcmExpProList[(int)EWEEST_STALEMATE] = (float)nStalemateCount / nTotalCount;
	m_EcmExpSituation.fEcmExpProList[(int)EWEEST_INFERIORITY] = (float)nInferiorityCount / nTotalCount;
}

void CWar::calcAroundBossCount(int nMurderCamp, UID nMurderUID, EWBossType bossType)
{
	WarCampMap::iterator itCamp = m_mapWarCamp.find(nMurderCamp);
	if (itCamp == m_mapWarCamp.end())
	{
		return;
	}

	// 获取击杀者的位置
	ActorHelper helper(nMurderUID);
	IActorService* pActorService = helper.m_ptr;
	if (pActorService == NULL)
	{
		return;
	}
	Vector3 murderLoc = pActorService->getLocation();

	WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
	for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
	{
		bool isAddCount = false;
		if (itPerson->uID == nMurderUID)
		{
			isAddCount = true;
		}
		else
		{
			ActorHelper helper(itPerson->uID);
			IActorService* pActorService = helper.m_ptr;
			if (pActorService == NULL)
			{
				continue;
			}

			// 24米内的友方玩家也算
			Vector3 v = pActorService->getLocation();
			if (murderLoc.checkDistance(v, 24))
			{
				isAddCount = true;
			}
		}

		if (isAddCount)
		{
			if (bossType == EWBossType_BIGDRAGON)
			{
				itPerson->nBigBossMonster++;
			}
			else if (bossType == EWBossType_SMALLDRAGON)
			{
				itPerson->nSmallBossMonster++;
			}

			itPerson->nBaseRecord[EDT_KillDragon]++;
		}
	}

}

int CWar::getHeroReliveTime(int nLevel)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return 0;

	ISchemeHeroReliveTime* pHeroReliveTime = pSchemeCenter->getSchemeHeroReliveTime();
	if (pHeroReliveTime == NULL)
		return 0;

	return pHeroReliveTime->getHeroReliveTime(m_context.nConfigWarID, nLevel);
}

int CWar::getEnemyCamp(int nSelfCamp)
{
	if (nSelfCamp == CAMP_BLUE)
	{
		return CAMP_RED;
	}
	else
	{
		return CAMP_BLUE;
	}
}

// 计算某个阵营的总经济
int CWar::getTotalEmcByCamp(int nCamp, bool bInlucdeComputer)
{
	WarCampMap::iterator iterCamp = m_mapWarCamp.find(nCamp);
	if (iterCamp == m_mapWarCamp.end())
	{
		return 0;
	}

	// 统计该阵营所有金币
	int nTotalMoney = 0;
	SWarCamp* sWarCamp = &iterCamp->second;
	WarPersonVector::iterator itPer = sWarCamp->warPersVect.begin();
	for (; itPer != sWarCamp->warPersVect.end(); ++itPer)
	{
		if (!bInlucdeComputer && itPer->bIsComputer)
			continue;

		nTotalMoney += itPer->nBaseRecord[EDT_Money];
	}

	return nTotalMoney;
}

int CWar::getTotalExpByCamp(int nCamp, bool bInlucdeComputer)
{
	WarCampMap::iterator iterCamp = m_mapWarCamp.find(nCamp);
	if (iterCamp == m_mapWarCamp.end())
	{
		return 0;
	}

	// 统计该阵营所有金币
	int nTotalExp = 0;
	SWarCamp* sWarCamp = &iterCamp->second;
	WarPersonVector::iterator itPer = sWarCamp->warPersVect.begin();
	for (; itPer != sWarCamp->warPersVect.end(); ++itPer)
	{
		if (!bInlucdeComputer && itPer->bIsComputer)
			continue;

		nTotalExp += itPer->nExperien;
	}

	return nTotalExp;
}

int CWar::getAvgExpByCamp(int nCamp, bool bInlucdeComputer)
{
	int nPersonNum = getCampPlayerCount(nCamp, bInlucdeComputer);
	int nTotalExp = getTotalEmcByCamp(nCamp, bInlucdeComputer);

	if (nPersonNum == 0 || nTotalExp == 0)
	{
		return 0;
	}

	return nTotalExp / nPersonNum;
}

void CWar::warEndPunishSubObserveCount()
{
	ICenterConnectorService* pCenterConnectorService = gServerGlobal->getCenterConnectorService();
	if (pCenterConnectorService == NULL)
	{
		ErrorLn(__FUNCTION__": pCenterConnectorService == NULL");
		return;
	}
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		for (WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin(); itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (itPerson->nBottingCount >= BOTTING_COUNT_MAX)
			{
				return;
			}

			// 本局未挂机,则把减少观察期信息到社会服
			ActorHelper actorHelper(itPerson->uID, ACTOR_UID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				continue;
			}
			SMsgPunishSubObserve_SO Msgdata;
			Msgdata.dwActorID = pActorService->getProperty_Integer(PROPERTY_ID);
			Msgdata.dwFromWorldID = pActorService->getFromGameWorldID();

			// 子消息相关
			SMsgMatchDataSubMsg subMsg;
			subMsg.dwMsgCode = SO_MSG_MATCH_PUNISH_SUB_OBSERVE;

			// 数据打包
			obuf256 obMsg;
			obMsg << subMsg << Msgdata;

			pCenterConnectorService->sendToSocial(MSG_MODULEID_MATCH, MSG_MATCH_SERVICE_MODULEMSG, (const char*)obMsg.data(), obMsg.size());
		}
	}
}

void CWar::setLegendCupInfo(const SMsgLegendCupRoomInfo& sLegendCupInfo)
{
	memcpy(&m_sLegendCupInfo, &sLegendCupInfo, sizeof(m_sLegendCupInfo));
}

void CWar::sendLegendCupWarEndInfo()
{
	if (m_sLegendCupInfo.dwLedendCupID == 0)
	{
		return;
	}

	// 每个真人玩家发送下场杯赛信息
	int nFailedCamp = getEnemyCamp(getWinCamp());
	int nTypeWinScore = getWinScoreByType(m_sLegendCupInfo.byBoType);
	bool bNodeEnd = false;
	if (m_sLegendCupInfo.nKin1Camp + 1 == nFailedCamp)
	{
		m_sLegendCupInfo.nNodeKin2Score++;
	}

	if (m_sLegendCupInfo.nKin2Camp + 1 == nFailedCamp)
	{
		m_sLegendCupInfo.nNodeKin1Score++;
	}
	if (m_sLegendCupInfo.nNodeKin2Score >= nTypeWinScore || m_sLegendCupInfo.nNodeKin1Score >= nTypeWinScore)
	{
		bNodeEnd = true;
	}

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = SC_MSG_WAR_END_LEGENDCUP_INFO;

	// 更新到所有的客户端
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		bool isFailed = false;
		if (itCamp->first == nFailedCamp)
		{
			isFailed = true;
		}

		DWORD dwNodeEndNextWarTime = 0;
		bool bIsLastNode = false;
		if (m_sLegendCupInfo.nKin1Camp + 1 == itCamp->first)
		{
			bIsLastNode = m_sLegendCupInfo.bKin1RoundLastNode;
			dwNodeEndNextWarTime = m_sLegendCupInfo.dwKin1NodeEndNextWarTime;

		}

		if (m_sLegendCupInfo.nKin2Camp + 1 == itCamp->first)
		{
			bIsLastNode = m_sLegendCupInfo.bKin2RoundLastNode;
			dwNodeEndNextWarTime = m_sLegendCupInfo.dwKin2NodeEndNextWarTime;
		}

		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (itPerson->bIsComputer)
			{
				continue;
			}

			obuf ob;
			ob << head;
			msg_war_end_legendCup_info data;
			data.eRoundType = m_sLegendCupInfo.eRoundType;
			data.bRoundLastNode = bIsLastNode;
			data.bIsOnlineCup = m_sLegendCupInfo.bIsOnlineCup;
			data.bIsLastRound = m_sLegendCupInfo.nRoundID == m_sLegendCupInfo.nMaxRound;
			data.bWin = !isFailed;
			data.bNodeEnd = bNodeEnd;
			data.dwNextStartInterval = m_sLegendCupInfo.dwWarInterval;
			data.dwNextRoundStartTime = dwNodeEndNextWarTime;

			ob << data;

			send2Client(itPerson->uID, PACKAGE_PTR(new std::string(ob.data(), ob.size())));
		}
	}

}

void CWar::forceEndWar(int nCamp, bool bNormalEnd)
{
	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml == NULL)
		return;

	if (nCamp != CAMP_BLUE && nCamp != CAMP_RED)
		return;

	if (m_CampBaseUID[nCamp] == INVALID_UID)
		return;

	UID uid = m_CampBaseUID[nCamp];

	AddBuff(uid, pEntityXml->nKillBaseBuffID, 1, uid);

	m_WarInfo.bNormalEnd = bNormalEnd;
}

void CWar::setBanHeroList(char * data, size_t len)
{
	if (data == NULL || len < sizeof(SMsgMatchRoomBanHeroData))
	{
		ErrorLn(__FUNCTION__ ": the message is invalid, size=" << len);
		return;
	}

	SMsgMatchRoomBanHeroData* pMsg = (SMsgMatchRoomBanHeroData*)data;
	if (len != sizeof(SMsgMatchRoomBanHeroData) + pMsg->nCount * sizeof(SMsgMatchRoomBanInfo))
	{
		ErrorLn(__FUNCTION__ ": the message is invalid, size=" << len);
		return;
	}
	SMsgMatchRoomBanInfo *pBanHead = (SMsgMatchRoomBanInfo *)(pMsg + 1);
	for (int n = 0; n < pMsg->nCount; ++n)
	{
		int nCamp = pBanHead->nCamp;

		// 这边收到的是阵营是0,1代表蓝、红方，战场这边使用1、2代表蓝、红方
		nCamp++;

		if (nCamp >= 0 && nCamp < CAMP_MAX)
			m_CampBanHeroList[nCamp].push_back(pBanHead->nBanHeroID);

		pBanHead++;
	}
}

void CWar::onAllActorEnter()
{
	EmphasisLn("All Actor Enter War");

	IAIEcmCtrlMgr* pAIEcmCtrlMgr = (IAIEcmCtrlMgr*)getWarMgr(Mgr_AIEcmCtrl);
	if (pAIEcmCtrlMgr != NULL)
	{
		pAIEcmCtrlMgr->initAvgMatchScore();
	}

	IAIExpCtrlMgr* pAIExpCtrlMgr = (IAIExpCtrlMgr*)getWarMgr(Mgr_AIExpCtrl);
	if (pAIExpCtrlMgr != NULL)
	{
		pAIExpCtrlMgr->initAvgMatchScore();
	}
}

// 设置充能值
void CWar::setChargedValue(UID uidTarget, int nEffectID, int nValue)
{
	CHARGED_MAP::iterator Iter = m_ChargedMap.find(uidTarget);
	if (Iter != m_ChargedMap.end())
	{
		CHARGED_EFFECT_MAP &effectMap = Iter->second;
		CHARGED_EFFECT_MAP::iterator IterEffect = effectMap.find(nEffectID);
		if (IterEffect != effectMap.end())
		{
			IterEffect->second = nValue;
		}
		else
		{
			// 插入效果充能表
			effectMap.insert(CHARGED_EFFECT_MAP::value_type(nEffectID, nValue));
		}
	}
	else
	{
		CHARGED_EFFECT_MAP effectMap;
		// 插入效果充能表
		effectMap.insert(CHARGED_EFFECT_MAP::value_type(nEffectID, nValue));
		// 插入充能表
		m_ChargedMap.insert(CHARGED_MAP::value_type(uidTarget, effectMap));
	}
}

// 取得充能值
int CWar::getChargedValue(UID uidTarget, int nEffectID)
{
	CHARGED_MAP::iterator Iter = m_ChargedMap.find(uidTarget);
	if (Iter == m_ChargedMap.end())
	{
		return 0;
	}

	CHARGED_EFFECT_MAP &effectMap = Iter->second;
	CHARGED_EFFECT_MAP::iterator IterEffect = effectMap.find(nEffectID);
	if (IterEffect == effectMap.end())
	{
		return 0;
	}

	return IterEffect->second;
}

BYTE CWar::getMaxCamp()
{
    return m_context.byCampCount;
}


DWORD CWar::warSerchTypeUIDList(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID * pReturnArray, DWORD dwArrayMaxSize)
{
	// 搜寻战场所有指定类型UID
	DWORD dwArraySize = 0;
	if (bySerchType == EWME_None)
	{
		// 查找玩家
		WarCampMap::iterator itCamp = m_mapWarCamp.begin();
		for (; itCamp != m_mapWarCamp.end(); ++itCamp)
		{
			if (dwArraySize >= dwArrayMaxSize)
			{
				return dwArrayMaxSize;
			}

			for (WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin(); itPerson != itCamp->second.warPersVect.end(); ++itPerson)
			{
				switch (byGetCampType)
				{
				case EWCT_SELFCAMP:
					{
						// 己方
						if (itCamp->first == bySelfCamp)
						{
							pReturnArray[dwArraySize++] = itPerson->uID;
						}

					}
					break;
				case EWCT_ENIMICAMP:
					{
						if (itCamp->first != bySelfCamp)
						{
							pReturnArray[dwArraySize++] = itPerson->uID;
						}
					}
					break;
				case EWCT_ALL:
					{
						pReturnArray[dwArraySize++] = itPerson->uID;
					}
					break;
				default:break;
				}
			}
		}
	}
	else
	{
		IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)getWarMgr(Mgr_Monster);
		if (pWarMonsterMgr != NULL)
		{
			dwArraySize = pWarMonsterMgr->getUIDListByMonsterType(bySelfCamp, byGetCampType, bySerchType, pReturnArray, dwArrayMaxSize);
		}
	}

	return dwArraySize;
}

bool CWar::createOBViewerEntity()
{
    //MatchScenePlayer* pSceneRoomPlayer = getOBPlayer(PlayerID);
    //if (pSceneRoomPlayer == NULL)
    //{
    //    ErrorLn(__FUNCTION__": pSceneRoomPlayer == NULL PlayerID ="<<PlayerID);
    //    return false;
    //}
    //ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    //if ( pSchemeCenter == NULL )
    //{
    //    ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
    //    return false;
    //}
    //SSchemeMatchRoom* pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom()->getSchemeMatchRoomByMatchTypeID(m_msg.dwMatchTypeID);
    //if(pSchemeMatchRoom == NULL)
    //{
    //    ErrorLn(__FUNCTION__ << ": pSchemeMatchRoom == NULL dwMatchTypeID ="<<m_msg.dwMatchTypeID);
    //    return false;
    //}

    //ISchemePersonModel* pSchemeModel = pSchemeCenter->getSchemePersonModel();
    //if(pSchemeModel == NULL)
    //{
    //    return false;
    //}

    //PlayerRoleProperty property;
    //property.nPDBID = pSceneRoomPlayer->m_role.dwPDBID;
    //sstrcpyn(property.szName, pSceneRoomPlayer->m_role.szName, sizeof(property.szName));
    //property.nSceneID = m_nSceneID;
    //property.nVocation = 1;
    //property.nSkin = 0;
    //property.byCamp = 0;
    //property.nLevel = 1;

    //const PersonSkinModelItem * pPersonSkinModelItem = pSchemeModel->getPersonSkinModelItem(1);
    //if(!pPersonSkinModelItem)
    //{
    //    ErrorLn(__FUNCTION__ << " failed,name="<< property.szName <<", pdbid="<< property.nPDBID <<", vocation=" << property.nVocation<<" check the config PersonModel.csv!");
    //    return false;
    //}

    ////位置相关
    //SPosWithAngle ptBornPos;
    //if (getBornPos(property.byCamp, pSceneRoomPlayer->m_role.nIndex, ptBornPos) == false)
    //{
    //    ErrorLn(__FUNCTION__":getBornPos failed.nCamp=" << property.byCamp << ",roleIndex=" << pSceneRoomPlayer->m_role.nIndex);
    //}
    //property.location = ptBornPos.ptPos;
    //property.angle = Vector3(0, ptBornPos.fAngleY, 0);
    //property.nAISchemeID = -1;
    //property.nSkin = (property.nSkin == 0) ? pPersonSkinModelItem->nDefaultSkinID : property.nSkin;
    //property.bySex = pPersonSkinModelItem->bySex;
    //property.byObserver = 1;

    //obuf o;
    //short partid = PART_PROPERTY;
    //o << partid;
    //int nLenOffset = o.size();
    //int nLen = 0;
    //o << nLen;  // 压入部件长度
    //int nBegin = o.size();
    //property.getDataBuffer(o, TO_OTHER_SEVER);
    //*((int*)(o.data() + nLenOffset)) = o.size() - nBegin;

    //partid = PART_WAR;
    //o << partid;
    //nLenOffset = o.size();
    //nLen = 0;
    //o << nLen;  // 压入部件长度
    //nBegin = o.size();
    //o << m_nWarID;
    //o << (int)pSceneRoomPlayer->m_role.nIndex; 

    //// 取召唤师技能
    //int arraySkill[SUMMONER_SKILL_COUNT] = {0};
    //int arraySlot[SUMMONER_SKILL_COUNT] = {0};
    //int nCount = SUMMONER_SKILL_COUNT;
    //memcpy(arraySlot, pSceneRoomPlayer->m_SummonerSlot,sizeof(arraySlot));
    //memcpy(arraySkill, pSceneRoomPlayer->m_role.nSummonserSkill,sizeof(arraySkill));

    //o << nCount;
    //for(int i = 0; i < nCount; ++i)
    //{
    //    o << arraySlot[i] << arraySkill[i];
    //}
    //*((int*)(o.data() + nLenOffset)) = o.size() - nBegin; // 保存部件长度

    //SActorCreateContext context;
    //context.client = pSceneRoomPlayer->m_role.clientID;
    //if(pSceneRoomPlayer->m_role.isComputerPlayer)
    //{
    //    context.nFlag = CONTEXT_FLAG_ATTACH_THREAD;
    //}
    //context.udbid = pSceneRoomPlayer->m_role.dwUserID;
    //context.pdbid = pSceneRoomPlayer->m_role.dwPDBID; 
    //context.nSceneID = m_nSceneID;
    //context.nSceneSerialNo = gServerGlobal->getSceneManager()->sceneIDToSerialNo(context.nSceneID);
    //context.locBorn = ptBornPos.ptPos;
    //context.fAngle = ptBornPos.fAngleY;
    //context.eDStype = eds_type_cache;
    //context.nCreateDataLen = o.size();
    //context.nCreateDataFlag = TO_OTHER_SEVER;  

    //obuf buf;
    //buf << context;
    //buf.push_back(o.data(), o.size());

    //int nReason = elogin_in_process;
    //nReason = elogin_online;

    //m_OBViewerUID = gServerGlobal->getActorMgr()->createActor((SActorCreateContext*)buf.data(), nReason);
    return true;
}

void CWar::releaseOBViewerEntity()
{
    int nSceneID = UID_2_SCENE(m_OBViewerUID);
    SceneLogicHelper helper(nSceneID);
    if(!helper.m_ptr)
    {
        ErrorLn(__FUNCTION__ << __LINE__ << ", uid=" << m_OBViewerUID << ", destroy failed, scenelogic not found");
    }
    else
    {
        helper.m_ptr->destroyActor(m_OBViewerUID, elogout_offline);
    }
}

void CWar::calcContKill()
{
	WarCampMap::iterator it = m_mapWarCamp.begin();
	for (; it != m_mapWarCamp.end(); ++it)
	{
		WarPersonVector::iterator swPerson = it->second.warPersVect.begin();
		for (; swPerson != it->second.warPersVect.end(); ++swPerson)
		{
			// 清空连杀
			setKillMaxInfo(&(*swPerson));

			swPerson->deadContKillList.push_back(swPerson->nDeadContKill);  // 记录不死连杀
			swPerson->timeContKillList.push_back(swPerson->nTimeContKill);  // 记录时间连杀

			swPerson->nDeadContKill = 0;
			swPerson->nTimeContKill = 0;
			swPerson->nAliveContAssist = 0;

			map<int, SCampTeamRecordInfo>::iterator itTeamResult = m_mapCampTeamRecord.find(swPerson->nCamp);
			if (itTeamResult != m_mapCampTeamRecord.end())
			{
				int nCampKillNum = max(itTeamResult->second.nCampKillNum, 1);
				swPerson->nBaseRecord[EDT_JoinBattleRate] = 100 * ((float)(swPerson->nBaseRecord[EDT_KillCount] + swPerson->nBaseRecord[EDT_AssistCount])) / nCampKillNum;
			}

		}
	}
}

void CWar::calcFailedCampMvp(int nCamp, map<int, SEDTPersonData> &pMaxData)
{
	SWarCamp* pWarCamp = getWarCamp(nCamp);
	SWPerson* pMaxPerson = NULL;
	int nMaxScore = 0;
	if (pWarCamp != NULL)
	{
		for (auto itPerson = pWarCamp->warPersVect.begin(); itPerson != pWarCamp->warPersVect.end(); ++itPerson)
		{
			if (itPerson->nBaseRecord[EDT_Score] > nMaxScore)
			{
				nMaxScore = itPerson->nBaseRecord[EDT_Score];
				pMaxPerson = &(*itPerson);
			}
		}
	}
	if (pMaxPerson != NULL)
	{
		pMaxPerson->nBaseRecord[EDT_MvpOfLoser] = 1;
		pMaxData[EDT_MvpOfLoser].dwActorID = pMaxPerson->uPDBID;
		pMaxData[EDT_MvpOfLoser].nBaseRecord = 1;
	}
}
