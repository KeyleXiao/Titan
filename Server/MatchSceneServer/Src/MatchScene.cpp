#include "stdafx.h"
#include "MatchScene.h"
#include "MatchSceneService.h"
#include "IServerGlobal.h"
#include "ISchemeCenter.h"
#include "EntityHelper.h"
#include "IMiscManager.h"

#if MatchScene_DisableShowDebug
#undef MATCH_TRACELN
#define MATCH_TRACELN(x)
#endif


bool MatchScene::load()
{
	MATCH_TRACELN("MatchScene::load");

    // 主动创建Service
	createMatchSceneService();

	return true;
}

void MatchScene::onStop()
{
    // 主动释放Service
    if(m_MatchSceneServices)
    {
        m_MatchSceneServices->stop();
    }
    m_MatchSceneServices = SERVICE_PTR(0);
}

void MatchScene::release()
{
	MATCH_TRACELN("MatchScene::release");

	delete this;
}

bool MatchScene::createMatchSceneService()
{
	MATCH_TRACELN("MatchScene::createMatchSceneService ");

    m_MatchSceneServices = gServerGlobal->getMiscManager()->create_service("MatchSceneService");

	return true;
}


SERVICE_PTR MatchScene::getMatchSceneService()
{
	return m_MatchSceneServices;
}

/** 取得IMatchSceneService接口指针
@return  
*/
IMatchSceneService*	MatchScene::getIMatchSceneService()
{
	if (m_MatchSceneServices == 0)
	{
		return NULL;
	}
	return (IMatchSceneService*)m_MatchSceneServices->query_interface();
}




///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IMatchScene * CreateMatchScene()
{
	return new MatchScene;
}