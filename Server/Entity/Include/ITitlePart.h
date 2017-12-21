/*******************************************************************
** 文件名:	ITitlePart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-27
** 版  本:	1.0
** 描  述:	实体称号部件接口

********************************************************************/

#pragma once

#include "IEntity.h"
#include "TitleDef.h"


// 实体称号部件接口
struct __ITitlePart : public __IEntityPart
{
	/** 加入称号
	@param nTitleID: 称号ID
	@param szTitleName: 称号  
	@param bCoverTitle: 是否能覆盖 
	@return  
	*/
	virtual bool				addTitle(int nTitleID, const char * szTitleName, bool bCoverTitle=true) = 0;

	/** 删除系统
	@param   
	@param   
	@return  
	*/
	virtual bool				deleteTitle(int nTitleID) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __ITitlePart * CreateTitlePart();
#	define	CreateTitlePartProc	CreateTitlePart
#else													/// 动态库版本
typedef __ITitlePart * (RKT_CDECL * procCreateTitlePart)();
#	define	CreateTitlePartProc	DllApi<procCreateTitlePart>::load(MAKE_DLL_NAME(ENTITY), "CreateTitlePart")
#endif
