/*******************************************************************
** 文件名:	E:\speed\Server\KinServer\Include\IKinScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜系统场景服			
  
********************************************************************/

#pragma once
#include "KinDef.h"
#include "IServiceContainer.h"

// 战队对象接口
struct IKin
{
    /// purpose: 释放
    virtual void release() = 0;

    /// purpose: 取得战队的整型数据
    virtual int getNumProp(DWORD dwPropID) = 0;

    /// purpose: 取得战队名字
    virtual LPCSTR getName() = 0;

};


struct IKinScene
{
	/// purpose:  加载 
	virtual bool			load() = 0;
    // 执行释放Service
    virtual void            onStop() = 0;
	/// purpose:  释放 
	virtual void			release() = 0;

	/// purpose:  获取战队服务
	virtual SERVICE_PTR		getKinSceneService() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(KinSceneServer_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(KinSceneServer))
extern "C" IKinScene * CreateKinScene();
#	define	CreateKinSceneProc	CreateKinScene
#else													/// 动态库版本
typedef IKinScene * (RKT_CDECL * procCreateKinScene)();
#	define	CreateKinSceneProc	DllApi<procCreateKinScene>::load(MAKE_DLL_NAME(KinServer), "CreateKinScene")
#endif


