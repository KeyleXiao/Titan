/*******************************************************************
** 文件名:	PlayerBankPart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-22
** 版  本:	1.0
** 描  述:	实体数据仓库部件

********************************************************************/

#include "stdafx.h"
#include "PlayerBankPart.h"
#include "IServerGlobal.h"
#include "ISceneManager.h"
#include "IEntityProperty.h"
#include "IPlayerRole.h"
#include "IEntityEvent.h"
#include "EntityHelper.h"
#include "MatchSchemeDef.h"
#include "ICenterConnectorService.h"
#include "OrderMsgDef.h"
#include "IBuffServer.h"
#include "IServerGlobal.h"
#include "ITimerService.h"
#include "IKinSceneService.h"
#include "IClanSceneService.h"
#include "ClanHelper.h"
#include "TaskStorageDef.h"
#include "ISpell.h"
#include "IShareServer.h "
#include "ExternalHelper.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"
#include "IAntiAddictServer.h"
#include "IMailSceneService.h"
#include "MailHelper.h"
#include "Mail_ManagedDef.h"
#include "StaticWarSceneHelper.h"
#include "IEntityAOI.h"
#include "ITestServer.h"

using namespace Mail_ManagerDef;


CPlayerBankPart::CPlayerBankPart()
{
	m_pMaster = NULL;

	// 个人任务数据
	m_pTaskData = NULL;
	m_nIntTaskDataLen = 0;
	m_pIntTaskData = NULL;
	m_nByteTaskDataLen = 0;
	m_pByteTaskData = NULL;
	m_bWaitForLoadTaskData = false;

	// 是否正在增加金币给DB，DB返回后自动设置false
	m_isAddingTicketToDB = false;

	m_mapMatchTypeRank.clear();
	m_mapMatchHeroScore.clear();

	memset(m_cRenameCache,0,DBDEF_ACTORNAME_MAXSIZE);
	nRenameMoneyCache = 0;
}

CPlayerBankPart::~CPlayerBankPart()
{
	SAFE_DELETEARRAY(m_pIntTaskData);
	SAFE_DELETEARRAY(m_pByteTaskData);
}

///////////////////////////////// ISerializable ////////////////////////////
bool CPlayerBankPart::onSerialize( rkt::ibuffer & in,int flag )
{
	if (!in.good())
	{
		return false;
	}


	if(flag & TO_DATABASE)
	{
		void* pBuf = NULL;
		int nBufLen = 0;
		DWORD dwIDCard = 0;
		bool bIsAdult = false;
        DWORD dwPassportFlags = 0;	  // 通行证相关标识 PassportFlags
        DWORD dwServiceFlags = 0;     // 服务器标识 ServiceFlags
        string strPwd;
		in >> pBuf >> nBufLen >> dwIDCard >> bIsAdult >> dwPassportFlags >> dwServiceFlags >> strPwd;
		if(nBufLen < sizeof(DBREQ_RESULT_SELECTACTOR))
		{
			ErrorLn(__FUNCTION__ << __LINE__ << ", format error");
			return false;
		}

		DBREQ_RESULT_SELECTACTOR* pResult = (DBREQ_RESULT_SELECTACTOR*)pBuf;
		m_playInfo.dwUserID = pResult->dwUserID;
		m_playInfo.dwIdCard = dwIDCard;
		m_playInfo.bIsAdult = bIsAdult;
        m_playInfo.dwPassportFlags = dwPassportFlags;
        m_playInfo.dwServiceFlags = dwServiceFlags;
		m_playInfo.herocnt = pResult->herocnt;
		m_playInfo.player.dwUserID = pResult->dwUserID;
		m_playInfo.player.dwWorldID = pResult->dwWorldID;
		m_playInfo.player.dwActorID = pResult->dwActorID;
		sstrcpyn( m_playInfo.player.szActorName, pResult->szActorName, sizeof(m_playInfo.player.szActorName));
        sstrcpyn(m_playInfo.player.szPwd, strPwd.c_str(), sizeof(m_playInfo.player.szPwd));
		m_playInfo.player.nHeroID = pResult->nHeroID;
		m_playInfo.player.nSkinID = pResult->nSkinID;
		m_playInfo.player.nLevel = pResult->nLevel;
		m_playInfo.player.bSex = pResult->bSex;
		m_playInfo.player.dwMapId = pResult->dwMapId;
		m_playInfo.player.nLocX = pResult->nLocX;
		m_playInfo.player.nLocY = pResult->nLocY;
		m_playInfo.player.nLocZ = pResult->nLocZ;
        m_playInfo.player.nAngle = pResult->nAngle;
		m_playInfo.player.dwExp = pResult->dwExp;
		m_playInfo.player.dwBindTicket = pResult->dwBindTicket; // 金币
		m_playInfo.player.dwTicket = pResult->dwTicket;         // 点券
		m_playInfo.player.dwHeroPoolLimit = pResult->dwHeroPoolLimit;
		m_playInfo.player.wHeadID = pResult->wHeadID;
		m_playInfo.player.dwClanID = pResult->dwClanID;
		m_playInfo.player.dwKinID = pResult->dwKinID;
		m_playInfo.player.dwTeamID = pResult->dwTeamID;
		memcpy(m_playInfo.player.bySummonerSkill, pResult->bySummonerSkill, sizeof(m_playInfo.player.bySummonerSkill));
		memcpy(m_playInfo.player.byGlory, pResult->byGlory, sizeof(m_playInfo.player.byGlory));
		memcpy(m_playInfo.player.byCampHeroID, pResult->byCampHeroID, sizeof(m_playInfo.player.byCampHeroID));
		m_playInfo.player.dwFightCapacity = pResult->dwFightCapacity;
		m_playInfo.player.dwPKWinNum = pResult->dwPKWinNum;
		m_playInfo.player.dwPKTotalNum = pResult->dwPKTotalNum;
		m_playInfo.player.dwSupportNum = pResult->dwSupportNum;
        m_playInfo.player.nPrivilege = pResult->nPrivilege;

		ibuffer inbuf(pResult + 1, nBufLen - sizeof(DBREQ_RESULT_SELECTACTOR));

#ifdef FUNCTION_RUNE_OPEN
        m_runeManager.onSerialize(inbuf);
#endif
		// 取出英雄池数据
		return m_heroMgr.onSerialize(inbuf, m_playInfo.herocnt);
	}

	in >> m_playInfo;

#ifdef FUNCTION_RUNE_OPEN
    m_runeManager.onSerialize(in);
#endif

	if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
	{
		m_heroMgr.onSerialize(in, m_playInfo.herocnt);
	}

	// 人物段位信息
    if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {
        m_mapMatchTypeRank.clear();
        int nCount = 0;
        in >> nCount;
        for(int i = 0; i < nCount; ++i)
        {
            if (!in.good())
            {
                return false;
            }

            MatchTypeRankNode node;
            in >> node;

            m_mapMatchTypeRank[(EMMatchType)node.nMatchType] = node;
        }
    }


	// 增益卡数据
	if (!m_beneficardMgr.onSerialize(in))
		return false;

	if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
	{

#ifdef FUNCTION_TALENTPAGE_OPEN
		m_talentPageMgr.onSerializePage(in);
#endif

#ifdef FUNCTION_RUNE_OPEN
        m_runeManager.onSerializePage(in);
#endif

		// 任务数据
		in >> m_nIntTaskDataLen;
		if (m_nIntTaskDataLen > 0)
		{
			SAFE_DELETEARRAY(m_pIntTaskData);
			m_pIntTaskData = new BYTE[m_nIntTaskDataLen];
			in.pop(m_pIntTaskData, m_nIntTaskDataLen);
		}

		in >> m_nByteTaskDataLen;
		if (m_nByteTaskDataLen > 0)
		{
			SAFE_DELETEARRAY(m_pByteTaskData);
			m_pByteTaskData = new BYTE[m_nByteTaskDataLen];
			in.pop(m_pByteTaskData, m_nByteTaskDataLen);
		}
	}

	m_actorLog.onSerialize(in, flag);

	m_crownPageMgr.onSerialize(in);

	if (!m_CommonMgr.onSerialize(in, flag))
		return false;

	if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
	{
		if (!m_actorPrizeMgr.onSerialize(in))
			return false;

		if (!m_rankSeasonMgr.onSerialize(in))
			return false;

		if (!m_emojiMgr.onSerialize(in))
			return false;
	}

	return true;
}

bool CPlayerBankPart::deSerialize( rkt::obuf & out,int flag )
{
	m_playInfo.herocnt = m_heroMgr.getHeroCount();
	//m_playInfo.player.dwBindTicket = m_pMaster->getIntProperty(PROPERTY_GOLD);
	out << m_playInfo;

	// 符文拥有信息
#ifdef FUNCTION_RUNE_OPEN
    m_runeManager.deSerialize(out, flag);
#endif

	if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
	{
		m_heroMgr.deSerialize(out);
	}

	//// 人物段位信息
    if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {
        int nCount = m_mapMatchTypeRank.size();
        out.push_back(&nCount, sizeof(int));
        for (MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.begin();iter != m_mapMatchTypeRank.end(); ++iter)
        {
            out.push_back(&iter->second, sizeof(MatchTypeRankNode));
        }
    }

	// 压入增益卡数据
	if (!m_beneficardMgr.deSerialize(out))
		return false;

	if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
	{    

#ifdef FUNCTION_TALENTPAGE_OPEN
		m_talentPageMgr.deSerializePage(out);
#endif

#ifdef FUNCTION_RUNE_OPEN
        m_runeManager.deSerializePage(out);
#endif

		// 压入任务数据
		BYTE byIntData[8000];
		int nIntDataLen = sizeof(byIntData);
		BYTE byByteData[8000];
		int nByteDataLen = sizeof(byByteData);
		if (m_pTaskData != NULL)
		{
			m_pTaskData->exportDBContext(m_pMaster->getIntProperty(PROPERTY_ID), byIntData, nIntDataLen, byByteData, nByteDataLen);
		}
		else
		{
			nIntDataLen = nByteDataLen = 0;
		}

		if (nIntDataLen > 0)
		{
			out.push_back(byIntData, nIntDataLen);
		}
		else
		{
			nIntDataLen = 0;
			out.push_back(&nIntDataLen, sizeof(int));
		}

		if (nByteDataLen > 0)
		{
			out.push_back(byByteData, nByteDataLen);
		}
		else
		{
			nByteDataLen = 0;
			out.push_back(&nByteDataLen, sizeof(int));
		}
	}


	m_actorLog.deSerialize(out, flag);

	m_crownPageMgr.deSerialize(out);

	if (!m_CommonMgr.deSerialize(out, flag))
		return false;

	if(flag&TO_OTHER_SEVER || flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
	{
		if (!m_actorPrizeMgr.deSerialize(out))
			return false;

		if (!m_rankSeasonMgr.deSerialize(out))
			return false;

		if (!m_emojiMgr.deSerialize(out))
			return false;
	}

	if(out.fail())
	{
		ErrorLn(__FUNCTION__ << ", out.fail()");
	}

	return true;
}

///////////////////////////////// __IEntityPart //////////////////////////
short CPlayerBankPart::getPartID()
{
	return PART_BANK;
}

bool CPlayerBankPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	m_pMaster = pEntity;

	m_pTaskData = CreatePlayerTaskDataProc();
	if(m_pTaskData == NULL)
	{
		return false;
	}
	__IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		ErrorLn("playerbank part onLoad pEntityEvent==NULL");
		return false;
	}

	// 个人上下日记类执行加载初始化
	m_actorLog.onLoad(pEntity, nLoginMode);
	// 皇冠初始化
	m_crownPageMgr.onLoad(pEntity,this);
	m_pTaskData->create(m_pMaster);

	if (!m_valueGoodsHelper.init(this, m_pMaster))
		return false;

	if (!m_heroMgr.init(pEntity,this, nLoginMode))
	{
		return false;
	}

	if (!m_pointShopMgr.init(this, m_pMaster))
		return false;
	if (!m_actorPrizeMgr.init(this, m_pMaster, nLoginMode))
		return false;
    if(!m_moneyMgr.init(this))
        return false;

	// 数据库接口注册
	gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	// 登陆上线时读取
	if(nLoginMode == elogin_online)
	{
#ifdef FUNCTION_TALENTPAGE_OPEN
		// 启动定时器
		gServerGlobal->getTimerService()->SetTimer(ETIMERID_READ_TALENT_FROM_DATABASE, READ_TALENT_INTERVAL, this,  1);
#endif 

#ifdef FUNCTION_RUNE_OPEN
		gServerGlobal->getTimerService()->SetTimer(ETIMERID_READ_RUNE_FROM_DATABASE, READ_RUNE_INTERVAL, this,  1);
#endif // #ifdef FUNCTION_RUNE_OPEN

		// 读取任务数据
		m_pTaskData->read();
	}
	else
	{
		m_bWaitForLoadTaskData = true;
	}

	initActorUnlockPrizeInfo();
	__IEntityProperty * pProperty = (__IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
	if (!pProperty)
	{
		ErrorLn(__FUNCTION__": can't find PART_PROPERTY");
		return false;
	}

	// 注册实体创建消息
	pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE, __FUNCTION__);
	pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_PREPARE_LOGOUT_SAVE, __FUNCTION__);
	pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_READTASKDATA, __FUNCTION__);
	pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CONTINUE_PLAY, __FUNCTION__);
	pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ACTOR_CHANGE_LEVEL, __FUNCTION__);
	pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_CHANGE_MONEY, __FUNCTION__);
    pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_CHANGE_HERO), __FUNCTION__);
    pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_LOGOUT, __FUNCTION__);
    
    
	IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
	if(pEventEngine)
	{
		// 订阅全局事件 新一天事件
		pEventEngine->Subscibe(this, EVENT_NEW_DAY,SOURCE_TYPE_SYSTEM,0, __FUNCTION__);
	}

	// 增益卡
	if (!m_beneficardMgr.init(m_pMaster, this))
		return false;

	// 杂项数据
	if (!m_CommonMgr.init(m_pMaster, this))
		return false;

	// 赛季数据管理
	if (!m_rankSeasonMgr.init(this, m_pMaster, nLoginMode))
		return false;

	// 表情包管理
	if (!m_emojiMgr.init(this, m_pMaster, nLoginMode))
		return false;

#ifdef FUNCTION_RUNE_OPEN
	// 符文
	if (!m_runeManager.onLoad(m_pMaster, this, &m_talentEffectProxy))
		return false;
#endif

#ifdef FUNCTION_TALENTPAGE_OPEN
	if (!m_talentPageMgr.onLoad(m_pMaster, this, &m_talentEffectProxy))
		return false;
#endif 

	return true;
}

bool CPlayerBankPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch(msgId)
	{
	case SS_MSG_ENTITY_UPDATE_EXTRACT_TICKET:
		{
			// 服务器更新角色提取点券
			onTransmitUpdateExtractTicket( data, len);
		}
		break;

	case CS_MSG_ENTITY_MATCHTYP_RANK:       // 人物撮合段位信息
		{
			if (len != sizeof(msg_entity_matchtype_rank))
			{
				return false;
			}
			msg_entity_matchtype_rank * pData = (msg_entity_matchtype_rank*)data; 
			OnClientRequestMatchTypeRank(pData);
		}
		break;

	case CS_MSG_ENTITY_MATCH_HEROSCORE:     // 人物英雄熟练度信息
		{
			if (len < sizeof(msg_entity_match_heroscore))
			{
				return false;
			}
			msg_entity_match_heroscore * pData = (msg_entity_match_heroscore*)data;
			OnClientRequestHeroScore(pData);
		}
		break;

	case CS_MSG_ENTITY_TALENTPAGEPOINTS_SET_SINGLE:         // 保存单页天赋页的加点数据
		{
#ifdef FUNCTION_TALENTPAGE_OPEN
			m_talentPageMgr.onMessage(pEntityHead, msgId, data, len);
#endif
		}
		break;
	case CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_EFFECT:       // 告诉服务器最后调用哪页的效果
		{
#ifdef FUNCTION_TALENTPAGE_OPEN
			m_talentPageMgr.onMessage(pEntityHead, msgId, data, len);
#endif
		}
		break;	
	case CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_DELETE:       // 删除某页
		{
#ifdef FUNCTION_TALENTPAGE_OPEN
			m_talentPageMgr.onMessage(pEntityHead, msgId, data, len);
#endif
		}
		break;

	case CS_MSG_ENTITY_RUNE_SAVE:       // 请求符文信息
		{
#ifdef FUNCTION_RUNE_OPEN
            m_runeManager.onMessage(pEntityHead, msgId, data, len);
#endif
		}
		break;
	case CS_MSG_ENTITY_RUNE_REQUEST_OTHER_INFO:
		{
#ifdef FUNCTION_RUNE_OPEN
            m_runeManager.onMessage(pEntityHead, msgId, data, len);
#endif
		}
		break;
	case CS_MSG_ENTITY_RUNE_DELETE:             // 客户端请求删除符文页
        {
#ifdef FUNCTION_RUNE_OPEN
            m_runeManager.onMessage(pEntityHead, msgId, data, len);
#endif // #ifdef FUNCTION_RUNE_OPEN
        }
        break;
	case CS_MSG_ENTITY_RUNE_INDEX_EFFECT:       // 告诉服务器最后调用哪页的效果
		{
#ifdef FUNCTION_RUNE_OPEN
            m_runeManager.onMessage(pEntityHead, msgId, data, len);
#endif // #ifdef FUNCTION_RUNE_OPEN
		}
		break;

	case CS_MSG_ENTITY_REQ_PLAYER_INFO:     // 客户端请求召唤师信息
		{
			onReqPlayerInfo(data, len);
		}
		break;

	case CS_MSG_ENTITY_REQ_HERO_INFO:       // 客户端请求已经拥有的英雄
		{
			onReqPlayerHeroInfo(data, len);
		}
		break;

	case CS_MSG_ENTITY_REQ_UPDATE_HERO_STAR:
		{
			if ( len < sizeof(int) )
			{
				return false;
			}
			int nHeroID = *(int*)data;
			onReqUpdageHeroStar(nHeroID);
		}
		break;

	case CS_MSG_ENTITY_CHANGE_HERO:     //切换英雄
		{
			if (onMessageChangeHero(data, len))
			{
				g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_PLAYER_CHANGEHERO_SUCESS);
			}
			else
			{
				g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_PLAYER_CHANGEHERO_FAIL);
			}
		}
		break;

	case CS_MSG_ENTITY_OPEN_CHEST:      // 打开宝箱
		{
			return m_actorPrizeMgr.onMessageOpenChest(data, len);
		}
		break;
	case CS_MSG_ENTITY_BROADCAST_CHEST_RESULT:		// 广播开宝箱结果
		{
			return m_actorPrizeMgr.broadcastChestResult();
		}
		break;
	case CS_MSG_ENTITY_POINTSHOP_BUYGOODS:      // 购买商城物品
		{
			return m_pointShopMgr.onMessagePointShopBuyGoods(data, len);
		}
		break;
	case CS_MSG_ENTITY_ACTORNAME_FIX:      // 名字修改
		{
			onClientRequestActorNameChange(data, len);
		}
		break;
	case CS_MSG_ENTITY_CROWNAGEMSTONEPPRAISAL:      // 宝石鉴定
		{
			m_crownPageMgr.onMessageRequestAppraisalGemstone((LPCSTR)data,len);
		}
		break;
	case CS_MSG_ENTITY_CROWNSAVEPAGE:    		   // 保存皇冠页
		{
			m_crownPageMgr.onMessageRequestSaveCrownPage((LPCSTR)data,len);
		}
		break;
	case CS_MSG_ENTITY_CROWNDELETEPAGE:      	  // 删除皇冠页
		{
			m_crownPageMgr.onMessageRequestDeleteCrownPage((LPCSTR)data, len);
		}
		break;
	case CS_MSG_ENTITY_CROWNDROOMSELECTPAGE:      	  // 大厅选择皇冠页
		{
			int nPageDBID = *((int*)data);
			m_crownPageMgr.onMessageRequestSwitchCrownPage(nPageDBID);
		}
		break;
	case CS_MSG_ENTITY_CROWNDCOBBLESTON_SELECTED: // 选择原石
		{
			m_crownPageMgr.onMessageGemstoneSelectCobblestone((LPCSTR)data, len);
		}
		break;
	case CS_MSG_ENTITY_CROWNDWINDOWSTONE:      	 // 花钱开窗
		{
			m_crownPageMgr.onMessageGemstoneOpenWindow((LPCSTR)data, len);
		}
		break;
	case CS_MSG_ENTITY_CROWNDWINDOWSTONE_SELECTED: // 选择开窗原石
		{
			m_crownPageMgr.onMessageGemstoneWindowOpenToSelecet((LPCSTR)data, len);
		}
		break;
	case CS_MSG_ENTITY_CROWNDGEMSTONE_SEIL:       // 宝石出售
		{
			m_crownPageMgr.onMessageRequestSaleGemstone((LPCSTR)data, len);
		}
		break;
	case  CS_MSG_ENTITY_UNLOCK_XP_SKILL_BYFRAGMENT:		// 客户端碎片集齐解锁XP
		{
			m_heroMgr.OnClientUnlockXPSkillByFragments(data, len);
		}
		break;
	case  CS_MSG_ENTITY_SELECT_XP_SKILL:		// 大厅动态点选英雄xp技能
		{
			m_heroMgr.OnClientSelectHeroXPSkill(data, len);
		}
		break;
	case  CS_MSG_ENTITY_SET_NOVICEGUIDE:		// 设置新手引导类型
		{
			onReqSetNoviceGuideData(data, len);
		}
		break;
	case CS_MSG_ENTITY_REFLUSH_CARD:
		{
			m_pointShopMgr.onMessageReflushCardList();
		}
		break;
	case CS_MSG_ENTITY_LIFEHERO_ADD:
	case CS_MSG_ENTITY_LIFEHERO_DEL:
	case CS_MSG_ENTITY_LIFEHERO_REPLACE:
	case CS_MSG_ENTITY_LIFEHERO_EXCHANGE:
	case CS_MSG_ENTITY_LIFEHERO_QUERY:
	case CS_MSG_ENTITY_LIFEHERO_OBTAIN_PRIZE:
		{
			m_heroMgr.onMessage(msgId, data, len);
		}
		break;
	case CS_MSG_ENTITY_REQ_SEASON_DETAIL:		// 请求赛季详细数据
		{
			m_rankSeasonMgr.onMessageRequestSeasonDetail();
		}
		break;
	case CS_MSG_ENTITY_REQ_SEASON_RECORD:		// 请求赛季历史数据
		{
			m_rankSeasonMgr.onMessageRequestSeasonRecord();
		}
		break;
	default:
		break;
	}
	return true;
}

__IEntity * CPlayerBankPart::getHost()
{
	return m_pMaster;
}

