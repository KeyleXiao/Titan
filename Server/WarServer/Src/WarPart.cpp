/*******************************************************************
** 文件名:	WarPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/6/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "WarPart.h"
#include "EntityHelper.h"
#include "IServiceContainer.h"
#include "IServerGlobal.h"
#include "IEntityEvent.h"
#include "EntityDef.h"
#include "ServiceHelper.h"
#include <math.h>
#include "WarMessageDef.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "IPlayerRole.h"
#include "ISpell.h"
#include "ExternalHelper.h"

//#include "LuaHelper.h"
#define  NOTLUA_CALL_LINE
////////////////////* WarPart */////////////////////////////////////////////
WarPart::WarPart()
	:m_pMaster(NULL), m_nWarID(INVALID_WAR_ID), m_nIndex(-1), m_nMatchTypeID(0), m_nWarTypeID(0)
{
	
}

WarPart::~WarPart()
{

}

////////////////////* IEntityPart */////////////////////////////////////////////
short WarPart::getPartID()
{
	return PART_WAR;
}

int WarPart::getWarID()
{
    return m_nWarID;
}

int WarPart::getWarTypeID()
{
    return m_nWarTypeID;
}

int WarPart::getMatchTypeID()
{
    return m_nMatchTypeID;
}

int WarPart::getIndex()
{
    return m_nIndex;
}

int WarPart::getSmSkillId(int nSlot)
{
    if ( nSlot >= SUMMONER_SKILL_COUNT )
    {
        return 0;
    }
    return m_nSmSkillId[nSlot];
}

bool WarPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	if (!pEntity)
	{
		return false;
	}

	m_pMaster = pEntity;

    if(m_pMaster->getType() != TYPE_PLAYER_ROLE)
    {
        return true;
    }

    if(m_nWarID < 0)
    {
        return true;
    }

    __IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent)
    {
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CREATE,__FUNCTION__);
    }

    // 加载召唤师技能信息
    __ISpellPart *pSpellPart = (__ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);

    if (pSpellPart == nullptr)
    {
        return false;
    }

    for ( int i = 0;i < SUMMONER_SKILL_COUNT; ++i )
    {
        pSpellPart->addSpell( m_nSmSkillId[i], SLOT_TYPE_SKILL, m_nSmSkillIndex[i], 0 );

    }

    SERVICE_PTR pContainer = gServerGlobal->getWarManager()->GetWar(m_nWarID);
    if(pContainer == 0)
    {
        ErrorLn(__FUNCTION__": pContainer == 0 war="<< m_nWarID <<",mode="<< nLoginMode);
        return false;
    }

    IWarService* pWarService = (IWarService*)pContainer->query_interface();
    if(pWarService == NULL)
    {
        ErrorLn(__FUNCTION__":  pWarService == NULL, war=" << m_nWarID << ",mode=" << nLoginMode);
        return false;
    }
    m_nMatchTypeID = pWarService->getMatchTypeID();
    m_nWarTypeID = pWarService->getWarTypeId();

    // BUFF管理
    m_buffMgr.onLoad(this);

	return true;
}

void WarPart::release()
{
    // 重置
    m_buffMgr.restore();

    // 退定系统消息
    if (m_pMaster)
    {
        __IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
        if (pEntityEvent)
        {
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CREATE);

        }
    }

    m_pMaster = 0;

    delete this;
}

/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 容器事件触发后执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	*/
void WarPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
		// 实体创建
	case EVENT_ENTITY_CREATE:
        {
            // 通知战场角色进入事件
            SERVICE_PTR pContainer = gServerGlobal->getWarManager()->GetWar(m_nWarID);
            if(pContainer == 0)
            {
                ErrorLn(__FUNCTION__": pContainer == 0 m_nWarID ="<< m_nWarID);
                return ;
            }

            IWarService* pWarService = (IWarService*)pContainer->query_interface();
            if(pWarService == NULL)
            {
                ErrorLn(__FUNCTION__":  pWarService == NULL");
                return ;
            }

            __IEntityProperty* pProperty = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
            if(pProperty == NULL)
            {
                ErrorLn(__FUNCTION__":  pProperty == NULL");
                return ;
            }

            IActorService* pPerson = ActorHelper(m_pMaster->getIntProperty(PROPERTY_ID), ACTOR_DBID).m_ptr;
            if (pPerson != NULL)
            {
                pPerson->setGameState(GAME_STATE_WAR);
            }

            SActorEnterWarBaseInfo sActorEnterWarBaseInfo;
            sActorEnterWarBaseInfo.uidActor = m_pMaster->getUID();
            sActorEnterWarBaseInfo.ulClient = ((__IPlayerRole*)m_pMaster)->getClientID();
            sActorEnterWarBaseInfo.pdbid    = pProperty->getProperty_Integer(PROPERTY_ID);
            sActorEnterWarBaseInfo.nHeroID  = pProperty->getProperty_Integer(PROPERTY_VOCATION);
            sActorEnterWarBaseInfo.nCamp    = pProperty->getProperty_Integer(PROPERTY_CAMP);
            sActorEnterWarBaseInfo.dwUserID = pPerson->getUserID();
			sActorEnterWarBaseInfo.nSummerID = pPerson->getSummonerSkillInfo(0);
			sActorEnterWarBaseInfo.nHeroSkinID = pProperty->getProperty_Integer(PROPERTY_SKIN);
            sActorEnterWarBaseInfo.bIsOB       = pProperty->getProperty_Integer(PROPERTY_OBSERVER) == 1;
            pWarService->onActorEnterWar(sActorEnterWarBaseInfo);
        }
		break;

	default:
		break;
	}
}

