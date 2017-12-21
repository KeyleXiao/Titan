
#pragma once

#include "EventRefBase.h"

/***************************************************************/
///////////////////// 房间事件 SOURCE_TYPE_ROOM////////////////
/***************************************************************/
#pragma pack(1)

// =========================================单纯的消息回应============================================


// EVENT_ROOM_ADD_ACTOR 进入房间
struct SEventRoomAddActor : public event_base
{
    int nActorID;
    DWORD dwRoomID;
    int nCamp; 

    SEventRoomAddActor()
    {
        memset(this, 0, sizeof(*this));
    }
};

// EVENT_ROOM_DEL_ACTOR,     // 离开房间
struct SEventRoomDelActor : public event_base
{
    int nActorID;

    SEventRoomDelActor()
    {
        memset(this, 0, sizeof(*this));
    }
};

// EVENT_ROOM_ACTOR_CHANGE_CAMP, // 玩家换阵营
struct SEventRoomActorChangeCamp : public event_base
{
    int nActorID;
    DWORD dwRoomID;
    int nCamp;
    
    SEventRoomActorChangeCamp()
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack()