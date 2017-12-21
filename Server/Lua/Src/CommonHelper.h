/*******************************************************************
** 文件名:	CommonHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/24/2016
** 版  本:	1.0
** 描  述:	公共帮助函数
********************************************************************/

#include "stdafx.h"
#include "SceneServerAPI.h"

/**
读取csv文件
@param 读取是否成功
*/
MY_LUA_API int readCsvFile(lua_State *L);

/**
获取时间服务器时间
*/
MY_LUA_API int getSyncTick();