/*******************************************************************
** 文件名:	GameAPI.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/11/2014
** 版  本:	1.0
** 描  述:	C++导出接口
			
********************************************************************/

#include "stdafx.h"
#include "GameAPI.h"
#include "ClientGlobal.h"
#include "IEntityClient.h"
#include "IClientEntity.h"
#include "RenderTaskMgr.h"
#include "LogicTask.h"
#include "Thread.h"
#include "IPathFindService.h"
#include "ConfigIni.h"
#include "LanguageVer.h"
#include "IBankPart.h"
#include "ISlotLogicPart.h"
#include "ActorClient.h"
#include "IWarClient.h"
#include "FileReader.h"
//#include "IGameDatabaseDef.h"
#include "WindowsShotcutHelper.h"
#include "HeightFieldMgr.h"
#include "DistrictList.h"
#include "Game_ManagedDef.h"
#include "ExternalHelper.h"
#include <map>

static bool	g_bMultiThread = false;

static ThreadPool* g_pThreadPool = NULL;


//// API函数 //////////////////////////////////////////////////////////////////////

// 获取主窗口句柄
GAME_API HWND getMainWindHwnd()
{
    return g_MainWnd;
}

/**
@name   客户端开始log
*/
GAME_API void OpenLog()
{
	CClientGlobal::getInstance().openLog();

    CLogicTask::getInstance().clear();
}

GAME_API void Start(void* pRenderEntry)
{
    rkt::Ini config("config.ini");

    g_bMultiThread = ( config.getInt( "App","EnableMultiThread", 1 ) == 1);

    if(g_bMultiThread)
    {
        WarningLn("enable multithread = true");
    }
    else
    {
        WarningLn("enable mutithread = false");
    }

    if(g_bMultiThread && g_pThreadPool == NULL)
    {
        g_pThreadPool = new ThreadPool();
    }

    CRenderTaskMgr::getInstance().create(pRenderEntry);


    // 如果开启多线程 逻辑线程设置自动开启
    if(g_bMultiThread)
    {
        // 逻辑线程中要照顾定时器和网络收包问题
        CLogicTask::getInstance().setTimeSlice(20);

        CLogicTask::getInstance().autoRun(g_pThreadPool);
    }

    // Disable when full screen
    CWindowsShotcutHelper::getInstance().AllowAccessibilityShortcutKeys(false);
}

GAME_API void Stop()
{
    // Restore back when going to windowed or shutting down
    CWindowsShotcutHelper::getInstance().AllowAccessibilityShortcutKeys(true);

    CRenderTaskMgr::getInstance().stop();
    CLogicTask::getInstance().stop();

    if(g_pThreadPool)
    {
        g_pThreadPool->wait();
        g_pThreadPool->clear();
        delete g_pThreadPool;
        g_pThreadPool = NULL;
    }
    else
    {
         CLogicTask::getInstance().update();      // 单线程 驱动产生onstop消息
    }

    CRenderTaskMgr::getInstance().release();
}

GAME_API void onFrameUpdate()
{
    CRenderTaskMgr::getInstance().update();

    if(!g_bMultiThread)
    {
        CLogicTask::getInstance().update();
    }
}

/** 接受实体显示层发出的事件
@param entityID   : 实体ID
@param eventID    : 事件ID,具体定义见(EntityViewDef.h)
@param arg1       : 时间参数1
@param arg2       : 时间参数2
@return           : 是否接收成功
*/
GAME_API void onEntityEvent( ENTITY_ID entityID,int32 eventID,int nParam,const char * strParam,void * ptrParam, int nLen)
{
    CLogicTask::getInstance().OnEntityEvent(entityID, eventID, nParam, strParam, ptrParam, nLen);
}

/** 接受显示层发出的事件
@param entityID   : 实体ID
@param eventID : 事件ID,具体定义见(GameViewDef.h)
@param arg1    : 时间参数1
@param arg2    : 时间参数2
@return        : 是否接收成功
*/
GAME_API void onGameViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam, int nLen)
{
    CLogicTask::getInstance().OnGameViewEvent(eventID, nParam, strParam, ptrParam, nLen);
}

/** 输出调试信息
*/
GAME_API void traceLog(const char* msg,int32 level)
{
    CLogicTask::getInstance().traceLog(msg, level);
}

// 获得当前Tick数
GAME_API int32 getSyncTickCount()
{
    if(gClientGlobal == 0)
    {
        ErrorLn(__FUNCTION__ << ", gClientGlobal == 0");
        return 0;
    }
	return gClientGlobal->getTimeSyncer()->GetTick();
}

// 获取C风格字符串长度
GAME_API int getStringLen(const char* str)
{
	return strlen(str) + 1;
}

static IMinimapScheme* getMinimapScheme()
{
    if(gClientGlobal == NULL)
    {
        ErrorLn("getMinimapScheme gClientGlobal == NULL");
        return 0;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        ErrorLn("getMinimapScheme pSchemeCenter == NULL");
        return 0;
    }

    return pSchemeCenter->getSchemeMinimap();
}

// 获取小地图信息
GAME_API void* getMinimapInfo(int nMapID)
{
    IMinimapScheme* pMinimapScheme = getMinimapScheme();
    if(pMinimapScheme == NULL)
    {
        ErrorLn("getMinimapInfo pMinimapScheme == NULL");
        return 0;
    }

    return pMinimapScheme->GetMinimapInfo(nMapID);
}

// 获取小地图图标信息
GAME_API void* getMinimapIconInfo(int nIconID)
{
    IMinimapScheme* pMinimapScheme = getMinimapScheme();
    if(pMinimapScheme == NULL)
    {
        ErrorLn("getMinimapIconInfo pMinimapScheme == NULL");
        return 0;
    }

    return pMinimapScheme->GetMinimapIconInfo(nIconID);
}

// 根据地图ID 静态打点序号获得静态打点信息
GAME_API void* getMinimapStaticObjInfo(int nMapID, int nIndex)
{
    IMinimapScheme* pMinimapScheme = getMinimapScheme();
    if(pMinimapScheme == NULL)
    {
        ErrorLn("getMinimapStaticObjInfo pMinimapScheme == NULL");
        return 0;
    }

    return pMinimapScheme->GetMinimapStaticIcon(nMapID, nIndex);
}

// 根据序号，获取下一个静态打点信息
GAME_API void* getNextMinimapStaticIcon(int nMapID, int nFromIndex)
{
    IMinimapScheme* pMinimapScheme = getMinimapScheme();
    if(pMinimapScheme == NULL)
    {
        ErrorLn("getNextMinimapStaticIcon pMinimapScheme == NULL");
        return 0;
    }

    return pMinimapScheme->GetNextMinimapStaticIcon(nMapID, nFromIndex);
}

// 获取角色后退速度因子
GAME_API float getHeroBackwardSpeedFactor(void)
{
    if(gClientGlobal == NULL)
    {
        return 1.0;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return 1.0;
    }

    ISchemeEntity* pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
    if(pSchemeEntity == NULL)
    {
        return 1.0;
    }

    return pSchemeEntity->getEntityXml()->fHeroBackwardSpeedFactor;
}

