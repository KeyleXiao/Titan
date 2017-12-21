#include "stdafx.h"
#include "net/net.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "IRenderView.h"
#include "NetMessageDef.h"
#include "MatchDef.h"
#include "Stage_ManagedDef.h"
#include "EntityViewDef.h"
#include "MigrateDef.h"
#include "IBankPart.h"
#include "IMatchClient.h"
#include "Stage_ManagedDef.h"
#include "IMiscManager.h"

#include "CurrentMatchRoomInfo.h"

//////////////////////////////////////////////////////////////////////////
#include <iterator>  
cmd_Type_data TransferSpellToCmd (SSkinSpell param)
{
    cmd_Type_data data;
    data.nID = param.nSkinID;                    // 皮肤ID
    data.nType = param.nType;                  // 类型ID
    data.nOldValue = param.nOldValue;              // 旧数据
    data.nNewValue = param.nNewValue;              // 新数据

    return data;
}

//////////////////////////////////////////////////////////////////////////
CurrentMatchRoomInfo::CurrentMatchRoomInfo():StateManage(MatchRoomStates)
{
	newState( new RoomState_Wait(this));
	newState( new RoomState_SelectHero(this));
	newState( new RoomState_Fighting(this));	
	newState( new RoomState_ReadyGo(this));
}

CurrentMatchRoomInfo::~CurrentMatchRoomInfo()
{
    StateManage::release();
}

void CurrentMatchRoomInfo::inital( void * context,int len, bool bGuidRoom )
{
    memset(&m_RoomData, 0, sizeof(m_RoomData));
    
    m_SceneID = INVALID_SCENE_ID;

	if ( context!=NULL && len!=0 )
	{
		initFromMatch( context,len );
	}

    //引导房间不用显示出来
    if(!bGuidRoom)
    {
        switch ( m_nFromWhere )
        {
        case GVIEWCMD_MATCH_ENTER_ROOM:
            {
                showRoom();
            }
            break;
        }
    }

    // 初始化记录下哪些位置能ban选玩家
    initBanHeroInfo();

    // 更新下自己ban选标示
    updateSelfCanBanHero();
}

void CurrentMatchRoomInfo::release()
{
    TraceLn(__FUNCTION__);
    memset(&m_RoomData, 0, sizeof(m_RoomData));
    m_vctRoles.clear();
    m_SceneID = 0;
    m_BanHeroPos.clear();
    m_vctOBRoles.clear();
    m_nFromWhere;
}

void CurrentMatchRoomInfo::onMatchMessage( SNetMsgHead* head, void* data, size_t len )
{
	getState()->onServerMsg(head, data,len);
}

void CurrentMatchRoomInfo::initFromMatch( void* data,size_t len )
{	
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    if (pMatchClient == NULL)
    {
        ErrorLn(__FUNCTION__": pMatchClient == NULL");
        return;
    }
	if (len < sizeof(SMsgSelfEnterRoomData))
	{
		ErrorLn(__FUNCTION__": SMsgSelfEnterRoomData len= " << len);
		return;
	}
	SMsgSelfEnterRoomData* pMsg = (SMsgSelfEnterRoomData*)data;
	data = (void*)(pMsg + 1);
	len -= sizeof(SMsgSelfEnterRoomData);
	MATCH_TRACELN(__FUNCTION__": RoomID=" << pMsg->m_RoomID << " RoleCount=" << pMsg->m_RoleCount);

	m_RoomData = *pMsg;

	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom m_RoomData.m_RoomID= " << m_RoomData.m_RoomID);
		return;
	}

	m_vctRoles.clear();
	m_vctRoles.resize( pMatchClient->getPlayerCountMax(pRoom->dwMatchTypeID));

	m_vctOBRoles.clear();
	m_vctOBRoles.resize(pMatchClient->getObCountMax(pRoom->dwMatchTypeID));

	int nSelfPos = m_RoomData.m_SelfPos;
    if (m_RoomData.m_bIsOBEnter)
    {
        // OB位置
        m_vctOBRoles[ nSelfPos ].dwPDBID  = m_RoomData.m_PDBID;
        m_vctOBRoles[ nSelfPos ].nIndex   = nSelfPos;
        m_vctOBRoles[ nSelfPos ].dwHeroID	= m_RoomData.m_SelfHeroID;
        m_vctOBRoles[ nSelfPos ].bySex    = m_RoomData.m_bySex;
        m_vctOBRoles[ nSelfPos ].nHeroXPSkillID = m_RoomData.m_nHeroXPSkill;
        memcpy(m_vctOBRoles[nSelfPos].nSummonserSkill, m_RoomData.m_SummonerSKill, sizeof(m_vctOBRoles[nSelfPos].nSummonserSkill));
        strcpy_s(m_vctOBRoles[ nSelfPos ].szName, sizeof(m_vctOBRoles[ nSelfPos ].szName), m_RoomData.m_szName);
    }
    else
    {
        m_vctRoles[ nSelfPos ].dwPDBID  = m_RoomData.m_PDBID;
        m_vctRoles[ nSelfPos ].nIndex   = nSelfPos;
        m_vctRoles[ nSelfPos ].dwHeroID	= m_RoomData.m_SelfHeroID;
        m_vctRoles[ nSelfPos ].bySex    = m_RoomData.m_bySex;
        m_vctRoles[ nSelfPos ].nHeroXPSkillID = m_RoomData.m_nHeroXPSkill;
        memcpy(m_vctRoles[nSelfPos].nSummonserSkill, m_RoomData.m_SummonerSKill, sizeof(m_vctRoles[nSelfPos].nSummonserSkill));
        strcpy_s(m_vctRoles[ nSelfPos ].szName, sizeof(m_vctRoles[ nSelfPos ].szName), m_RoomData.m_szName);
    }
	
	for (int i = 0; i < pMsg->m_RoleCount; ++i)
	{
		SRoomRoleBase* pMsgRoomRole = (SRoomRoleBase*)data;
		data = (void*)(pMsgRoomRole + 1);
		len -= sizeof(SRoomRoleBase);

		m_vctRoles[ pMsgRoomRole->nIndex ] = *pMsgRoomRole;
	}

    for (int i = 0; i < pMsg->m_OBRoleCount; ++i)
    {
        SRoomRoleBase* pMsgRoomRole = (SRoomRoleBase*)data;
        data = (void*)(pMsgRoomRole + 1);
        len -= sizeof(SRoomRoleBase);

        m_vctOBRoles[ pMsgRoomRole->nIndex ] = *pMsgRoomRole;
    }

	m_SceneID = 0;
	pRoom->byState = MatchRoomState_Wait;
	m_nFromWhere = GVIEWCMD_MATCH_ENTER_ROOM;

    //初始化进入房间状态
    toState(pRoom->byState);
}

