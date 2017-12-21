#pragma once

#pragma pack(push, 1)

enum
{
    MSG_TRACE_ROUTE = 255,
};

enum TRACE_ROUTE_STAGE
{
    TR_CLIENT_GO,  
    TR_GATE_GO,
    TR_SERVER_ARRIVED,
    TR_SERVER_DO_OK,
    TR_GATE_BACK,
    TR_CLIENT_BACK,

    TR_STAGE_MAX,
};

struct SMsgTraceRoute
{
    DWORD aTick[TR_STAGE_MAX];

    SMsgTraceRoute()
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack(pop)