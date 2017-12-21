/*******************************************************************
** 文件名:	e:\Rocket\Client\WarClient\Src\WarClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/7/28  21:25
** 版  本:	1.0
** 描  述:	战场客户端
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "IWarClient.h"

class CWarClient : public IWarClient, public IMessageHandler
{
public:
	//////////////////////IWarClient///////////////////////////////
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			release(void);

	/** 给实体创建冷却部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual IWarPart *	CreatePart(void);

	/** 给实体创建战场物品部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual IMobaGoodsPart *	CreateMobaGoodsPart(void);


	//////////////////////CWarClient/////////////////////////////////
	/** 创建
	@param   
	@param   
	@return  
	*/
	bool					Create(void);


	/** 构造
	@param   
	@param   
	@return  
	*/
	CWarClient(void);

	/** 析构
	@param   
	@param   
	@return  
	*/
	virtual ~CWarClient(void);

	////////////////////////////////////////IMessageHandler////////////////////////////////////////////////////////////
	// 模块消息处理
	virtual void			onMessage(SNetMsgHead* head, void* data, size_t len);

	// 战场消息
	bool					onWarMessageID(int msgId, void * data, size_t len);

	/////////////////////具体消息处理///////////////////////////
private:
	void OnMessagePlayerReconnection(void *pData, size_t nLen);

	void onMessageWarSafetyZoneInfo(void *pData, size_t nLen);

	void onMessageWarNoxSpreadCDInfo(void *pData, size_t nLen);

	void onMessageWarToViewMsg(void *pData, size_t nLen);

private:

};
extern CWarClient *		g_pWarClient;
