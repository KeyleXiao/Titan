/*******************************************************************
** 文件名:	PlayerBankPart.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015/8/27
** 版  本:	1.0
** 描  述:	实体数据仓库部件

********************************************************************/

#include "stdafx.h"
#include "PlayerBankPart.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "IEntityProperty.h"
#include "EntityDef.h"
#include "MatchSchemeDef.h"
#include "ISchemeCenter.h"
#include "IGameDatabaseDef.h"
#include "SchemeDef.h"
#include "ILoginServerProtocol.h"
#include "SpellDef.h"
#include "ISpell.h"
#include "IChatClient.h"
#include "War_ManagedDef.h"
#include "IMatchClient.h"

////////////////////* CPlayerBankPart */////////////////////////////////////////////
CPlayerBankPart::CPlayerBankPart():m_pMaster(NULL),  m_nPlayerGuideTypeData(ENNOVICEGUIDE_TYPE_GUID_NEWPLAYER)
{
	m_nLifeHeroTaskData = 0; // 默认没有完成本命英雄引导过程
}

CPlayerBankPart::~CPlayerBankPart()
{

}

////////////////////IEntityPart/////////////////////////////////////////////
bool CPlayerBankPart::onLoad( IEntity* pEntity)
{
	if(pEntity == NULL)
		return false;

    m_pMaster = pEntity;
    m_heroMgr.onLoad(pEntity);
	m_crownPageMgr.onLoad(pEntity);
	m_CommonMgr.onLoad(pEntity);

    //todo
    if ( m_pMaster->isHero())
    {
        IEntityProperty *pProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
        if (!IsDynamicScene(pProperty->getProperty_Integer(PROPERTY_SCENEID)))
        {
            msg_entity_matchtype_rank RankData;
            RankData.bRequestSelf = true;
            // 人物撮合段位信息
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_MATCHTYP_RANK, (char *)&RankData, sizeof(msg_entity_matchtype_rank));
        }
    }

    return true;
}

void CPlayerBankPart::release()
{
    delete this;
}

short CPlayerBankPart::getPartID()
{
    return PART_BANK;
}

IEntity * CPlayerBankPart::getHost()
{
    return m_pMaster;
}