// 获取角色左右移动速度因子
GAME_API float getHeroSidewaySpeedFactor(void)
{
    if(gClientGlobal == NULL)
    {
        return 1.0;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return 1.0;
    }

    ISchemeEntity* pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
    if(pSchemeEntity == NULL)
    {
        return 1.0;
    }

    return pSchemeEntity->getEntityXml()->fHeroSidewaySpeedFactor;
}

// 获取角色天赋最大可使用数
GAME_API int getHeroTalentMaxUseCount(void)
{
    if(gClientGlobal == NULL)
    {
        return 0;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return 0;
    }

    ISchemeEntity* pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
    if(pSchemeEntity == NULL)
    {
        return 0;
    }

    return pSchemeEntity->getEntityXml()->nTalentMaxUseCount;
}


// 获取xp技能充能时间
GAME_API int getXpSkillChargeTime(void)
{
	if(gClientGlobal == NULL)
	{
		return 0;
	}

	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeEntity* pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
	if(pSchemeEntity == NULL)
	{
		return 0;
	}

	return pSchemeEntity->getEntityXml()->nXPSkillTime;
}

// 获取网络延迟
GAME_API int getNetLatency(void)
{
    return gClientGlobal ? gClientGlobal->getNetLatency(): 0;
}

// 获取网络包
GAME_API int getPendingCount(void)
{
    return gClientGlobal ? gClientGlobal->getPendingCount(): 0;
}


// 获取路径
GAME_API bool findPath(void* pContext)
{
    if(gClientGlobal == 0)
    {
        ErrorLn(__FUNCTION__ << ", gClientGlobal == 0");
        return false;
    }

    Stage_Manager* pStageMgr = gClientGlobal->getStageManager();
    if(pStageMgr == NULL)
    {
        ErrorLn(__FUNCTION__ << ", pStageMgr == NULL");
        return false;
    }

    Stage_Base* pStage = pStageMgr->getState(pStageMgr->getCurState());
    if(pStage == NULL)
    {
        ErrorLn(__FUNCTION__ << ", pStage == NULL");
        return false;
    }

    return pStage->findPath(pContext);
}

// 获取地面高度 最底下的一层可走层
GAME_API float getGroundLayer(float fPosX, float fPosZ, float fAgentHeight)
{
    if(gClientGlobal == 0)
    {
        ErrorLn(__FUNCTION__ << ", gClientGlobal == 0");
        return -10000;
    }

    Stage_Manager* pStageMgr = gClientGlobal->getStageManager();
    if(pStageMgr == NULL)
    {
        ErrorLn(__FUNCTION__ << ", pStageMgr == NULL");
        return -10000;
    }

    EStage_State eCurStage = pStageMgr->getCurState(); 
    Stage_Base* pStage = pStageMgr->getState(eCurStage);
    if(pStage == NULL)
    {
        ErrorLn(__FUNCTION__ << ", pStage == NULL");
        return -10000;
    }

    if(eCurStage != ESS_GAME && eCurStage != ESS_BATTLE)
    {
        return -10000;
    }

    Vector3 loc(fPosX, -10000, fPosZ);
    if(gClientGlobal->getHeightFieldMgr()->correctPosFromPhysic(ActorClient::getInstance().getMapID(), loc, 0, 0))
    {
        return loc.y;
    }

    return -10000;
}

GAME_API int getSight(int nHeroId)
{
    if(gClientGlobal == NULL)
    {
        ErrorLn("getMinimapScheme gClientGlobal == NULL");
        return 0;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        ErrorLn("getMinimapScheme pSchemeCenter == NULL");
        return 0;
    }

    ISchemePersonModel*	pPersonScheme = pSchemeCenter->getSchemePersonModel();
    if ( pPersonScheme == NULL )
    {
        ErrorLn("getSchemePersonModel pPersonModel == NULL");
        return 0;
    }

    const PersonSkinModelItem* pItem = pPersonScheme->getPersonSkinModelItem(nHeroId);

    int nSight = pItem->nSightArray[0];

    IMiscManager* pMiscMgr = gClientGlobal->getMiscManager();
    if(pMiscMgr != NULL)
    {
        int nMapID = pMiscMgr->getMapID();   
        if(nMapID >= 0 && nMapID < MAP_MAXCOUNT && pItem->nSightArray[nMapID] != 0)
        {
            nSight = pItem->nSightArray[nMapID]; 
        }
    }

    return nSight;
}

// 检测FPS输出阀值，0将不输出
GAME_API int getAlertFPS(void)
{
    return ConfigIni::getInstance().getAlertFPS();
}

// 检测FPS统计均值间隔ms，默认1000，0.将不输出
GAME_API int getFPSInterVal(void)
{
    return ConfigIni::getInstance().getFPSInterVal();
}

// 获取语言目录名 
GAME_API const char* getLanguageDirName(void)
{
    return CLanguageVer::getInstance().getDirName();
}

// 获得可被秒杀时的血条颜色
GAME_API int getCanseckillHpColor( int nMonsterId )
{

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        ErrorLn("getCanseckillHpColor pSchemeCenter == NULL");
        return 0;
    }
    SMonsterScheme* pInfo = pSchemeCenter->getSchemeMonster()->getMonsterShemeInfo(nMonsterId);
    if (pInfo == NULL )
    {
        return 0;
    }
    return pInfo->nColor;
}

// 获取技能信息
GAME_API void* getSpellData(int nSpellID)
{
	if (gClientGlobal == NULL)
	{
		ErrorLn("getSpellData failed, gClientGlobal == NULL");
		return 0;
	}
	ISpellFactory* spellFactory = gClientGlobal->getSpellFactory();
	if (spellFactory == NULL)
	{
		ErrorLn("getSpellData failed, spellFactory == NULL");
		return 0;
	}

    if (nSpellID <= 0)
    {
        return 0;
    }

	SPELL_DATA* pSpellData = spellFactory->getSpellData(nSpellID);
    if (pSpellData == NULL)
    {
        return 0;
    }

	return pSpellData;
}

// 获取技能整形信息
GAME_API int getSpellIntData(int nSpellID, int nSpellDataIndex)
{
    if (gClientGlobal == NULL)
    {
        return 0;
    }

    ISpellFactory* spellFactory = gClientGlobal->getSpellFactory();
    if (spellFactory == NULL)
    {
        return 0;
    }

    if (nSpellID <= 0)
    {
        return 0;
    }

    SPELL_DATA* pSpellData = spellFactory->getSpellData(nSpellID);
    if (pSpellData == NULL)
    {
        return 0;
    }

    return pSpellData->getInt(nSpellDataIndex);
}

// 获取技能字符串信息
GAME_API const char* getSpellStrData(int nSpellID, int nSpellDataIndex)
{
    // 技能字符串数据
    static char strSpellStrData[SPELL_DESC_LEN] = {0};

    if (gClientGlobal == NULL)
    {
        return 0;
    }

    ISpellFactory* spellFactory = gClientGlobal->getSpellFactory();
    if (spellFactory == NULL)
    {
        return 0;
    }

    if (nSpellID <= 0)
    {
        return 0;
    }

    SPELL_DATA* pSpellData = spellFactory->getSpellData(nSpellID);
    if (pSpellData == NULL)
    {
        return 0;
    }

    // 不是字符串
    const char *pStrData = pSpellData->getStr(nSpellDataIndex);
    if (pStrData == NULL)
    {
        return 0;
    }

    sstrcpyn(strSpellStrData, a2utf8(pStrData), sizeof(strSpellStrData));

    return strSpellStrData;
}

