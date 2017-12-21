#include "stdafx.h"
#include "StaticWarScene.h"
#include "IServerGlobal.h"
#include "NetMessageDef.h"
#include "IMessageDispatch.h"
#include "IMiscManager.h"


#if StaticWarScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

bool StaticWarScene::load()
{
	TraceLn(__FUNCTION__);

    // 主动创建Service
	createStaticWarSceneService();

	return true;
}

void StaticWarScene::onStop()
{
    // 主动释放Service
    if(m_StaticWarSceneServices)
    {
        m_StaticWarSceneServices->stop();
    }
    m_StaticWarSceneServices = SERVICE_PTR(0);
}

void StaticWarScene::release()
{
	delete this;
}

SERVICE_PTR	StaticWarScene::getStaticWarSceneService()
{
	return m_StaticWarSceneServices;
}

IStaticWarSceneService* StaticWarScene::getIStaticWarSceneService()
{
	if ( m_StaticWarSceneServices == 0 )
	{
		return nullptr;
	}
	return (IStaticWarSceneService*)m_StaticWarSceneServices->query_interface();
}

bool StaticWarScene::createStaticWarSceneService()
{
	TraceLn("StaticWarScene::createStaticWarSceneService ");

    m_StaticWarSceneServices = gServerGlobal->getMiscManager()->create_service("StaticWarSceneService");

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IStaticWarScene * CreateStaticWarScene()
{
	return new StaticWarScene;
}