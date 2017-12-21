/*******************************************************************
** 文件名:	e:\Rocket\Server\EntityServer\Src\TankPropBank.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/4/21 14:53
** 版  本:	1.0
** 描  述:	载具属性银行
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "DEntityBuild.h"
#include "TankPropBank.h"
#include "ExternalFacade.h"
#include "CreatureIDTable.h"
#include "IGameDatabaseDef.h"
#include "Tank.h"
#include "IResAdjustServer.h"
// 放大比率比
#define ZOOMIN_MULTIPLE		10000.00
/** 
@param   
@param   
@return  
*/
CTankPropBank::CTankPropBank(void)
{
	// UID
	m_uid = INVALID_UID;

	// 场景
	m_pGameZone = NULL;

	// 位置
	m_ptMapTile.x = m_ptMapTile.y = 0;

	// 数组属性
	memset(m_nNumProp, 0, sizeof(m_nNumProp));

	// 怪物称号
	memset(m_szTitle, 0, sizeof(m_szTitle));

	// 扩展数据
	memset(m_szExtendData, 0, sizeof(m_szExtendData));

	// 载具实体
	m_pMaster = NULL;

	// 是否固化移动速度
	m_bSolidifyMoveSpeed = false;

	// 是否启动了延时刷新血量
	m_dwWaitSyncHPStartTick = 0;

	// 载具等级
	m_lLevel = 0;
}

/** 还原,不释放内存，只将状态还原到创建时状态
@param   未来继续使用
@param   
@return  
*/
void CTankPropBank::Restore(void)
{
	// UID
	m_uid = INVALID_UID;

	// 场景
	m_pGameZone = NULL;

	// 位置
	m_ptMapTile.x = m_ptMapTile.y = 0;

	// 数组属性
	memset(m_nNumProp, 0, sizeof(m_nNumProp));

	// 怪物称号
	memset(m_szTitle, 0, sizeof(m_szTitle));

	// 扩展数据
	memset(m_szExtendData, 0, sizeof(m_szExtendData));

	// 载具实体
	m_pMaster = NULL;

	// 是否固化移动速度
	m_bSolidifyMoveSpeed = false;

	// 是否启动了延时刷新血量
	m_dwWaitSyncHPStartTick = 0;

	// 载具等级
	m_lLevel = 0;
}

/** 创建
@param   
@param   
@return  
*/
bool CTankPropBank::Create(CTank * pTank)
{
	m_pMaster = pTank;

	return true;
}

/** 
@param   
@param   
@return  
*/
CTankPropBank::~CTankPropBank(void)
{

}

/** 设置UID
@param   
@param   
@return  
*/
void CTankPropBank::SetUID(LONGLONG uid)
{
	m_uid = uid;
}

/** 取得UID
@param   
@param   
@return  
*/
LONGLONG CTankPropBank::GetUID(void)
{
	return m_uid;
}

/** 取得场景ID
@param   
@param   
@return  
*/
bool CTankPropBank::SetZoneID(DWORD dwZoneID)
{
	m_pGameZone = g_ExternalFacade.GetGameZone(dwZoneID);
	if(m_pGameZone == NULL)
	{
		ErrorLn("创建载具时设置的zoneid无效, 场景不存在, zoneid="<<dwZoneID);
		debugBreak;

		return false;
	}	

	return true;
}

/** 取得场景ID
@param   
@param   
@return  
*/
DWORD CTankPropBank::GetZoneID(void)
{
	if(m_pGameZone == NULL)
	{
		return INVALID_ZONEID;
	}

	return m_pGameZone->GetZoneID();
}

/** 取得地图ID
@param   
@param   
@return  
*/
DWORD CTankPropBank::GetMapID(void)
{
	if(m_pGameZone == NULL)
	{
		return INVALID_MAPID;
	}

	return m_pGameZone->GetMapID();
}

/** 设置地图位置
@param   
@param   
@return  
*/
void CTankPropBank::SetMapLoc(POINT ptMapTile)
{
	m_ptMapTile = ptMapTile;
}

/** 取得所在的地图位置
@param   
@param   
@return  
*/
POINT CTankPropBank::GetMapLoc(void)
{
	return m_ptMapTile;
}

/** 取得数值属性
@param   
@param   
@return  
*/
int CTankPropBank::GetNumProp(DWORD dwPropID)
{
	// 要转成内部tankpropid
	if(dwPropID < 0 || dwPropID >= CREATURE_PROP_MAXID || g_CreatureID2TankPropIDTable[dwPropID] == 0)
	{
		return 0;
	}
	dwPropID = g_CreatureID2TankPropIDTable[dwPropID];

	return m_nNumProp[dwPropID];
}

/** 导出创建别人的客户端现场
@param   
@param   
@return  
*/
bool CTankPropBank::ExportShadowContext(LPSTR pszContext, int &nLen)
{
	ofixbuf ofixb(pszContext, nLen);

	// 位置
	ofixb.push_back(&m_ptMapTile, sizeof(POINT));

	// 数值属性
	ofixb.push_back(m_nNumProp, sizeof(int) * TANK_PROP_BROADCAST);

	// 载具等级
	ofixb.push_back(&m_lLevel, sizeof(long));

	// 载具称号
	if(m_szTitle[0] != 0/*这样判断最快*/)
	{
		ofixb.push_back(m_szTitle, sizeof(m_szTitle));
	}

	if(!ofixb)
	{
		return false;
	}

	nLen = ofixb.size();

	return true;	
}

/** 导出创建自已的客户端现场
@param   
@param   
@return  
*/
bool CTankPropBank::ExportHeroContext(LPSTR pszContext, int &nLen)
{
	ofixbuf ofixb(pszContext, nLen);

	// 位置
	ofixb.push_back(&m_ptMapTile, sizeof(POINT));

	// 数值属性
	ofixb.push_back(m_nNumProp, sizeof(int) * TANK_PROP_CUR_KICK_FATAL_PER);

	// 载具等级
	ofixb.push_back(&m_lLevel, sizeof(long));

	// 载具称号
	if(m_szTitle[0] != 0/*这样判断最快*/)
	{
		ofixb.push_back(m_szTitle, sizeof(m_szTitle));
	}

	if(!ofixb)
	{
		return false;
	}

	nLen = ofixb.size();

	return true;	
}


/** 导出数据库属性现场
@param   
@param   
@return  
*/
bool CTankPropBank::ExportMapContext(LPSTR pszContext, int &nLen)
{
	if(nLen < sizeof(STransMapTankContext))
	{
		return false;
	}
	STransMapTankContext * pContext = (STransMapTankContext *)(pszContext);

	// 载具称号
	sstrcpyn(pContext->szTitle, m_szTitle, sizeof(pContext->szTitle));

	// 载具ID 
	pContext->lTankID = m_nNumProp[TANK_PROP_TANKID];

	// 国籍
	pContext->nNation = m_nNumProp[TANK_PROP_NATION];

	// 座位数
	pContext->lSeatCount = m_nNumProp[TANK_PROP_SEATCOUNT];

	// 当前生命值
	pContext->lCurHP = m_nNumProp[TANK_PROP_CUR_HP];

	// 生命值上限
	pContext->lMaxCurHP = m_nNumProp[TANK_PROP_MAX_HP];

	// 当前法术值
	pContext->lCurMP = m_nNumProp[TANK_PROP_CUR_MP];

	// 法术值上限
	pContext->lMaxCurMP = m_nNumProp[TANK_PROP_MAX_MP];

	// 载具等级
	pContext->lLevel = m_lLevel;

	// 扩展数据
	memcpy(pContext->szExtendData, m_szExtendData, sizeof(pContext->szExtendData));

	// 角度
	pContext->lAngle = m_nNumProp[TANK_PROP_ANGLE];

	return true;	
}