// 获取技能对应的槽位
GAME_API int GetSlotIndexOfSpell(int nSpellID)
{
    do 
    {
        if (nSpellID <= 0)
        {
            break;
        }
        if (gClientGlobal == NULL)
        {
            break;
        }
        IClientEntity *pHero = gClientGlobal->getHero();
        if (pHero == NULL)
        {
            break;
        }
        ISlotLogicPart * pPart = (ISlotLogicPart*)pHero->getEntityPart(PART_SLOTLOGIC);
        if (pPart == NULL)
        {
            break;
        }
        SLOT_TYPE nSlotType = SLOT_TYPE_NONE;
        int nSlotIndex = 0;
        if (!pPart->GetSkillIDInfo(nSpellID, nSlotType, nSlotIndex))
        {
            break;
        }
        return nSlotIndex;

    } while (false);

    return -1;
}

//获取快捷键描述(这个时候显示层无法获取配置结构，所以先获取一个属性用着)
GAME_API const char* getShortcutDes(int nIndex, int nType)
{
	if (gClientGlobal == NULL)
	{
		ErrorLn("getShortcutDes failed, gClientGlobal == NULL");
		return 0;
	}
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn("getShortcutDes pSchemeCenter == NULL");
		return 0;
	}
	ISchemeSlotMapedShortcut* pSchemeSlotMapedShortcut = pSchemeCenter->getSchemeSlotMapedShortcut();
	if (pSchemeSlotMapedShortcut == NULL)
	{
		ErrorLn("getShortcutDes pSchemeSlotMapedShortcut == NULL");
		return 0;
	}
	return pSchemeSlotMapedShortcut->getSlotMapedShortcutShemeInfo(nIndex, nType)->szKeyName;
}

GAME_API int GetFreezeTime(DWORD dwClassID, DWORD dwFreezeID, int nIsShowFlag)
{
    do 
    {
         if (gClientGlobal == NULL)
             break;
         IFreezeClient* pFreezeClient = gClientGlobal->getFreezeClient();
         if(pFreezeClient == NULL)
             break;
         return nIsShowFlag == 1 ? pFreezeClient->GetFreezeShowTime(dwClassID, dwFreezeID) : pFreezeClient->GetFreezeTime(dwClassID, dwFreezeID);
    } while (false);    

    return 0;   
}

GAME_API int getMonsterDistance()
{
    if ( gClientGlobal == NULL )
    {
        ErrorLn(__FUNCTION__": gClientGlobal == null");
        return 0;
    }


    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__": pSchemeCenter == null ");
        return 0;
    }

    ISchemeWarPersonConfig* pWarPersonConfig = pSchemeCenter->getSchemeWarPersonConfig();
    if ( pWarPersonConfig == NULL )
    {
        return 0;
    }

    SWarPersonConfig* pConfig = pWarPersonConfig->GetSchemeWarPersonConfigInfo();
    if ( pConfig == NULL )
    {
        return 0;
    }
    return pConfig->nMonsDistance;
}

GAME_API int getTowerDistance()
{
    if ( gClientGlobal == NULL )
    {
        ErrorLn(__FUNCTION__": gClientGlobal == null");
        return 0;
    }


    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__": pSchemeCenter == null ");
        return 0;
    }

    ISchemeWarPersonConfig* pWarPersonConfig = pSchemeCenter->getSchemeWarPersonConfig();
    if ( pWarPersonConfig == NULL )
    {
        return 0;
    }

    SWarPersonConfig* pConfig = pWarPersonConfig->GetSchemeWarPersonConfigInfo();
    if ( pConfig == NULL )
    {
        return 0;
    }
    return pConfig->nTowerDistance;
}

GAME_API int getMonsterDeadEffect()
{
    if ( gClientGlobal == NULL )
    {
        ErrorLn(__FUNCTION__": gClientGlobal == null");
        return 0;
    }


    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__": pSchemeCenter == null ");
        return 0;
    }

    ISchemeWarPersonConfig* pWarPersonConfig = pSchemeCenter->getSchemeWarPersonConfig();
    if ( pWarPersonConfig == NULL )
    {
        return 0;
    }

    SWarPersonConfig* pConfig = pWarPersonConfig->GetSchemeWarPersonConfigInfo();
    if ( pConfig == NULL )
    {
        return 0;
    }
	return pConfig->nMonsterDeadEffect;
}

GAME_API int getMoneyEffect()
{
    if ( gClientGlobal == NULL )
    {
        ErrorLn(__FUNCTION__": gClientGlobal == null");
        return 0;
    }


    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__": pSchemeCenter == null ");
        return 0;
    }

    ISchemeWarPersonConfig* pWarPersonConfig = pSchemeCenter->getSchemeWarPersonConfig();
    if ( pWarPersonConfig == NULL )
    {
        return 0;
    }

    SWarPersonConfig* pConfig = pWarPersonConfig->GetSchemeWarPersonConfigInfo();
    if ( pConfig == NULL )
    {
        return 0;
    }
	return pConfig->nMoneyEffect;
}


GAME_API int getHeroStarLvLimit(int nHeroID)
{
    ISchemeHeroStarData* pStarData = gClientGlobal->getSchemeCenter()->getSchemeHeroStarInfo();
    if ( pStarData == NULL )
        return 0;
    return pStarData->getHeroStarLvLimit(nHeroID);
}

GAME_API int getHeroUpgradeNeedGold(int nHeroID, int nStarLv)
{
	ISchemeHeroStarData* pStarData = gClientGlobal->getSchemeCenter()->getSchemeHeroStarInfo();
	if ( pStarData == NULL )
	{
		return 0;
	}

	SHeroStarSchemeInfo* pStarInfo = pStarData->getHeroStarData(nHeroID, nStarLv);
	if ( pStarInfo == 0 )
	{
		return 0;
	}

	return pStarInfo->nExpendGold;
}

GAME_API int getPlayerLv()
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return 0;
    }

	ENTITY_ID evID = pClientEntity->getViewID();

	return getPlayerLevel(evID);
}

// 获取英雄对应的配置信息
GAME_API void* getPersonModel(WORD wHeroID)
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return 0;
    }
    ISchemePersonModel* pSchemePersonModel = pSchemeCenter->getSchemePersonModel();
    if(pSchemePersonModel == NULL)
    {
        return 0;
    }

    return (void*)(pSchemePersonModel->getPersonSkinModelItem(wHeroID));
}

// 根据等级获得英雄升级需要的经验
GAME_API int getStaticHeroUpNeedExp( int nHeroID, int nLv )
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return 0;
    }

    IBankPart *pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
    if (pBankPart == NULL)
    {
        return 0;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return 0;

    SHeroInfo* pHeroInfo = pBankPart->getHeroInfoByID(nHeroID);
    int nStar = 1;
    if (pHeroInfo != NULL)
    {
        nStar = pHeroInfo->nHeroStar;
    }
    ISchemeLevelUpgrade* pUpgradeInfo = pSchemeCenter->getSchemeLevelUpgrade();
    ISchemeHeroStarData* pStarData = pSchemeCenter->getSchemeHeroStarInfo();   
    if ( pUpgradeInfo == NULL || pStarData == NULL )
    {
        return 0;
    }
    int nExpTypeID = pStarData->getUpgradeNeedExpTypeID(nHeroID, nStar);
    return pUpgradeInfo->GetNextLevelExp(nLv, ENEXPTYPE_STATICHEROEXP, nExpTypeID);
}

