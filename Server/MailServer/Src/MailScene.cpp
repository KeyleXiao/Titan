#include "stdafx.h"
#include "MailScene.h"
#include "IServerGlobal.h"
#include "NetMessageDef.h"
#include "IMessageDispatch.h"
#include "IFramework.h"
#include "IMiscManager.h"

MailScene::MailScene()
{

}

MailScene::~MailScene()
{

}

bool MailScene::load()
{
    // 主动创建Service
	if (!createMailSceneService())
        return false;

	return true;
}

void MailScene::onStop()
{
    // 主动释放Service
    if(m_MailSceneServices)
    {
        m_MailSceneServices->stop();
    }
    m_MailSceneServices = SERVICE_PTR(0);
}

void MailScene::release()
{
	delete this;
}

SERVICE_PTR	MailScene::getMailSceneService()
{
	return m_MailSceneServices;
}

bool MailScene::createMailSceneService()
{
	MAIL_TRACELN("MailScene::createMailSceneService ");

	m_MailSceneServices = gServerGlobal->getMiscManager()->create_service("MailSceneService");

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IMailScene * CreateMailScene()
{
	return new MailScene;
}