/** 导入默认创建现场
@param   
@param   
@return  
*/
bool CTankPropBank::ImportDefaultCreateContext(LPCSTR pszContext, int nLen)
{
	ITankServer * pTankServer = g_ExternalFacade.GetTankServer();
	if(pTankServer == NULL)
	{
		return false;
	}

	if(nLen != sizeof(SBuildTankContext))
	{
		return false;
	}

	SBuildTankContext * pContext = (SBuildTankContext *)(pszContext);
	
	/*const STankSchemeInfo * pSchemeInfo = g_ExternalFacade.GetTankSchemeInfo(pContext->nTankID);
	if(pSchemeInfo == NULL)
	{
		WarningLn("创建的载具配置脚本里不存在 tankid = "<<pContext->nTankID<<endl);
		return false;
	}*/

	const STankSchemeInfo * pSchemeInfo = NULL; STankSchemeInfo tankInfo;
	pSchemeInfo = pTankServer->GetTankSchemeInfo(pContext->nTankID);
	if(pSchemeInfo == NULL)
	{
		WarningLn("创建的载具配置脚本里不存在 nTankID="<<pContext->nTankID);

		return false;
	}

	if(pSchemeInfo->lCustomType > 0)
	{
		if(pTankServer->GetTankSchemeInfo(pContext->nTankID, pContext->nLevel, tankInfo, pContext->nCivilLevel))
		{			
			pSchemeInfo = &tankInfo;
		}
		else
		{
			WarningLn("自定义载具 创建的载具配置脚本里不存在 nTankID="<<pContext->nTankID<<" nLevel="<<pContext->nLevel);
			return false;
		}
	}

	if(pSchemeInfo == NULL)
	{
		WarningLn("创建的载具配置脚本里不存在 nTankID="<<pContext->nTankID<<" nLevel="<<pContext->nLevel);
		return false;
	}

	// 载具ID
	m_nNumProp[TANK_PROP_TANKID] = pContext->nTankID;

	// 载具等级
	m_lLevel = pSchemeInfo->lLevel;

	// 位置
	m_ptMapTile = pContext->ptMapTile;

	// 当前生命值
	m_nNumProp[TANK_PROP_CUR_HP] = pSchemeInfo->lHP;

	// 生命值上限
	m_nNumProp[TANK_PROP_MAX_HP] = pSchemeInfo->lHP;

	// 当前法术值
	m_nNumProp[TANK_PROP_CUR_MP] = pSchemeInfo->lMP;

	// 法术值上限
	m_nNumProp[TANK_PROP_MAX_MP] = pSchemeInfo->lMP;

	// 当前移动速度
	m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] = pSchemeInfo->nMoveSpeed;

	// 面向角度
	m_nNumProp[TANK_PROP_ANGLE] = pContext->nAngle;

	// 所属国籍
	m_nNumProp[TANK_PROP_NATION] = pContext->nNation;

	// 座位数
	if(pContext->nSeatCount < 0)
	{
		m_nNumProp[TANK_PROP_SEATCOUNT] = pSchemeInfo->lLoadCount;
	}
	else
	{
		m_nNumProp[TANK_PROP_SEATCOUNT] = pContext->nSeatCount;
	}

	// 当前最小物理攻击
	m_nNumProp[TANK_PROP_CUR_MIN_P_A] = pSchemeInfo->lMinPAttack;

	// 当前最大物理攻击
	m_nNumProp[TANK_PROP_CUR_MAX_P_A] = pSchemeInfo->lMaxPAttack;

	// 当前最小法术攻击
	m_nNumProp[TANK_PROP_CUR_MIN_M_A] = pSchemeInfo->lMinMAttack;

	// 当前最大法术攻击
	m_nNumProp[TANK_PROP_CUR_MAX_M_A] = pSchemeInfo->lMaxMAttack;

	// 当前物理防御
	m_nNumProp[TANK_PROP_CUR_P_D] = pSchemeInfo->lPDefend;

	// 当前法术防御
	m_nNumProp[TANK_PROP_CUR_M_D] = pSchemeInfo->lMDefend;	

	// 当前火法术防御
	m_nNumProp[TANK_PROP_CUR_F_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lFDefend;

	// 当前冰法术防御
	m_nNumProp[TANK_PROP_CUR_I_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lIDefend;

	// 当前雷法术防御
	m_nNumProp[TANK_PROP_CUR_T_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lTDefend;

	// 当前道法术防御
	m_nNumProp[TANK_PROP_CUR_D_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lDDefend;

	// 当前物理致命概率
	m_nNumProp[TANK_PROP_CUR_P_FATAL_PER] = pSchemeInfo->lPFatalPer;

	// 当前法术致命概率
	m_nNumProp[TANK_PROP_CUR_M_FATAL_PER] = pSchemeInfo->lMFatalPer;

	// 当前命中概率
	m_nNumProp[TANK_PROP_CUR_PRESENT_PER] = pSchemeInfo->lPresentPer;

	// 当前躲闪概率
	m_nNumProp[TANK_PROP_CUR_HEDGE_PER] = pSchemeInfo->lHedgePer;

	// 当前格挡概率
	m_nNumProp[TANK_PROP_CUR_RESIST_PER] = pSchemeInfo->lResistPer;

	// 当前穿透概率
	m_nNumProp[TANK_PROP_CUR_PENETRATE_PER] = pSchemeInfo->lPenetratePer;

	// 系数加成
	IResAdjustServer* pResAdjustServer = g_ExternalFacade.GetResAdjustServer();
	if (pResAdjustServer!= NULL)
	{/*
		long lLevel = pResAdjustServer->GetGameUpLevelLimit();
		const STankCivilCoefSchemeInfo*	pCoefSchemeInfo = pTankServer->GetTankCivilCoefSchemeInfo(lLevel);
		if (pCoefSchemeInfo!=NULL)
		{
			// 当前生命值
			m_nNumProp[TANK_PROP_CUR_HP] = pSchemeInfo->lHP * pCoefSchemeInfo->fHPCoef;

			// 生命值上限
			m_nNumProp[TANK_PROP_MAX_HP] = pSchemeInfo->lHP * pCoefSchemeInfo->fHPCoef;

			// 当前法术值
			m_nNumProp[TANK_PROP_CUR_MP] = pSchemeInfo->lMP * pCoefSchemeInfo->fMPCoef;

			// 法术值上限
			m_nNumProp[TANK_PROP_MAX_MP] = pSchemeInfo->lMP * pCoefSchemeInfo->fMPCoef;

			// 当前最小物理攻击
			m_nNumProp[TANK_PROP_CUR_MIN_P_A] = m_nNumProp[TANK_PROP_CUR_MIN_P_A] * pCoefSchemeInfo->fPAttackCoef;

			// 当前最大物理攻击
			m_nNumProp[TANK_PROP_CUR_MAX_P_A] = m_nNumProp[TANK_PROP_CUR_MAX_P_A] * pCoefSchemeInfo->fPAttackCoef;

			// 当前最小法术攻击
			m_nNumProp[TANK_PROP_CUR_MIN_M_A] = m_nNumProp[TANK_PROP_CUR_MIN_M_A] * pCoefSchemeInfo->fMAttackCoef;

			// 当前最大法术攻击
			m_nNumProp[TANK_PROP_CUR_MAX_M_A] = m_nNumProp[TANK_PROP_CUR_MAX_M_A] * pCoefSchemeInfo->fMAttackCoef;

			// 当前物理防御
			m_nNumProp[TANK_PROP_CUR_P_D] = m_nNumProp[TANK_PROP_CUR_P_D] * pCoefSchemeInfo->fPDefendCoef;

			// 当前法术防御
			m_nNumProp[TANK_PROP_CUR_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] * pCoefSchemeInfo->fMDefendCoef;	

			// 当前火法术防御
			m_nNumProp[TANK_PROP_CUR_F_M_D] = m_nNumProp[TANK_PROP_CUR_F_M_D] * pCoefSchemeInfo->fFDefendCoef;

			// 当前冰法术防御
			m_nNumProp[TANK_PROP_CUR_I_M_D] = m_nNumProp[TANK_PROP_CUR_I_M_D] * pCoefSchemeInfo->fIDefendCoef;

			// 当前雷法术防御
			m_nNumProp[TANK_PROP_CUR_T_M_D] = m_nNumProp[TANK_PROP_CUR_T_M_D] * pCoefSchemeInfo->fTDefendCoef;

			// 当前道法术防御
			m_nNumProp[TANK_PROP_CUR_D_M_D] = m_nNumProp[TANK_PROP_CUR_D_M_D] * pCoefSchemeInfo->fDDefendCoef;
		}*/
	}
	
	return true;
}


/** 导入地图创建现场
@param   
@param   
@return  
*/
bool CTankPropBank::ImportMapCreateContext(LPCSTR pszContext, int nLen)
{
	ITankServer * pTankServer = g_ExternalFacade.GetTankServer();
	if(pTankServer == NULL)
	{
		return false;
	}

	if(nLen != sizeof(STransMapTankContext))
	{
		return false;
	}

	STransMapTankContext * pContext = (STransMapTankContext *)(pszContext);

	// 载具ID
	m_nNumProp[TANK_PROP_TANKID] = pContext->lTankID;

/*
	const STankSchemeInfo * pSchemeInfo = g_ExternalFacade.GetTankSchemeInfo(m_nNumProp[TANK_PROP_TANKID]);
	if(pSchemeInfo == NULL)
	{
		WarningLn("导入地图创建的载具配置脚本里不存在 tankid = "<<m_nNumProp[TANK_PROP_TANKID]<<endl);
		return false;
	}*/

	const STankSchemeInfo * pSchemeInfo = NULL; STankSchemeInfo tankInfo;
	pSchemeInfo = pTankServer->GetTankSchemeInfo(pContext->lTankID);
	if(pSchemeInfo == NULL)
	{
		WarningLn("创建的载具配置脚本里不存在 lTankID="<<pContext->lTankID);

		return false;
	}

	if(pSchemeInfo->lCustomType > 0)
	{
		if(pTankServer->GetTankSchemeInfo(pContext->lTankID, pContext->lLevel, tankInfo))
		{			
			pSchemeInfo = &tankInfo;
		}
		else
		{
			WarningLn("自定义载具 创建的载具配置脚本里不存在 lTankID="<<pContext->lTankID<<" lLevel="<<pContext->lLevel);
			return false;
		}
	}

	if(pSchemeInfo == NULL)
	{
		WarningLn("创建的载具配置脚本里不存在 lTankID="<<pContext->lTankID<<" lLevel="<<pContext->lLevel);
		return false;
	}

	// 载具等级
	m_lLevel = pSchemeInfo->lLevel;

	// 名字
	sstrcpyn(m_szTitle, pContext->szTitle, sizeof(m_szTitle));

	// 扩展数据
	memcpy(m_szExtendData, pContext->szExtendData, sizeof(m_szExtendData));

	// 位置
	m_ptMapTile.x = pContext->lLocX;
	m_ptMapTile.y = pContext->lLocY;

	// 当前生命值
	m_nNumProp[TANK_PROP_CUR_HP] = pContext->lCurHP;

	// 生命值上限
	m_nNumProp[TANK_PROP_MAX_HP] = pContext->lMaxCurHP;

	// 当前法术值
	m_nNumProp[TANK_PROP_CUR_MP] = pContext->lCurMP;

	// 法术值上限
	m_nNumProp[TANK_PROP_MAX_MP] = pContext->lMaxCurMP;

	// 所属国籍
	m_nNumProp[TANK_PROP_NATION] = pContext->nNation;

	// 座位数
	if(pContext->lSeatCount <= 0)
	{
		m_nNumProp[TANK_PROP_SEATCOUNT] = pSchemeInfo->lLoadCount;
	}
	else
	{
		m_nNumProp[TANK_PROP_SEATCOUNT] = pContext->lSeatCount;
	}
	
	// 面向角度,面向玩家
	m_nNumProp[TANK_PROP_ANGLE] = pContext->lAngle;

	// 当前移动速度
	m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] = pSchemeInfo->nMoveSpeed;

	// 当前最小物理攻击
	m_nNumProp[TANK_PROP_CUR_MIN_P_A] = pSchemeInfo->lMinPAttack;

	// 当前最大物理攻击
	m_nNumProp[TANK_PROP_CUR_MAX_P_A] = pSchemeInfo->lMaxPAttack;

	// 当前最小法术攻击
	m_nNumProp[TANK_PROP_CUR_MIN_M_A] = pSchemeInfo->lMinMAttack;

	// 当前最大法术攻击
	m_nNumProp[TANK_PROP_CUR_MAX_M_A] = pSchemeInfo->lMaxMAttack;

	// 当前物理防御
	m_nNumProp[TANK_PROP_CUR_P_D] = pSchemeInfo->lPDefend;

	// 当前法术防御
	m_nNumProp[TANK_PROP_CUR_M_D] = pSchemeInfo->lMDefend;	

	// 当前火法术防御
	m_nNumProp[TANK_PROP_CUR_F_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lFDefend;

	// 当前冰法术防御
	m_nNumProp[TANK_PROP_CUR_I_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lIDefend;

	// 当前雷法术防御
	m_nNumProp[TANK_PROP_CUR_T_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lTDefend;

	// 当前道法术防御
	m_nNumProp[TANK_PROP_CUR_D_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lDDefend;

	// 当前物理致命概率
	m_nNumProp[TANK_PROP_CUR_P_FATAL_PER] = pSchemeInfo->lPFatalPer;

	// 当前法术致命概率
	m_nNumProp[TANK_PROP_CUR_M_FATAL_PER] = pSchemeInfo->lMFatalPer;

	// 当前命中概率
	m_nNumProp[TANK_PROP_CUR_PRESENT_PER] = pSchemeInfo->lPresentPer;

	// 当前躲闪概率
	m_nNumProp[TANK_PROP_CUR_HEDGE_PER] = pSchemeInfo->lHedgePer;

	// 当前格挡概率
	m_nNumProp[TANK_PROP_CUR_RESIST_PER] = pSchemeInfo->lResistPer;

	// 当前穿透概率
	m_nNumProp[TANK_PROP_CUR_PENETRATE_PER] = pSchemeInfo->lPenetratePer;

	// 系数加成
	IResAdjustServer* pResAdjustServer = g_ExternalFacade.GetResAdjustServer();
	if (pResAdjustServer!= NULL)
	{/*
		long lLevel = pResAdjustServer->GetGameUpLevelLimit();
		const STankCivilCoefSchemeInfo*	pCoefSchemeInfo = pTankServer->GetTankCivilCoefSchemeInfo(lLevel);
		if (pCoefSchemeInfo!=NULL)
		{
			// 当前生命值
			//m_nNumProp[TANK_PROP_CUR_HP] = pSchemeInfo->lHP * pCoefSchemeInfo->fHPCoef;

			// 生命值上限
			//m_nNumProp[TANK_PROP_MAX_HP] = pSchemeInfo->lHP * pCoefSchemeInfo->fHPCoef;

			// 当前法术值
			//m_nNumProp[TANK_PROP_CUR_MP] = pSchemeInfo->lMP * pCoefSchemeInfo->fMPCoef;

			// 法术值上限
			//m_nNumProp[TANK_PROP_MAX_MP] = pSchemeInfo->lMP * pCoefSchemeInfo->fMPCoef;

			// 当前最小物理攻击
			m_nNumProp[TANK_PROP_CUR_MIN_P_A] = m_nNumProp[TANK_PROP_CUR_MIN_P_A] * pCoefSchemeInfo->fPAttackCoef;

			// 当前最大物理攻击
			m_nNumProp[TANK_PROP_CUR_MAX_P_A] = m_nNumProp[TANK_PROP_CUR_MAX_P_A] * pCoefSchemeInfo->fPAttackCoef;

			// 当前最小法术攻击
			m_nNumProp[TANK_PROP_CUR_MIN_M_A] = m_nNumProp[TANK_PROP_CUR_MIN_M_A] * pCoefSchemeInfo->fMAttackCoef;

			// 当前最大法术攻击
			m_nNumProp[TANK_PROP_CUR_MAX_M_A] = m_nNumProp[TANK_PROP_CUR_MAX_M_A] * pCoefSchemeInfo->fMAttackCoef;

			// 当前物理防御
			m_nNumProp[TANK_PROP_CUR_P_D] = m_nNumProp[TANK_PROP_CUR_P_D] * pCoefSchemeInfo->fPDefendCoef;

			// 当前法术防御
			m_nNumProp[TANK_PROP_CUR_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] * pCoefSchemeInfo->fMDefendCoef;	

			// 当前火法术防御
			m_nNumProp[TANK_PROP_CUR_F_M_D] = m_nNumProp[TANK_PROP_CUR_F_M_D] * pCoefSchemeInfo->fFDefendCoef;

			// 当前冰法术防御
			m_nNumProp[TANK_PROP_CUR_I_M_D] = m_nNumProp[TANK_PROP_CUR_I_M_D] * pCoefSchemeInfo->fIDefendCoef;

			// 当前雷法术防御
			m_nNumProp[TANK_PROP_CUR_T_M_D] = m_nNumProp[TANK_PROP_CUR_T_M_D] * pCoefSchemeInfo->fTDefendCoef;

			// 当前道法术防御
			m_nNumProp[TANK_PROP_CUR_D_M_D] = m_nNumProp[TANK_PROP_CUR_D_M_D] * pCoefSchemeInfo->fDDefendCoef;
		}*/
	}

	return true;
}

/** 取得怪物名字
@param   
@param   
@return  
*/
LPCSTR CTankPropBank::GetTitle(void)
{
	return m_szTitle;
}

/** 设置怪物名字
@param   
@param   
@return  
*/
void CTankPropBank::SetTitle(LPCSTR pszTitle)
{
	sstrcpyn(m_szTitle, pszTitle, sizeof(m_szTitle));

	UpdateClientTitle();
}

/** 设置数值属性
@param   nValue    ：每个属性有不同的更新方式（覆盖，禁止修改，增量）
@param   bSyncFlag ：是否要同步
@return  
*/
typedef void * (CTankPropBank::* TANK_CHANGE_PROC)(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);
bool CTankPropBank::SetNumProp(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	// 要转成内部tankpropid
	if(dwPropID < 0 || dwPropID >= CREATURE_PROP_MAXID || g_CreatureID2TankPropIDTable[dwPropID] == 0 || pSchemeInfo == NULL)
	{
		return false;
	}
	dwPropID = g_CreatureID2TankPropIDTable[dwPropID];

	/* 1、每个属性都有不同的更新策略（禁止修改、覆盖、增量、比对大小写、非负...等），如果通过if或者switch
	去判定，不仅耗效率，并且代码非常雍仲，用函数指针，使每个属性对应各自的策略。
	2、怪物的属性跟人物属性有两个不同的地方：怪物属性要比人物少得多；更改怪物一个属性不像更改人物一个属性，可能
	会影响一大片，差不多要将所有属性重算一遍，所以在类中，将怪的属性计算都挂在属性上，只要等属性一变，自动计算出所
	相关的属性，这样比一次计算一大片，相对于怪物数是人物40倍数，能提高不少效率，代码看起来整洁//*/
	static TANK_CHANGE_PROC changeproc[TANK_PROP_MAXID] =
	{		
		(TANK_CHANGE_PROC)0,													// 0空位
		(TANK_CHANGE_PROC)0,													// 载具ID
		(TANK_CHANGE_PROC)&CTankPropBank::SetProp_CurHP,						// 当前生命值
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 生命值上限
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前移动速度
		(TANK_CHANGE_PROC)0,													// 面向角度
		(TANK_CHANGE_PROC)&CTankPropBank::Cover_NoNegative,						// 所属国籍
		(TANK_CHANGE_PROC)&CTankPropBank::Cover_NoNegative,						// 座位数

		(TANK_CHANGE_PROC)0,													// 以上为广播属性

		(TANK_CHANGE_PROC)&CTankPropBank::Increment_NoNegative_Compare,			// 当前法术值
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 法术值上限

		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative_Compare,			// 当前最小物理攻击
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前最大物理攻击
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative_Compare,			// 当前最小法术攻击
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前最大法术攻击

		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前物理防御 
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前法术防御
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前火法术防御
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前冰法术防御
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前雷法术防御
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前道法术防御

		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前物理致命概率
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前法术致命概率

		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前命中概率
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前躲闪概率
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前格挡概率
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前穿透概率

		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前物理伤害吸收
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前物理伤害吸收百分比
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前法术伤害吸收
		(TANK_CHANGE_PROC)&CTankPropBank::Forbid_NoNegative,					// 当前法术伤害吸收百分比

		(TANK_CHANGE_PROC)&CTankPropBank::Increment_NoNegative,					// 反弹伤害
		(TANK_CHANGE_PROC)&CTankPropBank::Increment_NoNegative,					// 反致命概率

		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Move_Speed,					// 附加移动速度

		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Max_HP,						// 附加生命值上限
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Max_HP_Per,					// 附加生命值上限百分比
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Max_MP,						// 附加法术值上限
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Max_MP_Per,					// 附加法术值上限百分比

		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_P_A,							// 附加物理攻击（最大最小同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_P_A_Per,						// 附加物理攻击百分比（最大最小同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_M_A,							// 附加法术攻击（最大最小同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_M_A_Per,						// 附加法术攻击百分比（最大最小同时加减）

		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_P_D,							// 附加物理防御（最大最小同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_P_D_Per,						// 附加物理防御百分比（最大最小同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_F_D,							// 附加火法术防御
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_I_D,							// 附加冰法术防御
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_T_D,							// 附加雷法术防御
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_D_D,							// 附加道法术防御
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_M_D,							// 附加法术防御（最大最小同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_M_D_Per,						// 附加法术防御百分比（最大最小同时加减）

		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_PM_Fatal_Per,					// 附加致命概率（物理法术同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Persent_Per,					// 附加命中概率
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Hedge_Per,					// 附加躲闪概率
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Resist_Per,					// 附加格挡概率
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Penetrate_Per,				// 附加穿透概率

		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Absorb_Counter,				// 附加物理伤害吸收百分比
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Absorb_Counter,				// 附加法术伤害吸收百分比
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Absorb_Counter,				// 附加伤害吸收（物理法术同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Absorb_Counter,				// 附加伤害吸收百分比（物理法术同时加减）
		(TANK_CHANGE_PROC)&CTankPropBank::SetPlus_Kick_Fatal_Per,				// 附加反致命概率

																				// 最大属性ID
	};	

	if(changeproc[dwPropID] == NULL)
	{
		TraceLn("通过非法的属性ID修改属性，属性ID = "<<dwPropID<<", 载具名"<<pSchemeInfo->szName);
		return false;
	}

	if(nValue != 0)
	{
		(this->*changeproc[dwPropID])(dwPropID, nValue, bSyncFlag, pSchemeInfo);
	}

	CheckWaitSyncPropBroadcast();

	return true;		
}

/** 禁止修改 非负数
@param   
@param   
@return  
*/
void CTankPropBank::Forbid_NoNegative(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] < 0 ? m_nNumProp[dwPropID] = 0 : 0; 

	// 同步客户端属性
	UpdateClientProp(dwPropID, bSyncFlag);
}

/** 禁止修改 非负数 大小比
@param   
@param   
@return  
*/
void CTankPropBank::Forbid_NoNegative_Compare(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] < 0 ? m_nNumProp[dwPropID] = 0 : (m_nNumProp[dwPropID] > m_nNumProp[dwPropID + 1] ? m_nNumProp[dwPropID] = m_nNumProp[dwPropID + 1] : 0); 

	// 同步客户端属性
	UpdateClientProp(dwPropID, bSyncFlag);
}

/** 增量修改 非负数
@param   
@param   
@return  
*/
void CTankPropBank::Increment_NoNegative(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;	m_nNumProp[dwPropID] < 0 ? m_nNumProp[dwPropID] = 0 : 0;

	// 同步客户端属性
	UpdateClientProp(dwPropID, bSyncFlag);
}

/** 增量修改 非负数 大小比
@param   
@param   
@return  
*/
void CTankPropBank::Increment_NoNegative_Compare(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;
	m_nNumProp[dwPropID] < 0 ? m_nNumProp[dwPropID] = 0 : (m_nNumProp[dwPropID] > m_nNumProp[dwPropID + 1] ? m_nNumProp[dwPropID] = m_nNumProp[dwPropID + 1] : 0);	

	// 同步客户端属性
	UpdateClientProp(dwPropID, bSyncFlag);
}

/** 覆盖修改 非负数
@param   
@param   
@return  
*/
void CTankPropBank::Cover_NoNegative(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	nValue < 0 ? nValue = 0 : 0; m_nNumProp[dwPropID] = nValue;

	// 同步客户端属性
	UpdateClientProp(dwPropID, bSyncFlag);
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Move_Speed(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	if(m_bSolidifyMoveSpeed/*如果固化了移动速度，则不能影响当前移动速度*/)
	{
		return;
	}

	// 当前移动速度 = 脚本配置数值 + 附加移动速度
	m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] = pSchemeInfo->nMoveSpeed + m_nNumProp[dwPropID];
	
	ISchemeCenter * pSchemeCenter = g_ExternalFacade.GetSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return;
	}
	int nMaxMultiple = 0; int nMinMultiple = 0;
	pSchemeCenter->GetMoveMultiple(nMaxMultiple, nMinMultiple);
	if(m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] > nMaxMultiple)
	{
		m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] = nMaxMultiple;
	}

	if(m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] < nMinMultiple)
	{
		m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] = nMinMultiple;
	}

	// 同步客户端属性
	UpdateClientProp(TANK_PROP_CUR_MOVE_SPEED, bSyncFlag);

	// 如果生物正在移动，需要立即响应，否则跟客户端不同步
	IEntityPart * pEntityPart = m_pMaster->GetEntityPart(ENTITYPART_CREATURE_MOVE);
	IMovePart * pMovePart = (IMovePart *)(pEntityPart);
	if(pEntityPart != NULL && pMovePart->IsMoving())
	{
		pMovePart->OnMoveSpeedChanged();
	}
}	

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Max_HP(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 生命值上限 = 脚本配置生命值 + 附加生命值上限 + (脚本配置生命值 * 附加生命值上限百分比)
	m_nNumProp[TANK_PROP_MAX_HP] = pSchemeInfo->lHP + m_nNumProp[dwPropID] + pSchemeInfo->lHP * float(m_nNumProp[TANK_PROP_PLUS_MAX_HP_PER]/ZOOMIN_MULTIPLE/*放大比率比*/);
	m_nNumProp[TANK_PROP_MAX_HP] < 0 ? m_nNumProp[TANK_PROP_MAX_HP] = 0 : 0;

	bool bChange = true;
	m_nNumProp[TANK_PROP_CUR_HP] > m_nNumProp[TANK_PROP_MAX_HP] ? m_nNumProp[TANK_PROP_CUR_HP] = m_nNumProp[TANK_PROP_MAX_HP] : bChange = false;

	// 一定要放在同步TANK_PROP_MAX_HP之前，否则又会出现在最大血量小于最小血量
	if(bChange)
	{
		UpdateClientProp(TANK_PROP_CUR_HP, bSyncFlag);
	}

	// 同步客户端属性
	UpdateClientProp(TANK_PROP_MAX_HP, bSyncFlag);
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Max_HP_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 生命值上限 = 脚本配置生命值 + 附加生命值上限 + (脚本配置生命值 * 附加生命值上限百分比) 
	m_nNumProp[TANK_PROP_MAX_HP] = pSchemeInfo->lHP + m_nNumProp[TANK_PROP_PLUS_MAX_HP] + pSchemeInfo->lHP * float(m_nNumProp[dwPropID]/ZOOMIN_MULTIPLE/*放大比率比*/);
	m_nNumProp[TANK_PROP_MAX_HP] < 0 ? m_nNumProp[TANK_PROP_MAX_HP] = 0 : 0;

	bool bChange = true;
	m_nNumProp[TANK_PROP_CUR_HP] > m_nNumProp[TANK_PROP_MAX_HP] ? m_nNumProp[TANK_PROP_CUR_HP] = m_nNumProp[TANK_PROP_MAX_HP] : bChange = false;

	// 一定要放在同步TANK_PROP_MAX_HP之前，否则又会出现在最大血量小于最小血量
	if(bChange)
	{
		UpdateClientProp(TANK_PROP_CUR_HP, bSyncFlag);
	}

	// 同步客户端属性
	UpdateClientProp(TANK_PROP_MAX_HP, bSyncFlag);
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Max_MP(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 法术值上限 = 脚本配置法术值  + 附加法术值上限 + (脚本配置法术值 * 附加法术值上限百分比)
	m_nNumProp[TANK_PROP_MAX_MP] = pSchemeInfo->lMP + m_nNumProp[dwPropID] + pSchemeInfo->lMP * float(m_nNumProp[TANK_PROP_PLUS_MAX_MP_PER]/ZOOMIN_MULTIPLE/*放大比率比*/);
	m_nNumProp[TANK_PROP_MAX_MP] < 0 ? m_nNumProp[TANK_PROP_MAX_MP] = 0 : 0;

	bool bChange = true;
	m_nNumProp[TANK_PROP_CUR_MP] > m_nNumProp[TANK_PROP_MAX_MP] ? m_nNumProp[TANK_PROP_CUR_MP] = m_nNumProp[TANK_PROP_MAX_MP] : bChange = false;

	// 一定要放在同步TANK_PROP_MAX_MP之前，否则又会出现在最大血量小于最小血量
	if(bChange)
	{
		UpdateClientProp(TANK_PROP_CUR_MP, bSyncFlag);
	}	

	// 同步客户端属性
	UpdateClientProp(TANK_PROP_MAX_MP, bSyncFlag);
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Max_MP_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 法术值上限 = 脚本配置法术值  + 附加法术值上限 + (脚本配置法术值 * 附加法术值上限百分比)
	m_nNumProp[TANK_PROP_MAX_MP] = pSchemeInfo->lMP + m_nNumProp[TANK_PROP_PLUS_MAX_MP] + pSchemeInfo->lMP * float(m_nNumProp[dwPropID]/ZOOMIN_MULTIPLE/*放大比率比*/);
	m_nNumProp[TANK_PROP_MAX_MP] < 0 ? m_nNumProp[TANK_PROP_MAX_MP] = 0 : 0;

	bool bChange = true;
	m_nNumProp[TANK_PROP_CUR_MP] > m_nNumProp[TANK_PROP_MAX_MP] ? m_nNumProp[TANK_PROP_CUR_MP] = m_nNumProp[TANK_PROP_MAX_MP] : bChange = false;

	// 一定要放在同步TANK_PROP_MAX_MP之前，否则又会出现在最大血量小于最小血量
	if(bChange)
	{
		UpdateClientProp(TANK_PROP_CUR_MP, bSyncFlag);
	}

	// 同步客户端属性
	UpdateClientProp(TANK_PROP_MAX_MP, bSyncFlag);
}	

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_P_A(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前最小物理攻击 = 脚本配置最小物理攻击 + (脚本配置最小物理攻击 * 附加物理攻击百分比) +附加物理攻击
	m_nNumProp[TANK_PROP_CUR_MIN_P_A] = pSchemeInfo->lMinPAttack + pSchemeInfo->lMinPAttack * float(m_nNumProp[TANK_PROP_PLUS_P_A_PER]/ZOOMIN_MULTIPLE/*放大比率比*/)+ m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_MIN_P_A] < 0 ? m_nNumProp[TANK_PROP_CUR_MIN_P_A] = 0 : 0;

	// 当前最大物理攻击 = 脚本配置最大物理攻击 + (脚本配置最大物理攻击 * 附加物理攻击百分比) +附加物理攻击
	m_nNumProp[TANK_PROP_CUR_MAX_P_A] = pSchemeInfo->lMaxPAttack + pSchemeInfo->lMaxPAttack * float(m_nNumProp[TANK_PROP_PLUS_P_A_PER]/ZOOMIN_MULTIPLE/*放大比率比*/)+ m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_MAX_P_A] < 0 ? m_nNumProp[TANK_PROP_CUR_MAX_P_A] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_P_A_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前最小物理攻击 = 脚本配置最小物理攻击 + (脚本配置最小物理攻击 * 附加物理攻击百分比) +附加物理攻击
	m_nNumProp[TANK_PROP_CUR_MIN_P_A] = pSchemeInfo->lMinPAttack + pSchemeInfo->lMinPAttack * float(m_nNumProp[dwPropID]/ZOOMIN_MULTIPLE/*放大比率比*/) + m_nNumProp[TANK_PROP_PLUS_P_A];
	m_nNumProp[TANK_PROP_CUR_MIN_P_A] < 0 ? m_nNumProp[TANK_PROP_CUR_MIN_P_A] = 0 : 0;

	// 当前最大物理攻击 = 脚本配置最大物理攻击 + (脚本配置最大物理攻击 * 附加物理攻击百分比) +附加物理攻击
	m_nNumProp[TANK_PROP_CUR_MAX_P_A] = pSchemeInfo->lMaxPAttack + pSchemeInfo->lMaxPAttack * float(m_nNumProp[dwPropID]/ZOOMIN_MULTIPLE/*放大比率比*/) + m_nNumProp[TANK_PROP_PLUS_P_A];
	m_nNumProp[TANK_PROP_CUR_MAX_P_A] < 0 ? m_nNumProp[TANK_PROP_CUR_MAX_P_A] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_M_A(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前最小法术攻击 = 脚本配置最小法术攻击 + (脚本配置最小法术攻击 * 附加法术攻击百分比) +附加法术攻击
	m_nNumProp[TANK_PROP_CUR_MIN_M_A] = pSchemeInfo->lMinMAttack + pSchemeInfo->lMinMAttack * float(m_nNumProp[TANK_PROP_PLUS_M_A_PER]/ZOOMIN_MULTIPLE/*放大比率比*/) + m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_MIN_M_A] < 0 ? m_nNumProp[TANK_PROP_CUR_MIN_M_A] = 0 : 0;

	// 当前最大法术攻击 = 脚本配置最大法术攻击 + (脚本配置最大法术攻击 * 附加法术攻击百分比) +附加法术攻击
	m_nNumProp[TANK_PROP_CUR_MAX_M_A] = pSchemeInfo->lMaxMAttack + pSchemeInfo->lMaxMAttack * float(m_nNumProp[TANK_PROP_PLUS_M_A_PER]/ZOOMIN_MULTIPLE/*放大比率比*/) + m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_MAX_M_A] < 0 ? m_nNumProp[TANK_PROP_CUR_MAX_M_A] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_M_A_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前最小法术攻击 = 脚本配置最小法术攻击 + (脚本配置最小法术攻击 * 附加法术攻击百分比) +附加法术攻击
	m_nNumProp[TANK_PROP_CUR_MIN_M_A] = pSchemeInfo->lMinMAttack + pSchemeInfo->lMinMAttack * float(m_nNumProp[dwPropID]/ZOOMIN_MULTIPLE/*放大比率比*/)+ m_nNumProp[TANK_PROP_PLUS_M_A];
	m_nNumProp[TANK_PROP_CUR_MIN_M_A] < 0 ? m_nNumProp[TANK_PROP_CUR_MIN_M_A] = 0 : 0;

	// 当前最大法术攻击 = 脚本配置最大法术攻击 + (脚本配置最大法术攻击 * 附加法术攻击百分比) +附加法术攻击
	m_nNumProp[TANK_PROP_CUR_MAX_M_A] = pSchemeInfo->lMaxMAttack + pSchemeInfo->lMaxMAttack * float(m_nNumProp[dwPropID]/ZOOMIN_MULTIPLE/*放大比率比*/)+ m_nNumProp[TANK_PROP_PLUS_M_A];
	m_nNumProp[TANK_PROP_CUR_MAX_M_A] < 0 ? m_nNumProp[TANK_PROP_CUR_MAX_M_A] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_P_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前物理防御 ＝ 脚本配置物理防御 + (脚本配置物理防御 * 附加物理防御百分比) + 附加物理防御
	m_nNumProp[TANK_PROP_CUR_P_D] = pSchemeInfo->lPDefend + pSchemeInfo->lPDefend * float(m_nNumProp[TANK_PROP_PLUS_P_D_PER]/ZOOMIN_MULTIPLE/*放大比率比*/) + m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_P_D] < 0 ? m_nNumProp[TANK_PROP_CUR_P_D] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_P_D_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前物理防御 ＝ 脚本配置物理防御 + (脚本配置物理防御 * 附加物理防御百分比) + 附加物理防御
	m_nNumProp[TANK_PROP_CUR_P_D] = pSchemeInfo->lPDefend + pSchemeInfo->lPDefend * float(m_nNumProp[dwPropID]/ZOOMIN_MULTIPLE/*放大比率比*/) + m_nNumProp[TANK_PROP_PLUS_P_D];
	m_nNumProp[TANK_PROP_CUR_P_D] < 0 ? m_nNumProp[TANK_PROP_CUR_P_D] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_F_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	//  当前火法术防御 ＝ 当前法术防御 + 脚本配置火法术防御 + 附加火法术防御
	m_nNumProp[TANK_PROP_CUR_F_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lFDefend + m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_F_M_D] < 0 ? m_nNumProp[TANK_PROP_CUR_F_M_D] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_I_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前冰法术防御 ＝ 当前法术防御 + 脚本配置冰法术防御 + 附加冰法术防御
	m_nNumProp[TANK_PROP_CUR_I_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lIDefend + m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_I_M_D] < 0 ? m_nNumProp[TANK_PROP_CUR_I_M_D] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_T_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前雷法术防御 ＝ 当前法术防御 + 脚本配置雷法术防御 + 附加雷法术防御
	m_nNumProp[TANK_PROP_CUR_T_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lTDefend + m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_T_M_D] < 0 ? m_nNumProp[TANK_PROP_CUR_T_M_D] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_D_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前道法术防御 ＝ 当前法术防御 + 脚本配置道法术防御 + 附加道法术防御
	m_nNumProp[TANK_PROP_CUR_D_M_D] = m_nNumProp[TANK_PROP_CUR_M_D] + pSchemeInfo->lDDefend + m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_D_M_D] < 0 ? m_nNumProp[TANK_PROP_CUR_D_M_D] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_M_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前法术防御 ＝ 脚本配置法术防御 + (脚本配置法术防御 * 附加法术防御百分比) + 附加法术防御
	m_nNumProp[TANK_PROP_CUR_M_D] = pSchemeInfo->lMDefend + pSchemeInfo->lMDefend * float(m_nNumProp[TANK_PROP_PLUS_M_D_PER]/ZOOMIN_MULTIPLE/*放大比率比*/) + m_nNumProp[dwPropID];
	m_nNumProp[TANK_PROP_CUR_M_D] < 0 ? m_nNumProp[TANK_PROP_CUR_M_D] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_M_D_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前法术防御 ＝ 脚本配置法术防御 + (脚本配置法术防御 * 附加法术防御百分比) + 附加法术防御
	m_nNumProp[TANK_PROP_CUR_M_D] = pSchemeInfo->lMDefend + pSchemeInfo->lMDefend * float(m_nNumProp[dwPropID]/ZOOMIN_MULTIPLE/*放大比率比*/) + m_nNumProp[TANK_PROP_PLUS_M_D];
	m_nNumProp[TANK_PROP_CUR_M_D] < 0 ? m_nNumProp[TANK_PROP_CUR_M_D] = 0 : 0;
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_PM_Fatal_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前物理致命概率 = 脚本配置物理致命概率 + 附加致命概率
	m_nNumProp[TANK_PROP_CUR_P_FATAL_PER] = pSchemeInfo->lPFatalPer + m_nNumProp[dwPropID];

	// 当前法术致命概率 = 脚本配置法术致命概率 + 附加致命概率
	m_nNumProp[TANK_PROP_CUR_M_FATAL_PER] = pSchemeInfo->lMFatalPer + m_nNumProp[dwPropID];
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Persent_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前命中概率 = 脚本配置命中概率 + 附加命中概率
	m_nNumProp[TANK_PROP_CUR_PRESENT_PER] = pSchemeInfo->lPresentPer + m_nNumProp[dwPropID];
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Hedge_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前躲闪概率 = 脚本配置躲闪概率 + 附加躲闪概率
	m_nNumProp[TANK_PROP_CUR_HEDGE_PER] = pSchemeInfo->lHedgePer + m_nNumProp[dwPropID];
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Resist_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前格挡概率 = 脚本配置格挡概率 + 附加格挡概率
	m_nNumProp[TANK_PROP_CUR_RESIST_PER] = pSchemeInfo->lResistPer + m_nNumProp[dwPropID];
}

