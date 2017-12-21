#include "stdafx.h"
#include "ActorMgr.h"
#include "EntityDef.h"
#include "IFramework.h"
#include "IServiceMgr.h"
#include "IActorService.h"
#include "IServerGlobal.h"
#include "Simple_Atom_Lock.h"
#include "EntityHelper.h"

CActorMgr::CActorMgr()
    : m_mutex(0)
{

}

// 创建角色  nReason:ELoginMode
UID CActorMgr::createActor(SActorCreateContext* pContext, int nReason)
{
    if(pContext == NULL)
    {
        return INVALID_UID;
    }

    SERVICE_SCHEME scheme = *(gServerGlobal->getFramework()->get_service_scheme("Actor"));
    // 电脑玩家的话，分配到场景逻辑所在的线程 (因为现在的寻路库非线程安全)
    // 2016-7-14修改：为了扩展，可以支持其他的特殊原因（压力测试登录的账号也要和电脑一样附到场景LOGIC线程）
    if( (pContext->nFlag & CONTEXT_FLAG_ATTACH_THREAD) )
    {
        SceneLogicHelper helper(pContext->nSceneID);
        if(helper.m_pContainer == 0)
        {
            ErrorLn(__FUNCTION__ << __LINE__);
            return INVALID_UID;
        }

        scheme.thread_id = helper.m_pContainer->get_scheme().thread_id;

        TraceLn("computer player, allot to work thread whose id=" << scheme.thread_id);
    }

    // 生成UID
	UID_DATA uidInfo;
	{
		Simple_Atom_Lock lock(&m_mutex);

		uidInfo.type  = TYPE_PLAYER_ROLE;
		uidInfo.scene = pContext->nSceneID;
		uidInfo.serial= m_actorTable[pContext->nSceneID].allotID();    
	}
	UID uid = *((UID*)&uidInfo);

    obuf buf;
    buf << uid << nReason;
    buf.push_back(pContext, sizeof(SActorCreateContext) + pContext->nCreateDataLen + pContext->nUpdateDataLen);

    SERVICE_PTR pContainer = gServerGlobal->getFramework()->create_service(&scheme, 0, buf.data(), buf.size());
    if(pContainer == 0)
    {
        m_actorTable[pContext->nSceneID].revertID(uidInfo.serial);
        return INVALID_UID;
    }
	
	//Simple_Atom_Lock lock(&m_mutex);

    SERVICE_ID serviceID = pContainer->get_id();

    if(pContext->client != INVALID_CLIENT)
    {
        m_mapClient2UID.put(pContext->client, uid);
    }

    return uid;
}

void CActorMgr::onCreateActor(UID uid, UDBID dwUserID, DWORD dwPDBID, int nSceneID, SERVICE_ID serviceID, DWORD dwClientID, bool isComputer)
{
    {
        SActorIDSet* pIDSet = new SActorIDSet();
        pIDSet->client = dwClientID;
        pIDSet->uid = uid;
        pIDSet->udbid = dwUserID;
        pIDSet->pdbid = dwPDBID;
        pIDSet->service_id = serviceID;

        UID_DATA uidInfo = *((UID_DATA*)&uid);
        Simple_Atom_Lock lock(&m_mutex);
        // 保存到actortable
        m_actorTable[nSceneID].setData(uidInfo.serial, pIDSet);
    }
    // uid->service
    m_mapUID2SERVICE.put(uid, serviceID);
    // pdbid-->uid
    m_mapPDBID2UID.put(dwPDBID, uid);
    // udbid-->uid
    if(!isComputer)
    {
        m_mapUDBID2UID.put(dwUserID,uid);
    }
}

// 销毁角色  nReason:ELogoutMode
void CActorMgr::destroyActor(UID uid, int nReason)
{
    PDBID nPDBID = INVALID_PDBID;
    UDBID nDBID = INVALID_UDBID;
    ClientID clientID = INVALID_CLIENT;


    SActorIDSet* pIDSet = 0;
    UID_DATA* pUidInfo = (UID_DATA*)&uid;
    ACTOR_TABLE::iterator it;

    {
        Simple_Atom_Lock lock(&m_mutex);

        it = m_actorTable.find(pUidInfo->scene);
        if (it == m_actorTable.end())
        {
            ErrorLn(__FUNCTION__ << __LINE__ << ", destroy actor failed, uid=" << uid);
            return;
        }

        pIDSet = it->second.getData(pUidInfo->serial);
        if (pIDSet == NULL)
        {
            ErrorLn(__FUNCTION__ << __LINE__ << ", destroy actor failed, uid=" << uid);
            return;
        }

        nPDBID = pIDSet->pdbid;
        nDBID = pIDSet->udbid;
        clientID = pIDSet->client;

        it->second.revertID(pUidInfo->serial);
        delete pIDSet;
    }


    // 获取Service
    SERVICE_PTR pContainer = getActorByUID( uid );

    // pdbid-->uid
    m_mapPDBID2UID.remove(nPDBID);
    // udbid-->uid
    m_mapUDBID2UID.remove(nDBID);
    // uid -> service
    m_mapUID2SERVICE.remove(uid);
    // client -> service
    m_mapClient2UID.remove(clientID);

    if(pContainer == 0)
    {
        ErrorLn(__FUNCTION__ << ":destroy actor(uid=" << uid  << ") failed, service=" << pIDSet->service_id << " not found");
    }
    else
    {
        IActorService* pService = (IActorService*)pContainer->query_interface();
        if(pService != NULL)
        {
            // 通知角色下线存盘
            SNetMsgHead head;
            head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
            head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
            head.byKeyModule   = MSG_MODULEID_ENTITY;	
            head.byKeyAction   = SS_MSG_ACTOR_LOGOUT;

            // 实体类统一用子头来用于转向部件
            SMsgEntityHead entityHead;
            entityHead.uidMaster = uid;
            entityHead.byPartID = PART_LOGOUT;

            obuf256 data;
            data << head << entityHead << nReason;

            pService->handleMessage(data.data(), data.size());
        }
        else
        {
            pContainer->stop();
        }
    }
}

