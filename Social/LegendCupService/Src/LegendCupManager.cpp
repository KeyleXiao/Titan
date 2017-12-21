#include "stdafx.h"
#include "LegendCupManager.h"
#include "ISocialGlobal.h"
#include "IMessageDispatch.h"
#include "IFramework.h"
#include "ILegendCupService.h"

bool LegendCupManager::load()
{

	LEDENDCUP_TRACELN(__FUNCTION__);

	CreateLegendCupService();
	return true;
}

void LegendCupManager::release()
{
	LEDENDCUP_TRACELN(__FUNCTION__);
	delete this;
}


bool LegendCupManager::CreateLegendCupService()
{
	LEDENDCUP_TRACELN(__FUNCTION__ << "create LegendCupServer Begin" );
	IFramework* pFramework = gSocialGlobal->getFramework();
	if ( nullptr == pFramework )
	{
		ErrorLn(__FUNCTION__<< " failed! can not found the IFramework!");
		return false;
	}
	SERVICE_SCHEME * pServiceScheme = pFramework->get_service_scheme("LegendCupService");
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

	m_LegendCupServices = pContainer;
	TraceLn(__FUNCTION__<< "create LegendCupServer sucess" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) ILegendCupManager* CreateLegendCupManager()
{
	return new LegendCupManager;
}