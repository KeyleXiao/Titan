/*******************************************************************
** 文件名:	CommonPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李有红
** 日  期:	2017/1/22
** 版  本:	1.0
** 描  述:	通用部件
** 应  用:  	
	
*******************************************************************/

#include "stdafx.h"
#include "CommonPart.h"
#include "ChatDef.h"
#include "EntityViewDef.h"
#include "EntityDef.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "ICamp.h"


short CommonPart::getPartID()
{
    return PART_COMMON;
}

bool CommonPart::onLoad( IEntity* pEntity)
{
    // 所属实体
    m_pMaster = pEntity;


    return true;
}

// 部件转发消息
bool CommonPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch(msgId)
	{
	case SC_MSG_ENTITY_NPC_OPEN_SHOP:
		{
			SMsgEntityHead* pEntity = (SMsgEntityHead*)pEntityHead;

			if (data == NULL || len < sizeof(msg_entity_npc_open_shop))
			{
				return false;
			}
			msg_entity_npc_open_shop* pMsg = (msg_entity_npc_open_shop*)data;

			// 验证NPC是否合法
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_NPC_OPEN_SHOP, 0, 0, NULL, 0);
		}
		break;
	case SC_MSG_ENTITY_NPC_RESPONSE:
		{
			SMsgEntityHead* pEntity = (SMsgEntityHead*)pEntityHead;

			if (data == NULL || len < sizeof(msg_entity_npc_response))
			{
				return false;
			}
			msg_entity_npc_response* pMsg = (msg_entity_npc_response*)data;

			char* strRes = (char*)data + sizeof(msg_entity_npc_response);
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_NPC_RESPONSE, 0, strRes, NULL, 0);
		}
		break;
	case SC_MSG_ENTITY_NPC_CLOSE_DIALOGBOX:
		{
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_NPC_CLOSE_DIALOGBOX, 0, NULL, NULL, 0);
		}
		break;
	case SC_MSG_ENTITY_SYNC_DATA:
		{
			if (data == NULL || len < sizeof(msg_entity_sync_data))
			{
				return false;
			}	

			msg_entity_sync_data* pMsg = (msg_entity_sync_data*)data;
			if (pMsg->nDataType & NOTIFY_GAMESTATE)
			{
				cmd_entity_update_gamestate cmd;
				cmd.nGameState = pMsg->nData1;

				m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_GAMESTATE, 0, NULL, &cmd, sizeof(cmd));
			}

            // 通知数据
            if(pMsg->nDataType & NOTIFY_DATA)
            {
                obuf ob;

                cmd_entity_notify_data cmd;
                cmd.nType  = pMsg->nData1;      // 通知类型 NOTIFY_DATA_TYPE
                cmd.nValue = pMsg->nData2;      // 通知数值
                cmd.nStrLen= pMsg->nStrLen;     // 文本长度

                ob.push_back(&cmd, sizeof(cmd));
                if(cmd.nStrLen > 0)
                {
                    ob.push_back(pMsg+1, cmd.nStrLen);
                }

                m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_NOTIFY_DATA, 0, NULL, ob.data(), ob.size());
            }
		}
		break;
    case SC_MSG_ENTITY_NPC_BUY_SUCCESS:
        {
            SMsgEntityHead* pEntity = (SMsgEntityHead*)pEntityHead;

            if (data == NULL || len < sizeof(SNPCGoodsBuySuccess))
            {
                return false;
            }
            SNPCGoodsBuySuccess* pMsg = (SNPCGoodsBuySuccess*)data;

            // 通知View成功购买NPC装备
            m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_NPC_BUY_GOODS_SUCCESS, 0, 0, pMsg, sizeof(SNPCGoodsBuySuccess));
        }
        break;

    case SC_MSG_ENTITY_SEND_NAVGROUP:
        {
            onMessageSendNavGroup(data, len);
        }
        break;

    case SC_MSG_ENTITY_SEND_WEBURL:
        {
            onMessageSendWebUrl(data, len);
        }
        break;

    default: break;
	}

    return true;
}