// 通过uid获取角色
SERVICE_PTR CActorMgr::getActorByUID(UID uid)
{
    if(uid != INVALID_UID)
    {
        SERVICE_ID & serviceID = m_mapUID2SERVICE.get(uid);
        if (serviceID != INVALID_SERVICE_ID)
        {
            return gServerGlobal->getFramework()->get_service_manager()->get_service(serviceID);
        }
    }

    return SERVICE_PTR(0);
}

// 通过clientID获取角色
SERVICE_PTR CActorMgr::getActorByClientID(ClientID client)
{
    UID  uid = m_mapClient2UID.get(client);
    return getActorByUID(uid);
}

// 通过pdbid获取角色
SERVICE_PTR CActorMgr::getActorByPDBID(PDBID pdbid)
{
    UID uid = m_mapPDBID2UID.get(pdbid);

    return getActorByUID(uid);
}

// 通过udbid获取角色
SERVICE_PTR CActorMgr::getActorByUDBID(UDBID udbid)
{
    UID uid = m_mapUDBID2UID.get(udbid);
    
    return getActorByUID(uid);
}


// 获取角色总数
size_t CActorMgr::getActorCount()
{
    return m_mapClient2UID.size();
}

void CActorMgr::setClientID(PDBID idActor, ClientID client)
{
    UID uid = PDBID2UID(idActor);
    ClientID _client = UID2ClientID(uid);

    m_mapClient2UID.remove(_client);
    if(client != INVALID_CLIENT)
    {
        m_mapClient2UID.put(client, *((UID_INFO*)&uid));
    }

    Simple_Atom_Lock lock(&m_mutex);
    UID_DATA* pUidInfo = (UID_DATA*)&uid;
    ACTOR_TABLE::iterator it = m_actorTable.find(pUidInfo->scene);
    if(it != m_actorTable.end())
    {
        SActorIDSet* pIDSet = it->second.getData(pUidInfo->serial);
        if(pIDSet != NULL)
        {
            pIDSet->client = client;

            SERVICE_PTR pActor = gServerGlobal->getFramework()->get_service_manager()->get_service(pIDSet->service_id);
            if(pActor != 0)
            {
                IActorService* pActorService = (IActorService*)pActor->query_interface();
                pActorService->setClientID(client);
            }
        }
    }
}

///////////////////////////////id转换///////////////////////

ClientID CActorMgr::UID2ClientID(UID uid)
{
    Simple_Atom_Lock lock(&m_mutex);

    UID_DATA* pUidInfo = (UID_DATA*)&uid;
    ACTOR_TABLE::iterator it = m_actorTable.find(pUidInfo->scene);
    if(it == m_actorTable.end())
    {
        return INVALID_CLIENT;
    }

    SActorIDSet* pIDSet = it->second.getData(pUidInfo->serial);
    if(pIDSet == NULL)
    {
        return INVALID_CLIENT;
    }

    return pIDSet->client;
}

PDBID CActorMgr::UID2PDBID(UID uid)
{
    Simple_Atom_Lock lock(&m_mutex);

    UID_DATA* pUidInfo = (UID_DATA*)&uid;
    ACTOR_TABLE::iterator it = m_actorTable.find(pUidInfo->scene);
    if(it == m_actorTable.end())
    {
        return INVALID_PDBID;
    }

    SActorIDSet* pIDSet = it->second.getData(pUidInfo->serial);
    if(pIDSet == NULL)
    {
        return INVALID_PDBID;
    }

    return pIDSet->pdbid;
}

UDBID CActorMgr::UID2UDBID(UID uid)
{
    Simple_Atom_Lock lock(&m_mutex);

    UID_DATA* pUidInfo = (UID_DATA*)&uid;
    ACTOR_TABLE::iterator it = m_actorTable.find(pUidInfo->scene);
    if (it == m_actorTable.end())
    {
        return INVALID_PDBID;
    }

    SActorIDSet* pIDSet = it->second.getData(pUidInfo->serial);
    if (pIDSet == NULL)
    {
        return INVALID_PDBID;
    }

    return pIDSet->udbid;
}

UID CActorMgr::ClientID2UID(ClientID client)
{
    return m_mapClient2UID.get(client);
}

UDBID CActorMgr::ClientID2UDBID(ClientID client)
{
    SERVICE_PTR pService = getActorByClientID(client);
    if (pService)
    {
        IActorService  * pActorServcice = CAST_TYPE(IActorService  *, pService->query_interface());
        if (pActorServcice)
        {
            return pActorServcice->getUserID();
        }
    }

    return INVALID_UDBID;
}

PDBID CActorMgr::ClientID2PDBID(ClientID client)
{
    SERVICE_PTR pService = getActorByClientID(client);
    if (pService)
    {
        IActorService  * pActorServcice = CAST_TYPE(IActorService  *, pService->query_interface());
        if (pActorServcice)
        {
            return pActorServcice->getProperty_Integer(PROPERTY_ID);
        }
    }

    return INVALID_PDBID;
}

UID CActorMgr::PDBID2UID(PDBID pdbid)
{
    return m_mapPDBID2UID.get(pdbid);
}

extern "C" __declspec(dllexport) IActorMgr * CreateActorManager()
{
    return &CActorMgr::getInstance();
}