/** 
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Penetrate_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	m_nNumProp[dwPropID] += nValue;

	// 当前穿透概率 = 脚本配置穿透概率 + 附加穿透概率
	m_nNumProp[TANK_PROP_CUR_PENETRATE_PER] = pSchemeInfo->lPenetratePer + m_nNumProp[dwPropID];
}
//
///** 
//@param   
//@param   
//@return  
//*/
//void CTankPropBank::SetPlus_P_Absorb_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
//{
//	m_nNumProp[dwPropID] += nValue;
//}
//
///** 
//@param   
//@param   
//@return  
//*/
//void CTankPropBank::SetPlus_M_Absorb_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
//{
//	m_nNumProp[dwPropID] += nValue;
//}
//
///** 
//@param   
//@param   
//@return  
//*/
//void CTankPropBank::SetPlus_PM_Absorb(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
//{
//	m_nNumProp[dwPropID] += nValue;
//}
//
///** 
//@param   
//@param   
//@return  
//*/
//void CTankPropBank::SetPlus_PM_Absorb_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
//{
//	m_nNumProp[dwPropID] += nValue;
//}
//
///** 
//@param   
//@param   
//@return  
//*/
//void CTankPropBank::SetPlus_Rebound_Damage(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
//{
//	m_nNumProp[dwPropID] += nValue;
//}
//
///** 
//@param   
//@param   
//@return  
//*/
//void CTankPropBank::SetPlus_Kick_Fatal_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
//{
//	m_nNumProp[dwPropID] += nValue;
//}

