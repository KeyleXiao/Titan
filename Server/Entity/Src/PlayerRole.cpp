/*******************************************************************
** 文件名:	PlayerRole.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家实体对象
********************************************************************/

#include "StdAfx.h"
#include <float.h>
#include "EventDef.h"
#include "PlayerRole.h"
#include "ISpell.h"
#include "PlayerRoleProperty.h"
#include "IServerGlobal.h"
#include "IEntityAOI.h"
#include "IMovePart.h"
#include "IServerGlobal.h"
#include "IFreezeServer.h"
#include "IDiePart.h"
#include "IBuffServer.h"
#include "IDamageServer.h"
#include "IProgressBarPart.h"
#include "ITankServer.h"
#include "IEnmityServer.h"
#include "IGameSceneService.h"
#include "IDragonBallPart.h"
#include "ISchemeCenter.h"
#include "IWarPart.h"
#include "IMobaGoodsPart.h"
#include "PlayerBankPart.h"
#include "ISlotPart.h"
#include "IMigrateService.h"
#include "IShareServer.h"
#include "IChatPart.h"
#include "ITestServer.h"
#include "IWingPart.h"
#include "ILogoutPart.h"
#include "IAIPart.h"
#include "ITitlePart.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"
#include "IPlayerKillPart.h"
#include "TeamHelper.h"
#include "ICommonPart.h"
#include "IMailPart.h"
#include "StackTrace.h"
// 取得属性接口
#define GetProperty() ((__IEntityProperty*)m_pPartList[PART_PROPERTY])

///////////////////////////////////////////////////////////////////////////////////
PlayerRole::PlayerRole()
{
	memset( m_pPartList,0,sizeof(m_pPartList) ); 
	m_clientID = 0;
	m_pEventEngine = NULL;
	m_nMapID = INVALID_MAPID;

	m_nFlag = TO_INVALID;

#ifdef VERSEION_DEBUG
    m_bReleased = false;
#endif

	// 当前控制状态
	m_byCurControlState = ENTITY_STATE_UNKNOW;
	// 当前状态
	m_pCurrentState = 0;

	// 实体是否已初始化
    m_bInit = false;

    // 角色游戏状态
    m_dwGameState = GAME_STATE_INVALID;

    memset(m_szName,0,sizeof(m_szName));

    m_pMobaGoodsEffect = new CMobaGoodsEffect(CAST_TYPE(__IEntity *,this));
}

PlayerRole::~PlayerRole()
{
    // 调试代码:由于外界不可能直接调用delete ptr来删除实体, 所以这里增加断言，出现问题直接挂起方便查问题
#ifdef VERSEION_DEBUG
    if (!m_bReleased)
    {
        int* pi = 0; (*pi) = 0x01020304;
    }
#endif
}

int PlayerRole::send2Client( PACKAGE_PTR package, int nFlag )
{
#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
	SNetMsgHead* pHead = (SNetMsgHead*)package->c_str();
	if(pHead->byKeyModule == MSG_MODULEID_ENTITY)
	{
		((SMsgEntityHead*)(pHead+1))->dwServerTickSC = getTickCount();
	}
#endif

    if(m_clientID != INVALID_CLIENT)
    {
        gServerGlobal->getGatewayAcceptorService()->sendData( m_clientID,package,nFlag );
    }
	return 1;
}

void PlayerRole::send2ClientAndOB( PACKAGE_PTR package )
{
    send2Client(package);

    // 另外广播给ob
    int nSceneID = getSceneID();
    SceneHelper sceneHelper(nSceneID);
    if(sceneHelper.m_ptr)
    {
        std::string strMsg(package->data(), package->size());
        sceneHelper.m_ptr->broadcast_all(0, strMsg, CAMP_MASK(sceneHelper.m_ptr->getObCampID()));
    }
}

bool PlayerRole::sendMessageToClient(BYTE byPartID, BYTE byAction, void * data,size_t len)
{
	if (byPartID > ENTITY_PART_MAX)
	{
		return false;
	}

	// 通知客户端
	obuf256 obufData;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
	head.byKeyModule   = MSG_MODULEID_ENTITY;	
	head.byKeyAction   = byAction;
	obufData << head;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = getUID();	// 实体UID
	entityHead.byPartID = byPartID;		// 部件ID，ENTITY_PART_MAX时将由实体来处理

#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
	entityHead.dwServerTickSC = getTickCount();
#endif

	obufData << entityHead;
	if (data != NULL && len > 0)
	{
		obufData.push_back(data,len);
	}

	return send2Client(obufData.data(), obufData.size()); 
}

void PlayerRole::updateTalentPoint( int nPoint, int nChangeType, BYTE byAddType)
{
    UID uid = getUID();
    

    __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty == NULL)
    {
        return;
    }

    pProperty->addProperty_Integer(PROPERTY_TALENT_POINT, nPoint);
    //if (nChangeType != ENUM_TALENT_REVOKE && nChangeType != ENUM_TALENT_SELL )
    {
        // 发送金币改变事件
        event_entity_talent_change context;
        context.uId = uid;
        context.nTalentValue = getProperty_Integer(uid, PROPERTY_TALENT_POINT);
        context.nChangePoint = nPoint;
        context.byAddType = byAddType;
        context.byChangeType = nChangeType;
        getEntityEvent()->FireExecute(EVENT_GAME_ACTOR_TALENT_CHANGE,(LPCSTR)&context,sizeof(context));
    }
}

#define PLAYER_CREATE_FAIL_DELETE_PARTS(errinfo)	\
	ErrorLn(errinfo);\
    onDestroyFreePartList();\
    onReleaseFreePartList();