// 根据等级获得战场内升级需要的经验
GAME_API int GetWarHeroNextLevelExp(int nEntityID)
{
    IEntityFactory *pEntityFactory = gClientGlobal->getEntityFactory();
    if (pEntityFactory == NULL)
    {
        return 0;
    }

    IClientEntity* pEntity = pEntityFactory->getEntityByView(nEntityID);
    
    int nCurLevel = pEntity->getIntProperty(PROPERTY_LEVEL);
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return 0;
    }
    ISchemeLevelUpgrade* pSchemeLevelUpgrade = pSchemeCenter->getSchemeLevelUpgrade();
    if (pSchemeLevelUpgrade == NULL)
    {
        return 0;
    }
    // 超出等级上限，不再进行属性更新
    if (nCurLevel >= pSchemeLevelUpgrade->GetSchemeLevelUpgradeMaxLevel())
    {
        return 0;
    }

    if (nCurLevel>0)
    {
        //当前等级下已获经验值
        int nCurLevelExpValue = pEntity->getIntProperty(PROPERTY_LEVEL) - pSchemeLevelUpgrade->GetLevelAllExp(nCurLevel);
        //距下一等级所需经验值
        int nNextLevelExpMax = pSchemeLevelUpgrade->GetNextLevelExp(nCurLevel + 1);
        if (nCurLevelExpValue>nNextLevelExpMax)
        {
            ErrorLn(__FUNCTION__": PlayerExp Error: CurLevelAddExpValue = " << nCurLevelExpValue << " > NextLevelNeedExpValue =" << nNextLevelExpMax);
            return 0;
        }

        return nNextLevelExpMax;
    }

    return 0;
}

// 获取引导类型数据
GAME_API int getPlayerNoviceGuideTypeData()
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP;
    }

    IBankPart *pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
    if (pBankPart == NULL)
    {
        return ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP;
    }

    return pBankPart->getPlayerNoviceGuideTypeData();
}

/* 获取本命英雄引导完成情况数据，表示是否走完本命英雄的引导流程
* Return: 1表示已走完完所有本命英雄，0表示还未完成
*/
GAME_API int getLifeHeroTaskData()
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return 0;
	}

	IBankPart *pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
	if (pBankPart == NULL)
	{
		return 0;
	}

	return pBankPart->getLifeHeroTaskData();
}

// 获得某个阵营的人数限制信息
GAME_API int getCampPersonNumRestriction(int nMatchTypeID, int nCamp)
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return 0;

    ISchemeOutputRestriction* pSchemeInfo = pSchemeCenter->getSchemeOutPutRestriction();
    if(pSchemeInfo == NULL)
    {
        return 0;
    }
    return pSchemeInfo->getCampPersonNumRestriction(nMatchTypeID, nCamp);
}

// 获得总人数限制
GAME_API int getMatchAllPersonRestriction(int nMatchTypeID)
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return 0;

    ISchemeOutputRestriction* pSchemeInfo = pSchemeCenter->getSchemeOutPutRestriction();
    if(pSchemeInfo == NULL)
    {
        return 0;
    }
    return pSchemeInfo->getMatchAllPersonRestriction(nMatchTypeID);
}

// 获得机器人战绩炫耀标志位
GAME_API int getAIMilitaryExploitsFlag()
{
	if(gClientGlobal == NULL)
	{
		return 0;
	}

	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeEntity* pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
	if(pSchemeEntity == NULL)
	{
		return 0;
	}

	return pSchemeEntity->getEntityXml()->nAIMilitaryExploitsFlag;
}

// 当前地图是否禁止PK, 0.可PK,1.不能PK
GAME_API int isForbitPK()
{
    do 
    {
        if(gClientGlobal == NULL)
        {
            break;
        }
        IClientEntity * pHero = gClientGlobal->getHero();
        if(pHero == NULL)
        {
            break;
        }
        int nMapID = ActorClient::getInstance().getMapID();
        if(nMapID == INVALID_MAPID)
        {
            break;
        }
        ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
        if(pSchemeCenter == NULL)
        {
            break;
        }
        ISchemeSceneMatron * pSchemeSceneMatron = pSchemeCenter->getSchemeSceneMatron();
        if( pSchemeSceneMatron == NULL )
        {
            break;        
        }
        return pSchemeSceneMatron->IsForbitPK(nMapID);
    } while (false);

    return 1;
}

// 当前地图是否启用实体右键菜单
GAME_API bool isEnableEntityRightMenu()
{
    do
    {
        if (gClientGlobal == NULL)
        {
            break;
        }
        IClientEntity * pHero = gClientGlobal->getHero();
        if (pHero == NULL)
        {
            break;
        }
        int nMapID = ActorClient::getInstance().getMapID();
        if (nMapID == INVALID_MAPID)
        {
            break;
        }
        
        return isMapEnableEntityRightMenu(nMapID);
    } while (false);

    return false;
}

// 指定地图是否启用实体右键菜单
GAME_API bool isMapEnableEntityRightMenu(int nMapID)
{
    do 
    {
        ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            break;
        }
        ISchemeSceneMatron * pSchemeSceneMatron = pSchemeCenter->getSchemeSceneMatron();
        if (pSchemeSceneMatron == NULL)
        {
            break;
        }
        return pSchemeSceneMatron->IsEnableEntityRightMenu(nMapID);
    } while (0);

    return false;
}

// 当前地图启用快捷键方案配置序号
GAME_API int getHotkeyIndex()
{
    do
    {
        if (gClientGlobal == NULL)
        {
            break;
        }
        IClientEntity * pHero = gClientGlobal->getHero();
        if (pHero == NULL)
        {
            break;
        }
        int nMapID = ActorClient::getInstance().getMapID();
        if (nMapID == INVALID_MAPID)
        {
            break;
        }

        return getMapHotkeyIndex(nMapID);
    } while (false);

    return 1;
}

// 当前地图启用快捷键方案配置序号
GAME_API int getMapHotkeyIndex(int nMapID)
{
    do
    {
        ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            break;
        }
        ISchemeSceneMatron * pSchemeSceneMatron = pSchemeCenter->getSchemeSceneMatron();
        if (pSchemeSceneMatron == NULL)
        {
            break;
        }
        return pSchemeSceneMatron->GetHotkeyIndex(nMapID);
    } while (false);

    return 1;
}

// 主角是否在战场
GAME_API int isInWarScene()
{
    do 
    {
        if(gClientGlobal == NULL)
        {
            break;
        }
        IClientEntity * pHero = gClientGlobal->getHero();
        if(pHero == NULL)
        {
            break;
        }
        IWarPart * pWarPart = CAST_TYPE(IWarPart *, pHero->getEntityPart(PART_WAR));
        if(pWarPart == NULL)
        {
            break;
        }

        return (pWarPart->GetWarID() != INVALID_WAR_ID);
    } while (false);

    return 0;
}

