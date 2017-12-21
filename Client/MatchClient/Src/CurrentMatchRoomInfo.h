/*******************************************************************
** 文件名:	e:\speed\Client\Game\Src\Stage_MatchRoom.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/23/2015	11:48
** 版  本:	1.0
** 描  述:	
            比赛大厅 房间
********************************************************************/

#pragma once


#include "stdafx.h"
#include "IMessageDispatch.h"
#include "MatchDef.h"
#include "EntityViewDef.h"
#include "BaseRoomState.h"
#include "StateManage.h"
#include "RoomState_Wait.h"
#include "RoomState_SelectHero.h"
#include "RoomState_ReadyGo.h"
#include "RoomState_Fighting.h"
#include "IEntityClient.h"
#include "GameViewDef.h"
#include "Stage_ManagedDef.h"
#include "IMatchClient.h"
#include "ISchemeCenter.h"
#include "ISpell.h"
#include "IClientEntity.h"
#include "ExternalHelper.h"

using namespace SPELL;
using namespace Stage_ManagerDef;

struct SSchemeMatchRoom;

class CurrentMatchRoomInfo : public StateManage<BaseRoomState>,public Singleton<CurrentMatchRoomInfo>
{
public:
	typedef vector<SRoomRoleBase>		VCT_Roles;


public:
    CurrentMatchRoomInfo();
    ~CurrentMatchRoomInfo();

    void                inital(void * context,int len, bool bGuidRoom);

    void                release();

	void				initFromGame();
	void				initFromMatch( void* data,size_t len );
	void				showRoom();

	char const*			getTextInfo( int nID );

	// SSchemeMatchRoom*
	SSchemeMatchRoom*	getSchemeMatchRoom();
	// 获取阵营的玩家数量
	uint32				getCampRoleCount( int nCamp );
	uint32				getCampRoleCount( int nCamp, SSchemeMatchRoom* pSchemeMatchRoom );
	uint32				getCampRoleCount( int nCamp, uint32 u32CampRoleMax);
	// 位置上的角色[0, ...)
	SRoomRoleBase*		getRoleByPos(uchar Pos);
	// 位置上的角色[0, ...)
	SRoomRoleBase*		getObRoleByPos(uchar Pos);
	// 自己
	SRoomRoleBase*		getSelf();
	// 角色所在阵营[0, ...)
	uint32				getPlayerCamp(SRoomRoleBase* pRole);
	uint32				getPlayerCamp(uchar Pos);
	// 角色在阵营中位置[0, ...)
	uint32				getPlayerCampPos(SRoomRoleBase* pRole);
	uint32				getPlayerCampPos(uchar Pos);

    // 根据角色PDBID获得角色阵营位置
    SRoomRoleBase*      getPlayerRoomBaseInfo(PDBID pDbid);

	// 角色位置 view->c++
	uchar				getPos(uint32 u32Camp, uint32 u32CampPos);

	// 获取房间类型
	BYTE				getRoomType();

    // 获取房间ID
    int				    getCurrentRoomID();
	// 是房主吗
	bool				isMaster();
	bool				isMaster(uchar Pos);
	bool				isMaster(SRoomRoleBase* pRole);
    void				SetOBRoomRole( int nIndex, SRoomRoleBase sRoomRole);
	void				SetRoomRole( int nIndex, SRoomRoleBase sRoomRole);
	void				MakeRoleDataCmd( SRoomRoleBase* pRole, cmd_match_room_role_data &cmdMatchRoomRoleData );
	void				MakeRoleDataCmdSelf( cmd_match_room_role_data &cmdMatchRoomRoleData );
	void				MakeObDataCmd( SRoomRoleBase* pRole, cmd_match_room_ob_data &cmdMatchRoomRoleData );
	void				MakeObDataCmdSelf( cmd_match_room_ob_data &cmdMatchRoomRoleData );
	void				swapPos(int pos1, int pos2);
	void				loadSenece(DWORD dwMapID, Vector3 vecBornPos, bool bShowSelf = false);
	// 其他玩家离开了房间
	void				playerExit(bool bIsOb, uchar Pos, int nCmd );
	SMsgSelfEnterRoomData*	getRoomData();
    void                SendRoomHeroSlotToView();

    // 设置对应pdbid的玩家的召唤师技能
    void                setPDbidSummonerSKill( SMsgSummoner_Skill* pInfo );

    // 根据召唤师技能id 获得对应的图标
    int                 getSummonerSkillIconId( int nSkillId );

    // 发送所有玩家的召唤师技能信息
    void                sendAllPlayerSmSkillInfo();

    // 当玩家更换位置时更新召唤师技能显示
    void                changeSummonerSkillChangePos(int nPos);

    // 更新召唤师技能信息
    void                updateSummonerSkillInfo(int nPos, int nSkill[], int nSize);

    // 改变
    void                changeComputerAIID( PDBID dwComputerID, int nAIID);

    // 大厅服消息
    void                onMatchMessage( SNetMsgHead* head, void* data, size_t len);

    // 更新自己是否能ban选
    void                updateSelfCanBanHero();

protected:
    // 获得所有怪物皮肤
    void                getAllMonsterSkin( int nMapId, std::vector<int>& monsterVec);
    // 获得英雄的皮肤
    void                getHeroDefaultSkin( int nHeroId , DWORD &nSkinID );
    void                initBanHeroInfo();

private:

	SMsgSelfEnterRoomData	m_RoomData;
	VCT_Roles				m_vctRoles;
    VCT_Roles				m_vctOBRoles;
	DWORD					m_SceneID;

    set<int>                m_BanHeroPos;

	int		                m_nFromWhere;

};