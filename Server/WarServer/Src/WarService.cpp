/*******************************************************************
** 文件名:	WarService.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	2015-6-30
** 版  本:	1.0
** 描  述:	

			战场War服务
**************************** 修改记录 ******************************/

#include "StdAfx.h"
#include "WarService.h"
#include "MobaWar.h"
#include "MobaAltarWar.h"
#include "IServerGlobal.h"
#include "IWarManager.h"
#include "Game_ManagedDef.h"
#include "MonsterExec.h"
#include "MobaGuideWar.h"
#include "BattleWar.h"
#include "IWarMonsterMgr.h"
#include "IHeroGenicMgr.h"

WarService::WarService() 
    : m_pWar(0)
{

}

WarService::~WarService()
{

}

bool WarService::on_start(SERVICE_PTR pContainer, void* data, int len)
{
    ibuffer in(data,len);

    Room_Create_War_Context room_context;
    in >> room_context;

    // 创建战场
    if(room_context.nMode == EGM_Moba || room_context.nMode == EGM_MCAMP)
    {
        switch(room_context.dwMatchTypeID)
        {
        case MatchType_NewPlayerGuide2:
		case MatchType_NewPlayerGuide1:	// 新手引导战场
			{
				m_pWar = new CMobaGuideWar();
			}
			break;
		case MatchType_MultiCampMatch:  // 大逃杀战场
			{
				m_pWar = new CBattleWar();
			}
			break;
        default:
            {
                m_pWar = new CMobaWar();
            }
        }

        
		if(!m_pWar)
		{
			return false;
		}

		// 创建War上下文
		SWarCreateContext context;
		context.byRoomType		= room_context.byRoomType;			// 房间类型 EMHallRoomType
        context.dwMatchTypeID   = room_context.dwMatchTypeID;       // 撮合匹配类型
		context.dwRoomID		= room_context.nRoomIndex;			// 房间ID
		context.dwMapID			= room_context.nMapID;				// 地图ID
		context.byMode			= room_context.nMode;			    // 战场类型
		context.nConfigWarID	= room_context.nSchemeConfigWarID;	// 
		context.nWarID			= room_context.nWarID;				// 战场ID
		context.dwAvgRankScore  = room_context.dwAvgRankScore;		// 平均匹配分
		context.byRoleCount		= room_context.byRoleCount;			// 总人数
        context.byCampCount		= room_context.byCampCount;			// 阵营数量

        // 设置
        gServerGlobal->getWarManager()->onWarCreate(context.nWarID, pContainer);
		
        return m_pWar->create(this, context);
    }
    else
    {
        ErrorLn("WarService::on_start failed, this game mode not deal, nMode="<< room_context.nMode <<",RoomID="<<room_context.nRoomIndex<<", WarID="<< room_context.nWarID);
        return false;
    }
}

void WarService::destroy()
{
    m_pWar->UnInit();
    m_pWar->destroy();
}

bool WarService::on_stop()
{
    if(m_pWar == NULL)
    {
        return true;
    }

    int nWarID = m_pWar->getWarID();
    m_pWar->Release();

    gServerGlobal->getWarManager()->OnWarDestroied(nWarID);

    return true;
}

// 释放
void WarService::release()
{
    delete this;
}

void WarService::Start()
{
    m_pWar->Start();
}

// 玩家进入战场
bool WarService::PrepareEnterWar(SActorPreaperEnterWar sActorPreaperEnterWar)
{
    return m_pWar->ClientPrepareEnterWar(sActorPreaperEnterWar);
}

// 玩家进入战场事件
void WarService::onActorEnterWar(SActorEnterWarBaseInfo sActorEnterWarBaseInfo)
{
    return m_pWar->onActorEnterWar(sActorEnterWarBaseInfo);
}

// 保留原来接口!!
void WarService::onEntityDie( sEntityDieInfo entityDieInfo)
{
    m_pWar->onEntityDie(entityDieInfo);
}

void WarService::onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info)
{
    m_pWar->onEntityDieEx(entityDieInfo, info);
}

void WarService::onEntityRelive(UID uid)
{
	m_pWar->onEntityRelive(uid);
}

// client 的网络消息
void WarService::onWarClientMsg(UID uidActor, BYTE byKeyAction, PACKAGE_PTR msg)
{
    m_pWar->onWarClientMsg(uidActor, byKeyAction, msg);
}