void CPlayerBankPart::restore()
{
    // 数据库接口反注册
    gServerGlobal->getDBEngineService()->unregisterHandler(this);

    IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
    if(pEventEngine) {
        // 取消订阅全局事件 新一天事件
        pEventEngine->UnSubscibe(this, EVENT_NEW_DAY,SOURCE_TYPE_SYSTEM,0);
    }

    if (m_pMaster != NULL) {
        __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
        if (pEntityEvent != NULL) {
            IEventExecuteSink * pSink = static_cast<IEventExecuteSink*>(this);
            pEntityEvent->UnSubscibe(pSink, EVENT_ENTITY_CREATE);
            pEntityEvent->UnSubscibe(pSink, EVENT_ACTOR_PREPARE_LOGOUT_SAVE);
            pEntityEvent->UnSubscibe(pSink, EVENT_ACTOR_READTASKDATA);
            pEntityEvent->UnSubscibe(pSink, EVENT_ENTITY_CONTINUE_PLAY);
            pEntityEvent->UnSubscibe(pSink, EVENT_ENTITY_ACTOR_CHANGE_LEVEL);
            pEntityEvent->UnSubscibe(pSink, EVENT_ACTOR_CHANGE_MONEY);
            pEntityEvent->UnSubscibe(pSink, (EventKey)(EVENT_ENTITY_CHANGE_HERO));
            pEntityEvent->UnSubscibe(pSink, EVENT_ACTOR_LOGOUT);
            
            
        }
    }

    // 时间注销
    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService) {
#ifdef FUNCTION_TALENTPAGE_OPEN
        pTimerService->KillTimer(ETIMERID_READ_TALENT_FROM_DATABASE, this);
#endif

#ifdef FUNCTION_RUNE_OPEN
        pTimerService->KillTimer(ETIMERID_READ_RUNE_FROM_DATABASE, this);
#endif
    }

    if(m_pTaskData) m_pTaskData->restore();

    m_actorLog.restore();
    m_heroMgr.restore();
	m_crownPageMgr.restore();
    m_pointShopMgr.restore();
	m_beneficardMgr.restore();
	m_CommonMgr.restore();
	m_actorPrizeMgr.restore();
	m_rankSeasonMgr.restore();
	m_emojiMgr.restore();
	m_valueGoodsHelper.restore();

#ifdef FUNCTION_RUNE_OPEN
    m_runeManager.restore();
#endif
#ifdef FUNCTION_TALENTPAGE_OPEN
    m_talentPageMgr.restore();
#endif

    m_pMaster = 0;
}

void CPlayerBankPart::release()
{
	if(m_pTaskData) m_pTaskData->release();
	m_pTaskData = NULL;

	// 个人上下日记类执行释放
	m_actorLog.release();
	m_heroMgr.release();
	m_crownPageMgr.release();
	m_pointShopMgr.release();
	m_beneficardMgr.release();
	m_CommonMgr.release();
	m_actorPrizeMgr.release();
	m_rankSeasonMgr.release();
	m_emojiMgr.release();
	m_valueGoodsHelper.release();

#ifdef FUNCTION_RUNE_OPEN
    m_runeManager.release();
#endif

#ifdef FUNCTION_TALENTPAGE_OPEN
	m_talentPageMgr.release();
#endif

	delete this;
}

///////////////////////////////// __IBankPart //////////////////////////	
// 获取角色信息
SPlayerInfo * CPlayerBankPart::getPlayerInfo(void)
{
	return &m_playInfo.player;
}

// 获取指定英雄信息
SHeroInfo * CPlayerBankPart::getHeroInfo(int nHeroID)
{
	return m_heroMgr.getHeroInfo(nHeroID);
}

// 校验指定英雄信息
SHeroInfo * CPlayerBankPart::checkHeroInfo( int nHeroID )
{
	return m_heroMgr.checkHeroInfo(nHeroID);
}

// 获取所有英雄
bool CPlayerBankPart::getHeroArray(obuf & outData)
{
	return m_heroMgr.getHeroArray(outData);
}

// 获得当前玩家战场可用的英雄
bool CPlayerBankPart::getPlayerCanUseHeroArray(obuf & outData)
{
	return m_heroMgr.getPlayerCanUseHeroArray(outData);
}


int CPlayerBankPart::getHeroCount()
{
	return m_heroMgr.getHeroCount();
}

DWORD CPlayerBankPart::getFromGameWorldID()
{
	return getPlayerInfo()->dwWorldID;
}

DWORD CPlayerBankPart::getUserID()
{
	return m_playInfo.dwUserID;
}

// 获取IdCard数据
DWORD CPlayerBankPart::getIdCard()
{
	return m_playInfo.dwIdCard;
}

// 设置IdCard数据
void CPlayerBankPart::setIdCard(DWORD dwIdCard)
{
	m_playInfo.dwIdCard = dwIdCard;

	// TODO 后面看要不要同步客户端
}

// 是否为成年人
bool CPlayerBankPart::isAdult()
{
	return m_playInfo.bIsAdult;
}

// 设置为成年人
void CPlayerBankPart::setAdult()
{
	m_playInfo.bIsAdult = true;

	// 同步客户端
	onUpdateChangedPlayerInfo(ENPLAYERINFOTYPE_ADULT);
}

// 获得玩家角色等级
int CPlayerBankPart::getPlayerLevel()
{
    return m_playInfo.player.nLevel;
}

// 获得玩家角色经验
int CPlayerBankPart::getPlayerExp()
{
    return m_playInfo.player.dwExp;
}

// 设置实名认证标志
void CPlayerBankPart::setRealNameAutherFlag()
{
	m_playInfo.dwPassportFlags |= RealNameAuthent;

	// 同步客户端
	onUpdateChangedPlayerInfo(ENPLAYERINFOTYPE_PASSPORTFLAG);
}

// 购买商城物品
bool CPlayerBankPart::onMessagePointShopBuyGoods(void *data, size_t len)
{
	return m_pointShopMgr.onMessagePointShopBuyGoods(data, len);
}

// 战场结束设置数据
void CPlayerBankPart::setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo)
{
	m_rankSeasonMgr.setMatchRankSeasonInfo(pInfo);
}

// 检查玩家能否使用英雄皮肤
bool CPlayerBankPart::canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID)
{
	return m_heroMgr.canUseHeroSkin(dwHeroID, dwHeroSkinID);
}

// 建立师徒关系
void CPlayerBankPart::buildMentorship(DWORD dwDesPDBID)
{
	// 通知任务
	__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent != NULL)
	{
		event_game_actor_data_task eventdata;
		eventdata.nTaskLuaEvent = emTaskLuaEventType_BuildMentorship;
		eventdata.dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

		pEntityEvent->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
	}
}

// 发送任务事件
void CPlayerBankPart::sendActorDataTaskEvent(WORD nTaskEvent, int nParam1, int nParam2, int nParam3)
{
	// 通知任务
	__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent != NULL)
	{
		event_game_actor_data_task eventdata;
		eventdata.nTaskLuaEvent = nTaskEvent;
		eventdata.dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
		eventdata.nParam1 = nParam1;
		eventdata.nParam2 = nParam2;
		eventdata.nParam3 = nParam3;

		pEntityEvent->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
	}
}

// 获取永久英雄个数
int CPlayerBankPart::getPermanentHeroCount()
{
	return m_heroMgr.getPermanentHeroCount();
}

// 获取皮肤总个数
int CPlayerBankPart::getTotalSkinCount()
{
	return m_heroMgr.getTotalSkinCount();
}

// 获取静态地图皮肤ID
int CPlayerBankPart::getStaticSkin()
{
    return m_playInfo.player.nSkinID;
}

// 获取达到等级英雄个数
int CPlayerBankPart::getHeroCountByLevel(WORD nLevel)
{
	return m_heroMgr.getHeroCountByLevel(nLevel);
}

IRuneManager* CPlayerBankPart::getRuneMgr()
{
	IRuneManager* pIRuneManager = NULL;

#ifdef FUNCTION_RUNE_OPEN
	pIRuneManager = &m_runeManager;
#endif

	return pIRuneManager;
}

