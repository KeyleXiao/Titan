#include "WarManager.h"
#include "IServerGlobal.h"
#include "IFramework.h"
#include "Simple_Atom_Lock.h"
#include "IWarService.h"
#include "WarService_Stub.h"

// 创建战场
int CWarManager::CreateWar(Room_Create_War_Context& context)
{
	if (context.nMode >= EGM_MAX )
	{
		ErrorLn( __FUNCTION__ ": mode is invalid ,RoomID="<< context.nRoomIndex <<", MapID="<< context.nMapID <<", ConfigWarID="<< context.nSchemeConfigWarID <<", Mode="<< context.nMode );
		return INVALID_WAR_ID;
	}

    // 获取战场服务配置
    SERVICE_SCHEME* pServiceScheme = gServerGlobal->getFramework()->get_service_scheme("WarService");
    if(pServiceScheme == 0)
    {
		ErrorLn( __FUNCTION__ ": not get warservice config, RoomID="<< context.nRoomIndex <<", MapID="<< context.nMapID <<", ConfigWarID="<< context.nSchemeConfigWarID <<", Mode="<< context.nMode );
        return INVALID_WAR_ID;
    }

    // 分配战场ID
    context.nWarID = InterlockedIncrement(&m_nAllocID);
    if(context.nWarID >= MAXINT - 1)
    {
		ErrorLn( __FUNCTION__ ": alloc index is invalid, RoomID="<< context.nRoomIndex <<", MapID="<< context.nMapID <<", ConfigWarID="<< context.nSchemeConfigWarID <<", Mode="<< context.nMode );
        return INVALID_WAR_ID;
    }

    obuf256 ob;
    ob.push_back( &context, sizeof(context) );

    // 创建一个战场服务
    SERVICE_PTR pContainer = gServerGlobal->getFramework()->create_service(pServiceScheme, 0, ob.data(), ob.size());
    if(pContainer == 0)
    {
		ErrorLn( __FUNCTION__ ": pContainer == 0, "<<"dwMatchTypeID = "<< context.dwMatchTypeID<<", RoomID="<< context.nRoomIndex
                                <<", MapID="<< context.nMapID <<", ConfigWarID="<< context.nSchemeConfigWarID <<", Mode="<< context.nMode );
        return INVALID_WAR_ID;
    }

    return context.nWarID;
}

// 销毁战场
void CWarManager::DestroyWar(int nWarID)
{
    TraceLn(__FUNCTION__" nWarID ="<< nWarID);
    // 加锁保护
    Simple_Atom_Lock lock(&m_mutex);

    T_WARID_TO_WARSERVICE::iterator iter = m_mapWarID2Service.find(nWarID);
    if(iter->second == 0)
    {
		ErrorLn( __FUNCTION__ ": pContainer == 0 nWarID = "<<nWarID);
        return;
    }

    IWarService* pWarService = (IWarService*)iter->second->query_interface();
    if(pWarService != NULL)
    {
        pWarService->destroy();
    }
}

// 战场service退出回调
void CWarManager::OnWarDestroied(int nWarID)
{
    TraceLn(__FUNCTION__": nWarID ="<<nWarID);
    // 加锁保护
    Simple_Atom_Lock lock(&m_mutex);
    m_mapWarID2Service.erase(nWarID);
}

// 获取战场service
SERVICE_PTR CWarManager::GetWar(int nWarID)
{
    Simple_Atom_Lock lock(&m_mutex);
    T_WARID_TO_WARSERVICE::iterator iter = m_mapWarID2Service.find(nWarID);
    if (iter == m_mapWarID2Service.end())
    {
        return SERVICE_PTR(0);
    }

    if(iter->second == 0)
    {
        return SERVICE_PTR(0);
    }
    return iter->second;
}

void CWarManager::onStop()
{
}

 void CWarManager::release()
{
    delete this;
}

 void CWarManager::onWarCreate(int nWarID, SERVICE_PTR pContainer)
{
    Simple_Atom_Lock lock(&m_mutex);
    m_mapWarID2Service[nWarID] = pContainer;  
 }


//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IWarManager* CreateWarManager()
{
	return new CWarManager;
}