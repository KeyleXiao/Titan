/*******************************************************************
** 文件名:	e:\Rocket\Client\WarClient\Src\WarPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/7/28  21:25
** 版  本:	1.0
** 描  述:	战场部件
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "WarPart.h"
#include "WarClient.h"
#include "ExternalFacade.h"
#include "EventDef.h"
#include "SpellDef.h"
#include "IChatClient.h"
#include "IEntityProperty.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "Entity_inl.h"
#include "MatchSchemeDef.h"
#include "TimeSyncClient.h"
#include "SchemeDef.h"
#include "WarStaticMessageDef.h"
#include "ISpell.h"
#include "WarCheckBadBehavior.h"
#include "GameAPI.h"

using namespace SPELL;
using namespace War;
/** 
@param   
@param   
@return  
*/
CWarPart::CWarPart(void)
{
	// 所属实体
	m_pMaster = NULL;
	m_WarID = -1;
	m_TabIsOpen = false;
	m_IsOnceRequest = false;
	m_WarIsEnd = false;
	m_PosTime = gClientGlobal->getTimeSyncer()->GetTick();
	m_PosNum = 0;
	m_CanPosSignal = true;
	m_bBaseInfoIsChange = false;
	//gClientGlobal->getTimerAxis()->SetTimer( ETIMER_TABISOPEN, 500, this );
	m_pCheckBadBehavior = NULL;
	m_nBottingCount = 0;
	m_nSummerID = 0;
	m_bCheckBadBehavior = false;
}

/** 
@param   
@param   
@return  
*/
CWarPart::~CWarPart(void)
{
	//gClientGlobal->getTimerAxis()->KillTimer( ETIMER_TABISOPEN,this );
}
//////////////////////ITimerHandler////////////////////////////////////
void CWarPart::OnTimer( unsigned long dwTimerID )
{

	/*switch( dwTimerID )
	{
	case ETIMER_TABISOPEN:
	{

	SendWarPersonInfo();
	}
	break;
	default:
	{

	}
	break;
	}*/
}

///////////////////ISerializable////////////////////////////////////
/** 序列化支持接口
@return  
*/
bool CWarPart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
    in >> m_WarID;
	if (flag & TO_CLIENT_SELF)
	{
		// 在战场内
        bool bHaveData = false;
        in>> bHaveData;
        if (bHaveData)
        {
			in >> m_bCheckBadBehavior;
            in >> m_nBottingCount;
			in >> m_nSummerID;
        }
	}

	return true;
}

/** 反序列化支持接口
@return  
*/
bool CWarPart::deSerialize( rkt::obuf & out,int flag )
{
	return true;
}


///////////////////IEntityPart////////////////////////////////////
/** 取是部件ID
@param   
@param   
@return  
*/
short CWarPart::getPartID(void)
{
	return PART_WAR;
}

/** 创建，重新启用，也会调此接口
@param   
@param   
@return  
*/
bool CWarPart::onLoad( IEntity* pEntity)
{
	// 所属实体
	m_pMaster = pEntity;
	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
	{
		ErrorLn("War part onLoad pEventEngine==NULL");
		return false;
	}
	// 只有自己的英雄注册
	if (m_pMaster->isHero())
	{
		gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_WAR, this);
        gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_STATICWAR, this);
		//对主英雄注册实体创建事件
		UID uid = m_pMaster->getUID();
		BYTE type  = UID_2_TYPE(uid);

		pEventEngine->Subscibe(this, EVENT_ENTITY_CREATE_VIEW, type, uid,__FILE__);


		if (m_WarID > 0 && m_bCheckBadBehavior)
		{
			// 检测是否开启恶意行为检测

			m_pCheckBadBehavior = new CWarCheckBadBehavior(m_pMaster);
			if (!m_pCheckBadBehavior->Init(m_nBottingCount))
			{
				m_pCheckBadBehavior->Release();
				m_pCheckBadBehavior = NULL;
			}
		}

	}

	return true;

}

/** 释放,会释放内存
@param   
@param   
@return  
*/
void CWarPart::release(void)
{
	if ( m_pMaster )
	{
		UID uid = m_pMaster->getUID();
		BYTE type  = UID_2_TYPE(uid);

		// 只有自己的英雄注册
		if ( m_pMaster->getTypeMask()&MASK_ACTOR && ((IClientEntity*)m_pMaster)->isHero())
		{
			// 取消注册的模块消息
			gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_WAR, this);
            gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_STATICWAR, this);

			// 取消注册的事件
			gClientGlobal->getEventEngine()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE_VIEW, type, uid);
		}
	}

	// 所属实体
	m_pMaster = NULL;

	if (m_pCheckBadBehavior != NULL)
	{
		m_pCheckBadBehavior->Release();
		m_pCheckBadBehavior = NULL;
	}


	delete this;
}


bool CWarPart::onWarStaticMessageID(int msgId,void * data,size_t len)
{
    switch( msgId )
    {

    case SC_MSG_WAR_SEND_WARSTATICS_TOTAL:
        {
            OnMessageSendWarStaticsTotal(data, len);
        }
        break;
    case SC_MSG_WAR_SEND_WARSTATICS_WAR:
        {
            OnMessageSendWarStaticsWar(data, len);
        }
        break;
	case SC_MSG_WAR_SEND_HISTORY_RECORD:
		{
			OnMessageSendHistoryRecord(data, len);
		}
		break;
	case SC_MSG_WAR_OTHER_CROWN_PAGE_INLAY: // 其他玩家的皇冠页信息
		{
			OnMessageOtherCrownPageInfo(data, len);
		}
		break;
    default:
        break;
    }
    return true;
}


/** 发送命令,命令与事件不同，命令是外界通知实体做某件事情，实体做了，再发出事件
@param   
@param   
@return  
*/
bool CWarPart::onWarMessageID( int msgId,void * data,size_t len )
{
	switch( msgId )
	{
	case SC_MSG_WAR_NOTIFY_WAR_INFO:
		{
			// 服务器消息战场数据
			OnMessageNotifyWarInfo(data, len);
		}
		break;

    case SC_MSG_WAR_END_LEGENDCUP_INFO:
        {
            // 服务器杯赛结束提示信息
            OnMessageWarEndLegendCupInfo(data, len);
        }
        break;

	case SC_MSG_WAR_UPDATE_PERSONTAB:
		{
			// 服务器消息更新人物数据
			OnMessageUpdateTab(data, len);
		}
		break;
	case SC_MSG_WAR_PERSONINFO:
		{
			// 服务器消息更新人物数据
			OnMessageUpdatePersonInfo(data, len);
		}
		break;
	case SC_MSG_WAR_MONSTER_TIME:
		{
			// 服务器消息更新怪物事件数据
			OnMessageUpdateMonsterTime(data, len);
		}
		break;
	case SC_MSG_WAR_IMPORTANT_ENTITYDIE:
		{
			// 广播重要实体死亡
			OnMessageBroadImprotantEntityDie(data, len);
		}
		break;
	case SC_MSG_WAR_KILL_COUNT:
		{
			// 广播连杀
			OnMessageBroadKillCn(data, len);
		}
		break;
	case SC_MSG_WAR_END_INFO: 
		{
			// 战场结束信息
			OnMessageWarEndhandle(data, len);
		}
		break;

	case SC_MSG_WAR_MINI_MAP_INFO: 
		{
			// 小地图信息
			OnMessageMiniMapInfo(data, len);
		}
		break;
	case SC_MSG_WAR_MINI_MAP_ICON: 
		{
			// 小地图图标
			OnMessageMiniMapIcon(data, len);
		}
		break;
	case SC_MSG_WAR_IMPORTANT_ENTITYRELIVE: 
		{
			// 重要实体复活
			OnMessageBroadImprotantRelive(data, len);
		}
		break;
	case SC_MSG_WAR_IMPORTANT_MONSTERRELIVE: 
		{
			// 怪物复活
			OnMessageBroadImprotantMonsterRelive(data, len);
		}
		break;
	case SC_MSG_WAR_IMPORTANT_MONSTERTEXTDIE: 
		{
			// 重要怪物死亡文字广播
			OnMessageBroadMonsterDieText(data, len);
		}
		break;
	case SC_MSG_WAR_ENTITY_FIGHT_MINI_MAP_INFO: 
		{
			// 战场实体战斗小地图信息
			OnMessageEntityFightMinMapInfo(data, len);
		}
		break;
	case SC_MSG_WAR_BROADCAST_SKILL_STATE:
		{
			// 广播技能状态
			OnMessageSkillState(data, len );
		}
		break;
	case SC_MSG_WAR_IMPORTANT_MONSTERRELIVE_EARLY:
		{
			// 提早提醒玩家水晶等重要实体复活 
			OnMessageBroadImprotantMonsterRelive(data, len ,true);
		}
		break;
	case SC_MSG_WAR_ENTITY_ALL_PLAYER_CREATED:
		{
			// 战场全部玩家实体创建完毕下发
			OnMessageAllPlayerEntityCreated(data, len);
		}
		break;
	case SC_MSG_WAR_PROCESS_BROADCAST: 
		{
			// 流程广播控制
			OnMessageProcessBroadcast(data, len);
		}
		break;

	case SC_MSG_WAR_XP_RANDOM_PLAYER:
		{
			// 随机冷却中的xp技能到客户端
			OnMessageXPRandomPlayerInCool(data, len);
		}
		break;
	case SC_MSG_WAR_ENTITY_SPECIAL_SEARCH:
		{
			// 特殊实体范围搜索状态改变通知客户端 目前有瞭望台
			OnMessageEntitySearchStateChange(data, len);
		}
		break;
	case SC_MSG_WAR_SPECIALCONTRIBUTION:
		{
			// 玩家特殊贡献信息同步(惊现救治 夺命控制 团控)
			onMessageSpecialContribution(data, len);
		}
		break;
	case SC_MSG_WAR_FILTER_SUPPORT_PERSON:
		{
			// 筛选出到点赞平台条件的人
			onMessageAllFilterSupportPerson(data, len);
		}
		break;
	case SC_MSG_WAR_ADD_LABEL:
		{
			// 广播客户端添加标签操作
			onMessageAddLabel(data, len);
		}
		break;
	case SC_MSG_WAR_ACTIVE_TALENT_LIST:
		{
			// 激活天赋操作
			onMessageActiveTalentList(data, len);
		}
		break;
	case SC_MSG_WAR_BOTTING_NOTIFY:
		{
			// 挂机行为
			if(m_pCheckBadBehavior != NULL)
				m_pCheckBadBehavior->onMessage(msgId, data, len);
		}
		break;
	default:
		break;
	}
	return true;
}


// 模块消息处理
void CWarPart::onMessage( SNetMsgHead* head, void* data, size_t len )
{
    switch(head->byKeyModule)
    {
    case MSG_MODULEID_WAR:
        {
            // 战场模块消息处理
            onWarMessageID(head->byKeyAction, data, len);
        }
        break;
    case MSG_MODULEID_STATICWAR:
        {
            // 主城模块消息处理
            onWarStaticMessageID(head->byKeyAction, data, len);
        }
        break;
    default:
        break;
    }

}

// 部件消息处理
bool CWarPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch (msgId)
	{
	case SC_MSG_WAR_COMBA_KILL:
		{
			// 显示击杀图标
			OnMessageShowKillIcon(data, len);
		}
		break;
	case SC_MSG_WAR_PERSONINFO_INITAL:
		{
			// 更新人物数据
			OnMessageInitPersonInfo(data, len);
		}
		break;
	case SC_MSG_WAR_BACKROUND_MUSIC:
		{
			// 更新人物数据
			OnMessageBackGroundMusic(data, len);
		}
		break;
	default:
		break;
	}

	return true;
}