void CPlayerBankPart::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	switch ( nCmdID )
	{
	case GAMEDB_REQUEST_SAVEACTOR:
		{
			if (nDBRetCode != DBRET_CODE_SUCCEED)
			{
				ErrorLn(__FUNCTION__ << "GAMEDB_REQUEST_SAVEACTOR, nCmdID=" << nCmdID << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
			}
		}
		break;

	case GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK:         // 获取Rank分值
		{
			OnDBReturnMatchTypeRank(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;

	case GAMEDB_REQUEST_GET_MATCH_ALL_HEROSCORE:        // 获取英雄熟练度得分
		{
			OnDBReturnMatchHeroScore(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;

	case GAMEDB_REQUEST_GAMETICKET_ADDMINE:            // 游戏世界数据库 用户点券数变化记录
		{
            getMoneyMgr()->onDBReturnTicketAdd(nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_ADDSERVICES_USER_CHK:
		{
			addService_ResponseFromDB(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 服务器更新角色提取点券
void CPlayerBankPart::onTransmitUpdateExtractTicket(void * data, size_t len)
{
	if(data == nullptr || len != sizeof(msg_entity_update_extract_ticket))
	{
		ErrorLn(__FUNCTION__": message length is invalid ! len="<<len);
		return;
	}

	msg_entity_update_extract_ticket * pMsg = (msg_entity_update_extract_ticket*)data;
	if(pMsg->dwUserID != getUserID())
	{
		ErrorLn(__FUNCTION__": message data is invalid ! userid="<<getUserID() <<", msg_userid="<<pMsg->dwUserID);
		return;
	}

	// 更新用户TICKET数据
	m_pMaster->setIntProperty(PROPERTY_TICKET, pMsg->nTotalTicket);
	// 修改PLAYER内存数据
	m_playInfo.player.dwTicket = pMsg->nTotalTicket;

	if (((__IPlayerRole*)m_pMaster)->getGameState() == GAME_STATE_IDLE)
	{
		char szParam[32];
		ssprintf_s(szParam, sizeof(szParam), a2utf8("%d"), pMsg->nTicket);
		g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_PLAYER_TOPUP_SUCCESS, szParam);
	}

    __IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
	if (pEventEngine)
	{
		event_player_topup_success eventData;
		eventData.dwPDBID = pMsg->dwPDBID;
		eventData.nTicket = pMsg->nTicket;
		pEventEngine->FireExecute(EVENT_ACTOR_TOPUP_SUCCESS, (LPCSTR)&eventData, sizeof(eventData));
	}

	EmphasisLn(__FUNCTION__": PDBID="<<pMsg->dwPDBID<<",nTotalTicket="<< pMsg->nTotalTicket)
}

void CPlayerBankPart::OnClientRequestMatchTypeRank(msg_entity_matchtype_rank* pRequestData)
{
	// 客户端请求Rank数据
	if (pRequestData == NULL)
	{
		return;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return;
	}

	DBREQ_GET_MATCHTYPE_ALL_RANK pData;
	if (pRequestData->bRequestSelf)
	{
		// 请求自己
		pData.dwUserID  = getUserID();   //账号ID
		pData.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);   // 角色ID
	}
	else
	{
		// 请求其他人
		pData.dwActorID = pRequestData->dwFriendID;
	}

	if (pData.dwActorID == INVALID_PDBID)
	{
		return;
	}

	ActorHelper actorHelper(m_pMaster->getUID());
	IActorService *pActorService = actorHelper.m_ptr;
	if ( pActorService == NULL)
	{
		return;
	}

	obuf oBuff;
	oBuff.push_back(&pData, sizeof(DBREQ_GET_MATCHTYPE_ALL_RANK));

    if (pDBEngine->executeSP(GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK, pData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK failed");
    }
}

void CPlayerBankPart::OnClientRequestHeroScore(msg_entity_match_heroscore * pRequestData)
{
	// 客户端请求英雄熟练度数据
	if (pRequestData == NULL)
	{
		return;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return;
	}

	DBREQ_GET_MATCH_ALL_HEROSCORE pData;
	if (pRequestData->bRequestSelf)
	{
		// 请求自己
		pData.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	}
	else
	{
		// 请求其他人
		pData.dwActorID = pRequestData->dwFriendID;
	}

	if (pData.dwActorID == INVALID_PDBID)
	{
		return;
	}

	ActorHelper actorHelper(m_pMaster->getUID());
	IActorService *pActorService = actorHelper.m_ptr;
	if ( pActorService == NULL)
	{
		return;
	}

	obuf oBuff;

	oBuff.push_back(&pData, sizeof(DBREQ_GET_MATCHTYPE_ALL_RANK));

	PDBID dwCurActorID = pActorService->getProperty_Integer(PROPERTY_ID);
	if (pDBEngine->executeSP(GAMEDB_REQUEST_GET_MATCH_ALL_HEROSCORE, dwCurActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_GET_MATCH_ALL_HEROSCORE failed");
	}
}

void CPlayerBankPart::OnDBReturnMatchTypeRank(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}

	// 校验返回数据
	if ( pOutData == NULL || nOutLen < sizeof(DBREQ_RESULT_MATCHTYPE_ALL_RANK) )
	{
		ErrorLn(__FUNCTION__": pData ==NULL or length is invalid, nOutLen="<<nOutLen <<", sizeof="<<sizeof(DBREQ_RESULT_MATCHTYPE_ALL_RANK));
		return;
	}
	DBREQ_RESULT_MATCHTYPE_ALL_RANK * pData = (DBREQ_RESULT_MATCHTYPE_ALL_RANK *)pOutData;
	if ( nOutLen != (sizeof(*pData) + pData->nReCount * sizeof(MatchTypeRankNode)))
	{
		ErrorLn(__FUNCTION__": pData length error !");
		return ;
	}

	m_mapMatchTypeRank.clear();

    rkt:: IEventEngine* pEntityEngine = gServerGlobal->getEventEngine();

	// 解析当前返回数据
	MatchTypeRankNode* pDataHead = (MatchTypeRankNode *)(pData +1);
	for(int i = 0; i < pData->nReCount; ++i)
	{
		MatchTypeRankNode Data;
		Data.nGrade = pDataHead->nGrade;
		Data.nMatchType = pDataHead->nMatchType;
		Data.nRankScore = pDataHead->nRankScore;
		Data.dwMatchNum = pDataHead->dwMatchNum;
		Data.dwWinNum = pDataHead->dwWinNum;
        Data.nHideRankScore = pDataHead->nHideRankScore;

		m_mapMatchTypeRank.insert(pair<EMMatchType,MatchTypeRankNode> ((EMMatchType)Data.nMatchType ,Data));

        syncMatchTypeRankInfo(Data);

		event_entity_update_matchrank eventdata;
		eventdata.idActor = m_playInfo.player.dwActorID;
		eventdata.nMatchType = Data.nMatchType;
		eventdata.nRankScore = pDataHead->nRankScore;
		eventdata.dwMatchNum = pDataHead->dwMatchNum;
		eventdata.dwWinNum = pDataHead->dwWinNum;
		eventdata.nHideRankScore = pDataHead->nHideRankScore;
		if(pEntityEngine != NULL)
		{
			pEntityEngine->FireExecute(EVENT_GAME_ACTOR_UPDATE_MATCHRANK, SOURCE_TYPE_PERSON, eventdata.idActor, (LPCSTR)&eventdata, sizeof(eventdata));
		}

		if (pDataHead->nMatchType == MatchType_Rank)
		{
			m_playInfo.player.dwRankMatchNum = pDataHead->dwMatchNum;   // 比赛次数
			m_playInfo.player.wRankWinNum = pDataHead->dwWinNum;		// 胜利场次 
			onUpdateChangedPlayerInfo(ENPLAYERINFOTYPE_RANKNUM);
		}
		pDataHead++;
	}

	// 下发到客户端 更新数据
	obuf obmsg;
	obmsg.push_back(pData, nOutLen);

    __IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if (pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_MATCHTYP_RANK, obmsg.data(), obmsg.size());
    }
}

void CPlayerBankPart::syncMatchTypeRankInfo(MatchTypeRankNode Data)
{
    if(m_pMaster == NULL)
        return;

    if (Data.nMatchType == MatchType_Rank)
    {
        // 更新本地
        __IEntityProperty * pProperty = (__IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
        if (!pProperty)
        {
            return;
        }

        if (pProperty->getProperty_Integer(PROPERTY_MATCHTYPE) != Data.nMatchType)
        {
            pProperty->setProperty_Integer(PROPERTY_MATCHTYPE, Data.nMatchType);
        }

        if (pProperty->getProperty_Integer(PROPERTY_RANKSCORE) != Data.nRankScore)
        {
            pProperty->setProperty_Integer(PROPERTY_RANKSCORE, Data.nRankScore) ;
        }

        if (pProperty->getProperty_Integer(PROPERTY_RANKGRADE) != Data.nGrade)
        {
            pProperty->setProperty_Integer(PROPERTY_RANKGRADE, Data.nGrade) ;
        }

        obuf ob;
        BYTE byMatchType = pProperty->getProperty_Integer(PROPERTY_MATCHTYPE);
        DWORD dwRankScore = pProperty->getProperty_Integer(PROPERTY_RANKSCORE);
        DWORD nGrade = pProperty->getProperty_Integer(PROPERTY_RANKGRADE);
        ob<<byMatchType<<dwRankScore<<nGrade;
        //size(BYTE) + size(DWORD) + size(DWORD)
        memcpy(m_playInfo.player.byRankbuffdata, ob.data(), sizeof(m_playInfo.player.byRankbuffdata));
        
    }
}

void CPlayerBankPart::OnDBReturnMatchHeroScore(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return;
	}

	// 校验返回数据
	if ( pOutData == NULL || nOutLen < sizeof(DBREQ_RESULT_MATCH_ALL_HEROSCORE) )
	{
		ErrorLn(__FUNCTION__": pData ==NULL or length is invalid, nOutLen="<<nOutLen <<", sizeof="<<sizeof(DBREQ_RESULT_MATCH_ALL_HEROSCORE));
		return;
	}
	DBREQ_RESULT_MATCH_ALL_HEROSCORE * pData = (DBREQ_RESULT_MATCH_ALL_HEROSCORE *)pOutData;
	if ( nOutLen != sizeof(*pData) + pData->nReCount * sizeof(MatchHeroScoreNode))
	{
		ErrorLn(__FUNCTION__": pData length error !");
		return ;
	}

	m_mapMatchHeroScore.clear();

	// 解析当前返回数据
	MatchHeroScoreNode* pDataHead = (MatchHeroScoreNode *)(pData + 1);
	for(int i = 0; i < pData->nReCount; ++i)
	{
		MatchHeroScoreNode Data;    
		Data.nHeroID = pDataHead->nHeroID;          // 英雄ID
		Data.nHeroScore = pDataHead->nHeroScore;    // 英雄熟练度得分
		Data.nHeroGrade = pDataHead->nHeroGrade;    // 英雄熟练度段位

		m_mapMatchHeroScore.insert(pair<int, MatchHeroScoreNode>( Data.nHeroID, Data));
	}

	// 下发到客户端 更新数据
	obuf obmsg;
	obmsg.push_back(pData, nOutLen);

    __IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if (pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_MATCH_HEROSCORE, obmsg.data(), obmsg.size());
    }
}

void CPlayerBankPart::onReqPlayerInfo(void * data, size_t nLen)
{
	if (m_pMaster == NULL)
	{
		ErrorLn(__FUNCTION__ << " : m_pMaster == NULL");
		return;
	}
	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	if (data == NULL || nLen < sizeof(msg_req_player_info))
	{
		ErrorLn(__FUNCTION__ << " : message data is null or length is invalid! len = " << nLen << ", sizeof = " << sizeof(msg_req_player_info) << ", PDBID = " << dwPDBID);
		return;
	}

	msg_req_player_info *pRequestData = (msg_req_player_info*)data;
	if (pRequestData->dwActorID == INVALID_PDBID)
	{
		ErrorLn(__FUNCTION__ << "pRequestPDBID == INVALID_PDBID");
		return;
	}

	// 自身，取序列化结构
	if (pRequestData->dwActorID == dwPDBID)
	{
		msg_entity_player_info_return playerInfo;
		SPlayerInfo & data = m_playInfo.player;
		fillPlayerInfo(playerInfo, data);

		obuf obHeroData;
		playerInfo.nCount = m_heroMgr.fillHeroInfo(obHeroData);

		obuf obData;
		obData.push_back(&playerInfo, sizeof(msg_entity_player_info_return));
		obData.push_back( obHeroData.data(), obHeroData.size());
		__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
		if (pPlayerRole)
		{
			pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_RETURN_PLAYER_INFO, obData.data(), obData.size());
		}
	}
	else
	{
		StaticWarSceneHelper helper;
		IStaticWarSceneService * pStaticWarSceneService = helper.m_ptr;
		if (pStaticWarSceneService != NULL)
		{
			pStaticWarSceneService->reqOtherPlayerInfo(dwPDBID, pRequestData->dwActorID,pRequestData->bFake);
		}
	}
}

void CPlayerBankPart::fillPlayerInfo(msg_entity_player_info_return & playerInfo, SPlayerInfo& info)
{
	playerInfo.pDbid = info.dwActorID;
	playerInfo.nLv = info.nLevel;
	playerInfo.nHeadID = info.wHeadID;
	playerInfo.nHeroID = info.nHeroID;
	playerInfo.nSkinID = info.nSkinID;
	playerInfo.nSupportNum = info.dwSupportNum;
    playerInfo.dwRankMatchNum = info.dwRankMatchNum;                    // 排位比赛次数
    playerInfo.wRankWinNum = info.wRankWinNum;                      // 排位胜利场次 
    
    memcpy(&playerInfo.nRankGrade, info.byRankbuffdata + sizeof(BYTE)+ sizeof(DWORD), sizeof(playerInfo.nRankGrade));
    memcpy(&playerInfo.nRankScore, info.byRankbuffdata + sizeof(BYTE), sizeof(playerInfo.nRankScore));
	sstrcpyn(playerInfo.chActorName, info.szActorName, sizeof(playerInfo.chActorName));

	// 战队信息
	int nKin = info.dwKinID;
	while (nKin > 0)
	{
		IKinScene *pKinScene = gServerGlobal->getKinScene();
		if (pKinScene == NULL)
		{
			break;
		}
		SERVICE_PTR pKinContainer = pKinScene->getKinSceneService();
		if (pKinContainer == NULL)
		{
			break;
		}
		IKinSceneService *pScene = (IKinSceneService*)pKinContainer->query_interface();
		if (pScene == NULL)
		{
			break;
		}
		SKinInfo sKin = pScene->getKinInfo(nKin);
		if (sKin.nKinProp_ID != 0)
		{
			sstrcpyn(playerInfo.chKinName, sKin.szName, sizeof(playerInfo.chKinName));
		}
		break;
	}

	// 帮会信息
	int nClan = info.dwClanID;
	while (nClan > 0)
	{
		ClanHelper clanHelper;
		IClanSceneService * pClanService = clanHelper.m_ptr;
		if (pClanService != NULL)
		{
			sstrcpyn(playerInfo.chClanName, pClanService->getClanName(nClan).c_str(), sizeof(playerInfo.chClanName));
		}
		break;
	}
	sstrcpyn(playerInfo.chClanPos, "", sizeof(playerInfo.chClanPos));


	for (int i = 0; i < ENACHIEVE_MAX; i++)
	{
		memcpy(&playerInfo.dwGlory[i], info.byGlory + i * 12, sizeof(DWORD));
		memcpy(&playerInfo.llWarID[i], info.byGlory + i * 12 + 4, sizeof(LONGLONG));
	}
	playerInfo.dwCurrentExp = info.dwExp;
	playerInfo.dwFighting = info.dwFightCapacity;
	playerInfo.dwPKWinNum = info.dwPKWinNum;
	playerInfo.dwPKTotalNum = info.dwPKTotalNum;
	// dwGloryScore(荣耀积分)暂未赋值,如何获取？
	// nRankScore（排位得分）函数外另外获取
}

void CPlayerBankPart::fillPlayerInfo(DBREQ_PARAM_SAVEACTOR & savePlayerInfo, SPlayerInfo & info)
{
	savePlayerInfo.dwUserID = info.dwUserID;
	savePlayerInfo.dwWorldID = info.dwWorldID;
	savePlayerInfo.dwActorID = info.dwActorID;
	sstrcpyn(savePlayerInfo.szActorName, info.szActorName, sizeof(savePlayerInfo.szActorName));
	savePlayerInfo.nHeroID = info.nHeroID;
	savePlayerInfo.nSkinID = info.nSkinID;
	savePlayerInfo.nLevel = info.nLevel;
	savePlayerInfo.bSex = info.bSex;

	savePlayerInfo.dwMapId = info.dwMapId;
	savePlayerInfo.nLocX = info.nLocX;
	savePlayerInfo.nLocY = info.nLocY;
	savePlayerInfo.nLocZ = info.nLocZ;
    savePlayerInfo.nAngle = info.nAngle;

	savePlayerInfo.dwExp = info.dwExp;
	//savePlayerInfo.dwBindTicket = info.dwBindTicket;
	//savePlayerInfo.dwTicket = info.dwTicket;
	savePlayerInfo.dwHeroPoolLimit = info.dwHeroPoolLimit;
	savePlayerInfo.wHeadID = info.wHeadID;
	savePlayerInfo.dwClanID = info.dwClanID;
	savePlayerInfo.dwKinID = info.dwKinID;
	savePlayerInfo.dwTeamID = info.dwTeamID;
	memcpy(savePlayerInfo.bySummonerSkill, info.bySummonerSkill, sizeof(savePlayerInfo.bySummonerSkill));
	memcpy(savePlayerInfo.byGlory, info.byGlory, sizeof(savePlayerInfo.byGlory));
	savePlayerInfo.dwFightCapacity = info.dwFightCapacity;
	memcpy(savePlayerInfo.byCampHeroID, info.byCampHeroID, sizeof(savePlayerInfo.byCampHeroID));
	savePlayerInfo.dwPKWinNum = info.dwPKWinNum;
	savePlayerInfo.dwPKTotalNum = info.dwPKTotalNum;
	memcpy(savePlayerInfo.byRankbuffdata, info.byRankbuffdata, sizeof(savePlayerInfo.byRankbuffdata));
}

void CPlayerBankPart::onReqPlayerHeroInfo(void * pData, size_t nLen)
{
	if (m_pMaster == NULL)
	{
		ErrorLn(__FUNCTION__ << " : m_pMaster == NULL");
		return;
	}

	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (pData == NULL || nLen < sizeof(msg_req_hero_info))
	{
		ErrorLn(__FUNCTION__ << " : message data is null or length is invalid! len = " << nLen << ", sizeof = " << sizeof(msg_req_hero_info) << ", PDBID = " << dwPDBID);
		return;
	}

	IDBEngineService * pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__ << " : failed pDBEngine == NULL");
		return;
	}

	msg_req_hero_info *pRequestData = (msg_req_hero_info*)pData;
	IShareReceiver * pShareReceiver = gServerGlobal->getShareReceiver();//在共享服取一下信息
	if (pRequestData->dwActorID == INVALID_PDBID || pShareReceiver == NULL)
	{
		ErrorLn(__FUNCTION__ << "pRequestPDBID == INVALID_PDBID or failed pShareReceiver == NULL");
		return;
	}

	// 自身，取序列化结构
	if (pRequestData->dwActorID == dwPDBID)
	{
		PLAYERHEROMAP heroMap = m_heroMgr.getHeroMap();

		// 存在nHeroID为0的情况，所以这里加个判定，实际计算nCount
		int nCount = 0;
		obuf obHeroData;
		PLAYERHEROMAP::iterator it = heroMap.begin();
		msg_entity_hero_info msgheroInfo;
		for (; it != heroMap.end(); ++it)
		{
			if (it->second.sHeroInfo.nHeroID != 0)
			{
				fillHeroInfoMsg(msgheroInfo, it->second.sHeroInfo);
				obHeroData.push_back(&msgheroInfo, sizeof(msg_entity_hero_info));
				nCount++;
			}
		}

		obuf obData;
		obData << pRequestData->dwActorID << nCount;
		obData.push_back(obHeroData.data(), obHeroData.size());
		__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
		if (pPlayerRole)
		{
			pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_RETURN_HERO_INFO, obData.data(), obData.size());
		}
	}
	else
	{
		StaticWarSceneHelper helper;
		IStaticWarSceneService * pStaticWarSceneService = helper.m_ptr;
		if (pStaticWarSceneService != NULL)
		{
			pStaticWarSceneService->reqOtherPlayerHeroInfo(dwPDBID, pRequestData->dwActorID, pRequestData->bFake);
		}
	}
}

// 处理玩家请求设置新手引导类型
void CPlayerBankPart::onReqSetNoviceGuideData(void * data, size_t nLen)
{
	if (m_pMaster == NULL)
	{
		ErrorLn(__FUNCTION__ << " : m_pMaster == NULL");
		return;
	}

	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (data == NULL || nLen < sizeof(msg_req_set_noviceguide))
	{
		ErrorLn(__FUNCTION__ << " : message data is null or length is invalid! len = " << nLen << ", sizeof = " << sizeof(msg_req_set_noviceguide) << ", PDBID = " << dwPDBID);
		return;
	}

	msg_req_set_noviceguide *pRecvData = (msg_req_set_noviceguide*)data;

	if (pRecvData->dwNoviceGuideType < ENNOVICEGUIDE_TYPE_GUID_NEWPLAYER || pRecvData->dwNoviceGuideType >= ENNOVICEGUIDE_TYPE_GUIDE_MAX)
	{
		return;
	}

	// 设置新手引导类型
	addTaskDataInt(TASKKEY_ACTOR_NOVICE_GUIDE, pRecvData->dwNoviceGuideType);

	sendNoviceGuideTypeToClient();

	// 发送新手引导步骤改变
	sendActorDataTaskEvent(emTaskLuaEventType_NoviceGuide, 0, 0, 0);
}

void CPlayerBankPart::fillHeroInfoMsg(msg_entity_hero_info &sMsg, const SHeroInfo &sHeroInfo)
{
	memset(&sMsg, 0, sizeof(sMsg));
	DWORD nMatchNum = sHeroInfo.dwMatchNum;
	sMsg.dwExp = sHeroInfo.dwHeroExp;
	sMsg.dwGameCount = nMatchNum;
	sMsg.nHeroID = sHeroInfo.nHeroID;
	sMsg.nLv = sHeroInfo.nLevel;
	sMsg.byStarLv = sHeroInfo.nHeroStar;
	if ( nMatchNum == 0 )
	{
		sMsg.nWinPro = 0;
	}
	else
	{
		sMsg.nWinPro = (sHeroInfo.dwWinNum * 100) / nMatchNum;
	}
	sMsg.dwUseTime = sHeroInfo.dwUseTime;
	sMsg.bIsBuy = m_heroMgr.isHeroHave(sHeroInfo.dwFlags);
	sMsg.nUseXPSkillID = sHeroInfo.nUseXPID;
	sMsg.dwUnlockTime = sHeroInfo.dwUnlockTime;
}

DWORD CPlayerBankPart::getMatchTypeRank(EMMatchType eMatchType)
{

	MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.find(eMatchType);
	if (iter == m_mapMatchTypeRank.end() || iter->second.nRankScore == 0)
	{
        // 获得配置信息
        ISchemeMatchRankConfigInfo * pSchemeRankInfo = gServerGlobal->getSchemeCenter()->getSchemeMatchRankConfig();
        if(pSchemeRankInfo == NULL)
        {
            ErrorLn(__FUNCTION__": pSchemeRankInfo == NULL");
            return 0;
        }

        ISchemeEntity *pSchemeEntity = gServerGlobal->getSchemeCenter()->getSchemeEntity();
        if (pSchemeEntity == NULL)
        {
            ErrorLn(__FUNCTION__": pSchemeEntity == NULL");
            return 0;
        }
        // 取得显示打印信息
		return pSchemeEntity->getEntityXml()->nMatchTypeBaseScore;
	}

	return iter->second.nRankScore;
}

bool CPlayerBankPart::isMatchHeroCheckPassed(EMMatchType eMatchType)
{
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (nullptr == pSchemeCenter)
	{
        ErrorLn(__FUNCTION__ ": nullptr == pSchemeCenter");
		return false;
	}
    ISchemeMatchRankLimitInfo* pRankLimit = pSchemeCenter->getSchemeMatchRankLimit();
    SMatchRankLimitHeroSchemeInfo* pSchemeLimit = pRankLimit->getRankLimitHeroShemeInfo(eMatchType);
    if(pSchemeLimit == NULL)
    {
        return true;
    }

    int nCount = m_heroMgr.getAppointHeroStarCount(pSchemeLimit->nLimitHeroStarLevel);
    if (nCount >= pSchemeLimit->nLimitHeroCount)
    {
        return true;
    }

    return false;
}


DWORD CPlayerBankPart::getMatchTypeRankGrade(EMMatchType eMatchType)
{
	MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.find(eMatchType);
	if (iter == m_mapMatchTypeRank.end())
	{
		return 0;
	}
	return iter->second.nGrade;
}


bool CPlayerBankPart::setMatchTypeGrade(EMMatchType eMatchType,int nGrade)
{
    if(m_pMaster == NULL)
    {
        return false;
    }

    ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
    if(pConnector == NULL)
    {
        ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
        return false;
    }

    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return false;
    }

    /// 获取配置中心接口
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (!pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" -- pSchemeCenter == NULL");
        return false;
    }

    ISchemeMatchRankConfigInfo* pSchemeInfo = pSchemeCenter->getSchemeMatchRankConfig() ;
    if (pSchemeInfo == NULL)
    {
        ErrorLn(__FUNCTION__": failed pSchemeinfo == NULL");
        return false;
    }


    ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
    if (pSchemeEntity == NULL)
    {
        ErrorLn(__FUNCTION__" -- pSchemeEntity == NULL");
        return false;
    }

    msg_entity_match_single_rank sMsgInfo;

    // 更新本地缓存记录
    MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.find(eMatchType);
    if (iter == m_mapMatchTypeRank.end())
    {
        return false;
    }
    else
    {
        // 设置段位等级
        if (nGrade == iter->second.nGrade)
        {
            return false;
        }

        const SMatchRankConfigSchemeInfo* pMatchTypeSchemeInfo = pSchemeInfo->getMatchRankConfigShemeInfo(eMatchType, nGrade);
        if(pMatchTypeSchemeInfo != NULL)
        {
            iter->second.nGrade = pMatchTypeSchemeInfo->nGradeID;
        }
    }

    // 数据
    MATCHTYPERANK_MAP::iterator iterFind = m_mapMatchTypeRank.find(eMatchType);
    if(iterFind != m_mapMatchTypeRank.end())
    {
        // 同步及更新shareservice排位属性
        syncMatchTypeRankInfo(iterFind->second);
        event_entity_update_matchrank eventdata;
        eventdata.idActor = m_playInfo.player.dwActorID;
        eventdata.nMatchType = eMatchType;
        eventdata.nRankScore = iterFind->second.nRankScore;
        eventdata.dwMatchNum = iterFind->second.dwMatchNum;
        eventdata.dwWinNum = iterFind->second.dwWinNum;
        eventdata.nHideRankScore = iterFind->second.nHideRankScore;         // 隐藏段位分
        rkt:: IEventEngine* pEntityEngine = gServerGlobal->getEventEngine();
        if(pEntityEngine != NULL)
        {
            pEntityEngine->FireExecute(EVENT_GAME_ACTOR_UPDATE_MATCHRANK, SOURCE_TYPE_PERSON, eventdata.idActor, (LPCSTR)&eventdata, sizeof(eventdata));
        }

        // 设置数据库Rank数据
        DBREQ_SET_MATCHTYPE_RANK sData;
        sData.dwUserID      = getUserID();                              // 玩家账号ID
        sData.dwActorID     = m_pMaster->getIntProperty(PROPERTY_ID);   // 角色ID
        sData.nMatchType    = eMatchType;	                            // 撮合类型
        sData.nRankScore    = iterFind->second.nRankScore;              // 得分
        sData.nGrade        = iterFind->second.nGrade;                  // 段位
        sData.dwMatchNum	= iterFind->second.dwMatchNum;				// 匹配总数
        sData.dwWinNum		= iterFind->second.dwWinNum;				// 胜利总数
        sData.nHideRankScore = iterFind->second.nHideRankScore;         // 隐藏段位分
        obuf oBuff;
        oBuff.push_back(&sData, sizeof(DBREQ_SET_MATCHTYPE_RANK));

        if (pDBEngine->executeSP(GAMEDB_REQUEST_SET_MATCHTYPE_RANK, sData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_SET_MATCHTYPE_RANK failed");
            return false;
        }

        // 角色段位升级 (角色段位等级变化时记录)
        if (sMsgInfo.bRiseGrade || sMsgInfo.bFallGrade)
        {
            IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
            if (pOSSLogService != NULL)
                pOSSLogService->addActorUpgradeLog(m_playInfo.player.dwWorldID, m_playInfo.dwUserID, m_playInfo.player.dwActorID, 2, sMsgInfo.RankNodeInfo.nGrade, time(NULL), eMatchType);
        }
    }
    return true;

}
bool CPlayerBankPart::setMatchTypeRank(EMMatchType eMatchType,int nRankScore,int nHideRankScore)
{
	if(m_pMaster == NULL)
	{
		return false;
	}

    if (nRankScore < 0)
    {
        ErrorLn(__FUNCTION__": nRankScore can't less 0");
        return false;
    }

    if (nHideRankScore < 0)
    {
        ErrorLn(__FUNCTION__": nHideRankScore can't less 0");
        return false;
    }

	ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
	if(pConnector == NULL)
	{
		ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
		return false;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}

    /// 获取配置中心接口
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (!pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" -- pSchemeCenter == NULL");
        return false;
    }

    ISchemeMatchRankConfigInfo* pSchemeInfo = pSchemeCenter->getSchemeMatchRankConfig() ;
    if (pSchemeInfo == NULL)
    {
        ErrorLn(__FUNCTION__": failed pSchemeinfo == NULL");
        return false;
    }


    ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
    if (pSchemeEntity == NULL)
    {
        ErrorLn(__FUNCTION__" -- pSchemeEntity == NULL");
        return false;
    }

    int nMatchTypeBaseScore = pSchemeEntity->getEntityXml()->nMatchTypeBaseScore;

	__IPlayerRole* pMaster = CAST_TYPE(__IPlayerRole*, m_pMaster);

	msg_entity_match_single_rank sMsgInfo;

	// 更新本地缓存记录
	MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.find(eMatchType);
	if (iter == m_mapMatchTypeRank.end())
	{
		// 计算段位等级 添加基础分
		SMatchRankConfigSchemeInfo* pMatchTypeSchemeInfo = pSchemeInfo->getShemeInfoByTypeAndScore(eMatchType, nRankScore);
		if(pMatchTypeSchemeInfo != NULL)
		{
			MatchTypeRankNode pNode;
			pNode.nMatchType    = eMatchType;
			pNode.nRankScore    = nRankScore;

			// 设置客户端消息
			sMsgInfo.bRiseGrade         = true;     // 晋级 动画相关
			sMsgInfo.bFallGrade         = false;    // 降级 动画相关

			pNode.nGrade                = pMatchTypeSchemeInfo->nGradeID;
            pNode.nHideRankScore        = nHideRankScore;
			m_mapMatchTypeRank[eMatchType] = pNode;
		}
	}
	else
	{
		// 计算新得分
		iter->second.nRankScore             = nRankScore;
        iter->second.nHideRankScore         = nHideRankScore;
		// 查找新段位等级
		SMatchRankConfigSchemeInfo* pMatchTypeSchemeInfo = pSchemeInfo->getShemeInfoByTypeAndScore(eMatchType, iter->second.nRankScore);
		if(pMatchTypeSchemeInfo != NULL)
		{
			int nOldGrade = iter->second.nGrade;
			int nNewGrade = pMatchTypeSchemeInfo->nGradeID;
			if( nOldGrade != nNewGrade)
			{
				// 升级
				if (nNewGrade > nOldGrade)
				{
					sMsgInfo.bRiseGrade         = true;     // 晋级 动画相关
					sMsgInfo.bFallGrade         = false;    // 降级 动画相关
				}
				//降级
				else if (nNewGrade < nOldGrade)
				{
					sMsgInfo.bRiseGrade         = false;    // 晋级 动画相关
					sMsgInfo.bFallGrade         = true;     // 降级 动画相关
				}
			}
			else
			{
				// 设置客户端消息
				sMsgInfo.bRiseGrade = false;     // 晋级 动画相关
				sMsgInfo.bFallGrade = false;    // 降级 动画相关
			}

			// 设置段位等级
			iter->second.nGrade = pMatchTypeSchemeInfo->nGradeID;
		}        
	}

	// 数据
	MATCHTYPERANK_MAP::iterator iterFind = m_mapMatchTypeRank.find(eMatchType);
	if(iterFind != m_mapMatchTypeRank.end())
	{
        // 同步及更新shareservice排位属性
        syncMatchTypeRankInfo(iterFind->second);

		// 发送比赛数据更新事件
		event_entity_update_matchrank eventdata;
		eventdata.idActor = m_playInfo.player.dwActorID;
		eventdata.nMatchType = eMatchType;
		eventdata.nRankScore = iterFind->second.nRankScore;
		eventdata.dwMatchNum = iterFind->second.dwMatchNum;
		eventdata.dwWinNum = iterFind->second.dwWinNum;
        eventdata.nHideRankScore = iterFind->second.nHideRankScore;
		rkt:: IEventEngine* pEntityEngine = gServerGlobal->getEventEngine();
		if(pEntityEngine != NULL)
		{
			pEntityEngine->FireExecute(EVENT_GAME_ACTOR_UPDATE_MATCHRANK, SOURCE_TYPE_PERSON, eventdata.idActor, (LPCSTR)&eventdata, sizeof(eventdata));
		}

		// 同步到社会服
		obuf oData;
		SMsg_UpdatePersonMatchTypeRank SendData;
		SendData.idActor = m_pMaster->getIntProperty(PROPERTY_ID);
		SendData.eMatchType = eMatchType;
		SendData.RankNodeInfo = iterFind->second;
		memcpy(SendData.szActorName, m_pMaster->getName(), sizeof(SendData.szActorName));
		oData.push_back(&SendData, sizeof(SMsg_UpdatePersonMatchTypeRank));

		pConnector->sendToSocial(MSG_MODULEID_ORDER, SO_UPDATE_PERSON_MATCHTYPE_ORDERINFO, oData.data(), oData.size());

		// 下发到自己客户端 更新数据
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
		head.byKeyModule   = MSG_MODULEID_ENTITY;
		head.byKeyAction   = SC_MSG_ENTITY_MATCHTYP_SINGLE_RANK; // 人物撮合段位信息               

		// 部件
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
		entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

		// 设置下发数据
		sMsgInfo.RankNodeInfo = iterFind->second;

		obuf obmsg;
		obmsg << head << entityHead;
		obmsg.push_back(&(sMsgInfo), sizeof(msg_entity_match_single_rank));
		pMaster->send2Client(obmsg.data(),obmsg.size());

		// 设置数据库Rank数据
		DBREQ_SET_MATCHTYPE_RANK sData;
		sData.dwUserID      = getUserID();                              // 玩家账号ID
		sData.dwActorID     = m_pMaster->getIntProperty(PROPERTY_ID);   // 角色ID
		sData.nMatchType    = eMatchType;	                            // 撮合类型
		sData.nRankScore    = iterFind->second.nRankScore;              // 得分
		sData.nGrade        = iterFind->second.nGrade;                  // 段位
		sData.dwMatchNum	= iterFind->second.dwMatchNum;				// 匹配总数
		sData.dwWinNum		= iterFind->second.dwWinNum;				// 胜利总数
        sData.nHideRankScore = iterFind->second.nHideRankScore;         // 隐藏段位分
		obuf oBuff;
		oBuff.push_back(&sData, sizeof(DBREQ_SET_MATCHTYPE_RANK));

		if (pDBEngine->executeSP(GAMEDB_REQUEST_SET_MATCHTYPE_RANK, sData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_SET_MATCHTYPE_RANK failed");
			return false;
		}

		// 角色段位升级 (角色段位等级变化时记录)
		if (sMsgInfo.bRiseGrade || sMsgInfo.bFallGrade)
		{
			IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
			if (pOSSLogService != NULL)
			    pOSSLogService->addActorUpgradeLog(m_playInfo.player.dwWorldID, m_playInfo.dwUserID, m_playInfo.player.dwActorID, 2, sMsgInfo.RankNodeInfo.nGrade, time(NULL), eMatchType);
		}
	}

	setPlayerFightCapacity();

	return true;
}

bool CPlayerBankPart::addMatchTypeRank(EMMatchType eMatchType,int nRankScore, int nHideRankScore)
{
	if(m_pMaster == NULL)
	{
		return false;
	}
    __ITestPart * pTestPart = (__ITestPart *)m_pMaster->getEntityPart(PART_TEST);
    if (pTestPart->isTester())
    {
        return false;
    }

	ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
	if(pConnector == NULL)
	{
		ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
		return false;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}
    /// 获取配置中心接口
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (!pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" -- pSchemeCenter == NULL");
        return false;
    }

	ISchemeMatchRankConfigInfo* pSchemeInfo = pSchemeCenter->getSchemeMatchRankConfig() ;
	if (pSchemeInfo == NULL)
	{
		ErrorLn(__FUNCTION__": failed pSchemeinfo == NULL");
		return false;
	}

    
    ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
    if (pSchemeEntity == NULL)
    {
        ErrorLn(__FUNCTION__" -- pSchemeEntity == NULL");
        return false;
    }

    int nMatchTypeBaseScore = pSchemeEntity->getEntityXml()->nMatchTypeBaseScore;

	__IPlayerRole* pMaster = CAST_TYPE(__IPlayerRole*, m_pMaster);

	msg_entity_match_single_rank sMsgInfo;

	// 更新本地缓存记录
	MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.find(eMatchType);
	if (iter == m_mapMatchTypeRank.end())
	{
        // 以前没有打过该比赛类型的 有基础分数的
        // 先看是不是保底段位
        MatchTypeRankNode sNode;
        // 隐藏分数处理
        int nHideRank = nMatchTypeBaseScore;
        SMatchRankConfigSchemeInfo* pHideSchemeInfo = pSchemeInfo->getShemeInfoByTypeAndScore(eMatchType, nHideRank);
        if (pHideSchemeInfo != NULL)
        {
            int nHideNewRank = nHideRank + nHideRankScore;
            sNode.nHideRankScore = nHideNewRank;
        }

        // 保底分数来算初始段位
        SMatchRankConfigSchemeInfo* pBaseSchemeInfo = pSchemeInfo->getShemeInfoByTypeAndScore(eMatchType, nMatchTypeBaseScore);
        if (pBaseSchemeInfo == NULL)
        {
            ErrorLn(__FUNCTION__": pBaseSchemeInfo == NULL eMatchType="<<eMatchType<<" nMatchTypeBaseScore ="<< nMatchTypeBaseScore);
            return false;
        }

        // 新分数
        int nNewRank = nMatchTypeBaseScore + nRankScore;
        if(nNewRank < 0)
        {
            ErrorLn(__FUNCTION__": nNewRank < 0 nMatchTypeBaseScore ="<<nMatchTypeBaseScore<<" nRankScore ="<<nRankScore<<" nNewRank ="<<nNewRank);
            return false;
        }
        if (pBaseSchemeInfo->bIsBaseGrade)
        {
            // 新分数小于保底段位分 则设置为保底段位分
            if (nNewRank <= pBaseSchemeInfo->nGradeScore)
            {
                sNode.nMatchType    = eMatchType;
                sNode.nRankScore    = pBaseSchemeInfo->nGradeScore;
            }
            else
            {
                sNode.nMatchType    = eMatchType;
                sNode.nRankScore    = nNewRank;
            }

            // 第一次打降级升级都不播动画
            sMsgInfo.bRiseGrade         = false;     // 晋级 动画相关
            sMsgInfo.bFallGrade         = false;    // 降级 动画相关
            sNode.nGrade                = pBaseSchemeInfo->nGradeID;
            
            m_mapMatchTypeRank[eMatchType] = sNode;
            
        }
        else
        {
            // 第一次打计算段位等级 不是保底段位的处理 
            int nNewRank = nMatchTypeBaseScore + nRankScore;
            if (nNewRank < 0)
            {
                nNewRank = nMatchTypeBaseScore;
            }

            SMatchRankConfigSchemeInfo* pMatchTypeSchemeInfo = pSchemeInfo->getShemeInfoByTypeAndScore(eMatchType, nNewRank);
            if(pMatchTypeSchemeInfo != NULL)
            {
                sNode.nMatchType    = eMatchType;
                sNode.nRankScore    = nNewRank;

                if (nRankScore > 0)
                {
                    // 设置客户端消息
                    sMsgInfo.bRiseGrade         = false;     // 晋级 动画相关
                    sMsgInfo.bFallGrade         = false;    // 降级 动画相关
                }
                else
                {
                    // 设置客户端消息
                    sMsgInfo.bRiseGrade         = false;     // 晋级 动画相关
                    sMsgInfo.bFallGrade         = false;    // 降级 动画相关
                }

                if (sNode.nRankScore <= 0)
                {
                    sNode.nRankScore = 0;
                }

                sNode.nGrade                = pMatchTypeSchemeInfo->nGradeID;
                m_mapMatchTypeRank[eMatchType] = sNode;
            }
        }

        IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
        if (NULL != pOSSLogService && eMatchType == MatchType_Rank)
        {
            char szText[512] = {0};
            sprintf_s(szText, sizeof(szText), a2utf8("%s(%d)得分:0(%d) 当前分(%d) 隐藏分:0(%d) 当前隐藏分(%d)"), 
                                                        &m_playInfo.player.szActorName, m_playInfo.player.dwActorID, nRankScore, sNode.nRankScore, nHideRankScore, sNode.nHideRankScore );
            pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_RANK, m_playInfo.player.dwActorID, szText);
            TraceLn(szText);
        }
		
	}
	else
	{
        // 如果都为0 先处理下把隐藏分数和排位分都设置为基础分数 基础段位
        if (iter->second.nHideRankScore <= 0)
        {
            const SMatchRankConfigSchemeInfo* pHideSchemeInfo = pSchemeInfo->getShemeHideInfoByTypeAndScore(eMatchType, nMatchTypeBaseScore);
            if (pHideSchemeInfo == NULL)
            {
                ErrorLn(__FUNCTION__": pSchemeInfo == NULL iter->second.nHideRankScore <= 0");
            }
            else
            {
                iter->second.nHideRankScore = nMatchTypeBaseScore;
            }
            
        }

        if (iter->second.nRankScore <= 0)
        {
            const SMatchRankConfigSchemeInfo* pBaseSchemeInfo = pSchemeInfo->getShemeInfoByTypeAndScore(eMatchType, nMatchTypeBaseScore);
            if (pBaseSchemeInfo == NULL)
            {
                ErrorLn(__FUNCTION__": pSchemeInfo == NULL iter->second.nRankScore == 0");
            }
            else
            {
                iter->second.nRankScore = nMatchTypeBaseScore;
                iter->second.nGrade = pBaseSchemeInfo->nGradeID;
            }
            
        }

        // 隐藏分算法
        int nOldHideRank = iter->second.nHideRankScore;
        if (nHideRankScore != 0)
        {
            const SMatchRankConfigSchemeInfo* pHideSchemeInfo = pSchemeInfo->getShemeHideInfoByTypeAndScore(eMatchType, nOldHideRank);
            if (pHideSchemeInfo != NULL)
            {
                
                int nNewHideRank = nOldHideRank + nHideRankScore;
                iter->second.nHideRankScore = nNewHideRank;
            }
        }

        // 先判定当前段位是否是保底段位
        const SMatchRankConfigSchemeInfo* pBaseSchemeInfo = pSchemeInfo->getMatchRankConfigShemeInfo(eMatchType, iter->second.nGrade);
        if (pBaseSchemeInfo == NULL)
        {
            ErrorLn(__FUNCTION__": pBaseSchemeInfo == NULL eMatchType ="<< eMatchType<<" nGrade ="<<iter->second.nGrade);
            return false;
        }

		// 计算新得分
        int nOldRankScore = iter->second.nRankScore;
        int nNewRank = nOldRankScore + nRankScore;

        if (nNewRank < 0)
        {
            ErrorLn(__FUNCTION__": name"<<m_playInfo.player.szActorName<<" nNewRank < 0 nOldRankScore ="<<nOldRankScore<<" nRankScore ="<<nRankScore<<" nNewRank ="<<nNewRank);
            return false;
        }
        sMsgInfo.nPreScore = nOldRankScore;
        sMsgInfo.nPreGrade = iter->second.nGrade;

        // 保底段位
        if (pBaseSchemeInfo->bIsBaseGrade)
        {
            if (nNewRank <= pBaseSchemeInfo->nGradeScore)
            {
                // 低于保底段位置为保底段位分
                iter->second.nRankScore    = pBaseSchemeInfo->nGradeScore;
            }
            else
            {
                iter->second.nRankScore = nNewRank;
            }
        }
        else
        {
            iter->second.nRankScore = nNewRank;
        }

		// 不处理王者段位等级的 王者段位等级的由社会服处理更新
        SMatchRankConfigSchemeInfo* pMatchTypeSchemeInfo = pSchemeInfo->getShemeInfoByTypeAndScore(eMatchType, iter->second.nRankScore);
        if(pMatchTypeSchemeInfo != NULL)
        {
            int nOldGrade = iter->second.nGrade;
            int nNewGrade = pMatchTypeSchemeInfo->nGradeID;

            bool bKingRankUpdate = false;
            int nBaseKingRankScore = pSchemeInfo->getMatchTypeKingRankBaseScore(eMatchType);
            SMatchRankConfigSchemeInfo* pKingRankConfigScheme = pSchemeInfo->getShemeInfoByTypeAndScore(eMatchType, nBaseKingRankScore);
            if(pKingRankConfigScheme!= NULL)
            {
                // 前后都是王者段位的不处理通常的升降段位
                if(nNewGrade >= pKingRankConfigScheme->nGradeID && nOldGrade>= pKingRankConfigScheme->nGradeID)
                {
                    // 王者段位更新标示
                    bKingRankUpdate = true;
                }
            }

            // 更新前后都是王者段位则不走平常的分数计算 升段位 但是王者降级是要处理的
            if(!bKingRankUpdate)
            {
                if( nOldGrade != nNewGrade)
                {
                    if (nNewGrade > nOldGrade)
                    {
                        // 升级
                        sMsgInfo.bRiseGrade         = true;     // 晋级 动画相关
                        sMsgInfo.bFallGrade         = false;    // 降级 动画相关
                        // 升级要设置为段位最低分
                        iter->second.nRankScore     = pMatchTypeSchemeInfo->nGradeScore;
                       
                    }
                    else if (nNewGrade < nOldGrade)
                    {
                        //降级
                        sMsgInfo.bRiseGrade         = false;    // 晋级 动画相关
                        sMsgInfo.bFallGrade         = true;     // 降级 动画相关
                        
                    }
                }
                else
                {
                    // 设置客户端消息
                    sMsgInfo.bRiseGrade = false;     // 晋级 动画相关
                    sMsgInfo.bFallGrade = false;    // 降级 动画相关
                }

                // 设置段位等级
                iter->second.nGrade = pMatchTypeSchemeInfo->nGradeID;
            }
        }   


        IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
        if (NULL != pOSSLogService && eMatchType == MatchType_Rank)
        {
            char szText[512] = {0};
            sprintf_s(szText, sizeof(szText), a2utf8("%s(%d)得分：%d(%d) 当前分(%d) 隐藏分:%d(%d) 当前隐藏分(%d)"), 
                &m_playInfo.player.szActorName, m_playInfo.player.dwActorID, nOldRankScore,nRankScore, iter->second.nRankScore, nOldHideRank, nHideRankScore, iter->second.nHideRankScore );
            pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_RANK, m_playInfo.player.dwActorID, szText);
            TraceLn(szText);
        }
		     
	}

	// 数据
	MATCHTYPERANK_MAP::iterator iterFind = m_mapMatchTypeRank.find(eMatchType);
	if(iterFind != m_mapMatchTypeRank.end())
	{
        syncMatchTypeRankInfo(iterFind->second);
		event_entity_update_matchrank eventdata;
		eventdata.idActor = m_playInfo.player.dwActorID;
		eventdata.nMatchType = eMatchType;
		eventdata.nRankScore = iterFind->second.nRankScore;
		eventdata.dwMatchNum = iterFind->second.dwMatchNum;
		eventdata.dwWinNum = iterFind->second.dwWinNum;
        eventdata.nHideRankScore = iterFind->second.nHideRankScore;         // 隐藏段位分
		rkt:: IEventEngine* pEntityEngine = gServerGlobal->getEventEngine();
		if(pEntityEngine != NULL)
		{
			pEntityEngine->FireExecute(EVENT_GAME_ACTOR_UPDATE_MATCHRANK, SOURCE_TYPE_PERSON, eventdata.idActor, (LPCSTR)&eventdata, sizeof(eventdata));
		}

		// 同步到社会服
		obuf oData;
		SMsg_UpdatePersonMatchTypeRank SendData;
		SendData.idActor = m_pMaster->getIntProperty(PROPERTY_ID);
		SendData.eMatchType = eMatchType;
		SendData.RankNodeInfo = iterFind->second;
		memcpy(SendData.szActorName, m_pMaster->getName(), sizeof(SendData.szActorName));
		oData.push_back(&SendData, sizeof(SMsg_UpdatePersonMatchTypeRank));

		pConnector->sendToSocial(MSG_MODULEID_ORDER, SO_UPDATE_PERSON_MATCHTYPE_ORDERINFO, oData.data(), oData.size());

		// 下发到自己客户端 更新数据
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
		head.byKeyModule   = MSG_MODULEID_ENTITY;
		head.byKeyAction   = SC_MSG_ENTITY_MATCHTYP_SINGLE_RANK; // 人物撮合段位信息               

		// 部件
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
		entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

		// 设置下发数据
		sMsgInfo.RankNodeInfo = iterFind->second;

		obuf obmsg;
		obmsg << head << entityHead;
		obmsg.push_back(&(sMsgInfo), sizeof(msg_entity_match_single_rank));
		pMaster->send2Client(obmsg.data(),obmsg.size());

		// 设置数据库Rank数据
		DBREQ_SET_MATCHTYPE_RANK sData;
		sData.dwUserID      = getUserID();                              // 玩家账号ID
		sData.dwActorID     = m_pMaster->getIntProperty(PROPERTY_ID);   // 角色ID
		sData.nMatchType    = eMatchType;	                            // 撮合类型
		sData.nRankScore    = iterFind->second.nRankScore;              // 得分
		sData.nGrade        = iterFind->second.nGrade;                  // 段位
		sData.dwMatchNum    = iterFind->second.dwMatchNum;              // 段位
		sData.dwWinNum		= iterFind->second.dwWinNum;                // 段位
        sData.nHideRankScore = iterFind->second.nHideRankScore;         // 隐藏段位分
		obuf oBuff;
		oBuff.push_back(&sData, sizeof(DBREQ_SET_MATCHTYPE_RANK));

		if (pDBEngine->executeSP(GAMEDB_REQUEST_SET_MATCHTYPE_RANK, sData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_SET_MATCHTYPE_RANK failed");
			return false;
		}

        // 角色段位升级 (角色段位等级变化时记录)
		if (sMsgInfo.bRiseGrade || sMsgInfo.bFallGrade)
		{
			IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
			if (pOSSLogService != NULL)
			    pOSSLogService->addActorUpgradeLog(m_playInfo.player.dwWorldID, m_playInfo.dwUserID, m_playInfo.player.dwActorID, 2, sMsgInfo.RankNodeInfo.nGrade, time(NULL), eMatchType);
		}
	}

	setPlayerFightCapacity();

	return true;
}

