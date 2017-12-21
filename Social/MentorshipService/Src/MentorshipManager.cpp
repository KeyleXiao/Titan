#include "stdafx.h"
#include "MentorshipManager.h"
#include "ISocialGlobal.h"
#include "IMessageDispatch.h"
#include "IFramework.h"
#include "IMentorshipService.h"


bool MentorshipManager::Load()
{

	TraceLn(__FUNCTION__);

	CreateMentorshipService();

 
	return true;
}

void MentorshipManager::Release()
{
	TraceLn(__FUNCTION__);


	delete this;
}

bool MentorshipManager::CreateMentorshipService()
{
	TraceLn(__FUNCTION__ << "create MentorshipServer Begin" );
	IFramework* pFramework = gSocialGlobal->getFramework();
	if ( nullptr == pFramework )
	{
		ErrorLn(__FUNCTION__<< " failed! can not found the IFramework!");
		return false;
	}
	SERVICE_SCHEME * pServiceScheme = pFramework->get_service_scheme("MentorshipService");
	if (nullptr == pServiceScheme)
	{
		ErrorLn(__FUNCTION__ << " failed! can not found SERVICE_SCHEME!");
		return false;
	}

	// 在当前线程创建一个服务 will call MentorshipService_Stub::on_start
	SERVICE_PTR pContainer = pFramework->create_service(pServiceScheme, 0);
	if (nullptr == pContainer)
	{
		ErrorLn(__FUNCTION__<< " failed!  can not found SERVICE_PTR! ");
		return false;
	}

	m_MentorshipServices = pContainer;
	TraceLn(__FUNCTION__<< "create MentorshipServer sucess" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IMentorshipManager* CreateMentorshipManager()
{
	return new MentorshipManager;
}