/** 物理、法术伤害吸收
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Absorb_Counter(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	// 先把属性修改
	m_nNumProp[dwPropID] += nValue;

	// 当前物理伤害吸收 = 附加伤害吸收（物理法术同时加减）
	m_nNumProp[TANK_PROP_CUR_P_ABSORB] = m_nNumProp[TANK_PROP_PLUS_PM_ABSORB];

	// 当前物理伤害吸收百分比 = 附加物理伤害吸收百分比 + 附加伤害吸收百分比（物理法术同时加减）
	m_nNumProp[TANK_PROP_CUR_P_ABSORB_PER] = m_nNumProp[TANK_PROP_PLUS_P_ABSORB_PER] + m_nNumProp[TANK_PROP_PLUS_PM_ABSORB_PER];

	// 当前法术伤害吸收 = 附加伤害吸收（物理法术同时加减）
	m_nNumProp[TANK_PROP_CUR_M_ABSORB] = m_nNumProp[TANK_PROP_PLUS_PM_ABSORB];

	// 当前法术伤害吸收百分比 = 附加法术伤害吸收百分比 + 附加伤害吸收百分比（物理法术同时加减）
	m_nNumProp[TANK_PROP_CUR_M_ABSORB_PER] = m_nNumProp[TANK_PROP_PLUS_M_ABSORB_PER] + m_nNumProp[TANK_PROP_PLUS_PM_ABSORB_PER];
}


/** 附加反致命概率
@param   
@param   
@return  
*/
void CTankPropBank::SetPlus_Kick_Fatal_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	// 先把属性修改
	Increment_NoNegative(dwPropID, nValue, bSyncFlag, pSchemeInfo);

	// 当前反致命概率 = 附加反致命概率
	m_nNumProp[TANK_PROP_CUR_KICK_FATAL_PER] = m_nNumProp[TANK_PROP_PLUS_KICK_FATAL_PER];
}

