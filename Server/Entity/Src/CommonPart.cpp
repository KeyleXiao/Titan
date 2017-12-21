/*******************************************************************
** 文件名:	CommonPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/1/22
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "CommonPart.h"
#include "EntityHelper.h"
#include "IPlayerRole.h"
#include "ICenterConnectorService.h"
#include "EntityDef.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "IActorService.h"
#include "EntityHelper.h"
#include "CampDef.h"
#include "ProgressBarObserver.h"
#include "IBankPart.h"
#include "WebUrlManager.h"


CommonPart::CommonPart()
	:m_pMaster(NULL)
	,m_pGuideStep(NULL)
    ,m_nChargeValue(0)
	,m_pProgressBarChangeVoc(0)
{

}
CommonPart::~CommonPart()
{

}

short CommonPart::getPartID()
{
	return PART_COMMON;
}

void CommonPart::restore()
{
    // 释放前优先停止一些逻辑处理然后再释放
}

bool CommonPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	if (pEntity == NULL)
	{
		return false;
	}

	m_pMaster = pEntity;
	if (m_pMaster->getType() != TYPE_PLAYER_ROLE)
	{
		return false;
	}
	// 人物才有这个功能
	m_pGuideStep = new CGuideStep();
	if ( m_pGuideStep )
	{
		m_pGuideStep->create(this);
	}

	m_pProgressBarChangeVoc = new CProgressBarObserver();
	if (m_pProgressBarChangeVoc->create(m_pMaster) == false)
	{
		m_pProgressBarChangeVoc->release();
		m_pProgressBarChangeVoc = NULL;
		return false;
	}

	__IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		ErrorLn("CommonPart onLoad pEntityEvent==NULL");
		return false;
	}

	// 订阅全局事件 角色游戏状态发生改变
	bool nReturn = pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_CHANGE_GAMESTATE, __FUNCTION__);

	return true;
}

__IEntity * CommonPart::getHost()
{
	return m_pMaster;
}

bool CommonPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    if(!m_pMaster)
    {
        return false;
    }

	switch(msgId)
	{
	case CS_MSG_ENTITY_NPC_TRIGGER:     // 玩家触发NPC
		{
            onMessageTriggerNpc(data, len);
		}
		break;

	case CS_MSG_ENTITY_NPC_FUNC:        // 玩家点击NPC功能
		{
            onMessageFireNpcFunction(data, len);
		}
		break;

    case CS_MSG_RIGHTBUTTON_DOWN:       // 鼠标右键按下
        {
            onMessageRightButtonClick(data, len);
        }
        break;

	case CS_MSG_ENTITY_GAMESTATE_LEAVE:
		{
			onMessageGameStateIsLeave(data, len);
		}
		break;
	case CS_MSG_ENTITY_WAR_CHANGE_VOCATION:
		{
			onMessageCastProcessbarSpell(data, len);
		}
		break;

    case CS_MSG_ENTITY_REQUEST_NAVGROUP:             // 请求发送WEB导航
        {
            onMessageRequestNavGroup(data, len);
        }
        break;

    case CS_MSG_ENTITY_REQUEST_WEBURL:                   // 请求发送WebURL
        {
            onMessageRequestWebUrl(data, len);
        }
        break;

    default:
        break;
	}
	
	return true;
}

void CommonPart::release()
{
	if (m_pMaster != NULL)
	{
		__IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
		if (pEntityEvent != NULL)
		{
			// 取消订阅全局事件 角色游戏状态发生改变
			pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_CHANGE_GAMESTATE);
		}
	}

    if(m_pGuideStep)
    {
        m_pGuideStep->release();
    }
	m_pGuideStep = nullptr;
	
	if (m_pProgressBarChangeVoc != NULL)
	{
		m_pProgressBarChangeVoc->release();
	}
	m_pProgressBarChangeVoc = NULL;

	m_pMaster = NULL;

	delete this;
}

bool CommonPart::onSerialize( rkt::ibuffer & in,int flag )
{
	return true;
}

bool CommonPart::deSerialize( rkt::obuf & out,int flag )
{
	return true;
}

/////////////////////////////////////////__ICommonPart/////////////////////////////////////////
/*@name 通知客户端数据
@param nNotifyType: 通知数据类型NOTICE_DATA_TYPE
@param nValue:      数值内容
@param pText:       文本内容
@return 
*/
bool CommonPart::notifyClientData(int nNotifyType, int nValue, char* pText)
{
    obuf msg;

    msg_entity_sync_data data;
    data.nDataType = NOTIFY_DATA;
    data.nData1 = nNotifyType;  // 通知类型
    data.nData2 = nValue;       // 通知数值
    // 通知文本
    if(pText) {
        data.nStrLen = SIZE_INT(strlen(pText) + sizeof(char));
    }

    msg.push_back(&data, sizeof(data));
    // 压入字符串数据
    if(data.nStrLen > 0) {
        msg << pText;
    }

    SendEntityMessage(m_pMaster->getUID(), PART_COMMON, SC_MSG_ENTITY_SYNC_DATA, msg.data(), SIZE_INT(msg.size()) );

    // 如果是步骤将保存到数据库中
    if(nNotifyType == NOTIFY_TYPE_STEP)
    {
    	saveGuideStep(nValue);
    }

    return true;
}


