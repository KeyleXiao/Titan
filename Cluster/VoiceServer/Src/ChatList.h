/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ChatList.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2012-03-21
** 版  本:	1.0
** 描  述:	私聊对象列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifndef __CHAT_LIST_H__
#define __CHAT_LIST_H__

#include "VoiceDef.h"
#include "Singleton.h"
#include "Chat.h"
#include <list>

// 私聊对象列表类
class CChatList : public Singleton<CChatList>
{
public:
	typedef std::map<DWORD, CChat*> CHAT_LIST;

	// 增加私聊对象，bNew为ture表示新加私聊对象
	CChat * Add(void);

	// 删除私聊对象
	bool Del( DWORD dwChatID );

	// 查找私聊对象
	CChat * Get( DWORD dwChatID );

	// 获取私聊对象数量
	DWORD Count(void);

	// 清除所有私聊对象
	void Clear(void);

	// 保存用户信息列表到csv文件
	void SaveUserListToCSV();

	void WriteData2File(BYTE* pData,int nLens,const _TCHAR * szFileName);

	// 更新私聊列表到客户端
	void UpdateChatListToClient( DWORD dwClientID );

	// 构造函数
	CChatList(void);

	// 析构函数
	virtual ~CChatList(void);

private:
	// 私聊对象列表
	CHAT_LIST m_list;

	// 私聊ID起始值
	DWORD m_dwStartID;
};



#endif