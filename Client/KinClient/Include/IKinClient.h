/*******************************************************************
** 文件名:	IKinClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	战队系统客户端
** 应  用:  		
	
*******************************************************************/

#pragma once
#include "KinMsgDef.h"


struct IKin
{
    /** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			release(void) = NULL;

};

struct IKinClient
{
	/** 释放
	@param   
	@warning 
	@retval buffer 
	*/
	virtual void			release(void) = NULL;

    /** 创建战队
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallCreateKin(const char* szName) = NULL;

    /** 申请加入战队
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallJoinKin(DWORD dwKinID, string strLeaveWord) = NULL;

     /** 退出战队
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallQuitKin() = NULL;

    /** 解散战队
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallDissmissKin() = NULL;

};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(KinClient))
extern "C" IKinClient * CreateKinClient(void);
#	define	CreateKinClientProc	CreateKinClient
#	define	CloseKinClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(KinClient))
API_IMPORT IKinClient * CreateKinClient(void);
#	define	CreateKinClientProc	CreateKinClient
#	define	CloseKinClientProc()
#else													// 动态加载
typedef IKinClient * (RKT_CDECL *procCreateKinClient)(void);
#	define	CreateKinClientProc	DllApi<procCreateKinClient>::load(MAKE_DLL_NAME(KinClient), "CreateKinClient")
#	define	CloseKinClientProc	DllApi<procCreateKinClient>::freelib
#endif