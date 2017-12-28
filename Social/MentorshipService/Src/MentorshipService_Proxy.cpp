#include "stdafx.h"
#include "MentorshipService_Proxy.h"
#include "StubMacroDef.h"

 //////////////////////////////////////////////////////////////////////////////////
 void MentorshipService_Proxy::handleServerMsg( DWORD serverID,SNetMsgHead head,void * data, size_t len )
 {
	 if (m_pContainer == 0)
	 {
		 return;
	 }
     obuf256 t_data;
     t_data << serverID << head << len;
     t_data.push_back(data, len);

     BUILD_MSG_BUFFER(IMentorshipService::handleServerMsg, t_data);

     m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
 };

 //////////////////////////////////////////////////////////////////////////////////
 void MentorshipService_Proxy::handleClientMsg( DWORD client,SNetMsgHead head,void * data, size_t len )
 {
	 if (m_pContainer == 0)
	 {
		 return;
	 }
     obuf256 t_data;
     t_data << client << head << len;
     t_data.push_back(data, len);

     BUILD_MSG_BUFFER(IMentorshipService::handleClientMsg, t_data);

     m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
 };



 