///////////////////////////////////////////////////////////////////////////////////
bool PlayerRole::create(UID uid,void * context,int len,int flag)
{
	// 事件中心
	if ( m_pEventEngine ) delete m_pEventEngine;
	m_pEventEngine = new EntityEvent(uid);

	m_nFlag = flag;

	//////////////////////////////////////////////////////////////////////////
	// 属性部件
	__IEntityProperty * property = new EntityPart_RoleProperty;
	if ( property==0  )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create property part failed!");
		return false;
	}
	m_pPartList[PART_PROPERTY] = property;
	property->setProperty_Integer(PROPERTY_UID, uid);

	// 数据仓库部件
	__IBankPart * pBankPart = CreateBankPartProc();
	if ( pBankPart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create bank part failed!");
		return false;
	}
	m_pPartList[PART_BANK] = pBankPart;

	// 视野部件
	__IEntityAOI * pAOI = CreateEntityAOIProc();
	if ( pAOI==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create AOI part failed!");
		return false;
	}
	m_pPartList[PART_AOI] = pAOI;

	// 移动组件
	__IMovePart * pMove = CreateMovePartProc();
	if ( pMove==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create Move part failed!");
		return false;
	}
	m_pPartList[PART_MOVE] = pMove;

	// 技能部件
	__ISpellPart *pSpellPart = gServerGlobal->getSpellFactory()->createSpellPart();
	if (pSpellPart == 0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create Spell Part failed!");
		return false;
	}
	m_pPartList[PART_SPELL] = pSpellPart;

	// 冷却服务器
	IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
	if (pFreezeServer == 0)
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("get Freeze Server Failed!");
		return false;
	}
	// 冷却部件
	__IFreezePart *pFreezePart = pFreezeServer->CreatePart();
	if (pFreezePart == NULL)
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create pFreezePart failed!");
		return false;
	}
	m_pPartList[PART_FREEZE] = pFreezePart;

	// 死亡部件
	__IDiePart * pDiePart = CreateDiePartProc();
	if ( pDiePart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create die part failed!");
		return false;
	}
	m_pPartList[PART_DIE] = pDiePart;

	// Buff服务器
	IBuffServer *pBuffServer = gServerGlobal->getBuffServer();
	if (pBuffServer == 0)
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("get Buff Server Failed!");
		return false;
	}
	// Buff部件
	__IBuffPart *pBuffPart = pBuffServer->CreatePart();
	if ( pBuffPart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create Buff part failed!");
		return false;
	}
	m_pPartList[PART_BUFF] = pBuffPart;

	// 伤害服务器
	IDamageServer *pDamageServer = gServerGlobal->getDamageServer();
	if (pDamageServer == 0)
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("get Damage Server Failed!");
		return false;
	}
	// 伤害部件
	__IDamagePart *pDamagePart = pDamageServer->crateDamagePart();
	if ( pDamagePart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create Damage part failed!");
		return false;
	}
	m_pPartList[PART_DAMAGE] = pDamagePart;

	// 读条部件
	__IProgressBarPart *pProgressBar = CreateEntityProgressBarProc();
	if ( pProgressBar==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create ProgressBar part failed!");
		return false;
	}
	m_pPartList[PART_PROGRESS_BAR] = pProgressBar;
	
	// 载具部件
	ITankServer *pTankServer = gServerGlobal->getTankServer();
	if (pTankServer == 0)
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("get Tank Server Failed!");
		return false;
	}
	__IPersonTankPart *pTankBoxPart = pTankServer->CreatePersonTankPart();
	if ( pTankBoxPart==0)
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create PersonTank part failed!");
		return false;
	}
	m_pPartList[PART_PERSON_TANK] = pTankBoxPart;

	// 仇恨服务器
	IEnmityServer *pEnmityServer = gServerGlobal->getEnmityServer();
	if (pEnmityServer == 0)
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("get Enmity Server Failed!");
		return false;
	}
	// 仇恨部件
	__IEnmityPart *pEnmityPart = pEnmityServer->CreatePart();
	if ( pEnmityPart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create Enmity part failed!");
		return false;
	}
	m_pPartList[PART_ENMITY] = pEnmityPart;

	// 七龙珠部件
    __IDragonBallPart * pDragonBallPart = CreateDragonBallPartProc();
    if ( pDragonBallPart )
    {
        m_pPartList[PART_DRAGONBALL] = pDragonBallPart;
    }
    
	// 战场部件
	__IWarPart * pWarPart = CreateWarPartProc();
	if ( pWarPart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create pWarPart part failed!");
		return false;
	}
	m_pPartList[PART_WAR] = pWarPart;

	// 战场物品部件
	__ISlotPart * pSlotPart = CreateSlotPartProc();
	if ( pSlotPart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create pMobaGoodsPart part failed!");
		return false;
	}
	m_pPartList[PART_SLOTLOGIC] = pSlotPart;

    // AI部件 PART_AI
    __IAIPart * pAIPart = CreateAIPartProc();
    if ( pAIPart==0 )
    {
        PLAYER_CREATE_FAIL_DELETE_PARTS("create AI part failed!");
        return false;
    }
    m_pPartList[PART_AI] = pAIPart;

	// 战场物品部件
	__IMobaGoodsPart * pMobaGoodsPart = CreateMobaGoodsPartProc();
	if ( pMobaGoodsPart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create pMobaGoodsPart part failed!");
		return false;
	}
	m_pPartList[PART_MOBAGOODS] = pMobaGoodsPart;

	// 聊天部件
	__IChatPart* pChatPart = CreateChatPartProc();
	if ( pChatPart == 0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create pChatPart part failed!");
		return false;
	}
	m_pPartList[PART_CHAT] = pChatPart;

    // 翅膀组件
    __IWingPart * pWing = CreateWingPartProc();
    if ( pWing==0 )
    {
        PLAYER_CREATE_FAIL_DELETE_PARTS("create Wing part failed!");
        return false;
    }
	m_pPartList[PART_WING] = pWing;

	// 称号组件
	__ITitlePart * pTitlePart = CreateTitlePartProc();
	if ( pTitlePart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create Title part failed!");
		return false;
	}
	m_pPartList[PART_TITLE] = pTitlePart;

    __ILogoutPart* pLogoutPart = CreateLogoutPartProc();
    if(pLogoutPart == 0)
    {
        PLAYER_CREATE_FAIL_DELETE_PARTS("create logout part failed!");
        return false;
    }
    m_pPartList[PART_LOGOUT] = pLogoutPart;

    // PK组件
    __IPlayerKillPart *pPlayerKill = CreatePlayerKillPartProc();
    if ( pPlayerKill==0 )
    {
        PLAYER_CREATE_FAIL_DELETE_PARTS("create pPlayerKill part failed!");
        return false;
    }
    m_pPartList[PART_PK] = pPlayerKill;

    // 测试部件
    ITestServer * pTestServer = gServerGlobal->getTestServer();
    if(pTestServer)
    {
        __ITestPart * pTestPart = pTestServer->createTestPart();
        if ( pTestPart == 0 )
        {
            PLAYER_CREATE_FAIL_DELETE_PARTS("create pTestPart part failed!");
            return false;
        }
        m_pPartList[PART_TEST] = pTestPart;
    }

	// 邮件组件
	__IMailPart * pMailPart = CreateMailPartProc();
	if ( pMailPart==0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create Mail part failed!");
		return false;
	}
	m_pPartList[PART_MAIL] = pMailPart;

	// 公共部件
	__ICommonPart* pCommonPart = CreateCommonPartProc();
	if ( pCommonPart == 0 )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("create pCommonPart part failed!");
		return false;
	}
	m_pPartList[PART_COMMON] = pCommonPart;
	//////////////////////////////////////////////////////////////////////////

    // 导入数据
    importContext(context, len, flag);

	// 站立状态
	m_EntityStandState.create(this);
	// 死亡状态
	m_EntityDieState.create(this);
	// 战斗死亡状态
	m_EntityFightState.create(this);
	// 转到站立状态
	changeState(ENTITY_STATE_STAND);

    ///////////////////////////////////////////////////////////////////
    // 队伍ID
    TeamHelper teamHelper;
    ITeamSceneService *pTeamService = teamHelper.m_ptr;
    if (pTeamService != NULL)
    {
        DWORD dwTeamID = pTeamService->getTeamIDByPlayer(getIntProperty(PROPERTY_ID));
        if(dwTeamID != INVALID_TEAM_ID)
        {
            setIntProperty(PROPERTY_TEAMID, dwTeamID);
        }
    }



	return true;
}

