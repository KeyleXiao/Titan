/*******************************************************************
** 文件名: EquipSchemeMgr.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/4/18
** 版  本: 1.0
** 描  述: 装备方案管理
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#include "Stdafx.h"
#include "EquipSchemeMgr.h"
#include "WarMessageDef.h"
#include "EntityViewDef.h"
#include "Entity_inl.h"

//#define __EQUIPSCHEME_MGR_DEBUG__

CEquipSchemeMgr::CEquipSchemeMgr()
	:m_pMaster(NULL)
{
}

CEquipSchemeMgr::~CEquipSchemeMgr()
{
}

bool CEquipSchemeMgr::init(IEntity* pMaster)
{
	if (pMaster == NULL)
		return false;

	m_pMaster = pMaster;

	return true;
}

void CEquipSchemeMgr::restore()
{
	m_pMaster = NULL;
}

bool CEquipSchemeMgr::onSerialize(rkt::ibuffer & in, int flag)
{

	return true;
}

bool CEquipSchemeMgr::deSerialize(rkt::obuf & out, int flag)
{
	return true;
}

bool CEquipSchemeMgr::onMessage(int msgId, void * data, size_t len)
{
	switch (msgId)
	{
		case SC_MSG_WAR_EQUIPSCHEME_LIST:
		{
			if (len < sizeof(msg_war_equipscheme_list))
			{
				ErrorLn(__FUNCTION__": The length of msg is too short.len=" << len);
				return false;
			}

			msg_war_equipscheme_list* msg = (msg_war_equipscheme_list*)data;
			int nSchemeCount = msg->nSchemeCount;

			int nNeedLen = sizeof(msg_war_equipscheme_list) + nSchemeCount * sizeof(SWarEquipScheme);
			if (len < nNeedLen)
			{
				ErrorLn(__FUNCTION__": The length of msg is too short than need length.len=" << len << ", nNeedLen=" << nNeedLen);
				return false;
			}

			obuf ob;
			cmd_war_equipscheme_list cmd;
			cmd.nHeroID = msg->nHeroID;
			cmd.pdbid = msg->pdbid;
			cmd.nSchemeCount = msg->nSchemeCount;

			ob << cmd;

			SWarEquipScheme * pScheme = (SWarEquipScheme*)((char*)data + sizeof(msg_war_equipscheme_list));
			for (int i = 0; i < nSchemeCount; ++i)
			{
				cmd_war_equipscheme scheme;
				scheme.nHeroID = cmd.nHeroID;
				scheme.bySchemeID = pScheme->bySchemeID;
				sstrcpyn(scheme.szAuthorName, pScheme->szAuthorName, sizeof(scheme.szAuthorName));
				sstrcpyn(scheme.szSchemeName, pScheme->szSchemeName, sizeof(scheme.szSchemeName));
				sstrcpyn(scheme.szTitle, pScheme->szSchemeTitle, sizeof(scheme.szTitle));
				for (int j = 0; j < WARQUIPSCHEME_EQUIP_MAXSIZE; ++j)
				{
					scheme.nEquipIDList[j] = pScheme->equipWeightInfo[j].nEquipID;
				}

				ob << scheme;

				pScheme++;
			}

			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_EQUIPSCHEME_LIST, 0, NULL, ob.data(), ob.size());
		}
		break;
		default:break;
	}

	return true;
}

bool CEquipSchemeMgr::onCommand(int cmdid, void * data, size_t len)
{
	switch (cmdid)
	{
		case ENTITY_CMD_WAR_ADD_EQUIPSCHEME:
		{
			if (len < sizeof(cmd_war_add_equipscheme))
			{
				return false;
			}

			cmd_war_add_equipscheme* pCmd = (cmd_war_add_equipscheme*)data;
			
			msg_war_add_equipscheme msg;
			msg.nHeroID = pCmd->nHeroID;
			sstrcpyn(msg.szAuthorName, pCmd->szAuthorName, sizeof(msg.szAuthorName));
			sstrcpyn(msg.szSchemeName, pCmd->szSchemeName, sizeof(msg.szSchemeName));
			sstrcpyn(msg.szSchemeTitle, pCmd->szTitle, sizeof(msg.szSchemeTitle));
			for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
			{
				msg.nEquipIDList[i] = pCmd->nEquipIDList[i];
			}

			SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_ADD_EQUIPSCHEME, &msg, sizeof(msg));
		}
		break;
		case ENTITY_CMD_WAR_DEL_EQUIPSCHEME:
		{
			if (len < sizeof(cmd_war_del_equipscheme))
			{
				return false;
			}

			cmd_war_del_equipscheme* pCmd = (cmd_war_del_equipscheme*)data;

			msg_war_del_equipscheme msg;
			msg.nHeroID = pCmd->nHeroID;
			msg.bySchemeID = pCmd->bySchemeID;

			SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_DEL_EQUIPSCHEME, &msg, sizeof(msg));
		}
		break;
		case ENTITY_CMD_WAR_MODIFY_EQUIPSCHEME:
		{
			if (len < sizeof(cmd_war_modify_equipscheme))
			{
				return false;
			}

			cmd_war_modify_equipscheme* pCmd = (cmd_war_modify_equipscheme*)data;

			msg_war_modify_equipscheme msg;
			msg.nHeroID = pCmd->nHeroID;
			msg.bySchemeID = pCmd->bySchemeID;
			sstrcpyn(msg.szAuthorName, pCmd->szAuthorName, sizeof(msg.szAuthorName));
			sstrcpyn(msg.szSchemeName, pCmd->szSchemeName, sizeof(msg.szSchemeName));
			sstrcpyn(msg.szSchemeTitle, pCmd->szTitle, sizeof(msg.szSchemeTitle));
			for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
			{
				msg.nEquipIDList[i] = pCmd->nEquipIDList[i];
			}

			SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_MODIFY_EQUIPSCHEME, &msg, sizeof(msg));
		}
		break;
		case ENTITY_CMD_WAR_READ_EQUIPSCHEME:
		{
			if (len < sizeof(cmd_war_read_equipscheme))
			{
				return false;
			}

			cmd_war_read_equipscheme* pCmd = (cmd_war_read_equipscheme*)data;

			msg_war_read_equipscheme msg;
			msg.nHeroID = pCmd->nHeroID;

			SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_READ_EQUIPSCHEME, &msg, sizeof(msg));
		}
		break;
	default:
		break;
	}

	return true;
}



void CEquipSchemeMgr::printEquipSchemeInfo(const SWarEquipScheme& scheme)
{
	TraceLn("----------------装备方案 Start--------------------");
	TraceLn("schemeID=" << scheme.bySchemeID);
	TraceLn("schemeName=" << scheme.szSchemeName);
	TraceLn("authorName" << scheme.szAuthorName);
	TraceLn("schemeTitle" << scheme.szSchemeTitle);
	for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
	{
		TraceLn(scheme.equipWeightInfo[i].nEquipID << "(" << scheme.equipWeightInfo[i].nWeight << ") ")
	}
	TraceLn("--------------装备方案 End----------------------");
}