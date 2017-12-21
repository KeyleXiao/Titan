/*******************************************************************
** 文件名:	ShareServerHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015/10/10
** 版  本:	1.0
** 描  述:	共享辅助类
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IMessageDispatch.h"

class CShareServerHelper : public ITransmitHandler, public IMessageHandler
{
public:
	////// ITransmitHandler /////////////////////////////////////////////////////////////
	/** 消息处理
	@param server	源服务器ID
	@param uMsgID	消息ID
	@param head		消息头
	@param data		消息
	@param len		消息长度
	*/
	virtual void onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void	onServerListUpdated();

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	////// IMessageHandler /////////////////////////////////////////////////////////////
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void onMessage(ulong endpointId, ulong actionId, SNetMsgHead* head, void* data, size_t len);


	////// CShareServerHelper /////////////////////////////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	bool		Create(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void		Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CShareServerHelper(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CShareServerHelper(void);
};
extern CShareServerHelper g_ShareServerHelper;