void CurrentMatchRoomInfo::showRoom()
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    if (pMatchClient == NULL)
    {
        ErrorLn(__FUNCTION__": pMatchClient == NULL");
        return;
    }

	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << m_RoomData.m_RoomID);
		return;
	}

	uint32 u32CampPlayerCount	= pMatchClient->getCampPlayerCountMax( pRoom->dwMatchTypeID );
	{// 房间数据
		cmd_match_hall_add_room cmdMatchHallAddRoom;
		pMatchClient->make(pRoom, cmdMatchHallAddRoom);
		gClientGlobal->getRenderView()->sendControllerCommand(m_nFromWhere, 0, 0, &cmdMatchHallAddRoom, sizeof(cmdMatchHallAddRoom));
	}

	cmd_match_room_role_data cmdMatchRoomRoleData;
	cmd_match_room_ob_data cmdMatchRoomObData;

	if (m_RoomData.m_bIsOBEnter)
	{
		// 进入大厅房间 view显示 自己Ob数据	
		MakeObDataCmdSelf( cmdMatchRoomObData );
		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_ROOM_SELF_OB, 0, 0, &cmdMatchRoomObData, sizeof(cmdMatchRoomObData));
		MATCH_TRACELN("CurrentMatchRoomInfo::showRoom GVIEWCMD_MATCH_ENTER_ROOM_SELF_OB name=" << cmdMatchRoomObData.szPlayerName);
	}
	else	
	{
		// 进入大厅房间 view显示 自己角色数据	
		MakeRoleDataCmdSelf( cmdMatchRoomRoleData );
		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_ROOM_SELF, 0, 0, &cmdMatchRoomRoleData, sizeof(cmdMatchRoomRoleData));
		MATCH_TRACELN("CurrentMatchRoomInfo::showRoom GVIEWCMD_MATCH_ENTER_ROOM_SELF name=" << cmdMatchRoomRoleData.szPlayerName);
	}

	{// 进入房间 view显示 除自己外其他角色数据
		MATCH_TRACELN(__FUNCTION__": PlayerCount= " << m_vctRoles.size());

		for (size_t i = 0; i < m_vctRoles.size(); ++i)
		{
			if (Invalid_MatchRoomPos == m_vctRoles[i].nIndex)
			{
				continue;
			}
			if (m_RoomData.m_bIsOBEnter == false && m_RoomData.m_SelfPos == m_vctRoles[i].nIndex)
			{
				continue;
			}
			MakeRoleDataCmd(&m_vctRoles[i], cmdMatchRoomRoleData);
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_ROOM_OTHER, 0, 0, &cmdMatchRoomRoleData, sizeof(cmdMatchRoomRoleData));
			MATCH_TRACELN("CurrentMatchRoomInfo::showRoom GVIEWCMD_MATCH_ENTER_ROOM_OTHER name=" << cmdMatchRoomRoleData.szPlayerName);
		}
	}

	{// 进入房间 view显示 除自己外其他Ob数据
		MATCH_TRACELN(__FUNCTION__": ObCount= " << m_vctOBRoles.size());

		for (size_t i = 0; i < m_vctOBRoles.size(); ++i)
		{
			if (Invalid_MatchRoomPos == m_vctOBRoles[i].nIndex)
			{
				continue;
			}
			if (m_RoomData.m_bIsOBEnter == true && m_RoomData.m_SelfPos == m_vctOBRoles[i].nIndex)
			{
				continue;
			}
			MakeObDataCmd(&m_vctOBRoles[i], cmdMatchRoomObData);
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_ROOM_OTHER_OB, 0, 0, &cmdMatchRoomObData, sizeof(cmdMatchRoomObData));
			MATCH_TRACELN("CurrentMatchRoomInfo::showRoom GVIEWCMD_MATCH_ENTER_ROOM_OTHER_OB name=" << cmdMatchRoomObData.szPlayerName);
		}
	}

	
	// 发送系统和自己英雄槽位信息到显示层
	SendRoomHeroSlotToView();      

	// 进入房间 显示所有玩家的召唤师技能
	sendAllPlayerSmSkillInfo();

	{// 进入房间 禁用机器已选择英雄
		for (size_t i = 0; i < m_vctRoles.size();++i)
		{
			if (m_vctRoles[i].nIndex == Invalid_MatchRoomPos)
				continue;
			if (m_RoomData.m_bIsOBEnter == false && m_vctRoles[i].nIndex == m_RoomData.m_SelfPos)
				continue;
			if (!m_vctRoles[i].isComputerPlayer)
				continue;

			cmd_match_room_computer_hero_can_select cmdCanSelectData;
			cmdCanSelectData.nCamp = getPlayerCamp(m_vctRoles[i].nIndex);
			cmdCanSelectData.bCanSelect = 0;
			cmdCanSelectData.nHeroID = m_vctRoles[i].dwHeroID;

			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CAN_SELECT, 0, 0, &cmdCanSelectData, sizeof(cmdCanSelectData));
		}
	}

	//SetRoomHeroInfo( ActorClient::getInstance().getFirstHeroID());
}

void CurrentMatchRoomInfo::swapPos(int pos1, int pos2)
{
	std::swap(m_vctRoles[pos1], m_vctRoles[ pos2]);
}

SRoomRoleBase* CurrentMatchRoomInfo::getRoleByPos( uchar Pos )
{
    if (Pos >= m_vctRoles.size())
    {
        return nullptr;
    }
	return &m_vctRoles[ Pos ];
}

SRoomRoleBase* CurrentMatchRoomInfo::getObRoleByPos(uchar Pos)
{
    if (Pos >= m_vctOBRoles.size())
    {
        return nullptr;
    }
	return &m_vctOBRoles[Pos];
}


rkt::uint32 CurrentMatchRoomInfo::getPlayerCamp( SRoomRoleBase* pRole )
{
	if (nullptr == pRole)
	{
		ErrorLn(__FUNCTION__": nullptr == pRole  ");
		return Invalid_MatchRoomCamp;
	}
	return getPlayerCamp( pRole->nIndex );
}