bool CPlayerBankPart::setMatchHeroScore(int nHeroID, int nRankScore)
{
	if(m_pMaster == NULL)
	{
		return false;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}

	__IPlayerRole* pMaster = CAST_TYPE(__IPlayerRole*, m_pMaster);

	// todo 检查是否能使用的英雄

	// 添加计算英雄熟练度等级熟练度并下发
	MATCHTHEROSCORE_MAP::iterator iter = m_mapMatchHeroScore.find(nHeroID);
	if (iter == m_mapMatchHeroScore.end())
	{
		MatchHeroScoreNode pNode;
		pNode.nHeroID = nHeroID;	        // 英雄ID
		pNode.nHeroScore = nRankScore;      // 英雄熟练度得分
		// 计算英雄等级
		pNode.nHeroGrade = 1;               // 英雄熟练度段位
		m_mapMatchHeroScore[nHeroID] = pNode;
	}
	else
	{
		iter->second.nHeroScore += nRankScore;
		// todo 计算增加的等级
		iter->second.nHeroGrade = 1;
	}

	// 数据
	MATCHTHEROSCORE_MAP::iterator iterFind = m_mapMatchHeroScore.find(nHeroID);
	if(iterFind != m_mapMatchHeroScore.end())
	{
		// 下发到客户端 更新数据

		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
		head.byKeyModule   = MSG_MODULEID_ENTITY;
		head.byKeyAction   = SC_MSG_ENTITY_MATCH_SINGLE_HEROSCORE; // 人物撮合段位信息               

		// 部件
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
		entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

		obuf obmsg;
		obmsg << head << entityHead;
		obmsg.push_back(&(iterFind->second), sizeof(MatchHeroScoreNode));
		pMaster->send2Client(obmsg.data(),obmsg.size());

		// 设置数据库英雄熟练度数据
		DBREQ_SET_MATCH_HEROSCORE sDBData;

		sDBData.dwUserID   = getUserID();     // 玩家账号ID
		sDBData.dwActorID  = m_pMaster->getIntProperty(PROPERTY_ID);     // 角色ID
		sDBData.nHeroID    = nHeroID;	        // 英雄ID
		sDBData.nHeroScore = nRankScore;      // 英雄熟练度得分 增加减少多少得分
		sDBData.nHeroGrade = 1;               // 英雄熟练度段位

		obuf oBuff;
		oBuff.push_back(&sDBData, sizeof(DBREQ_SET_MATCH_HEROSCORE));

		if (pDBEngine->executeSP(GAMEDB_REQUEST_SET_MATCH_HEROSCORE, sDBData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_SET_MATCH_HEROSCORE failed");
			return false;
		}
	}

	return true;
}

////////////////////////////////////以下是天赋页操作//////////////////////////////////////









void CPlayerBankPart::OnTimer( unsigned long dwTimerID )
{
	// 发送信息到客户端
	switch( dwTimerID )
	{
	case ETIMERID_READ_TALENT_FROM_DATABASE:
		{
#ifdef FUNCTION_TALENTPAGE_OPEN
			m_talentPageMgr.RequestDataBaseAllTalentPageInfo();
#endif
		}
		break;

	case ETIMERID_READ_RUNE_FROM_DATABASE:
		{
#ifdef FUNCTION_RUNE_OPEN
			m_runeManager.requestDataBaseRuneInlayInfo();			
#endif
		}
		break;
	default:
		break;
	}
}

////////////////////////////////// 符文系统 Rune //////////////////////////////////





///////////////////////////////////////自己拥有的符文///////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CPlayerBankPart::getSummonerSkillInfo(int nSolt)
{
	int nSkill = 0;
	memcpy( &nSkill, (m_playInfo.player.bySummonerSkill + nSolt*4), sizeof( nSkill));
	return nSkill;
}

void CPlayerBankPart::setActorBaseInfo(LONG lgValue, int nType, int nMode, BYTE byNumType, BYTE byOpType, WORD wApproach, DWORD dwTargetID, LPCSTR szLogMark, int nLogReserve)
{
	if(m_pMaster == NULL || nType < 0 || nType >= EPIT_MAX)
		return;

	__IEntityProperty * pProperty = (__IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
	if (!pProperty)
	{
		return;
	}

	SPlayerInfo* sPlayerInfo = &m_playInfo.player;
	int nPDBID = pProperty->getProperty_Integer(PROPERTY_ID);
	int nInfoChangeType = nType;
	switch(nType)
	{
	case ENPLAYERINFOTYPE_WORLDID:
		{
			setMustSetTypeInfo(&sPlayerInfo->dwWorldID, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_HEROID:
		{
			setMustSetTypeInfo(&sPlayerInfo->nHeroID, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_SKIN:
		{
			setMustSetTypeInfo(&sPlayerInfo->nSkinID, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_LEVEL:
		{
			if ( ENCHANGEDATAMODE_SET == nMode)
				setActorLevel(lgValue);
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				addActorLevel(lgValue);
		}
		break;

	case ENPLAYERINFOTYPE_SEX:
		{
			if ( ENCHANGEDATAMODE_SET == nMode)
			{
				bool bNewSex = (lgValue == 1 ? false : true);
				sPlayerInfo->bSex = bNewSex;
			}
			else if ( ENCHANGEDATAMODE_ADD == nMode )
			{
				WarningLn(__FUNCTION__": skin id muset set can't add!!!");
			}
		}
		break;

	case ENPLAYERINFOTYPE_MAPID:
		{
			// 地图id未发生改变，则直接返回
			if (lgValue == sPlayerInfo->dwMapId)
				return;

			setMustSetTypeInfo(&sPlayerInfo->dwMapId, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_EXP:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
			{
				sPlayerInfo->dwExp = lgValue;
			}else if ( ENCHANGEDATAMODE_ADD == nMode )
			{
				addPlayerExp(lgValue);
			}
		}
		break;

	//case ENPLAYERINFOTYPE_GOLD:
	//	{
	//		//TODO 加金币类的需要加日志
	//		setMoneyTypeInfo(&sPlayerInfo->dwBindTicket, lgValue, nMode);

	//		pProperty->setProperty_Integer(PROPERTY_GOLD, sPlayerInfo->dwBindTicket);
	//	}
	//	break;

	//case ENPLAYERINFOTYPE_TICKET:
	//	{
	//		//TODO 加金币类的需要加日志
	//		setMoneyTypeInfo(&sPlayerInfo->dwTicket, lgValue, nMode);
	//		pProperty->setProperty_Integer(PROPERTY_TICKET, sPlayerInfo->dwTicket);
	//	}
	//	break;

	case ENPLAYERINFOTYPE_POOLLIMIT:
		{
			setMustSetTypeInfo(&sPlayerInfo->dwHeroPoolLimit, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_HEADID:
		{
			setMustSetTypeInfo(&sPlayerInfo->wHeadID, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_CLANID:
		{
			setMustSetTypeInfo(&sPlayerInfo->dwClanID, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_KINID:
		{
			setMustSetTypeInfo(&sPlayerInfo->dwKinID, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_TEAMID:
		{
			setMustSetTypeInfo(&sPlayerInfo->dwTeamID, lgValue, nMode);
		}
		break;

	case ENPLAYERINFOTYPE_PKWINNUM:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
				sPlayerInfo->dwPKWinNum = lgValue;
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				sPlayerInfo->dwPKWinNum += lgValue;
		}
		break;
	case ENPLAYERINFOTYPE_PKTOTALNUM:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
				sPlayerInfo->dwPKTotalNum = lgValue;
			else if ( ENCHANGEDATAMODE_ADD == nMode )
				sPlayerInfo->dwPKTotalNum += lgValue;
		}
		break;
	case ENPLAYERINFOTYPE_GEMSTONEEXP:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
			{

			}else if ( ENCHANGEDATAMODE_ADD == nMode )
			{
				m_crownPageMgr.addCrownInlayGemstoneExp(lgValue);
			}
		}
		break;
	case ENPLAYERINFOTYPE_SUPPORTPOINT:
		{
			if ( ENCHANGEDATAMODE_SET == nMode )
			{
				m_playInfo.player.dwSupportNum = lgValue;
			}else if ( ENCHANGEDATAMODE_ADD == nMode )
			{
				m_playInfo.player.dwSupportNum += lgValue;
			}
		}
		break;

	default:
		{
			nInfoChangeType = 0;
		}
		break;
	}

	onUpdateChangedPlayerInfo(nInfoChangeType);
	// 如果是经验加成，也会影响等级
	if (nInfoChangeType == ENPLAYERINFOTYPE_EXP)
	{
		onUpdateChangedPlayerInfo(ENPLAYERINFOTYPE_LEVEL);
	}
	// 写入日记
	if ( wApproach >= OSSRES_ADD_MIN && wApproach <= OSSRES_TRANOUT_MAX && byNumType > 0 && byNumType < OSS_NUMERIC_MAX && byOpType < OSS_RESOURCE_OPT_MAX )
	{
		IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
		if (pOSSLogServer)
		{
			pOSSLogServer->addNumericLog(getFromGameWorldID(), OSS_OPT_TYPE_PERSON, nPDBID, dwTargetID, lgValue, byNumType, byOpType, wApproach, 0, szLogMark);
		}
	}
}

void CPlayerBankPart::setActorSummonerOrGlory(int nData[], int nLen,int nType, long long llWarDBKey)
{
	SPlayerInfo* pPlayerInfo = &m_playInfo.player;
	if( pPlayerInfo == NULL)
		return;

	switch( nType )
	{
	case ENPLAYERINFOTYPE_SUMMONER:
		{
			int nSummonerLen = sizeof(pPlayerInfo->bySummonerSkill);
			if (nLen > nSummonerLen)
			{
				ErrorLn(__FUNCTION__": nLen > nSummonerLen  nLen = " << nLen << ": nSummonerLen = " << nSummonerLen );
				return;
			}
			memcpy(pPlayerInfo->bySummonerSkill, nData, nSummonerLen);

			onUpdateChangedPlayerInfo(nType);
		}
		break;

	case ENPLAYERINFOTYPE_GLORY:
		{
			int nGloryLen = sizeof(pPlayerInfo->byGlory);
			if (nLen > nGloryLen)
			{
				ErrorLn(__FUNCTION__": nLen > nGloryLen  nLen = " << nLen << ": nGloryLen = " << nGloryLen );
				return;
			}

			for ( int i = 0; i < ENACHIEVE_MAX; ++i )
			{
				int nValue  = 0;
				long long llWarID = 0;
				if ( i <= ENACHIEVE_ADDVALUE)
				{
					memcpy( &nValue, (pPlayerInfo->byGlory+ i*12), sizeof(nValue));
					nValue += nData[i];
					memcpy((pPlayerInfo->byGlory + i*12), &nValue, sizeof(int));

					memcpy(&llWarID, pPlayerInfo->byGlory + i*12 + 4, sizeof(long long));
					llWarID = nData[i] > 0 ? llWarDBKey : llWarID;
					memcpy((pPlayerInfo->byGlory + i*12 + 4), &llWarID, sizeof(long long));
				}
				else
				{
					memcpy(&nValue, (pPlayerInfo->byGlory + i*12), sizeof(nValue));
					if ( nValue < nData[i] )
					{
						memcpy((pPlayerInfo->byGlory + i*12), nData + i, sizeof(int));
						memcpy((pPlayerInfo->byGlory + i*12 + 4), &llWarDBKey, sizeof(long long));
					}
				}
			}
			onUpdateChangedPlayerInfo(nType);
		}
		break;
	default:
		break;
	}
}

DWORD CPlayerBankPart::getFirstWinTime()
{
	DWORD firstWinTime = getTaskDataInt(TASKKEY_ACTOR_WAR_FIRSTWINTIME);
	firstWinTime = (firstWinTime == TASKDATA_DEFAULT_VALUE ? 0 : firstWinTime);

	if (time(NULL) - firstWinTime >= FIRSTWINTIME_INTERVAL)
	{
		return 0;
	}
	return firstWinTime;
}

void CPlayerBankPart::setPlayerFirstWinFlag( DWORD dwTime)
{
	if (getFirstWinTime() != 0)
	{
		return;
	}
	addTaskDataInt(TASKKEY_ACTOR_WAR_FIRSTWINTIME, dwTime);
}

int CPlayerBankPart::getPlayerFightCapacity()
{
	return m_playInfo.player.dwFightCapacity;
}

// 设置任务数据
bool CPlayerBankPart::addTaskDataInt(int nKey, int nData)
{
	if (m_pTaskData == NULL)
		return false;

	return m_pTaskData->add(nKey, nData);
}

// 删除任务数据
bool CPlayerBankPart::removeTaskDataInt(int nKey)
{
	if (m_pTaskData == NULL)
		return false;

	return m_pTaskData->remove(nKey);
}

// 获取任务数据
int CPlayerBankPart::getTaskDataInt(int nKey)
{
	if (m_pTaskData == NULL)
		return TASKDATA_DEFAULT_VALUE;

	return m_pTaskData->get(nKey);
}

// 注册任务数据
bool CPlayerBankPart::declareTaskData(int nKey, bool bIsInt, bool bSave)
{
	if (m_pTaskData == NULL)
		return false;

	return m_pTaskData->registerKey(nKey, bIsInt, bSave);
}

// 设置玩家活动度
bool CPlayerBankPart::setDayActivity(int value, bool isSync/* = true*/)
{
    // 更新数值
    if(! addTaskDataInt(TASKKEY_ACTOR_ACTIVITY_VALUE, value) )
        return false;
    // 更新日期
    addTaskDataInt(TASKKEY_ACTOR_ACTIVITY_DAY, (int)time(NULL));

    // 发玩家活跃度改变事件
    event_actor_set_activity context;
    context.uActorUID = m_pMaster->getUID();
    context.nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    context.nDayValue = value;
    context.nFlag = ACITIVITY_EVENT_FLAG_DAY;
    m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_ACTOR_SET_ACTIVITY,(LPCSTR)&context,sizeof(context));	

    if (isSync) {
        syncActivity();
    }

    return true;
}

// 获取玩家活跃度
int CPlayerBankPart::getDayActivity(void)
{
    int nOldTime = getTaskDataInt(TASKKEY_ACTOR_ACTIVITY_DAY);
    if(nOldTime == TASKDATA_DEFAULT_VALUE)
    {
        return 0;
    }

    tm local_OldTime;
    time_t long_OldTime = (time_t)nOldTime; 
    localtime_s(&local_OldTime,&long_OldTime);

    tm local_NowTime;
    int nCurrentTime = (int)time(NULL);
    time_t long_NowTime = (time_t)nCurrentTime; 
    localtime_s(&local_NowTime,&long_NowTime);

    // 当天就返回任务数据表中的数值，否则清0
    if (local_NowTime.tm_yday == local_OldTime.tm_yday && local_NowTime.tm_year == local_OldTime.tm_year)
    {
        return getTaskDataInt(TASKKEY_ACTOR_ACTIVITY_VALUE);
    }
    else
    {
        setDayActivity(0, false);
        return 0;
    }
}


// 设置玩家周活跃度
bool CPlayerBankPart::setWeekActivity(int value, bool isSync /*= true*/)
{
    // 更新数值
    if (!addTaskDataInt(TASKKEY_ACTOR_ACTIVITY_WEEK_VALUE, value))
        return false;
    // 更新日期
    addTaskDataInt(TASKKEY_ACTOR_ACTIVITY_WEEK, (int)time(NULL));

    // 发玩家活跃度改变事件
    event_actor_set_activity context;
    context.uActorUID = m_pMaster->getUID();
    context.nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    context.nWeekValue = value;
    context.nFlag = ACITIVITY_EVENT_FLAG_WEEK;
    m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_ACTOR_SET_ACTIVITY, (LPCSTR)&context, sizeof(context));

    if (isSync) {
        syncActivity();
    }

    return true;
}

// 获取玩家周活跃度
int CPlayerBankPart::getWeekActivity(void)
{
    int nOldTime = getTaskDataInt(TASKKEY_ACTOR_ACTIVITY_WEEK);
    if (nOldTime == TASKDATA_DEFAULT_VALUE)
    {
        return 0;
    }

    tm local_OldTime;
    time_t long_OldTime = (time_t)nOldTime;
    localtime_s(&local_OldTime, &long_OldTime);

    tm local_NowTime;
    int nCurrentTime = (int)time(NULL);
    time_t long_NowTime = (time_t)nCurrentTime;
    localtime_s(&local_NowTime, &long_NowTime);

    // 是否为相同周
    bool isSameWeek = false;
    // 相同的年月

    if (nCurrentTime >= nOldTime) {
        isSameWeek = (diffWeek(nOldTime, nCurrentTime)  == 0);
    }

    if (isSameWeek) {
        return getTaskDataInt(TASKKEY_ACTOR_ACTIVITY_WEEK_VALUE);
    }
    else {
        setWeekActivity(0, false);
        return 0;
    }
}

// 触发玩家活跃度数值同步
void CPlayerBankPart::syncActivity()
{
    if (!m_pMaster)
        return;

    int nDayValue = getDayActivity();
    int nWeekValue = getWeekActivity();

    // 发玩家活跃度改变事件
    event_actor_set_activity context;
    context.uActorUID = m_pMaster->getUID();
    context.nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    context.nDayValue = nDayValue;
    context.nWeekValue = nWeekValue;
    context.nFlag = ACITIVITY_EVENT_FLAG_DAYWEEK;

    // 发全局事件
    rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine)
    {
        pEventEngine->FireExecute(EVENT_GAME_ACTOR_SET_ACTIVITY, SOURCE_TYPE_PERSON, context.nPDBID, (LPCSTR)&context, sizeof(context));
    }
}

// 判断两个时间点之间相差几周
int CPlayerBankPart::diffWeek(DWORD dwTime1, DWORD dwTime2)
{
    // 一天的秒数
    int nDaySec = 3600 * 24;

    // 格式化
    time_t tTime1 = (DWORD)dwTime1;
    time_t tTime2 = (DWORD)dwTime2;

    tm tmTime1;
    memcpy(&tmTime1, localtime(&tTime1), sizeof(tmTime1));

    tm tmTime2;
    memcpy(&tmTime2, localtime(&tTime2), sizeof(tmTime2));

    // 星期几的处理(按照中国大陆的习惯，星期一算一个星期的第一天)
    int nWeekday1 = tmTime1.tm_wday;
    int nWeekday2 = tmTime2.tm_wday;
    int nDiffMonday1 = 0;		// nWeekday1与星期一相差几天
    int nDiffMonday2 = 0;		// nWeekday2与星期一相差几天
    
    if (nWeekday1 == 0)
    {
        nDiffMonday1 = 6;
    }
    else
    {
        nDiffMonday1 = nWeekday1 - 1;
    }

    if (nWeekday2 == 0)
    {
        nDiffMonday2 = 6;
    }
    else
    {
        nDiffMonday2 = nWeekday2 - 1;
    }

    // 得到dwTime1和dwTime2去掉零头后的秒数(相对于星期一零点，多出来的都是零头)
    DWORD dwTimeFinal1 = dwTime1 - (nDiffMonday1 * nDaySec) - (tmTime1.tm_hour * 3600 + tmTime1.tm_min * 60 + tmTime1.tm_sec);
    DWORD dwTimeFinal2 = dwTime2 - (nDiffMonday2 * nDaySec) - (tmTime2.tm_hour * 3600 + tmTime2.tm_min * 60 + tmTime2.tm_sec);

    return ((int)(dwTimeFinal1 - dwTimeFinal2)) / (7 * nDaySec);
}

bool CPlayerBankPart::updatePlayerInfoToDB()
{
	if(m_playInfo.dwUserID == 0)
	{
		ErrorLn(__FUNCTION__ << ", userid = 0");
		return false;
	}

	if(m_pMaster == NULL)
		return false;

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return false;

	obuf oBuffer;

	DBREQ_PARAM_SAVEACTOR actorInfo;
	fillPlayerInfo(actorInfo, m_playInfo.player);
	//actorInfo.dwBindTicket = m_pMaster->getIntProperty(PROPERTY_GOLD);
	//actorInfo.player.nLocX *= ACTOR_LOCATION_SCALE;
	//actorInfo.player.nLocY *= ACTOR_LOCATION_SCALE;
	//actorInfo.player.nLocZ *= ACTOR_LOCATION_SCALE;
	oBuffer.push_back(&actorInfo, sizeof(DBREQ_PARAM_SAVEACTOR));

#ifdef FUNCTION_RUNE_OPEN
	obuf obData; // 压入符文不定长
    int nRuneStoreItemLen =  sizeof(EntityHeroRuneStore);
    
    int nRuneCount = m_runeManager.getStore(obData);
	int nRuneCountLen = nRuneCount * nRuneStoreItemLen;
    oBuffer.push_back(&nRuneCountLen, sizeof(int));
    oBuffer.push_back(obData.data(), obData.size());
#endif

	if (! pDBEngine->executeSP(GAMEDB_REQUEST_SAVEACTOR, actorInfo.dwActorID, oBuffer.data(), oBuffer.size(), static_cast<IDBRetSink *>(this)) )
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_SAVEACTOR failed");
		return false;
	}

    // 输出下角色下线信息
    ENTITY_WARNINGLN("Logout Save: name=["<< actorInfo.dwActorID <<"]"<< actorInfo.szActorName <<", mapid="<< actorInfo.dwMapId <<", x="<<actorInfo.nLocX<<",y="<<actorInfo.nLocY<<",z="<<actorInfo.nLocZ<<",angle="<< actorInfo.nAngle);

	return true;
}

void CPlayerBankPart::setRuneTalentPageIndex()
{
	// 存入天赋数据
	int nCurRunePageIndex = 0;
	int nCurTalentPageIndex = 0;
#ifdef FUNCTION_RUNE_OPEN
	nCurRunePageIndex = m_runeManager.getCurRunePage();
#endif
#ifdef FUNCTION_TALENTPAGE_OPEN
	nCurTalentPageIndex = m_talentPageMgr.GetCurTalentPage();
#endif

	PDBID nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	MatchSceneHelper helper;
	helper.m_ptr->setRuneTalentPageIndex(nPDBID, nCurRunePageIndex, nCurTalentPageIndex);
}

bool CPlayerBankPart::onReqUpdageHeroStar(int nHeroID)
{
	ISchemeHeroStarData*	pStarData		= gServerGlobal->getSchemeCenter()->getSchemeHeroStarData();
	SHeroInfo*				pHeroInfo		= getHeroInfo(nHeroID);
	IOSSLogService *		pOSSLogServer	= gServerGlobal->getOSSLogService();
	if( pHeroInfo == NULL || pStarData == NULL || pOSSLogServer == NULL)
		return false;

	// 判是否拥有该英雄
	if (!m_heroMgr.isHeroHave(pHeroInfo->dwFlags))
		return false;

	// 取升星所需要的消耗
	SHeroStarSchemeInfo starInfo = pStarData->getHeroStarData(nHeroID, pHeroInfo->nHeroStar);
	DWORD	dwNeedCardID	= starInfo.dwNeedCardID;
	int		nExpendCount	= starInfo.nNeedCount;
	int		nExpendGold		= starInfo.nExpendGold;
	int		nCardDiscount	= starInfo.nCardDiscount;
	int		nGoldDiscount	= starInfo.nGoldDiscount;

	// 判是否本命英雄
	int	nRealCard = nExpendCount;
	int	nRealGold = nExpendGold;
	char szCardCount[32] = {0};
	char szGoldCount[32] = {0};
	if (m_heroMgr.checkIsLifeHero(nHeroID))
	{
		if (nCardDiscount > 0)
		{
			nRealCard = nCardDiscount;
			ssprintf_s(szCardCount, sizeof(szCardCount), "(本命折扣)");
		}
		if (nGoldDiscount > 0)
		{
			nRealGold = nGoldDiscount;
			ssprintf_s(szGoldCount, sizeof(szGoldCount), "(本命折扣)");
		}
	}
	if (nRealCard <= 0 || nRealGold <= 0)
		return false;

	// 判金钱和卡牌是否足够
	__IPlayerRole * pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if( pPlayerRole == NULL)
		return false;
	bool bGoldIsEnough = pPlayerRole->canAddMoney(0, -nRealGold);
	bool bIsCanUp = m_heroMgr.isCanUpStar( nHeroID, dwNeedCardID, nRealCard);
	if ( !bIsCanUp || !bGoldIsEnough)
		return false;

	// 扣钱
	char szLogInfo[256] = {0};
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("英雄id:%d, 星级:%d, 金币消耗:%d%s"), nHeroID, starInfo.nStarLV, nRealGold, szGoldCount);
	pPlayerRole->addMoney(0, -nRealGold, OSSRES_DEL_HERO_UPGRADE_LEVEL, szLogInfo, 0, 0);

	// 扣卡牌
	memset( szLogInfo, 0, sizeof(szLogInfo));
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("英雄id:%d, 星级:%d, 卡牌消耗:%d%s"), nHeroID, starInfo.nStarLV, nRealCard, szCardCount);
	PDBID pDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	pOSSLogServer->addActorGoodsLog(getFromGameWorldID(), OSS_OPT_TYPE_PERSON, pDBID, OSS_OPT_TYPE_SYSTEM, 0, OSS_GOODS_HEROCARD, OSS_RESOURCE_OPT_DEL, OSSRES_DEL_HERO_UPGRADE_LEVEL, dwNeedCardID, nRealCard, time(NULL), szLogInfo);
	m_heroMgr.expendHeroCardInfo(dwNeedCardID, -nRealCard);

	// 升星并将升星成功通知客户端
	setHeroOneInfo( nHeroID, 1, ENHEROINFOTYPE_STAR, ENCHANGEDATAMODE_ADD, OSS_NUMERIC_MAX, OSS_RESOURCE_OPT_MAX, 0, 0, 0, 0);
	pHeroInfo = getHeroInfo(nHeroID);
	if( pHeroInfo == NULL)
	{
		ErrorLn(__FUNCTION__": can't find nHeroId = " << nHeroID << ", this hero Info");
		return false;
	}

	// 通知任务
	__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent != NULL)
	{
		event_game_actor_data_task eventdata;
		eventdata.nTaskLuaEvent = emTaskLuaEventType_HeroUpGrade;
		eventdata.dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
		eventdata.nParam1 = nHeroID;
		eventdata.nParam2 = pHeroInfo->nHeroStar;

		pEntityEvent->FireExecute(EVENT_GAME_ACTOR_DATA_TASK, (LPCSTR)&eventdata, sizeof(eventdata));
	}

	// 将升星成功后的数据发送到客户端
	obuf obData;
	msg_up_star_sucess msgInfo;
	msgInfo.nHeroID = nHeroID;
	msgInfo.bIsSucess = true;
	msgInfo.nCurrentStar = pHeroInfo->nHeroStar;
	msgInfo.nCurrentGold = m_pMaster->getIntProperty(PROPERTY_GOLD);
	msgInfo.nCurrentCardCount = m_heroMgr.getOneCardCount(dwNeedCardID);
	obData.push_back( &msgInfo, sizeof(msgInfo));

    if (pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_UPGRADE_STAR_SUCESS, obData.data(), obData.size());
    }

	return true;
}

// 切换英雄和皮肤
bool CPlayerBankPart::onMessageChangeHero(void * data, size_t len)
{
    if(m_pMaster == NULL)
    {
		ErrorLn(__FUNCTION__": m_pMaster == NULL ");
        return false;
    }
	if ( len < sizeof(msg_entity_change_hero))
	{
		ErrorLn(__FUNCTION__": nLen < sizeof(int) len = " << len);
		return false;
	}
    // 判断是否能切英雄和皮肤
    int nMaskFlag = m_pMaster->getIntProperty(PROPERTY_MASK);
    // 已经设置了禁止切英雄和皮肤
    if( (nMaskFlag & MASKFLAG_FORBID_CHANGE) == MASKFLAG_FORBID_CHANGE)
    {
        return false;
    }

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if ( pSchemeCenter == NULL )
	{
		return false;
	}
	ISchemePersonModel* pPersonModel = pSchemeCenter->getSchemePersonModel();
	if ( pPersonModel == NULL )
	{
		return false;
	}
	ISchemeSkinInfo* pPersonSkin = pSchemeCenter->getSchemeSkinInfo();
	if ( pPersonSkin == NULL )
	{
		return false;
	}
    __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent == NULL)
    {
        return false;
    }

	msg_entity_change_hero* pMsg = (msg_entity_change_hero*)data;
	// 设置皮肤id 和英雄id
	__IEntityProperty *pPropertyPart = (__IEntityProperty*)(m_pMaster->getEntityPart(PART_PROPERTY));
	if(pPropertyPart == NULL)
	{
		return false;
	}

	if(pMsg->nOptType == CHANGE_HERO_TYPE_HERO)        // 切英雄
	{
        // 检测是否拥有这个英雄
        SHeroInfo* pHeroInfo = getHeroInfo(pMsg->nChangeID);
        if ( pHeroInfo == NULL )
        {
            ErrorLn(__FUNCTION__": can't find heroID = " << pMsg->nChangeID);
            return false;
        }

        // 切英雄
        __IPlayerRole* pPlayer = (__IPlayerRole*)m_pMaster;
        if(!pPlayer->changeVoc(pMsg->nChangeID, 0))
        {
            return false;
        }
		// 切换皮肤
        onChangeSkin(pMsg->nChangeID, pMsg->nReserveData);
	}
	else if( pMsg->nOptType == CHANGE_HERO_TYPE_SKIN)    // 切皮肤
	{
        if(!onChangeSkin(pMsg->nReserveData, pMsg->nChangeID))
            return false;
	}
	else
	{
		return false;
	}

	
	updatePlayerInfoToDB();
	return true;
}

// 切换英雄皮肤
bool  CPlayerBankPart::onChangeSkin(int nHeroID, int nSkinID)
{
    if(m_pMaster == NULL)
    {
        return false;
    }
    __IEntityProperty *pPropertyPart = (__IEntityProperty*)(m_pMaster->getEntityPart(PART_PROPERTY));
    if(pPropertyPart == NULL)
    {
        return false;
    }
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        return false;
    }
    ISchemePersonModel* pPersonModel = pSchemeCenter->getSchemePersonModel();
    if ( pPersonModel == NULL )
    {
        return false;
    }
    ISchemeSkinInfo* pPersonSkin = pSchemeCenter->getSchemeSkinInfo();
    if ( pPersonSkin == NULL )
    {
        return false;
    }

    // 校验皮肤的有效性
    const PersonSkinModelItem * pPersonSkinModelItem = gServerGlobal->getSchemeCenter()->getSchemePersonModel()->getPersonSkinModelItem(nHeroID, nSkinID); 
    // 不是当前英雄的皮肤时，将用默认皮肤
    if ( !pPersonSkinModelItem )
    {
        pPersonSkinModelItem = gServerGlobal->getSchemeCenter()->getSchemePersonModel()->getPersonSkinModelItem(nHeroID); 
        if (!pPersonSkinModelItem)
        {
        	return false;
        }
        nSkinID = pPersonSkinModelItem->nDefaultSkinID;
    }
    // 校验皮肤是否有配置
    SkinInfoScheme_S* pSkinItemInfo = pPersonSkin->getSkinShemeInfo(nSkinID);
    if(pSkinItemInfo == NULL)
    {
        ErrorLn(__FUNCTION__": can't find skinID in = SchemeSkin" << nSkinID);
        return false;
    }
	// 校验是否已拥有皮肤
	if (!m_heroMgr.checkIsPossessSkin(nHeroID, nSkinID))
	{
		return false;
	}

    //.设置人物当前皮肤数据
    m_playInfo.player.nSkinID = nSkinID;
    // 设置皮肤属性
    pPropertyPart->setProperty_Integer(PROPERTY_SKIN, nSkinID);
    return true;
}

void CPlayerBankPart::updateMapInfo(int nMatchType, bool bIsWin)
{

	MATCHTYPERANK_MAP:: iterator it = m_mapMatchTypeRank.find((EMMatchType)nMatchType );
	if ( it == m_mapMatchTypeRank.end() )
	{
		MatchTypeRankNode dbPlayerMap;
		dbPlayerMap.nMatchType = nMatchType;
		dbPlayerMap.dwMatchNum++;
		if ( bIsWin )
		{
			dbPlayerMap.dwWinNum++;
		}

        if (nMatchType == MatchType_Rank)
        {
            m_playInfo.player.dwRankMatchNum = dbPlayerMap.dwMatchNum;                                 // 比赛次数
            m_playInfo.player.wRankWinNum = dbPlayerMap.dwWinNum;                                    // 胜利场次 
        }

		m_mapMatchTypeRank[(EMMatchType)nMatchType] = dbPlayerMap;
	}else
	{
		MatchTypeRankNode& dbPlayerMap = it->second;
		dbPlayerMap.dwMatchNum++;
		if ( bIsWin )
		{
			dbPlayerMap.dwWinNum++;
		}
        if (nMatchType == MatchType_Rank)
        {
            m_playInfo.player.dwRankMatchNum = dbPlayerMap.dwMatchNum;                                 // 比赛次数
            m_playInfo.player.wRankWinNum = dbPlayerMap.dwWinNum;                                    // 胜利场次 
        }
	}

    if (nMatchType == MatchType_Rank)
    {
        onUpdateChangedPlayerInfo(ENPLAYERINFOTYPE_RANKNUM);
    }

	// 数据
    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL || m_pMaster == NULL)
    {
        return;
    }
    DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	MATCHTYPERANK_MAP::iterator iterFind = m_mapMatchTypeRank.find((EMMatchType)nMatchType);
	if (iterFind != m_mapMatchTypeRank.end())
	{
		//syncMatchTypeRankInfo(iterFind->second);
		event_entity_update_matchrank eventdata;
		eventdata.idActor = m_playInfo.player.dwActorID;
		eventdata.nMatchType = nMatchType;
		eventdata.nRankScore = iterFind->second.nRankScore;
		eventdata.dwMatchNum = iterFind->second.dwMatchNum;
		eventdata.dwWinNum = iterFind->second.dwWinNum;
        eventdata.nHideRankScore = iterFind->second.nHideRankScore;         // 隐藏段位分
		rkt::IEventEngine* pEntityEngine = gServerGlobal->getEventEngine();
		if (pEntityEngine != NULL)
		{
			pEntityEngine->FireExecute(EVENT_GAME_ACTOR_UPDATE_MATCHRANK, SOURCE_TYPE_PERSON, eventdata.idActor, (LPCSTR)&eventdata, sizeof(eventdata));
		}

		//// 同步到社会服
		//obuf oData;
		//SMsg_UpdatePersonMatchTypeRank SendData;
		//SendData.idActor = m_pMaster->getIntProperty(PROPERTY_ID);
		//SendData.eMatchType = eMatchType;
		//SendData.RankNodeInfo = iterFind->second;
		//memcpy(SendData.szActorName, m_pMaster->getName(), sizeof(SendData.szActorName));
		//oData.push_back(&SendData, sizeof(SMsg_UpdatePersonMatchTypeRank));

		//pConnector->sendToSocial(MSG_MODULEID_ORDER, SO_UPDATE_PERSON_MATCHTYPE_ORDERINFO, oData.data(), oData.size());

		// 下发到自己客户端 更新数据
		//SNetMsgHead head;
		//head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		//head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
		//head.byKeyModule = MSG_MODULEID_ENTITY;
		//head.byKeyAction = SC_MSG_ENTITY_MATCHTYP_SINGLE_RANK; // 人物撮合段位信息               

		//													   // 部件
		//SMsgEntityHead entityHead;
		//entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
		//entityHead.byPartID = PART_BANK;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

		//											// 设置下发数据
		//sMsgInfo.RankNodeInfo = iterFind->second;

		//obuf obmsg;
		//obmsg << head << entityHead;
		//obmsg.push_back(&(sMsgInfo), sizeof(msg_entity_match_single_rank));
		//pMaster->send2Client(obmsg.data(), obmsg.size());

		// 设置数据库Rank数据
		DBREQ_SET_MATCHTYPE_RANK sData;
		sData.dwUserID = getUserID();                              // 玩家账号ID
		sData.dwActorID = dwActorID;   // 角色ID
		sData.nMatchType = nMatchType;	                            // 撮合类型
		sData.nRankScore = iterFind->second.nRankScore;              // 得分
		sData.nGrade = iterFind->second.nGrade;                  // 段位
		sData.dwMatchNum = iterFind->second.dwMatchNum;              // 总场数
		sData.dwWinNum = iterFind->second.dwWinNum;                // 胜利数
        sData.nHideRankScore = iterFind->second.nHideRankScore;         // 隐藏段位分
		obuf oBuff;
		oBuff.push_back(&sData, sizeof(DBREQ_SET_MATCHTYPE_RANK));

		if (pDBEngine->executeSP(GAMEDB_REQUEST_SET_MATCHTYPE_RANK, sData.dwActorID, oBuff.data(), oBuff.size(), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_SET_MATCHTYPE_RANK failed");
			return;
		}

        obuf ob;
        DBREQ_PARAM_UPDATEACTORINFO sActorInfo;
        sActorInfo.dwPDBID = dwActorID;                    // 玩家角色DBID
        sActorInfo.wLevel = m_playInfo.player.nLevel;                     // 最高等级
        sActorInfo.nLastlogout = (int)time(NULL);                // 临时打就保存 当前时间 
        sActorInfo.nRecordsCount = 1;              // 比赛战绩记录数

        DBREQ_WARRECORD_INFO sInfo;
        sInfo.nMatchType = nMatchType;   //撮合类型
        sInfo.dwMatchNum = iterFind->second.dwMatchNum;
        sInfo.dwWinNum = iterFind->second.dwWinNum;
        int nSize = sizeof(DBREQ_WARRECORD_INFO);
        ob<<sActorInfo<<nSize<<sInfo;
        if (!pDBEngine->executeSP(GAMEDB_REQUEST_UPDATEACTORINFO, dwActorID, ob.data() , ob.size(), static_cast<IDBRetSink *>(this)) )
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_UPDATEACTORINFO failed !");
            return;
        }
	}
}


void CPlayerBankPart::setPlayerFightCapacity()
{
	ISchemeFightingCapacityConfig* pFightConfig = gServerGlobal->getSchemeCenter()->getSchemeFightingCapacityConfig();
	if ( pFightConfig == NULL )
	{
		ErrorLn(__FUNCTION__": pFightConfig == NULL");
		return ;
	}

	MATCHTYPERANK_MAP::iterator itBegin = m_mapMatchTypeRank.begin();
	DWORD nSumFight = 0;
	for (;itBegin != m_mapMatchTypeRank.end(); ++itBegin)
	{
		const SFightingCapacityConfig* pConfig = pFightConfig->getFightConfig(itBegin->first);
		if ( pConfig == NULL )
		{
			return ;
		}
		const MatchTypeRankNode& node = itBegin->second;

		nSumFight += (node.nRankScore * pConfig->fFightCoeff);
	}
	m_playInfo.player.dwFightCapacity = nSumFight;

	//设置玩家战力值
	if (m_pMaster && nSumFight > 0)
	{
		m_pMaster->setIntProperty(PROPERTY_FIGHTCAPACITY, nSumFight);
	}

}

int CPlayerBankPart::randHeroCardForWar(int nCount, DWORD *pCard, int nArrCount)
{
	if (nCount > nArrCount)
	{
		ErrorLn(__FUNCTION__": nCount is larger than the max number, nCount = " << nCount << ", max number = " << nArrCount);
		return 0;
	}

	vector<sRandHeroCard> heroPro;
	getHeroCardPro(heroPro);

	// 随机
	int nIndex = 0;
	for (int i = 0; i < nCount; i++)
	{
		int nRand = mt_range_rand(0, PROBABILITY_ENLARGE - 1);
		for (vector<sRandHeroCard>::iterator iter = heroPro.begin(); iter != heroPro.end(); iter++)
		{
			// 命中iter->heroCardPro
			if (nRand < iter->heroCardPro)
			{
				pCard[nIndex++] = iter->heroID;
				break;
			}
		}
	}
	return nCount;
}

bool sRandHeroCardSorter (const sRandHeroCard& lValue, const sRandHeroCard& rValue)
{
	if (lValue.heroType == rValue.heroType)
	{
		return lValue.matchNum > rValue.matchNum;
	}
	else	
	{
		return lValue.heroType > rValue.heroType;
	}
}
int CPlayerBankPart::randHeroCardForChest(int nPrizeID, DWORD *pCard, int nArrCount)
{
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return 0;

	ISchemeActorPrize *pActorPrizeScheme = pSchemeCenter->getSchemeActorPrize();
	ISchemeHeroCardCreat *pHeroCardConfig = pSchemeCenter->getSchemeHeroCardCreat();
	if (pActorPrizeScheme == NULL || pHeroCardConfig == NULL)
		return 0;

	SActorPrizeConfig_S *pPrizeConfig = pActorPrizeScheme->getActorPrizeConfig(nPrizeID);
	SHeroCardCreatPro* pProInfo = pHeroCardConfig->getHeroCardCreatPro(m_playInfo.player.nLevel);
	if ( pPrizeConfig == NULL || pProInfo == NULL )
		return 0;

	int nCount = pPrizeConfig->nPrizeParam[0];
	int nHeroType = pPrizeConfig->nPrizeParam[1];
	float nFloatRate = (float)pPrizeConfig->nPrizeParam[2] / 100;
	if (nCount == 0 || nHeroType == 0)
		return 0;

	if (nCount > nArrCount)
	{
		ErrorLn(__FUNCTION__": nCount is larger than the max number, nCount = " << nCount << ", max number = " << nArrCount);
		return 0;
	}

	// step1
	vector<sRandHeroCard> heroPro;
	getHeroCardPro(heroPro);
	vector<sRandHeroCard> newHeroPro;
	if (heroPro.size() <= nHeroType)
	{
		newHeroPro = heroPro;
	}
	else
	{
		for (int nIndex = 0; nIndex < nHeroType; nIndex++)
		{
			int nRand = mt_range_rand(0, PROBABILITY_ENLARGE - 1);
			for (int i = 0; i < heroPro.size(); i++)
			{
				// 命中iter->heroCardPro
				if (nRand < heroPro[i].heroCardPro)
				{
					// 加入
					newHeroPro.push_back(sRandHeroCard(heroPro[i]));

					// 更新概率表
					int nMinusPro = (i == 0 ? heroPro[i].heroCardPro : (heroPro[i].heroCardPro - heroPro[i-1].heroCardPro));
					int nOffset = nMinusPro / (heroPro.size() - newHeroPro.size());	//if (heroPro.size() <= nHeroType)保证为正值
					heroPro[i].heroCardPro = 0;										//i置为0

					int nPlusNum = 0;												//i之前加offset
					int nLastPlusIndex = 0;
					for (int j = 0; j < i; j++)
					{
						if (heroPro[j].heroCardPro <= 0)
							continue;

						++nPlusNum;
						heroPro[j].heroCardPro += nPlusNum * nOffset;
						nLastPlusIndex = j;
					}

					for (int j = i + 1; j < heroPro.size(); j++)					//i之后减Pro加offset
					{
						if (heroPro[j].heroCardPro <= 0)
							continue;

						++nPlusNum;
						heroPro[j].heroCardPro = heroPro[j].heroCardPro - nMinusPro + nPlusNum * nOffset;
						nLastPlusIndex = j;
					}

					heroPro[nLastPlusIndex].heroCardPro = PROBABILITY_ENLARGE;		//float情况最后一项取满值
					break;
				}
			}
		}
	}

	// step2: vector按使用、队友、私有、周免排序
	for(int i = 0; i < newHeroPro.size(); i++)
	{
		SHeroInfo* pHeroInfo = m_heroMgr.getHeroInfo(newHeroPro[i].heroID);
		if (pHeroInfo == NULL)
			continue;

		newHeroPro[i].matchNum = pHeroInfo->dwMatchNum;
	}
	sort(newHeroPro.begin(), newHeroPro.end(), sRandHeroCardSorter);

	// step3:
	if (newHeroPro.size() == 0)
		return 0;
	if (newHeroPro.size() == 1)
	{
		int nIndex = 0;
		for (int i = 0; i < nCount; i++)
		{
			pCard[nIndex++] = newHeroPro[0].heroID;
		}
		return nCount;
	}

	// step4:
	int nCountLowLimit = (1 - nFloatRate) * nCount * (PROBABILITY_ENLARGE - pProInfo->nMainHeroPro) / ((newHeroPro.size() - 1) * PROBABILITY_ENLARGE);
	int nCountHighLimit = (1 + nFloatRate) * nCount * (PROBABILITY_ENLARGE - pProInfo->nMainHeroPro) / ((newHeroPro.size() - 1) * PROBABILITY_ENLARGE);
	for (int i = 1; i < newHeroPro.size(); i++)
	{
		newHeroPro[i].heroCardNum = mt_range_rand(nCountLowLimit, nCountHighLimit);
	}

	// step5:
	int nTotalCount = nCount;
	int nLoop = 0;
	for (int i = 1; i < newHeroPro.size(); i++)			// 从第二项开始
	{
		if (nTotalCount <= newHeroPro[i].heroCardNum)
			break;

		nTotalCount -= newHeroPro[i].heroCardNum;
		nLoop++;
	}
	newHeroPro[0].heroCardNum = nTotalCount;

	int nIndex = 0;
	for (int i = 0; i <= nLoop; i++)
	{
		for (int j = 0; j < newHeroPro[i].heroCardNum; j++)
		{
			pCard[nIndex++] = newHeroPro[i].heroID;
		}
	}
	return nCount;
}

void CPlayerBankPart::getHeroCardPro(vector<sRandHeroCard>& cardPro)
{
	ISchemeHeroCardCreat *pHeroCardConfig = gServerGlobal->getSchemeCenter()->getSchemeHeroCardCreat();
	ISchemeHeroFreeWeekly *pSchemeHeroFreeWeekly = gServerGlobal->getSchemeCenter()->getSchemeHeroFreeWeekly();
	if ( pHeroCardConfig == NULL || pSchemeHeroFreeWeekly == NULL)
	{
		ErrorLn(__FUNCTION__": pHeroCardConfig  == NULL || pSchemeHeroFreeWeekly == NULL");
		return;
	}

	int nPlayerLv = m_playInfo.player.nLevel;
	SHeroCardCreatPro* pProInfo = pHeroCardConfig->getHeroCardCreatPro(nPlayerLv);
	if ( pProInfo == NULL )
	{
		ErrorLn(__FUNCTION__": can't find nPlayerLv = " << nPlayerLv << ", heroCardCreatPro config");
		return;
	}

	int nTotalPro= pProInfo->nMainHeroPro + pProInfo->nTeamHeroPro + pProInfo->nPrivateHeroPro + pProInfo->nWeekFreePro;
	if (nTotalPro != PROBABILITY_ENLARGE)
	{
		ErrorLn(__FUNCTION__": totalPro != PROBABILITY_ENLARGE; totalPro = " << nTotalPro << ", PROBABILITY_ENLARGE = " << PROBABILITY_ENLARGE);
		return;
	}

	map<int, sRandHeroCard> heroCardPro;
	// step1: 添加map（4,3,2,1为了排序用，周免-私有-队友-主添加顺序不能变，防止高等级Type被覆盖）
	// 周免
	vector<int> weekFreeHeroID;
	const int nFreeHeroArrayLen = 2 * HEROFREEWEEKLY_FREEID;
	int arrayFreeHero[nFreeHeroArrayLen] = {0};
	int nFreeHeroCnt = pSchemeHeroFreeWeekly->GetFreeHeroListByLv(arrayFreeHero, nFreeHeroArrayLen, nPlayerLv);
	for (int i = 0; i < nFreeHeroCnt; ++i)
	{
		if (arrayFreeHero[i] <= 0)
			continue;

		weekFreeHeroID.push_back(arrayFreeHero[i]);

		sRandHeroCard temp;
		temp.heroID = arrayFreeHero[i];
		temp.heroType = 1;
		heroCardPro[arrayFreeHero[i]] = temp;
	}

	// 私有
	vector<int> privateHeroID;

	obuf obData;
	getHeroArray(obData);
	const char * pHeroData = obData.data();
	int *pCount = (int *)pHeroData;
	int nBankHeroCount = *pCount;
	SHeroInfo *pBankHeroInfo = (SHeroInfo *)(pCount + 1);

	//SHeroInfo pBankHeroInfo[256];
	//int nBankHeroCount = m_heroMgr.getHeroArray(pBankHeroInfo, 256);
	for (int i = 0; i < nBankHeroCount; ++i, ++pBankHeroInfo)
	{
		if (pBankHeroInfo->nHeroID <= 0)
			continue;

		if ((pBankHeroInfo->dwFlags & HERO_FLAG_NOT_HAVE) == 0)
		{
			privateHeroID.push_back(pBankHeroInfo->nHeroID);

			sRandHeroCard temp;
			temp.heroID = pBankHeroInfo->nHeroID;
			temp.heroType = 2;
			heroCardPro[pBankHeroInfo->nHeroID] = temp;
		}
	}

	// 队友
	vector<int> teamHeroID;
	for (int i = 1; i < DBDEF_CAMPUSE_HEROID_MAXCOUNT; ++i)
	{
		int nHeroID = getPlayerInfo()->byCampHeroID[i];
		if (nHeroID <= 0)
			continue;

		teamHeroID.push_back(nHeroID);

		sRandHeroCard temp;
		temp.heroID = nHeroID;
		temp.heroType = 3;
		heroCardPro[nHeroID] = temp;
	}

	// 主英雄
	int nMainHeroID = getPlayerInfo()->byCampHeroID[0];
	if (nMainHeroID > 0)
	{
		sRandHeroCard temp;
		temp.heroID = nMainHeroID;
		temp.heroType = 4;
		heroCardPro[nMainHeroID] = temp;
	}

	// 剔除配置已关闭英雄
	ISchemePersonModel * pPersonModel = gServerGlobal->getSchemeCenter()->getSchemePersonModel();
	if (pPersonModel == NULL)
	{
		ErrorLn(__FUNCTION__": pPersonModel == NULL");
		return;
	}
	vector<int> vecLockHeroID;
	for (map<int, sRandHeroCard>::iterator iter = heroCardPro.begin(); iter != heroCardPro.end(); ++iter)
	{
		const PersonSkinModelItem * pPersonSkinModelItem = pPersonModel->getPersonSkinModelItem(iter->first);
		if (pPersonSkinModelItem == NULL)
		{
			vecLockHeroID.push_back(iter->first);
			WarningLn(__FUNCTION__": scheme PersonModel error! heroid = " << iter->first);
		}
	}
	for (vector<int>::iterator it = vecLockHeroID.begin(); it != vecLockHeroID.end(); ++it)
	{
		map<int, sRandHeroCard>::iterator itFind = heroCardPro.find(*it);
		heroCardPro.erase(itFind);

		vector<int>::iterator itFree = find(weekFreeHeroID.begin(), weekFreeHeroID.end(), *it);
		if (itFree != weekFreeHeroID.end())
			weekFreeHeroID.erase(itFree);
		
		vector<int>::iterator itPrivate = find(privateHeroID.begin(), privateHeroID.end(), *it);
		if (itPrivate != privateHeroID.end())
			privateHeroID.erase(itPrivate);

		vector<int>::iterator itTeam = find(teamHeroID.begin(), teamHeroID.end(), *it);
		if (itTeam != teamHeroID.end())
			teamHeroID.erase(itTeam);

		if (nMainHeroID == *it)
			nMainHeroID = 0;
	}

	// step2: 计算概率
	int heroProSize = heroCardPro.size();
	if (heroProSize <= 0)
	{
		return;
	}

	// 主英雄
	if (nMainHeroID > 0)
	{
		heroCardPro[nMainHeroID].heroCardPro += pProInfo->nMainHeroPro;
	}
	else
	{
		for (map<int, sRandHeroCard>::iterator iter = heroCardPro.begin(); iter != heroCardPro.end(); ++iter)
		{
			iter->second.heroCardPro += pProInfo->nMainHeroPro / heroProSize;
		}
	}

	// 队友
	if (teamHeroID.size() > 0)
	{
		for (int i = 0; i < teamHeroID.size(); i++)
		{
			heroCardPro[teamHeroID[i]].heroCardPro += pProInfo->nTeamHeroPro / teamHeroID.size();
		}
	}
	else
	{
		for (map<int, sRandHeroCard>::iterator iter = heroCardPro.begin(); iter != heroCardPro.end(); ++iter)
		{
			iter->second.heroCardPro += pProInfo->nTeamHeroPro / heroProSize;
		}
	}

	// 私有
	if (privateHeroID.size() > 0)
	{
		for (int i = 0; i < privateHeroID.size(); i++)
		{
			heroCardPro[privateHeroID[i]].heroCardPro += pProInfo->nPrivateHeroPro / privateHeroID.size();
		}
	}
	else
	{
		for (map<int, sRandHeroCard>::iterator iter = heroCardPro.begin(); iter != heroCardPro.end(); ++iter)
		{
			iter->second.heroCardPro += pProInfo->nPrivateHeroPro / heroProSize;
		}
	}

	// 周免
	if (weekFreeHeroID.size() > 0)
	{
		for (int i = 0; i < weekFreeHeroID.size(); i++)
		{
			heroCardPro[weekFreeHeroID[i]].heroCardPro += pProInfo->nWeekFreePro / weekFreeHeroID.size();
		}
	}
	else
	{
		for (map<int, sRandHeroCard>::iterator iter = heroCardPro.begin(); iter != heroCardPro.end(); ++iter)
		{
			iter->second.heroCardPro += pProInfo->nWeekFreePro / heroProSize;
		}
	}

	// step3: 添加vector
	for (map<int, sRandHeroCard>::iterator iter = heroCardPro.begin(); iter != heroCardPro.end(); ++iter)
	{
		cardPro.push_back(iter->second);
	}

	// step4: 计算分布区间
	for (int i = cardPro.size() - 1; i > 0; i--)
	{
		for (int j = 0; j < i; j++)
		{
			cardPro[i].heroCardPro += cardPro[j].heroCardPro;
		}
	}
	int nSize = cardPro.size();
	cardPro[nSize-1].heroCardPro = PROBABILITY_ENLARGE;
}

void CPlayerBankPart::setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode, BYTE byNumType, BYTE byOpType, WORD wApproach, DWORD dwTargetID, LPCSTR szLogMark, int nLogReserve)
{
	int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	m_heroMgr.setHeroOneInfo(nHeroID, lgValue, nType, nMode);
}

bool CPlayerBankPart::setHeroExclusiveTalentEffect( int nHeroID )
{
	m_heroMgr.setHeroExclusiveTalentEffect(nHeroID);
	return true;
}

bool CPlayerBankPart::removeHeroExclusiveTalentEffect()
{
	return m_heroMgr.removeHeroExclusiveTalentEffect();
}

bool CPlayerBankPart::addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount)
{
	return m_heroMgr.addHeroCardInfo(dwCardID, nValue, nCount);
}

bool CPlayerBankPart::canAddHeroCard(int nCardID, int nCount)
{
	return m_heroMgr.canAddHeroCard(nCardID, nCount);
}

bool CPlayerBankPart::setNewHero( SHeroInfo sNewHeroInfo )
{
	if( m_heroMgr.setNewHero(sNewHeroInfo) )
	{
		m_playInfo.herocnt = m_heroMgr.getHeroCount();
	}

	return true;
}

bool CPlayerBankPart::addNewHero( int nHeroID, int nLv, int nStar, DWORD dwDeadLine)
{
	if( m_heroMgr.addNewHero(nHeroID, nLv, nStar, dwDeadLine))
	{
		m_playInfo.herocnt = m_heroMgr.getHeroCount();
	}
	return true;
}

bool CPlayerBankPart::canAddHero( int nHeroID )
{
	return m_heroMgr.canAddHero(nHeroID);
}

bool CPlayerBankPart::delHero(int nHeroID)
{
	return m_heroMgr.delHero(nHeroID);
}

void CPlayerBankPart::setCrownEffect(int timeToAddEffect)
{
    m_crownPageMgr.setCrownEffect(timeToAddEffect);
}

// 是否可以给角色奖励
bool CPlayerBankPart::canPrizeActor(int nPrizeID, int nNum)
{
	return m_actorPrizeMgr.canPrizeActor(nPrizeID, nNum);
}

// 给角色奖励
bool CPlayerBankPart::prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
{
	return m_actorPrizeMgr.prizeActor(nPrizeID, nNum, wApproach, dwTargetID, szLogDesc, true);
}

// 给角色奖励组
bool CPlayerBankPart::groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
{
	return m_actorPrizeMgr.groupPrizeActor(nPrizeGroupID, pOutPrizeID, nOutPrizeCount, wApproach, dwTargetID, szLogDesc);
}

// 添加宝箱
bool CPlayerBankPart::addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
{
	return m_actorPrizeMgr.addPrizeChest(nChestType, nNum, wApproach, dwTargetID, szLogDesc);
}

void CPlayerBankPart::addPlayerExp(LONG lgValue)
{
    if(m_pMaster == NULL)
    {
        return;
    }
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return;
    }
	ISchemeLevelUpgrade* pUpdateScheme = pSchemeCenter->getLevelUpgradeSheme();
	if( pUpdateScheme == NULL)
    {
		return;
    }
	__IEntityProperty * pProperty = (__IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
	if (!pProperty)
	{
		return;
	}

	// 防沉迷衰减
	IAntiAddictServer * pAntiAddict = gServerGlobal->getAntiAddictServer();
	if ( pAntiAddict && lgValue > 0)
	{
		UID uid = m_pMaster->getUID();
		int reduce = pAntiAddict->getReduceRate(uid);
		if ( reduce < 100 )
		{
			lgValue = ( lgValue * reduce ) / 100;
			char szParam[256];
			ssprintf_s(szParam, sizeof(szParam), a2utf8("%d;防沉迷状态,收益衰减至%d％,实际获得角色经验%d"), CHAT_CHANNEL_SYSTEM, reduce, lgValue);
			g_EHelper.showSystemMessage(uid, CHAT_TIP_ANTIADDICT_INCOME_REDUCE, szParam);
		}
	}

	SPlayerInfo& playerInfo = m_playInfo.player;
    // 取得人升级的经验
	DWORD & dwRefExp = playerInfo.dwExp;

    // 当减少一个比原经验多的值时，经验值强制为0
    if(lgValue < 0 && -lgValue>dwRefExp)
    {
        dwRefExp = 0;
    }
    else
    {        
        dwRefExp += lgValue;
    }

    // 获取下一级所需经验
    DWORD dwNextLevelExp = (DWORD)pUpdateScheme->GetNextLevelExp( playerInfo.nLevel, ENEXPTYPE_STATICPLAYEREXP);
    if ( dwRefExp < dwNextLevelExp )
    {
        return;
    }

    // 获取最大等级
    int nMaxLevel = pUpdateScheme->GetSchemeLevelUpgradeMaxLevel(ENEXPTYPE_STATICPLAYEREXP);

    // 升级中逻辑处理
	while( dwRefExp >= dwNextLevelExp )
	{
        // 已经到最大等级，不能再升级了
        if(playerInfo.nLevel >= nMaxLevel)
            break;

        // 增加等级
		addActorLevel(1);
        
        // 处理升级后的经验值
		dwRefExp -= dwNextLevelExp;

        // 获取下一级的升级经验
		dwNextLevelExp = (DWORD)pUpdateScheme->GetNextLevelExp( playerInfo.nLevel, ENEXPTYPE_STATICPLAYEREXP);
	}
}

void CPlayerBankPart::setMustSetTypeInfo(DWORD* pData, LONG lgValue, int nMode)
{
	if ( ENCHANGEDATAMODE_SET == nMode)
		*pData = lgValue;
	else if ( ENCHANGEDATAMODE_ADD == nMode )
		WarningLn(__FUNCTION__": this data muset set can't add!!!");
}

void CPlayerBankPart::setMustSetTypeInfo(WORD* pData, WORD wValue, int nMode)
{
	if ( ENCHANGEDATAMODE_SET == nMode)
		*pData = wValue;
	else if ( ENCHANGEDATAMODE_ADD == nMode )
		WarningLn(__FUNCTION__": this data muset set can't add!!!");
}

void CPlayerBankPart::setMustSetTypeInfo(BYTE* pData, BYTE byValue, int nMode)
{
	if ( ENCHANGEDATAMODE_SET == nMode)
		*pData = byValue;
	else if ( ENCHANGEDATAMODE_ADD == nMode )
		WarningLn(__FUNCTION__": this data muset set can't add!!!");
}

void CPlayerBankPart::sendFirstWinTimeOnLogin()
{
	// 下发到客户端 更新数据
	obuf obmsg;

	DWORD dwFirstWinTime =getTaskDataInt(TASKKEY_ACTOR_WAR_FIRSTWINTIME);
	DWORD dwCurrentTime = time(NULL);
	dwFirstWinTime = (dwFirstWinTime == TASKDATA_DEFAULT_VALUE ? (dwCurrentTime - FIRSTWINTIME_INTERVAL) : dwFirstWinTime);

	DWORD dwResidueTime = FIRSTWINTIME_INTERVAL - (dwCurrentTime - dwFirstWinTime);
	dwResidueTime = (dwResidueTime < 0 ? 0 : dwResidueTime);
	obmsg.push_back( &dwResidueTime , sizeof(dwResidueTime ) );

    __IPlayerRole* pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if (pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_LOGIN_SENDDATA, obmsg.data(), obmsg.size());
    }
}

