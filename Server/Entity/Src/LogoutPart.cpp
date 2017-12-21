#include "stdafx.h"
#include "LogoutPart.h"
#include "ActorDef.h"
#include "IDBEngineService.h"
#include "IPlayerRole.h"
#include "IFramework.h"
#include "IServiceMgr.h"
#include "IServerGlobal.h"
#include "IEntityEvent.h"
#include "EventDef.h"

#include "IBankPart.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include "IWarPart.h"

LogoutPart::LogoutPart() : m_bLogouting(false), m_pMaster(NULL), m_nLogoutReason(elogout_unknown)
{

}

short LogoutPart::getPartID()
{
    return PART_LOGOUT;
}

bool LogoutPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
    m_pMaster = pEntity;
    gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);
    return true;
}

bool LogoutPart::onMessage(void* pEntityHead, int msgId, void* data, size_t len)
{
    switch(msgId)
    {
    case SS_MSG_ACTOR_LOGOUT:
        {
            if(len < sizeof(int))
            {
                ErrorLn(__FUNCTION__ << "," << __LINE__);
                return false;
            }
            return doLogout(*((int*)data));
        }
        break;
    }

    return true;
}

__IEntity* LogoutPart::getHost()
{
    return m_pMaster;
}

void LogoutPart::restore()
{
    gServerGlobal->getDBEngineService()->unregisterHandler(this);
    m_pMaster = 0;
}

void LogoutPart::release()
{
    delete this;
}

bool LogoutPart::onSerialize( rkt::ibuffer & in,int flag )
{
    return true;
}

bool LogoutPart::deSerialize( rkt::obuf & out,int flag )
{
    return true;
}

int LogoutPart::getLogoutReason()
{
    return m_nLogoutReason;
}

bool LogoutPart::doLogout(int nReason)
{
    if(m_bLogouting)
    {
        WarningLn(__FUNCTION__ << ", logout more than once");
        return false;
    }

    m_bLogouting = true;
    m_nLogoutReason = nReason;

    // 如果是战场玩家 清理数据库续完信息
//     __IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
//     DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID); 
//     if(pWarPart != NULL && pWarPart->getWarID() != INVALID_WAR_ID && !IsComputerPDBID(dwActorID))
//     {
//         DBREQ_SAVE_TOBECONTINUED_INFO dbParam;
//         dbParam.dwActorID = dwActorID;
//         dbParam.bEnable = false;
// 
//         // 存到来源区的数据库 TODO.
//         gServerGlobal->getDBEngineService()->executeSP(GAMEDB_REQUEST_SAVE_TOBECONTINUED_INFO, 0, (LPCSTR)(&dbParam), sizeof(dbParam), this);
//     }

    m_pMaster->getEntityEvent()->FireExecute(EVENT_ACTOR_PREPARE_LOGOUT, (LPCSTR)&nReason, sizeof(nReason));

    // 处理角色地图坐标
    updatePlayerMapInfo(nReason);

    // 存盘
    m_pMaster->getEntityEvent()->FireExecute(EVENT_ACTOR_PREPARE_LOGOUT_SAVE, (LPCSTR)&nReason, sizeof(nReason));


    // 解锁 todo.
    if(nReason == elogout_offline || nReason == elogout_cross_gameworld)
    {
        PDBID pdbid = (PDBID)m_pMaster->getIntProperty(PROPERTY_ID);
        if( !IsComputerPDBID(pdbid) )
        {
            __IBankPart *pBank = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
            if( pBank )
            {
                DBREQ_PARAM_SETUSERLOCK dbParam;
                dbParam.dwUserID         = pBank->getUserID();
                dbParam.nLockFlag        = 0/*帐号永久锁*/;   
                dbParam.OnOffLine        = 2/*下线*/;
                dbParam.dwActorID        = pdbid;
                dbParam.nCheckActorExist = 0/*不需要验证*/;
                dbParam.nZoneSvrHardID   = gServerGlobal->getServerID();
                dbParam.nGameWorldID     = getThisGameWorldID();

                if (gServerGlobal->getDBEngineService()->executeSP(GAMEDB_REQUEST_SETUSERLOCK, dbParam.dwUserID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this), dbParam.nGameWorldID ) == false)
                {
                    ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_SETUSERLOCK Logoff failed! PDBID="<<dbParam.dwActorID);
                }
            }
        }
    }

	if (nReason == elogout_offline)
	{
		// 设置离线状态
		((__IPlayerRole*)m_pMaster)->setGameState(GAME_STATE_OFFLINE);
	}

    // 人物内部发实体登出事件
    if (m_pMaster->getType() == TYPE_PLAYER_ROLE)
    {
        event_actor_logout eventdata;
        eventdata.client    = (CAST_TYPE(__IPlayerRole*, m_pMaster))->getClientID();
        eventdata.dwDBID    = m_pMaster->getIntProperty(PROPERTY_ID);
        eventdata.uid       = m_pMaster->getUID();
        eventdata.nSceneID  = m_pMaster->getSceneID();
        eventdata.eMode     = (ELogoutMode)nReason;
        m_pMaster->getEntityEvent()->FireExecute(EVENT_ACTOR_LOGOUT, (LPCSTR)&eventdata, sizeof(eventdata));
    }

    // 等解锁成功后，销毁角色，这里暂时先直接销毁
    SERVICE_PTR pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service(((__IPlayerRole*)m_pMaster)->getServiceID());
    if(pContainer != 0)
    {
        pContainer->stop();
    }

    return true;
}