rkt::uint32 CurrentMatchRoomInfo::getPlayerCamp( uchar Pos )
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom m_RoomData.m_RoomID= " << m_RoomData.m_RoomID);
		return Invalid_MatchRoomCamp;
	}

	uint32 u32CampPlayerCount	= pMatchClient->getCampPlayerCountMax( pRoom->dwMatchTypeID);
	return (Pos / u32CampPlayerCount);
}

rkt::uint32 CurrentMatchRoomInfo::getPlayerCampPos( uchar Pos )
{
	// todo:pt 等建聪弄好才能用新的
	//return Pos;

	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn("CurrentMatchRoomInfo::getPlayerCampPos nullptr == pRoom m_RoomData.m_RoomID= " << m_RoomData.m_RoomID);
		return Invalid_MatchRoomPos;
	}

	uint32 u32CampPlayerCount	= pMatchClient->getCampPlayerCountMax( pRoom->dwMatchTypeID );
	return (Pos % u32CampPlayerCount);
}

rkt::uint32 CurrentMatchRoomInfo::getPlayerCampPos( SRoomRoleBase* pRole )
{
	if (nullptr == pRole)
	{
		ErrorLn(__FUNCTION__": nullptr == pRole  ");
		return Invalid_MatchRoomPos;
	}
	return getPlayerCampPos( pRole->nIndex );
}

SRoomRoleBase* CurrentMatchRoomInfo::getPlayerRoomBaseInfo(PDBID pDbid)
{
	VCT_Roles::iterator itBegin = m_vctRoles.begin();
	for (; itBegin != m_vctRoles.end(); ++itBegin )
	{
		if (itBegin->dwPDBID == pDbid )
		{
			return &(*itBegin);
		}
	}
	return NULL;
}

bool CurrentMatchRoomInfo::isMaster( uchar Pos )
{
	return (Pos == m_RoomData.m_MasterPos);
}

bool CurrentMatchRoomInfo::isMaster( SRoomRoleBase* pRole )
{

	if (nullptr == pRole)
	{
		ErrorLn(__FUNCTION__": nullptr == pRole  ");
		return false;
	}
	return isMaster( pRole->nIndex );
}

bool CurrentMatchRoomInfo::isMaster()
{
	return (m_RoomData.m_MasterPos== m_RoomData.m_SelfPos);
}

void CurrentMatchRoomInfo::MakeRoleDataCmd( SRoomRoleBase* pRole, cmd_match_room_role_data &cmdMatchRoomRoleData )
{
	if (nullptr == pRole)
	{
		ErrorLn(__FUNCTION__": nullptr == pRole  ");
		return;
	}

	cmdMatchRoomRoleData.nPlayerSlotID	= getPlayerCampPos( pRole->nIndex);
	cmdMatchRoomRoleData.nCamp			= getPlayerCamp( pRole->nIndex );
	cmdMatchRoomRoleData.bIsMaster		= (int)(isMaster( pRole->nIndex ));
	cmdMatchRoomRoleData.nHeroID		= pRole->dwHeroID;
	cmdMatchRoomRoleData.bIsComputerPlayer = (int)pRole->isComputerPlayer;
	strcpy_s(cmdMatchRoomRoleData.szPlayerName, sizeof(cmdMatchRoomRoleData.szPlayerName), pRole->szName);
}

void CurrentMatchRoomInfo::MakeRoleDataCmdSelf( cmd_match_room_role_data &cmdMatchRoomRoleData )
{

	cmdMatchRoomRoleData.nPlayerSlotID	= getPlayerCampPos( m_RoomData.m_SelfPos ); 
	cmdMatchRoomRoleData.nCamp			= getPlayerCamp( m_RoomData.m_SelfPos );
	cmdMatchRoomRoleData.bIsMaster		= isMaster( m_RoomData.m_SelfPos );
	cmdMatchRoomRoleData.nHeroID		= 0;
	cmdMatchRoomRoleData.bIsOb			= m_RoomData.m_bIsOBEnter ? 1 : 0;

	strcpy_s(cmdMatchRoomRoleData.szPlayerName, sizeof(cmdMatchRoomRoleData.szPlayerName), m_RoomData.m_szName);
}

void CurrentMatchRoomInfo::MakeObDataCmd( SRoomRoleBase* pRole, cmd_match_room_ob_data &cmdMatchRoomRoleData )
{
	if (nullptr == pRole)
	{
		ErrorLn(__FUNCTION__": nullptr == pRole  ");
		return;
	}

	cmdMatchRoomRoleData.nObPos = pRole->nIndex;
	cmdMatchRoomRoleData.nActorID = pRole->dwPDBID;
	//cmdMatchRoomRoleData.nHeadID = 0;
	cmdMatchRoomRoleData.nSex = (int)pRole->bySex;
	strcpy_s(cmdMatchRoomRoleData.szPlayerName, sizeof(cmdMatchRoomRoleData.szPlayerName), pRole->szName);
}
void CurrentMatchRoomInfo::MakeObDataCmdSelf( cmd_match_room_ob_data &cmdMatchRoomRoleData )
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		ErrorLn(__FUNCTION__": pClientEntity == NULL");
		return;
	}

	cmdMatchRoomRoleData.nObPos = m_RoomData.m_SelfPos;
	cmdMatchRoomRoleData.nActorID = m_RoomData.m_PDBID;
	//cmdMatchRoomRoleData.nHeadID = 0;
	cmdMatchRoomRoleData.nSex = (int)m_RoomData.m_bySex;
	strcpy_s(cmdMatchRoomRoleData.szPlayerName, sizeof(cmdMatchRoomRoleData.szPlayerName), m_RoomData.m_szName);
}

uchar CurrentMatchRoomInfo::getPos( uint32 u32Camp, uint32 u32CampPos )
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << m_RoomData.m_RoomID);
		return Invalid_MatchRoomPos;
	}
	if (u32Camp >= pMatchClient->getCampCount( pRoom->dwMatchTypeID ))
	{
		ErrorLn(__FUNCTION__": u32Camp= " << u32Camp);
		return Invalid_MatchRoomPos;
	}
	uint32 u32CampPlayerCount	= pMatchClient->getCampPlayerCountMax( pRoom->dwMatchTypeID );
	if (u32CampPos >= u32CampPlayerCount)
	{
		ErrorLn("CurrentMatchRoomInfo::getPos u32CampPos= " << u32CampPos);
		return Invalid_MatchRoomPos;
	}

	return static_cast<uchar>(u32Camp * u32CampPlayerCount + u32CampPos);
}