bool CWarPart::onCommand( int cmdid,void * data,size_t len )
{
	switch( cmdid )
	{
	case ENTITY_CMD_MINIMAP_INFO:						
		{
			if ( limitPosFlagNum() == false )
			{
				gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_WAR_MINIMAP_FLAG_BUSY);
				return true;
			}

			m_PosNum++;

			if (len != sizeof(cmd_war_mini_map_info))
			{
				ErrorLn("CWarPart::onCommand len != sizeof(cmd_war_mini_map_info)");
				return false;
			}
			cmd_war_mini_map_info* pData = (cmd_war_mini_map_info *)data;
			obuf ob;

			msg_war_entity_minimap_info sendData;
			sendData.nType = pData->nType;
			sendData.uPlayerUID = m_pMaster->getUID(); 
			sendData.vec[0] = pData->x;
			sendData.vec[1] = pData->y;
			sendData.vec[2] = pData->z;
			sendData.nEntityID = pData->nEntityID;

			ob.push_back(&sendData, sizeof(sendData));
			SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_MINI_MAP_INFO, ob.data(), ob.size());
			return true;
		}

	case ENTITY_CMD_PLAYER_BASE_INFO:
		{
			int* pData = (int*)data;
			m_IsOnceRequest = *pData;
			SendWarPersonInfo();
			return true;
		}
		break;
	case ENTITY_CMD_PALYER_TAB_ISOPEN:
		{
			int* pData = (int *)data;
			m_TabIsOpen = *pData;

			msg_war_entity_update_table_request sendData;
			sendData.uPlayerUID = m_pMaster->getUID();
			sendData.bIsOpen = m_TabIsOpen;

			obuf ob;
			ob.push_back(&sendData, sizeof(sendData));
			SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_PLAYER_TABLE_REQUEST, ob.data(), ob.size());
			return true;
		}
		break;
	case ENTITY_CMD_REQ_WARSTATISTIC_TOTAL:
		{
			cmd_warstatistic_req_total *pData = (cmd_warstatistic_req_total*)data;

			obuf ob;
			SNetMsgHead head;
			head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
			head.byDestEndPoint = MSG_ENDPOINT_SCENE;
			head.byKeyModule = MSG_MODULEID_STATICWAR;
			head.byKeyAction = CS_MSG_WAR_GET_WARSTATICS_TOTAL;
			ob << head;

			msg_war_get_warstatics_total sendData;
			sendData.actorID = pData->nActorID;
			ob.push_back(&sendData, sizeof(sendData));

            net_senddata( ob.data(), ob.size() );

			return true;
		}
		break;
	case ENTITY_CMD_REQ_WARSTATISTIC_WAR:
		{
			cmd_warstatistic_req_war *pData = (cmd_warstatistic_req_war*)data;

			obuf ob;
			SNetMsgHead head;
			head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
			head.byDestEndPoint = MSG_ENDPOINT_SCENE;
			head.byKeyModule = MSG_MODULEID_STATICWAR;
			head.byKeyAction = CS_MSG_WAR_GET_WARSTATICS_WAR;
			ob << head;

			msg_war_get_warstatics_war sendData;
			sendData.nWarID = pData->nWarID;
            sendData.byWnd  = pData->byWnd;
			ob.push_back(&sendData, sizeof(sendData));

            net_senddata( ob.data(), ob.size() );

			return true;
		}
		break;
	case ENTITY_CMD_AI_COMMAND_FOLLOW:
		{
			if (len != sizeof(cmd_war_ai_command_follow))
			{
				ErrorLn("CWarPart::onCommand len != sizeof(cmd_war_ai_command_follow)");
				return false;
			}
			cmd_war_ai_command_follow* pData = (cmd_war_ai_command_follow *)data;

			obuf ob;
			msg_fllow_me sendData;
			sendData.uidTarget = pData->uidTarget;
			ob.push_back(&sendData, sizeof(sendData));
			SendMessageToEntityScene(m_pMaster->getUID(), PART_AI, CS_MSG_FOLLOW_ME, ob.data(), ob.size());

			IClientEntity *pEntity = (IClientEntity*)gClientGlobal->getEntityFactory()->getEntity(pData->uidTarget);
			ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
			IChatClient *pChatClient = gClientGlobal->getChatClient();
			if (pEntity == NULL || pSchemeCenter == NULL || pChatClient == NULL)
			{
				return false;
			}

			char stTargetVocationName[64];
			ISchemePersonModel *pPersonScheme = pSchemeCenter->getSchemePersonModel();
			if (pPersonScheme != NULL)
			{
				sstrcpyn(stTargetVocationName, pPersonScheme->getHeroName(pEntity->getIntProperty(PROPERTY_VOCATION)), sizeof(stTargetVocationName));
			}

			pChatClient->showSystemMessage(CHAT_TIP_WAR_AICOMMAND_JOINFOLLOW, stTargetVocationName);

			return true;
		}
		break;
	case ENTITY_CMD_AI_COMMAND_BREAK_FOLLOW:
		{
			if (len != sizeof(cmd_war_ai_command_break_follow))
			{
				ErrorLn("CWarPart::onCommand len != sizeof(cmd_war_ai_command_break_follow)");
				return false;
			}
			cmd_war_ai_command_break_follow* pData = (cmd_war_ai_command_break_follow *)data;

			obuf ob;
			msg_break_fllow_me sendData;
			sendData.uidTarget = pData->uidTarget;
			ob.push_back(&sendData, sizeof(sendData));
			SendMessageToEntityScene(m_pMaster->getUID(), PART_AI, CS_MSG_BREAK_FOLLOW_ME, ob.data(), ob.size());

			IClientEntity *pEntity = (IClientEntity*)gClientGlobal->getEntityFactory()->getEntity(pData->uidTarget);
			ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
			IChatClient *pChatClient = gClientGlobal->getChatClient();
			if (pEntity == NULL || pSchemeCenter == NULL || pChatClient == NULL)
			{
				return false;
			}

			char stTargetVocationName[64];
			ISchemePersonModel *pPersonScheme = pSchemeCenter->getSchemePersonModel();
			if (pPersonScheme != NULL)
			{
				sstrcpyn(stTargetVocationName, pPersonScheme->getHeroName(pEntity->getIntProperty(PROPERTY_VOCATION)), sizeof(stTargetVocationName));
			}

			pChatClient->showSystemMessage(CHAT_TIP_WAR_AICOMMAND_CANCELFOLLOW, stTargetVocationName);
			return true;
		}
		break;
	case ENTITY_CMD_SPECIAL_SELECT:						
		{
			if (len != sizeof(cmd_Select_Special_Entity))
			{
				ErrorLn("CWarPart::onCommand len != sizeof(cmd_war_ai_command_break_follow)");
				return false;
			}

			cmd_Select_Special_Entity* pData = (cmd_Select_Special_Entity *)data;

			msg_war_select_special_entity sendData;
			sendData.uidOperator = pData->uidOperator;
			sendData.uidTarget = pData->uidTarget;
			sendData.nFunctionType = pData->nFunctionType;

			SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_ENTITY_SPECIAL_SELECT, &sendData,sizeof(msg_war_select_special_entity));
			return true;
		}
	case ENTITY_CMD_SPECIAL_SEARCH:						
		{
			if (len != sizeof(cmd_Special_Entity_Range_Search))
			{
				ErrorLn("CWarPart::onCommand len != sizeof(cmd_war_ai_command_break_follow)");
				return false;
			}

			cmd_Special_Entity_Range_Search* pData = (cmd_Special_Entity_Range_Search *)data;

			msg_war_special_entity_range_search sendData;
			sendData.uidOperator = pData->uidOperator;
			sendData.uidTarget = pData->uidTarget;
			sendData.nFunctionType = pData->nFunctionType;
			sendData.bSearchOrBreak = pData->bSearchOrBreak;
			SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_ENTITY_SPECIAL_SEARCH, &sendData,sizeof(msg_war_special_entity_range_search));
			return true;
		}
	case ENTITY_CMD_WAR_ADD_LABEL:						
		{
			if (len != sizeof(cmd_war_add_label))
			{
				ErrorLn("CWarPart::onCommand len != sizeof(cmd_war_add_label)");
				return false;
			}

			cmd_war_add_label* pData = (cmd_war_add_label *)data;

			msg_war_add_label sendData;
			sendData.targetPdbid = pData->targetPdbid;
			sendData.nLabelID = pData->nLabelID;
		
			SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_ADD_LABEL, &sendData,sizeof(msg_war_add_label));
			return true;
		}
    case ENTITY_CMD_PICK_HP:
        {
            cmd_pick_hp* pData = (cmd_pick_hp*)data;

            msg_pick_up_item msg;
            msg.itemID = pData->entityID;
            SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_PICK_UP_ITEM, &msg,sizeof(msg));
            return true;
        }
	case ENTITY_CMD_BOTTING_CANCEL:
		{
			if (m_pCheckBadBehavior != NULL)
				m_pCheckBadBehavior->onCommand(cmdid, data, len);

			return true;
		}
	case ENTITY_CMD_REQ_HISTORY_RECORD:
		{
			if (len != sizeof(cmd_warstatistic_req_history_record))
			{
				ErrorLn("CWarPart::onCommand len != sizeof(cmd_warstatistic_req_history_record)");
				return false;
			}

			obuf64 ob;
			SNetMsgHead head;
			head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
			head.byDestEndPoint = MSG_ENDPOINT_SCENE;
			head.byKeyModule = MSG_MODULEID_STATICWAR;
			head.byKeyAction = CS_MSG_WAR_GET_HISTORY_RECORD;
			ob << head;

			cmd_warstatistic_req_history_record* pData = (cmd_warstatistic_req_history_record*)data;
			msg_war_get_history_record msg;
			msg.actorID = pData->dwActorID;
			ob.push_back(&msg, sizeof(msg));

            net_senddata( ob.data(), ob.size() );

			return true;
		}
	case ENTITY_CMD_CROWN_DOTHER_PAGE:
		{
			if (len != sizeof(cmd_Crown_Request_Other_Page))
			{
				ErrorLn(__FUNCTION__": ENTITY_CMD_ACTOR_NAME_FIX nLen = " << len);
				return false;
			}

			cmd_Crown_Request_Other_Page *pCmdInfo = (cmd_Crown_Request_Other_Page*)data;

			Msg_Request_Other_Crown_Points msgInfo;
			msgInfo.dwFriendID = pCmdInfo->RequestFriendID;

			obuf256 ob;
			SNetMsgHead head;
			head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
			head.byDestEndPoint = MSG_ENDPOINT_SCENE;
			head.byKeyModule = MSG_MODULEID_STATICWAR;
			head.byKeyAction = CS_MSG_WAR_CROWNDOTHER_PAGE;
			ob << head;
			ob.push_back(&msgInfo, sizeof(msgInfo));
            net_senddata( ob.data(), ob.size() );
			
			return true;
		}
	case ENTITY_CMD_REQ_EXIT_WAR: // 请求退出战场
		{
			SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_REQ_EXIT_WAR, NULL, 0);

			return true;
		}
	default:
		return false;
	}

	return false;
}

/** 取是主人
@param   
@param   
@return  
*/
IEntity * CWarPart::getHost(void)
{
	return m_pMaster;
}


// 服务器消息
void CWarPart::OnMessageUpdatePersonInfo(void *pData, size_t nLen)
{
	// 战绩数据缓存
	if (m_pMaster == NULL )
	{
		return;
	}
	// 解析数据
	if (pData == NULL || nLen < sizeof(msg_war_person_info_list) )
	{
		return;
	}
	msg_war_person_info_list *pMsgPersonListHead = (msg_war_person_info_list *)pData;
	int nPersonNum = pMsgPersonListHead->nNum;

	if(nLen != sizeof(msg_war_person_info)*nPersonNum + sizeof(msg_war_person_info_list))
	{
		ErrorLn(__FUNCTION__ ": nLen != sizeof(msg_war_person_info)*nPersonNum + sizeof(msg_war_person_info_list)  nLen = " << nLen << ", sizeof(msg_war_person_info)*nPersonNum + sizeof(msg_war_person_info_list) = " << sizeof(msg_war_person_info)*nPersonNum + sizeof(msg_war_person_info_list));
		return;
	}

	IEntityProperty *pEntityProperty = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		ErrorLn(__FUNCTION__": pEntityProperty == NULL");
		return;
	}
	PDBID idActor =pEntityProperty->getProperty_Integer(PROPERTY_ID);

	msg_war_person_info *pPersonFirst = (msg_war_person_info *)(pMsgPersonListHead + 1);

	// 处理数据
	for (int i = 0; i < nPersonNum; ++i)
	{
		// 将数据填充到逻辑服的容器中
		fillCmdWarpersoninfo(idActor, pPersonFirst);
		pPersonFirst++;
	}

	if ( m_WarIsEnd == true)
	{
		return;
	}

	SendWarPersonInfo();
}