// 导出静态数据(从战场的临时角色导出)
bool PlayerRole::deSerializeStaticData(rkt::obuf & out)
{
    __IBankPart* pBankPart = (__IBankPart*)getEntityPart(PART_BANK);
    if(pBankPart == NULL)
    {
        return false;
    }
    SPlayerInfo* pPlayerInfo = pBankPart->getPlayerInfo();
    if(pPlayerInfo == NULL)
    {
        ErrorLn(__FUNCTION__ << ", pPlayerInfo == NULL");
        return false;
    }

    // 导出属性部件数据
    PlayerRoleProperty property;
    property.nPDBID = pPlayerInfo->dwActorID;
    sstrcpyn(property.szName, pPlayerInfo->szActorName, sizeof(property.szName));
    property.nSceneID     = pPlayerInfo->dwMapId;
    property.location.x   = ((float)pPlayerInfo->nLocX)/ACTOR_LOCATION_SCALE;
    property.location.y   = ((float)pPlayerInfo->nLocY)/ACTOR_LOCATION_SCALE;
    property.location.z   = ((float)pPlayerInfo->nLocZ)/ACTOR_LOCATION_SCALE;
    SMapLandSchemeInfo* pInfo = gServerGlobal->getSchemeCenter()->getSchemeMapLand()->GetMapLandSchemeInfo(pPlayerInfo->dwMapId, EGM_RPG, 0);
    if(pInfo && !pInfo->vecBornPos.empty()) {
        property.angle = Vector3(0, pInfo->vecBornPos.front().fAngleY, 0); 
    }
    property.nVocation    = pPlayerInfo->nHeroID;
    property.bySex        = pPlayerInfo->bSex;
    property.nSkin        = pPlayerInfo->nSkinID;
    property.nLevel       = pPlayerInfo->nLevel;
    property.nKinID       = pPlayerInfo->dwKinID;
    property.nClanID       = pPlayerInfo->dwClanID;
    property.nFightCapacity = pPlayerInfo->dwFightCapacity;
    property.nGold          = pPlayerInfo->dwBindTicket;
    property.nTicket        = pPlayerInfo->dwTicket;
    property.nFaceID        = pPlayerInfo->wHeadID;
    property.byMatchType     = 0;
    property.nRankScore     = 0;
    property.nRankGrade     = 0;


    out << (short)PART_PROPERTY;
    int nLenOffset = out.size();
    out << (int)0;  // 压入部件长度
    int nBegin = out.size();
    if(!property.getDataBuffer(out, TO_OTHER_SEVER))
    {
        ErrorLn(__FUNCTION__ << ", property.getDataBuffer failed");
        return false;
    }
    // 保存长度
    *((int*)(out.data() + nLenOffset)) = out.size() - nBegin;

    return true;
}