// 获取房间类型
BYTE CurrentMatchRoomInfo::getRoomType()
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << m_RoomData.m_RoomID);
		return HallRoomType_Unknown;
	}

	return pRoom->nRoomType;
}

void CurrentMatchRoomInfo::initFromGame()
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << m_RoomData.m_RoomID);
		return;
	}
	if (MatchRoomState_Fighting != pRoom->byState)
	{
		ErrorLn(__FUNCTION__": MatchRoomState_Wait != pRoom->m_byState RoomID= " << m_RoomData.m_RoomID<<"byState= " << pRoom->byState);
		return;
	}
	pRoom->byState = MatchRoomState_Wait;
	m_nFromWhere = GVIEWCM_MATCH_OVER_ENTER_ROOM;
}



rkt::uint32 CurrentMatchRoomInfo::getCampRoleCount( int nCamp, uint32 u32CampRoleMax )
{
	uint32 u32Count = 0;
	int nBegin	= (nCamp - 1) * u32CampRoleMax;
	int nEnd	= nBegin + u32CampRoleMax;
	for (int j = nBegin; j < nEnd; ++j)
	{
		if (Invalid_MatchRoomPos != m_vctRoles[j].nIndex)
		{
			++u32Count;
		}
	}
	return u32Count;
}

rkt::uint32 CurrentMatchRoomInfo::getCampRoleCount( int nCamp, SSchemeMatchRoom* pSchemeMatchRoom )
{
	return getCampRoleCount(nCamp, pSchemeMatchRoom->m_CampRoleMax);
}

rkt::uint32 CurrentMatchRoomInfo::getCampRoleCount( int nCamp )
{
	return getCampRoleCount(nCamp, getSchemeMatchRoom());
}

SSchemeMatchRoom* CurrentMatchRoomInfo::getSchemeMatchRoom()
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    if (pMatchClient == NULL)
    {
        return NULL;
    }
	SMsgCreateRoom_OC* pRoomData = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoomData)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoomData m_RoomData.m_RoomID=" << m_RoomData.m_RoomID);
		return nullptr;
	}
	return pMatchClient->getSchemeMatchRoomByMatchType(pRoomData->dwMatchTypeID);
}


char const* CurrentMatchRoomInfo::getTextInfo( int nID )
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	return pMatchClient->getTextInfo( nID );
}


SRoomRoleBase* CurrentMatchRoomInfo::getSelf()
{
	return getRoleByPos( m_RoomData.m_SelfPos );
}



void CurrentMatchRoomInfo::playerExit(bool bIsOb, uchar Pos, int nCmd)
{
	SRoomRoleBase* pRole = NULL;
	if (bIsOb)
	{
		pRole = getObRoleByPos( Pos );
	}
	else
	{
		pRole = getRoleByPos( Pos );
	}
	if (Invalid_MatchRoomPos == pRole->nIndex)
	{
		ErrorLn(__FUNCTION__": Invalid_MatchRoomPos == pRole->m_Pos Pos= " << Pos);
		return;
	}

	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom m_RoomData.m_RoomID= " << m_RoomData.m_RoomID);
		return;
	}

	pRole->nIndex = Invalid_MatchRoomPos;

	cmd_match_room_hero_exit cmdMatchRoomHeroExit;
	cmdMatchRoomHeroExit.bIsOb	= bIsOb;
	cmdMatchRoomHeroExit.nPos	= Pos;
	gClientGlobal->getRenderView()->sendControllerCommand(nCmd, 0, 0, &cmdMatchRoomHeroExit, sizeof(cmdMatchRoomHeroExit));

	if (MatchRoomState_Wait != pRoom->byState && pRoom->byState < MatchRoomState_Fighting)
	{// 中断开始
		pRoom->byState = MatchRoomState_Wait;
		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCM_MATCH_PLAYER_EXIT_ROOM_STATE_2_WAIT, 0, 0, 0, 0);
	}
}

void CurrentMatchRoomInfo::SetRoomRole( int nIndex, SRoomRoleBase sRoomRole)
{
	m_vctRoles[nIndex] = sRoomRole;
}



void CurrentMatchRoomInfo::SetOBRoomRole(int nIndex, SRoomRoleBase sRoomRole)
{
    m_vctOBRoles[nIndex] = sRoomRole;
}


SMsgSelfEnterRoomData* CurrentMatchRoomInfo::getRoomData()
{
	return &m_RoomData;
}