// 服务器消息增加buff
void CWarPart::OnMessageUpdateMonsterTime(void *pData, size_t nLen)
{
	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_war_monster_time))
	{
		return;
	}

	msg_war_monster_time *pWarMonsterTime = (msg_war_monster_time *)pData;
	cmd_war_monsterinfo cmdMonsterInfo;
	cmdMonsterInfo.nIconIndex = pWarMonsterTime->nIconIndex;
	cmdMonsterInfo.nTime = pWarMonsterTime->nTime;
	cmdMonsterInfo.nType = pWarMonsterTime->nType;
	cmdMonsterInfo.nTableIndex = pWarMonsterTime->nTableIndex;
	UpdateWarMonsterInfo(&cmdMonsterInfo);
}

//服务器广播击杀重要实体
void CWarPart::OnMessageBroadImprotantEntityDie(void *pData, size_t nLen)
{
	IEntityProperty *pEntityProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		return;
	}
	//校验数据大小
	if (pData == NULL || nLen < sizeof(msg_war_important_entiy_dieinfo))
	{
		ErrorLn(__FUNCTION__ " The length is too short!");
		return;
	}

	//获取图标
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return;
	}

	ISchemeMonster* pScheme = pCenter->getSchemeMonster();
	if ( NULL==pScheme )
	{
		return;
	}
	ISchemePersonModel* pSchemePersonModel= pCenter ->getSchemePersonModel();
	if(pSchemePersonModel==NULL)
	{
		return;
	}

	msg_war_important_entiy_dieinfo *pEntityInfo = (msg_war_important_entiy_dieinfo *)pData;
	int nNeedLen = sizeof(msg_war_important_entiy_dieinfo) + pEntityInfo->nAssistCount * sizeof(msg_war_entity_assist_info);
	if (nLen < nNeedLen)
	{
		ErrorLn(__FUNCTION__ " The length is too short!!,nLen=" << nLen << " nNeedLen=" << nNeedLen);
		return;
	}

	cmd_war_important_entiydieinfo cmdEntityInfo;

	if (pEntityInfo->MasterEntityType == TYPE_MONSTER)
	{
		SMonsterScheme* pMonsterScheme = pScheme->getMonsterShemeInfo(pEntityInfo->MasterHeroID);
		if (NULL == pMonsterScheme)
		{
			ErrorLn("OnMessageBroadImprotantEntityDie() -- not found pMonsterScheme pEntityInfo->MasterHeroID="<<pEntityInfo->MasterHeroID);
			return;
		}

		// 怪物名字
		sstrcpyn(pEntityInfo->MasterName, pMonsterScheme->szName, sizeof(pEntityInfo->MasterName));
		// 转换成图标ID
		pEntityInfo->MasterHeroID = pMonsterScheme->nIconID;
	}
	else if (pEntityInfo->MasterEntityType == TYPE_PLAYER_ROLE)
	{
		// 被杀者是人情况取职业描述
		sstrcpyn(cmdEntityInfo.MasterVocationName, pSchemePersonModel->getHeroName(pEntityInfo->MasterHeroID), sizeof(cmdEntityInfo.MasterVocationName));
	}

	if (pEntityInfo->MurdererEntityType == TYPE_MONSTER)
	{
		SMonsterScheme* pMonsterScheme = pScheme->getMonsterShemeInfo(pEntityInfo->MurdererHeroID);
		if (NULL == pMonsterScheme)
		{
			ErrorLn("OnMessageBroadImprotantEntityDie() -- not found pMonsterScheme pEntityInfo->MurdererHeroID="<<pEntityInfo->MurdererHeroID);
			return;
		}

		//转换成图标ID
		pEntityInfo->MurdererHeroID = pMonsterScheme->nIconID;
		// 怪物名字
		sstrcpyn(cmdEntityInfo.MurdererName, pMonsterScheme->szName,sizeof(cmdEntityInfo.MurdererName));
	}
	else if(pEntityInfo->MurdererEntityType == TYPE_PLAYER_ROLE)
	{
		sstrcpyn(cmdEntityInfo.MurdererName, pEntityInfo->MurdererName,sizeof(cmdEntityInfo.MurdererName));
		// 击杀者是人情况取职业描述
		sstrcpyn(cmdEntityInfo.MurdererVocationName, pSchemePersonModel->getHeroName( pEntityInfo->MurdererHeroID),sizeof(cmdEntityInfo.MurdererVocationName));
	}
	
	
	cmdEntityInfo.MasterEntityType = pEntityInfo->MasterEntityType;
	cmdEntityInfo.MurdererEntityType = pEntityInfo->MurdererEntityType;
	cmdEntityInfo.nMasterHeroID = pEntityInfo->MasterHeroID;
	cmdEntityInfo.nMurdererHeroID = pEntityInfo->MurdererHeroID;
	cmdEntityInfo.bIsCampSkill = pEntityInfo->bIsCampSkill;
	cmdEntityInfo.bIsFirstBlood = pEntityInfo->bIsFirstBlood;
	sstrcpyn(cmdEntityInfo.MasterName, pEntityInfo->MasterName,sizeof(cmdEntityInfo.MasterName));

	cmdEntityInfo.nKillCount = pEntityInfo->nKillCount;
	cmdEntityInfo.nCamp = pEntityInfo->nCamp;
	cmdEntityInfo.isSampCampKill = 0;
	if (pEntityProperty->getProperty_Integer(PROPERTY_CAMP) == cmdEntityInfo.nCamp)
	{
		cmdEntityInfo.isSampCampKill = 1;
	}
	cmdEntityInfo.bIsAce = pEntityInfo->isAce;
	cmdEntityInfo.bIsOver = pEntityInfo->bIsOver;
	if (pEntityInfo->nBroadCastIndex > 0)
	{
		ISchemeWarBroadCastInfo* pSchemeInfo = pCenter->getSchemeWarBroadCastInfo();
		SWarBroadCastInfoScheme* pInfo = pSchemeInfo->getWarBroadCastInfoByIndex(pEntityInfo->nBroadCastIndex);
		if (pInfo != NULL)
		{
			// 查找设置广播信息
			if (cmdEntityInfo.isSampCampKill == 1)
			{
				cmdEntityInfo.nSoundID = pInfo->nSelfCampSoundID;
				sstrcpyn(cmdEntityInfo.szDieDes, pInfo->szSelfCampDes,sizeof(cmdEntityInfo.szDieDes));
			}
			else
			{
				cmdEntityInfo.nSoundID = pInfo->nEnimyCampSoundID;
				sstrcpyn(cmdEntityInfo.szDieDes, pInfo->szEnimyCampDes,sizeof(cmdEntityInfo.szDieDes));
			}
		}
	}
	cmdEntityInfo.nAddTalentPoint = pEntityInfo->nAddTalentPoint;
	cmdEntityInfo.uidMaster = pEntityInfo->uidMaster;
	cmdEntityInfo.uidMurderer = pEntityInfo->uidMurderer;
	cmdEntityInfo.nAssistCount = pEntityInfo->nAssistCount;

	// 助攻者信息
	int nAssistCount = pEntityInfo->nAssistCount;
	msg_war_entity_assist_info* pAssistInfo = (msg_war_entity_assist_info*)(pEntityInfo+1);
	vector<cmd_war_assist_info> assistList;
	for (int i = 0; i < nAssistCount; ++i)
	{
		cmd_war_assist_info cmdAssist;
		sstrcpyn(cmdAssist.AssistName, pAssistInfo->szName, sizeof(cmdAssist.AssistName));
		if (pAssistInfo->nEntityType == TYPE_PLAYER_ROLE)
		{
			// 助攻者是人情况取职业描述
			sstrcpyn(cmdAssist.AssistVocationName, pSchemePersonModel->getHeroName(pAssistInfo->nHeroID), sizeof(cmdAssist.AssistVocationName));
		}
		cmdAssist.MasterEntityType = pAssistInfo->nEntityType;
		cmdAssist.nHerorID = pAssistInfo->nHeroID;
		cmdAssist.uidAssist = pAssistInfo->uidAssist;

		assistList.push_back(cmdAssist);

		pAssistInfo++;
	}

	BroadImprotantEt(&cmdEntityInfo, assistList);
}

//服务器广播击杀
void CWarPart::OnMessageBroadKillCn(void *pData, size_t nLen)
{
	IEntityProperty *pEntityProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		return;
	}

	//校验数据大小
	if (pData == NULL || nLen < sizeof(msg_war_entity_kill_countinfo))
	{
		ErrorLn(__FUNCTION__ " The length of msg is too short！");
		return;
	}

	//获取图标
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return;
	}
	ISchemePersonModel* pSchemePersonModel= pCenter ->getSchemePersonModel();
	if(pSchemePersonModel==NULL)
	{
		return;
	}

	msg_war_entity_kill_countinfo *pKillInfo = (msg_war_entity_kill_countinfo *)pData;
	int nNeedLen = sizeof(msg_war_entity_kill_countinfo) + pKillInfo->nAssistCount * sizeof(msg_war_entity_assist_info);
	if (nLen < nNeedLen)
	{
		ErrorLn(__FUNCTION__ " The length is too short!!,nLen=" << nLen << " nNeedLen=" << nNeedLen);
		return;
	}

	cmd_war_entity_killcountinfo cmdKillInfo;
	cmdKillInfo.MasterEntityType = pKillInfo->MasterEntityType;
	cmdKillInfo.MurdererEntityType = pKillInfo->MurdererEntityType;
	sstrcpyn(cmdKillInfo.MasterName,pKillInfo->MasterName,sizeof(cmdKillInfo.MasterName));
	sstrcpyn(cmdKillInfo.MurdererName,pKillInfo->MurdererName,sizeof(cmdKillInfo.MurdererName));
	cmdKillInfo.nMurdererHeroID = pKillInfo->MurdererHeroID;
	cmdKillInfo.nMasterHeroID = pKillInfo->MasterHeroID;
	cmdKillInfo.nTimeKillCount = pKillInfo->nTimeKillCount;
	cmdKillInfo.nCamp =  pKillInfo->MurdererCamp;
	cmdKillInfo.isSampCampKill = 0;
	cmdKillInfo.nIsAce = pKillInfo->bIsAce;
	cmdKillInfo.uidMaster = pKillInfo->uidMaster;
	cmdKillInfo.uidMurderer = pKillInfo->uidMurderer;
	sstrcpyn(cmdKillInfo.MurdererVocationName, pSchemePersonModel->getHeroName( pKillInfo->MurdererHeroID),sizeof(cmdKillInfo.MurdererVocationName));
	sstrcpyn(cmdKillInfo.MasterVocationName, pSchemePersonModel->getHeroName(pKillInfo->MasterHeroID ),sizeof(cmdKillInfo.MasterVocationName));

	//WarningLn(__FUNCTION__": pKillInfo->MurdererHeroID = " << pKillInfo->MurdererHeroID);

	if (pEntityProperty->getProperty_Integer(PROPERTY_CAMP) == cmdKillInfo.nCamp)
	{
		cmdKillInfo.isSampCampKill = 1;
	}
	cmdKillInfo.nAssistCount = pKillInfo->nAssistCount;


	// 助攻者信息
	int nAssistCount = pKillInfo->nAssistCount;
	msg_war_entity_assist_info* pAssistInfo = (msg_war_entity_assist_info*)(pKillInfo+1);
	vector<cmd_war_assist_info> assistList;
	for (int i = 0; i < nAssistCount; ++i)
	{
		cmd_war_assist_info cmdAssist;
		sstrcpyn(cmdAssist.AssistName, pAssistInfo->szName, sizeof(cmdAssist.AssistName));
		if (pAssistInfo->nEntityType == TYPE_PLAYER_ROLE)
		{
			// 助攻者是人情况取职业描述
			sstrcpyn(cmdAssist.AssistVocationName, pSchemePersonModel->getHeroName(pAssistInfo->nHeroID), sizeof(cmdAssist.AssistVocationName));
		}
		cmdAssist.MasterEntityType = pAssistInfo->nEntityType;
		cmdAssist.nHerorID = pAssistInfo->nHeroID;
		cmdAssist.uidAssist = pAssistInfo->uidAssist;

		assistList.push_back(cmdAssist);

		pAssistInfo++;
	}

	BroadKillCn(&cmdKillInfo, assistList);
}


