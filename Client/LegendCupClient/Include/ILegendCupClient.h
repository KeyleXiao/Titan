/*******************************************************************
** 文件名:	ILegendCupClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	比赛系统客户端
** 应  用:  		
	
*******************************************************************/

#pragma once


struct ILegendCupClient
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
	virtual void			Release(void) = NULL;

};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(LegendCupClient))
extern "C" ILegendCupClient * CreateLegendCupClient(void);
#	define	CreateLegendCupClientProc	CreateLegendCupClient
#	define	CloseLegendCupClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(LegendCupClient))
API_IMPORT ILegendCupClient * CreateLegendCupClient(void);
#	define	CreateLegendCupClientProc	CreateLegendCupClient
#	define	CloseLegendCupClientProc()
#else													// 动态加载
typedef ILegendCupClient * (RKT_CDECL *procCreateLegendCupClient)(void);
#	define	CreateLegendCupClientProc	DllApi<procCreateLegendCupClient>::load(MAKE_DLL_NAME(LegendCupClient), "CreateLegendCupClient")
#	define	CloseLegendCupClientProc	DllApi<procCreateLegendCupClient>::freelib
#endif