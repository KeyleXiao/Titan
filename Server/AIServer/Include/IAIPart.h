#pragma once
#include "IEntity.h"
#include "IAIService.h"

struct __IAIPart : public __IEntityPart
{
    // ����AI����ID
    virtual void setAISchemeID(int nSchemeID) = 0;

    // ����·��ID
    virtual void setPathID(int nPathID) = 0;

    // ��¼���Կ��Ƶ�ʵ��
    virtual void onAddRemoveSummonEntity(UID uid, bool bAdd) = 0;

    virtual bool isAICharacter() = 0;

    virtual void restore(){};
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(AISERVER))
extern "C" __IAIPart * CreateAIPart();
#	define	CreateAIPartProc	CreateAIPart
#else													/// ��̬��汾
typedef __IAIPart * (RKT_CDECL * procCreateAIPart)();
#	define	CreateAIPartProc	DllApi<procCreateAIPart>::load(MAKE_DLL_NAME(AISERVER), "CreateAIPart")
#endif