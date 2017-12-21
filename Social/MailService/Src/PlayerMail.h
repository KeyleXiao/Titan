/*******************************************************************
** 文件名:	speed\Social\MailServer\Src\PlayerMail.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	邮件个人数据对象类
**************************************1******************************/

#ifndef __PlayerMail_h__
#define __PlayerMail_h__

#include "IMailService.h"
#include "IGameDatabaseDef.h"
#include "MailService.h"
#include "MailMsgDef.h"

// 对象实现类
class CPlayerMail
{
// 玩家邮件列表
typedef multimap<LONGLONG, SMailBaseData>					TMap_MailList;
// 玩家邮件列表迭代器
typedef multimap<LONGLONG, SMailBaseData>::iterator		TMap_MailListIt;

public:
	/// purpose: 构造函数
	CPlayerMail(DWORD dwPDBID);

	/// purpose: 析构函数
	virtual ~CPlayerMail(void);

public:
	/////////////////////////////CPlayerMail////////////////////////////////////////////
	//
	void release();

	// 添加邮件基础数据
	void addMailBaseData(SMailBaseData *pMailBaseData);

	// 发送邮件基础数据列表toClient
	void sendMailListToClient();

	// 获取邮件数量
	int getPlayerMailCount(int& nPlusNum);

	// 删除邮件
	void delMail(LONGLONG llMailID);

	// 设置邮件已读
	void setMailRead(LONGLONG llMailID);

	// 通知玩家新邮件数量提示
	void notifyPlayerNewMailCount();

private:
	DWORD m_dwPDBID;								// 玩家ID
	TMap_MailList m_MailList;						// 邮件列表
};

#endif // __PlayerMail_h__