GAME_API void GetMainMonitorSize(int &width, int &height)
{
	POINT p;
	p.x = 0;
	p.y = 0;
	HMONITOR hMonitor = ::MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFO mi;

	mi.cbSize = sizeof(mi);
	::GetMonitorInfo(hMonitor, &mi);
	width = mi.rcMonitor.right;
	height = mi.rcMonitor.bottom;
}

// 根据EntityID取等级
GAME_API int getPlayerLevel(ENTITY_ID viewID)
{
	IEntityFactory * pEntityFactory = gClientGlobal->getEntityFactory();
	if (pEntityFactory == NULL)
	{
		return 0;
	}

	IClientEntity * pClientEntity = pEntityFactory->getEntityByView(viewID);
	if (pClientEntity == NULL)
	{
		return 0;
	}

	IBankPart * pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
	if (pBankPart == NULL)
	{
		return 0;
	}

	const SPlayerDatabasesInfo * pPlayerDatabasesInfo = pBankPart->getPlayerDatabasesInfo();
	if (pPlayerDatabasesInfo == NULL)
	{
		return 0;
	}

	return pPlayerDatabasesInfo->player.nLevel;
}

// 主角是否成年人
GAME_API bool isAdult()
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return 0;
	}

	IBankPart * pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
	if (pBankPart == NULL)
	{
		return 0;
	}

	const SPlayerDatabasesInfo * pPlayerDatabasesInfo = pBankPart->getPlayerDatabasesInfo();
	if (pPlayerDatabasesInfo == NULL)
	{
		return 0;
	}

	return pPlayerDatabasesInfo->bIsAdult;
}

// 是否可用七龙珠功能
GAME_API int isOpenSupperStone()
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return 0;
    }

    // 有龙珠部件就可以用
    return  pClientEntity->getEntityPart(PART_DRAGONBALL) ? 1 : 0;
}

// 获取英雄攻击范围
GAME_API int getPersonPropAttackRange(int nHeroID)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemePersonBaseProp* pSchemeProp = gClientGlobal->getSchemeCenter()->getSchemePersonBaseProp();
	if(pSchemeProp == NULL)
	{
		return 0;
	}

	SPersonBasePropScheme* pPersonProp = &pSchemeProp->getOneHeroDetailInfo(nHeroID);
	if(pPersonProp == NULL)
	{
		return 0;
	}

	return pPersonProp->nAttackRange;
}

// 取英雄的星级ID
GAME_API int getHeroStarIconID(int nHeroID, int nStarLv)
{
	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeHeroStarData * pSchemeHeroStarData = pSchemeCenter->getSchemeHeroStarInfo();
	if (pSchemeHeroStarData == NULL)
	{
		return 0;
	}

	SHeroStarSchemeInfo * pHeroStarSchemeInfo = pSchemeHeroStarData->getHeroStarData(nHeroID, nStarLv);
	if (pHeroStarSchemeInfo == NULL)
	{
		return 0;
	}

	return pHeroStarSchemeInfo->nStarIconID;
}

// 取皮肤配置
GAME_API void* getHeroSkinConfig(int nSkin)
{
	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeSkinInfo * pSchemeSkinInfo = pSchemeCenter->getSchemeSkinInfo();
	if (pSchemeSkinInfo == NULL)
	{
		return 0;
	}

	return (void*)pSchemeSkinInfo->getSkinShemeInfo(nSkin);
}

// 取星级配置
GAME_API void* getHeroStarConfig(int nHeroID, int nStarLv)
{
	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeHeroStarData * pSchemeHeroStarData = pSchemeCenter->getSchemeHeroStarInfo();
	if (pSchemeHeroStarData == NULL)
	{
		return 0;
	}

	return (void *)pSchemeHeroStarData->getHeroStarData(nHeroID, nStarLv);
}

// 是否拥有某个BUFF
GAME_API bool IsExistBuff(DWORD dwBuffID)
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return false;
    }

    IBuffPart *pBuffPart = (IBuffPart *)pClientEntity->getEntityPart(PART_BUFF);
    if (pBuffPart == NULL)
    {
        return false;
    }

    return pBuffPart->IsExist(dwBuffID);
}

GAME_API DWORD getCurRoomMatchType()
{
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    if (pMatchClient == NULL)
    {
        return 0;
    }

    return pMatchClient->getCurRoomMatchType();
}

GAME_API int getCurRoomType()
{
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    if (pMatchClient == NULL)
    {
        return 0;
    }

    return pMatchClient->getCurRoomType();
}

// 取得当前房间的ID
GAME_API int getCurrentRoomID()
{
    IMatchClient* pMatchClient = gClientGlobal->getMatchClient();
    if (pMatchClient == NULL)
    {
        return 0;
    }

    return pMatchClient->getCurRoomID();
}


// 加载SCP配置
GAME_API int LoadScp(const char* pszConfigName, void * &pBuff)
{
    int nBuffSize = 0;

	if ( CFileReader::getInstance().Load( SCP_PATH_FORMAT(pszConfigName)) )
    {
        pBuff = CFileReader::getInstance().GetBuffer();
        nBuffSize = CFileReader::getInstance().GetLength();
    }

    return nBuffSize;
}

// 释放SCP配置
GAME_API void FreeScpBuff(void)
{
    CFileReader::getInstance().Close();
}

// 获取组队TeamID
GAME_API int getTeamID(void)
{
	ITeamClient *pTeamClient = gClientGlobal->getTeamClient();
	if (pTeamClient == NULL)
		return 0;

	ITeam *pTeam = pTeamClient->GetTeam();
	if (pTeam == NULL)
		return 0;
	
	return pTeam->GetTeamID();
}

// 取得英雄解锁等级
GAME_API int getUnlockLevel(WORD wVocation)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return -1;

	ISchemePersonModel* pSchemePersonModel = pSchemeCenter->getSchemePersonModel();
	if (pSchemePersonModel == NULL)
		return -1;

	return pSchemePersonModel->getUnlockLevel(wVocation);
}

GAME_API int GetTickCountEx()
{
	return getTickCountEx();
}

// 获取实体的位置
GAME_API void* getEntityLocation(ENTITY_ID viewID)
{
	IEntityFactory * pEntityFactory = gClientGlobal->getEntityFactory();
	if (pEntityFactory == NULL)
	{
		return NULL;
	}

	IClientEntity * pClientEntity = pEntityFactory->getEntityByView(viewID);
	if (pClientEntity == NULL)
	{
		return NULL;
	}

	IEntityProperty* pPropertyPart = (IEntityProperty *)pClientEntity->getEntityPart(PART_PROPERTY);
	if (pPropertyPart == NULL)
	{
		return NULL;
	}

	void * data = 0;
	int len = 0;
	if (pPropertyPart->getProperty_custom(PROPERTY_LOCATION, data, len))
	{
		return data;
	}

	return NULL;
}

