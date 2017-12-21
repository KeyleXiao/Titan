#include "stdafx.h"
#include "MentorshipScene.h"
#include "IServerGlobal.h"
#include "NetMessageDef.h"
#include "IMessageDispatch.h"
#include "IFramework.h"
#include "IMiscManager.h"

MentorshipScene::MentorshipScene()
{

}

MentorshipScene::~MentorshipScene()
{

}

bool MentorshipScene::load()
{
    // 主动创建Service
	if (!createMentorshipSceneService())
        return false;

	return true;
}

void MentorshipScene::onStop()
{
    // 主动释放Service
    if(m_MentorshipSceneServices)
    {
        m_MentorshipSceneServices->stop();
    }
    m_MentorshipSceneServices = SERVICE_PTR(0);
}

void MentorshipScene::release()
{
	delete this;
}

SERVICE_PTR	MentorshipScene::getMentorshipSceneService()
{
	return m_MentorshipSceneServices;
}

bool MentorshipScene::createMentorshipSceneService()
{
	MENTORSHIP_TRACELN("MentorshipScene::createMentorshipSceneService ");

	m_MentorshipSceneServices = gServerGlobal->getMiscManager()->create_service("MentorshipSceneService");

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IMentorshipScene * CreateMentorshipScene()
{
	return new MentorshipScene;
}