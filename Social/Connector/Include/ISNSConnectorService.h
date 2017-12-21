/*******************************************************************
** 文件名:	ISNSConnectorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2016-11-30
** 版  本:	1.0
** 描  述:	SNS服务器连接服务			
********************************************************************/

#pragma once

#include "ISNSConnector.h"
#include "buffer.h"
#include <vector>

/**
	职责: 
	1.SNS服务器连接服务
*/
struct ISNSConnectorService
{
	/** 直接发送数据给SNS服务器
	@param msg: 要发送的数据
	*/
	virtual void  sendMessage(int nMessageID, void * pData, int nDataLen) = 0;
};