// 检查是否有对应的权限
bool PlayerRole::checkPrivilege(int nPrivilege)
{
    __IBankPart * pBankPart = (__IBankPart *)getEntityPart(PART_BANK);
    if( !pBankPart )
    {
        return false;
    }

    return ( (pBankPart->getPlayerInfo()->nPrivilege & nPrivilege) == nPrivilege );
}

// 增加权限
void PlayerRole::addPrivilege(int nPrivilege)
{
    __IBankPart * pBankPart = (__IBankPart *)getEntityPart(PART_BANK);
    if (pBankPart)
    {
        pBankPart->getPlayerInfo()->nPrivilege |= nPrivilege;
    }
}

// 移除权限
void PlayerRole::removePrivilege(int nPrivilege)
{
    __IBankPart * pBankPart = (__IBankPart *)getEntityPart(PART_BANK);
    if (pBankPart)
    {
        pBankPart->getPlayerInfo()->nPrivilege &= ~nPrivilege;
    }
}

// 导入创建对象的上下文数据
bool PlayerRole::importContext(void * pContext, size_t len, int nFlag)
{
	// 序列化
	ibuffer in(pContext,len);
	if ( onSerialize(in,nFlag)==false )
	{
		PLAYER_CREATE_FAIL_DELETE_PARTS("entity importCreateContext failed! flag="<<nFlag);
		return false;
	}

    return true;
}

// 导入对象的更新上下文
bool PlayerRole::onCreated(ELoginMode loginMode)
{
	// 初始化各部件数据
	for (int i=PART_PROPERTY; i<ENTITY_PART_MAX; ++i)
	{
		if (m_pPartList[i] && m_pPartList[i]->onLoad(this, loginMode) == false)
		{
			PLAYER_CREATE_FAIL_DELETE_PARTS("onCreated failed! partID:"<<i);
			return false;
		}		
	}

    m_belongEntityMgr.onload(this);

	UID uid = getUID();
    DWORD dwClientID = getClientID();
    SERVICE_ID serviceID = getServiceID();

    IActorMgr* pActorMgr = gServerGlobal->getActorMgr();
    if (pActorMgr)
    {
        pActorMgr->onCreateActor(uid, getUserID(), getIntProperty(PROPERTY_ID), getIntProperty(PROPERTY_SCENEID), serviceID, dwClientID, getIntProperty(PROPERTY_AI_SCHEME_ID) > 0 );
    }

	// 发出实体创建事件
	event_entity_create context;
	context.uid = uid;
	context.flag= m_nFlag;
	context.dwData = dwClientID;    // 人物：ClientID
	context.nData = serviceID;      // 人物：SERVICEID
    Vector3 location = getLocation();

	// 触发角色创建事件
	getEntityEvent()->FireExecute(EVENT_ENTITY_CREATE,(LPCSTR)&context,sizeof(context));
    // 触发角色创建完成事件 
    getEntityEvent()->FireExecute(EVENT_ENTITY_AFTER_CREATE, NULL, 0);

	ENTITY_TRACELN( __FUNCTION__":"<<getName()<<",PDBID="<< getIntProperty(PROPERTY_ID)<<",UID=" << uid<<",HeroID="<<getIntProperty(PROPERTY_VOCATION)<<",Skin="<<getIntProperty(PROPERTY_SKIN)
        <<",SceneID="<<getSceneID()<<",ClientID="<<getClientID()<<",ServiceID="<<m_serviceID<<",Flag="<<m_nFlag<<",x="<<location.x<<",y="<<location.y<<",z="<<location.z);

    //// 如果在静态场景改变玩家状态
    //if( !IsDynamicScene(getSceneID()))
    //{
    //    setGameState(GAME_STATE_IDLE);
    //}

	// 实体是否已初始化
	m_bInit = true;

	return true;
}
///////////////////////////////////////////////////////////////////////////////////
// 执行Destory时释放部件
void PlayerRole::onDestroyFreePartList(void)
{
    // todo 释放之前先停止所有部件？然后才释放
    for (int i = ENTITY_PART_MAX - 1; i >= 0; --i)
    {
        // 特殊处理：销毁属性部件
        if (m_pPartList[i] != 0)
        {
            m_pPartList[i]->restore();
        }
    }

    for (int i = ENTITY_PART_MAX - 1; i >= 0; --i)
    {
        // 特殊处理：销毁属性部件
        if (i == PART_PROPERTY)
        {
            __IEntityProperty* pProperty = (__IEntityProperty*)m_pPartList[i];
            if (pProperty != 0)
            {
                pProperty->destroy();
            }
            continue;
        }

        // 特殊处理：销毁pk部件 其他部件todo.
        if (i == PART_PK)
        {
            __IPlayerKillPart* pPK = (__IPlayerKillPart*)m_pPartList[i];
            if (pPK != 0)
            {
                pPK->destroy();
            }
            continue;
        }

        if (m_pPartList[i] != 0)
        {
            m_pPartList[i]->release();
            m_pPartList[i] = 0;
        }
    }
}