// 检测通知引导类型任务数据
void CPlayerBankPart::checkNoticeGuideTypeTask()
{
    DWORD dwNoviceGuideType = getTaskDataInt(TASKKEY_ACTOR_NOVICE_GUIDE);

    if (dwNoviceGuideType == TASKDATA_DEFAULT_VALUE)
    {
        dwNoviceGuideType = ENNOVICEGUIDE_TYPE_GUID_NEWPLAYER;
        addTaskDataInt(TASKKEY_ACTOR_NOVICE_GUIDE, dwNoviceGuideType);
    }
}

// 发送本命英雄入口完成情况任务数据
void CPlayerBankPart::sendLifeHeroTaskDataToClient()
{
	__IPlayerRole* pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole)
	{
		int heroFinished = getTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_FINISHED, 0);
		pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_LIFEHERO_TASKDATA, &heroFinished, sizeof(heroFinished));
	}
}

// 发送本命英雄奖励数据
void CPlayerBankPart::sendLifeHeroPrizeTaskDataToClient()
{
	__IPlayerRole* pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole)
	{
		int prize = getTaskDataIntEx(TASKKEY_ACTOR_LIFEHERO_PRIZE, 0);
		pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_LIFEHERO_PRIZE_TASKDATA, &prize, sizeof(prize));
	}
}