bool CommonPart::onCommand( int cmdid,void * data,size_t len )
{
	switch(cmdid)
	{
	case ENTITY_CMD_NPC_TRIGGER:
		{
			if (data == NULL || len < sizeof(cmd_entity_npc_trigger))
			{
				ErrorLn(__FUNCTION__ ",data == NULL || len < sizeof(cmd_entity_npc_trigger)");
				return false;
			}

			cmd_entity_npc_trigger* pCmd= (cmd_entity_npc_trigger*)data;

			onMeetNPC(pCmd->uidNPC);
		}
		break;
	case ENTITY_CMD_NPC_FUNC:
		{
			if (data == NULL || len < sizeof(cmd_entity_npc_func))
			{
				ErrorLn(__FUNCTION__ ",data == NULL || len < sizeof(cmd_entity_npc_func)");
				return false;
			}

			cmd_entity_npc_func* pCmd= (cmd_entity_npc_func*)data;

			// 查找发送到哪个服务器，第一个'/'前的字符串为目标服务器标识
			// 格式例子：Server/HTMLRequestHandler?1=RequestBuyWings&2=17&3=18&4=**abc
			char* pReqStr = pCmd->szHTMLRequest;
			char* pfirstSlash = pReqStr;
			while(*pfirstSlash != '\0' && *pfirstSlash != '/') pfirstSlash++;
			string strEndPoint(pReqStr, pfirstSlash-pReqStr);

			// 判断格式是否正确
			int strLen = pfirstSlash - pReqStr;
			if (*pfirstSlash == '\0' || strLen >= strlen(pReqStr))
			{
				ErrorLn(__FUNCTION__ ", Illegal HTMLRequest format.szHTMLRequest=" << pReqStr)
				return false;
			}

			char* pFinalStr = pfirstSlash + 1;

			msg_entity_npc_func msg;
			msg.uidNPC = pCmd->uidNPC;
			msg.dwParamLen = strlen(pFinalStr) + 1;		// 加上后面的'\0'

			obuf ob;
			ob << msg;
			ob.push_back(pFinalStr, msg.dwParamLen);

			if (strEndPoint == "Server")
			{
				// 发送到场景服
				SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_NPC_FUNC, ob.data(), ob.size());
			}
			else if(strEndPoint == "Social")
			{
				// 发送到社会服, TODO...
			}
			else
			{
				ErrorLn(__FUNCTION__ ",Unknow endpoint,strEndPoint=" << strEndPoint);
				return false;
			}
		}
		break;
	case ENTITY_CMD_GAMESTATE_LEAVE:
		{
			if (data == NULL || len != sizeof(cmd_entity_gamestate_leave))
			{
				ErrorLn(__FUNCTION__ ",data == NULL || len ！= sizeof(cmd_entity_gamestate_leave)");
				return false;
			}

			cmd_entity_gamestate_leave* pCmd = (cmd_entity_gamestate_leave*)data;
			
			msg_entity_gamestate_leave msg;
			msg.isLeave = pCmd->isLeave;

			SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_GAMESTATE_LEAVE, &msg, sizeof(msg));
		}
		break;
	case ENTITY_CMD_WAR_CHANGE_VOCATION:
		{
			if (data == NULL || len != sizeof(cmd_entity_war_change_vocation))
			{
				ErrorLn(__FUNCTION__ ",data == NULL || len ！= sizeof(cmd_entity_war_change_vocation)");
				return false;
			}
			
			cmd_entity_war_change_vocation* pCmd = (cmd_entity_war_change_vocation*)data;

			msg_entity_war_change_vocation msg;
			msg.nGenicSlotID = pCmd->nGenicSlotID;

			SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_WAR_CHANGE_VOCATION, &msg, sizeof(msg));
		}
		break;

    case ENTITY_CMD_REQUEST_NAVGROUP:       // 请求游戏导航栏
        {
            SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_REQUEST_NAVGROUP, NULL, 0);
        }
        break;
    case ENTITY_CMD_REQUEST_WEBURL:       // 请求URL字符串
        {
            msg_entity_request_weburl_cs msg;
            msg.nWebUrlID = *((int*)data);
            SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_REQUEST_WEBURL, &msg, sizeof(msg));
        }
        break;

	default:
		return false;
	}

    return true;
}

IEntity * CommonPart::getHost()
{
    return m_pMaster;
}

void CommonPart::release()
{
    // 所属实体
    m_pMaster = NULL;

    delete this;
}

void CommonPart::restore()
{

}

bool CommonPart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
    return true;
}

bool CommonPart::deSerialize( rkt::obuf & out,int flag )
{
    return true;
}


void CommonPart::onMeetNPC( UID uidNPC )
{
	// 最后点击NPC的时间
	static DWORD m_dwLastClickTicks = 0;
	// 最后一次点击的NPC
	static UID m_LastClickNPC = 0;

    // 非主角
    if( NULL == m_pMaster || !m_pMaster->isHero() )
    {
        return;
    }

    // 不是怪物将不能点击
    if (TYPE_MONSTER != UID_2_TYPE(uidNPC))
    {
        return;
    }

    // 根据UID查找实体
    IClientEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidNPC);
    if (NULL == pEntity)
    {
        return;
    }

    int nSubType = pEntity->getIntProperty(PROPERTY_VOCATION);
    // 非NPC
    if (MONSTER_SUB_TYPE_NPC != nSubType)
    {
        return;
    }

    int nCamp = pEntity->getIntProperty(PROPERTY_CAMP);
    if (CAMP_NEUTRAL != nCamp && CampFlag_Friend != Camp::getCampFlag(m_pMaster, pEntity))
    {
        return;
    }


    TraceLn("Trigger NPC, uidNPC=" << uidNPC);

	msg_entity_npc_trigger msg;
	msg.uidNPC = uidNPC;

	DWORD dwNowTicks = GetTickCount();
	// 同一个NPC两次点击的间隔为500ms
	if (m_LastClickNPC == uidNPC && (dwNowTicks - m_dwLastClickTicks) < 500)
	{
		return;
	}

	m_LastClickNPC = uidNPC;
	m_dwLastClickTicks = dwNowTicks;

	SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_NPC_TRIGGER, &msg, sizeof(msg));
}

// 接收到服务器下发NavGroup数据
void CommonPart::onMessageSendNavGroup(void * data, size_t len)
{
    if (NULL == data || len < sizeof(msg_entity_send_navgroup))
        return;
    msg_entity_send_navgroup * pMsg = static_cast<msg_entity_send_navgroup*>(data);

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_PROCESS_NAVGROUP, pMsg->nNavCount, NULL, pMsg+1, len-sizeof(msg_entity_send_navgroup));
}

// 接收到服务器下发WebUrl数据
void CommonPart::onMessageSendWebUrl(void * data, size_t len)
{
    if (NULL == data || len < sizeof(msg_entity_send_weburl_sc))
        return;
    msg_entity_send_weburl_sc * pMsg = static_cast<msg_entity_send_weburl_sc*>(data);
    if (len < sizeof(msg_entity_send_weburl_sc) + pMsg->nStrLen )
        return;

    processWebUrlToView(pMsg->nWebUrlID, reinterpret_cast<char*>(pMsg+1));
}

// 处理WebUrl通知到显示层
void CommonPart::processWebUrlToView(int nWebUrlID, const char* szUrl)
{
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_PROCESS_WEBURL, nWebUrlID, szUrl, NULL, 0);
}