void CurrentMatchRoomInfo::loadSenece(DWORD dwMapID, Vector3 vecBornPos, bool bShowSelf)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (NULL == pSchemeCenter)
	{
		ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
		return;
	}

	ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
	if (NULL == pMatchRankConfigInfo)
	{
		ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
		return;
	}

	ISchemeComputerRankConfigInfo *pComputerRankConfigInfo =  pSchemeCenter->getSchemeComputerRankConfig();
	if (NULL == pComputerRankConfigInfo)
	{
		ErrorLn(__FUNCTION__": NULL == pComputerRankConfigInfo");
		return;
	}
	
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    if (NULL == pMatchClient)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchClient");
        return;
    }
	SMsgCreateRoom_OC*  pRoomData = pMatchClient->getRoomData( m_RoomData.m_RoomID );
	if (nullptr == pRoomData)
	{
		ErrorLn(__FUNCTION__":  pRoomData==null, room=" << m_RoomData.m_RoomID);
		return;
	}
	SMapSchemeInfo*  pSchemeMap = pMatchClient->getSchemeMap( pRoomData->dwMapID );
	if (nullptr == pSchemeMap)
	{
		ErrorLn(__FUNCTION__":  pSchemeMap == nullptr, mapID=" << pRoomData->dwMapID);
		return;
	}
    ISchemeSkinSpell *pSchemeSkinSpell = pSchemeCenter->getSchemeSkinSpell();
    if (nullptr == pSchemeSkinSpell)
    {
        ErrorLn(__FUNCTION__":  pSchemeSkinSpell == nullptr, mapID=" << pRoomData->dwMapID);
        return;
    }

    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__":  nullptr == pSchemeMatchRoom, roomID=" << m_RoomData.m_RoomID);
        return;
    }

    if (pSchemeMatchRoom->m_CampCount <= 0)
    {
        ErrorLn(__FUNCTION__":  pSchemeMatchRoom->m_CampCount <= 0, matchType=" << pSchemeMatchRoom->m_dwMatchTypeID);
        return;
    }

    if (pSchemeMatchRoom->m_dwMatchTypeID == MatchType_MultiCampMatch)
    {
        bShowSelf = true;
    }

    IRenderViewProxy * pRenderView = gClientGlobal->getRenderView();
    if (nullptr == pRenderView)
    {
        ErrorLn(__FUNCTION__":  pRenderView == nullptr, mapID=" << pRoomData->dwMapID);
        return;
    }

	if (MatchRoomState_Fighting != pRoomData->byState)
	{
		pRoomData->byState = MatchRoomState_Fighting;
	}

	m_SceneID = dwMapID;

	obuf obLoadPlayerData;
	cmd_Camp_Count cmdCampCount; 
    cmdCampCount.nMapID             = pRoomData->dwMapID;					// 地图ID
    cmdCampCount.nMatchType			= pSchemeMatchRoom->m_dwMatchTypeID;	// 战场类型 
	cmdCampCount.bObEnter			= m_RoomData.m_bIsOBEnter ? 1 : 0;		// 当前是否Ob玩家
	int nCampPlayerNum = m_vctRoles.size()/2;
	if(bShowSelf)
	{
		VCT_Roles::iterator iter = m_vctRoles.begin();
		for ( ;iter!=m_vctRoles.end(); ++iter)
		{
			if (iter->nIndex != Invalid_MatchRoomPos)
			{
				++cmdCampCount.vCampCount[iter->nIndex/pSchemeMatchRoom->m_CampRoleMax];
			}
		}
	}
	else
	{
		VCT_Roles::iterator iter = m_vctRoles.begin();
		for ( ;iter!=m_vctRoles.end(); ++iter)
		{
			if (iter->nIndex != Invalid_MatchRoomPos)
			{
				if (iter->nIndex < nCampPlayerNum)
				{
					++cmdCampCount.vCampCount[0];
				} 
				else
				{
					++cmdCampCount.vCampCount[1];
				}
			}
		}
	}

	obLoadPlayerData.push_back(&cmdCampCount,sizeof(cmdCampCount));

	int index[2] = {};
    BYTE nRoomType = getRoomType();
	VCT_Roles::iterator iter = m_vctRoles.begin();	
	for ( ;iter!=m_vctRoles.end(); iter++)
	{
		SRoomRoleBase & role = (*iter);

        if (role.nIndex == Invalid_MatchRoomPos)
        {
            if (role.dwPDBID != INVALID_PDBID)
            {
                WarningLn("Load Scene find player's index is invalid! szName="<<role.szName<<", hero="<<role.dwHeroID);
            }
            continue;
        }

		// 多阵营单独计算
        int nCampIndex = role.nIndex/nCampPlayerNum == 0 ? 0 : 1;
        int nCampPosIndex = index[nCampIndex]++;
        int nSelfCampIndex = m_RoomData.m_SelfPos/nCampPlayerNum == 0 ? 0 : 1;
		if (bShowSelf)
		{
            nCampIndex = role.nIndex / pSchemeMatchRoom->m_CampRoleMax;
            nCampPosIndex = role.nIndex % pSchemeMatchRoom->m_CampRoleMax;
            nSelfCampIndex = m_RoomData.m_SelfPos / pSchemeMatchRoom->m_CampRoleMax;
		}
		// Ob玩家阵营暂定为-1
		if (m_RoomData.m_bIsOBEnter)
		{
			nSelfCampIndex = -1;
		}

		cmd_match_load_scene_playerinfo playerInfo;
		playerInfo.idActor          = role.dwPDBID;
		playerInfo.nCamp            = nCampIndex;
		playerInfo.nHeroID          = role.dwHeroID;
		playerInfo.nSkinID          = role.dwSkinID;
		playerInfo.nCampIndex       = nCampPosIndex;
		playerInfo.nSelfCamp        = nSelfCampIndex;
        playerInfo.nLoadSceneRate   = (role.isComputerPlayer) ? 100 : 0;
        playerInfo.nIsComputer      = role.isComputerPlayer;
		playerInfo.nXPSkillID		= role.nHeroXPSkillID;
		playerInfo.nSelfInfo		= role.dwPDBID == m_RoomData.m_PDBID ? 1 : 0;

		for (int i = 0; i < SUMMONER_SKILL_COUNT; ++i ) {
			playerInfo.nSupportSkill[i] = getSummonerSkillIconId(role.nSummonserSkill[i]);
		}

        if (role.isComputerPlayer)  
        {            
            if (nRoomType == HallRoomType_Custom || nRoomType == HallRoomType_MacthMachine)
            {
                // 电脑玩家名字处理
                string strName = role.szName;
                string strSub;
                ISchemeAIIDTable *pSchemeAIIDTable = pSchemeCenter->getSchemeAIIDTable();
                if (pSchemeAIIDTable != NULL)
                {
                    string AIDes = pSchemeAIIDTable->getHeroAIDes(pRoomData->nConfigWarID, iter->dwHeroID, iter->nAIID);
                    if (AIDes.size() != 0)
                    {
                        strSub = "(" + AIDes + ")";
                    }
                }
                    
                if (string::npos == strName.find(strSub.data(),0))
                {
                    strName += strSub;
                }
                sstrcpyn(role.szName, strName.data(), sizeof(role.szName));

            }
        }
        sstrcpyn(playerInfo.szPlayerName, role.szName, sizeof(playerInfo.szPlayerName) );

		SPELL_SLOT *sTempSlot = g_EHelper.getSpellSlot(iter->dwHeroID);
		if (sTempSlot != NULL)
		{
            sstrcpyn(playerInfo.szVocationName, sTempSlot->szVocation, sizeof(playerInfo.szVocationName));
		}

        //////////////////////////////////////////////////////////////////////////
		// 获得这个英雄的所有皮肤id
        if(role.dwSkinID == 0)
        {
            getHeroDefaultSkin(role.dwHeroID, role.dwSkinID );
            playerInfo.nSkinID = role.dwSkinID;
        }

		if (role.isComputerPlayer)
		{
			// 电脑取构造虚拟数据
			const SComputerRankConfigSchemeInfo *pComputerRank =  pComputerRankConfigInfo->getComputerRankConfigShemeInfo();
			if(pComputerRank != NULL)
			{
				playerInfo.cmdPlayerVsDetails.nSex = pComputerRank->bySex;
				playerInfo.cmdPlayerVsDetails.fHeroWinRate = pComputerRank->fWinRate;
				playerInfo.cmdPlayerVsDetails.nHeroGameNum = pComputerRank->nMatchNum;

				// 获得段位信息
				DWORD dwMatchType = getSchemeMatchRoom()->m_dwMatchTypeID;
				const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(dwMatchType, pComputerRank->nMatchRankGrade);
				if (pSchemeInfo != NULL)
				{
					sstrcpyn( playerInfo.cmdPlayerVsDetails.szGradeName, pSchemeInfo->szDes, sizeof(playerInfo.cmdPlayerVsDetails.szGradeName));
				}
			}
		}
		else
		{
			// 人物正常流程
			if (VOCATION_INVALID < role.dwHeroID && role.dwHeroID < VOCATION_MAX)
			{
				playerInfo.cmdPlayerVsDetails.nSex = role.bySex;
				playerInfo.cmdPlayerVsDetails.fHeroWinRate = role.fWinRate;
				playerInfo.cmdPlayerVsDetails.nHeroGameNum = role.nMatchNum;
			}

			//// 获得段位信息
			const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(MatchType_Rank, role.nRankGrade);
			if (pSchemeInfo != NULL)
			{
				sstrcpyn( playerInfo.cmdPlayerVsDetails.szGradeName, pSchemeInfo->szDes, sizeof(playerInfo.cmdPlayerVsDetails.szGradeName));
			}
		}


		obLoadPlayerData.push_back(&playerInfo, sizeof(playerInfo));
	}

	pRenderView->sendControllerCommand(GVIEWCMD_MATCH_LOAD_SCENE_PLAYERINFO, 0, NULL, obLoadPlayerData.data(), obLoadPlayerData.size());
    
	// 加载地图
    gClientGlobal->getMiscManager()->loadScene(GVIEWCMD_MATCH_LOAD_SCENE, pRoomData->dwMapID, vecBornPos.x, vecBornPos.y, vecBornPos.z);
}

