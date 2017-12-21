/*******************************************************************
** 文件名:	AntiAddictClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	11/15/2016
** 版  本:	1.0
** 描  述:	

			防沉迷客户端模块: 处理防沉迷相关的客户端逻辑
********************************************************************/

#pragma once

#include "IMessageDispatch.h"
#include "IAntiAddictClient.h"
#include "IEntityClient.h"

class CAntiAddictClient : public IAntiAddictClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	////////////////////////////////////IGameViewEventHandler///////////////////////////////////////
	/** 
	@name   : 显示层事件管理器
	@param
	@param   
	@return  
	*/
	virtual bool		onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam );


	///////////////////////////////////////IMessageHandler//////////////////////////////////////////
	/** 
	@name   : 消息处理
	@param
	@param   
	@return  
	*/
	virtual void		onMessage( SNetMsgHead *head, void *data, size_t len );


	//////////////////////////////////////IAntiAddictClient/////////////////////////////////////////
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


	//////////////////////////////////////CAntiAddictClient/////////////////////////////////////////
	/** 
	@name   : 构造
	@param
	@param   
	@return  
	*/
	CAntiAddictClient(void);

	/** 
	@name   : 析构
	@param
	@param   
	@return  
	*/
	virtual ~CAntiAddictClient(void);

private:
	/** 
	@name   : 防沉迷验证
	@param
	@param   
	@return  
	*/
	void			AntiAddictAuth(void * ptrParam);

	/** 
	@name   : 防沉迷验证结果
	@param
	@param   
	@return  
	*/
	void			OnMsgAuthResult(void *data, size_t len );

	/** 
	@name   : 防沉迷信息提醒
	@param
	@param   
	@return  
	*/
	void			OnMsgAddictTip(void *data, size_t len );
};
extern CAntiAddictClient *			g_pAntiAddictClient;
