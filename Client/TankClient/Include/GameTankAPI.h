/*******************************************************************
** 文件名:	GameTankAPI.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2009/04/22
** 版  本:	1.0
** 描  述:	载具系统API接口
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "DTankModule.h"
#include "ITankClient.h"


#ifndef __I_GameTankAPI_H__
#define __I_GameTankAPI_H__


#ifndef  TANK_API
#	ifdef TANKCLIENT_EXPORTS
#		define  TANK_API  extern "C" __declspec(dllexport)
#	elif defined(_LIB)
#		define  TANK_API  extern "C"
#		pragma comment(lib, MAKE_LIB_NAME(TankClient))
#	else
#		define  TANK_API  extern "C" __declspec(dllimport)
#		pragma comment(lib, MAKE_DLL_LIB_NAME(TankClient))
#	endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

// tolua_begin

TANK_API ITankClient * GetTankClient(void);

/** 用载具ID取载具脚本配置参数信息
@param   DWORD dwTankID：载具ID
@param   
@return  载具脚本配置参数信息
*/
TANK_API const STankSchemeInfo*  GetTankSchemeInfo(DWORD dwTankID);



// tolua_end


#endif // __I_GameTankAPI_H__