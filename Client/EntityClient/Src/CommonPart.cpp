/*******************************************************************
** �ļ���:	CommonPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	���к�
** ��  ��:	2017/1/22
** ��  ��:	1.0
** ��  ��:	ͨ�ò���
** Ӧ  ��:  	
	
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
    // ����ʵ��
    m_pMaster = pEntity;


    return true;
}

// ����ת����Ϣ
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

			// ��֤NPC�Ƿ�Ϸ�
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

            // ֪ͨ����
            if(pMsg->nDataType & NOTIFY_DATA)
            {
                obuf ob;

                cmd_entity_notify_data cmd;
                cmd.nType  = pMsg->nData1;      // ֪ͨ���� NOTIFY_DATA_TYPE
                cmd.nValue = pMsg->nData2;      // ֪ͨ��ֵ
                cmd.nStrLen= pMsg->nStrLen;     // �ı�����

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

            // ֪ͨView�ɹ�����NPCװ��
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

			// ���ҷ��͵��ĸ�����������һ��'/'ǰ���ַ���ΪĿ���������ʶ
			// ��ʽ���ӣ�Server/HTMLRequestHandler?1=RequestBuyWings&2=17&3=18&4=**abc
			char* pReqStr = pCmd->szHTMLRequest;
			char* pfirstSlash = pReqStr;
			while(*pfirstSlash != '\0' && *pfirstSlash != '/') pfirstSlash++;
			string strEndPoint(pReqStr, pfirstSlash-pReqStr);

			// �жϸ�ʽ�Ƿ���ȷ
			int strLen = pfirstSlash - pReqStr;
			if (*pfirstSlash == '\0' || strLen >= strlen(pReqStr))
			{
				ErrorLn(__FUNCTION__ ", Illegal HTMLRequest format.szHTMLRequest=" << pReqStr)
				return false;
			}

			char* pFinalStr = pfirstSlash + 1;

			msg_entity_npc_func msg;
			msg.uidNPC = pCmd->uidNPC;
			msg.dwParamLen = strlen(pFinalStr) + 1;		// ���Ϻ����'\0'

			obuf ob;
			ob << msg;
			ob.push_back(pFinalStr, msg.dwParamLen);

			if (strEndPoint == "Server")
			{
				// ���͵�������
				SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_NPC_FUNC, ob.data(), ob.size());
			}
			else if(strEndPoint == "Social")
			{
				// ���͵�����, TODO...
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
				ErrorLn(__FUNCTION__ ",data == NULL || len ��= sizeof(cmd_entity_gamestate_leave)");
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
				ErrorLn(__FUNCTION__ ",data == NULL || len ��= sizeof(cmd_entity_war_change_vocation)");
				return false;
			}
			
			cmd_entity_war_change_vocation* pCmd = (cmd_entity_war_change_vocation*)data;

			msg_entity_war_change_vocation msg;
			msg.nGenicSlotID = pCmd->nGenicSlotID;

			SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_WAR_CHANGE_VOCATION, &msg, sizeof(msg));
		}
		break;

    case ENTITY_CMD_REQUEST_NAVGROUP:       // ������Ϸ������
        {
            SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_REQUEST_NAVGROUP, NULL, 0);
        }
        break;
    case ENTITY_CMD_REQUEST_WEBURL:       // ����URL�ַ���
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
    // ����ʵ��
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
	// �����NPC��ʱ��
	static DWORD m_dwLastClickTicks = 0;
	// ���һ�ε����NPC
	static UID m_LastClickNPC = 0;

    // ������
    if( NULL == m_pMaster || !m_pMaster->isHero() )
    {
        return;
    }

    // ���ǹ��ｫ���ܵ��
    if (TYPE_MONSTER != UID_2_TYPE(uidNPC))
    {
        return;
    }

    // ����UID����ʵ��
    IClientEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidNPC);
    if (NULL == pEntity)
    {
        return;
    }

    int nSubType = pEntity->getIntProperty(PROPERTY_VOCATION);
    // ��NPC
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
	// ͬһ��NPC���ε���ļ��Ϊ500ms
	if (m_LastClickNPC == uidNPC && (dwNowTicks - m_dwLastClickTicks) < 500)
	{
		return;
	}

	m_LastClickNPC = uidNPC;
	m_dwLastClickTicks = dwNowTicks;

	SendMessageToEntityScene(m_pMaster->getUID(), PART_COMMON, CS_MSG_ENTITY_NPC_TRIGGER, &msg, sizeof(msg));
}

// ���յ��������·�NavGroup����
void CommonPart::onMessageSendNavGroup(void * data, size_t len)
{
    if (NULL == data || len < sizeof(msg_entity_send_navgroup))
        return;
    msg_entity_send_navgroup * pMsg = static_cast<msg_entity_send_navgroup*>(data);

    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_PROCESS_NAVGROUP, pMsg->nNavCount, NULL, pMsg+1, len-sizeof(msg_entity_send_navgroup));
}

// ���յ��������·�WebUrl����
void CommonPart::onMessageSendWebUrl(void * data, size_t len)
{
    if (NULL == data || len < sizeof(msg_entity_send_weburl_sc))
        return;
    msg_entity_send_weburl_sc * pMsg = static_cast<msg_entity_send_weburl_sc*>(data);
    if (len < sizeof(msg_entity_send_weburl_sc) + pMsg->nStrLen )
        return;

    processWebUrlToView(pMsg->nWebUrlID, reinterpret_cast<char*>(pMsg+1));
}

// ����WebUrl֪ͨ����ʾ��
void CommonPart::processWebUrlToView(int nWebUrlID, const char* szUrl)
{
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_PROCESS_WEBURL, nWebUrlID, szUrl, NULL, 0);
}