bool CPlayerBankPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch( msgId )
    {
    case SC_MSG_ENTITY_MATCHTYP_RANK:
        {
            OnMsgMatchTypeRank((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_MATCH_HEROSCORE:
        {
            OnMsgMatchHeroScore((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_MATCHTYP_SINGLE_RANK:
        {
            OnMsgMatchSingleTypeRank((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_MATCH_SINGLE_HEROSCORE:
        {
            OnMsgMatchSingleHeroScore((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_TALENTPAGEPOINTS_ALL:
        {
            // 请求所有天赋页数据
            OnMsgTalentPageInfoAll((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_RUNE_SYNC:
        {
            OnMsgRuneInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_OTHERRUNE_SYNC:
        {		
            OnMsgOtherPlayerRuneInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_RETURN_PLAYER_INFO:
        {
            OnMsgMatchReturnPlayerInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_RETURN_HERO_INFO:
        {
            OnMsgMatchReturnHeroInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_UPDATE_HERO_INFO:
        {
            OnMsgUpdateHeroInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_UPDATE_PLAYER_INFO:
        {
            OnMsgUpdatePlayerInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_UPDATE_HERO_CARD:
        {
            OnMsgUpdateHeroCard((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_UPGRADE_STAR_SUCESS:
        {
            OnMsgUpgradeStarSucess((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_WARENDAWARK_RESULT:
        {
            onMsgWarEndAwarkResult((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_STATICHERO_UPGRADE:
        {
            onMsgStaticHeroUpgrade((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_LOGIN_SENDDATA:
        {
            onMsgLoginRecvData((LPCSTR)data, len);
        }
        break;

    case SC_MSG_ENTITY_HEROTALENT_INFO:
        {
            OnMsgHeroTalentInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_HEROTALENT_UPTATE:
        {
            OnMsgUpdateActiveHeroTalentInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_RUNESTORE_UPTATE:
        {
            OnMsgUpdateActiveRuneStoreInfo((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_UPDATE_CHEST_INFO:
        {
            OnMsgUpdateChestInfo((LPCSTR)data, len);
        }
        break;
	case SC_MSG_ENTITY_UPDATE_EMOJI_INFO:
		{
			OnMsgUpdateEmojiInfo((LPCSTR)data, len);
		}
		break;
    case SC_MSG_ENTITY_OPENCHEST_RESULT:
        {
            OnMsgOpenChestResult((LPCSTR)data, len);
        }
        break;
	case SC_MSG_ENTITY_RETURN_CHEST_RANDCARD_INFO:
		{
			OnMsgReturnChestRandCardInfo((LPCSTR)data, len);
		}
		break;
    case SC_MSG_ENTITY_HEROSKIN_INFO:
        {
            OnMsgHeroSkin((LPCSTR)data, len);
        }
        break;
    case SC_MSG_ENTITY_HEROSKIN_UPTATE:
        {
            OnMsgUpdateHeroSkin((LPCSTR)data, len);
        }
        break;
	case SC_MSG_ENTITY_POINTSHOP_UPDATE_BOUGHTNUM:
		{
			OnMsgPointshopUpdateBoughtNum((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_POINTSHOP_BUYRESULT:
		{
			OnMsgPointshopBuyResult((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_POINTSHOP_UPDATE_CARDLIST:
		{
			OnMsgPointshopUpdateCardList((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_POINTSHOP_REFLUSH_BOUGHTNUM:
		{
			OnMsgPointshopReflushBoughtNum();
		}
		break;
	case SC_MSG_ENTITY_RENMAE_RESULT:
		{
			onMsgActorRenameResult((LPCSTR)data, len);
		}
		break;	
	case SC_MSG_ENTITY_BENEFITCARD_INFO:
		{
			onMsgBenefitCardInfo((LPCSTR)data, len);
		}
		break;
	case	SC_MSG_ENTITY_CROWNALLGEMSTONE_UPDATE:					// 初始数据全部宝石更新
		{
			m_crownPageMgr.onMsgCrownAllGemstoneItem((LPCSTR)data, len);
		}
		break;
	case	SC_MSG_ENTITY_CROWNALLCROWNPAGE_UPDATE:					// 初始数据全部皇冠页更新
		{
			m_crownPageMgr.onMsgCrownAllCrownPage((LPCSTR)data, len);
		}
		break;
	case	SC_MSG_ENTITY_CROWNGEMSTONE_UPDATE:				    	// 宝石变化同步
		{
			m_crownPageMgr.onMsgUpdateCrownGemstoneItem((LPCSTR)data, len);
		}
		break;
	case	SC_MSG_ENTITY_CROWNGEMSTONE_PRODUCE:					// 宝石产出同步
		{
			m_crownPageMgr.onMsgCrownGemstoneProduceInfo((LPCSTR)data, len);
		}
		break;
	case	SC_MSG_ENTITY_HERO_XP_SKILL_UPDATE:					// XP技能同步
		{
			m_heroMgr.onMsgUpdateHeroXPSkillInfo((LPCSTR)data, len);
		}
		break;
	case	SC_MSG_ENTITY_HERO_XP_FRAGMENT_UPDATE:				// XP技能碎片同步
		{
			m_heroMgr.onMsgUpdateHeroXPFragementInfo((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_CROWNGEMSTONE_APPRAISAL_RET:				// 宝石鉴定结果
		{
			m_crownPageMgr.onMsgAppraisalCrownGemstoneRet((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_HEROUPSTAR_UNLOCK:			// 英雄解锁弹出奖励界面
		{
			OnMsgHeroUpStarUnlockPrize((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_NOVICEGUIDE_DATA:
		{
			onMsgNoviceGuideData((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_LABEL_LIST:
		{
			m_CommonMgr.onMessage(msgId, (LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_LIFEHERO_QUERY_RESULT:
	case SC_MSG_ENTITY_LIFEHERO_UPDATE:
		{
			m_heroMgr.onMessage(msgId, data, len);
		}
		break;
	case SC_MSG_ENTITY_LIFEHERO_TASKDATA:
		{
			onMsgLifeHeroTaskData((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_LIFEHERO_PRIZE_TASKDATA:
		{
			onMsgLifeHeroPrizeTaskData((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_ACTOR_UNLOCK_RESULT:
		{
			onMsgActorUnlockData((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_RET_SEASON_DETAIL:
		{
			onMsgRankSeasonDetail((LPCSTR)data, len);
		}
		break;
	case SC_MSG_ENTITY_RET_SEASON_RECORD:
		{
			onMsgRankSeasonRecord((LPCSTR)data, len);
		}
		break;
	default:
        break;
    }
    return true;
}

// 显示层的命令处理
bool CPlayerBankPart::onCommand( int cmdid,void * data,size_t len )
{
    switch( cmdid )
    {
        // 请求匹配战绩相关记录信息
    case ENTITY_CMD_MATCHTYPE_RANK_RECORD:						
        {
            if (len != sizeof(cmd_entity_matchtype_rank))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_entity_matchtype_rank)");
                return false;
            }

            // 发出请求信息
            cmd_entity_matchtype_rank *pMsg = (cmd_entity_matchtype_rank *)data;
            msg_entity_matchtype_rank RankData; 
            if (pMsg->RequestFriendID == INVALID_PDBID)
            {
                RankData.bRequestSelf = true;
            }

            RankData.dwFriendID = pMsg->RequestFriendID;
            // 人物撮合段位信息
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_MATCHTYP_RANK, (char *)&RankData, sizeof(msg_entity_matchtype_rank));

            return true;
        }
        break;
        // 请求英雄熟练度数据记录信息
    case ENTITY_CMD_MATCH_HEROSCORE_RECORD:						
        {
            if (len != sizeof(cmd_entity_match_heroscore))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_entity_match_heroscore)");
                return false;
            }
            // 发出请求信息
            cmd_entity_match_heroscore *pMsg = (cmd_entity_match_heroscore *)data;

            msg_entity_match_heroscore HeroScoreData; 
            if (pMsg->RequestFriendID == INVALID_PDBID)
            {
                HeroScoreData.bRequestSelf = true;
            }

            HeroScoreData.dwFriendID = pMsg->RequestFriendID;

            // 人物英雄熟练度
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_MATCH_HEROSCORE, (char *)&HeroScoreData, sizeof(msg_entity_match_heroscore));

            return true;
        }
        break;

        // 广播召唤师技能信息
    case ENTITY_CMD_SUMMONER_BROAD_INFO:
        {
            if ( len != sizeof(cmd_SmSillInfo_Return))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_SmSillInfo_Return), len = " << len << ", sizeof(cmd_SmSillInfo_Return) = " << sizeof(cmd_SmSillInfo_Return));
                return false;
            }
            cmd_SmSillInfo_Return* pCmdInfo = (cmd_SmSillInfo_Return*)data;
            SMsgSummoner_Skill MsgData;
            memcpy(MsgData.nSummonerSkill, pCmdInfo->nAllSoltSkillId, sizeof(MsgData.nSummonerSkill));

            obuf256 obMsg;
            SNetMsgHead MsgHead;
            MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
            MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
            MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
            MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

            SMsgMatchDataSubMsg outData;
            outData.dwMsgCode	= CS_MSG_SUMMONER_SKILL;						// 子消息代码  EMRoomMessageCode

            obMsg << MsgHead<<outData;
            obMsg.push_back(&MsgData, sizeof(SMsgSummoner_Skill));
            net_senddata( obMsg.data(), obMsg.size() );

            return true;
        }
        break;

    case ENTITY_CMD_SAVE_TALENT_PAGE_SINGLE:  // 发送保存天赋页的信息
        {
#ifdef  FUNCTION_TALENTPAGE_OPEN
			if ( len != sizeof(cmd_TalentPage_Points_Single))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_TalentPage_Points_Single), len = " << len << ", sizeof(cmd_TalentPage_Points_Single) = " << sizeof(cmd_TalentPage_Points_Single));
                return false;
            }

            cmd_TalentPage_Points_Single* pCmdInfo = (cmd_TalentPage_Points_Single*)data;
            int nCurPageIndex=pCmdInfo->nCurPageIndex;
            int nTalentPageNum=pCmdInfo->nTalentNum;

            // 缓存数据
            TALENTPAGEINFO talentPageInfo;
            talentPageInfo.talentBaseInfo.nCurPageIndex=nCurPageIndex;
            talentPageInfo.talentBaseInfo.nPageTalentNum=nTalentPageNum;
            sstrcpyn(talentPageInfo.talentBaseInfo.szTalentPageName,pCmdInfo->szTalentPageName,sizeof(talentPageInfo.talentBaseInfo));

            // 服务器数据
            msg_Entity_TalentPage_Points_Set_Single MsgData;
            MsgData.nCurPageIndex=nCurPageIndex;
            MsgData.nPageTalentNum=nTalentPageNum;
            sstrcpyn(MsgData.szTalentPageName,pCmdInfo->szTalentPageName,sizeof(MsgData.szTalentPageName));

            obuf obdata;
            obdata.push_back(&MsgData,sizeof(MsgData));

            for(int i=0;i<nTalentPageNum;i++)
            {
                TALENT_INFO node;
                node.byTalentID = (BYTE)pCmdInfo->nTalentId[i];
                node.byPoints   = (BYTE)pCmdInfo->nTalentPoints[i];

                obdata.push_back(&node, sizeof(node));
                talentPageInfo.vTalentInfoVec.push_back(node);
            }

            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_TALENTPAGEPOINTS_SET_SINGLE, obdata.data(), obdata.size());
#endif
            return true;
        }
        break;
        // 请求所有天赋点
    case ENTITY_CMD_REQUEST_TALENT_PAGE_ALL:
        {
            if (len != sizeof(cmd_Entity_Request_Talent_Page))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_Entity_Request_Talent_Page)");
                return false;
            }
            // 发出请求信息
            cmd_Entity_Request_Talent_Page *pMsg = (cmd_Entity_Request_Talent_Page *)data;
            if(pMsg->bIsRequestSelf==true)
            {
                // 请求自己直接从缓存中读取发
                SendMyTalentPageInfoAllToView();
            }
            //请求别人先不做处理
            return true;

        }
        break;
        // 发送当前选定的页数index
    case ENTITY_CMD_BEGIN_TALENT_PAGE_EFFECT:
        {
#ifdef  FUNCTION_TALENTPAGE_OPEN
			if (len != sizeof(cmd_Entity_Talent_Start_Effect))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_Entity_Talent_Start_Effect)");
                return false;
            }
            // 发出请求信息
            cmd_Entity_Talent_Start_Effect *pMsg = (cmd_Entity_Talent_Start_Effect *)data;
            msg_Entity_TalentPage_Index_Effect TalentPageEffect;

            TalentPageEffect.nCurPageIndex=pMsg->nCurTalentPage;

            //// 请求服务器数据
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_EFFECT, (char *)&TalentPageEffect, sizeof(msg_Entity_TalentPage_Index_Effect));
#endif
            return true;
        }
        break;
    case ENTITY_CMD_BEGIN_TALENT_PAGE_DELETE:
        {
#ifdef  FUNCTION_TALENTPAGE_OPEN
			if (len != sizeof(cmd_Entity_Talent_Page_Detele))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_Entity_Talent_Page_Detele)");
                return false;
            }
            // 发出请求信息
            cmd_Entity_Talent_Page_Detele *pMsg = (cmd_Entity_Talent_Page_Detele *)data;

            msg_Entity_TalentPage_Index_Delete TalentPageDelete;
            TalentPageDelete.nCurPageIndex=pMsg->nCurTalentPage;
            // 请求服务器数据
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_DELETE, (char *)&TalentPageDelete, sizeof(msg_Entity_TalentPage_Index_Delete));
#endif
            return true;
        }
        break;
    case  ENTITY_CMD_QUERY_RUNE:
        {
            if (len != sizeof(cmd_Request_Rune_Info))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_Request_Rune_Info)");
                return false;
            }

            // 请求自己直接从缓存中读取发
            // SendRunePageInfoAllToView();

            return true;
        }
        break;
        // 保存设置好的符文配置信息,同时同步到服务器上
    case ENTITY_CMD_SAVE_RUNE_PAGE:
        {
#ifdef  FUNCTION_RUNE_OPEN
			if (len != sizeof(cmd_Rune_Single_PageInfo))
            {
                ErrorLn(__FUNCTION__ << ": len != sizeof(cmd_rune_change)");
                return false;
            }

            // 校验符文页
            cmd_Rune_Single_PageInfo *pRuneData = reinterpret_cast<cmd_Rune_Single_PageInfo *>(data);
            int nPageIndex = pRuneData->nPageIndex;

            if(m_mRunePage.find(nPageIndex) == m_mRunePage.end()) //缓存起来 
            {
                Entity_Rune_Page_Info RunePage;
                m_mRunePage[nPageIndex] = RunePage;
            }
            // 准备发送要保存的符文信息
            msg_runePage_save pMsgData;
            pMsgData.nRunePageIdx = nPageIndex;
            m_mRunePage[nPageIndex].nPageIdx = nPageIndex;

            pMsgData.nRuneNum = pRuneData->nRuneNum;
            m_mRunePage[nPageIndex].nRuneNum = pRuneData->nRuneNum;

            sstrcpyn( pMsgData.szRunePageName, pRuneData->sRunePageName, sizeof( pMsgData.szRunePageName));
            sstrcpyn(m_mRunePage[nPageIndex].szRunePageName, pRuneData->sRunePageName, sizeof(m_mRunePage[nPageIndex].szRunePageName));
            int nRuneGrooveLen  = sizeof(m_mRunePage[nPageIndex].nRuneGroove);
            memcpy(m_mRunePage[nPageIndex].nRuneGroove,pRuneData->nRuneGroove,nRuneGrooveLen);

            obuf oBuffer;
            oBuffer.push_back(&pMsgData, sizeof(pMsgData));

            if(pMsgData.nRuneNum!=0)
            {
                // 替换数据仓库部件中的改变的符文页
                for (int i = 0; i < RUNE_GROOVE_MAX; ++i)
                {
                    if(pRuneData->nRuneGroove[i]==0)
                    {
                        continue;
                    }

                    DBREQ_PARAM_RUNE_INFO pRuneInfo;
                    pRuneInfo.nID = pRuneData->nRuneGroove[i];
                    pRuneInfo.nPostion = i;
                    oBuffer.push_back(&pRuneInfo, sizeof(pRuneInfo));
                }
            } 

            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_RUNE_SAVE, oBuffer.data(), oBuffer.size());
#endif
            return true;
        }
        break;

        // 删除指定符文页
    case ENTITY_CMD_DELETE_RUNE_PAGE:
        {
#ifdef  FUNCTION_RUNE_OPEN
			if (len != sizeof(cmd_Delete_Rune_Page))
            {
                ErrorLn(__FUNCTION__ << ": len != sizeof(cmd_rune_change)");
                return false;
            }

            // 校验符文页
            cmd_Delete_Rune_Page *pRuneInfo = reinterpret_cast<cmd_Delete_Rune_Page *>(data);

            int nPageIndex = pRuneInfo->nPageIdx;
            if(m_mRunePage.find(nPageIndex) != m_mRunePage.end())  
            {
                m_mRunePage.erase(nPageIndex);
            }
            // 准备发送要删除的符文信息
            msg_runePage_delete pMsgData;
            pMsgData.nPageIdx = pRuneInfo->nPageIdx;
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_RUNE_DELETE, reinterpret_cast<char *>(&pMsgData), sizeof(msg_runePage_delete));
#endif
            return true;
        }
        break;
        // 发送当前选定的页数index
    case ENTITY_CMD_BEGIN_RUNE_PAGE_EFFECT:
        {
#ifdef  FUNCTION_RUNE_OPEN
			if (len != sizeof(cmd_Effect_Rune_Page))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_Entity_Talent_Start_Effect)");
                return false;
            }
            // 发出请求信息
            cmd_Effect_Rune_Page *pMsg = (cmd_Effect_Rune_Page *)data;
            msg_Entity_RunePage_Index_Effect RunePageEffect;

            RunePageEffect.nCurPageIndex=pMsg->nPageIdx;

            //// 请求服务器数据
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_RUNE_INDEX_EFFECT, (char *)&RunePageEffect, sizeof(msg_Entity_RunePage_Index_Effect));
#endif
            return true;
        }
        break;

    case ENTITY_CMD_REQ_PLAYER_BASE_INFO:
        {
            if (len != sizeof(cmd_Request_Player_Info))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_Request_Player_Info)");
                return false;
            }

			cmd_Request_Player_Info *pMsg = (cmd_Request_Player_Info *)data;
			msg_req_player_info PlayerInfoRequest;
			PlayerInfoRequest.dwActorID = pMsg->nActorID;
            PlayerInfoRequest.bFake = pMsg->bFake;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_REQ_PLAYER_INFO, (char *)&PlayerInfoRequest, sizeof(msg_req_player_info));

            return true;
        }
        break;

    case ENTITY_CMD_REQ_UPGRADE_STAR:
        {
            if (len != sizeof(int))
            {
                ErrorLn(__FUNCTION__": len != sizeof(int) nLen = " << len);
                return false;
            }
            int nHeroID = *(int*)data;

            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_REQ_UPDATE_HERO_STAR, &nHeroID, sizeof(nHeroID));

            return true;
        }
        break;

    case ENTITY_CMD_REQ_UPGRADESTARANDHEROPRO:
        {
            if ( len != sizeof(cmd_get_hero_detail_info))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_get_hero_detail_info) nLen = " << len);
                return false;
            }

			cmd_get_hero_detail_info * pDetail = (cmd_get_hero_detail_info *)data;
            sendHeroInfoUpgradeFrontAndBack(pDetail->nHeroID, pDetail->nStarLv, 0, 0, pDetail->nStaticLv);

            return true;
        }
        break;

    case ENTITY_CMD_REQ_CHANGEHERO:
        {
            if ( len != sizeof(cmd_entity_change_hero))
            {
                ErrorLn(__FUNCTION__": len != sizeof(int) nLen = " << len);
                return false;
            }
            cmd_entity_change_hero* pcmdInfo = (cmd_entity_change_hero*)data;

            msg_entity_change_hero msgInfo;
            msgInfo.nOptType    = pcmdInfo->nOptType;       // 当前操作类型:0.切英雄（带皮肤ID），1.切皮肤(带英雄ID)
            msgInfo.nChangeID   = pcmdInfo->nChangeID;      // 英雄/皮肤ID
            msgInfo.nReserveData= pcmdInfo->nReserveData;   // 备用数据(切英雄时，此值为皮肤ID)

            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CHANGE_HERO, &msgInfo, sizeof(msgInfo));
        }
        break;

	case ENTITY_CMD_REQ_HERO_INFO:
		{
		    if (len != sizeof(cmd_Request_Hero_Info))
		    {
		        ErrorLn(__FUNCTION__": len != sizeof(cmd_Request_Hero_Info)");
		        return false;
		    }

		    // 请求别人,请求服务器数据
		    cmd_Request_Hero_Info *pMsg = (cmd_Request_Hero_Info *)data;
		    msg_req_hero_info HeroInfoRequest;
		    HeroInfoRequest.dwActorID = pMsg->nActorID;
            HeroInfoRequest.bFake = pMsg->bFake;
		    SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_REQ_HERO_INFO, (char *)&HeroInfoRequest, sizeof(msg_req_hero_info));

		    return true;
		}
		break;

    case ENTITY_CMD_QUERY_OTHER_RUNE:
        {
#ifdef  FUNCTION_RUNE_OPEN
			if (len != sizeof(cmd_Request_Rune_Info))
            {
                ErrorLn(__FUNCTION__": len != sizeof(cmd_Request_Rune_Info)");
                return false;
            }
            // 发出请求信息,请求服务器数据
            cmd_Request_Rune_Info *pMsg = (cmd_Request_Rune_Info *)data;
            msg_Request_Other_Rune_Points RuneInfoRequest;
            RuneInfoRequest.dwFriendID = pMsg->RequestFriendID;
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_RUNE_REQUEST_OTHER_INFO, (char *)&RuneInfoRequest, sizeof(msg_Request_Other_Rune_Points));
#endif
            return true;
        }
        break;

	case ENTITY_CMD_QUERY_HERO_DETAIL_INFO:
		{
			if (len != sizeof(cmd_Request_Hero_Detail_Info))
			{
				ErrorLn(__FUNCTION__": len != sizeof(cmd_Request_Hero_Detail_Info)");
				return false;
			}

			// 将cmd_entity_hero_info转换为cmd_entity_hero_detail_info
			SendHeroDetailInfoToView((LPCSTR)data, len);

			return true;
		}
		break;

    case ENTITY_CMD_REQ_OPENCHEST:
        {
            if (len != sizeof(cmd_entity_open_chest))
            {
                ErrorLn(__FUNCTION__": ENTITY_CMD_REQ_OPENCHEST nLen = " << len);
                return false;
            }

            cmd_entity_open_chest *pCmdInfo = (cmd_entity_open_chest*)data;

            msg_entity_open_chest msgInfo;
            msgInfo.nChestType = pCmdInfo->nChestType;
            SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_OPEN_CHEST, &msgInfo, sizeof(msgInfo));
        }
        break;

	case ENTITY_CMD_REQ_BROADCAST_CHEST_RESULT:
		{
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_BROADCAST_CHEST_RESULT, NULL, 0);
		}
		break;

	case ENTITY_CMD_POINTSHOP_BUYGOODS:
		{
			if (len != sizeof(cmd_entity_pointshop_buygoods))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_POINTSHOP_BUYGOODS nLen = " << len);
				return false;
			}

			cmd_entity_pointshop_buygoods *pCmdInfo = (cmd_entity_pointshop_buygoods*)data;

			msg_entity_pointshop_buygoods msgInfo;
			msgInfo.nSellID = pCmdInfo->nSellID;
			msgInfo.nResType = pCmdInfo->nResType;
			msgInfo.nClientPrice = pCmdInfo->nClientPrice;
			msgInfo.nNum = pCmdInfo->nBuyNum;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_POINTSHOP_BUYGOODS, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_ACTOR_NAME_FIX:
		{
			if (len != sizeof(cmd_Actor_Name_Fix))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ACTOR_NAME_FIX nLen = " << len);
				return false;
			}

			cmd_Actor_Name_Fix *pCmdInfo = (cmd_Actor_Name_Fix*)data;

			msg_ActorName_Change msgInfo;
			sstrcpyn( msgInfo.szActorName,pCmdInfo->szNewName, sizeof(msgInfo.szActorName));
			msgInfo.nCostType = pCmdInfo->nCostType;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_ACTORNAME_FIX, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_CROWN_AGEMSTONEPPRAISAL:
		{
			if (len != sizeof(cmd_Crown_Gemstone_Appraisal_Info))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ACTOR_NAME_FIX nLen = " << len);
				return false;
			}

			cmd_Crown_Gemstone_Appraisal_Info *pCmdInfo = (cmd_Crown_Gemstone_Appraisal_Info*)data;

			Msg_Appraisal_Crown_Gemstone msgInfo;
			memcpy(msgInfo.gemstoneDBIDList,pCmdInfo->gemstoneDBIDList,sizeof(msgInfo.gemstoneDBIDList));
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNAGEMSTONEPPRAISAL, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_CROWN_SAVEPAGE:
		{
			if (len != sizeof(cmd_Crown_Page_Item_Info))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ACTOR_NAME_FIX nLen = " << len);
				return false;
			}

			cmd_Crown_Page_Item_Info *pCmdInfo = (cmd_Crown_Page_Item_Info*)data;

			Msg_CrownPage_Save msgInfo;
			msgInfo.nPageDBID  = pCmdInfo->nPageDBID;
			sstrcpyn( msgInfo.szCrownPageName,pCmdInfo->szName, sizeof(msgInfo.szCrownPageName));
			memcpy(msgInfo.crownInlaySlotList,pCmdInfo->crownInlaySlotList,sizeof(msgInfo.crownInlaySlotList));
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNSAVEPAGE, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_CROWN_DELETEPAGE:
		{
			if (len != sizeof(cmd_Crown_Page_Delete_Info))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ACTOR_NAME_FIX nLen = " << len);
				return false;
			}

			cmd_Crown_Page_Delete_Info *pCmdInfo = (cmd_Crown_Page_Delete_Info*)data;

			Msg_CrownPage_Delete msgInfo;
			msgInfo.nPageDBID  = pCmdInfo->nPageDBID;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNDELETEPAGE, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_CROWN_ROOMSELECTPAGE:
		{
			int *pPageDBID = (int*)data;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNDROOMSELECTPAGE, pPageDBID, sizeof(int));
		}
		break;
	case ENTITY_CMD_CROWN_DCOBBLESTON_SELECTED:
		{
			if (len != sizeof(cmd_Crown_Cobbleston_Selected))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ACTOR_NAME_FIX nLen = " << len);
				return false;
			}

			cmd_Crown_Cobbleston_Selected *pCmdInfo = (cmd_Crown_Cobbleston_Selected*)data;

			Msg_Cobbleston_Selected_Crown_Gemstone msgInfo;
			msgInfo.nGemstoneID  = pCmdInfo->nGemstoneID;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNDCOBBLESTON_SELECTED, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_CROWN_DWINDOWSTON:
		{	
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNDWINDOWSTONE, NULL, 0);
		}
		break;
	case ENTITY_CMD_CROWN_DWINDOWSTONE_SELECTED:
		{
			if (len != sizeof(cmd_Crown_WindowStone_Selected))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ACTOR_NAME_FIX nLen = " << len);
				return false;
			}

			cmd_Crown_WindowStone_Selected *pCmdInfo = (cmd_Crown_WindowStone_Selected*)data;

			Msg_WindowStone_Selected_Crown_Gemstone msgInfo;
			msgInfo.nGemstoneID  = pCmdInfo->nGemstoneID;
			msgInfo.nSlotID = pCmdInfo->nSlotID;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNDWINDOWSTONE_SELECTED, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_CROWN_DGEMSTONE_SEIL:
		{
			if (len != sizeof(cmd_Crown_Gemstone_Seil))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ACTOR_NAME_FIX nLen = " << len);
				return false;
			}

			cmd_Crown_Gemstone_Seil *pCmdInfo = (cmd_Crown_Gemstone_Seil*)data;

			Msg_Seil_Crown_Gemstone msgInfo;
	
			msgInfo.nGemstoneDBID = pCmdInfo->nGemstoneDBID;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNDGEMSTONE_SEIL, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_XP_SKILL_OPERATE_UNLOCK:
		{
			if (len != sizeof(cmd_Hero_XP_Skill_Info))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_XP_SKILL_OPERATE_UNLOCK nLen = " << len);
				return false;
			}

			cmd_Hero_XP_Skill_Info *pCmdInfo = (cmd_Hero_XP_Skill_Info*)data;

			Msg_Hero_XP_Skill_Unlock msgInfo;
			msgInfo.nXPSkillID = pCmdInfo->nXPSkillID;
			msgInfo.nHeroID = pCmdInfo->nHeroID;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_UNLOCK_XP_SKILL_BYFRAGMENT, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_XP_SKILL_OPERATE_SELECT:
		{
			if (len != sizeof(cmd_Hero_XP_Skill_Info))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_XP_SKILL_OPERATE_SELECT nLen = " << len);
				return false;
			}

			cmd_Hero_XP_Skill_Info *pCmdInfo = (cmd_Hero_XP_Skill_Info*)data;

			Msg_Hero_XP_Skill_Select msgInfo;
			msgInfo.nXPSkillID = pCmdInfo->nXPSkillID;
			msgInfo.nHeroID = pCmdInfo->nHeroID;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_SELECT_XP_SKILL, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_ENTITY_SET_NOVICEGUIDE:
		{
			if (len != sizeof(cmd_entity_set_noviceguide))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ENTITY_SET_NOVICEGUIDE nLen = " << len);
				return false;
			}

			cmd_entity_set_noviceguide *pCmdInfo = (cmd_entity_set_noviceguide*)data;

			msg_req_set_noviceguide msgInfo;
			msgInfo.dwNoviceGuideType = pCmdInfo->nNoviceGuideType;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_SET_NOVICEGUIDE, &msgInfo, sizeof(msgInfo));
		}
		break;
	case ENTITY_CMD_REFLUSH_CARD:
		{
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_REFLUSH_CARD, NULL, 0);
		}
		break;
	case ENTITY_CMD_LIFEHERO_ADD:
	case ENTITY_CMD_LIFEHERO_DEL:
	case ENTITY_CMD_LIFEHERO_REPLACE:
	case ENTITY_CMD_LIFEHERO_EXCHANGE:
	case ENTITY_CMD_LIFEHERO_QUERY:
	case ENTITY_CMD_LIFEHERO_OBTAIN_PRIZE:
		{
			return m_heroMgr.onCommand(cmdid, data, len);
		}
		break;
	case ENTITY_CMD_REQ_SEASON_DETAIL:
		{
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_REQ_SEASON_DETAIL, NULL, 0);
		}
		break;
	case ENTITY_CMD_REQ_SEASON_RECORD:
		{
			SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_REQ_SEASON_RECORD, NULL, 0);
		}
		break;
	default:
		break;
    }
    return false;
}

////////////////////ISerializable/////////////////////////////////////////////
bool CPlayerBankPart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
    in >> m_playInfo;
    if ( !in )
        return false;

#ifdef  FUNCTION_RUNE_OPEN
	m_mapRuneStore.clear();
    {
        int nRuneStoreLen = 0;		
        in >> nRuneStoreLen;

        int nRuneStoreCount = nRuneStoreLen/ sizeof(EntityHeroRuneStore);
        for(int i = 0; i < nRuneStoreCount; ++i)
        {
            if (!in.good())
            {
                return false;
            }

            EntityHeroRuneStore node;
            in >> node;

            m_mapRuneStore[node.nRuneId] = node;
        }
    }
#endif

#ifdef FUNCTION_MATCHTYPE_OPEN
    m_MatchTypeRank.clear();
    //m_heroMgr.onSerialize(in, m_playInfo.herocnt);

    // 人物段位信息
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

        m_MatchTypeRank[(EMMatchType)node.nMatchType] = node;
    }
#endif
    loadPlayerGlory();



    return true;
}

bool CPlayerBankPart::deSerialize( rkt::obuf & out,int flag )
{
    out << getPartID() << m_playInfo;

#ifdef  FUNCTION_RUNE_OPEN
    {
        int nRuneStoreItemLen =  sizeof(EntityHeroRuneStore);
        int nRuneLen = m_mapRuneStore.size()*nRuneStoreItemLen;

        out.push_back(&nRuneLen, sizeof(int));
        RUNESTORE_MAP::iterator iterRuneStore = m_mapRuneStore.begin();
        for (;iterRuneStore != m_mapRuneStore.end(); ++iterRuneStore)
        {
            out.push_back(&iterRuneStore->second, nRuneStoreItemLen);
        }
    }
#endif //  

    //m_heroMgr.deSerialize(out);

    // 人物段位信息
#ifdef FUNCTION_MATCHTYPE_OPEN
    int nCount = m_MatchTypeRank.size();
    out.push_back(&nCount, sizeof(int));
    MATCHTYPERANK::iterator iter = m_MatchTypeRank.begin();
    for (;iter != m_MatchTypeRank.end(); ++iter)
    {
        out.push_back(&iter->second, sizeof(MatchTypeRankNode));
    }
#endif

    return true;
}

////////////////////IBankPart/////////////////////////////////////////////
// 取得来源游戏世界ID
DWORD CPlayerBankPart::getWorldID(void)
{
    return m_playInfo.player.dwWorldID;
}

int CPlayerBankPart::getPlayerHeroArray(SHeroInfo* pArrHero, int nArrayCount)
{
    return m_heroMgr.getPlayerHeroArray(pArrHero, nArrayCount);
}

int CPlayerBankPart::getPlayerCanUseHeroArray(SHeroInfo* pArrHero, int nArrayCount)
{
	return m_heroMgr.getPlayerCanUseHeroArray(pArrHero, nArrayCount);
}

int* CPlayerBankPart::getSystemHeros(WORD& nCount)
{
    return m_heroMgr.getSystemHeros(nCount);
}

SHeroInfo*  CPlayerBankPart::getHeroInfoByID(int nHeroID)
{
    return m_heroMgr.getHeroInfoByID(nHeroID);
}

SHeroInfo* CPlayerBankPart::getPossessOrFreeHeroInfoByID(int nHeroID) 
{
	return m_heroMgr.getPossessOrFreeHeroInfoByID(nHeroID);
}

DBREQ_PARAM_HERO_CARD CPlayerBankPart::getHeroCardInfo( int nCardID )
{

    return m_heroMgr.getHeroCardInfo(nCardID);
}

SHeroStarSchemeInfo* CPlayerBankPart::getHeroCardInfoConfig(int nHeroID )
{
    SHeroInfo* pHeroInfo = getHeroInfoByID(nHeroID);
    if ( pHeroInfo == NULL )
    {
        return NULL;
    }
    int nStarLv = pHeroInfo->nHeroStar;


    ISchemeHeroStarData* pHeroStarInfo = gClientGlobal->getSchemeCenter()->getSchemeHeroStarInfo();
    if ( pHeroStarInfo == NULL )
    {
        return NULL;
    }
    return pHeroStarInfo->getHeroStarData(nHeroID, nStarLv);

}

const SPlayerDatabasesInfo* CPlayerBankPart::getPlayerDatabasesInfo()
{
    return &m_playInfo;
}


void CPlayerBankPart::OnMsgMatchTypeRank(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL)
    {
        return;
    }

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

    // 校验返回数据
    DBREQ_RESULT_MATCHTYPE_ALL_RANK * pData = (DBREQ_RESULT_MATCHTYPE_ALL_RANK *)pszMsg;
    if ( nLen != sizeof(*pData) + pData->nReCount * sizeof(MatchTypeRankNode))
    {
        ErrorLn(__FUNCTION__": pData length error !");
        return ;
    }

    m_MatchTypeRank.clear();
    // 解析当前返回数据
    MatchTypeRankNode* pDataHead = (MatchTypeRankNode *)(pData +1);
    for(int i = 0; i < pData->nReCount; ++i)
    {
        MatchTypeRankNode Data;
        Data.nGrade = pDataHead->nGrade;
        Data.nMatchType = pDataHead->nMatchType;
        Data.nRankScore = pDataHead->nRankScore;

        m_MatchTypeRank.insert(pair<EMMatchType,MatchTypeRankNode> ((EMMatchType)Data.nMatchType ,Data));
        pDataHead++;
    }

    ////发送到客户端显示
    //obuf obData;
    //cmd_count cmdCount;
    //cmdCount.nCount = m_MatchTypeRank.size();
    //obData << cmdCount;
    //MATCHTYPERANK::iterator itBegin = m_MatchTypeRank.begin();
    //for (;itBegin != m_MatchTypeRank.end(); ++itBegin )
    //{
    //    cmd_entity_match_type_rank Data;
    //    Data.nMatchType = itBegin->second.nMatchType;
    //    Data.nGrade = itBegin->second.nGrade;
    //    Data.nRankScore = itBegin->second.nRankScore;
    //    // 获得段位信息
    //    const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(Data.nMatchType, Data.nGrade);
    //    if (pSchemeInfo != NULL)
    //    {
    //        sstrcpyn( Data.szGradeName,(char *)pSchemeInfo->szDes, sizeof(Data.szGradeName));
    //    }

    //    obData.push_back(&Data, sizeof(cmd_entity_match_type_rank));
    //}

    //m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_MATCHTYPE_RANK_RECORD,0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgMatchHeroScore(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL)
    {
        return;
    }

    // 校验返回数据
    DBREQ_RESULT_MATCH_ALL_HEROSCORE * pData = (DBREQ_RESULT_MATCH_ALL_HEROSCORE *)pszMsg;
    if ( nLen != sizeof(*pData) + pData->nReCount * sizeof(MatchHeroScoreNode))
    {
        ErrorLn(__FUNCTION__": pData length error !");
        return ;
    }

    m_MatchHeroScore.clear();
    // 解析当前返回数据
    MatchHeroScoreNode* pDataHead = (MatchHeroScoreNode *)(pData + 1);
    for(int i = 0; i < pData->nReCount; ++i)
    {
        MatchHeroScoreNode Data;    
        Data.nHeroID = pDataHead->nHeroID;          // 英雄ID
        Data.nHeroScore = pDataHead->nHeroScore;    // 英雄熟练度得分
        Data.nHeroGrade = pDataHead->nHeroGrade;    // 英雄熟练度段位

        m_MatchHeroScore.insert(pair<int, MatchHeroScoreNode>( Data.nHeroID, Data));
    }

    //发送到客户端显示
    obuf obData;
    cmd_count cmdCount;
    cmdCount.nCount = m_MatchHeroScore.size();
    obData << cmdCount;
    MATCHTHEROSCORE::iterator itBegin = m_MatchHeroScore.begin();
    for (;itBegin != m_MatchHeroScore.end(); ++itBegin )
    {
        cmd_entity_match_hero_score Data;
        Data.nHeroID = itBegin->second.nHeroID;          // 英雄ID
        Data.nHeroScore = itBegin->second.nHeroScore;    // 英雄熟练度得分
        Data.nHeroGrade = itBegin->second.nHeroGrade;    // 英雄熟练度段位

        obData.push_back(&Data, sizeof(cmd_entity_match_hero_score));
    }

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_MATCH_HEROSCORE_RECORD,0, 0, obData.data(), obData.size());

}

void CPlayerBankPart::OnMsgMatchSingleTypeRank(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL)
    {
        return;
    }
    // 校验返回数据
    msg_entity_match_single_rank * pData = (msg_entity_match_single_rank *)pszMsg;
    if ( nLen != sizeof(msg_entity_match_single_rank))
    {
        ErrorLn(__FUNCTION__": pData length error !");
        return ;
    }

    MATCHTYPERANK::iterator iter = m_MatchTypeRank.find((EMMatchType)pData->RankNodeInfo.nMatchType);
    if (iter == m_MatchTypeRank.end())
    {
        // 添加
        m_MatchTypeRank[(EMMatchType)pData->RankNodeInfo.nMatchType] = pData->RankNodeInfo;
    }
    else
    {
        // 更新
        iter->second.nRankScore = pData->RankNodeInfo.nRankScore;
        iter->second.nGrade = pData->RankNodeInfo.nGrade;
    }
    int nDownOrUp = 0;
    if (pData->bFallGrade)
    {
        nDownOrUp = -1;
    }
    if (pData->bRiseGrade)
    {
        nDownOrUp = 1;
    }

    //客户端显示更新
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    if (pMatchClient == NULL)
    {
        ErrorLn(__FUNCTION__": pMatchClient == NULL");
        return;
    }
    SSchemeMatchRoom* pSchemeMatchRoom = pMatchClient->getSchemeMatchRoomByMatchType(pData->RankNodeInfo.nMatchType);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return;
    }

    ISchemeLevelUpgrade* pLvUpgrade = gClientGlobal->getSchemeCenter()->getSchemeLevelUpgrade();

    ISchemeMatchRankConfigInfo* pMatchRank = gClientGlobal->getSchemeCenter()->getSchemeMatchRankConfig();
    if( pLvUpgrade == NULL || pMatchRank == NULL )
        return;
    if (pSchemeMatchRoom->m_nbShowDownOrUpWnd)
    {
        // 通知客户端显示排位升级或者降级
        cmd_entity_rank_down_or_upgrade cmdInfo;
        cmdInfo.nDownOrUp   = nDownOrUp;  // 0：不变 1： 升 -1：降
        cmdInfo.nPreGrade   = pData->nPreGrade;
        cmdInfo.nPreScore   = pData->nPreScore;

        cmdInfo.nRankScore  = pData->RankNodeInfo.nRankScore;
        cmdInfo.nGrade      = pData->RankNodeInfo.nGrade;
        const SMatchRankConfigSchemeInfo* pRankConfigInfo = pMatchRank->getMatchRankConfigShemeInfo(pData->RankNodeInfo.nMatchType, pData->RankNodeInfo.nGrade);
        if( pRankConfigInfo != NULL )
        {
            cmdInfo.nRankIconID = pRankConfigInfo->nIconID;
            sstrcpyn(cmdInfo.chRankName, pRankConfigInfo->szDes, sizeof(cmdInfo.chRankName));
        }

        m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RANK_DWON_OR_UPGRADE,0, 0, &cmdInfo, sizeof(cmdInfo));
    }
}

void CPlayerBankPart::OnMsgMatchSingleHeroScore(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL)
    {
        return;
    }

    // 校验返回数据
    MatchHeroScoreNode * pData = (MatchHeroScoreNode *)pszMsg;
    if ( nLen !=  sizeof(MatchHeroScoreNode))
    {
        ErrorLn(__FUNCTION__": pData length error !");
        return ;
    }
    MATCHTHEROSCORE::iterator iter = m_MatchHeroScore.find(pData->nHeroID);
    if (iter == m_MatchHeroScore.end())
    {
        // 添加
        m_MatchHeroScore[pData->nHeroID] = *pData;
    }
    else
    {
        // 更新
        iter->second.nHeroGrade = pData->nHeroGrade;
        iter->second.nHeroScore = pData->nHeroScore;
    }


    //发送到客户端显示
    obuf obData;
    cmd_count cmdCount;
    cmdCount.nCount = m_MatchHeroScore.size();
    obData << cmdCount;
    MATCHTHEROSCORE::iterator itBegin = m_MatchHeroScore.begin();
    for (;itBegin != m_MatchHeroScore.end(); ++itBegin )
    {
        cmd_entity_match_hero_score Data;
        Data.nHeroID = itBegin->second.nHeroID;          // 英雄ID
        Data.nHeroScore = itBegin->second.nHeroScore;    // 英雄熟练度得分
        Data.nHeroGrade = itBegin->second.nHeroGrade;    // 英雄熟练度段位

        obData.push_back(&Data, sizeof(cmd_entity_match_hero_score));
    }

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_MATCH_HEROSCORE_RECORD,0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgTalentPageInfoAll(LPCSTR pszMsg, int nLen)
{
#ifdef  FUNCTION_TALENTPAGE_OPEN
	if (pszMsg == NULL)
    {
        return;
    }
    // 效验长度值
    int nEfficacyLen=sizeof(TALENT_DATA_RESULT);
    if(nLen<nEfficacyLen)
    {
        ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< TALENT_DATA_RESULT Size= "<<nEfficacyLen);
        return ;
    }
    m_mapTalentPage.clear();
    // 校验返回数据
    TALENT_DATA_RESULT * pData = (TALENT_DATA_RESULT *)pszMsg;

    if( pData->byPageNum==0)
    {
        // lol目前没有页的时候会默认创建两个天赋页的，存到数据库去
        CreateDefaultTalentPage();
        return;
    }
    // 解析当前返回数据,到时看看数据库那边怎么返回再调整
    // 循环天赋页
    char *pHead=(char*)(pData+1);

    for(int i = 0; i < pData->byPageNum; ++i)
    {
        TALENT_PAGE* pTalentBase=(TALENT_PAGE*)(pHead);
        TALENTPAGEINFO talentPageInfo;
        talentPageInfo.talentBaseInfo.nCurPageIndex=pTalentBase->nCurPageIndex;
        sstrcpyn( talentPageInfo.talentBaseInfo.szTalentPageName,pTalentBase->szTalentPageName,sizeof( talentPageInfo.talentBaseInfo.szTalentPageName));
        int * pTalentPointsLen=(int*)(pTalentBase+1);
        int nTalentPointsLen=*pTalentPointsLen;
        int nTalentCount=nTalentPointsLen/sizeof(TALENT_INFO);
        // 拿到添加多个SingleTalentPoints的obuf
        TALENT_INFO *pTalentInfo = (TALENT_INFO *)(pTalentPointsLen+1);
        for (int j=0;j<nTalentCount;j++)
        {
            talentPageInfo.vTalentInfoVec.push_back(*pTalentInfo);
            pTalentInfo++;
        }

        nEfficacyLen+=sizeof(TALENT_PAGE)+sizeof(int)+nTalentPointsLen;
        if (nLen<nEfficacyLen)
        {
            ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< EfficacyTotalLen= "<<nEfficacyLen);
            return ;
        }
        pHead=(char*)(pTalentInfo);
        m_mapTalentPage.insert(pair<int,TALENTPAGEINFO> ( pTalentBase->nCurPageIndex ,talentPageInfo));
    }
#endif
}

void CPlayerBankPart::SendMyTalentPageInfoAllToView()
{
#ifdef  FUNCTION_TALENTPAGE_OPEN
	if(m_pMaster==NULL)
    {
        return ;
    }
    //发送到客户端显示
    obuf obData;

    cmd_count cmdCount;
    cmdCount.nCount = m_mapTalentPage.size();
    obData << cmdCount;

    MAPTALENTPAGE::iterator itBegin = m_mapTalentPage.begin();
    for (;itBegin != m_mapTalentPage.end(); ++itBegin )
    {
        TALENT_INFO_VECTOR & vTalentVec = itBegin->second.vTalentInfoVec;

        cmd_TalentPage_Points_Single cmdTalentInfo;
        cmdTalentInfo.nCurPageIndex = itBegin->second.talentBaseInfo.nCurPageIndex;
        sstrcpyn(cmdTalentInfo.szTalentPageName, itBegin->second.talentBaseInfo.szTalentPageName,sizeof(cmdTalentInfo.szTalentPageName));
        cmdTalentInfo.nTalentNum = 0;

        for(TALENT_INFO_VECTOR::iterator iter=vTalentVec.begin(); iter!=vTalentVec.end(); ++iter)
        {
            cmdTalentInfo.nTalentId[cmdTalentInfo.nTalentNum]     = iter->byTalentID;
            cmdTalentInfo.nTalentPoints[cmdTalentInfo.nTalentNum] = iter->byPoints;
            ++cmdTalentInfo.nTalentNum;
        }

        obData.push_back(&cmdTalentInfo, sizeof(cmdTalentInfo));
    }

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_TALENT_PAGE_INFO_ALL,0, 0, obData.data(), obData.size());
#endif
}

void CPlayerBankPart::CreateDefaultTalentPage()
{
#ifdef  FUNCTION_TALENTPAGE_OPEN
	if(m_pMaster==NULL)
    {
        return ;
    }
    int nMaxPageCount = min(DefaulTalentPageCount, 20);
    for(int i=0;i<nMaxPageCount;i++)
    {
        // 服务器一份
        msg_Entity_TalentPage_Points_Set_Single MsgData;
        int nCurPageIndex=i+1;
        MsgData.nCurPageIndex=nCurPageIndex;
        MsgData.nPageTalentNum=0;
        string sDefaultName;
        ssprintf_s(MsgData.szTalentPageName, sizeof(MsgData.szTalentPageName), "TalentPage%d", nCurPageIndex);
        // 客户端缓存一份
        TALENTPAGEINFO talentPageInfo;
        talentPageInfo.talentBaseInfo.nCurPageIndex=nCurPageIndex;
        talentPageInfo.talentBaseInfo.nPageTalentNum=0;
        sstrcpyn(talentPageInfo.talentBaseInfo.szTalentPageName,MsgData.szTalentPageName,sizeof(talentPageInfo.talentBaseInfo.szTalentPageName));
        m_mapTalentPage.insert(pair<int,TALENTPAGEINFO> (  MsgData.nCurPageIndex ,talentPageInfo));
        // 发到服务器保存
        obuf obdata;
        obdata.push_back(&MsgData,sizeof(MsgData));
        SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_TALENTPAGEPOINTS_SET_SINGLE, obdata.data(), obdata.size());
    }
#endif
}

void CPlayerBankPart::OnMsgMatchReturnPlayerInfo(LPCSTR pszMsg, int nLen)
{
    if ( pszMsg == NULL || nLen < sizeof(msg_entity_player_info_return))
        return;

    ISchemeLevelUpgrade* pLvUpgrade = gClientGlobal->getSchemeCenter()->getSchemeLevelUpgrade();
    ISchemeMatchRankConfigInfo* pMatchRank = gClientGlobal->getSchemeCenter()->getSchemeMatchRankConfig();
    if( pLvUpgrade == NULL || pMatchRank == NULL )
        return;
	
    msg_entity_player_info_return* pPlayrInfo = (msg_entity_player_info_return*) pszMsg;
    obuf obData;
    cmd_player_info_return cmdPlayerInfoRet;
    cmdPlayerInfoRet.bFake = pPlayrInfo->bFake == true? 1: 0;
    cmdPlayerInfoRet.pDbid = pPlayrInfo->pDbid;
    cmdPlayerInfoRet.nLv = pPlayrInfo->nLv;
    cmdPlayerInfoRet.usHeadID = pPlayrInfo->nHeadID;
	cmdPlayerInfoRet.nHeroID = pPlayrInfo->nHeroID;
	cmdPlayerInfoRet.nSkinID = pPlayrInfo->nSkinID;
    sstrcpyn(cmdPlayerInfoRet.chClanName, (pPlayrInfo->chClanName), sizeof(cmdPlayerInfoRet.chClanName));
    sstrcpyn(cmdPlayerInfoRet.chClanPos, (pPlayrInfo->chClanPos), sizeof(cmdPlayerInfoRet.chClanPos));
    sstrcpyn(cmdPlayerInfoRet.chKinName, (pPlayrInfo->chKinName), sizeof(cmdPlayerInfoRet.chKinName));
    sstrcpyn(cmdPlayerInfoRet.chPlayerName, (pPlayrInfo->chActorName), sizeof(cmdPlayerInfoRet.chPlayerName));
    for (int i = 0; i < GLORYMAX; i++)
    {
        cmdPlayerInfoRet.nGlory[i] = pPlayrInfo->dwGlory[i];
        cmdPlayerInfoRet.llWarID[i] = pPlayrInfo->llWarID[i];
    }
    cmdPlayerInfoRet.ulFighting = pPlayrInfo->dwFighting;
    cmdPlayerInfoRet.ulGloryScore = pPlayrInfo->dwGloryScore;
    sstrcpyn(cmdPlayerInfoRet.chMostGlory, getMostGlory().c_str(), sizeof(cmdPlayerInfoRet.chMostGlory));
    cmdPlayerInfoRet.ulCurrentExp = pPlayrInfo->dwCurrentExp;
    cmdPlayerInfoRet.ulNeedExp = pLvUpgrade->GetNextLevelExp(cmdPlayerInfoRet.nLv, ENEXPTYPE_STATICPLAYEREXP);
    cmdPlayerInfoRet.nCount = pPlayrInfo->nCount;
	cmdPlayerInfoRet.nPKWinNum = pPlayrInfo->dwPKWinNum;
	cmdPlayerInfoRet.nSupportNum = pPlayrInfo->nSupportNum;
	cmdPlayerInfoRet.nPKTotalNum = pPlayrInfo->dwPKTotalNum;
    cmdPlayerInfoRet.nRankMatchNum = pPlayrInfo->dwRankMatchNum;
    cmdPlayerInfoRet.nRankWinNum   = pPlayrInfo->wRankWinNum;
	const SMatchRankConfigSchemeInfo* pRankConfigInfo = pMatchRank->getMatchRankConfigShemeInfo(MatchType_Rank, pPlayrInfo->nRankGrade);
	if( pRankConfigInfo != NULL )
	{
		cmdPlayerInfoRet.nRankIconID = pRankConfigInfo->nIconID;
		sstrcpyn(cmdPlayerInfoRet.chRankName, pRankConfigInfo->szDes, sizeof(cmdPlayerInfoRet.chRankName));
		cmdPlayerInfoRet.nRankScore = pPlayrInfo->nRankScore;

		const SMatchRankConfigSchemeInfo* pNextRankConfigInfo = pMatchRank->getMatchRankConfigShemeInfo(MatchType_Rank, pRankConfigInfo->nGradeID + 1);
		if (pNextRankConfigInfo != NULL)
		{
			cmdPlayerInfoRet.nNextRankScore = pNextRankConfigInfo->nGradeScore;
		}
	}

	obuf obTemp;
    if (nLen == sizeof(msg_entity_player_info_return) + pPlayrInfo->nCount * sizeof(msg_entity_hero_info))
    {
        msg_entity_hero_info* pMsgHeroInfo = (msg_entity_hero_info*)(pPlayrInfo + 1);
		int nRealCount = fillHeroInfo(pMsgHeroInfo, pPlayrInfo->nCount, obTemp, pPlayrInfo->pDbid);
		pPlayrInfo->nCount = nRealCount;
    }
	obData.push_back(&cmdPlayerInfoRet, sizeof(cmdPlayerInfoRet));
	obData.push_back(obTemp.data(), obTemp.size());

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_PLAYER_BASE_INFO,0, 0, obData.data(), obData.size());
}

////////////////////  符文系统 Rune  ///////////////////////////////
void CPlayerBankPart::OnMsgRuneInfo(LPCSTR pszMsg, int nLen)
{
#ifdef  FUNCTION_RUNE_OPEN
	if (pszMsg == NULL)
	{
		return;
	}

	int nEfficacyLen = sizeof(int);
	if (nLen < nEfficacyLen)
	{
		ErrorLn(__FUNCTION__": pData length error= !"<<nLen);
		return ;
	}

	m_mRunePage.clear();
	int nRunePageNum = 0;

	ibuffer inBuffer(pszMsg, nLen);

	inBuffer >> nRunePageNum;
	for (int runePageIdx = 0; runePageIdx < nRunePageNum; ++runePageIdx)
	{
		Entity_Rune_Page_Info runePage;
		inBuffer >> runePage;
		m_mRunePage.insert(make_pair(runePage.nPageIdx, runePage)); 
	}

	if(nRunePageNum==0)
	{
		CreateDefaultRunePage();
	}

	SendRunePageInfoAllToView();
#endif
}

void CPlayerBankPart::OnMsgOtherPlayerRuneInfo(LPCSTR pszMsg, int nLen)
{
    if( m_pMaster == NULL )
    {
        return ;
    }

    if (pszMsg == NULL)
    {
        return;
    }
    // 效验长度值
    int nEfficacyLen=sizeof(DBREQ_RESULT_QUERYRUNEINFO);
    if(nLen<nEfficacyLen)
    {
        ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< DBREQ_RESULT_QUERYRUNEINFO Size= "<<nEfficacyLen);
        return ;
    }

    // 校验返回数据
    DBREQ_RESULT_QUERYRUNEINFO * pData = (DBREQ_RESULT_QUERYRUNEINFO *)pszMsg;

    obuf obRuneData;
    cmd_Rune_Info_Total runeInfo;
    runeInfo.dwActorID = pData->dwActorID;
    runeInfo.nPageNum = pData->nPageNum;
    obRuneData.push_back(&runeInfo, sizeof(runeInfo));

    char *pHead=(char*)(pData+1);
    for(int i = 0; i < runeInfo.nPageNum; ++i)
    {	
        cmd_Rune_Single_PageInfo cmdRuneInfo;

        RUNE_PAGE *pRunePageInfo = reinterpret_cast<DBREQ_PARAM_RUNE_PAGE *>(pHead);
        sstrcpyn( cmdRuneInfo.sRunePageName, pRunePageInfo->szTalentPageName, sizeof(cmdRuneInfo.sRunePageName));// 名字
        cmdRuneInfo.nPageIndex = pRunePageInfo->nPageIndex;

        int *pRuneInfoLen = (int*)(pRunePageInfo+1);
        cmdRuneInfo.nRuneNum = *pRuneInfoLen / sizeof(RUNE_INFO);

        RUNE_INFO *pRuneInfo = (RUNE_INFO*)(pRuneInfoLen+1);
        int nRuneNum = cmdRuneInfo.nRuneNum;
        if(nRuneNum > RUNE_GROOVE_MAX)
        {
            nRuneNum = RUNE_GROOVE_MAX;
            ErrorLn(__FUNCTION__": nRuneNum > RUNE_GROOVE_MAX "  <<"nRuneNum ="<<cmdRuneInfo.nRuneNum);
        }
        for (int j = 0; j < nRuneNum; ++j)
        {
            int nPostion = pRuneInfo->nPostion;
            if(pRuneInfo->nPostion > RUNE_GROOVE_MAX-1)
            {
                nPostion = RUNE_GROOVE_MAX-1;
                ErrorLn(__FUNCTION__": pRuneInfo->nPostion > RUNE_GROOVE_MAX - 1"  <<"pRuneInfo->nPostion ="<<pRuneInfo->nPostion);
            }
            cmdRuneInfo.nRuneGroove[nPostion] = pRuneInfo->nID;
            pRuneInfo++;			
        }

        obRuneData.push_back(&cmdRuneInfo,sizeof(cmdRuneInfo)); 
        pHead = reinterpret_cast<char *>(pRuneInfo);
    }

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_OTHER_RUNE_PAGE_INFO_ALL,0, 0, obRuneData.data(), obRuneData.size());
}

void CPlayerBankPart::OnMsgUpdateChestInfo(LPCSTR data, int len)
{
    if (data == NULL || len < sizeof(int))
        return;

	obuf obData;

    int *pCount = (int *)data;
	int nCount = *pCount;
	obData << nCount;

	char * pNode = (char *)(pCount + 1);
	for (int i = 0; i < nCount; ++i)
	{
		DWORD * pdwID		= (DWORD *)(pNode + 2 * i * sizeof(DWORD));
		DWORD * pdwCount	= (DWORD *)(pNode + 2 * i * sizeof(DWORD) + sizeof(DWORD));

		obData << *pdwID << *pdwCount;
	}

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_CHEST_INFO, 0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgUpdateEmojiInfo(LPCSTR data, int len)
{
	if (data == NULL || len < sizeof(int))
		return;

	obuf obData;

	int *pCount = (int *)data;
	int nCount = *pCount;
	obData << nCount;

	BYTE * pNode = (BYTE *)(pCount + 1);
	for (int i = 0; i < nCount; ++i)
	{
		obData << *pNode;

		++pNode;
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_EMOJI_INFO, 0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgOpenChestResult(LPCSTR data, int len)
{
    if (data == NULL || len < sizeof(msg_entity_open_chest_result))
        return;

    msg_entity_open_chest_result *pResultInfo = (msg_entity_open_chest_result*)data;

    cmd_entity_open_chest_result cmdInfo;
    cmdInfo.nChestType = pResultInfo->nChestType;
    for (int i = 0; i < PRIZEGROUP_RESULT_MAX; ++i)
    {
        cmdInfo.nPrizeID[i] = pResultInfo->nPrizeID[i];
    }

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_OPEN_CHEST_RESULT, 0, 0, &cmdInfo, sizeof(cmdInfo));
}

void CPlayerBankPart::OnMsgReturnChestRandCardInfo(LPCSTR data, int len)
{
	if (data == NULL || len < 2 * sizeof(int))
		return;

	int * pPrizeId = (int *)data;
	int * pCount = (int *)(pPrizeId + 1);

	if (len != 2 * sizeof(int) + (*pCount) * sizeof(msg_entity_hero_card))
		return;

	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if( pSchemeCenter == NULL) 
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL ");
		return;
	}
	ISchemeHeroCard* pSchemeHeroCard = pSchemeCenter->getSchemeHeroCard();
	if(pSchemeHeroCard == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeHeroCard == NULL");
		return;
	}
	
	obuf obData;
	obData << *pPrizeId << *pCount;

	msg_entity_hero_card * pCard = (msg_entity_hero_card *)(pCount + 1);
	for (int i = 0; i < *pCount; i++)
	{
		cmd_entity_hero_card cmdCard;
		cmdCard.nHeroID = pCard->nHeroID;
		cmdCard.nCount = pCard->nCount;
		//string stCardName = pSchemeHeroCard->getUBBNameByCardID(pCard->nHeroID);
		string stCardName = pSchemeHeroCard->getByCardID(pCard->nHeroID)->szCardName;
		sstrcpyn(cmdCard.chHeroCardName, stCardName.c_str(), sizeof(cmdCard.chHeroCardName));
		sstrcpyn(cmdCard.chHeroCardDesc, pSchemeHeroCard->getByCardID(pCard->nHeroID)->szRemark, sizeof(cmdCard.chHeroCardDesc));
		obData.push_back(&cmdCard, sizeof(cmdCard));
		pCard++;
	}
	
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CHEST_RANDCARD_RESULT, 0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgPointshopUpdateBoughtNum(LPCSTR data, int len)
{
	if (data == NULL || len < sizeof(msg_entity_pointshop_update_boughtnum))
		return;

	msg_entity_pointshop_update_boughtnum *pInfo = (msg_entity_pointshop_update_boughtnum*)data;
	data += sizeof(msg_entity_pointshop_update_boughtnum);
	int nNeedLen = sizeof(msg_entity_pointshop_update_boughtnum) + pInfo->nCount * sizeof(msg_entity_pointshop_update_boughtnum_sub);
	if (len != nNeedLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength="<< len << "  needlen=" << nNeedLen);
		return;
	}

	for (int i = 0; i < pInfo->nCount; ++i)
	{
		msg_entity_pointshop_update_boughtnum_sub *pSubInfo = (msg_entity_pointshop_update_boughtnum_sub*)data;
		data += sizeof(msg_entity_pointshop_update_boughtnum_sub);

		cmd_entity_update_pointshop_buynum_info cmdInfo;
		cmdInfo.nSellID = pSubInfo->nSellID;
		cmdInfo.nBoughtNum = pSubInfo->nBoughtNum;
		m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_POINTSHOP_BUYNUM_INFO, 0, 0, &cmdInfo, sizeof(cmdInfo));
	}
}

void CPlayerBankPart::OnMsgPointshopReflushBoughtNum()
{
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_REFLUSH_POINTSHOP_BUYNUM_INFO, 0, 0, NULL, 0);
}

void CPlayerBankPart::OnMsgHeroUpStarUnlockPrize(LPCSTR data, int len)
{
	if (data == NULL || len	< sizeof(msg_entity_heroupstar_unlock))
	{
		ErrorLn(__FUNCTION__"data == NULL || len != needlen len = " << len << "needlen = " << sizeof(msg_entity_heroupstar_unlock));
		return;
	}

	msg_entity_heroupstar_unlock * msgData = (msg_entity_heroupstar_unlock *)data;
	cmd_entity_heroupstar_unlockprize cmdData;
	cmdData.nHeroID = msgData->nHeroID;
	cmdData.nStarLevel = msgData->nStarLevel;
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HEROUPSTAR_UNLOCKPRIZE, 0, 0, &cmdData, sizeof(cmd_entity_heroupstar_unlockprize));
}

void CPlayerBankPart::onMsgNoviceGuideData(LPCSTR data, int len)
{
	if ( data == NULL || len != sizeof(DWORD))
	{
		ErrorLn(__FUNCTION__": len = " << len << ", sizeof(DWORD) = " << sizeof(DWORD));
		return;
	}

	updateNoviceGuideData(*(DWORD*)data);
}

void CPlayerBankPart::onMsgLifeHeroTaskData(LPCSTR data, int len)
{
	if (data == NULL || len != sizeof(int))
	{
		ErrorLn(__FUNCTION__": len = " << len << ", sizeof(int) = " << sizeof(int));
		return;
	}

	m_nLifeHeroTaskData = *(int*)data;
}

void CPlayerBankPart::onMsgLifeHeroPrizeTaskData(LPCSTR data, int len)
{
	if (data == NULL || len != sizeof(int))
	{
		ErrorLn(__FUNCTION__": len = " << len << ", sizeof(int) = " << sizeof(int));
		return;
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_LIFEHERO_PRIZE_TASKDATA, 0, 0, (void *)data, len);
}

void CPlayerBankPart::onMsgActorUnlockData(LPCSTR data, int len)
{
	if (data == NULL || len < 2 * sizeof(int))
		return;

	int *pActorLv = (int *)data;
	int *pCount = (int *)(pActorLv + 1);
	if (len != 2 * sizeof(int) + (*pCount) * sizeof(int))
	{
		ErrorLn(__FUNCTION__": len = " << len << ", correctLen = " << (*pCount + 2) * sizeof(int));
		return;
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTOR_UNLOCK, 0, 0, (void *)data, len);
}

void CPlayerBankPart::OnMsgPointshopBuyResult(LPCSTR data, int len)
{
	if (data == NULL || len < sizeof(msg_entity_pointshop_buyresult))
		return;

	msg_entity_pointshop_buyresult *pInfo = (msg_entity_pointshop_buyresult*)data;

	cmd_entity_pointshop_buyresult cmdInfo;
	cmdInfo.nSucceed = pInfo->nSucceed;

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_POINTSHOP_BUY_RESULT, 0, 0, &cmdInfo, sizeof(cmdInfo));
}

void CPlayerBankPart::OnMsgPointshopUpdateCardList(LPCSTR data, int len)
{
	if (data == NULL || len < sizeof(msg_entity_pointshop_update_cardlist))
		return;

	msg_entity_pointshop_update_cardlist *pInfo = (msg_entity_pointshop_update_cardlist*)data;

	cmd_entity_pointshop_update_cardlist cmdInfo;
	cmdInfo.nLeftReflushTime = pInfo->nReflushLeftTime;
	cmdInfo.nLeftReflushNum = pInfo->nReflushLeftNum;
	for (int i = 0; i < POINTSHOP_CARDLIST_MAXCOUNT; ++i)
	{
		cmdInfo.nSellID[i] = pInfo->nSellID[i];
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_POINTSHOP_UPDATE_CARDLIST, 0, 0, &cmdInfo, sizeof(cmdInfo));
}

void CPlayerBankPart::SendRunePageInfoAllToView()
{
#ifdef  FUNCTION_RUNE_OPEN
	if(m_pMaster==NULL)
    {
        return ;
    }

    obuf obData;
    obuf obDataPage;
    obuf obDataStore;

    //发送到客户端显示

    cmd_count cmdPageCount;
    cmdPageCount.nCount = m_mRunePage.size();
    obDataPage.push_back(&cmdPageCount,sizeof(cmd_count));

    T_RunePageMap::iterator itBegin = m_mRunePage.begin();
    for (;itBegin != m_mRunePage.end(); ++itBegin )
    {
        cmd_Rune_Single_PageInfo cmdRuneInfo;
        cmdRuneInfo.nPageIndex=itBegin->second.nPageIdx;
        sstrcpyn(cmdRuneInfo.sRunePageName, itBegin->second.szRunePageName,sizeof(cmdRuneInfo.sRunePageName));
        memcpy(cmdRuneInfo.nRuneGroove,itBegin->second.nRuneGroove,sizeof(cmdRuneInfo.nRuneGroove));
        obDataPage.push_back(&cmdRuneInfo,sizeof(cmdRuneInfo)); 
    }

    cmd_count cmdStoreCount;
    cmdStoreCount.nCount = m_mapRuneStore.size();
    obDataStore.push_back(&cmdStoreCount,sizeof(cmd_count));
    RUNESTORE_MAP::iterator itStoreBegin = m_mapRuneStore.begin();
    for (;itStoreBegin != m_mapRuneStore.end(); ++itStoreBegin )
    {
        cmd_Rune_InStore_Item cmdRuneStore;
        cmdRuneStore.nRuneId=itStoreBegin->second.nRuneId;
        cmdRuneStore.nRuneNum=itStoreBegin->second.byActiveRuneCount;
        obDataStore.push_back(&cmdRuneStore,sizeof(cmdRuneStore));
    }

    obData.push_back(obDataStore.data(),obDataStore.size());
    obData.push_back(obDataPage.data(),obDataPage.size());

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_RUNE_PAGE_INFO_ALL,0, 0, obData.data(), obData.size());
#endif
}

void CPlayerBankPart::CreateDefaultRunePage()
{
#ifdef  FUNCTION_RUNE_OPEN
	if(m_pMaster==NULL)
    {
        return ;
    }

    int nMaxPageCount = min(DefaulRunePageCount, 20);

    for(int i=0;i<nMaxPageCount;++i)
    {
        // 服务器一份
        msg_runePage_save MsgData;
        int nCurPageIndex=i+1;
        MsgData.nRunePageIdx=nCurPageIndex;
        MsgData.nRuneNum=0;
        string sDefaultName;
        ssprintf_s(MsgData.szRunePageName, sizeof(MsgData.szRunePageName), "RunePage%d", nCurPageIndex);
        // 客户端缓存一份
        Entity_Rune_Page_Info runePage;
        runePage.nPageIdx=nCurPageIndex;
        runePage.nRuneNum=0;
        sstrcpyn(runePage.szRunePageName,MsgData.szRunePageName,sizeof(runePage.szRunePageName));
        m_mRunePage.insert(make_pair(runePage.nPageIdx, runePage)); 
        // 发到服务器保存
        obuf obdata;
        obdata.push_back(&MsgData,sizeof(MsgData));
        SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_RUNE_SAVE, obdata.data(), obdata.size());
    }
#endif
}

void CPlayerBankPart::OnMsgUpdateActiveRuneStoreInfo(LPCSTR pszMsg, int nLen)
{
#ifdef  FUNCTION_RUNE_OPEN
	if (pszMsg == NULL)
    {
        return;
    }

    if(m_pMaster == NULL)
    {
        return;
    }

    int nEfficacyLen=sizeof(EntityHeroRuneStore);
    if(nLen<nEfficacyLen)
    {
        ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< EntityHeroRuneStore Size= "<<nEfficacyLen);
        return ;
    }

    EntityHeroRuneStore* heroRuneStore= (EntityHeroRuneStore*)pszMsg;
    if(heroRuneStore == NULL)
    {
        ErrorLn(__FUNCTION__":heroRuneStore == null , Update Talent Is Fail");
        return;
    }

    int nRuneId = heroRuneStore->nRuneId;
    cmd_Rune_InStore_Item cmdRuneStoreItem;
    cmdRuneStoreItem.nRuneId = nRuneId;
    cmdRuneStoreItem.nRuneNum = heroRuneStore->byActiveRuneCount;

    if(m_mapRuneStore.find(nRuneId) == m_mapRuneStore.end())
    {
        EntityHeroRuneStore heroRuneStore;
        heroRuneStore.nRuneId = nRuneId;
        m_mapRuneStore[nRuneId] = heroRuneStore;
    }

    m_mapRuneStore[nRuneId].byActiveRuneCount += cmdRuneStoreItem.nRuneNum;
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HERO_RUNESTOREUPDATE,0, 0, &cmdRuneStoreItem, sizeof(cmd_Rune_InStore_Item));
#endif
}

void CPlayerBankPart::SendHeroDetailInfoToView(LPCSTR pszMsg, int nLen)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if ( pSchemeCenter == NULL )
		return;
	ISchemeHeroStarData* pStarData = pSchemeCenter->getSchemeHeroStarInfo();   
	ISchemePersonModel* pModel = pSchemeCenter->getSchemePersonModel();
	ISchemeLevelUpgrade* pUpgradeInfo = pSchemeCenter->getSchemeLevelUpgrade();
	ISchemeHeroAchievementLvConfig* pAchievementLv = pSchemeCenter->getSchemeHeroAchievementLv();
	if( pStarData == NULL || pModel == NULL || pUpgradeInfo == NULL || pAchievementLv == NULL )
		return;

	int dwSelfPDBID = gClientGlobal->getHero()->getIntProperty(PROPERTY_ID);

	cmd_Request_Hero_Detail_Info * pReqHeroInfo = (cmd_Request_Hero_Detail_Info *)pszMsg;
	int dwPDBID = pReqHeroInfo->nActorID;
	int nHeroID = pReqHeroInfo->nHeroID;
	cmd_entity_hero_info sHeroInfo = pReqHeroInfo->heroInfo;

	int nStar = 1;
	int nStaticLv = 1;
	int nMatchNum = 0;
	int nHeroExp = 0;
	int nScore = 0;
	int nWinNum = 0;

	cmd_entity_hero_detail_info cmdHeroDetailInfo;
	cmdHeroDetailInfo.bIsHaveHero = false;

	// nHeroID > 0,则表示该英雄有数据；bIsBuy > 0,则表示已购买
	if (sHeroInfo.nHeroID != 0)
	{
		nStar = sHeroInfo.byStarLv;
		nStaticLv = sHeroInfo.nLv;
		nMatchNum = sHeroInfo.nGameCount;
		nHeroExp = sHeroInfo.ulExp;
		nScore = sHeroInfo.ulScore;
		nWinNum = sHeroInfo.nGameCount * sHeroInfo.usWinPro / 100;
		cmdHeroDetailInfo.nCount = pAchievementLv->getIconIDAccordScore(nScore, cmdHeroDetailInfo.nIconID, ICONGROUP_COUNT_MAX);
	}
	if (sHeroInfo.bIsBuy > 0)
	{
		cmdHeroDetailInfo.bIsHaveHero = true;
	}
	
	SHeroStarSchemeInfo* pHeroStarData = pStarData->getHeroStarData(nHeroID, nStar);
	if( pHeroStarData == NULL ) return;
	
	cmdHeroDetailInfo.nHeroID = nHeroID;
	cmdHeroDetailInfo.nLv = nStaticLv;
	cmdHeroDetailInfo.nLimitLv = pHeroStarData->nLimitLV;
	cmdHeroDetailInfo.nExp = nHeroExp;
	cmdHeroDetailInfo.nStar = nStar;
	cmdHeroDetailInfo.nScore = nScore;
	cmdHeroDetailInfo.nGameCount = nMatchNum;

	cmdHeroDetailInfo.usWinPro = (nWinNum * 100)/ (nMatchNum == 0 ? 1 : nMatchNum);
	cmdHeroDetailInfo.nStarNeedExp = pHeroStarData->nNeedCount;//sHeroInfo.nNeedCardCount;
	sstrcpyn(cmdHeroDetailInfo.chStarDesc, pHeroStarData->chStarDesc, sizeof(cmdHeroDetailInfo.chStarDesc));

	// 自身信息，特殊赋值该项
	if (dwPDBID == dwSelfPDBID)
	{
		DBREQ_PARAM_HERO_CARD dbHeroDatar = m_heroMgr.getHeroCardInfo(pHeroStarData->dwNeedCardID);
		cmdHeroDetailInfo.nStarExp = dbHeroDatar.nCardCount;
	}

	sstrcpyn(cmdHeroDetailInfo.chHeroName, pModel->getHeroName(nHeroID), sizeof(cmdHeroDetailInfo.chHeroName) );
	int nExpTypeID = pStarData->getUpgradeNeedExpTypeID(nHeroID, nStar);
	cmdHeroDetailInfo.nNeedExp = pUpgradeInfo->GetNextLevelExp( nStaticLv, ENEXPTYPE_STATICHEROEXP, nExpTypeID);
	fillHeroProperty(nHeroID, nStar, nStaticLv, cmdHeroDetailInfo);
	obuf obData;
	obData << dwPDBID << cmdHeroDetailInfo;

	// 加载英雄技能信息
	loadHeroSkillInfo(nHeroID,obData);
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RETURN_HERO_DETAIL_INFO,0, 0, obData.data(), obData.size());
}


void CPlayerBankPart::OnMsgMatchReturnHeroInfo(LPCSTR pszMsg, int nLen)
{
	if ( pszMsg == NULL || nLen < sizeof(int) * 2)
	    return;
	int* pdwPDBID = (int *)pszMsg;
	int* pCount = (int *)(pdwPDBID + 1);
	int dwPDBID = *pdwPDBID;
	int nCount = *pCount;
	
	if (nLen != 2 * sizeof(int) + nCount * sizeof(msg_entity_hero_info))
	    return;
	msg_entity_hero_info* pHeroData = (msg_entity_hero_info*)(pCount + 1);
	
	
	obuf obData;
	// 填充已拥有英雄数据
	obuf obHaveHero;
	msg_entity_hero_info* pMsgHeroInfo = pHeroData;
	int nHaveCount = fillHeroInfo(pMsgHeroInfo, nCount, obHaveHero, dwPDBID);
	cmd_pdbid_count cmdHave;
	cmdHave.nCount = nHaveCount;
	cmdHave.dwPDBID = dwPDBID;
	obData.push_back(&cmdHave, sizeof(cmdHave));
	obData.push_back(obHaveHero.data(), obHaveHero.size());
	
	// 填充未拥有英雄数据
	std::map<int, msg_entity_hero_info *> tmpHeroMap;
	for (int i = 0; i < nCount; i++)
	{
	    tmpHeroMap[pHeroData->nHeroID] = pHeroData;
	    pHeroData++;
	}
	fillNoHaveHeroInfo(tmpHeroMap, obData, dwPDBID);

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HERO_INFO,0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgUpdateHeroInfo(LPCSTR pszMsg, int nLen)
{
    if ( pszMsg == NULL || nLen < sizeof(int))
        return;
    int* pCount = (int*)pszMsg;
	int nCount = *pCount;
    if ( nLen != sizeof(int) + nCount * sizeof(SHeroInfo))
        return;

	PDBID dwPDBID = gClientGlobal->getHero()->getIntProperty(PROPERTY_ID);

    SHeroInfo* pHeroInfo = (SHeroInfo*)(pCount + 1);
	msg_entity_hero_info *pMsgInfo = new msg_entity_hero_info[nCount];
    for (int i = 0; i < nCount; ++i )
    {
        m_heroMgr.updatePlayerHeroInfo(pHeroInfo);
		fillHeroInfoMsg(pMsgInfo[i], *pHeroInfo);
		pHeroInfo++;
    }

	obuf obData;
	obuf obTemp;
	int nRealCount = fillHeroInfo(pMsgInfo, nCount, obTemp, dwPDBID);
	obData.push_back(&nRealCount, sizeof(int));
	obData.push_back(obTemp.data(), obTemp.size());
	delete [] pMsgInfo;

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_HERO_INFO,0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgUpdatePlayerInfo(LPCSTR pszMsg, int nLen)
{
    if ( pszMsg == NULL || nLen < sizeof(SPlayerDatabasesInfo))
    {
        return;
    }
	ISchemeLevelUpgrade* pLvUpgrade = gClientGlobal->getSchemeCenter()->getSchemeLevelUpgrade();
	if (pLvUpgrade == NULL)
	{
		return;
	}


    SPlayerDatabasesInfo* pInfo = (SPlayerDatabasesInfo*)pszMsg;
    m_playInfo = *pInfo;
    // 现在先考虑角色部分分类更新  全部更新到客户端好像数据太多了 以后看看是否需要全部更新
    int nInfoChangeType = *(int*)(pInfo+1);
	cmd_actor_info_change changeInfo;
	changeInfo.nType = nInfoChangeType;
    switch(nInfoChangeType)
    {
		case ENPLAYERINFOTYPE_LEVEL:
			{
				changeInfo.nValue1 = m_playInfo.player.nLevel;
				m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTOR_INFO_CAHNGE, 0, 0, &changeInfo, sizeof(changeInfo));
			}
			break;
		case ENPLAYERINFOTYPE_EXP:
			{
				changeInfo.nValue1 = m_playInfo.player.dwExp;
				changeInfo.nValue2 = pLvUpgrade->GetNextLevelExp(m_playInfo.player.nLevel, ENEXPTYPE_STATICPLAYEREXP);
				m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTOR_INFO_CAHNGE, 0, 0, &changeInfo, sizeof(changeInfo));
			}
			break;
		case ENPLAYERINFOTYPE_PKWINNUM:
			{
				changeInfo.nValue1 = m_playInfo.player.dwPKWinNum;
				m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTOR_INFO_CAHNGE, 0, 0, &changeInfo, sizeof(changeInfo));
			}
			break;
		case ENPLAYERINFOTYPE_PKTOTALNUM:
			{
				changeInfo.nValue1 = m_playInfo.player.dwPKTotalNum;
				m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTOR_INFO_CAHNGE, 0, 0, &changeInfo, sizeof(changeInfo));
			}
			break;
		case ENPLAYERINFOTYPE_NAME:
			{
				sstrcpyn(changeInfo.str1, m_playInfo.player.szActorName, sizeof(changeInfo.str1));
				m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTOR_INFO_CAHNGE, 0, 0, &changeInfo, sizeof(changeInfo));
			}
			break;
		case ENPLAYERINFOTYPE_GLORY:
			{
				for (int i = 0; i < ENACHIEVE_MAX; i++)
				{
					memcpy(&changeInfo.nGlory[i], m_playInfo.player.byGlory + i * 12, sizeof(DWORD));
					memcpy(&changeInfo.llWarID[i], m_playInfo.player.byGlory + i * 12 + 4, sizeof(LONGLONG));
				}
				m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTOR_INFO_CAHNGE, 0, 0, &changeInfo, sizeof(changeInfo));
			}
			break;
        case ENPLAYERINFOTYPE_RANKNUM:
            {
                // 同时改变的
                changeInfo.nValue1 = m_playInfo.player.dwRankMatchNum;
                changeInfo.nValue2 = m_playInfo.player.wRankWinNum;
                m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTOR_INFO_CAHNGE, 0, 0, &changeInfo, sizeof(changeInfo));
            }
            break;
            
		default:
			break;
    }
}

void CPlayerBankPart::OnMsgUpdateHeroCard(LPCSTR pszMsg, int nlen)
{
    m_heroMgr.updateHeroCardInfo(pszMsg, nlen);
}

void CPlayerBankPart::OnMsgUpgradeStarSucess(LPCSTR data, int nlen)
{
    if ( data == NULL || nlen < sizeof(msg_up_star_sucess))
    {
        ErrorLn(__FUNCTION__": nLen = " << nlen << ", sizeof(msg_up_star_sucess) = " << sizeof(msg_up_star_sucess));
        return;
    }

	msg_up_star_sucess* msgInfo = (msg_up_star_sucess*)data;

	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if ( pSchemeCenter == NULL )
		return;
	ISchemeHeroStarData* pStarData = pSchemeCenter->getSchemeHeroStarInfo(); 
	if ( pStarData == NULL)
		return;
	SHeroStarSchemeInfo* pHeroStarData = pStarData->getHeroStarData(msgInfo->nHeroID, msgInfo->nCurrentStar);
	if( pHeroStarData == NULL ) 
		return;

	// 1.显示升级成功画面 2.改变数值，detail,money 3.服务器按同步钱、卡牌、星级，卡牌数据同步时引导升星界面计算有问题，此处星级数据同步后再记性一次计算
	cmd_Return_HeroDetail_Upgrade_Success cmdSuccess;
	cmdSuccess.nCurStarLv = msgInfo->nCurrentStar;
	cmdSuccess.nSucess = (msgInfo->bIsSucess == true ? 1 : 0);
	cmdSuccess.nOldStarIconID = 0;
	cmdSuccess.nCurStarIconID = 0;
	cmdSuccess.nSpecialEffect = (isGradeSuccessSpecialEffect(msgInfo->nHeroID, msgInfo->nCurrentStar, cmdSuccess.nOldStarIconID, cmdSuccess.nCurStarIconID) == true ? 1 : 0);
	sstrcpyn(cmdSuccess.chStarDesc, pHeroStarData->chStarDesc, sizeof(cmdSuccess.chStarDesc));
	m_pMaster->sendCommandToEntityView( ENTITY_TOVIEW_UPGRADE_SUCESS, 0, 0, &cmdSuccess, sizeof(cmdSuccess) );

    sendHeroInfoUpgradeFrontAndBack(msgInfo->nHeroID, msgInfo->nCurrentStar, msgInfo->nCurrentGold, msgInfo->nCurrentCardCount);
}

void CPlayerBankPart::onMsgWarEndAwarkResult(LPCSTR data, int len)
{
    if ( data == NULL || len < sizeof(msg_entity_warend_awark))
    {
        ErrorLn(__FUNCTION__": data = NULL len = " << len << ", sizeof(msg_entity_warend_awark) = " << sizeof(msg_entity_warend_awark));
        return;
    }
    msg_entity_warend_awark* pmsgAwark = (msg_entity_warend_awark*)data;
    if(pmsgAwark == NULL)
    {
        ErrorLn(__FUNCTION__": pmsgAwark == NULL")
        return;
    }
    if ( len - sizeof(msg_entity_warend_awark) != pmsgAwark->nCount * sizeof(msg_entity_hero_card))
    {
        ErrorLn(__FUNCTION__":length is invalid, len=" << len << ", need_len==" << (sizeof(msg_entity_warend_awark) + pmsgAwark->nCount * sizeof(msg_entity_hero_card)) );
        return;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if( pSchemeCenter == NULL) 
    {
        ErrorLn(__FUNCTION__": pSchemeCenter == NULL ");
        return;
    }
    ISchemeHeroStarData* pStarData = pSchemeCenter->getSchemeHeroStarInfo();   
    ISchemeLevelUpgrade* pUpgradeInfo =pSchemeCenter->getSchemeLevelUpgrade();
    ISchemeHeroCard* pSchemeHeroCard = pSchemeCenter->getSchemeHeroCard();
    if( pStarData == NULL || pUpgradeInfo == NULL || pSchemeHeroCard == NULL)
    {
        ErrorLn(__FUNCTION__": pUpgradeInfo == NULL || pStarData == NULL|| pSchemeHeroCard == NULL");
        return;
    }

    int nHeroID = pmsgAwark->nHeroID;

    obuf obdata;
    cmd_entity_return_player_awark cmdAwark;
    cmdAwark.ulWarID = pmsgAwark->lDBWarID;
    cmdAwark.bIsWin = pmsgAwark->bIsWin;
    cmdAwark.nActorGetExp = pmsgAwark->nActorGetExp;
	cmdAwark.nHeroExp = pmsgAwark->nHeroExp;
	cmdAwark.nHeroOldExp = pmsgAwark->nHeroOldExp;
	cmdAwark.nHeroOldLv = pmsgAwark->nHeroOldLv;
	cmdAwark.nCurrentExp = pmsgAwark->nCurrentExp;
	cmdAwark.nCurrentLv = pmsgAwark->nCurrentLv;
	cmdAwark.nPlayerOldLv = pmsgAwark->nPlayerOldLv;
	cmdAwark.nPlayerNewLv = pmsgAwark->nPlayerNewLv;
	cmdAwark.nGold = pmsgAwark->nGold;
	cmdAwark.nSuccessScore = pmsgAwark->nSuccessScore;
	cmdAwark.nAntiReduceRate = pmsgAwark->nAntiReduceRate;
	cmdAwark.nCount = pmsgAwark->nCount;

	cmdAwark.nActorCurExp = pmsgAwark->nActorCurExp;
	cmdAwark.nActorOldExp = pmsgAwark->nActorOldExp;
	cmdAwark.nActorNeedExp = pUpgradeInfo->GetNextLevelExp( cmdAwark.nPlayerNewLv, ENEXPTYPE_STATICPLAYEREXP);
	cmdAwark.nActorOldNeedExp = pUpgradeInfo->GetNextLevelExp( cmdAwark.nPlayerOldLv, ENEXPTYPE_STATICPLAYEREXP);

	SHeroInfo* pHeroInfo = m_heroMgr.getHeroInfoByID(nHeroID);
	if ( pHeroInfo )
	{
		int nExpTypeID = pStarData->getUpgradeNeedExpTypeID(nHeroID, pHeroInfo->nHeroStar);
        cmdAwark.nHeroNeedExp = pUpgradeInfo->GetNextLevelExp( pmsgAwark->nCurrentLv, ENEXPTYPE_STATICHEROEXP, nExpTypeID);
		cmdAwark.nHeroOldNeedExp = pUpgradeInfo->GetNextLevelExp( pmsgAwark->nHeroOldLv, ENEXPTYPE_STATICHEROEXP, nExpTypeID);
    }
    cmdAwark.nTime = pmsgAwark->dwFirstWinTime;
    if ( pmsgAwark->dwFirstWinTime != 0 )
    {
        updateFirstwinTime(pmsgAwark->dwFirstWinTime);
    }
    obdata.push_back( &cmdAwark, sizeof(cmdAwark));

    msg_entity_hero_card* pCardInfo = (msg_entity_hero_card*)(pmsgAwark + 1);
    for (int i = 0; i < cmdAwark.nCount; ++i, pCardInfo++)
    {
        SHeroCardScheme* pCardScheme = pSchemeHeroCard->getByCardID(pCardInfo->nHeroID);
        if ( pCardScheme == NULL )
            continue;

        cmd_entity_hero_card cmdCard;
        cmdCard.nHeroID = pCardInfo->nHeroID;
        cmdCard.nCount = pCardInfo->nCount;
        string stCardName = pSchemeHeroCard->getUBBNameByCardID(pCardInfo->nHeroID);
        sstrcpyn(cmdCard.chHeroCardName, stCardName.c_str(), sizeof(cmdCard.chHeroCardName));
        sstrcpyn(cmdCard.chHeroCardDesc, pCardScheme->szRemark, sizeof(cmdCard.chHeroCardDesc));
        cmdCard.nCardFrameID = pCardScheme->nCardFrameIcon;

        obdata.push_back( &cmdCard, sizeof(cmdCard));
    }

    m_pMaster->sendCommandToEntityView( ENTITY_TOVIEW_RETURN_PLAYER_AWARK, 0, 0, obdata.data(), obdata.size() );
}

void CPlayerBankPart::onMsgStaticHeroUpgrade(LPCSTR data, int len)
{
    if ( data == NULL || len != sizeof(msg_entity_static_hero_upgrade))
    {
        ErrorLn(__FUNCTION__": len = " << len << ", sizeof(msg_entity_static_hero_upgrade) = " << sizeof(msg_entity_static_hero_upgrade));
        return;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return;
    ISchemePersonModel* pModel = pSchemeCenter->getSchemePersonModel();
    ISchemeHeroStarData* pStarData = pSchemeCenter->getSchemeHeroStarInfo();
    ISchemePersonBaseProp* pBaseProp = pSchemeCenter->getSchemePersonBaseProp();
    if ( pModel == NULL || pStarData == NULL || pBaseProp == NULL)
        return;

    msg_entity_static_hero_upgrade* pInfo = (msg_entity_static_hero_upgrade*)data;
    if( pInfo == NULL) return;

    cmd_entity_statichero_upgrade cmdInfo;

    cmdInfo.nFrontLv = pInfo->nFrontLv;
    cmdInfo.nCurrntLv = pInfo->nCurrntLv;
    cmdInfo.nCurrntExp = pInfo->nCurrntExp;
    cmdInfo.nHeroID = pInfo->nHeroID;
    int nHeroID = pInfo->nHeroID;
    sstrcpyn(cmdInfo.chHeroName, pModel->getHeroName(nHeroID), sizeof(cmdInfo.chHeroName) );

    int nStar = pInfo->nCurrentStarLv;


    SHeroStarSchemeInfo* pStarInfo = pStarData->getHeroStarData(nHeroID, nStar);
    if( pStarInfo == NULL ) return;
    SHeroGrowProInfo* pGrowProInfo = pStarData->getGrowProInfo(nHeroID, nStar);
    SPersonBasePropScheme HeroBaseInfo = pBaseProp->getOneHeroDetailInfo(nHeroID);

    for (int i = 0; i < PERSON_STAR_PROP_COUNT; ++i )
    {
        SPersonStarProp sStarProp = pGrowProInfo->sStarProp[i];
        SPersonBaeProp sBaseProp = HeroBaseInfo.sProp[i];
        //	1、英雄星级部分：
        //      星级总血量＝星级血量+星级血量成长*（熟练等级-1）
        //      星级总血量成长率＝星级血量成长率+星级血量成长率成长*（熟练等级-1）
        //
        //    2、这些属性带到战场内后
        //    战场内英雄实际血量＝基础血量+星级总血量+（血量成长率+星级总血量成长率）*（场内等级-1）
        // 计算各个属性的值， 这里只是计算1级的属性所以做了一些调整
        float fSumGrowRate = (sStarProp.nStarGrowRate + sStarProp.nStarRateGrow * (cmdInfo.nFrontLv - 1) + sBaseProp.fGrowRate)/BASE_IN_MULTIPLE;
        float fSumGrowRateAdd = (sStarProp.nStarGrowRate + sStarProp.nStarRateGrow * (cmdInfo.nCurrntLv - 1) + sBaseProp.fGrowRate)/BASE_IN_MULTIPLE;
        fSumGrowRateAdd -= fSumGrowRate;
        float fSumValue = (sStarProp.nStarBase + sStarProp.nStarGrow * (cmdInfo.nFrontLv - 1) + sBaseProp.nBase)/BASE_IN_MULTIPLE;
        float fSumValueAdd = (sStarProp.nStarBase + sStarProp.nStarGrow * (cmdInfo.nCurrntLv - 1) +sBaseProp.nBase)/BASE_IN_MULTIPLE;
        fSumValueAdd -= fSumValue;
        switch(sBaseProp.nPropID)
        {
        case PROPERTY_BASE_HP:
            {
                cmdInfo.fHP[PT_BASEVALUE] = fSumValue;
                cmdInfo.fHP[PT_GROWRATE] = fSumGrowRate;
                cmdInfo.fHP[PT_BASEVALUEADD] = fSumValueAdd;
                cmdInfo.fHP[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_MP:
            {
                cmdInfo.fMagic[PT_BASEVALUE] = fSumValue;
                cmdInfo.fMagic[PT_GROWRATE] = fSumGrowRate;
                cmdInfo.fMagic[PT_BASEVALUEADD] = fSumValueAdd;
                cmdInfo.fMagic[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_PA:
            {
                cmdInfo.fPA[PT_BASEVALUE] = fSumValue;
                cmdInfo.fPA[PT_GROWRATE] = fSumGrowRate;
                cmdInfo.fPA[PT_BASEVALUEADD] = fSumValueAdd;
                cmdInfo.fPA[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_MA:
            {
                cmdInfo.fMA[PT_BASEVALUE] = fSumValue;
                cmdInfo.fMA[PT_GROWRATE] = fSumGrowRate;
                cmdInfo.fMA[PT_BASEVALUEADD] = fSumValueAdd;
                cmdInfo.fMA[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_PD:
            {
                cmdInfo.fPD[PT_BASEVALUE] = fSumValue;
                cmdInfo.fPD[PT_GROWRATE] = fSumGrowRate;
                cmdInfo.fPD[PT_BASEVALUEADD] = fSumValueAdd;
                cmdInfo.fPD[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_MD:
            {
                cmdInfo.fMD[PT_BASEVALUE] = fSumValue;
                cmdInfo.fMD[PT_GROWRATE] = fSumGrowRate;
                cmdInfo.fMD[PT_BASEVALUEADD] = fSumValueAdd;
                cmdInfo.fMD[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        default:
            break;
        }
    }
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RETURN_STATICHERO_UPGRADE, 0, 0, &cmdInfo, sizeof(cmdInfo));
}

void CPlayerBankPart::onMsgLoginRecvData(LPCSTR data, int len)
{
    if ( data == NULL || len != sizeof(DWORD))
    {
        ErrorLn(__FUNCTION__": len = " << len << ", sizeof(DWORD) = " << sizeof(DWORD));
        return;
    }

    updateFirstwinTime(*(DWORD*)data);
}

void CPlayerBankPart::fillHeroProperty( int nHeroID, int nStar,int nStaticLv, cmd_entity_hero_detail_info& cmdHeroDetailInfo)
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return;
    ISchemePersonBaseProp* pBaseProp = pSchemeCenter->getSchemePersonBaseProp();
    ISchemeHeroStarData* pStarData = pSchemeCenter->getSchemeHeroStarInfo();
    if ( pStarData == NULL || pBaseProp == NULL )
    {
        return;
    }
    SHeroGrowProInfo* pGrowProInfo = pStarData->getGrowProInfo(nHeroID, nStar);
    SPersonBasePropScheme HeroBaseInfo = pBaseProp->getOneHeroDetailInfo(nHeroID);
    for (int i = 0; i < PERSON_STAR_PROP_COUNT; ++i )
    {
        SPersonStarProp sStarProp = pGrowProInfo->sStarProp[i];
        SPersonBaeProp sBaseProp = HeroBaseInfo.sProp[i];
        //	1、英雄星级部分：
        //      星级总血量＝星级血量+星级血量成长*（熟练等级-1）
        //      星级总血量成长率＝星级血量成长率+星级血量成长率成长*（熟练等级-1）
        //
        //    2、这些属性带到战场内后
        //    战场内英雄实际血量＝基础血量+星级总血量+（血量成长率+星级总血量成长率）*（场内等级-1）
        // 计算各个属性的值 上面的公式是原始公式， 这里只是计算1级的属性所以做了一些调整
        float fSumGrowRate = (sStarProp.nStarGrowRate + sStarProp.nStarRateGrow * (nStaticLv - 1) + sBaseProp.fGrowRate)/BASE_IN_MULTIPLE;
        float fSumValue = (sStarProp.nStarBase + sStarProp.nStarGrow * (nStaticLv - 1) + sBaseProp.nBase)/BASE_IN_MULTIPLE;

        // 下一等级的属性值
        float fNextLvGrowRate = (sStarProp.nStarGrowRate + sStarProp.nStarRateGrow * (nStaticLv) + sBaseProp.fGrowRate)/BASE_IN_MULTIPLE;
        float fNextLvSumValue = (sStarProp.nStarBase + sStarProp.nStarGrow * (nStaticLv) + sBaseProp.nBase)/BASE_IN_MULTIPLE;

        switch(sBaseProp.nPropID)
        {
        case PROPERTY_BASE_HP:
            {
                cmdHeroDetailInfo.fHP[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fHP[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fHP[PT_BASEVALUEADD] = fNextLvSumValue;
                cmdHeroDetailInfo.fHP[PT_GROWRATEADD] = fNextLvGrowRate;
            }
            break;
        case PROPERTY_BASE_MP:
            {
                cmdHeroDetailInfo.fMagic[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fMagic[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fMagic[PT_BASEVALUEADD] = fNextLvSumValue;
                cmdHeroDetailInfo.fMagic[PT_GROWRATEADD] = fNextLvGrowRate;
            }
            break;
        case PROPERTY_BASE_PA:
            {
                cmdHeroDetailInfo.fPA[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fPA[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fPA[PT_BASEVALUEADD] = fNextLvSumValue;
                cmdHeroDetailInfo.fPA[PT_GROWRATEADD] = fNextLvGrowRate;
            }
            break;
        case PROPERTY_BASE_MA:
            {
                cmdHeroDetailInfo.fMA[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fMA[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fMA[PT_BASEVALUEADD] = fNextLvSumValue;
                cmdHeroDetailInfo.fMA[PT_GROWRATEADD] = fNextLvGrowRate;
            }
            break;
        case PROPERTY_BASE_PD:
            {
                cmdHeroDetailInfo.fPD[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fPD[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fPD[PT_BASEVALUEADD] = fNextLvSumValue;
                cmdHeroDetailInfo.fPD[PT_GROWRATEADD] = fNextLvGrowRate;
            }
            break;
        case PROPERTY_BASE_MD:
            {
                cmdHeroDetailInfo.fMD[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fMD[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fMD[PT_BASEVALUEADD] = fNextLvSumValue;
                cmdHeroDetailInfo.fMD[PT_GROWRATEADD] = fNextLvGrowRate;
            }
            break;
        default:
            break;
        }
    }
}

void CPlayerBankPart::sendHeroInfoUpgradeFrontAndBack(int nHeroID, int nCurrentStar,int nGold, int nCardCount, int nStaticLv)
{
    // 以下是各种属性的赋值
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return;
    IChatClient *pChatClient = gClientGlobal->getChatClient();
    if( pChatClient == NULL ) 
		return;
	ISchemeHeroStarData* pStarData = pSchemeCenter->getSchemeHeroStarInfo();   
	ISchemePersonModel* pModel = pSchemeCenter->getSchemePersonModel();
	if( pStarData == NULL || pModel == NULL)
		return;

	int nStar	  = nCurrentStar;
	int nNextStar = nStar + 1;
	if (nStar == HERO_STAR_LEVEL_MAX)
		nNextStar = HERO_STAR_LEVEL_MAX;

    ISchemePersonBaseProp* pBaseProp = pSchemeCenter->getSchemePersonBaseProp();
    SHeroStarSchemeInfo* pStarInfo = pStarData->getHeroStarData(nHeroID, nStar);
    SHeroGrowProInfo* pGrowProInfo = pStarData->getGrowProInfo(nHeroID, nStar);
    SHeroStarSchemeInfo* pStarInfoAdd = pStarData->getHeroStarData(nHeroID, nNextStar);
    SHeroGrowProInfo* pGrowProInfoAdd = pStarData->getGrowProInfo(nHeroID, nNextStar);
    SPersonBasePropScheme HeroBaseInfo = pBaseProp->getOneHeroDetailInfo(nHeroID);
    if ( pBaseProp == NULL || pStarInfo  == NULL || pStarInfoAdd  == NULL )
        return;

    DBREQ_PARAM_HERO_CARD& cardInfo= m_heroMgr.getHeroCardInfo(pStarInfo->dwNeedCardID);

    cmd_entity_hero_diff_expend cmdHeroDetailInfo;
    cmdHeroDetailInfo.nHeroID = nHeroID;
    cmdHeroDetailInfo.nStarLv = nStar;

	cmdHeroDetailInfo.nDiscountGold = pStarInfo->nGoldDiscount;
    cmdHeroDetailInfo.nExpedGold = pStarInfo->nExpendGold;
    IEntityProperty *pEntityProperty = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
    if (pEntityProperty == NULL)
        return;
    if ( nGold == 0 )
		cmdHeroDetailInfo.nHaveGold = pEntityProperty->getProperty_Integer(PROPERTY_GOLD);
    else
        cmdHeroDetailInfo.nHaveGold = nGold;

	cmdHeroDetailInfo.nDiscountCard = pStarInfo->nCardDiscount;
    cmdHeroDetailInfo.nExpendCard = pStarInfo->nNeedCount;
    if ( nCardCount == 0)
        cmdHeroDetailInfo.nHaveCardCount = cardInfo.nCardCount;
    else
        cmdHeroDetailInfo.nHaveCardCount = nCardCount;

    cmdHeroDetailInfo.nLimitLv = pStarInfo->nLimitLV;
    cmdHeroDetailInfo.nLimitLvAdd = pStarInfoAdd->nLimitLV - pStarInfo->nLimitLV;
    sstrcpyn(cmdHeroDetailInfo.chHeroName, pModel->getHeroName(nHeroID), sizeof(cmdHeroDetailInfo.chHeroName) );
    for (int i = 0; i < PERSON_STAR_PROP_COUNT; ++i )
    {
        SPersonStarProp sStarProp = pGrowProInfo->sStarProp[i];
        SPersonStarProp sStarPropAdd = pGrowProInfoAdd->sStarProp[i];
        SPersonBaeProp sBaseProp = HeroBaseInfo.sProp[i];
        //	1、英雄星级部分：
        //      星级总血量＝星级血量+星级血量成长*（熟练等级-1）
        //      星级总血量成长率＝星级血量成长率+星级血量成长率成长*（熟练等级-1）
        //
        //    2、这些属性带到战场内后
        //    战场内英雄实际血量＝基础血量+星级总血量+（血量成长率+星级总血量成长率）*（场内等级-1）
        // 计算各个属性的值， 这里只是计算1级的属性所以做了一些调整
        float fSumGrowRate = (sStarProp.nStarGrowRate + sStarProp.nStarRateGrow * (nStaticLv - 1) + sBaseProp.fGrowRate)/BASE_IN_MULTIPLE;
        float fSumGrowRateAdd = (sStarPropAdd.nStarGrowRate + sStarPropAdd.nStarRateGrow * (nStaticLv - 1)+ sBaseProp.fGrowRate)/BASE_IN_MULTIPLE;
        fSumGrowRateAdd -= fSumGrowRate;
        float fSumValue = (sStarProp.nStarBase + sStarProp.nStarGrow * (nStaticLv - 1) + sBaseProp.nBase)/BASE_IN_MULTIPLE;
        float fSumValueAdd = (sStarPropAdd.nStarBase + sStarPropAdd.nStarGrow * (nStaticLv - 1) + sBaseProp.nBase)/BASE_IN_MULTIPLE;
        fSumValueAdd -= fSumValue;
        switch(sBaseProp.nPropID)
        {
        case PROPERTY_BASE_HP:
            {
                cmdHeroDetailInfo.fHP[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fHP[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fHP[PT_BASEVALUEADD] = fSumValueAdd;
                cmdHeroDetailInfo.fHP[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_MP:
            {
                cmdHeroDetailInfo.fMagic[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fMagic[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fMagic[PT_BASEVALUEADD] = fSumValueAdd;
                cmdHeroDetailInfo.fMagic[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_PA:
            {
                cmdHeroDetailInfo.fPA[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fPA[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fPA[PT_BASEVALUEADD] = fSumValueAdd;
                cmdHeroDetailInfo.fPA[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_MA:
            {
                cmdHeroDetailInfo.fMA[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fMA[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fMA[PT_BASEVALUEADD] = fSumValueAdd;
                cmdHeroDetailInfo.fMA[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_PD:
            {
                cmdHeroDetailInfo.fPD[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fPD[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fPD[PT_BASEVALUEADD] = fSumValueAdd;
                cmdHeroDetailInfo.fPD[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        case PROPERTY_BASE_MD:
            {
                cmdHeroDetailInfo.fMD[PT_BASEVALUE] = fSumValue;
                cmdHeroDetailInfo.fMD[PT_GROWRATE] = fSumGrowRate;
                cmdHeroDetailInfo.fMD[PT_BASEVALUEADD] = fSumValueAdd;
                cmdHeroDetailInfo.fMD[PT_GROWRATEADD] = fSumGrowRateAdd;
            }
            break;
        default:
            break;
        }
    }
    obuf obData;
    obData << cmdHeroDetailInfo;
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPGRADESTARTAPROANDEXPEND,0, 0, obData.data(), obData.size());
}

int CPlayerBankPart::fillHeroInfo(msg_entity_hero_info* pMsgHeroInfo, int nCount, obuf& obData, PDBID nPDBID)
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
        return 0;
    ISchemePersonModel* pPerson = pSchemeCenter->getSchemePersonModel();
    ISchemeHeroStarData* pHeroStarInfo = pSchemeCenter->getSchemeHeroStarInfo();
    ISchemeLevelUpgrade* pLvUpgrade = pSchemeCenter->getSchemeLevelUpgrade();
    ISchemeHeroAchievementLvConfig* pAchievementLv = pSchemeCenter->getSchemeHeroAchievementLv();
	ISchemeHeroFreeWeekly* pHeroFreeWeekly = pSchemeCenter->getSchemeHeroFreeWeekly();
	IEntityProperty *pEntityProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
    if( pHeroStarInfo == NULL || pPerson == NULL || pLvUpgrade == NULL || pAchievementLv == NULL || pHeroFreeWeekly == NULL || pEntityProperty == NULL)
        return 0;

	// 返回自身数据，特殊处理部分
	PDBID dwPDBID = gClientGlobal->getHero()->getIntProperty(PROPERTY_ID);
	int nSelfUseHeroID = dwPDBID == nPDBID ? pEntityProperty->getProperty_Integer(PROPERTY_VOCATION) : 0;

	int nRealCount = 0;
    int nHeroId,nStarLv,nHeroLv;
    cmd_entity_hero_info heroInfo;
    const int SKIN_ARRAY_SIZE = 100;
	//PersonSkinModelItem* arrSkinMode[SKIN_ARRAY_SIZE] = {0};
    for (int i = 0; i < nCount; ++i )
    {
        nHeroId = pMsgHeroInfo->nHeroID;
        nStarLv = pMsgHeroInfo->byStarLv;
        nHeroLv = pMsgHeroInfo->nLv;
        SHeroStarSchemeInfo* pStarInfo = pHeroStarInfo->getHeroStarData(nHeroId, nStarLv);
        if( pStarInfo == NULL )
        {
            pMsgHeroInfo++;
            continue;
        }
        DWORD dwCardID = pStarInfo->dwNeedCardID;

        const PersonSkinModelItem*  pPersonSkinItem = pPerson->getPersonSkinModelItem(nHeroId);
        if (!pPersonSkinItem)
        {
			pMsgHeroInfo++;
            continue;
        }

        memset( &heroInfo, 0, sizeof(heroInfo) );
        heroInfo.nHeroID = pMsgHeroInfo->nHeroID;
        sstrcpyn(heroInfo.chHeroName, pPerson->getHeroName(nHeroId), sizeof(heroInfo.chHeroName));
        heroInfo.nLv = nHeroLv;
        int nExpTypeID =pStarInfo->nExpType;
        heroInfo.ulUpNeedExp = pLvUpgrade->GetNextLevelExp(nHeroLv, ENEXPTYPE_STATICHEROEXP, nExpTypeID);
        heroInfo.ulExp = pMsgHeroInfo->dwExp;
        heroInfo.byStarLv = nStarLv;
        heroInfo.usWinPro = pMsgHeroInfo->nWinPro;
        heroInfo.nGameCount = pMsgHeroInfo->dwGameCount;
        heroInfo.ulScore = pMsgHeroInfo->dwScore;
        heroInfo.nCount = pAchievementLv->getIconIDAccordScore(heroInfo.ulScore, heroInfo.nIconID, ICONGROUP_COUNT_MAX);
        heroInfo.nRank = pMsgHeroInfo->nRank;
        heroInfo.ulUseTime = pMsgHeroInfo->dwUseTime;
        heroInfo.nNeedCardCount = pStarInfo->nNeedCount;
		heroInfo.nDiscountCardCount = pStarInfo->nCardDiscount;
		heroInfo.bIsBuy = pMsgHeroInfo->bIsBuy ? 1 : 0;
		heroInfo.nUseXPSkillID = pMsgHeroInfo->nUseXPSkillID;
		heroInfo.ulUnlockTime = pMsgHeroInfo->dwUnlockTime;
		// 返回自身数据，特殊处理部分
		if (dwPDBID == nPDBID)
		{
			int nPlayerLv = gClientGlobal->getHero()->getIntProperty(PROPERTY_LEVEL);
			heroInfo.bIsWeekFree = pHeroFreeWeekly->CheckIsWeekFree(nHeroId, nPlayerLv) ? 1 : 0;

			DBREQ_PARAM_HERO_CARD dbParamCard = m_heroMgr.getHeroCardInfo(dwCardID);
			heroInfo.nCardCount = dbParamCard.nCardCount;
			heroInfo.bIsUse = (nSelfUseHeroID == pMsgHeroInfo->nHeroID) ? 1 : 0;

			//  此处临时处理， 只要要把数据统一一下
			SHeroInfo* pHeroInfo = m_heroMgr.getHeroInfoByID(nHeroId);
			if ( pHeroInfo != NULL )
			{
				pHeroInfo->nHeroStar = nStarLv;
			}
		}

		obData.push_back(&heroInfo, sizeof(heroInfo));
        pMsgHeroInfo++;
		nRealCount++;
    }
	return nRealCount;
}

void CPlayerBankPart::fillNoHaveHeroInfo(map<int, msg_entity_hero_info *> & m_tmpHero, obuf& obData, PDBID nPDBID)
{
	PDBID dwPDBID = gClientGlobal->getHero()->getIntProperty(PROPERTY_ID);

	ISchemePersonModel* pPersonModel = gClientGlobal->getSchemeCenter()->getSchemePersonModel();
	ISchemeHeroStarData* pHeroStarData = gClientGlobal->getSchemeCenter()->getSchemeHeroStarInfo();
	if( pPersonModel == NULL || pHeroStarData  == NULL )
		return;

	PersonSkinModelItem arrVocation[VOCATION_MAX];
	int nCount = pPersonModel->getAllVocation(arrVocation, VOCATION_MAX);

	cmd_count cmdCount;
	obuf obHeroData;
	for (int i=0; i<nCount; ++i)
	{
		PersonSkinModelItem & node = arrVocation[i];

		map<int, msg_entity_hero_info *>::iterator iterHero = m_tmpHero.find(node.nHeroID);
		if(iterHero != m_tmpHero.end())
		{
			continue;
		}

		SHeroStarSchemeInfo* pStarInfo = pHeroStarData->getHeroStarData( node.nHeroID);
		if( pStarInfo == NULL )
		{
			ErrorLn(__FUNCTION__": HeroStarData.csv can't find heroId = " << node.nHeroID );
			continue;
		}

		cmdCount.nCount++;
		cmd_entity_nohanve_hero cmdHero;
		cmdHero.nHeroID = node.nHeroID;
		cmdHero.nNeedCardCount = pStarInfo->nNeedCount;
		cmdHero.nDiscountCardCount = pStarInfo->nCardDiscount;
		sstrcpyn( cmdHero.chHeroName, node.szHeroName, sizeof(cmdHero.chHeroName));

		// 返回自身数据，特殊处理部分
		if (dwPDBID == nPDBID)
		{
			DBREQ_PARAM_HERO_CARD dbCardInfo = m_heroMgr.getHeroCardInfo(pStarInfo->dwNeedCardID);
			cmdHero.nHaveCardCount = dbCardInfo.nCardCount;
		}

		obHeroData.push_back( &cmdHero, sizeof(cmdHero));
	}
	obData << cmdCount;
	obData.push_back( obHeroData.data(), obHeroData.size());
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

void CPlayerBankPart:: loadHeroSkillInfo( int nHeroID, obuf& obData )
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if( pSchemeCenter == NULL )
        return;

    ISchemeSpellSlot* pSpellSlot = pSchemeCenter->getSchemeSpellSlot();
    if ( pSpellSlot == NULL )
        return;

    ISpellFactory* pSpellFactory = gClientGlobal->getSpellFactory();
    if( pSpellFactory == NULL )
        return;

    SPELL_SLOT* pSpellSlotData=pSpellSlot->getSpellSlot(nHeroID);
    if(pSpellSlotData==NULL)
        return;


    cmd_count cmdCount;
    cmdCount.nCount = SPELL_SLOT_SHOW_COUNT;
    obData << cmdCount;
    obuf obSpellData;

    for ( int i = 0; i < SPELL_SLOT_SHOW_COUNT; ++i )
    {
        int nSpellID = pSpellSlotData->nSpellID[i];
        SPELL_DATA* pSpellData = pSpellFactory->getSpellData(nSpellID);
        if( pSpellData == NULL )
            continue;
        cmd_match_room_hero_skill_desc cmdHeroSkillDesc;

        cmdHeroSkillDesc.nSpellIconID=pSpellData->nIcon;

        sstrcpyn(cmdHeroSkillDesc.heroSkillDes, pSpellSlotData->nSpellDesc[i], sizeof(cmdHeroSkillDesc.heroSkillDes));
        sstrcpyn(cmdHeroSkillDesc.heroSkillName, a2utf8(pSpellData->szName), sizeof(cmdHeroSkillDesc.heroSkillName));
        obData.push_back(&cmdHeroSkillDesc, sizeof(cmd_match_room_hero_skill_desc));
    }
}

void CPlayerBankPart::loadPlayerGlory()
{
    ISchemePlayerGloryInfo* pGloryInfo = gClientGlobal->getSchemeCenter()->getSchemeGloryInfo();
    if( pGloryInfo == NULL )
        return;
    std::map<int, SGlorySchemeInfo>* pMapGlorySchemeInfo = pGloryInfo->getGlorySchemeInfo();
    if ( pMapGlorySchemeInfo == NULL )
        return;

    std::map<int, SGlorySchemeInfo>::iterator itGloryBegin = pMapGlorySchemeInfo->begin();
    for (; itGloryBegin != pMapGlorySchemeInfo->end(); ++itGloryBegin)
    {
        SGlorySchemeInfo sGloryInfo = itGloryBegin->second;
        std::map<int,long>::iterator itCondition = sGloryInfo.mapCondition.begin();
        int nType = 0;
        for (; itCondition != sGloryInfo.mapCondition.end(); ++itCondition )
        {
            nType = itCondition->first;
			if ((int)m_playInfo.player.byGlory[nType*12] < itCondition->second)
                break;
        }
        if ( itCondition == sGloryInfo.mapCondition.end() )
            m_mapGloryInfo[itGloryBegin->first] = sGloryInfo;
    }
}

string CPlayerBankPart::getMostGlory()
{
    if (m_mapGloryInfo.size() == 0)
        return "";
    std::map<int,SGlorySchemeInfo>::iterator itGlory = m_mapGloryInfo.begin();
    int nTmpValue = 0;
    int nMostGloryID = 0;
    string stMostGlory = "";
    for (; itGlory != m_mapGloryInfo.end(); ++itGlory )
    {
        SGlorySchemeInfo sGloryScheme = itGlory->second;
        int nDifficulty = sGloryScheme.nDifficulty;
        if ( nDifficulty < nTmpValue )
            continue;
        nTmpValue = nDifficulty;
        nMostGloryID = itGlory->first;
        stMostGlory = sGloryScheme.stDesc;
    }
    return stMostGlory;
}

// 更新首胜时间
void CPlayerBankPart::updateFirstwinTime(DWORD time)
{
    cmd_entity_firstwin_notice cmd;
    cmd.nLeftTime = time;
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_FIRSTWIN_NOTICE, sizeof(cmd), 0, &cmd, sizeof(cmd));
}

bool CPlayerBankPart::isSystemHero(int nHeroID)
{
    return m_heroMgr.isSystemHero(nHeroID);
}

// 更新新手引导数据
void CPlayerBankPart::updateNoviceGuideData(DWORD dwNoviceGuideType)
{
	m_nPlayerGuideTypeData = dwNoviceGuideType;
	cmd_entity_update_voviceguidedata cmd;
	cmd.guidedata = dwNoviceGuideType;
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_NOVICEGUIDEDATA, sizeof(cmd), 0, &cmd, sizeof(cmd));
}

void CPlayerBankPart::OnMsgHeroTalentInfo(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL)
    {
        return;
    }

    if(m_pMaster == NULL)
    {
        return;
    }

    int nEfficacyLen=sizeof(int);
    if(nLen<nEfficacyLen)
    {
        ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< int Size= "<<nEfficacyLen);
        return ;
    }

    int* pHeroInfoNum =(int*)pszMsg;
    char *pHead=(char*)(pHeroInfoNum+1);

    obuf obData;
    cmd_count heroCount;
    heroCount.nCount = *pHeroInfoNum;
    obData.push_back(&heroCount,sizeof(cmd_count));
    // 有时间记得做效验
    for (int nTalentIndex = 0; nTalentIndex < heroCount.nCount; ++nTalentIndex)
    {		
        int nTalentCount  = (*pHead);
        pHead = pHead+sizeof(int);
        int nHeroId = (*pHead);
        pHead = pHead+sizeof(int);

        cmd_count talentCount;
        talentCount.nCount = nTalentCount;
        obData.push_back(&talentCount,sizeof(cmd_count));

        int nSingleLen = sizeof(HERO_TALENT_INFO);
        for (int i=0;i<nTalentCount;++i )
        {
            HERO_TALENT_INFO* talentInfo = (HERO_TALENT_INFO*)(pHead);
            cmd_Hero_Talent_Info cmdHeroTalentInfo;
            cmdHeroTalentInfo.nHeroId = nHeroId;
            cmdHeroTalentInfo.nTalentId = talentInfo->dwTalentID;
            cmdHeroTalentInfo.nActive = talentInfo->bActive;

            obData.push_back(&cmdHeroTalentInfo, sizeof(cmd_Hero_Talent_Info));
            pHead = pHead+nSingleLen;
        }
    }

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HERO_TALENTINFO,0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgUpdateActiveHeroTalentInfo(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL)
    {
        return;
    }

    if(m_pMaster == NULL)
    {
        return;
    }

    int nEfficacyLen=sizeof(int)+sizeof(HERO_TALENT_INFO);
    if(nLen<nEfficacyLen)
    {
        ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< int Size= "<<nEfficacyLen);
        return ;
    }

    int* pHeroId = (int*)pszMsg;
    HERO_TALENT_INFO* talentInfo = (HERO_TALENT_INFO*)(pHeroId+1);

    if(!talentInfo->bActive)
    {
        ErrorLn(__FUNCTION__":Hero Talent Is UnActive , Update Talent Is Fail");
        return;
    }

    cmd_Hero_Talent_Info cmdHeroTalentInfo;
    cmdHeroTalentInfo.nHeroId = *pHeroId;
    cmdHeroTalentInfo.nTalentId = talentInfo->dwTalentID;
    cmdHeroTalentInfo.nActive = 1;

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HERO_TALENTUPDATE,0, 0, &cmdHeroTalentInfo, sizeof(cmd_Hero_Talent_Info));
}

void CPlayerBankPart::OnMsgHeroSkin(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL)
    {
        return;
    }

    if(m_pMaster == NULL)
    {
        return;
    }

    int nEfficacyLen=sizeof(int);
    if(nLen<nEfficacyLen)
    {
        ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< int Size= "<<nEfficacyLen);
        return ;
    }

    int* pHeroInfoNum =(int*)pszMsg;
    char *pHead=(char*)(pHeroInfoNum+1);

    obuf obData;
    cmd_count heroCount;
    heroCount.nCount = *pHeroInfoNum;
    obData.push_back(&heroCount,sizeof(cmd_count));
    int nSkinCmdItemLen = sizeof(cmd_Hero_Skin_Info);
    int nSkinItemLen =  sizeof(HERO_SKIN_INFO);
    for (int nIndex = 0; nIndex < heroCount.nCount; ++nIndex)
    {		
        int nSkinCount  = (*pHead);
        pHead = pHead+sizeof(int);
        int nHeroId = (*pHead);
        pHead = pHead+sizeof(int);

        cmd_count skinCount;
        skinCount.nCount = nSkinCount;
        obData.push_back(&skinCount,sizeof(cmd_count));

        for (int i=0;i<nSkinCount;++i )
        {
            HERO_SKIN_INFO* pItem = (HERO_SKIN_INFO*)(pHead);
            cmd_Hero_Skin_Info cmdHeroSkin;
            cmdHeroSkin.nHeroId = nHeroId;
            cmdHeroSkin.nSkinId = pItem->dwSkinID;
            cmdHeroSkin.nSkinLevel = 0;
            cmdHeroSkin.dwDurationTime = pItem->dwDurationTime;

            obData.push_back(&cmdHeroSkin, nSkinCmdItemLen);
            pHead += nSkinItemLen;
        }
    }

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HERO_SKIN_INFO,0, 0, obData.data(), obData.size());
}

void CPlayerBankPart::OnMsgUpdateHeroSkin(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL)
	{
		return;
	}

	if(m_pMaster == NULL)
	{
		return;
	}

	int nEfficacyLen=sizeof(int);
	if(nLen<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< int Size= "<<nEfficacyLen);
		return ;
	}
	int *pHeroId = ((int*)pszMsg);

    HERO_SKIN_INFO* pItem = (HERO_SKIN_INFO*)(pHeroId+1);

    cmd_Hero_Skin_Info cmdHeroSkin;
     cmdHeroSkin.nHeroId = *pHeroId;
    cmdHeroSkin.nSkinId = pItem->dwSkinID;
    cmdHeroSkin.nSkinLevel = 0;
    cmdHeroSkin.dwDurationTime = pItem->dwDurationTime;

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_HERO_SKIN_UPDATE,0, 0, &cmdHeroSkin, sizeof(cmd_Hero_Skin_Info));
}

void CPlayerBankPart::onMsgActorRenameResult(LPCSTR pszMsg,  int nLen )
{
	if (pszMsg == NULL)
	{
		return;
	}

	if(m_pMaster == NULL)
	{
		return;
	}

	int nEfficacyLen=sizeof(bool);
	if(nLen<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< int Size= "<<nEfficacyLen);
		return ;
	}
	bool  bRenameResult= *(bool*)pszMsg;
	int nRenameResult = bRenameResult?1:0;
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RENAME_RESUNLE,nRenameResult, 0, 0, 0);
}

void CPlayerBankPart::onMsgBenefitCardInfo( LPCSTR pszMsg, int nLen )
{
	if (NULL == pszMsg)
	{
		return;
	}

	if (NULL == m_pMaster)
	{
		return;
	}

	if (nLen < sizeof(msg_entity_benefitcard_info))
	{
		return;
	}

	msg_entity_benefitcard_info *pInfo = (msg_entity_benefitcard_info*)pszMsg;
	
	int nListType = pInfo->nListType;
	int nCount = pInfo->nCount;
	if (pInfo->nCount <= 0)
	{
		TraceLn(__FUNCTION__ " The number of benefitcard is 0. nCount=" << nCount);
		return;
	}

	if(nLen != (sizeof(msg_entity_benefitcard_info) + (nCount * sizeof(BenefitCardInfo))))
	{
		ErrorLn(__FUNCTION__ " nLen isn't matched with nCount, nCount=" << nCount);
		return;
	}

	cmd_benefit_card_list_head cardListHead;
	cardListHead.nCount = nCount;
	cardListHead.nListType = pInfo->nListType;

	BenefitCardInfo *pBenefitCardInfo = (BenefitCardInfo*)(pszMsg + sizeof(msg_entity_benefitcard_info));
	obuf ob;
	ob << cardListHead;
	for (int i = 0; i < nCount; ++i)
	{
		cmd_benefit_card cardInfo;
		cardInfo.nBenefitCardType = pBenefitCardInfo->nBenefitCardType;
		cardInfo.nBenefitCardSubType = pBenefitCardInfo->nBenefitCardSubType;
		cardInfo.nBenefitValue = pBenefitCardInfo->nBenefitValue;
		cardInfo.nMultiple = pBenefitCardInfo->nMultiple;

		ob.push_back(&cardInfo, sizeof(cardInfo));

		pBenefitCardInfo++;
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_BENEFITCARD, 0, NULL, ob.data(), ob.size());
}

// 是否升级特殊特效
bool CPlayerBankPart::isGradeSuccessSpecialEffect(int nHeroID, int nCurStarLv, int& nOldStarIconID, int& nCurStarIconID)
{
	ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
	if( pSchemeCenter == NULL) 
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL ");
		return false;
	}
	ISchemeHeroStarData* pStarData = pSchemeCenter->getSchemeHeroStarInfo(); 
	if (pStarData == NULL)
	{
		return false;
	}

	int nOldStarLv = nCurStarLv - 1;
	if (nOldStarLv <= 0)
	{
		return false;
	}

	SHeroStarSchemeInfo *pOldData = pStarData->getHeroStarData(nHeroID, nOldStarLv);
	if (pOldData == NULL)
	{
		return false;
	}

	SHeroStarSchemeInfo *pCurData = pStarData->getHeroStarData(nHeroID, nCurStarLv);
	if (pCurData == NULL)
	{
		return false;
	}

	if (pOldData->nStarIconID != pCurData->nStarIconID)
	{
		nOldStarIconID = pOldData->nStarIconID;
		nCurStarIconID = pCurData->nStarIconID;
		return true;
	}

	return false;

}

int CPlayerBankPart::getPlayerNoviceGuideTypeData()
{
    return m_nPlayerGuideTypeData;
}

/* 获取本命英雄引导完成情况数据，表示是否走完本命英雄的引导流程
* Return: 1表示已走完完所有本命英雄，0表示还未完成
*/
int CPlayerBankPart::getLifeHeroTaskData()
{
	return m_nLifeHeroTaskData;
}

void CPlayerBankPart::restore()
{

}

void CPlayerBankPart::onMsgRankSeasonDetail(LPCSTR data, int len)
{
	if (data == NULL || len != sizeof(msg_entity_season_detail))
	{
		return;
	}

	msg_entity_season_detail * pDetail = (msg_entity_season_detail*)data;

	cmd_entity_rank_season_detail cmdDetail;

	cmdDetail.dwEndTime = pDetail->dwEndTime;

	cmdDetail.wTopRankGrade = pDetail->wTopRankGrade;
	cmdDetail.wMatchCount = pDetail->wMatchCount;
	cmdDetail.wWinCount = pDetail->wWinCount;
	cmdDetail.wContinueWinCount = pDetail->wContinueWinCount;
	cmdDetail.wMvpCount = pDetail->wMvpCount;
	cmdDetail.wSuperGodCount = pDetail->wSuperGodCount;
	cmdDetail.wKill5Count = pDetail->wKill5Count;
	cmdDetail.wKill4Count = pDetail->wKill4Count;
	cmdDetail.wKill3Count = pDetail->wKill3Count;

	cmdDetail.wPrizeConfigID = pDetail->wPrizeConfigID;

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RANK_SEASON_DETAIL,0, 0, &cmdDetail, sizeof(cmdDetail));
}

void CPlayerBankPart::onMsgRankSeasonRecord(LPCSTR data, int len)
{
	if (data == NULL || len < sizeof(int))
	{
		return;
	}
	obuf obCmd;

	int * pCount = (int *)data;
	int nCount = *pCount;
	obCmd << nCount;

	msg_entity_season_record_node * pNode = (msg_entity_season_record_node *)(pCount + 1);
	for (int i = 0; i < nCount; ++i)
	{
		cmd_entity_rank_season_record_node node;
		node.wSeasonID = pNode->wSeasonID;
		node.wRankGrade = pNode->wRankGrade;
		node.wMatchCount = pNode->wMatchCount;
		node.wWinCount = pNode->wWinCount;
		for (int j = 0; j < MATCH_SEASON_HERO_RECORD_COUNT; ++j)
		{
			node.wHeroRecord[j] = pNode->wHeroRecord[j];
		}
		memcpy(node.szSeasonTitle, pNode->szSeasonTitle, sizeof(node.szSeasonTitle));
		node.nBeginTime = pNode->dwSeasonBeginTime;
		node.nEndTime = pNode->dwSeasonEndTime;
		obCmd << node;

		pNode++;
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RANK_SEASON_RECORD,0, 0, obCmd.data(), obCmd.size());
}
