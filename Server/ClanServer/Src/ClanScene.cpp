#include "stdafx.h"
#include "ClanScene.h"
#include "IServerGlobal.h"
#include "NetMessageDef.h"
#include "IMessageDispatch.h"
#include "IMiscManager.h"

ClanScene::ClanScene()
{

}

ClanScene::~ClanScene()
{

}

bool ClanScene::load()
{
    // 主动创建Service
	if (!createClanSceneService())
        return false;

	return true;
}

void ClanScene::onStop()
{
    // 主动释放Service
    if(m_ClanSceneServices)
    {
        m_ClanSceneServices->stop();
    }
    m_ClanSceneServices = SERVICE_PTR(0);
}

void ClanScene::release()
{


	delete this;
}

SERVICE_PTR	ClanScene::getClanSceneService()
{
	return m_ClanSceneServices;
}

bool ClanScene::createClanSceneService()
{
    m_ClanSceneServices = gServerGlobal->getMiscManager()->create_service("ClanSceneService");

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IClanScene * CreateClanScene()
{
	return new ClanScene;
}