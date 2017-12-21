#include "stdafx.h"
#include "LegendCupScene.h"
#include "IServerGlobal.h"
#include "NetMessageDef.h"
#include "IMessageDispatch.h"
#include "IFramework.h"


#if LegendCupScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

bool LegendCupScene::load()
{
	TraceLn(__FUNCTION__);

    // 主动创建Service
	createLegendCupSceneService();

	return true;
}

void LegendCupScene::onStop()
{
    // 主动释放Service
    if(m_LegendCupSceneServices)
    {
        m_LegendCupSceneServices->stop();
    }
    m_LegendCupSceneServices = SERVICE_PTR(0);
}

void LegendCupScene::release()
{
	delete this;
}

SERVICE_PTR	LegendCupScene::getLegendCupSceneService()
{
	return m_LegendCupSceneServices;
}

ILegendCupSceneService* LegendCupScene::getILegendCupSceneService()
{
	if ( m_LegendCupSceneServices == 0 )
	{
		return nullptr;
	}
	return (ILegendCupSceneService*)m_LegendCupSceneServices->query_interface();
}

bool LegendCupScene::createLegendCupSceneService()
{
	TraceLn("LegendCupScene::createLegendCupSceneService ");

	IFramework* pFramework = gServerGlobal->getFramework();
	if (nullptr == pFramework)
	{
		ErrorLn("LegendCupScene::createLegendCupSceneService failed! can not found the IFramework! ");
		return false;
	}

	SERVICE_SCHEME * pServiceScheme = pFramework->get_service_scheme("LegendCupSceneService");
	if (nullptr == pServiceScheme)
	{
		ErrorLn("LegendCupScene::createLegendCupSceneService failed! can not found SERVICE_SCHEME! ");
		return false;
	}

	// 在当前线程创建一个服务 will call MatchSceneService_Stub::on_start
	SERVICE_PTR pContainer = pFramework->create_service(pServiceScheme, 0, nullptr, 0);
	if (nullptr == pContainer)
	{
		ErrorLn("LegendCupScene::createLegendCupSceneService failed!  can not found SERVICE_PTR! ");
		return false;
	}
	m_LegendCupSceneServices = pContainer;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) ILegendCupScene * CreateLegendCupScene()
{
	return new LegendCupScene;
}