/*******************************************************************
** 文件名:	ActorMgr.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	2/23/2016
** 版  本:	1.0
** 描  述:	角色管理

********************************************************************/

#pragma once
#include "IActorMgr.h"
#include "Singleton.h"
#include "IServiceContainer.h"
#include "fast_id_map.h"
#include "ActorDef.h"
#include <map>
#include <unordered_map>
#include "ReadWriteMap.h"
using namespace std::tr1;

using namespace Gateway;


class CActorMgr : public IActorMgr, public Singleton<CActorMgr>
{
    struct SActorIDSet // 角色ID集合
    {
        ClientID client;
        UID uid;
        UDBID udbid;
        PDBID pdbid;
        SERVICE_ID service_id;

        SActorIDSet()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    typedef std::map<int, fast_id_map<short, SActorIDSet*>> ACTOR_TABLE;

    friend class SceneLogicService;

public:
    CActorMgr();

    // 创建角色  nReason:ELoginMode
    virtual UID createActor(SActorCreateContext* pContext, int nReason);

    // 通过udbid获取角色
    virtual SERVICE_PTR getActorByUDBID(UDBID udbid);

    // 通过pdbid获取角色
    virtual SERVICE_PTR getActorByPDBID(PDBID pdbid);

    // 通过uid获取角色
    virtual SERVICE_PTR getActorByUID(UID uid);

    // 通过clientID获取角色
    virtual SERVICE_PTR getActorByClientID(ClientID client);

    // 获取角色总数
    virtual size_t getActorCount();

    //////////id转换//////////////
    virtual ClientID UID2ClientID(UID uid);
    virtual PDBID UID2PDBID(UID uid);
    virtual UDBID UID2UDBID(UID uid);

    virtual UID ClientID2UID(ClientID client);
    virtual UDBID ClientID2UDBID(ClientID client);
    virtual PDBID ClientID2PDBID(ClientID client);

    virtual UID PDBID2UID(PDBID pdbid);

    virtual void onCreateActor(UID uid, UDBID dwUserID, DWORD dwPDBID, int nSceneID, SERVICE_ID serviceID, DWORD dwClientID, bool isComputer);

    virtual void setClientID(PDBID idActor, ClientID client);

protected:
    // 销毁角色  nReason:ELogoutMode
    virtual void destroyActor(UID uid, int nReason);

private:
    // <场景ID, <uid序号, serviceid>> 角色table
    ACTOR_TABLE                         m_actorTable;

    struct UID_INFO
    {
        UID     m_uid;
        UID_INFO()
            : m_uid(0xFFFFFFFF)
        {
        }
        UID_INFO(UID uid)
            : m_uid(uid)
        {
        }
        operator UID() { return m_uid; }
    };

    // client-->uid
    ReadWriteMap<ClientID, UID_INFO>    m_mapClient2UID;
	
    // pdbid-->uid  反查表
    ReadWriteMap<PDBID, UID_INFO>       m_mapPDBID2UID;
    // udbid-->uid 反查表
    ReadWriteMap<UDBID, UID_INFO>       m_mapUDBID2UID;

    // uid->service
    ReadWriteMap<UID, SERVICE_ID>       m_mapUID2SERVICE;

    volatile	LONG            m_mutex;
};