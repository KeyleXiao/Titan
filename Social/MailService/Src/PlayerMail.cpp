#include "StdAfx.h"
#include "MailService.h"
#include "PlayerMail.h"
#include "ExternalHelper.h"
#include "ISocialGlobal.h"
#include "IOSSLogService.h"
#include "Mail_ManagedDef.h"
#include "OssLogDef.h"

using namespace Mail_ManagerDef;

CPlayerMail::CPlayerMail(DWORD dwPDBID)
{
	m_dwPDBID = dwPDBID;
	m_MailList.clear();
}

/// purpose: 析构函数
CPlayerMail::~CPlayerMail(void)
{

}

void CPlayerMail::release()
{
	delete this;
}

// 添加邮件基础数据
void CPlayerMail::addMailBaseData(SMailBaseData *pMailBaseData)
{
    if ( m_MailList.find(pMailBaseData->llMailID) == m_MailList.end() )
    {
        m_MailList.insert(pair<LONGLONG, SMailBaseData>(pMailBaseData->llMailID, *pMailBaseData));
    }
}

// 发送邮件基础数据列表toClient
void CPlayerMail::sendMailListToClient()
{
	obuf ob;
	SMsgMail_MailList sendData;
	sendData.dwPDBID = m_dwPDBID;
	sendData.nCount = 0;

	obuf2048 obNode;
	for (TMap_MailListIt it = m_MailList.begin(); it != m_MailList.end(); ++it)
	{
		obNode.push_back(&((*it).second), sizeof(SMailBaseData));
		++sendData.nCount;
	}

	fillMailData(ob, MSG_ENDPOINT_CLIENT, MSG_MAIL_REQUESTMAILLIST);
	ob.push_back(&sendData, sizeof(sendData));
	ob.push_back(obNode.data(), obNode.size());
	g_EHelper.SendDataToPerson(m_dwPDBID, ob.data(), ob.size());
}

// 获取邮件数量
int CPlayerMail::getPlayerMailCount(int& nPlusNum)
{
	int nCount = 0;

	for (TMap_MailListIt it = m_MailList.begin(); it != m_MailList.end(); ++it)
	{
		if ((*it).second.bRead == false)
			++nCount;
		
		if ((*it).second.bIsPlusData && ((*it).second.nType == emMailType_System || (*it).second.nType == emMailType_Player))
			++nPlusNum;
	}

	return nCount;
}

// 删除邮件
void CPlayerMail::delMail(LONGLONG llMailID)
{
	TMap_MailListIt it = m_MailList.find(llMailID);
	if (it != m_MailList.end())
	{
		m_MailList.erase(it);
	}
}

// 设置邮件已读
void CPlayerMail::setMailRead(LONGLONG llMailID)
{
	TMap_MailListIt it = m_MailList.find(llMailID);
	if (it != m_MailList.end())
	{
		(*it).second.bRead = true;
	}
}

// 通知玩家新邮件数量提示
void CPlayerMail::notifyPlayerNewMailCount()
{
	int nPlusNum = 0;
	int nNewCount = getPlayerMailCount(nPlusNum);
	if (nNewCount > 0)
	{
		obuf ob;
		SMsgMail_NewMailCount sendData;
		sendData.nCount = nNewCount;
		sendData.nPlusCount = nPlusNum;
		fillMailData(ob, MSG_ENDPOINT_CLIENT, MSG_MAIL_NEWMAILCOUNT);
		ob.push_back(&sendData, sizeof(sendData));
		g_EHelper.SendDataToPerson(m_dwPDBID, ob.data(), ob.size());
	}
}