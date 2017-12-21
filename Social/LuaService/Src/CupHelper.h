/*******************************************************************
** 文件名:	CupHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	杯赛帮助函数
********************************************************************/

#include "stdafx.h"
#include "SocialLuaAPI.h"

/**
输出信息
@param 输出字符串
*/
MY_LUA_API void addCupRegister( DWORD dwLengedCupID,const char *pStr);


/**
输出信息
@param 输出字符串
*/
MY_LUA_API void removeCupRegister(DWORD dwLengedCupID, const char *pStr);
