/*******************************************************************
** 文件名:	RegisterGameSPCmdID.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2010-7-5
** 版  本:	1.0
** 描  述:	向数据库注册公共区内可运行的存储过程
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <list>

// 加入要注册的可运行的存储过程ID
void gAddGameDbSPCmdID(DWORD dwSpCmdID);
// 加入要注册的可运行的存储过程ID列表
void gGetAddGameDbSPList(DWORD dwServerType);
// 取得要注册的可运行的存储过程ID列表
std::list<DWORD>* gGetRegisterGameDbSPCmdIDList(DWORD dwServerType);