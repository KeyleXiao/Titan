﻿/*******************************************************************
** 文件名:	E:\speed\Social\MentorshipServer\Src\MentorshipService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/24/2016	17:50
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "IMentorshipService.h"


using namespace rkt;
using namespace std;

class MentorshipService_Proxy:public IMentorshipService
{
public:
	////////////////// MentorshipService_Proxy ////////////////////////////////////////////////////////

	MentorshipService_Proxy() :m_pContainer(0) {};
	~MentorshipService_Proxy() {};
public:
	///////////////////////////////IMentorshipService///////////////////////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len);

public:
	SERVICE_PTR m_pContainer;

private:
	void PostMsg(obuf256& oMsg);
};