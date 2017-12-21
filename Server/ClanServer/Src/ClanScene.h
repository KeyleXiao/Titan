/*******************************************************************
** �ļ���:	ClanScene.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/6/2016
** ��  ��:	1.0
** ��  ��:	��᳡��
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
    // ִ���ͷ�Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getClanSceneService();

private:
	bool createClanSceneService();

private:
	SERVICE_PTR m_ClanSceneServices;
};
#endif // __CLANSCENE_H__