// 获取玩家的userID
GAME_API DWORD getPlayerUserID(ENTITY_ID viewID)
{
	int dwUserID = 0;

	do
	{
		IEntityFactory * pEntityFactory = gClientGlobal->getEntityFactory();
		if (pEntityFactory == NULL)
			break;

		IClientEntity * pClientEntity = pEntityFactory->getEntityByView(viewID);
		if (pClientEntity == NULL)
			break;

		IBankPart* pBankPart = (IBankPart*)pClientEntity->getEntityPart(PART_BANK);
		if (pBankPart == NULL)
			break;

		const SPlayerDatabasesInfo* pDBInfo = pBankPart->getPlayerDatabasesInfo();
		if(pDBInfo == NULL)
			break;

		dwUserID = pDBInfo->dwUserID;
	} while (false);

	return dwUserID;
}

// 名称是否存在屏蔽字
GAME_API bool ExistKeyWordOfName(const char* szInStr)
{
	IKeyWordClient* pKeyWordClient = gClientGlobal->getKeyWordClient();
	if (pKeyWordClient == NULL)
		return false;

	return pKeyWordClient->ExistKeyWordOfName(szInStr, strlen(szInStr));
}

// 名称过滤屏蔽字
GAME_API int filterKeyWordOfName(const char* szInStr, const char * &szOutStr)
{
	IKeyWordClient* pKeyWordClient = gClientGlobal->getKeyWordClient();
	if (pKeyWordClient == NULL)
		return 0;

	int nInputSize = strlen(szInStr) + 1;

	static string strResult;
	strResult.resize(nInputSize);

	if (!pKeyWordClient->FilterKeyWordOfName(szInStr, strlen(szInStr), strResult.data(), strResult.size()))
		return 0;

	szOutStr = strResult.data();

	return strResult.size();
}

// 过滤屏蔽字
GAME_API int filterKeyWord(const char* szInStr, const char * &szOutStr)
{
	IKeyWordClient* pKeyWordClient = gClientGlobal->getKeyWordClient();
	if (pKeyWordClient == NULL)
		return 0;

	int nInputSize = strlen(szInStr) + 1;

	static string strResult;		
	strResult.resize(nInputSize);

	if (!pKeyWordClient->FilterKeyWord(szInStr, strlen(szInStr), strResult.data(), strResult.size()))
		return 0;

	szOutStr = strResult.data();

	return strResult.size();
}

// 获取玩家的PDBID
GAME_API DWORD getPlayerPDBID(ENTITY_ID viewID)
{
	int nPDBID = 0;
	do 
	{
		IEntityFactory * pEntityFactory = gClientGlobal->getEntityFactory();
		if (pEntityFactory == NULL)
			break;
		
		IClientEntity * pClientEntity = pEntityFactory->getEntityByView(viewID);
		if (pClientEntity == NULL)
			break;

		IBankPart * pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
		if(pBankPart == NULL)
			break;
		
		const SPlayerDatabasesInfo * pPlayerDataBaseInfo = pBankPart->getPlayerDatabasesInfo();
		if (pPlayerDataBaseInfo == NULL)
			break;
		
		nPDBID = pPlayerDataBaseInfo->player.dwActorID;
		
	} while (false);

	return nPDBID;
}

// 获取选人光效ID
GAME_API int getSelectEffectID(int nSkinID)
{
	int nEffectID = 0;
	do 
	{
		ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
		if (pSchemeCenter == NULL)
			break;

		ISchemeSkinInfo * pSchemeSkinInfo = pSchemeCenter->getSchemeSkinInfo();
		if (pSchemeSkinInfo == NULL)
			break;

		SkinInfoScheme * pSkinInfoScheme = pSchemeSkinInfo->getSkinShemeInfo(nSkinID);
		if (pSchemeSkinInfo == NULL)
			break;

		nEffectID = pSkinInfoScheme->nSelectEffectID;
	} while (false);

	return nEffectID;
}

GAME_API const char* toCoinLighting(int vocation, int type) {
	ISchemeDieLighting *pSchemeDieLighting = gClientGlobal->getSchemeCenter()->getSchemeDieLighting();
	if (pSchemeDieLighting == NULL)
	{
		return "";
	}

	SDieLighting *pDieLighting = pSchemeDieLighting->getDieLighting(vocation, type);
	if (pDieLighting == NULL)
	{
		return "";
	}
	return pDieLighting->szCoinLighting;
}

// 是否fps英雄
GAME_API bool checkIsFpsHero(WORD wVocation)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;

	ISchemePersonModel* pSchemePersonModel = pSchemeCenter->getSchemePersonModel();
	if (pSchemePersonModel == NULL)
		return false;

	return pSchemePersonModel->checkIsFpsHero(wVocation);
}

// 是否已实名认证
GAME_API bool isRealNameAuthed()
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return 0;
	}

	IBankPart * pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
	if (pBankPart == NULL)
	{
		return 0;
	}

	const SPlayerDatabasesInfo * pPlayerDatabasesInfo = pBankPart->getPlayerDatabasesInfo();
	if (pPlayerDatabasesInfo == NULL)
	{
		return 0;
	}

	return (pPlayerDatabasesInfo->dwPassportFlags & RealNameAuthent);
}

// 取英雄的星级框图标ID
GAME_API int getHeroStarFrameID(int nHeroID, int nStarLv)
{
	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeHeroStarData * pSchemeHeroStarData = pSchemeCenter->getSchemeHeroStarInfo();
	if (pSchemeHeroStarData == NULL)
	{
		return 0;
	}

	SHeroStarSchemeInfo * pHeroStarSchemeInfo = pSchemeHeroStarData->getHeroStarData(nHeroID, nStarLv);
	if (pHeroStarSchemeInfo == NULL)
	{
		return 0;
	}

	return pHeroStarSchemeInfo->nStarFrameID;
}

// 取英雄的星级光效ID
GAME_API int getHeroStarEffectID(int nHeroID, int nStarLv)
{
	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeHeroStarData * pSchemeHeroStarData = pSchemeCenter->getSchemeHeroStarInfo();
	if (pSchemeHeroStarData == NULL)
	{
		return 0;
	}

	SHeroStarSchemeInfo * pHeroStarSchemeInfo = pSchemeHeroStarData->getHeroStarData(nHeroID, nStarLv);
	if (pHeroStarSchemeInfo == NULL)
	{
		return 0;
	}

	return pHeroStarSchemeInfo->nStarEffectID;
}

//获取野怪头像ID
GAME_API int getMonsterHeadIconID(int nMonsterID)
{
	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeMonster * pSchemeMonsterData = pSchemeCenter->getSchemeMonster();
	if (pSchemeMonsterData == NULL)
	{
		return 0;
	}

	SMonsterScheme * pMonsterSchemeInfo = pSchemeMonsterData->getMonsterShemeInfo(nMonsterID);
	if (pMonsterSchemeInfo == NULL)
	{
		return 0;
	}

	return pMonsterSchemeInfo->nIconID;
}

// 打印Task相关信息
GAME_API void printTask(void)
{
    CLogicTask::getInstance().print();
}

GAME_API int getTimeCountKillInterval(int nWarID)
{
	if (gClientGlobal == NULL)
	{
		ErrorLn(__FUNCTION__": gClientGlobal == null");
		return 0;
	}


	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == null ");
		return 0;
	}

	ISchemeWarPersonConfig* pWarPersonConfig = pSchemeCenter->getSchemeWarPersonConfig();
	if (pWarPersonConfig == NULL)
	{
		return 0;
	}

	const SchemeWarPersonConfigInfo* pConfig = pWarPersonConfig->GetSchemeWarPersonConfigInfo(nWarID);
	if (pConfig == NULL)
	{
		return 0;
	}

	return pConfig->nTimeCountKillInterval;
}

