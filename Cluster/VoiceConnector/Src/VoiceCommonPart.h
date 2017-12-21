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
#pragma once

#ifndef __VOICE_COMMON_PART_H__
#define __VOICE_COMMON_PART_H__

#include "IVoiceConnector.h"
using namespace Voice;
using namespace rkt;


/**
@name : 社会服 语音连接器扩展信息类
@brief:
*/
class CVoiceCommonPart : public IVoicePart
{
private:

	DWORD m_dwServerID;			// 服务器ID
	DWORD m_dwWorldID;			// 服务器所在游戏世界ID
	WORD m_wServerType;			// 服务器类型
	WORD m_wSubID;				// 第n号服务器
	bool m_bIsLocal;			// 是本地对像

public:


	CVoiceCommonPart();

	virtual ~CVoiceCommonPart();

	/** 创建扩展部件
	DWORD dwID			: 服务器ID
	DWORD dwWorldID		: 服务器所在游戏世界ID
	WORD wServerType	: 服务器类型
	WORD wSubID			: 第n号服务器
	bool bIsLocal		: 本地对像
	@return  
	*/
	virtual bool Create(DWORD dwID,DWORD dwWorldID,WORD wServerType,WORD wSubID,bool bIsLocal=false);

	/** 释放,会释放内存
	@return  
	*/
	virtual void Release(void);

	// 取服务器ID
	virtual DWORD GetServerID(void);

	// 取服务器类型
	virtual DWORD GetWorldID(void);

	// 取服务器所在游戏世界ID
	virtual WORD GetServerType(void);

	// 取第n号服务器
	virtual WORD GetServerSubID(void);

};

#endif//__VOICE_COMMON_PART_H__