#include "stdafx.h"
#include "RoomState_Fighting.h"
#include "MatchDef.h"
#include "NetMessageDef.h"
#include "ITimerService.h"
#include "IClientGlobal.h"
#include "CurrentMatchRoomInfo.h"
#include "AOI_Define.h"
#include "MigrateDef.h"

void RoomState_Fighting::onEnter()
{
	MATCH_TRACELN(__FUNCTION__ ": Fight" );
	//发到显示层状态
    cmd_change_roomstate cmdInfo;
    cmdInfo.nStateID = getID();
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_STATE,getID(),0,&cmdInfo, sizeof(cmdInfo));
	
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_READY_TO_FIGHT,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA,this );
}

void RoomState_Fighting::onLeave()
{
	MATCH_TRACELN(__FUNCTION__);	
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_READY_TO_FIGHT);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA);
}

RoomState_Fighting::RoomState_Fighting( CurrentMatchRoomInfo* pRoom )
	:BaseRoomState(pRoom, MatchRoomState_Fighting)
{
}

RoomState_Fighting::~RoomState_Fighting()
{

}

void RoomState_Fighting::onServerMsg(SNetMsgHead* pHead, void* pData, size_t stLen)
{
	switch ( MAKEWORD(pHead->byKeyModule, pHead->byKeyAction) )
	{
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ROOM_READY_TO_FIGHT):
		{
			onMsgRoomReadyToFight(pHead, pData, stLen);
		}
		break;
	
	default:
		{ 
			BaseRoomState::onServerMsg(pHead, pData, stLen);
		}
	}
}


void RoomState_Fighting::onMsgRoomReadyToFight( SNetMsgHead* head, void* data, size_t len )
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << m_pRoom->getRoomData()->m_RoomID);
		return;
	}
	if (MatchRoomState_Fighting != pRoom->byState)
	{
		ErrorLn(__FUNCTION__": MatchRoomState_Wait != pRoom->m_byState RoomID= " << m_pRoom->getRoomData()->m_RoomID<<"pRoom->m_byState= " << pRoom->byState);
		return;
	}

	MATCH_TRACELN(__FUNCTION__ << "pRoom->m_byState = " << pRoom->byState << ", RoomId = " << m_pRoom->getRoomData()->m_RoomID );
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_READY_TO_FIGHT, 0, 0, 0, 0);
}


bool RoomState_Fighting::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{
	switch ( eventID )
	{
	case GVIEWCMD_MATCH_READY_TO_FIGHT:
		{
			onGVIEWCMD_MATCH_READY_TO_FIGHT(nParam, strParam, ptrParam);
		}
		break;
    case GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA:
        {
            onGVIEWCMD_MATCH_FROM_GETHREO_DES(nParam, strParam, ptrParam);
        }
        break;
	}
	return true;
}

void RoomState_Fighting::onGVIEWCMD_MATCH_READY_TO_FIGHT(int nParam, const char * strParam, void * ptrParam)
{
	//选择英雄状态结束
	obuf256 ob;
	SMsgReadyToFight data;
	data.bIsReady = true;

	SNetMsgHead MsgHead;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_READY_TO_FIGHT;						// 子消息代码  EMRoomMessageCode

	ob << MsgHead <<outData << data;

    net_senddata( ob.data(), ob.size() );
}


void RoomState_Fighting::onGVIEWCMD_MATCH_FROM_GETHREO_DES( int nParam, const char * strParam, void * ptrParam )
{
    if (nullptr == ptrParam)
    {
        MATCH_ERRORLN(__FUNCTION__": nullptr == ptrParam " );
        return;
    }

    IClientEntity * pMainEntity = gClientGlobal-> getHero();
    if(pMainEntity == NULL)
    {
        return ;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return ;
    }

    ISchemeSpellSlot* pSpellSlot =pSchemeCenter->getSchemeSpellSlot();
    if(pSpellSlot == NULL)
    {
        return ;

    }

    ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
    if(pSpellFactory== NULL )
    {
        return;
    }

    cmd_match_room_Mouse_HeroArea* pCmd = (cmd_match_room_Mouse_HeroArea*)ptrParam;
	int nHeroID = pCmd->nHeroVocationID;
    SPELL_SLOT* pSpellSlotData=pSpellSlot->getSpellSlot(nHeroID);
    if(pSpellSlotData==NULL)
    {
        return;
    }

    obuf obdata;
    cmd_match_room_hero_skill_desc_count nSkillDescCount;
	nSkillDescCount.nHeroID = nHeroID;
    nSkillDescCount.nSkillDescCount=SPELL_SLOT_SHOW_COUNT;
    obdata.push_back(&nSkillDescCount,sizeof(cmd_match_room_hero_skill_desc_count));
    for (int i=0;i<SPELL_SLOT_SHOW_COUNT;i++)
    {
        int nSpellID= pSpellSlotData->nSpellID[i];
        SPELL_DATA * pSpellData = pSpellFactory->getSpellData(nSpellID);
        if(pSpellData==NULL)
        {
            continue;
        }

        cmd_match_room_hero_skill_desc cmdHeroSkillDesc;
        cmdHeroSkillDesc.nSpellIconID=pSpellData->nIcon;
        cmdHeroSkillDesc.nSlotID=i;
        sstrcpyn(cmdHeroSkillDesc.heroSkillDes, pSpellSlotData->nSpellDesc[i], sizeof(cmdHeroSkillDesc.heroSkillDes));
        obdata.push_back(&cmdHeroSkillDesc, sizeof(cmd_match_room_hero_skill_desc ));
    }

    pMainEntity->sendCommandToEntityView(ENTITY_TOVIEW_MATCH_ROOM_SHOW_SKILL_DESC,0,0,obdata.data(), obdata.size());
}

void RoomState_Fighting::release()
{
    return;
}

void RoomState_Fighting::restoreState()
{
    return;
}