/*@name 保存引导步骤
@param nStep: 引导步骤
@return 
*/
bool CommonPart::saveGuideStep(int nStep)
{
    return m_pGuideStep ? m_pGuideStep->save(nStep) : false;
}

/*@name 获取实体自身当前充能值
@return 
*/
int CommonPart::getChargeValue(void)
{
    return m_nChargeValue;
}

/*@name 设置实体自身当前充能值
@param nValue: 充能数值
@return 
*/
void CommonPart::setChargeValue(int nValue)
{
    m_nChargeValue = nValue;
}

/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
void CommonPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if (m_pMaster == NULL)
	{
		return;
	}

	switch(wEventID)
	{
	case EVENT_ACTOR_CHANGE_GAMESTATE:
		{
			if (nLen < sizeof(event_entity_actor_change_gamestate))
			{
				return ;
			}

			event_entity_actor_change_gamestate* pEvent = (event_entity_actor_change_gamestate*)pszContext;
			
			msg_entity_sync_data data;
			data.nDataType = NOTIFY_GAMESTATE;
			data.nData1 = pEvent->dwState;      // 游戏状态

			SendEntityMessage(m_pMaster->getUID(), PART_COMMON, SC_MSG_ENTITY_SYNC_DATA, (LPCSTR)&data, sizeof(data));
		}
		break;
	default:
		break;
	}
}

void CommonPart::SendEntityMessage(UID uid, short byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
	obuf obufData;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule	= MSG_MODULEID_ENTITY;
	head.byKeyAction	= byKeyAction;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = uid;
	entityHead.byPartID = byPartID;

	obufData << head << entityHead;
	obufData.push_back(pszData,nDataLen);

	((__IPlayerRole*)m_pMaster)->send2Client(obufData.data(), obufData.size());
}


//////////////////////////////////////////////////////////////////////////////////
// 触发NPC
void CommonPart::onMessageTriggerNpc(void * data, size_t len)
{
    if (data == NULL || len < sizeof(msg_entity_npc_trigger))
    {
        ErrorLn(__FUNCTION__ ", data == NULL || len < sizeof(msg_entity_npc_trigger)");
        return;
    }
    msg_entity_npc_trigger* pMsg = (msg_entity_npc_trigger*)data;

    UID uidNPC = pMsg->uidNPC;
    // 非NPC将不能点击
    if (TYPE_MONSTER != UID_2_TYPE(uidNPC))
    {
        return;
    }
    int nSubType = getProperty_Integer(uidNPC, PROPERTY_VOCATION);
    if (MONSTER_SUB_TYPE_NPC != nSubType)
    {
        return;
    }
    int nCamp = getProperty_Integer(uidNPC, PROPERTY_CAMP);
    // 非中立或本阵营的NPC不允许对话
    if (CAMP_NEUTRAL != nCamp && m_pMaster->getIntProperty(PROPERTY_CAMP) != nCamp)
    {
        return;
    }

    __IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent)
    {
        event_entity_npc_trigger eventdata;
        eventdata.uidNPC = uidNPC;
		eventdata.pdbid = m_pMaster->getIntProperty(PROPERTY_ID);

        pEntityEvent->FireExecute(EVENT_GAME_NPC_TRIGGER, (LPCSTR)&eventdata, sizeof(eventdata));
    }
}

// 触发NPC函数功能
void CommonPart::onMessageFireNpcFunction(void * data, size_t len)
{
    if (data == NULL || len < sizeof(msg_entity_npc_func))
    {
        ErrorLn(__FUNCTION__ ", data == NULL || sizeof(msg_entity_npc_func) < len");
        return;
    }

    msg_entity_npc_func* pMsg = (msg_entity_npc_func*)data;

    __IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent == NULL)
    {
        return;
    }

    event_entity_npc_func eventdata;
    eventdata.uidNPC = pMsg->uidNPC;
    eventdata.dwParamLen = pMsg->dwParamLen;
	eventdata.pdbid = m_pMaster->getIntProperty(PROPERTY_ID);

    char* paramStr = (char*)data + sizeof(msg_entity_npc_func);
    DWORD paramLen = len - sizeof(msg_entity_npc_func);

    if (paramLen != eventdata.dwParamLen)
    {
        ErrorLn(__FUNCTION__ "paramLen != eventdata.dwParamLen, Invalid data length.");
        return;
    }

    obuf ob;
    ob << eventdata;
    ob.push_back(paramStr, paramLen);

    pEntityEvent->FireExecute(EVENT_GAME_NPC_FUNC, (LPCSTR)ob.data(), ob.size());
}

