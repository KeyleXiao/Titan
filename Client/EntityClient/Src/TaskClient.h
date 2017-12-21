/*******************************************************************
** 文件名:	TaskClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	任务客户端
********************************************************************/

#pragma once

#include "ITaskClient.h"
#include "IMessageDispatch.h"
#include "IEntityClient.h"

class CTaskClient : public ITaskClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	////////////////////////////////////IGameViewEventHandler///////////////////////////////////////
	/** 
	@name   : 显示层事件管理器
	@param
	@param   
	@return  
	*/
	virtual bool		onViewEvent(int eventID, int nParam, const char *strParam, void *ptrParam);


	///////////////////////////////////////IMessageHandler//////////////////////////////////////////
	/** 
	@name   : 消息处理
	@param
	@param   
	@return  
	*/
	virtual void		onMessage(SNetMsgHead *head, void *pData, size_t nLen);


	//////////////////////////////////////ITaskClient/////////////////////////////////////////
	/** 
	@name   : 创建防沉迷服务
	@param
	@param   
	@return  
	*/
	virtual bool Create();

	/** 
	@name   : 释放防沉迷服务
	@param
	@param   
	@return  
	*/
	virtual bool Release();


	//////////////////////////////////////CTaskClient/////////////////////////////////////////
	/** 
	@name   : 构造
	@param
	@param   
	@return  
	*/
	CTaskClient(void);

	/** 
	@name   : 析构
	@param
	@param   
	@return  
	*/
	virtual ~CTaskClient(void);

private:
	// 分包发送任务回复
	obuf	m_contactReplyInfo;
};
extern CTaskClient *			g_pTaskClient;
