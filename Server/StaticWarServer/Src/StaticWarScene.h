/*******************************************************************
** �ļ���: E:\speed\Server\OrderServer\Src\StaticWarScene.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	����ս�����ݷ���
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "StaticWarSceneService.h"
#include "IStaticWarScene.h"

class StaticWarScene:public IStaticWarScene
{
public:
	// IStaticWarScene
	//////////////////////////////////////////////////////////////////////////
	virtual bool			load();
    // ִ���ͷ�Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getStaticWarSceneService();

private:
	/** ȡ��IMatchSceneService�ӿ�ָ��
	@return  
	*/
	IStaticWarSceneService*		getIStaticWarSceneService();

private:
	SERVICE_PTR m_StaticWarSceneServices;

private:
	bool createStaticWarSceneService();

};