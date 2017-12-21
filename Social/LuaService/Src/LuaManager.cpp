#include "stdafx.h"
#include "LuaManager.h"
#include "ISocialGlobal.h"
#include "IMessageDispatch.h"
#include "IFramework.h"
#include "ILuaService.h"


bool LuaManager::load()
{
	createLuaService();

    return true;
}

void LuaManager::release()
{
	delete this;
}

bool LuaManager::createLuaService()
{
	IFramework* pFramework = gSocialGlobal->getFramework();
	if ( nullptr == pFramework )
	{
		ErrorLn(__FUNCTION__<< " failed! can not found the IFramework!");
		return false;
	}

	SERVICE_SCHEME * pServiceScheme = pFramework->get_service_scheme("LuaService");
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

	m_LuaServices = pContainer;
	TraceLn(__FUNCTION__<< "create LuaServer sucess" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) ILuaManager* CreateLuaManager()
{
	return new LuaManager;
}