// 执行Release时释放部件
void PlayerRole::onReleaseFreePartList(void)
{
    if (m_pPartList[PART_PROPERTY] != NULL)
    {
        m_pPartList[PART_PROPERTY]->release();
        m_pPartList[PART_PROPERTY] = 0;
    }

    if (m_pPartList[PART_PK] != NULL)
    {
        m_pPartList[PART_PK]->release();
        m_pPartList[PART_PK] = 0;
    }


}


void PlayerRole::release() 
{
#ifdef VERSEION_DEBUG
    m_bReleased = true;
#endif

	delete this;
}

///////////////////////////////////////////////////////////////////////////////////
void PlayerRole::destroy()
{
    releaseMobaGoodsEffect();

    int nPDBID = getIntProperty(PROPERTY_ID);
    SERVICE_ID serviceID = m_serviceID;
    UID uid = getUID();

    ENTITY_TRACELN( __FUNCTION__ ":["<<getName() <<"],PDBID="<< nPDBID <<",UID="<< uid <<",ClientID="<<",ServiceID="<< serviceID
        << getClientID() <<",SceneID="<<getIntProperty(PROPERTY_SCENEID) );

    event_entity_destroy context;
    context.uid = uid;
    context.dwData = m_clientID;	// 人物：ClientID
    context.bBroadAOI = true;

    getEntityEvent()->FireVote(EVENT_ENTITY_DESTROY, (LPCSTR)&context,sizeof(context));

    // 执行Destory时释放部件
    onDestroyFreePartList();

    getEntityEvent()->FireExecute(EVENT_ENTITY_DESTROY,(LPCSTR)&context,sizeof(context));
    
	// 事件中心
	if ( m_pEventEngine ) 
	{ 
		delete m_pEventEngine;
		m_pEventEngine = 0;
	}

	m_nMapID = INVALID_MAPID;

    // 剩余释放
    if (m_pMobaGoodsEffect)
    {
        m_pMobaGoodsEffect->release();
        m_pMobaGoodsEffect = NULL;
    }
    m_belongEntityMgr.restore();

    // 执行Release时释放部件
    onReleaseFreePartList();

	//gServerGlobal->getEntityFactory()->revertUID(uid);
	//ENTITY_TRACELN("Player is destroy, uid=" << uid);
}

///////////////////////////////////////////////////////////////////////////////////
UID PlayerRole::getUID() const
{
	UID ret = INVALID_UID;
    __IEntityProperty* pProperty = GetProperty();
	if ( pProperty && pProperty->getProperty_custom(PROPERTY_UID,&ret,sizeof(ret)) )
	{
		return ret;
	}

	return INVALID_UID;
}

///////////////////////////////////////////////////////////////////////////////////
ENTITY_TYPE PlayerRole::getType() 
{ 
	return TYPE_PLAYER_ROLE;
}

///////////////////////////////////////////////////////////////////////////////////
char * PlayerRole::getName()
{
    if(m_szName[0] == '\0')
    {
        __IEntityProperty* pProperty = GetProperty();
        if (pProperty)
        {
            sstrcpyn(m_szName,pProperty->getProperty_String(PROPERTY_NAME).c_str(),sizeof(m_szName));
        }
    }

    return m_szName;
}

///////////////////////////////////////////////////////////////////////////////////
int PlayerRole::getSceneID() const
{
    __IEntityProperty* pProperty = GetProperty();
    if ( !pProperty)
    {
        return INVALID_SCENE_ID;
    }

    int nSceneID = pProperty->getProperty_Integer(PROPERTY_SCENEID);
    if (nSceneID == 0 )
    {
        UID uid = getUID();
        nSceneID = UID_2_SCENE(uid);
    }
    return nSceneID;
}

// 取得当前所在地图ID
int PlayerRole::getMapID() const
{
	return m_nMapID;
}

///////////////////////////////////////////////////////////////////////////////////
Vector3 PlayerRole::getLocation() const 
{
	Vector3 ret(0.0f,0.0f,0.0f);
    __IEntityProperty* pProperty = GetProperty();
	pProperty ? pProperty->getProperty_custom(PROPERTY_LOCATION,&ret,sizeof(ret)): 0;
	return ret;
}

void PlayerRole::setLocation(Vector3 vLoc)
{
#ifdef ENTITY_CHECK_LOCATION
    if( _isnan(vLoc.x) || _isnan(vLoc.z) || abs(vLoc.x) > CHECK_LOCATION_THRESHOLD  || abs(vLoc.z) > CHECK_LOCATION_THRESHOLD )
    {
        __debugbreak();
    }
#endif

    __IEntityProperty* pProperty = GetProperty();
	pProperty ? pProperty->setProperty_custom( PROPERTY_LOCATION,(void*)&vLoc,sizeof(vLoc) ) : 0;
}