void CWarPart::SendWarPersonInfo()
{
	if (m_pMaster == NULL) return;

	cmd_war_personInfo_count cmdWarPersonInfoCount;
	int nCount = m_mapPersonWarRecord.size();
	cmdWarPersonInfoCount.nCount = nCount;
	obuf obdata;
	obdata.push_back(&cmdWarPersonInfoCount, sizeof(cmd_war_personInfo_count ));

	map<PDBID,cmd_war_personinfo>::iterator iter =  m_mapPersonWarRecord.begin();
	for (;iter != m_mapPersonWarRecord.end();++iter)
	{
		cmd_war_personinfo& cmdInfo = iter->second;
		obdata.push_back(&cmdInfo, sizeof(cmd_war_personinfo) );

	}
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_WAR_PERSONINFO,0,0,obdata.data(), obdata.size());
}

void CWarPart::RequestPersenInfoComplete()
{
	if ( m_pMaster == NULL )
	{
		return;
	}

	// 通告显示层第一次请求玩家战绩表数据完毕
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_REQUEST_PERSONINFO_COMPLETE, 0, 0, 0, 0 );
}

void CWarPart::UpdateWarMonsterInfo(cmd_war_monsterinfo *MonsterData)
{
	if (m_pMaster == NULL || MonsterData == NULL)
	{
		return;
	}
	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_WAR_MONSTERINFO,sizeof(MonsterData),0,MonsterData, sizeof(cmd_war_monsterinfo));
}

void CWarPart::BroadImprotantEt(cmd_war_important_entiydieinfo *EntityInfo, const vector<cmd_war_assist_info>& assistList)
{
	if (m_pMaster == NULL || EntityInfo == NULL)
	{
		return;
	}

	obuf ob;
	ob << *EntityInfo;

	vector<cmd_war_assist_info>::const_iterator iter = assistList.begin();
	for (; iter != assistList.end(); ++iter)
	{
		ob << *iter;
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_WAR_BDIMPENTITYINFO, 0, 0, ob.data(), ob.size());
}

void CWarPart::BroadKillCn(cmd_war_entity_killcountinfo *KillInfo, const vector<cmd_war_assist_info>& assistList)
{
	if (m_pMaster == NULL || KillInfo == NULL)
	{
		return;
	}
	
	obuf ob;
	ob << *KillInfo;

	vector<cmd_war_assist_info>::const_iterator iter = assistList.begin();
	for (; iter != assistList.end(); ++iter)
	{
		ob << *iter;
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_WAR_BROADKILLINFO, 0, 0, ob.data(), ob.size());
}

// 处理结束战场
void CWarPart::OnMessageWarEndhandle(void *pData, size_t nLen)
{
	//校验数据大小
	if ( pData == NULL || m_pMaster == NULL  )
	{
		ErrorLn(__FUNCTION__ ": pData == NULL or m_pMaster == NULL!! ");
		return;
	}
	
	m_WarIsEnd = true;

	// 战场结束处理
	onWarEnd();

	// 数据解析
	msg_war_entity_end_warinfo_list *pMsgWarListHead = (msg_war_entity_end_warinfo_list *)pData;
	LONGLONG dwDBWarID = pMsgWarListHead->dwDBWarID;
	int nWarlistNum = pMsgWarListHead->nPlayerCount;
	int nWinCamp = pMsgWarListHead->nWinCamp;

	if(nLen != sizeof(msg_war_entity_end_warinfo)*pMsgWarListHead->nPlayerCount + sizeof(msg_war_entity_end_warinfo_list))
	{
        ErrorLn(__FUNCTION__": error lenth!")
		return;
	}

	msg_war_entity_end_warinfo *pWarEndinfoFirst = (msg_war_entity_end_warinfo *)(pMsgWarListHead + 1);

	// 战场结束 将最新数据发向显示层最后更新一次

	IEntityProperty *pEntityProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		return;
	}

	cmd_show_win_or_fail cmdShowWinOrFail;
	cmdShowWinOrFail.nWin = 0;
	if (pEntityProperty->getProperty_Integer(PROPERTY_CAMP) == nWinCamp)
	{
		cmdShowWinOrFail.nWin = 1;
	}
	cmdShowWinOrFail.nUseTime = pMsgWarListHead->uUseTime;
	cmdShowWinOrFail.fBasePos_x = pMsgWarListHead->xBasePos / ACTOR_LOCATION_SCALE;
	cmdShowWinOrFail.fBasePos_y = pMsgWarListHead->yBasePos / ACTOR_LOCATION_SCALE;
	cmdShowWinOrFail.fBasePos_z = pMsgWarListHead->zBasePos / ACTOR_LOCATION_SCALE;
	cmdShowWinOrFail.isShowResultWnd = (getCurGameMode() == EGM_Moba);
	cmdShowWinOrFail.isPlayEndAnim = (getCurGameMode() == EGM_Moba);

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_WAR_SHOW_WIN_OR_FIAL,sizeof(cmdShowWinOrFail),0,&cmdShowWinOrFail, sizeof(cmd_show_win_or_fail));

	// 加载战后战绩表数据信息
	obuf obEndRecordData;
	obEndRecordData<<dwDBWarID;
	loadEndRecordData(pWarEndinfoFirst, nWarlistNum,obEndRecordData);

	// 通告显示层更新战后战绩表玩家数据
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_END_CREATE_RECORD,0,0,obEndRecordData.data(), obEndRecordData.size());
}

void CWarPart::OnMessageMiniMapInfo( void *pData, size_t nLen )
{
	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_war_entity_minimap_info) )
	{
		return;
	}

	msg_war_entity_minimap_info *pMsgMiniMapinfo = (msg_war_entity_minimap_info *)pData;
	IClientEntity *pEntity = (IClientEntity*)gClientGlobal->getEntityFactory()->getEntity(pMsgMiniMapinfo->uPlayerUID);
	if (pEntity == NULL)
	{
		return;
	}

	cmd_war_mini_map_info cmdMiniMapInfo;
	cmdMiniMapInfo.nType = pMsgMiniMapinfo->nType;
	cmdMiniMapInfo.x = pMsgMiniMapinfo->vec[0];
	cmdMiniMapInfo.y = pMsgMiniMapinfo->vec[1];
	cmdMiniMapInfo.z = pMsgMiniMapinfo->vec[2];
	cmdMiniMapInfo.nEntityID = pMsgMiniMapinfo->nEntityID;

	//玩家名字
	sstrcpyn(cmdMiniMapInfo.PlayerName, pEntity->getName(), sizeof(cmdMiniMapInfo.PlayerName));

	IClientEntity *pTargetEntity = NULL;
	if (cmdMiniMapInfo.nEntityID > 0)
	{
		pTargetEntity = (IClientEntity*)gClientGlobal->getEntityFactory()->getEntity(cmdMiniMapInfo.nEntityID);
		if (pTargetEntity != NULL)
		{
			sstrcpyn(cmdMiniMapInfo.TargetName, pTargetEntity->getName(), sizeof(cmdMiniMapInfo.TargetName));
		}
	}

	// 职业名称
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if (NULL != pCenter)
	{
		ISchemePersonModel *pPersonScheme = pCenter->getSchemePersonModel();
		if (pPersonScheme != NULL)
		{
			sstrcpyn(cmdMiniMapInfo.VocationName, pPersonScheme->getHeroName(pEntity->getIntProperty(PROPERTY_VOCATION)), sizeof(cmdMiniMapInfo.VocationName));

			if (pTargetEntity != NULL)
			{
				UID targetUID = pTargetEntity->getUID();
				if (UID_2_TYPE(targetUID)==TYPE_PLAYER_ROLE)
					sstrcpyn(cmdMiniMapInfo.TargetVocationName, pPersonScheme->getHeroName(pTargetEntity->getIntProperty(PROPERTY_VOCATION)), sizeof(cmdMiniMapInfo.TargetVocationName));
			}
		}
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_MINIMAP_INFO,0,0,&cmdMiniMapInfo, sizeof(cmd_war_mini_map_info));
}

void CWarPart::OnMessageMiniMapIcon( void *pData, size_t nLen )
{
	if (m_pMaster ==NULL)
	{
		return;
	}
	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_war_entity_minimap_icon) )
	{
		return;
	}

	msg_war_entity_minimap_icon *pMsgMiniMapinfo = (msg_war_entity_minimap_icon *)pData;
	// C++挡一下为0的index信息
	if (pMsgMiniMapinfo->nIconIndex <= 0)
	{
		return;
	}

	cmd_war_mini_map_icon cmdMiniMapIcon;
	cmdMiniMapIcon.bIsShow = pMsgMiniMapinfo->bIsShow;
	cmdMiniMapIcon.nIconIndex = pMsgMiniMapinfo->nIconIndex;
	cmdMiniMapIcon.nMapID = pMsgMiniMapinfo->nMapID;	

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_MINIMAP_ICON,0,0,&cmdMiniMapIcon, sizeof(cmd_war_mini_map_icon));
}


//服务器广播击杀重要实体
void CWarPart::OnMessageBroadImprotantRelive(void *pData, size_t nLen)
{
	if (m_pMaster ==NULL)
	{
		return;
	}
	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_war_important_entiy_reliveinfo))
	{
		return;
	}
	msg_war_important_entiy_reliveinfo *pEntityInfo = (msg_war_important_entiy_reliveinfo *)pData;
	cmd_war_important_entiyreliveinfo cmdEntityReliveInfo;
	DWORD dwSynTick = gClientGlobal->getTimeSyncer()->GetTick();
	cmdEntityReliveInfo.dwRevliveTime = (pEntityInfo->dwRevliveTime > dwSynTick) ? ((pEntityInfo->dwRevliveTime-dwSynTick)/1000)*1000 : 0; //pEntityInfo->dwRevliveTime;
	cmdEntityReliveInfo.uReliveUID = pEntityInfo->uReliveUID;
	IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(cmdEntityReliveInfo.uReliveUID);
	if (pEntity  == NULL)
	{
		return;
	}
	IEntityProperty *pEntityProperty = (IEntityProperty *)pEntity->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		return;
	}
	cmdEntityReliveInfo.uPBUID=pEntityInfo->uRelivePDBID;
	cmdEntityReliveInfo.nHeroIconID=pEntityProperty->getProperty_Integer(PROPERTY_VOCATION);
	cmdEntityReliveInfo.nCamp =pEntityProperty->getProperty_Integer(PROPERTY_CAMP);
	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RELIVE_TIME,0,0,&cmdEntityReliveInfo, sizeof(cmd_war_important_entiyreliveinfo));
}

