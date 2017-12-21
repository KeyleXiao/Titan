#include "stdafx.h"
#include "KinManager.h"
#include "ISocialGlobal.h"
#include "IMessageDispatch.h"
#include "IFramework.h"
#include "IKinService.h"


bool KinManager::Load()
{

	TraceLn(__FUNCTION__);

	CreateKinService();

 
	return true;
}

void KinManager::Release()
{
	TraceLn(__FUNCTION__);


	delete this;
}




bool KinManager::CreateKinService()
{
	TraceLn(__FUNCTION__ << "create KinServer Begin" );
	IFramework* pFramework = gSocialGlobal->getFramework();
	if ( nullptr == pFramework )
	{
		ErrorLn(__FUNCTION__<< " failed! can not found the IFramework!");
		return false;
	}
	SERVICE_SCHEME * pServiceScheme = pFramework->get_service_scheme("KinService");
	if (nullptr == pServiceScheme)
	{
		ErrorLn(__FUNCTION__ << " failed! can not found SERVICE_SCHEME!");
		return false;
	}

	// 在当前线程创建一个服务 will call MatchService_Stub::on_start
	SERVICE_PTR pContainer = pFramework->create_service(pServiceScheme, 0);
	if (nullptr == pContainer)
	{
		ErrorLn(__FUNCTION__<< " failed!  can not found SERVICE_PTR! ");
		return false;
	}

	m_KinServices = pContainer;
	TraceLn(__FUNCTION__<< "create KinServer sucess" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IKinManager* CreateKinManager()
{
	return new KinManager;
}