Vector3 PlayerRole::getAngle() const
{
    Vector3 ret(0.0f,0.0f,0.0f);
    __IEntityProperty* pProperty = GetProperty();
    if ( pProperty && pProperty->getProperty_custom(PROPERTY_ANGLE,&ret,sizeof(ret)) )
    {
        return ret;
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
__IEntityPart * PlayerRole::getEntityPart(ENTITY_PART id) 
{
	if ( id>=0 && id<ENTITY_PART_MAX )
	{
		return m_pPartList[id];
	}

	return 0;
}

bool PlayerRole::onSerialize( rkt::ibuffer & in,int flag )
{
    bool bResult = true;
	while( true )
	{
		short part_id = 0; int len = 0;
		in >> part_id >> len;
		if ( !in )
        {
			return ( in.current()==in.end() );
        }

        if(len == 0)
        {
            continue;
        }

        if(in.space() < len)
        {
            ErrorLn(__FUNCTION__ << ", space < len, part id=" << part_id);
            return false;
        }

		__IEntityPart * part = getEntityPart((ENTITY_PART)part_id);
		if ( part )
		{
			if ( !part->onSerialize(ibuffer(in.current(), len),flag) )
			{
                ErrorLn(__FUNCTION__ << " failed, part=" << part_id);
                bResult = false;
			}
		}
		else
		{
			ErrorLn("PlayerRole::onSerialize failed! Can not find part id"<<part_id);
			bResult = false;
		}

        in.offset(len);
	}

	return bResult;
}

bool PlayerRole::deSerialize( rkt::obuf & out,int flag )
{
	// 发出预导出数据事件
	if (m_pEventEngine)
	{
		event_actor_pre_export_data eventData;
		eventData.nFlag = flag;
		m_pEventEngine->FireExecute(EVENT_ACTOR_PRE_EXPORT_DATA, (LPCSTR)&eventData, sizeof(eventData));
	}

    bool bResult = true;
	for ( int id=0;id<ENTITY_PART_MAX;++id )
	{
		if ( m_pPartList[id] )
		{
            out << (short)id;                                      // 部件id

            int len_offset = out.size();     
            out << (int)0;                                            // 数据长度
            
            int begin = out.size();                                
			if ( !m_pPartList[id]->deSerialize(out,flag) )         // 数据
			{
                ErrorLn(__FUNCTION__ << ", part=" << id);
                bResult = false;
			}

            *((int*)(out.data() + len_offset)) = out.size() - begin;              // 保存长度
		}
	}

	return bResult;
}

void PlayerRole::handleMessage( void * data,int32 len )
{
	if ( data==NULL || len<sizeof(SNetMsgHead) )
	{
		ErrorLn("["<<getName()<<"] handleMessage failed! message length is too short! [len]" << len );
		return;
	}

	SNetMsgHead * head = (SNetMsgHead *)data;
	data = (void*)(head+1);
	len -= sizeof(SNetMsgHead);

	if ( len<sizeof(SMsgEntityHead) )
	{
		ErrorLn("["<<getName()<<"] handleMessage failed! message length is too short! msgid="<< head->byKeyAction );
		return;
	}

	SMsgEntityHead * pEntityHead = (SMsgEntityHead *)data;
	void *buff = (void*)(pEntityHead+1);
	len -= sizeof(SMsgEntityHead);

	BYTE partId = pEntityHead->byPartID;
	BYTE msgId = head->byKeyAction;

#	ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
	pEntityHead->dwServerTickCS = getTickCount();
#	endif

	// 实体中的异常消息
	if (partId > ENTITY_PART_MAX)
	{
		ErrorLn("["<<getName()<<"] handleMessage failed! partId overflow! msgid="<< msgId );
		return;
	}

	// 状态过滤
	if (m_pCurrentState && !m_pCurrentState->onMessage(head, pEntityHead, buff, len))
	{
		return;
	}
	
	// 由实体来处理
	if (partId < ENTITY_PART_MAX)
	{
		if (m_pPartList[partId]==NULL)
		{
			ErrorLn("["<<getName()<<"] handleMessage failed! partId not instance! partId="<<partId<<", msgid="<< msgId );
			return;
		}

		//ENTITY_TRACELN("PlayerRole::handleMessage() partid="<< partId <<", msg="<< msgId);
		m_pPartList[partId]->onMessage(pEntityHead, msgId, buff, len);
		return;
	}
	else
	{
		// 处理客户端来的消息
		onMessage(msgId, head, buff, len);
	}
}


/** 改变当前状态 
@ nState: 最新的状态 ENTITY_STATE
@return  
*/
bool PlayerRole::changeState(int nState)
{
	if (nState <= ENTITY_STATE_UNKNOW || nState >= ENTITY_STATE_MAXID)
	{
		return false;
	}
	// 状态没变化
	if (nState == m_byCurControlState)
	{
		return false;
	}

	// 停止当前状态
	if(m_pCurrentState != NULL)
	{
		m_pCurrentState->end();
	}

	switch (nState)
	{
	case ENTITY_STATE_DIE:
		{
			m_pCurrentState = &m_EntityDieState;
		}
		break;
	case ENTITY_STATE_FIGHT:
		{
			m_pCurrentState = &m_EntityFightState;
		}
		break;
	default:
		{
			m_pCurrentState = &m_EntityStandState;
		}
		break;
	}
	// 启动新状态
	if(m_pCurrentState == NULL)
	{
		return false;
	}
	m_pCurrentState->start();
	// 更新状态值
    int nOldState = m_byCurControlState;
	m_byCurControlState = nState;

	// 发送状态变化事件
	event_entity_change_state eventData;
	eventData.uid = getUID();
    eventData.nOldState = nOldState; 
	eventData.nState = nState;
    __IEntityEvent * pEntityEvent = getEntityEvent();
    if(pEntityEvent)
    {
        pEntityEvent->FireExecute(EVENT_ENTITY_CHANGE_STATE,(LPCSTR)&eventData, sizeof(eventData));
        pEntityEvent->FireExecute(EVENT_GAME_ENTITY_CHANGE_STATE,(LPCSTR)&eventData, sizeof(eventData));
    }

	// 状态发送到客户端	
	if(m_bInit)
	{
		sendMessageToClient(ENTITY_PART_MAX, SC_MSG_ENTITY_SWITCH_STATE, &nState, sizeof(nState));
	}

	return true;
}

// 取得当前状态 
int PlayerRole::getState(void)
{
	return m_byCurControlState;
}

// 取得数值属性
int PlayerRole::getIntProperty(int nPropID)
{
	__IEntityProperty * pPropertyPart = GetProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_Integer(nPropID) : 0);
}

// 设置数值属性
bool PlayerRole::setIntProperty(int nPropID, int nValue)
{
    __IEntityProperty * pPropertyPart = GetProperty();
    return (pPropertyPart ? pPropertyPart->setProperty_Integer(nPropID, nValue) : false);
}

// 取得字符串属性
std::string PlayerRole::getStringProperty(int nPropID)
{
	__IEntityProperty * pPropertyPart = GetProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_String(nPropID) : "");
}

