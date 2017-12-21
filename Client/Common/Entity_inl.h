/*******************************************************************
** 文件名:	Entity_inl.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/12/2014
** 版  本:	1.0
** 描  述:	实体相关公共内置函数

			
********************************************************************/

#ifndef __COMMON_ENTITY_INL_H__
#define __COMMON_ENTITY_INL_H__

#include "TimeSyncClient.h"
#include "IAOIPart.h"
#include "IEntityProperty.h"
#include "IClientEntity.h"

struct SNetMsgHead;
struct SMsgEntityHead;

//////////////////////////////////////////////////////////////////////////
// 客户端发送到场景服消息
#define SendMessageToEntityScene(uid, partid, msgid, lpstrData, nLen)   (send_message_to_entity_scene(uid, partid, msgid, lpstrData, nLen))
// 客户端发送到场景服消息
inline void send_message_to_entity_scene(UID uid, BYTE part_id, BYTE msg_id, void * pData, size_t nlen, unsigned long dwTick=0)
{
    SNetMsgHead head;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.byKeyModule    = MSG_MODULEID_ENTITY;
    head.byKeyAction    = msg_id;

    SMsgEntityHead entityHead;
    entityHead.uidMaster = uid;
    entityHead.byPartID = part_id;

    obuf256 oDataPack;
    oDataPack << head << entityHead;
    oDataPack.push_back(pData,nlen);

    net_senddata(oDataPack.data(), oDataPack.size());
}

struct IAOIPart;
#define IsInMasterEyeFocus(target)     is_in_Master_eye_focus(target)
inline bool is_in_Master_eye_focus(IEntity *pTarget)
{
    if(pTarget->isHero())
        return true;

    IEntity * pHero = CAST_TYPE(IEntity *, gClientGlobal->getHero());
    if(!pHero)
        return false;

    IAOIPart * pHeroAOI = CAST_TYPE(IAOIPart* ,pHero->getEntityPart(PART_AOI));
    if(!pHeroAOI)
        return false;

    Vector3 loc = pTarget->getLocation();

    return pHeroAOI->isInMasterEyeFocus(loc.x,loc.y,loc.z);
}

//////////////////////////////////////////////////////////////////////////
#define isCreature(pTarget)     ( (pTarget->getTypeMask() & MASK_CREATURE)==MASK_CREATURE)
#define isActor(pTarget)        ( (pTarget->getTypeMask() & MASK_ACTOR)==MASK_ACTOR)
#define isMonster(pTarget)      ( (pTarget->getTypeMask() & MASK_MONSTER)==MASK_MONSTER || (pTarget->getTypeMask() & MASK_NPC)==MASK_NPC)
#define isTank(pTarget)         ( (pTarget->getTypeMask() & MASK_TANK)==MASK_TANK)

inline bool IsOBClient()
{
    IClientEntity * pHero = gClientGlobal->getHero();
    if(!pHero)
        return false;

    return pHero->getIntProperty(PROPERTY_OBSERVER);
}
#define isOBClient()            IsOBClient()


//////////////////////////////////////////////////////////////////////////
#endif  // __COMMON_ENTITY_INL_H__