void CurrentMatchRoomInfo::SendRoomHeroSlotToView()
{

	ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return ;
	}

	ISchemePersonModel *pSchemePersonModel = pSchemeCenter->getSchemePersonModel();
	if (pSchemePersonModel == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemePersonModel == NULL");
		return ;
	}

	SSchemeMatchRoom*	pSchemeRoom = getSchemeMatchRoom();
	if (pSchemeRoom == NULL)
	{
		ErrorLn(__FUNCTION__": pScheme == NULL");
		return;
	}

	//房间全局禁用英雄
    map<int, bool> mapRoomGlobleBan;
    EGlobleSelectFlag  eSelectFlag = (EGlobleSelectFlag)pSchemeRoom->m_byGlobleSelectFlag;
    switch(eSelectFlag)
    {
    case SGS_GlobleBan:
        {
            // 全局禁用
            for (int i = 0; i < MAX_SELECT_FLAG_COUT; ++i)
            {
                int heroID = pSchemeRoom->m_SelectFlagHeroList[i];
                if (heroID > 0)
                {
                    // 插入到全局禁用列表
                    mapRoomGlobleBan[heroID] = false;
                }
            }
        }
        break;
    case SGS_GlobleOnlyCanUse:
        {
            // 全局只能用
            for (int nVocation = VOCATION_INVALID; nVocation < VOCATION_MAX; ++nVocation)
            {
                // 插入到全局禁用列表
                mapRoomGlobleBan[nVocation] = true;
                
            }

            for (int i = 0; i < MAX_SELECT_FLAG_COUT; ++i)
            {
                int heroID = pSchemeRoom->m_SelectFlagHeroList[i];
                if (heroID > 0)
                {
                    mapRoomGlobleBan.erase(heroID);
                }
            }

        }
        break;
    default:
        break;
    }

	// 原接口：
	// 1.下发拥有英雄
	// 2.下发未拥有英雄
	// 3.过滤掉Ban选英雄
	// 现接口：
	// 1.只下发Ban选英雄
    /*ISchemeHeroFreeWeekly *pSchemeHeroWeekFree = pSchemeCenter->getSchemeHeroFreeWeekly();
    if (pSchemeHeroWeekFree	== NULL)
    {
        ErrorLn(__FUNCTION__": pSchemeHeroWeekFree == NULL");
        return ;
    }*/
    /*IClientEntity* pClientEntity = gClientGlobal->getHero();
    if( pClientEntity == NULL )
    {
    ErrorLn(__FUNCTION__": pClientEntity == NULL");
    return;
    }

    IBankPart* pBankPart = (IBankPart*)pClientEntity->getEntityPart(PART_BANK);
    if( pBankPart == NULL )
    {
    ErrorLn(__FUNCTION__": pBankPart == NULL");
    return;
    }*/
	//// 自己英雄数量
	//WORD nMyHeroCounts;
	//// 系统可选择英雄(目前未做限制处理，暂时为全英雄)
	//WORD nSystemHerosCount;

	//SHeroInfo sHeroInfoArr[512];
	//nMyHeroCounts = pBankPart->getPlayerCanUseHeroArray( sHeroInfoArr, 512);
	//int* pHeroID = pBankPart->getSystemHeros(nSystemHerosCount);

	//// 总英雄数量（系统加自己的英雄）
	//int nPlayerLv = gClientGlobal->getHero()->getIntProperty(PROPERTY_LEVEL);
	//int nHerosCount = 0;

	//obuf obdatalist1;
	//SHeroInfo* pHeroInfo  = NULL;
	//for(int i=0;i<=nMyHeroCounts;i++)
	//{
	//	cmd_match_room_hero_slot cmd;
	//	if(i != 0 )
	//	{  
	//		int nHeroID = sHeroInfoArr[i-1].nHeroID;

	//		const PersonSkinModelItem* pPersonSkinModelInfo = pSchemePersonModel->getPersonSkinModelItem(nHeroID,0);
	//		if(pPersonSkinModelInfo == NULL)
	//		{
	//			continue;
	//		}

	//		if (mapRoomGlobleBan.find(nHeroID) != mapRoomGlobleBan.end())
	//		{
	//			continue;
	//		}

	//		cmd.bIsLock		= true;
	//		cmd.nHeroID		= nHeroID;
	//		cmd.nSlotIndex	= nHerosCount;
	//		cmd.nHeroType = pPersonSkinModelInfo->nType;

	//		sstrcpyn(cmd.szHeroName, pPersonSkinModelInfo->szHeroName, sizeof(cmd.szHeroName));
	//      memcpy(cmd.nSkinIDList, pPersonSkinModelInfo->nSkinIDList, sizeof(cmd.nSkinIDList) );
	//		pHeroInfo = pBankPart->getHeroInfoByID(nHeroID);
	//		cmd.nPractisedLevel = pHeroInfo ? pHeroInfo->nLevel : 1;
	//		cmd.bIsWeekFree = pSchemeHeroWeekFree->CheckIsWeekFree(nHeroID, nPlayerLv);
	//		
	//		if ((sHeroInfoArr[i-1].dwFlags & HERO_FLAG_NOT_HAVE) != HERO_FLAG_NOT_HAVE)
	//		{
	//			cmd.bCurrentIsHave = 1;
	//		}
	//		else
	//		{
	//			cmd.bCurrentIsHave = 0;
	//		}
	//		cmd.dwUnlockTime = sHeroInfoArr[i-1].dwUnlockTime;
	//	}
	//	else
	//	{
	//		cmd.bIsLock = true;
	//		cmd.nHeroID = VOCATION_RANDOM;
	//		cmd.nSlotIndex = nHerosCount;
	//		cmd.bCurrentIsHave = 1;
	//		cmd.nHeroType = -1;
	//		cmd.nPractisedLevel = 1;
	//	}

	//	obdatalist1.push_back(&cmd, sizeof(cmd_match_room_hero_slot ));
	//	nHerosCount++;
	//}

	//obuf obdatalist2;
	//// 循环下发所有系统英雄（并过滤玩家已经拥有的英雄）
	//for (int i = 0; i < nSystemHerosCount; ++i )
	//{
	//	int nHeroID = *pHeroID;
	//	pHeroID++;

	//	const PersonSkinModelItem* pPersonSkinModelInfo = pSchemePersonModel->getPersonSkinModelItem(nHeroID,0);
	//	if(pPersonSkinModelInfo == NULL)
	//	{
	//		continue;
	//	}
	//	// 拥有和周免英雄 过滤掉
	//	SHeroInfo* pPossessAndFreeHeroInfo = pBankPart->getPossessOrFreeHeroInfoByID(nHeroID);
	//	if( pPossessAndFreeHeroInfo != NULL )
	//	{
	//		continue;
	//	}

	//	if (mapRoomGlobleBan.find(nHeroID) != mapRoomGlobleBan.end())
	//	{
	//		continue;
	//	}

	//	cmd_match_room_hero_slot cmd;
	//	cmd.bIsLock = false;
	//	cmd.nHeroID = nHeroID;
	//	cmd.nSlotIndex = nHerosCount;
	//	cmd.bCurrentIsHave = 0;
	//	cmd.nHeroType = pPersonSkinModelInfo->nType;
	//	sstrcpyn(cmd.szHeroName, pPersonSkinModelInfo->szHeroName, sizeof(cmd.szHeroName));
	//     memcpy(cmd.nSkinIDList, pPersonSkinModelInfo->nSkinIDList, sizeof(cmd.nSkinIDList) );
	//	pHeroInfo = pBankPart->getHeroInfoByID(nHeroID);
	//	cmd.nPractisedLevel = pHeroInfo ? pHeroInfo->nLevel : 1;
	//	cmd.bIsWeekFree = pSchemeHeroWeekFree->CheckIsWeekFree(nHeroID, nPlayerLv);

	//	obdatalist2.push_back(&cmd, sizeof(cmd_match_room_hero_slot ));
	//    nHerosCount++;
	//}
	//obuf obdata;
	//cmd_match_room_hero_count HeroCount;
	//HeroCount.nHeroCount=nHerosCount;
	//obdata.push_back(&HeroCount,sizeof(cmd_match_room_hero_count));
	//obdata.push_back(obdatalist1.data(),obdatalist1.size());
	//obdata.push_back(obdatalist2.data(),obdatalist2.size());

	obuf obdata;

	cmd_match_room_hero_count heroCount;
	heroCount.nHeroCount = mapRoomGlobleBan.size();
	obuf obBanHero;
	map<int, bool>::iterator itBan = mapRoomGlobleBan.begin();
	for (itBan; itBan != mapRoomGlobleBan.end(); ++itBan)
	{
		int nHeroID = itBan->first;
		obBanHero.push_back(&nHeroID, sizeof(int));
	}
	obdata.push_back(&heroCount, sizeof(heroCount));
	obdata.push_back(obBanHero.data(), obBanHero.size());
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_HERO_SLOT,0,0,obdata.data(), obdata.size());
}