// 取得来源游戏世界ID
int PlayerRole::getFromGameWorldID()
{
	__IBankPart * pBankPart = (__IBankPart *)getEntityPart(PART_BANK);
	return (pBankPart ? pBankPart->getPlayerInfo()->dwWorldID : 0);
}

// 设置地图ID
void PlayerRole::setMapID(int nMapID)
{
	m_nMapID = nMapID;
}

//////////////////////////////////////////////////////////////////////////
// 处理客户端来的消息
void PlayerRole::onMessage(BYTE msgID, SNetMsgHead * head, void * data,int32 len)
{
	switch(msgID)
	{
	default:
		{
			ErrorLn( __FUNCTION__ ":has a message not handle! name="<<getName()<<",from="<<head->bySrcEndPoint<<",to="<<head->byDestEndPoint<<",module="<<head->byKeyModule <<",msg="<< msgID);
		}
		break;
	}
}

DWORD PlayerRole::getUserID()
{
    __IBankPart * pBankPart = (__IBankPart *)getEntityPart(PART_BANK);
    return (pBankPart ? pBankPart->getUserID() : 0);
}


// 设置游戏状态ACTOR_GAME_STATE
inline void PlayerRole::setGameState(DWORD dwState)
{
    if(dwState >= GAME_STATE_MAX)
    {
        ErrorLn(__FUNCTION__":" << getName() << " set the invalid state data!  dwState=" << dwState);
        return;
    }

    if(m_bInit && dwState == m_dwGameState)
    {
        return;
    }

    // 切换游戏状态
    event_actor_change_gamestate eventData;
    eventData.uid = getUID();
    eventData.dwDBID = getIntProperty(PROPERTY_ID);
    eventData.dwState = dwState;
    rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine)
    {
        pEventEngine->FireExecute(EVENT_GAME_ACTOR_CHANGE_GAMESTATE, SOURCE_TYPE_PERSON, 0, (LPCSTR)&eventData, sizeof(eventData));
    }

	// 通知实体事件
	__IEntityEvent* pEntityEvent = getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return;
	}
	event_entity_actor_change_gamestate entityEventData;
	entityEventData.dwState = dwState;
	pEntityEvent->FireExecute(EVENT_ACTOR_CHANGE_GAMESTATE, (LPCSTR)&entityEventData, sizeof(entityEventData));

    m_dwGameState = dwState;
}

// 获取游戏状态 ACTOR_GAME_STATE
inline DWORD PlayerRole::getGameState()
{
    return m_dwGameState;
}

// 获得玩家角色等级
int PlayerRole::getPlayerLevel()
{
    __IBankPart * pBankPart = (__IBankPart *)getEntityPart(PART_BANK);
    return (pBankPart ? pBankPart->getPlayerLevel() : 0);
}

// 获得玩家角色经验
int PlayerRole::getPlayerExp()
{
    __IBankPart * pBankPart = (__IBankPart *)getEntityPart(PART_BANK);
    return (pBankPart ? pBankPart->getPlayerExp() : 0);
}

/** 增加/减少经验值
@param   nExp:            经验值（负值减少，正值增加）
@param   wApproach:       消耗原因,0.不需要记录日记
@param   dwTargetID:      如果有目标者，这个就填入目标者ID
@param   szLogMark:       传入交易日志，供查询  
@param   nLogReserve:     日志的保留字段
@return
*/
bool PlayerRole::addExp(int nExp, WORD wApproach, DWORD dwTargetID, const char * pszRemark, int nLogReserve)
{
    __IEntityProperty * pPropertyPart = GetProperty();
    if(pPropertyPart)
    {
        int nOldExp = pPropertyPart->getProperty_Integer(PROPERTY_EXP);
        if( pPropertyPart->setProperty_Integer(PROPERTY_EXP, (nOldExp+nExp) ) )
        {
            if(wApproach > 0)
            {
                IOSSLogService * pOSSLogService = gServerGlobal->getOSSLogService();
                if(pOSSLogService != NULL)
                {
                    pOSSLogService->addExpLog(getFromGameWorldID(), (DWORD)getIntProperty(PROPERTY_ID), dwTargetID, nExp, wApproach, 0, pszRemark, nLogReserve);
                }
            }
            return true;
        }        
    }
    return false;
}

/** 是否可增加或扣除货币
@param nTicket: 添加的点券，负数表示扣除点卷
@param nGlod  : 添加的金币，负数表示扣除金币
@return : 
*/
bool PlayerRole::canAddMoney(int nTicket, int nGlod)
{
    __IBankPart* pBankPart = CAST_TYPE(__IBankPart*, getEntityPart(PART_BANK));
    return (pBankPart ? pBankPart->getMoneyMgr()->canAddMoney(nTicket, nGlod): false);
}

