#include "stdafx.h"
#include "MailManager.h"
#include "ISocialGlobal.h"
#include "IMessageDispatch.h"
#include "IFramework.h"
#include "IMailService.h"


bool MailManager::Load()
{

	TraceLn(__FUNCTION__);

	CreateMailService();

 
	return true;
}

void MailManager::Release()
{
	TraceLn(__FUNCTION__);


	delete this;
}

bool MailManager::CreateMailService()
{
	TraceLn(__FUNCTION__ << "create MailServer Begin" );
	IFramework* pFramework = gSocialGlobal->getFramework();
	if ( nullptr == pFramework )
	{
		ErrorLn(__FUNCTION__<< " failed! can not found the IFramework!");
		return false;
	}
	SERVICE_SCHEME * pServiceScheme = pFramework->get_service_scheme("MailService");
	if (nullptr == pServiceScheme)
	{
		ErrorLn(__FUNCTION__ << " failed! can not found SERVICE_SCHEME!");
		return false;
	}

	// 在当前线程创建一个服务 will call MailService_Stub::on_start
	SERVICE_PTR pContainer = pFramework->create_service(pServiceScheme, 0);
	if (nullptr == pContainer)
	{
		ErrorLn(__FUNCTION__<< " failed!  can not found SERVICE_PTR! ");
		return false;
	}

	m_MailServices = pContainer;
	TraceLn(__FUNCTION__<< "create MailServer sucess" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IMailManager* CreateMailManager()
{
	return new MailManager;
}