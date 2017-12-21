#include "stdafx.h"
#include "SNSManager.h"
#include "ISocialGlobal.h"
#include "IMessageDispatch.h"
#include "IFramework.h"
#include "ISNSService.h"

bool SNSManager::load()
{
	CreateSNSService();

	return true;
}

void SNSManager::release()
{
	delete this;
}

bool SNSManager::CreateSNSService()
{
	TraceLn(__FUNCTION__ << "create SNSService Begin" );
	IFramework* pFramework = gSocialGlobal->getFramework();
	if ( nullptr == pFramework )
	{
		ErrorLn(__FUNCTION__<< " failed! can not found the IFramework!");
		return false;
	}
	SERVICE_SCHEME * pServiceScheme = pFramework->get_service_scheme("SNSService");
	if (nullptr == pServiceScheme)
	{
		ErrorLn(__FUNCTION__ << " failed! can not found SERVICE_SCHEME!");
		return false;
	}

	// 在当前线程创建一个服务 will call SNSService_Stub::on_start
	SERVICE_PTR pContainer = pFramework->create_service(pServiceScheme, 0);
	if (nullptr == pContainer)
	{
		ErrorLn(__FUNCTION__<< " failed!  can not found SERVICE_PTR! ");
		return false;
	}

	m_SNSServices = pContainer;
	TraceLn(__FUNCTION__<< "create SNSService sucess" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) ISNSManager* CreateSNSManager()
{
	return new SNSManager;
}