void CurrentMatchRoomInfo::setPDbidSummonerSKill( SMsgSummoner_Skill* pInfo )
{
	if ( pInfo == NULL || m_vctRoles.size() == 0 )
	{
		TraceLn(__FUNCTION__ ": m_vctRoles.size() = " << m_vctRoles.size());
		return;
	}

	PDBID pDbid = pInfo->pDbid;
	VCT_Roles::iterator itBegin = m_vctRoles.begin();
	for (; itBegin != m_vctRoles.end(); ++itBegin )
	{
		if (itBegin->dwPDBID == pDbid )
		{ 
			memcpy(itBegin->nSummonserSkill, pInfo->nSummonerSkill, sizeof(itBegin->nSummonserSkill));
		}
	}
}

int CurrentMatchRoomInfo::getSummonerSkillIconId( int nSkillId )
{
	ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
	if (pSpellFactory == NULL)
	{
		return 0;
	}
	SPELL_DATA *pSpellData = pSpellFactory->getSpellData(nSkillId);
	if (pSpellData == NULL)
	{
		return 0;
	}

	return pSpellData->nIcon;
}

void CurrentMatchRoomInfo::sendAllPlayerSmSkillInfo()
{
	ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
	if (pSpellFactory == NULL)
	{
		return;
	}

	VCT_Roles::iterator itBegin = m_vctRoles.begin();
	cmd_count cmdCount;
	cmdCount.nCount = m_vctRoles.size();
	obuf obData;
	obData.push_back(&cmdCount, sizeof(cmd_count));
	for (; itBegin != m_vctRoles.end(); ++itBegin )
	{
		cmd_Summoner_Skill_Change cmdData;
		cmdData.bIsSelf = m_RoomData.m_PDBID == itBegin->dwPDBID ? 1 : 0;
		cmdData.nCamp = getPlayerCamp(itBegin->nIndex);
		cmdData.nPos = itBegin->nIndex;
		for (int i = 0; i < SUMMONER_SKILL_COUNT; ++i )
		{
			SPELL_DATA *pSpellData = pSpellFactory->getSpellData(itBegin->nSummonserSkill[i]);
			if (pSpellData != NULL)
			{
				cmdData.nSummonerSkill[i] = pSpellData->nIcon;
			}
		}
		cmdData.pDbid = itBegin->dwPDBID;
		obData.push_back(&cmdData, sizeof(cmdData) );
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_SUMMONER_SET_SKILLPIC_LOGIN,0,0,obData.data(), obData.size());
}

