/*******************************************************************
** 文件名:	IPlayerTaskData.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2016-6-1
** 版  本:	1.0
** 描  述:	个人任务数据
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __SERVER_TASKSERVER_INCLUDE_IACTORTASKDATA_H__
#define __SERVER_TASKSERVER_INCLUDE_IACTORTASKDATA_H__

struct __IEntity;

/// 个人任务数据
struct IPlayerTaskData
{
	// 创建
    virtual bool            create(__IEntity * pMaster) = NULL;

	// 释放
	virtual void			release(void) = NULL;
    
    virtual void            restore() = NULL;

	// 读取任务数据
    virtual void            read() = NULL;
	// 任务数据保存
    virtual void            save() = NULL;

	// 清除拥有者的数据
	virtual void			clear() = NULL;

	// 增加数据点
	virtual bool			add(int nKey, int nData) = NULL;

	// 增加数据点
	virtual bool			add(int nKey, LPCSTR pszData, int nLen) = NULL;

	// 移除数据点
	virtual bool			remove(int nKey) = NULL;

	// 取得值
	// @return  如果取失败了，则返回TASKDATA_DEFAULT_VALUE
	virtual int				get(int nKey) = NULL;

	// 取得值
	virtual LPCSTR			get(int nKey, int &nDataLen) = NULL;

    // key的注册
    virtual bool			registerKey(int nKey, bool bIsInt, bool bSave) = NULL;

    // 导出某个拥有者数据库存储数据
    virtual bool            exportDBContext(DWORD dwHolder, BYTE * pIntData, int &nIntLen, BYTE * pByteData, int &nByteLen) = NULL;

    // 导入某个拥有者存入数据库的数据
    virtual bool            importDBContext(DWORD dwPDBID, LPSTR pszContext, int nLen) = NULL;

    // 导入某个拥有者存入数据库的数据
    virtual bool            importBuffData(DWORD dwHolder, LPSTR pIntDataBuffer, int nIntDataLen, LPSTR pByteDataBuffer, int nByteDataLen) = NULL;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(TASKSERVER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(TASKSERVER))
extern "C" IPlayerTaskData * CreatePlayerTaskData();
#	define	CreatePlayerTaskDataProc	CreatePlayerTaskData
#else													/// 动态库版本
typedef IPlayerTaskData * (RKT_CDECL * procCreatePlayerTaskData)();
#	define	CreatePlayerTaskDataProc	DllApi<procCreatePlayerTaskData>::load(MAKE_DLL_NAME(TASKSERVER), "CreatePlayerTaskData")
#endif


#endif  // __SERVER_TASKSERVER_INCLUDE_IACTORTASKDATA_H__