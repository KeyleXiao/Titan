/*******************************************************************
** 文件名: E:\speed\Server\OrderServer\Src\StaticWarScene.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	所有战场数据服务
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
    // 执行释放Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getStaticWarSceneService();

private:
	/** 取得IMatchSceneService接口指针
	@return  
	*/
	IStaticWarSceneService*		getIStaticWarSceneService();

private:
	SERVICE_PTR m_StaticWarSceneServices;

private:
	bool createStaticWarSceneService();

};