/** 同步客户端属性
@param   
@param   
@return  
*/
void CTankPropBank::UpdateClientProp(DWORD dwPropID, bool bSyncFlag)
{
	if(!bSyncFlag || dwPropID <= 0 || dwPropID >= TANK_PROP_BROADCAST)
	{
		return;
	}

	obuf1024 obuf;
	SGameMsgHead * pHead = (SGameMsgHead *)obuf.data();
	obuf.offset(sizeof(SGameMsgHead));
	pHead->SrcEndPoint = MSG_ENDPOINT_ZONE;
	pHead->DestEndPoint = MSG_ENDPOINT_CLIENT;
	pHead->wKeyModule = MSG_MODULEID_PROP;
	pHead->wKeyAction = MSG_PROP_UPDATEPROP;

	SMsgPropUpdateProp_SC * pMidHead = (SMsgPropUpdateProp_SC *)obuf.current();
	obuf.offset(sizeof(SMsgPropUpdateProp_SC));
	pMidHead->uidEntity = m_uid;
	pMidHead->nUpdateMode = 1;			// 单个数值属性更新

	// 此处要转成生物ID，tankpropid不能对外使用。
	BYTE byPropID = (BYTE)g_TankPropID2CreatureIDTable[dwPropID];
	obuf.push_back(&byPropID, sizeof(BYTE));
	obuf.push_back(&m_nNumProp[dwPropID], sizeof(m_nNumProp[dwPropID]));

	m_pGameZone->BroadcastOptimize(m_pMaster, (LPCSTR)obuf.data(), obuf.size());
}

