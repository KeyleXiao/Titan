#pragma once

class AIManager;

struct IAIFactory
{
    // ���������ļ�
    virtual bool load(const char* szInfoFile, const char* szDecisionFile, const char* szCondFile, const char* szActionFile) = 0;

    // AI�����Ƿ���Ч
    virtual bool isAISchemeValid(int nSchemeID) = 0;

    // ����AI
    virtual AIManager* createAI(int nID) = 0;

    // AI�Ƿ����ĳ���¼�
    virtual bool isAICareEvent(int nSchemeID, int nAIEvent) = 0; 

    /** ִ��Service��on_stop
	@return  
	*/
    virtual void  Close(void) = 0;
};

struct IAIFactoryWrapper
{
    virtual bool load() = 0;

    virtual IAIFactory* get() = 0;

    virtual void close() = 0;

    virtual void release() = 0;
};

struct IPlayerActionRecord
{
	virtual void load() = 0;

    // �ص���ʱ��
    virtual void onStop() = 0;

	virtual void release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(AISERVER))
extern "C" IAIFactoryWrapper * CreateAIFactoryWrapper();
#	define	CreateAIFactoryWrapperProc	CreateAIFactoryWrapper
#else													/// ��̬��汾
typedef IAIFactoryWrapper * (RKT_CDECL * procCreateAIFactoryWrapper)();
#	define	CreateAIFactoryWrapperProc	DllApi<procCreateAIFactoryWrapper>::load(MAKE_DLL_NAME(AISERVER), "CreateAIFactoryWrapper")
#endif

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(AISERVER))
extern "C" IPlayerActionRecord * CreatePlayerActionRecord();
#	define	CreatePlayerActionRecordProc	CreatePlayerActionRecord
#else													/// ��̬��汾
typedef IPlayerActionRecord * (RKT_CDECL * procCreatePlayerActionRecord)();
#	define	CreatePlayerActionRecordProc	DllApi<procCreatePlayerActionRecord>::load(MAKE_DLL_NAME(AISERVER), "CreatePlayerActionRecord")
#endif