//服务器广播击杀重要实体
void CWarPart::OnMessageBroadImprotantMonsterRelive(void *pData, size_t nLen,bool nIsEarlyRelive)
{
	if (m_pMaster ==NULL)
	{
		return;
	}

	//获取图标
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return;
	}
	ISchemeMonster* pScheme = pCenter->getSchemeMonster();
	if ( NULL==pScheme )
	{
		return;
	}
	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_war_important_monster_reliveinfo))
	{
		return;
	}
	IEntityProperty *pEntityProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		return;
	}

	msg_war_important_monster_reliveinfo *pEntityInfo = (msg_war_important_monster_reliveinfo *)pData;
	SMonsterScheme* pMonsterScheme = pScheme->getMonsterShemeInfo(pEntityInfo->MasterHeroID);
	if (NULL == pMonsterScheme)
	{
		return;
	}

	cmd_war_important_monsterliveinfo cmdMonsterReliveInfo;
	cmdMonsterReliveInfo.isSampCamp = 0;
	if (pEntityProperty->getProperty_Integer(PROPERTY_CAMP) == pEntityInfo->nCamp)
	{
		cmdMonsterReliveInfo.isSampCamp = 1;
	}
	sstrcpyn(cmdMonsterReliveInfo.szName, pMonsterScheme->szName, sizeof(cmdMonsterReliveInfo.szName));
	cmdMonsterReliveInfo.MasterEntityType = pEntityInfo->MasterEntityType;
	cmdMonsterReliveInfo.MasterIconID = pMonsterScheme->nIconID;
	cmdMonsterReliveInfo.nCamp = pEntityInfo->nCamp;
	cmdMonsterReliveInfo.isEarlyRelive= nIsEarlyRelive==true?  1:0;
	if (pEntityInfo->nBroadCastIndex > 0)
	{
		ISchemeWarBroadCastInfo* pSchemeInfo = pCenter->getSchemeWarBroadCastInfo();
		SWarBroadCastInfoScheme* pInfo = pSchemeInfo->getWarBroadCastInfoByIndex(pEntityInfo->nBroadCastIndex);
		if (pInfo != NULL)
		{
			// 查找设置广播信息
			if (cmdMonsterReliveInfo.isSampCamp == 1)
			{
				cmdMonsterReliveInfo.nSoundID = pInfo->nSelfCampSoundID;
				sstrcpyn(cmdMonsterReliveInfo.szDes, pInfo->szSelfCampDes,sizeof(cmdMonsterReliveInfo.szDes));
			}
			else
			{
				cmdMonsterReliveInfo.nSoundID = pInfo->nEnimyCampSoundID;
				sstrcpyn(cmdMonsterReliveInfo.szDes, pInfo->szEnimyCampDes,sizeof(cmdMonsterReliveInfo.szDes));
			}
		}
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_IMPORTANT_MONSTER_RELIVE,0,0,&cmdMonsterReliveInfo, sizeof(cmd_war_important_monsterliveinfo));
}

void CWarPart::OnMessageClearAllMiniMapIcon(void *pData, size_t nLen)
{
	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_clear_all_mini_map_icon) )
	{
		return;
	}

	msg_clear_all_mini_map_icon *pMsgMiniMapinfo = (msg_clear_all_mini_map_icon *)pData;

	cmd_war_clear_all_mini_map_info cmdClearAll;
	cmdClearAll.isClearAll = pMsgMiniMapinfo->isClearAll == true? 1: 0;

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CLEAR_ALL_MINIMAP_INFO,0,0,&cmdClearAll, sizeof(cmd_war_clear_all_mini_map_info));
}

void CWarPart::OnMessageBroadMonsterDieText(void *pData, size_t nLen)
{
	if (m_pMaster ==NULL)
	{
		return;
	}

	//获取图标
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return;
	}

	ISchemeMonster* pScheme = pCenter->getSchemeMonster();
	if ( NULL==pScheme )
	{
		return;
	}
	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_war_important_monster_dietxtinfo))
	{
		return;
	}
	IEntityProperty *pEntityProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		return;
	}

	msg_war_important_monster_dietxtinfo *pEntityInfo = (msg_war_important_monster_dietxtinfo *)pData;
	SMonsterScheme* pMonsterScheme = pScheme->getMonsterShemeInfo(pEntityInfo->MasterHeroID);
	if (NULL == pMonsterScheme)
	{
		return;
	}

	cmd_war_important_monsterdietxtinfo cmdMonsterDieTxtInfo;
	cmdMonsterDieTxtInfo.isSampCamp = 0;
	if (pEntityProperty->getProperty_Integer(PROPERTY_CAMP) == pEntityInfo->nCamp)
	{
		cmdMonsterDieTxtInfo.isSampCamp = 1;
	}
	sstrcpyn(cmdMonsterDieTxtInfo.szName, pMonsterScheme->szName, sizeof(cmdMonsterDieTxtInfo.szName));
	cmdMonsterDieTxtInfo.MasterEntityType = pEntityInfo->MasterEntityType;
	cmdMonsterDieTxtInfo.MasterIconID = pMonsterScheme->nIconID;
	cmdMonsterDieTxtInfo.nCamp = pEntityInfo->nCamp;

	if (pEntityInfo->nBroadCastIndex > 0)
	{
		ISchemeWarBroadCastInfo* pSchemeInfo = pCenter->getSchemeWarBroadCastInfo();
		SWarBroadCastInfoScheme* pInfo = pSchemeInfo->getWarBroadCastInfoByIndex(pEntityInfo->nBroadCastIndex);
		if (pInfo != NULL)
		{
			// 查找设置广播信息
			if (cmdMonsterDieTxtInfo.isSampCamp == 1)
			{
				cmdMonsterDieTxtInfo.nSoundID = pInfo->nSelfCampSoundID;
				sstrcpyn(cmdMonsterDieTxtInfo.szDes, pInfo->szSelfCampDes,sizeof(cmdMonsterDieTxtInfo.szDes));
			}
			else
			{
				cmdMonsterDieTxtInfo.nSoundID = pInfo->nEnimyCampSoundID;
				sstrcpyn(cmdMonsterDieTxtInfo.szDes, pInfo->szEnimyCampDes,sizeof(cmdMonsterDieTxtInfo.szDes));
			}
		}
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_IMPORTANT_MONSTER_DIE_TXT,0,0,&cmdMonsterDieTxtInfo, sizeof(cmd_war_important_monsterdietxtinfo));
}

void CWarPart::OnMessageEntityFightMinMapInfo(void *pData, size_t nLen)
{
	if (m_pMaster ==NULL)
	{
		return;
	}

	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_entity_fight_mini_map_info))
	{
		return;
	}

	msg_entity_fight_mini_map_info *pEntityInfo = (msg_entity_fight_mini_map_info *)pData;


	cmd_war_entity_fight_minimap_info cmdEntiyFightMiniMapInfo;
	cmdEntiyFightMiniMapInfo.nType = pEntityInfo->nInfoType;
	cmdEntiyFightMiniMapInfo.SrcUID = pEntityInfo->dwSrcID;

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ENTITY_FIGHT_MINIMAP_INFO,0,0,&cmdEntiyFightMiniMapInfo, sizeof(cmd_war_entity_fight_minimap_info));
}

void CWarPart::OnMessageSkillState( void* pData, size_t nLen )
{
	if ( m_pMaster == NULL || pData == NULL )
	{
		return;
	}

	msg_player_skill_state* pSkillState = (msg_player_skill_state*)pData;
	cmd_player_skill_state cmdPlayerSkillState;
	cmdPlayerSkillState.nID = pSkillState->nID;
	cmdPlayerSkillState.nCoolTime= pSkillState->nCoolTime;
	cmdPlayerSkillState.uOperator = pSkillState->uOperator;
	IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity( pSkillState->uOperator);
	if (pEntity  == NULL)
	{
		return;
	}
	IEntityProperty *pEntityProperty = (IEntityProperty *)pEntity->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		return;
	}
	cmdPlayerSkillState.uPBUID=pEntityProperty->getProperty_Integer(PROPERTY_ID);
	//// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_PLAYER_SKILL_STATE,0,0,&cmdPlayerSkillState, sizeof(cmdPlayerSkillState));//使用技能后灭掉图标

}

void CWarPart::OnMessageShowKillIcon( void* pData, size_t nLen )
{
	if ( m_pMaster == NULL || pData == NULL )
	{
		return;
	}

	int nSkillNum = *(int*)pData;

	// 通知显示层
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_KILL_ICON, nSkillNum, 0, 0, 0);

}

void CWarPart::OnMessageInitPersonInfo( void *pData, size_t nLen )
{
    TraceLn(__FUNCTION__"init record info");

	// 初始化的数据请求
	if (m_pMaster == NULL || pData == NULL )
	{
		ErrorLn("recive date info is error or m_pMaster == NULL");
		return;
	}

	// 初始化队伍右侧队友信息
	OnMessageUpdatePersonInfo(pData, nLen);

	RequestPersenInfoComplete();

	m_IsOnceRequest = false;

	// 将快捷键信息发送到显示层
	//sendSlotInf();
}

bool CWarPart::fillCmdWarpersoninfo(PDBID idActor, msg_war_person_info *pPersonFirst)
{   
	CheckBaseInfoIsChange(pPersonFirst);
	cmd_war_personinfo PersonInfo;
	PersonInfo.uPDBID  = pPersonFirst->uPDBID;
	PersonInfo.nCamp	= pPersonFirst->nCamp;
	PersonInfo.nIndex	= pPersonFirst->nIndex;
	PersonInfo.uID		= pPersonFirst->uID;
	PersonInfo.nKillNum	= pPersonFirst->nKillNum;	// 玩家击杀数
	PersonInfo.nDeadNum	= pPersonFirst->nDeadNum;	// 死亡数
	PersonInfo.nHeroID	= pPersonFirst->nHeroID;	// 英雄ID
	PersonInfo.nSkinID	= pPersonFirst->nSkinID;	// 皮肤ID
	PersonInfo.nLevel	= pPersonFirst->nLevel;		// 等级
	PersonInfo.nEXP		= pPersonFirst->nEXP;		// 经验
	PersonInfo.nContKill= pPersonFirst->nContKill;	// 连杀
	PersonInfo.nAssists	= pPersonFirst->nAssists;	// 助攻
	PersonInfo.nKillMonsterNum	= pPersonFirst->nKillMonsterNum; //怪物击杀数量
	PersonInfo.bIsOnGame = (int)pPersonFirst->bIsOnGame;	// 是否在线
	PersonInfo.nIsAi = PersonInfo.nIsAi;
	PersonInfo.nAliveContAssist = pPersonFirst->nAliveContAssist;
	PersonInfo.nSex = pPersonFirst->bySex;
	PersonInfo.nPlayerType = pPersonFirst->byPlayerType;
	sstrcpyn(PersonInfo.szName, pPersonFirst->gcPlayerName, sizeof(PersonInfo.szName));

	if ( PersonInfo.uPDBID == idActor )
	{
		PersonInfo.isProtag = 1;
	}

	// 召唤师技能信息是固定信息，防止重复设置
	if(PersonInfo.nTalentId1 <= 0 )
	{
		int nSummerID = pPersonFirst->nSummerID;
		PersonInfo.nTalentId1 = nSummerID;

		ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
		if ( pSchemeCenter)
		{
			ISchemeSummonerSkill* pSchemeSummonerInfo = pSchemeCenter->getSchemeSummonerSkill();
			if ( pSchemeSummonerInfo )
			{
				ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
				if (pSpellFactory )
				{
					SPELL_DATA *pSpellData = pSpellFactory->getSpellData(nSummerID);
					if (pSpellData != NULL)
					{
						PersonInfo.nSummerIconID = pSpellData->nIcon;
					}

					SSummonerSkillInfo* pSummonerInfo = pSchemeSummonerInfo->getSummonerSkillInfo(nSummerID);
					if(pSummonerInfo != NULL)
					{
						sstrcpyn( PersonInfo.szSummerDesc, pSummonerInfo->stDesc, sizeof( PersonInfo.szSummerDesc));
					}
				}
			}
		}
	}
	// 记录到本地数据
	if (PersonInfo.uPDBID != INVALID_PDBID)
	{
		m_mapPersonWarRecord[PersonInfo.uPDBID] = PersonInfo;
	}

	return true;
}

bool CWarPart::CheckBaseInfoIsChange( msg_war_person_info *pPersonFirst )
{
	PDBID idActor = pPersonFirst->uPDBID;
	map<PDBID,cmd_war_personinfo>::iterator it = m_mapPersonWarRecord.find(idActor);
	if( it == m_mapPersonWarRecord.end())
	{
		m_bBaseInfoIsChange = true;
	}

	cmd_war_personinfo cmdWarPersonInfo;
	cmdWarPersonInfo = it->second;
	if ( cmdWarPersonInfo.nKillNum != pPersonFirst->nKillNum || cmdWarPersonInfo.nDeadNum != pPersonFirst->nDeadNum || cmdWarPersonInfo.nLevel != pPersonFirst->nLevel||
		cmdWarPersonInfo.nAssists != pPersonFirst->nAssists || cmdWarPersonInfo.nKillMonsterNum <= pPersonFirst->nKillMonsterNum ||cmdWarPersonInfo.bIsOnGame != pPersonFirst->bIsOnGame)
	{
		m_bBaseInfoIsChange = true;
	}

	return m_bBaseInfoIsChange;
}

