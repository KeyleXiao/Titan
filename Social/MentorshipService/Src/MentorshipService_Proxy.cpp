#include "stdafx.h"
#include "MentorshipService_Proxy.h"
#include "StubMacroDef.h"

 //////////////////////////////////////////////////////////////////////////////////
 void MentorshipService_Proxy::handleServerMsg( DWORD serverID,SNetMsgHead head,PACKAGE_PTR msg )
 {
     PACKAGE_PTR::T_BAG bag(msg);
     BUILD_MSG_CONTEXT_3( IMentorshipService::handleServerMsg,DWORD ,serverID,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

     m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
 };

 //////////////////////////////////////////////////////////////////////////////////
 void MentorshipService_Proxy::handleClientMsg( DWORD client,SNetMsgHead head,PACKAGE_PTR msg )
 {
     PACKAGE_PTR::T_BAG bag(msg);
     BUILD_MSG_CONTEXT_3( IMentorshipService::handleClientMsg,DWORD ,client,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

     m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
 };



 

