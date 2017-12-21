#include "stdafx.h"
#include "EmojiManager.h"
#include "PlayerBankPart.h"

CEmojiManager::CEmojiManager()
{
	m_pMaster = NULL;
	m_pBankPart = NULL;
	m_vecEmojiInfo.clear();
}

CEmojiManager::~CEmojiManager()
{

}

bool CEmojiManager::onSerialize(ibuffer& in)
{
	if (!in.good())
		return false;

	int nCount = 0;
	in >> nCount;

	m_vecEmojiInfo.clear();
	for (int i = 0; i < nCount; ++i)
	{
		if(!in.good())
			return false;

		BYTE byEmojiType;
		in >> byEmojiType;
		m_vecEmojiInfo.push_back(byEmojiType);
	}

	return true;
}

bool CEmojiManager::deSerialize(obuf& out)
{
	int nCount = m_vecEmojiInfo.size();
	out << nCount;

	for (int i = 0; i < nCount; ++i)
	{
		out << m_vecEmojiInfo[i];
	}

	return true;
}

bool CEmojiManager::init(CPlayerBankPart * pBankPart, __IEntity * pMaster, int nLoginMode)
{
	if (pBankPart == NULL || pMaster == NULL)
	{
		return false;
	}

	m_pBankPart = pBankPart;
	m_pMaster = pMaster;

	if (nLoginMode == elogin_online)
	{
		m_pBankPart->readValueGoodsInfoFromDB(ENVALUEGOODSTYPE_EMOJI);
	}

	return true;
}

void CEmojiManager::restore()
{
    m_pBankPart = NULL;
    m_pMaster = NULL;
}

void CEmojiManager::release()
{

}

bool CEmojiManager::canAddEmoji(BYTE byEmojiType)
{
	vector<BYTE>::iterator it = std::find(m_vecEmojiInfo.begin(), m_vecEmojiInfo.end(), byEmojiType);
	if (it == m_vecEmojiInfo.end())
	{
		return true;
	}

	return false;
}

bool CEmojiManager::addEmoji(BYTE byEmojiType)
{
	if (!canAddEmoji(byEmojiType))
		return false;

	m_vecEmojiInfo.push_back(byEmojiType);

	m_pBankPart->saveValueGoodsInfoToDB(ENVALUEGOODSTYPE_EMOJI, (DWORD)byEmojiType, 1);

	updateEmojiInfoToClient();
	
	// TODO »’÷æ

	return true;
}

void CEmojiManager::onDBReturnEmojiInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods)
{
	if (pvecGoods == NULL)
	{
		return;
	}

	vector<DBREQ_TABLE_VALUE_GOODS> vecGoods = *pvecGoods;

	for (int i = 0; i < vecGoods.size(); ++i)
	{
		m_vecEmojiInfo.push_back((BYTE)vecGoods[i].dwID);
	}

	updateEmojiInfoToClient();

	checkHaveFreeEmoji();
}

void CEmojiManager::updateEmojiInfoToClient()
{
	obuf obData;

	SNetMsgHead head;
	head.bySrcEndPoint	= MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule	= MSG_MODULEID_ENTITY;
	head.byKeyAction	= SC_MSG_ENTITY_UPDATE_EMOJI_INFO;
	obData << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_BANK;
	obData << entityHead;

	int nCount = m_vecEmojiInfo.size();
	obData << nCount;
	for(int i = 0; i < nCount; ++i)
	{
		obData << m_vecEmojiInfo[i];
	}

	((__IPlayerRole*)m_pMaster)->send2Client(obData.data(), obData.size());
}

void CEmojiManager::checkHaveFreeEmoji()
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;
	ISchemeEntity * pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
		return;
	SEntityXml_Server * pEntity = pSchemeEntity->getEntityXml();
	if (pEntity == NULL)
		return;

	for (int i = 0; i < MAX_FREE_EMOJI_COUNT; ++i)
	{
		if (pEntity->nArrFreeEmojiID[i] <= 0)
			continue;

		int nEmojiID = pEntity->nArrFreeEmojiID[i];
		if (std::find(m_vecEmojiInfo.begin(), m_vecEmojiInfo.end(), nEmojiID) != m_vecEmojiInfo.end())
			continue;

		addEmoji((BYTE)nEmojiID);
	}
}