void CPlayerBankPart::resetLifeHero()
{
	m_heroMgr.resetLifeHero();
}

void CPlayerBankPart::addLifeHero(int heroID, int cultureID)
{
	m_heroMgr.addLifeHero(heroID, cultureID);
}

// 发送新手引导类型
void CPlayerBankPart::sendNoviceGuideTypeToClient()
{
	DWORD dwNoviceGuideType = getTaskDataInt(TASKKEY_ACTOR_NOVICE_GUIDE);

    __IPlayerRole* pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if (pPlayerRole)
    {
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_NOVICEGUIDE_DATA, &dwNoviceGuideType, sizeof(dwNoviceGuideType));
    }
}


void CPlayerBankPart::onUpdateChangedPlayerInfo(int nInfoChangeType)
{
	if(nInfoChangeType <= 0)
	{
		return;
	}

	obuf obData;

    __IPlayerRole* pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
    if (pPlayerRole)
    {
	    obData << m_playInfo<<nInfoChangeType;
        pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_UPDATE_PLAYER_INFO, obData.data(), obData.size());
    }

	updatePlayerInfoToDB();
}

// 新的一天
void CPlayerBankPart::onNewDay()
{
    TASK_TRACELN("--------------------- new day -----------------------")

    // 商城管理器
    m_pointShopMgr.onNewDay();

    // 角色奖励管理器
    m_actorPrizeMgr.onNewDay();
}

