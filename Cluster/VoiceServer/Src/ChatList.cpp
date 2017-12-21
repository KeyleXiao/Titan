/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ChatList.cpp
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

#include "stdafx.h"
#include "ChatList.h"
#include "Chat.h"
#include <strstream>
#include "GlobalVoiceServer.h"

// 构造函数
CChatList::CChatList(void)
	: m_dwStartID(0)
{
	
}

// 析构函数
CChatList::~CChatList(void)
{
	
}

// 增加私聊对象
CChat * CChatList::Add(void)
{
	if( m_dwStartID == 0 )	// 0做无效ID处理
	{
		m_dwStartID ++;
	}
	
	CHAT_LIST::iterator it = m_list.find(m_dwStartID);
	if( it != m_list.end() )
	{
		ErrorLn( "CChatList::Add 私聊ID已经用完了？m_dwStartID=" << m_dwStartID << ", 当前私聊个数：" << m_list.size() );
		return NULL;
	}

	CChat * pNewChat = new CChat(m_dwStartID);
	if( pNewChat == NULL )
	{
		return NULL;
	}
	
	// 加入列表中
	m_list.insert( CHAT_LIST::value_type( m_dwStartID, pNewChat) );

	// ID递增，为下次使用
	m_dwStartID ++;

	return pNewChat;
}

// 删除私聊对象
bool CChatList::Del( DWORD dwChatID )
{
	m_list.erase(dwChatID);

	return true;
}

// 查找私聊对象
CChat * CChatList::Get( DWORD dwChatID )
{
	CHAT_LIST::iterator it = m_list.find(dwChatID);
	if( it != m_list.end() )
	{
		return it->second;
	}

	return NULL;
}

// 获取私聊对象数量
DWORD CChatList::Count(void)
{
	return m_list.size();
}

// 清除所有私聊对象
void CChatList::Clear(void)
{
	list<CChat *> deletelist;

	CChat * pCChat = NULL;
	for(CHAT_LIST::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		pCChat = it->second;
		deletelist.push_back(pCChat);
	}

	for (list<CChat *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); ++it0)
	{
		SafeRelease((*it0));
	}

	m_list.clear();
}

// 保存用户信息列表到csv文件
void CChatList::SaveUserListToCSV()
{	
	//char report_path[MAX_PATH];
	//sprintf( report_path,"%s\\Report\\",getWorkDir());
	//createDirectoryRecursive(report_path);
	//char file_name[MAX_PATH];
	//sprintf( file_name,"%s\\Report\\VoiceServer_ChatList.csv",getWorkDir());

	//DWORD dwChatCounts = 0;

	//ostrstream buffer;

	//buffer << "私聊对象UID,类型,世界ID,私聊对象ID值,大厅状态标记,大厅麦模式,大厅人数,大厅观察者数,大厅麦序数,统战状态标记,统战麦模式,统战人数,统战观察者数,统战麦序数,娱乐状态标记,娱乐麦模式,娱乐人数,娱乐观察者数,娱乐麦序数,聊天状态标记,聊天麦模式,聊天人数,聊天观察者数,聊天麦序数,挂机状态标记,挂机麦模式,挂机人数,挂机观察者数,挂机麦序数,会议状态标记,会议麦模式,会议人数,会议观察者数,会议麦序数,办公状态标记,办公麦模式,办公人数,办公观察者数,办公麦序数,密室状态标记,密室麦模式,密室人数,密室观察者数,密室麦序数," << std::endl;

	//for( Chat_LIST::iterator iter = m_list.begin(); iter!= m_list.end(); iter++)
	//{
	//	CChat * pChat = iter->second;
	//	if( NULL == pChat )
	//	{
	//		continue;
	//	}

	//	dwChatCounts ++;

	//	buffer << pChat->GetID() << "," 
	//		<<  pChat->GetType() << "," 
	//		<<  pChat->GetIDInfo().dwWorldID << "," 
	//		<<  pChat->GetIDInfo().dwID << ","; 

	//	// 大厅状态标记,大厅麦模式,大厅人数,大厅观察者数,大厅麦序数,
	//	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	//	{
	//		CRoom & room = pChat->GetRoom(i);
	//		buffer << room.GetFlagValue() << "," 
	//			   << room.GetMicState() << "," 
	//			   << room.Count() << "," 
	//			   << pChat->GetObserversCount(i) << "," 
	//			   << room.GetMicOrderCount() << ","; 
	//	}

	//	buffer << std::endl;
	//		
	//}

	//std::string strSave = buffer.str();

	//WriteData2File((BYTE*)strSave.c_str(),strSave.size(),file_name);

	//ErrorLn("保存私聊对象("<<dwChatCounts<<"个)信息列表到csv文件成功:"<<file_name);

}