int WarService::getAllPerson(UID *PersonArray, int nArraySize)
{
	return m_pWar->getAllPerson(PersonArray, nArraySize);
}

// 获取场景ID
int WarService::getWarSceneID()
{
	return m_pWar->getWarSceneID();
}

// 获得战场类型id
 int WarService::getWarTypeId()
 {
 	return m_pWar->getWarTypeId();
 }



void WarService::DragonBallEndWar(int nFailedCamp, bool bNormalEnd)
{
	m_pWar->forceEndWar(nFailedCamp, bNormalEnd);
}


// 场景已创建
void WarService::onSceneCreated(int nWarID)
{
	m_pWar->Init();
}

// 用于通知指定阵营野怪死亡状况
void WarService::msgToSpecCampWildInfo(UID uMonsterUid, int nCamp)
{
    if (m_pWar->isEnd())
    {
        return;
    }

	IWarMonsterMgr* pMonsterMgr = (IWarMonsterMgr*)m_pWar->getWarMgr(Mgr_Monster);
	if (pMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pMonsterMgr == NULL");
		return;
	}

    CMonsterExec *tempCM = reinterpret_cast<CMonsterExec *>(pMonsterMgr->getWarMonsterExecByType(EWME_Monster));
    if (!tempCM)
    {
        WarningLn("tempCM is NULL!!! " << " Function: " << __FUNCTION__ << " FILE: " << __FILE__ << " LINE: " << __LINE__ );
        return;
    }
    tempCM->msgToSpecCampWildInfo(uMonsterUid, nCamp);
}


LONGLONG WarService::getWarDBKey()
{
    return m_pWar->getWarDBKey();
}



// 取得比赛类型
 DWORD WarService::getMatchTypeID()
{
    return m_pWar->getMatchTypeID();
}

 // 设置充能值
void WarService::setChargedValue(UID uidTarget, int nEffectID, int nValue)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	warMiscMgr->setChargedValue(uidTarget, nEffectID, nValue);
}

// 取得充能值
int WarService::getChargedValue(UID uidTarget, int nEffectID)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return 0;
	}

    return warMiscMgr->getChargedValue(uidTarget, nEffectID);
}

// 设置结束战场原因
void WarService::setWarEndReason(BYTE byEndReason)
{
    return m_pWar->setWarEndReason(EWarEndReason(byEndReason));
}

DWORD WarService::warSerchTypeUIDList(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize)
{
    return m_pWar->warSerchTypeUIDList(bySelfCamp, byGetCampType, bySerchType, pReturnArray, dwArrayMaxSize);
}

/** 更新玩家BUFF信息列表
@param data
@param len
*/
void WarService::updateActorBuffList(char* data, size_t len)
{
    // TODO
}

void WarService::deSerialzed(UID uid, rkt::obuf & out, int flag)
{
	m_pWar->deSerialzed(uid, out, flag);
}

void WarService::setBanHeroList(char * data, size_t len)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	warMiscMgr->setBanHeroList(data, len);
}

void WarService::cureMember(UID uid)
{
	m_pWar->cureMember(uid);
}

void WarService::changeVocation(UID uid, int nSlotID)
{
	IHeroGenicMgr* pHeroGenicMgr = (IHeroGenicMgr*)m_pWar->getWarMgr(Mgr_HeroGenic);
	if (pHeroGenicMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pHeroGenicMgr == NULL");
		return;
	}

	pHeroGenicMgr->changeVocation(uid, nSlotID);
}

void WarService::collectHeroGenic(UID uid, int nAddHeroID, int nAddSkinID)
{
	IHeroGenicMgr* pHeroGenicMgr = (IHeroGenicMgr*)m_pWar->getWarMgr(Mgr_HeroGenic);
	if (pHeroGenicMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pHeroGenicMgr == NULL");
		return;
	}

	pHeroGenicMgr->onNewGenic(uid, nAddHeroID, nAddSkinID, "");
}

void WarService::onFakeDeadRelive(UID uid)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	warMiscMgr->onFakeDeadRelive(uid);
}

void WarService::setLegendCupInfo(SMsgLegendCupRoomInfo sLegendCupInfo)
{
    m_pWar->setLegendCupInfo(sLegendCupInfo);
}