void LogoutPart::updatePlayerMapInfo(int nReason)
{
    if(nReason != elogout_offline)
    {
        return;
    }
    if(m_pMaster == nullptr)
    {
        return;
    }
    __IBankPart * pBankPart = CAST_TYPE(__IBankPart*, m_pMaster->getEntityPart(ENTITY_PART::PART_BANK));
    if(pBankPart == nullptr )
    {
        return;
    }
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter == nullptr)
    {
        return;
    }
    ISchemeMapLand* pSchemeMapLand = pSchemeCenter ->getSchemeMapLand();
    if(pSchemeMapLand == nullptr)
    {
        return;
    }

    int nSceneID = m_pMaster->getSceneID();
    if( IsDynamicScene(nSceneID) )
    {
        return;
    }

    SPlayerInfo * pPlayerInfo = pBankPart->getPlayerInfo();
    if(!pPlayerInfo || pPlayerInfo->dwMapId != nSceneID)
    {
        return;
    }

    // 静态场景
    float fAngleY = 0;
    int nMapID = nSceneID;
    int nCampID = m_pMaster->getIntProperty(PROPERTY_CAMP);    
    SMapLandSchemeInfo* pInfo = pSchemeMapLand->GetMapLandSchemeInfo(nMapID, EGM_RPG, nCampID);
    if(pInfo != NULL && !pInfo->vecBornPos.empty())
    {
        srand(time(0)); 
        int nIndex = rand() % pInfo->vecBornPos.size();
        const SPosWithAngle& pos = pInfo->vecBornPos[nIndex];
        pPlayerInfo->nLocX = CAST_TYPE(int, pos.ptPos.x * ACTOR_LOCATION_SCALE);
        pPlayerInfo->nLocY = CAST_TYPE(int, pos.ptPos.y * ACTOR_LOCATION_SCALE);
        pPlayerInfo->nLocZ = CAST_TYPE(int, pos.ptPos.z * ACTOR_LOCATION_SCALE);
        pPlayerInfo->nAngle = CAST_TYPE(int, pos.fAngleY * ACTOR_LOCATION_SCALE);
    }
}
///////////////////////IDBRetSink////////////////////
void LogoutPart::OnReturn(IDBRetSink* pRealDBRetSink, int nCmdID, int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    // 销毁角色
}


////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __ILogoutPart * CreateLogoutPart()
{
    return new LogoutPart;
}