/** 同步客户端名字
@param   
@param   
@return  
*/
void CTankPropBank::UpdateClientTitle(void)
{
	SGameMsgHead gamemsghead;
	gamemsghead.SrcEndPoint = MSG_ENDPOINT_ZONE;
	gamemsghead.DestEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.wKeyModule = MSG_MODULEID_PROP;
	gamemsghead.wKeyAction = MSG_PROP_UPDATEPROP;

	SMsgPropUpdateProp_SC msgpropupdatepropsc;
	msgpropupdatepropsc.uidEntity  = m_uid;
	msgpropupdatepropsc.nUpdateMode = 3;// 单个数值属性更新

	obuf1024 obuf;
	obuf.push_back(&gamemsghead, sizeof(SGameMsgHead));
	obuf.push_back(&msgpropupdatepropsc, sizeof(SMsgPropUpdateProp_SC));

	// 此处临时用一个属性ID
	BYTE byNamePropID = CREATURE_PROP_TITLE;
	obuf.push_back(&byNamePropID, sizeof(BYTE));
	obuf.push_back(&m_szTitle, sizeof(m_szTitle));

	m_pGameZone->BroadcastOptimize(m_pMaster, (LPCSTR)obuf.data(), obuf.size());	
}

/** 固化移动速度
@param   nMoveSpeed ：0（表示取消固化移动速度），其他表示将当前移动速度固化在第几档
@param   
@return  
*/
void CTankPropBank::SolidifyMoveSpeed(int nMoveSpeed)
{
	if(nMoveSpeed == 0)
	{
		m_bSolidifyMoveSpeed = false;

		SetNumProp(CREATURE_PROP_PLUS_MOVE_SPEED, 0, true, m_pMaster->GetSchemeInfo());
	}
	else
	{		
		m_bSolidifyMoveSpeed = true;
		if(m_nNumProp[TANK_PROP_PLUS_MOVE_SPEED] == nMoveSpeed)
		{
			return;
		}

		// 当前移动速度 = 脚本配置数值 + 附加移动速度
		m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] = nMoveSpeed;

		ISchemeCenter * pSchemeCenter = g_ExternalFacade.GetSchemeCenter();
		if(pSchemeCenter == NULL)
		{
			return;
		}
		int nMaxMultiple = 0; int nMinMultiple = 0;
		pSchemeCenter->GetMoveMultiple(nMaxMultiple, nMinMultiple);
		if(m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] > nMaxMultiple)
		{
			m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] = nMaxMultiple;
		}

		if(m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] < nMinMultiple)
		{
			m_nNumProp[TANK_PROP_CUR_MOVE_SPEED] = nMinMultiple;
		}	

		// 同步客户端属性
		UpdateClientProp(TANK_PROP_CUR_MOVE_SPEED, true);
	}
}

