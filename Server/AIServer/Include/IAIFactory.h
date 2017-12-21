#pragma once

class AIManager;

struct IAIFactory
{
    // 加载配置文件
    virtual bool load(const char* szInfoFile, const char* szDecisionFile, const char* szCondFile, const char* szActionFile) = 0;

    // AI配置是否有效
    virtual bool isAISchemeValid(int nSchemeID) = 0;

    // 创建AI
    virtual AIManager* createAI(int nID) = 0;

    // AI是否关心某个事件
    virtual bool isAICareEvent(int nSchemeID, int nAIEvent) = 0; 

    /** 执行Service的on_stop
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

    // 关掉定时器
    virtual void onStop() = 0;

	virtual void release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(AISERVER))
extern "C" IAIFactoryWrapper * CreateAIFactoryWrapper();
#	define	CreateAIFactoryWrapperProc	CreateAIFactoryWrapper
#else													/// 动态库版本
typedef IAIFactoryWrapper * (RKT_CDECL * procCreateAIFactoryWrapper)();
#	define	CreateAIFactoryWrapperProc	DllApi<procCreateAIFactoryWrapper>::load(MAKE_DLL_NAME(AISERVER), "CreateAIFactoryWrapper")
#endif

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(AISERVER))
extern "C" IPlayerActionRecord * CreatePlayerActionRecord();
#	define	CreatePlayerActionRecordProc	CreatePlayerActionRecord
#else													/// 动态库版本
typedef IPlayerActionRecord * (RKT_CDECL * procCreatePlayerActionRecord)();
#	define	CreatePlayerActionRecordProc	DllApi<procCreatePlayerActionRecord>::load(MAKE_DLL_NAME(AISERVER), "CreatePlayerActionRecord")
#endif