void CWarPart::sendSlotInf()
{
	// 发送槽位对应映射
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return;
	}

	ISchemeSlotMapedShortcut* pSlotMapedShortcut = pCenter->getSchemeSlotMapedShortcut();
	if ( NULL==pSlotMapedShortcut )
	{
		return;
	}
	ISchemeSpellSlot*  pSchemeSpellSlot=pCenter->getSchemeSpellSlot();
	if (pSchemeSpellSlot == NULL)
	{
		return;
	}
	IEntityProperty *pProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if ( pProperty==NULL )
	{
		return;
	}
	vector<SSlotMapedShortcutScheme> slotVec = pSlotMapedShortcut->getAllSlotInfo();

	//obuf obData;
	//cmd_count cmdCount;
	//cmdCount.nCount = slotVec.size();
	//obData << cmdCount;
	//vector<SSlotMapedShortcutScheme>::iterator itBegin = slotVec.begin();
	//int nVocation=pProperty->getProperty_Integer( PROPERTY_VOCATION );
	//SPELL_SLOT *pSpellInfo=pSchemeSpellSlot->getSpellSlot(nVocation);

	//for (;itBegin != slotVec.end(); ++itBegin )
	//{
	//	cmd_Slot_Shortcut cmdSlotShortcut;
	//	cmdSlotShortcut.nSlotId = itBegin->nSlotIndex;
	//	cmdSlotShortcut.nType = itBegin->nSlotType;
	//	if( cmdSlotShortcut.nType ==ShortCutSlotType_Skills&&cmdSlotShortcut.nSlotId<20)
	//	{
	//		cmdSlotShortcut.nSkillId=pSpellInfo->nSpellID[cmdSlotShortcut.nSlotId];
	//	}
	//	sstrcpyn(cmdSlotShortcut.szShortcut, itBegin->szKeyName, sizeof(itBegin->szKeyName));
	//	obData.push_back(&cmdSlotShortcut, sizeof(cmd_Slot_Shortcut));
	//}
	//if (((IClientEntity*)m_pMaster)->isHero()) 
	//{
	//	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SHORTCUTSCHEME_INFO, 0, 0, obData.data(), obData.size());
	//}
}

void CWarPart::loadEndRecordData(msg_war_entity_end_warinfo* pInfo, int nPlayerCount, obuf& obData )
{
	//获取图标
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return;
	}
	ISchemePersonModel* pSchemePersonModel= pCenter ->getSchemePersonModel();
	if(pSchemePersonModel==NULL)
	{
		return;
	}

	ISchemeSummonerSkill* pSchemeInfo = pCenter->getSchemeSummonerSkill();
	if ( pSchemeInfo == NULL )
	{
		return;
	}

	ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
	if (pSpellFactory == NULL)
	{
		return;
	}

	ISchemeMatchRankConfigInfo* pMatchRank = pCenter->getSchemeMatchRankConfig();
	if (pMatchRank == NULL)
		return;

	//obuf obData;
	cmd_count cmdCount;
	cmdCount.nCount = nPlayerCount;
	obData << cmdCount;


	for (int i = 0; i < nPlayerCount; ++i )
	{
		cmd_entity_end_player_data cmdData;
		cmdData.pDbid = pInfo->pDbid;
		cmdData.nCamp = pInfo->nCamp;
		cmdData.nScore = pInfo->nScore;

		int nOldPlayerMatchScore = pInfo->nPlayerMatchScore - pInfo->nMatchTypeScore;
		// 修正最新的匹配分(因为有保底分)
		int nCurRankScore = getAddMatchScoreAfterScore(pInfo->nMatchType, nOldPlayerMatchScore, pInfo->nMatchTypeScore);

		cmdData.nPlayerMatchScore = nCurRankScore;
		cmdData.nOldPlayerMatchScore = nOldPlayerMatchScore;
		cmdData.nMatchScore = pInfo->nMatchTypeScore;
		cmdData.nUpgradeRank = pMatchRank->cmpRankGrade(pInfo->nMatchType, nOldPlayerMatchScore, nCurRankScore);
		cmdData.nMatchType = pInfo->nMatchType;
		cmdData.nPlayerType = pInfo->nPlayerType;
		sstrcpyn( cmdData.szMatchTypeName, pInfo->szPlayerMatchName, sizeof(cmdData.szMatchTypeName));
		sstrcpyn( cmdData.szPlayerName, pInfo->PlayerName, sizeof(cmdData.szPlayerName));
		memcpy(cmdData.gzAllData, pInfo->gzAllData, sizeof(pInfo->gzAllData));
		cmdData.nTitle = pInfo->nTitle;
		loadTalentSKill(pInfo->pDbid, pInfo->nTalentSkill1, pInfo->nTalentSkill2);

		map<PDBID,cmd_war_personinfo>::iterator it = m_mapPersonWarRecord.find(pInfo->pDbid);
		if ( it != m_mapPersonWarRecord.end())
		{
			const cmd_war_personinfo& cmdPersonInfo = it->second;
			cmdData.bIsSelf = cmdPersonInfo.isProtag;
			cmdData.nIconId = cmdPersonInfo.nHeroID;

			if (pSchemeInfo->getSummonerSkillInfo(cmdPersonInfo.nTalentId1) != NULL)
			{

				SPELL_DATA *pSpellData = pSpellFactory->getSpellData(cmdPersonInfo.nTalentId1);
				if (pSpellData != NULL)
				{
					cmdData.nTalentSkillId1 = pSpellData->nIcon;
				}

				sstrcpyn( cmdData.summonerSkillDesc1, pSchemeInfo->getSummonerSkillInfo(cmdPersonInfo.nTalentId1)->stDesc, sizeof(cmdData.summonerSkillDesc1));
			}

			if (pSchemeInfo->getSummonerSkillInfo(cmdPersonInfo.nTalentId2) != NULL)
			{
				SPELL_DATA *pSpellData = pSpellFactory->getSpellData(cmdPersonInfo.nTalentId2);
				if (pSpellData != NULL)
				{
					cmdData.nTalentSkillId2 = pSpellData->nIcon;
				}

				sstrcpyn( cmdData.summonerSkillDesc2, pSchemeInfo->getSummonerSkillInfo(cmdPersonInfo.nTalentId2)->stDesc, sizeof(cmdData.summonerSkillDesc2));
			}

			sstrcpyn( cmdData.szHeroName, pSchemePersonModel->getHeroName(cmdPersonInfo.nHeroID), sizeof(cmdData.szHeroName));
		}

		pInfo++;
		obData << cmdData;

	}
}

void CWarPart::loadTalentSKill(PDBID pDbid, int nTalentSkill1, int nTalentSkill2)
{    
	map<PDBID,cmd_war_personinfo>::iterator it = m_mapPersonWarRecord.find(pDbid);
	if ( it == m_mapPersonWarRecord.end())
	{
		return;
	}
	cmd_war_personinfo* pInfo = &it->second;
	pInfo->nTalentId1 = nTalentSkill1;
	pInfo->nTalentId2 = nTalentSkill2;
}



//接受事件处理函数
void CWarPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch(wEventID)
	{//实体创建事件
	case EVENT_ENTITY_CREATE_VIEW:
		{  
			obuf ob;
			IEntityProperty* pProperty = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
			if(pProperty == NULL)
			{
				ErrorLn("onUpdateProp failed, !property, name="<<m_pMaster->getName());
				break;
			}

			// 在战场中,请求战场基本信息
			if (m_WarID != -1)
			{
				SendMessageToEntityScene(m_pMaster->getUID(), PART_WAR, CS_MSG_WAR_REQ_WAR_BASE_INFO, NULL, 0);
			}
		}
		break;
	default:
		break;
	}
}

void CWarPart::OnMessageBackGroundMusic(void *pData, size_t nLen)
{
	if ( m_pMaster == NULL || pData == NULL )
	{
		return;
	}

	if (nLen != sizeof(msg_war_backround_music))
	{
		return;
	}
	msg_war_backround_music *pMsg = (msg_war_backround_music *)pData;
	cmd_creature_play_sound playSound;
	playSound.nSoundID = pMsg->nSoundID;
	playSound.nStop = 0;

	//// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_PLAY_SOUND, 0, 0, &playSound, sizeof(playSound));
}

bool CWarPart::limitPosFlagNum()
{
	DWORD dwSynTick = gClientGlobal->getTimeSyncer()->GetTick();

	if (m_CanPosSignal == true)
	{
		if ( dwSynTick - m_PosTime < POS_TIME_INTERVAL && m_PosNum >= POS_NUM_MAX)
		{
			m_CanPosSignal = false;
			m_PosTime = dwSynTick;
			return false;
		}else if( dwSynTick - m_PosTime >= POS_TIME_INTERVAL)
		{
			m_PosTime = dwSynTick;
			m_PosNum = 0;
		}
	}else
	{
		if ( dwSynTick - m_PosTime > POS_RECOVER_TIME )
		{
			m_PosTime = dwSynTick;
			m_PosNum = 0;
			m_CanPosSignal = true;
		}else
		{
			return false;
		}
	}
	return true;
}

void CWarPart::OnMessageAllPlayerEntityCreated(void *pData, size_t nLen)
{
	if ( m_pMaster == NULL || pData == NULL )
	{
		return;
	}
	int* pCount = (int*)pData;

	if (nLen != (*pCount) * sizeof(msg_war_all_players_created) + sizeof(int))
	{
		ErrorLn(__FUNCTION__": The length is not equal nLen = " << nLen << ", (*pCount) * sizeof(msg_war_all_players_created) + sizeof(int) = " << (*pCount) * sizeof(msg_war_all_players_created) + sizeof(int));
		return;
	}

	msg_war_all_players_created *pMsg = (msg_war_all_players_created *)(pCount + 1);
	obuf obdata;
	cmd_count cmdPersonCount;
	cmdPersonCount.nCount=*pCount;
	obdata.push_back(&cmdPersonCount, sizeof(cmd_count));
	for ( int i = 0; i < *pCount; ++i )
	{
		obuf uidData;
		// 由于后续功能也需要向显示层发送uid和pbuid，所以在此全发下去，在cs中过滤。
		cmd_Send_UID_And_PBUID cmdDoubleUID;
		cmdDoubleUID.uUID=pMsg->uUID;
		cmdDoubleUID.uPDBID=pMsg->uPDBID;
		cmdDoubleUID.nCamp=pMsg->nCamp;
		obdata.push_back(&cmdDoubleUID,sizeof(cmd_Send_UID_And_PBUID));
		pMsg++;
	}
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SEND_UIDANDPUBUID,0,0, obdata.data(),obdata.size());
}

void CWarPart::OnMessageProcessBroadcast(void *pData, size_t nLen)
{
	if (m_pMaster ==NULL)
	{
		return;
	}

	//获取图标
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return;
	}

	ISchemeMonster* pScheme = pCenter->getSchemeMonster();
	if ( NULL==pScheme )
	{
		return;
	}
	//校验数据大小
	if (pData == NULL || nLen != sizeof(msg_war_process_broadcast))
	{
		return;
	}
	IEntityProperty *pEntityProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		return;
	}

	msg_war_process_broadcast *pBroadInfo = (msg_war_process_broadcast *)pData;

	cmd_war_important_monsterdietxtinfo cmdTxtInfo;
	cmdTxtInfo.isSampCamp = 0;

	if (pBroadInfo->nBroadCastID > 0)
	{
		ISchemeWarBroadCastInfo* pSchemeInfo = pCenter->getSchemeWarBroadCastInfo();
		SWarBroadCastInfoScheme* pInfo = pSchemeInfo->getWarBroadCastInfoByIndex(pBroadInfo->nBroadCastID);
		if (pInfo != NULL)
		{
			// 查找设置广播信息
			cmdTxtInfo.nSoundID = pInfo->nEnimyCampSoundID;
			sstrcpyn(cmdTxtInfo.szDes, pInfo->szEnimyCampDes, sizeof(cmdTxtInfo.szDes));
		}
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_IMPORTANT_MONSTER_DIE_TXT,0,0,&cmdTxtInfo, sizeof(cmd_war_important_monsterdietxtinfo));
}

