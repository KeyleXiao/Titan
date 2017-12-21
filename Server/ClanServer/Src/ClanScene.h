/*******************************************************************
** 文件名:	ClanScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/6/2016
** 版  本:	1.0
** 描  述:	帮会场景
********************************************************************/

#ifndef __CLANSCENE_H__
#define __CLANSCENE_H__

#include "IMessageHandler.h"
#include "IServiceContainer.h"
#include "ClanSceneService.h"
#include "IClanScene.h"

class ClanScene : public IClanScene
{
public:
	ClanScene();
	virtual ~ClanScene();

public:
	// IClanScene
	//////////////////////////////////////////////////////////////////////////
	virtual bool			load();
    // 执行释放Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getClanSceneService();

private:
	bool createClanSceneService();

private:
	SERVICE_PTR m_ClanSceneServices;
};
#endif // __CLANSCENE_H__