// 右键点击
void CommonPart::onMessageRightButtonClick(void * data, size_t len)
{
    __IEntityEvent* pEntityEvent = m_pMaster->getEntityEvent();
    if(pEntityEvent == NULL)
        return;

    pEntityEvent->FireExecute(EVENT_ENTITY_RIGHTBUTTON_DOWN, NULL, 0);
}

void CommonPart::onMessageGameStateIsLeave(void * data, size_t len)
{
	if (data == NULL || len < sizeof(msg_entity_gamestate_leave))
	{
		ErrorLn(__FUNCTION__ ", data == NULL || sizeof(msg_entity_gamestate_leave) < len");
		return;
	}

	msg_entity_gamestate_leave* pMsg = (msg_entity_gamestate_leave*)data;

	__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole == NULL)
		return;

	int nOldState = pPlayerRole->getGameState();

	if (nOldState == GAME_STATE_IDLE && pMsg->isLeave)
	{
		pPlayerRole->setGameState(GAME_STATE_LEAVE);
	}
	else if(nOldState == GAME_STATE_LEAVE && !pMsg->isLeave)
	{
		pPlayerRole->setGameState(GAME_STATE_IDLE);
	}
}


void CommonPart::onMessageCastProcessbarSpell(void * data, size_t len)
{
	if (data == NULL || len != sizeof(msg_entity_war_change_vocation))
	{
		return;
	}

	msg_entity_war_change_vocation* pMsg = (msg_entity_war_change_vocation*)data;

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemeEntity* pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		return;
	}

	SEntityXml_Server* pEntityXml = pSchemeEntity->getEntityXml();
	if (pEntityXml == NULL)
	{
		return;
	}

	if (m_pProgressBarChangeVoc != NULL)
	{
		m_pProgressBarChangeVoc->start(pEntityXml->nChangeVocSpellID, pMsg->nGenicSlotID);
	}
}

// 请求下发Web导航组
void CommonPart::onMessageRequestNavGroup(void * data, size_t len)
{
    __IBankPart* pBankPart = CAST_TYPE(__IBankPart*, m_pMaster->getEntityPart(PART_BANK));
    if (NULL == pBankPart)
    {
        return;
    }

    obuf1024 ob;
    int nNavCount = g_WebUrlManager.getNavGroup(ob);

    msg_entity_send_navgroup msg;
    msg.nNavCount = static_cast<BYTE>(nNavCount);

    obuf1024 obMsg;
    obMsg.push_back(&msg, sizeof(msg));
    if (nNavCount > 0) {
        obMsg.push_back(ob.data(), ob.size());
    }

    SendEntityMessage(m_pMaster->getUID(), PART_COMMON, SC_MSG_ENTITY_SEND_NAVGROUP, (LPCSTR)obMsg.data(), obMsg.size());
}

// 请求下发Web页信息
void CommonPart::onMessageRequestWebUrl(void * data, size_t len)
{
    if (NULL == data || len != sizeof(msg_entity_request_weburl_cs))
    {
        return;
    }
    msg_entity_request_weburl_cs * pMsg = static_cast<msg_entity_request_weburl_cs*>(data);

    __IBankPart* pBankPart = CAST_TYPE(__IBankPart*, m_pMaster->getEntityPart(PART_BANK));
    if (NULL == pBankPart)
    {
        return;
    }

    string strUrl = g_WebUrlManager.getWebUrl(pMsg->nWebUrlID, pBankPart->getFromGameWorldID(), pBankPart->getUserID(), m_pMaster->getIntProperty(PROPERTY_ID), pBankPart->getPlayerInfo()->szPwd);
    //TraceLn("Web="<< strUrl.c_str() );
    msg_entity_send_weburl_sc msg;
    msg.nWebUrlID = pMsg->nWebUrlID;
    msg.nStrLen = strUrl.length()+1;
    
    obuf obMsg;
    obMsg << msg;
    obMsg.push_back(strUrl.c_str(), msg.nStrLen);

    SendEntityMessage(m_pMaster->getUID(), PART_COMMON, SC_MSG_ENTITY_SEND_WEBURL, (LPCSTR)obMsg.data(), obMsg.size());
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __ICommonPart * CreateCommonPart()
{
	return new CommonPart;
}