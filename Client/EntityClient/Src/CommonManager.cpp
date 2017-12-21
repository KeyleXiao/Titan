#include "StdAfx.h"
#include "CommonManager.h"
#include "EntityDef.h"
#include "EventDef.h"
#include "PlayerBankPart.h"

CCommonManager::CCommonManager()
	:m_pMaster(NULL)
{
}

CCommonManager::~CCommonManager()
{
	m_pMaster = NULL;
}

bool CCommonManager::onSerialize(rkt::ibuffer& in, int flag)
{
	return true;
}

bool CCommonManager::deSerialize(rkt::obuf& out, int flag)
{
	return true;
}

bool CCommonManager::onLoad(IEntity* pEntity)
{
	m_pMaster = pEntity;

	return true;
}

void CCommonManager::restore()
{
	m_pMaster = NULL;
}

void CCommonManager::onMessage(int nMsgID, LPCSTR pszMsg, int nLen)
{
	switch (nMsgID)
	{
	case SC_MSG_ENTITY_LABEL_LIST:
		{
			if (pszMsg == NULL || nLen < sizeof(msg_entity_label_list))
			{
				ErrorLn(__FUNCTION__ " The pointer of data is null, or the length of data isn't correct.");
				return;
			}

			msg_entity_label_list* pMsg = (msg_entity_label_list*)pszMsg;

			if (nLen < (pMsg->nCount * sizeof(LabelInfo) + sizeof(msg_entity_label_list)))
			{
				ErrorLn(__FUNCTION__ " The length of data isn't correct.");
				return;
			}

			cmd_count cmdCount;
			cmdCount.nCount = pMsg->nCount;

			obuf ob;
			ob << cmdCount;

			cmd_label_info* pLabelInfo = (cmd_label_info*)(pszMsg + sizeof(msg_entity_label_list));
			for (int i = 0; i < pMsg->nCount; ++i)
			{
				ob << *pLabelInfo;
				pLabelInfo++;
			}

			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_LABEL_LIST, 0, 0, ob.data(), ob.size());
		}
	break;
	default:
		break;
	}
}