void CurrentMatchRoomInfo::changeSummonerSkillChangePos(int nPos)
{
	ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
	if (pSpellFactory == NULL)
	{
		return;
	}

	VCT_Roles::iterator itBegin = m_vctRoles.begin();
	for (; itBegin != m_vctRoles.end(); ++itBegin )
	{
		if ( itBegin->nIndex == nPos )
		{
			cmd_Summoner_Skill_Change cmdData;
			cmdData.bIsSelf = m_RoomData.m_PDBID == itBegin->dwPDBID ? 1 : 0;
			cmdData.nCamp = getPlayerCamp(itBegin->nIndex);
			cmdData.nPos = itBegin->nIndex;
			for ( int i = 0; i < SUMMONER_SKILL_COUNT; ++i )
			{
				SPELL_DATA *pSpellData = pSpellFactory->getSpellData(itBegin->nSummonserSkill[i]);
				if (pSpellData != NULL)
				{
					cmdData.nSummonerSkill[i] = pSpellData->nIcon;
				}
			}

			cmdData.pDbid = itBegin->dwPDBID;
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_SUMMONER_BROAD_SKILL,0,0,&cmdData,sizeof(cmdData) );
			return;
		}
	}
}

void CurrentMatchRoomInfo::updateSummonerSkillInfo(int nPos, int nSkill[], int nSize)
{
	if ( nSize > SUMMONER_SKILL_COUNT )
		return;
	VCT_Roles::iterator itBegin = m_vctRoles.begin();
	for (; itBegin != m_vctRoles.end(); ++itBegin )
	{
		if ( itBegin->nIndex == nPos )
		{
			memcpy(itBegin->nSummonserSkill, nSkill, sizeof(itBegin->nSummonserSkill));
			break;
		}
	}
}

void CurrentMatchRoomInfo::getAllMonsterSkin( int nMapId, std::vector<int>& monsterSkinVec)
{
	ISchemePreloadMonsterRes* pSchemePreload = gClientGlobal->getSchemeCenter()->getSchemePreloadRes();
	ISchemeMonster* pSchemeMonster = gClientGlobal->getSchemeCenter()->getSchemeMonster();
	if ( pSchemePreload == NULL || pSchemeMonster == NULL)
	{
		return;
	}
	std::vector<int> allMonsterVec;
	pSchemePreload->getMapAllMonster(nMapId, allMonsterVec);
	std::vector<int>::iterator it = allMonsterVec.begin();
	for (; it != allMonsterVec.end(); ++it )
	{
		SMonsterScheme* pMonster = pSchemeMonster->getMonsterShemeInfo(*it);
		if( pMonster == NULL)
			continue;
		if ( monsterSkinVec.size() == 0 )
		{
			monsterSkinVec.push_back( pMonster->nDefaultSkin );
		}else
		{
			std::vector<int>::iterator itSkin = monsterSkinVec.begin();
			for (; itSkin != monsterSkinVec.end(); ++itSkin )
			{
				if ( *itSkin == pMonster->nDefaultSkin )
				{
					break;
				}
			}
			if ( itSkin != monsterSkinVec.end())
			{
				continue;
			}
			monsterSkinVec.push_back( pMonster->nDefaultSkin);
		}
	}
}

void CurrentMatchRoomInfo::getHeroDefaultSkin( int nHeroId, DWORD &wSkinID )
{
	ISchemePersonModel* pPersonScheme = gClientGlobal->getSchemeCenter()->getSchemePersonModel();
	if(pPersonScheme == NULL )
		return;

	const PersonSkinModelItem* pPersonSkinModelItem = pPersonScheme->getPersonSkinModelItem(nHeroId);
	if ( !pPersonSkinModelItem )
	{
		ErrorLn("check the scheme PersonSkinModel heroid = "<< nHeroId);
		return;
	}

    wSkinID = pPersonSkinModelItem->nDefaultSkinID;
}

void CurrentMatchRoomInfo::changeComputerAIID(PDBID dwComputerID, int nAIID)
{
    VCT_Roles::iterator itBegin = m_vctRoles.begin();
    for (; itBegin != m_vctRoles.end(); ++itBegin )
    {
        if ( itBegin->dwPDBID == dwComputerID )
        {
            itBegin->nAIID = nAIID;
            break;
        }
    }
}

int CurrentMatchRoomInfo::getCurrentRoomID()
{
    return m_RoomData.m_RoomID;
}

void CurrentMatchRoomInfo::updateSelfCanBanHero()
{
    bool showBanFlag = true;
	if (m_RoomData.m_bIsOBEnter)
	{
		showBanFlag = false;
	}
	else if (m_BanHeroPos.find(m_RoomData.m_SelfPos) == m_BanHeroPos.end())
	{
		showBanFlag = false;
	}

    // 通知客户端显示禁用按钮
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_CAN_BAN_HERO_FLAG, showBanFlag, 0, 0, 0);
}

void CurrentMatchRoomInfo::initBanHeroInfo()
{
    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoom();
    if (pSchemeMatchRoom == NULL)
    {
        return;
    }
    if (pSchemeMatchRoom->m_bOpenBan == false)
    {
        return;
    }
    
    if (pSchemeMatchRoom->m_nBanPosCount <= 0)
    {
        return;
    }

    for (int n = 0; n < pSchemeMatchRoom->m_nBanPosCount; ++n)
    {
        m_BanHeroPos.insert(pSchemeMatchRoom->m_BanPosList[n]);
    }
}