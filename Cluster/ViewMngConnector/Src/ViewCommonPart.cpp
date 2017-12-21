/*******************************************************************
** 文件名:	VoiceCommonPart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-6-28
** 版  本:	1.0
** 描  述:	通用 语音连接器扩展信息类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#include "stdafx.h"
#include "ViewCommonPart.h"

/**
@name : 语音连接器扩展信息类
@brief:
*/


CVoiceCommonPart::CVoiceCommonPart()
{
	m_dwServerID	= 0;			// 服务器ID
	m_dwWorldID		= 0;			// 服务器所在游戏世界ID
	m_wServerType	= 0;			// 服务器类型
	m_wSubID		= 0;			// 第n号服务器
	m_bIsLocal		= false;		// 是本地对像
}

CVoiceCommonPart::~CVoiceCommonPart() 
{
}

/** 创建扩展部件
DWORD dwID			: 服务器ID
DWORD dwWorldID		: 服务器所在游戏世界ID
WORD wServerType	: 服务器类型
WORD wSubID			: 第n号服务器
bool bIsLocal		: 本地对像
@return  
*/
bool CVoiceCommonPart::Create(DWORD dwID,DWORD dwWorldID,WORD wServerType,WORD wSubID,bool bIsLocal)
{
	m_dwServerID	= dwID;					// 服务器ID
	m_wServerType	= wServerType;			// 服务器类型
	m_dwWorldID		= dwWorldID;			// 服务器所在游戏世界ID
	m_wSubID		= wSubID;				// 第n号服务器	
	m_bIsLocal		= bIsLocal;				// 是本地对像

	return true;
}

/** 释放,会释放内存
@return  
*/
void CVoiceCommonPart::Release(void)
{
	delete this;
}

// 取服务器ID
DWORD CVoiceCommonPart::GetServerID(void)
{
	return m_dwServerID;
}
// 取服务器类型
DWORD CVoiceCommonPart::GetWorldID(void)
{
	return m_dwWorldID;
}
// 取服务器所在游戏世界ID
WORD CVoiceCommonPart::GetServerType(void)
{
	return m_wServerType;
}
// 取第n号服务器
WORD CVoiceCommonPart::GetServerSubID(void)
{
	return m_wSubID;
}