#include "stdafx.h"
#include "KinScene.h"
#include "IServerGlobal.h"
#include "NetMessageDef.h"
#include "IMessageDispatch.h"
#include "IMiscManager.h"


#if KinScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

bool KinScene::load()
{
	TraceLn(__FUNCTION__);

    // 主动创建Service
	createKinSceneService();

	return true;
}

void KinScene::onStop()
{
    // 主动释放Service
    if(m_KinSceneServices)
    {
        m_KinSceneServices->stop();
    }
    m_KinSceneServices = SERVICE_PTR(0);
}

void KinScene::release()
{

	delete this;
}

SERVICE_PTR	KinScene::getKinSceneService()
{
	return m_KinSceneServices;
}

IKinSceneService* KinScene::getIKinSceneService()
{
	if ( m_KinSceneServices == 0 )
	{
		return nullptr;
	}
	return (IKinSceneService*)m_KinSceneServices->query_interface();
}

bool KinScene::createKinSceneService()
{
	TraceLn("KinScene::createKinSceneService ");

    m_KinSceneServices = gServerGlobal->getMiscManager()->create_service("KinSceneService");

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IKinScene * CreateKinScene()
{
	return new KinScene;
}