/** 当前血量
@param   
@param   
@return  
*/
void CTankPropBank::SetProp_CurHP(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo)
{
	int nOldPropValue = m_nNumProp[dwPropID]; m_nNumProp[dwPropID] += nValue;
	m_nNumProp[dwPropID] < 0 ? m_nNumProp[dwPropID] = 0 : (m_nNumProp[dwPropID] > m_nNumProp[dwPropID + 1] ? m_nNumProp[dwPropID] = m_nNumProp[dwPropID + 1] : 0);

	if(!bSyncFlag || nOldPropValue == m_nNumProp[dwPropID])
	{
		return;
	}

	ISchemeCenter * pSchemeCenter = g_ExternalFacade.GetSchemeCenter();
	if(pSchemeCenter == NULL || m_pGameZone == NULL)
	{
		return;
	}

	SBOMInfo * pBOMInfo = pSchemeCenter->GetBOMInfoForMap(m_pGameZone->GetMapID());
	if(pBOMInfo == NULL)
	{
		return;
	}

	bool bLaunchBOFlag = false;
	if(pBOMInfo->bOpenFlag)
	{
		int n9GridPersonQty = m_pMaster->Get9GridPersonQty();
		if(n9GridPersonQty >= pBOMInfo->nPropSync_OptLine)
		{
			bLaunchBOFlag = true;
		}
	}

	if(bLaunchBOFlag)
	{
		if(m_dwWaitSyncHPStartTick == 0/*启动延时刷新血量*/)
		{
			m_dwWaitSyncHPStartTick = ::GetTickCount();
		}
	}
	else
	{
		obuf1024 obuf;			
		FILLNORMAL_TOCLIENT_MSG(obuf, MSG_MODULEID_PROP, MSG_PROP_SYNCHP, NULL, 0);
		SMsgPropSyncHP_SC * pMsg = (SMsgPropSyncHP_SC *)obuf.current();
		obuf.offset(sizeof(SMsgPropSyncHP_SC));
		pMsg->uidEntity = m_uid;
		pMsg->nCurHP	= GetNumProp(CREATURE_PROP_CUR_HP);	
		m_pGameZone->BroadcastOptimize(m_pMaster, (LPCSTR)obuf.data(), obuf.size());

		m_dwWaitSyncHPStartTick = 0; 	// 是否启动了延时刷新血量
	}
}

