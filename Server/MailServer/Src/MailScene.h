/*******************************************************************
** 文件名:	MailScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/7/2017
** 版  本:	1.0
** 描  述:	邮件场景
********************************************************************/

#ifndef __MAILSCENE_H__
#define __MAILSCENE_H__

#include "IMessageHandler.h"
#include "IServiceContainer.h"
#include "MailSceneService.h"
#include "IMailScene.h"

class MailScene : public IMailScene
{
public:
	MailScene();
	virtual ~MailScene();

public:
	// IMailScene
	//////////////////////////////////////////////////////////////////////////
	virtual bool			load();
    // 执行释放Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getMailSceneService();

private:
	bool createMailSceneService();

private:
	SERVICE_PTR m_MailSceneServices;
};
#endif // __MAILSCENE_H__