/*******************************************************************
** 文件名: E:\speed\Server\MatchSceneServer\Src
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 卜涛
** 日  期: 3/19/2015	14:42
** 版  本: 1.0
** 描  述: 比赛大厅场景服
			
 
********************************************************************/

#pragma once


#include "IMatchScene.h"
#include "IMessageDispatch.h"
#include "IEventEngine.h"

struct IMatchSceneService;

class MatchScene : public IMatchScene
{

public:

	//IMatchManager////////////////////////////////////////////////////////////////////////
	virtual bool			load();
    // 执行释放Service
    virtual void            onStop();
	virtual void			release();	
	virtual SERVICE_PTR		getMatchSceneService();

private:
	/** 取得IMatchSceneService接口指针
	@return  
	*/
	IMatchSceneService*		getIMatchSceneService();

private:
	SERVICE_PTR	m_MatchSceneServices;


private:
	bool		createMatchSceneService();
};