void CWarPart::OnMessageSendWarStaticsTotal(void *pData, size_t nLen)
{
	if (m_pMaster == NULL || pData == NULL)
	{
		return;
	}

	if (nLen < sizeof(msg_war_send_warstatics_total))
	{
		return;
	}
	ISchemePersonModel* pSchemePersonModel = gClientGlobal->getSchemeCenter()->getSchemePersonModel();

	msg_war_send_warstatics_total *pMsg = (msg_war_send_warstatics_total*)pData;
	if (nLen != sizeof(msg_war_send_warstatics_total) + pMsg->wWarInfoCount * sizeof(DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO))
	{
		return;
	}

	BYTE *offsetData = (BYTE*)pData + sizeof(msg_war_send_warstatics_total);
	obuf8192 ob;
	cmd_entity_update_warstatistic_total info;
	info.nActorID = pMsg->nActorID;
	info.nRecentKillCount = pMsg->dwRecentKillCount;
	info.nRecentDieCount = pMsg->dwRecentDieCount;
	info.nRecentAssistCount = pMsg->dwRecentAssistCount;
	info.nRecentKDAScore = pMsg->wRecentKDAScore;
	info.nRecentWinCount = pMsg->wRecentWinCount;
	info.nRecentLoseCount = pMsg->wRecentLoseCount;
	info.nWarInfoCount = pMsg->wWarInfoCount;
	info.nRecentScore = pMsg->dwRecentScore;
	ob.push_back(&info, sizeof(info));

	DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO *pWarInfo = (DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO*)offsetData;
	offsetData += pMsg->wWarInfoCount * sizeof(DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO);
	for (int i = 0; i < pMsg->wWarInfoCount; ++i)
	{
		cmd_entity_update_warstatistic_total_warinfo warInfo;
		warInfo.nWarID = pWarInfo->nWarID;
		warInfo.nBeginTime = pWarInfo->dwBeginTime;
		warInfo.nHeroID = pWarInfo->wHeroID;
		warInfo.nWarType = pWarInfo->wWarType;
		warInfo.nWin = pWarInfo->byWin;
		warInfo.nWinType = pWarInfo->byWinType;
		for (int i = 0; i < DBDEF_BAN_HERO_SIZE; ++i)
		{
			warInfo.nBanHeroID1[i] = pWarInfo->wBanHeroID1[i];
			warInfo.nBanHeroID2[i] = pWarInfo->wBanHeroID2[i];
		}

		ob.push_back(&warInfo, sizeof(warInfo));

		++pWarInfo;
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_WARSTATISTIC_TOTAL, 0, 0, ob.data(), ob.size());
}

void CWarPart::OnMessageSendWarStaticsWar(void *pData, size_t nLen)
{
	if (m_pMaster == NULL || pData == NULL)
	{
		return;
	}

	if (nLen < sizeof(msg_war_send_warstatics_war))
	{
		return;
	}

	msg_war_send_warstatics_war *pMsg = (msg_war_send_warstatics_war*)pData;
	if (nLen != sizeof(msg_war_send_warstatics_war) + pMsg->dwActorCount * sizeof(DBREQ_PARAM_WAR_ACTOR_INFO))
	{
		return;
	}

	BYTE *offsetData = (BYTE*)pData + sizeof(msg_war_send_warstatics_war);
	obuf8192 ob;
	cmd_entity_update_warstatistic_war info;
	info.nWarID = pMsg->nWarID;
	info.nBeginTime = pMsg->dwBeginTime;
	info.nEndTime = pMsg->dwEndTime;
	info.nWarType = pMsg->wWarType;
	info.nEndBecause = pMsg->wEndBecause;
	info.nActorCount = pMsg->dwActorCount;
	for (int i = 0; i < DBDEF_BAN_HERO_SIZE; ++i)
	{
		info.nBanHeroID1[i] = pMsg->wBanHeroID1[i];
		info.nBanHeroID2[i] = pMsg->wBanHeroID2[i];
	}
	ob.push_back(&info, sizeof(info));

	DBREQ_PARAM_WAR_ACTOR_INFO *pActorInfo = (DBREQ_PARAM_WAR_ACTOR_INFO*)offsetData;
	offsetData += pMsg->dwActorCount * sizeof(DBREQ_PARAM_WAR_ACTOR_INFO);
	for (int i = 0; i < (int)pMsg->dwActorCount; ++i)
	{
		cmd_entity_update_warstatistic_war_actorinfo actorInfo;
		actorInfo.nWorldID = pActorInfo->dwWorldID;
		actorInfo.nUserID = pActorInfo->dwUserID;
		actorInfo.nActorID = pActorInfo->dwActorID;
		strcpy_s(actorInfo.szActorName, sizeof(actorInfo.szActorName), pActorInfo->szActorName);
		for (int i = 0; i < MAX_GOODS_SLOT_COUNT; ++i )
		{
			actorInfo.nEquipID[i] = pActorInfo->dwEquipID[i];
		}
		actorInfo.nHeroID = pActorInfo->wHeroID;
		actorInfo.nHeroLevel = pActorInfo->wHeroLevel;
		actorInfo.nScore = pActorInfo->dwScore;
		actorInfo.nGetMoney = pActorInfo->dwGetMoney;
		actorInfo.nDieCount = pActorInfo->wDieCount;
		actorInfo.nAssistCount = pActorInfo->wAssistCount;
		actorInfo.nKillPersonNum = pActorInfo->wKillPersonNum;
		actorInfo.nKillMonsterNum = pActorInfo->wKillMonsterNum;
		actorInfo.nContinueKillMax = pActorInfo->wContinueKillMax;
		actorInfo.nMultiKillMax = pActorInfo->byMultiKillMax;
		actorInfo.nCrildDamageMax = pActorInfo->dwCrildDamageMax;
		actorInfo.nKillTowerNum = pActorInfo->byKillTowerNum;
		actorInfo.nOutHeroPhysics = pActorInfo->dwOutHeroPhysics;
		actorInfo.nOutHeroMagic = pActorInfo->dwOutHeroMagic;
		actorInfo.nOutHeroTrue = pActorInfo->dwOutHeroTrue;
		actorInfo.nOutTotal = pActorInfo->dwOutTotal;
		actorInfo.nDamageTotal = pActorInfo->dwDamageTotal;
		actorInfo.nBearMagic = pActorInfo->dwBearMagic;
		actorInfo.nBearPhysics = pActorInfo->dwBearPhysics;
		actorInfo.nBearTrue = pActorInfo->dwBearTrue;
		actorInfo.nCureTotal = pActorInfo->dwCureTotal;
		actorInfo.nCamp = pActorInfo->byCamp;
		actorInfo.nRecoveryHP = pActorInfo->dwRecoveryHP;
		actorInfo.nHeroType = pActorInfo->byHeroType;
		actorInfo.nKdaScore = pActorInfo->wKdaScore;
		actorInfo.nGlory = pActorInfo->wGlory;
		actorInfo.nWinType = pActorInfo->byWinType;
		actorInfo.nPlayerType = pActorInfo->byPlayerType;
		actorInfo.nAliveAssistMax = pActorInfo->byAliveAssistMax;
		actorInfo.nGPM = pActorInfo->wGPM;
		actorInfo.bFirstBlood = pActorInfo->byFirstBlood > 0 ? true : false;
		actorInfo.nDeadlyControl = pActorInfo->byDeadlyControl;
		actorInfo.nMoneyRate = pActorInfo->byMoneyRate;
		actorInfo.nOutputRate = pActorInfo->byOutputRate;
		actorInfo.nJoinBattleRate = pActorInfo->byJoinBattleRate;
		actorInfo.nThrillSave = pActorInfo->byThrillSave;
		actorInfo.nKillDragen = pActorInfo->byKillDragen;
		actorInfo.nXPSkillID = pActorInfo->dwXPSkillID;
		actorInfo.nSummerID = pActorInfo->dwSummerID;
		actorInfo.bySex = pActorInfo->nSex;

		ob.push_back(&actorInfo, sizeof(actorInfo));

		++pActorInfo;
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_WARSTATISTIC_WAR, 0, 0, ob.data(), ob.size());
}

void CWarPart::OnMessageSendHistoryRecord(void *pData, size_t nLen)
{
	if (pData == NULL)
	{
		return;
	}

	if (nLen < sizeof(int))
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nLen << " < sizeof(int) = " << sizeof(int));
		return;
	}

	int count = *(int*)pData;
	// 效验长度值
	int nEfficacyLen = sizeof(int) + count * sizeof(msg_war_send_history_record);
	if (nLen != nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nLen << "!= sizeof(msg_war_send_history_record) * count + sizeof(int) Size= " << nEfficacyLen);
		return;
	}

	obuf1024 ob;
	ob << count;
	msg_war_send_history_record* pHistoryRecord = (msg_war_send_history_record*)((char*)pData + sizeof(int));
	for (int i = 0; i < count; ++i, ++pHistoryRecord)
	{
		cmd_warstatistic_history_record record;
		record.wWarType = pHistoryRecord->wWarType;
		record.bIsFull = pHistoryRecord->bIsFull;
		record.nHeroID = pHistoryRecord->nHeroID;
		record.wAssistCount = pHistoryRecord->wAssistCount;
		record.wDieCount = pHistoryRecord->wDieCount;
		record.wKillPersonNum = pHistoryRecord->wKillPersonNum;
		ob.push_back(&record, sizeof(record));
	}

	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_HISTORY_RECORD, 0, 0, ob.data(), ob.size());
}

void CWarPart::OnMessageOtherCrownPageInfo(void* pData, int nLen)
{
	if (m_pMaster == NULL)
	{
		return;
	}

	if (pData == NULL)
	{
		return;
	}

	// 效验长度值
	int nEfficacyLen = sizeof(CROWN_PAGE_ITEM)*(int)CROWN_PAGE_MAX_COUNT + sizeof(GEMSTONE_ITEM)*(int)CROWN_GEMSTONE_MAX_COUNT;
	if (nLen != nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength =" << nLen << "< AllCrownPage Size= " << nEfficacyLen);
		return;
	}

	obuf obData1;
	obuf obData2;
	obuf obData3;

	cmd_count cmdCount;
	char *pPageHead = (char*)(pData);
	for (int i = 0; i<CROWN_PAGE_MAX_COUNT; ++i)
	{
		CROWN_PAGE_ITEM* pGemstoneItem = (CROWN_PAGE_ITEM*)pPageHead;
		pPageHead = (char*)(pGemstoneItem + 1);

		if (pGemstoneItem == NULL)
			continue;

		if (pGemstoneItem->nPageDBID <= 0)
			continue;

		cmd_Crown_Page_Item_Info info;
		info.nPageDBID = pGemstoneItem->nPageDBID;
		sstrcpyn(info.szName, pGemstoneItem->szName, sizeof(info.szName));
		memcpy(info.crownInlaySlotList, pGemstoneItem->crownInlaySlotList, sizeof(info.crownInlaySlotList));

		obData2.push_back(&info, sizeof(info));
		cmdCount.nCount++;
	}

	if (cmdCount.nCount == 0)
	{
		int nMaxPageCount = min(CROWN_PAGE_DEFAULT_COUNT, 20);
		for (int i = 0; i<nMaxPageCount; ++i)
		{
			char szCrownPageName[GAME_NAME_MAXSIZE] = { 0 };
			int nCurPageIndex = i + 1;
			string sDefaultName;

			Msg_CrownPage_Save MsgData;
			MsgData.nPageDBID = nCurPageIndex;
			ssprintf_s(szCrownPageName, sizeof(szCrownPageName), "核晶模组%d", nCurPageIndex);
			sstrcpyn(MsgData.szCrownPageName, a2utf8(szCrownPageName), sizeof(MsgData.szCrownPageName));

			cmd_Crown_Page_Item_Info info;
			info.nPageDBID = nCurPageIndex;
			ssprintf_s(info.szName, sizeof(info.szName), "核晶模组%d", nCurPageIndex);
			sstrcpyn(info.szName, a2utf8(szCrownPageName), sizeof(info.szName));
			obData2.push_back(&info, sizeof(info));
			cmdCount.nCount++;
		}
	}

	cmd_count cmdCount2;
	char *pCrownHead = (char*)(pPageHead);
	for (int i = 0; i<CROWN_GEMSTONE_MAX_COUNT; ++i)
	{
		GEMSTONE_ITEM* pGemstoneItem = (GEMSTONE_ITEM*)pCrownHead;
		pCrownHead = (char*)(pGemstoneItem + 1);

		if (pGemstoneItem == NULL)
			continue;

		if (pGemstoneItem->nGemstoneDBID <= 0)
			continue;

		cmd_Crown_Gemstone_Item_Info info;

		info.nGemstoneID = pGemstoneItem->nGemstoneID;
		info.nGemstoneDBID = pGemstoneItem->nGemstoneDBID;
		info.byGrade = pGemstoneItem->byGrade;
		info.nExp = pGemstoneItem->nExp;
		info.bAppraisal = pGemstoneItem->bAppraisal;
		info.nAppraisalCount = pGemstoneItem->nAppraisalCount;

		obData3.push_back(&info, sizeof(info));
		cmdCount2.nCount++;
	}

	obData1.push_back(&cmdCount, sizeof(cmdCount));
	obData1.push_back(obData2.data(), obData2.size());


	obData1.push_back(&cmdCount2, sizeof(cmdCount));
	obData1.push_back(obData3.data(), obData3.size());

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CROWNG_GEMSTONE_OTHER_INLAY, 0, 0, obData1.data(), obData1.size());
}

