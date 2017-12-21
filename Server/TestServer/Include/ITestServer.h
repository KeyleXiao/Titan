/*******************************************************************
** 文件名:	ITestServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015/11/19
** 版  本:	1.0
** 描  述:	测试系统服务器
** 应  用:

**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once

#include "TestModuleDef.h"

#ifndef __I_TestServer_H__
#define __I_TestServer_H__

//////////////////////////////////////////////////////////////////////////

////////////////////////////人物实体测试部件//////////////////////////////////
#include "IEntity.h"

struct __ITestPart : public __IEntityPart
{
    // 机器人行为
    virtual void                onEventAction(BYTE actionId, void * data, size_t len) = NULL;

    // 是否为压力测试角色
    virtual bool                isTester() = 0;
    //   // 取得数据
       //virtual STestPartData *     GetTestData(void) = NULL;

       //// 设定AI类型标志
       //virtual void				SetAITypeFlag(DWORD dwFlagType,bool bFlag=false) = NULL;
       //// 取得AI类型标志
       //virtual bool				GetAITypeFlag(DWORD dwFlagType) = NULL;
       //// 收到伤害
       //virtual void				OnDamageEvent(SEventCreatureInjured_S *eventData) = NULL;
       //// 死了
       //virtual void				OnDieEvent(void) = NULL;
};


// 测试系统模块
struct ITestServer
{
    // 创建
    virtual bool create(void) = NULL;

	/** 执行Service的on_stop
	@return  
	*/
    virtual void  onStop(void) = NULL;

    // 释放对象
    virtual void release(void) = NULL;

    // 创建一个人物实体测试部件
    virtual __ITestPart* createTestPart(void) = NULL;

    // 运行社会服行为,DWORD dwPlayerID:玩家角色ID DWORD dwActionID 行为代码 EMTestActionID,DWORD dwUserData1 用户数据1...
    virtual void runSocialAction(DWORD dwPlayerID, DWORD dwActionID, DWORD dwUserData1 = 0, DWORD dwUserData2 = 0, DWORD dwUserData3 = 0, DWORD dwUserData4 = 0, DWORD dwUserData5 = 0) = NULL;
};



//////////////////////////////////////////////////////////////////////////


//测试系统
#if defined(_LIB) || defined(TESTSERVER_STATIC_LIB)/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(TestServer))
extern "C" ITestServer* CreateTestServer(void);
#	define	CreateTestServerProc	CreateTestServer
#else /// 动态库版本
typedef ITestServer* (RKT_STDCALL *procCreateTestServer)(void);
#	define	CreateTestServerProc	DllApi<procCreateTestServer>::load(MAKE_DLL_NAME(TestServer), "CreateTestServer")
#endif



#endif // __I_TestServer_H__