// 获取任务数据
int CPlayerBankPart::getTaskDataIntEx(int nKey, int nDefault)
{
	int nResult = getTaskDataInt(nKey);
	if (nResult == TASKDATA_DEFAULT_VALUE)
		nResult = nDefault;
	return nResult;
}

// 设置任务数据
bool CPlayerBankPart::setTaskDataIntEx(int nKey, int nData, int nDefault)
{
	if (nData == nDefault)
		return removeTaskDataInt(nKey);
	return addTaskDataInt(nKey, nData);
}

/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
void CPlayerBankPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if (m_pMaster == NULL)
	{
		return;
	}

	// 系统的事件
	switch ( MAKE_EVENT_KEY (bSrcType, wEventID) )
	{
	case MAKE_EVENT_KEY (SOURCE_TYPE_SYSTEM, EVENT_NEW_DAY):    // 新的一天事件
		{
			onNewDay();
		}
		break;

	default: break;
	}

	// 人物自己的事件
	switch (wEventID)
	{
	case EVENT_ENTITY_CREATE:
		{
			// 进入战场调用效果 
#ifdef FUNCTION_TALENTPAGE_OPEN
			m_talentPageMgr.SetTalentPageEffect();
#endif

#ifdef FUNCTION_RUNE_OPEN
			m_runeManager.setRuneEffect();
#endif

			m_crownPageMgr.setCrownEffect(ETIME_TO_ADD_EFFECT::ENTITY_CREATE);

			if (m_bWaitForLoadTaskData)
			{
				m_pTaskData->importBuffData(m_pMaster->getIntProperty(PROPERTY_ID), (LPSTR)m_pIntTaskData, m_nIntTaskDataLen, (LPSTR)m_pByteTaskData, m_nByteTaskDataLen);
				SAFE_DELETEARRAY(m_pIntTaskData);
				SAFE_DELETEARRAY(m_pByteTaskData);

				m_bWaitForLoadTaskData = false;
			}

			m_heroMgr.learnHeroXPSpellInWar();
		}
		break;

	case EVENT_ACTOR_PREPARE_LOGOUT_SAVE:        // 角色预登出场景服存盘
		{
			if(m_pMaster != NULL)
			{
				DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
				if(!IsComputerPDBID(dwActorID))
				{
					updatePlayerInfoToDB();
					// 英雄数据更新到数据库
					m_heroMgr.updateHerosToDB();
					// 广播开宝箱结果（存在没点开就下线的）
					m_actorPrizeMgr.broadcastChestResult();
				}
			}
		}
		break;

     case EVENT_ACTOR_LOGOUT:        // 角色预登出场景服存盘
		{
            if(pszContext == NULL || nLen != sizeof(event_actor_logout))
            {
                return;
            }

            IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
            if (pDBEngine == NULL || m_pMaster == NULL)
            {
                return;
            }

            event_actor_logout* pEvent = (event_actor_logout*) pszContext;
            DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);                   // 玩家角色DBID
            if (IsComputerPDBID(dwActorID))
            {
                return;
            }

            if (pEvent->eMode == elogout_offline)
            {
                obuf ob;
                obuf bufdata;
                DBREQ_PARAM_UPDATEACTORINFO sActorInfo;
                sActorInfo.dwPDBID      = dwActorID;                   // 玩家角色DBID
                sActorInfo.wLevel       = m_playInfo.player.nLevel;                     // 最高等级
                sActorInfo.nLastlogout  = (int)time(NULL);                                          // 临时打就保存 当前时间 
                
                for (MATCHTYPERANK_MAP::iterator iterFind = m_mapMatchTypeRank.begin();iterFind != m_mapMatchTypeRank.end(); ++iterFind)
                {
                    
                    DBREQ_WARRECORD_INFO sInfo;
                    sInfo.nMatchType = (BYTE)iterFind->first;   //撮合类型
                    sInfo.dwMatchNum = iterFind->second.dwMatchNum;
                    sInfo.dwWinNum = iterFind->second.dwWinNum;
                    if (sInfo.dwMatchNum == 0 && sInfo.dwWinNum == 0)
                    {
                        continue;
                    }
                    bufdata<<sInfo;
                    sActorInfo.nRecordsCount++;              // 比赛战绩记录数
                    
                }
                int nBufSize = bufdata.size();
                ob<<sActorInfo<<nBufSize;
                ob.push_back(bufdata.data(), bufdata.size());

                if (!pDBEngine->executeSP(GAMEDB_REQUEST_UPDATEACTORINFO, dwActorID, ob.data() , ob.size(), static_cast<IDBRetSink *>(this)) )
                {
                    ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_UPDATEACTORINFO failed!");
                    return;
                }
            }
		}
		break;

	case EVENT_ACTOR_READTASKDATA:
		{
			// 首次登录刷新卡牌列表
			if( checkTodayFirstLogin() )
			{
               time_t nCurrentTime = time(NULL);
				// 设置登录时间
				setTaskDataIntEx(TASKKEY_ACTOR_LAST_LOGIN_TIME, nCurrentTime);
				// 刷新商城已购买数量和重置手动刷新次数
				m_pointShopMgr.reflushBuyNumAndResetOptCount();
			}
			// 发送首胜时间
			sendFirstWinTimeOnLogin();

            checkNoticeGuideTypeTask();
			// 发送本命英雄是否完成的任务数据
			sendLifeHeroTaskDataToClient();
			// 发送本命英雄奖励任务数据
			sendLifeHeroPrizeTaskDataToClient();
			// 发送新手引导类型
			sendNoviceGuideTypeToClient();
			// 发送商城已购买数量信息
			m_pointShopMgr.updatePointShopBuyNumToClient(-1);
			// 发送商城卡牌列表信息
			m_pointShopMgr.initReflushCardInfo();

            // 同步个人活跃度到共享属性
            syncActivity();
		}
		break;
	case EVENT_ENTITY_CONTINUE_PLAY:
		{
			m_heroMgr.sendHeroTalentInfoToClient();
		}
		break;
	case EVENT_ENTITY_ACTOR_CHANGE_LEVEL:
		{
			//// 角色升级 写入日志
			//IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
			//if ( pOSSLogServer )
			//{
			//	int nTime = (int)time(NULL);
			//	pOSSLogServer->addActorUpgradeLog(getThisGameWorldID(), m_playInfo.dwUserID, m_pMaster->getIntProperty(PROPERTY_ID),1,m_playInfo.player.nLevel,nTime);
			//}
		}
		break;
	case EVENT_ACTOR_CHANGE_MONEY:
		{
			if(pszContext == NULL || nLen < sizeof(event_actor_change_money))
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return;
			}

			event_actor_change_money* pEvent = (event_actor_change_money*)pszContext;

			// 點券變化原因 ：改名消耗
			bool bRenameCostTicket = m_cRenameCache[0]!='\0'?true:false;
			if(bRenameCostTicket)
			{
				if(!pEvent->bResult)
				{
					// db扣除点券失败
					onDBActorServiceCheckRenameFlagFail(pEvent->szParam);
				}
				else
				{
					// 点券扣除成功 判断改名是否成功
					bool bReNameSuccess = updateActorNameToDBAndCache((int)ENPOINTSHOPRESTYPE_POINT, nRenameMoneyCache);
					if(bReNameSuccess)
					{
						g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_ACTOR_INFO_NAME_CHANGE_SUCESS);

						/*	IGatewayAcceptorService * pGatewayAcceptorService = gServerGlobal->getGatewayAcceptorService();
						if(pGatewayAcceptorService )
						pGatewayAcceptorService->kickOutClient(((PlayerRole*)m_pMaster)->getClientID(), EKickoutClient_Reason_KickoutSelf);*/
					}
					else
					{
						g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_ACTOR_INFO_NAME_CHANGE_FAIL);
					}

					sendRenameResultToClient(bReNameSuccess);
				}

			}
		}
		break;
    case EVENT_ENTITY_CHANGE_HERO:
        {
            if ( pszContext==0 || nLen!=sizeof(event_entity_change_hero) )
            {
                return;
            }

			event_entity_change_hero* context = (event_entity_change_hero*)pszContext;
         
			// 主城切换英雄保存到数据库
			if (IsMainMap(m_pMaster->getMapID()))
			{
				m_playInfo.player.nHeroID = context->nNewHeroID;
			}
        }
        break;
	default:
		break;
	}
}

bool CPlayerBankPart::checkTodayFirstLogin()
{
	bool isTodayFirstLogin = false;

	time_t nCurrentTime = time(NULL);
	time_t nLastLoginTime = getTaskDataInt(TASKKEY_ACTOR_LAST_LOGIN_TIME);
	if(nLastLoginTime == TASKDATA_DEFAULT_VALUE)
	{
		isTodayFirstLogin = true;
	}
	else
	{
		tm sLastTime = *localtime(&nLastLoginTime);
		tm sCurTime = *localtime(&nCurrentTime);
		if (sLastTime.tm_year != sCurTime.tm_year || sLastTime.tm_yday != sCurTime.tm_yday)
		{
			isTodayFirstLogin = true;
		}
	}

	return isTodayFirstLogin;
}

int CPlayerBankPart::getCurTalentPage()
{
	int nCurTalentPage = -1;

#ifdef FUNCTION_TALENTPAGE_OPEN
	nCurTalentPage = m_talentPageMgr.GetCurTalentPage();
#endif

	return nCurTalentPage;
}


// 储存同队友和自己的上一把的英雄id
void CPlayerBankPart::setActorTeammateHeroID(int* pTeammateHeroID)
{
	if(pTeammateHeroID == NULL)
	{
		return;
	}

	for (int i=0;i<DBDEF_CAMPUSE_HEROID_MAXCOUNT;++i)
	{
		m_playInfo.player.byCampHeroID[i] = pTeammateHeroID[i];
	}

	updatePlayerInfoToDB();
}

int CPlayerBankPart::getRandHeroCard(DWORD *pCardIDList, int *pCardNumList, int nCardListSize, WORD wApproach, int nParam)
{
	IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
	if (pOSSLogServer == NULL)
	{
		ErrorLn(__FUNCTION__": pOSSLogServer = NULL");
		return 0;
	}
	if (pCardIDList == NULL || pCardNumList == NULL)
		return 0;

	// 随机
	int nCount = 0;
	DWORD arrCard[GETCARD_MAX_NUM] = {0};
	if (wApproach == OSSRES_ADD_WAR_PRIZE)
	{
		nCount = randHeroCardForWar(nParam, arrCard, GETCARD_MAX_NUM);
	}
	else if (wApproach == OSSRES_ADD_PRIZECHEST)
	{
		nCount = randHeroCardForChest(nParam, arrCard, GETCARD_MAX_NUM);
	}
	else
	{
		// 其他途径获得，先按宝箱的算法获得
		nCount = randHeroCardForChest(nParam, arrCard, GETCARD_MAX_NUM);
		//ErrorLn(__FUNCTION__": No such approach to get HeroCard, wApproach = " << wApproach);
		//return 0;
	}

	// 统计
	if	(nCount == 0)
	{
		return 0;
	}
	std::map<int, int> mapCardInfo;
	for (int i = 0; i < nCount; ++i)
	{
		mapCardInfo[arrCard[i]] += 1;
	}
	int nCardListIndex = 0;
	std::map<int, int>::iterator itBegin = mapCardInfo.begin();
	for ( ;itBegin != mapCardInfo.end(); ++itBegin )
	{
		if (nCardListIndex >= nCardListSize)
			break;
		pCardIDList[nCardListIndex] = itBegin->first;
		pCardNumList[nCardListIndex] = itBegin->second;
		++nCardListIndex;
	}

	// 记录
	char szlogDesc[64] = {0};
	int nCurTime = time(NULL);
	if (addHeroCardInfo(pCardIDList, pCardNumList, nCardListIndex))
	{
		for (int i = 0; i < nCardListIndex; ++i)
		{
			memset(szlogDesc, 0, sizeof(szlogDesc));
			ssprintf_s(szlogDesc, sizeof(szlogDesc), a2utf8("卡牌id:%d, 数量:%d"), pCardIDList[i], pCardNumList[i]);
			//pOSSLogServer->addNumericLog(getThisGameWorldID(), OSS_OPT_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), 0, pCardNumList[i], OSS_NUMERIC_HEROCARD, OSS_RESOURCE_OPT_ADD, wApproach, nCurTime, szlogDesc, pCardIDList[i]);
			pOSSLogServer->addActorGoodsLog(getFromGameWorldID(), OSS_OPT_TYPE_SYSTEM, 0, OSS_OPT_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), OSS_GOODS_HEROCARD, OSS_RESOURCE_OPT_ADD, wApproach, pCardIDList[i], pCardNumList[i], nCurTime, szlogDesc);
		}
	}

	return nCardListIndex;
}

/////////////////////////////////////英雄天赋/////////////////////////////////////

bool  CPlayerBankPart::canAddHeroTalentInfo(int nHeroId,int nTalentId)
{
	return m_heroMgr.canAddHeroTalentInfo( nHeroId, nTalentId);
}

bool  CPlayerBankPart::addHeroTalentInfo(int nHeroId,int nTalentId)
{
	return m_heroMgr.addHeroTalentInfo( nHeroId, nTalentId);
}

////////////////////////////////////英雄皮肤//////////////////////////////////////

bool CPlayerBankPart:: canAddHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime)
{
	return m_heroMgr.canAddHeroSkinInfo(nHeroId,nSkinId, dwDurationTime);
}

bool  CPlayerBankPart::addHeroSkinInfo(int nHeroId,int nSkinId,DWORD dwDurationTime)
{
	return m_heroMgr.addHeroSkinInfo(nHeroId,nSkinId, dwDurationTime);
}

bool CPlayerBankPart::getOtherRuneData(obuf & outData)
{
	bool bRet = false;
#ifdef FUNCTION_RUNE_OPEN
    bRet = m_runeManager.getOtherRuneData(outData);
#endif
	return bRet;
}

bool CPlayerBankPart::getOtherCrownPageData(obuf & outData)
{
	m_crownPageMgr.getOtherCrownPageData(outData);
	return true;
}

bool CPlayerBankPart::getOtherCrownGemstoneData(obuf & outData)
{
	m_crownPageMgr.getOtherCrownGemstoneData(outData);
	return true;
}

bool CPlayerBankPart::getOtherStrongHeroInfo(obuf & outData)
{
	obuf tmpBuf;
	int nCount = m_heroMgr.fillHeroInfo(tmpBuf);
	outData << nCount;
	outData.push_back(tmpBuf.data(), tmpBuf.size());

	return true;
}

bool CPlayerBankPart::initActorUnlockPrizeInfo()
{
	// 初始过有数据返回
	if(m_vecActorUnlockInfo.actorPrizeUnlockVec.size() > 0)
	{
		return true;
	}

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL !");
		return false;
	}

	ISchemePrizeUnlock* pPrizeUnlock = pSchemeCenter->getSchemePrizeUnlock();
	if ( pPrizeUnlock == NULL )
	{
		ErrorLn(__FUNCTION__": ISchemePrizeUnlock == NULL !");
		return false;
	}

	SActorPrizeUnlockVec* pActorUnlock = pPrizeUnlock->getActorPrizeUnlockInfo();
	if(pActorUnlock == NULL)
	{
		ErrorLn(__FUNCTION__": getActorPrizeUnlockInfo Fail, pActorUnlock == NULL!");
		return false;
	}

	m_vecActorUnlockInfo.actorPrizeUnlockVec.clear();
	m_vecActorUnlockInfo.actorPrizeUnlockVec = pActorUnlock->actorPrizeUnlockVec;
	int nActorLevel  = m_playInfo.player.nLevel;

	std::vector<SActorPrizeUnlockItem>::iterator iter =   m_vecActorUnlockInfo.actorPrizeUnlockVec.begin();
	std::vector<SActorPrizeUnlockItem>::iterator iterEnd =   m_vecActorUnlockInfo.actorPrizeUnlockVec.end();

	for (;iter!=iterEnd;++iter)
	{
		if(iter->bUnlock)
		{
			continue;
		}

		if(nActorLevel >= iter->nUnlockLevel)
		{
			iter->bUnlock = true;
		}
	}

	return true;
}