GAME_API int getConfigWarID()
{
	do
	{
		if (gClientGlobal == NULL)
			break;

		IMatchClient * pMatch = gClientGlobal->getMatchClient();
		if (pMatch == NULL)
			break;
	
		DWORD dwCurRoomID = pMatch->getCurRoomID();
		SMsgCreateRoom_OC* pRoomData = pMatch->getRoomData(dwCurRoomID);
		if (pRoomData == NULL)
			break;

		return pRoomData->nConfigWarID;

	} while (0);


	return INVALID_WAR_ID;
}

GAME_API int getWarEndWndDelayTime()
{
	if (gClientGlobal == NULL)
	{
		return 0;
	}

	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeEntity* pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		return 0;
	}

	return pSchemeEntity->getEntityXml()->fWarEndWndDelayTime;
}

GAME_API bool bIsComputerPDBID(int nPDBID)
{
    return IsComputerPDBID(nPDBID);
}

GAME_API bool isMainMap(int nMapID)
{
	return IsMainMap(nMapID);
}

GAME_API int getContactTaskSwitch()
{
	if (gClientGlobal == NULL)
	{
		return 0;
	}

	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return 0;
	}

	ISchemeEntity* pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		return 0;
	}

	return pSchemeEntity->getEntityXml()->nContactTaskSwitch;
}

// 取英雄的星级所需卡牌
GAME_API int getHeroStarNeedCard(int nHeroID, int nStarLv, bool bIsLifeHero)
{
	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return 0;

	ISchemeHeroStarData * pSchemeHeroStarData = pSchemeCenter->getSchemeHeroStarInfo();
	if (pSchemeHeroStarData == NULL)
		return 0;

	SHeroStarSchemeInfo * pStarScheme = pSchemeHeroStarData->getHeroStarData(nHeroID, nStarLv);
	if (pStarScheme == NULL)
		return 0;

	int nCount = pStarScheme->nNeedCount;
	if (bIsLifeHero)
	{
		nCount = pStarScheme->nCardDiscount > 0 ? pStarScheme->nCardDiscount : nCount;
	}
	
	return nCount;
}

GAME_API DWORD getMasterFromGameWorldID()
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return INVALID_WORLDID;
    }

    return pClientEntity->getFromGameWorldID();
}

GAME_API const char* getActorClientAccount()
{
    gameview_login & login = ActorClient::getInstance().getLoginInfo();
    if (login.account != NULL)
    {
        return login.account;
    }

    return "";
}


GAME_API int getMatchtTypeBaseKingRankScore(int nMatchType)
{
    ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return 0;

    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return 0;
    }

    // 王者段位基础分信息
    return pMatchRankConfigInfo->getMatchTypeKingRankBaseScore(nMatchType);
}

GAME_API int getRankIconIDByGrade(int nMatchType, int nGrade)
{
    ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return 0;

    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return 0;
    }

    // 获得段位信息
    const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(nMatchType, nGrade);
    if (pSchemeInfo == NULL)
    {
        return 0;
    }

    return pSchemeInfo->nIconID;
}


GAME_API int getActorRankIconID(int nMatchType, int nRankScore)
{
    ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return 0;

    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return 0;
    }

    // 获得段位信息
    const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getShemeInfoByTypeAndScore(nMatchType, nRankScore);
    if (pSchemeInfo == NULL)
    {
        return 0;
    }

    return pSchemeInfo->nIconID;
}

GAME_API int getActorRankStar(int nMatchType, int nRankScore)
{
    ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return 0;

    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return 0;
    }

    ISchemeEntity* pSchemeEntity = pSchemeCenter->getSchemeEntity();
    if(pSchemeEntity == NULL)
    {
        return 0;
    }

    if (nRankScore >= pMatchRankConfigInfo->getMatchTypeKingRankBaseScore(nMatchType))
    {
        return 0;
    }

    if (nRankScore < pSchemeEntity->getEntityXml()->nMatchTypeBaseScore)
    {
        nRankScore = pSchemeEntity->getEntityXml()->nMatchTypeBaseScore;
    }

    // 获得段位信息
    const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getShemeInfoByTypeAndScore(nMatchType, nRankScore);
    if (pSchemeInfo == NULL)
    {
        return 0;
    }

    int nStar = (nRankScore - pSchemeInfo->nGradeScore) / pSchemeInfo->nStarSection;
    return nStar;
}



GAME_API int getAddMatchScoreAfterScore(int nMatchType, int nRankScore, int nAddScore)
{
    int nLeftScore = nRankScore + nAddScore;
    ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return nLeftScore;

    ISchemeEntity* pSchemeEntity = pSchemeCenter->getSchemeEntity();
    if(pSchemeEntity == NULL)
    {
        return nLeftScore;
    }

    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return nLeftScore;
    }

    //先看当前分数是不是保底段位
    SMatchRankConfigSchemeInfo* pBaseSchemeInfo = pMatchRankConfigInfo->getShemeInfoByTypeAndScore(nMatchType, nRankScore);
    if (pBaseSchemeInfo == NULL)
    {
        ErrorLn(__FUNCTION__": pBaseSchemeInfo == NULL MatchType="<<nMatchType<<" CurMatchScore ="<< nRankScore);
        return nLeftScore;
    }

    if (pBaseSchemeInfo->bIsBaseGrade)
    {
        if (nRankScore + nAddScore <= pBaseSchemeInfo->nGradeScore)
        {
            return pBaseSchemeInfo->nGradeScore;
        }
    }

    return nLeftScore;
}


GAME_API const char* getActorRankName(int nMatchType, int nRankScore)
{
    ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return "";

    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return "";
    }

    ISchemeEntity* pSchemeEntity = pSchemeCenter->getSchemeEntity();
    if(pSchemeEntity == NULL)
    {
        return 0;
    }

    if (nRankScore < pSchemeEntity->getEntityXml()->nMatchTypeBaseScore)
    {
        nRankScore = pSchemeEntity->getEntityXml()->nMatchTypeBaseScore;
    }

    // 获得段位信息
    const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getShemeInfoByTypeAndScore(nMatchType, nRankScore);
    if (pSchemeInfo == NULL)
    {
        return "";
    }
    return pSchemeInfo->szDes;
}

GAME_API const char* getActorRankNameByGrade(int nMatchType, int nRankGrade)
{
	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return "";

	ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
	if (NULL == pMatchRankConfigInfo)
	{
		ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
		return "";
	}
	// 获得段位信息
	const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(nMatchType, nRankGrade);
	if (pSchemeInfo == NULL)
	{
		return "";
	}
	return pSchemeInfo->szDes;
}