/** 写入数据到文件
@param  BYTE* pData:要保存的数据
@param  int nLens: 数据大小
@param  char * szFileName:要保存的文件名
@return  
*/	
void CChatList::WriteData2File(BYTE* pData,int nLens,const _TCHAR * szFileName)
{
	try
	{
		BOOL   bSuccess ;
		DWORD  dwBytesWritten ;
		HANDLE hFile= CreateFile (szFileName, GENERIC_WRITE, 0, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

		if (hFile == INVALID_HANDLE_VALUE)
		{
			return ;
		}

		bSuccess = WriteFile (hFile, pData, nLens, &dwBytesWritten, NULL) ;

		CloseHandle (hFile) ;
	}
	catch(...)
	{

	}
}

// 更新私聊列表到客户端
void CChatList::UpdateChatListToClient( DWORD dwClientID )
{
	DWORD dwSendCount = 0;

	GlobalVoiceServer* pVoiceServer = (GlobalVoiceServer*)gGlobalServer;
	ofixbuf obufData(pVoiceServer->m_szDataBuf, sizeof(pVoiceServer->m_szDataBuf));
	ofixbuf obufName(pVoiceServer->m_szNameBuf, sizeof(pVoiceServer->m_szNameBuf));

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_CHATLIST;
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	SMsgVoiceChatList_VC listData;
	listData.dwCount = 0;
	obufData.push_back( &listData, sizeof(SMsgVoiceChatList_VC) );

	CClientList & clientList = CClientList::getInstance();
	for( CHAT_LIST::iterator it = m_list.begin(); it != m_list.end(); ++it  )
	{
		CChat * pChat = it->second;
		if( NULL != pChat )
		{
			dwSendCount ++;

			SMsgVoiceChatNode_VC nodeData;
			nodeData.dwChatID = pChat->GetID();				// 私聊ID
			nodeData.dwWorldIDInviter = 0;		// 邀请者世界ID
			nodeData.dwWorldIDAcceptor = 0;		// 接受者世界ID
			nodeData.nNameLenInviter = 0;	
			nodeData.nNameLenAcceptor = 0;

			// 邀请者
			CClientUser * pInviter = clientList.GetUser(pChat->GetChatUser(VoiceChatUserType_Inviter));
			if( pInviter != NULL )
			{
				SVoiceUserInfo & userInfo	= pInviter->GetUserInfo();
				nodeData.dwInviterUDBID		= userInfo.dwUDBID;
				nodeData.dwWorldIDInviter	= userInfo.dwFromWorldID;
				nodeData.nNameLenInviter	= strlen(userInfo.szName);
				obufName.push_back( userInfo.szName, nodeData.nNameLenInviter );
			}

			// 被邀请者
			CClientUser * pAcceptor = clientList.GetUser(pChat->GetChatUser(VoiceChatUserType_Acceptor));
			if( pAcceptor != NULL )
			{
				SVoiceUserInfo & userInfo	= pAcceptor->GetUserInfo();
				nodeData.dwAcceptorUDBID	= userInfo.dwUDBID;
				nodeData.dwWorldIDAcceptor	= userInfo.dwFromWorldID;
				nodeData.nNameLenAcceptor	= strlen(userInfo.szName);
				obufName.push_back( userInfo.szName, nodeData.nNameLenAcceptor );
			}

			obufData.push_back( &nodeData, sizeof(SMsgVoiceChatNode_VC) );

			// 判断大小和数量
			if( obufData.size() + obufName.size() + 5*sizeof(SMsgVoiceChatNode_VC) >= MAX_NET_PACKAGE_SIZE )
			{
				break;
			}
			else if( dwSendCount >= 300 )
			{
				break;
			}
		}
	}

	// 改变总数
	if( dwSendCount > 0 )
	{
		SMsgVoiceChatList_VC *pListData  = (SMsgVoiceChatList_VC *)(obufData.data() + sizeof(SGameMsgHead));
		pListData->dwCount = dwSendCount;
		obufData.push_back( obufName.data(), obufName.size() );
	}

	gGlobalServer->SendDataToClient( dwClientID, obufData.data(), obufData.size() );
}