/** 增加或扣除货币
@param nTicket: 添加的点券，负数表示扣除点卷
@param nGlod  : 添加的金币，负数表示扣除金币
@param  : nReason 添加的原因（产生OSS_RESOURCE_ADD_RANGE/消耗的途径OSS_RESOURCE_OPT_DEL/转进OSS_RESOURCE_OPT_TRANSIN/转出OSS_RESOURCE_OPT_TRANSOUT）
@param  : pszRemark 添加的点券日志的备注（记日志用）
@param  : nSubclassID, 子类ID，购买物品则填物品ID，没有子类默认0
@param  : nNum 个数，子类是物品ID则填物品个数
@param  : bAddictLimit，是否考虑沉迷限制
@ note  : 在添加点券操作提交数据库（包括其他模块提交的）还没有返回前，这时候你提交点券是会失败的
*/
bool PlayerRole::addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark/*=0*/, int nSubclassID/*=0*/, short nNum/*=0*/, bool bAddictLimit/*=true*/)
{
    __IBankPart* pBankPart = CAST_TYPE(__IBankPart*, getEntityPart(PART_BANK));
    return (pBankPart ? pBankPart->getMoneyMgr()->addMoney(nTicket, nGlod, nReason, pszRemark, nSubclassID, nNum, bAddictLimit): false);
}

/** 切职业
@param nNewVocID: 新的职业ID
@return : 
*/
bool PlayerRole::changeVoc(int nNewVoc, int nNewSkinID)
{
    __IEntityProperty* pPropertyPart = (__IEntityProperty*)getEntityPart(PART_PROPERTY);
    if(pPropertyPart == NULL)
    {
        return false;
    }

    int nOldVoc = pPropertyPart->getProperty_Integer(PROPERTY_VOCATION);
    if(nOldVoc != nNewVoc)
    {
        // 投票
        event_entity_change_hero eventChange;
        eventChange.nOldHeroID = nOldVoc;
        eventChange.nNewHeroID = nNewVoc;
        __IEntityEvent* pEntityEvent = getEntityEvent();
        if(pEntityEvent != NULL && !pEntityEvent->FireVote(EVENT_ENTITY_CHANGE_HERO, (LPCSTR)&eventChange, sizeof(eventChange)))
        {
            return false;
        }

        // 设置职业
        pPropertyPart->setProperty_Integer(PROPERTY_VOCATION, nNewVoc);
		// 设置皮肤
		if (nNewSkinID > 0)
		{
			pPropertyPart->setProperty_Integer(PROPERTY_SKIN, nNewSkinID);
		}

        // 需要切换的部件 注意下切换顺序
        static int nChangeParts[] = {PART_SPELL, PART_BUFF, PART_PROPERTY, PART_AOI, PART_MOBAGOODS, PART_WAR};
        
        // 切换前
        for(int i = 0; i < sizeof(nChangeParts)/sizeof(int); ++i)
        {
            __IEntityPart* pPart = getEntityPart((ENTITY_PART)nChangeParts[i]);
            if(pPart != NULL)
            {
                pPart->onChangeHero(nOldVoc, nNewVoc, true);
            }
        }

        // 切换后
        for(int i = 0; i < sizeof(nChangeParts)/sizeof(int); ++i)
        {
            __IEntityPart* pPart = getEntityPart((ENTITY_PART)nChangeParts[i]);
            if(pPart != NULL)
            {
                pPart->onChangeHero(nOldVoc, nNewVoc, false);
            }
        }

        // 发事件
        pEntityEvent->FireExecute(EVENT_ENTITY_CHANGE_HERO, (char *)&eventChange, sizeof(eventChange));
    }

    return true;
}

void PlayerRole:: updateName(char* pNewName)
{
	sstrcpyn(m_szName,pNewName,sizeof(m_szName));
}

int PlayerRole::mobaGoodsAddEffect(int nEffectID, void * pAddContext, size_t len)
{
    if (m_pMobaGoodsEffect == NULL)
    {
        return 0;
    }
    return m_pMobaGoodsEffect->mobaGoodsAddEffect(nEffectID, pAddContext, len);

}

void PlayerRole::mobaGoodsRemoveEffect(int nID)
{
    if (m_pMobaGoodsEffect == NULL)
    {
        return;
    }
    m_pMobaGoodsEffect->mobaGoodsRemoveEffect(nID);

}

void PlayerRole::releaseMobaGoodsEffect()
{
    if (m_pMobaGoodsEffect == NULL)
    {
        return;
    }
    m_pMobaGoodsEffect->releaseMobaGoodsEffect();
}

// 通知LUA添加交互回复
void PlayerRole::sendLuaAddContactReply(LONGLONG llContactID)
{
	// 通知任务
	if (m_pEventEngine == NULL)
		return;

	event_game_actor_data_task eventdata;
	eventdata.nTaskLuaEvent = emTaskLuaEventType_ContactReply;
	eventdata.dwPDBID = getIntProperty(PROPERTY_ID);
	eventdata.nParam1 = (int)HIGHLONG(llContactID);
	eventdata.nParam2 = (int)LOWLONG(llContactID);

	m_pEventEngine->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
}

// 通知LUA添加好友申请
void PlayerRole::sendLuaContactApplyFriend(DWORD dwTargetID, bool bIsDifSex)
{
	// 通知任务
	if (m_pEventEngine == NULL)
		return;

	event_game_actor_data_task eventdata;
	eventdata.nTaskLuaEvent = emTaskLuaEventType_ApplyFriend;
	eventdata.dwPDBID = getIntProperty(PROPERTY_ID);
	eventdata.nParam1 = (int)dwTargetID;
	eventdata.nParam2 = (int)bIsDifSex;

	m_pEventEngine->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
}