GAME_API int getActorCurRankMaxStar(int nMatchType, int nRankScore)
{
    ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return 0;

    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return 0;
    }

    ISchemeEntity* pSchemeEntity = pSchemeCenter->getSchemeEntity();
    if(pSchemeEntity == NULL)
    {
        return 0;
    }

    if (nRankScore >= pMatchRankConfigInfo->getMatchTypeKingRankBaseScore(nMatchType))
    {
        return 0;
    }

    if (nRankScore < pSchemeEntity->getEntityXml()->nMatchTypeBaseScore)
    {
        nRankScore = pSchemeEntity->getEntityXml()->nMatchTypeBaseScore;
    }

    // 获得段位信息
    const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getShemeInfoByTypeAndScore(nMatchType, nRankScore);
    if (pSchemeInfo == NULL)
    {
        return 0;
    }

    const SMatchRankConfigSchemeInfo* pNextSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(nMatchType, pSchemeInfo->nGradeID + 1);
    if (pNextSchemeInfo == NULL)
    {
        // 没有下一级信息 则找上一级信息
        const SMatchRankConfigSchemeInfo* pPreSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(nMatchType, pSchemeInfo->nGradeID - 1);
        if (pPreSchemeInfo == NULL)
        {
            return 0;
        }

        int nMaxStar = (pSchemeInfo->nGradeScore - pPreSchemeInfo->nGradeScore) / pPreSchemeInfo->nStarSection;
        return nMaxStar;
    }

    int nMaxStar = (pNextSchemeInfo->nGradeScore - pSchemeInfo->nGradeScore) / pSchemeInfo->nStarSection;
    return nMaxStar;
}

// 获取玩家拥有该英雄的卡牌数量
GAME_API int getCardCount(int heroID)
{
	ISchemeHeroStarData* pHeroStarInfo = gClientGlobal->getSchemeCenter()->getSchemeHeroStarInfo();
	if (pHeroStarInfo == NULL)
	{
		return -1;
	}
	
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (!pClientEntity)
		return -1;

	IBankPart *pBankPart = (IBankPart*)pClientEntity->getEntityPart(PART_BANK);
	if (!pBankPart)
		return -1;

	//SHeroStarSchemeInfo * pStarConfig = pBankPart->getHeroCardInfoConfig(heroID);
	SHeroStarSchemeInfo * pStarConfig = pHeroStarInfo->getHeroStarData(heroID, 1);
	if (!pStarConfig)
		return -1;

	DBREQ_PARAM_HERO_CARD heroCard = pBankPart->getHeroCardInfo(pStarConfig->dwNeedCardID);

	return heroCard.nCardCount;
}

GAME_API int GetCurNetClass()
{
    return NetSniffer::getInstance().GetCurNetClass();
}

// 获取主角战场使用的召唤师技能
GAME_API int getWarSummerID()
{
	if (gClientGlobal == NULL)
	{
		return 0;
	}
	IClientEntity * pHero = gClientGlobal->getHero();
	if (pHero == NULL)
	{
		return 0;
	}
	IWarPart * pWarPart = CAST_TYPE(IWarPart *, pHero->getEntityPart(PART_WAR));
	if (pWarPart == NULL)
	{
		return 0;
	}

	return pWarPart->GetSummerID();
}

// 判断英雄是否可以升星
GAME_API bool isHeroCanUpStar(int heroID, int starLv, bool isLifeHero)
{
	if (heroID < 0)
	{
		WarningLn(__FUNCTION__": Invalid parameter. heroID=" << heroID << ", starLv=" << starLv);
		return false;
	}

	ISchemeCenter * pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
		return false;
	}

	ISchemeHeroStarData * pSchemeHeroStarData = pSchemeCenter->getSchemeHeroStarInfo();
	if (pSchemeHeroStarData == NULL)
	{
		ErrorLn(__FUNCTION__": NULL == pSchemeHeroStarData");
		return false;
	}

	IClientEntity * pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		ErrorLn(__FUNCTION__": NULL == pClientEntity");
		return false;
	}

	IEntityProperty *pEntityProperty = (IEntityProperty *)pClientEntity->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		ErrorLn(__FUNCTION__": NULL == pEntityProperty");
		return false;
	}

	SHeroStarSchemeInfo * pStarScheme = pSchemeHeroStarData->getHeroStarData(heroID, starLv + 1);
	if (pStarScheme == NULL)
	{
		ErrorLn(__FUNCTION__": NULL == pStarScheme");
		return false;
	}

	int haveGold = pEntityProperty->getProperty_Integer(PROPERTY_GOLD);
	int haveCard = getCardCount(heroID);
	int needCard = isLifeHero && pStarScheme->nCardDiscount > 0 ? pStarScheme->nCardDiscount : pStarScheme->nNeedCount;
	int needGold = isLifeHero && pStarScheme->nGoldDiscount > 0 ? pStarScheme->nGoldDiscount : pStarScheme->nExpendGold;

	return haveGold >= needGold && haveCard >= needCard;
}

GAME_API int getCurGameMode()
{
	IMiscManager* pMiscMgr = gClientGlobal->getMiscManager();
	if (pMiscMgr != NULL)
	{
		if (isMainMap(pMiscMgr->getMapID()))
		{
			return EGM_RPG;
		}
	}

	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	if (pMatchClient == NULL)
		return EGM_RPG;

	int nMatchType = pMatchClient->getCurRoomMatchType();
	SSchemeMatchRoom* pSchemeMatchRoom = pMatchClient->getSchemeMatchRoomByMatchType(nMatchType);
	if(pSchemeMatchRoom == NULL)
		return EGM_RPG;

	return pSchemeMatchRoom->m_nModeID;
}

GAME_API int getGameModeByMatchType(int nMatchType)
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	if (pMatchClient == NULL)
		return EGM_RPG;

	SSchemeMatchRoom* pSchemeMatchRoom = pMatchClient->getSchemeMatchRoomByMatchType(nMatchType);
	if (pSchemeMatchRoom == NULL)
		return EGM_RPG;

	return pSchemeMatchRoom->m_nModeID;
}

// 获取比赛类型名称
GAME_API const char* getMatchNameByMatchType(int nMatchType)
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	if (pMatchClient == NULL)
		return "";

	SSchemeMatchRoom* pSchemeMatchRoom = pMatchClient->getSchemeMatchRoomByMatchType(nMatchType);
	if (pSchemeMatchRoom == NULL)
		return "";

	return pSchemeMatchRoom->m_szMatchTypeDes;
}

GAME_API bool bIsGM(int nPDBID)
{
	return IsGM(nPDBID);
}

// 获取客户端是否为续玩
GAME_API bool IsContinuePlay()
{
    IMiscManager* pMiscMgr = gClientGlobal->getMiscManager();
    return pMiscMgr ? pMiscMgr->IsContinuePlay() : false;
}

// 根据英雄ID获取技能ID
GAME_API void* GetSlotSpellID(int heroID)
{
    SPELL_SLOT* pSpell = g_EHelper.getSpellSlot(heroID);
    if (pSpell == NULL)
    {
        static SlotSpellID  s_NullSpell;
        return &s_NullSpell;
    }

    static SlotSpellID  s_obSpell;
    for (int i = 0; i < 4; i++)
    {
        s_obSpell.arrSpellID[i] = pSpell->nSpellID[i];
    }

    return &s_obSpell;
}

GAME_API bool clanCanPublicLegendCup()
{
    IClanClient *pClanClient = gClientGlobal->getClanClient();
    if (pClanClient == NULL)
    {
        return false;
    }
    return pClanClient->canPublicClanLegenCup();
}