/** 检查是否需延时刷新血量
@param   
@param   
@return  
*/
void CTankPropBank::CheckWaitSyncPropBroadcast(void)
{
	if(m_dwWaitSyncHPStartTick == 0)
	{
		return;
	}

	if(GetTickCount() - m_dwWaitSyncHPStartTick < 1000)
	{
		return;
	}

	IZoneManager * pZoneManager = g_ExternalFacade.GetZoneManager();
	if(pZoneManager == NULL)
	{
		return;
	}

	IGameZone * pGameZone = pZoneManager->GetGameZone(GetZoneID());
	if(pGameZone == NULL)
	{
		return;
	}

	obuf1024 obuf;			
	FILLNORMAL_TOCLIENT_MSG(obuf, MSG_MODULEID_PROP, MSG_PROP_SYNCHP, NULL, 0);
	SMsgPropSyncHP_SC * pMsg = (SMsgPropSyncHP_SC *)obuf.current();
	obuf.offset(sizeof(SMsgPropSyncHP_SC));
	pMsg->uidEntity = m_uid;
	pMsg->nCurHP	= GetNumProp(CREATURE_PROP_CUR_HP);
	pGameZone->BroadcastOptimize(m_pMaster, (LPCSTR)obuf.data(), obuf.size());
}

/** 取载具等级
@param   
@param   
@return  
*/
long CTankPropBank::GetTankLevel(void)
{
	return m_lLevel;
}

/** 获得所在场景对像
@param   
@param   
@return  
*/
IGameZone * CTankPropBank::GetGameZone(void)
{
	return m_pGameZone;
}

/** 取得扩展数据
@param   
@param   
@return  
*/
LPCSTR CTankPropBank::GetExtendData(void)
{
	return m_szExtendData;
}

/** 设置扩展数据
@param   
@param   
@return  
*/
bool CTankPropBank::SetExtendData(LPCSTR szExtendData, int nLen)
{
	if (nLen > TRANSMAP_MAX_EXTEND_LEN)
	{
		return false;
	}

	memcpy(m_szExtendData, szExtendData, nLen);

	return true;
}