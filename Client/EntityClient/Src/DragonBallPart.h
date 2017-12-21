/*******************************************************************
** 文件名:	DragonBallPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	3/10/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

/**
	七龙珠部件部件: 
	1.暂时只加了创建怪物功能
*/

#include "IDragonBallPart.h"
#include "IMessageDispatch.h"
#include "IClientGlobal.h"


// 实体七龙珠部件接口
class DragonBallPart : public IDragonBallPart, public IMessageHandler
{
public:
	////////////////////IEntityPart/////////////////////////////////////////////
	virtual bool		onLoad( IEntity* pEntity);

	virtual void		release();

	virtual short		getPartID();

	virtual IEntity *	getHost();

	// 消息通讯处理
	virtual bool		onMessage(void * pEntityHead, int msgId, void * data, size_t len);
	
	// 显示层的命令处理
	virtual bool 		onCommand( int cmdid,void * data,size_t len );

    virtual void        restore();

	////////////////////ISerializable/////////////////////////////////////////////
	virtual bool		onSerialize( UID uid, rkt::ibuffer & in,int flag );

	virtual bool		deSerialize( rkt::obuf & out,int flag );

	//////////////////////////IMessageHandler

    virtual void			onMessage( SNetMsgHead* head, void* data, size_t len);

	/** 发送命令,命令与事件不同，命令是外界通知实体做某件事情，实体做了，再发出事件
	@param   
	@param   
	@return  
	*/
	virtual bool		onWarMessageID( int msgId,void * data,int len );




	DragonBallPart();
	~DragonBallPart();

protected:
	void OnMessageShowPoistion(void *pData, size_t nLen);


protected:
	IEntity*		m_pMaster;			// 实体对象
};