void CPlayerBankPart::checkUnlockActorPrizeByLevelChange()
{
	if(m_pMaster == NULL)
		return;
	__IPlayerRole * pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if( pPlayerRole == NULL)
		return;

	if(m_vecActorUnlockInfo.actorPrizeUnlockVec.size() <= 0)
	{
		ErrorLn(__FUNCTION__": actorPrizeUnlockVec data is wrong");
		return ;
	}

	int nActorLevel  = m_playInfo.player.nLevel;
	DWORD uidSelf = m_pMaster->getUID();

	std::vector<SActorPrizeUnlockItem>::iterator iter =   m_vecActorUnlockInfo.actorPrizeUnlockVec.begin();
	std::vector<SActorPrizeUnlockItem>::iterator iterEnd =   m_vecActorUnlockInfo.actorPrizeUnlockVec.end();

	for (;iter!=iterEnd;++iter)
	{
		if(iter->bUnlock)
		{
			continue;
		}

		if(nActorLevel >= iter->nUnlockLevel)
		{
			int		nPrizeCount = 0;
			obuf	obPrize;
			for (int i = 0; i < PRIZEUNLOCK_PRIZEID_MAXCOUNT; i++)
			{
				int nPrizeId = iter->nPrizeId[i];
				if (nPrizeId <= 0)
					break;

				if (canPrizeActor(nPrizeId,1))
				{
					prizeActor(nPrizeId,1,OSSRES_ADD_ACTORUPGRADE,uidSelf,NULL);
				}

				nPrizeCount++;
				obPrize.push_back(&nPrizeId, sizeof(nPrizeId));
			}

			// 解锁成功, 发送邮件通知
			iter->bUnlock = true;

			MailHelper mailHelper;
			IMailSceneService * pMailSceneService = mailHelper.m_ptr;
			if (pMailSceneService == NULL)
				return;
			pMailSceneService->sendFillMail(m_playInfo.player.dwActorID, iter->nMailId);

			// 将升星成功后的数据发送到客户端
			if (nPrizeCount > 0)
			{
				obuf	obData;
				obData.push_back(&nActorLevel, sizeof(nActorLevel));
				obData.push_back(&nPrizeCount, sizeof(nPrizeCount));
				obData.push_back(obPrize.data(), obPrize.size());
				pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_ACTOR_UNLOCK_RESULT, obData.data(), obData.size());
			}
		}
	}
}

bool CPlayerBankPart::setActorLevel(int nValue)
{
	int nOldLevel  = m_playInfo.player.nLevel;
	if(nOldLevel == nValue)
	{
		return false;
	}

	m_playInfo.player.nLevel = nValue;
	fireActorLevelChangeEvent();
	return true;
}

bool  CPlayerBankPart::addActorLevel(int nAddValue)
{
	if(nAddValue == 0)
	{
		return false;
	}

    // 等级变化
	m_playInfo.player.nLevel += nAddValue;

	fireActorLevelChangeEvent();
	return true;
}

void  CPlayerBankPart::fireActorLevelChangeEvent()
{
	if(m_pMaster == NULL)
	{
		return;
	}
	__IEntityProperty* pProperty = (__IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pProperty == NULL)
	{
		return;
	}

    // 这里会自动更新等级属性
	pProperty->setProperty_Integer(PROPERTY_LEVEL, m_playInfo.player.nLevel);

	checkUnlockActorPrizeByLevelChange();

	// 角色等级升级日志
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (pOSSLogService)
    {
        pOSSLogService->addActorUpgradeLog(m_playInfo.player.dwWorldID, m_playInfo.dwUserID, m_playInfo.player.dwActorID, 1, m_playInfo.player.nLevel, time(NULL));
    }
	
    // 触发等级变化事件
	event_actor_change_level context;
	context.uChangedUID = m_pMaster->getUID();
	context.nLevel = m_playInfo.player.nLevel;
	m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_ACTOR_CHANGE_LEVEL,(LPCSTR)&context,sizeof(context));

	//
	sendActorDataTaskEvent(emTaskLuaEventType_Level, m_playInfo.player.nLevel, 0, 0);
}

void CPlayerBankPart::getPlayerRoomExData(SMatchPlayerRoomExData *pRoomExData)
{
    
	if (m_pMaster ==NULL)
	{
		return;
	}

	if (pRoomExData == nullptr)
	{
		return;
	}
	pRoomExData->dwWorldID = getThisGameWorldID();		                        
	pRoomExData->dwZoneServerID = gServerGlobal->getCenterConnectorService()->getLocalServerCSID();                         
	pRoomExData->dwSceneID = m_pMaster->getSceneID();
	pRoomExData->nSceneSN = gServerGlobal->getSceneManager()->sceneIDToSerialNo(pRoomExData->dwSceneID);
	pRoomExData->dwUserID = m_playInfo.dwUserID;

	for (int i = VOCATION_INVALID + 1; i < VOCATION_MAX; ++i)
	{
		SHeroInfo *pHero = getHeroInfo(i);
        if (pHero == NULL)
        {
            continue;
        }

        // 周免或者已购买英雄
        if (m_heroMgr.getHeroIsFree(i) || m_heroMgr.isHeroHave(pHero->dwFlags))
        {
            pRoomExData->nHero[i].bCanUse = true;
        }

		if (pHero!= NULL)
		{
			pRoomExData->nHero[i].byHeroStarLv = pHero->nHeroStar;
			pRoomExData->nHero[i].byHeroStaticLv = pHero->nLevel;
			if (pHero->dwMatchNum <= 0)
			{
				pRoomExData->nHero[i].fWinRate = 0.0f;
				pRoomExData->nHero[i].nMatchNum= 0;
			}
			else
			{
				pRoomExData->nHero[i].fWinRate = (float(pHero->dwWinNum)/float(pHero->dwMatchNum)) * 100.0f;
				pRoomExData->nHero[i].nMatchNum = pHero->dwMatchNum;
			} 
		}
	}

	for (int i = MatchType_None; i < MatchType_MaxID; ++i)
	{
		pRoomExData->nMatchTypeGrade[i] = getMatchTypeRankGrade((EMMatchType)i);
	}

	return;
}

void CPlayerBankPart::onClientRequestActorNameChange( void * data, size_t len )
{
	if(m_pMaster==NULL)
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return ;
	}

	if (data == NULL || len != sizeof(msg_ActorName_Change))
	{
		ErrorLn(__FUNCTION__": message is invalid!")
		return ;
	}

	msg_ActorName_Change* sMsg = (msg_ActorName_Change*)data;	

	// 验证是否符合规则 游戏库 询问名字是否符合规则
	DBREQ_PARAM_ADDSERVICES_USER_CHK reqData;
	reqData.dwWorldID			= m_playInfo.player.dwWorldID;									// 游戏世界ID
	reqData.dwUserID			= m_playInfo.dwUserID;											// 账号数据库ID
	reqData.dwActorID			= m_pMaster->getIntProperty(PROPERTY_ID);						// 角色数据库ID
	reqData.nType				= (BYTE)ActorServiceType_Rename;								// 修改类型 EMActorServiceType
	reqData.nMoneyType			= (BYTE)sMsg->nCostType;										// 扣钱类型（0点券 1金钱）
	sstrcpyn(reqData.szCurValues, m_playInfo.player.szActorName, sizeof(reqData.szCurValues));	// 现名称	
	sstrcpyn(reqData.szNewValues, sMsg->szActorName, sizeof(reqData.szNewValues));				// 新名称

	addService_RequestToDB(GAMEDB_REQUEST_ADDSERVICES_USER_CHK, (LPCSTR)&reqData, sizeof(DBREQ_PARAM_ADDSERVICES_USER_CHK));
	return;
}

// 增值服务DB操作
bool CPlayerBankPart::addService_RequestToDB(int nCmdID, LPCSTR lpszInData, int nInDataLen)
{
	if(m_pMaster==NULL)
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return false;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if(NULL == lpszInData || nInDataLen <= 0 || pDBEngine == NULL)
	{
		return false;
	}

	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	if (! pDBEngine->executeSP(nCmdID, dwPDBID, lpszInData ,nInDataLen, static_cast<IDBRetSink *>(this)) )
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() addService_RequestToDB failed nCmdID = "<< nCmdID);
		return false;
	}

	return true;
}

// 增值服务DB返回
bool CPlayerBankPart::addService_ResponseFromDB(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if ( nDBRetCode != DBRET_CODE_SUCCEED )
	{
		ErrorLn( __FUNCTION__<<" nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
		return false;
	}

	if( m_pMaster == NULL )
	{
		ErrorLn(__FUNCTION__": m_pMaster == NULL!");
		return false;
	}

	switch(nCmdID)
	{
	case GAMEDB_REQUEST_ADDSERVICES_USER_CHK:			// 增值服务费用信息查询()
		{
			if(nOutLen < sizeof(DBREQ_RESULT_ADDSERVICES_USER_CHK))
			{
				ErrorLn("GAMEDB_REQUEST_ADDSERVICES_USER_CHK DBReturn Fail ，nCmdID = " << nCmdID << " DBREQ_RESULT_ADDSERVICES_USER_CHK Len =" << sizeof(DBREQ_RESULT_ADDSERVICES_USER_CHK) << "nDBReturnLen" << nOutLen);
				return false;
			}

			// 数据库请求返回回调方法 增值服务费用信息查询 GAMEDB_REQUEST_ADDSERVICES_PRICE_READ
			DBREQ_RESULT_ADDSERVICES_USER_CHK * pResut = (DBREQ_RESULT_ADDSERVICES_USER_CHK *)pOutData;	
			onDBReturn_AddServiceUserCheck(*pResut, pszDBRetDesc);
		}
		break;
	default:
		break;
	}

	return true;

}
//数据库请求返回回调方法 增值服务游戏规则查询 GAMEDB_REQUEST_ADDSERVICES_USER_CHK
bool CPlayerBankPart::onDBReturn_AddServiceUserCheck(const DBREQ_RESULT_ADDSERVICES_USER_CHK & param, const char * pszErrorDesc)
{
	// 取得人物对象
	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	if(dwPDBID!=param.dwActorID)
	{
		ErrorLn(__FUNCTION__"m_pMaster dwPDBID : " << dwPDBID << " != DBReturn dwActorID =" << param.dwActorID );
		return false;
	}

	bool bDBCheckSuccess = false;
	// 增值服务类型
	switch(param.nType)
	{
	case ActorServiceType_Rename:				// 改角色名称 
		{
			if(param.nFlags == 0)
			{
				onDBActorServiceCheckRenameFlagFail(pszErrorDesc);
				break;
			}

			int nResultFlag = doActorServiceTypeRename(param.szNewValues, (int)param.nMoneyType);
			if(nResultFlag == (int)ENRENAMERESUNLTTYPE_SUCCESS)
			{
				g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_ACTOR_INFO_NAME_CHANGE_SUCESS);
				bDBCheckSuccess = true;
			}
			else if(nResultFlag == (int)ENRENAMERESUNLTTYPE_FAIL)
			{
				g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_ACTOR_INFO_NAME_CHANGE_FAIL);
				bDBCheckSuccess = false;
			}
			else if(nResultFlag == (int)ENRENAMERESUNLTTYPE_NOSURE)
			{
				bDBCheckSuccess = true;
				break;
			}

			sendRenameResultToClient(bDBCheckSuccess);
		}
		break;
	case ActorServiceType_ChangeSex:			// 改变性别
		{
			bDBCheckSuccess = true;
		}
		break;
	default:
		break;
	}

	return bDBCheckSuccess;
}

void CPlayerBankPart::sendRenameResultToClient(bool bSuccess)
{
	obuf obmsg;
	bool bRenameSuccess = bSuccess;
	obmsg.push_back(&bRenameSuccess, sizeof(bRenameSuccess));

	__IPlayerRole *pPlayerRole=CAST_TYPE(__IPlayerRole*, m_pMaster);
	if(pPlayerRole)
	{
		pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_RENMAE_RESULT, obmsg.data(), obmsg.size() );
	}
}

bool CPlayerBankPart::updateActorNameToDBAndCache(int nMoneyType,int nMoney)
{
	if(nMoney <= 0)
		return false;

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if( pDBEngine == NULL)
		return false;

	__IEntityProperty* pProperty = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if( pProperty == NULL)
		return false;

	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
	// 修改游戏中心角色数据
	DBREQ_PARAM_UPDATECENTERDBCREATEACTOR centerData;
	centerData.dwActorID = dwPDBID;
	centerData.dwUserID = m_playInfo.dwUserID;
	centerData.dwWorldID = m_playInfo.player.dwWorldID;
	centerData.nMoneyType = nMoneyType;
	centerData.nMoney = nMoney;
	centerData.nType = (int)ActorServiceType_Rename;
	sstrcpyn(centerData.szNewValues, m_cRenameCache, sizeof(centerData.szNewValues));
	sstrcpyn(centerData.szCurValues, m_playInfo.player.szActorName, sizeof(centerData.szCurValues));
	obuf oBuff;
	oBuff.push_back(&centerData, sizeof(DBREQ_PARAM_UPDATECENTERDBCREATEACTOR));

	if (! pDBEngine->executeSP(GAMEDB_REQUEST_UPDATECENTERDBCREATEACTOR, dwPDBID, oBuff.data() ,oBuff.size(), static_cast<IDBRetSink *>(this)) )
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() ActorServiceType_Rename in ActorCenterDB failed nCmdID = "<< GAMEDB_REQUEST_UPDATECENTERDBCREATEACTOR);
		return false;
	}

	// 修改游戏库中的角色数据
	sstrcpyn(m_playInfo.player.szActorName, m_cRenameCache, sizeof(m_playInfo.player.szActorName));
	onUpdateChangedPlayerInfo(ENPLAYERINFOTYPE_NAME);
	// 更新属性中名字
	pProperty->setProperty_custom(PROPERTY_NAME,m_playInfo.player.szActorName,sizeof(m_playInfo.player.szActorName));
	// 更新角色类中的名字变量
	((PlayerRole*)m_pMaster)->updateName(m_playInfo.player.szActorName);
	// 清空名字缓存
	memset(m_cRenameCache,0,DBDEF_ACTORNAME_MAXSIZE);
	nRenameMoneyCache = 0;

	// 发改名字事件
	event_actor_change_name context;
	context.uActorUID = m_pMaster->getUID();
    context.nPDBID = dwPDBID;
    sstrcpyn( context.szName, m_playInfo.player.szActorName, sizeof(context.szName) );
	m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_ACTOR_CHANGE_NAME,(LPCSTR)&context,sizeof(context));	

    // 发全局事件
    rkt:: IEventEngine* pEntityEngine = gServerGlobal->getEventEngine();
    if(pEntityEngine) {
        pEntityEngine->FireExecute(EVENT_GAME_ACTOR_CHANGE_NAME, SOURCE_TYPE_PERSON, dwPDBID, (LPCSTR)&context, sizeof(context));
    }
	return true;
}

int CPlayerBankPart::doActorServiceTypeRename(const char* pNewName, const int nMoneyType)
{
	if(isPublicGameWorld())
		return 0;

	if(m_playInfo.player.dwWorldID!=getThisGameWorldID())
		return 0;

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return 0;

	ISchemePointShop *pSchemeShop = pSchemeCenter->getSchemePointShop();
	if (pSchemeShop == NULL)
		return 0;

	// 扣钱
	vector<SPointShopGoodsConfig_S*> listSellConfig;
	pSchemeShop->getPointShopSellConfigList(POINTSHOP_GOODSTYPE_SPECIAL, listSellConfig);
	if(listSellConfig.empty())
		return 0;

	const SPointShopGoodsConfig_S* pRenameConfig = NULL;
	for (int i = 0; i <	listSellConfig.size(); ++i)
	{
		if (listSellConfig[i]->nConsumpApproach == (int)CONSUMP_APPROACH_ACTOR_RENAME)
		{
			pRenameConfig = listSellConfig[i];
			break;
		}
	}
	if (pRenameConfig == NULL)
		return 0;

	msg_entity_pointshop_buygoods msgInfo;
	char szMoneyType[32]= {0};
	if(nMoneyType ==  (int)ENPOINTSHOPRESTYPE_POINT)
	{
		msgInfo.nClientPrice = pRenameConfig->nPointPrice;
		sprintf(szMoneyType, "%s", a2utf8("点券"));
	}
	else if(nMoneyType ==  (int)ENPOINTSHOPRESTYPE_MONEY)
	{
		msgInfo.nClientPrice = pRenameConfig->nMoneyPrice;
		sprintf(szMoneyType, "%s", a2utf8("金币"));
	}
	else 
	{
		return 0;
	}

	msgInfo.nSellID = pRenameConfig->nSellID;
	msgInfo.nResType = nMoneyType;
	msgInfo.nNum = 1;

	if(!m_pointShopMgr.onMessagePointShopBuyGoods((void*)&msgInfo, sizeof(msgInfo)))
	{
		return 0;
	}

	// 记录文字日志
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (NULL != pOSSLogService)
	{
		char szText[512] = {0};
		sprintf_s(szText, sizeof(szText), a2utf8("%s(%d)改名为%s,消耗%s%d"), &m_playInfo.player.szActorName, m_playInfo.player.dwActorID, pNewName, szMoneyType, msgInfo.nClientPrice);
		pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, m_playInfo.player.dwActorID, szText);
	}

	nRenameMoneyCache = msgInfo.nClientPrice;
	memset(m_cRenameCache,0,DBDEF_ACTORNAME_MAXSIZE);
	sstrcpyn(m_cRenameCache, pNewName, sizeof(m_cRenameCache));

	// 点券的方式特殊：需要数据库返回的点券消费成功后才能更新名字的修改
	if(nMoneyType ==  ENPOINTSHOPRESTYPE_POINT)
	{
		return 2;
	}

	// 金币的方式：目前没有对金币消费的数据库查询返回 所以先直接更新名字的修改
	if(!updateActorNameToDBAndCache((int)ENPOINTSHOPRESTYPE_MONEY, pRenameConfig->nMoneyPrice))
	{
		return 0;
	}

	return 1;
}

void CPlayerBankPart::onDBActorServiceCheckRenameFlagFail( const char * pszErrorDesc )
{
	g_EHelper.showSystemMessage(m_pMaster,(CHAT_TIP_ENTITY_BEGIN),( LPCSTR)pszErrorDesc,CHAT_BROADCASTTYPE_SINGLE);
	sendRenameResultToClient(false);
}

bool CPlayerBankPart::canAddBenefitCard( int nBenefitCardID,int nNum)
{
	return m_beneficardMgr.canAddBenefitCard(nBenefitCardID, nNum);
}

bool CPlayerBankPart::addBenefitCard( int nBenefitCardID,int nNum)
{
	return m_beneficardMgr.addBenefitCard(nBenefitCardID, nNum);
}

void CPlayerBankPart::calcBenefitCardMultiple( int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple )
{
	return m_beneficardMgr.calcBenefitCardMultiple(nGoldMultiple, nActorExpMultiple, nHeroExpMultiple);
}

void CPlayerBankPart::onMessageOpenGemstoneChest(int nChestType)
{
	m_crownPageMgr.onMessageOpenGemstoneChest(nChestType);
}

void CPlayerBankPart::setProxyCrownPageEffect(int *propertyList, int timeToAddEffect)
{
	m_talentEffectProxy.AddCrownEffect(m_pMaster,propertyList, (ETIME_TO_ADD_EFFECT)timeToAddEffect);
}

void CPlayerBankPart::updateChestInfoToClient()
{
	m_actorPrizeMgr.updateChestInfoToClient();
}

bool CPlayerBankPart::canAddHeroXPSkill(int nHeroID,int nXPSkillID)
{
	return m_heroMgr.canAddHeroXPSkill(nHeroID, nXPSkillID);
}

bool CPlayerBankPart::addHeroXPSkill(int nHeroID,int nXPSkillID)
{
	return m_heroMgr.addHeroXPSkill(nHeroID, nXPSkillID);
}

bool CPlayerBankPart::canAddCrownGemstone(int nGemstoneID, BYTE byGrade,int nCount)
{	
	return  m_crownPageMgr.canAddCrownGemstone(nGemstoneID, byGrade, nCount);
}

bool CPlayerBankPart::addCrownGemstone(int nGemstoneID,int nCount, BYTE byGrade, int nExp, bool bAppraisal)
{
	return m_crownPageMgr.addCrownGemstone(nGemstoneID, nCount, byGrade, nExp, bAppraisal);
}

// 能否增加页面
bool CPlayerBankPart::canAddPage(DWORD nNum/*=1*/)
{
	return m_crownPageMgr.canAddPage(nNum);
}
// 创建新页面
bool CPlayerBankPart::createNewPage(DWORD nNum/*=1*/)
{
	return m_crownPageMgr.createNewPage(nNum);
}

DWORD CPlayerBankPart::getPKTotalNum()
{
    return m_playInfo.player.dwPKTotalNum;
}

int CPlayerBankPart::getXPSKillID(int nHeroID)
{
    SHeroInfo* pHeroInfo = m_heroMgr.getHeroInfo(nHeroID);
    if(pHeroInfo == NULL)
    {
        return 0;
    }
    return pHeroInfo->nUseXPID;
}

DWORD CPlayerBankPart::getTotalMatchNum()
{
    DWORD dwTotalMatchNum = 0;
	MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.begin();
    for (;iter != m_mapMatchTypeRank.end(); ++iter)
    {
        dwTotalMatchNum += iter->second.dwMatchNum;
    }
    return dwTotalMatchNum;
}


// DB存价值物品信息
bool CPlayerBankPart::saveValueGoodsInfoToDB(BYTE byType, DWORD dwGoodsID, DWORD dwGoodsCount)
{
	return m_valueGoodsHelper.saveValueGoodsInfoToDB(byType, dwGoodsID, dwGoodsCount);
}

// DB读价值物品信息
bool CPlayerBankPart::readValueGoodsInfoFromDB(BYTE byType)
{
	return m_valueGoodsHelper.readValueGoodsInfoFromDB(byType);
}

// DB读宝箱结果
void CPlayerBankPart::onDBReturnReadChestInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods)
{
	m_actorPrizeMgr.onDBReturnReadChestInfo(pvecGoods);
}

// 是否可以添加表情包
bool CPlayerBankPart::canAddEmoji(BYTE byEmojiType)
{
	return m_emojiMgr.canAddEmoji(byEmojiType);
}

// 添加表情包
bool CPlayerBankPart::addEmoji(BYTE byEmojiType)
{
	return m_emojiMgr.addEmoji(byEmojiType);
}

// DB读表情包结果
void CPlayerBankPart::onDBReturnReadEmojiInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods)
{
	m_emojiMgr.onDBReturnEmojiInfo(pvecGoods);
}


bool CPlayerBankPart::getAllMatchTypeRank(obuf & outData)
{
    int nCount = m_mapMatchTypeRank.size();
    outData<<nCount;
    for (MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.begin(); iter != m_mapMatchTypeRank.end(); ++iter)
    {
        outData<<iter->second;
    }

    return true;
}

DWORD CPlayerBankPart::getMatchTypeHideRank(EMMatchType eMatchType)
{
    MATCHTYPERANK_MAP::iterator iter = m_mapMatchTypeRank.find(eMatchType);
    if (iter == m_mapMatchTypeRank.end() || iter->second.nHideRankScore == 0)
    {
        // 获得配置信息
        ISchemeMatchRankConfigInfo * pSchemeRankInfo = gServerGlobal->getSchemeCenter()->getSchemeMatchRankConfig();
        if(pSchemeRankInfo == NULL)
        {
            ErrorLn(__FUNCTION__": pSchemeRankInfo == NULL");
            return 0;
        }

        ISchemeEntity *pSchemeEntity = gServerGlobal->getSchemeCenter()->getSchemeEntity();
        if (pSchemeEntity == NULL)
        {
            ErrorLn(__FUNCTION__": pSchemeEntity == NULL");
            return 0;
        }
        // 取得显示打印信息
        return pSchemeEntity->getEntityXml()->nMatchTypeBaseScore;
    }

    return iter->second.nHideRankScore;
}


extern "C" __declspec(dllexport) __IBankPart * CreateBankPart()
{
	return new CPlayerBankPart;
}