void CWarPart::OnMessageXPRandomPlayerInCool(void *pData, size_t nLen)
{
	if (m_pMaster == NULL || pData == NULL)
	{
		return;
	}

	// 通告显示层xp技能随机到自己去显示倒计时框
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_XP_RANDOMSELF_BEGINCOOL, 0, 0, NULL, NULL);
}

int CWarPart:: GetWarID()
{
	return m_WarID;
}

void CWarPart::OnMessageUpdateTab(void *pData, size_t nLen)
{
	// 战场结束
	if ( m_WarIsEnd == true)
	{
		return;
	}
	// 战绩数据缓存
	if (m_pMaster == NULL )
	{
		return;
	}
	// 解析数据
	if (pData == NULL || nLen < sizeof(msg_war_person_info_list) )
	{
		return;
	}
	msg_war_person_info_list *pMsgPersonListHead = (msg_war_person_info_list *)pData;
	int nPersonNum = pMsgPersonListHead->nNum;

	if(nLen != sizeof(msg_war_update_persontab)*nPersonNum + sizeof(msg_war_person_info_list))
	{
		ErrorLn(__FUNCTION__ ": nLen != sizeof(msg_war_person_info)*nPersonNum + sizeof(msg_war_person_info_list)  nLen = " << nLen << ", sizeof(msg_war_person_info)*nPersonNum + sizeof(msg_war_person_info_list) = " << sizeof(msg_war_person_info)*nPersonNum + sizeof(msg_war_person_info_list));
		return;
	}

	IEntityProperty *pEntityProperty = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
	if (pEntityProperty == NULL)
	{
		ErrorLn(__FUNCTION__": pEntityProperty == NULL");
		return;
	}
	PDBID idActor =pEntityProperty->getProperty_Integer(PROPERTY_ID);

	msg_war_update_persontab *pPersonFirst = (msg_war_update_persontab *)(pMsgPersonListHead + 1);

	// 更新显示层数据相关
	cmd_war_personInfo_count cmdWarPersonInfoCount;
	int nCount = pMsgPersonListHead->nNum;
	cmdWarPersonInfoCount.nCount = nCount;
	obuf obdata;
	obdata.push_back(&cmdWarPersonInfoCount, sizeof(cmd_war_personInfo_count ));

	// 处理数据
	for (int i = 0; i < nPersonNum; ++i)
	{
		//更新本地数据
		map<PDBID,cmd_war_personinfo>::iterator iter =  m_mapPersonWarRecord.find(pPersonFirst->uPDBID);
		if (iter == m_mapPersonWarRecord.end())
		{
			ErrorLn(__FUNCTION__": can't find tab info! update fail! uID ="<<pPersonFirst->uID);
			continue;
		}

		iter->second.bIsOnGame  = pPersonFirst->bIsOnGame;
		iter->second.uID        = pPersonFirst->uID;		// 玩家ID
		iter->second.nKillNum   = pPersonFirst->nKillNum;	// 玩家击杀数
		iter->second.nDeadNum   = pPersonFirst->nDeadNum;	// 死亡数
		iter->second.nLevel     = pPersonFirst->nLevel;		// 等级
		iter->second.nContKill  = pPersonFirst->nContKill;	// 连杀
		iter->second.nAssists   = pPersonFirst->nAssists;	// 助攻
		iter->second.nKillMonsterNum = pPersonFirst->nKillMonsterNum; //金币
        iter->second.nDestroyTower = pPersonFirst->nDestroyTower;  // 推塔数
		obdata.push_back(&(iter->second), sizeof(iter->second));

		pPersonFirst++;
	}


	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_WAR_PERSONINFO,0,0,obdata.data(), obdata.size());
}

void CWarPart::OnMessageNotifyWarInfo(void * pData, size_t nLen)
{
	if (m_pMaster == NULL || pData == NULL)
	{
		return;
	}

	if (nLen < sizeof(msg_war_rsp_war_info))
	{
		return;
	}

	msg_war_rsp_war_info *pMsg = (msg_war_rsp_war_info*)pData;

	cmd_war_info cmd;
	cmd.lDBWarID = pMsg->lDBWarID;
	cmd.nConfigWarID = pMsg->nConfigWarID;
	cmd.nWarStartTick = pMsg->nWarStartTick;
	cmd.isFull = pMsg->isFull;
	cmd.nMtachType= pMsg->nMatchType;


	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_WAR_INFO, 0, 0, &cmd, sizeof(cmd));
}

void CWarPart::OnMessageEntitySearchStateChange( void * data, size_t len )
{
	if (m_pMaster == NULL || data == NULL)
	{
		return;
	}

	if (len < sizeof(msg_war_special_entity_search_state_change))
	{
		return;
	}

	msg_war_special_entity_search_state_change *pMsg = (msg_war_special_entity_search_state_change*)data;
	cmd_Special_Entity_Search_State_Change cmdSearchState;
	cmdSearchState.uidOperator = pMsg->uidOperator;
	cmdSearchState.bSearch = pMsg->bSearch;

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SPECIAL_ENTITY_SEARCH_STATE_CHANGE,0,0,&cmdSearchState, sizeof(cmd_Special_Entity_Search_State_Change));
}

void CWarPart::onMessageSpecialContribution( void * data, size_t len )
{
	if (m_pMaster == NULL || data == NULL)
	{
		return;
	}

	if (len < sizeof(msg_war_special_contribution))
	{
		return;
	}

	msg_war_special_contribution *pMsg = (msg_war_special_contribution*)data;
	cmd_War_Special_Contribution cmdInfo;
	cmdInfo.nType = (int)pMsg->eType;
	cmdInfo.nOperatorHeroID = pMsg->nOperatorHeroID;
	cmdInfo.nParam1 = pMsg->nParam1;
	sstrcpyn(cmdInfo.szOperatorName, pMsg->szOperatorName, sizeof(cmdInfo.szOperatorName));

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SPECCIAL_CONTRIBUTION,0,0,&cmdInfo, sizeof(cmd_War_Special_Contribution));
}

void CWarPart::onMessageAllFilterSupportPerson( void * data, size_t len )
{
	if (m_pMaster == NULL || data == NULL)
	{
		return;
	}

	int nEfficacyLen=sizeof(int);
	if(len<nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<len<<"< int Size= "<<nEfficacyLen);
		return ;
	}

	int* pPlayerNum =(int*)data;

	obuf obData;
	cmd_count playerCount;
	playerCount.nCount = *pPlayerNum;	
	obData.push_back(&playerCount,sizeof(cmd_count));

	char *pHead=(char*)(pPlayerNum+1);
	for (int i=0;i<playerCount.nCount;++i )
	{
		// 效验长度
		nEfficacyLen=nEfficacyLen+sizeof(msg_war_filter_support_person);
		if (len<nEfficacyLen)
		{
			ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<len<<"< EfficacyTotalLen= "<<nEfficacyLen);
			return ;
		}

		if(pHead == NULL)
			continue;

		msg_war_filter_support_person* pMsg = (msg_war_filter_support_person*)pHead;
		cmd_update_support_vote_person cmdInfo;
		cmdInfo.uPDBIU = pMsg->uPDBIU;
		cmdInfo.nVoteID = pMsg->nVoteID;
		pHead = (char*)(pMsg+1);

		obData.push_back(&cmdInfo,sizeof(cmd_update_support_vote_person));
	}

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_WAR_FILTER_SUPPORT_PERSON,0,0,obData.data(),obData.size());
}

void CWarPart::onMessageAddLabel(void* data, size_t len)
{
	if (m_pMaster == NULL || data == NULL)
		return;

	if (len < sizeof(msg_war_add_label))
		return;

	msg_war_add_label* pMsg = (msg_war_add_label*)data;

	cmd_war_add_label cmdInfo;
	cmdInfo.targetPdbid = pMsg->targetPdbid;
	cmdInfo.nLabelID = pMsg->nLabelID;
	sstrcpyn(cmdInfo.szOperatorName, pMsg->szOperatorName, sizeof(cmdInfo.szOperatorName));
	sstrcpyn(cmdInfo.szTargetName, pMsg->szTargetName, sizeof(cmdInfo.szTargetName));

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_WAR_ADD_LABEL, 0, 0, &cmdInfo, sizeof(cmd_war_add_label));
}

void CWarPart::onWarEnd()
{
	if (m_pCheckBadBehavior != NULL)
	{
		m_pCheckBadBehavior->Release();
		m_pCheckBadBehavior = NULL;
	}
}

void CWarPart::onMessageActiveTalentList(void* data, size_t len)
{
	if (m_pMaster == NULL || data == NULL)
		return;

	if (len < sizeof(msg_war_active_talent_list))
		return;

	msg_war_active_talent_list* pMsg = (msg_war_active_talent_list*)data;

	cmd_war_active_talent_list cmdInfo;
	cmdInfo.nCurTalent = pMsg->nCurTalent;
	cmdInfo.pdbid = pMsg->pdbid;
	memcpy(cmdInfo.nTalentList, pMsg->nTalentList, sizeof(cmdInfo.nTalentList));
	
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ACTIVE_TALENT_LIST,0,0,&cmdInfo, sizeof(msg_war_active_talent_list));
}

void CWarPart::OnMessageWarEndLegendCupInfo(void *pData, size_t nLen)
{
    if (m_pMaster == NULL || pData == NULL)
    {
        return;
    }

    if (nLen < sizeof(msg_war_end_legendCup_info))
    {
        return;
    }

    msg_war_end_legendCup_info *pMsg = (msg_war_end_legendCup_info*)pData;
    cmd_war_end_legendcup_info cmd;
    cmd.eRoundType      = pMsg->eRoundType;
    cmd.bRoundLastNode  = pMsg->bRoundLastNode;
    cmd.bIsValid        = 1;
    cmd.bIsOnlineCup    = pMsg->bIsOnlineCup;
    cmd.bIsLastRound    = pMsg->bIsLastRound;
    cmd.bNodeEnd        = pMsg->bNodeEnd;
    cmd.bWin            = pMsg->bWin;
    cmd.dwNextStartInterval= pMsg->dwNextStartInterval;
    cmd.dwNextRoundStartTime= pMsg->dwNextRoundStartTime;

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_WAR_END_LEGENDCUP_INFO, 0, 0, &cmd, sizeof(cmd));
}

void CWarPart::restore()
{

}