bool WarPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    SERVICE_PTR pContainer = gServerGlobal->getWarManager()->GetWar(m_nWarID);
    if(pContainer == 0)
    {
        WarningLn("WarPart::onMessage pContainer == 0 msgId ="<<msgId<< ", m_nWarID = "<<m_nWarID);
        return false;
    }

    IWarService* pWarService = (IWarService*)pContainer->query_interface();
    if(pWarService == NULL)
    {
        ErrorLn("WarPart::onMessage pWarService == NULL msgId ="<<msgId<< ", m_nWarID = "<<m_nWarID);
        return false;
    }
    pWarService->onWarClientMsg(m_pMaster->getUID(), msgId, (const char *)data, len);
	return true;
}

__IEntity * WarPart::getHost()
{
	return m_pMaster;
}

// 切换英雄
void WarPart::onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore)
{
    if(isBefore)
    {
        // 切英雄前
    }
    else
    {
        // 切英雄后
        if (!m_pMaster)
        {
            return;
        }

        if(m_nWarID < 0)
        {
            return;
        }

        // 重新学习召唤师技能
        __ISpellPart *pSpellPart = (__ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);
        if(pSpellPart == NULL)
        {
            return;
        }

        for(int i = 0;i < SUMMONER_SKILL_COUNT; ++i)
        {
            pSpellPart->addSpell(m_nSmSkillId[i], SLOT_TYPE_SKILL, m_nSmSkillIndex[i], 0);
        }
    }
}

bool WarPart::onSerialize( rkt::ibuffer & in,int flag )
{
    if(flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {
        return true;
    }

    int nSummorSkillCount = 0;
	in >> m_nWarID >> m_nIndex >> nSummorSkillCount;

    for(int i = 0; i < nSummorSkillCount; ++i)
    {
        int nSlot;
        int nSkill;
        in >> nSlot >> nSkill;

        if(i < SUMMONER_SKILL_COUNT)
        {
            m_nSmSkillIndex[i] = nSlot;
            m_nSmSkillId[i] = nSkill;
        }
    }

	return true;
}

bool WarPart::deSerialize( rkt::obuf & out,int flag )
{
    if(flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {
        return true;
    }

    out << m_nWarID;

	if (flag & TO_CLIENT_SELF)
	{
        bool bHaveData = true;
        rkt::obuf desWarBuf;
        do 
        {
            if (m_pMaster == NULL)
            {
                bHaveData = false;
                break;
            }

            UID uid = m_pMaster->getUID();
            if (UID_2_TYPE(uid) !=TYPE_PLAYER_ROLE)
            {
                bHaveData = false;
                break;
            }

            SERVICE_PTR pContainer = gServerGlobal->getWarManager()->GetWar(m_nWarID);
            if (pContainer == 0)
            {
                bHaveData = false;
                break;
            }

            IWarService* pWarService = (IWarService*)pContainer->query_interface();
            if (pWarService == NULL)
            {
                bHaveData = false;
                break;
            }

            if (bHaveData)
            {
                pWarService->deSerialzed(m_pMaster->getUID(), desWarBuf, flag);
            }
        } while (false);

        out<<bHaveData;
        if (bHaveData)
        {
            out.push_back(desWarBuf.data(), desWarBuf.size());
        }
	}

	return true;
}

//
///////////////////////////////////////////////////////////////////////

bool WarPart::Parse(string linkContent)
{
	return true;
}

void WarPart::restore()
{

}

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __IWarPart * CreateWarPart()
{
	return new WarPart;
}